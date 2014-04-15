#include "djx16_key.h"
#include "djx16_hw.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/* reverse operation for bit-shift, return bit number n that is set
   in v, i.e. v == 1 << n. If v does not have exactly one bit set, return -1 */

static int8_t
get_bit_num(uint8_t v)
{
	char i;
	for (i=0; i<8; i++)
		if (v == (1<<i))
			return i;
	return -1;
}

uint8_t
djx16_key_get_raw(int enable_chan_keys)
{
	return djx16_hw_key;
#if 0
	uint8_t row;
	int8_t  col;
	uint8_t keycode;

	cli();
	row = djx16_hw_key_row;
	col = (int8_t)djx16_hw_key_sense;
	djx16_hw_key_row = 0;
	djx16_hw_key_sense = 0;
	sei();

	/* bit pattern to "bit number set", or -1 of no valid single-bit */
	col = get_bit_num((uint8_t)col);

	if (col == -1 && enable_chan_keys){
		row = DJX16_KEY_ROW_FLASH1_8;
		col = get_bit_num(djx16_key_flash1_8);
	}

	if (col == -1 && enable_chan_keys) {
		row = DJX16_KEY_ROW_FLASH9_16;
		col = get_bit_num(djx16_key_flash9_16);
	}

	if (col == -1)
		return DJX16_KEY_NONE; /* no key pressed */

	keycode = (row & 0x07) << 3 | col;
	return keycode;
#endif
}

#if 0

static uint8_t djx16_key_debounce_ctr
static uint8_t djx16_key_debounce_keycode;

uint8_t
djx16_key_get_debounced(int enable_chan_keys)
{
	uint8_t keycode;

	keycode = djx16_key_get_raw();
}

#endif
