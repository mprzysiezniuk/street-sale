CC=gcc
CFLAGS=-Wall
DEPS=

all: Client final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Client: client.o
	$(CC) -o Client client.o
