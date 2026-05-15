#include "console.h"


Console::Console(Config& config) {
	// TODO: exit statuses for failed inits
	if (gb.initialize(config.filePath, 0) != Error::None) {
		// How to deal with exit status in constructor
		// return init_status

	}

	int status = engine.initialize();
	//if (status != Error::None) {
	//	return status;
	//}
	debug = config.debug;
	if (config.debug) {
		// start debugger and link, turn console debug mode on
		debugger.init(engine);
	}
}

Console::~Console() {
	SDL_DestroyRenderer(engine.renderer);
	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}

void Console::start() {
	last_input_poll = SDL_GetTicks();
	return;
}

void Console::run() {
	while (running) {
		poll_events();
		if (!paused) {
			gb.step();

			// TODO: This will have to be moved in order for debugger to be operational while emu is paused
			if (debug &&
				mode == DEBUG_MODE::NORMAL &&
				gb.system.ppu.frame_ready) {
				debugger.frame(gb);
			}
			if (gb.system.ppu.is_frame_ready()) {
				engine.renderFrame(gb.system.ppu.current_frame);
			}
		}
	}
}

void Console::poll_events() {
	// TODO: This will continue to increase while the game is NOT stepping (debugging)
	//       Does this have negative effects (input every step)?
	// THIS IS AN OLD TODO, UPDATE TO NEW METHOD
	uint64_t now = SDL_GetTicks();
	SDL_Event event;

	if (now - last_input_poll >= INPUT_POLL_INTERVAL_MS) {
		last_input_poll = now;

		while (SDL_PollEvent(&event)) {
			if (debugger.handle_event(event)) {
				continue;
			}

			if (handle_global_event(event)) {
				continue;
			}

			if (gb.handle_event(event)) {
				continue;
			}
		}
	}

}

bool Console::handle_global_event(SDL_Event& event) {
	switch (event.type) {
	case SDL_EVENT_QUIT:
		running = false;
		return true;
	case SDL_EVENT_KEY_DOWN:
		switch (event.key.scancode) {
		// Pause
		case SDL_SCANCODE_P:
			paused = !paused;
			return true;

		// Quit
		case SDL_SCANCODE_Q:
			running = false;
			return true;

		// Green/Grayscale
		case SDL_SCANCODE_G:
			engine.palette = 1 - engine.palette;
			return true;
		}
	}
	return false;
}
/*

      Config
         |
      Console
       / | \
	  /  |  \
Debugger | Emulator
		 |
	  Renderer

*/