# -------------------------------------------------------
# util cmake build script for openbor engine.
# -------------------------------------------------------

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------

set(OPENBOR_SRC
pack-reader.cpp
animation.cpp
display-character.cpp
mod.cpp
util.cpp
data.cpp)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (openbor_module ${OPENBOR_SRC})
