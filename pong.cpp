#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_ttf.h>
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
  bool loadFromRenderedText(std::string texturedText, SDL_Color textColor);
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
};

bool init();

bool loadMedia();

void close();

SDL_Window* gameWindow = NULL;

SDL_Renderer* gameRenderer = NULL;

TTF_Font* gFont = NULL;

LTexture gTextTexture;


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
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
  }
  else{
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    newTexture = SDL_CreateTextureFromSurface(gameRenderer, loadedSurface);
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

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gameRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}

  return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gameRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}



bool init(){
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else{
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")){
      printf("Warning: Linear texture filtering not enabled!");
    }
    gameWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(gameWindow == NULL){
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else{
      gameRenderer = SDL_CreateRenderer(gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if(gameRenderer == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else{
        SDL_SetRenderDrawColor(gameRenderer, 0x00,0x00,0x00,0xFF);

        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
          printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
          success = false;
        }

        if(TTF_Init() == -1){
          printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
          success = false;
        }
      }
    }
  }
  return success;
}

bool loadMedia(){
  bool success = true;

  gFont = TTF_OpenFont("lazy.ttf", 30);
  if( gFont == NULL){
    printf("failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  }
  else{
    SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF};
    if(!gTextTexture.loadFromRenderedText("Press Enter to start", textColor)){
      printf("Failed to render text texture!\n");
      success = false;
    }
  }
  return success;
}

void close(){
  gTextTexture.free();

  TTF_CloseFont(gFont);
  gFont = NULL;

  SDL_DestroyRenderer(gameRenderer);
  SDL_DestroyWindow(gameWindow);
  gameWindow = NULL;
  gameRenderer = NULL;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char* args[]){
  if(!init()){
    printf("Failed to initialize\n");
  }
  else{
    if(!loadMedia()){
      printf("Failed to load media!\n");
    }
    else{
      bool quit = false;
      bool start = false;
      bool lastPressedEsc = false; //you have to press esc then enter once the game started to quit
      SDL_Event e;
      
      while(!quit){
        while(SDL_PollEvent(&e)!=0){
          if( e.type == SDL_QUIT){
            quit = true;
          }
          else if(e.type == SDL_KEYDOWN){
            if(!start){//if still at start screen
              switch (e.key.keysym.sym) {
                case SDLK_ESCAPE://escape key to quit
                  quit = true;
                  break;
                case SDLK_RETURN://enter key to start
                  start = true;
                  break;
                default:
                  break;
              }
            }
            else{//game started (now this is used for player controls)
              switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                  lastPressedEsc = true;
                  break;
                  //all the other cases now update lastPressedEsc to true because it wasnt pressed 2x in a row
                case SDLK_RETURN:
                  if(lastPressedEsc)
                    quit = true;
                  break;
                default:
                  lastPressedEsc = false;
                  break;
              }
            }
          }
        }
        
        SDL_SetRenderDrawColor(gameRenderer, 0,0,0,0xFF);
        SDL_RenderClear(gameRenderer);

        gTextTexture.render(( SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2); 
        SDL_RenderPresent(gameRenderer);

      }

    }
  }
  close();

  return 0;
}
