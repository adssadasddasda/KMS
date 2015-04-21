#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//The typedef struckt of cells
typedef struct _cell *cell;
typedef struct _cell {
  int contents;
  cell rchild;
  cell dchild;
} single_cell;

//Initialises a grid with set columns and rows.
cell grid_init(int num_columns, int num_rows) {
  cell new = malloc(sizeof(single_cell));
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

//Prints the grid.
void grid_print(cell grid) {
  int columns = 1;
  int i;
  cell here = grid;
  while (here->rchild!=NULL) {
    columns++;
    here = here->rchild;
  }
  cell row[columns];
  here = grid;
  for (i=0; i<columns; i++) {
    row[i] = here;
    here = here->rchild;
  }
  while (row[0]!=NULL) {
    printf("| ");
    for (i=0; i<columns; i++) {
      printf("%d ", row[i]->contents);
      row[i] = row[i]->dchild;
    }
    printf("|\n");
  }
}


//The surrounding construct for the DOT Language.
void grid_print_dot(cell grid) {
  FILE* file = fopen("H1.txt", "w");
  fprintf(file, "digraph {\n");
  grid_print_dot_cells(file, grid);
  fprintf(file, "}");
  fclose(file);
}

//Recursive writing the Adresses of the childs to the file.
void grid_print_dot_cells(FILE* file, cell grid) {
  if (grid!=NULL) {
    if (grid->dchild!=NULL) fprintf(file, "  M0x%p -> M0x%p\n", grid, grid->dchild);
    if (grid->rchild!=NULL) fprintf(file, "  M0x%p -> M0x%p\n", grid, grid->rchild);
    grid_print_dot_cells(file, grid->dchild);
    grid_print_dot_cells(file, grid->rchild);
  }
}

// Fills the specified cell with the content.
void grid_fillCell(int column, int row, int content, cell grid) {
  if (grid!=NULL){
    if (column>0) grid_fillCell(column-1, row, content, grid->rchild);
    else if (row>0) grid_fillCell(column, row-1, content, grid->dchild);
    else grid->contents = content;
  }
  else printf("Target Cell is outside the Grid!\n");
}

// Fills the Grid with random Numbers in the range between min and max.
void grid_fillrandom(cell grid, int min, int max) {
  cell column = grid;
  cell row;
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
  cell h = grid_init(4, 5); //(columns, rows)
  printf("M0x%p\n", h);
  grid_fillrandom(h, 1, 2); //(grid, minimum, maximum)
  grid_fillCell(3, 2, 0, h);
  grid_print(h); //(grid)
  grid_print_dot(h);
  return 0;
}
