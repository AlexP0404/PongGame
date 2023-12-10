#ifndef PADDLE_HPP
#define PADDLE_HPP

#include "entity.hpp"
//figure out how we are going to get the information about the 
//paddles out to the texture in the loop
class Paddle : public Entity { 
  public:
    using Entity::Entity;//using the base class constructor
    Paddle& operator=(const Paddle&){
      return *this;
    }

    void setScreenHeight(int screenHeight);
    void handleEvent(bool up);
    void setPos(int pos);
    int getPos();
    
    const int PADDLE_HEIGHT = 100;
    const int PAD_VEL = 50;
  public:
    int screenHeight;
    int velY;
};

#endif // !PADDLE_HPP
