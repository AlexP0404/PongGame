#include <iostream>
#include "gameLoop.hpp"

int main(){
  GameLoop pongLoop;
  if(!pongLoop.init()){
    std::cout << "Game Loop failed to initialize!\n";
    return -1;
  }
  if(!pongLoop.loadMedia()){
    std::cout << "Game loop failed to load media!\n";
    return -1;
  }

  pongLoop.loop();

  return 0;

}
