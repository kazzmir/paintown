# - Find allegro 5
# Find the native ALLEGRO 5 includes and library
#
#  ALLEGRO_INCLUDE_DIR - where to find allegro.h, etc.
#  ALLEGRO_LIBRARIES   - List of libraries when using allegro.
#  ALLEGRO_FOUND       - True if allegro found.


IF (ALLEGRO_INCLUDE_DIR)
  # Already in cache, be silent
  SET(ALLEGRO_FIND_QUIETLY TRUE)
ENDIF (ALLEGRO_INCLUDE_DIR)

FIND_PATH(ALLEGRO_INCLUDE_DIR allegro.h
  /usr/local/include/allegro5
  /usr/include/allegro5
  $ENV{MINGDIR}/include/allegro5
)

set(VERSION_NUMBER "5")

if(UNIX AND NOT CYGWIN)
	exec_program(pkg-config ARGS "allegro-${VERSION_NUMBER} allegro_ttf-${VERSION_NUMBER} allegro_memfile-${VERSION_NUMBER} allegro_image-${VERSION_NUMBER} allegro_primitives-${VERSION_NUMBER} allegro_audio-${VERSION_NUMBER} allegro_acodec-${VERSION_NUMBER} --libs" OUTPUT_VARIABLE ALLEGRO_LIBRARY)
    #IF (NOT ${ALLEGRO_LIBRARY})
	    # FALL BACK TO 5.0
        #    set(VERSION_NUMBER "5.0")
        #    exec_program(pkg-config ARGS "allegro-${VERSION_NUMBER} allegro_ttf-${VERSION_NUMBER} allegro_memfile-${VERSION_NUMBER} allegro_image-${VERSION_NUMBER} allegro_primitives-${VERSION_NUMBER} allegro_audio-${VERSION_NUMBER} allegro_acodec-${VERSION_NUMBER} --libs" OUTPUT_VARIABLE ALLEGRO_LIBRARY)
        # ENDIF (NOT ${ALLEGRO_LIBRARY})
	# Check if any of these checks have found if so then cause failure)
	STRING(REGEX MATCH "not found" LIB_CHECK ${ALLEGRO_LIBRARY})
	IF (${LIB_CHECK})
	    set(ALLEGRO_LIBRARY "")
	ENDIF (${LIB_CHECK})
# FIXME - Figure out what to do for windows 
#else(UNIX AND NOT CYGWIN)
#	SET(ALLEGRO_NAMES allegro allegrolib allegrodll)
#	FIND_LIBRARY(ALLEGRO_LIBRARY
#	NAMES ${ALLEGRO_NAMES}
#	PATHS /usr/lib /usr/local/lib $ENV{MINGDIR}/lib)
endif(UNIX AND NOT CYGWIN)

IF (ALLEGRO_INCLUDE_DIR AND ALLEGRO_LIBRARY)
   SET(ALLEGRO_FOUND TRUE)
   SET( ALLEGRO_LIBRARIES ${ALLEGRO_LIBRARY} )
   add_definitions(-DUSE_ALLEGRO5)
ELSE (ALLEGRO_INCLUDE_DIR AND ALLEGRO_LIBRARY)
   SET(ALLEGRO_FOUND FALSE)
   SET( ALLEGRO_LIBRARIES )
ENDIF (ALLEGRO_INCLUDE_DIR AND ALLEGRO_LIBRARY)

IF (ALLEGRO_FOUND)
   IF (NOT ALLEGRO_FIND_QUIETLY)
      MESSAGE(STATUS "Found Allegro 5: ${ALLEGRO_LIBRARY}")
   ENDIF (NOT ALLEGRO_FIND_QUIETLY)
ELSE (ALLEGRO_FOUND)
   IF (ALLEGRO_FIND_REQUIRED)
      MESSAGE(STATUS "Looked for Allegro 5 libraries named ${ALLEGRO_NAMES}.")
      MESSAGE(FATAL_ERROR "Could NOT find Allegro 5 library")
   ENDIF (ALLEGRO_FIND_REQUIRED)
ENDIF (ALLEGRO_FOUND)

MARK_AS_ADVANCED(
  ALLEGRO_LIBRARY
  ALLEGRO_INCLUDE_DIR
  )
