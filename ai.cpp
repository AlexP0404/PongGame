#include "ai.hpp"
#include "gameLoop.hpp"

AI::AI(){
  m_RightPaddle = true;
  m_MovePaddleUp = true;
}

AI::AI(bool rightPaddle){
  m_RightPaddle = rightPaddle;
  m_MovePaddleUp = true;
}

void AI::setPaddle(Paddle* paddle){
  m_AIpaddle = paddle;
  m_ScreenRatio = static_cast<float>(m_AIpaddle->getScreenH()) / m_AIpaddle->getScreenW();
  //by default is 0.75
}

void AI::setDotBounceX(bool dotDirection, int dotBounceX, bool bounceOffTop){
  if(dotDirection != m_RightPaddle) return;//only matters if heading in the same direction as paddle
  //dot bounces in 90 degree angle,
  //target y is ratio to dot bounce x and screenHeight/screenWidth
  m_TargetPaddleY = dotBounceX * m_ScreenRatio;
  if(bounceOffTop){ 
    m_TargetPaddleY -= m_AIpaddle->getScreenH() - PADDLE_HEIGHT;
    m_TargetPaddleY *= -1;
  }
  //THIS PROBABLY NEEDS SOME SIN/COS MATHS
  std::cout << "Target: " << m_TargetPaddleY << " Bounce off top: " << (bounceOffTop ? "True" : "False") << '\n';
}

void AI::movePaddle(){
   
  bool needToMove = !(m_AIpaddle->getPosY() + 10 > m_TargetPaddleY &&
    m_AIpaddle->getPosY() - 10 < m_TargetPaddleY + m_AIpaddle->getSizeY());
  
  if(!needToMove) return;
  std::cout << "Current Pos: " << m_AIpaddle->getPosY() << '\n'; 
  m_MovePaddleUp = (m_AIpaddle->getPosY() > m_TargetPaddleY);
  m_AIpaddle->move(m_MovePaddleUp);
  
}
