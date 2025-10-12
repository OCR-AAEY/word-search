CC=gcc
CFLAGS = -Wall -Wextra
SRC_MAIN=$(shell find ./src/main -name *.c)
SRC_TEST=$(shell find ./src/test -name *.c)
INCLUDE_FLAGS= -I./src/main
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)

image_loader: $(shell find ./src/main/image_loader -name *.c)
	$(CC) $(CFLAGS) $^ -o $@ $(GTK_FLAGS)

clean:
	$(RM) run_tests solver img-loader

.PHONY: clean solver