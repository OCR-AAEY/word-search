CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

all: rotate_gui

rotate_gui: main.c rotation.c ../image_loader/image_loading.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f rotate_gui
