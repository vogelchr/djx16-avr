#include "djx16_output.h"

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t djx16_output[DJX16_OUTPUT_NCHANS] = {1,2,3,4,5,6,7,8,9,10,11,12,13};

#define DMX_BAUD             250000ULL
#define UBRR_U2X_ON(baud)   ((unsigned long long)F_CPU/(8*(unsigned long long)(baud))-1)
#define UBRR_U2X_OFF(baud)  ((unsigned long long)F_CPU/(16*(unsigned long long)(baud))-1)

static const uint16_t ubrr_val_dmx = UBRR_U2X_ON(DMX_BAUD);
/* pausing for ~100us + ~100us delay: we send <start><5*0><5*1><2*stop> at
   a baudrate of 50000 bps (20us per bit) */
static const uint16_t ubrr_val_brk = UBRR_U2X_ON(50000);

SIGNAL(USART0_TXC_vect){ /* data register empty */
	static uint8_t djx16_output_state;

	uint8_t tmp = djx16_output_state;
	/* if we have finished transmitting all data, turn off transmitter
	   and transmit complete interrupt: timer interrupt will send a break
	   and reenable transmission. See djx16_hw, label check_dmx_tx */
	if (tmp < DJX16_OUTPUT_NCHANS) {
		UDR0 = djx16_output[tmp++];
	} else { /* generate break */
		if (tmp == DJX16_OUTPUT_NCHANS) {
			UBRR0H = (ubrr_val_brk >> 8) & 0x0f; /* UBRR011 ... UBRR08 */
			UBRR0L =  ubrr_val_brk       & 0xff; /* UBRR07  ... UBRR00 */
			/* at 50000 bps, 20us/bit:
			   <--5*20=100us ----><--6*20=120us---------->
			   <start><0><0><0><0><1><1><1><1><stop><stop>
			*/
			UDR0 = 0xf0;
			tmp++;
		} else {
			UBRR0H = (ubrr_val_dmx >> 8) & 0x0f; /* UBRR011 ... UBRR08 */
			UBRR0L =  ubrr_val_dmx       & 0xff; /* UBRR07  ... UBRR00 */
			UDR0 = 0x55;
			tmp = 0;
		}
	}
	djx16_output_state = tmp;
}

/* DMX out is on port D1 (Pin 11, TxD0) */
void
djx16_output_init(void)
{
	UBRR0H = (ubrr_val_dmx >> 8) & 0x0f; /* UBRR011 ... UBRR08 */
	UBRR0L =  ubrr_val_dmx       & 0xff; /* UBRR07  ... UBRR00 */

	 /* enable transmission for DMX, enable Tx data register empty irq */
	UCSR0A = _BV(U2X0);
	UCSR0B = _BV(TXEN0)|_BV(TXCIE0);
	UCSR0C = _BV(URSEL0)|_BV(USBS0)|_BV(UCSZ01)|_BV(UCSZ00); /* 8N2 */

	UDR0 = 0x00;
}

void
djx16_output_set(uint8_t ch, uint8_t val){
	if (ch >= DJX16_OUTPUT_NCHANS)
		return;
	djx16_output[ch]=val;
}
