# Testing Checklist - Nintendo Switch Port

Use this checklist to verify the Switch port is working correctly.

## Pre-Testing Setup

### Environment Setup
- [ ] devkitPro installed at `/opt/devkitpro` (or equivalent)
- [ ] `DEVKITPRO` environment variable set
- [ ] `DEVKITA64` environment variable set
- [ ] devkitPro tools in PATH (`/opt/devkitpro/tools/bin`)

### Dependencies Installed
- [ ] `switch-dev` package installed
- [ ] `switch-sdl2` package installed
- [ ] `deko3d` library available (or use EGL fallback)
- [ ] `libnx` library installed
- [ ] `switch-zlib` installed
- [ ] `switch-libpng` installed
- [ ] `switch-freetype` installed
- [ ] `switch-mesa` installed (for EGL/GLES3)

### Build Tools
- [ ] Python 3.8+ installed
- [ ] SCons 4.0+ installed
- [ ] `aarch64-none-elf-gcc` compiler works
- [ ] `pkg-config` available

## Build Testing

### Initial Build
- [ ] Clone repository
- [ ] Navigate to godot directory
- [ ] Run: `scons platform=switch target=release_debug -j4`
- [ ] Build completes without errors
- [ ] No missing library errors
- [ ] No compilation errors

### Build Artifacts
- [ ] `bin/godot` ELF file created
- [ ] `bin/switch_release_debug.nro` created
- [ ] `platform/switch/nacp_default.nacp` created
- [ ] File sizes reasonable (NRO ~30-50 MB)

### Build Variants
- [ ] Debug build works: `scons platform=switch target=debug`
- [ ] Release build works: `scons platform=switch target=release`
- [ ] LTO build works: `scons platform=switch target=release use_lto=yes`

### Clean Build
- [ ] Clean works: `scons platform=switch --clean`
- [ ] Rebuild after clean succeeds

## Hardware Testing (Switch Required)

### Deployment
- [ ] SD card formatted (FAT32 or exFAT)
- [ ] Created directory: `/switch/godot/`
- [ ] Copied `switch_release_debug.nro` to SD card
- [ ] SD card safely ejected
- [ ] SD card inserted in Switch

### Launch
- [ ] Switch boots to CFW (Atmosphere, etc.)
- [ ] Launched Homebrew Menu (hold R + game)
- [ ] Godot appears in homebrew list
- [ ] Icon displays correctly (if provided)
- [ ] Metadata displays correctly

### Initial Startup
- [ ] Application launches (no crash)
- [ ] Console output visible (if using nxlink)
- [ ] Initialization messages appear:
  - [ ] "SDL2: Initialized successfully"
  - [ ] "deko3d: Device initialized" OR "EGL context initialized"
  - [ ] "SDL2 Audio: Initialized"
  - [ ] "SDL2: Found X joysticks"
  - [ ] "SDL2: Opened controller 0"

### Graphics Testing
- [ ] Screen displays (not black)
- [ ] Clear color shows (dark blue by default)
- [ ] No graphical glitches
- [ ] Screen refreshes smoothly
- [ ] No flickering
- [ ] Resolution correct (1280×720)

### Performance Testing
- [ ] FPS counter appears in console every second
- [ ] FPS is stable (~60 FPS)
- [ ] No frame drops during idle
- [ ] No sustained drops below 30 FPS
- [ ] VSync working (no tearing)

### Input Testing - Joy-Con (Handheld)
- [ ] Left stick X axis responds
- [ ] Left stick Y axis responds
- [ ] Right stick X axis responds
- [ ] Right stick Y axis responds
- [ ] A button responds
- [ ] B button responds
- [ ] X button responds
- [ ] Y button responds
- [ ] L button responds
- [ ] R button responds
- [ ] ZL trigger responds
- [ ] ZR trigger responds
- [ ] Plus (+) button responds
- [ ] Minus (-) button responds
- [ ] D-pad up/down/left/right respond
- [ ] Plus button exits application

### Input Testing - Separated Joy-Con
- [ ] Left Joy-Con connects
- [ ] Right Joy-Con connects
- [ ] Same button tests as handheld
- [ ] Both Joy-Cons work simultaneously

### Input Testing - Pro Controller
- [ ] Pro Controller connects
- [ ] All buttons respond
- [ ] Analog sticks work
- [ ] Same functionality as Joy-Con

### Audio Testing
- [ ] SDL audio initializes without errors
- [ ] No audio crackling
- [ ] No distortion
- [ ] Sample rate correct (48kHz in logs)
- [ ] Can play test audio (if available)
- [ ] Audio sync with video

### Stability Testing
- [ ] Runs for 1 minute without crash
- [ ] Runs for 5 minutes without crash
- [ ] Runs for 15 minutes without crash
- [ ] Memory usage stable (not increasing)
- [ ] No thermal throttling issues
- [ ] Battery drain reasonable

### Exit Testing
- [ ] Press + button to exit
- [ ] Application closes cleanly
- [ ] Returns to Homebrew Menu
- [ ] No system freeze
- [ ] Can relaunch immediately

## Advanced Testing

### Multiple Controllers
- [ ] Connect second controller
- [ ] Second controller detected
- [ ] Both controllers work simultaneously
- [ ] Input doesn't interfere

### Docked vs Handheld
- [ ] Works in handheld mode
- [ ] Works in docked mode
- [ ] No difference in behavior
- [ ] Performance similar in both modes

### Sleep/Wake
- [ ] Press power to sleep
- [ ] Wake Switch
- [ ] Application resumes correctly
- [ ] No crash after wake
- [ ] Input still works

### Home Menu
- [ ] Press Home button
- [ ] Switch to Home Menu
- [ ] Return to application
- [ ] Application continues
- [ ] No corruption

### Different Game Projects
- [ ] Place test .pck file next to .nro
- [ ] Launch application
- [ ] .pck file loads
- [ ] Game content appears
- [ ] Game functions correctly

## Error Scenarios

### Missing Dependencies
- [ ] Build fails gracefully if devkitPro missing
- [ ] Build fails gracefully if SDL2 missing
- [ ] Clear error messages
- [ ] Instructions provided

### Runtime Errors
- [ ] Handles corrupted .nro gracefully
- [ ] Handles missing romfs data
- [ ] Handles disconnected controllers
- [ ] Handles full SD card
- [ ] Error messages helpful

## Performance Benchmarks

### Frame Rate
- [ ] Average FPS: ______ (target: 60)
- [ ] Minimum FPS: ______ (target: >30)
- [ ] Frame time: ______ ms (target: <16.67ms)

### Memory Usage
- [ ] Startup memory: ______ MB
- [ ] After 5 min: ______ MB
- [ ] Peak memory: ______ MB
- [ ] Target: <1 GB for engine + small game

### Load Times
- [ ] Engine launch: ______ seconds (target: <2s)
- [ ] Scene load: ______ seconds (target: <5s)

## Regression Testing

After any code changes:
- [ ] Clean build succeeds
- [ ] Graphics still work
- [ ] Input still works
- [ ] Audio still works
- [ ] No new crashes
- [ ] Performance unchanged

## Documentation Verification

- [ ] README.md accurate
- [ ] QUICK_START.md works as written
- [ ] PORT_SWITCH.md comprehensive
- [ ] IMPLEMENTATION_NOTES.md accurate
- [ ] Examples work as shown
- [ ] Build instructions correct

## Known Issues Found

List any issues discovered during testing:

1. _______________________________________________
2. _______________________________________________
3. _______________________________________________

## Test Results

**Date**: _____________
**Tester**: _____________
**Hardware**: Switch (HAC-001/001(-01)/HDH-001)
**Firmware**: _____________
**CFW**: _____________
**Build**: release_debug / release / debug
**Overall Result**: PASS / FAIL / PARTIAL

## Notes

Additional observations:
_______________________________________________________
_______________________________________________________
_______________________________________________________

## Sign-off

After completing all tests:

- [ ] All critical tests passed
- [ ] Documented any failures
- [ ] Updated issue tracker
- [ ] Ready for next phase

**Tester Signature**: _____________
**Date**: _____________

---

## Quick Test (5 minutes)

If you only have time for a quick test:

1. [ ] Build succeeds
2. [ ] .nro file created
3. [ ] Launches on Switch
4. [ ] Graphics display
5. [ ] FPS ~60
6. [ ] Input works
7. [ ] Exits cleanly

**Quick Test Result**: PASS / FAIL

---

## Automated Tests (Future)

Placeholder for automated testing:

- [ ] Unit tests for platform code
- [ ] Integration tests for SDL2
- [ ] Rendering tests for deko3d
- [ ] Performance regression tests
- [ ] Memory leak tests

---

**Version**: 1.0
**Last Updated**: November 2024
