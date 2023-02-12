.PHONY: debug clean

debug: build-debug
	meson compile -C build-debug

build-debug:
	mkdir build-debug
	meson setup build-debug

clean:
	rm -rf build-debug
