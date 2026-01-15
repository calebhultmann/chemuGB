#pragma once
#include <cstdint>
#include <queue>
#include "chemuPixelEngine.h"
#include <array>

class Bus;
class PPU;

class PixelFetcher
{
public:
	PixelFetcher(PPU* p);

	PPU* ppu = nullptr;
	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }

	struct Pixel {
		uint8_t color;
		uint8_t palette;
		uint8_t priority;
	};

	std::queue<Pixel> FIFO;

	uint8_t x;
	uint8_t y;

	uint8_t tile_x;
	uint8_t tile_y;
	uint8_t tile_index;

	uint8_t tile_data_low;
	uint8_t tile_data_high;

	void fetchBackground(uint8_t x);
};

class PPU
{
public:
	PPU();
	//~PPU();

	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }

	uint8_t vram[0x2000];
	uint8_t oam[0xA0];

	static constexpr int GB_W = 160;
	static constexpr int GB_H = 144;

	uint8_t current_frame[GB_W * GB_H];
	bool frame_ready;
	bool is_frame_ready();

	int dot_count = 0;
	
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	uint8_t getIdFromTilemap(uint8_t tile_x, uint8_t tile_y);
	uint16_t getTileAddress(uint8_t tile_index);
	
	void changeMode(uint8_t mode);
	void prepareScanline();

	PixelFetcher fifo;

	void clock();
};