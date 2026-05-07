#pragma once

#include "renderer/chemuPixelEngine.h"
#include "bus.h"
#include <filesystem>

class chemuGB {

public:
	cpe::pixelEngine engine;
	Bus system;

	chemuGB();
	~chemuGB();



	// how to get rid of these
	bool pause = false;
	bool done = false;

public:
	int initialize(std::filesystem::path romPath, uint8_t flags);
	void step();
	void exit();
	bool handle_event(SDL_Event& event);

};