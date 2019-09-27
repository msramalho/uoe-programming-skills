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