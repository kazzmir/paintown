#include <allegro.h>
#include <iostream>

#include <Python.h>

#include "character.h"

using namespace std;

static int error(const std::string & message){
    std::cout << message << std::endl;
    return -1;
}

int main(int argc, char ** argv){
    if (argc > 1){
        install_allegro(0, NULL, NULL);
        install_timer();
        install_keyboard();
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0){
            error("Couldn't create GFX window");
        }
        
        Py_Initialize();
        
        /* NOTE need to make sure we are trying to load in the same directory (is there a work around?) */
        PyRun_SimpleString("import sys"); 
        PyRun_SimpleString("sys.path.append('./')"); 
       
        try {
            Character * character = new Character(argv[1]);
            character->addAttribute("name", Character::String);
            std::cout << "Character Name: " << character->getStringValue("name") << std::endl;
            
            bool quit = false;
            while (!quit){
                if (key[KEY_ESC]){
                    quit = true;
                }
                
                character->act();
            }
            
            delete character;
        } catch (const PyException & ex){
            error("Problem loading module! Reason: " + ex.getReason());
        }
        
        Py_Finalize();
        
        allegro_exit();
        
        return 0;
    }
    
    std::cout << "Usage: ./test character_module_name" << std::endl;
    
    return 0;
    
}
END_OF_MAIN()
