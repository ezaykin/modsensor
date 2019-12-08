/*
 * modbus.c
 *
 * Created: 27.10.2018 17:03:49
 *  Author: Evgeny
 */ 

#include "Globals.h"
#include "Modbus.h"
#include "Bmp280.h"
#include "RfReceiver.h"
#include "EventHandlers.h"
#include "HAL/Timers.h"
#include "HAL/PIO.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>

static const int BMP_PERIOD = 30;
volatile int g_nStatus = 0;

static void OnPeriodicTimer()
{
    g_nStatus |= EVENT_TIMER;
}


int main(void)
{
    uint16_t unTimeCounter = 0;
    uint16_t unUpdateTime[SENSOR_COUNT];

    wdt_enable(WDTO_4S);
    
    PioInit();
    ModbusInit();
    ReceiverInit();
    Timer1Sec_Init(&OnPeriodicTimer);
    sei();
    int BmpConnected = 0;
    BmpConnected = BmpInit();
    
    //выходные регистры внутреннего датчика
    SetInputRegister(BMP_TEMPERATURE_REGISTER, INVALID_TEMPERATURE);
    SetInputRegister(BMP_PRESSURE_REGISTER, 0);

    //выходные регистры внешних датчиков
    for (int i=0; i<SENSOR_COUNT; ++i)
    {
        SetSensorValue(i, INVALID_TEMPERATURE, INVALID_HUMIDITY);
        unUpdateTime[i] = 0;
    }
    //внешние дискрентные выходы
    for (int i=0; i<MODBUS_COILS_COUNT; ++i)
    {
        ClearOutput(i);
    }
    
    Timer1Sec_Start();
    
    for(;;)
    {
        if (g_nStatus & EVENT_MODBUS)
        {
            g_nStatus &= ~EVENT_MODBUS;
            EventModbusHandler();
        }
        if (g_nStatus & EVENT_TIMER)
        {
            g_nStatus &= ~EVENT_TIMER;
            unTimeCounter++;
            EventTimerHandler(unUpdateTime, unTimeCounter);
            if (BmpConnected && (0 == (unTimeCounter % BMP_PERIOD)))		//периодический запуск получения данных из внутреннего датчика
            {
                BmpStart();
            }
        }
        if (g_nStatus & EVENT_TWI)	//по прерыванию от TWI прочитать данные из BMP280
        {
            g_nStatus &= ~EVENT_TWI;
            EventBmpHandler();
        }
        if (g_nStatus & EVENT_RF)		//по приему данных по радиоканалу копируем в регистры MODBUS
        {
            g_nStatus &= ~EVENT_RF;
            EventRfHandler(unUpdateTime, unTimeCounter);
        }
        wdt_reset();
    }
}