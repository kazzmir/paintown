# - Find R-Tech1
# Find the native R-Tech1 includes and library
#
#  RTECH1_INCLUDE_DIR - where to find r-tech1 includes
#  RTECH1_LIBRARIES   - List of libraries when using r-tech1
#  RTECH1_FOUND       - True if r-tech1 found.


IF (RTECH1_INCLUDE_DIR)
  # Already in cache, be silent
  SET(RTECH1_FIND_QUIETLY TRUE)
ENDIF (RTECH1_INCLUDE_DIR)

FIND_PATH(RTECH1_INCLUDE_DIR init.h
  /usr/local/include/r-tech1
  /usr/include/r-tech1
  $ENV{MINGDIR}/include/r-tech1
)

if(UNIX AND NOT CYGWIN)
	exec_program(pkg-config ARGS "r-tech1 --libs" OUTPUT_VARIABLE RTECH1_LIBRARY)
	# Check if any of these checks have found if so then cause failure)
	STRING(REGEX MATCH "not found" LIB_CHECK ${RTECH1_LIBRARY})
	IF (${LIB_CHECK})
	    set(RTECH1_LIBRARY "")
	ENDIF (${LIB_CHECK})
endif(UNIX AND NOT CYGWIN)

IF (RTECH1_INCLUDE_DIR AND RTECH1_LIBRARY)
   SET(RTECH1_FOUND TRUE)
   SET( RTECH1_LIBRARIES ${RTECH1_LIBRARY} )
   add_definitions(-I${RTECH1_INCLUDE_DIR}/libs -DHAVE_CXX11 -DDUMB_DECLARE_DEPRECATED)
ELSE (RTECH1_INCLUDE_DIR AND RTECH1_LIBRARY)
   SET(RTECH1_FOUND FALSE)
   SET( RTECH1_LIBRARIES )
ENDIF (RTECH1_INCLUDE_DIR AND RTECH1_LIBRARY)

IF (RTECH1_FOUND)
   IF (NOT RTECH1_FIND_QUIETLY)
      MESSAGE(STATUS "Found r-tech1: ${RTECH1_LIBRARY}")
   ENDIF (NOT RTECH1_FIND_QUIETLY)
ELSE (RTECH1_FOUND)
   IF (RTECH1_FIND_REQUIRED)
      MESSAGE(STATUS "Looked for r-tech1 libraries named ${RTECH1_NAMES}.")
      MESSAGE(FATAL_ERROR "Could NOT find r-tech1 library")
   ENDIF (RTECH1_FIND_REQUIRED)
ENDIF (RTECH1_FOUND)

MARK_AS_ADVANCED(
  RTECH1_LIBRARY
  RTECH1_INCLUDE_DIR
  )
