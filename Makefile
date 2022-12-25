CC := gcc
CFLAGS := -std=c99 -g -ggdb -Wall -Wextra -pedantic
IFLAGS := -Isrc -Iinclude
LFLAGS :=
DFLAGS :=

UNIT_TESTS := $(wildcard tests/*.c)

ifeq ($(OS), Windows_NT)
	LFLAGS += -I$(VULKAN_SDK)/Include
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LFLAGS += -lpthread -lm -lX11
	endif
endif

testbed:
	$(CC) $(CFLAGS) src/testbed.c src/es_impl.c -o bin/testbed $(IFLAGS) $(LFLAGS) $(DFLAGS)

unit_test:
	$(CC) $(CFLAGS) -shared -fPIC src/es_impl.c $(UNIT_TESTS) -o bin/unit_tests.dll $(IFLAGS) $(LFLAGS) $(DFLAGS)
