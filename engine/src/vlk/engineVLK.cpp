#include "engineVLK.hpp"
#include "renderer.hpp"
#include <memory>

// allows things like the input class to access the current application instance
// while avoiding creating a whole new engine object
static EngineVLK *s_Instance = nullptr;
EngineVLK &EngineVLK::Get() { return *s_Instance; }

EngineVLK::EngineVLK() {
  s_Instance = this; // set the current application instance
}

EngineVLK::~EngineVLK() {
  s_Instance = nullptr; // instance is no longer valid
}

bool EngineVLK::init() {
  mpWindow = std::unique_ptr<windowVLK>(
      new windowVLK(m_ScreenWidth, m_ScreenHeight, m_GameTitle));
  return mpWindow != nullptr;
}

bool EngineVLK::loadMedia() { return true; }

bool EngineVLK::shouldQuit() { return mpWindow->windowShouldClose(); }

bool EngineVLK::setTextureCoorCentered(const std::string &&textureName, int x,
                                       int y) {
  return true;
}

bool EngineVLK::setTextureCoor(const std::string &&textureName, int x, int y) {
  return true;
}

bool EngineVLK::setTextTexture(const std::string &&textureName,
                               const std::string &&fontName,
                               const std::string &text) {
  return true;
}

bool EngineVLK::createTextureFromFile(const std::string &&textureName,
                                      const fs::path &&fileName) {
  return true;
}

void EngineVLK::renderTextures() {}

void EngineVLK::renderScreen() { mpWindow->pollEvents(); }

void EngineVLK::clearScreen() {}

void EngineVLK::eraseTextures(
    const std::vector<std::string> &&texturesToErase) {}

void EngineVLK::eraseTexture(const std::string &&textureName) {}

void EngineVLK::drawNet() {}

void EngineVLK::drawDot(int dotX, int dotY, int dotRadius) {}

void EngineVLK::drawPaddles(int p1X, int p1Y, int p2X, int p2Y) {}

void EngineVLK::playBounce() {}

std::shared_ptr<Renderer> EngineVLK::PFN_GetRenderer() { return mpRenderer; }
std::shared_ptr<windowVLK> EngineVLK::PFN_GetWindowVLK() { return mpWindow; }
