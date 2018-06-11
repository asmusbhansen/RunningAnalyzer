/*----------------------------------------------------------------------------/
/  UART function implementation				                              /
/-----------------------------------------------------------------------------/
/
/ Minimalistic UART implementation for debug on Atmega328PB
/
/----------------------------------------------------------------------------*/


#ifndef UART_H_
#define UART_H_



#define BAUD 19200
//#define UBRR_VAL 0x067
#define UBRR_VAL 0x33

//UART Functions

void USART_Init();void USART_Transmit( unsigned char data );void USART_Transmit_String(char s[]);void USART_Transmit_Integer(int num);

#endif /* UART_H_ */