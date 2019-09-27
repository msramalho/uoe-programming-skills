#include <stdbool.h>

#include "lib/uni.h"
#include "pgm.h"

int **generateSquareGrid(int size);
void fillGridRandomly(int **map, options opt, int MAX);
bool isMaxNeighbours(int **map, int i, int j);
int convergeOnMaxCluster(int **map, options opt);
int percolates(int **map, options opt);
void writeGridToDatFile(int **map, options opt);
void printPercolationStatus(int percClusetrNum);
