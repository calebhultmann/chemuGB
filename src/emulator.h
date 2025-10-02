#pragma once

#include "chemuPixelEngine.h"


class chemuGB : public cpe::pixelEngine {
public:
	chemuGB();
	chemuGB(int s);
	~chemuGB();

public:
	bool initialize();
	void start();

	void drawDebug();
};