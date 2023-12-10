#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <random>

class Entity {
  public:
    Entity(int sizeX = 0, int sizeY = 0, int screenH = 0, int screenW = 0,int velX = 0, int velY = 0)
    : mSizeX(sizeX), mSizeY(sizeY), mScreenH(screenH), mScreenW(screenW) , mVelX(velX), mVelY(velY){}
    virtual ~Entity() = default;

    void setScreen(int width, int height){
       mScreenW = width;
       mScreenH = height;
    }

    void setPos(int posX, int posY){
      mPosX = posX;
      mPosY = posY;
    }

    void setVel(int velX, int velY){
      mVelX = velX;
      mVelY = velY;
    }

    int getPosX() { return mPosX; }
    int getPosY() { return mPosY; }
    int getSizeX() { return mSizeX; }
    int getSizeY() { return mSizeY; }
    inline bool genRandBool() { return mRandDev() % 10 > 5; } //50/50 to be true or false
  protected:
    int mSizeX   = 0;
    int mSizeY   = 0;
    int mPosX    = 0;
    int mPosY    = 0;
    int mScreenW = 0;
    int mScreenH = 0;
    int mVelX    = 0;
    int mVelY    = 0;
    std::random_device mRandDev;
};

#endif
