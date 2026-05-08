#include "config.h"
#include "emulator.h"
#include "debugger/debugger.h"
#include <SDL3/SDL_events.h>
#include "renderer/chemuPixelEngine.h"

enum DEBUG_MODE {
	NORMAL,
	STEP
};

class Console {
public:
	Console(Config& config);
	~Console();

	// renderer
	chemuGB gb;
	Debugger debugger;
	cpe::pixelEngine engine;

	// flags and attributes (in progress)
	bool debug;
	int color = 0;
	int init_status = 0;
	bool running = true;
	bool paused = false;
	// int scale = 8; can i get rid of this somehow? what is it used for?
	// normal mode -> update debugger every emu frame
	// step mode -> update debugger every emu step

	// TODO: TOGGLE MODE
	//	         New control button 'O' switches debug mode (only works in debug mode)
	//           When in Step Mode, emulator steps IFF step button 'I' is pressed
	//           Deactivating Step Mode returns the console back to Normal Mode
	int mode = DEBUG_MODE::NORMAL;

	const uint64_t INPUT_POLL_INTERVAL_MS = 2;
	uint64_t last_input_poll;

public:
	void start();
	void run();
	void poll_events();
	bool handle_global_event(SDL_Event& event);
};