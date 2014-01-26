#ifdef USE_ALLEGRO5

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "../init.h"
#include "util/debug.h"

namespace System{

void initSystem(const Global::InitConditions & conditions, Global::stream_type & out){
    out << "Allegro5 initialize " << (al_init() ? "Ok" : "Failed") << std::endl;
    uint32_t version = al_get_allegro_version();
    int major = version >> 24;
    int minor = (version >> 16) & 255;
    int revision = (version >> 8) & 255;
    int release = version & 255;
    out << "Allegro5 version " << major << "." << minor << "." << revision << "." << release << std::endl;
    out << "Init image: " << (al_init_image_addon() ? "Ok" : "Failed") << std::endl;
    out << "Init primitives " << (al_init_primitives_addon() ? "Ok" : "Failed") << std::endl;
    out << "Init keyboard " << (al_install_keyboard() ? "Ok" : "Failed") << std::endl;
    out << "Init joystick " << (al_install_joystick() ? "Ok" : "Failed") << std::endl;
    al_set_app_name("Paintown");
}

}

#endif
