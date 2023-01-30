#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>

const int SCORE_TO_WIN = 7;

class Scoreboard{
  public:
    Scoreboard();
    
    std::string getScoreString();
    void reset();
    bool gameOver();
    void incPlayer1();
    void incPlayer2();
  
  private:
    int player1Score;
    int player2Score;
};

#endif // !DEBUG
