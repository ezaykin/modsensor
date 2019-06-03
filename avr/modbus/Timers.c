/*
 * timers.c
 *
 * Created: 27.10.2018 17:28:36
 *  Author: Evgeny
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Modbus.h"
#include "Globals.h"

#define T0_PRESCALER    256         //Timer0 nPeriod up to 4096 Mks
#define T0_PERIOD_MKS   (1000000/F_CPU*T0_PRESCALER)
#define T2_PRESCALER    128         //Timer2 nPeriod up to 2048 Mks
#define T2_PERIOD_MKS   (1000000/F_CPU*T2_PRESCALER)

#define T1_PRESCALER    1024        //Timer1 Period up to 4 sec
#define T1_PERIOD_MKS   (1000000/F_CPU*T1_PRESCALER)
#define T1_TARGET_PERIOD_MKS   (1000000)    //Timer1 1 second interrupt

#define OCR1A_VALUE ((T1_TARGET_PERIOD_MKS/T1_PERIOD_MKS) - 1)

static void (*pTimer0IsrHandler)(void)=0;
static void (*pTimer1IsrHandler)(void)=0;
static void (*pTimer2IsrHandler)(void)=0;

void Timer0_Init(void (*IsrHandler)(void))
{
    TCCR0B = 0;			//stop timer
    pTimer0IsrHandler = IsrHandler;
	// Set the Timer Mode to CTC
	TCCR0A = (1<<WGM01);
	TIMSK0 = (1<<OCIE0A);
}

void Timer0_Start(uint16_t nPeriodMks)
{
    TCCR0B = 0;			//stop timer
    TCNT0 = 0;			//clear timer
    TIFR0 |= (1<<OCF0A);
	// Set the value that you want to count to
	OCR0A = (nPeriodMks/T0_PERIOD_MKS) - 1);
    TCCR0B = (1<<CS02);	//set prescaler to 256 and start the timer
}

void Timer0_Stop()
{
    TCCR0B = 0;
}

void Timer2_Init(void (*IsrHandler)(void))
{
    TCCR2B = 0;			//stop timer
    pTimer2IsrHandler = IsrHandler;
    // Set the Timer Mode to CTC
    TCCR2A = (1<<WGM01);
    TIMSK2 = (1<<OCIE2A);
}

void Timer2_Start(uint16_t nPeriodMks)
{
    TCCR2B = 0;
    TCNT2 = 0;
    TIFR2 |= (1<<OCF2A);
    // Set the value that you want to count to
    OCR2A = nPeriodMks/T2_PERIOD_MKS;
    TCCR2B = (1<<CS22)|(1<<CS20);    //F_CPU/128
}

void Timer2_Stop()
{
    TCCR2B = 0;
}

void Timer1_Init(void (*IsrHandler)(void))
{
    TCCR1B = 0;			//stop timer
    pTimer0IsrHandler = IsrHandler;
	OCR1A = (uint16_t)((T1_TARGET_PERIOD_MS/T1_PERIOD_MS) - 1);
    // Set the Timer Mode to CTC
    TCCR1A = (1<<WGM01);
    TIMSK1 = (1<<OCIE1A);
}

void Timer1_Start()
{
	TCCR1A = 0;
	TCCR1B = (1<<CS10)|(1<<CS12)|(1<<WGM12);
}

void Timer1_Stop()
{
    TCCR1B = 0;
}

ISR(TIMER0_COMPA_vect)
{
	Timer0_Stop();
    if (pTimer0IsrHandler)
    {
        *pTimer0IsrHandler();
    }
}

ISR(TIMER2_COMPA_vect)
{
    Timer2_Stop();
    if (pTimer0IsrHandler)
    {
        *pTimer0IsrHandler();
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (pTimer1IsrHandler)
    {
        *pTimer1IsrHandler();
    }
}