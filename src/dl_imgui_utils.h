#ifndef DL_SDL3_IMGUI_UTILS_H
#define DL_SDL3_IMGUI_UTILS_H
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

inline void InitImgui(SDL_Renderer* renderer, SDL_Window* window) {
    // Basic setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Set appearance and scaling of ImGui
    //ImGuiStyle& style = ImGui::GetStyle();
    //style.ScaleAllSizes(GameSettings::INITIALSCALE);
    // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    //style.FontScaleDpi = GameSettings::INITIALSCALE;
    ImGui::StyleColorsDark();

    // Enable keyboard navigation in imgui
    ImGuiIO& io {ImGui::GetIO()};
    io.FontGlobalScale = 2;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Initialise ImGui globals for SDL
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

inline void DrawImgui(SDL_Renderer* renderer) {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io {ImGui::GetIO()};
    ImGui::Begin("Tab");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

inline void QuitImgui(){
    // Shutdown ImGui
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}


#endif
