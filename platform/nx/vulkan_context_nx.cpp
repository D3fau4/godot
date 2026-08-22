/**************************************************************************/
/*  vulkan_context_nx.cpp                                                 */
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

#include "vulkan_context_nx.h"

#ifdef VULKAN_ENABLED

#include "drivers/vulkan/godot_vulkan.h"

const char *VulkanContextNX::_get_platform_surface_extension() const {
	return VK_NN_VI_SURFACE_EXTENSION_NAME;
}

bool VulkanContextNX::_use_validation_layers() {
	return false;
}

Error VulkanContextNX::window_create(NWindow *p_window, DisplayServer::VSyncMode p_vsync_mode, int p_width, int p_height) {
	ERR_FAIL_NULL_V(p_window, ERR_INVALID_PARAMETER);

	VkViSurfaceCreateInfoNN create_info;
	create_info.sType = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN;
	create_info.pNext = nullptr;
	create_info.flags = 0;
	create_info.window = p_window;

	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkResult err = vkCreateViSurfaceNN(get_instance(), &create_info, nullptr, &surface);
	if (err != VK_SUCCESS) {
		ERR_FAIL_V_MSG(ERR_CANT_CREATE, "vkCreateViSurfaceNN failed with error " + itos(err));
	}

	return _window_create(DisplayServer::MAIN_WINDOW_ID, p_vsync_mode, surface, p_width, p_height);
}

VulkanContextNX::VulkanContextNX() {
}

VulkanContextNX::~VulkanContextNX() {
}

#endif // VULKAN_ENABLED
