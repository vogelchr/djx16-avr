#ifndef DJX16_LED_H
#define DJX16_LED_H

#include <avr/io.h>

enum DJX16_LED_POS {
	DJX16_LED_SEG3,
	DJX16_LED_SEG2,
	DJX16_LED_SEG1,
	DJX16_LED_SEG0,
	DJX16_LED_INDIC_A,
	DJX16_LED_CH9_to_CH16,
	DJX16_LED_INDIC_B,
	DJX16_LED_LENGTH
};

extern void djx16_led_init(void);
extern void djx16_led_update(void);

extern void djx16_led_set_7seg(char index, char value);
extern void djx16_led_set_7seg_hex(char index, char nibble);

#endif
