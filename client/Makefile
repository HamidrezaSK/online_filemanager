client: client.c client_func.c
	clang -g -Wall -std=c99 -o client client.c client_func.c -I.
clean: 
	rm client

CC=gcc
CFLAGS=-g -Wall -std=c99 -I.
DEPS= client.h client_func.h 
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

client: client.o client_func.o
	$(CC) -o client client.o client_func.o
clean: 
	rm client