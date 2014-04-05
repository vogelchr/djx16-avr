#include "djx16_hw.h"

static inline void
djx16_latch_sleep(void)
{
	asm volatile ("nop;\n\t nop;\n\t nop;\n\t nop;\n\t nop;\n\t nop;");
}

void
djx16_latch(enum DJX16_LATCH latch, unsigned char value)
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
		PORTB |= _BV(2);
		djx16_latch_sleep();
		PORTB &= ~_BV(2);
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

