#include "djx16_adc.h"
#include "djx16_hw.h"
#include "djx16_key.h"
#include "djx16_led.h"
#include "djx16_output.h"
#include "djx16_ui.h"

#include <avr/io.h>
#include <avr/interrupt.h>

int
main(void)
{
	djx16_hw_init();
	djx16_led_init();
	djx16_output_init();
	djx16_ui_init();

	sei(); /* enable interrupts */

	while (1) {
		djx16_ui_run();
	}

}
