#include "djx16_led.h"
#include "djx16_key.h"
#include "djx16_adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

int
main(void)
{
	uint16_t i;
	uint8_t c;
	uint8_t adc,key,ch,sel;
	uint8_t indicator;

	djx16_hw_init();
	djx16_led_init();

	sei(); /* enable interrupts */

	ch = 0;
	indicator=0;
	ch=0;

	while (1) {
		i=0;
		do { } while(++i);

		if ((++c) > 7)
			c = 0;

		if (ch < DJX16_ADC_LENGTH) {
			adc = djx16_adc_get(ch);

			djx16_led_7seg_hex(2, adc >> 4);
			djx16_led_7seg_hex(3, adc & 0x0f);
		}

		for (i=0; i<DJX16_ADC_LENGTH; i++) {
			adc = djx16_adc_get(i);
			djx16_led_master(i, adc);
		}


		key = djx16_key_get();

		sel = (DJX16_KEY_ROW(key) << 3) + DJX16_KEY_COL(key);

		if (key != DJX16_KEY_NONE && sel < DJX16_ADC_LENGTH) {
			ch = sel;
			djx16_led_7seg_hex(0, sel );
			djx16_led_7seg(1, 0);
		}

		switch(key) {
		case DJX16_KEY_PUP:
		case DJX16_KEY_PDOWN:
			if (key == DJX16_KEY_PUP &&
			    indicator < DJX16_LED_NUM_INDICATORS-1)
				indicator++;
			if (key == DJX16_KEY_PDOWN && indicator > 0)
				indicator--;

			ch = DJX16_ADC_LENGTH; /* turn off adc disp */

			djx16_led_7seg_hex(0, indicator >> 4 );
			djx16_led_7seg_hex(1, indicator & 0x0f);

			break;
		case DJX16_KEY_LOOP:
			djx16_led_indicator(indicator, LED_ON);
			break;
		case DJX16_KEY_CHASE:
			djx16_led_indicator(indicator, LED_OFF);
			break;
		case DJX16_KEY_TAP:
			djx16_led_indicator(indicator, LED_TOGGLE);
			break;
		}

	}
}
