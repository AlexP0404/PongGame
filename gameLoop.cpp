#include "gameLoop.hpp"
#include "dot.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <string>

GameLoop::GameLoop() {
  gameWindow = nullptr;
  gameRenderer = nullptr;
  mainFont = nullptr;
  escFont = nullptr;
  bounce = nullptr;
  bounceOffPaddle = false;
  p1Scored = false;
  p1Wins = false;
  singlePlayer = false;
  modeSelection = 1;
  textColor = {0xFF, 0xFF, 0xFF, 0xFF};
}

GameLoop::~GameLoop(){
  TTF_CloseFont(mainFont);
  TTF_CloseFont(escFont);
  mainFont = nullptr;
  escFont = nullptr;

  Mix_FreeChunk(bounce);
  bounce = nullptr;

  SDL_DestroyRenderer(gameRenderer);
  SDL_DestroyWindow(gameWindow);
  gameWindow = nullptr;
  gameRenderer = nullptr;

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
  dot.setSize(DOT_RADIUS, DOT_RADIUS);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }
    gameWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                  SCREEN_HEIGHT,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS);
    if (gameWindow == nullptr) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      
      gameRenderer = SDL_CreateRenderer(
          gameWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gameRenderer == nullptr) {
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
  if (escFont == nullptr) {
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
      mainText = "Press Enter to start!";
      textures["startPrompt"] = unique_ptr<Texture>
        (new Texture(*gameRenderer,0,0));//uses constructor with no parameters
      setStartText();
      
      difficultySelectText = "Easy\t[Medium]\tHard";
      textures["difficultyPrompt"] = unique_ptr<Texture>(new Texture(*gameRenderer,0,0));
      setSpeedSelectText();
      
      modeSelectText = "[1 - Local Multiplayer]\t2 - Singleplayer"; 
      textures["modeSelectPrompt"] = unique_ptr<Texture>(new Texture(*gameRenderer,0,0));
      setModeSelectText();

      textures["scoreBoard"] = unique_ptr<Texture>(new Texture(*gameRenderer,0,0));

      bounce = Mix_LoadWAV("bounce.wav");
      if (bounce == nullptr) {
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
    textures.at("startPrompt")->loadFromRenderedText(mainText.c_str(), textColor, mainFont);
    textures.at("startPrompt")->setxCoor(( SCREEN_WIDTH - textures.at("startPrompt")->getWidth() ) / 2);
    textures.at("startPrompt")->setyCoor(( SCREEN_HEIGHT - textures.at("startPrompt")->getHeight() ) / 2);
  }
  catch(std::exception& e){
    textures["startPrompt"] = unique_ptr<Texture>
      (new Texture(*gameRenderer,0,0));
    setStartText();//this is a potential infinite loop lol
  }
}

void GameLoop::setModeSelectText(){
  try{
    textures.at("modeSelectPrompt")->loadFromRenderedText(modeSelectText.c_str(), textColor, escFont);
    textures.at("modeSelectPrompt")->setxCoor(( SCREEN_WIDTH - textures.at("modeSelectPrompt")->getWidth() ) / 2);
    textures.at("modeSelectPrompt")->setyCoor(textures.at("difficultyPrompt")->getyCoor()+textures.at("difficultyPrompt")->getHeight()+5);
  }
  catch(std::exception& e){
    setSpeedSelectText();//make sure this is initialized beforehand 
    textures["modeSelectPrompt"] = unique_ptr<Texture>
      (new Texture(*gameRenderer,0,0));
    setModeSelectText();//this is a potential infinite loop lol
  }
}

void GameLoop::setMode(){
  switch(modeSelection){
    case 0:
    //online?
    case 1:
      singlePlayer = false;
      break;
    case 2:
    default:
      singlePlayer = true;
      break;
  }

  std::string choices[3] = { " ", "1 - Local Multiplayer", "2 - Singleplayer"};

  choices[modeSelection] = '[' + choices[modeSelection] + ']';
  modeSelectText = choices[1] + '\t' + choices[2];//this can be changes to add online multiplayer in the future
  setModeSelectText();
}

void GameLoop::setSpeedSelectText(){
  try{
    textures.at("difficultyPrompt")->loadFromRenderedText(difficultySelectText.c_str(), textColor, escFont);
    textures.at("difficultyPrompt")->setxCoor((SCREEN_WIDTH-textures.at("difficultyPrompt")->getWidth()) / 2);//middle of screen
    textures.at("difficultyPrompt")->setyCoor(textures.at("startPrompt")->getyCoor() + textures.at("startPrompt")->getHeight()+5);//put right below start prompt
  }
  catch(std::exception& e){
    setStartText();//make sure start text is initialized so it can use its measurements
    textures["difficultyPrompt"] = unique_ptr<Texture>(new Texture(*gameRenderer,0,0));
    setSpeedSelectText();
  }
}

void GameLoop::setSpeed(int dir){
  static int difficulty = 1;//difficulty can be 0,1,2 (easy,medium,hard)
  if(dir >= 1  && difficulty >= 2) return;//cant go right more
  if(dir <= -1 && difficulty <= 0) return;//cant go left more
  difficulty += dir;//

  dot.setInitSpeed(difficulty);//changes the speed saved in dot
  p1.setSpeed(difficulty*5);
  p2.setSpeed(difficulty*5);//adjust speed for paddles when changing difficulty

  std::string choices[3] = {"Easy", "Medium", "Hard"};

  choices[difficulty] = '[' + choices[difficulty] + ']';//make the user selection obvious
  
  difficultySelectText = choices[0]+'\t'+choices[1]+'\t'+choices[2];//update stringsream
  setSpeedSelectText();
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

void GameLoop::drawDot(){
  //https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/_s_d_l2__gfx_primitives_8c_source.html#l01457
  //used SDL_gfx filledCircle but didn't want to include the whole library just for the circle so I edited it to use it here
  short cx = 0;
  short cy = DOT_RADIUS;
  short ocx = (short) 0xffff;
  short ocy = (short) 0xffff;
  short df = 1 - DOT_RADIUS;
  short d_e = 3;
  short d_se = -2 * DOT_RADIUS + 5;
  short xpcx, xmcx, xpcy, xmcy;
  short ypcy, ymcy, ypcx, ymcx;

   do {
    xpcx = dot.getPosX() + cx;
    xmcx = dot.getPosX() - cx;
    xpcy = dot.getPosX() + cy;
    xmcy = dot.getPosX() - cy;
    if (ocy != cy) {
      if (cy > 0) {
        ypcy = dot.getPosY() + cy;
        ymcy = dot.getPosY() - cy;
        SDL_RenderDrawLine(gameRenderer,xmcx,ypcy,xpcx,ypcy);
        SDL_RenderDrawLine(gameRenderer,xmcx,ymcy,xpcx,ymcy);
      }
      else{
        SDL_RenderDrawLine(gameRenderer, xmcx, dot.getPosY(), xpcx, dot.getPosY());
      }
      ocy = cy;
    }
    if(ocx != cx){
      if (cx != cy) {
        if (cx > 0) {
          ypcx = dot.getPosY() + cx;
          ymcx = dot.getPosY() - cx;
          SDL_RenderDrawLine(gameRenderer, xmcy,ymcx, xpcy, ymcx);
          SDL_RenderDrawLine(gameRenderer, xmcy,ypcx ,xpcy, ypcx);
        } else {
          SDL_RenderDrawLine(gameRenderer, xmcy,dot.getPosY(), xpcy, dot.getPosY());
        }
      }
      ocx = cx;
    }
    if(df<0){
      df += d_e;
      d_e += 2;
      d_se += 2;
    }
    else{
      df += d_se;
      d_e += 2;
      d_se += 4;
      cy--;
    }
    cx++;
  }while(cx <= cy);

}

void GameLoop::renderTextures() {
  SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 0xFF);
  SDL_RenderClear(gameRenderer);
  
  for(auto& [name,txtr] : textures){
  	txtr->render();
  }
}

bool GameLoop::collision(){
  if(m_CollisionPaddleTimer.ElapsedMillis() > COLLISION_CHECK_DELAY){
    if(dot.getPosX() - DOT_RADIUS <= PADDLE_OFFSET + PADDLE_WIDTH + 2 && dot.getPosX() + DOT_RADIUS >= PADDLE_OFFSET){//check for paddle collisions
      if(dot.getPosY() + DOT_RADIUS >= p1.getPosY() - DOT_RADIUS && dot.getPosY() - DOT_RADIUS <= p1.getPosY() + PADDLE_HEIGHT){
        if(!bounceOffPaddle)
          m_CollisionPaddleTimer.Reset();
        bounceOffPaddle = true;
        return true;
      }
    }
    if(dot.getPosX() + DOT_RADIUS >= SCREEN_WIDTH - PADDLE_OFFSET - 2 && dot.getPosX() - DOT_RADIUS <= SCREEN_WIDTH - PADDLE_OFFSET + PADDLE_WIDTH){
      if(dot.getPosY() + DOT_RADIUS >= p2.getPosY() - DOT_RADIUS && dot.getPosY() - DOT_RADIUS <= p2.getPosY() + PADDLE_HEIGHT){
        if(!bounceOffPaddle)
          m_CollisionPaddleTimer.Reset();
        bounceOffPaddle = true;
        return true;
      }
    }
  }
  if(m_CollisionBorderTimer.ElapsedMillis() > COLLISION_CHECK_DELAY){
    if(dot.getPosY() + DOT_RADIUS >= SCREEN_HEIGHT || dot.getPosY() < DOT_RADIUS){//bounce off top or bottom walls
      //std::cout << dot.getPosY() << " ";

      if (dot.getPosY() > SCREEN_HEIGHT / 2) // on the bottom half of the screen
        dot.setPos(dot.getPosX(),dot.getPosY() - 2);
      else
        dot.setPos(dot.getPosX(),dot.getPosY() + 2);

      bounceOffPaddle = false;
      m_CollisionBorderTimer.Reset();
      return true;
    }
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

void GameLoop::countDown(){
  int countDownSeconds = 3;
  Timer countDownTimer;
  for(int i = countDownSeconds; i >= 0; i--){
    mainText.clear();
    mainText = "Starting in " + std::to_string(i);
    setStartText();
    while(countDownTimer.ElapsedMillis() < 1000.0f);
    countDownTimer.Reset();
    renderTextures();
    SDL_RenderPresent(gameRenderer);
  }
}

void GameLoop::handleInputs(){
  static Timer inputDelayTimer;
  static Timer selectionDelay;
  if(inputDelayTimer.ElapsedMillis() < 8.0f) return;
  inputDelayTimer.Reset();
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      quit = true;
    } 
  }
  const Uint8 *keyStates = SDL_GetKeyboardState(nullptr);
  if (!start) { // if still at start screen
    if(keyStates[SDL_SCANCODE_ESCAPE])//escape key to quit
      quit = true;
    if(keyStates[SDL_SCANCODE_LEFT] && selectionDelay.ElapsedMillis() > 200.0f){//debounce arrows
      selectionDelay.Reset();
      setSpeed(-1);//change speed to slower
    } 
    if(keyStates[SDL_SCANCODE_RIGHT] && selectionDelay.ElapsedMillis() > 200.0f){
      selectionDelay.Reset();
      setSpeed(1);
    }
    if(keyStates[SDL_SCANCODE_1] && selectionDelay.ElapsedMillis() > 200.0f){
      selectionDelay.Reset();
      modeSelection = 1;
      setMode();
    }
    if(keyStates[SDL_SCANCODE_2] && selectionDelay.ElapsedMillis() > 200.0f){
      selectionDelay.Reset();
      modeSelection = 2;
      setMode();
    }

    if(keyStates[SDL_SCANCODE_RETURN]){//enter key to start
      textures.erase("modeSelectPrompt");
      textures.erase("difficultyPrompt");//remove difficultyPrompt before countdown starts
      countDown();
      textures.erase("startPrompt");
      start = true;
      if(singlePlayer)
        ai1.setPaddle(&p2);
      setScoreboardText();
      if(gameOver){
        gameOver = false;//if we are resetting the game
        sb.reset();
        setScoreboardText();
      }
      dot.set();
    }
  } else { // game started (now this is used for player controls) start = true
      if(keyStates[SDL_SCANCODE_ESCAPE]){
        if (lastPressedEsc){
          return;
        }
        lastPressedEsc = true;
        mainText = "Are you sure you want to quit?";
        setStartText();
        difficultySelectText = "Press Enter to confirm or spacebar to resume.";
        setSpeedSelectText();
      }
        // all the other cases now update lastPressedEsc to true because it
        // wasnt pressed 2x in a row
      if(keyStates[SDL_SCANCODE_RETURN] && lastPressedEsc)
          quit = true;
      if(keyStates[SDL_SCANCODE_W]) // start of the player controls
        p1.move(true);
      if(keyStates[SDL_SCANCODE_S])
        p1.move(false);
      if(!singlePlayer){
        if(keyStates[SDL_SCANCODE_UP])
          p2.move(true);
        if(keyStates[SDL_SCANCODE_DOWN])
          p2.move(false);
      }
      else{//singleplayer
        int numAImoves = p2.genRandNum(5); 
        for(int i = 0; i < numAImoves; i++)
          ai1.movePaddle();
      }
      if(keyStates[SDL_SCANCODE_SPACE] && lastPressedEsc){
          lastPressedEsc = false;
          textures.erase("difficultyPrompt");
          countDown();
          textures.erase("startPrompt");//remove the escape text
      }
    }
}

void GameLoop::loop() {
  quit = false;
  start = false;
  gameOver = false;
  lastPressedEsc = false;
    // you have to press esc then enter once the game started to quit
  Timer delayBetweenRounds;
  while (!quit) {
    while(m_GameTimer.ElapsedMillis() < GAME_LOOP_DELAY);
    m_GameTimer.Reset();
    handleInputs();
    
    if (gameOver) {
      SDL_RenderClear(gameRenderer);
      start = false;
      textures.erase("scoreBoard");
      mainText = "Player" + (p1Wins ? string(" 1 ") : string(" 2 ")) + "Wins!!! Press Enter to start again!";
      setStartText();
      setSpeed();
      setMode();
    }

    renderTextures();
    if (start && !lastPressedEsc) {
      dot.move();
      if(collision()){//this compares the positions of the dot and the walls and paddles and checks for a collision
        dot.bounce(bounceOffPaddle);
        Mix_PlayChannel(-1, bounce, 0);
        if(!bounceOffPaddle){
          ai1.setDotBounceX(dot.getDirectionX(),dot.getPosX(),dot.getPosY() < SCREEN_HEIGHT/2);//if bounce off the top, dot Y pos = 0
        }
        // std::cout << "COLLISION: " << ((bounceOffPaddle) ? "PADDLE\n" : "BORDER\n");
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
        gameOver = sb.gameOver();
        if (p1Scored && gameOver) {
          p1Wins = true;
        }
        dot.set();
      }
      drawNet();
      drawPaddles();
      drawDot();
    }
    SDL_RenderPresent(gameRenderer);
  }
}
