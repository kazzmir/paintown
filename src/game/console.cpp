#include "console.h"
#include "util/bitmap.h"

Console::Console(const int maxHeight):
state(Closed),
maxHeight(maxHeight),
height(0){
}

Console::~Console(){
}
    
void Console::act(){
    double speed = 10.0;
    switch (state){
        case Closed : {
            break;
        }
        case Open : {
            break;
        }
        case Closing : {
            int distance = height;
            height -= (int)((double)distance / speed + 1.0);
            if (height <= 0){
                height = 0;
                state = Closed;
            }
            break;
        }
        case Opening : {
            int distance = maxHeight - height;
            height += (int)((double)distance / speed + 1.0);
            if (height >= maxHeight){
                height = maxHeight;
                state = Open;
            }
            break;
        }
    }
}

void Console::draw(const Bitmap & work){
    /* if we can show something */
    if (height > 0){
        work.rectangleFill(0, 0, work.getWidth(), height, Bitmap::makeColor(200,0,0));
    }
}
    
void Console::toggle(){
    switch (state){
        case Open:
        case Opening: {
            state = Closing;
            break;
        }
        case Closed:
        case Closing: {
            state = Opening;
            break;
        }
    }
}
