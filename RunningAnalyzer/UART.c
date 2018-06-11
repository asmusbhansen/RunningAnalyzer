/*----------------------------------------------------------------------------/
/  UART function implementation				                              /
/-----------------------------------------------------------------------------/
/
/ Minimalistic UART implementation for debug on Atmega328PB
/
/----------------------------------------------------------------------------*/

#include <avr/io.h>
#include "UART.h"

//UART Functions

void USART_Init()
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(UBRR_VAL>>8);
	UBRR0L = (unsigned char)UBRR_VAL;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (3<<UCSZ00);//|(1<<USBS0);
}void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}void USART_Transmit_String(char s[])
{
	int i =0;
	
	while (s[i] != 0x00)
	{
		USART_Transmit(s[i]);
		i++;
	}
}void USART_Transmit_Integer(int num){	char buffer[64];	itoa(num,buffer,10);
	USART_Transmit_String(buffer);	USART_Transmit_String("\n\r");}