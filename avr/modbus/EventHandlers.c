/*
 * EventHandlers.c
 *
 * Created: 26.03.2019 20:14:40
 *  Author: Evgeny
 */ 

#include "EventHandlers.h"
#include "Modbus.h"
#include "RfReceiver.h"
#include "Bmp280.h"
#include "HAL/PIO.h"

#define RF_SENSORS_PARAMS_COUNT     2
#define TEMPERATURE_OFFSET          0
#define HUMIDITY_OFFSET             1
#define SENSOR_TIMEOUT              300


void SetSensorValue(int nSensorIndex, uint16_t nTemperature, uint16_t nHumidity)
{
    SetInputRegister(RF_SENSORS_REGISTERS+nSensorIndex*RF_SENSORS_PARAMS_COUNT+TEMPERATURE_OFFSET, nTemperature);
    SetInputRegister(RF_SENSORS_REGISTERS+nSensorIndex*RF_SENSORS_PARAMS_COUNT+HUMIDITY_OFFSET, nHumidity);
}

void EventModbusHandler()
{
    ModbusHandler();
    for(int i=0; i<MODBUS_COILS_COUNT; ++i) {
        if (GetCoilValue(i))
        {
            SetOutput(i);
        }
        else {
            ClearOutput(i);
        }
    }
}

void EventBmpHandler()
{
    SetInputRegister(BMP_TEMPERATURE_REGISTER, BmpGetTemperature());
    SetInputRegister(BMP_PRESSURE_REGISTER, BmpGetPressure());
}

void EventTimerHandler(uint16_t unUpdateTime[], uint16_t unTimeCounter)
{
    for (int i=0;i<SENSOR_COUNT;++i)	//сброс значений внешних датчиков, если данные долго не обновлялись
    {
        if ((unTimeCounter-unUpdateTime[i])>SENSOR_TIMEOUT)
        {
            SetSensorValue(i, INVALID_TEMPERATURE, INVALID_HUMIDITY);
            unUpdateTime[i]=unTimeCounter;
        }
    }
}

void EventRfHandler(uint16_t unUpdateTime[], uint16_t unTimeCounter)
{
    stSensorData_t SensorData;
    if(DecodeSensorData(&SensorData))
    {
        if ((SensorData.nChannel>0) && (SensorData.nChannel<=SENSOR_COUNT))
        {
            int SensorIndex=SensorData.nChannel - 1;
            SetSensorValue(SensorIndex, SensorData.nTemperature, SensorData.nHumidity);
            SetDiscreteInput(SensorIndex, SensorData.bBattery);
            unUpdateTime[SensorIndex] = unTimeCounter;
        }
    }

}