#pragma once

#include <SDL3/SDL_render.h>
#include <array>



class chemuGB;
class pixelEngine;

struct BGPixelData {
	uint8_t color_index;
};

struct WindowPixelData {
	uint8_t color_index;
	bool window_on;
};

struct OBJPixelData {
	uint8_t color_index;
	uint8_t priority;
	uint8_t palette;
};

enum class Layer {
	BG,
	Window,
	OBJ
};

struct CompositePixelData {
	Layer source_layer;
	uint8_t color_index;
	uint8_t palette;
	bool pixel_on;
};

class Debugger {
public:
	Debugger();
	~Debugger();
	void init(pixelEngine&, chemuGB&);

	static constexpr int GB_W = 160;
	static constexpr int GB_H = 144;
	static constexpr int GB_SIZE = GB_W * GB_H;

	pixelEngine* engine = nullptr;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* vramtexture;
	SDL_Texture* composite_layer_texture;

	std::array<uint32_t, 128 * 64> tileblock_buffer;

	std::array<BGPixelData,			GB_SIZE> bg_buffer;
	std::array<WindowPixelData,		GB_SIZE> window_buffer;
	std::array<OBJPixelData,		GB_SIZE> obj_buffer;
	std::array<CompositePixelData,	GB_SIZE> composite_buffer;
	std::array<uint32_t,			GB_SIZE> composite_data_buffer;

	bool background_layer = true;
	bool window_layer = true;
	bool objects_layer = true;

	bool handle_event(const SDL_Event&);

	void draw_registers(const chemuGB&);
	void draw_vram(const chemuGB&);
	void draw_layers(const chemuGB&);
	void compose_background_layer(const chemuGB&);
	void compose_window_layer(const chemuGB&);
	void compose_object_layer(const chemuGB&);
	void draw_composed_layers();

	
	void frame(const chemuGB&);
	void begin_frame();
	void draw(const chemuGB&);
	void end_frame();
};

/*

void chemuGB::drawDebug() {
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