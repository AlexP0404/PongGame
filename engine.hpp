#ifndef RENDER_CPP
#define RENDER_CPP

#include <SDL2/SDL_ttf.h>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_surface.h>

#include "Texture.hpp"

using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

struct FontDeleter {
  void operator()(TTF_Font *font) const {
    TTF_CloseFont(font);
    font = nullptr;
  }
};

class Engine {

public:
  Engine();
  Engine(const int screenWidth, const int screenHeight);
  ~Engine();

  void setScreenSize(const int screenWidth, const int screenHeight) {
    m_ScreenWidth = screenWidth, m_ScreenHeight = screenHeight;
  }

  bool init();
  bool loadMedia();
  bool setTextureCoorCentered(const string &&textureName, int x, int y);
  bool setTextureCoor(const string &&textureName, int x, int y);

  bool setTextTexture(const string &&textureName, const string &&fontName,
                      const string &text);

  bool createTextureFromFile(const string &&textureName,
                             const std::filesystem::path &&fileName);
  void renderTextures();
  void renderScreen();
  void clearScreen();
  void eraseTextures(const vector<string> &&texturesToErase);
  void eraseTexture(const string &&textureName);
  void setPaddleSize(int width, int height);
  void drawNet();
  void drawDot(int dotX, int dotY, int dotRadius);
  void drawPaddles(int p1X, int p1Y, int p2X, int p2Y);
  void playBounce();

private:
  int m_ScreenWidth;
  int m_ScreenHeight;
  int m_PaddleWidth;
  int m_PaddleHeight;

  SDL_Window *gameWindow;
  SDL_Renderer *gameRenderer;
  SDL_Surface *gameIcon;
  SDL_Color textColor;
  SDL_Rect p1Rect;
  SDL_Rect p2Rect;
  SDL_Rect dotRect;

  Mix_Chunk *bounce;
  unordered_map<string, unique_ptr<Texture>>
      textures; // map all textures on hash table
  unordered_map<string, TTF_Font *> fonts;
};

#endif
