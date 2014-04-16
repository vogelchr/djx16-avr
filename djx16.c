#include "djx16_led.h"
#include "djx16_key.h"
#include "djx16_adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

int
main(void)
{
	uint16_t i;
	uint8_t v,c;
	uint8_t adc,key,ch,sel;

	ch = 0;

	DDRA  = 0x00;
	PORTA = 0xff; /* pullup */

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

	djx16_led_init();
	sei(); /* enable interrupts */

	while(1){
		i=0;
		do { } while(++i);

		if ((++c) > 7)
			c = 0;

		adc = djx16_adc_get(ch);

		djx16_led_7seg_hex(2, adc >> 4);
		djx16_led_7seg_hex(3, adc & 0x0f);

		for (i=0; i<DJX16_ADC_LENGTH; i++) {
			adc = djx16_adc_get(i);
			djx16_led_master(i, adc);
		}


		key = djx16_key_get();

		if (key != DJX16_KEY_NONE) {
			sel = (DJX16_KEY_ROW(key) << 3) + DJX16_KEY_COL(key);

			if (sel <= DJX16_ADC_LENGTH) {
				ch = sel;
				djx16_led_7seg_hex(0, sel );
				djx16_led_7seg(1, 0);
			} else {
				djx16_led_7seg_hex(0, key >> 4);
				djx16_led_7seg_hex(1, key & 0x0f);
			}
		}

#if 0
		cli();
		if (djx16_hw_key_sense) {
			djx16_led_7seg_hex(0, djx16_hw_key_row);
			djx16_led_7seg_hex(2, djx16_hw_key_sense >> 4);
			djx16_led_7seg_hex(3, djx16_hw_key_sense & 0x0f);
			djx16_hw_key_sense = djx16_hw_key_row = 0;
		}
		sei();
#endif
	}
}
