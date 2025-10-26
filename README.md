# Word Search Project

This project is a [word search puzzle](https://en.wikipedia.org/wiki/Word_search) solver written in C. It takes a photo of the grid and the word list as input and solve the puzzle, i.e. it highlights the sought words in the grid.

# Compilation

## Requirements

This project requires GTK3 for graphics handling, and [Criterion](https://github.com/Snaipe/Criterion) for unit testing. The latter does not have to be installed to build and run the project.

It also requires clang-format 16 or higher in order to use `make format`.

### Ubuntu

On Ubuntu, it is possible to install the dependencies through:

```bash
sudo apt install build-essential libgtk-3-dev libcriterion-dev clang-format
```

## Building on Linux

To compile the project, use the following command in the source root:

```bash
make all
```

This command will create a `build/` directory in the source root, which contains all the target builds and the object files. You can then run the desired binaries using `./build/executable-name`.

## Running the tests

This project contains unit tests written for [Criterion](https://github.com/Snaipe/Criterion). To run them, first make sure that criterion is installed and then use:

```bash
make test
```

# Conventions

## Formatting

The code has to be formatted using clang-format according to the provided configuration (`./.clang-format`). It can be formatted using:

```bash
make format
```

The latter format make rule is an alias for:

```bash
find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
```

## Includes

The C macro checks preventing multiple times imports should be done in the header files and not in the C source files.
