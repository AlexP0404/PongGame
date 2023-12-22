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
  m_TargetPaddleY = m_AIpaddle->getScreenH() / 2;//start paddle in middle of the screen
}

void AI::setDotBounceX(bool dotDirection, int dotBounceX, bool bounceOffTop){
  if(dotDirection != m_RightPaddle) return;//only matters if heading in the same direction as paddle
  //dot bounces in 90 degree angle,
  //target y is ratio to dot bounce x and screenHeight/screenWidth
  m_TargetPaddleY = std::abs(m_AIpaddle->getPosX() - dotBounceX); 
  if(!bounceOffTop){ 
    m_TargetPaddleY = m_AIpaddle->getScreenH() - m_TargetPaddleY - 50;// - m_AIpaddle->getSizeY();
  }//
  //distance between dot bounce x and paddle = to paddle y target?
}

void AI::movePaddle(){
   
  bool needToMove = (m_AIpaddle->getPosY() > m_TargetPaddleY - 30 ||
    m_AIpaddle->getPosY() + m_AIpaddle->getSizeY() - 30 < m_TargetPaddleY );
  
  if(!needToMove) return;
  m_MovePaddleUp = (m_AIpaddle->getPosY() > m_TargetPaddleY - 30);
  m_AIpaddle->move(m_MovePaddleUp);
  
}
