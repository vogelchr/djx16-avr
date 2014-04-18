#include "djx16_ui.h"
#include "djx16_key.h"
#include "djx16_output.h"
#include "djx16_led.h"

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

/* key handlers */
static const struct djx16_ui_handler_entry PROGMEM djx16_ui_handler_table[] =
{
	{ 0x00, DJX16_KEY_PUP,     & djx16_ui_handle_pat_updown },
	{ 0x00, DJX16_KEY_PDOWN,   & djx16_ui_handle_pat_updown },
	{ 0x00, DJX16_KEY_SUP,     & djx16_ui_handle_step_updown },
	{ 0x00, DJX16_KEY_SDOWN,   & djx16_ui_handle_step_updown },
	{ 0x00, DJX16_KEY_STANDBY, & djx16_ui_handle_stby },
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
	uint8_t keycode, handler_key;
	const struct djx16_ui_handler_entry *p;
	key_handler_ptr_t hdlr;

	keycode = djx16_key_get();
	if (keycode == DJX16_KEY_NONE)
		return;

	p = djx16_ui_handler_table;
	while (1) {
		handler_key = pgm_read_byte(&(p->keycode));
		hdlr = (key_handler_ptr_t)pgm_read_word(&(p->handler_fct));

		if (!hdlr)
			break;

		if (handler_key == keycode)
			hdlr(keycode);
		p++;
	}
}

