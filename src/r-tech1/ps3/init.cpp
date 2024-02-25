#ifdef PS3

#if 0
extern "C" int SDL_JoystickInit();
static void ps3JoystickHack(){
    /* FIXME: hack for the ps3. at the start of the program only 1 joystick is enabled
     * even if more than 1 is connected, so we force another call to JoystickInit
     * to pick up all joysticks.
     */
    SDL_JoystickInit();
}
#endif

#endif