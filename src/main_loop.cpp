#include <cstddef>
#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <iostream>
#include "dl_imgui_utils.h"
#include "AppData.h"

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    std::cout << "App initialised\n";
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO)){
        SDL_Log("Failed to init video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	AppData* appData {new AppData()};

    SDL_SetHint("SDL_RENDER_VSYNC", "1");
    appData->window = SDL_CreateWindow("Low Level Game", appData->width, appData->height, SDL_WINDOW_RESIZABLE);
    if(!appData->window) {
        SDL_Log("Failed to initialise window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    appData->renderer = SDL_CreateRenderer(appData->window, NULL);

    if (!appData->renderer){
    	SDL_DestroyWindow(appData->window);
        SDL_Log("Failed to initialised renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(appData->renderer, 1);

    //SDL_SetRenderLogicalPresentation(appData->renderer, GAMEWIDTH, GAMEHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_ShowWindow(appData->window);

    SDL_RaiseWindow(appData->window);

    InitImgui(appData->renderer, appData->window);

	*appstate = appData;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
	AppData* appData {(AppData*)appstate};

    ImGui_ImplSDL3_ProcessEvent(event);
    switch(event->type) {
        case (SDL_EVENT_QUIT) : {
            return SDL_APP_SUCCESS;
        } break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate){
	AppData* appData {(AppData*)appstate};
    SDL_SetRenderDrawColor(appData->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(appData->renderer);
    // Get DeltaTime
    Uint64 currentTime {SDL_GetPerformanceCounter()};
    float deltaTime {(currentTime - appData->lastTime) / (float)SDL_GetPerformanceFrequency()};
    appData->lastTime = currentTime;

	// Update button timers here if any
	// Update

    DrawImgui(appData->renderer);

    SDL_RenderPresent(appData->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
	AppData* appData {(AppData*)appstate};
    SDL_DestroyRenderer(appData->renderer);
    SDL_DestroyWindow(appData->window);

    QuitImgui();
}