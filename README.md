# Godot for Switch

It's a port of the Godot open source game engine to the Switch, via homebrew libraries provided by devkitPro/switchbrew.
See the original README-original.md for info about Godot.

## Releases
See [the GitHub releases page](https://github.com/Stary2001/godot/releases) for pre-built editors for Windows, macOS and Linux.

## How do I export my game?
The editor builds above add a `Switch` exporter to the list - it will generate a .nro and a .pck file that can be transferred to your Switch.
If additional debugging is required, sending the nro via nxlink (make sure to send it to the right place with the -p argument) will allow the output of the console to be viewed on PC.

## How to build?
You shouldn't need to build the engine if you use a release as the templates should be included - these instructions are for development!

[See the official docs](https://docs.godotengine.org/en/latest/development/compiling/)
for compilation instructions for every officially supported platform.

### For Switch:

Install these packages from [devkitPro pacman](https://devkitpro.org/wiki/devkitPro_pacman):
`switch-pkg-config switch-freetype switch-bulletphysics switch-libtheora switch-libpcre2 switch-mesa switch-opusfile switch-mbedtls switch-libwebp switch-libvpx switch-miniupnpc switch-libzstd switch-wslay`
then run `scons platform=switch` in the root of the repo. Add `target=release` to build for release instead of debug.

Then, to build an engine release for Switch,
run `./scripts/create-switch-release.sh` in the root of the repo.

Or to build an export template for the editor,
run `./scripts/create-switch-template.sh` in the root of the repo.

## How can I get help?
Either make an issue on this repo, or join the [Discord](https://discordapp.com/invite/yUC3rUk)!

The official documentation is hosted on [ReadTheDocs](https://docs.godotengine.org).
It is maintained by the Godot community in its own [GitHub repository](https://github.com/godotengine/godot-docs).

The [class reference](https://docs.godotengine.org/en/latest/classes/)
is also accessible from the Godot editor.

We also maintain official demos in their own [GitHub repository](https://github.com/godotengine/godot-demo-projects)
as well as a list of [awesome Godot community resources](https://github.com/godotengine/awesome-godot).

There are also a number of other
[learning resources](https://docs.godotengine.org/en/latest/community/tutorials.html)
provided by the community, such as text and video tutorials, demos, etc.
Consult the [community channels](https://godotengine.org/community)
for more information.

[![Actions Build Status](https://github.com/godotengine/godot/workflows/Godot/badge.svg?branch=master)](https://github.com/godotengine/godot/actions)
[![Code Triagers Badge](https://www.codetriage.com/godotengine/godot/badges/users.svg)](https://www.codetriage.com/godotengine/godot)
[![Translate on Weblate](https://hosted.weblate.org/widgets/godot-engine/-/godot/svg-badge.svg)](https://hosted.weblate.org/engage/godot-engine/?utm_source=widget)
[![TODOs](https://badgen.net/https/api.tickgit.com/badgen/github.com/godotengine/godot)](https://www.tickgit.com/browse?repo=github.com/godotengine/godot)
