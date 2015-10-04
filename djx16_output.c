#include "djx16_output.h"

#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t          djx16_output[DJX16_OUTPUT_NCHANS] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
volatile uint8_t djx16_output_state;

#define DMX_BAUD             250000ULL
#define UBRR_U2X_ON(baud)   (F_CPU/(8*(baud))-1)
#define UBRR_U2X_OFF(baud)  (F_CPU/(16*(baud))-1)

static const uint16_t ubrr_val_dmx = UBRR_U2X_ON(DMX_BAUD);
static const uint16_t ubrr_val_brk = UBRR_U2X_ON(9600); /* ~100us / bit */

SIGNAL(USART0_TXC_vect){ /* data register empty */
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
			UDR0 = 0xff; /* 1 stop bit ~ 100us, 8data + 2 stop pause */
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
