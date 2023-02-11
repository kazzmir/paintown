# -------------------------------------------------------
# hawknl cmake build script for r-tech1.
# Written by: juvinious
# -------------------------------------------------------
project (hawknl C CXX)
# -------------------------------------------------------
# Set some specific stuff for hawknl
# -------------------------------------------------------
if(NOT MSVC)
	set(CFLAGS "-W -O2 -Wwrite-strings -Wstrict-prototypes -Wall -funroll-loops -Wmissing-declarations -fomit-frame-pointer -Wno-missing-declarations")
else(NOT MSVC)
	set(CFLAGS "-O2")
endif(NOT MSVC)

remove_definitions(${CXXFLAGS})
add_definitions(${CFLAGS})

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
set(HAWKNL_SRC
${CMAKE_CURRENT_SOURCE_DIR}/src/crc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/err.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/errorstr.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/group.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/ipx.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/loopback.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/nl.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/nltime.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/sock.cpp
PARENT_SCOPE)

#${CMAKE_CURRENT_SOURCE_DIR}/src/htcondition.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/hthread.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/htmutex.cpp

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
#include_directories(include hawknl)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (hawknl_module ${HAWKNL_SRC})

# Install
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/hawknl DESTINATION include/r-tech1/libs/hawknl)
