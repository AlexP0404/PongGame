#include "gameLoop.hpp"
#include <iostream>

int main() {

  GameLoop pongLoop;
  if (!pongLoop.init()) {
    std::cout << "Game Loop failed to initialize!\n";
    return -1;
  }

  pongLoop.loop();

  return 0;
}
