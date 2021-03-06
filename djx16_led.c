#include "djx16_led.h"
#include "djx16_hw.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

#include <string.h>

#define INIT_ONES(what) do { memset(what, 0xff, sizeof(what)); } while(0)

void
djx16_led_init(void)
{
	INIT_ONES(djx16_hw_led_masters);
	INIT_ONES(djx16_hw_led_buf);
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
	djx16_hw_led_buf[num]=code;
}

void
djx16_led_7seg(char index, char value)
{
	index &= 0x03; /* make sure index is 0..3 */

	/* implementation detail:
	 *    driver bit 0 is 4th segment
	 *    driver bit 3 is 1st segment
	 * LED segments on are 0
	 */
	djx16_hw_led_buf[3-index] = ~value;
}

void
djx16_led_7seg_hex(char index, char nibble)
{
	nibble &= 0x0f; /* make sure nibble is 0..f */
	djx16_led_7seg(index, pgm_read_byte(font_7seg + nibble));
}

static enum djx16_led_row
djx16_led_indicator_find(enum djx16_led_indic index, uint8_t *bit)
{
	enum djx16_led_row pos = DJX16_LED_INDIC_A;

	if (index >= DJX16_LED_NUM_INDICATORS)
		return DJX16_LED_NUM_INDICATORS; /* not valid */

	if (index < 8) {
		pos = DJX16_LED_INDIC_A;
	} else if (index < 16) {
		pos = DJX16_LED_INDIC_B;
		index -= 8;
	} else {
		pos = DJX16_LED_INDIC_C;
		index -= 16;
	}

	*bit = (1<<index);
	return pos;
}


void
djx16_led_indicator(enum djx16_led_indic index, enum djx16_on_off_toggle onofftoggle)
{
	enum djx16_led_row pos;
	unsigned char bit=0;

	pos = djx16_led_indicator_find(index, &bit);
	if (pos == DJX16_LED_NUM_INDICATORS)
		return;

	switch (onofftoggle) {
	case LED_OFF:
		djx16_hw_led_buf[pos] |= bit;
		break;
	case LED_ON:
		djx16_hw_led_buf[pos] &= ~bit;
		break;
	case LED_TOGGLE:
		djx16_hw_led_buf[pos] ^= bit;
		break;
	}
}

char
djx16_led_indicator_get(enum djx16_led_indic index)
{
	enum djx16_led_row pos;
	uint8_t bit;

	pos = djx16_led_indicator_find(index, &bit);
	if (pos == DJX16_LED_NUM_INDICATORS)
		return 0;

	return !!(djx16_hw_led_buf[pos] & bit);
}

/* the LED-masters array is organized as a sequence of groups of LEDs and
 * intensities, for 3 intensities and 2 LEDs, it's the following
 * organization:
 *
 * led_masters[] = { Group0_Intensits1, G1_I1, G0_I2, G1_I2, G0_I4, G1_I4 }
 *
 * The data for LED "i" is stored in group i/8, bit 1<<i.
 *
 * LEDs whose bit is 0 are turned on. Intensity slot I2 is displayed twice
 * as long as intensity slot I1, I4 4 (in reality: 5) times as long as I2.
 */

void
djx16_led_master(char index, uint8_t value)
{
	uint8_t *p, bit;
	char i;

	p = djx16_hw_led_masters;

	for (i=0; i<DJX16_N_MASTER_GROUPS; i++) { /* find group for index */
		if (index < 8)
			break;
		index -= 8;
		p++;				/* to to next group in arr */
	}

	if (index > 8)				/* index out of range */
		return;

	bit = 1 << index;			/* get bit for this LED */

	value >>= (8-DJX16_N_MASTER_INTENS);	/* align "N" intensity bit to LSB */

	for (i=0; i<DJX16_N_MASTER_INTENS; i++) {
		if (value & 0x01)
			*p &= ~bit;
		else
			*p |= bit;
		value >>= 1;
		p += DJX16_N_MASTER_GROUPS;
	}

}
