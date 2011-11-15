// main.c Uses timer1 to capture pulsewidth and print via semihosting
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


#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include <stdio.h>
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"


// TODO: insert other definitions and declarations here
uint32_t volatile Timer1CaptureValue = 0;
uint32_t volatile Timer1CaptureLowTime = 0;
uint32_t volatile Timer1CaptureHighTime = 0;
uint8_t CaptureOnRisingEdge = ENABLE;

/************************** PRIVATE FUNCTIONS *************************/
void setTimer1Capture(void);

/* Interrupt service routines */
void TIMER1_IRQHandler(void);

/*********************************************************************//**
 * @brief		TIMER1 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void TIMER1_IRQHandler(void)
{
	if (TIM_GetIntCaptureStatus(LPC_TIM1,0))
	{
		TIM_ClearIntCapturePending(LPC_TIM1,0);
		uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,0);
		Timer1CaptureLowTime = capt - Timer1CaptureValue;
		Timer1CaptureValue = capt;
	}
	else if (TIM_GetIntCaptureStatus(LPC_TIM1,1)){
		TIM_ClearIntCapturePending(LPC_TIM1,1);
		uint32_t capt = TIM_GetCaptureValue(LPC_TIM1,1);
		Timer1CaptureHighTime = capt - Timer1CaptureValue;
		Timer1CaptureValue = capt;
	}
}

void setTimer1Capture(void)
{
    TIM_CAPTURECFG_Type TIM_CaptureConfigStruct;
//    TIM_CAPTURECFG_Type TIM_CaptureConfigStruct1;

    TIM_CaptureConfigStruct.CaptureChannel = 0; // use channel 0, CAPn.0
    TIM_CaptureConfigStruct.RisingEdge = ENABLE; // Enable capture on CAPn.0 rising edge
    TIM_CaptureConfigStruct.FallingEdge = DISABLE; // Disable capture on CAPn.0 falling edge
    TIM_CaptureConfigStruct.IntOnCaption = ENABLE; // Generate capture interrupt
    TIM_ConfigCapture(LPC_TIM1, &TIM_CaptureConfigStruct);

    TIM_CaptureConfigStruct.CaptureChannel = 1; // use channel 1, CAPn.0
    TIM_CaptureConfigStruct.RisingEdge = DISABLE; // Disable capture on CAPn.0 rising edge
    TIM_CaptureConfigStruct.FallingEdge = ENABLE; // Enable capture on CAPn.0 falling edge
    TIM_CaptureConfigStruct.IntOnCaption = ENABLE; // Generate capture interrupt
    TIM_ConfigCapture(LPC_TIM1, &TIM_CaptureConfigStruct);

    TIM_ResetCounter(LPC_TIM1);
}

int main(void) {
	
	//Config P1.18 as CAP1.0 | LPC1769 (LQFP100) Pin 32 | LPCXpresso PAD1 (not linked to base board)
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);

	//Config P1.19 as CAP1.1 | LPC1769 (LQFP100) Pin 33 | LPCXpresso PAD2 (not linked to base board)
	PinCfg.Pinnum = 19;
	PINSEL_ConfigPin(&PinCfg);

	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &TIM_ConfigStruct);

    setTimer1Capture();

    NVIC_SetPriority(TIMER1_IRQn, ((0x01 << 3) | 0x01)); // preemption = 1, sub-priority = 1
    NVIC_EnableIRQ(TIMER1_IRQn); // Enable interrupt for timer 1
    TIM_Cmd(LPC_TIM1,ENABLE);

    uint32_t T1CntVal = LPC_TIM1->TC;
    Timer0_us_Wait(10000);
    uint32_t T1CntVal1 = LPC_TIM1->TC;
    printf("T1CntVal = %d T1CntVal = %d diff = %d \n", T1CntVal, T1CntVal1, T1CntVal1 - T1CntVal);
    // Enter an infinite loop, just incrementing a counter
    static volatile int i = 0 ;
	while(1) {
		printf("LowTime = %d  HighTime = %d\n", Timer1CaptureLowTime, Timer1CaptureHighTime);
		Timer0_Wait(1000);
		i++ ;
	}
	return 0 ;
}
