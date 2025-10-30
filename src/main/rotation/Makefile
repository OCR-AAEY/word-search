all:
	gcc test_rotation.c rotation.c ../image_loader/image_loading.c -DUNIT_TEST -o test_rotation `pkg-config --cflags --libs gtk+-3.0` -lm

clean:
	rm -f test_rotation

