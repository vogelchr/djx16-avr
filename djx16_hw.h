#ifndef DJX16_HW_H
#define DJX16_HE_H

#include <avr/io.h>
#include <avr/interrupt.h>

enum DJX16_LATCH {
	DJX16_MASTER_LED,
	DJX16_KEY_MTX,
	DJX16_ANALOX_MUX,
	DJX16_7SEG_COL,
	DJX16_7SEG_ROW,
	DJX16_CHAN_LED
};

/* get a tick-counter that is increased with about 500 Hz */
extern uint32_t djx16_hw_tick_ctr(void);
extern uint8_t djx16_hw_adc[16];


#endif
