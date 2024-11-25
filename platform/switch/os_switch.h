#include "core/input/input.h"
#include "core/os/os.h"
#include "joypad_switch.h"

class OS_SWITCH : public OS {
	JoypadSwitch *joypad;

	protected:
		virtual void initialize_core();
		void initialize() override;
		void initialize_joypads() override;

	public:

		virtual String get_executable_path() const;
        OS_SWITCH();
		void run();
};
