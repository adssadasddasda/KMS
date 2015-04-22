#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//The typedef struct of cell.
typedef struct _cell *cell;
typedef struct _cell {
  int contents;
  cell rchild;
  cell dchild;
} single_cell;

//Initializes a grid with set columns and rows.
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

//Recursive writing the addresses of the children to the file.
void grid_print_dot_cells(FILE* file, cell grid) {
  if (grid!=NULL) {
    if (grid->dchild!=NULL) fprintf(file, "  M0x%p -> M0x%p\n", grid, grid->dchild);
    if (grid->rchild!=NULL) fprintf(file, "  M0x%p -> M0x%p\n", grid, grid->rchild);
    grid_print_dot_cells(file, grid->dchild);
    grid_print_dot_cells(file, grid->rchild);
  }
}

//Creating, opening and finally closing the H1.exe and writing the surrounding construct for the DOT language.
void grid_print_dot(cell grid) {
  FILE* file = fopen("H1.txt", "w");
  fprintf(file, "digraph {\n");
  grid_print_dot_cells(file, grid);
  fprintf(file, "}");
  fclose(file);
}


//Fills the specified cell with the content.
void grid_fillCell(int column, int row, int content, cell grid) {
  if (grid!=NULL){
    if (column>0) grid_fillCell(column-1, row, content, grid->rchild);
    else if (row>0) grid_fillCell(column, row-1, content, grid->dchild);
    else grid->contents = content;
  }
  else printf("Target cell is outside of the grid!\n");
}

//Recursive runs through the grid and sets the content of every cell to a random number in the range of min to max.
void grid_fill_cell_random(cell grid, int min, int max) {
  grid->contents = rand()%(max+1-min)+min;
  if (grid->rchild!=NULL) grid_fill_cell_random(grid->rchild, min, max);
  if (grid->dchild!=NULL) grid_fill_cell_random(grid->dchild, min, max);
}

//This is necessary because frequent usage of srand(time(NULL)) resets the seed of rand and results in not so random integers.
void grid_fill_random(cell grid, int min, int max) {
  srand(time(NULL));
  grid_fill_cell_random(grid, min, max);
  }

int main() {
  cell h = grid_init(4, 5); //(columns, rows)
  printf("M0x%p\n", h);
  grid_fill_random(h, 1, 2); //(grid, minimum, maximum)
  grid_fillCell(3, 2, 0, h);
  grid_print(h); //(grid)
  grid_print_dot(h);
  return 0;
}
