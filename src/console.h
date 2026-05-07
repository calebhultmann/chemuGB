#include "config.h"
#include "emulator.h"
#include "debugger/debugger.h"

class Console {
public:
	Console(Config& config);
	~Console();

	// renderer
	chemuGB gb;
	Debugger debugger;

	SDL_Event event;

	// flags and attributes (in progress)
	bool debug;
	int color = 0;
	int init_status = 0;
	// int scale = 8; can i get rid of this somehow? what is it used for?

public:
	void start();
	void run();
};