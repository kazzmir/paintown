# -------------------------------------------------------
# Paintown cmake build script. Creates makefiles for unix 
# based systems and/or
# project files for a given environment like MSVC.
#
# TODO:
#	* Create CYGWIN TARGET
#
# Written by: juvinious
# -------------------------------------------------------

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
include(CMakeMacros)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
find_package(ZLIB)
find_package(PNG)
find_package(ALLEGRO)
find_package(FREETYPE)
#find_package(THREADS)

mark_as_advanced(CLEAR ZLIB_INCLUDE_DIR)
mark_as_advanced(CLEAR ZLIB_LIBRARY)

mark_as_advanced(CLEAR PNG_PNG_INCLUDE_DIR)
mark_as_advanced(CLEAR PNG_LIBRARY)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
if(NOT MSVC)
	set(CXXFLAGS "-O2 -Wall -funroll-loops -fexpensive-optimizations -Woverloaded-virtual -Werror")
else(NOT MSVC)
	set(CXXFLAGS "-O2")
endif(NOT MSVC)

add_definitions(${CXXFLAGS})

# allegro
if(NOT ALLEGRO_FOUND)
	message(FATAL_ERROR "Couldn't find Allegro")
endif(NOT ALLEGRO_FOUND)

if(NOT FREETYPE_FOUND)
	message(FATAL_ERROR "Couldn't find freetype")
endif(NOT FREETYPE_FOUND)

# -------------------------------------------------------
# Paths and system setup
# -------------------------------------------------------

# -------------------------------------------------------
# MINGW
# -------------------------------------------------------

if(MINGW)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	add_definitions(-DWINDOWS)
	set(WIN_LIB -lgdi32 -luser32)
#if(NOT CMAKE_THREAD_LIBS_INIT)
				 set(CMAKE_THREAD_LIBS_INIT -lpthreadGC2)
#endif(NOT CMAKE_THREAD_LIBS_INIT)
else(MINGW)
	 set(CMAKE_THREAD_LIBS_INIT -lpthread)
endif(MINGW)


if(APPLE)
	# -------------------------------------------------------
    # Added stuff for osx
	# -------------------------------------------------------
	set(WIN_LIB "-framework Carbon")
endif(APPLE)

# -------------------------------------------------------
# MSVC
# -------------------------------------------------------

if(MSVC)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	add_definitions("/DWINDOWS")
	set(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
	set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
	set(CMAKE_CONFIGURATION_TYPES "Release")
	
	# -------------------------------------------------------	
	# library checks
	# -------------------------------------------------------
	
	set(WIN_LIB /gdi32 /user32)
	
endif(MSVC)

# -------------------------------------------------------
# Put the linked libraries together
# -------------------------------------------------------
set(DUMB_LIBRARY -laldmb -ldumb)
set(REQUIRED_LIBS  ${DUMB_LIBRARY} ${ALLEGRO_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${WIN_LIB})

# -------------------------------------------------------
# Create the necessary header file for the library
# -------------------------------------------------------

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
set(BASE_SRC 
src/configuration.cpp
src/globals.cpp
src/loading.cpp
src/music.cpp
src/main.cpp
src/init.cpp
src/nameplacer.cpp
src/select_player.cpp
src/world.cpp
src/versus_world.cpp
versus_player.cpp
versus_enemy.cpp)

set(LEVEL_SRC
src/level/block.cpp
src/level/blockobject.cpp
src/level/scene.cpp)

set(FACTORY_SRC
src/factory/font_factory.cpp
src/factory/heart_factory.cpp
src/factory/object_factory.cpp
src/factory/font_render.cpp
src/factory/collector.cpp
src/factory/shadow.cpp)

set(OBJECT_SRC
src/object/animation.cpp
src/object/animation_event.cpp
src/object/animation_event_attack.cpp
src/object/animation_event_bbox.cpp
src/object/animation_event_coords.cpp
src/object/animation_event_delay.cpp
src/object/animation_event_face.cpp
src/object/animation_event_frame.cpp
src/object/animation_event_jump.cpp
src/object/animation_event_move.cpp
src/object/animation_event_nop.cpp
src/object/animation_event_projectile.cpp
src/object/animation_event_offset.cpp
src/object/animation_event_shadow.cpp
src/object/animation_event_sound.cpp
src/object/animation_event_status.cpp
src/object/animation_event_zdistance.cpp
src/object/display_character.cpp
src/object/effect.cpp
src/object/enemy.cpp
src/object/item.cpp
src/object/object.cpp
src/object/object_attack.cpp
src/object/object_nonattack.cpp
src/object/player.cpp
src/object/versus_player.cpp
src/object/versus_enemy.cpp
src/object/projectile.cpp
src/object/attack.cpp
src/object/stimulation.cpp
src/object/health_stimulation.cpp
src/object/character.cpp
src/object/heart.cpp)

set(UTIL_SRC
src/util/token.cpp
src/util/token_exception.cpp
src/util/tokenreader.cpp
src/util/timedifference.cpp
src/util/sound.cpp
src/util/keyboard.cpp
src/util/bitmap.cpp
src/util/font.cpp
src/util/ftalleg.cpp
src/util/funcs.cpp
src/util/ebox.cpp
src/util/load_exception.cpp)

set(LOADPNG_SRC
src/loadpng/loadpng.c)

set(ALL_SRC ${BASE_SRC} ${LEVEL_SRC} ${FACTORY_SRC} ${OBJECT_SRC} ${UTIL_SRC} ${LOADPNG_SRC})

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(src ${CMAKE_BINARY_DIR}/src ${CMAKE_BINARY_DIR}/src/level ${CMAKE_BINARY_DIR}/src/factory ${CMAKE_BINARY_DIR}/src/object ${CMAKE_BINARY_DIR}/src/util ${CMAKE_BINARY_DIR}/src/loadpng ${FREETYPE_INCLUDE_DIR})

# -------------------------------------------------------
# Paintown and test
# -------------------------------------------------------
add_library (main_module ${ALL_SRC})

add_executable (paintown src/xmain.cpp)
target_link_libraries(paintown main_module ${REQUIRED_LIBS})

add_executable (test src/test.cpp)
target_link_libraries(test main_module ${REQUIRED_LIBS})
