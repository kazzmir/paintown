# - Locate mpg123 library
# This module defines
#  MPG123_LIBRARY, the library to link against
#  MPG123_FOUND, if false, do not try to link to libmpg123
#  MPG123_INCLUDE_DIR, where to find headers.

IF(MPG123_LIBRARY AND MPG123_INCLUDE_DIR)
  # in cache already
  SET(MPG123_FIND_QUIETLY TRUE)
ENDIF(MPG123_LIBRARY AND MPG123_INCLUDE_DIR)


FIND_PATH(MPG123_INCLUDE_DIR
  mpg123.h
  PATHS
  $ENV{MPG123_DIR}/include
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/csw/include
  /opt/include
  PATH_SUFFIXES ogg
)

FIND_LIBRARY(MPG123_LIBRARY
  NAMES mpg123 libmpg123
  PATHS
  $ENV{MPG123_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
)

IF(MPG123_LIBRARY AND MPG123_INCLUDE_DIR)
  SET(MPG123_FOUND "YES")
  IF(NOT MPG123_FIND_QUIETLY)
    MESSAGE(STATUS "Found libmpg123: ${MPG123_LIBRARY}")
  ENDIF(NOT MPG123_FIND_QUIETLY)
ELSE(MPG123_LIBRARY AND MPG123_INCLUDE_DIR)
  IF(NOT MPG123_FIND_QUIETLY)
    MESSAGE(STATUS "Warning: Unable to find libmpg123!")
  ENDIF(NOT MPG123_FIND_QUIETLY)
ENDIF(MPG123_LIBRARY AND MPG123_INCLUDE_DIR)

