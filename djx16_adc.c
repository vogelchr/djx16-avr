#include "djx16_adc.h"
#include "djx16_hw.h"

#include <avr/io.h>

uint8_t
djx16_adc_get(enum djx16_adc_pos pos)
{
	djx16_hw_adc_dirty &= ~(1<<pos);
	return djx16_hw_adc[pos];
}

uint16_t
djx16_adc_get_dirty()
{
	return djx16_hw_adc_dirty;
}
