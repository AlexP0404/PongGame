#ifndef AI_HPP
#define AI_HPP

#include <iostream>
#include "paddle.hpp"

//this will control the second paddle when the user selects single player.
//it will mathematically determine where to place the paddle based on where the dot bounced off the border
//(this may be too overpowered)
//

class AI{

  public:
    AI();
    AI(bool rightPaddle);
    
    void setPaddle(Paddle* paddle);
    void setDotBounceX(bool dotDirection, int dotBounceX,bool bounceOffTop);
    void movePaddle();

  private:
    int m_TargetPaddleY;
    float m_ScreenRatio;
    bool m_RightPaddle;
    bool m_MovePaddleUp;
    Paddle* m_AIpaddle;
};

#endif
