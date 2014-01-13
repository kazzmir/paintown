#ifdef USE_ALLEGRO

#include <allegro.h>
#ifdef ALLEGRO_WINDOWS
#include <winalleg.h>
#endif

#include "util/graphics/allegro/gif/algif.h"
#include "util/graphics/allegro/loadpng/loadpng.h"
#include "util/sound/music.h"
#include "util/events.h"
#include "util/graphics/bitmap.h"

namespace System{

/* should probably call the janitor here or something */
static void close_paintown(){
    Music::pause();
    Graphics::setGfxModeText();
#ifdef USE_ALLEGRO
    allegro_exit();
#endif
    exit(0);
}

static void close_window(){
    /* when do_shutdown is 1 the game will attempt to throw ShutdownException
     * wherever it is. If the game is stuck or the code doesn't throw
     * ShutdownException then when the user tries to close the window
     * twice we just forcifully shutdown.
     */
    Util::do_shutdown += 1;
    if (Util::do_shutdown == 2){
        close_paintown();
    }
}
END_OF_FUNCTION(close_window)

void initSystem(Global::stream_type & out){
    out << "Allegro version: " << ALLEGRO_VERSION_STR << std::endl;
    out << "Allegro init: " <<allegro_init()<<std::endl;
    out << "Install timer: " <<install_timer()<<std::endl;

    /* png */
    loadpng_init();
    algif_init();

    out<<"Install keyboard: "<<install_keyboard()<<std::endl;
    /* do we need the mouse?? */
    // out<<"Install mouse: "<<install_mouse()<<std::endl;
    out<<"Install joystick: "<<install_joystick(JOY_TYPE_AUTODETECT)<<std::endl;
    /* 16 bit color depth */
    set_color_depth(16);

    LOCK_VARIABLE( speed_counter4 );
    LOCK_VARIABLE( second_counter );
    LOCK_FUNCTION( (void *)inc_speed_counter );
    LOCK_FUNCTION( (void *)inc_second_counter );
    
    /* keep running in the background */
    set_display_switch_mode(SWITCH_BACKGROUND);

    /* close window when the X is pressed */
    LOCK_FUNCTION(close_window);
    set_close_button_callback(close_window);
}

}

#endif

