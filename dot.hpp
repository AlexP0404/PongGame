#ifndef DOT_HPP
#define DOT_HPP
#include <iostream>
#include <random>
#include <time.h>
//dot needs to move on its own
//INITIALLY: moves in random direction slowly.
//when a collision is detected: bounce() needs to do some math to figure out the direction it goes next
                              //along with the velocity of the ball.

class Dot
{
  public:
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;
    
    static const int MAX_DOT_VEL = 2;

    Dot();
    void bounce(bool isBouncePaddle);//needs to handle bouncing off top and bottom walls and paddles
                  //how to handle bounce?
    void set();
    void move();//move faster the longer it goes without resetting (scoring)
    void render();//this will likely have to be made in the gameloop
    void setScreen(int h, int w);
    int getPosX();
    int getPosY();
    void setPosY(int y);

  private:
    int screenHeight, screenWidth;
    int directionX, directionY;
    int mPosX, mPosY;
    int mVelX, mVelY;
};

#endif // !DOT_HPP
