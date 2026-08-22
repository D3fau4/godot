/**************************************************************************/
/*  joypad_nx.cpp                                                         */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "joypad_nx.h"

static const u64 pad_ids[JoypadNX::JOYPADS_MAX] = {
	(1ULL << HidNpadIdType_No1) | (1ULL << HidNpadIdType_Handheld),
	(1ULL << HidNpadIdType_No2),
	(1ULL << HidNpadIdType_No3),
	(1ULL << HidNpadIdType_No4),
	(1ULL << HidNpadIdType_No5),
	(1ULL << HidNpadIdType_No6),
	(1ULL << HidNpadIdType_No7),
	(1ULL << HidNpadIdType_No8),
};

struct NXButtonMapping {
	HidNpadButton nx_button;
	JoyButton godot_button;
};

static const NXButtonMapping button_mapping[] = {
	{ HidNpadButton_B, JoyButton::A },
	{ HidNpadButton_A, JoyButton::B },
	{ HidNpadButton_Y, JoyButton::X },
	{ HidNpadButton_X, JoyButton::Y },
	{ HidNpadButton_Minus, JoyButton::BACK },
	{ HidNpadButton_Plus, JoyButton::START },
	{ HidNpadButton_StickL, JoyButton::LEFT_STICK },
	{ HidNpadButton_StickR, JoyButton::RIGHT_STICK },
	{ HidNpadButton_L, JoyButton::LEFT_SHOULDER },
	{ HidNpadButton_R, JoyButton::RIGHT_SHOULDER },
	{ HidNpadButton_Up, JoyButton::DPAD_UP },
	{ HidNpadButton_Down, JoyButton::DPAD_DOWN },
	{ HidNpadButton_Left, JoyButton::DPAD_LEFT },
	{ HidNpadButton_Right, JoyButton::DPAD_RIGHT },
};

JoypadNX::JoypadNX(Input *p_input) {
	input = p_input;

	padConfigureInput(JOYPADS_MAX, HidNpadStyleSet_NpadStandard);

	for (int i = 0; i < JOYPADS_MAX; i++) {
		padInitializeWithMask(&pads[i], pad_ids[i]);
	}
}

JoypadNX::~JoypadNX() {
}

void JoypadNX::process_joypads() {
	for (int index = 0; index < JOYPADS_MAX; index++) {
		padUpdate(&pads[index]);

		bool is_connected = padIsConnected(&pads[index]);
		if (is_connected != connected[index]) {
			connected[index] = is_connected;
			input->joy_connection_changed(index, is_connected, "Nintendo Switch Controller");
		}

		if (!is_connected) {
			continue;
		}

		const u64 buttons_down = padGetButtonsDown(&pads[index]);
		const u64 buttons_up = padGetButtonsUp(&pads[index]);

		if (buttons_down != 0 || buttons_up != 0) {
			for (const NXButtonMapping &mapping : button_mapping) {
				if (buttons_down & mapping.nx_button) {
					input->joy_button(index, mapping.godot_button, true);
				}
				if (buttons_up & mapping.nx_button) {
					input->joy_button(index, mapping.godot_button, false);
				}
			}

			if (buttons_down & HidNpadButton_ZL) {
				input->joy_axis(index, JoyAxis::TRIGGER_LEFT, 1.0f);
			}
			if (buttons_up & HidNpadButton_ZL) {
				input->joy_axis(index, JoyAxis::TRIGGER_LEFT, 0.0f);
			}
			if (buttons_down & HidNpadButton_ZR) {
				input->joy_axis(index, JoyAxis::TRIGGER_RIGHT, 1.0f);
			}
			if (buttons_up & HidNpadButton_ZR) {
				input->joy_axis(index, JoyAxis::TRIGGER_RIGHT, 0.0f);
			}
		}

		const HidAnalogStickState stick_l = padGetStickPos(&pads[index], 0);
		const HidAnalogStickState stick_r = padGetStickPos(&pads[index], 1);

		input->joy_axis(index, JoyAxis::LEFT_X, (float)stick_l.x / 32767.0f);
		input->joy_axis(index, JoyAxis::LEFT_Y, (float)-stick_l.y / 32767.0f);
		input->joy_axis(index, JoyAxis::RIGHT_X, (float)stick_r.x / 32767.0f);
		input->joy_axis(index, JoyAxis::RIGHT_Y, (float)-stick_r.y / 32767.0f);
	}
}
