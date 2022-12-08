CC := gcc
CFLAGS := -std=c99 -g -ggdb -Wall -Wextra -pedantic
IFLAGS := -Isrc -Iinclude
LFLAGS :=
DFLAGS :=

testbed:
	$(CC) $(CFLAGS) src/testbed.c src/es_impl.c -o bin/testbed $(IFLAGS) $(LFLAGS) $(DFLAGS)
