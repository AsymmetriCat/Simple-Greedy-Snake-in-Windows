#ifndef SNAKE_H_
#define SNAKE_H_
#include <stdbool.h>
#include <windows.h>

#define WIDTH 16
#define HEIGHT 16
#define MEDIUM "(medium)"
#define HARD "(hard)"
#define EASY "(easy)"
#define MENU_COLOR FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define EASY_MODE FOREGROUND_BLUE
#define MEDIUM_MODE FOREGROUND_RED | FOREGROUND_GREEN
#define HARD_MODE FOREGROUND_RED | FOREGROUND_INTENSITY
#define DEFAULT_COLOR FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

enum Direction {
  kStop,
  kUp,
  kDown,
  kLeft,
  kRight
};

struct Body {
  int x;
  int y;
  enum Direction dir;
  struct Body *prev;
  struct Body *next;
};

struct Snake {
  struct Body *head;
  struct Body *tail;
};

struct Food {
  int x;
  int y;
};

typedef struct Body Body_t;
typedef struct Snake Snake_t;
typedef struct Food Food_t;

/*Prototypes*/
void SnakeGameStart(void);
void SetCursor_win(void);
void SetColor_win(WORD color);
int Menu(void);
Snake_t *InitSnake(void);
Food_t *InitFood(void);
bool AddBody(Snake_t *snake);
bool IsOverlap(int x, int y, Snake_t const *snake);
void Move(Snake_t *snake);
bool IsEaten(Snake_t *snake, Food_t *food);
void GenerateFood(Food_t *food, Snake_t const *snake);
bool IsBitten(Snake_t const *snake);
void Draw(Snake_t const *snake, Food_t const *food, int score);
void GotoCoord(int x, int y);
void Refresh(Snake_t const *snake, Food_t const *food, int score);
void GetDirection(Snake_t *snake, Food_t *food);
void FreeSnakeBodies(Snake_t *snake);

#endif /*SNAKE_H_*/