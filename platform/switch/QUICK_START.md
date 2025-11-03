# Quick Start Guide - Godot for Nintendo Switch

Get up and running with Godot on Nintendo Switch in 5 steps!

## Prerequisites

- Nintendo Switch with homebrew access (Atmosphere CFW recommended)
- SD card (at least 2GB free)
- Computer with devkitPro installed
- Basic knowledge of terminal/command line

## Step 1: Install Development Tools (10 minutes)

### On Linux/macOS:

```bash
# Install devkitPro package manager
# Follow instructions at: https://devkitpro.org/wiki/Getting_Started

# Set environment variables (add to ~/.bashrc or ~/.zshrc)
export DEVKITPRO=/opt/devkitpro
export DEVKITA64=$DEVKITPRO/devkitA64
export PATH=$DEVKITPRO/tools/bin:$PATH

# Install Switch development packages
sudo dkp-pacman -S switch-dev switch-sdl2 switch-zlib switch-libpng \
                  switch-freetype switch-mesa deko3d libnx

# Install Python and SCons
pip3 install scons
```

### On Windows:

1. Download devkitPro installer from https://devkitpro.org
2. Install to `C:\devkitpro`
3. Add to PATH: `C:\devkitpro\tools\bin`
4. Install MSYS2 packages via devkitPro's pacman
5. Install Python 3 and SCons

## Step 2: Get Godot Source Code (2 minutes)

```bash
# Clone this repository
git clone https://github.com/your-username/godot.git
cd godot

# Or if you already have it, just switch to the switch branch
git checkout switch-port
```

## Step 3: Build Godot for Switch (5-20 minutes)

```bash
# Build release_debug version (recommended for testing)
scons platform=switch target=release_debug -j4

# Or build optimized release version
scons platform=switch target=release -j4

# Output will be at: bin/switch_release_debug.nro
```

**Expected output:**
```
[...]
Linking Program: bin/godot
Converting to NRO: bin/switch_release_debug.nro
scons: done building targets.
```

## Step 4: Deploy to Switch (2 minutes)

### Method A: SD Card (Recommended)

1. Copy `bin/switch_release_debug.nro` to your SD card:
   ```
   /switch/godot/switch_release_debug.nro
   ```

2. Safely eject SD card
3. Insert SD card into Switch
4. Launch Homebrew Menu (hold R while launching a game)
5. Navigate to Godot and launch it

### Method B: Network (Faster for iteration)

```bash
# On your computer, start nxlink
nxlink -a [SWITCH_IP] -s bin/switch_release_debug.nro

# On Switch, have nxlink stub running
# This will load the .nro over network
```

## Step 5: Test and Verify (1 minute)

When you launch the .nro on Switch, you should see:

1. **Console output** (if using nxlink):
   ```
   SDL2: Initialized successfully
   deko3d: Device initialized successfully
   deko3d: Swapchain initialized - 1280x720
   SDL2 Audio: Initialized - 48000 Hz, 2 channels
   SDL2: Found X joysticks
   SDL2: Opened controller 0: Pro Controller
   ```

2. **Dark blue screen** - This is the test scene background

3. **FPS counter** - Printed to console every second
   ```
   FPS: 60.0
   FPS: 60.0
   ```

4. **Responsive input** - Move the analog sticks (should show in logs)

5. **Exit works** - Press + button to quit back to Homebrew Menu

## Next Steps

### Create Your First Game

See `platform/switch/examples/README.md` for:
- Minimal project setup
- Controller input examples
- Audio examples
- Performance tips

### Export an Existing Game

1. Open your Godot project in the editor (on PC)
2. Export as PCK: `Project → Export → Export PCK/ZIP`
3. Copy both the engine .nro and game .pck to Switch:
   ```
   /switch/my_game/
   ├── switch_release_debug.nro
   └── my_game.pck
   ```
4. The engine will automatically load the .pck file

## Troubleshooting

### Build Fails

**Error: "DEVKITPRO not defined"**
```bash
# Set environment variable
export DEVKITPRO=/opt/devkitpro
```

**Error: "SDL2 not found"**
```bash
# Install SDL2
sudo dkp-pacman -S switch-sdl2
```

**Error: "scons: command not found"**
```bash
# Install SCons
pip3 install scons
```

### Switch Issues

**Black screen on launch**
- Check SD card has the .nro file
- Verify file is not corrupted (redownload)
- Try launching from a different folder

**Crashes immediately**
- SD card might be full
- Check for romfs directory (should be auto-created)
- Try debug build: `scons platform=switch target=debug`

**No input response**
- Detach and reattach controllers
- Try pressing L+R to connect as Player 1
- Check SDL_NumJoysticks() in logs

**No audio**
- Audio may not be initialized (check logs)
- Some games require audio files
- Try adjusting volume in Switch system settings

## Performance Optimization

Once you have a working build:

1. **Use release builds for performance:**
   ```bash
   scons platform=switch target=release optimize=speed
   ```

2. **Enable LTO for smaller binary:**
   ```bash
   scons platform=switch target=release use_lto=yes
   ```

3. **Profile your game:**
   - Use Godot's built-in profiler
   - Check FPS counter
   - Monitor memory usage

## Getting Help

If you're stuck:

1. Check the full documentation: `docs/PORT_SWITCH.md`
2. Review implementation notes: `platform/switch/IMPLEMENTATION_NOTES.md`
3. Look at examples: `platform/switch/examples/`
4. Open an issue on GitHub
5. Ask in Godot community forums

## Useful Commands

```bash
# Clean build
scons platform=switch --clean

# Build with more verbose output
scons platform=switch target=release_debug verbose=yes

# Build with specific number of jobs
scons platform=switch -j8

# Check if platform is detected
scons platform=switch --help
```

## Development Workflow

Recommended workflow for fast iteration:

1. **Edit code** on your PC
2. **Build** with `scons platform=switch target=release_debug -j4`
3. **Deploy** via network with `nxlink` (fastest) or SD card
4. **Test** on Switch, check console output
5. **Repeat** - typical cycle time: 30 seconds with nxlink

## What's Next?

- **Create a full game** - Use Godot's node system
- **Optimize performance** - Profile and iterate
- **Add custom assets** - Textures, sounds, fonts
- **Share your game** - Package as standalone .nro
- **Contribute** - Help improve the Switch port!

## Resources

- [Full Documentation](../../docs/PORT_SWITCH.md)
- [devkitPro Documentation](https://devkitpro.org/wiki)
- [Godot Engine Docs](https://docs.godotengine.org)
- [ReSwitched Discord](https://reswitched.team) - For homebrew help

---

**Happy developing on Switch! 🎮**

If this guide helped you, consider contributing back to the project!
