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
#include "drivers/gles3/rasterizer_gles3.h"
#include "main/main.h"

#include <cstdint>
#include <random>

OS_SWITCH::OS_SWITCH() {
    DisplayServerSwitch::register_switch_driver();
}

void OS_SWITCH::initialize_core() {
#if !defined(NO_THREADS)
	init_thread_posix();
#endif
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCES);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

#ifndef NO_NETWORK
	NetSocketPosix::make_default();
	IPUnix::make_default();
#endif

}

void OS_SWITCH::run() {
	if (!main_loop)
		return;

	main_loop->initialize();

	swkbdInlineLaunchForLibraryApplet(&inline_keyboard, SwkbdInlineMode_AppletDisplay, 0);
	/*swkbdInlineSetChangedStringCallback(&inline_keyboard, keyboard_string_changed_callback);
	swkbdInlineSetMovedCursorCallback(&inline_keyboard, keyboard_moved_cursor_callback);
	swkbdInlineSetDecidedEnterCallback(&inline_keyboard, keyboard_decided_enter_callback);
	swkbdInlineSetDecidedCancelCallback(&inline_keyboard, keyboard_decided_cancel_callback);*/

	int last_touch_count = 0;
	// maximum of 16 touches
	Vector2 last_touch_pos[16];
	HidTouchScreenState touch_state = { 0 };

	hidInitializeTouchScreen();

	while (appletMainLoop()) {

		joypad->process_joypads();

		swkbdInlineUpdate(&inline_keyboard, NULL);

		if (Main::iteration())
			break;
	}

	swkbdInlineClose(&inline_keyboard);
	main_loop->finalize();
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
#ifndef NO_NETWORK
	NetSocketPosix::cleanup();
#endif
}

bool OS_SWITCH::_check_internal_feature_support(const String &p_feature) {
	return p_feature == "mobile";
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
	return ERR_UNAVAILABLE;
}
Error OS_SWITCH::create_process(const String &p_path, const List<String> &p_arguments, OS::ProcessID *r_child_id, bool p_open_console) {
	return ERR_UNAVAILABLE;
}

Error OS_SWITCH::kill(const OS::ProcessID &p_pid) {
	return ERR_UNAVAILABLE;
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
