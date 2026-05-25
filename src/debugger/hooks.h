#pragma once
#include <functional>

enum class RenderEventType {
	BG_PIXEL,
	WINDOW_PIXEL,
	OBJECT_PIXEL,
	SCANLINE_START
};

struct RenderEvent {
	RenderEventType type;

	int x = 0;
	int y = 0;

	uint8_t color_index = 0;

	uint8_t scroll = 0;

	// optional metadata for later
	// uint16_t tile_index = 0
	// uint8_t obj_index = 0;

	uint8_t palette = 0;
	bool priority = false;
	bool window_on = false;
};

struct PPUHooks {
	std::function<void(const RenderEvent&)> on_render_event = nullptr;
};