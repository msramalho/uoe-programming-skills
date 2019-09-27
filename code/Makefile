CC=gcc

.SILENT: clean realclean

all: main.o percolate.o uni.o arralloc.o utils.o pgm.o
	$(CC) -g main.o arralloc.o uni.o percolate.o utils.o pgm.o -lm -o main.out
	@# remove .o files after .out is ready
	@rm -f *.o

main.o : main.c main.h
	$(CC) -g -c main.c

percolate.o : percolate.c percolate.h
	$(CC) -g -c percolate.c

uni.o : lib/uni.c lib/uni.h
	$(CC)  -g -c lib/uni.c

arralloc.o : lib/arralloc.c lib/arralloc.h
	$(CC)  -g -c lib/arralloc.c

utils.o : utils.c utils.h
	$(CC)  -g -c utils.c

pgm.o : pgm.c pgm.h
	$(CC)  -g -c pgm.c
		
realclean: clean
	rm -f *.dat *.pgm
clean :
	rm -f *.o *.out