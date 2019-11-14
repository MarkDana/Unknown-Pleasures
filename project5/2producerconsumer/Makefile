# makefile for bounded buffer

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: main.o buffer.o
	$(CC) $(CFLAGS) -o example main.o buffer.o $(PTHREADS)
buffer.o: buffer.h buffer.c
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)
clean:
	rm -rf *.o
	rm -rf example