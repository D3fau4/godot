import os
import sys
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from SCons import Environment


def is_active():
    return True


def get_name():
    return "NX"


def get_devkitpro_path():
    path = os.environ.get("DEVKITPRO", "")
    if not path:
        return ""

    path = path.replace("\\", "/").rstrip("/")
    if os.path.isdir(path):
        return path

    if os.name == "nt" and path.lower() == "/opt/devkitpro":
        for candidate in ["C:/devkitPro", "C:/devkitpro"]:
            if os.path.isdir(candidate):
                return candidate

    return path


def can_build():
    path = get_devkitpro_path()

    if not path:
        print("DEVKITPRO not defined in environment. NX disabled.")
        return False

    if not os.path.isdir(os.path.join(path, "devkitA64")):
        print("devkitA64 not found in %s. NX disabled." % path)
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
        ("vulkan", False),
        ("opengl3", True),
        ("use_volk", False),
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

    prefix = devkita64 + "/bin/aarch64-none-elf-"
    env["CC"] = prefix + "gcc"
    env["CXX"] = prefix + "g++"
    env["LINK"] = prefix + "g++"
    env["AS"] = prefix + "as"
    env["AR"] = prefix + "gcc-ar"
    env["RANLIB"] = prefix + "gcc-ranlib"
    env["STRIP"] = prefix + "strip"
    env["OBJCOPY"] = prefix + "objcopy"

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

    env.Append(CPPDEFINES=["NX_ENABLED", "__SWITCH__", "PTHREAD_NO_RENAME"])

    if env["touch"]:
        env.Append(CPPDEFINES=["TOUCH_ENABLED"])

    if env["nxlink"] and env.debug_features:
        env.Append(CPPDEFINES=["NXLINK_ENABLED"])

    if env["opengl3"]:
        env.Append(CPPDEFINES=["GLES3_ENABLED", "EGL_ENABLED"])
        env.Append(LIBS=["EGL", "GLESv2", "glapi", "drm_nouveau"])

    env.Append(LIBS=["nx", "m"])
