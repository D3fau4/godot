/**************************************************************************/
/*  display_server_nx.cpp                                                 */
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

#include "display_server_nx.h"

#include "core/config/project_settings.h"
#include "core/input/input.h"
#include "core/input/input_event.h"
#include "core/string/print_string.h"

#ifdef GLES3_ENABLED
#include "drivers/gles3/rasterizer_gles3.h"
#endif

#ifdef VULKAN_ENABLED
#include "servers/rendering/renderer_rd/renderer_compositor_rd.h"
#endif

#include <string.h>

DisplayServerNX *DisplayServerNX::get_singleton() {
	return static_cast<DisplayServerNX *>(DisplayServer::get_singleton());
}

bool DisplayServerNX::has_feature(Feature p_feature) const {
	switch (p_feature) {
		case FEATURE_TOUCHSCREEN:
		case FEATURE_VIRTUAL_KEYBOARD:
		case FEATURE_SWAP_BUFFERS:
			return true;
		default:
			return false;
	}
}

String DisplayServerNX::get_name() const {
	return "NX";
}

int DisplayServerNX::get_screen_count() const {
	return 1;
}

int DisplayServerNX::get_primary_screen() const {
	return 0;
}

Point2i DisplayServerNX::screen_get_position(int p_screen) const {
	return Point2i();
}

Size2i DisplayServerNX::screen_get_size(int p_screen) const {
	return window_size;
}

Rect2i DisplayServerNX::screen_get_usable_rect(int p_screen) const {
	return Rect2i(Point2i(), window_size);
}

int DisplayServerNX::screen_get_dpi(int p_screen) const {
	return docked ? 96 : 237;
}

float DisplayServerNX::screen_get_scale(int p_screen) const {
	return 1.0f;
}

float DisplayServerNX::screen_get_refresh_rate(int p_screen) const {
	return 60.0f;
}

bool DisplayServerNX::is_touchscreen_available() const {
	return !docked;
}

void DisplayServerNX::screen_set_keep_on(bool p_enable) {
	appletSetAutoSleepDisabled(p_enable);
}

bool DisplayServerNX::screen_is_kept_on() const {
	bool disabled = false;
	appletIsAutoSleepDisabled(&disabled);
	return disabled;
}

void DisplayServerNX::virtual_keyboard_show(const String &p_existing_text, const Rect2 &p_screen_rect, VirtualKeyboardType p_type, int p_max_length, int p_cursor_start, int p_cursor_end) {
	SwkbdConfig keyboard;
	if (R_FAILED(swkbdCreate(&keyboard, 0))) {
		ERR_FAIL_MSG("NX: Could not create the software keyboard.");
	}

	swkbdConfigMakePresetDefault(&keyboard);

	switch (p_type) {
		case KEYBOARD_TYPE_NUMBER:
		case KEYBOARD_TYPE_NUMBER_DECIMAL:
		case KEYBOARD_TYPE_PHONE:
			swkbdConfigSetType(&keyboard, SwkbdType_NumPad);
			break;
		case KEYBOARD_TYPE_PASSWORD:
			swkbdConfigSetPasswordFlag(&keyboard, 1);
			break;
		default:
			break;
	}

	CharString existing = p_existing_text.utf8();
	swkbdConfigSetInitialText(&keyboard, existing.get_data());

	if (p_max_length > 0) {
		swkbdConfigSetStringLenMax(&keyboard, p_max_length);
	}

	const int buffer_size = 2048;
	char *buffer = memnew_arr(char, buffer_size);
	buffer[0] = '\0';

	if (R_SUCCEEDED(swkbdShow(&keyboard, buffer, buffer_size))) {
		_window_callback(input_text_callback, String::utf8(buffer));
	}

	memdelete_arr(buffer);
	swkbdClose(&keyboard);
}

void DisplayServerNX::virtual_keyboard_hide() {
}

int DisplayServerNX::virtual_keyboard_get_height() const {
	return 0;
}

Vector<DisplayServer::WindowID> DisplayServerNX::get_window_list() const {
	Vector<WindowID> ret;
	ret.push_back(MAIN_WINDOW_ID);
	return ret;
}

DisplayServer::WindowID DisplayServerNX::get_window_at_screen_position(const Point2i &p_position) const {
	return MAIN_WINDOW_ID;
}

int64_t DisplayServerNX::window_get_native_handle(HandleType p_handle_type, WindowID p_window) const {
	switch (p_handle_type) {
#ifdef GLES3_ENABLED
		case DISPLAY_HANDLE: {
			return gl_manager ? (int64_t)gl_manager->get_display() : 0;
		}
		case OPENGL_CONTEXT: {
			return gl_manager ? (int64_t)gl_manager->get_context() : 0;
		}
#endif
		case WINDOW_HANDLE: {
			return (int64_t)nwindowGetDefault();
		}
		default: {
			return 0;
		}
	}
}

void DisplayServerNX::window_attach_instance_id(ObjectID p_instance, WindowID p_window) {
	window_attached_instance_id = p_instance;
}

ObjectID DisplayServerNX::window_get_attached_instance_id(WindowID p_window) const {
	return window_attached_instance_id;
}

void DisplayServerNX::window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window) {
	rect_changed_callback = p_callable;
}

void DisplayServerNX::window_set_window_event_callback(const Callable &p_callable, WindowID p_window) {
	window_event_callback = p_callable;
}

void DisplayServerNX::window_set_input_event_callback(const Callable &p_callable, WindowID p_window) {
	input_event_callback = p_callable;
}

void DisplayServerNX::window_set_input_text_callback(const Callable &p_callable, WindowID p_window) {
	input_text_callback = p_callable;
}

void DisplayServerNX::window_set_drop_files_callback(const Callable &p_callable, WindowID p_window) {
}

void DisplayServerNX::window_set_title(const String &p_title, WindowID p_window) {
}

int DisplayServerNX::window_get_current_screen(WindowID p_window) const {
	return 0;
}

void DisplayServerNX::window_set_current_screen(int p_screen, WindowID p_window) {
}

Point2i DisplayServerNX::window_get_position(WindowID p_window) const {
	return Point2i();
}

Point2i DisplayServerNX::window_get_position_with_decorations(WindowID p_window) const {
	return Point2i();
}

void DisplayServerNX::window_set_position(const Point2i &p_position, WindowID p_window) {
}

void DisplayServerNX::window_set_transient(WindowID p_window, WindowID p_parent) {
}

void DisplayServerNX::window_set_max_size(const Size2i p_size, WindowID p_window) {
}

Size2i DisplayServerNX::window_get_max_size(WindowID p_window) const {
	return Size2i();
}

void DisplayServerNX::window_set_min_size(const Size2i p_size, WindowID p_window) {
}

Size2i DisplayServerNX::window_get_min_size(WindowID p_window) const {
	return Size2i();
}

void DisplayServerNX::window_set_size(const Size2i p_size, WindowID p_window) {
}

Size2i DisplayServerNX::window_get_size(WindowID p_window) const {
	return window_size;
}

Size2i DisplayServerNX::window_get_size_with_decorations(WindowID p_window) const {
	return window_size;
}

void DisplayServerNX::window_set_mode(WindowMode p_mode, WindowID p_window) {
}

DisplayServer::WindowMode DisplayServerNX::window_get_mode(WindowID p_window) const {
	return WINDOW_MODE_FULLSCREEN;
}

bool DisplayServerNX::window_is_maximize_allowed(WindowID p_window) const {
	return false;
}

void DisplayServerNX::window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window) {
}

bool DisplayServerNX::window_get_flag(WindowFlags p_flag, WindowID p_window) const {
	return false;
}

void DisplayServerNX::window_request_attention(WindowID p_window) {
}

void DisplayServerNX::window_move_to_foreground(WindowID p_window) {
}

bool DisplayServerNX::window_is_focused(WindowID p_window) const {
	return true;
}

bool DisplayServerNX::window_can_draw(WindowID p_window) const {
	return true;
}

bool DisplayServerNX::can_any_window_draw() const {
	return true;
}

void DisplayServerNX::window_set_vsync_mode(VSyncMode p_vsync_mode, WindowID p_window) {
	vsync_mode = p_vsync_mode;
#ifdef GLES3_ENABLED
	if (gl_manager) {
		gl_manager->set_use_vsync(p_vsync_mode != VSYNC_DISABLED);
	}
#endif

#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		context_vulkan->set_vsync_mode(p_window, p_vsync_mode);
	}
#endif
}

DisplayServer::VSyncMode DisplayServerNX::window_get_vsync_mode(WindowID p_window) const {
#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		return context_vulkan->get_vsync_mode(p_window);
	}
#endif
	return vsync_mode;
}

Point2i DisplayServerNX::mouse_get_position() const {
	return last_touch_pos;
}

void DisplayServerNX::_window_callback(const Callable &p_callable, const Variant &p_arg, bool p_deferred) const {
	if (p_callable.is_null()) {
		return;
	}

	const Variant *argp = &p_arg;
	if (p_deferred) {
		p_callable.call_deferredp((const Variant **)&argp, 1);
	} else {
		Variant ret;
		Callable::CallError ce;
		p_callable.callp((const Variant **)&argp, 1, ret, ce);
	}
}

void DisplayServerNX::_dispatch_input_events(const Ref<InputEvent> &p_event) {
	DisplayServerNX::get_singleton()->_dispatch_input_event(p_event);
}

void DisplayServerNX::_dispatch_input_event(const Ref<InputEvent> &p_event) {
	_window_callback(input_event_callback, p_event);
}

void DisplayServerNX::_update_operation_mode() {
	const bool new_docked = appletGetOperationMode() == AppletOperationMode_Console;
	if (new_docked == docked) {
		return;
	}

	docked = new_docked;
	const Size2i new_size = docked ? Size2i(DOCKED_WIDTH, DOCKED_HEIGHT) : Size2i(HANDHELD_WIDTH, HANDHELD_HEIGHT);

#ifdef GLES3_ENABLED
	if (gl_manager && gl_manager->resize(new_size) != OK) {
		ERR_PRINT("NX: Could not resize the rendering surface.");
		return;
	}
#endif

#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		// Not nwindowSetDimensions(): it refuses to change the window while
		// buffers are registered on it, and the swapchain holds them until it
		// is recreated. Recreating it is what resizes the window.
		context_vulkan->window_resize(MAIN_WINDOW_ID, new_size.width, new_size.height);
	}
#endif

	window_size = new_size;
	_window_callback(rect_changed_callback, Rect2i(Point2i(), window_size));
}

void DisplayServerNX::_process_touch() {
#ifdef TOUCH_ENABLED
	bool seen[MAX_TOUCHES] = {};

	HidTouchScreenState state;
	memset(&state, 0, sizeof(state));

	if (hidGetTouchScreenStates(&state, 1)) {
		const float scale_x = (float)window_size.width / (float)HANDHELD_WIDTH;
		const float scale_y = (float)window_size.height / (float)HANDHELD_HEIGHT;

		for (int i = 0; i < (int)state.count; i++) {
			const HidTouchState &touch = state.touches[i];
			const int index = touch.finger_id;
			if (index < 0 || index >= MAX_TOUCHES) {
				continue;
			}

			seen[index] = true;

			const Point2i pos = Point2i(touch.x * scale_x, touch.y * scale_y);
			last_touch_pos = pos;

			if (!touch_state[index].active) {
				touch_state[index].active = true;
				touch_state[index].pos = pos;

				Ref<InputEventScreenTouch> ev;
				ev.instantiate();
				ev->set_index(index);
				ev->set_position(pos);
				ev->set_pressed(true);
				Input::get_singleton()->parse_input_event(ev);
			} else if (touch_state[index].pos != pos) {
				Ref<InputEventScreenDrag> ev;
				ev.instantiate();
				ev->set_index(index);
				ev->set_position(pos);
				ev->set_relative(Vector2(pos - touch_state[index].pos));
				touch_state[index].pos = pos;
				Input::get_singleton()->parse_input_event(ev);
			}
		}
	}

	for (int index = 0; index < MAX_TOUCHES; index++) {
		if (!touch_state[index].active || seen[index]) {
			continue;
		}

		touch_state[index].active = false;

		Ref<InputEventScreenTouch> ev;
		ev.instantiate();
		ev->set_index(index);
		ev->set_position(touch_state[index].pos);
		ev->set_pressed(false);
		Input::get_singleton()->parse_input_event(ev);
	}
#endif
}

void DisplayServerNX::process_events() {
	_update_operation_mode();
	_process_touch();
}

void DisplayServerNX::release_rendering_thread() {
#ifdef GLES3_ENABLED
	if (gl_manager) {
		gl_manager->release_current();
	}
#endif
}

void DisplayServerNX::make_rendering_thread() {
#ifdef GLES3_ENABLED
	if (gl_manager) {
		gl_manager->make_current();
	}
#endif
}

void DisplayServerNX::swap_buffers() {
#ifdef GLES3_ENABLED
	if (gl_manager) {
		gl_manager->swap_buffers();
	}
#endif
}

void DisplayServerNX::gl_window_make_current(WindowID p_window_id) {
#ifdef GLES3_ENABLED
	if (gl_manager) {
		gl_manager->make_current();
	}
#endif
}

void DisplayServerNX::set_native_icon(const String &p_filename) {
}

void DisplayServerNX::set_icon(const Ref<Image> &p_icon) {
}

Vector<String> DisplayServerNX::get_rendering_drivers_func() {
	Vector<String> drivers;

#ifdef VULKAN_ENABLED
	drivers.push_back("vulkan");
#endif

#ifdef GLES3_ENABLED
	drivers.push_back("opengl3");
#endif

	return drivers;
}

DisplayServer *DisplayServerNX::create_func(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error) {
	DisplayServer *ds = memnew(DisplayServerNX(p_rendering_driver, p_mode, p_vsync_mode, p_flags, p_position, p_resolution, p_screen, r_error));
	if (r_error != OK) {
		ERR_PRINT("NX: Failed to initialize the display server.");
		memdelete(ds);
		return nullptr;
	}
	return ds;
}

void DisplayServerNX::register_nx_driver() {
	register_create_function("nx", create_func, get_rendering_drivers_func);
}

DisplayServerNX::DisplayServerNX(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error) {
	rendering_driver = p_rendering_driver;

	docked = appletGetOperationMode() == AppletOperationMode_Console;
	window_size = docked ? Size2i(DOCKED_WIDTH, DOCKED_HEIGHT) : Size2i(HANDHELD_WIDTH, HANDHELD_HEIGHT);

#ifdef TOUCH_ENABLED
	hidInitializeTouchScreen();
#endif

#ifdef VULKAN_ENABLED
	if (rendering_driver == "vulkan") {
		NWindow *window = nwindowGetDefault();
		if (window == nullptr || R_FAILED(nwindowSetDimensions(window, window_size.width, window_size.height))) {
			r_error = ERR_CANT_CREATE;
			ERR_FAIL_MSG("NX: Could not configure the native window.");
		}

		context_vulkan = memnew(VulkanContextNX);
		if (context_vulkan->initialize() != OK) {
			memdelete(context_vulkan);
			context_vulkan = nullptr;
			r_error = ERR_CANT_CREATE;
			ERR_FAIL_MSG("NX: Could not initialize the Vulkan context.");
		}

		if (context_vulkan->window_create(window, p_vsync_mode, window_size.width, window_size.height) != OK) {
			memdelete(context_vulkan);
			context_vulkan = nullptr;
			r_error = ERR_CANT_CREATE;
			ERR_FAIL_MSG("NX: Could not create the Vulkan window.");
		}

		rendering_device_vulkan = memnew(RenderingDeviceVulkan);
		rendering_device_vulkan->initialize(context_vulkan);

		RendererCompositorRD::make_current();
	}
#endif

#ifdef GLES3_ENABLED
	if (rendering_driver == "opengl3") {
		gl_manager = memnew(GLManagerNX);
		r_error = gl_manager->initialize(window_size);
		if (r_error != OK) {
			memdelete(gl_manager);
			gl_manager = nullptr;
			ERR_FAIL_MSG("NX: Could not initialize the OpenGL ES context.");
		}

		RasterizerGLES3::make_current();
	}
#endif

	window_set_vsync_mode(p_vsync_mode);

	Input::get_singleton()->set_event_dispatch_function(_dispatch_input_events);

	r_error = OK;
}

DisplayServerNX::~DisplayServerNX() {
#ifdef GLES3_ENABLED
	if (gl_manager) {
		memdelete(gl_manager);
		gl_manager = nullptr;
	}
#endif

#ifdef VULKAN_ENABLED
	if (rendering_device_vulkan) {
		rendering_device_vulkan->finalize();
		memdelete(rendering_device_vulkan);
		rendering_device_vulkan = nullptr;
	}

	if (context_vulkan) {
		context_vulkan->window_destroy(MAIN_WINDOW_ID);
		memdelete(context_vulkan);
		context_vulkan = nullptr;
	}
#endif
}
