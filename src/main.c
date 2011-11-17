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

// other definitions and declarations
#define LED (1<<22)
#define TRIG (1<<20)
#define MAX_CHANNELS 12

uint32_t volatile Timer1CaptureValue = 0;		///< stores Timer1 value at each capture event
uint32_t volatile RecvChannel[MAX_CHANNELS];	///< receiver channel 1 to n
uint32_t volatile chIndex = 0;				///< index for Timer1CaptureHighTime array
uint32_t volatile max = 0;						///< number of captured pulses per cycle

/************************** PRIVATE FUNCTIONS *************************/
void setTimer1Capture(void);

/* Interrupt service routines */
void TIMER1_IRQHandler(void);

///////////////////////////////////////////////////////////////////////////////
/// @brief		TIMER1 interrupt handler sub-routine
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////
void TIMER1_IRQHandler(void)
{
	if (TIM_GetIntCaptureStatus(LPC_TIM1,0))
	{
		TIM_ClearIntCapturePending(LPC_TIM1,0);
		uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,0);
		uint32_t diff = capt - Timer1CaptureValue;
		if(diff > 5000) {
			GPIO_SetValue(1, TRIG);
			chIndex = 0;
		}
		else {
			if(chIndex < MAX_CHANNELS) {
				RecvChannel[chIndex] = diff;
			}
			chIndex++;
			GPIO_ClearValue(1, TRIG);
		}
		Timer1CaptureValue = capt;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @brief	set up TIMER1 capture register.
///
/// configures P1.18 as CAP1.0 | LPC1769 (LQFP100) Pin 32 | LPCXpresso PAD1 (not linked to base board)<br>
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////
void setTimer1Capture(void)
{
    TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;

	//Config P1.18 as CAP1.0 | LPC1769 (LQFP100) Pin 32 | LPCXpresso PAD1 (not linked to base board)
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);

	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &TIM_ConfigStruct);

    TIM_CaptureConfigStruct.CaptureChannel = 0; // use channel 0, CAPn.0
    TIM_CaptureConfigStruct.RisingEdge = DISABLE; // Enable capture on CAPn.0 rising edge
    TIM_CaptureConfigStruct.FallingEdge = ENABLE; // Disable capture on CAPn.0 falling edge
    TIM_CaptureConfigStruct.IntOnCaption = ENABLE; // Generate capture interrupt
    TIM_ConfigCapture(LPC_TIM1, &TIM_CaptureConfigStruct);

    TIM_ResetCounter(LPC_TIM1);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief		the main program.
///////////////////////////////////////////////////////////////////////////////
int main(void) {
	
	GPIO_SetDir(0, LED, 1);			// LEDs on PORT0 defined as Output
	GPIO_SetDir(1, TRIG, 1);		// Trigger signal for oscilloscope

    setTimer1Capture();

    NVIC_SetPriority(TIMER1_IRQn, ((0x01 << 3) | 0x01)); // preemption = 1, sub-priority = 1
    NVIC_EnableIRQ(TIMER1_IRQn); // Enable interrupt for timer 1
    TIM_Cmd(LPC_TIM1,ENABLE);

    // Enter an infinite loop
    static volatile int i = 0 ;
	while(1) {

		// attach LED to channel 1
		if(RecvChannel[0] > 1500) GPIO_SetValue(0, LED);
		else GPIO_ClearValue(0, LED);

		if(i==50) {
			int j;
			for(j=0; j<MAX_CHANNELS; j++) {
				printf("ch%d = %d ", j+1, RecvChannel[j]);
			}
			printf("\n");
			i=0;
		}
//		printf("LowTime = %d  HighTime = %d\n", Timer1CaptureLowTime[0], Timer1CaptureHighTime[0]);
		Timer0_Wait(100);
		i++ ;
	}
	return 0 ;
}
