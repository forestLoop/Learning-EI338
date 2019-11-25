CC=gcc
CFLAGS=-Wall

memory_allocator: memory_allocator.o memory_allocator.c
	$(CC) $(CFLAGS) -o memory_allocator memory_allocator.o

memory_allocator.o: memory_allocator.c
	$(CC) $(CFLAGS) -c memory_allocator.c

clean:
	rm *.o
	rm memory_allocator
