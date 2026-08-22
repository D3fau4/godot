/**************************************************************************/
/*  godot_nx.cpp                                                          */
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

#include "main/main.h"
#include "os_nx.h"

#include <stdio.h>
#include <stdlib.h>

static bool romfs_mounted = false;

static void nx_services_init() {
	socketInitializeDefault();
#ifdef NXLINK_ENABLED
	nxlinkStdio();
#endif

	Result rc = romfsInit();
	romfs_mounted = R_SUCCEEDED(rc);
	if (!romfs_mounted) {
		printf("NX: romfsInit() failed (0x%08x), romfs:/game.pck will not be available.\n", rc);
		fflush(stdout);
	}

	setInitialize();
	csrngInitialize();
}

static void nx_services_exit() {
	csrngExit();
	setExit();
	if (romfs_mounted) {
		romfsExit();
	}
	socketExit();
}

int main(int argc, char *argv[]) {
	nx_services_init();

#ifdef VULKAN_ENABLED
	// NVK refuses to enumerate the Tegra X1 unless the application asks for it:
	// the driver is not conformant on this GPU, so it returns
	// VK_ERROR_INCOMPATIBLE_DRIVER and no physical device is reported. Set
	// before Main::setup(), which is what brings the display server and the
	// Vulkan instance up. An existing value is left alone.
	setenv("NVK_I_WANT_A_BROKEN_VULKAN_DRIVER", "1", 0);
#endif

	const char *execpath = (argc > 0 && argv[0]) ? argv[0] : "sdmc:/switch/godot.nro";
	const int arg_count = argc > 0 ? argc - 1 : 0;

	OS_NX os(execpath);

	Error err = Main::setup(execpath, arg_count, arg_count > 0 ? &argv[1] : nullptr);
	if (err != OK) {
		nx_services_exit();
		return err == ERR_HELP ? EXIT_SUCCESS : 255;
	}

	if (Main::start()) {
		os.set_exit_code(EXIT_SUCCESS);
		os.run();
	}

	Main::cleanup();

	const int exit_code = os.get_exit_code();

	nx_services_exit();

	return exit_code;
}
