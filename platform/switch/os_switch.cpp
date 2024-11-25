/**************************************************************************/
/*  os_switch.cpp                                                         */
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

#include "os_switch.h"
#include "drivers/unix/dir_access_unix.h"
#include "drivers/unix/file_access_unix.h"
#include "drivers/unix/ip_unix.h"
#include "drivers/unix/net_socket_posix.h"
#include "drivers/unix/os_unix.h"
#include "drivers/unix/thread_posix.h"

#include <cstdint>
#include <random>

OS_SWITCH::OS_SWITCH() {
    return;
}

void OS_SWITCH::initialize_core() {
	init_thread_posix();

	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCES);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

	NetSocketPosix::make_default();
	IPUnix::make_default();
}

void OS_SWITCH::run() {
}

String OS_SWITCH::get_executable_path() const {
	return OS::get_executable_path();
}

void OS_SWITCH::initialize() {
	OS_SWITCH::initialize_core();
}

void OS_SWITCH::initialize_joypads() {
	joypad = memnew(JoypadSwitch(Input::get_singleton()));
}

void OS_SWITCH::set_main_loop(MainLoop *p_main_loop) {
	this->main_loop = p_main_loop;
}

void OS_SWITCH::delete_main_loop() {
	if (main_loop) {
		memdelete(main_loop);
	}
	main_loop = nullptr;
}

void OS_SWITCH::finalize() {
	OS_SWITCH::delete_main_loop();

	if (this->joypad){
		memdelete(this->joypad);
	}
}

void OS_SWITCH::finalize_core() {
	NetSocketPosix::cleanup();
}

bool OS_SWITCH::_check_internal_feature_support(const String &p_feature) {
	if (p_feature == "mobile") {
		return true;
	}
#if defined(__aarch64__)
	if (p_feature == "arm64-v8a" || p_feature == "arm64") {
		return true;
	}
#endif
	return false;
}
Vector<String> OS_SWITCH::get_video_adapter_driver_info() const {
	return Vector<String>();
}

String OS_SWITCH::get_stdin_string() {
	return String();
}

Error OS_SWITCH::get_entropy(uint8_t *r_buffer, int p_bytes) {
	if (!r_buffer || p_bytes <= 0) {
		return ERR_INVALID_PARAMETER;
	}

	try {
		// Utiliza un generador de números aleatorios seguro.
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<uint8_t> distribution(0, 255);

		for (int i = 0; i < p_bytes; ++i) {
			r_buffer[i] = distribution(generator);
		}
		return OK;
	} catch (...) {
		return ERR_UNAVAILABLE;
	}
}

Error OS_SWITCH::execute(const String &p_path, const List<String> &p_arguments, String *r_pipe, int *r_exitcode, bool read_stderr, Mutex *p_pipe_mutex, bool p_open_console) {

	Vector<String> rebuilt_arguments;
	rebuilt_arguments.push_back(p_path); // !!!! v important
	// This is a super dumb implementation to make the editor vaguely work.
	// It won't work if you don't exit afterwards.
	for (const List<String>::Element *E = p_arguments.front(); E; E = E->next()) {
		if ((*E)->find(" ") >= 0) {
			rebuilt_arguments.push_back(String("\"") + E->get() + String("\""));
		} else {
			rebuilt_arguments.push_back(E->get());
		}
	}

	envSetNextLoad(p_path.utf8().ptr(), String(" ").join(rebuilt_arguments).utf8().ptr());

	return OK;
}
Error OS_SWITCH::create_process(const String &p_path, const List<String> &p_arguments, OS::ProcessID *r_child_id, bool p_open_console) {
	return ERR_UNAVAILABLE;
}

Error OS_SWITCH::kill(const OS::ProcessID &p_pid) {
	return FAILED;
}

bool OS_SWITCH::is_process_running(const OS::ProcessID &p_pid) const {
	return false;
}

bool OS_SWITCH::has_environment(const String &p_var) const {
	return false;
}

String OS_SWITCH::get_environment(const String &p_var) const {
	return String();
}

void OS_SWITCH::set_environment(const String &p_var, const String &p_value) const {
}

void OS_SWITCH::unset_environment(const String &p_var) const {
}
String OS_SWITCH::get_name() const {
	return "NX";
}
String OS_SWITCH::get_distribution_name() const {
	return "Switch";
}
String OS_SWITCH::get_version() const {
	return String();
}
MainLoop *OS_SWITCH::get_main_loop() const {
	return this->main_loop;
}

OS::DateTime OS_SWITCH::get_datetime(bool utc) const {
	return OS::DateTime();
}
OS::TimeZoneInfo OS_SWITCH::get_time_zone_info() const {
	return OS::TimeZoneInfo();
}
void OS_SWITCH::delay_usec(uint32_t p_usec) const {
	svcSleepThread((int64_t)p_usec * 1000ll);
}
uint64_t OS_SWITCH::get_ticks_usec() const {
	static u64 tick_freq = armGetSystemTickFreq();
	return armGetSystemTick() / (tick_freq / 1000000);
}
