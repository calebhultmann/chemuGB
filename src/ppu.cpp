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
		if (is_window) {
			bg_scanline_buffer[curr_pixel] = { color, 1 };
		}
		else {
			bg_scanline_buffer[curr_pixel] = { color, 0 };
		}

		curr_bit--;
	}
}

void PPU::prepareObjects() {

	// Clear obj buffer to transparent pixels
	for (uint8_t x = 0; x < GB_W; x++) {
		obj_scanline_buffer[x].color = 0;
	}

	uint8_t objs[10] = { 40,40,40,40,40,40,40,40,40,40 };
	// TODO: Currently only handles 8x8 objects
	uint8_t OBJ_HEIGHT = 8;
	if (bus->lcdc & 0b00000100) {
		OBJ_HEIGHT = 16;
	}

	// Pull up to 10 suitable objects from OAM
	uint8_t obj_count = 0;
	for (uint8_t obj = 0; obj < 40; obj++) {
		uint8_t obj_y = oam[obj * 4];
		if (bus->ly + 16 < obj_y + OBJ_HEIGHT && obj_y <= bus->ly + 16) {
			objs[obj_count++] = obj;
			if (obj_count == 10) {
				break;
			}
		}
	}

	if (obj_count == 0) {
		return;
	}

	for (uint8_t obj = 0; obj < 10; obj++) {
		uint8_t obj_num = objs[obj];
		if (obj_num == 40) {
			break;
		}

		uint8_t tile_y = oam[obj_num * 4];
		uint8_t tile_x = oam[obj_num * 4 + 1];
		uint8_t tile_index = oam[obj_num * 4 + 2];
		uint8_t tile_attr = oam[obj_num * 4 + 3];

		uint8_t tile_line = (bus->ly - (tile_y - 16));
		if (tile_attr & Y_FLIP) {
			tile_line = (OBJ_HEIGHT - 1) - tile_line;
		}

		uint16_t tile_addr = 16 * tile_index + 2 * tile_line;

		uint8_t	tile_data_low = vram[tile_addr];
		uint8_t	tile_data_high = vram[tile_addr + 1];


		for (uint8_t pixel_x = 0; pixel_x < 8; pixel_x++) {
			uint8_t loc_x = tile_x + pixel_x;
			// Remaining pixels are not visible
			if (loc_x >= 168) {
				break;
			}
			// Current pixel is not visible
			if (loc_x < 8) {
				continue;
			}

			uint8_t curr_bit = 7 - pixel_x;
			if (tile_attr & X_FLIP) {
				curr_bit = 7 - curr_bit;
			}
			
			uint8_t high_bit = ((tile_data_high & (0b10000000 >> (7 - curr_bit))) >> curr_bit) << 1;
			uint8_t low_bit = (tile_data_low & (0b10000000 >> (7 - curr_bit))) >> curr_bit;
			uint8_t new_color = high_bit | low_bit;
			OBJ_Pixel& curr = obj_scanline_buffer[loc_x - 8];

			if (new_color == 0 || (curr.color != 0 && tile_x >= curr.init_x)) {
				continue;
			}
			curr.color = new_color;
			curr.palette = tile_attr & PALETTE;
			curr.priority = tile_attr & PRIORITY;
			curr.init_x = tile_x;
		}
	}
}

void PPU::prepareScanline() {
	prepareBackground();
	prepareObjects();

	for (uint8_t curr_pixel = 0; curr_pixel < 160; curr_pixel++) {
		uint8_t pixel_color;

		if (bg_scanline_buffer[curr_pixel].window || obj_scanline_buffer[curr_pixel].color == 0 || (obj_scanline_buffer[curr_pixel].priority && bg_scanline_buffer[curr_pixel].color != 0)) {
			uint8_t color = bg_scanline_buffer[curr_pixel].color;
			pixel_color = (bus->bgp & (0b11 << (2 * color))) >> (2 * color);
		}
		else {
			uint8_t color = obj_scanline_buffer[curr_pixel].color;
			if (obj_scanline_buffer[curr_pixel].palette) {
				pixel_color = (bus->obp1 & (0b11 << (2 * color))) >> (2 * color);
			}
			else {
				pixel_color = (bus->obp0 & (0b11 << (2 * color))) >> (2 * color);
			}
		}
		current_frame[bus->ly * 160 + curr_pixel] = pixel_color;
	}
}

void PPU::clock() {

	if (bus->ly == bus->lyc) {
		bus->stat |= 0b00000100;
	}
	else {
		bus->stat &= 0b11111011;
	}

	if (bus->ly < 144) {
		if (dot_count == 0) {
			if ((bus->stat & 0b01000000) && (bus->stat & 0b00000100)) {
				bus->interrupts |= INTERRUPT_LCD;
			}
		}

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