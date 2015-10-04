#include "djx16_key.h"
#include "djx16_hw.h"
#include "djx16_led.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t
djx16_key_get_raw()
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

uint8_t
djx16_key_get()
{
	static uint8_t djx16_key_state;
	uint8_t ret = DJX16_KEY_NONE;

	ret = djx16_key_get_raw();
	if (ret == DJX16_KEY_NONE)
		goto out;


	if (ret == DJX16_KEY_SHIFT) {
		djx16_key_state ^= DJX16_KEY_IS_SHIFTED;
		ret = DJX16_KEY_NONE;
//		djx16_led_indicator(DJX16_LED_MIDI, !!(state & DJX16_KEY_IS_SHIFTED));
		goto out;
	}

	if (djx16_key_state & DJX16_KEY_IS_SHIFTED) {
		djx16_key_state &= ~DJX16_KEY_IS_SHIFTED;
		ret   |=  DJX16_KEY_IS_SHIFTED;
	}

out:
	return ret;
}
