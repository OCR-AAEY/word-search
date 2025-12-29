# OCR and Word Search Solver

This project is a [word search puzzle](https://en.wikipedia.org/wiki/Word_search) solver written in C. It takes a photo of the grid and the word list as input and solves the puzzle, i.e. it highlights the sought words in the grid.

As required by the specifications, the Makefile contains the `make all` and `make clean` rules. Note that any of the executable files detailed below is built when `make all` is run. All executable built by the project will be located at the root of the project i.e. `./word-search`.

As a remainder, this project is meant to be run on the PIE, which means that there is no guarranty for it to work on a different operating system or linux distribution. For more information about the dependencies, check out the **Requirements** section of this document.

## The main program

To compile the app binary, use :

```bash
make app
```

This will create a new executable called `app` at the root. Then the `app` executable file can be run. When executed, the app GUI is launched and can be used to solve a word search grid.

## Solver

To compile the solver binary, use:

```bash
make solver
```

This will create a new executable called `sovler` at the root. Then the `solver` executable file can be run according to the specifications.

## Location

This part combines image processing, rotation and various computation methods to locate and extract letters from the grid and the word list.

To compile the binary, use :

```bash
make location
```
This will create a new executable file called `location` at the root of the repository.

### Usage
The `location` program can be used as follows :

```
Usage: ./location [LVL] [IMG]
- LVL : The level of the image to load (1 or 2).
- IMG : The number of the image to load (1 or 2).
```

### Example usage
To apply the location procedure on the Level 1 Image 1, you can run the following command
```bash
./location 1 1
```

###  Extracted images architecture

The extracted images are stored in the `extracted/` folder. Its architecture is as follows :
```
extracted/
│
├── examples/
│   └── (Illustration images showing the different steps of the extraction process)
│
├── grid/
│   └── (Images of the letters from the grid, named according to their position)
│
└── words/
    └── word_X/                  # A folder dedicated to the Xth word detected
        ├── full_word.png        # Image of the entire detected word
        └── letters/
            └── Y.png            # Image of the Yth letter within the word
```

## XNOR neural network

The XNOR neural network is split into two parts: the training and the application of the neural network.

### XNOR train

The XNOR neural network training program trains a multilayer perceptron to predict the XNOR function. The resuling neural network will be saved into `./xnor.net`

The XNOR neural network training algorithm can be compiled using:

```bash
make xnor_train
```

A `xnor_train` executable file will be created. To run it, simply run `./xnor_train`.

### XNOR run

The XNOR run program loads the neural network saved is the file `./xnor.net`. Therefore, the XNOR train program should be run before running this one.

The XNOR run program can be compiled using:

```bash
make xnor_run
```

A `xnor_run` executable file will be created. To run it, use:

```
./xnor_run [A] [B]
```

Where `A` and `B` are either 0 or 1. The program will then print the neural network result as a double between 0 and 1 along with the rounded value that gives the predicted result of A XNOR B.

# Contributing

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

This command will create a `build/` directory in the source root, which contains all the object files. The binaries will be located at the source root.
You can then run the desired binaries using `./executable-name`.

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
