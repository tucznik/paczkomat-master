/*
 * init.cpp
 *
 *  Created on: 1 sie 2017
 *      Author: tomek
 */

#include <avr/interrupt.h>
#include "timer.h"

void Init()
{
	timer.Assign(TIMER_DISPLAY_REFRESH, 4, DisplayRefresh);
	timer.Assign(TIMER_LOW_LEVEL_COUNTDOWN, 1000, LowLevelCountDown);
	sei();
}


