# NX (Nintendo Switch) platform port

This folder contains the C++ code for the Nintendo Switch homebrew platform port,
built with the devkitPro / devkitA64 toolchain and libnx.

## Requirements

Install devkitPro with the `switch-dev` and `switch-portlibs` groups:

```
dkp-pacman -S switch-dev switch-portlibs
```

`DEVKITPRO` must be set in the environment (for example `/opt/devkitpro`).

The build links against the portlibs copies of zlib, libpng, freetype, ogg,
vorbis, theora, webp, mbedtls, wslay, miniupnpc, enet, zstd and pcre2, so
`aarch64-none-elf-pkg-config` must be available. Mesa (`switch-mesa`) provides
the EGL/GLES3 driver and is part of the same group.

## Building

```
scons platform=nx target=template_release
scons platform=nx target=template_debug
```

Only export templates can be built; the editor is not supported on this platform.
The result is an ELF in `bin/`, which is turned into an NRO with the devkitPro
tools:

```
elf2nro bin/godot.nx.template_release.arm64.elf game.nro --romfsdir=romfs
```

The engine looks for the project data in this order:

1. A PCK embedded in the executable.
2. `romfs:/game.pck`.
3. `<executable>.pck` next to the NRO.

## Notes

- The renderer is Compatibility (OpenGL ES 3) only; there is no Vulkan driver
  for Switch homebrew. `rendering/renderer/rendering_method.nx` defaults to
  `gl_compatibility`.
- The window follows the console operation mode: 1280x720 in handheld,
  1920x1080 when docked.
- `user://` maps to `sdmc:/switch/godot/app_userdata/<project name>`.
- Touch input is only reported in handheld mode.
