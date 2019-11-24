CC=gcc
CFLAGS=-Wall

banker: banker.o banker.c
	$(CC) $(CFLAGS) -o banker banker.o

banker.o: banker.c
	$(CC) $(CFLAGS) -c banker.c

clean:
	rm *.o
	rm banker
