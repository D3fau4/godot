# Godot Engine - Nintendo Switch Platform

This directory contains the Nintendo Switch platform implementation for Godot Engine, targeting homebrew development using devkitPro, libnx, and deko3d.

## 📚 Documentation

- **[Quick Start Guide](QUICK_START.md)** - Get up and running in 5 steps
- **[Port Documentation](../../docs/PORT_SWITCH.md)** - Comprehensive build and deployment guide
- **[Implementation Notes](IMPLEMENTATION_NOTES.md)** - Technical architecture details
- **[Examples](examples/README.md)** - Sample code and project configurations

## 🎮 Features

### ✅ Working
- **Graphics**: OpenGL ES 3.0 via EGL
- **Graphics (Experimental)**: deko3d rendering backend (basic operations)
- **Input**: SDL2 GameController API with Joy-Con support
- **Audio**: SDL2 audio backend (48kHz stereo)
- **Display**: 1280x720 fullscreen rendering
- **System**: File I/O, timing, basic OS functions

### 🚧 In Progress
- Complete deko3d rendering pipeline
- Shader auto-conversion
- Performance profiling tools

### ❌ Not Implemented
- Editor support (use PC to edit, Switch to run)
- Network functionality
- Touch screen input
- Motion controls
- HD rumble

## 🔧 Quick Build

```bash
# Install devkitPro + packages
export DEVKITPRO=/opt/devkitpro
sudo dkp-pacman -S switch-dev switch-sdl2 deko3d libnx

# Build Godot
scons platform=switch target=release_debug -j4

# Output: bin/switch_release_debug.nro
```

## 📁 Directory Structure

```
platform/switch/
├── README.md                    # This file
├── QUICK_START.md              # Quick start guide
├── IMPLEMENTATION_NOTES.md     # Technical details
│
├── detect.py                   # Platform detection for SCons
├── SCsub                       # Build script
│
├── godot_switch.cpp            # Main entry point
├── os_switch.cpp/h             # OS abstraction layer
├── display_server_switch.*     # Display and window management
├── joypad_switch.*             # Input system (SDL2 + libnx)
├── switch_audio.*              # Audio backend (SDL2)
├── switch_test_scene.*         # Built-in test scene
├── context_gl_switch_egl.*     # OpenGL ES context
│
├── rendering/switch/           # Deko3d rendering backend
│   ├── README.md              # Backend documentation
│   ├── rd_deko3d.*            # RenderingDevice implementation
│   ├── dk_memory.h            # Memory management helpers
│   ├── dk_pipeline.h          # Pipeline helpers
│   └── shaders/               # Shader sources
│       ├── triangle.vert/frag # Basic shaders
│       └── build_shaders.py   # Build script
│
├── export/                     # Export plugin
├── examples/                   # Example code
└── romfs/                      # ROM filesystem data
```

## 🎯 Target Specifications

- **Platform**: Nintendo Switch (Homebrew)
- **CPU**: ARM Cortex-A57 (4 cores @ 1.02 GHz)
- **GPU**: NVIDIA Tegra X1 (Maxwell, 256 CUDA cores)
- **RAM**: 4 GB (shared CPU/GPU)
- **Resolution**: 1280×720 (docked and handheld)
- **Storage**: SD card via sdmc:/

## 🛠️ Development Tools

### Required
- **devkitPro** - ARM toolchain
- **devkitA64** - ARM64 compiler
- **libnx** - Switch homebrew library
- **SDL2** - Input/audio/window
- **deko3d** - GPU rendering (optional, has EGL fallback)
- **SCons** - Build system

### Optional
- **nxlink** - Network loading and console output
- **Atmosphere** - Custom firmware for Switch
- **Yuzu/Ryujinx** - Emulators (limited testing support)

## 🔬 Architecture

```
┌─────────────────────────────────────┐
│   Godot Core Engine                 │
│   (scene system, nodes, etc.)       │
└────────────┬────────────────────────┘
             │
┌────────────▼────────────────────────┐
│   Platform Layer (Switch)           │
│                                      │
│   ┌──────────┐  ┌────────────────┐ │
│   │ OS Layer │  │ Display Server │ │
│   └────┬─────┘  └────┬───────────┘ │
│        │             │              │
│   ┌────▼─────┐  ┌───▼──────┐      │
│   │SDL2 Core │  │ Deko3d   │      │
│   │(I/A/W)   │  │ or EGL   │      │
│   └──────────┘  └──────────┘      │
└─────────────────────────────────────┘
             │
┌────────────▼────────────────────────┐
│   Switch Hardware                    │
│   (Tegra X1, Joy-Con, etc.)         │
└─────────────────────────────────────┘

I/A/W = Input/Audio/Window
```

## 📊 Performance Guidelines

### Target Performance
- **FPS**: 60 FPS (VSync enabled)
- **Memory**: < 1 GB for small games
- **Load Time**: < 5 seconds

### Optimization Checklist
- ✅ Use texture compression (ETC2)
- ✅ Enable mipmaps for 3D
- ✅ Batch draw calls
- ✅ Use object pooling
- ✅ Profile regularly
- ✅ Test on real hardware

## 🧪 Testing

### Built-in Test Scene

The platform includes a test scene that verifies:
- Clear color rendering (dark blue)
- FPS counter (60 FPS target)
- Input handling (analog sticks)
- Exit functionality (+ button)

### Running Tests

```bash
# Build and run test
scons platform=switch target=release_debug
# Deploy to Switch and launch

# Expected output (console):
SDL2: Initialized successfully
deko3d: Device initialized successfully
FPS: 60.0
Press + button to exit
```

## 🐛 Common Issues

| Issue | Solution |
|-------|----------|
| Black screen | Check romfs initialization |
| No input | Verify SDL2 init, reconnect controllers |
| Low FPS | Build with target=release, optimize game |
| Crashes | Check memory usage, use debug build |
| No audio | Verify SDL2 audio init in logs |

See [IMPLEMENTATION_NOTES.md](IMPLEMENTATION_NOTES.md) for detailed troubleshooting.

## 🤝 Contributing

We welcome contributions! Areas that need work:

**High Priority:**
- Complete deko3d rendering backend
- GLSL to deko3d shader conversion
- Performance optimization
- More examples

**Medium Priority:**
- Touch screen support
- Motion controls (gyro/accelerometer)
- HD rumble
- Better debugging tools

**Nice to Have:**
- Network support (if feasible)
- Compute shaders
- Multiple controller support

### How to Contribute

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on real Switch hardware
5. Submit a pull request

See [IMPLEMENTATION_NOTES.md](IMPLEMENTATION_NOTES.md) for coding guidelines.

## 📜 License

Godot Engine is licensed under the MIT license. See [LICENSE.txt](../../LICENSE.txt).

Additional libraries:
- **libnx**: ISC License
- **SDL2**: zlib License  
- **deko3d**: zlib License

## 🙏 Credits

- **Godot Engine contributors** - Core engine
- **devkitPro team** - Toolchain and libraries
- **fincs** - deko3d graphics API
- **libnx developers** - Switch homebrew library
- **SDL2 Switch port contributors** - SDL2 implementation
- **Community** - Testing and feedback

## 📞 Support

- **Documentation**: See files in this directory
- **Issues**: GitHub issue tracker
- **Community**: Godot forums and Discord
- **Homebrew**: ReSwitched Discord

## 🔗 Links

- [Godot Engine](https://godotengine.org)
- [devkitPro](https://devkitpro.org)
- [deko3d](https://github.com/devkitPro/deko3d)
- [libnx](https://github.com/switchbrew/libnx)
- [ReSwitched](https://reswitched.team)
- [SwitchBrew](https://switchbrew.org)

---

**Status**: Active Development  
**Version**: Godot 4.0.5  
**Last Updated**: November 2024

Happy homebrew development! 🎮✨
