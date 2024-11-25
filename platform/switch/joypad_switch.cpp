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

	// TODO: n players?
	padConfigureInput(1, HidNpadStyleSet_NpadStandard);

	this->button_count = sizeof(pad_mapping) / sizeof(*pad_mapping);
	for (int i = 0; i < JOYPADS_MAX; i++) {
		padInitializeWithMask(&pads[i], pad_ids[i]);
	}
}

JoypadSwitch::~JoypadSwitch() {
}

void JoypadSwitch::process_joypads() {
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
