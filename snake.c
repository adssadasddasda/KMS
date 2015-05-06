#include "support.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define BOARD_X_SIZE 16 //X-Size of the board
#define BOARD_Y_SIZE 16 //Y-Size of the board
#define SNAKE_INIT_LENGTH 8 //must not be larger than BOARD_X_SIZE/2!!!!!!!!!!!!!!!!!!!!!!!

// snake struct
typedef struct _snake snake;
typedef struct _snake {
  snake* next;
  int x_pos;
  int y_pos;
} snake_elem;

//food struct
typedef struct _food food;
typedef struct _food {
  int x_pos;
  int y_pos;
} food_elem;

//initialize the snake in the middle of the board going left
snake* snake_init(int snake_init_length) {
  if (snake_init_length>0) {
    snake* new = malloc(sizeof(snake_elem));
    new->x_pos = BOARD_X_SIZE/2+snake_init_length;
    new->y_pos = BOARD_Y_SIZE/2;
    new->next = snake_init(snake_init_length-1);
    return new;
  }
  return NULL;
}

//initialize a new food element. Makes sure the food is not in the snake. Can result in an infinite loop if no space is available!
food* snake_food_init(snake* psnake) {
  snake* snakehere;
  int no_valid_pos = 1;
  food* new = malloc(sizeof(food_elem));
  srand(time(NULL));
  new->x_pos = rand()%(BOARD_X_SIZE-1);
  new->y_pos = rand()%(BOARD_Y_SIZE-1);
  while(no_valid_pos==1) {
    no_valid_pos = 0; //assume this position is valid
    snakehere = psnake;
    while((snakehere!=NULL)&&(no_valid_pos==0)){
      if((snakehere->x_pos==new->x_pos)&&(snakehere->y_pos==new->y_pos)) { //check if assumption is right. If not, then give food a new position.
	new->x_pos = rand()%(BOARD_X_SIZE-1);
	new->y_pos = rand()%(BOARD_Y_SIZE-1);
	no_valid_pos = 1;
      } else snakehere = snakehere->next;
    }
  }
  return new;
}

//delete the snake and free the memory.
void snake_delete(snake* psnake) {
  if(psnake->next!=NULL) snake_delete(psnake->next);
  free(psnake);
}

//Move the head to the direction where the snake is moved.
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

//delete the tail if the snake does not grow. then move the head
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

//check if the head touches the snake or the food. Additionally check if the head touches the screen border and places it on the opposite side of the screen if it does.
int snake_check_collision(snake* psnake, food* pfood) {
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
    return 2;
  }
  return 1;
}

//prints the board. shows score and speed and paints a border.
void snake_print_board(snake* psnake, food* pfood, int score, int speed) {
  int row, column;
  int board[BOARD_X_SIZE][BOARD_Y_SIZE] = {0};
  int value;
  while (psnake!=NULL) {
    board[psnake->x_pos][psnake->y_pos] = 1;
    psnake=psnake->next;
  }
  board[pfood->x_pos][pfood->y_pos] = 2;
  support_clear();
  for (column = 0; column<BOARD_X_SIZE-9; column++) {
    printf("-");
  }
  printf("SCORE=%05d\n", score);
  for (row = 0; row<BOARD_Y_SIZE; row++) {
      printf("|");
    for (column=0; column<BOARD_X_SIZE; column++) {
      value = board[column][row];
      switch(value) {
	case 0: printf(" "); break;
	case 1: printf("X"); break;
	case 2: printf("O"); break;
      }
    }
    printf("|\n");
  }
  for (column = 0; column<BOARD_X_SIZE-6; column++) {
    printf("-");
  }
  printf("SPEED=%02d\n", (1000-speed)/10);
}

//returns the maximum integer of a or b
int max(int num_a, int num_b) {
  return (num_a < num_b ? num_b : num_a);
}

//main loop for running the game. Here is where the chaos is!!
void snake_run() {
  snake* psnake = snake_init(SNAKE_INIT_LENGTH);
  int snake_size = SNAKE_INIT_LENGTH;
  int game_active = 1, key;
  char direction = 'a';
  int score = 0, grow, penalty = 0, penalty_counter = 0, speed;
  food* pfood = snake_food_init(psnake);
  while (game_active>0) {
    speed = max(1000-10*max(penalty, -9), 10);
    key = support_readkey(speed);
    if (key == 'q') {
      break;
    } else if (key == 'w') {
      if (direction!='s') direction = 'w';
    } else if (key == 'a') {
      if (direction!='d') direction = 'a';
    } else if (key == 's') {
      if (direction!='w') direction = 's';
    } else if (key == 'd') {
      if (direction!='a') direction = 'd';
    }
    psnake = snake_move(direction, psnake, grow);
    if (penalty_counter>9) {
     penalty++;
     penalty_counter=0;
    }
    grow = 0;
    game_active = snake_check_collision(psnake, pfood);
    if (game_active == 2) {
      grow = 1;
      game_active = 1;
      penalty_counter = 0;
      penalty--;
      score += max((1000-speed)/10, 1);
      snake_size++;
    } else penalty_counter++;
    if (snake_size > BOARD_X_SIZE+BOARD_Y_SIZE-1) game_active = 0;
    snake_print_board(psnake, pfood, score, speed);
  }
  support_clear();
  printf("   --- GAME OVER ---\n");
  printf("SCORE = %d\n", score);
  snake_delete(psnake);
  free(pfood);
}

//main method. doesnt do much.
int main() {
  support_init();
  snake_run();
  return 0;
}
