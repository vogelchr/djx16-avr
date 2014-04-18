#include "djx16_adc.h"
#include "djx16_hw.h"

#include <avr/io.h>

uint8_t
djx16_adc_get(enum djx16_adc_pos pos)
{
	return djx16_hw_adc[pos];
}
