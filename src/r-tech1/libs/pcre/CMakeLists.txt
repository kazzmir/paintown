# -------------------------------------------------------
# PCRE cmake build script for r-tech1.
# Written by: juvinious
# -------------------------------------------------------
project (pcre C CXX)

# -------------------------------------------------------
# Source directories containing all the necessary .c files
# -------------------------------------------------------
# PCRE Library
set(PCRE_SRC
${CMAKE_CURRENT_SOURCE_DIR}/pcre_chartables.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_compile.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_config.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_dfa_exec.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_exec.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_fullinfo.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_get.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_globals.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_info.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_maketables.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_newline.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_ord2utf8.c
${CMAKE_CURRENT_SOURCE_DIR}/pcreposix.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_refcount.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_study.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_tables.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_try_flipped.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_ucd.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_valid_utf8.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_version.c
${CMAKE_CURRENT_SOURCE_DIR}/pcre_xclass.c
PARENT_SCOPE)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (pcre_module ${PCRE_SRC})

# Install
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include/r-tech1/libs/pcre)
