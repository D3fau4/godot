#include "core/input/input.h"
#include "core/os/os.h"
#include "joypad_switch.h"
#include "servers/rendering_server.h"
#include "switch_wrapper.h"
#include "context_gl_switch_egl.h"
#include "display_server_switch.h"

class OS_SWITCH : public OS {
	JoypadSwitch *joypad;
	MainLoop *main_loop;
	SwkbdInline inline_keyboard;
	ContextGLSwitchEGL *gl_context;
	DisplayServerSwitch *display_server;

	protected:
		virtual void initialize_core();
		void initialize() override;
		void initialize_joypads() override;
		void set_main_loop(MainLoop *p_main_loop) override;
		void delete_main_loop() override;
		void finalize_core() override;
		void finalize() override;
		bool _check_internal_feature_support(const String &p_feature) override;
	public:
		Error get_entropy(uint8_t *r_buffer, int p_bytes) override;
		Vector<String> get_video_adapter_driver_info() const override;
		String get_stdin_string() override;
		virtual String get_executable_path() const;
		Error execute(const String &p_path, const List<String> &p_arguments, String *r_pipe, int *r_exitcode, bool read_stderr, Mutex *p_pipe_mutex, bool p_open_console) override;
		Error kill(const ProcessID &p_pid) override;
		Error create_process(const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id, bool p_open_console) override;
		bool is_process_running(const ProcessID &p_pid) const override;
		bool has_environment(const String &p_var) const override;
		String get_environment(const String &p_var) const override;
		void set_environment(const String &p_var, const String &p_value) const override;
		void unset_environment(const String &p_var) const override;
		String get_name() const override;
		String get_distribution_name() const override;
		String get_version() const override;
		MainLoop *get_main_loop() const override;
		DateTime get_datetime(bool utc) const override;
		TimeZoneInfo get_time_zone_info() const override;
		void delay_usec(uint32_t p_usec) const override;
		uint64_t get_ticks_usec() const override;

		void run();

		OS_SWITCH();
		virtual void swap_buffers();
};
