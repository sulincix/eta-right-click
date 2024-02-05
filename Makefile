debug: clean
	meson setup build -Ddebug=true
	ninja -C build

build: clean
	meson setup build -Ddebug=false
	ninja -C build

clean:
	rm -rf build || true