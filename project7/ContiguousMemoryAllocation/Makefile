CC=gcc
CFLAGS=-Wall


all: ContiguousMemoryAllocation.o
	$(CC) $(CFLAGS) -o allocator ContiguousMemoryAllocation.o
ContiguousMemoryAllocation.o: ContiguousMemoryAllocation.c
	$(CC) $(CFLAGS) -c ContiguousMemoryAllocation.c
clean:
	rm -rf *.o
	rm -rf allocator