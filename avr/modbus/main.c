/*
 * modbus.c
 *
 * Created: 27.10.2018 17:03:49
 *  Author: Evgeny
 */ 

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Globals.h"
#include "Modbus.h"
#include "Bmp280.h"
#include "Timers.h"
#include "PIO.h"
#include "RfReceiver.h"
#include "EventHandlers.h"

#define BMP_PERIOD	30


volatile int g_nStatus=0;


int main(void)
{
    uint16_t unTimeCounter=0;
    uint16_t unUpdateTime[SENSOR_COUNT];

    wdt_enable(WDTO_4S);
    
    PioInit();
    ModbusInit();
    ReceiverInit();
    sei();
    int BmpConnected=0;
    BmpConnected=BmpInit();
    
    SetInputRegister(BMP_TEMPERATURE_REGISTER,INVALID_TEMPERATURE);
    SetInputRegister(BMP_PRESSURE_REGISTER,0);

    for (int i=0;i<SENSOR_COUNT;++i)
    {
        SetSensorValue(i,INVALID_TEMPERATURE,INVALID_HUMIDITY);
        unUpdateTime[i]=0;
    }
    for (int i=0;i<MODBUS_COILS_COUNT;++i)
    {
        ClearOutput(i);
    }
    
    PeriodicTimerStart();
    
    while(1)
    {
        if (g_nStatus&EVENT_MODBUS)
        {
            g_nStatus&=~EVENT_MODBUS;
            EventModbusHandler();
        }
        if (g_nStatus&EVENT_TIMER)
        {
            g_nStatus&=~EVENT_TIMER;
            unTimeCounter++;
            EventTimerHandler(unUpdateTime, unTimeCounter);
            if (BmpConnected && (0==(unTimeCounter%BMP_PERIOD)))		//периодический запуск получения данных из внутреннего датчика
            {
                BmpStart();
            }
        }
        if (g_nStatus&EVENT_TWI)	//по прерыванию от TWI прочитать данные из BMP280
        {
            g_nStatus&=~EVENT_TWI;
            EventBmpHandler();
        }
        if (g_nStatus&EVENT_RF)		//по приему данных по радиоканалу копируем в регистры MODBUS
        {
            g_nStatus &= ~(EVENT_RF);
            EventRfHandler(unUpdateTime, unTimeCounter);
        }
        wdt_reset();
    }
}