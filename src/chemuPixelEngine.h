#pragma once

#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <vector>
#include <stdint.h>
#include <string>
#include "errors.h"
#include <array>

namespace cpe
{
	struct Pixel {
		Pixel();
		Pixel(uint8_t r, uint8_t g, uint8_t b);

		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
	// I don't think I need this struct. But I would have to change my font texture creation
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
										 // White       Light Gray  Dark Gray   Black
		uint32_t gameboy_palette[2][4] = { {0xFFFFFFFF, 0xFFA9A9A9, 0xFF545454, 0xFF000000},   // Grayscale
										   {0xFF9CC70F, 0xFF8AAC0F, 0xFF306230, 0xFF0F380F} }; // Greenscale
		int palette = 1;

		pixelEngine();

		SDL_Window* window = nullptr;
		SDL_Window* dbg_window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Renderer* dbg_renderer = nullptr;
		SDL_Texture* screen = nullptr;
		SDL_Texture* dbg_screen = nullptr;

		Sprite font;
		SDL_Texture* fontTexture = nullptr;

		static constexpr int GB_W = 160;
		static constexpr int GB_H = 144;
		std::array<uint32_t, GB_W * GB_H> frame_buffer;
		
		void constructFontSheet();
		int initialize(bool debug);
		void renderFrame(uint8_t current_frame[]);
		void DrawString(int x, int y, std::string text, Pixel color);
	};
}