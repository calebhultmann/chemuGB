#pragma once

#include "emulator.h"
#include <iostream>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL.h>

chemuGB::chemuGB() {
	SCALE = 8;
}

chemuGB::chemuGB(int s) {
	SCALE = s;
}

chemuGB::~chemuGB() {

}

bool chemuGB::initialize() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return false;
	}

	window = SDL_CreateWindow("Test SDL Window", (160 + 48) * SCALE, 144 * SCALE, 0);
	if (window == NULL) {
		SDL_Log("Window could not be created! %s", SDL_GetError());
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(window, NULL);
	font.toTexture(renderer, fontTexture);
	SDL_SetTextureScaleMode(fontTexture, SDL_SCALEMODE_NEAREST);

	for (int i = 0; i < 160; i++) {
		for (int j = 0; j < 144; j++) {
			if ((j + i) % 2) {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
			}
			SDL_FRect rect = { float(i * 8), float(j * 8), float(8), float(8) };
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	return true;
}

void chemuGB::start() {
	DrawString(161*SCALE, 1*SCALE, "Registers", cpe::Pixel{ 255,255,255 });
	DrawString(161 * SCALE, ((2 + 1) * SCALE), "A: $FF [255]  AF: $FFFF\nF: Z N H C      [65535]", cpe::Pixel{ 255,255,255 });
	DrawString(161 * SCALE, ((8 + 1) * SCALE), "B: $FF [255]  BC: $FFFF\nC: $FF [255]    [65535]", cpe::Pixel{ 255,255,255 });
	DrawString(161 * SCALE, ((14 + 1) * SCALE), "D: $FF [255]  DE: $FFFF\nE: $FF [255]    [65535]", cpe::Pixel{ 255,255,255 });
	DrawString(161 * SCALE, ((20 + 1) * SCALE), "H: $FF [255]  HL: $FFFF\nL: $FF [255]    [65535]", cpe::Pixel{ 255,255,255 });
	DrawString(161 * SCALE, ((26 + 1) * SCALE), "SP: $FFFF       [65535]", cpe::Pixel{ 255,255,255 });
	DrawString(161 * SCALE, ((28 + 1) * SCALE), "PC: $FFFF       [65535]", cpe::Pixel{ 255,255,255 });

	DrawString(161 * SCALE, (32+1) * SCALE, "Instructions: ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (34+1) * SCALE, " $4FA1: ADD A, R16", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (36+1) * SCALE, " $4FA3: XOR A, IMM8", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (38+1) * SCALE, " $04A5: RLCA", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (40+1) * SCALE, " $04A6: INC B", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (42+1) * SCALE, " $04A7: LD [BC], A", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (44+1) * SCALE, " $04A9: CB: SWAP E", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (46+1) * SCALE, " $04AB: LDH: A, [C]", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (48+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (50+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (52+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (54+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (56+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (58+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (60+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (62+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (64+1) * SCALE, ">$0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (66+1) * SCALE, " $4FA3: XOR A, IMM8", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (68+1) * SCALE, " $04A5: RLCA", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (70+1) * SCALE, " $04A6: INC B", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (72+1) * SCALE, " $04A7: LD [BC], A", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (74+1) * SCALE, " $04A9: CB: SWAP E", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (76+1) * SCALE, " $04AB: LDH: A, [C]", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (78+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (80+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (82+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (84+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (86+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (88+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (90+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (92+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (94+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });
	DrawString(161 * SCALE, (96+1) * SCALE, " $0000:       etc. ", cpe::Pixel{ 255 ,255 ,255 });



	SDL_RenderPresent(renderer);

	bool done = false;
	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
}