CC=gcc

CFLAGS=-Wall -g

all: fasttcx

fasttcx:
	$(CC) $(CFLAGS) src/main.c -o fasttcx

clean:
	rm fasttcx
