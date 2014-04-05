#ifndef DJX16_HW_H
#define DJX16_HE_H

#include <avr/io.h>

enum DJX16_LATCH {
	DJX16_MASTER_LED,
	DJX16_KEY_MTX,
	DJX16_ANALOX_MUX,
	DJX16_7SEG_COL,
	DJX16_7SEG_ROW,
	DJX16_CHAN_LED
};

extern void
djx16_latch(enum DJX16_LATCH latch, unsigned char value);

#endif
