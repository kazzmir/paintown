# - Locate mad (mpg1/2/3) library
# This module defines
#  MAD_LIBRARY, the library to link against
#  MAD_FOUND, if false, do not try to link to libmad
#  MAD_INCLUDE_DIR, where to find headers.

IF(MAD_LIBRARY AND MAD_INCLUDE_DIR)
  # in cache already
  SET(MAD_FIND_QUIETLY TRUE)
ENDIF(MAD_LIBRARY AND MAD_INCLUDE_DIR)


FIND_PATH(MAD_INCLUDE_DIR
  mad.h
  PATHS
  $ENV{MAD_DIR}/include
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/csw/include
  /opt/include
  PATH_SUFFIXES ogg
)

FIND_LIBRARY(MAD_LIBRARY
  NAMES mad libmad
  PATHS
  $ENV{MAD_DIR}/lib
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

IF(MAD_LIBRARY AND MAD_INCLUDE_DIR)
  SET(MAD_FOUND "YES")
  IF(NOT MAD_FIND_QUIETLY)
    MESSAGE(STATUS "Found libmad: ${MAD_LIBRARY}")
  ENDIF(NOT MAD_FIND_QUIETLY)
ELSE(MAD_LIBRARY AND MAD_INCLUDE_DIR)
  IF(NOT MAD_FIND_QUIETLY)
    MESSAGE(STATUS "Warning: Unable to find libmad!")
  ENDIF(NOT MAD_FIND_QUIETLY)
ENDIF(MAD_LIBRARY AND MAD_INCLUDE_DIR)

