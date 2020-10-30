CC=gcc
CFLAGS=-g -Wall -std=c99 -I.
DEPS= client.h client_func.h peer.h
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: client.o client_func.o peer.o
	$(CC) -o client client.o client_func.o peer.o
clean: 
	rm client