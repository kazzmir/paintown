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
find_package(GNUREGEX)

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

if(DEBUG)
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS "-Wall -funroll-loops -fexpensive-optimizations -Woverloaded-virtual -g3 -ggdb")
    set(CMAKE_C_FLAGS "-Wall -funroll-loops -fexpensive-optimizations -g3 -ggdb")
else(DEBUG)
    set(CMAKE_CXX_FLAGS "-O2 -Wall -funroll-loops -fexpensive-optimizations -Woverloaded-virtual")
    set(CMAKE_C_FLAGS "-O2 -Wall -funroll-loops -fexpensive-optimizations")
endif(DEBUG)

option(USE_SDL "Compile paintown with SDL instead of Allegro?" OFF)

if(USE_SDL)
    # packages
    find_package(SDL)
    find_package(SDL_image)
    find_package(SDL_gfx)

    # sdl
    add_definitions(-DUSE_SDL)
    if(NOT SDL_FOUND)
        message(FATAL_ERROR "Couldn't find SDL.")
    endif(NOT SDL_FOUND)
    if(NOT SDLIMAGE_FOUND)
        message(FATAL_ERROR "Couldn't find SDL_image module.")
    endif(NOT SDLIMAGE_FOUND)
    if(NOT SDLGFX_FOUND)
        message(FATAL_ERROR "Couldn't find SDL_gfx module.")
    endif(NOT SDLGFX_FOUND)
    # Set graphic libs and includes
    set(GRAPHICS_LIBRARIES ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLGFX_LIBRARY})
    set(GRAPHICS_INCLUDES ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLGFX_INCLUDE_DIR})
else(USE_SDL)
    #packages
    find_package(ALLEGRO) 
    if(NOT ALLEGRO_FOUND)
	    message(FATAL_ERROR "Couldn't find Allegro")
    endif(NOT ALLEGRO_FOUND)
    set(GRAPHICS_LIBRARIES ${ALLEGRO_LIBRARIES})
endif(USE_SDL)

# freetype
if(NOT FREETYPE_FOUND)
	message(FATAL_ERROR "Couldn't find freetype")
endif(NOT FREETYPE_FOUND)

# regex
if(NOT GNUREGEX_FOUND)
	message(FATAL_ERROR "Couldn't find GNU Regex")
endif(NOT GNUREGEX_FOUND)

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
# Put the linked libraries together
# -------------------------------------------------------
set(REQUIRED_LIBS ${GRAPHICS_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${GNUREGEX_LIBRARIES} ${WIN_LIB} ${WINSOCK})

#set(ALL_SRC ${LEVEL_SRC} ${FACTORY_SRC} ${OBJECT_SRC} ${UTIL_SRC}
#${LOADPNG_SRC} ${MENU_SRC} ${ENV_SRC} ${NETWORK_SRC} ${GUI_SRC} ${SCRIPT_SRC}
#${GAME_SRC} ${MUGEN_SRC} ${OGG_SRC} ${TRIGGER_SRC})

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
# include_directories(src ${CMAKE_BINARY_DIR}/src ${CMAKE_BINARY_DIR}/src/level ${CMAKE_BINARY_DIR}/src/factory ${CMAKE_BINARY_DIR}/src/object ${CMAKE_BINARY_DIR}/src/util ${CMAKE_BINARY_DIR}/src/loadpng ${CMAKE_BINARY_DIR}/src/gui ${CMAKE_BINARY_DIR}/src/modules src/hawknl ${FREETYPE_INCLUDE_DIR})
include_directories(src src/hawknl ${GRAPHICS_INCLUDES} ${FREETYPE_INCLUDE_DIR} ${GNUREGEX_INCLUDE_DIR})

# -------------------------------------------------------
# Paintown and test
# -------------------------------------------------------
#add_library (main_module ${ALL_SRC})

#Dumb
add_subdirectory(${CMAKE_SOURCE_DIR}/src/dumb)

#Hawknl
add_subdirectory(${CMAKE_SOURCE_DIR}/src/hawknl)

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

# SDL Stretch library
if (USE_SDL)
    add_library(stretch_module src/util/sdl/stretch/sdlscreen.c src/util/sdl/stretch/sdlstretch.c)
    set(STRETCH_MOD stretch_module)
else (USE_SDL)
    set(STRETCH_MOD "")
endif (USE_SDL)

# GME Library
add_library(gme_module src/gme/Ay_Apu.cpp src/gme/Ay_Cpu.cpp src/gme/Ay_Emu.cpp src/gme/Blip_Buffer.cpp src/gme/Classic_Emu.cpp src/gme/Data_Reader.cpp src/gme/Dual_Resampler.cpp src/gme/Effects_Buffer.cpp src/gme/Fir_Resampler.cpp src/gme/Gb_Apu.cpp src/gme/Gb_Cpu.cpp src/gme/Gb_Oscs.cpp src/gme/Gbs_Emu.cpp src/gme/gme.cpp src/gme/Gme_File.cpp src/gme/Gym_Emu.cpp src/gme/Hes_Apu.cpp src/gme/Hes_Cpu.cpp src/gme/Hes_Emu.cpp src/gme/Kss_Cpu.cpp src/gme/Kss_Emu.cpp src/gme/Kss_Scc_Apu.cpp src/gme/M3u_Playlist.cpp src/gme/Multi_Buffer.cpp src/gme/Music_Emu.cpp src/gme/Nes_Apu.cpp src/gme/Nes_Cpu.cpp src/gme/Nes_Fme7_Apu.cpp src/gme/Nes_Namco_Apu.cpp src/gme/Nes_Oscs.cpp src/gme/Nes_Vrc6_Apu.cpp src/gme/Nsfe_Emu.cpp src/gme/Nsf_Emu.cpp src/gme/Sap_Apu.cpp src/gme/Sap_Cpu.cpp src/gme/Sap_Emu.cpp src/gme/Sms_Apu.cpp src/gme/Snes_Spc.cpp src/gme/Spc_Cpu.cpp src/gme/Spc_Dsp.cpp src/gme/Spc_Emu.cpp src/gme/Vgm_Emu.cpp src/gme/Vgm_Emu_Impl.cpp src/gme/Ym2413_Emu.cpp src/gme/Ym2612_Emu.cpp)

#Script directory
add_subdirectory(${CMAKE_SOURCE_DIR}/src/script)

add_library(game_module ${GAME_SRC})
add_library(mugen_module ${MUGEN_SRC})
add_library(ogg_module ${OGG_SRC})
add_library(trigger_module ${TRIGGER_SRC})

#paintown executable
add_executable (paintown src/xmain.cpp)
target_link_libraries(paintown ${GROUP_START} base_module level_module factory_module object_module util_module loadpng_module 
        menu_module env_module network_module gui_module script_module game_module mugen_module ogg_module trigger_module dumb_module hawknl_module ${STRETCH_MOD} gme_module ${GROUP_END} ${REQUIRED_LIBS})

#add_executable (test src/test.cpp)
#target_link_libraries(test main_module dumb_module hawknl_module ${REQUIRED_LIBS})

#add_executable (menutest src/menu/menutest.cpp)
#target_link_libraries(menutest main_module dumb_module hawknl_module ${REQUIRED_LIBS})

if (NOT USE_SDL)
    add_executable (mugentest src/mugen/test.cpp)
    target_link_libraries(mugentest ${GROUP_START} base_module level_module factory_module object_module util_module loadpng_module 
        menu_module env_module network_module gui_module script_module game_module mugen_module ogg_module trigger_module dumb_module hawknl_module gme_module ${GROUP_END} ${REQUIRED_LIBS})
endif (NOT USE_SDL)

# Copy scripts and setup lex files
if(UNIX OR LINUX OR CYGWIN OR MINGW)
  configure_file(${CMAKE_SOURCE_DIR}/src/script/modules/paintown.py ${CMAKE_SOURCE_DIR}/data/scripts/paintown.py COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/src/script/modules/paintown.rb ${CMAKE_SOURCE_DIR}/data/scripts/paintown.rb COPYONLY)
endif(UNIX OR LINUX OR CYGWIN OR MINGW)

