#include "dot.hpp"
#include <random>

Dot::Dot(){
  mPosX = 0;
  mPosY = 0;
  
  currVel = 0;
  mVelY = 0;
  mVelX = 0;

  directionX = 0;
  directionY = 0;
}

void Dot::bounce(char wall){//this is gonna be the hardest part
  //orthogonal to the direction and the wall/paddle?
  wall = 'a';
}

void Dot::set(){//starts the dot out slowly in a random direction (probably lean more towards the players)
  currVel = 1;
  double lowerBnd = -1;
  double upperBnd = 1;
  std::uniform_int_distribution<int> unif(lowerBnd,upperBnd);
  std::default_random_engine re;
  
  directionX = unif(re);//should give two random directional values from -1 to 1
  directionY = unif(re);

  std::cout << directionX << ',' << directionY << std::endl; 
}

void Dot::move(){//updates where the ball is on the screen
                 //after this is updated in the loop, check for any collisions
  if(currVel <= MAX_DOT_VEL)
    currVel++;//this might be too fast 
  //I want to move the ball at the current velocity (speed) in the proper direction
  //how to deal with all other angles besides 0,45,90,135,180,225,270,315, and 360
  //double? 
  mVelX += currVel * directionX;
  mVelY += currVel * directionY;

  mPosX += mVelX;
  mPosY += mVelY;
}

int Dot::getPosX(){
  return int(mPosX);
} 

int Dot::getPosY(){
  return int(mPosY);
}
