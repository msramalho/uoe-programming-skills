#include <stdio.h>
#include <stdlib.h>

#include "percolate.h"

/**
 * @brief generates a square grid of given size filled with default value
 * the 2 extra rows and columns are added implicitly by this function
 * cells are not initialized
 * 
 * @param size the length of each dimension of the grid, #rows = #columns
 * @return int** to the grid
 */
int **generateSquareGrid(int size) {
	return (int **)arralloc(sizeof(int), 2, size + 2, size + 2);
}

/**
 * @brief Randomly fills the grid with the specified density and also inserts 
 * the single-cell incremental ids
 * 
 * @param map the 2x2 grid
 * @param opt the options struct that should have size and rho
 * @return int the number of empty cells left
 */
int fillGridRandomly(int **map, options opt) {
	int nEmpty = 0;
	for (int i = 1; i <= opt.size; i++) {
		for (int j = 1; j <= opt.size; j++) {
			if (random_uniform() <= opt.rho) {
				map[i][j] = FULL;  // mark as filled cell
			} else {
				map[i][j] = ++nEmpty;  // add increasing int values starting at 1
			}
		}
	}
	return nEmpty;
}

/**
 * @brief updates the value of the given cell to the max amongst its 4 neighbours
 * 
 * @param map the 2x2 grid
 * @param i the line in the grid for the current cell
 * @param j the column in the grid for the current cell
 * @return true if the cell was the largest already
 * @return false otherwise - its value was changed
 */
bool isMaxNeighbours(int **map, int i, int j) {
	int m = max(map[i][j], map[i - 1][j]);  // up
	m = max(m, map[i + 1][j]);              // down
	m = max(m, map[i][j - 1]);              // left
	m = max(m, map[i][j + 1]);              // right
	int old = map[i][j];
	map[i][j] = m;
	return old == m;
}

/**
 * @brief make the grid intersecting clusters converge on the largest amongst them
 * 
 * @param map the 2x2 grid
 * @param opt an options struct that should contain the map size
 * @return the number of loops
 */
int convergeOnMaxCluster(int **map, options opt) {
	int loop = 1, nchange = 1;
	while (nchange > 0) {
		nchange = 0;
		for (int i = 1; i <= opt.size; i++) {
			for (int j = 1; j <= opt.size; j++) {
				// if this cell is not a wall and was not the max amongst the 4 neighbours
				// increment the changes count
				if (map[i][j] != FULL && !isMaxNeighbours(map, i, j)) nchange++;
			}
		}
		printf("Number of changes on loop %d is %d\n", loop++, nchange);
	}
	return loop;
}

/**
 * @brief search the top and bottom parts of the 2x2 grid for the same cluster
 * 
 * @param map the 2x2 grid
 * @param opt an options struct that should contain the map size
 * @return 0 if no percolation was found or X where X is the number of the first percolation found 
 */
int percolates(int **map, options opt) {
	for (int jtop = 1; jtop <= opt.size; jtop++) {
		if (map[1][jtop] != FULL) {
			for (int jbot = 1; jbot <= opt.size; jbot++) {
				if (map[opt.size][jbot] == map[1][jtop]) {
					return map[opt.size][jbot];  // percolation was found
				}
			}
		}
	}
	return 0;  // no percolation was found
}
