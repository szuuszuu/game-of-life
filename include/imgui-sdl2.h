#ifndef IMGUI_SDL_H
#define IMGUI_SDL_H

// my headers
#include "GOL.h"

// imgui headers
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imfilebrowser.h"

// SDL2 header
#include <SDL2/SDL.h>

// general headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

const static int SCREEN_WIDTH = 1280;
const static int SCREEN_HEIGHT = 720;

class ImGuiSDL{
  private:
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    std::vector<std::vector<int>> arr;

  public:
    void setupImgui(
      SDL_Window* sdl_window, 
      SDL_Renderer* sdl_renderer);
    void cleanupImguiSDL(
      SDL_Window* sdl_window, 
      SDL_Renderer* sdl_renderer);
    void drawCells(
      std::vector<std::vector<int>> arr, 
      SDL_Renderer* sdl_renderer);
    void drawGridLines(SDL_Renderer* sdl_renderer);
    float calculateOffset(
      ImGuiStyle& style, 
      float width, 
      std::string name);
    void alignForWidth(
      float width, 
      float alignment);  
    std::vector<std::vector<int>> importData(
      std::vector<std::vector<int>> arr, 
      std::string fileName);
    void update(SDL_Window* sdl_window);
    bool gameLoop();
    void onClickImport(bool isImportClicked, ImGuiStyle& style, ImGui::FileBrowser& fileDialog);
    void onRadiobuttonChange(int& radioButtonIndex, int& lastSelectedIndex, int& iterations);
    void onClickReset(int& iterations);
};

#endif
