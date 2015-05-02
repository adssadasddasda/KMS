#include "support.c"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define BOARD_X_SIZE 16
#define BOARD_Y_SIZE 16
#define SNAKE_INIT_LENGTH 8

typedef struct _snake snake;
typedef struct _snake {
  snake* next;
  int x_pos;
  int y_pos;
} snake_elem;

typedef struct _food food;
typedef struct _food {
  int x_pos;
  int y_pos;
} food_elem;

typedef struct _score {int points}score;

snake* snake_init(int snake_init_length) {
  if (snake_init_length>0) {
    snake* new = malloc(sizeof(snake_elem));
    new->x_pos = BOARD_X_SIZE/2+snake_init_length;
    //printf("X:%d ", new->x_pos);
    new->y_pos = BOARD_Y_SIZE/2;
    //printf("Y:%d\n", new->y_pos);
    new->next = snake_init(snake_init_length-1);
    return new;
  }
  return NULL;
}

food* snake_food_init(snake* psnake) {
  food* new = malloc(sizeof(food_elem));
  srand(time(NULL));
  new->x_pos = rand()%(BOARD_X_SIZE-1);
  new->y_pos = rand()%(BOARD_Y_SIZE-1);
}

void snake_delete(snake* psnake) {
  if(psnake->next!=NULL) snake_delete(psnake->next);
  free(psnake);
}

void snake_move_head(snake* psnake, snake* new, char direction) {
    while (psnake->next!=NULL) psnake = psnake->next;
    psnake->next = new;
    if (direction == 'w') {
      new->x_pos=psnake->x_pos;
      new->y_pos=psnake->y_pos-1;
    } else if (direction == 'a') {
      new->x_pos=psnake->x_pos-1;
      new->y_pos=psnake->y_pos;
    } else if (direction == 's') {
      new->x_pos=psnake->x_pos;
      new->y_pos=psnake->y_pos+1;
    } else if (direction == 'd') {
      new->x_pos=psnake->x_pos+1;
      new->y_pos=psnake->y_pos;
    }
}

snake* snake_move(char direction, snake* psnake,int grow) {
  snake* new = malloc(sizeof(snake_elem));
  new->next = NULL;
  snake* tail = psnake;
  if (grow==1) {
    snake_move_head(psnake, new, direction);
  } else {
    tail = tail->next;
    free(psnake);
    psnake = tail;
    snake_move_head(psnake, new, direction);
  }
  return tail;
}

int snake_check_collision(snake* psnake, food* pfood, score* pscore) {
  snake* head = psnake;
  while (head->next!=NULL) head = head->next;
  if (head->x_pos<0) head->x_pos=BOARD_X_SIZE-1;
  else if (head->y_pos<0) head->y_pos=BOARD_Y_SIZE-1;
  else if (head->x_pos>BOARD_X_SIZE-1) head->x_pos=0;
  else if (head->y_pos>BOARD_Y_SIZE-1) head->y_pos=0;
  snake* body = psnake;
  while (body!=head) {
    if ((body->x_pos==head->x_pos)&&(body->y_pos==head->y_pos)) {
      return 0;
    }
    body = body->next;
  }
  if ((head->x_pos==pfood->x_pos)&&(head->y_pos==pfood->y_pos)) {
    free(pfood);
    pfood = snake_food_init(psnake);
    pscore->points+=10;
    return 2;
  }
  return 1;
}

void snake_print_board(snake* psnake, food* pfood, score* pscore) {
  int row, column;
  char board[BOARD_X_SIZE][BOARD_Y_SIZE];
  for (row = 0; row<BOARD_Y_SIZE; row++) {
    for (column=0; column<BOARD_X_SIZE; column++) {
      board[column][row] = ' ';
    }
  }
  while (psnake!=NULL) {
    board[psnake->x_pos][psnake->y_pos] = 'X';
    psnake=psnake->next;
  }
  board[pfood->x_pos][pfood->y_pos] = 'O';
  support_clear();
  for (column = 0; column<BOARD_X_SIZE-9; column++) {
    printf("_");
  }
  printf("SCORE=%05d\n", pscore->points);
  for (row = 0; row<BOARD_Y_SIZE; row++) {
      printf("|");
    for (column=0; column<BOARD_X_SIZE; column++) {
      printf("%c", board[column][row]);
    }
    printf("|\n");
  }
  for (column = 0; column<BOARD_X_SIZE+2; column++) {
    printf("_");
  }
}

void snake_run() {
  snake* psnake = snake_init(SNAKE_INIT_LENGTH);
  int game_active = 1, key;
  char direction = 'a';
  int grow;
  food* pfood = snake_food_init(psnake);
  score* pscore = malloc(sizeof(score));
  pscore->points = 0;
  while (game_active>0) {
    key = support_readkey(500);
    //printf("%d\n", key);
    if (key == 'q') {
        game_active = 0;
        printf("QUIT");
    }
    else if (key == 'w') {
      psnake = snake_move('w', psnake, grow);
      direction = 'w';
    } else if (key == 'a') {
      psnake = snake_move('a', psnake, grow);
      direction = 'a';
    } else if (key == 's') {
      psnake = snake_move('s', psnake, grow);
      direction = 's';
    } else if (key == 'd') {
      psnake = snake_move('d', psnake, grow);
      direction = 'd';
    } else psnake = snake_move(direction, psnake, grow);
    grow = 0;
    game_active = snake_check_collision(psnake, pfood, pscore);
    if (game_active == 2) {
      grow = 1;
      game_active = 1;
    }
    snake_print_board(psnake, pfood, pscore);
  }
  snake_delete(psnake);
}

void snake_print_positions(snake* psnake) {
  while (psnake->next!=NULL) {
    printf("X: %d Y: %d\n", psnake->x_pos, psnake->y_pos);
    psnake = psnake->next;
  }
  printf("HEAD: X: %d Y: %d\n", psnake->x_pos, psnake->y_pos);
}

int main() {
  support_init();
  snake_run();
  return 0;
}
