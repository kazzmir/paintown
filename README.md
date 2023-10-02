# [Paintown](http://paintown.org)
by [Jon Rafkind](https://github.com/kazzmir)
```
Updated on 04/11/2018
```

## Table of Contents

* [Releases](#releases)
* [Get Source](#get-source)
* [Requirements to build](#requirements-to-build)
* [Compilation](#compilation)
* [Install](#install)
  * Linux
* [Scripting](#scripting)
* [OS Specific Instructions](#os-specific-instructions)
* [Thanks](#thanks)
* [IRC](#irc)


## Releases

#### Current 
**[3.6.0](https://github.com/kazzmir/paintown/releases/tag/v3.6.0)**

#### All Releases
**[Click Here](https://github.com/kazzmir/paintown/releases)**

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
* Software construction tools (either or)
	* [Scons](http://www.scons.org)
	* [Cmake](http://www.cmake.org)
* [zlib](http://www.zlib.net/)
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* pthreads - If you are on Unix you should have this or you can install glibc and you will get it. (On Windows get it [here](http://sources.redhat.com/pthreads-win32/))
* freetype - Installed on most Unix systems along with X11. (On windows get it [here](http://gnuwin32.sourceforge.net/packages/freetype.htm))
* Scripting
	* [python](http://www.python.org) and its headers (usually this comes with python)
* [Allegro 5.x](https://github.com/liballeg/allegro5)
* [R-Tech1](http://github.com/kazzmir/r-tech1)
	* You can clone it directly in the paintown directory and it will be built automatically if using Scons otherwise you will need to install it separately

## Compilation
You can build paintown with either Scons or Cmake.

#### Build Environment Flags
* **DEBUG** - set to 0/1 (on/off in cmake) to enable or disable debugging
* **LLVM** - set to 0/1 (on/off in cmake) to build with clang
* **DATA_PATH** - set this to the desired data path
	* With Scons it will default to data
	* With Cmake it will default to `${CMAKE_SOURCE_DIR}/data`
	* _*Note: if planning on installing recommend setting it to something like /usr/local/games/paintown_

#### Linux

##### Using Scons to build Paintown
`$ make` or `$ scons`

##### Using Cmake to build Paintown
```
$ mkdir b
$ cd b
$ cmake ..
$ make
```

  
##### Ubuntu users
```
Note, this script hasn't been updated so it may be invalid.
```
There is a script that will install the necessary packages and then invoke the compilation scripts.
Type this: 
`
$ ./easy-compile-ubuntu
`

## Install

### Linux

##### Scons
After building with Scons use the install script below
`
$ ./install.sh -d <data> -b <bin>
`

The script will prompt you to input these directories if you do not
give -d and/or -b.

After running install.sh (assuming default values for <data> and <bin>) you can run
`
$ /usr/games/paintown
`
##### Cmake
After building with Cmake run `sudo make install` see notes in [Compilation](#compilation) about the data directory

### Anything other than Linux
Copy the binary and data somewhere. Otherwise use the prepackaged installers.

## Scripting

A python engine has been added to paintown so you can script a level. 
See [scripting.txt](scripting.txt) for more details.

## OS Specific Instructions

```
Note, that these are old instructions and may be invalid.
```
### OSX

`$ scons`

How to build a .dmg file of paintown:
```
$ scons static
$ misc/make-osx-bundle
```

This will create paintown-x-y.app/ and paintown-x-y.dmg.

You need [buildDmg.pl](http://www.objectpark.org/buildDMG.html)
I have placed the perl script in the [misc directory](misc/buildDMG.pl), so just put
that in a directory that is part of your path.

### Windows

##### Mingw
Type
`
$ scons env=mingw
`

##### Scons
`
c:\> cd path-to-paintown
c:\path-to-paintown> scons
`

To build a statically linked binary give the 'static' target
$ scons static

##### Cmake
```
c:\> cd path-to-paintown
c:\path-to-paintown> mkdir b
c:\path-to-paintown> cd b
c:\path-to-paintown> cmakesetup ..
c:\path-to-paintown> make
```

The executable will end up in bin/paintown.exe

##### Cygwin

Don't use cygwin's tools. Install python/scons/mingw for regular windows and use that.
```
$ export CYGWIN=1
$ c:\progra~1\Python24\Scripts\scons
```


_Note_ it may be easier to get the gcc4.5 environment from svn:
http://svn.code.sourceforge.net/p/paintown/code/build-environments/gcc4.5
Get unxutils for things like mv, rm, cp - http://www.sf.net/projects/unxutils
Find the .zip. It unzips as usr\local\wbin, so put that in your path or just
copy all the files in usr\local\wbin to your c:\mingw\bin directory.
For libpng/zlib see here: http://tjaden.strangesoft.net/loadpng/mingw.html
Python 2.4.4 (yes this is old but the game is hardcoded to use it):
http://www.python.org/download/releases/2.4.4/
Add c:\python24 to your PATH
Copy libs\libpython24.a to c:\mingw\lib
Scons - http://www.scons.org
Then add c:\python24\scripts to your PATH
pthreads - http://sources.redhat.com/pthreads-win32/
get pthreads-win32-2-8-0-release.exe, let it unzip somewhere, then copy
the stuff in prebuilt\include to your mingw\include directory. copy the
.a files to c:\mingw\lib
allegro - http://alleg.sf.net
get the 4.2.3 package and dx70_mgw.zip
Get freetype from http://gnuwin32.sourceforge.net/packages/freetype.htm. Copy
the resulting headers into your include path. Don't copy in freetype2, just the
path underneath it, freetype2\freetype.
Gnu regex: http://gnuwin32.sourceforge.net/packages/regex.htm
Use the dependancy walker: http://www.dependencywalker.com/ to check that
all required dlls are around.

##### Docker

Building
```shell
make build-docker
```
The executable will end up in build/bin/paintown

Running
```shell
./build/bin/paintown
```


## Thanks
* [Rain sound - by inchadney.](http://freesound.iua.upf.edu/usersViewSingle.php?id=28867)
* [DUMB](http://dumb.sf.net)
* [loadpng](http://tjaden.strangesoft.net/loadpng/)


## IRC
Join the chat at [libera](https://web.libera.chat/#paintown)
