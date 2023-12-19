#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP
//deals with window/render initializing
//deals with allocating/deallocating textures
//runs the game loop and calls the other classes
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_surface.h>

#include "Texture.hpp"
#include "dot.hpp"
#include "ai.hpp"//has paddle included already (I know I could include both because of header guards but whatevs)
#include "scoreboard.hpp"
#include "timer.hpp"

using std::unique_ptr;
using std::unordered_map;
using std::string;
//need to create an "entity" class that is the base class
//for the pong and the paddles
//need to start multithreading for keychecking so both players
//can move at once
//make the game loop based on timers instead of framerate
//fix collision with outter boundaries of the window
//then add networking (UDP?) to make it online multiplayer

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;
const int MAX_TEXTURES = 15;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;
const int PADDLE_OFFSET = 100;
const float COLLISION_CHECK_DELAY = 100.0f;
const float GAME_LOOP_DELAY = 10.0f;

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
    void setSpeedSelectText();
    void setSpeed(int dir = 0);
    void drawNet();
    void setScoreboardText();
    void drawDot();
    void drawPaddles();
    void loop();
    void handleInputs();
    void countDown();

  private:
    SDL_Window* gameWindow;
    SDL_Renderer* gameRenderer;
    SDL_Surface* gameIcon;
    TTF_Font* mainFont;
    TTF_Font* escFont;
    SDL_Color textColor;
    SDL_Rect p1Rect;
    SDL_Rect p2Rect;
    SDL_Rect dotRect;
    
    unordered_map<string,unique_ptr<Texture>> textures;//map all textures on hash table
    
    Dot dot;
    Scoreboard sb;
    Paddle p1;
    Paddle p2;
    AI ai1;  

    string mainText;
    string difficultySelectText;
    bool bounceOffPaddle;
    bool p1Scored;
    bool p1Wins;
    bool quit;
    bool start;
    bool gameOver;
    bool lastPressedEsc;
    bool singlePlayer;
    Mix_Chunk *bounce;
    Timer m_GameTimer;
    Timer m_CollisionPaddleTimer;
    Timer m_CollisionBorderTimer;
};

#endif // !GAMELOOP_HPP
