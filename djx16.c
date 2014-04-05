#include "djx16_hw.h"
#include "djx16_led.h"

#include <avr/io.h>
#include <avr/interrupt.h>

SIGNAL(TIMER0_OVF_vect){ /* timer/counter 0 overflow */
	djx16_led_update();
}

int
main(void)
{
	uint16_t ctr,i;

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
		ctr++;

		djx16_led_set_7seg_hex(0, 0x0f & (ctr >> 12));
		djx16_led_set_7seg_hex(1, 0x0f & (ctr >>  8));
		djx16_led_set_7seg_hex(2, 0x0f & (ctr >>  4));
		djx16_led_set_7seg_hex(3, 0x0f &  ctr       );
	}
}
