#ifndef DOT_HPP
#define DOT_HPP

#include "entity.hpp"
#include "timer.hpp"
// dot needs to move on its own
// INITIALLY: moves in random direction slowly.
// when a collision is detected: bounce() needs to do some math to figure out
// the direction it goes next along with the velocity of the ball.
const int DOT_RADIUS = 10;

class Dot : public Entity {
public:
  using Entity::Entity;
  Dot &operator=(const Dot &) { return *this; }
  void
  bounce(bool isBouncePaddle); // needs to handle bouncing off top and bottom
                               // walls and paddles how to handle bounce?
  void set();
  void move(bool dir = false)
      override; // move faster the longer it goes without resetting (scoring)
  void setInitSpeed(int speed = 2);
  bool getDirectionX();
  const int MAX_DOT_VEL = 5;

private:
  int directionX = 0;
  int directionY = 0;
  int m_InitSpeed = 0;
  Timer m_dotTimer;
};

#endif // !DOT_HPP
