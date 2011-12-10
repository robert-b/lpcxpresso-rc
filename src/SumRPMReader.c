// SumRPMReader.c 
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
/// \file SumRPMReader.c
/// \brief
/// SUM and RPM signal reader
/// \date 10.12.2011
/// \author cord
/// \details
/// Use timer1 to capture the SUM signal plus the RPM signal

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#define SUMREADER_C_

#include "SumRPMReader.h"

uint32_t volatile Timer1Capture0Value;		///< stores Timer1 value at each capture 0 event
uint32_t volatile Timer1Capture1Value;		///< stores Timer1 value at each capture 1 event
volatile struct RecvSignal_t Recv;
uint32_t volatile tim_cr0_int_error;
uint32_t volatile RPMCycleTime;				///< stores captured cycle time in us for RPM measurement

///////////////////////////////////////////////////////////////////////////////
/// @brief		TIMER1 interrupt handler sub-routine
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////
void TIMER1_IRQHandler(void)
{
	if(TIM_GetIntStatus(LPC_TIM1, TIM_CR0_INT))	{
		TIM_ClearIntPending(LPC_TIM1, TIM_CR0_INT);
		if(Timer1Capture0Value != TIM_GetCaptureValue(LPC_TIM1,0)) {
			uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,0);
			uint32_t diff = capt - Timer1Capture0Value;
			if(diff > 5000) {
				if(Recv.index != 12) {
					tim_cr0_int_error++;
				}
				Recv.index = 0;
			}
			else {
				if(Recv.index < MAX_CHANNELS) {
					Recv.channel[Recv.index] = diff;
				}
				if(Recv.index < 11) {
					Recv.index++;
				}
			}
			Timer1Capture0Value = capt;
		}
	}
	else if(TIM_GetIntStatus(LPC_TIM1, TIM_CR1_INT)) {
		TIM_ClearIntPending(LPC_TIM1, TIM_CR1_INT);
		if(Timer1Capture1Value != TIM_GetCaptureValue(LPC_TIM1,1)) {
			uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,1);
			RPMCycleTime = capt - Timer1Capture1Value;
			Timer1Capture1Value = capt;
		}
	}
	if(Timer1Capture0Value != TIM_GetCaptureValue(LPC_TIM1,0)) {
		uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,0);
		uint32_t diff = capt - Timer1Capture0Value;
		if(diff > 5000) {
			if(Recv.index != 12) {
				tim_cr0_int_error++;
			}
			Recv.index = 0;
		}
		else {
			if(Recv.index < MAX_CHANNELS) {
				Recv.channel[Recv.index] = diff;
			}
			if(Recv.index < 11) {
				Recv.index++;
			}
		}
		Timer1Capture0Value = capt;
	}
	if(Timer1Capture1Value != TIM_GetCaptureValue(LPC_TIM1,1)) {
		uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,1);
		RPMCycleTime = capt - Timer1Capture1Value;
		Timer1Capture1Value = capt;
	}
}

///////////////////////////////////////////////////////////////////////////////
/// @brief	set up TIMER1 capture register.
///
/// configures P1.18 as CAP1.0 | LPC1769 (LQFP100) Pin 32 | LPCXpresso PAD1 (not linked to base board)<br>
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////

void initSumRPMReader(void)
{
	Recv.index = 0;
	tim_cr0_int_error = 0;

	//Config P1.18 as CAP1.0 | LPC1769 (LQFP100) Pin 32 | LPCXpresso PAD1 (not linked to base board)
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 19;
	PINSEL_ConfigPin(&PinCfg);

	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &TIM_ConfigStruct);

    TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
    TIM_CaptureConfigStruct.CaptureChannel = 0; // use channel 0, CAPn.0

    // Achtung, Flanke aendern
    TIM_CaptureConfigStruct.RisingEdge = DISABLE; // Disable capture on CAPn.0 rising edge
    TIM_CaptureConfigStruct.FallingEdge = ENABLE; // Enable capture on CAPn.0 falling edge
    TIM_CaptureConfigStruct.IntOnCaption = ENABLE; // Generate capture interrupt
    TIM_ConfigCapture(LPC_TIM1, &TIM_CaptureConfigStruct);

    TIM_CaptureConfigStruct.CaptureChannel = 1; // use channel 1, CAPn.0
    TIM_CaptureConfigStruct.RisingEdge = DISABLE; // Enable capture on CAPn.0 rising edge
    TIM_CaptureConfigStruct.FallingEdge = ENABLE; // Disable capture on CAPn.0 falling edge
    TIM_CaptureConfigStruct.IntOnCaption = ENABLE; // Generate capture interrupt
    TIM_ConfigCapture(LPC_TIM1, &TIM_CaptureConfigStruct);

    TIM_ResetCounter(LPC_TIM1);
//TODO check priority for Timer1
    NVIC_SetPriority(TIMER1_IRQn, ((0x01 << 3) | 0x01)); // preemption = 1, sub-priority = 1
    NVIC_EnableIRQ(TIMER1_IRQn); // Enable interrupt for timer 1
    TIM_Cmd(LPC_TIM1,ENABLE);
}
