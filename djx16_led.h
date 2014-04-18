#ifndef DJX16_LED_H
#define DJX16_LED_H

#include <avr/io.h>

enum ON_OFF_TOGGLE {
	LED_OFF,
	LED_ON,
	LED_TOGGLE
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

enum DJX16_LED_INDICATORS {
	DJX16_LED_COLON    = 0x01,  /* group INDIC_A */
	DJX16_LED_MANUSTEP = 0x02,
	DJX16_LED_MANUAL   = 0x03,
	DJX16_LED_AUDIO    = 0x04,
	DJX16_LED_CHASE    = 0x05,
	DJX16_LED_TAPSYNC  = 0x06,

	DJX16_LED_CROSSOFF = 0x08,  /* group INDIC_B */
	DJX16_LED_CROSSON  = 0x09,
	DJX16_LED_ENABLE   = 0x0b,
	DJX16_LED_NOFADE   = 0x0c,
	DJX16_LED_STEPTIME = 0x0d,
	DJX16_LED_PATCH    = 0x0f,

	DJX16_LED_ADDKILL  = 0x10,  /* group INDIC_C */
	DJX16_LED_ASSIGN   = 0x11,
	DJX16_LED_PROGRAM  = 0x12,
	DJX16_LED_STANDBY  = 0x13,
	DJX16_LED_CHFLASH  = 0x14,
	DJX16_LED_AS_KILL  = 0x15,
	DJX16_LED_AS_ADD   = 0x16,
	DJX16_LED_MIDI     = 0x17
};

/* we have two groups of master LEDs (each 8 LEDs, logical)
   each gruop has 3 different intensity bits (2^3 = 8 intensities) */

#define DJX16_N_MASTER_GROUPS 2
#define DJX16_N_MASTER_INTENS 3

#define DJX16_LED_NUM_INDICATORS	24 /* indicator A, B, C... */

extern void djx16_led_init(void);

/* set 7seg position index to a certain bit-pattern */
extern void djx16_led_7seg(char index, char value);

/* set 7seg position index to display 0..f according to nibble */
extern void djx16_led_7seg_hex(char index, char nibble);

/* set LED indicator "index" either to ON, OFF or toggle it.
 * index is 0.. DJX16_LED_NUM_INDICATORS-1
 */
extern void djx16_led_indicator(char index, enum ON_OFF_TOGGLE onofftoggle);

extern void djx16_led_master(char index, uint8_t value);

#endif
