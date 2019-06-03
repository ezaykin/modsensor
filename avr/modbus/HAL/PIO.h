/*
 * PIO.h
 *
 * Created: 09.12.2018 12:22:00
 *  Author: Evgeny
 */ 


#ifndef PIO_H_
#define PIO_H_

void PioInit();
void SetOutput(int a_nOutput);
void ClearOutput(int a_nOutput);

void ExternalInt_Init(void (*IsrHandler)(uint8_t nLevel));
void ExternalInt_Enable();
void ExternalInt_Disable();


#endif /* PIO_H_ */