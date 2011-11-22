// ServoCtrl.h 
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
/// \file ServoCtrl.h
/// \brief
/// enter brief description of ServoCtrl.h here
/// \date 22.11.2011
/// \author cord
/// \details
/// enter detailed description here

#ifndef SERVOCTRL_H_
#define SERVOCTRL_H_

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

/// maximum number of servo output signals
#define MAX_SERVOS 4

struct Servo_t {
	uint8_t inpChannel;
	uint8_t port;
	uint8_t bit;
	uint32_t pulseLength;
};
/// servo signal pulse length array
struct ServoCtrl_t {
	struct Servo_t channel[MAX_SERVOS];	///< servo channel 1 to n
	uint32_t maxIndex;					///< number of servos
	uint32_t idx;						///< index for servo array
};

#ifndef SERVOCTRL_C_
extern volatile struct ServoCtrl_t ServoArray;
#endif

/************************** PRIVATE FUNCTIONS *************************/

void initServoCtrl(void);

uint8_t addServo(uint8_t index, struct Servo_t servo);

#endif /* SERVOCTRL_H_ */
