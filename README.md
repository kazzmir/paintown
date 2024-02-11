## [<img src="https://paintown.org/assets/logo.png" alt="Paintown">](https://paintown.org)
> by **[Jon Rafkind](https://github.com/kazzmir)**
> *(Updated on 02/07/2023)*

Paintown is a 2-d fighting game that is similar in style to Streets of Rage. Paintown also comes with a MUGEN engine.

## Table of Contents

* [Releases](#releases)
* [Get Source](#get-source)
* [Requirements to build](#requirements-to-build)
* [Compilation](#compilation)
* [Scripting](#scripting)
* [Thanks](#thanks)
* [Chat](#chat)

## Builds
![Linux Ubuntu Build](https://github.com/kazzmir/paintown/actions/workflows/compile.yml/badge.svg)
![Mac Arm Build](https://github.com/kazzmir/paintown/actions/workflows/compile-mac-arm.yml/badge.svg)
![Mingw Cross-Compile Build](https://github.com/kazzmir/paintown/actions/workflows/mingw-cross.yml/badge.svg)

## Releases

- **[Latest](https://github.com/kazzmir/paintown/releases/latest)**
- **[All](https://github.com/kazzmir/paintown/releases)**

## Get Source

If you are reading this you might already have the source. If not get it from the
[Paintown Repository](https://github.com/kazzmir/paintown).

Additional data which has collections of characters, levels and other items can be obtained in the [Paintown Data Repository](https://github.com/kazzmir/paintown-data).
```
Note to distribution maintainers it is recommended to omit the data from the above repository since it may contain copyrighted material.
The included data in the Paintown Repository should be sufficient for running the application (which excludes levels and content).
```
  
## Requirements to build

You need the following things to compile Paintown:
* [GNU build utils (gcc and g++)](http://www.gnu.org/software/gcc/)
	* *Note that you will need to have c++11 support*
* Software construction tools
	* [Meson](https://mesonbuild.com/)
* [zlib](http://www.zlib.net/)
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2)
	* SDL2_image
	* SDL2_ttf
	* SDL2_mixer
	* SDL2_gfx
* Optional Libs
	* OGG / Vorbis / Vorbis File
	* MPG123
* Scripting
	* [python](http://www.python.org) and its headers (usually this comes with python)

## Compilation
You can build paintown with [Meson](https://mesonbuild.com) using the Makefile.

### Linux
##### Build Debug
```
$ make
# To build tests do:
$ make test
```

##### Cross-compile for windows with mingw
```
$ make mingw
# To build tests do:
$ make testmingw
```
> *Note this build depends on x86_64-w64-mingw32 and hasn't been tested on any other distro than debian based ones*

### Ubuntu
There are several scripts that will install the necessary packages and then invoke the compilation
#### Basic build
`
$ ./easy-compile-ubuntu
`
#### Mingw build (cross compilation for windows)
`
$ ./easy-compile-mingw
`
> Binaries are dumped into the mingw-bin directory. Make sure you copy zlib1.dll with your binary.

### Fedora
`
$ ./easy-compile-fedora
`

### Mac Arm
`
$ ./easy-compile-mac-arm
`

## Scripting

A python engine has been added to paintown so you can script a level. 
See [scripting.txt](scripting.txt) for more details.

## Thanks
* [Rain sound - by inchadney.](http://freesound.iua.upf.edu/usersViewSingle.php?id=28867)
* [DUMB](http://dumb.sf.net)

## Chat
* Join us on [Discord](https://discord.com/invite/4SRhCE2Gch)
* Join us on [IRC](https://web.libera.chat/#paintown)
