#include "ppu.h"
#include "bus.h"
#include "chemuPixelEngine.h"

PixelFetcher::PixelFetcher(PPU* p) {
	ppu = p;
}

PPU::PPU() : fifo(this) {
	
}

void PixelFetcher::fetchBackground(uint8_t x) {
	// tile_x = ((bus->scx / 8) + x) & 0x1F;
	
	tile_x = x;
	tile_y = (bus->ly + bus->scy) & 0xFF;

	tile_index = ppu->getIdFromTilemap(0, tile_x, tile_y);
	
	uint16_t tile_address = ppu->getTileAddress(tile_index);
	tile_data_low = ppu->vram[tile_address];
	
	tile_address = ppu->getTileAddress(tile_index);
	tile_data_high = ppu->vram[tile_address + 1];

	for (int bit = 7; bit >= 0; bit--) {
		uint8_t high_bit = ((tile_data_high & (0b10000000 >> (7 - bit))) >> bit) << 1;
		uint8_t low_bit = (tile_data_low & (0b10000000 >> (7 - bit))) >> bit;

		FIFO.push({ uint8_t(high_bit | low_bit), 0, 0 });
	}
}

void PPU::changeMode(uint8_t mode) {
	bus->stat = (bus->stat & 0b11111100) | (mode & 0b00000011);
}

void PPU::prepareScanline() {
	for (uint8_t x = 0; x < 20; x++) {
		fifo.fetchBackground(x);
		for (uint8_t i = 0; i < 8; i++) {
			PixelFetcher::Pixel curr = fifo.FIFO.front();
			fifo.FIFO.pop();

			uint8_t pixel_color = (bus->bgp & (0b11 << (2 * curr.color))) >> (2 * curr.color);
			current_frame[bus->ly * 160 + x * 8 + i] = pixel_color;
		}
	}
}

void PPU::clock() {
	if (bus->ly < 144) {
		switch (++dot_count) {
		case 80:
			changeMode(3);
			prepareScanline();
			return;
		case 252: changeMode(0); return;
		case 456:
			dot_count = 0;
			if (++bus->ly == 144) {
				changeMode(1);
				return;
			}
			changeMode(2);
		}
		return;
	}
	if (bus->ly == 144 && dot_count == 0) {
		frame_ready = true;
	}
	if (++dot_count == 456) {
		if (++bus->ly > 153) {
			changeMode(1);
			bus->ly = 0;
		}
		dot_count = 0;
	}
}

bool PPU::is_frame_ready() {
	if (frame_ready) {
		frame_ready = false;
		return true;
	}

	return false;
}

uint8_t PPU::read(uint16_t addr) {
	if (addr >= 0x8000 && addr <= 0x9FFF) {
		if ((bus->stat & 0b00000011) == 3) {
			return 0xF;
		}

		return vram[addr & 0x1FFF];
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		if ((bus->stat & 0b00000011) == 2 || (bus->stat & 0b00000011) == 3) {
			return 0xF;
		}

		return oam[addr - 0xFE00];
	}
	return 0;
}

void PPU::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x8000 && addr <= 0x9FFF) {
		if ((bus->stat & 0b00000011) == 3) {
			return;
		}
		
		vram[addr & 0x1FFF] = data;
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		if ((bus->stat & 0b00000011) == 2 || (bus->stat & 0b00000011) == 3) {
			return;
		}

		oam[addr - 0xFE00] = data;
	}
}

uint8_t PPU::getIdFromTilemap(uint8_t tilemap, uint8_t tile_x, uint8_t tile_y) {
	return vram[0x1800 + 0x400 * tilemap + (tile_y / 8) * 32 + tile_x];
}

uint16_t PPU::getTileAddress(uint8_t tile_index) {
	if (bus->lcdc & 0b00010000) {
		return 0x0000 + 4 * tile_index;
	}
	return 0x0800 + 4 * static_cast<int8_t>(tile_index);
}