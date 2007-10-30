# -------------------------------------------------------
# dumb cmake build script for paintown.
# Written by: juvinious
# -------------------------------------------------------

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
src/it/itorder.c
src/it/xmeffect.c
src/it/itrender.c
src/it/itread2.c
src/it/itunload.c
src/it/loadmod.c
src/it/loads3m.c
src/it/loadxm2.c
src/it/itload2.c
src/it/readmod2.c
src/it/reads3m2.c
src/it/loadmod2.c
src/it/loadxm.c
src/it/loads3m2.c
src/it/itload.c
src/it/itmisc.c
src/it/itread.c
src/it/readxm.c
src/it/readmod.c
src/it/reads3m.c
src/it/readxm2.c
src/core/makeduh.c
src/core/unload.c
src/core/loadduh.c
src/core/rawsig.c
src/core/rendduh.c
src/core/rendsig.c
src/core/duhlen.c
src/core/duhtag.c
src/core/atexit.c
src/core/readduh.c
src/core/register.c
src/core/dumbfile.c
src/helpers/sampbuf.c
src/helpers/stdfile.c
src/helpers/resample.inc
src/helpers/clickrem.c
src/helpers/silence.c
src/helpers/resamp2.inc
src/helpers/resamp3.inc
src/helpers/memfile.c
src/helpers/resample.c
src/allegro/datit.c
src/allegro/datxm.c
src/allegro/datduh.c
src/allegro/datitq.c
src/allegro/datmod.c
src/allegro/dats3m.c
src/allegro/datxmq.c
src/allegro/datmodq.c
src/allegro/dats3mq.c
src/allegro/datunld.c
src/allegro/alplay.c
src/allegro/packfile.c)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include include/internal)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (dumb_module ${DUMB_SRC})

