#pragma once

#include "emulator.h"
#include <iostream>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL.h>

chemuGB::chemuGB() {

}

chemuGB::~chemuGB() {

}

int chemuGB::initialize(std::filesystem::path romPath, uint8_t flags) {
	engine.initialize();
	system.insertCartridge(romPath);
}

//void chemuGB::drawDebug() {
//// THIS FUNCTION IS CURRENTLY JUST SCAFFOLDING
//
//	cpe::Pixel white = cpe::Pixel{ 255,255,255 };
//	int debug_x = 161 * SCALE;
//
//	DrawString(debug_x, 1 * SCALE, "Registers", white);
//	DrawString(debug_x, (2 + 1) * SCALE, "A: $FF [255]  AF: $FFFF\nF: Z N H C      [65535]", white);
//	DrawString(debug_x, (8 + 1) * SCALE, "B: $FF [255]  BC: $FFFF\nC: $FF [255]    [65535]", white);
//	DrawString(debug_x, (14 + 1) * SCALE, "D: $FF [255]  DE: $FFFF\nE: $FF [255]    [65535]", white);
//	DrawString(debug_x, (20 + 1) * SCALE, "H: $FF [255]  HL: $FFFF\nL: $FF [255]    [65535]", white);
//	DrawString(debug_x, (26 + 1) * SCALE, "SP: $FFFF       [65535]", white);
//	DrawString(debug_x, (28 + 1) * SCALE, "PC: $FFFF       [65535]", white);
//	DrawString(debug_x, (32 + 1) * SCALE, "Instructions: ", white);
//	DrawString(debug_x, (34 + 1) * SCALE, " $4FA1: ADD A, R16", white);
//	DrawString(debug_x, (36 + 1) * SCALE, " $4FA3: XOR A, IMM8", white);
//	DrawString(debug_x, (38 + 1) * SCALE, " $04A5: RLCA", white);
//	DrawString(debug_x, (40 + 1) * SCALE, " $04A6: INC B", white);
//	DrawString(debug_x, (42 + 1) * SCALE, " $04A7: LD [BC], A", white);
//	DrawString(debug_x, (44 + 1) * SCALE, " $04A9: CB: SWAP E", white);
//	DrawString(debug_x, (46 + 1) * SCALE, " $04AB: LDH: A, [C]", white);
//	DrawString(debug_x, (48 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (50 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (52 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (54 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (56 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (58 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (60 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (62 + 1) * SCALE, " $0000:       etc. ", white);
//	DrawString(debug_x, (64 + 1) * SCALE, ">$0000:       etc. ", white);
//	DrawString(debug_x, (66 + 1) * SCALE, " $4FA3: XOR A, IMM8", white);
//	DrawString(debug_x, (68 + 1) * SCALE, " $04A5: RLCA", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (70 + 1) * SCALE, " $04A6: INC B", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (72 + 1) * SCALE, " $04A7: LD [BC], A", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (74 + 1) * SCALE, " $04A9: CB: SWAP E", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (76 + 1) * SCALE, " $04AB: LDH: A, [C]", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (78 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (80 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (82 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (84 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (86 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (88 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (90 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (92 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (94 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//	DrawString(debug_x, (96 + 1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
//}

void chemuGB::start() {
	drawDebug();
	SDL_RenderPresent(engine.renderer);

	bool done = false;
	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}
	}

	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}