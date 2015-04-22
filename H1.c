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

//Prints the grid. This method runs through the loops [number of columns]*2+[number of columns]*[number of rows]
//times, which is slightly less than the recursive solution i made, but uses an array therefore.
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

//Recursive prints the content of the given row
void grid_recursive_print_row(cell grid, int row) {
  if (row==0) printf("%d ", grid->contents);
  else grid_recursive_print_row(grid->dchild, row-1);
  if (grid->rchild!=NULL) grid_recursive_print_row(grid->rchild, row);
}

//Recursive solution for the print method. The problem is that every cell must be visited
//(SUM(i=0 to [number of row] i) times [number of columns) times additional to the loop ([number of rows] times) in this method.
void grid_recursive_print(cell grid) {
  int row = 0;
  cell here;
  for (here=grid; here!=NULL; here=here->dchild) {
    printf("| ");
    grid_recursive_print_row(grid, row);
    printf("|\n");
    row++;
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
  cell h = grid_init(10, 5); //(columns, rows) initialize the grid
  grid_fill_random(h, 100, 199); //(grid, minimum, maximum) fill random numbers into the cells
  grid_fillCell(6, 2, 666, h); //(column, row, content, grid) replace the content of one specific cell
  printf("Recursive print method:\n");
  grid_recursive_print(h); //(grid) print the grid into the terminal
  printf("Non-recursive print method;\n");
  grid_print(h); //(grid) print the grid into the terminal
  grid_print_dot(h); //(grid) create H1.txt and write the DOT language code of the structure in there
  return 0;
}
