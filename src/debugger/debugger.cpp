#include "debugger.h"
#include <SDL3/SDL.h>
#include "../emulator.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/backends/imgui_impl_sdl3.h"
#include "../external/imgui/backends/imgui_impl_sdlrenderer3.h"

Debugger::Debugger() {

}

Debugger::~Debugger() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
#include <iostream>
void Debugger::init() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsDark();

	window = SDL_CreateWindow("debug", 480, 270, 0);
	renderer = SDL_CreateRenderer(window, NULL);

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	
	ImGui_ImplSDLRenderer3_Init(renderer);
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
}

void Debugger::end_frame() {
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}