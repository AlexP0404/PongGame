#include "gameLoop.hpp"

GameLoop::GameLoop(){
  numActiveTextures = 0;
  gameWindow = NULL;
  gameRenderer = NULL;
  mainFont = NULL;
  escFont = NULL;
  textColor = {0xFF, 0xFF, 0xFF, 0xFF}; 
}

GameLoop::~GameLoop(){
  startPromptTexture.free();
  escPromptTexture.free();

  TTF_CloseFont(mainFont);
  TTF_CloseFont(escFont);
  mainFont = NULL;
  escFont = NULL;

  SDL_DestroyRenderer(gameRenderer);
  SDL_DestroyWindow(gameWindow);
  gameWindow = NULL;
  gameRenderer = NULL;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool GameLoop::init(){
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else{
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
      printf("Warning: Linear texture filtering not enabled!");
    }
    gameWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(gameWindow == NULL){
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else{
      gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if(gameRenderer == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else{
        SDL_SetRenderDrawColor(gameRenderer, 0x00,0x00,0x00,0xFF);
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
          printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
          success = false;
        }

        if(TTF_Init() == -1){
          printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
          success = false;
        }
      }
    }
  }
  return success;
}

bool GameLoop::loadMedia(){
  bool success = true;

  escFont = TTF_OpenFont("lazy.ttf", 15);
  if( escFont == NULL){
    printf("failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  }
  else{
    escPromptTexture.setRenderer(*gameRenderer);
    if(!escPromptTexture.loadFromRenderedText("Press Escape to quit", textColor,escFont)){
      printf("Failed to render text texture!\n");
      success = false;
    }
    escPromptTexture.setxCoor(0);
    escPromptTexture.setyCoor(0);
    textures[numActiveTextures] = escPromptTexture;
    numActiveTextures++;
    mainFont = TTF_OpenFont("lazy.ttf", 30);
    mainText.str("");
    mainText << "Press Enter to start";
    startPromptTexture.setRenderer(*gameRenderer);
    setStartText();
  }
  return success;
}

void GameLoop::setStartText(){//sets the large MAIN  text to the current mainText string and adds the texture to the list of active textures
  startPromptTexture.loadFromRenderedText(mainText.str().c_str(), textColor, mainFont);
  startPromptTexture.setxCoor(( SCREEN_WIDTH - startPromptTexture.getWidth() ) / 2);
  startPromptTexture.setyCoor(( SCREEN_HEIGHT - startPromptTexture.getHeight() ) / 2);
  textures[numActiveTextures] = startPromptTexture;
  numActiveTextures++;
}

void GameLoop::renderTextures(){
  SDL_SetRenderDrawColor(gameRenderer, 0,0,0,0xFF);
  SDL_RenderClear(gameRenderer);
  
  for(int i = 0; i < numActiveTextures; i++){
    textures[i].render();
  }
  SDL_RenderPresent(gameRenderer);
  
}

void GameLoop::loop(){
  
  bool quit = false;
  bool start = false;
  bool gameOver = false;
  bool lastPressedEsc = false; //you have to press esc then enter once the game started to quit
  
  SDL_Event e;
  
  while(!quit){
    while(SDL_PollEvent(&e)!=0){
      if( e.type == SDL_QUIT){
        quit = true;
      }
      else if(e.type == SDL_KEYDOWN){
        if(!start){//if still at start screen
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE://escape key to quit
              quit = true;
              break;
            case SDLK_RETURN://enter key to start
              start = true;
              mainText.clear();
              mainText.str("");
              numActiveTextures--;
              break;
            default:
              break;
          }
        }
        else{//game started (now this is used for player controls)
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              if(lastPressedEsc)
                break;
              lastPressedEsc = true;
              mainText << "Are you sure you want to quit? Press Enter if yes.";
              setStartText();
              mainText.clear();
              mainText.str("");
              break;
              //all the other cases now update lastPressedEsc to true because it wasnt pressed 2x in a row
            case SDLK_RETURN:
              if(lastPressedEsc)
                quit = true;
              break;
            default:
              numActiveTextures--;
              lastPressedEsc = false;
              break;
          }
        }
      }
    }
    if(numActiveTextures <= 0){    
      textures[0] = escPromptTexture;//if somehow accidentally deleted too many from the array, keep at least the escape prompt
      numActiveTextures = 1; 
    }
    renderTextures();

  }
}

