.PHONY: debug clean

debug: build-debug
	(cd build-debug; meson configure -Dbuild_tests=false)
	meson compile -C build-debug
	cp build-debug/paintown .

clang:
	test -d build-debug || (mkdir build-debug && CXX=clang++ CC=clang CXXFLAGS=-stdlib=libc++ meson setup build-debug)
	(cd build-debug; meson configure -Dbuild_tests=false)
	meson compile -C build-debug
	cp build-debug/paintown .

build-debug:
	mkdir build-debug
	meson setup build-debug

release: build-release
	(cd build-release; meson configure -Dbuild_tests=false -Drelease=true -Doptimization=2)
	meson compile -C build-release
	cp build-release/paintown .
	strip paintown

build-release:
	mkdir build-release
	meson setup build-release

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

build-psp:
	mkdir build-psp
	misc/psp-environment.sh
	#meson setup --cross-file psp-cross.txt misc/psp build-psp
	meson setup --cross-file psp-cross.txt build-psp

psp: build-psp
	(cd build-psp; meson configure -Dbuild_tests=false)
	meson compile  -C build-psp

clean:	clean-psp
	rm -rf .tmp build build-debug build-release build-mingw

clean-psp:
	rm -rf build-psp paintown-psp*
