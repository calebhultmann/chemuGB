#pragma once

#include "emulator.h"
#include <iostream>
#include <SDL3/SDL_render.h>

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

void chemuGB::drawDebug() {
	cpe::Pixel white = cpe::Pixel{ 255,255,255 };
	int debug_x = 0;
	//std::cout << "active: ";
	//if (system.joyp & DPAD_ENABLE) {
	//	std::cout << "DPAD: ";
	//	if (~system.joypad.dpad & KEY_RIGHT) {
	//		std::cout << "RIGHT, ";
	//	}
	//	if ((system.joyp & 0b0001) == 0) {
	//		std::cout << "(1), ";
	//	}
	//	
	//	if (~system.joypad.dpad & KEY_LEFT) {
	//		std::cout << "LEFT, ";
	//	}
	//	if ((system.joyp & 0b0010) == 0) {
	//		std::cout << "(2), ";
	//	}

	//	if (~system.joypad.dpad & KEY_UP) {
	//		std::cout << "UP, ";
	//	}
	//	if ((system.joyp & 0b0100) == 0) {
	//		std::cout << "(3), ";
	//	}
	//	
	//	if (~system.joypad.dpad & KEY_DOWN) {
	//		std::cout << "DOWN, ";
	//	}
	//	if ((system.joyp & 0b1000) == 0) {
	//		std::cout << "(4), ";
	//	}

	//}
	//if (system.joyp & BUTTON_ENABLE) {
	//	std::cout << "BUTTONS: ";
	//	if (~system.joypad.buttons & (KEY_A & KEY)) {
	//		std::cout << "A, ";
	//	}
	//	if ((system.joyp & 0b0001) == 0) {
	//		std::cout << "(1), ";
	//	}

	//	if (~system.joypad.buttons & (KEY_B & KEY)) {
	//		std::cout << "B, ";
	//	}
	//	if ((system.joyp & 0b0010) == 0) {
	//		std::cout << "(2), ";
	//	}

	//	if (~system.joypad.buttons & (KEY_SELECT & KEY)) {
	//		std::cout << "SELECT, ";
	//	}
	//	if ((system.joyp & 0b0100) == 0) {
	//		std::cout << "(3), ";
	//	}

	//	if (~system.joypad.buttons & (KEY_START & KEY)) {
	//		std::cout << "START, ";
	//	}
	//	if ((system.joyp & 0b1000) == 0) {
	//		std::cout << "(4), ";
	//	}
	//}
	//std::cout << "\n";
	//drawDebugRegs();

	//engine.DrawString(debug_x, (32 + 1) * SCALE, "Instructions: ", white);
	//engine.DrawString(debug_x, (34 + 1) * SCALE, std::format("${:04X}: ", system.cpu.PC()), white);

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

void chemuGB::start() {
	bool done = false;
	bool pause = true;

	constexpr uint64_t INPUT_POLL_INTERVAL_MS = 2;
	uint64_t last_input_poll = SDL_GetTicks();
	SDL_Event event;

	while (!done) {
		uint64_t now = SDL_GetTicks();
		
		if (now - last_input_poll >= INPUT_POLL_INTERVAL_MS) {
			last_input_poll = now;

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_EVENT_QUIT:
					done = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.scancode) {
					// P - Pause
					case SDL_SCANCODE_P:
						pause = !pause;
						break;
					// Q - Quit
					case SDL_SCANCODE_Q:
						done = true;
						break;
					// G - Green/Grayscale
					case SDL_SCANCODE_G:
						engine.palette = 1 - engine.palette;
						break;
					// W - D-pad Up
					case SDL_SCANCODE_W:
						system.joypad.press(KEY_UP);
						break;
					// A - D-pad Left
					case SDL_SCANCODE_A:
						system.joypad.press(KEY_LEFT);
						break;
					// S - D-pad Down
					case SDL_SCANCODE_S:
						system.joypad.press(KEY_DOWN);
						break;
					// D - D-pad Right
					case SDL_SCANCODE_D:
						system.joypad.press(KEY_RIGHT);
						break;
					// N - B Button
					case SDL_SCANCODE_N:
						system.joypad.press(KEY_B);
						break;
					// M - A button
					case SDL_SCANCODE_M:
						system.joypad.press(KEY_A);
						break;
					// V - Select
					case SDL_SCANCODE_V:
						system.joypad.press(KEY_SELECT);
						break;
					// B - Start
					case SDL_SCANCODE_B:
						system.joypad.press(KEY_START);
						break;
					}
					break;
				case SDL_EVENT_KEY_UP:
					switch (event.key.scancode) {
						// W - D-pad Up
					case SDL_SCANCODE_W:
						system.joypad.release(KEY_UP);
						break;
						// A - D-pad Left
					case SDL_SCANCODE_A:
						system.joypad.release(KEY_LEFT);
						break;
						// S - D-pad Down
					case SDL_SCANCODE_S:
						system.joypad.release(KEY_DOWN);
						break;
						// D - D-pad Right
					case SDL_SCANCODE_D:
						system.joypad.release(KEY_RIGHT);
						break;
						// N - B Button
					case SDL_SCANCODE_N:
						system.joypad.release(KEY_B);
						break;
						// M - A button
					case SDL_SCANCODE_M:
						system.joypad.release(KEY_A);
						break;
						// V - Select
					case SDL_SCANCODE_V:
						system.joypad.release(KEY_SELECT);
						break;
						// B - Start
					case SDL_SCANCODE_B:
						system.joypad.release(KEY_START);
						break;
					}
					break;
				}
				if (event.type == SDL_EVENT_QUIT) {
					done = true;
				}
			}
		}

		if (!pause) {
			system.clock();
			if (system.ppu.is_frame_ready()) {
				engine.renderFrame(system.ppu.current_frame);
				drawDebug();
				SDL_RenderPresent(engine.dbg_renderer);
			}
		}
	}

	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}