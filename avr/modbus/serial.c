#include "serial.h"

#include <util/setbaud.h> //С помощью этих макросов рассчитывается константы UBRRH_VALUE и UBRRL_VALUE

//Буфер для вывода
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
_FDEV_SETUP_WRITE);

//Функция выталкивающая буфер в порт USART
int uart_putchar(char c, FILE *stream){
	if (c == '\n')
	uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}
//Инициализация USART
void SerilalIni(){

	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	// USART Baud rate: 9600

	UCSR0A=0x00;
	UCSR0B=(1<<RXEN0) | (1<<TXEN0);
	UCSR0C=0x06;

	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	stdout = &mystdout;//Направляем поток вывода на наш буфер
}
