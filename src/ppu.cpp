#include "ppu.h"
#include "bus.h"

void PPU::changeMode(uint8_t mode) {
	bus->stat = (bus->stat & 0b11111100) | (mode & 0b00000011);
}

void PPU::prepareBackground() {
	bool is_window = false;
	bool wx_condition = false;

	uint8_t tile_x_index = 0;
	uint8_t tile_data_low = 0;
	uint8_t tile_data_high = 0;

	int curr_bit = -1;
	curr_bit -= bus->scx % 8;

	for (uint8_t curr_pixel = 0; curr_pixel < 160; curr_pixel++) {
		// Check for window activation
		if (curr_pixel + 7 == bus->wx) {
			wx_condition = true;

			if ((bus->lcdc & 0b00100000) && wy_condition) {
				is_window = true;

				uint8_t tile_x = tile_x_index;
				uint8_t tile_y = (bus->ly + bus->wy) & 0xFF;

				uint8_t tile_index = getIdFromTilemap(is_window, tile_x, tile_y);
				uint16_t tile_address = getTileAddress(tile_index);

				tile_data_low = vram[tile_address + (2 * (tile_y % 8))];
				tile_data_high = vram[tile_address + (2 * (tile_y % 8)) + 1];
			}
		}

		// Get next tile from background/window
		if (curr_bit < 0) {
			uint8_t tile_x;
			uint8_t tile_y;

			if (is_window) {
				tile_x = tile_x_index;
				tile_y = (bus->ly + bus->wy) & 0xFF;
				curr_bit = 7;

			}
			else {
				tile_x = ((bus->scx / 8) + tile_x_index) & 0x1F;
				tile_y = (bus->ly + bus->scy) & 0xFF;
				curr_bit += 8;
			}

			uint8_t tile_index = getIdFromTilemap(is_window, tile_x, tile_y);
			uint16_t tile_address = getTileAddress(tile_index);

			tile_data_low = vram[tile_address + (2 * (tile_y % 8))];
			tile_data_high = vram[tile_address + (2 * (tile_y % 8)) + 1];

			tile_x_index++;
		}

		uint8_t high_bit = ((tile_data_high & (0b10000000 >> (7 - curr_bit))) >> curr_bit) << 1;
		uint8_t low_bit = (tile_data_low & (0b10000000 >> (7 - curr_bit))) >> curr_bit;

		uint8_t color = high_bit | low_bit;
		uint8_t pixel_color = (bus->bgp & (0b11 << (2 * color))) >> (2 * color);

		bg_scanline_buffer[curr_pixel] = pixel_color;

		curr_bit--;
	}
}

void PPU::prepareScanline() {
	prepareBackground();
	//prepareObjects();

	for (uint8_t curr_pixel = 0; curr_pixel < 160; curr_pixel++) {
		// Eventually, pull from BG buffer and OBJ buffer simultaneously to meld them together correctly
		current_frame[bus->ly * 160 + curr_pixel] = bg_scanline_buffer[curr_pixel];
	}
}

void PPU::clock() {
	if (bus->ly < 144) {
		if (bus->ly == bus->wy) {
			wy_condition = true;
		}
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
		wy_condition = false;
		bus->interrupts |= INTERRUPT_VBLANK;
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

uint8_t PPU::getIdFromTilemap(bool is_window, uint8_t tile_x, uint8_t tile_y) {
	uint16_t init_addr = 0x1800;
	if (is_window && bus->lcdc & 0b01000000) {
		init_addr = 0x1C00;
	}
	else if (bus->lcdc & 0b00001000) {
		init_addr = 0x1C00;
	}
	
	return vram[init_addr + (tile_y / 8) * 32 + tile_x];
}

uint16_t PPU::getTileAddress(uint8_t tile_index) {
	if (bus->lcdc & 0b00010000) {
		return 0x0000 + 16 * tile_index;
	}
	return 0x1000 + 16 * static_cast<int8_t>(tile_index);
}