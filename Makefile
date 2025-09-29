CC=gcc

SRC_MAIN=$(shell find ./src/main -name *.c)
SRC_TEST=$(shell find ./src/test -name *.c)

test: $(SRC_TEST) $(SRC_MAIN)
	$(CC) -DUNIT_TEST -I./src/main -I./Unity/src $^ -o run_tests -Wall -Wextra -lcriterion

clean:
	$(RM) ./run_tests

.PHONY: clean