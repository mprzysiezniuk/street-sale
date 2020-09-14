CC=gcc
CFLAGS=-Wall
DEPS=

all: Distributor final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Distributor: distributor.o
	$(CC) -o Distributor distributor.o
