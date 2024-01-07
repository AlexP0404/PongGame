#include "scoreboard.hpp"
#include <string>

Scoreboard::Scoreboard(){
  reset();
}

void Scoreboard::reset(){
  player1Score = 0;
  player2Score = 0;
}

std::string Scoreboard::getScoreString(){
  return std::to_string(player1Score) + "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" + std::to_string(player2Score); 
}

void Scoreboard::incPlayer1(){
  player1Score++;
}

void Scoreboard::incPlayer2(){
  player2Score++;
}

bool Scoreboard::gameOver(){
  return player1Score >= SCORE_TO_WIN || player2Score >= SCORE_TO_WIN;
}
