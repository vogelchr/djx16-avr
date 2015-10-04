#include "djx16_ui.h"
#include "djx16_key.h"
#include "djx16_output.h"
#include "djx16_led.h"
#include "djx16_adc.h"

#include <avr/pgmspace.h>

uint8_t djx16_ui_state;

/* list of key handlers */
typedef char (PROGMEM * key_handler_ptr_t)(uint8_t);
struct djx16_ui_handler_entry {
	uint8_t ui_mode_mask;
	uint8_t keycode;
	key_handler_ptr_t handler_fct;
};

#define KEY_HANDLER(name) static char name(uint8_t key)

/* *************************************************************************
   ** Key Handlers                                                        **
   ************************************************************************* */

KEY_HANDLER(djx16_ui_handle_stby) {
	(void)key;
	djx16_led_indicator(DJX16_LED_STANDBY, LED_TOGGLE);
	return 0;
}

KEY_HANDLER(djx16_ui_handle_pat_updown) {
	if (key == DJX16_KEY_PUP) {
		/* up */
	} else {
		/* down */
	}
	return 0;
}

KEY_HANDLER(djx16_ui_handle_step_updown) {
	if (key == DJX16_KEY_SUP) {
		/* up */
	} else {
		/* down */
	}
	return 0;
}

KEY_HANDLER(djx16_ui_handle_flash) {
	djx16_led_7seg_hex(0, 2);
	djx16_led_7seg_hex(2, (key & 0xf0) >> 4);
	djx16_led_7seg_hex(3, key & 0x0f);
}

KEY_HANDLER(djx16_ui_handle_latch) {
	djx16_led_7seg_hex(0, 1);
	djx16_led_7seg_hex(2, (key & 0xf0) >> 4);
	djx16_led_7seg_hex(3, key & 0x0f);
}

/* key handlers */
static const struct djx16_ui_handler_entry PROGMEM djx16_ui_handler_table[] =
{
	{ 0x00, DJX16_KEY_PUP,         & djx16_ui_handle_pat_updown },
	{ 0x00, DJX16_KEY_PDOWN,       & djx16_ui_handle_pat_updown },
	{ 0x00, DJX16_KEY_SUP,         & djx16_ui_handle_step_updown },
	{ 0x00, DJX16_KEY_SDOWN,       & djx16_ui_handle_step_updown },
	{ 0x00, DJX16_KEY_STANDBY,     & djx16_ui_handle_stby },
	{ 0x00, DJX16_KEY_FLASH_ANY_A, & djx16_ui_handle_flash },
	{ 0x00, DJX16_KEY_FLASH_ANY_B, & djx16_ui_handle_flash },
	{ 0x00, DJX16_KEY_LATCH_ANY,   & djx16_ui_handle_latch },
	{ 0x00, 0x00,              0 } /* --- eof --- */
};

void
djx16_ui_init(void)
{

}

#define NELEMENS(x)  (sizeof((x)) / sizeof((x)[0]))

void
djx16_ui_run(void)
{
	uint8_t keycode, handler_key, adc_val, i;
	uint16_t dirty_adc;
	const struct djx16_ui_handler_entry *p;
	key_handler_ptr_t hdlr;

	/* check all dirty ADCs */
	dirty_adc = djx16_adc_get_dirty();
	for (i=0; i<DJX16_ADC_LENGTH; i++) {
		if (dirty_adc & (1<<i)) {
			adc_val = djx16_adc_get(i);
			djx16_led_7seg_hex(0, i);
			djx16_led_7seg_hex(2, (adc_val & 0xf0) >> 4);
			djx16_led_7seg_hex(3, adc_val & 0x0f);
			break;
		}
	}

	keycode = djx16_key_get();
	if (keycode == DJX16_KEY_NONE)
		return;

	/* iterate over key handler list */
	p = djx16_ui_handler_table;
	while (1) {
		handler_key = pgm_read_byte(&(p->keycode));
		hdlr = (key_handler_ptr_t)pgm_read_word(&(p->handler_fct));

		if (!hdlr) /* list ends with NULL handler */
			break;

		/* flash, latch occupy one logica "row" of buttons,
		   so we can simplify the handlers for them */
		if (handler_key & DJX16_KEY_WHOLEROW) {
			handler_key &= ~DJX16_KEY_WHOLEROW;
			handler_key |= (keycode & 0x07);
		}

		if (handler_key == keycode)
			hdlr(keycode);
		p++;
	}
}
