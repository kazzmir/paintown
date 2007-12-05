# -------------------------------------------------------
# File list for paintown
#
# -------------------------------------------------------

set(BASE_SRC 
src/configuration.cpp
src/globals.cpp
src/loading.cpp
src/music.cpp
src/main.cpp
src/init.cpp
src/nameplacer.cpp
src/network.cpp
src/network_world.cpp
src/network_world_client.cpp
src/select_player.cpp
src/world.cpp
src/versus_world.cpp)

set(LEVEL_SRC
src/level/block.cpp
src/level/blockobject.cpp
src/level/scene.cpp)

set(ENV_SRC
src/environment/atmosphere.cpp
src/environment/rain_atmosphere.cpp
src/environment/snow_atmosphere.cpp
src/environment/night_atmosphere.cpp
src/environment/fog_atmosphere.cpp)

set(FACTORY_SRC
src/factory/font_factory.cpp
src/factory/heart_factory.cpp
src/factory/object_factory.cpp
src/factory/font_render.cpp
src/factory/collector.cpp
src/factory/shadow.cpp)

set(OBJECT_SRC
src/object/actor.cpp
src/object/animation.cpp
src/object/animation_event.cpp
src/object/animation_event_attack.cpp
src/object/animation_event_bbox.cpp
src/object/animation_event_coords.cpp
src/object/animation_event_delay.cpp
src/object/animation_event_face.cpp
src/object/animation_event_frame.cpp
src/object/animation_event_jump.cpp
src/object/animation_event_move.cpp
src/object/animation_event_nop.cpp
src/object/animation_event_projectile.cpp
src/object/animation_event_offset.cpp
src/object/animation_event_shadow.cpp
src/object/animation_event_sound.cpp
src/object/animation_event_status.cpp
src/object/animation_event_zdistance.cpp
src/object/buddy_player.cpp
src/object/cat.cpp
src/object/display_character.cpp
src/object/effect.cpp
src/object/enemy.cpp
src/object/gib.cpp
src/object/item.cpp
src/object/network_character.cpp
src/object/object.cpp
src/object/object_attack.cpp
src/object/object_nonattack.cpp
src/object/player.cpp
src/object/versus_player.cpp
src/object/versus_enemy.cpp
src/object/projectile.cpp
src/object/attack.cpp
src/object/stimulation.cpp
src/object/health_stimulation.cpp
src/object/character.cpp
src/object/heart.cpp)

set(UTIL_SRC
src/util/token.cpp
src/util/token_exception.cpp
src/util/tokenreader.cpp
src/util/timedifference.cpp
src/util/sound.cpp
src/util/keyboard.cpp
src/util/bitmap.cpp
src/util/font.cpp
src/util/ftalleg.cpp
src/util/funcs.cpp
src/util/ebox.cpp
src/util/load_exception.cpp)

set(LOADPNG_SRC
src/loadpng/loadpng.c
src/loadpng/regpng.c
src/loadpng/savepng.c)

set(MENU_SRC
src/menu/menu.cpp
src/menu/menu_option.cpp
src/menu/option_adventure.cpp
src/menu/option_background.cpp
src/menu/option_credits.cpp
src/menu/option_key.cpp
src/menu/option_menu.cpp
src/menu/option_versus.cpp
src/menu/option_speed.cpp
src/menu/optionfactory.cpp
)

