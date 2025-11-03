/**************************************************************************/
/*  switch_test_scene.cpp                                                 */
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

#include "switch_test_scene.h"
#include "core/io/print_string.h"
#include "servers/rendering_server.h"
#include "servers/display_server.h"

SwitchTestScene::SwitchTestScene() {
	print_line("SwitchTestScene: Initialized");
}

SwitchTestScene::~SwitchTestScene() {
	print_line("SwitchTestScene: Finalized");
}

void SwitchTestScene::initialize() {
	print_line("SwitchTestScene: Starting test scene");
	print_line("Press + button to exit");
	
	frame_count = 0;
	last_fps_update = 0;
	current_fps = 0.0f;
	
	// Initialize rendering
	RenderingServer *rs = RenderingServer::get_singleton();
	if (rs) {
		// Create a simple viewport for rendering
		viewport = rs->viewport_create();
		rs->viewport_set_update_mode(viewport, RenderingServer::VIEWPORT_UPDATE_ALWAYS);
		rs->viewport_set_clear_mode(viewport, RenderingServer::VIEWPORT_CLEAR_ALWAYS);
		
		Size2i screen_size = DisplayServer::get_singleton()->screen_get_size();
		rs->viewport_set_size(viewport, screen_size.width, screen_size.height);
		
		print_line(vformat("Viewport created: %dx%d", screen_size.width, screen_size.height));
	}
}

bool SwitchTestScene::process(double p_time) {
	// Update FPS counter
	frame_count++;
	uint64_t current_time_ms = OS::get_singleton()->get_ticks_msec();
	
	if (current_time_ms - last_fps_update >= 1000) {
		current_fps = frame_count * 1000.0f / (current_time_ms - last_fps_update);
		frame_count = 0;
		last_fps_update = current_time_ms;
		
		// Print FPS to console
		print_line(vformat("FPS: %.1f", current_fps));
	}
	
	// Render test scene
	_render_test_scene();
	
	// Check for exit (+ button is typically button index 11)
	Input *input = Input::get_singleton();
	if (input && input->is_joy_button_pressed(0, JoyButton::START)) {
		print_line("Exit requested via + button");
		return true; // Signal to exit
	}
	
	// Continue running
	return false;
}

void SwitchTestScene::finalize() {
	// Cleanup viewport
	if (viewport.is_valid()) {
		RenderingServer *rs = RenderingServer::get_singleton();
		if (rs) {
			rs->free(viewport);
		}
	}
	
	print_line("SwitchTestScene: Cleanup complete");
}

void SwitchTestScene::_render_test_scene() {
	RenderingServer *rs = RenderingServer::get_singleton();
	if (!rs || !viewport.is_valid()) {
		return;
	}
	
	// Set clear color (dark blue background)
	rs->viewport_set_clear_color(viewport, Color(0.1, 0.1, 0.3, 1.0));
	
	// TODO: Draw "Hello Switch" text
	// TODO: Draw FPS counter
	// For now, just clear the screen with the color
}

#endif // __SWITCH__
