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
djx16_key_get()
{
	uint8_t ret = DJX16_KEY_NONE;
	cli();
	if (djx16_hw_key     != DJX16_KEY_NONE &&
	    djx16_hw_key_ctr == DJX16_KEY_DEBOUNCE) {
		djx16_hw_key_ctr++; /* make sure we only return once */
		ret = djx16_hw_key;
	}
	sei();
	return ret;
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
