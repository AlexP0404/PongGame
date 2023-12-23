#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <random>

class Entity {
public:
  Entity(int sizeX = 0, int sizeY = 0, int velX = 0, int velY = 0)
      : mSizeX(sizeX), mSizeY(sizeY), mVelX(velX), mVelY(velY) {}
  virtual ~Entity() = default;
  virtual void move(bool dir = false) = 0;
  void setScreen(int width, int height) {
    mScreenW = width;
    mScreenH = height;
  }

  void setPos(int posX, int posY) {
    mPosX = posX;
    mPosY = posY;
  }

  void setVel(int velX, int velY) {
    mVelX = velX;
    mVelY = velY;
  }
  void setSize(int sizeX, int sizeY) {
    mSizeX = sizeX;
    mSizeY = sizeY;
  }

  int getPosX() { return mPosX; }
  int getPosY() { return mPosY; }
  int getSizeX() { return mSizeX; }
  int getSizeY() { return mSizeY; }
  int getScreenW() { return mScreenW; }
  int getScreenH() { return mScreenH; }
  inline bool genRandBool() {
    return mRandDev() % 10 > 5;
  } // 50/50 to be true or false
  inline int genRandNum(int maxVal) { return mRandDev() % maxVal; }

protected:
  int mSizeX;
  int mSizeY;
  int mPosX = 0;
  int mPosY = 0;
  int mScreenW = 0;
  int mScreenH = 0;
  int mVelX;
  int mVelY;
  std::random_device mRandDev;
};

#endif
