#ifndef DJX16_ADC_H
#define DJX16_ADC_H

#include <avr/io.h>

enum djx16_adc_pos {
	DJX16_ADC_CH0,
	DJX16_ADC_CH1,
	DJX16_ADC_CH2,
	DJX16_ADC_CH3,
	DJX16_ADC_CH4,
	DJX16_ADC_CH5,
	DJX16_ADC_CH6,
	DJX16_ADC_CH7,
	DJX16_ADC_MASTER,
	DJX16_ADC_LEVEL,	/* inverted! */
	DJX16_ADC_SPEED,	/* inverted! */
	DJX16_ADC_LENGTH
};

extern uint8_t djx16_adc_get(enum djx16_adc_pos pos);

#endif
