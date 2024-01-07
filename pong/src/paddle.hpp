#ifndef PADDLE_HPP
#define PADDLE_HPP

#include "entity.hpp"
//figure out how we are going to get the information about the 
//paddles out to the texture in the loop
class Paddle : public Entity { 
  public:
    using Entity::Entity;//using the base class constructor
    Paddle(Paddle &p){
      p = *this; 
    }
    Paddle& operator=(const Paddle&){
      return *this;
    }
    
    void setSpeed(int speed = 0);
    void move(bool dir) override;
    
    const int PADDLE_HEIGHT = 100;
    const int PAD_VEL = 10;
  private:
    int m_PaddleSpeed = 0;
};

#endif // !PADDLE_HPP
