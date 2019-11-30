CC=gcc
CFLAGS=-Wall

vm_manager: vm_manager.o vm_manager.c
	$(CC) $(CFLAGS) -o vm_manager vm_manager.o

vm_manager.o: vm_manager.c
	$(CC) $(CFLAGS) -c vm_manager.c

clean:
	rm -f *.o vm_manager.exe vm_manager
