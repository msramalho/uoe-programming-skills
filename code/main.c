#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "getopt.h"

/**
 * @brief uses getopt to parse the command line arguments to fill the @options struct
 * 
 * @param argc as originated from int main()
 * @param argv as originated from int main()
 * @return options a struct with either the default or user-specified values, if the last are present
 */
options loadCmdOptions(int argc, char *argv[]) {
	options opt = {.size = 20, .seed = 1564, .rho = 0.4, .dataFile = "map.dat", .percFile = "map.pgm", .maxClusterDisplay = -1};
	int o;
	while ((o = getopt(argc, argv, "g:s:r:m:d:p:")) != -1) {
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
			case 'm':
				sscanf(optarg, "%d", &opt.maxClusterDisplay);
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
	opt.max = opt.size * opt.size;
	// if not user provided, assume that all clusters are to be shown
	if (opt.maxClusterDisplay == -1) opt.maxClusterDisplay = opt.max;

	printf("Parameters are rho=%f, size=%d, seed=%d, maxDisplay=%d, data=%s, perc=%s\n", opt.rho, opt.size, opt.seed, opt.maxClusterDisplay, opt.dataFile, opt.percFile);
	return opt;
}


/**
 * @brief writes the given 2x2 grid to a .dat file
 * 
 * @param map the 2x2 grid
 * @param opt an options struct that should contain the map size
 */
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

/**
 * @brief the main function of the program
 * 
 * @param argc the number of command line arguments
 * @param argv command line arguments
 * @return int exit code, 0 is normal execution
 */
int main(int argc, char *argv[]) {
	// read the CMD line arguments or load defaults
	options opt = loadCmdOptions(argc, argv);

	// seed the random number generator
	rinit(opt.seed);

	// initialize the grid variable with appropriate extra rows and columns
	int **map = generateSquareGrid(opt.size);

	// Randomly fill the grid and print real vs expected density
	fillGridRandomly(map, opt);

	// make every cell converge on their cluster number
	convergeOnMaxCluster(map, opt);

	// test if there was a percolation in the cluster
	printPercolationStatus(percolates(map, opt));

	// output grid to the .dat file specified by the user
	writeGridToDatFile(map, opt);

	// output grid to the .pgm file specified by the user
	prepareAndWriteGridToPgmFile(map, opt);

	//free the allocated memory
	free(map);

	return 0;
}
