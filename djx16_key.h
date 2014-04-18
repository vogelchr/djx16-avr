#ifndef DJX16_KEY_H
#define DJX16_KEY_H

#include <avr/io.h>

/*
 *       +----+----+----+----+----+----+----+----+
 * (MSB) : D7 : D6 : D5 : D4 : D3 : D2 : D1 : D0 : (LSB)
 *       +----+----+----+----+----+----+----+----+
 *       : S  : Row(0..4)    : 0  : Column(0..7) :
 *       +----+----+----+----+----+----+----+----+
 *         ^
 *         |
 *         +-- shift pressed before (or'ed with keycode)
 */

#define DJX16_KEY_ROW_SHIFT	4

#define DJX16_KEY_ROW(kc)	(((kc)>>DJX16_KEY_ROW_SHIFT) & 0x07)
#define DJX16_KEY_COL(kc)	((kc) & 0x07)

/* _FAST does not make sure that row and col are 0..7 */
#define DJX16_KEYCODE_FAST(row,col) (((row)<<DJX16_KEY_ROW_SHIFT)|(col))
#define DJX16_KEYCODE(row,col)	DJX16_KEYCODE_FAST((row)&0x07,(col)&0x07)

enum djx16_key_rows {
	DJX16_KEY_ROW_FLASH1_8,
	DJX16_KEY_ROW_FLASH9_16,
	DJX16_KEY_ROW_LATCH9_16,
	DJX16_KEY_ROW_TOP,
	DJX16_KEY_ROW_BOTTOM,
	DJX16_KEY_ROW_NROWS
};

enum djx16_key_names {
	DJX16_KEY_FLASH1  = DJX16_KEYCODE(0, 0),
	DJX16_KEY_FLASH2  = DJX16_KEYCODE(0, 1),
	DJX16_KEY_FLASH3  = DJX16_KEYCODE(0, 2),
	DJX16_KEY_FLASH4  = DJX16_KEYCODE(0, 3),
	DJX16_KEY_FLASH5  = DJX16_KEYCODE(0, 4),
	DJX16_KEY_FLASH6  = DJX16_KEYCODE(0, 5),
	DJX16_KEY_FLASH7  = DJX16_KEYCODE(0, 6),
	DJX16_KEY_FLASH8  = DJX16_KEYCODE(0, 7),
	DJX16_KEY_FLASH9  = DJX16_KEYCODE(1, 0),

	DJX16_KEY_FLASH10 = DJX16_KEYCODE(1, 1),
	DJX16_KEY_FLASH11 = DJX16_KEYCODE(1, 2),
	DJX16_KEY_FLASH12 = DJX16_KEYCODE(1, 3),
	DJX16_KEY_FLASH13 = DJX16_KEYCODE(1, 4),
	DJX16_KEY_FLASH14 = DJX16_KEYCODE(1, 5),
	DJX16_KEY_FLASH15 = DJX16_KEYCODE(1, 6),
	DJX16_KEY_FLASH16 = DJX16_KEYCODE(1, 7),

	DJX16_KEY_LATCH9  = DJX16_KEYCODE(2, 0),
	DJX16_KEY_LATCH10 = DJX16_KEYCODE(2, 1),
	DJX16_KEY_LATCH11 = DJX16_KEYCODE(2, 2),
	DJX16_KEY_LATCH12 = DJX16_KEYCODE(2, 3),
	DJX16_KEY_LATCH13 = DJX16_KEYCODE(2, 4),
	DJX16_KEY_LATCH14 = DJX16_KEYCODE(2, 5),
	DJX16_KEY_LATCH15 = DJX16_KEYCODE(2, 6),
	DJX16_KEY_LATCH16 = DJX16_KEYCODE(2, 7),

	DJX16_KEY_CROSS   = DJX16_KEYCODE(3, 0),
	DJX16_KEY_PUP     = DJX16_KEYCODE(3, 1),
	DJX16_KEY_PDOWN   = DJX16_KEYCODE(3, 2),
	DJX16_KEY_SUP     = DJX16_KEYCODE(3, 3),
	DJX16_KEY_SDOWN   = DJX16_KEYCODE(3, 4),

	DJX16_KEY_ADDKILL = DJX16_KEYCODE(3, 5),
	DJX16_KEY_PROGM   = DJX16_KEYCODE(3, 6),
	DJX16_KEY_ASSIGN  = DJX16_KEYCODE(3, 7),
	DJX16_KEY_PATCH   = DJX16_KEYCODE(4, 0),
	DJX16_KEY_FULLON  = DJX16_KEYCODE(4, 1),

	DJX16_KEY_SHIFT   = DJX16_KEYCODE(4, 2),
	DJX16_KEY_LOOP    = DJX16_KEYCODE(4, 3),
	DJX16_KEY_CHASE   = DJX16_KEYCODE(4, 4),
	DJX16_KEY_TAP     = DJX16_KEYCODE(4, 5),
	DJX16_KEY_STANDBY = DJX16_KEYCODE(4, 6),

	DJX16_KEY_NONE    = 0xff,
	DJX16_KEY_IS_SHIFTED = 0x80
};

/* used in hardware loop to count periods for how many the
 * key has been pressed ...
 */
#define DJX16_KEY_DEBOUNCE	3

extern uint8_t
djx16_key_get(void);

#endif
