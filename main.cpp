/*
 * main.cpp
 *
 *  Created on: 25 maj 2017
 *      Author: tomek
 * Jak rozwiazac wskazniki do funkcji z timerem ? Zeby sztucznie nie tworzyc nowych poza klasa.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "timer.h"
#include "usart.h"
#include "enc28j60.h"
#include "machine_type.h"
#include "motor.h"
#include "stack.h"
#include "eeprom.h"
#include "modbus_rtu.h"
#include "modbus_tcp.h"
Eeprom eprom;
Timer timer(T0_PS_1024, 17);
Motor motor;
Display display;
Stack stack;
Usart usart;
UsartData usart_data;
ModbusRTU modbus_rtu;
ModbusTcp modbus_tcp;

int main()
{
	_delay_ms(1000);
	timer.Assign(0, 1, DisplayRefresh);
	motor.Enable(Forward, 20);
	sei();
	//Machine *m = GetTypeOfMachine(Lockerbox);
	//uint16_t stan = m->Who();
	//wysw.Wypisz(0);
	while(1)
	{
		stack.StackPoll();
		/*
		if(t_flaga_dluga)
		{
			m = WybierzTypMaszyny(TDynabox);
			m->ZmienStan(2);
			stan = m->PrzedstawSie();
			wysw.Wypisz(stan);
			t_flaga_dluga = 0;
			wysw.Wypisz(licznik++);
			if(licznik == 9999) licznik = 0;
		}
*/


	}

}
