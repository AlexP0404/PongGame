#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP
//deals with window/render initializing
//deals with allocating/deallocating textures
//runs the game loop and calls the other classes

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include "Texture.hpp"
#include "dot.hpp"
#include "paddle.hpp"
#include "scoreboard.hpp"
#include "timer.hpp"
#include <SDL2/SDL_surface.h>
#include <iostream>
#include <string>
#include <sstream>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;
const int MAX_TEXTURES = 100;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;
const int PADDLE_OFFSET = 100;

class GameLoop{
  public:
    GameLoop();
    ~GameLoop();
    
    void renderTextures();
    bool init();
    bool loadMedia();
    bool collision();
    bool score();
    void setStartText();
    void drawNet();
    void setScoreboardText();
    void drawDot();
    void drawPaddles();
    void loop();

  private:
    std::stringstream mainText;
    SDL_Window* gameWindow;
    SDL_Renderer* gameRenderer;
    SDL_Surface* gameIcon;
    TTF_Font* mainFont;
    TTF_Font* escFont;
    Texture textures[MAX_TEXTURES];
    Texture startPromptTexture;
    Texture escPromptTexture;
    Texture scoreboardTexture;
    int numActiveTextures;
    SDL_Color textColor;
    Dot dot;
    Scoreboard sb;
    Paddle p1;
    Paddle p2;
    SDL_Rect p1Rect;
    SDL_Rect p2Rect;
    SDL_Rect dotRect;
    bool bounceOffPaddle;
    bool p1Scored;
    bool p1Wins;
    int framesSinceCollision;
    Mix_Chunk *bounce = NULL;
    Timer m_GameTimer;
};

#endif // !GAMELOOP_HPP
