# -------------------------------------------------------
# SFL cmake build script for r-tech1.
# Written by: juvinious
# -------------------------------------------------------
project (sfl C)
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
set(SFL_SRC
${CMAKE_CURRENT_SOURCE_DIR}/sflbits.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcomp.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcons.c
${CMAKE_CURRENT_SOURCE_DIR}/sflconv.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcryp.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvbs.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvdp.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvds.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvns.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvsb.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvsd.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvsn.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvtp.c
${CMAKE_CURRENT_SOURCE_DIR}/sflcvts.c
${CMAKE_CURRENT_SOURCE_DIR}/sfldbio.c
${CMAKE_CURRENT_SOURCE_DIR}/sfldir.c
${CMAKE_CURRENT_SOURCE_DIR}/sflenv.c
${CMAKE_CURRENT_SOURCE_DIR}/sflexdr.c
${CMAKE_CURRENT_SOURCE_DIR}/sflfile.c
${CMAKE_CURRENT_SOURCE_DIR}/sflfind.c
${CMAKE_CURRENT_SOURCE_DIR}/sflfort.c
${CMAKE_CURRENT_SOURCE_DIR}/sflhttp.c
${CMAKE_CURRENT_SOURCE_DIR}/sflini.c
${CMAKE_CURRENT_SOURCE_DIR}/sfllang.c
${CMAKE_CURRENT_SOURCE_DIR}/sfllbuf.c
${CMAKE_CURRENT_SOURCE_DIR}/sfllist.c
${CMAKE_CURRENT_SOURCE_DIR}/sflmath.c
${CMAKE_CURRENT_SOURCE_DIR}/sflmem.c
${CMAKE_CURRENT_SOURCE_DIR}/sflmesg.c
${CMAKE_CURRENT_SOURCE_DIR}/sflmime.c
${CMAKE_CURRENT_SOURCE_DIR}/sflnode.c
${CMAKE_CURRENT_SOURCE_DIR}/sflprint.c
${CMAKE_CURRENT_SOURCE_DIR}/sflslot.c
${CMAKE_CURRENT_SOURCE_DIR}/sflstr.c
${CMAKE_CURRENT_SOURCE_DIR}/sflsymb.c
${CMAKE_CURRENT_SOURCE_DIR}/sflsyst.c
${CMAKE_CURRENT_SOURCE_DIR}/sfltok.c
${CMAKE_CURRENT_SOURCE_DIR}/sfltree.c
${CMAKE_CURRENT_SOURCE_DIR}/sfltron.c
${CMAKE_CURRENT_SOURCE_DIR}/sfluid.c
${CMAKE_CURRENT_SOURCE_DIR}/sflxml.c
${CMAKE_CURRENT_SOURCE_DIR}/sfldate.c
PARENT_SCOPE)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include ./)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (sfl_module ${SFL_SRC})

# Install
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include/r-tech1/libs/sfl)


