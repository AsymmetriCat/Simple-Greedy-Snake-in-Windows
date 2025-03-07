#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#include "snake.h"

/*Clean input buffer*/
static void CleanIB(void);

/*Entry the game*/
void SnakeGameStart(void) {
  int score = 0;
  int level = Menu();

  /*Init snake and food*/
  Snake_t *snake = InitSnake();
  if (snake == NULL) {
    exit(EXIT_FAILURE);
  }
  Food_t *food = InitFood();
  if (food == NULL) {
    FreeSnakeBodies(snake);
    free(snake);
    exit(EXIT_FAILURE);
  }

  /*Set difficulty title*/
  if (level == 1) {
    SetColor_win(EASY_MODE);
  } else if (level == 2) {
    SetColor_win(MEDIUM_MODE);
  } else {
    SetColor_win(HARD_MODE);
  }

  SetCursor_win();  /*Hide Cursor*/
  srand(time(NULL));
  system("cls");  /*Clean*/
  Draw(snake, food, score);  /*Draw the maze*/
  do {
    /*Get direction from keyboard
      w/W is up, s/S is down, a/A is left, d/D is right
      q/Q is quit, space is stop*/
    if (_kbhit()) {
      GetDirection(snake, food);
    }
    Move(snake);
    if (IsEaten(snake, food) == true) {
      if (AddBody(snake) == false) {
        FreeSnakeBodies(snake);
        free(snake);
        free(food);
        exit(EXIT_FAILURE);
      }
      GenerateFood(food, snake);
      if (level == 1) {
        score += 1;
      } else if (level == 2) {
        score += 2;
      } else {
        score += 4;
      }
    }
    Refresh(snake, food, score);
    if (level == 1) {
      Sleep(60);
    } else if (level == 2) {
      Sleep(45);
    } else {
      Sleep(30);
    }
  } while(IsBitten(snake) == false);

  /*End*/
  printf("\nYour Scores: %d!", score);
  FreeSnakeBodies(snake);
  free(snake);
  snake = NULL;
  free(food);
  food = NULL;
  printf("\nBye.");
  SetColor_win(DEFAULT_COLOR);
}

void SetCursor_win(void) {
  HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO cursor_info;
  GetConsoleCursorInfo(h_stdout, &cursor_info);
  cursor_info.bVisible = false;
  SetConsoleCursorInfo(h_stdout, &cursor_info);
}

void SetColor_win(WORD color) {
  HANDLE h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(h_stdout, color);
}

int Menu(void) {
  int level = 2;  /*Easy is 1, medium is 2, hard is 3*/
  char *difficulty = MEDIUM;  /*Default is medium*/
  int choice;

  do {
    system("cls");

    /*Print menu*/
    SetColor_win(MENU_COLOR);
    printf("**************************************\n"
           "*  ___  ____  ____  ____  ____  _  _ *\n"
           "* / __)(  _ \\( ___)( ___)(  _ \\( \\/ )*\n"
           "*( (_-. )   / )__)  )__)  )(_) )\\  / *\n"
           "* \\___/(_)\\_)(____)(____)(____/ (__) *\n"
           "*                      _             *\n"
           "*                     | |            *\n"
           "*      ___ _ __   __ _| | _____      *\n"
           "*     / __| '_ \\ / _` | |/ / _ \\     *\n"
           "*     \\__ \\ | | | (_| |   <  __/     *\n"
           "*     |___/_| |_|\\__,_|_|\\_\\___|     *\n"
           "**************************************\n"
           "*   1) Start!                        *\n");
    printf("*   2) Choose difficulty.%-8s    *\n", difficulty);
    printf("*   3) Quit!                         *\n"
           "**************************************\n");

    /*Get choice*/
    do {
      if (scanf("%d", &choice) != 1) {
        printf("Enter 1 ~ 3 please ^w^\n");
        CleanIB();
        continue;
      }
      CleanIB();
      if (choice == 1 || choice == 2 || choice == 3) {
        break;
      } else {
        printf("Enter 1 ~ 3 please ^w^\n");
        continue;
      }
    } while (1);

    /*Change difficulty*/
    if (choice == 2) {
      printf("1) easy  2) medium  3) hard\n");
      printf("Make a choice:");
      do {
        if (scanf("%d", &level) != 1) {
          printf("Enter 1 ~ 3 please ^w^\n");
          CleanIB();
          continue;
        }
        CleanIB();
        if (level == 1 || level == 2 || level == 3) {
          if (level == 1) {
            difficulty = EASY;
          } else if (level == 2) {
            difficulty = MEDIUM;
          } else {
            difficulty = HARD;
          }
          break;
        } else {
          printf("Enter 1 ~ 3 please ^w^\n");
          continue;
        }
      } while (1);
    }
    if (choice == 3) {
      SetColor_win(DEFAULT_COLOR);
      exit(EXIT_FAILURE);
    }
  } while (choice == 2);

  return level;
}

Snake_t *InitSnake(void) {
  Snake_t *snake = (Snake_t *)malloc(sizeof(Snake_t));
  if (snake == NULL) {
    fprintf(stderr, "Error: can't allocate memories");
    SetColor_win(DEFAULT_COLOR);
    return NULL;
  }

  snake->head = (Body_t *)malloc(sizeof(Body_t));
  if (snake->head == NULL) {
    fprintf(stderr, "Error: can't allocate memories");
    SetColor_win(DEFAULT_COLOR);
    return NULL;
  }

  snake->tail = snake->head;
  snake->head->prev = NULL;
  snake->head->next = NULL;
  snake->head->x = HEIGHT / 2;
  snake->head->y = WIDTH / 2;
  snake->head->dir = kStop;

  return snake;
}

Food_t *InitFood(void) {
  Food_t *food = (Food_t *)malloc(sizeof(Food_t));
  if (food == NULL) {
    fprintf(stderr, "Error: can't allocate memories");
    SetColor_win(DEFAULT_COLOR);
    return NULL;
  }

  do {
    food->x = rand() % HEIGHT + 1;
    food->y = rand() % WIDTH + 1;
  } while (food->x != HEIGHT / 2 && food->y != WIDTH / 2);

  return food;
}

bool AddBody(Snake_t *snake) {
  Body_t *new_body = (Body_t *)malloc(sizeof(Body_t));
  Body_t *scan = snake->head->next;
  if (new_body == NULL) {
    fprintf(stderr, "Error: can't allocate memories!");
    SetColor_win(DEFAULT_COLOR);
    return false;
  }
  new_body->next = NULL;

  /*Add new body to snake*/
  if (scan == NULL) {
    snake->head->next = new_body;
    new_body->prev = snake->head;
    snake->tail = new_body;
    if (snake->head->dir == kUp) {
      new_body->x = snake->head->x + 1;
      new_body->y = snake->head->y;
      new_body->dir = snake->head->dir;
    } else if (snake->head->dir == kDown) {
      new_body->x = snake->head->x - 1;
      new_body->y = snake->head->y;
      new_body->dir = snake->head->dir;
    } else if (snake->head->dir == kLeft) {
      new_body->x = snake->head->x;
      new_body->y = snake->head->y + 1;
      new_body->dir = snake->head->dir;
    } else {
      new_body->x = snake->head->x;
      new_body->y = snake->head->y - 1;
      new_body->dir = snake->head->dir;
    }
  } else {
    while (scan->next != NULL) {
      scan = scan->next;
    }
    scan->next = new_body;
    new_body->prev = scan;
    snake->tail = new_body;
    if (scan->dir == kUp) {
      if (scan->x == HEIGHT) {
        new_body->x = 1;
        new_body->y = scan->y;
        new_body->dir = scan->dir;
      } else {
        new_body->x = scan->x + 1;
        new_body->y = scan->y;
        new_body->dir = scan->dir;
      }
    } else if (scan->dir == kDown) {
      if (scan->x == 1) {
        new_body->x = HEIGHT;
        new_body->y = scan->y;
        new_body->dir = scan->dir;
      } else {
        new_body->x = scan->x - 1;
        new_body->y = scan->y;
        new_body->dir = scan->dir;
      }
    } else if (scan->dir == kLeft) {
      if (scan->y == WIDTH) {
        new_body->x = scan->x;
        new_body->y = 1;
        new_body->dir = scan->dir;
      } else {
        new_body->x = scan->x;
        new_body->y = scan->y + 1;
        new_body->dir = scan->dir;
      }
    } else {
      if (scan->y == 1) {
        new_body->x = scan->x;
        new_body->y = WIDTH;
        new_body->dir = scan->dir;
      } else {
        new_body->x = scan->x;
        new_body->y = scan->y - 1;
        new_body->dir = scan->dir;
      }
    }
  }
  return true;
}

bool IsOverlap(int x, int y, Snake_t const *snake) {
  Body_t const *scan = snake->head->next;
  if (x == snake->head->x && y == snake->head->y) {
    return true;
  }
  while (scan != NULL) {
    if (x == scan->x && y == scan->y) {
      return true;
    } else {
      scan = scan->next;
    }
  }
  return false;
}

void Move(Snake_t *snake) {
  Body_t *scan = snake->head;
  if (snake->head->dir == kUp) {
    while (scan->next != NULL) {
      scan = scan->next;
    }
    while (scan->prev != NULL) {
      scan->x = scan->prev->x;
      scan->y = scan->prev->y;
      scan->dir = scan->prev->dir;
      scan = scan->prev;
    }
    if (snake->head->x - 1 != 0) {
      snake->head->x--;
    } else {
      snake->head->x = HEIGHT;
    }
  } else if (snake->head->dir == kDown) {
    while (scan->next != NULL) {
      scan = scan->next;
    }
    while (scan->prev != NULL) {
      scan->x = scan->prev->x;
      scan->y = scan->prev->y;
      scan->dir = scan->prev->dir;
      scan = scan->prev;
    }
    if (snake->head->x + 1 != HEIGHT + 1) {
      snake->head->x++;
    } else {
      snake->head->x = 1;
    }
  } else if (snake->head->dir == kLeft) {
    while (scan->next != NULL) {
      scan = scan->next;
    }
    while (scan->prev != NULL) {
      scan->x = scan->prev->x;
      scan->y = scan->prev->y;
      scan->dir = scan->prev->dir;
      scan = scan->prev;
    }
    if (snake->head->y - 1 != 0) {
      snake->head->y--;
    } else {
      snake->head->y = WIDTH;
    }
  } else if (snake->head->dir == kRight) {
    while (scan->next != NULL) {
      scan = scan->next;
    }
    while (scan->prev != NULL) {
      scan->x = scan->prev->x;
      scan->y = scan->prev->y;
      scan->dir = scan->prev->dir;
      scan = scan->prev;
    }
    if (snake->head->y + 1 != WIDTH + 1) {
      snake->head->y++;
    } else {
      snake->head->y = 1;
    }
  } else {
    ;
  }
}

bool IsEaten(Snake_t *snake, Food_t *food) {
  if (snake->head->x == food->x && snake->head->y == food->y) {
    return true;
  } else {
    return false;
  }
}

void GenerateFood(Food_t *food, Snake_t const *snake) {
  do {
    food->x = rand() % HEIGHT + 1;
    food->y = rand() % WIDTH + 1;
  } while (IsOverlap(food->x, food->y, snake) == true);
}

bool IsBitten(Snake_t const *snake) {
  Body_t const *scan = snake->head->next;
  while (scan != NULL) {
    if (scan->x == snake->head->x && scan->y == snake->head->y) {
      return true;
    } else {
      scan = scan->next;
    }
  }
  return false;
}

void Draw(Snake_t const *snake, Food_t const *food, int score) {
  for (int i = 0; i <= HEIGHT + 1; i++) {
    for (int j = 0; j <= WIDTH + 1; j++) {
      if (IsOverlap(i, j, snake) == true) {
        if (i == snake->head->x && j == snake->head->y) {
          printf("%c%c", '@', ' ');
        } else {
          printf("%c%c", 'O', ' ');
        }
      } else if (i == food->x && j == food->y) {
        printf("%c%c", 'o', ' ');
      } else if (i == 0 || j == 0 || i == HEIGHT + 1 || j == WIDTH + 1) {
        printf("* ");
      } else {
        printf("  ");
      }
    }
    if (i == 0) {
      printf("Scores: %d", score);
    }
    putchar('\n');
  }
}

void GotoCoord(int x, int y) {
  COORD coord = {y, x};  /*In this program, x axis is vertical, y axis is horizontal*/
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Refresh(Snake_t const *snake, Food_t const *food, int score) {
  /*Refresh snake and food*/
  for (int i = 1; i <= HEIGHT; i++) {
    GotoCoord(i, 2);
    for (int j = 1; j <= WIDTH; j++) {
      if (IsOverlap(i, j, snake) == true) {
        if (i == snake->head->x && j == snake->head->y) {
          printf("%c%c", '@', ' ');
        } else {
          printf("%c%c", 'O', ' ');
        }
      } else if (i == food->x && j == food->y) {
        printf("%c%c", 'o', ' ');
      } else {
        printf("  ");
      }
    }
  }

  /*Refresh scores*/
  GotoCoord(0, 2 * WIDTH + 4);
  printf("Scores: %d", score);

  /*Make cursor back*/
  GotoCoord(HEIGHT + 2, 0);
}

void GetDirection(Snake_t *snake, Food_t *food) {
  char dir = _getch();
  switch(dir) {
    case 'w':
    case 'W':
      if (snake->head->dir != kDown) {
        snake->head->dir = kUp;
      }
      break;
    case 's':
    case 'S':
    if (snake->head->dir != kUp) {
      snake->head->dir = kDown;
    }
    break;
    case 'a':
    case 'A':
    if (snake->head->dir != kRight) {
      snake->head->dir = kLeft;
    }
    break;
    case 'd':
    case 'D':
    if (snake->head->dir != kLeft) {
      snake->head->dir = kRight;
    }
    break;
    case ' ':
      snake->head->dir = kStop;
      break;
    case 'q':
    case 'Q':
      SetColor_win(DEFAULT_COLOR);
      FreeSnakeBodies(snake);
      free(snake);
      free(food);
      exit(EXIT_FAILURE);
  }
}

void FreeSnakeBodies(Snake_t *snake) {
  Body_t *scan1 = snake->head;
  Body_t *scan2;
  while (scan1 != NULL) {
    scan2 = scan1->next;
    free(scan1);
    scan1 = scan2;
  }
  snake->head = NULL;
  snake->tail = NULL;
}

static void CleanIB(void) {
  int ch;
  while ((ch = getchar()) != EOF && ch != '\n') {
    ;
  }
}