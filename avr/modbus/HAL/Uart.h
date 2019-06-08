/*
 * Uart.h
 *
 * Created: 28.10.2018 14:48:08
 *  Author: Evgeny
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void UartInit(void (*IsrHandler)(uint8_t));
void UartStart();
void UartStop();
void UartWrite(uint8_t* pBuffer, int nSize);

#endif /* UART_H_ */