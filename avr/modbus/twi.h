#ifndef TWI_H_
#define TWI_H_

#include <stdint.h>

uint8_t TWI_MasterInit(uint16_t fr);
void TwiStartWrite(void* pData, uint8_t Size);
void TwiStartRead(void* pData, uint8_t Register, uint8_t Size);

#endif /* TWI_H_ */