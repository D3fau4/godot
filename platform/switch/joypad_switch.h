#ifndef JOYPAD_SWITCH_H
#define JOYPAD_SWITCH_H

#include "core/input/input.h"
#include "switch_wrapper.h"

#define JOYPADS_MAX 8

class JoypadSwitch {
public:
	JoypadSwitch(Input *in);
	~JoypadSwitch();
	void process_joypads();
private:
	Input *input;
	PadState pads[JOYPADS_MAX];
	int button_count = 0;
};

#endif //JOYPAD_SWITCH_H
