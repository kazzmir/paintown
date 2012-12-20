# -------------------------------------------------------
# util cmake build script for paintown.
# Written by: juvinious
# Modified by: kazzmir
# -------------------------------------------------------

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------

set(GAME_SRC
game/world.cpp
game/game.cpp
game/move-list.cpp
game/select_player.cpp
game/character-select.cpp
game/nameplacer.cpp
game/argument.cpp
game/adventure_world.cpp
game/mod.cpp
game/options.cpp)

# FIXME: move this to script/CMakeLists.txt
set(SCRIPT_SRC
script/script.cpp
script/modules/python.cpp)

set(LEVEL_SRC
level/block.cpp
level/blockobject.cpp
level/cacher.cpp
level/scene.cpp
level/random-scene.cpp
level/utils.cpp)

set(NETWORK_SRC
network/cacher.cpp
network/client.cpp
network/chat_client.cpp
network/chat_server.cpp
network/chat-widget.cpp
network/network.cpp
network/network_world.cpp
network/network_world_client.cpp
network/server.cpp)

set(ENV_SRC
environment/atmosphere.cpp)

set(PAINTOWN_FACTORY_SRC
factory/heart_factory.cpp
factory/object_factory.cpp
factory/shadow.cpp)

set(OBJECT_SRC
object/actor.cpp
object/animation.cpp
object/animation_event.cpp
object/animation_trail.cpp
object/buddy_player.cpp
object/cat.cpp
object/display_character.cpp
object/draw-effect.cpp
object/effect.cpp
object/enemy.cpp
object/gib.cpp
object/item.cpp
object/alliance.cpp
object/network_character.cpp
object/network_player.cpp
object/object.cpp
object/object_attack.cpp
object/object_nonattack.cpp
object/player.cpp
object/player-common.cpp
object/versus_player.cpp
object/versus_enemy.cpp
object/projectile.cpp
object/trigger.cpp
object/attack.cpp
object/stimulation.cpp
object/character.cpp
object/heart.cpp)

set(TRIGGER_SRC
trigger/trigger.cpp
)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
#include_directories(include include/internal)

# add_subdirectory(${CMAKE_SOURCE_DIR}/src/paintown-engine/script)
add_subdirectory(script)

# -------------------------------------------------------
# module
# -------------------------------------------------------
add_library (paintown_module ${GAME_SRC} ${LEVEL_SRC} ${NETWORK_SRC} ${TRIGGER_SRC} ${OBJECT_SRC} ${ENV_SRC} ${PAINTOWN_FACTORY_SRC})
