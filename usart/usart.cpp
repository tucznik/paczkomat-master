/*
 * usart.cpp
 *
 *  Created on: 26 maj 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <util/delay.h>

#define USART_KIERUNEK_DDR 		DDRC
#define USART_KIERUNEK_PORT 	PORTC
#define USART_KIERUNEK	 		2

#define USART_KIERUNEK_NAD 	USART_KIERUNEK_PORT |=  (1 << USART_KIERUNEK)
#define USART_KIERUNEK_ODB 	USART_KIERUNEK_PORT &= ~(1 << USART_KIERUNEK)

#define USART_ROZMIAR_BUFORA 50

char usart_buf_nad[USART_ROZMIAR_BUFORA];
volatile unsigned int poz_buf = 0, index = 0;
volatile unsigned char usart_zajety = 0;
volatile unsigned int usart_flaga = 0;

void USART_Init(unsigned int ubrr)
{
	USART_KIERUNEK_DDR |= (1 << USART_KIERUNEK);
	USART_KIERUNEK_ODB;

	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
/*
void USART_WyslijZnak(uint8_t znak)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = znak;
}
*/
void USART_WyslijRamke(const char *ramka)
{
	//_delay_ms(100);
	while(usart_zajety);
	usart_zajety = 1;
	while(*ramka)
	{
		usart_buf_nad[poz_buf++] = *ramka++;
	}

	USART_KIERUNEK_NAD;
	UCSR0B |= (1 << UDRIE0);
}

void USART_WyslijLiczbe(uint16_t liczba, uint16_t podstawa)
{
	char buf[10];
	itoa(liczba, buf, podstawa);
	USART_WyslijRamke(buf);
}

void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa)
{
	USART_WyslijRamke(txt);
	USART_WyslijLiczbe(liczba, podstawa);
}

/*
void USART_WyslijString(const char *buf)
{
	while(*buf)
	{
		USART_WyslijZnak(*buf++);
	}
}
*/
/*
void USART_DodajDoBufora(const char znak)
{
	usart_buf_nad[poz_buf++] = znak;
	if(poz_buf == 16)
	{
		USART_KIERUNEK_NAD;
		UCSR0B |= (1 << UDRIE0);
	}
}
*/

// --------- Debugowanie
// http://mckmragowo.pl/mck/pliki/programming/clib/?f=va_start


ISR(USART0_RX_vect)
{
	char znak = UDR0;
//	usart_flaga = 1;

}

ISR(USART0_UDRE_vect)
{
	if(poz_buf > 0)
	{
		UDR0 = usart_buf_nad[index++];
		poz_buf--;
	}
	else
	{
		UCSR0B &= ~(1 << UDRIE0);
		index = 0;
	}
}

ISR(USART0_TX_vect)
{
	USART_KIERUNEK_ODB;
	usart_zajety = 0;
}


