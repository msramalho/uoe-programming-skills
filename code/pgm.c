#include <stdio.h>
#include <stdlib.h>

#include "pgm.h"

/**
 * @brief allocate memory for rank array and clusterList
 * 
 * @param clusterList pointer to cluster struct array
 * @param rank pointer to int array
 * @param opt an options struct that should contain the map size
 * @param MAX the max number of cells in the grid
 */
void initializeClusterListAndRank(struct cluster *clusterList, int *rank, options opt) {
	// initialize ranks to -1, and cluster ids with size 1
	for (int i = 0; i < opt.max; i++) {
		rank[i] = -1;
		clusterList[i].size = 0;
		clusterList[i].id = i;
	}
}

/**
 * @brief update clusterList with the number of cells in each cluster
 * 
 * @param map the 2x2 grid
 * @param opt an options struct that should contain the map size
 * @param clusterList pointer to cluster struct array
 */
void computeClusterSize(int **map, options opt, struct cluster *clusterList) {
	// find the right size for each cluster
	for (int i = 1; i <= opt.size; i++) {
		for (int j = 1; j <= opt.size; j++) {
			if (map[i][j] != FULL) clusterList[map[i][j] - 1].size++;
		}
	}
}

/**
 * @brief count the number of non-empty clusters in a sorted array of clusters
 * 
 * @param clusterList the sorted (in decreasing size) array of clusters
 * @param MAX the max number of cells in the grid
 * @return the number of clusters
 */
int countNonEmptyClusters(struct cluster *clusterList, options opt) {
	// get the number of non-empty clusters
	for (int nCluster = 0; nCluster < opt.max; nCluster++) {
		if (clusterList[nCluster].size == 0) return nCluster;
	}
	return 0;
}

/**
 * @brief print verbose information regarding the number of clusters and the largest found
 * 
 * @param nCluster the number of clusters
 * @param maxsize the size of the largest cluster
 * @param MAX the max number of cells in the grid
 */
void printClusterInfo(int nCluster, int maxsize, options opt) {
	printf("Map has %d clusters, maximum cluster size is %d\n", nCluster, maxsize);
	if (opt.maxClusterDisplay == 1) {
		printf("Displaying the largest cluster only\n");
	} else if (opt.maxClusterDisplay == nCluster) {
		printf("Displaying all clusters\n");
	} else {
		printf("Displaying the largest %d clusters\n", opt.maxClusterDisplay);
	}
}

/**
 * @brief write the map to a pgm file, given a rank array and maxValue
 * 
 * @param map the 2x2 grid
 * @param opt an options struct
 * @param rank an array-like "hashmap" of index = clusterId and value = clusterNumber in sorted order
 * @param maxValue 
 */
void writeGridToPgmFile(int **map, options opt, int *rank) {
	printf("Opening file <%s>\n", opt.percFile);
	FILE *fp;
	fp = fopen(opt.percFile, "w");
	printf("Writing data ...\n");
	fprintf(fp, "P2\n");
	fprintf(fp, "%d %d\n%d\n", opt.size, opt.size, max(opt.maxClusterDisplay, 1));
	int colour, clusterOrderNum;
	for (int i = 1; i <= opt.size; i++) {
		for (int j = 1; j <= opt.size; j++) {
			clusterOrderNum = rank[map[i][j] - 1];
			// display only if cluster is in the "maxClusterDisplay" largest clusters, and is not a wall
			colour = map[i][j] == FULL ? opt.maxClusterDisplay : min(clusterOrderNum, opt.maxClusterDisplay);
			fprintf(fp, " %4d", colour);
		}
		fprintf(fp, "\n");
	}
	printf("...done\n");
	fclose(fp);
	printf("File closed\n");
}

/**
 * @brief custom compare function for the qsort function
 * 
 * @param p1 the first cluster
 * @param p2 the second cluster
 * @return size difference or id difference if size is the same for both clusters
 */
static int clusterCompare(const void *p1, const void *p2) {
	int size1, size2, id1, id2;

	size1 = ((struct cluster *)p1)->size;
	size2 = ((struct cluster *)p2)->size;

	id1 = ((struct cluster *)p1)->id;
	id2 = ((struct cluster *)p2)->id;

	return size1 != size2 ? size2 - size1 : id2 - id1;
}

/**
 * @brief sorts an array of clusters using qsort
 * 
 * @param list the array to sort
 * @param n the size of the array
 */
void percolationSort(struct cluster *list, int n) {
	qsort(list, (size_t)n, sizeof(struct cluster), clusterCompare);
}

/**
 * @brief perform all operations required to convert the original cluster grid
 * into a pgm-friendly data structure and then write it to the percolation file
 * 
 * @param map the 2x2 grid
 * @param opt an options struct
 */
void prepareAndWriteGridToPgmFile(int **map, options opt) {
	struct cluster *clusterList = (struct cluster *)arralloc(sizeof(struct cluster), 1, opt.max);
	int *rank = (int *)arralloc(sizeof(int), 1, opt.max);

	// fill rank and clusterList with default values
	initializeClusterListAndRank(clusterList, rank, opt);
	// update the cluster sizes
	computeClusterSize(map, opt, clusterList);
	// sort the clusters by decreasing size
	percolationSort(clusterList, opt.max);
	//get size of larest cluster
	int maxsize = clusterList[0].size;
	// count non-empty clusters
	int nCluster = countNonEmptyClusters(clusterList, opt);
	// update how many clusters to display
	opt.maxClusterDisplay = min(opt.maxClusterDisplay, nCluster);

	// update rank as a "hashmap" of clusterId -> clusterNumber in sorted order
	for (int i = 0; i < nCluster; i++) rank[clusterList[i].id] = i;

	// write PGM data to file
	printClusterInfo(nCluster, maxsize, opt);

	writeGridToPgmFile(map, opt, rank);

	free(clusterList);
	free(rank);
}