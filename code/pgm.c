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
void initializeClusterListAndRank(struct cluster *clusterList, int *rank, options opt, int MAX) {
	// initialize ranks to -1, and cluster ids with size 1
	for (int i = 0; i < MAX; i++) {
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
int countNonEmptyClusters(struct cluster *clusterList, int MAX) {
	// get the number of non-empty clusters
	for (int nCluster = 0; nCluster < MAX; nCluster++) {
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
void printClusterInfo(int nCluster, int maxsize, int MAX) {
	printf("Map has %d clusters, maximum cluster size is %d\n", nCluster, maxsize);
	if (MAX == 1) {
		printf("Displaying the largest cluster\n");
	} else if (MAX == nCluster) {
		printf("Displaying all clusters\n");
	} else {
		printf("Displaying the largest %d clusters\n", MAX);
	}
}

void writeGridToPgmFile(int **map, options opt, int *rank, int MAX) {
	printf("Opening file <%s>\n", opt.percFile);
	FILE *fp;
	fp = fopen(opt.percFile, "w");
	printf("Writing data ...\n");
	fprintf(fp, "P2\n");
	fprintf(fp, "%d %d\n%d\n", opt.size, opt.size, max(MAX, 1));
	int colour;
	for (int i = 1; i <= opt.size; i++) {
		for (int j = 1; j <= opt.size; j++) {
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

void prepareAndWriteGridToPgmFile(int **map, options opt, int MAX) {
	struct cluster *clusterList = (struct cluster *)arralloc(sizeof(struct cluster), 1, MAX);
	int *rank = (int *)arralloc(sizeof(int), 1, MAX);
	// fill rank and clusterList with default values
	initializeClusterListAndRank(clusterList, rank, opt, MAX);
	// update the cluster sizes
	computeClusterSize(map, opt, clusterList);
	// sort the clusters by decreasing size
	percsort(clusterList, MAX);
	//get size of larest cluster
	int maxsize = clusterList[0].size;
	// count non-empty clusters
	int nCluster = countNonEmptyClusters(clusterList, MAX);
	MAX = min(MAX, nCluster);

	// update rank as a "hashmap" of clusterId -> clusterNumber in sorted order
	for (int i = 0; i < nCluster; i++) rank[clusterList[i].id] = i;

	// write PGM data to file
	printClusterInfo(nCluster, maxsize, MAX);

	writeGridToPgmFile(map, opt, rank, MAX);

	free(clusterList);
	free(rank);
}