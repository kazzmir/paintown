#include <atomic>
#include <functional>

#include "r-tech1/file-system.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/events.h"
#include "r-tech1/init.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"

Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

bool epsilon(double a, double small){
    return a < small;
}

void test1(){

    std::atomic<uint32_t> counter(0);

    std::function<bool()> logic = [&](){
        counter += 1;
        if (counter >= 90){
            return true;
        }

        return false;
    };

    std::function<void(const Graphics::Bitmap&)> draw = [](const Graphics::Bitmap & buffer){
    };

    TimeDifference time;
    time.startTime();

    /* 30 fps. after 3 seconds the logic loop should have executed 90 times, so counter should be 90 */
    Util::standardLoop(logic, [](double ticks){ return Global::ticksPerSecond(ticks) * 30; }, draw);

    time.endTime();

    if (counter == 90 && epsilon(abs((double) time.getTime() - 3000.0), 50)){
        DebugLog << "Success" << std::endl;
    } else {
        DebugLog << "Time difference " << (time.getTime() / 1000.0) << " counter: " << counter << ". 3 seconds should have elapsted for counter to be 90" << std::endl;
    }
}

#ifndef WINDOWS
int test_main(int argc, char** argv){
#else
#include <SDL2/SDL.h>
int test_main(int argc, char *argv[]){
#endif
    Global::setDebug(1);

    Global::InitConditions conditions;
    // conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);

    InputManager manager;

    Graphics::Bitmap screen(*Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);

    test1();

    return 0;
}

#ifndef WINDOWS
int main(int argc, char** argv){
#else
#include <SDL2/SDL.h>
int main(int argc, char *argv[]){
#endif
    return test_main(argc, argv);
}
