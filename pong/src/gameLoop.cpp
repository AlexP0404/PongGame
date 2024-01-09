#include "gameLoop.hpp"

#include <stdexcept>

using std::string;

GameLoop::GameLoop() {
  bounceOffPaddle = false;
  p1Scored = false;
  p1Wins = false;
  singlePlayer = false;
  modeSelection = 1;
}

GameLoop::~GameLoop() = default;

bool GameLoop::init() {

  p1.setSize(PADDLE_WIDTH, PADDLE_HEIGHT);
  p2.setSize(PADDLE_WIDTH, PADDLE_HEIGHT);
  p1.setPos(PADDLE_OFFSET, SCREEN_HEIGHT / 2);
  p2.setPos(SCREEN_WIDTH - PADDLE_OFFSET, SCREEN_HEIGHT / 2);
  p1.setScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
  p2.setScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
  dot.setScreen(SCREEN_WIDTH, SCREEN_HEIGHT);
  dot.setSize(DOT_RADIUS, DOT_RADIUS);

  gameEngine.setScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  gameEngine.setPaddleSize(PADDLE_WIDTH, PADDLE_HEIGHT);

  if (!gameEngine.init() || !gameEngine.loadMedia()) {
    return false;
  }

  mainText = "Press Enter To Start!";
  difficultySelectText = "Easy\t[Medium]\tHard";
  modeSelectText = "[1 - Local Multiplayer]\t2 - Singleplayer";

  setStartText();
  setSpeedSelectText();
  setModeSelectText();

  return true;
}

void GameLoop::setScoreboardText() {
  if (!gameEngine.setTextTexture("scoreBoard", "mainFont",
                                 sb.getScoreString())) {
    throw std::runtime_error("Failed to set the scoreboard text!");
  }
  gameEngine.setTextureCoorCentered("scoreBoard", SCREEN_WIDTH / 2, 150);
}

void GameLoop::setStartText() { // sets the large MAIN  text to the current
  if (!gameEngine.setTextTexture("mainTextTexture", "mainFont", mainText)) {
    throw std::runtime_error("Failed to set the main text!");
  }
  gameEngine.setTextureCoorCentered("mainTextTexture", SCREEN_WIDTH / 2,
                                    SCREEN_HEIGHT / 2);
}

void GameLoop::setModeSelectText() {
  if (!gameEngine.setTextTexture("modeSelectPrompt", "escFont",
                                 modeSelectText)) {
    throw std::runtime_error("Failed to set the mode select text!");
  }
  gameEngine.setTextureCoorCentered("modeSelectPrompt", SCREEN_WIDTH / 2,
                                    SCREEN_HEIGHT / 2 + 150);
}

void GameLoop::setMode() {
  switch (modeSelection) {
  case 0:
  // online?
  case 1:
    singlePlayer = false;
    break;
  case 2:
  default:
    singlePlayer = true;
    break;
  }

  std::string choices[3] = {" ", "1 - Local Multiplayer", "2 - Singleplayer"};

  choices[modeSelection] = '[' + choices[modeSelection] + ']';
  modeSelectText =
      choices[1] + '\t' +
      choices[2]; // this can be changes to add online multiplayer in the future
  setModeSelectText();
}

void GameLoop::setSpeedSelectText() {
  if (!gameEngine.setTextTexture("difficultyPrompt", "escFont",
                                 difficultySelectText)) {
    throw std::runtime_error("Failed to set the difficulty select text!");
  }
  gameEngine.setTextureCoorCentered("difficultyPrompt", SCREEN_WIDTH / 2,
                                    SCREEN_HEIGHT / 2 + 100);
}

void GameLoop::setSpeed(int dir) {
  static int difficulty = 1; // difficulty can be 0,1,2 (easy,medium,hard)
  if (dir >= 1 && difficulty >= 2)
    return; // cant go right more
  if (dir <= -1 && difficulty <= 0)
    return;          // cant go left more
  difficulty += dir; //

  dot.setInitSpeed(difficulty); // changes the speed saved in dot
  p1.setSpeed(difficulty * 5);
  p2.setSpeed(difficulty *
              5); // adjust speed for paddles when changing difficulty

  std::string choices[3] = {"Easy", "Medium", "Hard"};

  choices[difficulty] =
      '[' + choices[difficulty] + ']'; // make the user selection obvious

  difficultySelectText =
      choices[0] + '\t' + choices[1] + '\t' + choices[2]; // update stringsream
  setSpeedSelectText();
}

bool GameLoop::collision() {
  if (m_CollisionPaddleTimer.ElapsedMillis() > COLLISION_CHECK_DELAY) {
    if (dot.getPosX() - DOT_RADIUS <= PADDLE_OFFSET + PADDLE_WIDTH + 2 &&
        dot.getPosX() + DOT_RADIUS >=
            PADDLE_OFFSET) { // check for paddle collisions
      if (dot.getPosY() + DOT_RADIUS >= p1.getPosY() - DOT_RADIUS &&
          dot.getPosY() - DOT_RADIUS <= p1.getPosY() + PADDLE_HEIGHT) {
        if (!bounceOffPaddle)
          m_CollisionPaddleTimer.Reset();
        bounceOffPaddle = true;
        return true;
      }
    }
    if (dot.getPosX() + DOT_RADIUS >= SCREEN_WIDTH - PADDLE_OFFSET - 2 &&
        dot.getPosX() - DOT_RADIUS <=
            SCREEN_WIDTH - PADDLE_OFFSET + PADDLE_WIDTH) {
      if (dot.getPosY() + DOT_RADIUS >= p2.getPosY() - DOT_RADIUS &&
          dot.getPosY() - DOT_RADIUS <= p2.getPosY() + PADDLE_HEIGHT) {
        if (!bounceOffPaddle)
          m_CollisionPaddleTimer.Reset();
        bounceOffPaddle = true;
        return true;
      }
    }
  }
  if (m_CollisionBorderTimer.ElapsedMillis() > COLLISION_CHECK_DELAY) {
    if (dot.getPosY() + DOT_RADIUS >= SCREEN_HEIGHT ||
        dot.getPosY() < DOT_RADIUS) { // bounce off top or bottom walls
      // std::cout << dot.getPosY() << " ";

      if (dot.getPosY() > SCREEN_HEIGHT / 2) // on the bottom half of the screen
        dot.setPos(dot.getPosX(), dot.getPosY() - 2);
      else
        dot.setPos(dot.getPosX(), dot.getPosY() + 2);

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

void GameLoop::countDown() {
  int countDownSeconds = 3;
  Timer countDownTimer;
  for (int i = countDownSeconds; i >= 0; i--) {
    mainText.clear();
    mainText = "Starting in " + std::to_string(i);
    setStartText();
    while (countDownTimer.ElapsedMillis() < 1000.0f)
      ;
    countDownTimer.Reset();
    gameEngine.renderTextures();
    gameEngine.renderScreen();
  }
}

void GameLoop::handleInputs() {
  static Timer inputDelayTimer;
  static Timer selectionDelay;
  if (inputDelayTimer.ElapsedMillis() < 8.0f)
    return;
  inputDelayTimer.Reset();
  gameEngine.inputs.updateKeyStates(); // refresh they keyboard buffer
  if (!start) {                        // if still at start screen
    if (gameEngine.inputs.IsKeyDown(KeyCode::Escape)) // escape key to quit
      quit = true;
    if (gameEngine.inputs.IsKeyDown(KeyCode::Left) &&
        selectionDelay.ElapsedMillis() > 200.0f) { // debounce arrows
      selectionDelay.Reset();
      setSpeed(-1); // change speed to slower
    }
    if (gameEngine.inputs.IsKeyDown(KeyCode::Right) &&
        selectionDelay.ElapsedMillis() > 200.0f) {
      selectionDelay.Reset();
      setSpeed(1);
    }
    if (gameEngine.inputs.IsKeyDown(KeyCode::D1) &&
        selectionDelay.ElapsedMillis() > 200.0f) {
      selectionDelay.Reset();
      modeSelection = 1;
      setMode();
    }
    if (gameEngine.inputs.IsKeyDown(KeyCode::D2) &&
        selectionDelay.ElapsedMillis() > 200.0f) {
      selectionDelay.Reset();
      modeSelection = 2;
      setMode();
    }

    if (gameEngine.inputs.IsKeyDown(KeyCode::Enter)) { // enter key to start
      gameEngine.eraseTextures(
          {"modeSelectPrompt", "difficultyPrompt"}); // countdown starts
      countDown();
      gameEngine.eraseTexture("mainTextTexture");
      start = true;
      if (singlePlayer) {
        ai1.setPaddle(&p2);
        ai2.setPaddle(&p1);
      }
      setScoreboardText();
      if (gameOver) {
        gameOver = false; // if we are resetting the game
        sb.reset();
        setScoreboardText();
      }
      dot.set();
    }
  } else { // game started (now this is used for player controls) start = true
    if (gameEngine.inputs.IsKeyDown(KeyCode::Escape)) {
      if (lastPressedEsc) {
        return;
      }
      lastPressedEsc = true;
      mainText = "Are You Sure You Want To Quit?";
      setStartText();
      difficultySelectText = "Press Enter To Confirm Or Spacebar To Resume.";
      setSpeedSelectText();
    }
    // all the other cases now update lastPressedEsc to true because it
    // wasnt pressed 2x in a row
    if (gameEngine.inputs.IsKeyDown(KeyCode::Enter) && lastPressedEsc)
      quit = true;
    if (!singlePlayer) {
      if (gameEngine.inputs.IsKeyDown(
              KeyCode::W)) // start of the player controls
        p1.move(true);
      if (gameEngine.inputs.IsKeyDown(KeyCode::S))
        p1.move(false);
      if (gameEngine.inputs.IsKeyDown(KeyCode::Up))
        p2.move(true);
      if (gameEngine.inputs.IsKeyDown(KeyCode::Down))
        p2.move(false);
    } else { // singleplayer
      int numAImoves = p2.genRandNum(5);
      for (int i = 0; i < numAImoves; i++) {
        ai1.movePaddle();
        ai2.movePaddle();
      }
    }
    if (gameEngine.inputs.IsKeyDown(KeyCode::Space) && lastPressedEsc) {
      lastPressedEsc = false;
      gameEngine.eraseTexture("difficultyPrompt");
      countDown();
      gameEngine.eraseTexture("mainTextTexture");
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
    quit = gameEngine.shouldQuit();
    while (m_GameTimer.ElapsedMillis() < GAME_LOOP_DELAY)
      ;
    m_GameTimer.Reset();
    handleInputs();

    if (gameOver) {
      gameEngine.clearScreen();
      start = false;
      mainText = "Player" + (p1Wins ? string(" 1 ") : string(" 2 ")) +
                 "Wins!!! Press Enter To Start Again!";
      setStartText();
      setSpeed();
      setMode();
    }

    gameEngine.renderTextures();
    if (start && !lastPressedEsc) {
      dot.move();
      if (collision()) { // this compares the positions of the dot and the walls
                         // and paddles and checks for a collision
        dot.bounce(bounceOffPaddle);
        gameEngine.playBounce();
        if (!bounceOffPaddle) {
          ai1.setDotBounceX(dot.getDirectionX(), dot.getPosX(),
                            dot.getPosY() <
                                SCREEN_HEIGHT /
                                    2); // if bounce off the top, dot Y pos = 0
          ai2.setDotBounceX(dot.getDirectionX(), dot.getPosX(),
                            dot.getPosY() < SCREEN_HEIGHT / 2);
        }
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
      gameEngine.drawNet();
      gameEngine.drawPaddles(p1.getPosX(), p1.getPosY(), p2.getPosX(),
                             p2.getPosY());
      gameEngine.drawDot(dot.getPosX(), dot.getPosY(), DOT_RADIUS);
    }
    gameEngine.renderScreen();
  }
}
