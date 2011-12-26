#include "../common/init.h"
#include "util/debug.h"
#include <stdlib.h>

#include <string>

using std::string;

static void run(string path){
}

int main(int argc, char ** argv){
    Screen::realInit();
    atexit(Screen::realFinish);
    Global::setDebug(0);
    if (argc > 1){
        run(argv[1]);
    } else {
        run("src/test/hqx/test.png");
    }
    return 0;
}
