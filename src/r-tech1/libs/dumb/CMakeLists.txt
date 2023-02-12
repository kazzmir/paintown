# -------------------------------------------------------
# dumb cmake build script for r-tech1.
# Written by: juvinious
# -------------------------------------------------------
project (dumb C)
# -------------------------------------------------------
# Set some specific stuff for dumb
# -------------------------------------------------------
if(NOT MSVC)
	set(CFLAGS "-W -O2 -Wwrite-strings -Wstrict-prototypes -Wall -funroll-loops -Wmissing-declarations -fomit-frame-pointer -Wno-missing-declarations")
else(NOT MSVC)
	set(CFLAGS "-O2")
endif(NOT MSVC)

remove_definitions(${CXXFLAGS})
add_definitions(${CFLAGS})

if(MSVC)
	add_definitions("/DDUMB_DECLARE_DEPRECATED")
else(MSVC)
	add_definitions(-DDUMB_DECLARE_DEPRECATED)
endif(MSVC)

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
set(DUMB_SRC
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itorder.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/xmeffect.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itrender.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itread2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itunload.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/loadmod.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/loads3m.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/loadxm2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itload2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/readmod2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/reads3m2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/loadmod2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/loadxm.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/loads3m2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itload.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itmisc.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/itread.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/readxm.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/readmod.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/reads3m.c
${CMAKE_CURRENT_SOURCE_DIR}/src/it/readxm2.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/makeduh.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/unload.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/loadduh.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/rawsig.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/rendduh.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/rendsig.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/duhlen.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/duhtag.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/atexit.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/readduh.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/register.c
${CMAKE_CURRENT_SOURCE_DIR}/src/core/dumbfile.c
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/sampbuf.c
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/stdfile.c
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/resample.inc
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/clickrem.c
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/silence.c
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/resamp2.inc
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/resamp3.inc
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/memfile.c
${CMAKE_CURRENT_SOURCE_DIR}/src/helpers/resample.c
PARENT_SCOPE)

set(UNUSED_DUMB_SRC
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datit.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datxm.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datduh.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datitq.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datmod.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/dats3m.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datxmq.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datmodq.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/dats3mq.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/datunld.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/alplay.c
${CMAKE_CURRENT_SOURCE_DIR}/src/allegro/packfile.c
PARENT_SCOPE)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include include/internal)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (dumb_module ${DUMB_SRC})

# Install
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include/r-tech1/libs/dumb)

