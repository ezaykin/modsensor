/*
 * EventHandlers.h
 *
 * Created: 26.03.2019 20:16:57
 *  Author: Evgeny
 */ 


#ifndef EVENTHANDLERS_H_
#define EVENTHANDLERS_H_

#include <stdint.h>

#define SENSOR_COUNT                3
#define INVALID_TEMPERATURE         -2730
#define INVALID_HUMIDITY            -1
#define BMP_TEMPERATURE_REGISTER    0
#define BMP_PRESSURE_REGISTER       1
#define RF_SENSORS_REGISTERS        2

void EventModbusHandler();
void EventBmpHandler();
void EventTimerHandler();
void EventRfHandler();

void SetSensorValue(int nSensorIndex, uint16_t nTemperature, uint16_t nHumidity);




#endif /* EVENTHANDLERS_H_ */