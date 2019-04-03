/*
 * Uart.c
 *
 * Created: 27.10.2018 17:26:49
 *  Author: Evgeny
 */ 

#include "Globals.h"
#include "Timers.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h> //С помощью этих макросов рассчитывается константы UBRRH_VALUE и UBRRL_VALUE

#define UART_BUFFER_SIZE    32

uint8_t g_UartBuffer[UART_BUFFER_SIZE];
volatile int  g_nUartBytesCount=0;

void UartInit()
{
    UCSR0A=0x00;
    UCSR0C=((1<<UCSZ00)|(1<<UCSZ01));      //Character Size: 8 bit

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
}

void UartStart()
{
    g_nUartBytesCount=0;
    UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
}

void UartStop()
{
    UCSR0B = 0;
}

void UartWrite(uint8_t* pBuffer, int nSize)
{
    for (int i=0;i<nSize;++i)
    {
        UDR0=pBuffer[i];
        while (!(UCSR0A & (1<<UDRE0)));
    }
}

ISR(USART_RX_vect)
{
    g_UartBuffer[g_nUartBytesCount++]=UDR0;
    ModbusTimerStart();
}
