.PHONY: all doc static quiet clean win nsis count test

all:
	@scons -j `python misc/cores.py` || echo "Get scons at www.scons.org or read the README for compilation instructions"
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

test:
	@scons -j `python misc/cores.py` test-all
	@python misc/gnome-notify "Finished compiling" 2>/dev/null
	@echo
	@echo Running tests
	build/test/token/test
	build/test/regex/test
	@-rm /tmp/token*

# usually only be executed by the buildbot script
# gch is broken for some reason: http://gcc.gnu.org/ml/gcc-help/2005-02/msg00007.html
buildbot: buildbot_normal buildbot_static buildbot_editor buildbot_cmake
buildbot_normal:
	@scons -j `python misc/cores.py` gch=0 colors=0
	@scons -j `python misc/cores.py` gch=0 colors=0 sdl=1
buildbot_static:
	@scons static -j `python misc/cores.py` gch=0 colors=0
buildbot_editor:
	cd editor; $(MAKE)
buildbot_cmake:
	-rm -rf b
	-mkdir b
	cd b; cmake ..; $(MAKE)

static:
	@scons -j `python misc/cores.py` static
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

quiet:
	@scons -j `python misc/cores.py` -s
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

clean:
	scons -c

win_clean:
	@scons -c env=mingw

# use to build on windows with mingw
win:
	@echo Building windows..
	@scons env=mingw

nsis:
	makensis misc/installer.nsi

count:
	bash -c 'wc src/{.,factory,object,network,level,environment,util,script,script/modules,menu,gui,mugen,game,input,util/allegro,util/sdl}/*.{h,cpp} src/mugen/parser/*.peg'


doc:
	doxygen misc/doxygen.conf
