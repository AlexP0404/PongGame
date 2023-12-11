#include "gameLoop.hpp"
#include "Texture.hpp"
#include "dot.hpp"

GameLoop::GameLoop() {
  gameWindow = NULL;
  gameRenderer = NULL;
  mainFont = NULL;
  escFont = NULL;
  bounceOffPaddle = false;
  p1Scored = false;
  p1Wins = false;
  textColor = {0xFF, 0xFF, 0xFF, 0xFF};
}

GameLoop::~GameLoop(){
  TTF_CloseFont(mainFont);
  TTF_CloseFont(escFont);
  mainFont = NULL;
  escFont = NULL;

  Mix_FreeChunk(bounce);
  bounce = NULL;

  SDL_DestroyRenderer(gameRenderer);
  SDL_DestroyWindow(gameWindow);
  gameWindow = NULL;
  gameRenderer = NULL;

  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

bool GameLoop::init() {
  bool success = true;

  p1.setSize(PADDLE_WIDTH, PADDLE_HEIGHT);
  p2.setSize(PADDLE_WIDTH, PADDLE_HEIGHT);
  p1.setPos(PADDLE_OFFSET,SCREEN_HEIGHT / 2);
  p2.setPos(SCREEN_WIDTH - PADDLE_OFFSET,SCREEN_HEIGHT / 2);
  p1.setScreen(SCREEN_WIDTH,SCREEN_HEIGHT);
  p2.setScreen(SCREEN_WIDTH,SCREEN_HEIGHT);
  dot.setScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
  dot.setSize(DOT_WIDTH, DOT_HEIGHT);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }
    gameWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gameWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      gameRenderer = SDL_CreateRenderer(
          gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gameRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        success = false;
      } else {
        SDL_SetRenderDrawColor(gameRenderer, 0x00, 0x00, 0x00, 0xFF);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_image Error: %s\n",
                 IMG_GetError());
          success = false;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
          printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                 Mix_GetError());
          success = false;
        }
        if (TTF_Init() == -1) {
          printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                 TTF_GetError());
          success = false;
        }
      }
    }
  }
  m_GameTimer.Reset();
  return success;
}

bool GameLoop::loadMedia() {
  bool success = true;

  
  gameIcon = IMG_Load("dot.bmp");
  SDL_SetWindowIcon(gameWindow, gameIcon);

  escFont = TTF_OpenFont("lazy.ttf", 15);
  if (escFont == NULL) {
    printf("failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  } else {
    textures["escapePrompt"] = unique_ptr<Texture>
      (new Texture(*gameRenderer,0,0));//uses full parameter constructor
    if (!textures.at("escapePrompt")->loadFromRenderedText(
        "Press Escape to quit",textColor, escFont)) {
      printf("Failed to render text texture!\n");
      success = false;
    } else {

      mainFont = TTF_OpenFont("lazy.ttf", 30);
      mainText.str("");
      mainText << "Press Enter to start!";
      textures["startPrompt"] = unique_ptr<Texture>
        (new Texture(*gameRenderer,0,0));//uses constructor with no parameters
      setStartText();
      textures["speedPrompt"] = unique_ptr<Texture>(new Texture(*gameRenderer,0,0));
      textures["scoreBoard"] = unique_ptr<Texture>(new Texture(*gameRenderer,0,0));

      dot.setScreen(SCREEN_HEIGHT, SCREEN_WIDTH);
      bounce = Mix_LoadWAV("bounce.wav");
      if (bounce == NULL) {
        printf("Failed to load bounce sound effect! SDL_mixer Error: %s\n",
               Mix_GetError());
        success = false;
      }
    }
  }
  return success;
}

void GameLoop::setScoreboardText() {
  try{
    textures.at("scoreBoard")->loadFromRenderedText(sb.getScoreString().c_str(), textColor,
                                           mainFont);
    textures.at("scoreBoard")->setxCoor((SCREEN_WIDTH - textures.at("scoreBoard")->getWidth()) /
                               2);   // center
    textures.at("scoreBoard")->setyCoor(150); // more towards the top of the screen
  }
  catch(std::exception& e){
    textures["scoreBoard"] = unique_ptr<Texture>
      (new Texture(*gameRenderer,0,0));
    setScoreboardText();//this is a potential infinite loop lol
  }
}

void GameLoop::setStartText(){//sets the large MAIN  text to the current mainText string and adds the texture to the list of active textures
  try{
    textures.at("startPrompt")->loadFromRenderedText(mainText.str().c_str(), textColor, mainFont);
    textures.at("startPrompt")->setxCoor(( SCREEN_WIDTH - textures.at("startPrompt")->getWidth() ) / 2);
    textures.at("startPrompt")->setyCoor(( SCREEN_HEIGHT - textures.at("startPrompt")->getHeight() ) / 2);
  }
  catch(std::exception& e){
    textures["startPrompt"] = unique_ptr<Texture>
      (new Texture(*gameRenderer,0,0));
    setStartText();//this is a potential infinite loop lol
  }
}

void GameLoop::drawNet() {
  SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (int i = 0; i < SCREEN_HEIGHT; i += 4) {
    SDL_RenderDrawPoint(gameRenderer, SCREEN_WIDTH / 2, i);
  }
}

void GameLoop::drawPaddles() {
  p1Rect = {p1.getPosX(), p1.getPosY(), p1.getSizeX(), p1.getSizeY()}; 
  // initialize the first paddle in the middle left of the screen
  p2Rect = {p2.getPosX(), p2.getPosY(),p2.getSizeX(),p2.getSizeY()};
  SDL_RenderFillRect(gameRenderer, &p1Rect);
  SDL_RenderFillRect(gameRenderer, &p2Rect);
}

void GameLoop::drawDot() {
  dotRect = {dot.getPosX(), dot.getPosY(), dot.getSizeX(), dot.getSizeY()};
  SDL_RenderFillRect(gameRenderer, &dotRect);
}

void GameLoop::renderTextures() {
  SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 0xFF);
  SDL_RenderClear(gameRenderer);
  
  for(auto& [name,txtr] : textures){
  	txtr->render();
  }
}

bool GameLoop::collision(){
  if(dot.getPosX() <= PADDLE_OFFSET + PADDLE_WIDTH /*+ 2*/ && dot.getPosX() >= PADDLE_OFFSET){//check for paddle collisions
    if(dot.getPosY() >= p1.getPosY() - DOT_HEIGHT && dot.getPosY() <= p1.getPosY() + PADDLE_HEIGHT){
      if(!bounceOffPaddle)
	m_CollisionTimer.Reset();
      bounceOffPaddle = true;
      return true;
    }
  }
  if(dot.getPosX() >= SCREEN_WIDTH - PADDLE_OFFSET + 2 && dot.getPosX() <= SCREEN_WIDTH - PADDLE_OFFSET + PADDLE_WIDTH){
    if(dot.getPosY() >= p2.getPosY() - DOT_HEIGHT && dot.getPosY() <= p2.getPosY() + PADDLE_HEIGHT){
      if(!bounceOffPaddle)
        m_CollisionTimer.Reset();
      bounceOffPaddle = true;
      return true;
    }
  }

  if(dot.getPosY() + DOT_HEIGHT >= SCREEN_HEIGHT || dot.getPosY() < DOT_HEIGHT){//bounce off top or bottom walls
    //std::cout << dot.getPosY() << " ";

    if (dot.getPosY() > SCREEN_HEIGHT / 2) // on the bottom half of the screen
      dot.setPos(dot.getPosX(),dot.getPosY() - 2);
    else
      dot.setPos(dot.getPosX(),dot.getPosY() + 2);

    bounceOffPaddle = false;
    m_CollisionTimer.Reset();
    return true;
  }

  return false;
}

bool GameLoop::score() {
  if (dot.getPosX() <= 0) {
    p1Scored = false; // p2 scored on p1
    return true;
  }
  if (dot.getPosX() >= SCREEN_WIDTH) {
    p1Scored = true;
    return true;
  }
  return false;
}

void GameLoop::loop() {

  bool quit = false;
  bool start = false;
  bool gameOver = false;
  bool lastPressedEsc =
      false; // you have to press esc then enter once the game started to quit
  bool sbTextureLoaded = false;
  Timer delayBetweenRounds;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        if (!start) { // if still at start screen
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE://escape key to quit
              quit = true;
              break;
            case SDLK_RETURN://enter key to start
              start = true;
	            m_GameTimer.Reset();
              sbTextureLoaded = true;
              setScoreboardText();
              if(gameOver){
                gameOver = false;//if we are resetting the game
                sbTextureLoaded = false;
                sb.reset();
                setScoreboardText();
              }
              mainText.clear();
              mainText.str("");
              textures.erase("startPrompt");
              dot.set();
              break;
            default:
              break;
          }
        } else { // game started (now this is used for player controls) start = true
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              if (lastPressedEsc)
                break;
              lastPressedEsc = true;
              mainText << "Are you sure you want to quit? Press Enter if yes.";
              setStartText();
              mainText.clear();
              mainText.str("");
              break;
              // all the other cases now update lastPressedEsc to true because it
              // wasnt pressed 2x in a row
            case SDLK_RETURN:
              if (lastPressedEsc)
                quit = true;
              break;
            case SDLK_w: // start of the player controls
              p1.move(true);
              break;
            case SDLK_s:
              p1.move(false);
              break;
            case SDLK_UP:
              p2.move(true);
              break;
            case SDLK_DOWN:
              p2.move(false);
              break;
            default:
              if (lastPressedEsc) {
                lastPressedEsc = false;
              }
              break;
          }
        }
      }
    }
    if (gameOver) {
      SDL_RenderClear(gameRenderer);
      start = false;
      textures.erase("scoreBoard");
      if (p1Wins)
        mainText << "Player 1 Wins!!! Press Enter to start again!";
      else
        mainText << "Player 2 Wins!!! Press Enter to start again!";
      setStartText();
      mainText.clear();
      mainText.str("");
    }

    if (start && !sbTextureLoaded) {
      sbTextureLoaded = true;
    }

    renderTextures();
    if (start && !lastPressedEsc) {
      dot.move();
      if(m_CollisionTimer.ElapsedMillis() > 80.0f && collision()){//this compares the positions of the dot and the walls and paddles and checks for a collision
        //std::cout << dot.getPosX() << ',' << dot.getPosY() << " ";
        dot.bounce(bounceOffPaddle);
        Mix_PlayChannel(-1, bounce, 0);
        //m_CollisionTimer.Reset();
      }
      if (score()) { // this needs to compare the position of the dot and the
                     // paddles and check if it scored. if it did score, figure
                     // out which player scored and increment the score then
                     // call scoreboard.gameOver() to check if the game is over
        if (p1Scored)
          sb.incPlayer1();
        else
          sb.incPlayer2();

        setScoreboardText();
        sbTextureLoaded = false;
        gameOver = sb.gameOver();
        if (p1Scored && gameOver) {
          p1Wins = true;
        }
        dot.set();
        while(delayBetweenRounds.ElapsedMillis() < 500);//give user a tiny bit of time before starting again
      }
      drawNet();
      drawPaddles();
      drawDot();
    }
    SDL_RenderPresent(gameRenderer);
  }
}
