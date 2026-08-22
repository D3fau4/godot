import os
import sys
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from SCons import Environment

MSYS_DEVKITPRO = "/opt/devkitpro"

# The NVK archive that must be linked whole: its dispatch tables are weak
# symbols, and an archive member reached only by a weak reference is never
# pulled in, which leaves a NULL entry point instead of a link error.
NVK_WHOLE_LIB = "src/nouveau/vulkan/libnvk.a"

# The rest of the mesa-nvk-horizon link line, in the order its meson.build
# lists them. They have cycles, hence the group.
NVK_LIBS = [
    "src/nouveau/rust_runtime/libnouveau_rust_runtime.a",
    "src/nouveau/compiler/libnak.a",
    "src/nouveau/nil/liblibnil_format_table.a",
    "src/nouveau/mme/libnouveau_mme.a",
    "src/nouveau/headers/libnvidia_headers_c.a",
    "src/vulkan/util/libvulkan_util.a",
    "src/vulkan/wsi/libvulkan_wsi.a",
    "src/compiler/spirv/libvtn.a",
    "src/compiler/nir/libnir.a",
    "src/compiler/libcompiler.a",
    "src/compiler/rust/libcompiler_c_helpers.a",
    "src/util/libxmlconfig.a",
    "src/util/libmesa_util.a",
    "src/util/libmesa_util_simd.a",
    "src/util/blake3/libblake3.a",
    "src/c11/impl/libmesa_util_c11.a",
]

HORIZON_LIBS = ["lib/libhorizon_gpu.a", "lib/libhorizon_compat.a"]


def is_active():
    return True


def get_name():
    return "NX"


def get_devkitpro_path():
    path = os.environ.get("DEVKITPRO", "")
    if not path:
        return ""

    path = path.replace("\\", "/").rstrip("/")
    if os.path.isdir(os.path.join(path, "devkitA64")):
        return path

    if os.name == "nt" and path.lower().endswith(MSYS_DEVKITPRO):
        for candidate in ["C:/devkitPro", "C:/devkitpro"]:
            if os.path.isdir(candidate):
                return candidate

    return path


def _arg(name, env_var):
    from SCons.Script import ARGUMENTS

    path = ARGUMENTS.get(name, os.environ.get(env_var, ""))
    return path.replace("\\", "/").rstrip("/")


def get_nvk_path():
    return _arg("nvk_path", "NVK_PATH")


def get_horizon_path():
    path = _arg("horizon_path", "HORIZON_PATH")
    if path:
        return path

    nvk = get_nvk_path()
    if not nvk:
        return ""

    # scripts/package-horizon.sh writes build/pkg beside build/mesa-nvk.
    return os.path.dirname(nvk) + "/pkg"


def has_nvk():
    nvk = get_nvk_path()
    return bool(nvk) and os.path.isfile(os.path.join(nvk, NVK_WHOLE_LIB))


def can_build():
    path = get_devkitpro_path()

    if not path:
        print("DEVKITPRO not defined in environment. NX disabled.")
        return False

    if not os.path.isdir(os.path.join(path, "devkitA64")):
        print("devkitA64 not found in %s. NX disabled." % path)
        return False

    if not os.path.isfile(os.path.join(path, "portlibs", "switch", "lib", "libz.a")):
        print("switch-portlibs not found in %s. NX disabled." % path)
        return False

    return True


def get_opts():
    from SCons.Variables import BoolVariable

    return [
        BoolVariable("touch", "Enable touch events", True),
        BoolVariable("nxlink", "Redirect stdout/stderr to nxlink on debug builds", True),
        ("nvk_path", "Path to a mesa-nvk-horizon NVK build directory; enables the Vulkan driver", get_nvk_path()),
        ("horizon_path", "Path to a packaged horizon_gpu tree; defaults to pkg/ beside nvk_path", get_horizon_path()),
    ]


def get_flags():
    return [
        ("arch", "arm64"),
        ("target", "template_release"),
        ("vulkan", has_nvk()),
        # One Mesa per binary: switch-mesa's GLES3 driver and NVK define the
        # same util symbols, so linking both is a multiple-definition error.
        ("opengl3", not has_nvk()),
        ("use_volk", has_nvk()),
        ("builtin_enet", False),
        ("builtin_libogg", False),
        ("builtin_libtheora", False),
        ("builtin_libvorbis", False),
        ("builtin_libwebp", False),
        ("builtin_mbedtls", False),
        ("builtin_miniupnpc", False),
        ("builtin_pcre2", False),
        ("builtin_wslay", False),
        ("builtin_zstd", False),
        ("builtin_pcre2_with_jit", False),
        ("module_denoise_enabled", False),
        ("module_lightmapper_rd_enabled", False),
        ("module_raycast_enabled", False),
        ("module_openxr_enabled", False),
        ("module_mono_enabled", False),
    ]


def configure(env: "Environment"):
    supported_arches = ["arm64"]
    if env["arch"] not in supported_arches:
        print(
            'Unsupported CPU architecture "%s" for NX. Supported architectures are: %s.'
            % (env["arch"], ", ".join(supported_arches))
        )
        sys.exit(255)

    devkitpro = get_devkitpro_path()
    devkita64 = devkitpro + "/devkitA64"
    libnx = devkitpro + "/libnx"
    portlibs = devkitpro + "/portlibs/switch"

    env["ENV"]["DEVKITPRO"] = devkitpro
    env["ENV"]["DEVKITA64"] = devkita64
    env.PrependENVPath("PATH", devkitpro + "/tools/bin")
    env.PrependENVPath("PATH", portlibs + "/bin")
    env.PrependENVPath("PATH", devkita64 + "/bin")
    os.environ["PATH"] = env["ENV"]["PATH"]

    prefix = devkita64 + "/bin/aarch64-none-elf-"
    env["CC"] = prefix + "gcc"
    env["CXX"] = prefix + "g++"
    env["LINK"] = prefix + "g++"
    env["AS"] = prefix + "as"
    env["AR"] = prefix + "gcc-ar"
    env["RANLIB"] = prefix + "gcc-ranlib"
    env["STRIP"] = prefix + "strip"
    env["OBJCOPY"] = prefix + "objcopy"

    env["ASCOM"] = "$CC $CCFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS -c -o $TARGET $SOURCES"

    env["OBJPREFIX"] = ""
    env["OBJSUFFIX"] = ".o"
    env["SHOBJPREFIX"] = ""
    env["SHOBJSUFFIX"] = ".os"
    env["LIBPREFIX"] = "lib"
    env["LIBSUFFIX"] = ".a"
    env["LIBPREFIXES"] = ["$LIBPREFIX"]
    env["LIBSUFFIXES"] = ["$LIBSUFFIX"]
    env["PROGSUFFIX"] = ".elf"

    if os.name == "nt":
        env.use_windows_spawn_fix()

    arch_flags = ["-march=armv8-a+crc+crypto", "-mtune=cortex-a57", "-mtp=soft", "-fPIE", "-ftls-model=local-exec"]

    env.Append(CCFLAGS=arch_flags + ["-ffunction-sections", "-fdata-sections"])
    env.Append(LINKFLAGS=arch_flags + ["-specs={}/switch.specs".format(libnx), "-Wl,--gc-sections"])

    env.Prepend(CPPPATH=["#platform/nx"])
    env.Append(CCFLAGS=["-isystem", libnx + "/include", "-isystem", portlibs + "/include"])
    env.Append(LIBPATH=[libnx + "/lib", portlibs + "/lib"])

    env.Append(CPPDEFINES=["NX_ENABLED", "__SWITCH__", "PTHREAD_NO_RENAME", "UNIX_SOCKET_UNAVAILABLE"])

    if env["touch"]:
        env.Append(CPPDEFINES=["TOUCH_ENABLED"])

    if env["nxlink"] and env.debug_features:
        env.Append(CPPDEFINES=["NXLINK_ENABLED"])

    if env["vulkan"]:
        nvk = (env["nvk_path"] or get_nvk_path()).replace("\\", "/").rstrip("/")
        horizon = (env["horizon_path"] or get_horizon_path()).replace("\\", "/").rstrip("/")

        archives = [nvk + "/" + NVK_WHOLE_LIB] + [nvk + "/" + lib for lib in NVK_LIBS]
        archives += [horizon + "/" + lib for lib in HORIZON_LIBS]

        missing = [a for a in archives if not os.path.isfile(a)]
        if missing:
            print("Vulkan is enabled for NX but these mesa-nvk-horizon archives are missing:")
            for a in missing:
                print("  " + a)
            print("Build them with scripts/build-mesa-nvk.sh and scripts/package-horizon.sh,")
            print("then pass nvk_path= and horizon_path=.")
            sys.exit(255)

        env.Append(CPPDEFINES=["VULKAN_ENABLED", "VK_USE_PLATFORM_VI_NN"])
        env.Append(CCFLAGS=["-isystem", horizon + "/include"])

        # The whole driver goes in LINKFLAGS, ahead of the engine objects. The
        # only symbol the engine takes from it is vk_icdGetInstanceProcAddr,
        # which is in libnvk.a and therefore always present; everything else is
        # a reference between these archives, which the group resolves.
        env.Append(
            LINKFLAGS=[
                "-Wl,--start-group",
                "-Wl,--whole-archive",
                nvk + "/" + NVK_WHOLE_LIB,
                "-Wl,--no-whole-archive",
            ]
            + [nvk + "/" + lib for lib in NVK_LIBS]
            + [horizon + "/" + lib for lib in HORIZON_LIBS]
            + ["-L" + portlibs + "/lib", "-lstdc++", "-lzstd", "-lz", "-Wl,--end-group"]
        )

    if env["opengl3"]:
        env.Append(CPPDEFINES=["GLES3_ENABLED", "EGL_ENABLED"])
        env.Append(LIBS=["EGL", "GLESv2", "glapi", "drm_nouveau"])

    if env["builtin_freetype"] or env["builtin_libpng"] or env["builtin_zlib"]:
        env["builtin_freetype"] = True
        env["builtin_libpng"] = True
        env["builtin_zlib"] = True

    if not env["builtin_freetype"]:
        env.Append(CCFLAGS=["-isystem", portlibs + "/include/freetype2"])
        env.Append(LIBS=["freetype", "bz2"])

    if not env["builtin_libpng"]:
        env.Append(CCFLAGS=["-isystem", portlibs + "/include/libpng16"])
        env.Append(LIBS=["png16"])

    if not env["builtin_libtheora"]:
        env["builtin_libogg"] = False
        env["builtin_libvorbis"] = False
        env.Append(LIBS=["theora", "theoradec"])

    if not env["builtin_libvorbis"]:
        env["builtin_libogg"] = False
        env.Append(LIBS=["vorbisfile", "vorbis"])

    if not env["builtin_libogg"]:
        env.Append(LIBS=["ogg"])

    if not env["builtin_libwebp"]:
        env.Append(LIBS=["webp"])

    if not env["builtin_enet"]:
        env.Append(LIBS=["enet"])

    if not env["builtin_mbedtls"]:
        env.Append(LIBS=["mbedtls", "mbedx509", "mbedcrypto"])

    if not env["builtin_wslay"]:
        env.Append(LIBS=["wslay"])

    if not env["builtin_miniupnpc"]:
        env.Append(CCFLAGS=["-isystem", portlibs + "/include/miniupnpc"])
        env.Append(LIBS=["miniupnpc"])

    if not env["builtin_pcre2"]:
        env.Append(LIBS=["pcre2-32"])

    if not env["builtin_zstd"]:
        env.Append(LIBS=["zstd"])

    if not env["builtin_zlib"]:
        env.Append(LIBS=["z"])

    env.Append(LIBS=["nx", "m"])
