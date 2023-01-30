#include "gameLoop.hpp"
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>

GameLoop::GameLoop(){
  numActiveTextures = 0;
  gameWindow = NULL;
  gameRenderer = NULL;
  mainFont = NULL;
  escFont = NULL;
  textColor = {0xFF, 0xFF, 0xFF, 0xFF};
}

GameLoop::~GameLoop(){
  for(auto i : textures){
    i.free();
  }
  startPromptTexture.free();
  escPromptTexture.free();
  scoreboardTexture.free();

  TTF_CloseFont(mainFont);
  TTF_CloseFont(escFont);
  mainFont = NULL;
  escFont = NULL;

  Mix_FreeChunk(bounce);
  bounce = NULL;

  SDL_DestroyRenderer(gameRenderer);
  SDL_DestroyWindow(gameWindow);
  gameWindow = NULL;
  gameRenderer = NULL;
  
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool GameLoop::init(){
  bool success = true;
  
  p1.setScreenHeight(SCREEN_HEIGHT);
  p2.setScreenHeight(SCREEN_HEIGHT);
  p1.setPos(SCREEN_HEIGHT / 2);
  p2.setPos(SCREEN_HEIGHT / 2);

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
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
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
          printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
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
    else {
      escPromptTexture.setxCoor(0);
      escPromptTexture.setyCoor(0);
      textures[numActiveTextures] = escPromptTexture;
      numActiveTextures++;

      mainFont = TTF_OpenFont("lazy.ttf", 30);
      mainText.str("");
      mainText << "Press Enter to start";
      startPromptTexture.setRenderer(*gameRenderer);
      setStartText();

      scoreboardTexture.setRenderer(*gameRenderer);
      
      bounce = Mix_LoadWAV("bounce.wav");
      if(bounce == NULL){
        printf("Failed to load bounce sound effect! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
      }
    }
  }
  return success;
}

void GameLoop::setScoreboardText(){
  scoreboardTexture.loadFromRenderedText(sb.getScoreString().c_str(), textColor, mainFont);
  scoreboardTexture.setxCoor((SCREEN_WIDTH - scoreboardTexture.getWidth()) / 2);//center
  scoreboardTexture.setyCoor(150);// more towards the top of the screen
}

void GameLoop::setStartText(){//sets the large MAIN  text to the current mainText string and adds the texture to the list of active textures
  startPromptTexture.loadFromRenderedText(mainText.str().c_str(), textColor, mainFont);
  startPromptTexture.setxCoor(( SCREEN_WIDTH - startPromptTexture.getWidth() ) / 2);
  startPromptTexture.setyCoor(( SCREEN_HEIGHT - startPromptTexture.getHeight() ) / 2);
  textures[numActiveTextures] = startPromptTexture;
  numActiveTextures++;
}

void GameLoop::drawPaddles(){
  p1Rect = { 100, p1.getPos(), PADDLE_WIDTH, PADDLE_HEIGHT};//initialize the first paddle in the middle left of the screen
  p2Rect = { SCREEN_WIDTH - 100, p2.getPos() , PADDLE_WIDTH, PADDLE_HEIGHT};
  SDL_RenderFillRect(gameRenderer, &p1Rect);
  SDL_RenderFillRect(gameRenderer, &p2Rect);
}

void GameLoop::drawNet(){
  SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for(int i = 0; i < SCREEN_HEIGHT; i +=4){
    SDL_RenderDrawPoint(gameRenderer, SCREEN_WIDTH / 2, i);
  }
}

void GameLoop::renderTextures(){
  SDL_SetRenderDrawColor(gameRenderer, 0,0,0,0xFF);
  SDL_RenderClear(gameRenderer);
  
  for(int i = 0; i < numActiveTextures; i++){
    textures[i].render();
    //std::cout << i << " ";//prints the indexes of the textures as they are being rendered
  }
  
}

void GameLoop::loop(){
  
  bool quit = false;
  bool start = false;
  bool gameOver = false;
  bool lastPressedEsc = false; //you have to press esc then enter once the game started to quit
  bool sbTextureLoaded = false;
  setScoreboardText();

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
        else{//game started (now this is used for player controls) start = true
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
            case SDLK_w://start of the player controls
              p1.handleEvent(true);
              break;
            case SDLK_s:
              p1.handleEvent(false);
              break;
            case SDLK_UP:
              p2.handleEvent(true);
              break;
            case SDLK_DOWN:
              p2.handleEvent(false);
              break;
            default:
              //Mix_PlayChannel(-1, bounce, 0); //this will be called along with the dot.bounce() function
              /*sb.incPlayer1();
              setScoreboardText(); //this is somewhat how the scoreboard is gonna be used and updated 
              sbTextureLoaded = false;*/
              if(lastPressedEsc){
                numActiveTextures--;
                lastPressedEsc = false;
              }
              break;
          }
        }
      }
    }
    

    if(start && !sbTextureLoaded){
      sbTextureLoaded = true;
      textures[numActiveTextures] = scoreboardTexture;
      numActiveTextures++;
      //std::cout << "After the start texture the next index is: " << numActiveTextures << std::endl;
    }

    if(numActiveTextures <= 0){    
      textures[0] = escPromptTexture;//if somehow accidentally deleted too many from the array, keep at least the escape prompt
      numActiveTextures = 1; 
    }
    renderTextures();
    if(start && !lastPressedEsc){
      drawNet();
      drawPaddles();
    }
    SDL_RenderPresent(gameRenderer);

  }
}

