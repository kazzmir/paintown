# -------------------------------------------------------
# Paintown cmake build script. Creates makefiles for unix
# based systems, cygwin(sp?) or mingw
#
# Written by: juvinious
# Modified: 06/27/2009
# -------------------------------------------------------

# -------------------------------------------------------
# Ensure that we are doing an out of source build
# Prevents any mishaps
# Also remove the makefile to regenerate the filelist
# -------------------------------------------------------
cmake_minimum_required(VERSION 2.6)
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
# Directory in which extra macros can be found
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
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# -------------------------------------------------------
# project name
# -------------------------------------------------------
project (paintown)

# -------------------------------------------------------
# Including needed macros
# -------------------------------------------------------
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
include(FileList)

mark_as_advanced(CLEAR ZLIB_INCLUDE_DIR)
mark_as_advanced(CLEAR ZLIB_LIBRARY)

mark_as_advanced(CLEAR PNG_PNG_INCLUDE_DIR)
mark_as_advanced(CLEAR PNG_LIBRARY)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
option(DEBUG "Compile paintown with debug information?" OFF)

if($ENV{DEBUG})
    set(DEBUG ON)
elseif(NOT $ENV{DEBUG})
    set(DEBUG OFF)
endif($ENV{DEBUG})

if(DEBUG)
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -Woverloaded-virtual -g3 -ggdb")
    set(CMAKE_C_FLAGS "-Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -g3 -ggdb")
else(DEBUG)
    set(CMAKE_CXX_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations -Woverloaded-virtual")
    set(CMAKE_C_FLAGS "-O2 -Wall -Wno-unused-variable -Wno-unused-function -funroll-loops -fexpensive-optimizations")
endif(DEBUG)
    
# FIXME: not sure if this is in the right place
add_definitions(-DHAVE_NETWORKING)

option(USE_SDL "Compile paintown with SDL instead of Allegro?" ON)
option(USE_ALLEGRO5 "Compile paintown with ALLEGRO 5 instead of SDL or Allegro 4.x?" OFF)

if($ENV{ALLEGRO5})
    set(USE_ALLEGRO5 ON)
    set(USE_SDL OFF)
elseif(NOT $ENV{ALLEGRO5})
    set(USE_ALLEGRO5 OFF)
endif($ENV{ALLEGRO5})

if($ENV{SDL})
    set(USE_SDL ON)
elseif(NOT $ENV{SDL})
    set(USE_SDL OFF)
endif($ENV{SDL})

if(OGG_FOUND)
    add_definitions(-DHAVE_OGG)
    set(OGG_LIBS ${OGG_LIBRARY} ${VORBIS_LIBRARIES})
    set(OGG_INCLUDES ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR})
else(OGG_FOUND)
    set(OGG_LIBS "")
    set(OGG_INCLUDES "")
endif(OGG_FOUND)

if(USE_SDL)
    # packages
    find_package(SDL)

    # sdl
    add_definitions(-DUSE_SDL)
    if(NOT SDL_FOUND)
        message(FATAL_ERROR "Couldn't find SDL.")
    endif(NOT SDL_FOUND)
    
    # SET Mixer definition
    if (MAD_FOUND)
	add_definitions(-DMP3_MAD_MUSIC)
    endif(MAD_FOUND)
    
    set(GRAPHICS_LIBRARIES sfl_module sprig_module sdl_stretch_module image_module mixer_module ${SDL_LIBRARY})
    set(GRAPHICS_INCLUDES ${SDL_INCLUDE_DIR})
    if (APPLE)
	add_definitions(-DUSE_SDL_MAIN)
    endif(APPLE)
elseif(USE_ALLEGRO5)
    find_package(ALLEGRO5)
    if(NOT ALLEGRO_FOUND)
	message(FATAL_ERROR "Couldn't find Allegro 5 and/or a required Allegro 5 module")
    endif(NOT ALLEGRO_FOUND)
    set(GRAPHICS_LIBRARIES sfl_module ${ALLEGRO_LIBRARIES})
    set(GRAPHICS_INCLUDES ${ALLEGRO_INCLUDE_DIR})
else(USE_SDL)
    #packages
    find_package(ALLEGRO) 
    if(NOT ALLEGRO_FOUND)
	    message(FATAL_ERROR "Couldn't find Allegro")
    endif(NOT ALLEGRO_FOUND)
    add_library(algif_module src/util/allegro/gif/algif.c src/util/allegro/gif/gif.c src/util/allegro/gif/lzw.c)
    set(GRAPHICS_LIBRARIES ${ALLEGRO_LIBRARIES} algif_module)
endif(USE_SDL)

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

# regex
#if(NOT GNUREGEX_FOUND)
#	message(FATAL_ERROR "Couldn't find GNU Regex")
#endif(NOT GNUREGEX_FOUND)

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
include_directories(src src/util/network/hawknl ${GRAPHICS_INCLUDES} ${FREETYPE_INCLUDE_DIR} ${OGG_INCLUDES} ${MP3_INCLUDES})

# -------------------------------------------------------
# Put the linked libraries together
# -------------------------------------------------------
set(REQUIRED_LIBS ${GRAPHICS_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OGG_LIBS} ${MP3_LIBS} ${WIN_LIB} ${WINSOCK})


# -------------------------------------------------------
# Paintown and test
# -------------------------------------------------------
#add_library (main_module ${ALL_SRC})

#Dumb
add_subdirectory(${CMAKE_SOURCE_DIR}/src/util/dumb)

#Hawknl
add_subdirectory(${CMAKE_SOURCE_DIR}/src/util/network/hawknl)

#Source modules
add_library(base_module ${BASE_SRC})
add_library(level_module ${LEVEL_SRC})
add_library(factory_module ${FACTORY_SRC})
add_library(object_module ${OBJECT_SRC})
add_library(util_module ${UTIL_SRC})
add_library(loadpng_module ${LOADPNG_SRC})
add_library(menu_module ${MENU_SRC})
add_library(env_module ${ENV_SRC})
add_library(network_module ${NETWORK_SRC})
add_library(gui_module ${GUI_SRC})
# add_library(algif_module src/util/gif/algif.c src/util/gif/gif.c src/util/gif/lzw.c)

# GME Library
add_library(gme_module src/util/gme/Ay_Apu.cpp src/util/gme/Ay_Cpu.cpp src/util/gme/Ay_Emu.cpp src/util/gme/Blip_Buffer.cpp src/util/gme/Classic_Emu.cpp src/util/gme/Data_Reader.cpp src/util/gme/Dual_Resampler.cpp src/util/gme/Effects_Buffer.cpp src/util/gme/Fir_Resampler.cpp src/util/gme/Gb_Apu.cpp src/util/gme/Gb_Cpu.cpp src/util/gme/Gb_Oscs.cpp src/util/gme/Gbs_Emu.cpp src/util/gme/gme.cpp src/util/gme/Gme_File.cpp src/util/gme/Gym_Emu.cpp src/util/gme/Hes_Apu.cpp src/util/gme/Hes_Cpu.cpp src/util/gme/Hes_Emu.cpp src/util/gme/Kss_Cpu.cpp src/util/gme/Kss_Emu.cpp src/util/gme/Kss_Scc_Apu.cpp src/util/gme/M3u_Playlist.cpp src/util/gme/Multi_Buffer.cpp src/util/gme/Music_Emu.cpp src/util/gme/Nes_Apu.cpp src/util/gme/Nes_Cpu.cpp src/util/gme/Nes_Fme7_Apu.cpp src/util/gme/Nes_Namco_Apu.cpp src/util/gme/Nes_Oscs.cpp src/util/gme/Nes_Vrc6_Apu.cpp src/util/gme/Nsfe_Emu.cpp src/util/gme/Nsf_Emu.cpp src/util/gme/Sap_Apu.cpp src/util/gme/Sap_Cpu.cpp src/util/gme/Sap_Emu.cpp src/util/gme/Sms_Apu.cpp src/util/gme/Snes_Spc.cpp src/util/gme/Spc_Cpu.cpp src/util/gme/Spc_Dsp.cpp src/util/gme/Spc_Emu.cpp src/util/gme/Vgm_Emu.cpp src/util/gme/Vgm_Emu_Impl.cpp src/util/gme/Ym2413_Emu.cpp src/util/gme/Ym2612_Emu.cpp)

#Script directory
add_subdirectory(${CMAKE_SOURCE_DIR}/src/paintown-engine/script)

add_library(game_module ${GAME_SRC})
add_library(mugen_module ${MUGEN_SRC})
add_library(platformer_module ${PLATFORMER_SRC})
add_library(trigger_module ${TRIGGER_SRC})
add_library(pcre_module src/util/pcre/pcre_chartables.c src/util/pcre/pcre_compile.c src/util/pcre/pcre_config.c src/util/pcre/pcre_dfa_exec.c src/util/pcre/pcre_exec.c src/util/pcre/pcre_fullinfo.c src/util/pcre/pcre_get.c src/util/pcre/pcre_globals.c src/util/pcre/pcre_info.c src/util/pcre/pcre_maketables.c src/util/pcre/pcre_newline.c src/util/pcre/pcre_ord2utf8.c src/util/pcre/pcreposix.c src/util/pcre/pcre_refcount.c src/util/pcre/pcre_study.c src/util/pcre/pcre_tables.c src/util/pcre/pcre_try_flipped.c src/util/pcre/pcre_ucd.c src/util/pcre/pcre_valid_utf8.c src/util/pcre/pcre_version.c src/util/pcre/pcre_xclass.c)

if (USE_SDL)
    # IMAGE
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/util/sdl/image)
    # SFL
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/util/sfl)
    # Stretch
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/util/sdl/stretch)
    # SPRIG
    add_library(sprig_module src/util/sdl/sprig/SPG_extended.c src/util/sdl/sprig/SPG_primitives.c src/util/sdl/sprig/SPG_misc.c src/util/sdl/sprig/SPG_rotation.c src/util/sdl/sprig/SPG_polygon.c src/util/sdl/sprig/SPG_surface.c)
    # Mixer
    add_library(mixer_module src/util/sdl/mixer/music.c src/util/sdl/mixer/mixer.c src/util/sdl/mixer/music_ogg.c src/util/sdl/mixer/music_flac.c src/util/sdl/mixer/music_mad.c src/util/sdl/mixer/dynamic_ogg.c src/util/sdl/mixer/effect_position.c src/util/sdl/mixer/effects_internal.c src/util/sdl/mixer/effect_stereoreverse.c src/util/sdl/mixer/load_voc.c src/util/sdl/mixer/load_aiff.c src/util/sdl/mixer/wavestream.c src/util/sdl/mixer/load_ogg.c src/util/sdl/mixer/convert.cpp)
elseif (USE_ALLEGRO5)
    # SFL
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/util/sfl)
endif (USE_SDL)

#paintown executable
add_executable (paintown src/util/xmain.cpp)
target_link_libraries(paintown ${GROUP_START} base_module level_module factory_module object_module util_module loadpng_module 
        menu_module env_module network_module gui_module script_module game_module mugen_module platformer_module trigger_module pcre_module dumb_module hawknl_module gme_module ${GROUP_END} ${REQUIRED_LIBS})

#add_executable (test src/test.cpp)
#target_link_libraries(test main_module dumb_module hawknl_module ${REQUIRED_LIBS})

#add_executable (menutest src/menu/menutest.cpp)
#target_link_libraries(menutest main_module dumb_module hawknl_module ${REQUIRED_LIBS})

#if (NOT USE_SDL)
#    add_executable (mugentest src/mugen/test.cpp)
#    target_link_libraries(mugentest ${GROUP_START} base_module level_module factory_module object_module util_module loadpng_module 
#        menu_module env_module network_module gui_module script_module game_module mugen_module ogg_module trigger_module pcre_module dumb_module hawknl_module gme_module ${GROUP_END} ${REQUIRED_LIBS})
#endif (NOT USE_SDL)

# Copy scripts and setup lex files
if(UNIX OR LINUX OR CYGWIN OR MINGW)
  configure_file(${CMAKE_SOURCE_DIR}/src/paintown-engine/script/modules/paintown.py ${CMAKE_SOURCE_DIR}/data/scripts/paintown.py COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/src/paintown-engine/script/modules/paintown.rb ${CMAKE_SOURCE_DIR}/data/scripts/paintown.rb COPYONLY)
endif(UNIX OR LINUX OR CYGWIN OR MINGW)

