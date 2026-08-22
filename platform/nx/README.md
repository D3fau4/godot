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

## Vulkan

A Vulkan build needs the driver from
[mesa-nvk-horizon](https://github.com/D3fau4/mesa-nvk-horizon), which is NVK with a
Horizon kernel-mode backend. There is no loader and no shared library on this platform:
the driver is linked into the executable and reached through its ICD entry point.

Build it there (`scripts/build-mesa-nvk.sh`, then `scripts/package-horizon.sh`) and
point this build at the result:

```
scons platform=nx target=template_release \
      nvk_path=/path/to/mesa-nvk-horizon/build/mesa-nvk \
      horizon_path=/path/to/mesa-nvk-horizon/build/pkg
```

`horizon_path` defaults to `pkg/` beside `nvk_path`, and both can also come from
`$NVK_PATH` and `$HORIZON_PATH`. Missing archives stop the build and are named.

Meson writes those archives thin: they hold paths to the object files rather than the
objects, resolved against the archive's own directory. Point `nvk_path` at the build
directory itself (`build/mesa-nvk`) so the `.a.p/` directories sit beside them. The
copies `scripts/package-horizon.sh` stages under `build/pkg/lib/` are not linkable on
their own for that reason; convert them first if you want a self-contained tree:

```
cd /path/to/mesa-nvk-horizon
aarch64-none-elf-ar t build/meson/libhorizon_gpu.a | xargs aarch64-none-elf-ar qcs out.a
```

Enabling Vulkan turns `opengl3` off: switch-mesa's GLES3 driver and NVK are two Mesa
builds that define the same symbols, so one executable cannot hold both. Pass
`opengl3=yes` to try anyway.

The driver and the toolchain must come from the same devkitA64, and the Mesa build must
have been made with `-fPIE` rather than `-fPIC`: on devkitA64 gcc 15.2.0 `-mtp=soft
-fPIC` miscompiles every thread-local access. `scripts/check-tls-relocs.sh` in
mesa-nvk-horizon is the check for it.

A project selects the renderer with `rendering/renderer/rendering_method.nx`
(`mobile` or `forward_plus`) and `rendering/rendering_device/driver.nx` (`vulkan`).

Vulkan needs the memory a title takeover gives (3155 MiB); the album applet's 237 MiB
is not enough.

Verified so far: the whole engine cross-compiles and **links** against a real NVK build
(Mesa 26.1.7 + the 81 mesa-nvk-horizon patches, built with
`mesa-nvk-horizon/nx-dev-mesa`), producing a 73.8 MB ELF and an NRO. The driver is
inside it — `nvkmd_horizon`, the Horizon WSI and NAK are all in the binary.

**It has never run on a console.** Nothing here has drawn a frame yet.

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

- The default renderer is Compatibility (OpenGL ES 3). Vulkan is available in a
  build made against mesa-nvk-horizon; see above.
- The window follows the console operation mode: 1280x720 in handheld,
  1920x1080 when docked.
- `user://` maps to `sdmc:/switch/godot/app_userdata/<project name>`.
- Touch input is only reported in handheld mode.
