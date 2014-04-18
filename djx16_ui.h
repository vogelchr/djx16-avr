#ifndef DJX16_UI_H
#define DJX16_UI_H

#include <avr/io.h>

enum djx16_ui_state {
	DJX16_UI_MANUAL,
	DJX16_UI_SCENE,
	DJX16_UI_SCENE_EDIT
};

extern void djx16_ui_init(void);
extern void djx16_ui_run(void);

#endif
