#pragma once

#include "chemuPixelEngine.h"


class chemuGB : public cpe::pixelEngine {
// Add options for:
//  - Optional debug mode
//  - Gray/greenscale

public:
	chemuGB();
	chemuGB(int s);
	~chemuGB();

public:
	bool initialize();
	void start();

	void drawDebug();
};