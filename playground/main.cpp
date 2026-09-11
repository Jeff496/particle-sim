// Dear ImGui: standalone example application for SDL3 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL3.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <stdio.h>
#include <SDL3/SDL.h>

#include "aulara/sim/world.h"
#include "aulara/sim/profile.h"

#include <vector>
#include <cstdio>
#include <cstdint>

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// Main code
int main(int, char**)
{
	// resolutions
	// const int W = 480; const int H = 270; // 270p
	const int W = 640; const int H = 360; // 360p - official game resolution maybe?
	// const int W = 854; const int H = 480; // 480p
	// const int W = 1280; const int H = 720; // 720p
	int zoom = 2; // screen pixels per cell

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	if (!SDL_CreateWindowAndRenderer("Aulara playground", W * zoom, H * zoom, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		std::fprintf(stderr, "SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetRenderVSync(renderer, 1);

	// One streaming texture the size of the grid; nearest-neighbor so cells stay crisp when scaled.
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, W, H);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	aulara::World world(W, H, /*seed=*/1);
	std::vector<uint8_t> pixels(static_cast<size_t>(W) * H * 4);

	int brush_material = static_cast<int>(aulara::Material::Sand);
	int brush_radius = 4;
	bool paused = false;
	double step_ms = 0.0;

	// Fixed timestep: the sim always advances in 1/60 s ticks no matter the frame rate.
	const double tick = 1.0 / 60.0;
	const double freq = static_cast<double>(SDL_GetPerformanceFrequency());
	double accumulator = 0.0;
	uint64_t last = SDL_GetPerformanceCounter();

	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL3_ProcessEvent(&e);
			if (e.type == SDL_EVENT_QUIT) {
				running = false;
			}
		}

		const uint64_t now = SDL_GetPerformanceCounter();
		accumulator += static_cast<double>(now - last) / freq;
		last = now;
		if (accumulator > 0.25) {
			accumulator = 0.25; // never try to catch up more than a quarter second
		}

		// Mouse painting (window pixels -> cells), unless the mouse is over an ImGui window.
		if (!ImGui::GetIO().WantCaptureMouse) {
			float mx = 0, my = 0;
			const SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mx, &my);
			if (buttons & SDL_BUTTON_LMASK) {
				const int cx = static_cast<int>(mx) / zoom;
				const int cy = static_cast<int>(my) / zoom;
				for (int dy = -brush_radius; dy <= brush_radius; ++dy) {
					for (int dx = -brush_radius; dx <= brush_radius; ++dx) {
						const int x = cx + dx, y = cy + dy;
						if (dx * dx + dy * dy <= brush_radius * brush_radius && x >= 0 && x < W && y >= 0 && y < H) {
							world.set_cell(x, y, static_cast<aulara::MaterialId>(brush_material));
						}
					}
				}
			}
		}

		while (accumulator >= tick) {
			if (!paused) {
				const uint64_t t0 = SDL_GetPerformanceCounter();
				world.step();
				step_ms = static_cast<double>(SDL_GetPerformanceCounter() - t0) * 1000.0 / freq;
			}
			accumulator -= tick;
		}

		world.render_rgba(pixels.data());
		SDL_UpdateTexture(texture, nullptr, pixels.data(), W * 4);

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Aulara");
		ImGui::Text("step: %.3f ms   frame: %llu", step_ms, static_cast<unsigned long long>(world.frame()));
		ImGui::SliderInt("zoom", &zoom, 1, 4);
		ImGui::SliderInt("brush radius", &brush_radius, 1, 32);
		ImGui::RadioButton("air", &brush_material, static_cast<int>(aulara::Material::Air));
		ImGui::SameLine();
		ImGui::RadioButton("sand", &brush_material, static_cast<int>(aulara::Material::Sand));
		ImGui::SameLine();
		ImGui::RadioButton("stone", &brush_material, static_cast<int>(aulara::Material::Stone));
		ImGui::Checkbox("paused", &paused);
		if (ImGui::Button("fill with sand (stress test)")) {
			for (int y = 0; y < H; ++y) {
				for (int x = 0; x < W; ++x) {
					world.set_cell(x, y, id(aulara::Material::Sand));
				}
			}
		}
		ImGui::End();
		ImGui::Render();

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		const SDL_FRect dst{ 0.0f, 0.0f, static_cast<float>(W * zoom), static_cast<float>(H * zoom) };
		SDL_RenderTexture(renderer, texture, nullptr, &dst);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
		AULARA_FRAME();
	}

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}