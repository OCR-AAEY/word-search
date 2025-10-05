# Word Search Project

*Add description*

## Dependencies

- GTK for graphics
- none for image manipulation (or graphicsmagick/libMagickWand if too complicated)
- none for neuronal networks
- criterion for unit testing

## Tests

This project contains unit tests written thanks to [criterion](https://github.com/Snaipe/Criterion/tree/master?tab=readme-ov-file). To run them, make sure that criterion is installed and run:

```bash
make test
./run_tests
```

## Conventions

### Formatting

The code has to be formatted using clang-format according to the provided configuration (`./.clang-format`).

```bash
find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
```

### Includes

The C macro checks preventing multiple times imports should be done in the header files and not in the C source files.
