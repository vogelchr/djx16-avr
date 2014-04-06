#include "djx16_hw.h"
#include "djx16_led.h"
#include "djx16_key.h"

#include <avr/interrupt.h>
#include <avr/io.h>

/*
 * for efficiency reasons, everything related to LED matrix, key matrix
 * or ADC readout happens in this huge timer ISR
 */

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


// static uint8_t ISR_Counter;
static uint32_t tick_ctr;

uint8_t djx16_hw_adc[16];

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
	LATCH(bit, PORTD, 4); /* latch DJX16_7SEG_ROW, LED row selector */
	LATCH(v, PORTD, 3);   /* latch DJX16_7SEG_COL */

skip_mux_leds:
	/* ===== MASTER LEDS ============================================= */
	if (count == 0)
		offset = 0;
	else if (count < 3)
		offset = 2;
	else
		offset = 4;

	v = djx16_led_masters[offset];
	LATCH(v, PORTD, 5); /* latch DJX16_CHAN_LED */

	v = djx16_led_masters[offset+1]; /* DJX16_MASTER_LED */
	LATCH(v, PORTB, 0);

	/* ===== KEYS ==================================================== */
	if (bit & 0xe0)
		goto skip_keys;

	LATCH(~bit, PORTB, 1); /* latch  -> DJX16_KEY_MTX */

	/* read in key matrix */
	DDRA  = 0x00;       /* port A all inputs */
	PORTA = 0xff;       /* weak pullup */
	READIN(v, PORTB, 2); /* PB2 = key matrix driver output enable */
	PORTA = 0x00;       /* disable pullups on porta */
	DDRA  = 0xff;       /* output again */

	if (v != 0xff) {
		djx16_key_row     = count;
		djx16_key_pressed = ~v;
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
		LATCH(v, PORTB, 3); /* \WR = 0 */
	}

	if ( count == 7 ) {
		/* read last conversion */
		DDRA = 0x00; /* all input */
		READIN(v, PORTB, 4);
		DDRA = 0xff; /* all output again */

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

#if 0

static inline void
djx16_latch_sleep(void)
{
	asm volatile ("nop;\n\t nop;\n\t nop;\n\t nop;\n\t nop;\n\t nop;");
}

void
djx16_hw_latch(enum DJX16_LATCH latch, unsigned char value)
{
	DDRA = 0xff;
	PORTA = value;

	switch(latch){
	case DJX16_MASTER_LED:
		PORTB |= _BV(0);
		djx16_latch_sleep();
		PORTB &= ~_BV(0);
		break;

	case DJX16_KEY_MTX:
		PORTB |= _BV(1);
		djx16_latch_sleep();
		PORTB &= ~_BV(1);
		break;

	case DJX16_ANALOX_MUX:
		PORTB |= _BV(3);
		djx16_latch_sleep();
		PORTB &= ~_BV(3);
		break;

	case DJX16_7SEG_COL:
		PORTD |= _BV(3);
		djx16_latch_sleep();
		PORTD &= ~_BV(3);
		break;

	case DJX16_7SEG_ROW:
		PORTD |= _BV(4);
		djx16_latch_sleep();
		PORTD &= ~_BV(4);
		break;

	case DJX16_CHAN_LED:
		PORTD |= _BV(5);
		djx16_latch_sleep();
		PORTD &= ~_BV(5);
		break;
	}

	DDRA  = 0x00;
	PORTA = 0x00;
}

#endif
