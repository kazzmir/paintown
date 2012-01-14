# -------------------------------------------------------
# scripting specific cmake module
# Written by: juvinious
# -------------------------------------------------------

# -------
# macros
# -------
# find_package(Ruby)
include(TestForPythonEmbed)

# Python
if(PYTHON_EMBEDABLE)
	set(PY_INCLUDE ${PYTHON_INCLUDE_PATH})
	set(PY_LIBS ${PYTHON_LIBRARIES})
	add_definitions(-DHAVE_PYTHON)
endif(PYTHON_EMBEDABLE)

# Ruby
#if(RUBY_INCLUDE_PATH)
#	set(RUBY_INCLUDE ${RUBY_INCLUDE_PATH})
#	set(RUBY_LIBS ${RUBY_LIBRARY})
#	add_definitions(-DHAVE_RUBY)
#endif(RUBY_INCLUDE_PATH)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
# include_directories(${PY_INCLUDE} ${RUBY_INCLUDE})
include_directories(${PY_INCLUDE})

# -------------------------------------------------------
# module
# -------------------------------------------------------
# have to modify script src
foreach(src_file ${SCRIPT_SRC})
        string(REPLACE "script/" "" src_file ${src_file})
        list(APPEND SRC ${src_file})
endforeach(src_file ${SCRIPT_SRC})
add_library(script_module ${SRC})
# target_link_libraries(script_module ${PY_LIBS} ${RUBY_LIBS})
target_link_libraries(script_module ${PY_LIBS})
