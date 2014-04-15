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

/*
 *       +----+----+----+----+----+----+----+----+
 * (MSB) : D7 : D6 : D5 : D4 : D3 : D2 : D1 : D0 : (LSB)
 *       +----+----+----+----+----+----+----+----+
 *       : 0  : Column(0..7) : 0  :  Row(0..4)   :
 *       +----+----+----+----+----+----+----+----+
 */

#define DJX16_KEY_COL_SHIFT	4

#define DJX16_KEY_ROW(kc)	((kc) & 0x07)
#define DJX16_KEY_COL(kc)	(((kc)>>DJX16_KEY_COL_SHIFT) & 0x07)

/* _FAST does not make sure that row and col are 0..7 */
#define DJX16_KEYCODE_FAST(col,row) (((col)<<DJX16_KEY_COL_SHIFT)|(row))
#define DJX16_KEYCODE(col,row)	DJX16_KEYCODE_FAST((col)&0x07,(row)&0x07)

extern uint8_t
djx16_key_get(void);

#endif
