/*
 * Globals.h
 *
 * Created: 28.10.2018 14:41:06
 *  Author: Evgeny
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#define F_CPU			16000000UL
#define BAUD			38400UL
#define EVENT_MODBUS	(1<<0)
#define EVENT_TWI		(1<<1)
#define EVENT_TIMER		(1<<2)
#define EVENT_RF		(1<<3)


extern volatile int		g_nStatus;


#endif /* GLOBALS_H_ */