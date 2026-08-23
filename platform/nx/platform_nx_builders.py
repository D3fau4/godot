"""Functions used to generate source files during build time

All such functions are invoked in a subprocess on Windows to prevent build flakiness.

"""
from platform_methods import subprocess_main


def make_applet_splash(target, source, env):
    src = source[0]
    dst = target[0]

    with open(src, "rb") as f:
        buf = f.read()

    with open(dst, "w") as g:
        g.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
        g.write("#ifndef APPLET_SPLASH_GEN_H\n")
        g.write("#define APPLET_SPLASH_GEN_H\n")
        g.write("static const unsigned char applet_splash_rgba_gz[] = {\n")
        for i in range(len(buf)):
            g.write(str(buf[i]) + ",\n")
        g.write("};\n")
        g.write("#endif")


if __name__ == "__main__":
    subprocess_main(globals())
