CC=gcc
CFLAGS = -Wall -Wextra
SRC_MAIN=$(shell find ./src/main -name *.c)
SRC_TEST=$(shell find ./src/test -name *.c)
TEST_FLAGS=-DUNIT_TEST -I./src/main
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)

solver: $(shell find ./src/main/solver -name *.c)
	$(CC) $(CFLAGS) $^ -o $@

test: $(SRC_TEST) $(SRC_MAIN)
	$(CC) $(CFLAGS) $(TEST_FLAGS) $^ -o run_tests $(GTK_FLAGS) -lcriterion

image_loader: $(shell find ./src/main/image_loader -name *.c)
	$(CC) $(CFLAGS) $^ -o $@ $(GTK_FLAGS)

clean:
	$(RM) run_tests solver image_loader

.PHONY: clean solver image_loader
