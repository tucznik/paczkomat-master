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
#include <string.h>
#include "machine_type.h"
#include "usart.h"
#include "motor.h"

Machine* m = NULL;

Usart::Usart(uint16_t baud) : rx_head(0), rx_tail(0), tx_head(0), tx_tail(0)
{
	uint8_t ubrr = F_CPU / 16 / baud - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << TXCIE0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	USART_DE_INIT;
	RxEnable();
	//wsk_f[0] = &Uart::ST_Gotowy;
	//wsk_f[1] = &Uart::ST_OdebranyZnak;
	//wsk_f[2] = &Uart::ST_Wysylanie;

	//Zdarzenie(ST_GOTOWY);
	//WskPustyBufor = PustyBufor;
	//wUsartDane = &dane;
}

void Usart::RxEnable()
{
	USART_DE_RECEIVE;
	UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
}

void Usart::RxDisable()
{
	UCSR0B &= ~((1 << RXEN0) | (1 << RXCIE0));
}

void Usart::TxEnable()
{
	USART_DE_SEND;
	UCSR0B |= (1 << UDRIE0);
}

void Usart::TxDisable()
{
	UCSR0B &= ~(1 << UDRIE0);
}
/*
void Uart::ZD_NowyZnak(Uart_Param *Dane)
{
	const uint16_t TAB_PRZEJSC[] =
	{
		ST_ODEBRANY_ZNAK,				// ST_GOTOWY
		ST_ODEBRANY_ZNAK,				// ST_ODEBRANY_ZNAK
		//ST_ODEBRANY_ZNAK,				// ST_ODEBRANY_STRING
		//ST_GOTOWY 						// ST_BLAD
	};
	Zdarzenie(TAB_PRZEJSC[StanBiezacy], Dane);
}

void Uart::ST_OdebranyZnak(Uart_Param *Dane)
{
	//strcpy(Dane->ramka, "Tomeczek");
	Dane->ramka = "Testowa";
	ZD_WyslijRamke(Dane);
	//Zdarzenie(ST_);
}
*/
void Usart::CharReceived(UsartData* pdata)
{
	static uint8_t v = 20;
	switch(pdata->c)
	{
		case 'l':	// to bedzie zapisane w EEPROMIE
			m = GetTypeOfMachine(Lockerbox);
		break;
		case 'd':
			m = GetTypeOfMachine(Dynabox);
		break;
		case 's':
			usart_data.c = m->Who();
			usart.SendInt(&usart_data);
	}
	if(pdata->c == 'a')
	{
		motor.SetSpeed(++v);
	}
	if(pdata->c == 'z')
	{
		motor.SetSpeed(--v);
	}
	//usart_data.c = v;
	//usart.SendInt(&usart_data);
}

void Usart::TXBufferEmpty(UsartData* pdata)
{
	if(tx_head > 0)
	{
		UDR0 = buf_tx[tx_tail++];
		tx_head--;
	}
	else
	{
		TxDisable();
		tx_tail = 0;
	}
}

void Usart::TXComplete(UsartData* pdata)
{
	RxEnable();
}

void Usart::SendFrame(UsartData* pdata)
{
	RxDisable();
	const char *w = pdata->frame;
	while(*w)
	{
		buf_tx[tx_head++] = *w++;
	}
	TxEnable();
}

void Usart::SendInt(UsartData *pdata)
{
	char buf[10];
	itoa(pdata->c, buf, 10);
	pdata->frame = buf;
	SendFrame(pdata);
}
/*
void USART_Debug(const char *txt, uint16_t liczba, uint16_t podstawa)
{
	//USART_WyslijRamke(txt);
	USART_WyslijLiczbe(liczba, podstawa);
}
*/
// --------- Debugowanie
// http://mckmragowo.pl/mck/pliki/programming/clib/?f=va_start


ISR(USART0_RX_vect)
{
	usart_data.c = UDR0;
	usart.CharReceived(&usart_data);
}

ISR(USART0_UDRE_vect)
{
	usart.TXBufferEmpty();
}

ISR(USART0_TX_vect)
{
	usart.TXComplete();
}
