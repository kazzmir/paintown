# -------------------------------------------------------
# SFL cmake build script for paintown.
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
sflbits.c
sflcomp.c
sflcons.c
sflconv.c
sflcryp.c
sflcvbs.c
sflcvdp.c
sflcvds.c
sflcvns.c
sflcvsb.c
sflcvsd.c
sflcvsn.c
sflcvtp.c
sflcvts.c
sfldbio.c
sfldir.c
sflenv.c
sflexdr.c
sflfile.c
sflfind.c
sflfort.c
sflhttp.c
sflini.c
sfllang.c
sfllbuf.c
sfllist.c
sflmail.c
sflmath.c
sflmem.c
sflmesg.c
sflmime.c
sflnode.c
sflprint.c
sflslot.c
sflstr.c
sflsymb.c
sflsyst.c
sfltok.c
sfltree.c
sfltron.c
sfluid.c
sflxml.c
sfldate.c)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include ./)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (sfl_module ${SRC})

