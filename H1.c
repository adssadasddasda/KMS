#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct _cells *cells;
typedef struct _cells {
  int contents;
  struct cell* rchild;
  struct cell* dchild;
} cell;

cells grid_init(int num_columns, int num_rows) {
  cells new = malloc(sizeof(cell));
  new->contents = 0;
  new->rchild = NULL;
  new->dchild = NULL;
  if (num_columns>1) {
    new->rchild = grid_init(num_columns-1, num_rows);
  }
  if (num_rows>1) {
    new->dchild = grid_init(0, num_rows-1);
  }
  return new;
}

void grid_print(cells grid) {
  int columns = 1;
  int i;
  cells here = grid;
  while (here->rchild!=NULL) {
    columns++;
    here = here->rchild;
  }
  cells row[columns];
  here = grid;
  for (i=0; i<columns; i++) {
    row[i] = here;
    here = here->rchild;
  }
  while (row[0]!=NULL) {
    printf("%s","| ");
    for (i=0; i<columns; i++) {
      printf("%d ", row[i]->contents);
      row[i] = row[i]->dchild;
    }
    printf("%s\n", "|");
  }
}

void grid_print_dot(cells grid) {
  
}

// Fills the specified cell with the content.
void grid_fillCell(int column, int row, int content, cells grid) {
  if (column=0&row=0) {
    grid->contents = content;
  }
  if (column>0) {
    grid_fillCell(column-1, row, content, grid->rchild);
  }
  if (row>0) {
    grid_fillCell(column, row-1, content, grid->dchild);
  }
}

// Fills the Grid with random Numbers in the range between min and max.
void grid_fillrandom(cells grid, int min, int max) {
  cells column = grid;
  cells row;
  srand(time(NULL));
  while (column!=NULL) {
    row = column;
    while (row!=NULL) {
      row->contents = rand()%(max+1-min)+min;
      row = row->dchild;
    }
    column = column->rchild;
  }
}

int main() {
  cells h = grid_init(4, 5); //(columns, rows)
  grid_fillrandom(h, 1, 2); //(grid, minimum, maximum)
  grid_print(h); //(grid)
  return 0;
}
