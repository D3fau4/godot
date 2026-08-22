/**************************************************************************/
/*  os_nx.cpp                                                             */
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

#include "os_nx.h"

#include "core/config/project_settings.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/string/print_string.h"
#include "drivers/unix/dir_access_unix.h"
#include "drivers/unix/file_access_unix.h"
#include "drivers/unix/ip_unix.h"
#include "drivers/unix/net_socket_posix.h"
#include "drivers/unix/thread_posix.h"
#include "main/main.h"

#include "display_server_nx.h"

#include <stdlib.h>
#include <time.h>

OS_NX *OS_NX::get_singleton() {
	return static_cast<OS_NX *>(OS::get_singleton());
}

void OS_NX::initialize_core() {
	init_thread_posix();

	start_tick = armGetSystemTick();

	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_RESOURCES);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccessUnix>(FileAccess::ACCESS_FILESYSTEM);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccessUnix>(DirAccess::ACCESS_FILESYSTEM);

	NetSocketPosix::make_default();
	IPUnix::make_default();
}

void OS_NX::initialize() {
	initialize_core();
}

void OS_NX::initialize_joypads() {
	joypad = memnew(JoypadNX(Input::get_singleton()));
}

void OS_NX::finalize() {
	if (joypad) {
		memdelete(joypad);
		joypad = nullptr;
	}

	delete_main_loop();
}

void OS_NX::finalize_core() {
	NetSocketPosix::cleanup();
}

void OS_NX::set_main_loop(MainLoop *p_main_loop) {
	main_loop = p_main_loop;
}

void OS_NX::delete_main_loop() {
	if (main_loop) {
		memdelete(main_loop);
		main_loop = nullptr;
	}
}

MainLoop *OS_NX::get_main_loop() const {
	return main_loop;
}

bool OS_NX::_check_internal_feature_support(const String &p_feature) {
	return p_feature == "switch";
}

Vector<String> OS_NX::get_video_adapter_driver_info() const {
	return Vector<String>();
}

String OS_NX::get_stdin_string() {
	return String();
}

Error OS_NX::get_entropy(uint8_t *r_buffer, int p_bytes) {
	if (R_FAILED(csrngGetRandomBytes(r_buffer, p_bytes))) {
		return FAILED;
	}
	return OK;
}

Error OS_NX::execute(const String &p_path, const List<String> &p_arguments, String *r_pipe, int *r_exitcode, bool read_stderr, Mutex *p_pipe_mutex, bool p_open_console) {
	return ERR_UNAVAILABLE;
}

Error OS_NX::create_process(const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id, bool p_open_console) {
	return ERR_UNAVAILABLE;
}

Error OS_NX::kill(const ProcessID &p_pid) {
	return ERR_UNAVAILABLE;
}

bool OS_NX::is_process_running(const ProcessID &p_pid) const {
	return false;
}

bool OS_NX::has_environment(const String &p_var) const {
	return getenv(p_var.utf8().get_data()) != nullptr;
}

String OS_NX::get_environment(const String &p_var) const {
	const char *val = getenv(p_var.utf8().get_data());
	if (!val) {
		return String();
	}
	return String::utf8(val);
}

void OS_NX::set_environment(const String &p_var, const String &p_value) const {
	ERR_FAIL_COND_MSG(p_var.is_empty() || p_var.contains("="), vformat("Invalid environment variable name '%s', cannot be empty or include '='.", p_var));
	setenv(p_var.utf8().get_data(), p_value.utf8().get_data(), 1);
}

void OS_NX::unset_environment(const String &p_var) const {
	ERR_FAIL_COND_MSG(p_var.is_empty() || p_var.contains("="), vformat("Invalid environment variable name '%s', cannot be empty or include '='.", p_var));
	unsetenv(p_var.utf8().get_data());
}

String OS_NX::get_name() const {
	return "NX";
}

String OS_NX::get_distribution_name() const {
	return "Horizon";
}

String OS_NX::get_version() const {
	const u32 version = hosversionGet();
	return vformat("%d.%d.%d", HOSVER_MAJOR(version), HOSVER_MINOR(version), HOSVER_MICRO(version));
}

String OS_NX::get_model_name() const {
	return appletGetOperationMode() == AppletOperationMode_Console ? "Nintendo Switch (docked)" : "Nintendo Switch (handheld)";
}

int OS_NX::get_processor_count() const {
	u64 core_mask = 0;
	if (R_FAILED(svcGetInfo(&core_mask, InfoType_CoreMask, CUR_PROCESS_HANDLE, 0))) {
		return 1;
	}

	const int count = __builtin_popcountll(core_mask);
	return count > 0 ? count : 1;
}

String OS_NX::get_locale() const {
	u64 language_code = 0;
	if (R_FAILED(setGetSystemLanguage(&language_code))) {
		return "en";
	}

	SetLanguage language = SetLanguage_ENUS;
	if (R_FAILED(setMakeLanguage(language_code, &language))) {
		return "en";
	}

	switch (language) {
		case SetLanguage_JA:
			return "ja";
		case SetLanguage_FR:
		case SetLanguage_FRCA:
			return "fr";
		case SetLanguage_DE:
			return "de";
		case SetLanguage_IT:
			return "it";
		case SetLanguage_ES:
		case SetLanguage_ES419:
			return "es";
		case SetLanguage_ZHCN:
		case SetLanguage_ZHHANS:
			return "zh_CN";
		case SetLanguage_ZHTW:
		case SetLanguage_ZHHANT:
			return "zh_TW";
		case SetLanguage_KO:
			return "ko";
		case SetLanguage_NL:
			return "nl";
		case SetLanguage_PT:
		case SetLanguage_PTBR:
			return "pt";
		case SetLanguage_RU:
			return "ru";
		default:
			return "en";
	}
}

OS::DateTime OS_NX::get_datetime(bool p_utc) const {
	time_t t = time(nullptr);
	struct tm lt;

	if (p_utc) {
		gmtime_r(&t, &lt);
	} else {
		localtime_r(&t, &lt);
	}

	DateTime dt;
	dt.year = 1900 + lt.tm_year;
	dt.month = (Month)(lt.tm_mon + 1);
	dt.day = lt.tm_mday;
	dt.weekday = (Weekday)lt.tm_wday;
	dt.hour = lt.tm_hour;
	dt.minute = lt.tm_min;
	dt.second = lt.tm_sec;
	dt.dst = lt.tm_isdst > 0;

	return dt;
}

OS::TimeZoneInfo OS_NX::get_time_zone_info() const {
	time_t t = time(nullptr);

	struct tm lt;
	struct tm gt;
	localtime_r(&t, &lt);
	gmtime_r(&t, &gt);

	TimeZoneInfo ret;
	ret.name = "UTC";
	ret.bias = 0;

	char name[16];
	if (strftime(name, sizeof(name), "%Z", &lt) > 0) {
		ret.name = String::utf8(name);
	}

	gt.tm_isdst = 0;
	ret.bias = (int)((t - mktime(&gt)) / 60);

	return ret;
}

void OS_NX::delay_usec(uint32_t p_usec) const {
	svcSleepThread((u64)p_usec * 1000ULL);
}

uint64_t OS_NX::get_ticks_usec() const {
	return armTicksToNs(armGetSystemTick() - start_tick) / 1000ULL;
}

String OS_NX::get_executable_path() const {
	return executable_path;
}

String OS_NX::get_data_path() const {
	return "sdmc:/switch";
}

String OS_NX::get_config_path() const {
	return get_data_path();
}

String OS_NX::get_cache_path() const {
	return get_user_data_dir().path_join("cache");
}

String OS_NX::get_user_data_dir() const {
	String appname = get_safe_dir_name(GLOBAL_GET("application/config/name"));
	if (!appname.is_empty()) {
		bool use_custom_dir = GLOBAL_GET("application/config/use_custom_user_dir");
		if (use_custom_dir) {
			String custom_dir = get_safe_dir_name(GLOBAL_GET("application/config/custom_user_dir_name"), true);
			if (custom_dir.is_empty()) {
				custom_dir = appname;
			}
			return get_data_path().path_join(custom_dir);
		} else {
			return get_data_path().path_join(get_godot_dir_name()).path_join("app_userdata").path_join(appname);
		}
	}

	return get_data_path().path_join(get_godot_dir_name()).path_join("app_userdata").path_join("[unnamed project]");
}

void OS_NX::alert(const String &p_alert, const String &p_title) {
	print_line(vformat("%s: %s", p_title, p_alert));
}

void OS_NX::run() {
	if (!main_loop) {
		return;
	}

	main_loop->initialize();

	while (appletMainLoop()) {
		if (joypad) {
			joypad->process_joypads();
		}

		DisplayServer::get_singleton()->process_events();

		if (Main::iteration()) {
			break;
		}
	}

	main_loop->finalize();
}

OS_NX::OS_NX(const char *p_execpath) {
	executable_path = String::utf8(p_execpath);

	AudioDriverManager::add_driver(&audio_driver_nx);

	DisplayServerNX::register_nx_driver();
}

OS_NX::~OS_NX() {
}
