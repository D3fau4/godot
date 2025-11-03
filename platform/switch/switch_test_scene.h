/**************************************************************************/
/*  switch_test_scene.h                                                   */
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

#ifndef SWITCH_TEST_SCENE_H
#define SWITCH_TEST_SCENE_H

#ifdef __SWITCH__

#include "core/os/main_loop.h"
#include "core/os/os.h"
#include "core/input/input.h"

class SwitchTestScene : public MainLoop {
	GDCLASS(SwitchTestScene, MainLoop);

private:
	RID viewport;
	uint64_t frame_count = 0;
	uint64_t last_fps_update = 0;
	float current_fps = 0.0f;
	
	void _render_test_scene();

public:
	virtual void initialize() override;
	virtual bool process(double p_time) override;
	virtual void finalize() override;
	
	SwitchTestScene();
	~SwitchTestScene();
};

#endif // __SWITCH__

#endif // SWITCH_TEST_SCENE_H
