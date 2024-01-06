#ifndef RENDER_CPP
#define RENDER_CPP

#include <SDL2/SDL_ttf.h>
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
  bool setTextureCoor(const string &&textureName, int x, int y);
  bool setTextTexture(const string &&textureName, const string &&fontName,
                      const string &&text);
  void renderTextures();
  void renderScreen();
  void eraseTextures(const vector<string> &&texturesToErase);

private:
  int m_ScreenWidth = 1280;
  int m_ScreenHeight = 960;

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
  unordered_map<string, unique_ptr<TTF_Font, FontDeleter>> fonts;
};

#endif
