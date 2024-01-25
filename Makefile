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

mingw: build-mingw
	(cd build-mingw; meson configure -Dbuild_tests=false)
	meson compile -C build-mingw

build-mingw:
	mkdir build-mingw
	misc/mingw-environment.sh
	meson setup --cross-file mingw_x86_64.txt build-mingw

testmingw: build-mingw
	(cd build-mingw; meson configure -Dbuild_tests=true)
	meson compile -C build-mingw
	mkdir -p mingw-bin
	cp build-mingw/paintown.exe mingw-bin
	find build-mingw/src/test -type f -name \*.exe -exec sh -c "cp {} mingw-bin" \;

clean:
	rm -rf build-debug
	rm -rf build-mingw
