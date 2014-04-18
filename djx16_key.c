#include "djx16_key.h"
#include "djx16_hw.h"

#include <avr/io.h>
#include <avr/interrupt.h>

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

