#include "util/graphics/bitmap.h"
#include "globals.h"
#include <streambuf>
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/message-queue.h"
#include "util/thread.h"
#include <iostream>
#include <sstream>

using namespace std;


void Global::showTitleScreen(){
    Graphics::Bitmap s(*Storage::instance().open(titleScreen()));
    s.BlitToScreen();
}

const Filesystem::AbsolutePath Global::titleScreen(){
    return Storage::instance().find(Filesystem::RelativePath("menu/paintown.png"));
}

namespace Global{


/*
int getScreenWidth(){
    return Graphics::Bitmap::getScreenWidth();
}

int getScreenHeight(){
    return Graphics::Bitmap::getScreenHeight();
}
*/

}
