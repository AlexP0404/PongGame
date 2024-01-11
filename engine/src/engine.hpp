#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class Engine {
protected:
  int m_ScreenWidth;
  int m_ScreenHeight;
  int m_PaddleWidth;
  int m_PaddleHeight;
  std::string m_GameTitle;

public:
  void setPaddleSize(int width, int height) {
    m_PaddleWidth = width, m_PaddleHeight = height;
  }
  void setScreenSize(const int screenWidth, const int screenHeight) {
    m_ScreenWidth = screenWidth, m_ScreenHeight = screenHeight;
  }
  void setTitle(const std::string &&title) { m_GameTitle = title; }

  virtual ~Engine() = default;

  virtual bool init() = 0;
  virtual bool loadMedia() = 0;
  virtual bool shouldQuit() = 0;

  virtual bool setTextureCoorCentered(const std::string &&textureName, int x,
                                      int y) = 0;
  virtual bool setTextureCoor(const std::string &&textureName, int x,
                              int y) = 0;

  virtual bool setTextTexture(const std::string &&textureName,
                              const std::string &&fontName,
                              const std::string &text) = 0;

  virtual bool createTextureFromFile(const std::string &&textureName,
                                     const fs::path &&fileName) = 0;
  virtual void renderTextures() = 0;
  virtual void renderScreen() = 0;
  virtual void clearScreen() = 0;
  virtual void
  eraseTextures(const std::vector<std::string> &&texturesToErase) = 0;
  virtual void eraseTexture(const std::string &&textureName) = 0;
  virtual void drawNet() = 0;
  virtual void drawDot(int dotX, int dotY, int dotRadius) = 0;
  virtual void drawPaddles(int p1X, int p1Y, int p2X, int p2Y) = 0;
  virtual void playBounce() = 0;
};

#endif
