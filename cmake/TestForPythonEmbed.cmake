# - Test if Python is embedable
# Facilitate python within paintown
#
#  PYTHON_EMBEDABLE - True if embedable

find_package(PythonLibs)

if(PYTHONLIBS_FOUND)
	BUILD_RUN("${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestForPythonEmbed.c" "" "Checking if Python is Embedable" "-I${PYTHON_INCLUDE_PATH}" "${PYTHON_LIBRARIES}")
        IF(NOT BUILD_RUN_RETURN)
	    MESSAGE(STATUS "Python is embedable")
            SET(PYTHON_EMBEDABLE TRUE)
	ELSE(NOT BUILD_RUN_RETURN)
	    MESSAGE(STATUS "Python is not embedable")
	ENDIF(NOT BUILD_RUN_RETURN)
ELSE(PYTHONLIBS_FOUND)
    MESSAGE(STATUS "Python is not embedable")
    SET(PYTHON_EMBEDABLE FALSE)
ENDIF(PYTHONLIBS_FOUND)

