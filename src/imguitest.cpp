#include "GOL.h"

// imgui headers
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imfilebrowser.h"

// general headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <SDL2/SDL.h>

GameOfLife gol;

bool startGame = false;

const static int SCREEN_WIDTH = 1280;
const static int SCREEN_HEIGHT = 800;

int FPS = 30;

void setupImgui(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer)
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

void cleanupImguiSDL(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer) {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

void drawCells(std::vector<std::vector<int>> arr, SDL_Renderer* renderer) {

  for (int x = 0; x < gridCountX; x++) {
    for (int y = 0; y < gridCountY; y++) {
      if (arr[x][y] == 1) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);
      }

      SDL_Rect cell {
		.x = x * gridCellSize,
		.y = y * gridCellSize,
		.w = gridCellSize,
		.h = gridCellSize,
	  };
      

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

std::vector<std::vector<int>> importData(std::vector<std::vector<int>> arr, std::string fileName) {
	std::string line;
    std::string path = fileName;
	std::ifstream myFileStream(path);

	if (!myFileStream) {
        std::cerr<<"Error: Missing file named "<<fileName<<std::endl;
		exit(EXIT_SUCCESS);
	}

	while (getline(myFileStream, line)) {
        std::istringstream iss(line);
        int x, y;
        if (!(iss >> x >> y)) {
            std::cerr<<"Error: Invalid line format: "<<line<<std::endl;
            continue;
        }

		if (x < gridCountX && y < gridCountY) {
			arr[x][y] = 1;
		} else {
            std::cerr<<"Error: Point out of reach: "<<line<<std::endl;
        }
    }

	myFileStream.close();
	return arr;
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
		// ImGui::Text("RadioButtonID: %d", radioButtonIndex);
		// ImGui::Text("LastRadioButtonID: %d", lastSelectedIndex);
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

	ImGui::FileBrowser fileDialog;
	fileDialog.SetTitle("title");
    fileDialog.SetTypeFilters({ ".txt"});

	SDL_Rect gridCursor = {
		.x = (gridCountX - 1) / 2 * gridCellSize,
		.y = (gridCountY - 1) / 2 * gridCellSize,
		.w = gridCellSize,
		.h = gridCellSize,
    };

	SDL_Rect gridCursorGhost = {gridCursor.x, gridCursor.y, gridCellSize, gridCellSize};

	SDL_Color gridCursorGhostColor = {44, 44, 44, 255};
    SDL_Color gridCursorColor = {255, 255, 255, 255}; // White
	
	SDL_bool mouse_active = SDL_FALSE;
    SDL_bool mouse_hover = SDL_FALSE;



	SDL_Window* window;
	SDL_Renderer* renderer;

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}
	std::cout << "SDL Init succeeded." << std::endl;

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


	setupImgui(window, renderer);

	int radioButtonIndex = 0;
	int lastSelectedIndex = -1;

	int iterations = 0;

	bool isStartClicked = 0;
	bool isPauseClicked = 0;
	bool isImportClicked = 0;

	bool clickMode = 1;

	std::string path = "";

	int arrX = 0;
	int arrY = 0;

	std::vector<std::vector<int>> arr = gol.clear();
	std::vector<std::vector<int>> arrImported = gol.clear();

	Uint32 startTime, endTime, frameTime;
    const int desiredFrameTime = 1000 / FPS;

	while (true)
	{
		startTime = SDL_GetTicks();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type) { 
				case SDL_QUIT:
					return false;
				case SDL_MOUSEBUTTONDOWN:
					if (clickMode && gridCursorGhost.x < size_x && gridCursorGhost.y < size_y) {
						arrX = gridCursorGhost.x/gridCellSize;
						arrY = gridCursorGhost.y/gridCellSize;
					} else {
						break;
					}
					arr = gol.changeState(arrX, arrY);
					break;
				case SDL_MOUSEMOTION:
					gridCursorGhost.x = (event.motion.x / gridCellSize) * gridCellSize;
					gridCursorGhost.y = (event.motion.y / gridCellSize) * gridCellSize;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
						mouse_hover = SDL_TRUE;
					else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_hover)
						mouse_hover = SDL_FALSE;
					break;
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
		if (!isStartClicked) {
			width += style.ItemSpacing.x;
			width += ImGui::CalcTextSize("START").x;
			width += 7.5f;
		}
		
		width += style.ItemSpacing.x;
		width += ImGui::CalcTextSize(isPauseClicked ? "RESUME" : "PAUSE").x;
		width += 7.5f;
		width += style.ItemSpacing.x;
		width += ImGui::CalcTextSize("RESET").x;
		alignForWidth(width);

		if (!isStartClicked) {
			if (ImGui::Button("START")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
				isStartClicked = 1;
				startGame = true;
			} 
			ImGui::SameLine();
		}
		
		if (ImGui::Button(isPauseClicked ? "RESUME" : "PAUSE")) {    // Buttons return true when clicked (most widgets return true when edited/activated)
			isPauseClicked = !isPauseClicked;
			startGame = isPauseClicked ? false : true;
		}  
		ImGui::SameLine();
		if (ImGui::Button("RESET")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
			startGame = false;
			isStartClicked = false;
			isPauseClicked = false;
			iterations = 0;
			if (radioButtonIndex == 0) {
				arr = gol.init();
			} else if (radioButtonIndex == 2){
				arr = gol.clear();
				arr = arrImported;
			} else {
				arr = gol.clear();
			}
		} 
		
		
		if (isImportClicked) {
			width = 0.0f;
			width += style.ItemSpacing.x;
			width += ImGui::CalcTextSize("CLEAR").x;
			width += 7.5f;
			width += style.ItemSpacing.x;
			width += ImGui::CalcTextSize("Import .txt file").x;
			alignForWidth(width);

			if (ImGui::Button("CLEAR")) {
				arrImported = gol.clear();
				arr = arrImported;
			}
			ImGui::SameLine();
			if (ImGui::Button("Import .txt file")) {
				clickMode = false;
				fileDialog.Open();
			}
		}
		

		ImGui::Text("Array x: %d", arrX);
		ImGui::Text("Array y: %d", arrY);


		ImGui::Text("Cursor x: %d", gridCursorGhost.x);
		ImGui::Text("Cursor y: %d", gridCursorGhost.y);
		if (radioButtonIndex != lastSelectedIndex) {
			startGame = false; // idk czy to zostawić tutaj
			isStartClicked = false;
			isPauseClicked = false;
			isImportClicked = false;
			arr = gol.clear();
			if (radioButtonIndex == 0) {
				arr = gol.init();
			} else if (radioButtonIndex == 1) {
				arr = gol.clear();
			} else if (radioButtonIndex == 2) {
				isImportClicked = true;
				arr = arrImported;
			}
			lastSelectedIndex = radioButtonIndex;
		}

		if (startGame) {
			arr = gol.update();
			gol.swapArrays();
			iterations++;
		}

		ImGui::Text("ClickMode: %d", clickMode);
		ImGui::Text("Iterations: %d", iterations);
		ImGui::Text("GameStatus: %d", startGame);
		ImGui::End();


		fileDialog.Display();
        
        if (fileDialog.HasSelected()) {
			path = fileDialog.GetSelected().string();
			arrImported = gol.clear();
			arrImported = importData(arrImported, path);
			arr = arrImported;
            fileDialog.ClearSelected();
        }

		if (!fileDialog.IsOpened()) {
			clickMode = true;
		}

		drawCells(arr, renderer);
		drawGridLines(renderer);

		if (mouse_hover) {
            SDL_SetRenderDrawColor(renderer, gridCursorGhostColor.r,
                                   gridCursorGhostColor.g,
                                   gridCursorGhostColor.b,
                                   gridCursorGhostColor.a);
            SDL_RenderFillRect(renderer, &gridCursorGhost);
        }

		endTime = SDL_GetTicks();
        frameTime = endTime - startTime;
        if (frameTime < desiredFrameTime) {
            SDL_Delay(desiredFrameTime - frameTime);
        }

		update(window);
	}

	cleanupImguiSDL(window, renderer);
}