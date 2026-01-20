#pragma once
#include <cstdint>


enum obj_attributes : uint8_t {
	PRIORITY = 0b10000000,
	Y_FLIP = 0b01000000,
	X_FLIP = 0b00100000,
	PALETTE = 0b00010000
};

class Bus;

class PPU
{
public:
	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }

	uint8_t vram[0x2000];
	uint8_t oam[0xA0];

	struct Pixel {
		uint8_t color;
		uint8_t palette;
		uint8_t priority;
		uint8_t init_x;
	};

	static constexpr int GB_W = 160;
	static constexpr int GB_H = 144;

	uint8_t current_frame[GB_W * GB_H];
	uint8_t bg_scanline_buffer[GB_W];
	Pixel obj_scanline_buffer[GB_W] {};
	bool frame_ready;
	bool is_frame_ready();

	int dot_count = 0;
	bool wy_condition = false;
	
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	uint8_t getIdFromTilemap(bool is_window, uint8_t tile_x, uint8_t tile_y);
	uint16_t getTileAddress(uint8_t tile_index);
	
	void changeMode(uint8_t mode);
	void prepareScanline();
	void prepareBackground();
	void prepareObjects();

	void clock();
};