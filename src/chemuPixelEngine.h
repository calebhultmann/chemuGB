#pragma once

#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <vector>
#include <stdint.h>
#include <string>

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
		pixelEngine();

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		Sprite font;
		SDL_Texture* fontTexture = nullptr;

		void constructFontSheet();
		void DrawString(int x, int y, std::string text, Pixel color);
	};
}