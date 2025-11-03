# Nintendo Switch Platform Implementation Notes

## Overview

This document describes the implementation details of the Godot Engine port for Nintendo Switch using the homebrew toolchain (libnx, deko3d, SDL2).

## Build System Integration

### Platform Detection (detect.py)

The `detect.py` file is the entry point for SCons platform detection:

**Key Functions:**
- `can_build()` - Checks for required dependencies (devkitPro, SDL2, deko3d)
- `get_opts()` - Returns platform-specific build options
- `get_flags()` - Returns default flags (tools=no, builtin libs configuration)
- `configure(env)` - Sets up compiler flags, includes, and libraries

**Environment Variables Required:**
- `DEVKITPRO` - Path to devkitPro installation
- `DEVKITA64` - Set automatically from DEVKITPRO

**Compiler Setup:**
- Compiler: `aarch64-none-elf-gcc/g++`
- Architecture: ARM64 (ARMv8-A, Cortex-A57)
- Flags: `-fPIE`, `-ffunction-sections`, `-fdata-sections`

### Build Script (SCsub)

Defines the list of source files to compile and sets up the .nro packaging:

```python
files = [
    "godot_switch.cpp",          # Main entry point
    "os_switch.cpp",             # OS abstraction
    "joypad_switch.cpp",         # Input handling
    "context_gl_switch_egl.cpp", # OpenGL ES context
    "display_server_switch.cpp", # Display/window management
    "switch_audio.cpp",          # Audio backend
    "switch_test_scene.cpp",     # Test scene
    "rendering/switch/rd_deko3d.cpp",  # Deko3d backend
]
```

**Post-build Steps:**
1. `elf2nro` - Converts ELF to NRO format
2. `nacptool` - Creates NACP metadata file

## Architecture Components

### 1. OS Layer (os_switch.cpp/h)

**Purpose:** Provides OS-level abstractions

**Key Responsibilities:**
- Initialize core systems (filesystem, networking, threads)
- SDL2 initialization and cleanup
- Audio driver registration
- Main loop execution
- Time management

**SDL2 Initialization:**
```cpp
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)
```

### 2. Display Server (display_server_switch.cpp/h)

**Purpose:** Manages window and display

**Key Features:**
- SDL2 window creation (1280x720 fullscreen)
- Event processing (quit, focus events)
- OpenGL ES 3.0 context via EGL
- Fixed resolution (Switch screen)

**Event Handling:**
- SDL_QUIT → WINDOW_EVENT_CLOSE_REQUEST
- SDL_WINDOWEVENT_FOCUS_GAINED → WINDOW_EVENT_FOCUS_IN
- SDL_WINDOWEVENT_FOCUS_LOST → WINDOW_EVENT_FOCUS_OUT

### 3. Input System (joypad_switch.cpp/h)

**Purpose:** Handles controller input

**Two Modes:**
1. **SDL2 GameController** (default, `use_sdl=true`)
   - Uses SDL2's game controller API
   - Automatic Joy-Con detection
   - Standard button mapping
   
2. **libnx PadState** (fallback, `use_sdl=false`)
   - Direct libnx pad API
   - Lower-level control
   - Legacy support

**Button Mapping (SDL2):**
```
SDL → Godot JoyButton
A → JoyButton::A
B → JoyButton::B
X → JoyButton::X
Y → JoyButton::Y
START (+) → JoyButton::START
BACK (-) → JoyButton::BACK
L/R → JoyButton::LEFT/RIGHT_SHOULDER
```

### 4. Audio System (switch_audio.cpp/h)

**Purpose:** Provides audio output via SDL2

**Configuration:**
- Sample Rate: 48kHz
- Format: 16-bit signed, little-endian
- Channels: Stereo (2)
- Buffer Size: 1024 samples

**Integration:**
- Registered as `AudioDriverSDL2`
- Callback function fills buffer from `audio_server_process()`

### 5. Rendering Backend (rendering/switch/rd_deko3d.cpp/h)

**Purpose:** GPU rendering via deko3d

**Architecture:**
```
RenderingDeviceDeko3D
├── Device (dk::UniqueDevice)
├── Queue (dk::UniqueQueue)
├── Command Buffer (dk::UniqueCmdBuf)
├── Memory Block (dk::UniqueMemBlock)
├── Swapchain (dk::UniqueSwapchain)
└── Framebuffers (DkImage[2])
```

**Rendering Flow:**
1. `begin_frame()` - Acquire next image, start recording commands
2. `clear_color()` - Clear framebuffer
3. Drawing commands (not yet fully implemented)
4. `end_frame()` - Finish recording, submit to queue
5. `present()` - Present to screen, wait if VSync enabled

**Memory Types:**
- Command buffers: CPU uncached, GPU cached
- Framebuffers: GPU cached, image memory

### 6. Test Scene (switch_test_scene.cpp/h)

**Purpose:** Minimal test application

**Features:**
- FPS counter (prints to console every second)
- Clear color (dark blue background)
- Exit on + button press
- Viewport creation

## File Organization

```
platform/switch/
├── detect.py                    # Platform detection
├── SCsub                        # Build rules
├── godot_switch.cpp            # Main entry point
├── os_switch.cpp/h             # OS abstraction
├── display_server_switch.cpp/h # Display/window
├── joypad_switch.cpp/h         # Input handling
├── context_gl_switch_egl.cpp/h # OpenGL context
├── switch_audio.cpp/h          # Audio backend
├── switch_test_scene.cpp/h     # Test scene
├── switch_wrapper.h            # libnx includes
├── rendering/switch/
│   ├── rd_deko3d.cpp/h        # Deko3d backend
│   ├── dk_memory.h            # Memory helpers
│   ├── dk_pipeline.h          # Pipeline helpers
│   ├── README.md              # Backend docs
│   └── shaders/
│       ├── triangle.vert      # Vertex shader
│       ├── triangle.frag      # Fragment shader
│       └── build_shaders.py   # Shader builder
├── export/                     # Export plugin
├── romfs/                      # ROM filesystem
└── *.png, *.jpg               # Icons/splash
```

## Dependencies

### Required Libraries (via devkitPro)

**Core:**
- `libnx` - Nintendo Switch homebrew library
- `switch-sdl2` - SDL2 port for Switch
- `deko3d` - GPU rendering library

**Support:**
- `switch-zlib` - Compression
- `switch-libpng` - PNG images
- `switch-freetype` - Fonts
- `switch-mesa` - EGL/OpenGL ES

### Compile-time Defines

```cpp
#define __SWITCH__              // Platform identifier
#define HORIZON_ENABLED         // Horizon OS (Switch OS)
#define GLES3_ENABLED          // OpenGL ES 3.0 support
#define LIBC_FILEIO_ENABLED    // Standard C file I/O
#define NO_NETWORK             // Network disabled
#define PTHREAD_NO_RENAME      // Don't rename pthread functions
```

## Integration Points with Godot Core

### Initialization Order

1. `main()` in godot_switch.cpp
2. `OS_SWITCH` constructor - Register display server
3. `OS_SWITCH::initialize_core()` - File I/O, threads, network
4. `OS_SWITCH::initialize()` - SDL2, audio driver
5. `Main::setup()` - Godot core initialization
6. `OS_SWITCH::initialize_joypads()` - Input system
7. `Main::start()` - Start main loop
8. `OS_SWITCH::run()` - Main loop iteration

### Main Loop

```cpp
while (appletMainLoop()) {
    joypad->process_joypads();     // Process input
    swkbdInlineUpdate(&keyboard);  // Update on-screen keyboard
    if (Main::iteration()) break;  // Run one frame
}
```

## Known Limitations

1. **No Editor Support** - Tools disabled (tools=no)
2. **No Network** - NO_NETWORK flag set
3. **Fixed Resolution** - 1280x720 only
4. **Single Window** - Multi-window not supported
5. **Limited Deko3d** - Only basic rendering operations
6. **Shader Conversion** - Manual process, not automated

## Performance Considerations

### Memory

**Switch RAM:**
- Total: 4 GB
- Available to apps: ~3.2 GB
- Shared between CPU and GPU

**Optimization:**
- Use texture compression (DXT, ASTC)
- Pre-allocate memory pools
- Minimize dynamic allocations
- Profile with Godot's built-in profiler

### GPU

**Tegra X1 GPU:**
- Mobile-class Maxwell architecture
- 256 CUDA cores @ 768 MHz (docked) / 384 MHz (handheld)
- Shared memory with CPU

**Best Practices:**
- Minimize draw calls (batch rendering)
- Use instancing where possible
- Optimize shaders (avoid complex branching)
- Use mipmaps for textures
- Profile with deko3d tools

### CPU

**ARM Cortex-A57:**
- 4 cores @ 1.02 GHz
- ARMv8-A architecture

**Optimization:**
- Use threading (Godot has built-in thread pool)
- Avoid excessive main thread work
- Profile with Performance class

## Debugging

### Console Output

**nxlink:** Network-based console output
```bash
nxlink -s switch_release_debug.nro
```

**Print functions:**
```cpp
print_line("Message");
print_line(vformat("Value: %d", value));
ERR_PRINT("Error message");
WARN_PRINT("Warning message");
```

### Common Issues

**Black Screen:**
- Check romfs initialization
- Verify .nro packaging
- Check GPU initialization logs

**No Input:**
- SDL2 initialization failed
- Controller not connected
- Check SDL_NumJoysticks()

**Crash on Startup:**
- Out of memory
- Missing libnx initialization
- Check appletMainLoop()

**Audio Issues:**
- SDL audio subsystem not initialized
- Wrong sample format
- Buffer size too small

## Future Improvements

### Short-term
- [ ] Complete deko3d rendering pipeline
- [ ] Shader auto-conversion (GLSL → deko3d)
- [ ] Better error handling
- [ ] Performance profiling tools

### Long-term
- [ ] Compute shader support
- [ ] Network support (if feasible)
- [ ] Touch screen input
- [ ] Motion controls (gyro, accelerometer)
- [ ] HD rumble
- [ ] Multiple controller support (8 players)
- [ ] Docked/handheld mode detection
- [ ] Dynamic resolution scaling

## Testing Checklist

Before releasing a build:

- [ ] Compile successfully with `scons platform=switch target=release_debug`
- [ ] .nro file generated correctly
- [ ] Launches on Switch (via Homebrew Menu)
- [ ] FPS counter displays and is stable
- [ ] Input responds (all buttons, sticks)
- [ ] Audio plays without crackling
- [ ] Can exit cleanly with + button
- [ ] No crashes during 5 minute run
- [ ] Memory usage is reasonable (<1 GB)

## Contributing

When contributing to the Switch port:

1. **Test on real hardware** - Emulators may not be accurate
2. **Follow Godot style** - Use clang-format
3. **Document changes** - Update this file and docs/PORT_SWITCH.md
4. **Minimize changes** - Keep platform-specific code isolated
5. **Use #ifdef __SWITCH__** - Don't affect other platforms

## References

- [Godot Engine Architecture](https://docs.godotengine.org/en/stable/development/cpp/index.html)
- [devkitPro Documentation](https://devkitpro.org/wiki)
- [libnx API Reference](https://switchbrew.github.io/libnx/)
- [deko3d Documentation](https://github.com/devkitPro/deko3d/wiki)
- [SDL2 Documentation](https://wiki.libsdl.org/SDL2/FrontPage)

---

Last Updated: November 2024
