CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wmissing-prototypes -pedantic -g -I/usr/include/libxml2/ -O3 -std=c99
LDFLAGS=-lxml2 -lm

all: fasttcx library

fasttcx:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE main.c tcx.c -o fasttcx $^ $(LDFLAGS)

library:
	$(CC) $(CFLAGS) -fPIC -D_DEFAULT_SOURCE -shared tcx.c -o libfasttcx.so $^ $(LDFLAGS)

clean:
	rm fasttcx
	rm libfasttcx.so
