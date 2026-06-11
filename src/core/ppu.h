#pragma once
#include <cstdint>
#include "debugger/hooks.h"

enum obj_attributes : uint8_t {
	PRIORITY = 0b10000000,
	Y_FLIP = 0b01000000,
	X_FLIP = 0b00100000,
	PALETTE = 0b00010000
};

class Bus;
struct PPUHooks;

class PPU
{
public:
	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }
	PPUHooks hooks;

	uint8_t vram[0x2000];
	uint8_t oam[0xA0];

	struct OBJ_Pixel {
		uint8_t color_index;
		uint8_t palette;
		uint8_t priority;
		uint8_t init_x;
	};

	struct WINDOW_Pixel {
		uint8_t color_index;
		bool window;
	};

	static constexpr int GB_W = 160;
	static constexpr int GB_H = 144;

	uint8_t current_frame[GB_W * GB_H];
	uint8_t bg_scanline_buffer[GB_W] {};
	WINDOW_Pixel window_scanline_buffer[GB_W]{};
	OBJ_Pixel obj_scanline_buffer[GB_W] {};
	bool frame_ready;
	bool is_frame_ready();

	int dot_count = 0;
	bool wy_condition = false;
	
	uint8_t read(uint16_t addr) const;
	void write(uint16_t addr, uint8_t data);

	uint8_t getIdFromTilemap(bool is_window, uint8_t tile_x, uint8_t tile_y) const;
	uint16_t getTileAddress(uint8_t tile_index) const;
	uint8_t getColorFromIndex(uint8_t palette, uint8_t index) const;

	void changeMode(uint8_t mode);
	void prepareScanline();
	void prepareBackground();
	void prepareWindow();
	void prepareObjects();

	void clock();
};