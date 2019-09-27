#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"

#include "lib/arralloc.h"
#include "lib/uni.h"
#include "percolate.h"

// Using macros for increased readability
#define FULL 0

typedef int bool;

struct cluster {
	int id;
	int size;
};

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

typedef struct {
	int size;
	int seed;
	float rho;
	char *dataFile;
	char *percFile;
} options;

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

int max(int a, int b) { return a > b ? a : b; }
int min(int a, int b) { return a < b ? a : b; }

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

int main(int argc, char *argv[]) {
	options opt = loadCmdOptions(argc, argv);
	printf("Parameters are rho=%f, size=%d, seed=%d, data=%s, perc=%s\n", opt.rho, opt.size, opt.seed, opt.dataFile, opt.percFile);

	// seed the random number generator
	rinit(opt.seed);
	int MAX = opt.size * opt.size;

	// generate the grid in an empty state
	int **map = generateSquareGrid(opt.size);

	// Randomly fill the grid
	int nEmpty = fillGridRandomly(map, opt);

	// print real vs expected density
	printf("rho = %f, actual density = %f\n", opt.rho, 1 - nEmpty / (double)MAX);

	// make every cell converge on their cluster number
	int loop = 1, nchange = 1;
	while (nchange > 0) {
		nchange = 0;
		for (int i = 1; i <= opt.size; i++) {
			for (int j = 1; j <= opt.size; j++) {
				if (map[i][j] != FULL && !isMaxNeighbours(map, i, j)) nchange++;
			}
		}
		printf("Number of changes on loop %d is %d\n", loop++, nchange);
	}

	// test if there was a percolation in the cluster
	int percClusetrNum = percolates(map, opt);
	if (percClusetrNum) {
		printf("Cluster DOES percolate. Cluster number: %d\n", percClusetrNum);
	} else {
		printf("Cluster DOES NOT percolate\n");
	}

	// output .dat file
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

	// generate a hashmap of cluster ID -> size
	int ncluster, maxsize;
	struct cluster *clustlist;
	int colour;
	int *rank;
	clustlist = (struct cluster *)arralloc(sizeof(struct cluster), 1, opt.size * opt.size);
	rank = (int *)arralloc(sizeof(int), 1, opt.size * opt.size);
	for (int i = 0; i < opt.size * opt.size; i++) {
		rank[i] = -1;
		clustlist[i].size = 0;
		clustlist[i].id = i + 1;
	}
	for (int i = 1; i <= opt.size; i++) {
		for (int j = 1; j <= opt.size; j++) {
			if (map[i][j] != FULL) {
				clustlist[map[i][j] - 1].size++;
			}
		}
	}

	// sort the clusters by size
	percsort(clustlist, opt.size * opt.size);

	// find out how many clusters are there??
	maxsize = clustlist[0].size;
	for (ncluster = 0; ncluster < opt.size * opt.size && clustlist[ncluster].size > 0; ncluster++)
		;
	if (MAX > ncluster) {
		MAX = ncluster;
	}
	for (int i = 0; i < ncluster; i++) {
		rank[clustlist[i].id - 1] = i;
	}

	// write PGM data to file
	printf("Opening file <%s>\n", opt.percFile);
	fp = fopen(opt.percFile, "w");
	printf("Map has %d clusters, maximum cluster size is %d\n", ncluster, maxsize);
	if (MAX == 1) {
		printf("Displaying the largest cluster\n");
	} else if (MAX == ncluster) {
		printf("Displaying all clusters\n");
	} else {
		printf("Displaying the largest %d clusters\n", MAX);
	}
	printf("Writing data ...\n");
	fprintf(fp, "P2\n");
	fprintf(fp, "%d %d\n%d\n", opt.size, opt.size, max(MAX, 1));

	for (int j = opt.size; j >= 1; j--) {
		for (int i = 1; i <= opt.size; i++) {
			colour = map[i][j];
			if (map[i][j] > 0) {  // TODO: update to != FULL?
				colour = min(rank[map[i][j] - 1], MAX);
			} else {
				colour = MAX;
			}
			fprintf(fp, " %4d", colour);
		}
		fprintf(fp, "\n");
	}
	printf("...done\n");
	fclose(fp);
	printf("File closed\n");
	free(clustlist);
	free(rank);

	free(map);
}

static int clustcompare(const void *p1, const void *p2) {
	int size1, size2, id1, id2;

	size1 = ((struct cluster *)p1)->size;
	size2 = ((struct cluster *)p2)->size;

	id1 = ((struct cluster *)p1)->id;
	id2 = ((struct cluster *)p2)->id;

	if (size1 != size2) {
		return (size2 - size1);
	} else {
		return (id2 - id1);
	}
}

void percsort(struct cluster *list, int n) {
	qsort(list, (size_t)n, sizeof(struct cluster), clustcompare);
}
