#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "getopt.h"

#include "percolate.h"


/**
 * @brief uses getopt to parse the command line arguments to fill the @options struct
 * 
 * @param argc as originated from int main()
 * @param argv as originated from int main()
 * @return options a struct with either the default or user-specified values, if the last are present
 */
options loadCmdOptions(int argc, char *argv[]) {
	options opt = {.size = 20, .seed = 1564, .rho = 0.4, .dataFile = "map.dat", .percFile = "map.pgm"};
	int o;
	while ((o = getopt(argc, argv, "g:s:r:d:p:")) != -1) {
		switch (o) {
			case 'g':
				sscanf(optarg, "%d", &opt.size);
				break;
			case 's':
				sscanf(optarg, "%d", &opt.seed);
				break;
			case 'r':
				sscanf(optarg, "%f", &opt.rho);
				break;
			case 'd':
				opt.dataFile = optarg;
				break;
			case 'p':
				opt.percFile = optarg;
				break;
			case ':':
				printf("Option %c needs a value\n", optopt);
				break;
			case '?':
				printf("Unknown option: %c\n", optopt);
				break;
		}
	}
	return opt;
}

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

void writeGridToDatFile(int **map, options opt) {
	printf("Opening file <%s>\n", opt.dataFile);
	FILE *fp;
	fp = fopen(opt.dataFile, "w");
	printf("Writing data ...\n");
	for (int i = 1; i <= opt.size; i++) {
		for (int j = 1; j <= opt.size; j++) {
			fprintf(fp, " %4d", map[i][j]);
		}
		fprintf(fp, "\n");
	}
	printf("...done\n");
	fclose(fp);
	printf("File closed\n");
}


int main(int argc, char *argv[]) {
	options opt = loadCmdOptions(argc, argv);
	printf("Parameters are rho=%f, size=%d, seed=%d, data=%s, perc=%s\n", opt.rho, opt.size, opt.seed, opt.dataFile, opt.percFile);

	int MAX = opt.size * opt.size;

	// seed the random number generator
	rinit(opt.seed);

	// generate the grid in an empty state
	int **map = generateSquareGrid(opt.size);

	// Randomly fill the grid and print real vs expected density
	int nEmpty = fillGridRandomly(map, opt);
	printf("rho = %f, actual density = %f\n", opt.rho, 1 - nEmpty / (double)MAX);

	// make every cell converge on their cluster number
	convergeOnMaxCluster(map, opt);

	// test if there was a percolation in the cluster
	int percClusetrNum = percolates(map, opt);
	if (percClusetrNum) {
		printf("Cluster DOES percolate. Cluster number: %d\n", percClusetrNum);
	} else {
		printf("Cluster DOES NOT percolate\n");
	}

	// output grid to the .dat file specified by the user
	writeGridToDatFile(map, opt);

	// output grid to the .pgm file specified by the user
	prepareAndWriteGridToPgmFile(map, opt, MAX);

	free(map);
}
