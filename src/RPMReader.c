// RPMReader.c 
// Copyright (C) 2011
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


///
/// \file RPMReader.c
/// \brief
/// enter brief description of RPMReader.c here
/// \date 25.11.2011
/// \author cord
/// \details
/// enter detailed description here

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

#define RPMREADER_C_
#include "RPMReader.h"

uint32_t volatile Timer0Capture0Value;		///< stores Timer1 value at each capture 1 event
uint32_t volatile RPMCycleTime;				///< stores captured cycle time in us for RPM measurement

#define RPM_TIMER LPC_TIM3
///////////////////////////////////////////////////////////////////////////////
/// @brief		TIMER0 interrupt handler sub-routine
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////
void TIMER3_IRQHandler(void)
{
	if(TIM_GetIntStatus(RPM_TIMER, TIM_CR0_INT)) {
		TIM_ClearIntPending(RPM_TIMER, TIM_CR0_INT);
		uint32_t capt = TIM_GetCaptureValue(RPM_TIMER,0);
		RPMCycleTime = capt - Timer0Capture0Value;
		Timer0Capture0Value = capt;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \brief	set up TIMER3 capture register.
///
/// configures P0.23 as CAP3.0 | LPC1769 (LQFP100) Pin 9 | LPCXpresso J6/15 (not linked to base board)<br>
/// \return 	None
///////////////////////////////////////////////////////////////////////////////

void initRPMReader(void)
{
	Timer0Capture0Value = 0;
	RPMCycleTime = 0;

	// Config P0.23 as CAP3.0 | LPC1769 (LQFP100) Pin 9 | LPCXpresso J6/15 (not linked to base board)
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 23;
	PINSEL_ConfigPin(&PinCfg);

	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;
	TIM_Init(RPM_TIMER, TIM_TIMER_MODE, &TIM_ConfigStruct);

    TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
    TIM_CaptureConfigStruct.CaptureChannel = 0; // use channel 0, CAPn.0
    TIM_CaptureConfigStruct.RisingEdge = DISABLE; // Enable capture on CAPn.0 rising edge
    TIM_CaptureConfigStruct.FallingEdge = ENABLE; // Disable capture on CAPn.0 falling edge
    TIM_CaptureConfigStruct.IntOnCaption = ENABLE; // Generate capture interrupt
    TIM_ConfigCapture(RPM_TIMER, &TIM_CaptureConfigStruct);

    TIM_ResetCounter(RPM_TIMER);
//TODO check priority for Timer1
    NVIC_SetPriority(TIMER3_IRQn, ((0x01 << 3) | 0x04)); // preemption = 1, sub-priority = 1
    NVIC_EnableIRQ(TIMER3_IRQn); // Enable interrupt for timer 1
    TIM_Cmd(RPM_TIMER,ENABLE);

}
