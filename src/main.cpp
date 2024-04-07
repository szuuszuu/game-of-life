#include "GOL.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>


class MainComponent {
  public:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    int windowSizeX;
    int windowSizeY;

    void init();
    void menu();
    void alignForWidth(float width, float alignment);
    // void clearRenderer();
    // void updateRenderer();
    void drawCells(std::vector<std::vector<int>> arr);
    void drawGridLines();
    ~MainComponent();
};

void MainComponent::init() {
  // Set up SDL2
  SDL_Init(SDL_INIT_EVERYTHING);       
  
  int windowSizeX = (gridCountX * gridCellSize) + 1; 
  int windowSizeY = (gridCountY * gridCellSize) + 1; 

  SDL_CreateWindowAndRenderer(windowSizeX, windowSizeY, 0, &window, &renderer);
  SDL_RenderSetScale(renderer, 1, 1);

  SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);
  SDL_RenderClear(renderer);

  // Set up ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void MainComponent::alignForWidth(float width, float alignment = 0.5f) {
  ImGuiStyle& style = ImGui::GetStyle();
  float avail = ImGui::GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off); }

}

void MainComponent::menu() {

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame(); 


    static int e = 0;

    ImGui::Begin("Menu");                                   // Create a window called "Menu" and append into it.

    ImGui::Text("Choose your population:");

    ImGui::RadioButton("Random population", &e, 0);
    ImGui::RadioButton("Choose population via clicking cells", &e, 1);
    ImGui::RadioButton("Import population via .txt file", &e, 2);


    ImGuiStyle& style = ImGui::GetStyle();
    float width = 0.0f;
    width += ImGui::CalcTextSize("START").x;
    width += style.ItemSpacing.x;
    width += 7.5f;
    width += style.ItemSpacing.x;
    width += ImGui::CalcTextSize("CLEAR").x;
    alignForWidth(width);

    if (ImGui::Button("START")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
      // doSomething();
    }  
    ImGui::SameLine();
    if (ImGui::Button("CLEAR")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
      // doSomething();
    }                           
      
    ImGui::End();
    
}

void MainComponent::drawCells(std::vector<std::vector<int>> arr) {

  for (int x = 0; x < gridCountX; x++) {
    for (int y = 0; y < gridCountY; y++) {
      if (arr[x][y] == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);
      }

      SDL_Rect cell;
      cell.x = x * gridCellSize;
      cell.y = y * gridCellSize;
      cell.w = gridCellSize;
      cell.h = gridCellSize;

      SDL_RenderFillRect(renderer, &cell);
    }
  }
}

void MainComponent::drawGridLines() {
  SDL_SetRenderDrawColor(renderer, 44, 44, 44, 255);

  int rightWall = 1 + gridCountX * gridCellSize;
  int bottomWall = 1 + gridCountY * gridCellSize;

  for (int x = 0; x < rightWall; x += gridCellSize) {
    SDL_RenderDrawLine(renderer, x, 0, x, windowSizeY);
  }
  for (int y = 0; y < bottomWall; y += gridCellSize) {
    SDL_RenderDrawLine(renderer, 0, y, windowSizeX, y);
  }
}

MainComponent::~MainComponent() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}


int main(int argc, char *argv[]) {
    MainComponent mainComponent;
    GameOfLife gol;

    std::vector<std::vector<int>> out(gridCountX, std::vector<int> (gridCountY, 0));


    mainComponent.init();
    gol.init();

    // Infinite loop for application
    bool gameIsRunning = true;
    while (gameIsRunning) {

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) { gameIsRunning = false; }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mainComponent.window)) { gameIsRunning = false; }
      }   

      mainComponent.menu();

      SDL_Delay(10);

      out = gol.update();
      
      
      SDL_RenderClear(mainComponent.renderer);
      ImGui::Render();

      mainComponent.drawCells(out);
      mainComponent.drawGridLines();

      ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

      
      SDL_RenderPresent(mainComponent.renderer);

      gol.swapArrays();
    }

    SDL_Quit();
    return 0;
}