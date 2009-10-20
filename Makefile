.PHONY: all doc static quiet clean win nsis count

all:
	@scons -j 2 || echo "Get scons at www.scons.org or read the README for compilation instructions"
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

static:
	@scons -j 2 static
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

quiet:
	@scons -j 2 -s
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

clean:
	scons -c

# use to build on windows with mingw
win:
	@echo Building windows..
	@scons env=mingw

nsis:
	makensis misc/installer.nsi

count:
	wc src/*.cpp src/*.h src/factory/*.cpp src/factory/*.h src/object/*.cpp src/object/*.h src/network/*.cpp src/network/*.h src/level/*.cpp src/level/*.h src/environment/*.cpp src/environment/*.h src/util/*.cpp src/util/*.h src/script/*.cpp src/script/*.h src/script/modules/*.cpp src/script/modules/*.h src/menu/*.cpp src/menu/*.h src/gui/*.cpp src/gui/*.h src/mugen/*.cpp src/mugen/*.h src/mugen/*.l src/mugen/*.yy src/game/*.h src/game/*.cpp src/input/*.cpp src/input/*.h


doc:
	doxygen misc/doxygen.conf
