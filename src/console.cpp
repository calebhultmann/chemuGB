#include "console.h"

Console::Console(Config& config) {
	
	if (gb.initialize(config.filePath, 0) != Error::None) {
		// How to deal with exit status in constructor
		// return init_status

	}

	if (config.debug) {
		// start debugger and link, turn console debug mode on
	}
	
}

Console::~Console() {

}

void Console::start() {
	return;
}


void Console::run() {

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