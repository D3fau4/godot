/**************************************************************************/
/*  switch_audio.h                                                        */
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

#ifndef SWITCH_AUDIO_H
#define SWITCH_AUDIO_H

#ifdef __SWITCH__

#include "servers/audio_server.h"
#include <SDL2/SDL.h>

class AudioDriverSDL2 : public AudioDriver {
private:
	SDL_AudioDeviceID device_id = 0;
	SDL_AudioSpec audio_spec;
	bool active = false;
	
	static void audio_callback(void *userdata, Uint8 *stream, int len);

public:
	const char *get_name() const override {
		return "SDL2";
	}

	Error init() override;
	void start() override;
	int get_mix_rate() const override;
	SpeakerMode get_speaker_mode() const override;
	void lock() override;
	void unlock() override;
	void finish() override;

	AudioDriverSDL2();
	~AudioDriverSDL2();
};

#endif // __SWITCH__

#endif // SWITCH_AUDIO_H
