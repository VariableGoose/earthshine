CC := gcc
CFLAGS := -std=c99 -g -ggdb -Wall -Wextra -pedantic
IFLAGS := -Isrc -Iinclude
LFLAGS :=
DFLAGS :=

ifeq ($(OS), Windows_NT)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LFLAGS += -lpthread -lm
	endif
endif

testbed:
	$(CC) $(CFLAGS) src/testbed.c src/es_impl.c -o bin/testbed $(IFLAGS) $(LFLAGS) $(DFLAGS)
