CC=gcc
CFLAGS=-Wall


all: VirtualMemoryManager.o
	$(CC) $(CFLAGS) -o VirtualMemoryManager  VirtualMemoryManager.o
VirtualMemoryManager.o: VirtualMemoryManager.c
	$(CC) $(CFLAGS) -c VirtualMemoryManager.c
clean:
	rm -rf *.o
	rm -rf VirtualMemoryManager