#include "paddle.hpp"

void Paddle::setScreenHeight(int screenHeight){
  this->screenHeight = screenHeight;
}

void Paddle::setPos(int pos){
  mPosY = pos;
}
int Paddle::getPos(){
  return mPosY;
}

void Paddle::handleEvent(bool up){
  if(up){
    mPosY -= PAD_VEL;
    if(mPosY < 0)
      mPosY = 0;
  }
  else{
    mPosY += PAD_VEL;
    if(mPosY + PADDLE_HEIGHT > screenHeight)
      mPosY = screenHeight - PADDLE_HEIGHT;
  }
}

