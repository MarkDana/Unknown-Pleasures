CC=gcc
CFLAGS=-Wall


all: memoryManager.o
	$(CC) $(CFLAGS) -o a.out memoryManager.o
memoryManager.o: memoryManager.c
	$(CC) $(CFLAGS) -c memoryManager.c
clean:
	rm -rf *.o
	rm -rf a.out