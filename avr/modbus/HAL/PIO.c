/*
 * PIO.c
 *
 * Created: 09.12.2018 12:25:55
 *  Author: Evgeny
 */
#include <avr/io.h>
#include <avr/interrupt.h>

static void (*pExtIntIsrHandler)(uint8_t unLevel) = 0;

void PioInit()
{
    ACSR |= (1 << ACD); //analog comparator disable
    ADCSRA = 0;         //ADC disable
    PORTC = (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
    DDRC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3);
}

void ClearOutput(uint8_t unOutput)
{
    switch (unOutput)
    {
    case 0:
        PORTC |= (1 << PORTC0);
        break;
    case 1:
        PORTC |= (1 << PORTC1);
        break;
    case 2:
        PORTC |= (1 << PORTC2);
        break;
    case 3:
        PORTC |= (1 << PORTC3);
        break;
    }
}

void SetOutput(uint8_t unOutput)
{
    switch (unOutput)
    {
    case 0:
        PORTC &= ~(1 << PORTC0);
        break;
    case 1:
        PORTC &= ~(1 << PORTC1);
        break;
    case 2:
        PORTC &= ~(1 << PORTC2);
        break;
    case 3:
        PORTC &= ~(1 << PORTC3);
        break;
    }
}

void ExternalInt_Init(void (*IsrHandler)(uint8_t unLevel))
{
    pExtIntIsrHandler = IsrHandler;
    EICRA = (1 << ISC00); //Any logical change on INT0 generates an interrupt request.
}

void ExternalInt_Enable()
{
    EIFR |= (1 << INTF0);
    EIMSK |= (1 << INT0);
}

void ExternalInt_Disable()
{
    EIMSK &= ~(1 << INT0);
    EIFR |= (1 << INTF0);
}

ISR(INT0_vect)
{
    if (pExtIntIsrHandler) {
        if (PIND & (1 << PIND2)) {
            (*pExtIntIsrHandler)(1);
        }
        else {
            (*pExtIntIsrHandler)(0);
        }
    }
}
