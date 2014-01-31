#include "util/debug.h"
#include "util/init.h"
#include "util/graphics/bitmap.h"
#include "util/input/keyboard.h"
#include "util/input/input-source.h"
#include "util/input/input-manager.h"
#include <stdlib.h>

#include <string>

using std::string;

static void run(string path){
    Graphics::Bitmap screen(1024, 768);
    Graphics::Bitmap image(path);

    screen.clear();

    /* Plain unstretched image */
    image.Blit(0, 0, screen);

    /* Using normal stretch routines */
    Graphics::Bitmap stretch1(screen, image.getWidth() + 50, 0, image.getWidth() * 2, image.getHeight() * 2);
    image.Stretch(stretch1);

    /* Using hq2x */
    Graphics::Bitmap stretch2(screen, image.getWidth() + 50 + stretch1.getWidth() + 50, 0, image.getWidth() * 2, image.getHeight() * 2);
    image.StretchXbr(stretch2);

    Graphics::Bitmap stretch3(screen, image.getWidth() + 50, stretch1.getHeight() + 50, image.getWidth() * 3, image.getHeight() * 3);
    image.StretchXbr(stretch3);

    Graphics::Bitmap stretch4(screen, image.getWidth() + 50 + stretch1.getWidth() + 50 + stretch2.getWidth() + 50 , 0, image.getWidth() * 4, image.getHeight() * 4);
    image.StretchXbr(stretch4);

    /* done */
    screen.BlitToScreen();
    InputManager::waitForKeys(Keyboard::Key_SPACE, Keyboard::Key_ESC, InputSource(true));
}

int main(int argc, char ** argv){
    Global::InitConditions conditions;
    Global::init(conditions);
    Global::setDebug(0);
    InputManager input;
    if (argc > 1){
        run(argv[1]);
    } else {
        run("src/test/hqx/test.png");
    }
    return 0;
}
