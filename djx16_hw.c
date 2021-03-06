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
uint8_t djx16_hw_led_buf[DJX16_LED_LENGTH];
uint8_t djx16_hw_led_masters[DJX16_N_MASTER_GROUPS*DJX16_N_MASTER_INTENS];
static uint32_t djx16_hw_tick_ctr;
uint8_t djx16_hw_adc[DJX16_ADC_LENGTH];
uint16_t djx16_hw_adc_dirty;

uint8_t djx16_hw_key;
uint8_t djx16_hw_key_ctr;

/*
 * this macro latches value "val" into the HC573 buffer with its
 * LE pin connected to PORT/bit being pulled high
 */
#define LATCH(val, port, bit) do {                     \
		PORTA = (val);                         \
		(port) |= _BV(bit);                    \
		asm volatile ("nop;");                 \
		(port) &= ~_BV(bit);                   \
        } while(0)

/*
 * this macro reads in data from the HC573 buffers used for input,
 * the \OE pin connected to PORT/bit will be pulled low. Also
 * used for the ADC with PORT/bit connected to the \RD pin.
 *
 * (the weak pullups don't work with 2 nops, need at least
 *  3 or 4 for the signal to stabilize)
 */
#define READIN(val, port, bit) do {                    \
		PORTA = 0xff;                          \
		DDRA = 0x00;                           \
		(port) &= ~_BV(bit);                   \
		asm volatile ("nop;\n\tnop;");         \
		asm volatile ("nop;\n\tnop;");         \
		val = PINA;                            \
		(port) |= _BV(bit);                    \
		PORTA = 0x00;                          \
		DDRA = 0xff;                           \
        } while(0)

#define DJX16_LATCH_CHAN_LED(v)		LATCH(v, PORTD, 5)
#define DJX16_LATCH_MASTER_LED(v)	LATCH(v, PORTB, 0)
#define DJX16_LATCH_KEY_MTX(v)		LATCH(v, PORTB, 1)
#define DJX16_LATCH_ADC_MUX(v)		LATCH(v, PORTB, 3)
#define DJX16_LATCH_7SEG_ROW(v)		LATCH(v, PORTD, 4)
#define DJX16_LATCH_7SEG_COL(v)		LATCH(v, PORTD, 3)

SIGNAL(TIMER0_OVF_vect){ /* timer/counter 0 overflow */
	uint8_t count, bit, v, adc_chan;
	int offset;
	uint8_t key_col, last_key_row, keycode;

	djx16_hw_tick_ctr++;                      /* global tick counter */
	count  =  djx16_hw_tick_ctr       & 0x07; /* 3 lowest bits */
	adc_chan = (djx16_hw_tick_ctr >> 1) & 0x0f;
	bit   = 1 << count;              /* used at various places below.. */

	DDRA  = 0xff;          /* make sure we can write out... */

	/* ===== MULTIPLEXED LEDS ======================================== */
	if (count >= DJX16_LED_LENGTH)
		goto skip_mux_leds;

	v = djx16_hw_led_buf[count];

	DJX16_LATCH_7SEG_ROW(bit);
	DJX16_LATCH_7SEG_COL(v);

skip_mux_leds:
	/* ===== MASTER LEDS ============================================= */
	if (count == 0)		/* *-------    active period, weight = 1 */
		offset = 0;	/* 01234567 <- count                     */
	else if (count == 1)	/* -**-----                   weight = 2 */
		offset = 2;	/*                                       */
	else if (count == 3)	/* ---*****                   weight = 5 */
		offset = 4;	/*                                       */
	else
		goto skip_master_leds;

	v = djx16_hw_led_masters[offset];	/* channel 1..8 LEDs */
	DJX16_LATCH_CHAN_LED(v);

	v = djx16_hw_led_masters[offset+1];	/* master M,A,B LEDs */
	DJX16_LATCH_MASTER_LED(v);

skip_master_leds:
	/* ===== KEYS ==================================================== */
	if (count >= DJX16_KEY_ROW_NROWS)
		goto skip_keys;

	/* the keyboard matrixx functions like this:
	 *
	 *     LE  \OE
	 *      |  |
	 *     +----+            Key Matrix
	 *    /|D  Q|---..       -|-
	 *  ||/|D  Q|-------|<|--o o--+
	 *  ||/|D  Q|---..            |
	 *  || +----+               ::|
	 *  ||                      |||
	 *  ++======================+++=========== Data Bus
	 *
	 * To sense a key, we put a zero corresponding to the row we want
	 * to sense into the latch, but keep \OE still high, so the outputs
	 * don't get pulled low: In case of a pressed switch, this would
	 * mess up the data bus and feed back into the latch input.
	 *
	 * Then we put the data-bus to input, with pullups enabled
	 * (PORTA=0xff DDRA=0xff) and briefly turn on the latch' output
	 * (\OE = 0). We sample the data bus, turn off the output and put
	 * everything back to normal.
	 *
	 * When idle, we force the latch to all ones, with the output
	 * enabled to minimize the risk that the cathode side of a diode
	 * would drift low over time, and when pressing a button charge
	 * would be injected onto the databus.
	 *
	 * Therefore, we latch 0xff onto the '738 and enable the output
	 * when not using the matrix:
	 *	DJX16_LATCH_KEY_MTX(0xff); PORTB &= ~_BV(2);
	 */

	PORTB |= _BV(2);		/* matrix driver H -> Z */
	DJX16_LATCH_KEY_MTX(~bit);	/* pull "current" row low */

	READIN(v, PORTB, 2);		/* PB2 = key mtx driver output enable */

	/* now put again the "output high" state in the matrix register */
	DJX16_LATCH_KEY_MTX(0xff);	/* matrix to idle */
	PORTB &= ~_BV(2);		/* \OE -> low */

	last_key_row = DJX16_KEY_ROW(djx16_hw_key);

	if (djx16_hw_key != DJX16_KEY_NONE) {	/* currently key pressed */
		if (last_key_row != count)	/* don't even bother other rows */
			goto skip_keys;
	}

	key_col = 0;			/* start column 0 */
	while (v) {			/* unpressed keys are 1 bits */
		if (!(v&0x01))		/* LSB is unset? -> key pressed */
			break;
		key_col += 1;
		v >>= 1;
	}

	if (key_col == 8)		/* no key was pressed */
		keycode = DJX16_KEY_NONE;
	else
		keycode = DJX16_KEYCODE(count, key_col);

	if (keycode == djx16_hw_key) {
		if (djx16_hw_key_ctr < DJX16_KEY_DEBOUNCE)
			djx16_hw_key_ctr++;
	} else {
		djx16_hw_key_ctr = 0;
		djx16_hw_key = keycode;
	}

skip_keys:
	/* ===== ADC ===================================================== */

	/* that's a little ugly, we borrow two bits from c_high, two bits
	 * from the counter, and have two phases. First, we get the
	 * last converted result, then we setup the mux, after the mux
	 * was allowed to settle for one period, we trigger the ADC
	 * via the \WR pin. */

	if (adc_chan >= DJX16_ADC_LENGTH)
		goto skip_adc;

	if (!(count & 0x01)) {				/* read chan */
		int8_t adc_diff;

		READIN(v, PORTB, 4);
		adc_diff = djx16_hw_adc[adc_chan] - v;

		if ((adc_diff < -1) || (adc_diff > 1)) {
			djx16_hw_adc[adc_chan] = v;
			djx16_hw_adc_dirty |= (1<<adc_chan);
		}
	}

	adc_chan++;					/* prepare next ch */
	if (adc_chan >= DJX16_ADC_LENGTH)		/* wrap to zero */
		adc_chan = 0;

	v = ((adc_chan & 0x01) << 5) |			/* bit0 -> D5 */
	    ((adc_chan & 0x02) << 3) |			/* bit1 -> D4 */
	    ((adc_chan & 0x04) << 1) |			/* bit2 -> D3 */
	    0x07;  /* D0 = \WR, D1 = \E(masters), D2 = \E(chans) */

	if (adc_chan & 0x08)				/* 0..7=ch 8..x=mst */
		v &= ~_BV(1);				/* \E on master mux */
	else
		v &= ~_BV(2);				/* \E on chan mux */

	DJX16_LATCH_ADC_MUX(v);				/* \WR = 1 */

	if (count & 0x01) {				/* 2nd phase */
		DJX16_LATCH_ADC_MUX(v & ~_BV(0));	/* \WR = 0 */
		DJX16_LATCH_ADC_MUX(v);			/* \WR = 1 */
	}

skip_adc:
	DDRA = 0x00;
	PORTA = 0x00;
}

uint32_t
djx16_hw_get_tick_ctr(void)
{
	uint32_t v;
	cli();
	v = djx16_hw_tick_ctr;
	sei();
	return v;
}

void
djx16_hw_init(void)
{

	DDRA  = 0x00;

	DDRB  = 0x5f;  /* PB0..4 & 6 */
	PORTB = 0x14;  /* \RD ADC and \OE KeyMtx high, else low */

	DDRC  = 0x00; /* A8..A15, not used */
	PORTC = 0xff; /* weak pullup */

	DDRD  = 0x38; /* 7-segment latch enables, channel LEDs */
	PORTD = 0x00;

	DDRE  = 0x01; /* PB1 = ALE, pull down */
	PORTE = 0x00;

	/* configure timer/counter0 */
	/* 8-bit counter, fCLK = 8 MHz, prescale 64 -> 488 Hz Interrupt */
	TCCR0 = _BV(CS01)|_BV(CS00);

	/* enable timer overflow interrupt */
	TIMSK |= _BV(TOIE0);

}
