/**************************************************************************/
/*  audio_driver_nx.h                                                     */
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

#ifndef AUDIO_DRIVER_NX_H
#define AUDIO_DRIVER_NX_H

#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "core/templates/safe_refcount.h"
#include "servers/audio_server.h"
#include "switch_wrapper.h"

class AudioDriverNX : public AudioDriver {
	enum {
		AUDREN_BUFFER_COUNT = 2,
	};

	Thread thread;
	Mutex mutex;

	LibnxAudioDriver audren_driver;
	AudioDriverWaveBuf audren_buffers[AUDREN_BUFFER_COUNT];
	void *audren_pool_ptr = nullptr;
	size_t audren_pool_size = 0;
	unsigned int audren_buffer_size = 0;

	Vector<int32_t> samples_in;
	Vector<int16_t> samples_out;

	unsigned int buffer_frames = 0;
	unsigned int mix_rate = 0;
	int channels = 2;
	SpeakerMode speaker_mode = SPEAKER_MODE_STEREO;

	bool active = false;
	bool audren_started = false;
	SafeFlag exit_thread;

	Error init_device();
	void finish_device();

	static void thread_func(void *p_udata);

public:
	virtual const char *get_name() const override { return "AUDREN"; }

	virtual Error init() override;
	virtual void start() override;
	virtual int get_mix_rate() const override;
	virtual SpeakerMode get_speaker_mode() const override;
	virtual void lock() override;
	virtual void unlock() override;
	virtual void finish() override;

	AudioDriverNX() {}
	~AudioDriverNX() {}
};

#endif // AUDIO_DRIVER_NX_H
