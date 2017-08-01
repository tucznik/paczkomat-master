/*
 * timer.cpp
 *
 *  Created on: 15 cze 2017
 *      Author: tomek
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timer.h"
#include "system.h"
#include "display.h"
#include "usart.h"
#include "modbus_rtu.h"
#include "motor.h"
#include "encoder.h"

Timer::Timer(T2Prescallers prescaller)
{
	for(uint8_t n = 0; n < 8; n++)
	{
		timer_handlers[n].active = false;
		timer_handlers[n].counter = 0;
		timer_handlers[n].interval = 0;
		timer_handlers[n].fp = NULL;
	}
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B |= prescaller;
	// for main system timer
	OCR2B = 255;
	TIMSK2 |= (1 << OCIE2B);
	main_timer_prescaler = 0;
}

void Timer::Irq()
{
	main_timer_prescaler++;
	if(main_timer_prescaler == MAIN_TIMER_PRESCALER)
	{
		main_timer_prescaler = 0;
		for(uint8_t n = 0; n < NUMBER_OF_TIMERS; n++)
		{
			if ((timer_handlers[n].active) && (timer_handlers[n].fp != NULL))
			{
				if ((timer_handlers[n].counter == timer_handlers[n].interval))
				{
					timer_handlers[n].counter = 0;
					timer_handlers[n].fp();
				}
				else
				{
					timer_handlers[n].counter++;
				}
			}
		}
	}
}

void Timer::Assign(uint8_t handler_id, uint64_t interval, void(*fp)())
{
	timer_handlers[handler_id].interval = interval;
	timer_handlers[handler_id].counter = 0;
	timer_handlers[handler_id].active = true;
	timer_handlers[handler_id].fp = fp;
}

void Timer::Enable(uint8_t handler_id)
{
	timer_handlers[handler_id].active = true;
	timer_handlers[handler_id].counter = 0;
}

void Timer::Disable(uint8_t handler_id)
{
	timer_handlers[handler_id].active = false;
}

// TIMER_DISPLAY_REFRESH
void DisplayRefresh()
{
	display.Refresh();
}

// TIMER_LOW_LEVEL_COUNTDOWN
void LowLevelCountDown()
{
	static uint16_t i = 5;
	display.Write(i);
	if(i == 0)
	{
		timer.Disable(TIMER_LOW_LEVEL_COUNTDOWN);
	}
	i--;
}

// TIMER_BUTTON_POLL
void ButtonPoll()
{
	if(!(BUTTON_PIN & (1 << BUTTON_PIN_NUMBER)))
	{
		startup_config.EV_ButtonPress(&startup_config_data);
	}
}

// TIMER_BUTTON_TIME_TO_ENTER
void EnterConfig()
{
	if(!(BUTTON_PIN & (1 << BUTTON_PIN_NUMBER)))
	{
		startup_config.EV_ButtonPress(&startup_config_data);
	}
}

// TIMER_ENCODER_STATUS
void EncoderStatus()
{
	static uint8_t param = 1;
	encoder.CheckStatus();
	if (encoder.GetStatus() == 1)
	{
		param++;
		display.Write(Parameter, param);
		encoder.ResetStatus();
	}
	if (encoder.GetStatus() == 2)
	{
		param--;
		display.Write(Parameter, param);
		encoder.ResetStatus();
	}
}

// TIMER_MODBUS_RTU_35T
void ModbusRTU35T()
{
	usart.RTU35T(&usart_data);
}

// TIMER_MODBUS_RTU_POLL
void ModbusPoll()
{
	modbus_rtu.PollDoors();
}

// TIMER_MOTOR_ACCELERATE
void MotorAccelerate()
{
	motor.Accelerate();
}

ISR(TIMER2_COMPB_vect)
{
	timer.Irq();
}
