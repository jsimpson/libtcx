CC=gcc
CFLAGS=-Wall -g -I/usr/local/opt/libxml2/include/libxml2/libxml/ -O3 -std=gnu11
LDFLAGS=-lxml2

all: fasttcx

fasttcx:
	$(CC) $(CFLAGS) main.c tcx.c -o fasttcx $^ $(LDFLAGS)

library:
	$(CC) $(CFLAGS) -shared tcx.c -o libfasttcx.so $^ $(LDFLAGS)

clean:
	rm fasttcx
	rm libfasttcx.so
