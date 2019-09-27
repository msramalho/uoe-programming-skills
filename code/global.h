#include "utils.h"
#include "lib/arralloc.h"

#define FULL 0

struct cluster {
	int id;
	int size;
};

typedef struct {
	int size;
	int max;
	int maxClusterDisplay;
	int seed;
	float rho;
	char *dataFile;
	char *percFile;
} options;