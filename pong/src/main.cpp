#include "gameLoop.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION

int main() {

  GameLoop pongLoop;
  if (!pongLoop.init()) {
    std::cout << "Game Loop failed to initialize!\n";
    return -1;
  }
  try {
    pongLoop.loop();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
