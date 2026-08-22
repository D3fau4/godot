/**************************************************************************/
/*  gl_manager_nx.cpp                                                     */
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

#include "gl_manager_nx.h"

#if defined(NX_ENABLED) && defined(GLES3_ENABLED)

#include "core/string/print_string.h"
#include "switch_wrapper.h"

Error GLManagerNX::_create_surface() {
	NWindow *window = nwindowGetDefault();
	ERR_FAIL_NULL_V_MSG(window, ERR_CANT_CREATE, "NX: No default window available.");

	if (R_FAILED(nwindowSetDimensions(window, window_size.width, window_size.height))) {
		ERR_PRINT("NX: Could not set the native window dimensions.");
	}

	surface = eglCreateWindowSurface(display, config, window, nullptr);
	ERR_FAIL_COND_V_MSG(surface == EGL_NO_SURFACE, ERR_CANT_CREATE, vformat("NX: eglCreateWindowSurface failed: 0x%x.", (int)eglGetError()));

	ERR_FAIL_COND_V_MSG(!eglMakeCurrent(display, surface, surface, context), ERR_CANT_CREATE, vformat("NX: eglMakeCurrent failed: 0x%x.", (int)eglGetError()));

	eglSwapInterval(display, use_vsync ? 1 : 0);

	return OK;
}

void GLManagerNX::_destroy_surface() {
	if (surface == EGL_NO_SURFACE) {
		return;
	}

	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(display, surface);
	surface = EGL_NO_SURFACE;
}

Error GLManagerNX::initialize(const Vector2i &p_size) {
	window_size = p_size;

	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	ERR_FAIL_COND_V_MSG(display == EGL_NO_DISPLAY, ERR_CANT_CREATE, vformat("NX: eglGetDisplay failed: 0x%x.", (int)eglGetError()));

	if (!eglInitialize(display, nullptr, nullptr)) {
		display = EGL_NO_DISPLAY;
		ERR_FAIL_V_MSG(ERR_CANT_CREATE, "NX: eglInitialize failed.");
	}

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		cleanup();
		ERR_FAIL_V_MSG(ERR_CANT_CREATE, "NX: eglBindAPI failed.");
	}

	static const EGLint config_attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_NONE
	};

	EGLint num_configs = 0;
	if (!eglChooseConfig(display, config_attribs, &config, 1, &num_configs) || num_configs == 0) {
		cleanup();
		ERR_FAIL_V_MSG(ERR_CANT_CREATE, "NX: No suitable EGL config found.");
	}

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 0,
		EGL_NONE
	};

	context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
	if (context == EGL_NO_CONTEXT) {
		cleanup();
		ERR_FAIL_V_MSG(ERR_CANT_CREATE, "NX: eglCreateContext failed.");
	}

	Error err = _create_surface();
	if (err != OK) {
		cleanup();
		return err;
	}

	return OK;
}

void GLManagerNX::cleanup() {
	if (display == EGL_NO_DISPLAY) {
		return;
	}

	_destroy_surface();

	if (context != EGL_NO_CONTEXT) {
		eglDestroyContext(display, context);
		context = EGL_NO_CONTEXT;
	}

	eglTerminate(display);
	display = EGL_NO_DISPLAY;
	config = nullptr;
}

Error GLManagerNX::resize(const Vector2i &p_size) {
	ERR_FAIL_COND_V(display == EGL_NO_DISPLAY, ERR_UNCONFIGURED);

	if (p_size == window_size) {
		return OK;
	}

	_destroy_surface();
	window_size = p_size;

	return _create_surface();
}

void GLManagerNX::release_current() {
	if (display == EGL_NO_DISPLAY) {
		return;
	}
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void GLManagerNX::make_current() {
	if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
		return;
	}
	eglMakeCurrent(display, surface, surface, context);
}

void GLManagerNX::swap_buffers() {
	if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
		return;
	}
	eglSwapBuffers(display, surface);
}

void GLManagerNX::set_use_vsync(bool p_use) {
	use_vsync = p_use;
	if (display != EGL_NO_DISPLAY) {
		eglSwapInterval(display, p_use ? 1 : 0);
	}
}

GLManagerNX::~GLManagerNX() {
	cleanup();
}

#endif // NX_ENABLED && GLES3_ENABLED
