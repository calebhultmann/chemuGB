#include "config.h"
#include "emulator.h"
#include "debugger/debugger.h"
#include <SDL3/SDL_events.h>
#include "renderer/chemuPixelEngine.h"
#include "audio/chemuAudioEngine.h"

enum DEBUG_MODE {
	NORMAL,
	STEP
};

enum class CONSOLE_MODE {
	RUNNING,
	PAUSED,
	STEPPING,
	EXITING
};

class Console {
public:
	Console(Config& config);
	~Console();

	// renderer
	chemuGB gb;
	Debugger debugger;
	pixelEngine engine;

	// flags and attributes (in progress)
	bool debug;
	int color = 0;
	int init_status = 0;
	bool running = true;
	bool paused = false;
	// int scale = 8; can i get rid of this somehow? what is it used for?

	CONSOLE_MODE mode;

	const uint64_t INPUT_POLL_INTERVAL_MS = 2;
	uint64_t last_input_poll;

public:
	void start();
	void run();
	void poll_events();
	bool handle_global_event(SDL_Event& event);
};