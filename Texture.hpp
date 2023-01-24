#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <stdio.h>

class Texture{
public:
  Texture();
  Texture(SDL_Renderer& renderer);
  ~Texture();
  
  void setRenderer(SDL_Renderer& renderer);
  bool loadFromFile(std::string path);
  bool loadFromRenderedText(std::string texturedText, SDL_Color textColor, TTF_Font* gFont);
  void free();
  void setColor(Uint8 red, Uint8 green, Uint8 blue);
  void setBlendMode(SDL_BlendMode blending);
  void setAlpha(Uint8 alpha);
  void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) ;
  int getWidth();
  int getHeight();

private:
  SDL_Texture* mTexture;
  int mWidth;
  int mHeight;
  SDL_Renderer* gameRenderer;
};
#endif // !TEXTURE_HPP
