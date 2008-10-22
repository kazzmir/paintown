# Macro to provide an option only if a set of other variables are ON.
# Example invocation:
#
#  DEPENDENT_OPTION(USE_FOO "Use Foo" ON "USE_BAR;USE_ZOT" OFF)
#
# If both USE_BAR and USE_ZOT are true, this provides an option called
# USE_FOO that defaults to ON.  Otherwise, it sets USE_FOO to OFF.  If
# the status of USE_BAR or USE_ZOT ever changes, any value for the
# USE_FOO option is saved so that when the option is re-enabled it
# retains its old value.
#
MACRO(DEPENDENT_OPTION option doc default depends force)
  SET(${option}_AVAILABLE 1)

  # Test for each required variable.
  FOREACH(d ${depends})
    IF(NOT ${d})
      SET(${option}_AVAILABLE 0)
    ENDIF(NOT ${d})
  ENDFOREACH(d)
  IF(${option}_AVAILABLE)
    # The option is available.
    OPTION(${option} "${doc}" "${default}")

    # Show the cache value to the user.
    SET(${option} "${${option}}" CACHE BOOL "${doc}" FORCE)
  ELSE(${option}_AVAILABLE)
    # Option should not be available.  Hide the cache value.
    SET(${option} "${${option}}" CACHE INTERNAL "${doc}")

    # Force the option value to be that specified.
    SET(${option} ${force})
  ENDIF(${option}_AVAILABLE)
ENDMACRO(DEPENDENT_OPTION)

# compile and run a file
MACRO(BUILD_RUN FILENAME ARGUMENTS MSG FLAGS LIBS)
        MESSAGE(STATUS ${MSG})
        IF(NOT ${FLAGS})
                SET(${FLAGS} " ")
        ENDIF(NOT ${FLAGS})
        IF(NOT ${LIBS})
                SET(${LIBS} " ")
        ENDIF(NOT ${LIBS})
	TRY_RUN(VAR_1 VAR_2
                    ${CMAKE_BINARY_DIR}
                    ${FILENAME}
                    COMPILE_DEFINITIONS ${FLAGS}
                    CMAKE_FLAGS -DLINK_LIBRARIES:STRING=${LIBS}
                    OUTPUT_VARIABLE OUTPUT
		    ARGS ${ARGUMENTS})
			#message(STATUS compile: ${VAR_2} | run: ${VAR_1} | output: ${OUTPUT})
			if(VAR_2)
				FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeError.log "Compilation of ${FILENAME} failed with the following output:\n" "${OUTPUT}\n")
			endif(VAR_2)
			if(VAR_1)
				FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeError.log "Couldn't run ${FILENAME} failed with the following output:\n" "${OUTPUT}\n")
			endif(VAR_1)
		    SET(BUILD_RUN_RETURN ${VAR_1})
ENDMACRO(BUILD_RUN FILENAME ARGUMENTS MSG FLAGS LIBS)
