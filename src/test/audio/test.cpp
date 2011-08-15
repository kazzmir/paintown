#include <iostream>
#include <string>
#include "util/music-player.h"

using namespace std;

void play(const string & path){
}

void initialize(){
}

int main(int argc, char ** argv){
    if (argc < 2){
        cout << "Give an audio file as an argument" << endl;
        return 0;
    }
    initialize();
    play(argv[1]);
}
