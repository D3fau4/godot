/**************************************************************************/
/*  dk_pipeline.h                                                         */
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

#ifndef DK_PIPELINE_H
#define DK_PIPELINE_H

#ifdef __SWITCH__

#include <deko3d.hpp>

// Pipeline management helpers for deko3d

struct DkPipelineState {
	DkPrimitive primitive = DkPrimitive_Triangles;
	bool depth_test_enabled = false;
	bool blend_enabled = false;
	DkBlendOp blend_op = DkBlendOp_Add;
	DkBlendFactor src_blend = DkBlendFactor_One;
	DkBlendFactor dst_blend = DkBlendFactor_Zero;
	DkCullMode cull_mode = DkCullMode_None;
	DkFrontFace front_face = DkFrontFace_CCW;
};

// Helper to setup basic render state
inline void setup_basic_render_state(dk::CmdBuf &cmdbuf, const DkPipelineState &state) {
	// Set primitive topology
	cmdbuf.bindPrimitive(state.primitive);
	
	// Set culling
	cmdbuf.setCullMode(state.cull_mode);
	cmdbuf.setFrontFace(state.front_face);
	
	// Set depth test
	if (state.depth_test_enabled) {
		cmdbuf.setDepthTestEnable(true);
		cmdbuf.setDepthWriteEnable(true);
		cmdbuf.setDepthCompareOp(DkCompareOp_Less);
	} else {
		cmdbuf.setDepthTestEnable(false);
		cmdbuf.setDepthWriteEnable(false);
	}
	
	// Set blending
	if (state.blend_enabled) {
		cmdbuf.setBlendEnable(0, true);
		cmdbuf.setBlendOp(0, state.blend_op, state.blend_op);
		cmdbuf.setBlendFunc(0, state.src_blend, state.dst_blend, state.src_blend, state.dst_blend);
	} else {
		cmdbuf.setBlendEnable(0, false);
	}
}

// Common vertex attribute formats
struct VertexAttribute {
	uint32_t buffer_index;
	uint32_t offset;
	DkVtxAttribSize size;
	DkVtxAttribType type;
	bool normalized;
};

// Helper to setup vertex attributes
inline void setup_vertex_attributes(dk::CmdBuf &cmdbuf, const VertexAttribute *attributes, uint32_t count) {
	for (uint32_t i = 0; i < count; i++) {
		cmdbuf.bindVtxAttribState(i, 1);
		cmdbuf.bindVtxAttrib(i, attributes[i].buffer_index, attributes[i].offset);
		cmdbuf.setVtxAttribFmt(i, attributes[i].size, attributes[i].type, attributes[i].normalized);
	}
}

#endif // __SWITCH__

#endif // DK_PIPELINE_H
