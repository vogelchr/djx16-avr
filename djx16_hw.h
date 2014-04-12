#ifndef DJX16_HW_H
#define DJX16_HE_H

#include <avr/io.h>
#include <avr/interrupt.h>

/* get a tick-counter that is increased with about 500 Hz */
extern uint32_t djx16_hw_tick_ctr(void);
extern uint8_t djx16_hw_adc[16];


#endif
