#ifndef JOYPAD_SWITCH_H
#define JOYPAD_SWITCH_H

#include "core/input/input.h"
#include "switch_wrapper.h"
#include <SDL2/SDL.h>

#define JOYPADS_MAX 8

class JoypadSwitch {
public:
	JoypadSwitch(Input *in);
	~JoypadSwitch();
	void process_joypads();
private:
	Input *input;
	PadState pads[JOYPADS_MAX];
	SDL_GameController *controllers[JOYPADS_MAX];
	int button_count = 0;
	bool use_sdl = true; // Use SDL2 by default
	
	void process_sdl_controllers();
	void process_libnx_pads();
};

#endif //JOYPAD_SWITCH_H
