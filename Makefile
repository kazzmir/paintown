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
	mkdir -p build && cd build && cmake .. && make

build-docker:
	docker build `pwd` -t paintown-builder
	docker run -it -v `pwd`:/workdir -w /workdir paintown-builder make build-cmake