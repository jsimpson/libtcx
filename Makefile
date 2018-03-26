CC=gcc
CFLAGS=-Wall -g -I/usr/include/libxml2/ -O3 -std=gnu11
LDFLAGS=-lxml2

all: fasttcx library

fasttcx:
	$(CC) $(CFLAGS) -fPIC main.c tcx.c -o fasttcx $^ $(LDFLAGS)

library:
	$(CC) $(CFLAGS) -fPIC -shared tcx.c -o libfasttcx.so $^ $(LDFLAGS)

clean:
	rm fasttcx
	rm libfasttcx.so
