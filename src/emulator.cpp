#pragma once

#include "emulator.h"
#include <iostream>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL.h>
#include <chrono>

chemuGB::chemuGB() {
	DEBUG = true;
}

chemuGB::~chemuGB() {
	delete disassembler;
}

int chemuGB::initialize(std::filesystem::path romPath, uint8_t flags) {
	int status = engine.initialize(false);
	if (status != Error::None) {
		return status;
	}
	
	status = system.insertCartridge(romPath);
	if (status != Error::None) {
		return status;
	}

	if (DEBUG) {
		disassembler = new Disassembler(&system.cpu);
		//disassembler->disassembleROM(system.cart->romBanks);
	}
	return 0;
}

void chemuGB::drawDebugRegs() {
	cpe::Pixel white = cpe::Pixel{ 255,255,255 };
	cpe::Pixel green = cpe::Pixel{   0,255,100 };
	cpe::Pixel red =   cpe::Pixel{ 255,  0,  50 };

	int DEBUG_OFFSET = 161 * SCALE;
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

void chemuGB::drawDebug() {
	cpe::Pixel white = cpe::Pixel{ 255,255,255 };
	int debug_x = 161 * SCALE;
	drawDebugRegs();

	engine.DrawString(debug_x, (32 + 1) * SCALE, "Instructions: ", white);
	engine.DrawString(debug_x, (34 + 1) * SCALE, std::format("${:04X}: ", system.cpu.PC()), white);

	// Draw Tileblocks
	// For each tileblock
	for (int tileblock = 0; tileblock < 3; tileblock++) {
		uint16_t block_start_addr = 0x800 * tileblock;
		int tileblock_x = 161;
		int tileblock_y = 93 + 17 * tileblock;

		// For each tile
		for (int tile_id = 0; tile_id < 16 * 8; tile_id += 1) {
			uint16_t tile_start_addr = block_start_addr + tile_id * 16;
			int tile_x = (tileblock_x + ((tile_id % 16) * 2)) * 8;
			int tile_y = (tileblock_y + ((tile_id / 16) * 2)) * 8;

			// For each horizontal line
			for (int line_y = 0; line_y < 8; line_y++) {
				uint8_t data_low = system.ppu.vram[tile_start_addr + line_y * 2];
				uint8_t	data_high = system.ppu.vram[tile_start_addr + line_y * 2 + 1];

				// For each pixel in that line
				for (int bit = 0; bit < 8; bit++) {
					int high_bit = ((data_high & (0b10000000 >> bit)) >> (7 - bit)) << 1;
					int low_bit = (data_low & (0b10000000 >> bit)) >> (7 - bit);
					int pixel_color_id = high_bit | low_bit;
					int pixel_color = (system.bgp & (0b11 << (2 * pixel_color_id))) >> (2 * pixel_color_id);
					//cpe::Pixel color = engine.gameboy_palette[engine.palette][pixel_color];
					//SDL_SetRenderDrawColor(engine.renderer, color.r, color.g, color.b, 255);
					int pixel_x = tile_x + bit * 2;
					int pixel_y = tile_y + line_y * 2;

					SDL_FRect rect = { float(pixel_x), float(pixel_y), float(2), float(2) };
					SDL_RenderFillRect(engine.renderer, &rect);
				}
			}
		}
	}
	

	// Draw Tilemaps
	// For each tilemap
	for (int tilemap = 0; tilemap < 2; tilemap++) {
		int tilemap_x = 194;
		int tilemap_y = 78 + 33 * tilemap;
		uint16_t tilemap_start_addr = 0x1800 + 0x400 * tilemap;
		// For each tile
		for (int tile_x = 0; tile_x < 32; tile_x++) {
			for (int tile_y = 0; tile_y < 32; tile_y++) {
				int tile_pixel_x = tilemap_x + tile_x;
				int tile_pixel_y = tilemap_y + tile_y;
				uint16_t tile_start_addr = tilemap_start_addr + 32 * tile_y + tile_x;
				uint8_t tile_id = system.ppu.vram[tile_start_addr];
				tile_start_addr = tile_id * 16;

				// For each pixel
				for (int line_y = 0; line_y < 8; line_y++) {
					uint8_t data_low = system.ppu.vram[tile_start_addr + line_y * 2];
					uint8_t	data_high = system.ppu.vram[tile_start_addr + line_y * 2 + 1];

					// For each pixel in that line
					for (int bit = 0; bit < 8; bit++) {
						int high_bit = ((data_high & (0b10000000 >> bit)) >> (7 - bit)) << 1;
						int low_bit = (data_low & (0b10000000 >> bit)) >> (7 - bit);
						int pixel_color_id = high_bit | low_bit;
						int pixel_color = (system.bgp & (0b11 << (2 * pixel_color_id))) >> (2 * pixel_color_id);
						//cpe::Pixel color = engine.greenscale[pixel_color];
						//SDL_SetRenderDrawColor(engine.renderer, color.r, color.g, color.b, 255);
						int pixel_x = tile_pixel_x * 8 + bit;
						int pixel_y = tile_pixel_y * 8 + line_y;

						SDL_FRect rect = { float(pixel_x), float(pixel_y), float(1), float(1) };
						SDL_RenderFillRect(engine.renderer, &rect);
					}
				}
			}
		}
	}


	SDL_SetRenderDrawColor(engine.renderer, 255, 100, 100, 255);
	SDL_FRect outline = { float(194 * 8 + system.scx), float(78 * 8 + system.scy), float(160), float(144) };
	SDL_RenderRect(engine.renderer, &outline);


	/*
	DrawString(debug_x, (34 + 1) * SCALE, " $4FA1: ADD A, R16", white);
	DrawString(debug_x, (36 + 1) * SCALE, " $4FA3: XOR A, IMM8", white);
	...
	DrawString(debug_x, (62 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (64 + 1) * SCALE, ">$0000:       etc. ", white);
	DrawString(debug_x, (66 + 1) * SCALE, " $4FA3: XOR A, IMM8", white);
	...
	DrawString(debug_x, (94 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (96 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });*/

}

void chemuGB::start() {
	bool done = false;
	bool pause = true;

	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				done = true;
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (event.key.scancode) {
				case SDL_SCANCODE_P:
					pause = !pause;
					break;
				case SDL_SCANCODE_Q:
					done = true;
					break;
				case SDL_SCANCODE_F:
					// draw debug frame
					//drawDebug();
					//SDL_RenderPresent(engine.renderer);
					break;
				case SDL_SCANCODE_G:
					engine.palette = 1 - engine.palette;
				}
				break;
			}
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}

		if (!pause) {
			system.clock();
			if (system.ppu.is_frame_ready()) {
				engine.renderFrame(system.ppu.current_frame);
			}
		}
	}

	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}