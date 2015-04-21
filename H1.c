#include <stdio.h>
#include <stdlib.h>

typedef struct _cell *cells;
typedef struct _cell {
  int contents;
  struct cells* rchild;
  struct cells* dchild;
} cell;

//#define NILCell ((cells)0)

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

void grid_fillCell(int column, int row, int content, cells grid) {
  int i;
  cells here = grid;
  for (i=1; i<column; i++) {
    here = here->rchild;
  }
  for (i=1; i<row; i++) {
    here = here->dchild;
  }
  here->contents = content;
}

int main() {
  cells h = grid_init(3, 4);
  grid_fillCell(2, 3, 5, h);
  grid_fillCell(1, 1, 3, h);
  grid_fillCell(3, 4, 9, h);
  grid_print(h);
  return 0;
}