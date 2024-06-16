/*  Nikolaos Kontogeorgis - 4655
	Assignment 4
	sudoku.c  */

#include "sudoku.h"

/*	Read a sudoku grid from stdin and return an object Grid_T
	initialized to these values.  */
Grid_T sudoku_read(void) {
	Grid_T g;
	int i, j, n;
	char c;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			do {
				c = getchar();
			} while (c < '0' || c > '9');
			sscanf(&c, "%d", &n);
			grid_update_value(&g, i, j, n);
		}
	}
	return g;
}

/*  Print the sudoku puzzle defined by g to stream s in the same format
	as expected by sudoku_read().  */
void sudoku_print(FILE *s, Grid_T g) {
	int i, j;
	assert(s);

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			fprintf(s, "%d ", grid_read_value(g, i, j));
		}
		fprintf(s, "\n");
	}
}

/*  Check row of element (row, col) for conflicts  */
int check_row(Grid_T g, int row, int col) {
	int j, n = grid_read_value(g, row, col);

	for (j = 0; j < 9; j++)
		if (j != col && grid_read_value(g, row, j) == n)
			return j;

	return -1;
}
static int ncheck_row(Grid_T g, int row, int col, int n) {
	int j;

	for (j = 0; j < 9; j++)
		if (j != col && grid_read_value(g, row, j) == n)
			return j;

	return -1;
}

/*  Check column of element (row, col) for conflicts */
static int check_col(Grid_T g, int row, int col) {
	int i, n = grid_read_value(g, row, col);

	for (i = 0; i < 9; i++)
		if (i != row && grid_read_value(g, i, col) == n)
			return i;

	return -1;
}
static int ncheck_col(Grid_T g, int row, int col, int n) {
	int i;

	for (i = 0; i < 9; i++)
		if (i != row && grid_read_value(g, i, col) == n)
			return i;

	return -1;
}

/*  Check tile of element (row, col) for conflicts */
static int check_tile(Grid_T g, int row, int col) {
	int i, j, n = grid_read_value(g, row, col);

	for (i = row - (row % 3); i < row + 3 - (row % 3); i++) {
		for (j = col - (col % 3); j < col + 3 - (col % 3); j++) {
			if ((i != row && j != col) && grid_read_value(g, i, j) == n)
				return i + j * 10;
		}
	}
	return -1;
}
static int ncheck_tile(Grid_T g, int row, int col, int n) {
	int i, j;

	for (i = row - (row % 3); i < row + 3 - (row % 3); i++) {
		for (j = col - (col % 3); j < col + 3 - (col % 3); j++) {
			if ((i != row && j != col) && grid_read_value(g, i, j) == n)
				return i + j * 10;
		}
	}
	return -1;
}

/*	Print all row, col, sub-grid errors/conflicts found in puzzle g;
	some errors may be reported more than once.  */
void sudoku_print_errors(Grid_T g) {
	int i, j, n;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			n = check_row(g, i, j);
			if (n > j)
				fprintf(stdout, "Row conflict: elements (%d, %d) and "
								"(%d, %d)\n",
						i, j, i, n);
			n = check_col(g, i, j);
			if (n > i)
				fprintf(stdout, "Column conflict: elements (%d, %d) and "
								"(%d, %d)\n",
						i, j, n, j);
			n = check_tile(g, i, j);
			if (n > i + j * 10)
				fprintf(stderr, "Tile conflict: elements (%d, %d) and "
								"(%d, %d)\n",
						i, j, n % 10, n / 10);
		}
	}
}

/*	Return true iff puzzle g is correct. */
int sudoku_is_correct(Grid_T g) {
	int i, j;
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			if ((check_row(g, i, j) + check_col(g, i, j) + check_tile(g, i, j)) != -3)
				return 0;
	return 1;
}

/* Initializes available choices for all elts of g */
static void sudoku_init_choices(Grid_T *g) {
	int i, j, n;
	assert(g);

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			grid_clear_choice(g, i, j, 0);
			if (grid_read_value(*g, i, j) == 0) {
				grid_set_count(g, i, j);
				for (n = 1; n < 10; n++) {
					grid_set_choice(g, i, j, n);
					if ((ncheck_row(*g, i, j, n) != -1) ||
						(ncheck_col(*g, i, j, n) != -1) ||
						(ncheck_tile(*g, i, j, n) != -1)) {
							grid_remove_choice(g, i, j, n);
					}
				}
			}
			else {
				for (n = 1; n < 10; n++)
					grid_clear_choice(g, i, j, n);
				grid_clear_count(g, i, j);
			}
		}
	}
}

/*  Returns a random choice of an elt with the lowest number of available
	choices. If there is none, returns 0  */
static int sudoku_try_next(Grid_T g, int *row, int *col) {
	int i, j, n, c;
	for (c = 1; c < 10; c++) {
		for (i = rand() % 9, n = 0; n++ < 9; i == 8 ? i = 0 : i++) {
			for (j = 0; j < 9; j++) {
				if (grid_read_value(g, i, j) > 0)
					continue;
				if (grid_read_count(g, i, j) == 0)
					return 0;
				if (grid_read_count(g, i, j) == c) {
					assert(!grid_choice_is_valid(g, i, j, 0));
					*row = i;
					*col = j;
					while ((n = (rand() % 9) + 1))
						if (grid_choice_is_valid(g, *row, *col, n))
							return n;
				}
			}
		}
	}
	return 0;
}

/* Eliminates n as a choice for all elts that conflict with elt (r, c) */
static void sudoku_eliminate_choice(Grid_T *g, int r, int c, int n) {
	int i, j, tile_has;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (i == r || j == c)
				grid_remove_choice(g, i, j, n);
		}
	}
	while ((tile_has = ncheck_tile(*g, r, c, n) != -1))
		grid_remove_choice(g, tile_has % 10, tile_has / 10, n);
}

/*  Updates value of elt (i, j) to n and removes n as a choice.
	Returns remaining available choices */
static int sudoku_update_choice(Grid_T *g, int i, int j, int n){
	assert(g);
	grid_update_value(g, i, j, n);
	grid_remove_choice(g, i, j, n);
	return grid_read_count(*g, i, j);
}

/*	Solves puzzle g and return the solved puzzle; if the puzzle has
	multiple solutions, return one of the possible solutions.  */
Grid_T sudoku_solve(Grid_T g){
	int i, j, n;
	Grid_T sol;
	sudoku_init_choices(&g);
	grid_set_unique(&g);

	while ((n = sudoku_try_next(g, &i, &j))) {
		if (sudoku_update_choice(&g, i, j, n)) {
			sol = sudoku_solve(g);
			if (sudoku_is_correct(sol)) {
				if (sudoku_try_next(g, &i, &j))
					grid_clear_unique(&sol);
				return sol;
			}
			grid_update_value(&g, i, j, 0);
		}
		else
			sudoku_eliminate_choice(&g, i, j, n);
	}
	return g;
}

/*	Returns true if solution g, as returned by sudoku_solve, has a
	unique choice for each step (no backtracking required). (Note, g
	must have been computed with the use of sudoku_solve.) */
int sudoku_solution_is_unique(Grid_T g) {
	return g.unique;
}

Grid_T sudoku_generate_complete() {
	Grid_T g;
	int i, j, count = 11;
	int solved[81] = {7, 4, 1, 3, 8, 2, 5, 6, 9,
					  5, 2, 9, 4, 6, 1, 7, 8, 3,
					  6, 3, 8, 5, 7, 9, 4, 1, 2,
					  1, 5, 3, 9, 4, 7, 6, 2, 8,
					  9, 6, 2, 8, 1, 5, 3, 4, 7,
					  4, 8, 7, 6, 2, 3, 9, 5, 1,
					  3, 7, 6, 2, 5, 8, 1, 9, 4,
					  2, 1, 4, 7, 9, 6, 8, 3, 5,
					  8, 9, 5, 1, 3, 4, 2, 7, 6};

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			grid_update_value(&g, i, j, 0);

	while (--count && !sudoku_is_correct(g))
		g = sudoku_solve(g);

	if (!sudoku_is_correct(g)) {
		count = 0;
		for (i = 0; i < 9; i++)
			for (j = 0; j < 9; j++)
				grid_update_value(&g, i, j, solved[count++]);
	}
	return g;
}

/*	Generate and return a sudoku puzzle with "approximately" nelts
	elements having non-0 value. The smaller nelts the harder may be to
	generate/solve the puzzle. For instance, nelts=81 should return a
	completed and correct puzzle. */
Grid_T sudoku_generate(int nelts, int u) {
	Grid_T g = sudoku_generate_complete();
	int i, j, n, val;
	assert(nelts <= 81 && nelts >= 0);

	for (n = 0; n < 81 - nelts; n++) {
		do {
			i = rand() % 9;
			j = rand() % 9;
		} while (!(val = grid_read_value(g, i, j)));

		grid_update_value(&g, i, j, 0);

		if (u && !sudoku_solution_is_unique(sudoku_solve(g))) {
			grid_update_value(&g, i, j, val);
		}
	}
	return g;
}

int main(int argc, char *argv[]) {
	int n;
	Grid_T g, sol;

	if (argc == 1) {
		g = sudoku_read();
		fprintf(stdout, "\n");
		sudoku_print(stderr, g);
		sol = sudoku_solve(g);

		if (!sudoku_is_correct(sol))
			fprintf(stderr, "Puzzle has no solution.\n");
		else if (sudoku_solution_is_unique(sol))
			fprintf(stderr, "Puzzle has a (unique choice) solution:\n");
		else
			fprintf(stderr, "Puzzle has non-unique solution.\n");

		sudoku_print(stdout, sol);
	}
	else if (!strcmp(argv[1], "-c")) {
		g = sudoku_read();
		sudoku_print(stderr, g);

		if (sudoku_is_correct(g))
			fprintf(stderr, "Puzzle solution is correct.\n");
		else
			fprintf(stderr, "Puzzle solution is incorrect:\n");

		sudoku_print_errors(g);
	}
	else if (!strcmp(argv[1], "-g")) {
		sscanf(argv[2], "%d", &n);

		if (argc > 3 && !strcmp(argv[3], "-u"))
			g = sudoku_generate(n, 1);
		else
			g = sudoku_generate(n, 0);

		fprintf(stderr, "New puzzle:\n");
		sudoku_print(stdout, g);
	}
	else {
		printf("Invalid arguments\n");
		return -1;
	}
	return 0;
}