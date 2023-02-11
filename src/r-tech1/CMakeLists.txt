# -------------------------------------------------------
# util cmake build script for r-tech1.
# Written by: juvinious
# Modified by: kazzmir
# -------------------------------------------------------
project (util C CXX)

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
set(UTIL_SRC
${CMAKE_CURRENT_SOURCE_DIR}/argument.cpp
${CMAKE_CURRENT_SOURCE_DIR}/configuration.cpp
${CMAKE_CURRENT_SOURCE_DIR}/network/network.cpp
${CMAKE_CURRENT_SOURCE_DIR}/network/chat.cpp
${CMAKE_CURRENT_SOURCE_DIR}/network/irc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/token.cpp
${CMAKE_CURRENT_SOURCE_DIR}/resource.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tokenreader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/timedifference.cpp
${CMAKE_CURRENT_SOURCE_DIR}/debug.cpp
${CMAKE_CURRENT_SOURCE_DIR}/timer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/init.cpp
${CMAKE_CURRENT_SOURCE_DIR}/utf.cpp
${CMAKE_CURRENT_SOURCE_DIR}/console.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/keyboard.cpp
${CMAKE_CURRENT_SOURCE_DIR}/loading.cpp
${CMAKE_CURRENT_SOURCE_DIR}/messages.cpp
${CMAKE_CURRENT_SOURCE_DIR}/graphics/bitmap.cpp
${CMAKE_CURRENT_SOURCE_DIR}/graphics/image.cpp
${CMAKE_CURRENT_SOURCE_DIR}/events.cpp
${CMAKE_CURRENT_SOURCE_DIR}/font.cpp
${CMAKE_CURRENT_SOURCE_DIR}/font_factory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/graphics/fire.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ftalleg.cpp
${CMAKE_CURRENT_SOURCE_DIR}/funcs.cpp
${CMAKE_CURRENT_SOURCE_DIR}/directory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/file-system.cpp
${CMAKE_CURRENT_SOURCE_DIR}/graphics/gradient.cpp
${CMAKE_CURRENT_SOURCE_DIR}/ebox.cpp
${CMAKE_CURRENT_SOURCE_DIR}/regex.cpp
${CMAKE_CURRENT_SOURCE_DIR}/language-string.cpp
${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/text-input.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/input-manager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/input-source.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/joystick.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/allegro5/joystick.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/touch.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/allegro5/touch.cpp
${CMAKE_CURRENT_SOURCE_DIR}/xenon/xenon.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system.cpp
${CMAKE_CURRENT_SOURCE_DIR}/version.cpp
${CMAKE_CURRENT_SOURCE_DIR}/compress.cpp
${CMAKE_CURRENT_SOURCE_DIR}/message-queue.cpp
${CMAKE_CURRENT_SOURCE_DIR}/input/linux_joystick.cpp
${CMAKE_CURRENT_SOURCE_DIR}/exceptions/load_exception.cpp
${CMAKE_CURRENT_SOURCE_DIR}/windows/system.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nacl/module.cpp
${CMAKE_CURRENT_SOURCE_DIR}/nacl/network-system.cpp
${CMAKE_CURRENT_SOURCE_DIR}/exceptions/exception.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/actionfactory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/action_speed.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/menu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/font-info.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/menu_action.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/menu_option.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/options.cpp
${CMAKE_CURRENT_SOURCE_DIR}/menu/optionfactory.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/animation.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/box.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/container.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/context-box.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/coordinate.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/cutscene.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/fadetool.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/lineedit.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/list.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/rectarea.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/popup-box.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/scroll-list.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/select-list.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/tab-container.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/tabbed-box.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/timer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gui/widget.cpp
${CMAKE_CURRENT_SOURCE_DIR}/sound/sound.cpp
${CMAKE_CURRENT_SOURCE_DIR}/sound/audio.cpp
${CMAKE_CURRENT_SOURCE_DIR}/sound/music-player.cpp
${CMAKE_CURRENT_SOURCE_DIR}/sound/music-renderer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/sound/music.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/timer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/allegro5/timer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/system/allegro5/init.cpp
PARENT_SCOPE)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
#include_directories(include include/internal)

# -------------------------------------------------------
# module
# -------------------------------------------------------
# add_library (util_module ${UTIL_SRC})
