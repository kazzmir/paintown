#include "../common/init.h"
#include "util/debug.h"
#include "util/bitmap.h"
#include "util/timedifference.h"
#include <stdlib.h>

#include <math.h>
#include <string>

using std::string;

static void test(Graphics::Bitmap input, string size, int increase){
    Graphics::Bitmap output(input.getWidth() * increase, input.getWidth() * increase);
    for (int i = 1; i < 4; i++){
        TimeDifference timer;
        int max = pow(10, i);
        timer.startTime();
        for (int x = 0; x < max; x++){
            input.StretchHqx(output);
        }
        timer.endTime();
        Global::debug(0) << timer.printAverageTime(size, max) << std::endl;
    }
}

static void run(string path){
    Graphics::Bitmap image(path);

    test(image, "2x", 2);
    test(image, "3x", 3);
    test(image, "4x", 4);
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
