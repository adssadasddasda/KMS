#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//The typedef struct of cell.
typedef struct _cell cell;
typedef struct _cell {
  int contents;
  cell* rchild;
  cell* dchild;
} single_cell;


//Initializes a grid with set columns and rows.
cell* grid_init(int num_columns, int num_rows) {
  cell* new = malloc(sizeof(single_cell));
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

//Recursive prints the content of the given row
void grid_print_row(cell* grid, int row) {
  if (row==0) printf("%d ", grid->contents);
  else grid_print_row(grid->dchild, row-1);
  if (grid->rchild!=NULL) grid_print_row(grid->rchild, row);
}

//Recursive solution for the print method. The problem is that every cell must be visited
//(SUM(i=0 to [number of rows]) i) * [number of columns]) times additional to the loop ([number of rows] times) in this method.
void grid_print(cell* grid) {
  int row = 0;
  cell* here;
  for (here=grid; here!=NULL; here=here->dchild) {
    printf("| ");
    grid_print_row(grid, row);
    printf("|\n");
    row++;
  }
}

//Recursive writing the addresses of the children to the file.
void grid_print_dot_cells(FILE* file, cell* grid) {
  if (grid!=NULL) {
    if (grid->dchild!=NULL) fprintf(file, "  M0x%p -> M0x%p\n", grid, grid->dchild);
    if (grid->rchild!=NULL) fprintf(file, "  M0x%p -> M0x%p\n", grid, grid->rchild);
    grid_print_dot_cells(file, grid->dchild);
    grid_print_dot_cells(file, grid->rchild);
  }
}

//Creating, opening and finally closing the H1.exe and writing the surrounding construct for the DOT language.
void grid_print_dot(cell* grid) {
  FILE* file = fopen("H1.txt", "w");
  fprintf(file, "digraph {\n");
  grid_print_dot_cells(file, grid);
  fprintf(file, "}");
  fclose(file);
}

//Fills the specified cell with the content.
void grid_fillCell(int column, int row, int content, cell* grid) {
  if (grid!=NULL){
    if (column>0) grid_fillCell(column-1, row, content, grid->rchild);
    else if (row>0) grid_fillCell(column, row-1, content, grid->dchild);
    else grid->contents = content;
  }
  else printf("Target cell is outside of the grid for grid_fillCell(int column, int row, int content, cell* grid).\n");
}

//Recursive runs through the grid and sets the content of every cell to a random number in the range of min to max.
void grid_fill_cell_random(cell* grid, int min, int max) {
  grid->contents = rand()%(max+1-min)+min;
  if (grid->rchild!=NULL) grid_fill_cell_random(grid->rchild, min, max);
  if (grid->dchild!=NULL) grid_fill_cell_random(grid->dchild, min, max);
}

//This is necessary because frequent usage of srand(time(NULL)) resets the seed of rand and results in not so random integers.
void grid_fill_random(cell* grid, int min, int max) {
  srand(time(NULL));
  grid_fill_cell_random(grid, min, max);
  }

//Prints the grid. This method runs through the loops [number of columns]*2+[number of columns]*[number of rows]
//times, which is slightly less than the recursive solution i made, but uses an array therefore.
void alt_grid_print(cell* grid) {
  int columns = 1;
  int i;
  cell* here;
  for (here=grid; here->rchild!=NULL; here=here->rchild) {
    columns++;
  }
  cell* row[columns];
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

//Deletes the cell and all dchilds of that cell.
void grid_delete_column_cell(cell* column) {
  if (column->dchild!=NULL) grid_delete_column_cell(column->dchild);
  free(column);
}

//Get the top cell of the desired column.
cell* grid_get_column(cell* grid, int column) {
  cell* target = grid;
  int i;
  for (i=0; i<column; i++) target = target->rchild;
  return target;
}

//Deletes and reliks the desired column. Frees memory.
cell* grid_delete_column(cell* grid, int column) {
  if (grid!=NULL){
    if (column>1) {
      grid->rchild = grid_delete_column(grid->rchild, column-1);
      return grid;
    } else if (column==0) {
      cell* rchild = grid->rchild;
      grid_delete_column_cell(grid);
      return rchild;
    } else if (column<0) {
      printf("Column does not exist for grid_delete_column(cell* grid, int column).\n");
      return NULL;
    }
  } else {
    printf("Column does not exist for grid_delete_column(cell* grid, int column).\n");
    return NULL;
  }
  return NULL;
}

//For use in grid_delete_row, deletes 1 cell in the desired row and relinks if necessary. Doesnt affect other columns!
cell* grid_delete_row_cell(cell* grid, int row) {
  if (grid!=NULL) {
    if (row>1) {
      grid_delete_row_cell(grid->dchild, row-1);
      return grid;
    } else if (row==1) {
      grid->dchild = grid_delete_row_cell(grid->dchild, row-1);
      return grid;
    } else if (row==0) {
      cell* dchild = grid->dchild;
      free(grid);
      return dchild;
    }
  } else {
    printf("Row does not exist for grid_delete_row(cell* grid, int row).\n");
    return NULL;
  }
  return NULL;
}

//Deletes a row of the grid, frees memory and relinks cells if necessary.
cell* grid_delete_row(cell* grid, int row) {
  if (grid!=NULL) {
      if (row==0) {
        if (grid->dchild!=NULL) {
          cell* here = grid;
          cell* next;
          cell* dchild = grid->dchild;
          while (here!=NULL && here->rchild!=NULL) {
            here->dchild->rchild = here->rchild->dchild;
            next = here->rchild;
            free(here);
            here = next;
          }
          free(here);
          return dchild;
        } else {
          cell* here = grid;
          while (here!=NULL) {
            while(here->rchild!=NULL) {
              here = here->rchild;
            }
          }
        }
      } else if (row>0) {
        cell* here;
        for (here=grid; here!=NULL; here=here->rchild) {
          grid_delete_row_cell(here, row);
        }
        return grid;
      } else {
        printf("Target row cannot be <0 for grid_delete_row(cell* grid, int row).\n");
        return NULL;
      }
  }
  return NULL;
}

//Delete the whole grid and free memory.
void grid_delete(cell* grid) {
  if (grid->rchild!=NULL) grid_delete(grid->rchild);
  grid_delete_column_cell(grid);
}

int main() {
  cell* h = grid_init(10, 5); //(columns, rows) initialize the grid
  grid_fill_random(h, 100, 199); //(grid, minimum, maximum) fill random numbers into the cells
  grid_fillCell(6, 2, 666, h); //(column, row, content, grid) replace the content of one specific cell
  printf("Recursive print method:\n");
  grid_print(h); //(grid) print the grid into the terminal
  grid_print_dot(h); //(grid) create H1.txt and write the DOT language code of the structure in there

  //Some extra stuff
  h = grid_delete_column(h, 0);
  h = grid_delete_row(h, 0);
  printf("Non-recursive print method;\n");
  alt_grid_print(h); //(grid) print the grid into the terminal

  //Necessary since C does not have a garbage collector like for example Java has.
  grid_delete(h);
  return 0;
}
