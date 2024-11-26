#ifndef DISPLAY_SERVER_SWITCH_H
#define DISPLAY_SERVER_SWITCH_H

#include "servers/display_server.h"
#include "context_gl_switch_egl.h"

class DisplayServerSwitch : public DisplayServer {
 _THREAD_SAFE_CLASS_
public:
 ContextGLSwitchEGL *gl_context;
 String rendering_driver;
 Size2i resolution;

 ObjectID window_attached_instance_id;
 Callable window_event_callback;
 Callable window_resize_callback;
 Callable input_event_callback;
 Callable input_text_callback;

 DisplayServerSwitch(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error);
 static void register_switch_driver();
 static DisplayServerSwitch *get_singleton();
 bool has_feature(Feature p_feature) const override;
 String get_name() const override;
 int get_screen_count() const override;
 int get_primary_screen() const override;
 Point2i screen_get_position(int p_screen) const override;
 Size2i screen_get_size(int p_screen) const override;
 Rect2i screen_get_usable_rect(int p_screen) const override;
 int screen_get_dpi(int p_screen) const override;
 float screen_get_refresh_rate(int p_screen) const override;
 bool is_touchscreen_available() const override;
 Vector<WindowID> get_window_list() const override;
 WindowID get_window_at_screen_position(const Point2i &p_position) const override;
 void window_attach_instance_id(ObjectID p_instance, WindowID p_window) override;
 ObjectID window_get_attached_instance_id(WindowID p_window) const override;
 void window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window) override;
 void window_set_window_event_callback(const Callable &p_callable, WindowID p_window) override;
 void window_set_input_event_callback(const Callable &p_callable, WindowID p_window) override;
 void window_set_input_text_callback(const Callable &p_callable, WindowID p_window) override;
 void window_set_drop_files_callback(const Callable &p_callable, WindowID p_window) override;
 void window_set_title(const String &p_title, WindowID p_window) override;
 int window_get_current_screen(WindowID p_window) const override;
 void window_set_current_screen(int p_screen, WindowID p_window) override;
 Point2i window_get_position(WindowID p_window) const override;
 Point2i window_get_position_with_decorations(WindowID p_window) const override;
 void window_set_position(const Point2i &p_position, WindowID p_window) override;
 void window_set_transient(WindowID p_window, WindowID p_parent) override;
 void window_set_max_size(Size2i p_size, WindowID p_window) override;
 Size2i window_get_max_size(WindowID p_window) const override;
 void window_set_min_size(Size2i p_size, WindowID p_window) override;
 Size2i window_get_min_size(WindowID p_window) const override;
 void window_set_size(Size2i p_size, WindowID p_window) override;
 Size2i window_get_size(WindowID p_window) const override;
 Size2i window_get_size_with_decorations(WindowID p_window) const override;
 void window_set_mode(WindowMode p_mode, WindowID p_window) override;
 WindowMode window_get_mode(WindowID p_window) const override;
 bool window_is_maximize_allowed(WindowID p_window) const override;
 void window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window) override;
 bool window_get_flag(WindowFlags p_flag, WindowID p_window) const override;
 void window_request_attention(WindowID p_window) override;
 void window_move_to_foreground(WindowID p_window) override;
 bool window_is_focused(WindowID p_window) const;
 bool window_can_draw(WindowID p_window) const override;
 bool can_any_window_draw() const override;
 void process_events() override;
 void _dispatch_input_event(const Ref<InputEvent> &p_event);
 VSyncMode window_get_vsync_mode(WindowID p_vsync_mode) const override;
 virtual void swap_buffers();
 static DisplayServer *create_func(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error);
 static Vector<String> get_rendering_drivers_func();
};

#endif //DISPLAY_SERVER_SWITCH_H