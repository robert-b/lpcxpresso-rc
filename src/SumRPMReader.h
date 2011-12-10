// SumRPMReader.h 
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
/// \file SumRPMReader.h
/// \brief
/// enter brief description of SumRPMReader.h here
/// \date 10.12.2011
/// \author cord
/// \details
/// enter detailed description here10.12.2011

#ifndef SUMRPMREADER_H_
#define SUMRPMREADER_H_

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

/// maximum number of receiver channel to read from sum signal
#define MAX_CHANNELS 12


/// receiver signal pulse lenght array
struct RecvSignal_t {
	uint32_t volatile channel[MAX_CHANNELS];	///< receiver channel 1 to n
	uint32_t volatile index;					///< index for channel array
};

#ifndef SUMREADER_C_
extern volatile struct RecvSignal_t Recv;
#endif

/************************** PRIVATE FUNCTIONS *************************/

void initSumRPMReader(void);



#endif /* SUMRPMREADER_H_ */
