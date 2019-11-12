CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: buffer.o main.o
	$(CC) $(CFLAGS) -o pc buffer.o main.o $(PTHREADS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

clean:
	rm -rf *.o
	rm -rf pc
