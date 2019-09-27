#include "utils.h"
#include "lib/arralloc.h"

#define FULL 0

/**
 * @brief a struct to represent clusters by their id and size
 * 
 */
struct cluster {
	int id; /**< the unique cluster id */
	int size; /**< the size of the cluster */
};

/**
 * @brief a struct to represent user options that are used throughout the code
 * this also includes properties that would otherwise have to be added to the function calls
 * instead of doing so, it is only necessary to add future properties to this struct
 * as it is passed to most functions
 * 
 */
typedef struct {
	int size; /**< the number of rows (or columns as it is square) of the grid */
	int max; /**< the number of cells in the grid (automatically calculated) */
	int maxClusterDisplay; /**< maximum number of clusters to output in the pgm file */
	int seed; /**< the seend for the random number generator */
	float rho; /**< rho, the desired density of filled/total cells in the grid */
	char *dataFile; /**< the name of the file where the .dat version of the grid will be written to */
	char *percFile; /**< the name of the file where the .pgm version of the grid will be written to */
} options;