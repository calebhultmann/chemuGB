#include "debugger.h"
#include <SDL3/SDL.h>
#include "../emulator.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/backends/imgui_impl_sdl3.h"
#include "../external/imgui/backends/imgui_impl_sdlrenderer3.h"
#include "../renderer/chemuPixelEngine.h"
#include "hooks.h"

Debugger::Debugger() {

}

Debugger::~Debugger() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
#include <iostream>
void Debugger::init(pixelEngine& eng, chemuGB& gb) {
	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	window = SDL_CreateWindow("debug", 960, 540, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, NULL);

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	engine = &eng;

	// Initialize necessary textures
	vramtexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		128,
		192
	);

	composite_layer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		160,
		144
	);

	gb.system.ppu.hooks.on_render_event = [&](const RenderEvent& event) {
		int pixel_index = event.y * 160 + event.x;
		switch (event.type) {
		case RenderEventType::BG_PIXEL:
			bg_buffer[pixel_index].color_index = event.color_index;
			break;
		case RenderEventType::WINDOW_PIXEL:
			window_buffer[pixel_index].color_index = event.color_index;
			window_buffer[pixel_index].window_on = event.window_on;
			break;
		case RenderEventType::OBJECT_PIXEL:
			obj_buffer[pixel_index].color_index = event.color_index;
			obj_buffer[pixel_index].priority = event.priority;
			obj_buffer[pixel_index].palette = event.palette;
			break;
		}

	};

}

bool Debugger::handle_event(const SDL_Event& event) {
	switch (event.type) {
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		ImGui_ImplSDL3_ProcessEvent(&event);
		return false;
	case SDL_EVENT_MOUSE_BUTTON_UP:
		ImGui_ImplSDL3_ProcessEvent(&event);
		return false;
	case SDL_EVENT_MOUSE_WHEEL:
		ImGui_ImplSDL3_ProcessEvent(&event);
		return true;
	case SDL_EVENT_MOUSE_MOTION:
		ImGui_ImplSDL3_ProcessEvent(&event);
		return true;
	}
	return false;
}

void Debugger::draw_registers(const chemuGB& gb) {
	using namespace ImGui;
	auto& cpu = gb.system.cpu;

	if (BeginTable("regs", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedSame)) {
		TableNextRow();
		TableSetColumnIndex(0);
		Text("B");
		TableSetColumnIndex(1);
		Text("%02X", cpu.bc.high);
		TableSetColumnIndex(2);
		Text("  %3d", cpu.bc.high);
		TableSetColumnIndex(3);
		Text("BC");
		TableSetColumnIndex(4);
		Text(" %04X", cpu.bc.reg);
		TableNextRow();
		TableSetColumnIndex(0);
		Text("C");
		TableSetColumnIndex(1);
		Text("%02X", cpu.bc.low);
		TableSetColumnIndex(2);
		Text("  %3d", cpu.bc.low);
		TableSetColumnIndex(4);
		Text("%5d", cpu.bc.reg);

		TableNextRow();
		TableSetColumnIndex(0);
		Text("D");
		TableSetColumnIndex(1);
		Text("%02X", cpu.de.high);
		TableSetColumnIndex(2);
		Text("  %3d", cpu.de.high);
		TableSetColumnIndex(3);
		Text("DE");
		TableSetColumnIndex(4);
		Text(" %04X", cpu.de.reg);
		TableNextRow();
		TableSetColumnIndex(0);
		Text("E");
		TableSetColumnIndex(1);
		Text("%02X", cpu.de.low);
		TableSetColumnIndex(2);
		Text("  %3d", cpu.de.low);
		TableSetColumnIndex(4);
		Text("%5d", cpu.de.reg);

		TableNextRow();
		TableSetColumnIndex(0);
		Text("H");
		TableSetColumnIndex(1);
		Text("%02X", cpu.hl.high);
		TableSetColumnIndex(2);
		Text("  %3d", cpu.hl.high);
		TableSetColumnIndex(3);
		Text("HL");
		TableSetColumnIndex(4);
		Text(" %04X", cpu.hl.reg);
		TableNextRow();
		TableSetColumnIndex(0);
		Text("L");
		TableSetColumnIndex(1);
		Text("%02X", cpu.hl.low);
		TableSetColumnIndex(2);
		Text("  %3d", cpu.hl.low);
		TableSetColumnIndex(4);
		Text("%5d", cpu.hl.reg);

		TableNextRow();
		TableSetColumnIndex(0);
		Text("PC");
		TableSetColumnIndex(2);
		Text(" %04X", cpu.pc);
		TableSetColumnIndex(3);
		Text("SP");
		TableSetColumnIndex(4);
		Text(" %04X", cpu.sp);
		TableNextRow();
		TableSetColumnIndex(2);
		Text("%5d", cpu.pc);
		TableSetColumnIndex(4);
		Text("%5d", cpu.sp);

		EndTable();
	}
}

void Debugger::draw_vram(const chemuGB& gb) {
	auto& vram = gb.system.ppu.vram;

	for (int tileblock = 0; tileblock < 3; tileblock++) {
		uint16_t block_start_addr = 0x800 * tileblock;

		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 16; x++) {
				int tile_index = (y / 8) * 16 + x;
				uint16_t tile_addr = block_start_addr + tile_index * 16;
				uint16_t tile_line_addr = tile_addr + (y % 8) * 2;
				uint8_t data_low = vram[tile_line_addr];
				uint8_t	data_high = vram[tile_line_addr + 1];

				for (int bit = 0; bit < 8; bit++) {
					int high_bit = ((data_high & (0b10000000 >> bit)) >> (7 - bit)) << 1;
					int low_bit = (data_low & (0b10000000 >> bit)) >> (7 - bit);
					int pixel_color_id = high_bit | low_bit;
					int pixel_color = (gb.system.bgp & (0b11 << (2 * pixel_color_id))) >> (2 * pixel_color_id);
					uint32_t color = engine->gameboy_palette[engine->palette][pixel_color];
					tileblock_buffer[(y * 128) + (x * 8) + bit] = color;
				}
			}
		}

		SDL_Rect tileblock_rect = { 0, 64 * tileblock, 128, 64 };
		SDL_UpdateTexture(
			vramtexture,
			&tileblock_rect,
			tileblock_buffer.data(),
			128 * sizeof(uint32_t)
		);
	}

	ImGui::Begin("Tile Viewer");

	ImGui::Image(
		(ImTextureID)(intptr_t)vramtexture,
		ImVec2(128 * 2, 192 * 2)
	);

	ImGui::End();
}

void Debugger::compose_background_layer(const chemuGB& gb) {
	for (size_t i = 0; i < GB_SIZE; i++) {	
		composite_buffer[i].source_layer = Layer::BG;
		composite_buffer[i].color_index = bg_buffer[i].color_index;
		composite_buffer[i].palette = gb.system.bgp;
		composite_buffer[i].pixel_on = true;
	}
}

void Debugger::compose_window_layer(const chemuGB& gb) {
	for (size_t i = 0; i < GB_SIZE; i++) {
		if (!window_buffer[i].window_on) {
			continue;
		}

		composite_buffer[i].source_layer = Layer::Window;
		composite_buffer[i].color_index = window_buffer[i].color_index;
		composite_buffer[i].palette = gb.system.bgp;
		composite_buffer[i].pixel_on = true;
	}
}

void Debugger::compose_object_layer(const chemuGB& gb) {
	for (size_t i = 0; i < GB_SIZE; i++) {
		if (obj_buffer[i].color_index == 0) {
			continue;
		}

		if ((obj_buffer[i].priority == 0) ||
			(composite_buffer[i].color_index == 0)) {
			composite_buffer[i].source_layer = Layer::OBJ;
			composite_buffer[i].color_index = obj_buffer[i].color_index;
			composite_buffer[i].palette = obj_buffer[i].palette;
			composite_buffer[i].pixel_on = true;
		}
	}
}

void Debugger::draw_composed_layers() {
	for (size_t i = 0; i < GB_SIZE; i++) {
		if (!composite_buffer[i].pixel_on) {
			continue;
		}

		uint8_t color = (composite_buffer[i].palette & (0b11 << (2 * composite_buffer[i].color_index))) >> (2 * composite_buffer[i].color_index);
		composite_data_buffer[i] = engine->gameboy_palette[engine->palette][color];
	}
}

void Debugger::draw_layers(const chemuGB& gb) {
	CompositePixelData zero = { Layer::BG, 0, 0, 0 };

	std::fill(composite_buffer.begin(), composite_buffer.end(), zero);
	std::fill(composite_data_buffer.begin(), composite_data_buffer.end(), 0);

	ImGui::Begin("Layer Viewer");
	ImGui::Checkbox("Background", &background_layer);
	ImGui::Checkbox("Window", &window_layer);
	ImGui::Checkbox("Objects", &objects_layer);
	if (background_layer) {
		compose_background_layer(gb);
	}
	if (window_layer) {
		compose_window_layer(gb);
	}
	if (objects_layer) {
		compose_object_layer(gb);
	}

	draw_composed_layers();

	SDL_UpdateTexture(
		composite_layer_texture,
		nullptr,
		composite_data_buffer.data(),
		GB_W * sizeof(uint32_t)
	);

	ImGui::Image(
		(ImTextureID)(intptr_t)composite_layer_texture,
		ImVec2(160 * 2, 144 * 2)
	);

	ImGui::End();
}
//void Debugger::draw_background(const chemuGB& gb) {
//	auto& vram = gb.system.ppu.vram;
//	uint32_t scanline_buffer[160];
//
//	uint8_t tile_x_index = 0;
//	uint8_t tile_data_low = 0;
//	uint8_t tile_data_high = 0;
//
//	int curr_bit = -1;
//	curr_bit -= gb.system.scx % 8;
//
//	for (uint8_t curr_pixel = 0; curr_pixel < 160; curr_pixel++) {
//		// Get next tile from background/window
//		if (curr_bit < 0) {
//			uint8_t tile_x;
//			uint8_t tile_y;
//
//			tile_x = ((gb.system.scx / 8) + tile_x_index) & 0x1F;
//			tile_y = (gb.system.ly + gb.system.scy) & 0xFF;
//			curr_bit += 8;
//
//			uint8_t tile_index = gb.system.ppu.getIdFromTilemap(false, tile_x, tile_y);
//			uint16_t tile_address = gb.system.ppu.getTileAddress(tile_index);
//
//			tile_data_low = vram[tile_address + (2 * (tile_y % 8))];
//			tile_data_high = vram[tile_address + (2 * (tile_y % 8)) + 1];
//
//			tile_x_index++;
//		}
//
//		uint8_t high_bit = ((tile_data_high & (0b10000000 >> (7 - curr_bit))) >> curr_bit) << 1;
//		uint8_t low_bit = (tile_data_low & (0b10000000 >> (7 - curr_bit))) >> curr_bit;
//
//		uint8_t color = high_bit | low_bit;
//
//		scanline_buffer[curr_pixel] = engine->gameboy_palette[engine->palette][color];
//		curr_bit--;
//
//		//SDL_Rect tilemap_rect = { 0, 0, 256, 256 };
//
//		//SDL_UpdateTexture(
//		//	engine.dbg_screen,
//		//	&tilemap_rect,
//		//	tilemap_buffer.data(),
//		//	256 * sizeof(uint32_t)
//		//);
//	}
//}

void Debugger::frame(const chemuGB& gb) {
	begin_frame();
	draw(gb);
	end_frame();
}

void Debugger::begin_frame() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();
}

void Debugger::draw(const chemuGB& gb) {
	draw_registers(gb);
	draw_vram(gb);
	draw_layers(gb);
}

void Debugger::end_frame() {
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}