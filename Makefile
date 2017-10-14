CC=gcc
CFLAGS=-Wall -g -I/usr/local/opt/libxml2/include/libxml2/libxml/ -O3 -std=gnu11
LDFLAGS=-lxml2

all: fasttcx

fasttcx:
	$(CC) $(CFLAGS) fasttcx.c tcx.c utils.c -o fasttcx $^ $(LDFLAGS)

clean:
	rm fasttcx
