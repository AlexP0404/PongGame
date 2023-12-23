#ifndef AI_HPP
#define AI_HPP

#include "paddle.hpp"
// this will control the second paddle when the user selects single player.
// it will mathematically determine where to place the paddle based on where the
// dot bounced off the border (this may be too overpowered)
//

class AI {

public:
  AI();

  void setPaddle(Paddle *paddle);
  void setDotBounceX(bool dotDirection, int dotBounceX, bool bounceOffTop);
  void movePaddle();

private:
  int m_TargetPaddleY;
  bool m_IsRightPaddle;
  bool m_MovePaddleUp;
  Paddle *m_AIpaddle;
};

#endif
