project(minizip C)

set(MINIZIP_SRC
${CMAKE_CURRENT_SOURCE_DIR}/ioapi.c
${CMAKE_CURRENT_SOURCE_DIR}/unzip.c
${CMAKE_CURRENT_SOURCE_DIR}/zip.c PARENT_SCOPE)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
# include_directories(include include/internal)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (minizip_module ${MINIZIP_SRC})

# Install
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include/r-tech1/libs/zip)
