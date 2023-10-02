.PHONY: debug clean

debug: build-debug
	meson compile -C build-debug
	cp build-debug/paintown .

build-debug:
	mkdir build-debug
	meson setup build-debug

clean:
	rm -rf build-debug build paintown

build-cmake:	clean
	mkdir -p build && cd build && cmake -DDATA_PATH=data .. && make

build-docker:
	docker build `pwd` -t paintown && docker run --rm -it -u `id -u`:`id -g` -v `pwd`:/workdir -w /workdir paintown