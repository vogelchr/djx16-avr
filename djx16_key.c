#include "djx16_key.h"
#include "djx16_hw.h"

#include <avr/io.h>
#include <avr/interrupt.h>

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
djx16_hw_get_key_raw(void){
	uint8_t row;
	int8_t  pressed;
	uint8_t keycode;

	cli();
	row =     djx16_hw_key_row;
	pressed = get_bit_num(djx16_hw_key_sense);
	djx16_hw_key_row = 0;
	djx16_hw_key_sense = 0;
	sei();

	keycode = (row & 0x07) << 3 | pressed;
	return keycode;
}
