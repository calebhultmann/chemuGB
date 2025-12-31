#include "ppu.h"

PixelFetcher::PixelFetcher(PPU* p) {
	ppu = p;
}

void PixelFetcher::func() {
	switch (mode) {
	case 0:
		// assume $9800, ignore logic for now
		tile_x = ((bus->scx / 8) + x) & 0x1F;
		tile_y = (bus->ly + bus->scy) & 255;
		tile_index = ppu->getVRAMtile(0, tile_x, tile_y);
		mode = 1;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	}
}

// These will eventually require checking that vram/oam can be accessed
uint8_t PPU::read(uint16_t addr) {
	if (addr >= 0x8000 && addr <= 0x9FFF) {
		return vram[addr & 0x1FFF];
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		return oam[addr - 0xFE00];
	}
}

void PPU::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x8000 && addr <= 0x9FFF) {
		vram[addr & 0x1FFF] = data;
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		oam[addr - 0xFE00] = data;
	}
}

uint8_t PPU::getVRAMtile(uint8_t tilemap,  uint8_t tile_x, uint8_t tile_y) {
	return read(0x9800 + 32 * tile_y + tile_x);
}