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

	window = SDL_CreateWindow("debug", 1920, 1080, 0);
	renderer = SDL_CreateRenderer(window, NULL);

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	
	ImGui_ImplSDLRenderer3_Init(renderer);
}

bool Debugger::handle_event(const SDL_Event& event) {
	return true;
}

void Debugger::frame(const chemuGB& gb) {
	begin_frame();
	draw(gb);
	end_frame();
}

void Debugger::begin_frame() {
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();

	ImGui::NewFrame();
}

void Debugger::draw(const chemuGB& gb) {
	ImGui::Begin("Debugger");

	ImGui::Text("Hello from debugger");

	ImGui::End();
}

void Debugger::end_frame() {
	ImGui::Render();
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	SDL_RenderPresent(renderer);
}