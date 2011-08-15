#include "generator.h"

using namespace std;

int main(int argc, char ** argv){
    if (argc > 2){
        Mugen::CharacterGenerator character(argv[1]);
        character.output(argv[2]);
    } else if (argc > 1){
        Mugen::CharacterGenerator character(argv[1]);
        character.output(Mugen::stripDir(Mugen::stripExtension(argv[1]) + ".py"));
    } else {
        std::cout << "Usage: ./converter character.def [character.py]\n     - Second parameter is optional, if ommited will default to the original filename" << endl;
    }
}
