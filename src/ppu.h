#pragma once
#include <cstdint>
#include <queue>

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

	uint8_t mode = 0;
	std::queue<Pixel> FIFO;

	uint8_t x;
	uint8_t y;

	uint8_t tile_x;
	uint8_t tile_y;
	uint8_t tile_index;

	uint8_t tile_data_low;
	uint8_t tile_data_high;

	void func();
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

	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	uint8_t getVRAMtile(uint8_t tilemap,  uint8_t tile_x, uint8_t tile_y);
	uint16_t getTileAddress(uint8_t tile_index);
	
	PixelFetcher fifo;

	void clock();
};