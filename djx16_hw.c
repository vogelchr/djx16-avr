#include "djx16_hw.h"
#include "djx16_led.h"
#include "djx16_key.h"

#include <avr/interrupt.h>
#include <avr/io.h>

/*
 * for efficiency reasons, everything related to LED matrix, key matrix
 * or ADC readout happens in this huge timer ISR
 */

/* === data accessed by the ISR === */

/* buffer for storing the multiplexed LED state */
uint8_t djx16_led_buf[DJX16_LED_LENGTH];
uint8_t djx16_led_masters[DJX16_N_MASTER_GROUPS*DJX16_N_MASTER_INTENS];
static uint32_t tick_ctr;
uint8_t djx16_hw_adc[DJX16_ADC_LENGTH];

uint8_t djx16_key_row;
uint8_t djx16_key_pressed;
uint8_t djx16_key_flash1_8;
uint8_t djx16_key_flash9_16;

/*
 * this macro latches value "val" into the HC573 buffer with its
 * LE pin connected to PORT/bit
 */
#define LATCH(val, port, bit) do {                     \
		PORTA = (val);                         \
		(port) |= _BV(bit);                    \
		asm volatile ("nop;\n\tnop;\n\tnop;"); \
		(port) &= ~_BV(bit);                   \
        } while(0)

#define READIN(val, port, bit) do {                    \
		(port) &= ~_BV(bit);                   \
		asm volatile ("nop;\n\tnop;");         \
		val = PINA;                            \
		(port) |= _BV(bit);                    \
        } while(0)

#define DJX16_LATCH_CHAN_LED(v)		LATCH(v, PORTD, 5)
#define DJX16_LATCH_MASTER_LED(v)	LATCH(v, PORTB, 0)
#define DJX16_LATCH_KEY_MTX(v)		LATCH(v, PORTB, 1)
#define DJX16_LATCH_ADC_MUX(v)		LATCH(v, PORTB, 3)
#define DJX16_LATCH_7SEG_ROW(v)		LATCH(v, PORTD, 4)
#define DJX16_LATCH_7SEG_COL(v)		LATCH(v, PORTD, 3)

SIGNAL(TIMER0_OVF_vect){ /* timer/counter 0 overflow */
	uint8_t count, c_high;  /* 3 LSB and 5 not-so-LSB of tick counter */
	uint8_t bit;            /* 1 << count */
	uint8_t v;
	uint8_t offset;
	uint8_t adc_chan;

	tick_ctr++;                      /* global tick counter */
	count  =  tick_ctr       & 0x07; /* 3 lowest bits */
	c_high = (tick_ctr >> 3) & 0x1f; /* 5 next bits */
	bit   = 1 << count;              /* used at various places below.. */

	DDRA  = 0xff;          /* make sure we can write out... */
	/* ===== MULTIPLEXED LEDS ======================================== */

	if (count >= DJX16_LED_LENGTH)
		goto skip_mux_leds;

	v = djx16_led_buf[count];

	DJX16_LATCH_7SEG_ROW(bit);
	DJX16_LATCH_7SEG_COL(v);

skip_mux_leds:
	/* ===== MASTER LEDS ============================================= */
	if (count == 0)
		offset = 0;
	else if (count < 3)
		offset = 2;
	else
		offset = 4;

	v = djx16_led_masters[offset];
	DJX16_LATCH_CHAN_LED(v);

	v = djx16_led_masters[offset+1]; /* DJX16_MASTER_LED */
	DJX16_LATCH_MASTER_LED(v);

	/* ===== KEYS ==================================================== */
	if (bit & 0xe0)
		goto skip_keys;

	DJX16_LATCH_KEY_MTX(~bit); /* latch  -> DJX16_KEY_MTX */

	/* read in key matrix */
	DDRA  = 0x00;       /* port A all inputs */
	PORTA = 0xff;       /* weak pullup */
	READIN(v, PORTB, 2); /* PB2 = key matrix driver output enable */
	PORTA = 0x00;       /* disable pullups on porta */
	DDRA  = 0xff;       /* output again */

	if (count == DJX16_KEY_ROW_FLASH1_8) {
		djx16_key_flash1_8 = ~v;
	} else if (count == DJX16_KEY_ROW_FLASH9_16) {
		djx16_key_flash9_16 = ~v;
	} else {
		if (v != 0xff) {
			djx16_key_row     = count;
			djx16_key_pressed = ~v;
		}
	}

skip_keys:
	/* ===== ADC ===================================================== */
	adc_chan = c_high & 0x0f;
	/* on cycle 0, set mux, let analog system settle,
           on cycle 6, trigger conversion
           on cycle 7, read back
         */
	if ( (count == 0) || (count == 6) ) {
		v = ((adc_chan & 0x07) << 3) | _BV(1) | _BV(2) | _BV(0);
			/* analog mux input select S1..S3 = Q3..Q5,
			 * turn on both \E bits, clear on demand below */
		if (adc_chan & 0x08) {
			v &= ~_BV(1);  /* E on master mux */
		} else {
			v &= ~_BV(2);  /* E on channel mux */
		}

		if (count == 0)
			v &= ~_BV(0); /* \WR = 0 in cycle 0, 1 in cycle 6 */
		DJX16_LATCH_ADC_MUX(v);
	}

	if ( count == 7 ) {
		/* read last conversion */
		DDRA = 0x00; /* all input */
		READIN(v, PORTB, 4);
		DDRA = 0xff; /* all output again */

		if (adc_chan <= DJX16_ADC_LENGTH)
			djx16_hw_adc[adc_chan] = v;
	} 

	DDRA = 0x00;
	PORTA = 0x00;
}

extern uint32_t djx16_hw_tick_ctr(void){
	uint32_t v;
	cli();
	v = tick_ctr;
	sei();
	return v;
}


