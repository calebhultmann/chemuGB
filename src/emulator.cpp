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
	int status = engine.initialize(true);
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
	/*
	DrawString(debug_x, (34 + 1) * SCALE, " $4FA1: ADD A, R16", white);
	DrawString(debug_x, (36 + 1) * SCALE, " $4FA3: XOR A, IMM8", white);
	DrawString(debug_x, (38 + 1) * SCALE, " $04A5: RLCA", white);
	DrawString(debug_x, (40 + 1) * SCALE, " $04A6: INC B", white);
	DrawString(debug_x, (42 + 1) * SCALE, " $04A7: LD [BC], A", white);
	DrawString(debug_x, (44 + 1) * SCALE, " $04A9: CB: SWAP E", white);
	DrawString(debug_x, (46 + 1) * SCALE, " $04AB: LDH: A, [C]", white);
	DrawString(debug_x, (48 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (50 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (52 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (54 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (56 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (58 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (60 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (62 + 1) * SCALE, " $0000:       etc. ", white);
	DrawString(debug_x, (64 + 1) * SCALE, ">$0000:       etc. ", white);
	DrawString(debug_x, (66 + 1) * SCALE, " $4FA3: XOR A, IMM8", white);
	DrawString(debug_x, (68 + 1) * SCALE, " $04A5: RLCA", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (70 + 1) * SCALE, " $04A6: INC B", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (72 + 1) * SCALE, " $04A7: LD [BC], A", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (74 + 1) * SCALE, " $04A9: CB: SWAP E", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (76 + 1) * SCALE, " $04AB: LDH: A, [C]", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (78 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (80 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (82 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (84 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (86 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (88 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (90 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (92 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (94 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(debug_x, (96 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });*/

}

void chemuGB::start() {




	//print rom dump

	bool done = false;
	int frame = 0;
	bool display = false;
	std::chrono::steady_clock::time_point start;
	
	while (!done) {
		if (frame == 0) {
			start = std::chrono::steady_clock::now();
		}
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}

		//if (!display && system.cpu.HL() == 0x7FFF) {
		//	display = true;
		//}
		//if (display) {
		//	if (DEBUG) {
		//		drawDebug();
		//	}
		//	SDL_RenderPresent(engine.renderer);
		//}



		system.clock();

		if (++frame == 16384) {
			auto end = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

			std::cout << "Time: " << duration.count() << "ms" << std::endl;
			frame = 0;
		}
		// Step CPU
		// Draw frame


	}

	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}