/**************************************************************************/
/*  dk_memory.h                                                           */
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

#ifndef DK_MEMORY_H
#define DK_MEMORY_H

#ifdef __SWITCH__

#include <deko3d.hpp>

// Memory management helpers for deko3d

namespace DkMemory {

// Standard memory block sizes
constexpr uint32_t MB = 1024 * 1024;
constexpr uint32_t COMMAND_BUFFER_SIZE = 16 * MB;
constexpr uint32_t VERTEX_BUFFER_SIZE = 4 * MB;
constexpr uint32_t UNIFORM_BUFFER_SIZE = 1 * MB;

// Helper to create a CPU-accessible memory block
inline dk::UniqueMemBlock create_cpu_mem_block(dk::Device device, uint32_t size) {
	return dk::MemBlockMaker{device, size}
		.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
		.create();
}

// Helper to create a GPU-only memory block
inline dk::UniqueMemBlock create_gpu_mem_block(dk::Device device, uint32_t size) {
	return dk::MemBlockMaker{device, size}
		.setFlags(DkMemBlockFlags_GpuCached)
		.create();
}

// Helper to create an image memory block
inline dk::UniqueMemBlock create_image_mem_block(dk::Device device, uint32_t size) {
	return dk::MemBlockMaker{device, size}
		.setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
		.create();
}

} // namespace DkMemory

#endif // __SWITCH__

#endif // DK_MEMORY_H
