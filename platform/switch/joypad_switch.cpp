#include "joypad_switch.h"
#include "core/os/os.h"

static u64 pad_ids[JOYPADS_MAX] = {
	(1ull << HidNpadIdType_No1) | (1ull << HidNpadIdType_Handheld),
	(1ull << HidNpadIdType_No2),
	(1ull << HidNpadIdType_No3),
	(1ull << HidNpadIdType_No4),
	(1ull << HidNpadIdType_No5),
	(1ull << HidNpadIdType_No6),
	(1ull << HidNpadIdType_No7),
	(1ull << HidNpadIdType_No8)
};

// from editor "Project Settings > Input Map"
static const HidNpadButton pad_mapping[] = {
	HidNpadButton_B, HidNpadButton_A, HidNpadButton_Y, HidNpadButton_X,
	HidNpadButton_L, HidNpadButton_R, HidNpadButton_ZL, HidNpadButton_ZR,
	HidNpadButton_StickL, HidNpadButton_StickR,
	HidNpadButton_Minus, HidNpadButton_Plus,
	HidNpadButton_Up, HidNpadButton_Down, HidNpadButton_Left, HidNpadButton_Right
};

JoypadSwitch::JoypadSwitch(Input *in) {
	this->input = in;

	// Initialize SDL2 controllers
	for (int i = 0; i < JOYPADS_MAX; i++) {
		controllers[i] = nullptr;
	}
	
	if (use_sdl) {
		// Open SDL2 game controllers
		int num_joysticks = SDL_NumJoysticks();
		print_line(vformat("SDL2: Found %d joysticks", num_joysticks));
		
		for (int i = 0; i < num_joysticks && i < JOYPADS_MAX; i++) {
			if (SDL_IsGameController(i)) {
				controllers[i] = SDL_GameControllerOpen(i);
				if (controllers[i]) {
					const char *name = SDL_GameControllerName(controllers[i]);
					print_line(vformat("SDL2: Opened controller %d: %s", i, name ? name : "Unknown"));
					input->joy_connection_changed(i, true, String(name ? name : "Joy-Con"), "");
				}
			}
		}
	} else {
		// Fallback to libnx
		padConfigureInput(1, HidNpadStyleSet_NpadStandard);
		this->button_count = sizeof(pad_mapping) / sizeof(*pad_mapping);
		for (int i = 0; i < JOYPADS_MAX; i++) {
			padInitializeWithMask(&pads[i], pad_ids[i]);
		}
	}
}

JoypadSwitch::~JoypadSwitch() {
	// Close SDL2 controllers
	for (int i = 0; i < JOYPADS_MAX; i++) {
		if (controllers[i]) {
			SDL_GameControllerClose(controllers[i]);
			controllers[i] = nullptr;
		}
	}
}

void JoypadSwitch::process_joypads() {
	if (use_sdl) {
		process_sdl_controllers();
	} else {
		process_libnx_pads();
	}
}

void JoypadSwitch::process_sdl_controllers() {
	for (int i = 0; i < JOYPADS_MAX; i++) {
		if (!controllers[i]) {
			continue;
		}
		
		// Check if controller is still connected
		if (!SDL_GameControllerGetAttached(controllers[i])) {
			input->joy_connection_changed(i, false, "", "");
			SDL_GameControllerClose(controllers[i]);
			controllers[i] = nullptr;
			continue;
		}
		
		// Process analog sticks
		float left_x = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
		float left_y = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
		float right_x = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
		float right_y = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;
		
		input->joy_axis(i, JoyAxis::LEFT_X, left_x);
		input->joy_axis(i, JoyAxis::LEFT_Y, left_y);
		input->joy_axis(i, JoyAxis::RIGHT_X, right_x);
		input->joy_axis(i, JoyAxis::RIGHT_Y, right_y);
		
		// Process triggers
		float trigger_l = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;
		float trigger_r = SDL_GameControllerGetAxis(controllers[i], SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
		input->joy_axis(i, JoyAxis::TRIGGER_LEFT, trigger_l);
		input->joy_axis(i, JoyAxis::TRIGGER_RIGHT, trigger_r);
		
		// Process buttons (mapping to Godot's JoyButton enum)
		static const struct {
			SDL_GameControllerButton sdl_button;
			JoyButton godot_button;
		} button_map[] = {
			{ SDL_CONTROLLER_BUTTON_A, JoyButton::A },
			{ SDL_CONTROLLER_BUTTON_B, JoyButton::B },
			{ SDL_CONTROLLER_BUTTON_X, JoyButton::X },
			{ SDL_CONTROLLER_BUTTON_Y, JoyButton::Y },
			{ SDL_CONTROLLER_BUTTON_BACK, JoyButton::BACK },
			{ SDL_CONTROLLER_BUTTON_GUIDE, JoyButton::GUIDE },
			{ SDL_CONTROLLER_BUTTON_START, JoyButton::START },
			{ SDL_CONTROLLER_BUTTON_LEFTSTICK, JoyButton::LEFT_STICK },
			{ SDL_CONTROLLER_BUTTON_RIGHTSTICK, JoyButton::RIGHT_STICK },
			{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER, JoyButton::LEFT_SHOULDER },
			{ SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, JoyButton::RIGHT_SHOULDER },
			{ SDL_CONTROLLER_BUTTON_DPAD_UP, JoyButton::DPAD_UP },
			{ SDL_CONTROLLER_BUTTON_DPAD_DOWN, JoyButton::DPAD_DOWN },
			{ SDL_CONTROLLER_BUTTON_DPAD_LEFT, JoyButton::DPAD_LEFT },
			{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT, JoyButton::DPAD_RIGHT },
		};
		
		for (const auto &mapping : button_map) {
			bool pressed = SDL_GameControllerGetButton(controllers[i], mapping.sdl_button);
			input->joy_button(i, mapping.godot_button, pressed);
		}
	}
}

void JoypadSwitch::process_libnx_pads() {
	for (int index = 0; index < JOYPADS_MAX; index++) {
		padUpdate(&pads[index]);

		HidAnalogStickState l_stick = padGetStickPos(&pads[index], 0);
		HidAnalogStickState r_stick = padGetStickPos(&pads[index], 1);

		// Axes
		input->joy_axis(index, JoyAxis::LEFT_X, (float)(l_stick.x / 32767.0f));
		input->joy_axis(index, JoyAxis::LEFT_Y, (float)(-l_stick.y / 32767.0f));
		input->joy_axis(index, JoyAxis::RIGHT_X, (float)(r_stick.x / 32767.0f));
		input->joy_axis(index, JoyAxis::RIGHT_Y, (float)(-r_stick.y / 32767.0f));

		// Buttons
		u64 buttons_up = padGetButtonsUp(&pads[index]);
		u64 buttons_down = padGetButtonsDown(&pads[index]);

		if (buttons_up != 0 || buttons_down != 0) {
			for (int i = 0; i < button_count; i++) {
				if (buttons_up & pad_mapping[i]) {
					input->joy_button(index, (JoyButton)i, false);
				}
				if (buttons_down & pad_mapping[i]) {
					input->joy_button(index, (JoyButton)i, true);
				}
			}
		}
	}
}
