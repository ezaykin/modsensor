/*
 * PIO.h
 *
 * Created: 09.12.2018 12:22:00
 *  Author: Evgeny
 */ 


#ifndef PIO_H_
#define PIO_H_

#include <stdint.h>

void PioInit();
void SetOutput(uint8_t unOutput);
void ClearOutput(uint8_t unOutput);

void ExternalInt_Init(void (*IsrHandler)(uint8_t unLevel));
void ExternalInt_Enable();
void ExternalInt_Disable();


#endif /* PIO_H_ */