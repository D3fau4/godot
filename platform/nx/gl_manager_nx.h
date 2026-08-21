/**************************************************************************/
/*  gl_manager_nx.h                                                       */
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

#ifndef GL_MANAGER_NX_H
#define GL_MANAGER_NX_H

#if defined(NX_ENABLED) && defined(GLES3_ENABLED)

#include "core/error/error_list.h"
#include "core/math/vector2i.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

class GLManagerNX {
	EGLDisplay display = EGL_NO_DISPLAY;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext context = EGL_NO_CONTEXT;
	EGLConfig config = nullptr;

	Vector2i window_size;
	bool use_vsync = true;

	Error _create_surface();
	void _destroy_surface();

public:
	Error initialize(const Vector2i &p_size);
	void cleanup();

	Error resize(const Vector2i &p_size);
	Vector2i get_size() const { return window_size; }

	void release_current();
	void make_current();
	void swap_buffers();

	void set_use_vsync(bool p_use);
	bool is_using_vsync() const { return use_vsync; }

	void *get_context() const { return context; }
	void *get_display() const { return display; }
	void *get_config() const { return config; }

	GLManagerNX() {}
	~GLManagerNX();
};

#endif // NX_ENABLED && GLES3_ENABLED

#endif // GL_MANAGER_NX_H
