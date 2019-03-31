/*
 * PIO.c
 *
 * Created: 09.12.2018 12:25:55
 *  Author: Evgeny
 */ 
#include <avr/io.h>

void PioInit()
{
	ACSR |= (1<<ACD);	//analog comparator disable
	ADCSRA =0;	//ADC disable
	PORTC = (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC2)|(1<<PORTC3);
	DDRC |= (1<<PORTC0)|(1<<PORTC1)|(1<<PORTC2)|(1<<PORTC3);
}

void ClearOutput(int a_nOutput)
{
	switch(a_nOutput)
	{
		case 0:
			PORTC |= (1<<PORTC0);
			break;
		case 1:
			PORTC |= (1<<PORTC1);
			break;
		case 2:
			PORTC |= (1<<PORTC2);
			break;
		case 3:
			PORTC |= (1<<PORTC3);
		break;
	}
}

void SetOutput(int a_nOutput)
{
	switch(a_nOutput)
	{
		case 0:
			PORTC &= ~(1<<PORTC0);
			break;
		case 1:
			PORTC &= ~(1<<PORTC1);
			break;
		case 2:
			PORTC &= ~(1<<PORTC2);
			break;
		case 3:
			PORTC &= ~(1<<PORTC3);
		break;
	}

}
