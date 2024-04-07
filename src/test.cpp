#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <iostream>

void FunctionForOption1() {
    std::cout << "Function for Option 1 called." << std::endl;
}

void FunctionForOption2() {
    std::cout << "Function for Option 2 called." << std::endl;
}

void FunctionForOption3() {
    std::cout << "Function for Option 3 called." << std::endl;
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("ImGui SDL2 Radio Buttons Example",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Error creating SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
    ImGui::StyleColorsDark();

    // Variables to hold radio button state
    int radioButtonIndex = 0;
    int lastSelectedIndex = -1; // Track last selected index

    // Main loop
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                done = true;
            }
        }

        // Start ImGui frame
   		ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Create radio buttons
        ImGui::RadioButton("Option 1", &radioButtonIndex, 0);
        ImGui::RadioButton("Option 2", &radioButtonIndex, 1);
        ImGui::RadioButton("Option 3", &radioButtonIndex, 2);

        // Check if selection changed and call corresponding function
        if (radioButtonIndex != lastSelectedIndex) {
            if (radioButtonIndex == 0) {
                FunctionForOption1();
            } else if (radioButtonIndex == 1) {
                FunctionForOption2();
            } else if (radioButtonIndex == 2) {
                FunctionForOption3();
            }
            lastSelectedIndex = radioButtonIndex; // Update last selected index
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui::Render();
	    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
        
    }

    // Cleanup
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}