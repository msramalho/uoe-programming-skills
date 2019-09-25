#include <stdio.h>
#include <stdlib.h>

#include "lib/arralloc.h"
#include "lib/uni.h"
#include "percolate.h"
/**
 * @brief generates a square grid of given size filled with default value
 * 
 * @param size the length of each dimension of the grid, #rows = #columns
 * @param defaultValue the value to insert in each cell in the matrix
 * @return int** to the grid
 */
int **generateSquareGrid(int size, int defaultValue) {
	int** grid = (int **)arralloc(sizeof(int), 2, size, size);
    for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			grid[i][j] = defaultValue;
		}
	}
    return grid;
}

int main(void) {
	int L;
	float rho;
	int seed;
	int MAX;
	char *datafile;
	char *percfile;
	int **map = generateSquareGrid(L + 2, 0);

	L = 20;
	rho = 0.40;
	seed = 1564;
	datafile = "map.dat";
	percfile = "map.pgm";
	MAX = L * L;

	rinit(seed);

	printf("Parameters are rho=%f, L=%d, seed=%d\n", rho, L, seed);

	int nfill;
	int i, j;
	float r;
	nfill = 0;
	for (i = 1; i <= L; i++) {
		for (j = 1; j <= L; j++) {
			r = random_uniform();
			if (r > rho) {
				nfill++;
				map[i][j] = 1;
			}
		}
	}
	printf("rho = %f, actual density = %f\n", rho,
	       1.0 - ((double)nfill) / ((double)L * L));

	/* Fix bug. */
	nfill = 0;
	for (i = 1; i <= L; i++) {
		for (j = 1; j <= L; j++) {
			if (map[i][j] != 0) {
				nfill++;
				map[i][j] = nfill;
			}
		}
	}

	int loop, nchange, old;
	loop = 1;
	nchange = 1;
	while (nchange > 0) {
		nchange = 0;
		for (i = 1; i <= L; i++) {
			for (j = 1; j <= L; j++) {
				if (map[i][j] != 0) {
					old = map[i][j];
					if (map[i - 1][j] > map[i][j]) map[i][j] = map[i - 1][j];
					if (map[i + 1][j] > map[i][j]) map[i][j] = map[i + 1][j];
					if (map[i][j - 1] > map[i][j]) map[i][j] = map[i][j - 1];
					if (map[i][j + 1] > map[i][j]) map[i][j] = map[i][j + 1];
					if (map[i][j] != old) {
						nchange++;
					}
				}
			}
		}
		printf("Number of changes on loop %d is %d\n", loop, nchange);
		loop++;
	}

	int itop, ibot, percclusternum;
	int percs = 0;
	percclusternum = 0;
	for (itop = 1; itop <= L; itop++) {
		if (map[itop][L] > 0) {
			for (ibot = 1; ibot <= L; ibot++) {
				if (map[itop][L] == map[ibot][1]) {
					percs = 1;
					percclusternum = map[itop][L];
				}
			}
		}
	}
	if (percs) {
		printf("Cluster DOES percolate. Cluster number: %d\n", percclusternum);
	} else {
		printf("Cluster DOES NOT percolate\n");
	}

	printf("Opening file <%s>\n", datafile);
	FILE *fp;
	fp = fopen(datafile, "w");
	printf("Writing data ...\n");
	for (j = L; j >= 1; j--) {
		for (i = 1; i <= L; i++) {
			fprintf(fp, " %4d", map[i][j]);
		}
		fprintf(fp, "\n");
	}
	printf("...done\n");
	fclose(fp);
	printf("File closed\n");

	int ncluster, maxsize;
	struct cluster *clustlist;
	int colour;
	int *rank;
	clustlist = (struct cluster *)arralloc(sizeof(struct cluster), 1, L * L);
	rank = (int *)arralloc(sizeof(int), 1, L * L);
	for (i = 0; i < L * L; i++) {
		rank[i] = -1;
		clustlist[i].size = 0;
		clustlist[i].id = i + 1;
	}
	for (i = 1; i <= L; i++) {
		for (j = 1; j <= L; j++) {
			if (map[i][j] != 0) {
				++(clustlist[map[i][j] - 1].size);
			}
		}
	}
	percsort(clustlist, L * L);
	maxsize = clustlist[0].size;
	for (ncluster = 0; ncluster < L * L && clustlist[ncluster].size > 0;
	     ncluster++)
		;
	if (MAX > ncluster) {
		MAX = ncluster;
	}
	for (i = 0; i < ncluster; i++) {
		rank[clustlist[i].id - 1] = i;
	}
	printf("Opening file <%s>\n", percfile);
	fp = fopen(percfile, "w");
	printf("Map has %d clusters, maximum cluster size is %d\n", ncluster,
	       maxsize);
	if (MAX == 1) {
		printf("Displaying the largest cluster\n");
	} else if (MAX == ncluster) {
		printf("Displaying all clusters\n");
	} else {
		printf("Displaying the largest %d clusters\n", MAX);
	}
	printf("Writing data ...\n");
	fprintf(fp, "P2\n");
	if (MAX > 0) {
		fprintf(fp, "%d %d\n%d\n", L, L, MAX);
	} else {
		fprintf(fp, "%d %d\n%d\n", L, L, 1);
	}
	for (j = L; j >= 1; j--) {
		for (i = 1; i <= L; i++) {
			colour = map[i][j];
			if (map[i][j] > 0) {
				colour = rank[map[i][j] - 1];
				if (colour >= MAX) {
					colour = MAX;
				}
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
