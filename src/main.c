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
/// Features
///
///  - read receiver sum signal, e.g. for *Graupner HoTT* system
///  - drive multiple (up to 8) servos
///  - read RPM
///  - drive LCD (Electronic Assembly DOGS 102x64)
/// .
/// Planned features
///
///  - drive up to 24 servos
///  - read sensors signals
///    - temperature
///    - others
///  - telemetry
/// .
/// <img src="dogs102x64_small.jpg" alt="LCD"> <img src="rc-ecu.jpg" alt="RC-ECU">
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
//#include "SumReader.h"
#include "ServoCtrl.h"
//#include "RPMReader.h"
#include "SumRPMReader.h"
#include "DOGSdisplay.h"

// other definitions and declarations
#define LED (1<<22)
extern uint32_t volatile RPMCycleTime;				///< stores captured cycle time in us for RPM measurement


void printRecvChannels(int y)
{
	char buf[10];
	int i,j;
	int rows = MAX_CHANNELS/3;
	int columns = MAX_CHANNELS/4;
	for(i=0; i<columns; i++)  {
		for(j=0; j<rows; j++) {
			if(j*columns+i<MAX_CHANNELS) {
				sprintf(buf, "%5d", Recv.channel[j*columns+i]);
				lcd_string(1+i*5*6,y+1+j*9, buf, ptr_font_6x8, DELETE);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @brief		the main program.
///////////////////////////////////////////////////////////////////////////////
int main(void)
{
	GPIO_SetDir(0, LED, 1);			// LEDs on PORT0 defined as Output

    initServoCtrl();
    initSumRPMReader();
    initDogs();

	lcd_string(0,5,"   RC-ECU", ptr_font_8x16, DELETE); //Printing a line in big fonts, inverted
	lcd_string(0,5 + pgm_read_byte(ptr_font_8x16 + 5), "RC-Engine Control", ptr_font_6x8, DELETE); //Printing next line small fonts, normal

	Timer0_Wait(10000);

	lcd_clear_all();

    // define servo outputs
    struct Servo_t servoSpec = {1,20,1500};  	/// Servo channel 1 on P1[20]
    setServo(1, servoSpec);
    servoSpec.bit = 21;  						/// Servo channel 2 on P1[21]
    setServo(2, servoSpec);

	lcd_string(0,5," RX-Channels", ptr_font_8x16, DELETE); //Printing a line in big fonts, inverted

    // Enter an infinite loop
    static volatile int i = 0 ;
	while(1) {

		// attach LED to channel 1
		if(Recv.channel[0] > 1500) GPIO_SetValue(0, LED);
		else GPIO_ClearValue(0, LED);

		// mix servo channels to show some action
		ServoArray.channel[0].pulseLength = (Recv.channel[0] + Recv.channel[1]) / 2;
		ServoArray.channel[1].pulseLength = (Recv.channel[2] + Recv.channel[3]) / 2;

		printRecvChannels(5 + pgm_read_byte(ptr_font_8x16 + 5));

//		if(i==500) {
//			int j;
//			for(j=0; j<MAX_CHANNELS; j++) {
//				printf("ch%d = %d ", j+1, Recv.channel[j]);
//			}
//			printf(" RPM = %d", 60000000/RPMCycleTime);
//			printf("\n");
//			i=0;
//		}
		Timer0_Wait(10);
		i++ ;
	}
	return 0 ;
}
