CC=gcc
CFLAGS=-Wall
DEPS=

all: Seller final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Seller: seller.o
	$(CC) -o Seller seller.o
