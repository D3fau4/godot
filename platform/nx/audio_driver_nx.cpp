/**************************************************************************/
/*  audio_driver_nx.cpp                                                   */
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

#include "audio_driver_nx.h"

#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/os/os.h"

#include <malloc.h>
#include <string.h>

static const AudioRendererConfig audren_config = {
	.output_rate = AudioRendererOutputRate_48kHz,
	.num_voices = 24,
	.num_effects = 0,
	.num_sinks = 1,
	.num_mix_objs = 1,
	.num_mix_buffers = 2,
};

Error AudioDriverNX::init_device() {
	mix_rate = _get_configured_mix_rate();
	channels = 2;
	speaker_mode = SPEAKER_MODE_STEREO;

	int latency = Engine::get_singleton()->get_audio_output_latency();
	buffer_frames = closest_power_of_2(latency * mix_rate / 1000);

	samples_in.resize(buffer_frames * channels);
	samples_out.resize(buffer_frames * channels);

	ERR_FAIL_COND_V_MSG(R_FAILED(audrenInitialize(&audren_config)), ERR_CANT_OPEN, "NX: audrenInitialize failed.");

	if (R_FAILED(audrvCreate(&audren_driver, &audren_config, channels))) {
		audrenExit();
		ERR_FAIL_V_MSG(ERR_CANT_OPEN, "NX: audrvCreate failed.");
	}

	audren_buffer_size = sizeof(int16_t) * buffer_frames * channels;
	audren_pool_size = (audren_buffer_size * AUDREN_BUFFER_COUNT + 0xFFF) & ~0xFFF;
	audren_pool_ptr = memalign(0x1000, audren_pool_size);

	if (!audren_pool_ptr) {
		audrvClose(&audren_driver);
		audrenExit();
		ERR_FAIL_V_MSG(ERR_OUT_OF_MEMORY, "NX: Could not allocate the audio memory pool.");
	}

	memset(audren_pool_ptr, 0, audren_pool_size);

	for (int i = 0; i < AUDREN_BUFFER_COUNT; i++) {
		audren_buffers[i] = {};
		audren_buffers[i].data_raw = audren_pool_ptr;
		audren_buffers[i].size = audren_pool_size;
		audren_buffers[i].start_sample_offset = i * buffer_frames;
		audren_buffers[i].end_sample_offset = audren_buffers[i].start_sample_offset + buffer_frames;
	}

	int mempool_id = audrvMemPoolAdd(&audren_driver, audren_pool_ptr, audren_pool_size);
	audrvMemPoolAttach(&audren_driver, mempool_id);

	static const u8 sink_channels[] = { 0, 1 };
	audrvDeviceSinkAdd(&audren_driver, AUDREN_DEFAULT_DEVICE_NAME, channels, sink_channels);

	audrvUpdate(&audren_driver);

	ERR_FAIL_COND_V_MSG(R_FAILED(audrenStartAudioRenderer()), ERR_CANT_OPEN, "NX: audrenStartAudioRenderer failed.");
	audren_started = true;

	audrvVoiceInit(&audren_driver, 0, channels, PcmFormat_Int16, mix_rate);
	audrvVoiceSetDestinationMix(&audren_driver, 0, AUDREN_FINAL_MIX_ID);
	audrvVoiceSetMixFactor(&audren_driver, 0, 1.0f, 0, 0);
	audrvVoiceSetMixFactor(&audren_driver, 0, 0.0f, 0, 1);
	audrvVoiceSetMixFactor(&audren_driver, 0, 0.0f, 1, 0);
	audrvVoiceSetMixFactor(&audren_driver, 0, 1.0f, 1, 1);
	audrvVoiceStart(&audren_driver, 0);

	return OK;
}

void AudioDriverNX::finish_device() {
	if (audren_started) {
		audrvVoiceStop(&audren_driver, 0);
		audrvClose(&audren_driver);
		audrenExit();
		audren_started = false;
	}

	if (audren_pool_ptr) {
		free(audren_pool_ptr);
		audren_pool_ptr = nullptr;
	}
}

Error AudioDriverNX::init() {
	active = false;
	exit_thread.clear();

	Error err = init_device();
	if (err != OK) {
		return err;
	}

	thread.start(AudioDriverNX::thread_func, this);

	return OK;
}

void AudioDriverNX::thread_func(void *p_udata) {
	AudioDriverNX *ad = static_cast<AudioDriverNX *>(p_udata);

	svcSetThreadPriority(CUR_THREAD_HANDLE, 0x2B);

	const unsigned int frame_count = ad->buffer_frames * ad->channels;

	while (!ad->exit_thread.is_set()) {
		int free_buffer = -1;
		for (int i = 0; i < AUDREN_BUFFER_COUNT; i++) {
			const u8 state = ad->audren_buffers[i].state;
			if (state == AudioDriverWaveBufState_Free || state == AudioDriverWaveBufState_Done) {
				free_buffer = i;
				break;
			}
		}

		if (free_buffer < 0) {
			audrvUpdate(&ad->audren_driver);
			audrenWaitFrame();
			continue;
		}

		ad->lock();
		ad->start_counting_ticks();

		if (ad->active) {
			ad->audio_server_process(ad->buffer_frames, ad->samples_in.ptrw());
			int16_t *out = ad->samples_out.ptrw();
			const int32_t *in = ad->samples_in.ptr();
			for (unsigned int i = 0; i < frame_count; i++) {
				out[i] = in[i] >> 16;
			}
		} else {
			memset(ad->samples_out.ptrw(), 0, ad->audren_buffer_size);
		}

		ad->stop_counting_ticks();
		ad->unlock();

		uint8_t *dst = (uint8_t *)ad->audren_pool_ptr + (free_buffer * ad->audren_buffer_size);
		memcpy(dst, ad->samples_out.ptr(), ad->audren_buffer_size);
		armDCacheFlush(dst, ad->audren_buffer_size);

		audrvVoiceAddWaveBuf(&ad->audren_driver, 0, &ad->audren_buffers[free_buffer]);
		if (!audrvVoiceIsPlaying(&ad->audren_driver, 0)) {
			audrvVoiceStart(&ad->audren_driver, 0);
		}

		audrvUpdate(&ad->audren_driver);
		audrenWaitFrame();
	}
}

void AudioDriverNX::start() {
	active = true;
}

int AudioDriverNX::get_mix_rate() const {
	return mix_rate;
}

AudioDriver::SpeakerMode AudioDriverNX::get_speaker_mode() const {
	return speaker_mode;
}

void AudioDriverNX::lock() {
	mutex.lock();
}

void AudioDriverNX::unlock() {
	mutex.unlock();
}

void AudioDriverNX::finish() {
	exit_thread.set();
	if (thread.is_started()) {
		thread.wait_to_finish();
	}

	finish_device();
}
