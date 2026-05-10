#pragma once

#include <SDL3/SDL_render.h>

class chemuGB;
class pixelEngine;

class Debugger {
public:
	Debugger();
	~Debugger();
	void init(pixelEngine&);

	pixelEngine* engine = nullptr;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

	uint32_t vram_pixels[128 * 64];

	bool handle_event(const SDL_Event&);

	void draw_registers(const chemuGB&);
	void draw_vram(const chemuGB&);
	
	void frame(const chemuGB&);
	void begin_frame();
	void draw(const chemuGB&);
	void end_frame();
};

/*

void chemuGB::drawDebug() {
	cpe::Pixel white = cpe::Pixel{ 255,255,255 };
	int debug_x = 0;

	// Draw Tileblocks
	// For each tileblock
	std::array<uint32_t, 128*64> tileblock_buffer;

	for (int tileblock = 0; tileblock < 3; tileblock++) {
		uint16_t block_start_addr = 0x800 * tileblock;

		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 16; x++) {
				int tile_index = (y / 8) * 16 + x;
				uint16_t tile_addr = block_start_addr + tile_index * 16;
				uint16_t tile_line_addr = tile_addr + (y % 8) * 2;
				uint8_t data_low = system.ppu.vram[tile_line_addr];
				uint8_t	data_high = system.ppu.vram[tile_line_addr + 1];

				for (int bit = 0; bit < 8; bit++) {
					int high_bit = ((data_high & (0b10000000 >> bit)) >> (7 - bit)) << 1;
					int low_bit = (data_low & (0b10000000 >> bit)) >> (7 - bit);
					int pixel_color_id = high_bit | low_bit;
					int pixel_color = (system.bgp & (0b11 << (2 * pixel_color_id))) >> (2 * pixel_color_id);
					uint32_t color = engine.gameboy_palette[engine.palette][pixel_color];
					tileblock_buffer[(y * 128) + (x * 8) + bit] = color;
				}
			}
		}

		SDL_Rect tileblock_rect = { 128 * tileblock, 256, 128, 64 };

		SDL_UpdateTexture(
			engine.dbg_screen,
			&tileblock_rect,
			tileblock_buffer.data(),
			128 * sizeof(uint32_t)
		);
	}

	std::array<uint32_t, 256 * 256> tilemap_buffer;

	for (int tilemap = 0; tilemap < 2; tilemap++) {
		uint16_t tilemap_start_addr = 0x1800 + 0x400 * tilemap;

		for (int y = 0; y < 256; y++) {
			for (int x = 0; x < 32; x++) {
				int tile_index = (y / 8) * 32 + x;
				uint8_t tile_id = system.ppu.vram[tilemap_start_addr + tile_index];
				uint16_t tile_start_addr;
				if (system.lcdc & 0b00010000) {
					tile_start_addr = tile_id * 16;
				}
				else {
					tile_start_addr = 0x1000 + (16 * static_cast<int8_t>(tile_id));
				}
				uint16_t tile_line_addr = tile_start_addr + (y % 8) * 2;
				uint8_t data_low = system.ppu.vram[tile_line_addr];
				uint8_t	data_high = system.ppu.vram[tile_line_addr + 1];

				for (int bit = 0; bit < 8; bit++) {
					int high_bit = ((data_high & (0b10000000 >> bit)) >> (7 - bit)) << 1;
					int low_bit = (data_low & (0b10000000 >> bit)) >> (7 - bit);
					int pixel_color_id = high_bit | low_bit;
					int pixel_color = (system.bgp & (0b11 << (2 * pixel_color_id))) >> (2 * pixel_color_id);
					uint32_t color = engine.gameboy_palette[engine.palette][pixel_color];
					tilemap_buffer[(y * 256) + (x * 8) + bit] = color;
				}
			}
		}

		SDL_Rect tilemap_rect = { 256 * tilemap, 0, 256, 256 };

		SDL_UpdateTexture(
			engine.dbg_screen,
			&tilemap_rect,
			tilemap_buffer.data(),
			256 * sizeof(uint32_t)
		);
	}
	SDL_RenderTexture(engine.dbg_renderer, engine.dbg_screen, NULL, NULL);

	// Draw Background Outline
	SDL_SetRenderDrawColor(engine.dbg_renderer, 255, 100, 100, 255);
	float x = system.scx;
	float y = system.scy;
	float wrap_x = static_cast<float>(((int)x + 160) % 256);
	float wrap_y = static_cast<float>(((int)y + 144) % 256);
	float offset = 0;
	if (system.lcdc & 0b00001000) {
		offset = 512;
	}
	//draw top and bottom
	if (x + 160 > 256) {
		// top
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, 256 * 2 + offset, y * 2);
		SDL_RenderLine(engine.dbg_renderer, 0 + offset, y * 2, wrap_x * 2 + offset, y * 2);
		// bottom
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, wrap_y * 2, 256 * 2 + offset, wrap_y * 2);
		SDL_RenderLine(engine.dbg_renderer, 0 + offset, wrap_y * 2, wrap_x * 2 + offset, wrap_y * 2);
	}
	else {
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, (x + 160) * 2 + offset, y * 2);
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, wrap_y * 2, (x + 160) * 2 + offset, wrap_y * 2);
	}

	//draw left and right
	if (y + 144 > 256) {
		//left
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, x * 2 + offset, 256 * 2);
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, 0, x * 2 + offset, wrap_y * 2);
		//right
		SDL_RenderLine(engine.dbg_renderer, wrap_x * 2 + offset, y * 2, wrap_x * 2 + offset, 256 * 2);
		SDL_RenderLine(engine.dbg_renderer, wrap_x * 2 + offset, 0, wrap_x * 2 + offset, wrap_y * 2);
	}
	else {
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, x * 2 + offset, (y + 144) * 2);
		SDL_RenderLine(engine.dbg_renderer, wrap_x * 2 + offset, y * 2, wrap_x * 2 + offset, (y + 144) * 2);
	}

	offset = 0;
	if (system.lcdc & 0b01000000) {
		offset = 512;
	}
	if (system.lcdc & 0b00100000) {
		SDL_SetRenderDrawColor(engine.dbg_renderer, 100, 100, 255, 255);
	}
	else {
		SDL_SetRenderDrawColor(engine.dbg_renderer, 255, 255, 100, 255);
	}

	x = (float)(system.wx - 7);
	y = system.wy;
	// Rewrite this to show the opposite of what it currently shows
	if (x + 160 > 256) {
		// top
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, 256 * 2 + offset, y * 2);
	}
	else {
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, (x + 160) * 2 + offset, y * 2);
	}

	if (y + 144 > 256) {
		//left
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, x * 2 + offset, 256 * 2);
	}
	else {
		SDL_RenderLine(engine.dbg_renderer, x * 2 + offset, y * 2, x * 2 + offset, (y + 144) * 2);
	}
}
*/

/*
// Depreciated function that does not utilize the new rendering system
void chemuGB::drawDebugRegs() {
	cpe::Pixel white = cpe::Pixel{ 255,255,255 };
	cpe::Pixel green = cpe::Pixel{   0,255,100 };
	cpe::Pixel red =   cpe::Pixel{ 255,  0,  50 };

	int DEBUG_OFFSET = 0;
	int Z_OFFSET = 6 * SCALE;
	int N_OFFSET = 10 * SCALE;
	int H_OFFSET = 14 * SCALE;
	int C_OFFSET = 18 * SCALE;

	engine.DrawString(DEBUG_OFFSET, 1 * SCALE, "Registers", white);

	// AF Regs
	engine.DrawString(DEBUG_OFFSET, (2 + 1) * SCALE, std::format("A: ${:02X} [{:3}]  AF: ${:04X}\nF:              [{:5}]",
																	system.cpu.A(), system.cpu.A(), system.cpu.AF(),
																									system.cpu.AF()), white);
	// Overlay flags with color
	engine.DrawString(DEBUG_OFFSET + Z_OFFSET, ( 4 + 1) * SCALE, "Z", (system.cpu.getFlag(FLAG_Z) ? green : red));
	engine.DrawString(DEBUG_OFFSET + N_OFFSET, ( 4 + 1) * SCALE, "N", (system.cpu.getFlag(FLAG_N) ? green : red));
	engine.DrawString(DEBUG_OFFSET + H_OFFSET, ( 4 + 1) * SCALE, "H", (system.cpu.getFlag(FLAG_H) ? green : red));
	engine.DrawString(DEBUG_OFFSET + C_OFFSET, ( 4 + 1) * SCALE, "C", (system.cpu.getFlag(FLAG_C) ? green : red));

	// BC Regs
	engine.DrawString(DEBUG_OFFSET, (8 + 1) * SCALE, std::format("B: ${:02X} [{:3}]  BC: ${:04X}\nC: ${:02X} [{:3}]    [{:5}]",
																	system.cpu.B(), system.cpu.B(), system.cpu.BC(),
																	system.cpu.C(), system.cpu.C(), system.cpu.BC()), white);
	// DE Regs
	engine.DrawString(DEBUG_OFFSET, (14 + 1) * SCALE, std::format("D: ${:02X} [{:3}]  DE: ${:04X}\nE: ${:02X} [{:3}]    [{:5}]",
																	system.cpu.D(), system.cpu.D(), system.cpu.DE(),
																	system.cpu.E(), system.cpu.E(), system.cpu.DE()), white);
	// HL Regs
	engine.DrawString(DEBUG_OFFSET, (20 + 1) * SCALE, std::format("H: ${:02X} [{:3}]  HL: ${:04X}\nL: ${:02X} [{:3}]    [{:5}]",
																	system.cpu.H(), system.cpu.H(), system.cpu.HL(),
																	system.cpu.L(), system.cpu.L(), system.cpu.HL()), white);
	engine.DrawString(DEBUG_OFFSET, (26 + 1) * SCALE, std::format("SP: ${:04X}       [{:5}]", system.cpu.SP(), system.cpu.SP()), white);
	engine.DrawString(DEBUG_OFFSET, (28 + 1) * SCALE, std::format("PC: ${:04X}       [{:5}]", system.cpu.PC(), system.cpu.PC()), white);

}
*/