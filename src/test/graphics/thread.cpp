#include <iostream>
#include <mutex>
#include <thread>

#include "r-tech1/file-system.h"
#include "r-tech1/debug.h"
#include "r-tech1/init.h"
#include "r-tech1/events.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/graphics/bitmap.h"

using namespace std;

enum Keys{
    Up=0,
    Down,
    Left,
    Right,
    Esc,
    Enter,
};

/* FIXME: dont put these methods in this test file */
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

int test_main(int argc, char** argv){
    DebugLog << "Thread test" << endl;

    Global::setDebug(1);

    Global::InitConditions conditions;
    // conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);

    InputManager manager;

    Graphics::Bitmap screen(*Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);

    InputMap<Keys> input;
    input.set(Keyboard::Key_ESC, 0, true, Esc);
    input.set(Keyboard::Key_ENTER, 0, true, Enter);
    input.set(Keyboard::Key_UP, 0, true, Up);
    input.set(Keyboard::Key_DOWN, 0, true, Down);
    input.set(Keyboard::Key_LEFT, 0, true, Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Right);

    Graphics::Bitmap simple1(std::string("src/test/graphics/simple.png"));

    std::mutex lock;
    Util::ReferenceCount<Graphics::Bitmap> simple2;

    /* load the bitmap in a thread */
    /*
    std::thread thread([&](){
        std::lock_guard<std::mutex> guard(lock);
        simple2 = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(std::string("src/test/graphics/simple.png")));
    });
    */

    std::function<bool()> logic = [&]()->bool{
        for (const InputMap<Keys>::InputEvent & event: InputManager::getEvents(input, InputSource(true))){
            if (event.enabled){
                if (event.out == Esc){
                    return true;
                }
            }
        }

        return false;
    };

    std::function<void(const Graphics::Bitmap&)> draw = [&](const Graphics::Bitmap& screen){
        simple1.draw(10, 10, screen);

        int x = 150;
        int y = 10;

        screen.rectangle(x-1, y-1, simple1.getWidth() + x + 1, simple1.getHeight() + y + 1, Graphics::makeColor(255, 0, 0));

        {
            std::lock_guard<std::mutex> guard(lock);
            if (simple2 != nullptr){
                simple2->draw(x, y, screen);
            }
        }
    };

    Util::standardLoop(logic, [](double ticks){ return ticks; }, draw);

    // thread.join();

    return 0;
}

int main(int argc, char** argv){
    test_main(argc, argv);
}
