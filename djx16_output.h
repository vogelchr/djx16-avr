#ifndef DJX16_OUTPUT_H
#define DJX16_OUTPUT_H

#include <avr/io.h>

#define DJX16_OUTPUT_NCHANS 16

extern void djx16_output_init(void);
extern void djx16_output_set(uint8_t ch, uint8_t val);

#endif
