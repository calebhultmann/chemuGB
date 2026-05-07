#pragma once

#include "bus.h"
#include <filesystem>

class chemuGB {

public:
	Bus system;

	chemuGB();
	~chemuGB();



	// how to get rid of these
	bool pause = false;
	bool done = false;

public:
	int initialize(std::filesystem::path romPath, uint8_t flags);
	void step();
	bool handle_event(SDL_Event& event);
};