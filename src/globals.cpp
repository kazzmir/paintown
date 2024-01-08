#include "globals.h"
#include <streambuf>
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/message-queue.h"
#include "r-tech1/thread.h"
#include <iostream>
#include <sstream>

using namespace std;


void Global::showTitleScreen(){
    Graphics::Bitmap s(*Storage::instance().open(titleScreen()), false);
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
