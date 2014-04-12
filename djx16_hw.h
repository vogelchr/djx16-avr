#ifndef DJX16_HW_H
#define DJX16_HE_H

#include "djx16_led.h"
#include "djx16_adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

/* get a tick-counter that is increased with about 500 Hz */
extern uint32_t djx16_hw_tick_ctr(void);

/* multiplexed LEDs data */
extern uint8_t djx16_hw_led_buf[DJX16_LED_LENGTH];

/* master channel LEDs data */ 
extern uint8_t djx16_hw_led_masters[DJX16_N_MASTER_GROUPS*DJX16_N_MASTER_INTENS];

/* multiplexed ADC data */
extern uint8_t djx16_hw_adc[DJX16_ADC_LENGTH];

/* ISR detected press of a key, stored here */
extern uint8_t djx16_hw_key_row;
extern uint8_t djx16_hw_key_sense;

extern uint8_t djx16_key_flash1_8;
extern uint8_t djx16_key_flash9_16;


#endif
