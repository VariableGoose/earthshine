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
	@mkdir -p bin
	$(CC) $(CFLAGS) src/testbed.c src/es_impl.c -o bin/testbed $(IFLAGS) $(LFLAGS) $(DFLAGS)

test: unit_test_dll
	@mkdir -p bin
	$(CC) $(CFLAGS) src/unit_tester.c src/es_impl.c -o bin/unit_tester $(IFLAGS) $(LFLAGS) $(DFLAGS)
	./bin/unit_tester $(UNIT_TESTS) bin/unit_tests.dll

unit_test_dll:
	@mkdir -p bin
	$(CC) $(CFLAGS) -shared -fPIC src/es_impl.c $(UNIT_TESTS) -o bin/unit_tests.dll $(IFLAGS) $(LFLAGS) $(DFLAGS)
