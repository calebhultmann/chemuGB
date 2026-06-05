#include "console.h"


Console::Console(Config& config) {
	// TODO: exit statuses for failed inits
	if (gb.initialize(config.filePath, 0) != Error::None) {
		// How to deal with exit status in constructor
		// return init_status

	}
	
	int status = engine.initialize();
	status = speaker.initialize(&gb.system.apu);
	//speaker.play_default_sound();
	//if (status != Error::None) {
	//	return status;
	//}
	debug = config.debug;
	if (config.debug) {
		// start debugger and link, turn console debug mode on
		debugger.init(engine, gb);
	}
}

Console::~Console() {
	SDL_DestroyRenderer(engine.renderer);
	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}

void Console::start() {
	last_input_poll = SDL_GetTicks();
	mode = CONSOLE_MODE::RUNNING;
	return;
}

void Console::run() {
	while (running) {
		switch (mode) {
		case CONSOLE_MODE::RUNNING:
			poll_events();
			gb.step();
			if (gb.system.ppu.is_frame_ready()) {
				engine.renderFrame(gb.system.ppu.current_frame);
				if (debug) {
					debugger.frame(gb);
				}
			}
			speaker.step();
			break;

		case CONSOLE_MODE::PAUSED:
			poll_events();
			if (debug) {
				debugger.frame(gb);
			}
			break;

		case CONSOLE_MODE::STEPPING:
			poll_events();
			gb.step();
			if (gb.system.ppu.is_frame_ready()) {
				engine.renderFrame(gb.system.ppu.current_frame);
				if (debug) {
					debugger.frame(gb);
				}
				mode = CONSOLE_MODE::PAUSED;
			}
			break;
		
		case CONSOLE_MODE::EXITING:
			break;
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
			switch (mode) {
			case CONSOLE_MODE::RUNNING:
				mode = CONSOLE_MODE::PAUSED;
				break;
			case CONSOLE_MODE::PAUSED:
				mode = CONSOLE_MODE::RUNNING;
				break;
			}
			return true;

		// Quit
		case SDL_SCANCODE_Q:
			running = false;
			return true;

		// Green/Grayscale
		case SDL_SCANCODE_G:
			engine.palette = 1 - engine.palette;
			return true;

		// Frame Step
		case SDL_SCANCODE_I:
			if (mode == CONSOLE_MODE::PAUSED) {
				mode = CONSOLE_MODE::STEPPING;
			}
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