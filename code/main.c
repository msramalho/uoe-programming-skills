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
	printf("Parameters are rho=%f, size=%d, seed=%d, data=%s, perc=%s\n", opt.rho, opt.size, opt.seed, opt.dataFile, opt.percFile);
	opt.max = opt.size * opt.size;
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
	options opt = loadCmdOptions(argc, argv);

	// seed the random number generator
	rinit(opt.seed);

	//helpful variable
	// int MAX = opt.size * opt.size;


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

	free(map);

	return 0;
}
