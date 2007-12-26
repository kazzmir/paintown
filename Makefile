all:
	scons

count:
	wc src/*.cpp src/*.h src/factory/*.cpp src/factory/*.h src/object/*.cpp src/object/*.h src/network/*.cpp src/network/*.h src/level/*.cpp src/level/*.h src/environment/*.cpp src/environment/*.h src/util/*.cpp src/util/*.h
