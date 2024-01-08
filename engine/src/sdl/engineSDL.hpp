#ifndef ENGINE_SDL_HPP
#define ENGINE_SDL_HPP

#include <memory>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include "Texture.hpp"
#include "engine.hpp"

using std::unique_ptr;
using std::unordered_map;

struct FontDeleter {
  void operator()(TTF_Font *font) const {
    TTF_CloseFont(font);
    font = nullptr;
  }
};

class EngineSDL : public Engine {

public:
  EngineSDL();
  ~EngineSDL() override;

  bool init() override;
  bool loadMedia() override;
  bool setTextureCoorCentered(const string &&textureName, int x,
                              int y) override;
  bool setTextureCoor(const string &&textureName, int x, int y) override;

  bool setTextTexture(const string &&textureName, const string &&fontName,
                      const string &text) override;

  bool createTextureFromFile(const string &&textureName,
                             const fs::path &&fileName) override;
  void renderTextures() override;
  void renderScreen() override;
  void clearScreen() override;
  void eraseTextures(const vector<string> &&texturesToErase) override;
  void eraseTexture(const string &&textureName) override;
  void drawNet() override;
  void drawDot(int dotX, int dotY, int dotRadius) override;
  void drawPaddles(int p1X, int p1Y, int p2X, int p2Y) override;
  void playBounce() override;

private:
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
