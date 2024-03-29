#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include <string>

const int SCORE_TO_WIN = 7; // maybe this can be a game option

class Scoreboard {
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
