#ifndef DL_SDL3_APPDATA_H
#define DL_SDL3_APPDATA_H
#include <SDL3/SDL.h>
struct AppData {
	static constexpr int width {1280};
	static constexpr int height {720};

	SDL_Renderer *renderer{nullptr};
	SDL_Window *window {nullptr};

	int lastTime {0};
	float deltaTime {0.0f};

};

#endif