#include "display_server_switch.h"
#include <drivers/gles3/rasterizer_gles3.h>

const size_t gfxMemSize = 8 * 1024 * 1024;

DisplayServerSwitch::DisplayServerSwitch(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error) {
    rendering_driver = p_rendering_driver;
    
    if (rendering_driver != "opengl3") {
        print_line("Invalid rendering driver: " + rendering_driver);
        r_error = ERR_UNAVAILABLE;
        return;
    }
    
    gl_context = memnew(ContextGLSwitchEGL(false));

    if (gl_context->initialize() != OK) {
        print_line("Failed to initialize EGL context");
        memdelete(gl_context);
        gl_context = nullptr;
    }

    RasterizerGLES3::make_current();
    gl_context->set_use_vsync(p_vsync_mode == VSYNC_ENABLED);
    resolution = Size2i(p_resolution.width, p_resolution.height);

    r_error = OK;
}

DisplayServer *DisplayServerSwitch::create_func(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error) {
    return memnew(DisplayServerSwitch(p_rendering_driver, p_mode, p_vsync_mode, p_flags, p_position, p_resolution, p_screen, r_error));
}

Vector<String> DisplayServerSwitch::get_rendering_drivers_func() {
    return Vector<String>{"opengl3"};
}

void DisplayServerSwitch::register_switch_driver() {
    register_create_function("switch", create_func, get_rendering_drivers_func);
}

DisplayServerSwitch *DisplayServerSwitch::get_singleton() {
    return static_cast<DisplayServerSwitch *>(DisplayServer::get_singleton());
}

bool DisplayServerSwitch::has_feature(Feature p_feature) const {
    return p_feature == FEATURE_TOUCHSCREEN;
}

String DisplayServerSwitch::get_name() const {
    return "Nintendo Switch";
}

int DisplayServerSwitch::get_screen_count() const {
    return 1;
}

int DisplayServerSwitch::get_primary_screen() const {
    return 0;
}

Point2i DisplayServerSwitch::screen_get_position(int p_screen) const {
    return Point2i();
}

Size2i DisplayServerSwitch::screen_get_size(int p_screen) const {
    return resolution;
}

Rect2i DisplayServerSwitch::screen_get_usable_rect(int p_screen) const {
    return Rect2i(screen_get_position(p_screen), screen_get_size(p_screen));
}

int DisplayServerSwitch::screen_get_dpi(int p_screen) const {
    return 72;
}

float DisplayServerSwitch::screen_get_refresh_rate(int p_screen) const {
    return 60.0f;
}

bool DisplayServerSwitch::is_touchscreen_available() const {
    return true;
}

Vector<DisplayServer::WindowID> DisplayServerSwitch::get_window_list() const {
    return Vector<DisplayServer::WindowID>{MAIN_WINDOW_ID};
}

DisplayServer::WindowID DisplayServerSwitch::get_window_at_screen_position(const Point2i &p_position) const {
    return MAIN_WINDOW_ID;
}

void DisplayServerSwitch::window_attach_instance_id(ObjectID p_instance, WindowID p_window) {
    window_attached_instance_id = p_instance;
}

ObjectID DisplayServerSwitch::window_get_attached_instance_id(WindowID p_window) const {
    return window_attached_instance_id;
}

void DisplayServerSwitch::window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window) {
    window_resize_callback = p_callable;
}

void DisplayServerSwitch::window_set_window_event_callback(const Callable &p_callable, WindowID p_window) {
    window_event_callback = p_callable;
}

void DisplayServerSwitch::window_set_input_event_callback(const Callable &p_callable, WindowID p_window) {
    input_event_callback = p_callable;
}

void DisplayServerSwitch::window_set_input_text_callback(const Callable &p_callable, WindowID p_window) {
    input_text_callback = p_callable;
}

void DisplayServerSwitch::window_set_drop_files_callback(const Callable &p_callable, WindowID p_window) {
    // not supported
}

void DisplayServerSwitch::window_set_title(const String &p_title, WindowID p_window) {
    // not supported
}

int DisplayServerSwitch::window_get_current_screen(WindowID p_window) const {
    return SCREEN_OF_MAIN_WINDOW;
}

void DisplayServerSwitch::window_set_current_screen(int p_screen, WindowID p_window) {
    // not supported
}

Point2i DisplayServerSwitch::window_get_position(WindowID p_window) const {
    return Point2i();
}

Point2i DisplayServerSwitch::window_get_position_with_decorations(WindowID p_window) const {
    return Point2i();
}

void DisplayServerSwitch::window_set_position(const Point2i &p_position, WindowID p_window) {
    // not supported
}

void DisplayServerSwitch::window_set_transient(WindowID p_window, WindowID p_parent) {
    // not supported
}

void DisplayServerSwitch::window_set_max_size(const Size2i p_size, WindowID p_window) {
    // not supported
}

Size2i DisplayServerSwitch::window_get_max_size(WindowID p_window) const {
    return Size2i();
}

void DisplayServerSwitch::window_set_min_size(const Size2i p_size, WindowID p_window) {
    // not supported
}

Size2i DisplayServerSwitch::window_get_min_size(WindowID p_window) const {
    return Size2i();
}

void DisplayServerSwitch::window_set_size(const Size2i p_size, WindowID p_window) {
    // not supported
}

Size2i DisplayServerSwitch::window_get_size(WindowID p_window) const {
    return resolution;
}

Size2i DisplayServerSwitch::window_get_size_with_decorations(WindowID p_window) const {
    return window_get_size(p_window);
}

void DisplayServerSwitch::window_set_mode(WindowMode p_mode, WindowID p_window) {
    // not supported
}

DisplayServer::WindowMode DisplayServerSwitch::window_get_mode(WindowID p_window) const {
    return DisplayServer::WindowMode::WINDOW_MODE_FULLSCREEN;
}

bool DisplayServerSwitch::window_is_maximize_allowed(WindowID p_window) const {
    return false;
}

void DisplayServerSwitch::window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window) {
    // not supported
}

bool DisplayServerSwitch::window_get_flag(WindowFlags p_flag, WindowID p_window) const {
    return false;
}

void DisplayServerSwitch::window_request_attention(WindowID p_window) {
    // not supported
}

void DisplayServerSwitch::window_move_to_foreground(WindowID p_window) {
    // not supported
}

bool DisplayServerSwitch::window_is_focused(WindowID p_window) const {
    return true;
}

bool DisplayServerSwitch::window_can_draw(WindowID p_window) const {
    return true;
}

bool DisplayServerSwitch::can_any_window_draw() const {
    return true;
}

void DisplayServerSwitch::process_events() {
    // no implementation needed
}

void DisplayServerSwitch::_dispatch_input_event(const Ref<InputEvent> &p_event) {
    Callable cb = get_singleton()->input_event_callback;
    if (!cb.is_null()) {
        Variant ev = p_event;
        Variant *evp = &ev;
        Variant ret;
        Callable::CallError ce;
        cb.callp((const Variant **)&evp, 1, ret, ce);
    }
}

DisplayServer::VSyncMode DisplayServerSwitch::window_get_vsync_mode(WindowID p_vsync_mode) const {
    return DisplayServer::VSYNC_ENABLED;
}

void DisplayServerSwitch::swap_buffers() {
    gl_context->swap_buffers();
}