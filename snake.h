#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <vector>

struct snakeSegment{
  int x, y;
  snakeSegment();
  snakeSegment(int x, int y);
};

class Snake {
  SDL_Surface* screen;
  TTF_Font* font, *scoreFont;
  std::vector<snakeSegment> snake;
  snakeSegment food;

  int exitGame, backToMenu, gameOver, width, height, delay, score;
  Uint32 time, snakeColor, backgroundColor;
  char direction;
  bool ate;

  public:
    Snake();
    ~Snake();
    int showMenu();
    void flipScreen();
    void initSnake();
    void initArea();
    int showGameOver();
    void putFood();
    bool collision();
    void start();
    int startGame();
    void drawRect(int x, int y, Uint32 color, int w = 10, int h = 10);
    int moveSnake();
};

class SnakeMenuItem {
  int state;

  public:
    int x, y, w, h;
    const char* label;

    SnakeMenuItem();
    void set(const char* label);
    void setWidthHeight(int w, int h);
    void setPosition(int x, int y);
    void setState(bool state);
    bool getState();
    bool checkBounds(int posX, int posY);
};
