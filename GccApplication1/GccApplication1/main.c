/*
 * GccApplication1.c
 *
 * Created: 23-07-2019 07:08:29 PM
 * Author : Sachin
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define F_CPU 8000000UL

#define BAUD 9600
#define BAUDRATE 51

void uart_initial(int baud){
	UBRR0H = (unsigned char)(baud >>8); //To store complete value of baud rate
	UBRR0L=(unsigned char)(baud);
	UCSR0B=(1<<TXEN0)|(1<<RXEN0); //Receiver and Transmitter enables
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00); // To set data length and set parity using UPM00 or UPM01
	
}
void uart_transmit(unsigned char data){
	while(!(UCSR0A & (1<<UDRE0)));  // UDRE is a flag, indicate that buffer is ready to receive new data
	UDR0=data;
}
unsigned char uart_receive(void){
	while(!(UCSR0A & (1<<RXC0))); // RXC is a flag set by CPU ,0 for empty
	return UDR0;
}
int main(void)
{  uart_initial(51);
	unsigned char c =uart_receive();
	
    
	while(1){
		uart_transmit(c);
		_delay_ms(1000);
		
		}
	
}

