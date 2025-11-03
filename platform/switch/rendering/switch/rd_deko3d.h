/**************************************************************************/
/*  rd_deko3d.h                                                           */
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

#ifndef RD_DEKO3D_H
#define RD_DEKO3D_H

#ifdef __SWITCH__

#include "servers/rendering/rendering_device.h"
#include <deko3d.hpp>

class RenderingDeviceDeko3D : public RenderingDevice {
private:
	dk::UniqueDevice device;
	dk::UniqueQueue queue;
	dk::UniqueMemBlock mem_block;
	dk::UniqueCmdBuf cmd_buf;
	dk::UniqueSwapchain swapchain;
	
	DkImage framebuffer_images[2];
	DkImageView framebuffer_views[2];
	
	uint32_t current_frame = 0;
	uint32_t fb_width = 1280;
	uint32_t fb_height = 720;
	
	bool initialized = false;
	bool vsync_enabled = true;

	void _initialize_device();
	void _initialize_swapchain();
	void _cleanup();

public:
	// Minimal initialization for basic rendering
	Error initialize(uint32_t p_width = 1280, uint32_t p_height = 720);
	void finalize();
	
	// Basic rendering operations
	void begin_frame();
	void end_frame();
	void clear_color(const Color &p_color);
	void present();
	
	// VSync control
	void set_vsync_enabled(bool p_enabled) { vsync_enabled = p_enabled; }
	bool get_vsync_enabled() const { return vsync_enabled; }
	
	// Getters
	uint32_t get_width() const { return fb_width; }
	uint32_t get_height() const { return fb_height; }
	bool is_initialized() const { return initialized; }
	
	RenderingDeviceDeko3D();
	~RenderingDeviceDeko3D();
};

#endif // __SWITCH__

#endif // RD_DEKO3D_H
