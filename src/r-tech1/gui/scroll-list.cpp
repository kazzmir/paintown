#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/funcs.h"
#include "r-tech1/gui/scroll-list.h"
#include "r-tech1/font.h"
#include <math.h>
#include "r-tech1/debug.h"

using namespace Gui;

static const double FONT_SPACER = 1.3;
// static const int GradientMax = 50;

const double EPSILON = 0.01;

/*
static int selectedGradientStart(){
    static int color = Graphics::makeColor(19, 167, 168);
    return color;
}

static int selectedGradientEnd(){
    static int color = Graphics::makeColor(27, 237, 239);
    return color;
}
*/

int justify(Justify justification, int left, int right, int size){
    switch (justification){
        case LeftJustify: return left;
        case RightJustify: return right - size;
        case CenterJustify: return (left + right) / 2 - size / 2;
    }
    return 0;
}

ScrollListInterface::ScrollListInterface():
allowWrap(true){
}
    
ScrollListInterface::~ScrollListInterface(){
}
    
ScrollItem::ScrollItem(){
}

ScrollItem::~ScrollItem(){
}

ScrollList::ScrollList():
currentIndex(0),
fontSpacingX(0),
fontSpacingY(0),
currentPosition(0),
scrollWait(0),
scrollWaitTime(4),
scrollMotion(1.2),
// selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()),
// useGradient(false),
useHighlight(false),
scroll(0),
justification(CenterJustify){}

ScrollList::ScrollList(const ScrollList & copy):
currentIndex(copy.currentIndex),
fontSpacingX(copy.fontSpacingX),
fontSpacingY(copy.fontSpacingY),
currentPosition(copy.currentPosition),
scrollWait(copy.scrollWait),
// selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()),
// useGradient(copy.useGradient),
useHighlight(copy.useHighlight),
scroll(0){}

ScrollList::~ScrollList(){}

ScrollList & ScrollList::operator=(const ScrollList & copy){
    return *this;
}

void ScrollList::act(){
    if (scrollWait == 0){
        if (scroll > EPSILON){
            // scroll -= SCROLL_MOTION;
            scroll /= scrollMotion;
        } else if (scroll < -EPSILON){
            // scroll += SCROLL_MOTION;
            scroll /= scrollMotion;
        } else {
            scroll = 0;
            currentPosition = currentIndex;
        }
    } else {
        scrollWait -= 1;
    }

    /*
    if (scrollWait > 0){
        scrollWait -= 1;
    } else {
        currentPosition = currentIndex;
    }
    */
}


/* this is the smooth scroll stuff from context-box */
void ScrollList::doDraw(int x, int y, int min_y, int max_y, const Font & font, int current, int selected, const Graphics::Bitmap & area, int direction) const {
    /* sanity check */
    if (text.size() == 0){
        return;
    }

    while (y < max_y && y > min_y){
        /* circuluar */
        int pick = current;
        while (pick < 0){
            pick += text.size();
        }
        pick = pick % text.size();

        Util::ReferenceCount<ScrollItem> option = text[pick];
        /* center justification */
        const int startx = justify(justification, 1, area.getWidth() - 1, option->size(font));

        /* the selected option will have a distance of 0 */
        int distance = current - selected;
        option->draw(x + startx, y, area, font, distance);

        if (text.size() == 1){
            return;
        }

        current += direction;
        y += direction * font.getHeight() / FONT_SPACER;
    }
}

void ScrollList::render(const Graphics::Bitmap & where, const Font & font) const {
    /* middle of the bitmap offset by the scroll amount. */
    int y = where.getHeight() / 2 + scroll * font.getHeight() / 2 - font.getHeight() / 2;

    /* allow options to be drawn a little off the bitmap */
    int min_y = 0 - font.getHeight() / FONT_SPACER;
    int max_y = where.getHeight();

    /* draw down starting from the current selection */
    doDraw(0, y, min_y, max_y, font, currentIndex, currentIndex, where, 1);

    /* then draw up, skipping the current selection */
    doDraw(0, y - font.getHeight() / FONT_SPACER, min_y, max_y, font, (int) currentIndex - 1, currentIndex, where, -1);
}

void ScrollList::addItem(const Util::ReferenceCount<ScrollItem> item){
    this->text.push_back(item);
}
    
void ScrollList::addItems(const std::vector<Util::ReferenceCount<ScrollItem> > & texts){
    this->text.insert(text.end(), texts.begin(), texts.end());
}

const std::vector<Util::ReferenceCount<ScrollItem> > & ScrollList::getItems() const{
    return this->text;
}

void ScrollList::clearItems(){
    this->text.clear();
}

void ScrollList::setPosition(const Gui::Coordinate & location){
    this->position = location;
}

bool ScrollList::next(){
    /* FIXME: probably if the current index goes past the boundary we shouldn't scroll */
    currentIndex++;
    scroll = 1;
    if (scrollWait == 0){
        scrollWait = scrollWaitTime;
    }
    if (currentIndex >= text.size()){
        if (allowWrap){
            currentIndex = 0;
            return true;
        } else {
            currentIndex = text.size()-1;
            return false;
        }
    }
    return true;
}

bool ScrollList::previous(){
    scroll = -1;
    if (scrollWait == 0){
        scrollWait = scrollWaitTime;
    }
    if (currentIndex > 0){
        currentIndex--;
    } else if (currentIndex == 0){
        if (allowWrap){
            currentIndex = text.size()-1;
            return true;
        } else {
            return false;
        }
    }
    return true;
}

bool ScrollList::setCurrentIndex(unsigned int index){
    if (index >= text.size()){
        return false;
    }
    currentIndex = index;
    return true;
}

NormalList::NormalList():
position(0),
endPosition(0),
first(-1),
last(-1),
justification(CenterJustify),
fontHeight(0),
moveToX(0), 
moveToY(0),
currentX(0), 
currentY(0),
fontBeginOffsetX(0),
fontBeginOffsetY(0),
fontSpaceX(0),
fontSpaceY(0),
visibleItems(0){
}

NormalList::~NormalList(){
}

void NormalList::act(){
    /* FIXME smooth scrolling needed (location/current?) */
    const int speed = fontHeight/2;
    if (currentY < moveToY){
        currentY += speed;
        if (currentY > moveToY){
            currentY = moveToY;
        }
    } else if (currentY > moveToY){
        currentY -= speed;
        if (currentY < moveToY){
            currentY = moveToY;
        }
    }
}

void NormalList::render(const Graphics::Bitmap & work, const Font & font) const {
    /* FIXME 
     *  - Get spacing somewhere else, for now update it here 
     *  - Get visible items properly elsewhere */
    fontHeight = (font.getHeight() / FONT_SPACER) + fontSpaceY;
    visibleItems = (work.getHeight() / fontHeight);
    if (first == -1){
        first = 0;
        last = Util::min(visibleItems, text.size());
    }
    if (position < first){
        int difference = first - position;
        first = 0;
        last -= difference;
    }
    if (position > last){
        int difference = position - last;
        last = position;
        first += difference;
    }
    first = 0;
    last = text.size();

    double y = currentY;
    for (int index = first; index < last; index += 1){
        int distance = position - index;
        Util::ReferenceCount<ScrollItem> option = text[index];
        const int x = justify(justification, 1, work.getWidth() - 1, option->size(font));
        option->draw(x, y, work, font, distance);
        y += fontHeight;
    }
}

void NormalList::addItem(const Util::ReferenceCount<ScrollItem> item){
    text.push_back(item);
}

void NormalList::addItems(const std::vector<Util::ReferenceCount<ScrollItem> > & texts){
    this->text.insert(text.end(), texts.begin(), texts.end());
}

const std::vector<Util::ReferenceCount<ScrollItem> > & NormalList::getItems() const{
    return this->text;
}

void NormalList::clearItems(){
    this->text.clear();
}
    
unsigned int NormalList::getCurrentIndex() const {
    return position;
}

bool NormalList::next(){
    // First move forward check if endpostition has been set
    checkEndPosition();
    if ((unsigned int)position < text.size() - 1){
        position += 1;
        if (allItemsViewable()){
            if (position > endPosition){
                moveToY-=fontHeight;
                endPosition++;
            }
        }
        return true;
    } else {
        if (allowWrap){
            position = 0;
            if (allItemsViewable()){
                endPosition = visibleItems-1;
                moveToY = 0;
            }
            return false;
        }
    }   
    return false;
}

bool NormalList::previous(){
    if (position > 0){
        position -= 1;
        if (allItemsViewable()){
            if (position < (endPosition - visibleItems+1)){
                moveToY+=fontHeight;
                endPosition--;
            }
        }
        return true;
    } else {
        if (allowWrap){ 
            position = text.size()-1;
            if (allItemsViewable()){
                endPosition = position;
                moveToY = ((position - visibleItems+1) * fontHeight) * -1;
            }
            return false;
        }
    }
    return false;
}

void NormalList::checkEndPosition(){
    if (endPosition == 0){
        endPosition = visibleItems-1;
    }
}

bool NormalList::allItemsViewable(){
    return ((unsigned int)visibleItems < text.size());
}
