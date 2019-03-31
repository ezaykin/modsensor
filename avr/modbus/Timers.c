/*
 * timers.c
 *
 * Created: 27.10.2018 17:28:36
 *  Author: Evgeny
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Modbus.h"
#include "Uart.h"
#include "Globals.h"

#define T0_PERIOD  0.00175
#define T0_PRESCALER 256
#define T1_PERIOD 1
#define T1_PRESCALER 1024
#define T2_PERIOD_MKS 8

#define OCR0A_VALUE (((F_CPU / T0_PRESCALER) * T0_PERIOD) - 1)
#define OCR1A_VALUE (((F_CPU / T1_PRESCALER) * T1_PERIOD) - 1)

void ModbusTimerInit()
{
	// Set the Timer Mode to CTC
	TCCR0A = (1<<WGM01);
	// Set the value that you want to count to
	OCR0A = OCR0A_VALUE;
	TIMSK0 = (1<<OCIE0A);
}

void ModbusTimerStart()
{
	TCCR0B = 0;			//stop timer
	TCNT0 = 0;			//clear timer
    TCCR0B = (1<<CS02);	//set prescaler to 256 and start the timer
}

void PeriodicTimerStart()
{
	OCR1A = (uint16_t)OCR1A_VALUE;
	TIMSK1 = (1<<OCIE1A);
	TCCR1A = 0;
 	TCCR1B = (1<<CS10)|(1<<CS12)|(1<<WGM12);
}

void RfTimerInit()
{
	TIMSK2 &= ~(1<<OCIE2A);
	// Set the Timer Mode to CTC
	TCCR2A = (1<<WGM01);
	TCCR2B = 0;
}


void RfTimerStart(int16_t a_nDelay)
{
	TIMSK2 &= ~(1<<OCIE2A);
	TCCR2B = 0;
	OCR2A = a_nDelay/T2_PERIOD_MKS;
	TCNT2 = 0;
	TIFR2 |= (1<<OCF2A);
	TIMSK2 |= (1<<OCIE2A);
	TCCR2B = (1<<CS22)|(1<<CS20);    //F_CPU/128
}

void RfTimerStop()
{
	TIMSK2 &= ~(1<<OCIE2A);
	TCCR2B = 0;
}

ISR(TIMER0_COMPA_vect)
{
	TCCR0B = 0; //stop timer
	UartStop();
	g_nStatus |= EVENT_MODBUS;
}

ISR(TIMER1_COMPA_vect)
{
	g_nStatus |= EVENT_TIMER;
}