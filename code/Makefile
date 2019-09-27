CC=gcc

.SILENT: clean realclean

all: percolate.o uni.o arralloc.o utils.o
	$(CC) -g arralloc.o uni.o percolate.o utils.o -lm -o percolate.out
	# remove .o files after .out is ready
	rm -f *.o

percolate.o : percolate.c percolate.h lib/arralloc.h lib/uni.h
	$(CC) -g -c percolate.c

uni.o : lib/uni.c lib/uni.h
	$(CC)  -g -c lib/uni.c

arralloc.o : lib/arralloc.c lib/arralloc.h
	$(CC)  -g -c lib/arralloc.c

utils.o : utils.c utils.h
	$(CC)  -g -c utils.c
		
realclean: clean
	rm -f *.dat *.pgm
clean :
	rm -f *.o *.out