#include "ai.hpp"

AI::AI(){
  m_RightPaddle = true;
  m_MovePaddleUp = true;
}

AI::AI(bool rightPaddle){
  m_RightPaddle = rightPaddle;
  m_MovePaddleUp = true;
}

void AI::setPaddle(Paddle* paddle){
  if(!paddle) throw std::runtime_error("Paddle pointer not initialized!"); 

  m_AIpaddle = paddle;
  m_ScreenRatio = static_cast<float>(m_AIpaddle->getScreenH()) / m_AIpaddle->getScreenW();
  //by default is 0.75
  std::cout << "Screen Ratio " << m_ScreenRatio << '\n';
}

void AI::setDotBounceX(bool dotDirection, int dotBounceX, bool bounceOffTop){
  if(dotDirection != m_RightPaddle) return;//only matters if heading in the same direction as paddle
  //dot bounces in 90 degree angle,
  //target y is ratio to dot bounce x and screenHeight/screenWidth
  m_TargetPaddleY = dotBounceX * ((dotBounceX > m_AIpaddle->getScreenW() / 2)
    ? m_ScreenRatio : m_ScreenRatio + 0.1);// + m_AIpaddle->getPosX();
  if(bounceOffTop){ 
    m_TargetPaddleY = m_AIpaddle->getScreenH() - m_TargetPaddleY;// - m_AIpaddle->getSizeY();
  }
  std::cout << "Dot bounced X : " << dotBounceX << '\n';
  std::cout << "Target: " << m_TargetPaddleY << " Bounce off top: " << (bounceOffTop ? "True" : "False") << '\n';
  std::cout << "Current Pos: " << m_AIpaddle->getPosY() << '\n'; 
}

void AI::movePaddle(){
   
  bool needToMove = (m_AIpaddle->getPosY() > m_TargetPaddleY - 30 ||
    m_AIpaddle->getPosY() + m_AIpaddle->getSizeY() - 30 < m_TargetPaddleY );
  
  if(!needToMove) return;
  m_MovePaddleUp = (m_AIpaddle->getPosY() > m_TargetPaddleY - 30);
  m_AIpaddle->move(m_MovePaddleUp);
  
}
