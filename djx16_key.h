#ifndef DJX16_KEY_H
#define DJX16_KEY_H

#include <avr/io.h>

enum djx16_key_rows {
	DJX16_KEY_ROW_FLASH1_8,
	DJX16_KEY_ROW_FLASH9_16,
	DJX16_KEY_ROW_LATCH9_16,
	DJX16_KEY_ROW_TOP,
	DJX16_KEY_ROW_BOTTOM,
	DJX16_KEY_ROW_NROWS
};

/* key code for: currently no key pressed */
#define DJX16_KEY_NONE		0xff

extern uint8_t
djx16_key_get(void);

#endif
