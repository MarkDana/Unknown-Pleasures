CC=g++
CFLAGS=-Wall

all: BankerAlgorithm.o
	$(CC) $(CFLAGS) -o BankerAlgorithm  BankerAlgorithm.o
buffer.o: BankerAlgorithm.cpp
	$(CC) $(CFLAGS) -c BankerAlgorithm.cpp

clean:
	rm -rf *.o
	rm -rf BankerAlgorithm