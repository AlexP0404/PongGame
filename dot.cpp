#include "dot.hpp"

Dot::Dot(){
  srandom(time(NULL));
  mPosX = 0;
  mPosY = 0;
  
  mVelY = 0;
  mVelX = 0;
  m_InitSpeed = 2;

  directionX = 0;
  directionY = 0;
}

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

  if(mVelX == 0 || mVelY == 0){
    mVelX += directionX * m_InitSpeed;
    mVelY += directionY * m_InitSpeed;
  }
  //std::cout << mVelX << "," << mVelY <<  " ";
}

void Dot::setInitSpeed(int speed){
  this->m_InitSpeed = speed;
}

void Dot::setPosY(int y){
  this->mPosY = y;
}

void Dot::setScreen(int h, int w){
  screenHeight = h;
  screenWidth = w;
}

bool genRandom(){
  return (rand() % 10 > 5);//if it is 
}

void Dot::set(){//starts the dot out slowly in a random direction (probably lean more towards the players)
  mPosX = screenWidth / 2;
  mPosY = screenHeight / 2;
  
  
  directionX = (genRandom()) ? 1 : -1;//if true, positive, else negative
  directionY = (genRandom()) ? 1 : -1;
  
  mVelX = directionX * m_InitSpeed;
  mVelY = directionY * m_InitSpeed;
  
  //std::cout << directionX << ',' << directionY << std::endl; 
}

void Dot::move(){//updates where the ball is on the screen
                 //after this is updated in the loop, check for any collisions
  
  mPosX += mVelX * m_dotTimer.ElapsedMillis() / 10;
  mPosY += mVelY * m_dotTimer.ElapsedMillis() / 10;
  m_dotTimer.Reset();
}

int Dot::getPosX(){
  return mPosX;
} 

int Dot::getPosY(){
  return mPosY;
}
