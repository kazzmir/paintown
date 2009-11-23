# - Test if Python is embedable
# Facilitate python within paintown
#
#  PYTHON_EMBEDABLE - True if embedable

find_package(PythonLibs)

if(PYTHONLIBS_FOUND)
	MESSAGE(STATUS "Checking if Python is Embedable")
	SET(CMAKE_REQUIRED_INCLUDES ${PYTHON_INCLUDE_PATH})
	# If on Slackware, uncomment the next line to fix a missing library issue
	# As of 11/22/2009 this is needed for Slackware 13
	# SET(PYTHON_LIBRARIES ${PYTHON_LIBRARIES} -ldl -lm -lpthread -lutil)
	SET(CMAKE_REQUIRED_LIBRARIES ${PYTHON_LIBRARIES})
	CHECK_C_SOURCE_COMPILES(
"#include <Python.h>
int main(int argc, char *argv[]) {
   Py_Initialize();
   return 0;
}

" PY_EMBED)
	IF(PY_EMBED)
	    MESSAGE(STATUS "Python is embedable")
	    SET(PYTHON_EMBEDABLE TRUE)
	ELSE(PY_EMBED)
	    MESSAGE(STATUS "Python is not embedable")
	    SET(PYTHON_EMBEDABLE FALSE)
	ENDIF(PY_EMBED)
ELSE(PYTHONLIBS_FOUND)
    MESSAGE(STATUS "Python is not embedable")
    SET(PYTHON_EMBEDABLE FALSE)
ENDIF(PYTHONLIBS_FOUND)

