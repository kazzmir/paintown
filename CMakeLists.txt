# -------------------------------------------------------
# Paintown cmake build script. Creates makefiles for unix 
# based systems and/or
# project files for a given environment like MSVC.
#
# Written by: juvinious
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
find_package(ALLEGRO)
find_package(FREETYPE)
find_package(Threads)
find_package(Ruby)
include(TestForPythonEmbed)

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
if(NOT MSVC)
	set(CXXFLAGS "-O2 -Wall -funroll-loops -fexpensive-optimizations -Woverloaded-virtual")
else(NOT MSVC)
	set(CXXFLAGS "-O2")
endif(NOT MSVC)

add_definitions(${CXXFLAGS})

# allegro
if(NOT ALLEGRO_FOUND)
	message(FATAL_ERROR "Couldn't find Allegro")
endif(NOT ALLEGRO_FOUND)

# freetype
if(NOT FREETYPE_FOUND)
	message(FATAL_ERROR "Couldn't find freetype")
endif(NOT FREETYPE_FOUND)

# Python
if(PYTHON_EMBEDABLE)
	set(PY_INCLUDE ${PYTHON_INCLUDE_PATH}) 
	set(PY_LIBS ${PYTHON_LIBRARIES})
	add_definitions(-DHAVE_PYTHON)
endif(PYTHON_EMBEDABLE)

# Ruby
if(RUBY_INCLUDE_PATH)
	set(RUBY_INCLUDE ${RUBY_INCLUDE_PATH}) 
	set(RUBY_LIBS ${RUBY_LIBRARY})
	add_definitions(-DHAVE_RUBY)
endif(RUBY_INCLUDE_PATH)

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


if(APPLE)
	# -------------------------------------------------------
    # Added stuff for osx
	# -------------------------------------------------------
	set(WIN_LIB "-framework Carbon")
	add_definitions(-DMACOSX)
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
set(REQUIRED_LIBS ${ALLEGRO_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} ${FREETYPE_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${PY_LIBS} ${RUBY_LIBS} ${WIN_LIB} ${WINSOCK})

set(ALL_SRC ${BASE_SRC} ${LEVEL_SRC} ${FACTORY_SRC} ${OBJECT_SRC} ${UTIL_SRC}
${LOADPNG_SRC} ${MENU_SRC} ${ENV_SRC} ${NETWORK_SRC} ${GUI_SRC} ${SCRIPT_SRC} ${GAME_SRC} ${MUGEN_SRC})

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(src ${CMAKE_BINARY_DIR}/src ${CMAKE_BINARY_DIR}/src/level ${CMAKE_BINARY_DIR}/src/factory ${CMAKE_BINARY_DIR}/src/object ${CMAKE_BINARY_DIR}/src/util ${CMAKE_BINARY_DIR}/src/loadpng ${CMAKE_BINARY_DIR}/src/gui ${CMAKE_BINARY_DIR}/src/modules ${FREETYPE_INCLUDE_DIR} src/hawknl ${PY_INCLUDE} ${RUBY_INCLUDE})

# -------------------------------------------------------
# Paintown and test
# -------------------------------------------------------
add_library (main_module ${ALL_SRC})

#Dumb
add_subdirectory(${CMAKE_SOURCE_DIR}/src/dumb)

#Hawknl
add_subdirectory(${CMAKE_SOURCE_DIR}/src/hawknl)

add_executable (paintown src/xmain.cpp)
target_link_libraries(paintown main_module dumb_module hawknl_module ${REQUIRED_LIBS})

#add_executable (test src/test.cpp)
#target_link_libraries(test main_module dumb_module hawknl_module ${REQUIRED_LIBS})

#add_executable (menutest src/menu/menutest.cpp)
#target_link_libraries(menutest main_module dumb_module hawknl_module ${REQUIRED_LIBS})

#add_executable (mugentest src/mugen/test.cpp)
#target_link_libraries(mugentest main_module dumb_module hawknl_module ${REQUIRED_LIBS})

# Do scrip zip only in *nix land
if(UNIX OR LINUX OR CYGWIN)
  configure_file(${CMAKE_SOURCE_DIR}/src/script/modules/paintown.py ${CMAKE_BINARY_DIR}/build/script/modules/paintown.py COPYONLY)
  configure_file(${CMAKE_SOURCE_DIR}/src/script/modules/paintown.rb ${CMAKE_BINARY_DIR}/build/script/modules/paintown.rb COPYONLY)
  execute_process(COMMAND mkdir bin
		  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		  OUTPUT_QUIET)
  execute_process(COMMAND zip -r bin/scripts build
		  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(UNIX OR LINUX OR CYGWIN)
