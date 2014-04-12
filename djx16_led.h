#ifndef DJX16_LED_H
#define DJX16_LED_H

#include <avr/io.h>

enum ON_OFF_TOGGLE {
	LED_ON, LED_OFF, LED_TOGGLE
};

/* order matches bit value in latch DJX16_7SEG_ROW */
enum DJX16_LED_POS {
	DJX16_LED_SEG3,		/* 1<<0 = 7seg LED, rightmost digit */
	DJX16_LED_SEG2,		/*   :   */
	DJX16_LED_SEG1,		/*   :   */
	DJX16_LED_SEG0,		/* 1<<3 = 7seg LED, leftmost digit */
	DJX16_LED_INDIC_A,
	DJX16_LED_INDIC_B,
	DJX16_LED_CH9_to_CH16,	/* 1<<6 = Channel 9-16 LEDs */ 
	DJX16_LED_INDIC_C,
	DJX16_LED_LENGTH
};

#define DJX16_N_MASTER_GROUPS 2
#define DJX16_N_MASTER_INTENS 3

extern uint8_t djx16_led_buf[DJX16_LED_LENGTH]; /* LED mux data */
extern uint8_t djx16_led_masters[DJX16_N_MASTER_GROUPS*DJX16_N_MASTER_INTENS];

extern void djx16_led_init(void);

#if 0
extern void djx16_led_update(void);
#endif

extern void djx16_led_7seg(char index, char value);
extern void djx16_led_7seg_hex(char index, char nibble);

extern void djx16_led_indic(char index, enum ON_OFF_TOGGLE onofftoggle);

#endif
