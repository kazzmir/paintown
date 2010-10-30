# -------------------------------------------------------
# hawknl cmake build script for paintown.
# Written by: juvinious
# -------------------------------------------------------

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
set(SRC
src/crc.c
src/err.c
src/errorstr.c
src/group.c
src/htcondition.c
src/hthread.c
src/htmutex.c
src/ipx.c
src/loopback.c
src/nl.c
src/nltime.c
src/sock.c)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include hawknl)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (hawknl_module ${SRC})

