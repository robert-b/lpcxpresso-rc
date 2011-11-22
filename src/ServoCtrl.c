// ServoCtrl.c 
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
/// \file ServoCtrl.c
/// \brief
/// enter brief description of ServoCtrl.c here
/// \date 22.11.2011
/// \author cord
/// \details
/// enter detailed description here

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#define SERVOCTRL_C_

#include "ServoCtrl.h"

volatile struct ServoCtrl_t ServoArray;


///////////////////////////////////////////////////////////////////////////////
/// @brief		TIMER2 interrupt handler sub-routine
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////
void TIMER2_IRQHandler(void)
{
	if(TIM_GetIntStatus(LPC_TIM2, TIM_MR0_INT)) {	// MR0 Interrupt
		TIM_ClearIntPending(LPC_TIM2, TIM_MR0_INT);
		GPIO_ClearValue(ServoArray.channel[0].port, (1<<ServoArray.channel[0].bit));
//		GPIO_SetValue(ServoArray.channel[0].port, (1<<ServoArray.channel[0].bit));
//		GPIO_ClearValue(ServoArray.channel[0].port, (1<<ServoArray.channel[0].bit));
	}
	else if(TIM_GetIntStatus(LPC_TIM2, TIM_MR1_INT)) {	// MR1 Interrupt
		TIM_ClearIntPending(LPC_TIM2, TIM_MR1_INT);
		LPC_TIM2->MR1 = LPC_TIM2->TC + 20000;
		LPC_TIM2->MR0 = LPC_TIM2->TC + ServoArray.channel[0].pulseLength;
		ServoArray.idx = 0;
		GPIO_SetValue(ServoArray.channel[0].port, (1<<ServoArray.channel[0].bit));
	}
}

///////////////////////////////////////////////////////////////////////////////
/// \brief	add servo channel description
///
/// \param[in]	index	servo index (1..MAX_SERVOS)
/// \param[in]  servo   servo description
/// \return 	None
///////////////////////////////////////////////////////////////////////////////
uint8_t addServo(uint8_t index, struct Servo_t servo)
{
	if(index > 0 && index<=MAX_SERVOS) {
		GPIO_SetDir(servo.port, (1<<servo.bit), 1);	// Trigger signal for oscilloscope
		GPIO_ClearValue(servo.port, (1<<servo.bit));
		ServoArray.channel[index-1] = servo;
		ServoArray.maxIndex++;
		return 1;
	}
	else {
		return 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
/// @brief	set up TIMER2 match register.
///
/// @param[in]	None
/// @return 	None
///////////////////////////////////////////////////////////////////////////////

void initServoCtrl(void)
{
	ServoArray.maxIndex = 0;
	ServoArray.idx = 0;

	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &TIM_ConfigStruct);

	TIM_MATCHCFG_Type TIM_MatchConfigStruct;
	TIM_MatchConfigStruct.MatchChannel = 0;
	TIM_MatchConfigStruct.ResetOnMatch = DISABLE;
	TIM_MatchConfigStruct.StopOnMatch = DISABLE;
	TIM_MatchConfigStruct.IntOnMatch = ENABLE;
	TIM_MatchConfigStruct.MatchValue = 5000;
	TIM_MatchConfigStruct.ExtMatchOutputType = 0;
	TIM_ConfigMatch(LPC_TIM2, &TIM_MatchConfigStruct);

	TIM_MatchConfigStruct.MatchChannel = 1;
	TIM_MatchConfigStruct.ResetOnMatch = ENABLE;
	TIM_MatchConfigStruct.StopOnMatch = DISABLE;
	TIM_MatchConfigStruct.IntOnMatch = ENABLE;
	TIM_MatchConfigStruct.MatchValue = 20000;
	TIM_MatchConfigStruct.ExtMatchOutputType = 0;
	TIM_ConfigMatch(LPC_TIM2, &TIM_MatchConfigStruct);

    TIM_ResetCounter(LPC_TIM2);

//TODO check priority for Timer2
    NVIC_SetPriority(TIMER2_IRQn, ((0x01 << 3) | 0x02)); // preemption = 1, sub-priority = 2
    NVIC_EnableIRQ(TIMER2_IRQn); // Enable interrupt for timer 2
    TIM_Cmd(LPC_TIM2,ENABLE);
}

