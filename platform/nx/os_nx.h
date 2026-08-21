/**************************************************************************/
/*  os_nx.h                                                               */
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

#ifndef OS_NX_H
#define OS_NX_H

#include "core/os/main_loop.h"
#include "core/os/os.h"

#include "audio_driver_nx.h"
#include "joypad_nx.h"

class OS_NX : public OS {
	MainLoop *main_loop = nullptr;
	AudioDriverNX audio_driver_nx;
	JoypadNX *joypad = nullptr;

	uint64_t start_tick = 0;

protected:
	virtual void initialize() override;
	virtual void initialize_core();
	virtual void initialize_joypads() override;

	virtual void set_main_loop(MainLoop *p_main_loop) override;
	virtual void delete_main_loop() override;

	virtual void finalize() override;
	virtual void finalize_core() override;

public:
	static OS_NX *get_singleton();

	virtual bool _check_internal_feature_support(const String &p_feature) override;

	virtual Vector<String> get_video_adapter_driver_info() const override;

	virtual String get_stdin_string() override;

	virtual Error get_entropy(uint8_t *r_buffer, int p_bytes) override;

	virtual Error execute(const String &p_path, const List<String> &p_arguments, String *r_pipe = nullptr, int *r_exitcode = nullptr, bool read_stderr = false, Mutex *p_pipe_mutex = nullptr, bool p_open_console = false) override;
	virtual Error create_process(const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id = nullptr, bool p_open_console = false) override;
	virtual Error kill(const ProcessID &p_pid) override;
	virtual bool is_process_running(const ProcessID &p_pid) const override;

	virtual bool has_environment(const String &p_var) const override;
	virtual String get_environment(const String &p_var) const override;
	virtual void set_environment(const String &p_var, const String &p_value) const override;
	virtual void unset_environment(const String &p_var) const override;

	virtual String get_name() const override;
	virtual String get_distribution_name() const override;
	virtual String get_version() const override;
	virtual String get_model_name() const override;
	virtual String get_locale() const override;

	virtual MainLoop *get_main_loop() const override;

	virtual DateTime get_datetime(bool p_utc = false) const override;
	virtual TimeZoneInfo get_time_zone_info() const override;

	virtual void delay_usec(uint32_t p_usec) const override;
	virtual uint64_t get_ticks_usec() const override;

	virtual String get_data_path() const override;
	virtual String get_config_path() const override;
	virtual String get_cache_path() const override;
	virtual String get_user_data_dir() const override;

	virtual void alert(const String &p_alert, const String &p_title = "ALERT!") override;

	void run();

	OS_NX();
	~OS_NX();
};

#endif // OS_NX_H
