#include "paddle.hpp"

void Paddle::setSpeed(int speed){
  m_PaddleSpeed = speed;
}

void Paddle::move(bool dir){
  if(dir){//up
    mPosY -= (PAD_VEL + m_PaddleSpeed);
    if(mPosY < 0)
      mPosY = 0;
  }
  else{
    mPosY += PAD_VEL + m_PaddleSpeed;
    if(mPosY + PADDLE_HEIGHT > mScreenH)
      mPosY = mScreenH- PADDLE_HEIGHT;
  }
}

