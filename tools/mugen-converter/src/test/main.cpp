#include <allegro.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <Python.h>

#include "character.h"

using namespace std;

static int error(const std::string & message){
    std::cout << message << std::endl;
    return -1;
}

std::string localKeys = "5900";
int enteredState = 0;

bool checkKeys(){
    if (keypressed()){
        int val = readkey();
        if (((val & 0xff) == 45) && localKeys.empty()){
            localKeys += (char)(val & 0xff);
        }
        else if ((val & 0xff) >= 48 && (val & 0xff) <= 57){
            localKeys += (char)(val & 0xff);
        } else if ((val & 0xff) == 13){
            enteredState = atoi(localKeys.c_str());
            localKeys.clear();
            return true;
        } else if ((val & 0xff) == 8){
            if (localKeys.size() > 0){
                localKeys.erase(localKeys.size()-1);
            }
        }
    }
    return false;
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
        BITMAP * buffer = create_bitmap(640, 480);
            
        try {
            Character * character = new Character(argv[1]);
            
            bool quit = false;
            while (!quit){
                if (key[KEY_ESC]){
                    quit = true;
                }
                
                character->act();
                
                poll_keyboard();
                if (checkKeys()){
                    // Change state
                    character->changeState(enteredState);
                }
                
                clear_to_color(buffer, makecol(0,0,0));
                
                
                textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "Loaded Character: %s", character->getName().c_str());
                textprintf_ex(buffer, font, 10, 20, makecol(255, 255, 255), -1, "Current State: %d", character->getCurrentStateNumber());
                
                textprintf_ex(buffer, font, 10, 60, makecol(255, 255, 255), -1, "Input new state: %s", localKeys.c_str());
                
                blit(buffer, screen, 0, 0, 0, 0, 640, 480);
            }
            
            
            delete character;
        } catch (const PyException & ex){
            error("Problem with module! Reason: " + ex.getReason());
        }
        
        destroy_bitmap(buffer);
        
        Py_Finalize();
        
        allegro_exit();
        
        return 0;
    }
    
    std::cout << "Usage: ./test character_module_name" << std::endl;
    
    return 0;
    
}
END_OF_MAIN()
