# Code structure
 * [global.h](code/global.h) contains global macros, global `#defines` and global structs that are used by the other files
 * [main.c](code/main.c) and [main.h](code/main.h) contain the driver code, cmd line arguments parsing and any other small and isolated application-specific functions 
 * [percolate.c](code/percolate.c) and [percolate.h](code/percolate.h) contain functions for generating, filling and manipulating the grid with respect to the percolation algorithm
 * [pgm.c](code/pgm.c) and [pgm.h](code/pgm.h) contain functions for the transformation of the original grid into a pgm-friendly data-structure as well as its output to a file, as there was a significant amount of code that pertained to this operation
 * [utils.c](code/utils.c) and [utils.h](code/utils.h) contain application-independent functions that were could even be used on other projects due to their high level of abstraction.
 * the [lib](code/lib/) folder contains the provided `arralloc` and `uni` library functions and are isolated from the main code

# Build script
This is done through a [Makefile](code/Makefile) which has the following main invocations:
* `make clean` will remove files generated but leave output of the execution
* `make realclean` which will remove generated and output files (stronger than just `clean`)
* `make` or  `make all` which will compile and link source code into an executable `main.out`
* for further information call `make help`
* variables, automatic variables, pattern rules and a self-documentation approach was used

# Command line arguments
The program should be invocated as `./main.out -x1 VALUE1 -x2 VALUE2` where `x` is the flag name of the corresponding configuration option and `VALUE` is the value to assign it. The possible flags are as follows:
* `-g` which is the grid size, ie. the number of rows (or columns as it is a square grid) [`default=20`]
* `-s` the value to use for the seed [`default=1564`]
* `-r` the value for `rho` (`[0,1]`) [`default=0.4`]
* `-m` the maximum number of clusters to show, if not present all clusters are displayed
* `-d` the name of the `.dat` file to output the grid to  [`default=map.dat`]
* `-p` the name of the `.pgm` file to output the grid (in a pgm format) to [`default=map.pgm`]

An example invocation would be: `./main.out -r 0.5 -g 7 -m 2` which would produce _dat_:
```
    3    3    3    0    0   26   26
    0    0    0   26   26   26   26
    0    0   10    0    0   26    0
    0    0    0   26   26   26   26
   20   20   20    0    0   26    0
    0    0   20    0   26   26   26
    0   24    0    0   26    0   26
```

and _pgm_:
```
P2
7 7
2
    2    2    2    2    2    0    0
    2    2    2    0    0    0    0
    2    2    2    2    2    0    2
    2    2    2    0    0    0    0
    1    1    1    2    2    0    2
    2    2    1    2    0    0    0
    2    2    2    2    0    2    0
```
without `-m 2` the _pgm_ will be:
```
P2
7 7
5
    2    2    2    5    5    0    0
    5    5    5    0    0    0    0
    5    5    4    5    5    0    5
    5    5    5    0    0    0    0
    1    1    1    5    5    0    5
    5    5    1    5    0    0    0
    5    3    5    5    0    5    0
```

# Notes on the refactoring process

### 1 - On the interpretation of the grid

While refactoring the original code it was considered that given a grid `int **`  its default position was top-left as the first value and bottom-right as the last one, and also that the each `int * ` in it corresponded to a line in the grid, and thus `grid[0][2]` corresponds to the third column of the first line.

This specification is important because, in the original code, some operations were performed in a way that was not coherent with the above model, for instance:
 * when writing the `.dat` file, the grid is iterated from right to left (so the output is a vertically mirrored version of the grid)
 * the same is done when writing to the `.pgm` file
 * likewise, when checking for percolations the `itop` and `ibot` are compared with `map[itop][L] == map[ibot][1]` which means it is actually comparing the left and write for a percolation instead of the top and bottom.

In light of these _inconsistencies_, it was decided that the state in which the grid is generated corresponds to the **top-left to botton-right** version and, as such, the above points were changed so that this model became the standard throughout the code. The difference between the final and original code is only in terms of perspective and in terms of checking for percolations from what is believed to be top-to-bottom instead of left-to-right. 

### 2 - On changing the order of a specific output
In the original code, the following print logic: 
```c
	printf("Map has %d clusters, maximum cluster size is %d\n", ncluster, maxsize);
	if (MAX == 1) {
		printf("Displaying the largest cluster\n");
	} else if (MAX == ncluster) {
		printf("Displaying all clusters\n");
	} else {
		printf("Displaying the largest %d clusters\n", MAX);
	}
```

was present after the `.pgm` file was opened for writing. While refactoring the code, this did not make a lot of sense as this is not good isolation of code. Therefore, this print logic was refactored and anticipated to a stage prior to opening the file so that they would, as they should, be properly isolated.

### 3 - Command line arguments management
To complement the above definition of all the available command line arguments and their default values, it is worth mentioning that these are managed internally with a struct (`options`) that holds their values instead of individual variables for each property, as this is more scalable. Additionally, this struct also holds some pre-calculated values that would have to be recalculated many times if this was not the case, like the number of cells in the grid (as the product of rows and columns). 

### 4 - Code style
The code style used is based on Google Style Guide and its specific options are available in the [settings.json](.vscode/settings.json) file. Although this is an IDE-specific file it was used during development and contains enough information for these settings to be applied in other IDEs, hence it being included in the project delivery. 