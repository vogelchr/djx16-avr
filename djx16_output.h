#ifndef DJX16_OUTPUT_H
#define DJX16_OUTPUT_H

#include <avr/io.h>

#define DJX16_OUTPUT_NCHANS 16

extern uint8_t djx16_output[DJX16_OUTPUT_NCHANS];
extern uint8_t djx16_output_state;

extern void djx16_output_init(void);

#endif
