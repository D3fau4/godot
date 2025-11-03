# Godot Engine - Nintendo Switch Port

This document describes how to build and run Godot Engine for Nintendo Switch using the homebrew toolchain.

## Overview

This port uses:
- **libnx**: Nintendo Switch homebrew library
- **devkitA64**: ARM64 toolchain from devkitPro
- **deko3d**: Modern GPU rendering API for Switch
- **SDL2**: Cross-platform library for input, audio, and window management
- **SCons**: Godot's build system

**Important**: This is a homebrew port and does NOT use Nintendo's official SDK or NVN graphics API.

## Prerequisites

### 1. Install devkitPro

Download and install devkitPro from: https://devkitpro.org/wiki/Getting_Started

Set up environment variables:
```bash
export DEVKITPRO=/opt/devkitpro
export DEVKITA64=$DEVKITPRO/devkitA64
export PATH=$DEVKITPRO/tools/bin:$PATH
```

### 2. Install Switch Development Packages

Using the devkitPro package manager (`dkp-pacman`), install required libraries:

```bash
# Core development tools
sudo dkp-pacman -S switch-dev

# Required libraries
sudo dkp-pacman -S switch-sdl2        # SDL2 for input/audio/window
sudo dkp-pacman -S switch-zlib        # Compression
sudo dkp-pacman -S switch-libpng      # PNG images
sudo dkp-pacman -S switch-freetype    # Font rendering
sudo dkp-pacman -S deko3d             # GPU rendering (if available)
sudo dkp-pacman -S libnx              # Switch homebrew library

# Optional but recommended
sudo dkp-pacman -S switch-mesa        # For EGL/OpenGL ES support
sudo dkp-pacman -S switch-glad        # OpenGL loader
```

### 3. Install Python and SCons

```bash
# Install Python 3
sudo apt-get install python3 python3-pip

# Install SCons
pip3 install scons
```

## Building Godot

### Build Command

From the Godot source directory:

```bash
scons platform=switch target=release_debug
```

Build options:
- `target=release_debug` - Optimized build with debug symbols (recommended for development)
- `target=release` - Fully optimized release build
- `target=debug` - Debug build with full debugging info (slow)

### Output Files

The build process generates:
- `bin/godot` - ELF executable
- `bin/switch_release_debug.nro` - Homebrew NRO file (this is what you run on Switch)

## Deploying to Switch

### 1. Prepare SD Card

Copy the `.nro` file to your Switch's SD card:
```
/switch/godot/switch_release_debug.nro
```

### 2. Launch from Homebrew Menu

1. Insert SD card into Switch
2. Launch Homebrew Menu (hold R while starting a game)
3. Navigate to `/switch/godot/`
4. Launch `switch_release_debug.nro`

## Architecture

### Platform Integration

The Switch platform implementation is located in `platform/switch/`:

- **detect.py** - SCons platform detection and configuration
- **SCsub** - Build rules for Switch platform
- **godot_switch.cpp** - Main entry point
- **os_switch.cpp/h** - OS abstraction (filesystem, time, etc.)
- **display_server_switch.cpp/h** - Display and window management via SDL2
- **joypad_switch.cpp/h** - Input handling with Joy-Con support via SDL2
- **switch_audio.cpp/h** - Audio backend using SDL2
- **context_gl_switch_egl.cpp/h** - OpenGL ES 3.0 context via EGL
- **rendering/switch/rd_deko3d.cpp/h** - deko3d rendering backend (minimal)

### Rendering Backend

Currently supports:
- **OpenGL ES 3.0** via EGL (default)
- **deko3d** - Minimal implementation for basic rendering

The deko3d backend provides:
- Device initialization
- Command buffer management
- Swapchain for double buffering
- Basic clear operations
- VSync support

### Input Mapping

Joy-Con buttons are mapped to standard gamepad inputs:
- **A/B/X/Y** - Face buttons
- **L/R** - Shoulder buttons
- **ZL/ZR** - Triggers
- **+/-** - Start/Select (Plus/Minus)
- **Left/Right Stick** - Analog input with click
- **D-Pad** - Directional buttons

### Audio

Audio is handled via SDL2:
- **Sample Rate**: 48kHz
- **Format**: 16-bit stereo
- **Buffer Size**: 1024 samples

### Display

- **Resolution**: 1280x720 (docked and handheld)
- **Refresh Rate**: 60Hz
- **VSync**: Enabled by default

## Performance Considerations

### Optimization Tips

1. **Use release_debug or release builds** - Debug builds are very slow
2. **Enable LTO**: `scons platform=switch target=release use_lto=yes`
3. **Limit draw calls** - Switch GPU is mobile-class
4. **Use texture compression** - Reduces memory bandwidth
5. **Profile your game** - Use built-in profiler to identify bottlenecks

### Memory Constraints

The Switch has:
- **4GB total RAM** (3.2GB available to applications)
- **Shared memory** between CPU and GPU
- Keep textures and assets optimized

### Known Limitations

1. **No network support** - Compiled with `NO_NETWORK` flag
2. **No editor** - Tools are disabled (`tools=no`)
3. **Limited shader support** - GLES3 shaders only (deko3d shaders need conversion)
4. **Single window** - No multi-window support
5. **Fixed resolution** - 1280x720, no dynamic resolution scaling

## Test Scene

A basic test scene is included that demonstrates:
- Clear color rendering
- FPS counter display
- Text rendering ("Hello Switch")
- Input handling (stick movement, button press to exit)
- Audio playback (test beep)

To test:
1. Build the engine
2. Deploy to Switch
3. Launch the .nro file
4. You should see "Hello Switch" with FPS counter
5. Use + button to exit

## Troubleshooting

### Build Issues

**Error: "DEVKITPRO not defined"**
- Ensure devkitPro is installed and environment variables are set

**Error: "SDL2 not found"**
- Install switch-sdl2: `sudo dkp-pacman -S switch-sdl2`

**Error: "deko3d not found"**
- Check if deko3d is in portlibs: `/opt/devkitpro/portlibs/switch/lib/libdeko3d.a`

### Runtime Issues

**Black screen on launch**
- Check if romfs directory exists
- Verify .nro is properly built with nacptool

**No input response**
- Ensure controllers are connected
- Check SDL2 initialization logs

**Crashes on startup**
- Try debug build for more error messages
- Check SD card has enough free space

## Development Tips

### Debugging

Since there's no debugger, use print statements:
```cpp
print_line("Debug message");
print_line(vformat("Value: %d", some_value));
```

Logs are visible via:
- nxlink (network logging)
- Serial console (if available)

### Building Custom Projects

To export your Godot project for Switch:
1. Build the custom template
2. Place the .nro in your project's export folder
3. Configure export presets in Godot editor (on PC)
4. Export project files
5. Package with the .nro

## Contributing

Contributions are welcome! Areas that need work:
- Full deko3d rendering backend
- Shader conversion pipeline (GLSL → deko3d)
- Improved performance profiling
- Better memory management
- Network support (if feasible)

## License

Godot Engine is licensed under the MIT license. See LICENSE.txt for details.

This port uses additional libraries with their own licenses:
- libnx: ISC License
- SDL2: zlib License
- deko3d: zlib License

## Credits

- Godot Engine contributors
- devkitPro team
- libnx developers
- fincs (deko3d)
- SDL2 Switch port contributors

## Support

For issues specific to the Switch port:
- Open an issue on GitHub
- Check the Godot community forums
- Join the ReSwitched Discord for homebrew development help

---

**Last Updated**: November 2024
**Godot Version**: 4.0.5
**Platform**: Nintendo Switch (Homebrew)
