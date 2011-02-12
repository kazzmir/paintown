.PHONY: all doc static quiet clean win nsis count test

osx_version="3-5-0"

all:
	@scons -j `python misc/cores.py` || echo "Get scons at www.scons.org or read the README for compilation instructions"
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

# Same as `all' but shows time taken by scons
time:
	@scons --debug=time -j `python misc/cores.py` || echo "Get scons at www.scons.org or read the README for compilation instructions"
	@python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

test:
	@scons --keep-going -j `python misc/cores.py` test-all
	@python misc/gnome-notify "Finished compiling" 2>/dev/null
	@-rm /tmp/token*

install:
	sh install.sh

# usually only be executed by the buildbot script
# gch is broken for some reason: http://gcc.gnu.org/ml/gcc-help/2005-02/msg00007.html
buildbot: buildbot_normal buildbot_static buildbot_editor buildbot_cmake
buildbot_normal:
	@scons -j `python misc/cores.py` gch=0 colors=0
	@scons -j `python misc/cores.py` gch=0 colors=0 allegro=1
buildbot_static:
	@scons static -j `python misc/cores.py` gch=0 colors=0
buildbot_editor:
	cd editor; $(MAKE)
buildbot_cmake:
	-rm -rf b
	-mkdir b
	cd b; cmake ..; $(MAKE)

# Special target for setting up an OSX build
osx:
	@scons -j `python misc/cores.py` static data=paintown-${osx_version}.app/Contents/Resources/data

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
	@echo Building windows (SDL)..
	@scons env=mingw

winallegro:
	@echo Building windows (Allegro)..
	@scons env=mingw allegro=1

nsis:
	makensis /V2 misc/installer.nsi

count:
	bash -c 'wc src/{.,factory,object,network,level,environment,util,script,script/modules,menu,gui,mugen,game,input,util/allegro,util/sdl}/*.{h,cpp} src/mugen/parser/*.peg'


doc:
	doxygen misc/doxygen.conf
