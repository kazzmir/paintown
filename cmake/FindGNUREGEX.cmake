# - Find regex
# Find the native GNU REGEX includes and library
#
#  GNUREGEX_INCLUDE_DIR - where to find allegro.h, etc.
#  GNUREGEX_LIBRARIES   - List of libraries when using allegro.
#  GNUREGEX_FOUND       - True if allegro found.


IF (GNUREGEX_INCLUDE_DIR)
  # Already in cache, be silent
  SET(GNUREGEX_FIND_QUIETLY TRUE)
ENDIF (GNUREGEX_INCLUDE_DIR)

FIND_PATH(GNUREGEX_INCLUDE_DIR regex.h
  /usr/local/include
  /usr/include
  $ENV{MINGDIR}/include
)

IF(UNIX AND NOT CYGWIN)
        # not needed
ELSE(UNIX AND NOT CYGWIN)
	SET(GNUREGEX_NAMES regex regexlib regexdll regex.dll regex-bcc)
	FIND_LIBRARY(GNUREGEX_LIBRARY
	NAMES ${GNUREGEX_NAMES}
	PATHS /usr/lib /usr/local/lib $ENV{MINGDIR}/lib)
ENDIF(UNIX AND NOT CYGWIN)

IF(UNIX AND NOT CYGWIN)
        SET(GNUREGEX_FOUND TRUE)
        SET(GNUREGEX_LIBRARIES "")
ELSE(UNIX AND NOT CYGWIN)
       IF (GNUREGEX_INCLUDE_DIR AND GNUREGEX_LIBRARY)
                SET(GNUREGEX_FOUND TRUE)
                SET(GNUREGEX_LIBRARIES ${GNUREGEX_LIBRARY} )
        ELSE (GNUREGEX_INCLUDE_DIR AND GNUREGEX_LIBRARY)
                SET(GNUREGEX_FOUND FALSE)
                SET(GNUREGEX_LIBRARIES )
        ENDIF (GNUREGEX_INCLUDE_DIR AND GNUREGEX_LIBRARY)

        IF (GNUREGEX_FOUND)
                IF (NOT GNUREGEX_FIND_QUIETLY)
                        MESSAGE(STATUS "Found GNU Regex: ${GNUREGEX_LIBRARY}")
                ENDIF (NOT GNUREGEX_FIND_QUIETLY)
        ELSE (GNUREGEX_FOUND)
                IF (GNUREGEX_FIND_REQUIRED)
                        MESSAGE(STATUS "Looked for GNU Regex libraries named ${GNUREGEX_NAMES}.")
                        MESSAGE(FATAL_ERROR "Could NOT find GNU Regex library")
                ENDIF (GNUREGEX_FIND_REQUIRED)
        ENDIF (GNUREGEX_FOUND)
ENDIF(UNIX AND NOT CYGWIN)

MARK_AS_ADVANCED(
  GNUREGEX_LIBRARY
  GNUREGEX_INCLUDE_DIR
  )
