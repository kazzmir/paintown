# -------------------------------------------------------
# util cmake build script for paintown.
# Written by: juvinious
# Modified by: kazzmir
# -------------------------------------------------------

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
set(MUGEN_SRC
argument.cpp
background.cpp
behavior.cpp
network-behavior.cpp
characterhud.cpp
character.cpp
character-select.cpp
config.cpp
compiler.cpp
helper.cpp
game.cpp
command.cpp
storyboard.cpp
state.cpp
projectile.cpp
animation.cpp
exception.cpp
font.cpp
item.cpp
item-content.cpp
match.cpp
menu.cpp
reader.cpp
section.cpp
sound.cpp
sprite.cpp
stage.cpp
sff.cpp
util.cpp
search.cpp
state-controller.cpp
option-options.cpp
widgets.cpp
ast/ast.cpp
versus.cpp
parse-cache.cpp
parser/parse-exception.cpp
parser/def.cpp
parser/cmd.cpp
parser/air.cpp)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
#include_directories(include include/internal)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (mugen_module ${MUGEN_SRC})
