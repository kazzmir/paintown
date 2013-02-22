# - Test if Python is embedable
# Facilitate python within paintown
#
#  PYTHON_EMBEDABLE - True if embedable

set(PythonLibs_FIND_VERSION "2")
find_package(PythonLibs)
if(UNIX OR LINUX)
    # Some systems require these libs to be added explicitly
    find_library(DL dl)
    find_library(MATH m)
    find_library(PTHREAD pthread)
    find_library(UTIL util)
endif(UNIX OR LINUX)

if(PYTHONLIBS_FOUND)
	MESSAGE(STATUS "Checking if Python is Embedable")
	SET(CMAKE_REQUIRED_INCLUDES ${PYTHON_INCLUDE_PATH})
	# ensure the libs are included with python
	SET(PYTHON_LIBRARIES ${PYTHON_LIBRARIES} ${DL} ${MATH} ${PTHREAD} ${UTIL})
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

