CC=gcc
CFLAGS=-g -Wall -std=c99 -I.
DEPS= server.h server_func.h hashtable.h 
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o server_func.o hashtable.o
	$(CC) -o server server.o server_func.o hashtable.o
clean: 
	rm server