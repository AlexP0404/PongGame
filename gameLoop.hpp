#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP
//deals with window/render initializing
//deals with allocating/deallocating textures
//runs the game loop and calls the other classes

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include "Texture.hpp"
#include "dot.hpp"
#include "paddle.hpp"
#include "scoreboard.hpp"
#include <iostream>
#include <string>
#include <sstream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

class GameLoop{
  public:
    GameLoop();
    ~GameLoop();
  
    bool init();
    bool loadMedia();
    void loop();

  private:
    SDL_Window* gameWindow;
    SDL_Renderer* gameRenderer;
    TTF_Font* mainFont;
    TTF_Font* escFont;
    Texture startPromptTexture;
    Texture escPromptTexture;
    SDL_Color textColor;
};

#endif // !GAMELOOP_HPP
