#include "snake.h"

/**
 * Snake constructor.
 */
Snake::Snake() {
  // Initialize SDL and TTF.
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  // Dimensions of the window.
  width = 800;
  height = 400;

  // Menu option variables to be used throughtout game loop.
  backToMenu = 9;
  gameOver = 8;

  // The snake speed delay.(Decrements in game mode)
  delay = 110;

  // SDL surface used throught game.
  screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);

  // Reusable fonts.
  font = TTF_OpenFont("resources/kasnake.ttf", 60);
  scoreFont = TTF_OpenFont("resources/kasnake.ttf", 20);

  // Reusable colors.
  snakeColor = SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);
  backgroundColor = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
}

/**
 * Deconstructor. Deinitialize everything.
 */
Snake::~Snake() {
  // Close all fonts opened.
  TTF_CloseFont(font);
  TTF_CloseFont(scoreFont);

  // Quit SDL and TTF.
  SDL_Quit();
  TTF_Quit();
}

/**
 * Start the main game loop.
 */
void Snake::start() {
  // Start on menu.
  int action = backToMenu;

  // As long as we are not ready to exit the game loop through available actions.
  while (action != exitGame) {
    switch (action) {
      case 0:
        action = startGame();
        break;
      case 8:
        action = showGameOver();
        break;
      default:
        action = showMenu();
        break;
    }
  }
}

/**
 * Start a new game.
 */
int Snake::startGame() {
  // Action to determine which action should be taken next.
  int action = 0;

  // Initialize for beginning of a new game.
  direction = 'l';
  ate = false;
  score = 0;

  // Create the game board.
  initArea();
  // Create the character.
  initSnake();

  // Secondary game loop to allow the player to play the game.
  while (true) {
    // As long as the user didn't collide with themselves or the barrier
    // keep on truckin'.
    if (collision()) {
      return gameOver;
      break;
    }

    // As long as the user didn't exit the game keep on truckin'.
    if ((action = moveSnake())) {
      // The user hit escape or exit.
      return action;
    }

    // Redraw the screen.
    flipScreen();
    // Adjust the snakes speed per the current delay.
    SDL_Delay(delay);
  }

  return 0;
}

/**
 * Show the game over screen.
 */
int Snake::showGameOver() {
  SDL_Color color = {255, 0, 0};
  SDL_Surface* gameOverText = TTF_RenderText_Solid(font, "Game Over", color);

  SDL_Rect gameOverContainer;
  gameOverContainer.x = screen->clip_rect.w/2 - gameOverText->clip_rect.w/2;
  gameOverContainer.y = screen->clip_rect.h/2 - gameOverText->clip_rect.h;
  drawRect(gameOverContainer.x, gameOverContainer.y, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00), gameOverText->clip_rect.w, gameOverText->clip_rect.h);

  SDL_BlitSurface(gameOverText, NULL, screen, &gameOverContainer);
  SDL_FreeSurface(gameOverText);

  flipScreen();
  SDL_Delay(1200);

  return backToMenu;
}

/**
 * Initialize the snake character to the default length and starting area.
 */
void Snake::initSnake() {
  snake.clear();

  for (int i = 0; i < 5; i++)
    snake.push_back(snakeSegment(40+i, 10));

  for (int i = 0; i < (int) snake.size(); i++)
    drawRect(snake[i].x*10, snake[i].y*10, snakeColor);
}

void Snake::putFood() {
  while (true) {
    int tmpx = rand()%width/10+1;
    int tmpy = rand()%height/10+1;

    for (int i = 0; i < (int) snake.size(); i++)
      if (snake[i].x == tmpx && snake[i].y == tmpy)
        continue;
      if (tmpx >= width/10-2 || tmpy >= height/10/3)
        continue;
      food.x = tmpx;
      food.y = tmpy;
      break;
  }

  drawRect(food.x*10, food.y*10, SDL_MapRGB(screen->format, 0xff, 0x00, 0xff));
}

/**
 * Move the snake in accordance to the input of the user.
 */
int Snake::moveSnake() {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT)
      return exitGame;
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_LEFT:
          if (direction != 'r')
            direction = 'l';
            break;
        case SDLK_UP:
          if (direction != 'd')
            direction = 'u';
            break;
        case SDLK_DOWN:
          if (direction != 'u')
            direction = 'd';
            break;
        case SDLK_RIGHT:
          if(direction != 'l')
            direction = 'r';
          break;
        case SDLK_ESCAPE:
          return backToMenu;
          break;
        default:
          break;
      }
    }
  }

  // If the snake didn't eat any food then remove the last part of the tail so
  // he won't continue to get bigger.
  if (!ate) {
    // Replace the tail with a black rectangle.
    drawRect(snake[snake.size()-1].x*10, snake[snake.size()-1].y*10, backgroundColor);
    // Remove the area from the snake.
    snake.pop_back();
  }

  // Add new segment based on the snakes direction to similate movement.
  if (direction == 'l')
    snake.insert(snake.begin(), snakeSegment(snake[0].x-1, snake[0].y));
  else if (direction == 'r')
    snake.insert(snake.begin(), snakeSegment(snake[0].x+1, snake[0].y));
  else if (direction == 'u')
    snake.insert(snake.begin(), snakeSegment(snake[0].x, snake[0].y-1));
  else if (direction == 'd')
    snake.insert(snake.begin(), snakeSegment(snake[0].x, snake[0].y+1));

  // Draw the new segment of the snake.
  drawRect(snake[0].x*10, snake[0].y*10, snakeColor);

  // Everything is going well for the snake so let the secondary game loop
  // know everything is a OK.
  return 0;
}

/**
 * Check if the snake collided with the barriers, itself, or food.
 */
bool Snake::collision() {
  // Check if the snake collided with the barrier.
  if (snake[0].x == 0 || snake[0].x == width/10-1)
    return true;
  if (snake[0].y == 0 || snake[0].y == height/10-3)
    return true;

  // Check if the snake collided with itself.
  for (int i = 2; i < (int) snake.size(); i++)
    if (snake[0].x == snake[i].x && snake[i].y == snake[0].y)
      return true;

  // Check if the snake collided with food.
  if (snake[0].x == food.x && snake[0].y == food.y) {
    // We need to tell other parts that we feed so that the snake will gain a
    // segment.
    ate = true;

    // Increase the users score.
    score += 10;

    // Create a string to be used to display the score.
    char scoreString[5];

    // Convert score to string for rendered points.
    sprintf(scoreString, "%d", score);

    // Create score text.
    SDL_Color color = {255, 255, 255};
    SDL_Surface* scoreText = TTF_RenderText_Solid(scoreFont, scoreString, color);

    // Create a score container.
    SDL_Rect scoreContainer = {0, 380};
    // Draw rectangle for the container.
    drawRect(0, 380, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00), 100, 20);
    // Add score to screen.
    SDL_BlitSurface(scoreText, NULL, screen, &scoreContainer);
    // Free our text.
    SDL_FreeSurface(scoreText);

    // When the user scores 100 points decrease the delay to increase the snake's
    // speed.
    if ((score % 100) == 0 && delay > 0)
      delay -= 10;

    // Add a new piece of food.
    putFood();
  }
  // The snake didn't get any food :(
  else {
    ate = false;
  }

  return false;
}

/**
 * Create the game board.
 */
void Snake::initArea() {
  // Fill the screen with the background color.
  SDL_FillRect(screen, &screen->clip_rect, backgroundColor);

  // Add barriers.
  Uint32 color = SDL_MapRGB(screen->format, 0xff, 0x00, 0x00);
  drawRect(0, 0, color, this->width, 10);
  drawRect(0, 0, color, 10, this->height-20);
  drawRect(0, height-30, color, this->width, 10);
  drawRect(width-10, 0, color, 10, this->height-20);

  // Put the food out.
  putFood();
}

/**
 * Helper method to draw rectangles.
 */
void Snake::drawRect(int x, int y, Uint32 color, int w, int h) {
  SDL_Rect tmp = {x, y, w, h};
  SDL_FillRect(screen, &tmp, color);
}

/**
 * Display the screen buffer to the user.
 */
void Snake::flipScreen() {
  // Show the screen buffer to the user.
  SDL_Flip(screen);

  // Regulate the FPS.
  time = SDL_GetTicks();
  if(1000/150 > (SDL_GetTicks()-time))
    SDL_Delay(1000/150 - (SDL_GetTicks()-time));
}

/**
 * Display the menu to the user.
 */
int Snake::showMenu() {
  // The number of menu items we have.
  const int NUMITEMS = 3;

  // The color when the menu item is enabled.
  SDL_Color colorEnabled = {255, 255, 255};
  // The color when the menu item is disabled.
  SDL_Color colorDisabled = {255, 0, 0};

  // Create an array of menu items.
  SnakeMenuItem menuInfo[NUMITEMS];
  // Set the labels for the menu items.
  menuInfo[0].set("Start");
  menuInfo[1].set("High Scores");
  menuInfo[2].set("Exit");
  // The option used throughout to exit the game.
  exitGame = 2;

  // The current active menu item.
  int activeMenuItem;

  SDL_Surface* menuItems[NUMITEMS];
  SDL_Rect menuPositions[NUMITEMS];

  for (int i = 0; i < NUMITEMS; i++) {
    menuItems[i] = TTF_RenderText_Solid(font, menuInfo[i].label, colorDisabled);
    menuInfo[i].w = menuItems[i]->clip_rect.w;
    menuInfo[i].h = menuItems[i]->clip_rect.h;

    menuPositions[i].x = screen->clip_rect.w/2 - menuInfo[i].w/2;
    menuPositions[i].y = screen->clip_rect.h/NUMITEMS - menuInfo[i].h/NUMITEMS + i*menuInfo[i].h;

    menuInfo[i].setPosition(menuPositions[i].x, menuPositions[i].y);
  }

  SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  SDL_Event event;
  while (true) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          for (int i = 0; i < NUMITEMS; i++)
            SDL_FreeSurface(menuItems[i]);
            return exitGame;
          break;
        case SDL_MOUSEMOTION:
          for (int i = 0; i < NUMITEMS; i++)
            if (menuInfo[i].checkBounds(event.motion.x, event.motion.y)) {
              if (menuInfo[i].getState() == 0) {
                menuInfo[i].setState(1);
                SDL_FreeSurface(menuItems[i]);
                menuItems[i] = TTF_RenderText_Solid(font, menuInfo[i].label, colorEnabled);
              }
            }
            else {
              if (menuInfo[i].getState() == 1) {
                menuInfo[i].setState(0);
                SDL_FreeSurface(menuItems[i]);
                menuItems[i] = TTF_RenderText_Solid(font, menuInfo[i].label, colorDisabled);
              }
            }
          break;
        case SDL_MOUSEBUTTONDOWN:
          for (int i = 0; i < NUMITEMS; i++) {
            if (menuInfo[i].checkBounds(event.motion.x, event.motion.y)) {
              activeMenuItem = i;
            }
            SDL_FreeSurface(menuItems[i]);
          }
          return activeMenuItem;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            for (int i = 0; i < NUMITEMS; i++) {
              SDL_FreeSurface(menuItems[i]);
            }
            return exitGame;
          }

      }
    }

    for(int i = 0; i < NUMITEMS; i++) {
      SDL_BlitSurface(menuItems[i], NULL, screen, &menuPositions[i]);
    }

    flipScreen();
  }
}

SnakeMenuItem::SnakeMenuItem() {}

void SnakeMenuItem::set(const char* label) {
  this->label = label;
  this->state = 0;
}

void SnakeMenuItem::setWidthHeight(int w, int h) {
  this->w = w;
  this->h = h;
}

void SnakeMenuItem::setPosition(int x, int y) {
  this->x = x;
  this->y = y;
}

void SnakeMenuItem::setState(bool state) {
  this->state = state;
}

bool SnakeMenuItem::getState() {
  return state;
}

bool SnakeMenuItem::checkBounds(int posX, int posY) {
  if (posX >= x && posX <= x+w)
    if (posY >=y && posY <= y+h)
      return true;

  return false;
}

snakeSegment::snakeSegment() {
  this->x = 0;
  this->y = 0;
}
snakeSegment::snakeSegment(int x, int y) {
  this->x = x;
  this->y = y;
}
