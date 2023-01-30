#include "paddle.hpp"

Paddle::Paddle(){
  posY = 0;
  velY = 0;
  screenHeight = 0;
}

void Paddle::setScreenHeight(int screenHeight){
  this->screenHeight = screenHeight;
}

void Paddle::setPos(int pos){
  posY = pos;
}
int Paddle::getPos(){
  return posY;
}

void Paddle::handleEvent(bool up){
  if(up){
    posY -= PAD_VEL;
    if(posY < 0)
      posY = 0;
  }
  else{
    posY += PAD_VEL;
    if(posY + PADDLE_HEIGHT > screenHeight)
      posY = screenHeight - PADDLE_HEIGHT;
  }
}

