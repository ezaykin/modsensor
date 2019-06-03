/*
 * timers.h
 *
 * Created: 27.10.2018 17:29:27
 *  Author: Evgeny
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

// Timer resolution 16 mks: up to 4096 mks
void TimerModbus_Init(void (*IsrHandler)(void));
void TimerModbus_Start(uint16_t nPeriodMks);
void TimerModbus_Stop();

// Timer resolution 8 mks: up to 2048 mks
void TimerRf_Init(void (*IsrHandler)(void));
void TimerRf_Start(uint16_t nPeriodMks);
void TimerRf_Stop();

// Timer 1 second
void Timer1Sec_Init(void (*IsrHandler)(void));
void Timer1Sec_Start();
void Timer1Sec_Stop();


#endif /* TIMERS_H_ */