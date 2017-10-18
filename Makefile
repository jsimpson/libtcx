CC=gcc
CFLAGS=-Wall -g -I/usr/local/opt/libxml2/include/libxml2/libxml/ -O3 -std=gnu11
LDFLAGS=-lxml2

all: georoute

georoute:
	$(CC) $(CFLAGS) georoute.c tcx.c -o georoute $^ $(LDFLAGS)

clean:
	rm georoute
