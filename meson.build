# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('paintown', ['c', 'cpp'], default_options: ['cpp_std=c++11'])

add_global_arguments('-DUSE_SDL=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')
add_global_arguments('-g3', language: ['cpp', 'c'])

includes = include_directories('src')
dumb_includes = include_directories('src/r-tech1/libs/dumb/include')
all_code = [
  'src/xmain.cpp',
  'src/main-menu.cpp',
  
  'src/factory/collector.cpp',
  'src/factory/font_render.cpp',

  'src/paintown-engine/game/mod.cpp',
  'src/paintown-engine/game/argument.cpp',
  'src/paintown-engine/game/options.cpp',
  'src/paintown-engine/game/nameplacer.cpp',

  'src/paintown-engine/game/adventure_world.cpp',
  'src/paintown-engine/game/character-select.cpp',
  'src/paintown-engine/game/game.cpp',
  'src/paintown-engine/game/move-list.cpp',
  'src/paintown-engine/game/select_player.cpp',
  'src/paintown-engine/game/world.cpp',

  'src/paintown-engine/script/script.cpp',

  'src/paintown-engine/network/cacher.cpp',
  'src/paintown-engine/network/chat_client.cpp',
  'src/paintown-engine/network/chat_server.cpp',
  'src/paintown-engine/network/chat-widget.cpp',
  'src/paintown-engine/network/client.cpp',
  'src/paintown-engine/network/network.cpp',
  'src/paintown-engine/network/network_world_client.cpp',
  'src/paintown-engine/network/network_world.cpp',
  'src/paintown-engine/network/server.cpp',

  'src/paintown-engine/object/actor.cpp',
  'src/paintown-engine/object/alliance.cpp',
  'src/paintown-engine/object/animation.cpp',
  'src/paintown-engine/object/animation_event.cpp',
  'src/paintown-engine/object/animation_trail.cpp',
  'src/paintown-engine/object/attack.cpp',
  'src/paintown-engine/object/buddy_player.cpp',
  'src/paintown-engine/object/cat.cpp',
  'src/paintown-engine/object/character.cpp',
  'src/paintown-engine/object/display_character.cpp',
  'src/paintown-engine/object/draw-effect.cpp',
  'src/paintown-engine/object/effect.cpp',
  'src/paintown-engine/object/enemy.cpp',
  'src/paintown-engine/object/gib.cpp',
  'src/paintown-engine/object/item.cpp',
  'src/paintown-engine/object/network_character.cpp',
  'src/paintown-engine/object/network_player.cpp',
  'src/paintown-engine/object/object_attack.cpp',
  'src/paintown-engine/object/object.cpp',
  'src/paintown-engine/object/object_nonattack.cpp',
  'src/paintown-engine/object/player-common.cpp',
  'src/paintown-engine/object/player.cpp',
  'src/paintown-engine/object/projectile.cpp',
  'src/paintown-engine/object/stimulation.cpp',
  'src/paintown-engine/object/trigger.cpp',

  'src/paintown-engine/factory/object_factory.cpp',
  'src/paintown-engine/level/block.cpp',
  'src/paintown-engine/level/blockobject.cpp',
  'src/paintown-engine/level/cacher.cpp',
  'src/paintown-engine/level/random-scene.cpp',
  'src/paintown-engine/level/scene.cpp',
  'src/paintown-engine/level/utils.cpp',
  'src/paintown-engine/trigger/trigger.cpp',

  'src/paintown-engine/environment/atmosphere.cpp',

  'src/openbor/animation.cpp',
  'src/openbor/data.cpp',
  'src/openbor/display-character.cpp',
  'src/openbor/mod.cpp',
  'src/openbor/pack-reader.cpp',
  'src/openbor/util.cpp',

  'src/globals.cpp',

  'src/mugen/argument.cpp',
  'src/mugen/option-options.cpp',
  'src/mugen/exception.cpp',
  'src/mugen/config.cpp',

  'src/mugen/animation.cpp',
  'src/mugen/background.cpp',
  'src/mugen/behavior.cpp',
  'src/mugen/character.cpp',
  'src/mugen/characterhud.cpp',
  'src/mugen/character-select.cpp',
  'src/mugen/command.cpp',
  'src/mugen/compiler.cpp',
  'src/mugen/constraint.cpp',
  'src/mugen/effect.cpp',
  'src/mugen/font.cpp',
  'src/mugen/game.cpp',
  'src/mugen/helper.cpp',
  'src/mugen/item-content.cpp',
  'src/mugen/item.cpp',
  'src/mugen/match.cpp',
  'src/mugen/menu.cpp',
  'src/mugen/network-behavior.cpp',
  'src/mugen/network.cpp',
  'src/mugen/parse-cache.cpp',
  'src/mugen/projectile.cpp',
  'src/mugen/random.cpp',
  'src/mugen/reader.cpp',
  'src/mugen/run-match.cpp',
  'src/mugen/search.cpp',
  'src/mugen/section.cpp',
  'src/mugen/serialize-auto.cpp',
  'src/mugen/serialize.cpp',
  'src/mugen/sff.cpp',
  'src/mugen/sound.cpp',
  'src/mugen/sprite.cpp',
  'src/mugen/stage.cpp',
  'src/mugen/state-controller.cpp',
  'src/mugen/state.cpp',
  'src/mugen/storyboard.cpp',
  'src/mugen/util.cpp',
  'src/mugen/versus.cpp',
  'src/mugen/widgets.cpp',
  'src/mugen/world.cpp',

  'src/mugen/parser/air.cpp',
  'src/mugen/parser/cmd.cpp',
  'src/mugen/parser/def.cpp',
  'src/mugen/parser/parse-exception.cpp',
  'src/mugen/ast/ast.cpp',

  ]

sdl_dependency = dependency('sdl', required: true)
freetype_dependency = dependency('freetype2', required: true)
zlib_dependency = dependency('zlib', required: true)

subdir('src/r-tech1')

executable('paintown', all_code,
  include_directories: [includes, dumb_includes],
  dependencies: [sdl_dependency, freetype_dependency, zlib_dependency],
  link_with: rtech1_library,
  )
