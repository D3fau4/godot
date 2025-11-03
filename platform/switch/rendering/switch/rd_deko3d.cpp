/**************************************************************************/
/*  rd_deko3d.cpp                                                         */
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

#include "rd_deko3d.h"
#include "core/config/project_settings.h"
#include "core/io/print_string.h"
#include <switch.h>

RenderingDeviceDeko3D::RenderingDeviceDeko3D() {
}

RenderingDeviceDeko3D::~RenderingDeviceDeko3D() {
	finalize();
}

void RenderingDeviceDeko3D::_initialize_device() {
	// Create deko3d device
	device = dk::DeviceMaker{}.create();
	if (!device) {
		ERR_FAIL_MSG("Failed to create deko3d device");
	}
	
	// Create memory pool for command buffers (16 MB)
	constexpr uint32_t cmdBufSize = 16 * 1024 * 1024;
	mem_block = dk::MemBlockMaker{device, cmdBufSize}
		.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
		.create();
	
	if (!mem_block) {
		ERR_FAIL_MSG("Failed to create deko3d memory block");
	}
	
	// Create command buffer
	cmd_buf = dk::CmdBufMaker{device}
		.create();
	if (!cmd_buf) {
		ERR_FAIL_MSG("Failed to create deko3d command buffer");
	}
	
	cmd_buf.addMemory(mem_block, 0, cmdBufSize);
	
	// Create queue
	queue = dk::QueueMaker{device}
		.setFlags(DkQueueFlags_Graphics)
		.create();
	if (!queue) {
		ERR_FAIL_MSG("Failed to create deko3d queue");
	}
	
	print_line("deko3d: Device initialized successfully");
}

void RenderingDeviceDeko3D::_initialize_swapchain() {
	// Create swapchain for double buffering
	constexpr uint32_t imageCount = 2;
	
	// Allocate memory for framebuffers
	DkImageLayout layout;
	dk::ImageLayoutMaker{device}
		.setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
		.setFormat(DkImageFormat_RGBA8_Unorm)
		.setDimensions(fb_width, fb_height)
		.initialize(layout);
	
	const uint32_t fbSize = layout.getSize();
	const uint32_t fbAlign = layout.getAlignment();
	const uint32_t totalSize = (fbSize + fbAlign - 1) & ~(fbAlign - 1);
	
	// Create memory block for framebuffers
	dk::UniqueMemBlock fb_mem_block = dk::MemBlockMaker{device, totalSize * imageCount}
		.setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
		.create();
	
	if (!fb_mem_block) {
		ERR_FAIL_MSG("Failed to create framebuffer memory block");
	}
	
	// Create framebuffer images
	for (uint32_t i = 0; i < imageCount; i++) {
		framebuffer_images[i].initialize(layout, fb_mem_block, i * totalSize);
		framebuffer_views[i].initialize(framebuffer_images[i]);
	}
	
	// Create swapchain
	swapchain = dk::SwapchainMaker{device, nwindowGetDefault(), framebuffer_images, imageCount}
		.create();
	
	if (!swapchain) {
		ERR_FAIL_MSG("Failed to create swapchain");
	}
	
	print_line(vformat("deko3d: Swapchain initialized - %dx%d", fb_width, fb_height));
}

Error RenderingDeviceDeko3D::initialize(uint32_t p_width, uint32_t p_height) {
	if (initialized) {
		return OK;
	}
	
	fb_width = p_width;
	fb_height = p_height;
	
	print_line(vformat("deko3d: Initializing rendering device - %dx%d", fb_width, fb_height));
	
	_initialize_device();
	_initialize_swapchain();
	
	initialized = true;
	print_line("deko3d: Rendering device initialized successfully");
	print_line(vformat("deko3d: VSync %s", vsync_enabled ? "ON" : "OFF"));
	
	return OK;
}

void RenderingDeviceDeko3D::finalize() {
	if (!initialized) {
		return;
	}
	
	// Wait for GPU to finish
	if (queue) {
		queue.waitIdle();
	}
	
	// Cleanup swapchain
	swapchain.destroy();
	
	// Cleanup command buffer and queue
	cmd_buf.destroy();
	queue.destroy();
	
	// Cleanup memory blocks
	mem_block.destroy();
	
	// Cleanup device
	device.destroy();
	
	initialized = false;
	print_line("deko3d: Rendering device finalized");
}

void RenderingDeviceDeko3D::begin_frame() {
	if (!initialized) {
		return;
	}
	
	// Acquire next framebuffer image
	current_frame = swapchain.acquireImage();
	
	// Start recording commands
	cmd_buf.clear();
}

void RenderingDeviceDeko3D::clear_color(const Color &p_color) {
	if (!initialized) {
		return;
	}
	
	// Set render target
	cmd_buf.bindRenderTargets(&framebuffer_views[current_frame]);
	
	// Clear color
	DkColorFloat clear_color = {
		p_color.r,
		p_color.g,
		p_color.b,
		p_color.a
	};
	cmd_buf.clearColor(0, DkColorMask_RGBA, &clear_color);
}

void RenderingDeviceDeko3D::end_frame() {
	if (!initialized) {
		return;
	}
	
	// Finish command buffer
	queue.submitCommands(cmd_buf.finishList());
}

void RenderingDeviceDeko3D::present() {
	if (!initialized) {
		return;
	}
	
	// Present the current frame
	queue.presentImage(swapchain, current_frame);
	
	// Wait for presentation to complete if VSync is enabled
	if (vsync_enabled) {
		queue.waitIdle();
	}
}

void RenderingDeviceDeko3D::_cleanup() {
	finalize();
}

#endif // __SWITCH__
