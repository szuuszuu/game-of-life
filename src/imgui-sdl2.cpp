#include "imgui-sdl2.h"

GameOfLife gol;

bool startGame = false;
bool quit = false;

int FPS = 30;
Uint32 startTime, endTime, frameTime;
const int desiredFrameTime = 1000 / FPS;

int radioButtonIndex = 0;
int lastSelectedIndex = -1;
bool isStartClicked = 0;
bool isPauseClicked = 0;
bool isImportClicked = 0;
bool clickMode = 1;

int iterations = 0;

std::string path = "";

int arrX = 0;
int arrY = 0;
std::vector<std::vector<int>> arr = gol.clear();
std::vector<std::vector<int>> arrImported = gol.clear();

void ImGuiSDL::setupImgui(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer) {
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

void ImGuiSDL::cleanupImguiSDL(SDL_Window* sdl_window, SDL_Renderer* sdl_renderer) {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

void ImGuiSDL::drawCells(std::vector<std::vector<int>> arr, SDL_Renderer* sdl_renderer) {
  for (int x = 0; x < gridCountX; x++) {
    for (int y = 0; y < gridCountY; y++) {
      if (arr[x][y] == 1) {
        SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(sdl_renderer, 22, 22, 22, 255);
      }

      SDL_Rect cell {
				.x = x * gridCellSize,
				.y = y * gridCellSize,
				.w = gridCellSize,
				.h = gridCellSize,
			};
      
      SDL_RenderFillRect(sdl_renderer, &cell);
    }
  }
}

void ImGuiSDL::drawGridLines(SDL_Renderer* sdl_renderer) {
  SDL_SetRenderDrawColor(sdl_renderer, 44, 44, 44, 255);

  int rightWall = 1 + gridCountX * gridCellSize;
  int bottomWall = 1 + gridCountY * gridCellSize;

  for (int x = 0; x < rightWall; x += gridCellSize) {
    SDL_RenderDrawLine(sdl_renderer, x, 0, x, SCREEN_WIDTH);
  }
  for (int y = 0; y < bottomWall; y += gridCellSize) {
    SDL_RenderDrawLine(sdl_renderer, 0, y, SCREEN_WIDTH, y);
  }
}

float ImGuiSDL::calculateOffset(ImGuiStyle& style, float width, std::string name) {
	width += style.ItemSpacing.x;
	width += ImGui::CalcTextSize(name.c_str()).x;
	width += 7.5f;
	return width;
}

void ImGuiSDL::alignForWidth(float width, float alignment = 0.5f) {
  ImGuiStyle& style = ImGui::GetStyle();
  float avail = ImGui::GetContentRegionAvail().x;
  float off = (avail - width) * alignment;
  if (off > 0.0f) { ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off); }
}

std::vector<std::vector<int>> ImGuiSDL::importData(std::vector<std::vector<int>> arr, std::string fileName) {
	std::string line;
	std::ifstream myFileStream(fileName);

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

void ImGuiSDL::update(SDL_Window* sdl_window) {
	ImGui::Render();
	SDL_Renderer* sdl_renderer = SDL_GetRenderer(sdl_window);
	
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(sdl_renderer);
	SDL_RenderClear(sdl_renderer);
}

void ImGuiSDL::onClickImport(bool isImportClicked, ImGuiStyle& style, ImGui::FileBrowser& fileDialog) {
  if (isImportClicked) {
    float width = 0.0f;
    width = calculateOffset(style, width, "CLEAR");
    width = calculateOffset(style, width, "Import .txt file");
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
}

void ImGuiSDL::onRadiobuttonChange(int& radioButtonIndex, int& lastSelectedIndex, int& iterations) {
  if (radioButtonIndex != lastSelectedIndex) {
    startGame = false; // idk czy to zostawić tutaj
    isStartClicked = false;
    isPauseClicked = false;
    isImportClicked = false;
    iterations = 0;
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
}

void ImGuiSDL::onClickReset(int& iterations) {
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

bool ImGuiSDL::gameLoop() {

	// Setup file dialog library
	ImGui::FileBrowser fileDialog;
	fileDialog.SetTitle("title");
	fileDialog.SetTypeFilters({ ".txt"});

	// Setup SDL and ImGui
	SDL_Window* window;
	SDL_Renderer* renderer;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

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

	// Setup for cursor
	SDL_Rect gridCursor = {
		.x = (gridCountX - 1) / 2 * gridCellSize,
		.y = (gridCountY - 1) / 2 * gridCellSize,
		.w = gridCellSize,
		.h = gridCellSize,
    };
	SDL_Color gridCursorColor = {44, 44, 44, 255};
	SDL_bool mouse_hover = SDL_FALSE;

	while (!quit) {
		startTime = SDL_GetTicks();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			switch (event.type) { 
				case SDL_QUIT:
          quit = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (clickMode && gridCursor.x < size_x && gridCursor.y < size_y) {
						arrX = gridCursor.x/gridCellSize;
						arrY = gridCursor.y/gridCellSize;
					} else {
						break;
					}
					arr = gol.changeState(arrX, arrY);
					break;
				case SDL_MOUSEMOTION:
					gridCursor.x = (event.motion.x / gridCellSize) * gridCellSize;
					gridCursor.y = (event.motion.y / gridCellSize) * gridCellSize;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover) {
            mouse_hover = SDL_TRUE;
          } else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_hover) {
            mouse_hover = SDL_FALSE;
          }
					break;
			}
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();	
		ImGui::NewFrame();
    ImGuiStyle& style = ImGui::GetStyle();

		ImGui::Begin("Game Of Life");
		ImGui::Text("Choose your population:");
		ImGui::RadioButton("Random population", &radioButtonIndex, 0);
		ImGui::RadioButton("Choose population via clicking cells", &radioButtonIndex, 1);
		ImGui::RadioButton("Import population via .txt file", &radioButtonIndex, 2);

		float width = 0.0f;
		if (!isStartClicked) { width = calculateOffset(style, width, "START");}
		width = calculateOffset(style, width, (isPauseClicked ? "RESUME" : "PAUSE"));
		width = calculateOffset(style, width, "RESET");
		alignForWidth(width);

		if (!isStartClicked) {
			if (ImGui::Button("START")) {                          
				isStartClicked = 1;
				startGame = true;
			} 
			ImGui::SameLine();
		}
		if (ImGui::Button(isPauseClicked ? "RESUME" : "PAUSE")) { 
			isPauseClicked = !isPauseClicked;
			startGame = isPauseClicked ? false : true;
		}  
		ImGui::SameLine();
		if (ImGui::Button("RESET")) {                         
			onClickReset(iterations);
		} 
		
    onClickImport(isImportClicked, style, fileDialog);
    onRadiobuttonChange(radioButtonIndex, lastSelectedIndex, iterations);
    
    if (startGame) {
			arr = gol.update();
			gol.swapArrays();
			iterations++;
		}

		// ImGui::Text("Cursor x: %d", gridCursor.x);
		// ImGui::Text("Cursor y: %d", gridCursor.y);
		ImGui::Text("Iterations: %d", iterations);
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
			SDL_SetRenderDrawColor(renderer, gridCursorColor.r,
			gridCursorColor.g,
			gridCursorColor.b,
			gridCursorColor.a);

			SDL_RenderFillRect(renderer, &gridCursor);
		}

		endTime = SDL_GetTicks();
		frameTime = endTime - startTime;
		if (frameTime < desiredFrameTime) {
				SDL_Delay(desiredFrameTime - frameTime);
		}

		update(window);
	}

	cleanupImguiSDL(window, renderer);
  return 1;
}