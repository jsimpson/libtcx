CC=gcc
CFLAGS=-Wall -g -I/usr/local/opt/libxml2/include/libxml2/libxml/
LDFLAGS=-lxml2

all: fasttcx

fasttcx:
	$(CC) $(CFLAGS) fasttcx.c -o fasttcx $^ $(LDFLAGS)

clean:
	rm fasttcx
