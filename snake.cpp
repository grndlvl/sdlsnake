#include "snake.h"

Snake::Snake() {
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  width = 800;
  height = 400;
  int action = 0;
  backToMenu = 9;

  screen = SDL_SetVideoMode(width, height, 32, SDL_SWSURFACE);
  font = TTF_OpenFont("resources/kasnake.ttf", 30);

  while (action != exitGame) {
    action = showMenu();

    switch (action) {
      case 0:
        action = start();
        break;
      case 1:
        break;
    }
  }
}

int Snake::start() {
  initArea();
  initSnake();
  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          return exitGame;
        case SDL_KEYDOWN:
          return backToMenu;
      }
    }
    movesnake();
    flipScreen();
  }
}

void Snake::initSnake() {
  Uint32 color = SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);

  for (int i = 0; i < 5; i++)
    snake.push_back(snakeSegment(40+i, 10));

  for (int i = 0; i < snake.size(); i++)
    drawRect(snake[i].x*10, snake[i].y*10, color);
}

void Snake::moveSnake() {
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    if(event.type==SDL_KEYDOWN) {
      switch(event.key.keysym.sym) {
        case SDLK_LEFT:
          if(direction!='r')
            direction='l';
            break;
        case SDLK_UP:
          if(direction='d')
            direction='u';
            break;
        case SDLK_DOWN:
          if(direction!='u')
            direction='d';
            break;
        case SDLK_RIGHT:
          if(direction!='l')
            direction='r';
          break;
        case SDLK_ESCAPE:
          direction='q';
          break;
      }
    }
    else if(event.type==SDL_QUIT)
      direction='q';
  }

  if(!get) {
    drawRect(snake[snake.size()-1].x*10,snake[snake.size()-1].y*10,SDL_MapRGB(screen->format,0x00,0x00,0x00));
    snake.pop_back();
  }

  if(direction=='l')
    snake.insert(snake.begin(),snakepart(snake[0].x-1,snake[0].y));
  else if(direction=='r')
    snake.insert(snake.begin(),snakepart(snake[0].x+1,snake[0].y));
  else if(direction=='u')
    snake.insert(snake.begin(),snakepart(snake[0].x,snake[0].y-1));
  else if(direction=='d')
    snake.insert(snake.begin(),snakepart(snake[0].x,snake[0].y+1));

  drawRect(snake[0].x*10,snake[0].y*10,SDL_MapRGB(screen->format,0x00,0xff,0x00));
}

void Snake::initArea() {
  SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

  Uint32 color = SDL_MapRGB(screen->format,0xff,0x00,0x00);
  drawRect(0, 0, color, this->width, 10);
  drawRect(0, 0, color, 10, this->height-20);
  drawRect(0, height-30, color, this->width, 10);
  drawRect(width-10, 0, color, 10, this->height-20);

}

void Snake::drawRect(int x, int y, Uint32 color, int w, int h) {
  SDL_Rect tmp = {x, y, w, h};
  SDL_FillRect(screen, &tmp, color);
}

std::vector<snakeSegment> Snake::getSnake() {
  return snake;
}

void Snake::flipScreen() {
  time = SDL_GetTicks();

  SDL_Flip(screen);
  if(1000/30 > (SDL_GetTicks()-time))
    SDL_Delay(1000/30 - (SDL_GetTicks()-time));
}

int Snake::showMenu() {
  const int NUMITEMS = 3;

  SDL_Color colorEnabled = {255, 255, 255};
  SDL_Color colorDisabled = {255, 0, 0};

  SnakeMenuItem menuInfo[NUMITEMS];
  menuInfo[0].set("Start");
  menuInfo[1].set("High Scores");
  menuInfo[2].set("Exit");
  exitGame = 2;

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
