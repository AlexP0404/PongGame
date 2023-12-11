#include "paddle.hpp"

void Paddle::move(bool dir){
  if(dir){//up
    mPosY -= PAD_VEL;
    if(mPosY < 0)
      mPosY = 0;
  }
  else{
    mPosY += PAD_VEL;
    if(mPosY + PADDLE_HEIGHT > mScreenH)
      mPosY = mScreenH- PADDLE_HEIGHT;
  }
}

