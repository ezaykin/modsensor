/*
* modbus.h
*
* Created: 27.10.2018 17:08:14
*  Author: Evgeny
*/


#ifndef MODBUS_H_
#define MODBUS_H_

#include <stdint.h>

#define MODBUS_ADDR             1
#define MODBUS_INPUTREG_COUNT   8
#define MODBUS_COILS_COUNT      4
#define MODBUS_INPUTS_COUNT     3

void ModbusInit(void);
void SetInputRegister(uint8_t nRegister, int16_t nValue);
void SetDiscreteInput(uint8_t nInput, uint8_t nValue);
uint8_t GetCoilValue(uint8_t nCoil);
void ModbusHandler(void);




#endif /* MODBUS_H_ */