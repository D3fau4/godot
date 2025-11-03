/**************************************************************************/
/*  switch_audio.cpp                                                      */
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

#ifdef __SWITCH__

#include "switch_audio.h"
#include "core/config/project_settings.h"
#include "core/io/print_string.h"

AudioDriverSDL2::AudioDriverSDL2() {
}

AudioDriverSDL2::~AudioDriverSDL2() {
	finish();
}

void AudioDriverSDL2::audio_callback(void *userdata, Uint8 *stream, int len) {
	AudioDriverSDL2 *ad = (AudioDriverSDL2 *)userdata;
	
	if (!ad->active) {
		memset(stream, 0, len);
		return;
	}
	
	// Calculate number of frames
	int frames = len / (2 * sizeof(int16_t)); // 2 channels, 16-bit samples
	
	// Fill with audio data from Godot's audio server
	int32_t *buf = (int32_t *)stream;
	ad->audio_server_process(frames, buf);
}

Error AudioDriverSDL2::init() {
	// Initialize SDL audio subsystem if not already initialized
	if (!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO)) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
			ERR_PRINT(vformat("Failed to initialize SDL audio: %s", SDL_GetError()));
			return ERR_CANT_OPEN;
		}
	}
	
	// Configure audio specification
	SDL_AudioSpec desired;
	SDL_zero(desired);
	desired.freq = 48000; // 48kHz sample rate
	desired.format = AUDIO_S16LSB; // 16-bit signed little-endian
	desired.channels = 2; // Stereo
	desired.samples = 1024; // Buffer size
	desired.callback = audio_callback;
	desired.userdata = this;
	
	// Open audio device
	device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, &audio_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	
	if (device_id == 0) {
		ERR_PRINT(vformat("Failed to open SDL audio device: %s", SDL_GetError()));
		return ERR_CANT_OPEN;
	}
	
	print_line(vformat("SDL2 Audio: Initialized - %d Hz, %d channels, buffer: %d samples", 
		audio_spec.freq, audio_spec.channels, audio_spec.samples));
	
	return OK;
}

void AudioDriverSDL2::start() {
	if (device_id > 0) {
		active = true;
		SDL_PauseAudioDevice(device_id, 0); // Start audio playback
		print_line("SDL2 Audio: Started");
	}
}

int AudioDriverSDL2::get_mix_rate() const {
	return audio_spec.freq;
}

AudioDriver::SpeakerMode AudioDriverSDL2::get_speaker_mode() const {
	return SPEAKER_MODE_STEREO;
}

void AudioDriverSDL2::lock() {
	if (device_id > 0) {
		SDL_LockAudioDevice(device_id);
	}
}

void AudioDriverSDL2::unlock() {
	if (device_id > 0) {
		SDL_UnlockAudioDevice(device_id);
	}
}

void AudioDriverSDL2::finish() {
	if (device_id > 0) {
		active = false;
		SDL_CloseAudioDevice(device_id);
		device_id = 0;
		print_line("SDL2 Audio: Finished");
	}
}

#endif // __SWITCH__
