set(BASE_SRC 
src/configuration.cpp
src/globals.cpp
src/main-menu.cpp)

set(ASTEROIDS_SRC
src/asteroids/game.cpp)

set(GAME_SRC
src/paintown-engine/game/world.cpp
src/paintown-engine/game/game.cpp
src/paintown-engine/game/move-list.cpp
src/paintown-engine/game/select_player.cpp
src/paintown-engine/game/character-select.cpp
src/paintown-engine/game/nameplacer.cpp
src/paintown-engine/game/adventure_world.cpp
src/paintown-engine/game/mod.cpp
src/paintown-engine/game/options.cpp)

set(SCRIPT_SRC
src/paintown-engine/script/script.cpp
src/paintown-engine/script/modules/python.cpp)

set(LEVEL_SRC
src/paintown-engine/level/block.cpp
src/paintown-engine/level/blockobject.cpp
src/paintown-engine/level/cacher.cpp
src/paintown-engine/level/scene.cpp
src/paintown-engine/level/random-scene.cpp
src/paintown-engine/level/utils.cpp)

set(NETWORK_SRC
src/paintown-engine/network/cacher.cpp
src/paintown-engine/network/client.cpp
src/paintown-engine/network/chat_client.cpp
src/paintown-engine/network/chat_server.cpp
src/paintown-engine/network/chat-widget.cpp
src/paintown-engine/network/network_world.cpp
src/paintown-engine/network/network_world_client.cpp
src/paintown-engine/network/server.cpp)

set(ENV_SRC
src/paintown-engine/environment/atmosphere.cpp)

set(FACTORY_SRC
src/factory/font_factory.cpp
src/paintown-engine/factory/heart_factory.cpp
src/paintown-engine/factory/object_factory.cpp
src/factory/font_render.cpp
src/factory/collector.cpp
src/paintown-engine/factory/shadow.cpp)

set(OBJECT_SRC
src/paintown-engine/object/actor.cpp
src/paintown-engine/object/animation.cpp
src/paintown-engine/object/animation_event.cpp
src/paintown-engine/object/animation_trail.cpp
src/paintown-engine/object/buddy_player.cpp
src/paintown-engine/object/cat.cpp
src/paintown-engine/object/display_character.cpp
src/paintown-engine/object/draw-effect.cpp
src/paintown-engine/object/effect.cpp
src/paintown-engine/object/enemy.cpp
src/paintown-engine/object/gib.cpp
src/paintown-engine/object/item.cpp
src/paintown-engine/object/alliance.cpp
src/paintown-engine/object/network_character.cpp
src/paintown-engine/object/network_player.cpp
src/paintown-engine/object/object.cpp
src/paintown-engine/object/object_attack.cpp
src/paintown-engine/object/object_nonattack.cpp
src/paintown-engine/object/player.cpp
src/paintown-engine/object/player-common.cpp
src/paintown-engine/object/versus_player.cpp
src/paintown-engine/object/versus_enemy.cpp
src/paintown-engine/object/projectile.cpp
src/paintown-engine/object/attack.cpp
src/paintown-engine/object/stimulation.cpp
src/paintown-engine/object/character.cpp
src/paintown-engine/object/heart.cpp)

set(OPENBOR_SRC
src/openbor/pack-reader.cpp
src/openbor/animation.cpp
src/openbor/display-character.cpp
src/openbor/mod.cpp
src/openbor/util.cpp
src/openbor/data.cpp)

set(LOADPNG_SRC
src/util/allegro/loadpng/loadpng.c
src/util/allegro/loadpng/regpng.c
src/util/allegro/loadpng/savepng.c)

set(PLATFORMER_SRC
src/platformer/game/animation.cpp
src/platformer/game/background.cpp
src/platformer/game/camera.cpp
src/platformer/game/game.cpp
src/platformer/game/options.cpp
src/platformer/game/tile.cpp
src/platformer/game/world.cpp
src/platformer/object/object.cpp
src/platformer/script/script.cpp
)

set(TRIGGER_SRC
src/paintown-engine/trigger/trigger.cpp
)
