#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum KeyPressMoves{
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,
  KEY_PRESS_SURFACE_TOTAL
};

class LTexture{
  public:
    LTexture();
    ~LTexture();

    bool loadFromFile(std::string path);
    void free();
    void render(int x, int y);
    void move(int direction);//if we want to move the texture down, add (INTEGER AMOUNT) to height
    int getWidth();
    int getHeight();
    int getX();
    int getY(); 
  private:
      SDL_Texture* mTexture;
      int x = 240;
      int y = 190; //coords of image placement
      int mWidth;
      int mHeight;
};

bool init();

bool loadMedia();

void close();


SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

LTexture gFooTexture;
LTexture gBackgroundTexture;

LTexture::LTexture(){
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture(){
  free();
}

bool LTexture::loadFromFile(std::string path){
  free();

  SDL_Texture* newTexture = NULL;

  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL){
    printf("Unable to load image %s! SDL_image ERror: %s\n", path.c_str(), IMG_GetError());
  }
  else{
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if(newTexture == NULL){
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else{
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }
    SDL_FreeSurface(loadedSurface);
  }
  mTexture = newTexture;
  return mTexture != NULL;
}

void LTexture::free(){
  if(mTexture != NULL){
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::render(int x, int y){
  this->x = x;
  this->y = y;
  SDL_Rect renderQuad = {x, y, mWidth, mHeight};
  SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad);
}

void LTexture::move(int direction){
  //need to free the texture, create a new one, then render it in new location
  int xCoor = x;
  int yCoor = y;
  free(); //old texture gone
  gFooTexture.loadFromFile("example/foo.png"); //new one generated
  switch(direction){
    case KEY_PRESS_SURFACE_DOWN:
      if(yCoor < 360){//check if in bounds
        render(xCoor, yCoor+20);//new location
      }
      break;
    case KEY_PRESS_SURFACE_UP:
      if(yCoor > 9){
        render(xCoor,yCoor-20);
      }
      break;
    case KEY_PRESS_SURFACE_LEFT:
      if(xCoor > 0){
        render(xCoor-20,yCoor);
      }
      break;
    case KEY_PRESS_SURFACE_RIGHT:
      if(xCoor < 580){
        render(xCoor+20, yCoor);
      }
      break;
    default:
      render(xCoor,yCoor);
      break;
  }


}

int LTexture::getWidth(){
  return mWidth;
}

int LTexture::getHeight(){
  return mHeight;
}

int LTexture::getX(){
  return x;
}

int LTexture::getY(){
  return y;
}

bool init(){
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else
  {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
      printf("Warning: Linear texture filtering not enabled!\n");
    }

  gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if(gWindow  == NULL){
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
  else{
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
      if(gRenderer == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else{
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
          printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
          success = false;
        }

      }
    }

  }
  return success;
}

bool loadMedia(){
  bool success = true;
  
  if(!gFooTexture.loadFromFile("example/foo.png")){
    printf("Failed to load Foo texture image!\n");
    success = false;
  }

  if(!gBackgroundTexture.loadFromFile("example/background.png")){
    printf("Failed to load background texture image!\n");
    success = false;
  }

  return success;
}

void close(){

  gFooTexture.free();
  gBackgroundTexture.free();

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  IMG_Quit();
  SDL_Quit();
}

int main(int argc,char* args[]){
  if(!init()){
    printf("Failed to initialize!\n");
  }
  else{
    if(!loadMedia()){
      printf("Failed to load media!\n");
    }
    else{
      bool quit = false;
      SDL_Event e;

      SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(gRenderer);
     //gFooTexture.render(240, 190);

      while(!quit){
        gBackgroundTexture.render(0,0);
        while(SDL_PollEvent(&e) != 0){
          if(e.type == SDL_QUIT){
            quit = true;
          }
          else if (e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym){
              case SDLK_UP:
                gFooTexture.move(KEY_PRESS_SURFACE_UP);
                break;
              case SDLK_DOWN:
                gFooTexture.move(KEY_PRESS_SURFACE_DOWN);
                break;
              case SDLK_LEFT:
                gFooTexture.move(KEY_PRESS_SURFACE_LEFT);
                break;
              case SDLK_RIGHT:
                gFooTexture.move(KEY_PRESS_SURFACE_RIGHT);
                break;
              case SDLK_ESCAPE:
                quit = true;//escape now quits as well :)
                break;
              default:
                gFooTexture.move(KEY_PRESS_SURFACE_DEFAULT);
                break;
            }
          }
        }

        gFooTexture.move(KEY_PRESS_SURFACE_DEFAULT); //will have to replace this with move()
        SDL_RenderPresent(gRenderer);
        
      }
    }
  }
  close();

  return 0;
}
