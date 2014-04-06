#ifndef DJX16_KEY_H
#define DJX16_KEY_H

/* ISR detected press of a key, stored here */
extern char djx16_key_row;
extern char djx16_key_pressed;

extern char
djx16_key_scan(void);

#endif
