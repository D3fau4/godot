# Deko3d Rendering Backend for Godot Switch Port

This directory contains the deko3d rendering backend implementation for Nintendo Switch.

## Overview

Deko3d is a modern, low-level graphics API for Nintendo Switch homebrew. It provides direct access to the GPU with minimal driver overhead, similar to Vulkan or Metal.

## Files

### Core Backend
- **rd_deko3d.h/cpp** - Main RenderingDevice implementation using deko3d
  - Device initialization and management
  - Queue and command buffer handling
  - Swapchain for double-buffered rendering
  - Basic rendering operations (clear, present)
  - VSync control

### Helper Modules
- **dk_memory.h** - Memory management utilities
  - Helper functions for creating memory blocks
  - Standard memory block sizes
  - Different memory types (CPU-accessible, GPU-only, image)

- **dk_pipeline.h** - Pipeline state management
  - Common pipeline configurations
  - Vertex attribute setup helpers
  - Render state helpers

### Shaders
- **shaders/triangle.vert** - Basic vertex shader
- **shaders/triangle.frag** - Basic fragment shader
- **shaders/build_shaders.py** - Shader compilation script

## Implementation Status

### ✅ Implemented
- Device initialization
- Queue creation (graphics queue)
- Command buffer management
- Swapchain setup (double buffering)
- Memory block allocation
- Clear color operations
- Frame presentation
- VSync control

### 🚧 In Progress
- Shader loading and pipeline creation
- Vertex buffer management
- Uniform buffer handling
- Texture support

### ❌ Not Implemented
- Compute shaders
- Multiple render targets
- Depth/stencil buffers
- Full SPIR-V to deko3d shader conversion
- Advanced rendering features

## Usage

The deko3d backend is automatically initialized when building for Switch:

```cpp
RenderingDeviceDeko3D *rd = memnew(RenderingDeviceDeko3D);
Error err = rd->initialize(1280, 720);
if (err == OK) {
    // Start rendering
    rd->begin_frame();
    rd->clear_color(Color(0.1, 0.1, 0.3, 1.0));
    // ... draw operations ...
    rd->end_frame();
    rd->present();
}
```

## Architecture

```
┌─────────────────────────────────────┐
│   Godot RenderingServer             │
│   (High-level rendering API)        │
└────────────┬────────────────────────┘
             │
             v
┌─────────────────────────────────────┐
│   RenderingDeviceDeko3D              │
│   (deko3d abstraction layer)        │
└────────────┬────────────────────────┘
             │
             v
┌─────────────────────────────────────┐
│   deko3d Library                     │
│   (Low-level GPU API)               │
└────────────┬────────────────────────┘
             │
             v
┌─────────────────────────────────────┐
│   Nintendo Switch GPU               │
│   (NVIDIA Tegra X1)                 │
└─────────────────────────────────────┘
```

## Performance Notes

### Memory Management
- Command buffers use uncached CPU memory for GPU efficiency
- Image memory is GPU-cached for fast rendering
- Avoid frequent allocations - pre-allocate pools

### Command Buffers
- Command buffers are reusable
- Clear commands between frames
- Submit work in batches to minimize overhead

### VSync
- VSync is enabled by default for smooth 60 FPS
- Disable for benchmarking (may cause tearing)

## Building Shaders

Shaders must be compiled to deko3d format:

```bash
cd platform/switch/rendering/switch/shaders
python3 build_shaders.py
```

This requires the `uam` shader compiler from devkitPro.

## Debugging

Enable debug output by building with:
```bash
scons platform=switch target=debug
```

This will print detailed initialization logs:
- Device creation
- Memory allocations
- Queue setup
- Swapchain configuration

## References

- [deko3d Documentation](https://github.com/devkitPro/deko3d/wiki)
- [deko3d Examples](https://github.com/devkitPro/deko3d-examples)
- [Switch Graphics Architecture](https://switchbrew.org/wiki/GPU)

## Contributing

When extending the deko3d backend:
1. Keep abstractions minimal - follow deko3d's low-level nature
2. Document memory management clearly
3. Test on real hardware (emulators may not be accurate)
4. Profile GPU performance using deko3d's built-in tools
5. Follow Godot's coding style

## Known Issues

1. **Shader Compilation** - Automatic GLSL to deko3d conversion not yet implemented
2. **Limited Features** - Only basic rendering operations supported
3. **No Compute** - Compute shaders not implemented
4. **Single Queue** - Only one graphics queue used (could optimize with transfer queue)

## Future Work

- Full RenderingDevice API implementation
- SPIR-V shader support via shader translation
- Texture compression (ASTC, DXT)
- Render pipeline caching
- Multi-threaded command buffer recording
- GPU profiling integration
- Memory defragmentation
