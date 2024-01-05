.PHONY: debug clean

debug: build-debug
	(cd build-debug; meson configure -Dbuild_tests=false)
	meson compile -C build-debug
	cp build-debug/paintown .

build-debug:
	mkdir build-debug
	meson setup build-debug

test: build-debug
	(cd build-debug; meson configure -Dbuild_tests=true)
	meson compile -C build-debug

clean:
	rm -rf build-debug
