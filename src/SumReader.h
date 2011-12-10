///
/// \file SumReader.h
/// \brief
/// Receiver sum signal reader module
/// \date 20.11.2011
/// \author cord
/// \details
/// - read Graupner HoTT SUM signal, connect SUM signal to P1[18]

#ifndef SUMREADER_H_
#define SUMREADER_H_

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

/// maximum number of receiver channel to read from sum signal
#define MAX_CHANNELS 12

#define TRIG_PIN (1<<23)
#define TRIG_PIN1 (1<<26)
#define TRIG_PIN2 (1<<29)
#define TRIG_PORT 1

/// receiver signal pulse lenght array
struct RecvSignal_t {
	uint32_t volatile channel[MAX_CHANNELS];	///< receiver channel 1 to n
	uint32_t volatile index;					///< index for channel array
};

#ifndef SUMREADER_C_
extern volatile struct RecvSignal_t Recv;
#endif

/************************** PRIVATE FUNCTIONS *************************/

void initSumReader(void);


#endif /* SUMREADER_H_ */
