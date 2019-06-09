/*
 * RfReceiver.h
 *
 * Created: 04.12.2018 20:36:35
 *  Author: Evgeny
 */ 


#ifndef RFRECEIVER_H_
#define RFRECEIVER_H_

#include <stdint.h>

typedef struct  _SensorData_
{
    uint8_t nId;
    int16_t nTemperature;
    int8_t nHumidity;
    int8_t bBattery;
    int8_t nChannel;
}stSensorData_t;

int DecodeSensorData(stSensorData_t* a_stSensorData);
void ReceiverInit();




#endif /* RFRECEIVER_H_ */