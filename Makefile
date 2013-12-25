.PHONY: all doc static quiet clean win nsis count test

osx_version="3-6-1"
# scons:=scons -j `python misc/cores.py`
scons:=scons
gnome_notify:=python misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"

all:
	@$(scons) || echo "Get scons at www.scons.org or read the README for compilation instructions"
	@$(gnome_notify)

# Convenient target for building 32-bit and 64-bit nacl
nacl:
	nacl=1 nacl_32=1 $(scons)
	nacl=1 nacl_64=1 $(scons)
	@$(gnome_notify)

# Same as `all' but shows time taken by scons
time:
	@$(scons) --debug=time || echo "Get scons at www.scons.org or read the README for compilation instructions"
	@$(gnome_notify)

test:
	@$(scons) --keep-going test-all
	@$(gnome_notify)
	@-rm /tmp/token*

install:
	sh install.sh

# usually only be executed by the buildbot script
# gch is broken for some reason: http://gcc.gnu.org/ml/gcc-help/2005-02/msg00007.html
buildbot: buildbot_normal buildbot_static buildbot_editor buildbot_cmake
buildbot_normal:
	@$(scons) gch=0 colors=0
	# @$(scons) gch=0 colors=0 allegro=1
buildbot_static:
	@$(scons) static gch=0 colors=0
buildbot_editor:
	cd editor; $(MAKE)
buildbot_cmake:
	-rm -rf b
	-mkdir b
	cd b; cmake ..; $(MAKE)

# Special target for setting up an OSX build
osx:
	@$(scons) static data=paintown-${osx_version}.app/Contents/Resources/data

static:
	@$(scons) static
	@$(gnome_notify)

quiet:
	@$(scons) -s
	@$(gnome_notify)

clean:
	-rm -rf build
	-rm -rf .sconf_temp .sconsign.dblite config.log fail.log *.pyc

win_clean:
	@scons -c env=mingw

# use to build on windows with mingw
win:
	@echo Building windows (SDL)..
	@scons env=mingw

winallegro:
	@echo Building windows (Allegro)..
	@scons env=mingw allegro=1

winallegro5:
	@echo Building windows (Allegro5)..
	@scons env=mingw allegro5=1

nsis:
	makensis /V2 misc/installer.nsi

count:
	bash -c 'wc src/{.,factory,object,network,level,environment,util,script,script/modules,menu,gui,mugen,game,input,util/allegro,util/sdl}/*.{h,cpp} src/mugen/parser/*.peg'


doc:
	doxygen misc/doxygen.conf

# Build wii
wii:
	@echo Building for wii
	@DEVKITPRO=/opt/devkitpro DEVKITPPC=/opt/devkitpro/devkitPPC/ wii=1 $(scons)
	@$(gnome_notify)
