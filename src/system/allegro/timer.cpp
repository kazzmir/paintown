#include "../timer.h"
#include "util/init.h"

#ifdef USE_ALLEGRO
#include <allegro.h>

namespace System{

/* game counter, controls FPS */
static void inc_speed_counter(){
    /* probably put input polling here, InputManager::poll(). no, don't do that.
     * polling is done in the standardLoop now.
     */
    Global::speed_counter4 += 1;
}
END_OF_FUNCTION(inc_speed_counter)

/* if you need to count seconds for some reason.. */
static void inc_second_counter() {
    Global::second_counter += 1;
}
END_OF_FUNCTION(inc_second_counter)

void startTimers(){
    install_int_ex(inc_speed_counter, BPS_TO_TIMER(Global::TICS_PER_SECOND));
    install_int_ex(inc_second_counter, BPS_TO_TIMER(1));
}

}

#endif
