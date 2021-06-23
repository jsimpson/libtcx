CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wmissing-prototypes -pedantic -g -I/usr/include/libxml2/ -O3 -std=c99
LDFLAGS=-lxml2 -lm

all: tcx library

tcx:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE main.c tcx.c -o tcx $^ $(LDFLAGS)

library:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE -shared tcx.c -o libtcx.so $^ $(LDFLAGS)

clean:
	rm tcx
	rm libtcx.so
