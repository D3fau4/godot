import os
import sys
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from SCons import Environment

MSYS_DEVKITPRO = "/opt/devkitpro"

# Where scripts/package-portlibs.sh installs the driver, relative to devkitPro.
NVK_LIBDIR = "portlibs/switch/lib/nvk"

# The NVK archive that must be linked whole: its dispatch tables are weak
# symbols, and an archive member reached only by a weak reference is never
# pulled in, which leaves a NULL entry point instead of a link error.
NVK_WHOLE_LIB = "nvk"

# The rest of the mesa-nvk-horizon link line, in the order nvk.pc lists them.
# They have cycles, hence the group.
NVK_LIBS = [
    "nouveau_rust_runtime",
    "nak",
    "libnil_format_table",
    "nouveau_mme",
    "nvidia_headers_c",
    "vulkan_util",
    "vulkan_wsi",
    "vtn",
    "nir",
    "compiler",
    "compiler_c_helpers",
    "xmlconfig",
    "mesa_util",
    "mesa_util_simd",
    "blake3",
    "mesa_util_c11",
]

HORIZON_LIBS = ["horizon_gpu", "horizon_compat"]


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


def get_nvk_libdir():
    devkitpro = get_devkitpro_path()
    return devkitpro + "/" + NVK_LIBDIR if devkitpro else ""


def missing_nvk_libs():
    libdir = get_nvk_libdir()
    if not libdir:
        return ["lib" + NVK_WHOLE_LIB + ".a"]

    missing = ["lib" + lib + ".a" for lib in [NVK_WHOLE_LIB] + NVK_LIBS]
    missing = [name for name in missing if not os.path.isfile(libdir + "/" + name)]

    portlibs = os.path.dirname(libdir)
    missing += [
        "lib" + lib + ".a" for lib in HORIZON_LIBS if not os.path.isfile(portlibs + "/lib" + lib + ".a")
    ]
    return missing


def has_nvk():
    libdir = get_nvk_libdir()
    return bool(libdir) and os.path.isfile(libdir + "/lib" + NVK_WHOLE_LIB + ".a")


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
        missing = missing_nvk_libs()
        if missing:
            print("Vulkan is enabled for NX but these mesa-nvk-horizon archives are missing:")
            for name in missing:
                print("  " + name)
            print("Install the driver into devkitPro with make install in the")
            print("mesa-nvk-horizon tree, or use an image that already ships it.")
            sys.exit(255)

        env.Append(CPPDEFINES=["VULKAN_ENABLED", "VK_USE_PLATFORM_VI_NN"])

        # The whole driver goes in LINKFLAGS, ahead of the engine objects. The
        # only symbol the engine takes from it is vk_icdGetInstanceProcAddr,
        # which is in libnvk.a and therefore always present; everything else is
        # a reference between these archives, which the group resolves. The
        # library path has to be repeated here because SCons emits LINKFLAGS
        # before the LIBPATH directories.
        env.Append(
            LINKFLAGS=[
                "-L" + portlibs + "/lib/nvk",
                "-L" + portlibs + "/lib",
                "-Wl,--start-group",
                "-Wl,--whole-archive",
                "-l" + NVK_WHOLE_LIB,
                "-Wl,--no-whole-archive",
            ]
            + ["-l" + lib for lib in NVK_LIBS]
            + ["-l" + lib for lib in HORIZON_LIBS]
            + ["-lstdc++", "-lzstd", "-lz", "-Wl,--end-group"]
        )

    if env["opengl3"]:
        env.Append(CPPDEFINES=["GLES3_ENABLED"])
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
