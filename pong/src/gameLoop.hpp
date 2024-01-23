#ifndef GAMELOOP_HPP
#define GAMELOOP_HPP
// deals with window/render initializing
// deals with allocating/deallocating textures
// runs the game loop and calls the other classes
#include <string>
#include <thread>

#ifndef ENGINE_USE_VLK
#include "engineSDL.hpp"
#include "inputSDL.hpp"
typedef EngineSDL selectedEngine;
typedef inputSDL Input;
#else
#include "engineVLK.hpp"
#include "inputsVLK.hpp"
typedef EngineVLK selectedEngine;
typedef inputVLK Input;
#endif

#include "ai.hpp"
#include "dot.hpp"
#include "scoreboard.hpp"
#include "timer.hpp"

// then add networking (UDP?) to make it online multiplayer

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;
const int MAX_TEXTURES = 15;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 100;
const int PADDLE_OFFSET = 100;
const float COLLISION_CHECK_DELAY = 100.0f;
const float GAME_LOOP_DELAY = 1.0f;

class GameLoop {
public:
  GameLoop();
  ~GameLoop();

  bool init();
  void loop();

private:
  bool collision();
  bool score();
  void setStartText();
  void setScoreboardText();
  void setModeSelectText();
  void setMode();
  void setSpeedSelectText();
  void setSpeed(int dir = 0);
  void handleInputs();
  void countDown();

private:
  selectedEngine gameEngine;
  Input input;
  std::thread m_InputsThread;

  Dot dot;
  Scoreboard sb;
  Paddle p1;
  Paddle p2;
  AI ai1, ai2;

  std::string mainText;
  std::string difficultySelectText;
  std::string modeSelectText;

  int modeSelection;
  bool bounceOffPaddle;
  bool p1Scored;
  bool p1Wins;
  bool quit;
  bool start;
  bool gameOver;
  bool lastPressedEsc;
  bool singlePlayer;
  Timer m_GameTimer;
  Timer m_CollisionPaddleTimer;
  Timer m_CollisionBorderTimer;
};

#endif // !GAMELOOP_HPP
