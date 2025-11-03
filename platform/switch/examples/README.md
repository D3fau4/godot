# Switch Platform Examples

This directory contains example configurations and code snippets for the Nintendo Switch port.

## Minimal Project

To create a minimal Godot project for Switch:

1. **Create project directory structure:**
```
my_switch_game/
├── project.godot
├── main.gd
└── icon.png (optional)
```

2. **project.godot** (minimal configuration):
```ini
[application]

config/name="My Switch Game"
run/main_scene="res://main.tscn"
config/icon="res://icon.png"

[display]

window/size/viewport_width=1280
window/size/viewport_height=720
window/size/resizable=false
window/size/fullscreen=true
window/vsync/vsync_mode=1

[input]

ui_accept={
"deadzone": 0.5,
"events": [Object(InputEventJoypadButton,"resource_local_to_scene":false,"resource_name":"","device":0,"button_index":0,"pressure":0.0,"pressed":false,"script":null)
]
}

ui_cancel={
"deadzone": 0.5,
"events": [Object(InputEventJoypadButton,"resource_local_to_scene":false,"resource_name":"","device":0,"button_index":1,"pressure":0.0,"pressed":false,"script":null)
]
}

[rendering]

renderer/rendering_method="gl_compatibility"
textures/vram_compression/import_etc2=true
textures/default_filters/use_nearest_mipmap_filter=true
```

3. **main.gd** (simple script):
```gdscript
extends Node2D

var fps_label: Label

func _ready():
    # Create FPS label
    fps_label = Label.new()
    fps_label.position = Vector2(10, 10)
    add_child(fps_label)
    
    print("Hello Switch!")
    print("Press + to exit")

func _process(_delta):
    # Update FPS
    fps_label.text = "FPS: %.1f" % Engine.get_frames_per_second()
    
    # Exit on + button (START button)
    if Input.is_joy_button_pressed(0, JOY_BUTTON_START):
        get_tree().quit()
```

## Controller Input Example

```gdscript
extends Node2D

func _process(_delta):
    # Check buttons
    if Input.is_joy_button_pressed(0, JOY_BUTTON_A):
        print("A button pressed")
    
    if Input.is_joy_button_pressed(0, JOY_BUTTON_B):
        print("B button pressed")
    
    # Read analog sticks
    var left_stick = Vector2(
        Input.get_joy_axis(0, JOY_AXIS_LEFT_X),
        Input.get_joy_axis(0, JOY_AXIS_LEFT_Y)
    )
    
    var right_stick = Vector2(
        Input.get_joy_axis(0, JOY_AXIS_RIGHT_X),
        Input.get_joy_axis(0, JOY_AXIS_RIGHT_Y)
    )
    
    # Move player with left stick
    if left_stick.length() > 0.1:
        position += left_stick * 200 * _delta
```

## Audio Example

```gdscript
extends Node

var audio_player: AudioStreamPlayer

func _ready():
    # Create audio player
    audio_player = AudioStreamPlayer.new()
    add_child(audio_player)
    
    # Load audio file
    var stream = load("res://sounds/beep.wav")
    audio_player.stream = stream
    
    # Play sound
    audio_player.play()

func _input(event):
    # Play sound on button press
    if event is InputEventJoypadButton and event.pressed:
        audio_player.play()
```

## Building and Deploying

### 1. Export PCK

From Godot Editor (on PC):
```
Project → Export → Add → Switch (create custom template)
Export PCK/ZIP → my_game.pck
```

### 2. Build Engine Template

```bash
cd godot
scons platform=switch target=release_debug
```

### 3. Package for Switch

The .nro file is automatically created at `bin/switch_release_debug.nro`

### 4. Combine Engine + Game

Copy both files to SD card:
```
/switch/my_game/
├── switch_release_debug.nro
└── my_game.pck
```

Note: The engine will automatically load `my_game.pck` if present in the same directory.

## Performance Tips

### Memory Management
```gdscript
# Preload resources
const TEXTURE = preload("res://texture.png")
const SCENE = preload("res://scene.tscn")

# Free unused resources
func _exit_tree():
    queue_free()
```

### Optimize Draw Calls
```gdscript
# Use TileMaps instead of many Sprite2D nodes
var tilemap = TileMap.new()

# Batch similar objects
var multimesh = MultiMeshInstance2D.new()
```

### Texture Optimization
- Use compressed textures (ETC2)
- Enable mipmaps for 3D textures
- Resize textures to power-of-2 sizes

## Common Issues

### Low FPS
- Reduce draw calls
- Optimize shaders
- Use simpler particle effects
- Enable texture compression

### High Memory Usage
- Unload unused scenes
- Use texture streaming
- Reduce texture sizes
- Profile with Godot's debugger

### Input Not Working
- Check controller is connected
- Verify button mappings in project settings
- Test with Input.get_connected_joypads()

### Audio Crackling
- Increase audio buffer size in project settings
- Use lower sample rate (22050 Hz)
- Avoid loading large audio files

## Testing Without Switch

You can test input mapping on PC:
1. Connect a gamepad (Xbox, PlayStation, etc.)
2. Use the same JOY_BUTTON constants
3. Test in editor before deploying to Switch

## Advanced Examples

See the main Godot documentation for:
- 3D rendering
- Particle systems
- Networking (note: not supported on Switch)
- GDExtension (native code)

## Resources

- [Godot Switch Port Documentation](../../docs/PORT_SWITCH.md)
- [Input Mapping Reference](https://docs.godotengine.org/en/stable/tutorials/inputs/input_examples.html)
- [Performance Optimization](https://docs.godotengine.org/en/stable/tutorials/performance/index.html)

---

**Note:** These examples assume you're using the custom Switch template built from this port. The standard Godot export templates do not support Switch.
