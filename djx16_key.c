#include "djx16_key.h"
#include "djx16_hw.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t
djx16_hw_get_key_raw(void){
	uint8_t row, pressed;
	cli();
	row =     djx16_key_row;
	pressed = djx16_key_pressed;
	djx16_key_row = 0;
	djx16_key_pressed = 0;
	sei();
}
