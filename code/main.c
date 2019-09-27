#include <stdio.h>
#include <stdlib.h>

#include "main.h"

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