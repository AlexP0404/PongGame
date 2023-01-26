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
const int MAX_TEXTURES = 100;

class GameLoop{
  public:
    GameLoop();
    ~GameLoop();
    
    void renderTextures();
    bool init();
    bool loadMedia();
    void setStartText();
    void setScoreboardText();
    void loop();

  private:
    std::stringstream mainText;
    SDL_Window* gameWindow;
    SDL_Renderer* gameRenderer;
    TTF_Font* mainFont;
    TTF_Font* escFont;
    Texture textures[MAX_TEXTURES];
    Texture startPromptTexture;
    Texture escPromptTexture;
    Texture scoreboardTexture;
    int numActiveTextures;
    SDL_Color textColor;
    Scoreboard sb;
};

#endif // !GAMELOOP_HPP
