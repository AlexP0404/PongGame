#include "engine.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>

Engine::Engine() {
  gameWindow = nullptr;
  gameRenderer = nullptr;
  bounce = nullptr;
  textColor = {0xFF, 0xFF, 0xFF, 0xFF};
  m_ScreenWidth = 1280;
  m_ScreenHeight = 960;
}

Engine::Engine(const int screenWidth, const int screenHeight)
    : m_ScreenHeight(screenHeight), m_ScreenWidth(screenWidth) {
  gameWindow = nullptr;
  gameRenderer = nullptr;
  bounce = nullptr;
  textColor = {0xFF, 0xFF, 0xFF, 0xFF};
}

Engine::~Engine() {
  Mix_FreeChunk(bounce);
  bounce = nullptr;

  SDL_DestroyRenderer(gameRenderer);
  SDL_DestroyWindow(gameWindow);
  gameWindow = nullptr;
  gameRenderer = nullptr;

  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool Engine::init() {
  bool success = true;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }
    gameWindow = SDL_CreateWindow(
        "Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_ScreenWidth,
        m_ScreenHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS);
    if (gameWindow == nullptr) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {

      gameRenderer = SDL_CreateRenderer(
          gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gameRenderer == nullptr) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        success = false;
      } else {
        SDL_SetRenderDrawColor(gameRenderer, 0x00, 0x00, 0x00, 0xFF);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_image Error: %s\n",
                 IMG_GetError());
          success = false;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
          printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                 Mix_GetError());
          success = false;
        }
        if (TTF_Init() == -1) {
          printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                 TTF_GetError());
          success = false;
        }
      }
    }
  }
  return success;
}

bool Engine::loadMedia() {
  gameIcon = IMG_Load("dot.bmp");
  SDL_SetWindowIcon(gameWindow, gameIcon);

  fonts["escFont"] =
      unique_ptr<TTF_Font, FontDeleter>(TTF_OpenFont("lazy.ttf", 15));
  if (fonts.at("escFont") == nullptr) {
    printf("failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    return false;
  }
  textures["escapePrompt"] = unique_ptr<Texture>(new Texture(
      *gameRenderer, 0, 0, true)); // uses full parameter constructor
  if (!textures.at("escapePrompt")
           ->loadFromRenderedText("Press Escape To Quit", textColor,
                                  fonts.at("escFont").get())) {
    printf("Failed to render text texture!\n");
    return false;
  }

  fonts["mainFont"] =
      unique_ptr<TTF_Font, FontDeleter>(TTF_OpenFont("lazy.ttf", 30));

  bounce = Mix_LoadWAV("bounce.wav");
  if (bounce == nullptr) {
    printf("Failed to load bounce sound effect! SDL_mixer Error: %s\n",
           Mix_GetError());
    return false;
  }

  return true;
}

bool Engine::setTextureCoor(const string &&textureName, int x, int y) {
  if (textures.find(textureName) == textures.end()) {
    std::cout << "Failed to set texture coor: " << textureName << "\n";
    return false;
  }
  textures.at(textureName)->setCoors(x, y);
  return true;
}

bool Engine::setTextTexture(const string &&textureName, const string &&fontName,
                            const string &&text) {
  if (textures.find(textureName) == textures.end()) {
    textures[textureName] =
        unique_ptr<Texture>(new Texture(*gameRenderer, 0, 0, true));
  }

  if (fonts.find(fontName) == fonts.end()) {
    std::cout << "Invalid font given! Using default font!\n";
    return textures.at(textureName)
        ->loadFromRenderedText(text.c_str(), textColor,
                               fonts.at(fontName).get());
  }

  return textures.at(textureName)
      ->loadFromRenderedText(text.c_str(), textColor, fonts.at(fontName).get());
}

void Engine::renderTextures() {
  SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 0xFF);
  SDL_RenderClear(gameRenderer);

  for (auto &[name, txtr] : textures) {
    txtr->render();
  }
}

void Engine::renderScreen() { SDL_RenderPresent(gameRenderer); }

void Engine::eraseTextures(const vector<string> &&texturesToErase) {
  for (auto &textureName : texturesToErase) {
    textures.erase(textureName);
  }
}
