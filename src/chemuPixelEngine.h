#pragma once

#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <vector>
#include <stdint.h>
#include <string>
#include "errors.h"

namespace cpe
{
	struct Pixel {
		Pixel();
		Pixel(uint8_t r, uint8_t g, uint8_t b);

		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	struct Sprite {
		Sprite();
		Sprite(int w, int h);

		int width;
		int height;

		std::vector<Pixel> data;

		void resize(int w, int h);
		void toTexture(SDL_Renderer* renderer, SDL_Texture*& texture);

		void setPixel(int x, int y, Pixel color);
		Pixel& getPixel(int x, int y);
	};

	class pixelEngine {
	public:
		int SCALE = 0;
		                     //    White       Light Gray    Dark Gray   Black
		Pixel grayscale[4] = { {255,255,255}, {169,169,169},{84,84,84}, {0,0,0} };
		Pixel greenscale[4] = { {155,188,15}, {139,172,15}, {48,98,48}, {15,56,15} };

		pixelEngine();

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		Sprite font;
		SDL_Texture* fontTexture = nullptr;

		void constructFontSheet();
		int initialize(bool debug);
		void drawLine();
		void DrawString(int x, int y, std::string text, Pixel color);
	};
}