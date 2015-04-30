#include "support.h"
#include <stdlib.h>
#include <stdio.h>

#define BOARD_X_SIZE 16
#define BOARD_Y_SIZE 16
#define SNAKE_INIT_LENGTH 8

typedef struct _snake snake;
typedef struct _snake {
  snake* next;
  int x_pos;
  int y_pos;
} snake_elem;

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

void snake_delete(snake* psnake) {
  if(psnake->next!=NULL) snake_delete(psnake->next);
  free(psnake);
}

snake* snake_move(char direction, snake* psnake) {
  snake* new = malloc(sizeof(snake_elem));
  new->next = NULL;
  snake* tail = psnake;
  int grow = 0;
  if (grow) {
    //Not yet!
  } else {
    tail = tail->next;
    free(psnake);
    psnake = tail;
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
  return tail;
}

int snake_check_collision(snake* psnake) {
  snake* head = psnake;
  while (head->next!=NULL) head = head->next;
  snake* body = psnake;
  while (body!=head) {
    if ((body->x_pos==head->x_pos)&&(body->y_pos==head->y_pos)) {
      //printf("Body: X: %d Y: %d || HEAD: X: %d Y: %d\n", body->x_pos, body->y_pos, head->x_pos, head->y_pos);
      return 0;
    }
    body = body->next;
  }
    if (head->x_pos<0) head->x_pos=BOARD_X_SIZE-1;
    else if (head->y_pos<0) head->y_pos=BOARD_Y_SIZE-1;
    else if (head->x_pos>BOARD_X_SIZE-1) head->x_pos=0;
    else if (head->y_pos>BOARD_Y_SIZE-1) head->y_pos=0;
  return 1;
}

void snake_print_board(snake* psnake) {
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
  support_clear();
  for (row = 0; row<BOARD_Y_SIZE; row++) {
    for (column=0; column<BOARD_X_SIZE; column++) {
      printf("%c", board[column][row]);
    }
    printf("\n");
  }
}

void snake_run() {
  snake* psnake = snake_init(SNAKE_INIT_LENGTH);
  int game_active = 1, key;
  char direction = 'a';
  while (game_active) {
    key = support_readkey(500);
    //printf("%d\n", key);
    if (key == 'q') game_active = 0;
    else if (key == 'w') {
      psnake = snake_move('w', psnake);
      direction = 'w';
    } else if (key == 'a') {
      psnake = snake_move('a', psnake);
      direction = 'a';
    } else if (key == 's') {
      psnake = snake_move('s', psnake);
      direction = 's';
    } else if (key == 'd') {
      psnake = snake_move('d', psnake);
      direction = 'd';
    } else psnake = snake_move(direction, psnake);
    game_active = snake_check_collision(psnake);
    snake_print_board(psnake);
  }
  snake_delete(psnake);
}

int main() {
  support_init();
  snake_run();
  return 0;
}
