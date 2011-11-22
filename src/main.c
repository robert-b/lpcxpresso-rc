// main.c Read Graupner HoTT SUM signal
// Copyright (C) 2011  Cord Johannmeyer
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


/// @mainpage lpcxpresso-rc
///
/// Project description
///
/// Example: Read Graupner HoTT SUM signal and attach LED to channel 1
///
/// <img src="tekway45_6.gif" alt="Screenshot">
///
/// @author Cord Johannmeyer


/// \file main.c
/// Purpose
///
///	- read Graupner HoTT SUM signal, connect SUM signal to P1[18]
///	- generate trigger signal for oscilloscope at P1[20]
///	- generate threshold signal for channel 1 to show some action
///

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

/// Variable to store CRP value in.
/// Will be placed automatically by the linker when "Enable Code Read Protect" selected.<br>
/// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include <stdio.h>
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "SumReader.h"
#include "ServoCtrl.h"

// other definitions and declarations
#define LED (1<<22)


///////////////////////////////////////////////////////////////////////////////
/// @brief		the main program.
///////////////////////////////////////////////////////////////////////////////
int main(void) {
	
	GPIO_SetDir(0, LED, 1);			// LEDs on PORT0 defined as Output
	GPIO_SetDir(TRIG_PORT, TRIG_PIN, 1);	// Trigger signal for oscilloscope
	GPIO_SetValue(TRIG_PORT, TRIG_PIN);

    initSumReader();
    initServoCtrl();

    struct Servo_t servo1 = {1,1,20,1600};
    addServo(1, servo1);

    // Enter an infinite loop
    static volatile int i = 0 ;
	while(1) {

		// attach LED to channel 1
		if(Recv.channel[0] > 1500) GPIO_SetValue(0, LED);
		else GPIO_ClearValue(0, LED);

		ServoArray.channel[0].pulseLength = Recv.channel[1];

//		if(i==5000) {
//			int j;
//			for(j=0; j<MAX_CHANNELS; j++) {
//				printf("ch%d = %d ", j+1, Recv.channel[j]);
//			}
//			printf("\n");
//			i=0;
//		}
		Timer0_Wait(10);
		i++ ;
	}
	return 0 ;
}
