# Word Search Project

*Add description*

## Dependencies

- GTK for graphics
- none for image manipulation (or graphicsmagick/libMagickWand if too complicated)
- none for neuronal networks

## Conventions

The code has to be formatted using clang-format according to the provided configuration (`./.clang-format`).
```bash
find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
```

The C macro checks preventing multiple times imports should be done in the header files and not in the C source files.
