CFLAGS:=-Wall -Wextra -Werror -Wno-unused
PREFIX:=/usr
build: clean
	mkdir build
	$(CC) $(CFLAGS) -g3 $(wildcard src/*.c) -Iinclude -o build/main `pkg-config --cflags --libs libevdev`

clean:
	rm -rf build
