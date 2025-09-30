CC=gcc

SRC_MAIN=$(shell find ./src/main -name *.c)
SRC_TEST=$(shell find ./src/test -name *.c)

solver: $(shell find ./src/main/solver -name *.c)
	$(CC) $^ -o solver -Wall -Wextra

test: $(SRC_TEST) $(SRC_MAIN)
	$(CC) -DUNIT_TEST -I./src/main $^ -o run_tests -Wall -Wextra -lcriterion

clean:
	$(RM) ./run_tests
	$(RM) ./solver

.PHONY: clean