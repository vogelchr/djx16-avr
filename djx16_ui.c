#include "djx16_ui.h"
#include "djx16_key.h"
#include "djx16_output.h"
#include "djx16_led.h"

#include <avr/pgmspace.h>

uint8_t djx16_ui_state;

/* list of key handlers */
typedef char (*const key_handler_ptr_t)(char);
struct djx16_ui_handler_entry {
	uint8_t ui_mode_mask;
	uint8_t keycode;
	key_handler_ptr_t handler_fct;
};

static char
djx16_ui_handle_stby(char tmp)
{
	djx16_led_indicator(DJX16_LED_STANDBY, LED_TOGGLE);
	return 0;
}

/* key handlers */
const struct djx16_ui_handler_entry PROGMEM djx16_ui_handler_table[] =
{
	{ 0x00, DJX16_KEY_STANDBY, djx16_ui_handle_stby },
	{ 0x00, 0x00,              0 } /* --- eof --- */
};

void
djx16_ui_init(void)
{

}
