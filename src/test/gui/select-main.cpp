#include "../common/init.h"
#include "../common/timer.h"

#include <iostream>

#include "character-select.h"

int main(int argc, char ** argv){
    if (argc > 1){
        
        std::cout << "Attempting to load file [" << argv[1] << "]..." << std::endl;
        CharacterSelect select(std::string(argv[1]));
        
        Screen::realInit();
        Common::startTimers();
        
        
        Screen::realFinish();
    } else {
        std::cout << "Usage: ./" << argv[0] << " select-screen.txt" << std::endl;
    }
    return 0;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif