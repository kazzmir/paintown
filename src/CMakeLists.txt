set(BASE_SRC 
globals.cpp
main-menu.cpp)

set(FACTORY_SRC
factory/font_render.cpp
factory/collector.cpp)

set(SYSTEM_SRC
system/timer.cpp
system/allegro5/timer.cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/paintown-engine)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/openbor)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/mugen)


add_library(base_module ${BASE_SRC} ${FACTORY_SRC} ${SYSTEM_SRC})
