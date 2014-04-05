#include "djx16_led.h"
#include "djx16_hw.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

/* buffer for storing the multiplexed LED state */
static uint8_t djx16_led_buf[DJX16_LED_LENGTH];

/* counter for LED updates, also shared by the ADC and mater led
 * updates, just makes the matrixx go dark a little
 */
static uint8_t djx16_led_ctr=0;

void
djx16_led_init(void)
{
	/* nothing */
}

/* DJX16_7SEG_ROW latch controls the LED segment, logic 1 turns on digit
 * Bit0 (LSB) : Rightmost LED digit
 *    :
 * Bit3       : Leftmost LED digit
 * Bit4       : Minus sign and single dots in LED display
 * Bit5       : Enable, Cross On, Cross Off, No Fade Time, Step Time, Patch
 * Bit6       : Channel 9..16
 * Bit7       : Flash, Assign/Kill, Assign Add, 
 *
 * DJX16_7SEG_COL latch controls single segments
 *
 *        0=01
 *       +---+
 *  20=5 |   | 1=02
 *       +-6-+      6=40
 *  10=4 |   | 2=04
 *       +---+
 *        3=08
 *
 */

const unsigned char font_7seg[] PROGMEM = {
	/* 0 */ 0x3f,  /* 1 */ 0x06,  /* 2 */ 0x5b, /* 3 */ 0x4f,
	/* 4 */ 0x66,  /* 5 */ 0x6d,  /* 6 */ 0x7d, /* 7 */ 0x27,
	/* 8 */ 0x7f,  /* 9 */ 0x6f,  /* a */ 0x77, /* b */ 0x7c,
	/* c */ 0x58,  /* d */ 0x5e,  /* e */ 0x79, /* f */ 0x71,
};

void
djx16_led_update_digit_raw(unsigned char num, unsigned char code)
{
	djx16_led_buf[num]=code;
}

void
djx16_led_update(void)
{
	int i;
	unsigned char c;

	if (djx16_led_ctr > 7) {
		djx16_led_ctr = 0;
	}

	djx16_latch(DJX16_7SEG_ROW, 1 << djx16_led_ctr);
	djx16_latch(DJX16_7SEG_COL, djx16_led_buf[djx16_led_ctr]);

	c = 0;
	for(i=0; i<8; i++)
		if( i < djx16_led_ctr )
			c |= (1<<i);
	djx16_latch(DJX16_CHAN_LED, c);
	djx16_latch(DJX16_MASTER_LED, c);

	djx16_led_ctr++;
}


void
djx16_led_set_7seg(char index, char value)
{
	/* implementation detail:
	 *    driver bit 0 is 4th segment
	 *    driver bit 3 is 1st segment
	 * LED segments on are 0
	 */
	djx16_led_buf[3-index] = ~value;
}

void
djx16_led_set_7seg_hex(char index, char nibble)
{
	djx16_led_set_7seg(index, pgm_read_byte(font_7seg + nibble));
}
