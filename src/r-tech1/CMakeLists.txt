# -------------------------------------------------------
# util cmake build script for r-tech1.
# Written by: juvinious
# Modified by: kazzmir
# -------------------------------------------------------
cmake_minimum_required(VERSION 3.7)
project (r-tech1 C CXX)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)

# -------------------------------------------------------
# Directory in which extra macros can be found
# -------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../../cmake)


# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
#file(GLOB UTIL_SRC *.cpp *.h */*.h */*.cpp PARENT_SCOPE)
set(UTIL_SRC
argument.cpp
configuration.cpp
network/network.cpp
network/chat.cpp
network/irc.cpp
token.cpp
resource.cpp
tokenreader.cpp
timedifference.cpp
debug.cpp
timer.cpp
init.cpp
utf.cpp
console.cpp
input/keyboard.cpp
loading.cpp
messages.cpp
graphics/bitmap.cpp
graphics/image.cpp
events.cpp
font.cpp
font_factory.cpp
graphics/fire.cpp
ftalleg.cpp
funcs.cpp
directory.cpp
file-system.cpp
graphics/gradient.cpp
ebox.cpp
regex.cpp
language-string.cpp
thread.cpp
input/text-input.cpp
input/input-manager.cpp
input/input-source.cpp
input/joystick.cpp
input/allegro5/joystick.cpp
input/touch.cpp
input/allegro5/touch.cpp
xenon/xenon.cpp
system.cpp
version.cpp
compress.cpp
message-queue.cpp
input/linux_joystick.cpp
exceptions/load_exception.cpp
windows/system.cpp
nacl/module.cpp
nacl/network-system.cpp
exceptions/exception.cpp
menu/actionfactory.cpp
menu/action_speed.cpp
menu/menu.cpp
menu/font-info.cpp
menu/menu_action.cpp
menu/menu_option.cpp
menu/options.cpp
menu/optionfactory.cpp
gui/animation.cpp
gui/box.cpp
gui/container.cpp
gui/context-box.cpp
gui/coordinate.cpp
gui/cutscene.cpp
gui/fadetool.cpp
gui/lineedit.cpp
gui/list.cpp
gui/rectarea.cpp
gui/popup-box.cpp
gui/scroll-list.cpp
gui/select-list.cpp
gui/tab-container.cpp
gui/tabbed-box.cpp
gui/timer.cpp
gui/widget.cpp
sound/sound.cpp
sound/audio.cpp
sound/music-player.cpp
sound/music-renderer.cpp
sound/music.cpp
system/timer.cpp
system/allegro5/timer.cpp
system/allegro5/init.cpp
)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include
        libs/7z
        libs/dumb/include
        libs/gme
        libs/hawknl
        libs/lz4
        libs/pcre
        libs/sfl
        libs/zip
        libs
        sound
        .
        ..
        ../paintown-engine
        ../openbor
        ../mugen
)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (util_module ${UTIL_SRC})

# -------------------------------------------------------
# Ensure that we are doing an out of source build
# Prevents any mishaps
# Also remove the makefile to regenerate the filelist
# -------------------------------------------------------
if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeCache.txt)
    file(REMOVE ${CMAKE_SOURCE_DIR}/CMakeCache.txt)
    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/CMakeFiles)
endif(EXISTS ${CMAKE_SOURCE_DIR}/CMakeCache.txt)
if(EXISTS ${CMAKE_BINARY_DIR}/Makefile)
    file(REMOVE ${CMAKE_BINARY_DIR}/Makefile)
endif(EXISTS ${CMAKE_BINARY_DIR}/Makefile)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    if(UNIX AND NOT CYGWIN)
        message(FATAL_ERROR "Oops. Don't do an in-source build. Create an extra directory change into it and run cmake pointing to the base directory. IE: \nmkdir mybuild && cd mybuild && cmake ../ && make\nYou may need to remove CMakeCache.txt and the CMakeFiles directory in ${CMAKE_SOURCE_DIR} if you can't get rid of this error.")
    else(UNIX AND NOT CYGWIN)
        message(FATAL_ERROR "Oops. Don't do an in-source build. Create an extra directory change into it and run cmake pointing to the base directory. IE: \nmkdir mybuild; cd mybuild; cmakesetup ../\nYou may need to remove CMakeCache.txt and the CMakeFiles directory in  ${CMAKE_SOURCE_DIR} if you can't get rid of this error.")
    endif(UNIX AND NOT CYGWIN)
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

# -------------------------------------------------------
# Output directories
# -------------------------------------------------------
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# -------------------------------------------------------
# Directory in which extra macros can be found
# -------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# -------------------------------------------------------
# Add in uninstall target
# -------------------------------------------------------
#configure_file(
#        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#        IMMEDIATE @ONLY)

#add_custom_target(uninstall
#        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# -------------------------------------------------------
# Including needed macros
# -------------------------------------------------------
include(UseLLVM)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
find_package(ZLIB)
find_package(PNG)
find_package(FREETYPE)
find_package(Threads)
#find_package(GNUREGEX)
find_package(VORBIS)
find_package(OGG)
find_package(MPG123)

# -------------------------------------------------------
# Include Source
# -------------------------------------------------------
mark_as_advanced(CLEAR ZLIB_INCLUDE_DIR)
mark_as_advanced(CLEAR ZLIB_LIBRARY)

mark_as_advanced(CLEAR PNG_PNG_INCLUDE_DIR)
mark_as_advanced(CLEAR PNG_LIBRARY)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
option(DEBUG "Compile r-tech1 with debug information?" OFF)

if($ENV{DEBUG})
    set(DEBUG ON)
elseif(NOT $ENV{DEBUG})
    set(DEBUG OFF)
endif($ENV{DEBUG})

if(DEBUG)
    add_definitions(-DDEBUG)
    if(USE_LLVM)
        set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -Woverloaded-virtual -g3 -ggdb")
        set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -g3 -ggdb")
    else(USE_LLVM)
        set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -Woverloaded-virtual -g3 -ggdb")
        set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -g3 -ggdb")
    endif(USE_LLVM)
else(DEBUG)
    if(USE_LLVM)
        set(CMAKE_CXX_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -Woverloaded-virtual")
        set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops")
    else(USE_LLVM)
        set(CMAKE_CXX_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -Woverloaded-virtual")
        set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations")
    endif(USE_LLVM)
endif(DEBUG)

# Networking
add_definitions(-DHAVE_NETWORKING)

add_definitions(-DHAVE_CXX11 -DDUMB_DECLARE_DEPRECATED)

option(USE_ALLEGRO5 "Compile r-tech1 with ALLEGRO 5 instead of SDL or Allegro 4.x?" ON)

if($ENV{ALLEGRO5})
    set(USE_ALLEGRO5 ON)
elseif(NOT $ENV{ALLEGRO5})
    set(USE_ALLEGRO5 OFF)
endif($ENV{ALLEGRO5})

if(OGG_FOUND)
    add_definitions(-DHAVE_OGG)
    set(OGG_LIBS ${OGG_LIBRARY} ${VORBIS_LIBRARIES})
    set(OGG_INCLUDES ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR})
else(OGG_FOUND)
    set(OGG_LIBS "")
    set(OGG_INCLUDES "")
endif(OGG_FOUND)

# Allegro5
find_package(ALLEGRO5)
if(NOT ALLEGRO_FOUND)
    message(FATAL_ERROR "Couldn't find Allegro 5 and/or a required Allegro 5 module")
endif(NOT ALLEGRO_FOUND)
set(GRAPHICS_LIBRARIES ${ALLEGRO_LIBRARIES})
set(GRAPHICS_INCLUDES ${ALLEGRO_INCLUDE_DIR})

if(MPG123_FOUND)
    add_definitions(-DHAVE_MP3_MPG123)
    set(MP3_LIBS ${MPG123_LIBRARY})
    set(MP3_INCLUDES ${MPG123_INCLUDE_DIR})
else(MPG123_FOUND)
    if (SDL_FOUND)
        # Try looking for libmad instead only if SDL is available
        find_package(MAD)
        if (MAD_FOUND)
            add_definitions(-DHAVE_MP3_MAD)
            set(MP3_LIBS ${MAD_LIBRARY})
            set(MP3_INCLUDES ${MAD_INCLUDE_DIR})
        else(MAD_FOUND)
            set(MP3_LIBS "")
            set(MP3_INCLUDES "")
        endif(MAD_FOUND)
    endif (SDL_FOUND)
endif(MPG123_FOUND)

# freetype
if(NOT FREETYPE_FOUND)
    message(FATAL_ERROR "Couldn't find freetype")
endif(NOT FREETYPE_FOUND)

# -------------------------------------------------------
# Paths and system setup
# -------------------------------------------------------

# -------------------------------------------------------
# MINGW
# -------------------------------------------------------

if(MINGW OR CYGWIN)
    # -------------------------------------------------------
    # Take those user options and set the necessary compile time preprocessors
    # -------------------------------------------------------
    add_definitions(-DWINDOWS)
    set(WIN_LIB -lgdi32 -luser32 -lwsock32)
    set(CMAKE_THREAD_LIBS_INIT -lpthreadGC2)
else(MINGW OR CYGWIN)
    set(CMAKE_THREAD_LIBS_INIT -lpthread)
endif(MINGW OR CYGWIN)

# -------------------------------------------------------
# Apple OSX
# -------------------------------------------------------

if(APPLE)
    # -------------------------------------------------------
    # Added stuff for osx
    # -------------------------------------------------------
    set(WIN_LIB "-framework Carbon")
    add_definitions(-DMACOSX)

    # Groups
    set(GROUP_START "-Wl,-all_load")
    set(GROUP_END "")
else(APPLE)
    # Groups
    set(GROUP_START "-Wl,--start-group")
    set(GROUP_END "-Wl,--end-group")
endif(APPLE)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(src src/libs src/libs/hawknl src/libs/dumb/include include ${GRAPHICS_INCLUDES} ${FREETYPE_INCLUDE_DIR} ${OGG_INCLUDES} ${MP3_INCLUDES})

# -------------------------------------------------------
# Put the linked libraries together
# -------------------------------------------------------
set(REQUIRED_LIBS ${GRAPHICS_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OGG_LIBS} ${MP3_LIBS} ${WIN_LIB} ${WINSOCK})

# -------------------------------------------------------
# Source files
# -------------------------------------------------------

#Base Source
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR})

# 7z
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/7z)

#Dumb
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/dumb)

#Hawknl
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/hawknl)

#Minizip
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/zip)

# SFL
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/sfl)

# GME Library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/gme)

# PCRE
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/pcre)

# LZ4
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/lz4)

# Create r-tech1 library
add_library(rtech1_static STATIC ${SFL_SRC} ${UTIL_SRC} ${PCRE_SRC} ${x7Z_SRC} ${DUMB_SRC} ${MINIZIP_SRC} ${HAWKNL_SRC} ${GME_SRC} ${LZ4_SRC})

# Pkgconfig
if(UNIX OR LINUX OR CYGWIN OR MINGW)
    if(DEBUG)
        set(lib "r-tech1-debug")
        set(libs
                "allegro-debug-5 allegro_ttf-debug-5 allegro_memfile-debug-5 allegro_image-debug-5 allegro_primitives-debug-5 allegro_audio-debug-5 allegro_acodec-debug-5 freetype2 zlib")
    else(DEBUG)
        set(lib "r-tech1")
        set(libs
                "allegro-5 allegro_ttf-5 allegro_memfile-5 allegro_image-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 freetype2 zlib")
    endif(DEBUG)
    set(prefix ${CMAKE_INSTALL_PREFIX})
    set(rtech1_version "1")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/misc/r-tech1.pc.in ${LIBRARY_OUTPUT_PATH}/pkgconfig/${lib}.pc @ONLY)
    install(FILES ${LIBRARY_OUTPUT_PATH}/pkgconfig/${lib}.pc DESTINATION lib/pkgconfig)
endif(UNIX OR LINUX OR CYGWIN OR MINGW)