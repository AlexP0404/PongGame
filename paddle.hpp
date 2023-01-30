#ifndef PADDLE_HPP
#define PADDLE_HPP
#include <iostream>
//figure out how we are going to get the information about the 
//paddles out to the texture in the loop
class Paddle{
  public:
    const int PADDLE_HEIGHT = 100;
    const int PAD_VEL = 30;
    
    Paddle();
    
    void setScreenHeight(int screenHeight);
    void handleEvent(bool up);
    void setPos(int pos);
    int getPos();
    
  public:
    int screenHeight;
    int posY;
    int velY;
};

#endif // !PADDLE_HPP
