#include "dot.hpp"

void Dot::bounce(bool isBouncePaddle){
  //if bouncing off left/right wall
  //  directionX = -directionX
  //if bouncing off up/down wall
  //  directionY = -directionY
  if(isBouncePaddle){
    directionX = -directionX;
  }
  else{
    directionY = -directionY;
  }

  mVelX *= directionX;
  mVelY *= directionY;

  if(mVelX >= MAX_DOT_VEL || (mVelX * -1) >= MAX_DOT_VEL){
    mVelX = MAX_DOT_VEL * directionX;//hard cap
  }
  else
    mVelX += directionX;//this is making the velocity go to 0 when begining at 1/-1

  if(mVelY >= MAX_DOT_VEL || (mVelY * -1) >= MAX_DOT_VEL){
    mVelY = MAX_DOT_VEL * directionY;
  }
  else
    mVelY += directionY;

  if(mVelX == 0 && mVelY == 0){
    mVelX += (directionX > 0) ? directionX + m_InitSpeed : directionX - m_InitSpeed;
    mVelY += (directionY > 0) ? directionY + m_InitSpeed : directionY - m_InitSpeed;
  }
}

void Dot::setInitSpeed(int speed){
  this->m_InitSpeed = speed;
}

void Dot::set(){//starts the dot out slowly in a random direction (probably lean more towards the players)
  mPosX = mScreenW / 2;
  mPosY = mScreenH / 2;
  
  
  directionX = (genRandBool()) ? 1 : -1;//if true, positive, else negative
  directionY = (genRandBool()) ? 1 : -1;
  
  mVelX = (directionX > 0) ? directionX + m_InitSpeed : directionX - m_InitSpeed;
  mVelY = (directionY > 0) ? directionY + m_InitSpeed : directionY - m_InitSpeed;
  //TODO:: FIX DOT BEING STUCK OCCASIONALLY AT THE START
  //       FIX OVERRALL SPEED OF THE DOT (TIMERS)
  //       FIX DOT BOUNCE/COLLSION SYSTEM
  //
  //std::cout << directionX << ',' << directionY << std::endl; 
}

void Dot::move(bool dir){//updates where the ball is on the screen
                 //after this is updated in the loop, check for any collisions
  float elapsed = m_dotTimer.ElapsedMillis(); 
  mPosX += mVelX * elapsed / 5;
  mPosY += mVelY * elapsed / 5;
  m_dotTimer.Reset();
  std::cout << mPosX << "," << mPosY <<  " ";
}

