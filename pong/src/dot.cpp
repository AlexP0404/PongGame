#include "dot.hpp"

void Dot::bounce(bool isBouncePaddle) {
  // if bouncing off left/right wall
  //   directionX = -directionX
  // if bouncing off up/down wall
  //   directionY = -directionY
  if (isBouncePaddle) {
    directionX = -directionX;
    mVelX = -mVelX;
  } else {
    directionY = -directionY;
    mVelY = -mVelY;
  }

  capSpeed();

  if (mVelX == 0 && mVelY == 0) {
    mVelX +=
        (directionX > 0) ? directionX + m_InitSpeed : directionX - m_InitSpeed;
    mVelY +=
        (directionY > 0) ? directionY + m_InitSpeed : directionY - m_InitSpeed;
  }
}

void Dot::capSpeed() {
  if (mVelX >= MAX_DOT_VEL + m_InitSpeed ||
      (mVelX * -1) >= MAX_DOT_VEL + m_InitSpeed) {
    // top speed is changed by difficulty level!
    mVelX = (MAX_DOT_VEL + m_InitSpeed) * directionX; // hard cap
  }                                                   // else
  /* mVelX += */
  /* directionX; // this is making the velocity go to 0 when begining at 1/-1 */

  if (mVelY >= MAX_DOT_VEL + m_InitSpeed ||
      (mVelY * -1) >= MAX_DOT_VEL + m_InitSpeed) {
    mVelY = (MAX_DOT_VEL + m_InitSpeed) * directionY;
  } // else
  /* mVelY += directionY; */
}

void Dot::setInitSpeed(int speed) { this->m_InitSpeed = speed; }

bool Dot::getDirectionX() {
  return directionX > 0; // true = going right
}

void Dot::set() { // starts the dot out slowly in a random direction (probably
                  // lean more towards the players)
  mPosX = mScreenW / 2;
  mPosY = mScreenH / 2;
  m_dotTimer.Reset();
  directionX = (genRandBool()) ? 1 : -1; // if true, positive, else negative
  directionY = (genRandBool()) ? 1 : -1;

  mVelX =
      (directionX > 0) ? directionX + m_InitSpeed : directionX - m_InitSpeed;
  mVelY =
      (directionY > 0) ? directionY + m_InitSpeed : directionY - m_InitSpeed;
}

void Dot::move(bool dir) { // updates where the ball is on the screen
  // after this is updated in the loop, check for any collisions
  /* float elapsed = m_dotTimer.ElapsedMillis(); */
  mPosX += mVelX; // * elapsed / 2;
  mPosY += mVelY; // * elapsed / 2;
  capSpeed();
  m_dotTimer.Reset();
}
