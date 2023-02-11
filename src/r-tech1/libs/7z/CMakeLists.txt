# -------------------------------------------------------
# util cmake build script for r-tech1.
# Written by: juvinious
# Modified by: kazzmir
# -------------------------------------------------------

project (x7z C CXX)

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
set(x7Z_SRC
${CMAKE_CURRENT_SOURCE_DIR}/7zAlloc.c
${CMAKE_CURRENT_SOURCE_DIR}/7zBuf.c
${CMAKE_CURRENT_SOURCE_DIR}/7zBuf2.c
${CMAKE_CURRENT_SOURCE_DIR}/7zCrc.c
${CMAKE_CURRENT_SOURCE_DIR}/7zCrcOpt.c
${CMAKE_CURRENT_SOURCE_DIR}/7zDec.c
${CMAKE_CURRENT_SOURCE_DIR}/7zIn.c
${CMAKE_CURRENT_SOURCE_DIR}/CpuArch.c
${CMAKE_CURRENT_SOURCE_DIR}/LzmaDec.c
${CMAKE_CURRENT_SOURCE_DIR}/Lzma2Dec.c
${CMAKE_CURRENT_SOURCE_DIR}/Bra.c
${CMAKE_CURRENT_SOURCE_DIR}/Bra86.c
${CMAKE_CURRENT_SOURCE_DIR}/Bcj2.c
${CMAKE_CURRENT_SOURCE_DIR}/7zFile.c
${CMAKE_CURRENT_SOURCE_DIR}/7zStream.c
PARENT_SCOPE)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
#include_directories(include include/internal)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (x7z_module ${x7Z_SRC})


# Install
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include/r-tech1/libs/7z)
