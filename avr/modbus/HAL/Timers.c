/*
 * timers.c
 *
 * Created: 27.10.2018 17:28:36
 *  Author: Evgeny
 */ 
#include "../Globals.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define T0_PRESCALER    256ul         //Timer0 nPeriod up to 4096 Mks (for F_CPU=16MHz)
#define T0_PERIOD_MKS   (1000000ul*T0_PRESCALER/F_CPU)
#define T2_PRESCALER    128ul         //Timer2 nPeriod up to 2048 Mks
#define T2_PERIOD_MKS   (1000000ul*T2_PRESCALER/F_CPU)

#define T1_PRESCALER    1024ul        //Timer1 Period up to 4 sec
#define T1_PERIOD_MKS   (1000000ul*T1_PRESCALER/F_CPU)
#define T1_TARGET_PERIOD_MKS   (1000000l)    //Timer1 1 second interrupt

static void (*pTimer0IsrHandler)(void)=0;
static void (*pTimer1IsrHandler)(void)=0;
static void (*pTimer2IsrHandler)(void)=0;

void TimerModbus_Init(void (*IsrHandler)(void))
{
    TCCR0B = 0;			//stop timer
    pTimer0IsrHandler = IsrHandler;
	// Set the Timer Mode to CTC
	TCCR0A = (1<<WGM01);
	TIMSK0 = (1<<OCIE0A);
}

void TimerModbus_Start(uint16_t nPeriodMks)
{
    TCCR0B = 0;			//stop timer
    TCNT0 = 0;			//clear timer
    TIFR0 |= (1<<OCF0A);
	// Set the value that you want to count to
	OCR0A = ((nPeriodMks/T0_PERIOD_MKS) - 1);
    TCCR0B = (1<<CS02);	//set prescaler to 256 and start the timer
}

void TimerModbus_Stop()
{
    TCCR0B = 0;
}

void TimerRf_Init(void (*IsrHandler)(void))
{
    TCCR2B = 0;			//stop timer
    pTimer2IsrHandler = IsrHandler;
    // Set the Timer Mode to CTC
    TCCR2A = (1<<WGM21);
    TIMSK2 = (1<<OCIE2A);
}

void TimerRf_Start(uint16_t nPeriodMks)
{
    TCCR2B = 0;
    TCNT2 = 0;
    TIFR2 |= (1<<OCF2A);
    // Set the value that you want to count to
    OCR2A = nPeriodMks/T2_PERIOD_MKS;
    TCCR2B = (1<<CS20)|(1<<CS22);	//set prescaler to 128 and start the timer
}

void TimerRf_Stop()
{
    TCCR2B = 0;
}

void Timer1Sec_Init(void (*IsrHandler)(void))
{
    TCCR1B = 0;			//stop timer
    pTimer0IsrHandler = IsrHandler;
	OCR1A = (uint16_t)((T1_TARGET_PERIOD_MKS/T1_PERIOD_MKS) - 1);
    // Set the Timer Mode to CTC
    TCCR1A = (1<<WGM12);
    TIMSK1 = (1<<OCIE1A);
}

void Timer1Sec_Start()
{
	TCCR1A = 0;
	TCCR1B = (1<<CS10)|(1<<CS12);    //Prescaler 1024
}

void Timer1Sec_Stop()
{
    TCCR1B = 0;
}

ISR(TIMER0_COMPA_vect)
{
    if (pTimer0IsrHandler)
    {
        (*pTimer0IsrHandler)();
    }
}

ISR(TIMER2_COMPA_vect)
{
    if (pTimer0IsrHandler)
    {
        (*pTimer0IsrHandler)();
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (pTimer1IsrHandler)
    {
        (*pTimer1IsrHandler)();
    }
}