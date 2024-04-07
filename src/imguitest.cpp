#include "GOL.h"

// imgui headers
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

// general headers
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

using namespace std;
GameOfLife gol;

bool startGame = false;

const static int SCREEN_WIDTH = 1280;
const static int SCREEN_HEIGHT = 800;

const int FPS = 20;
// Uint32* buffer;

void setup_imgui_context(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer)
{
	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(sdl_window, sdl_renderer);
	ImGui_ImplSDLRenderer2_Init(sdl_renderer);

}


void drawCells(std::vector<std::vector<int>> arr, SDL_Renderer* renderer) {

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

void drawGridLines(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 44, 44, 44, 255);

  int rightWall = 1 + gridCountX * gridCellSize;
  int bottomWall = 1 + gridCountY * gridCellSize;

  for (int x = 0; x < rightWall; x += gridCellSize) {
    SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_WIDTH);
  }
  for (int y = 0; y < bottomWall; y += gridCellSize) {
    SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
  }
}

void alignForWidth(float width, float alignment = 0.5f) {
  ImGuiStyle& style = ImGui::GetStyle();
  float avail = ImGui::GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off); }

}

void menu(int radioButtonIndex, int lastSelectedIndex, SDL_Renderer* renderer) {

	ImGui::Begin("Game Of Life");

	ImGui::Text("Choose your population:");

	ImGui::RadioButton("Random population", &radioButtonIndex, 0);
	ImGui::RadioButton("Choose population via clicking cells", &radioButtonIndex, 1);
	ImGui::RadioButton("Import population via .txt file", &radioButtonIndex, 2);


	ImGuiStyle& style = ImGui::GetStyle();
	float width = 0.0f;
	width += ImGui::CalcTextSize("START").x;
	width += style.ItemSpacing.x;
	width += 7.5f;
	width += style.ItemSpacing.x;
	width += ImGui::CalcTextSize("CLEAR").x;
	alignForWidth(width);


	if (ImGui::Button("START")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
		startGame = true;
	}  
	ImGui::SameLine();
	if (ImGui::Button("CLEAR")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
		startGame = false;
	} 


	if (radioButtonIndex != lastSelectedIndex) {
		if (radioButtonIndex == 0) {
			drawCells(gol.init(), renderer);
		} else if (radioButtonIndex == 1) {
		
		} else if (radioButtonIndex == 2) {

		}
		ImGui::Text("RadioButtonID: %d", radioButtonIndex);
		ImGui::Text("LastRadioButtonID: %d", lastSelectedIndex);
		lastSelectedIndex = radioButtonIndex;

	}

	if (startGame) {
		drawCells(gol.update(), renderer);
	}

	ImGui::End();
}

void update(SDL_Window* window) {

	ImGui::Render();
	SDL_Renderer* renderer = SDL_GetRenderer(window);
	
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

int main(int argc, char* argv[]) {

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}
	cout << "SDL Init succeeded." << endl;

	// Create window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
	window = SDL_CreateWindow(
		"My Window",
		SDL_WINDOWPOS_CENTERED_DISPLAY(1),
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		window_flags);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);


	setup_imgui_context(window, renderer);

	int radioButtonIndex = 0;
	int lastSelectedIndex = -1;

	std::vector<std::vector<int>> arr;

	Uint32 startTime, endTime, frameTime;
    const int desiredFrameTime = 1000 / FPS;

	while (true)
	{
		startTime = SDL_GetTicks();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
			{
				cout << "Quit window event called. Closing the window." << endl;
				return false;
			}

		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();	
		ImGui::NewFrame();


		ImGui::Begin("Game Of Life");

		ImGui::Text("Choose your population:");

		ImGui::RadioButton("Random population", &radioButtonIndex, 0);
		ImGui::RadioButton("Choose population via clicking cells", &radioButtonIndex, 1);
		ImGui::RadioButton("Import population via .txt file", &radioButtonIndex, 2);


		ImGuiStyle& style = ImGui::GetStyle();
		float width = 0.0f;
		width += ImGui::CalcTextSize("START").x;
		width += style.ItemSpacing.x;
		width += 7.5f;
		width += style.ItemSpacing.x;
		width += ImGui::CalcTextSize("PAUSE").x;
		width += 7.5f;
		width += style.ItemSpacing.x;
		width += ImGui::CalcTextSize("CLEAR").x;
		alignForWidth(width);


		if (ImGui::Button("START")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
			startGame = true;
		} 
		ImGui::SameLine();
		if (ImGui::Button("PAUSE")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
			startGame = false;
		}  
		ImGui::SameLine();
		if (ImGui::Button("CLEAR")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
			arr = gol.clear();
			startGame = false;	
		} 

		ImGui::Text("RadioButtonID: %d", radioButtonIndex);
		ImGui::Text("LastRadioButtonID: %d", lastSelectedIndex);
		if (radioButtonIndex != lastSelectedIndex) {
			startGame = false; // idk czy to zostawić tutaj
			if (radioButtonIndex == 0) {
				arr = gol.init();
			} else if (radioButtonIndex == 1) {
			
			} else if (radioButtonIndex == 2) {

			}

			lastSelectedIndex = radioButtonIndex;

		}

		if (startGame) {
			arr = gol.update();
			gol.swapArrays();
		}

		ImGui::Text("GameStatus: %d", startGame);
		ImGui::End();


		// menu(radioButtonIndex, lastSelectedIndex, renderer);
		
		drawCells(arr, renderer);
		drawGridLines(renderer);

		endTime = SDL_GetTicks();
        frameTime = endTime - startTime;
        if (frameTime < desiredFrameTime) {
            SDL_Delay(desiredFrameTime - frameTime);
        }

		update(window);
	}

	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_Quit();

}