# -------------------------------------------------------
# Paintown cmake build script. Creates makefiles for unix
# based systems, cygwin(sp?) or mingw
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

mark_as_advanced(CLEAR ZLIB_INCLUDE_DIR)
mark_as_advanced(CLEAR ZLIB_LIBRARY)

mark_as_advanced(CLEAR PNG_PNG_INCLUDE_DIR)
mark_as_advanced(CLEAR PNG_LIBRARY)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
set(STARTING_DATA_PATH "${CMAKE_SOURCE_DIR}/data" CACHE INTERNAL "Starting data path." FORCE)
option(DEBUG "Compile paintown with debug information?" OFF)
set(DATA_PATH "${STARTING_DATA_PATH}" CACHE FILEPATH "Set the data path (Recommend /usr/local/games/paintown for system installation)")
option(DELETE_INSTALLED_DATA_PATH "Delete the installation data directory when running uninstall?" OFF)

# Data path
add_definitions(-DDATA_PATH=\"${DATA_PATH}\")

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
    
# No networking
add_definitions(-DHAVE_NETWORKING)

# Allegro5 is required
set(USE_ALLEGRO5 ON)

if(OGG_FOUND)
    add_definitions(-DHAVE_OGG)
    set(OGG_LIBS ${OGG_LIBRARY} ${VORBIS_LIBRARIES})
    set(OGG_INCLUDES ${OGG_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR})
else(OGG_FOUND)
    set(OGG_LIBS "")
    set(OGG_INCLUDES "")
endif(OGG_FOUND)

# Allegro 5
find_package(ALLEGRO5)
if(NOT ALLEGRO_FOUND)
    message(FATAL_ERROR "Couldn't find Allegro 5 and/or a required Allegro 5 module")
endif(NOT ALLEGRO_FOUND)

# r-tech1 libraries
find_package(RTECH1)
if (NOT RTECH1_FOUND)
    message(FATAL_ERROR "Could not find r-tech1 library, which is required to build paintown.")
endif(NOT RTECH1_FOUND)

# Libraries
set(GRAPHICS_LIBRARIES ${RTECH1_LIBRARIES})
set(GRAPHICS_INCLUDES ${RTECH1_INCLUDE_DIR})

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
include_directories(src ${GRAPHICS_INCLUDES} ${FREETYPE_INCLUDE_DIR} ${OGG_INCLUDES} ${MP3_INCLUDES})

# -------------------------------------------------------
# Put the linked libraries together
# -------------------------------------------------------
set(REQUIRED_LIBS ${GRAPHICS_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OGG_LIBS} ${MP3_LIBS} ${WIN_LIB} ${WINSOCK})

# -------------------------------------------------------
# Paintown and test
# -------------------------------------------------------
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_executable(paintown src/xmain.cpp)
target_link_libraries(paintown ${GROUP_START} base_module paintown_module script_module openbor_module mugen_module ${GROUP_END} ${REQUIRED_LIBS})

# Copy scripts and setup lex files
#if(UNIX OR LINUX OR CYGWIN OR MINGW)
#  configure_file(${CMAKE_SOURCE_DIR}/src/paintown-engine/script/modules/paintown.py ${CMAKE_SOURCE_DIR}/data/scripts/paintown.py COPYONLY)
#  configure_file(${CMAKE_SOURCE_DIR}/src/paintown-engine/script/modules/paintown.rb ${CMAKE_SOURCE_DIR}/data/scripts/paintown.rb COPYONLY)
#endif(UNIX OR LINUX OR CYGWIN OR MINGW)


# Install
if ("${DATA_PATH}" STREQUAL "${STARTING_DATA_PATH}")
    message(STATUS "Change DATA_PATH if you want to install the application. (Recommend /usr/local/games/paintown)")
else ("${DATA_PATH}" STREQUAL "${STARTING_DATA_PATH}")
    message(STATUS "Install configuration data path is set to ${DATA_PATH}")
    install (TARGETS paintown DESTINATION bin)
    install (DIRECTORY "${CMAKE_SOURCE_DIR}/data/" DESTINATION ${DATA_PATH})
endif ("${DATA_PATH}" STREQUAL "${STARTING_DATA_PATH}")
