/*
 * timers.h
 *
 * Created: 27.10.2018 17:29:27
 *  Author: Evgeny
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

void Timer0_Init(void (*IsrHandler)(void));
void Timer0_Start(uint16_t nPeriodMks);
void Timer0_Stop();

void PeriodicTimerStart();
void RfTimerInit();
void RfTimerStart(int16_t a_nDelay);
void RfTimerStop();



#endif /* TIMERS_H_ */