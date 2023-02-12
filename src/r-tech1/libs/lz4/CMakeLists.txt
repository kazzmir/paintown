# -------------------------------------------------------
# lz4 cmake build script for r-tech1.
# -------------------------------------------------------

project (lz4 C CXX)

# -------------------------------------------------------
# Source directories containing all the necessary .c files
# -------------------------------------------------------
set(LZ4_SRC
${CMAKE_CURRENT_SOURCE_DIR}/lz4.c
PARENT_SCOPE)

# Install
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include/r-tech1/libs/lz4)
