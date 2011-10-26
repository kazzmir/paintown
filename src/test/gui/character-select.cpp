#include "character-select.h"

#include "util/bitmap.h"
#include "util/debug.h"
#include "util/font.h"
#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/load_exception.h"

#include "util/gui/select-list.h"

#include <sstream>

Gui::Animation::Depth Profile::depth = Gui::Animation::BackgroundBottom;

static Gui::Animation::Depth parseDepth(const std::string & position, const std::string & level){
    Gui::Animation::Depth depth = Gui::Animation::BackgroundBottom;
    if (position == "background"){
        if (level == "bottom"){
            depth = Gui::Animation::BackgroundBottom;
        } else if (level == "middle"){
            depth = Gui::Animation::BackgroundMiddle;
        } else if (level == "top"){
            depth = Gui::Animation::BackgroundTop;
        }
    } else if (position == "foreground"){
        if (level == "bottom"){
            depth = Gui::Animation::ForegroundBottom;
        } else if (level == "middle"){
            depth = Gui::Animation::ForegroundMiddle;
        } else if (level == "top"){
            depth = Gui::Animation::ForegroundTop;
        }
    }
    return depth;
}

static bool parseBaseList(Util::ReferenceCount<Gui::SelectListInterface> list, const Token * token){
    int x = 0, y = 0;
    bool bool_value = false;
    if (token->match("cell-dimensions", x, y)){
        list->setCellDimensions(x, y);
        return true;
    } else if (token->match("cell-spacing", x, y)){
        list->setCellSpacing(x, y);
        return true;
    } else if (token->match("cell-margins", x, y)){
        list->setCellMargins(x, y);
        return true;
    } else if (token->match("cursors", x)){
        list->setCursors(x);
        return true;
    } else if (token->match("cursor-location", x, y)){
        list->setCurrentIndex(x, y);
        return true;
    } else if (token->match("access-empty", bool_value)){
        list->setAccessEmpty(bool_value);
    } else if (token->match("wrap", bool_value)){
        list->setWrap(bool_value);
    }
    return false;
}

static void parseSimpleList(Util::ReferenceCount<Gui::SimpleSelect> list, const Token * token){
    if ( *token != "simple-list" ){
        throw LoadException(__FILE__, __LINE__, "Not a Simple Select List");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            int offset=0;
            std::string layout;
            int viewable = 0;
            if (parseBaseList(list.convert<Gui::SelectListInterface>(), tok)){
            } else if (tok->match("viewable", viewable)){
                list->setViewable(viewable);
            } else if (tok->match("layout", layout)){
                if (layout == "horizontal"){
                    list->setLayout(Gui::SimpleSelect::Horizontal);
                } else if (layout == "vertical"){
                    list->setLayout(Gui::SimpleSelect::Vertical);
                }
            } else if (tok->match("scroll-offset", offset)){
                list->setScrollOffset(offset);
            } else {
                Global::debug(0) << "Unknown Simple List property: " << token->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Simple Select parse error");
        }
    }
}

static void parseGridList(Util::ReferenceCount<Gui::GridSelect> list, const Token * token){
    if ( *token != "grid-list" ){
        throw LoadException(__FILE__, __LINE__, "Not a Grid Select List");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            int x=0,y=0;
            std::string layout;
            if (parseBaseList(list.convert<Gui::SelectListInterface>(), tok)){
            } else if (tok->match("grid-size", x, y)){
                list->setGridSize(x, y);
            } else if (tok->match("layout", layout)){
                if (layout == "static"){
                    list->setLayout(Gui::GridSelect::Static);
                } else if (layout == "infinite-vertical"){
                    list->setLayout(Gui::GridSelect::InfiniteVertical);
                } else if (layout == "infinite-horizontal"){
                    list->setLayout(Gui::GridSelect::InfiniteHorizontal);
                }
            } else {
                Global::debug(0) << "Unknown Grid List property: " << token->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Grid Select parse error");
        }
    }
}

static Effects::Gradient defaultGradient(){
    return Effects::Gradient(0, Graphics::makeColor(255, 255, 255), Graphics::makeColor(255, 255, 255));
}

TextMessage::TextMessage():
x(0),
y(0),
low_r(255),
low_g(255),
low_b(255),
high_r(255),
high_g(255),
high_b(255),
interpolateDistance(50),
depth(Gui::Animation::BackgroundTop),
width(0),
height(0),
justification(Right),
profileAssociation(-1),
gradient(defaultGradient()){
}

TextMessage::TextMessage(const TextMessage & copy):
message(copy.message),
replace(copy.replace),
x(copy.x),
y(copy.y),
low_r(copy.low_r),
low_g(copy.low_g),
low_b(copy.low_b),
high_r(copy.high_r),
high_g(copy.high_g),
high_b(copy.high_b),
interpolateDistance(copy.interpolateDistance),
depth(copy.depth),
font(copy.font),
width(copy.width),
height(copy.height),
justification(copy.justification),
profileAssociation(copy.profileAssociation),
gradient(copy.gradient){
}

static int clamp(int low, int high, int value){
    if (value < low){
        return low;
    } else if (value > high){
        return high;
    }
    return value;
}

TextMessage::TextMessage(const Token * token):
x(0),
y(0),
low_r(255),
low_g(255),
low_b(255),
high_r(255),
high_g(255),
high_b(255),
interpolateDistance(50),
depth(Gui::Animation::BackgroundTop),
width(0),
height(0),
justification(Right),
profileAssociation(-1),
gradient(defaultGradient()){
    if ( *token != "text" ){
        throw LoadException(__FILE__, __LINE__, "Not a Text Message");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            std::string match_text, level;
            if (tok->match("message", message)){
            } else if (tok->match("replace-message", replace)){
            } else if (tok->match("location", x, y)){
            } else if (tok->match("depth", match_text, level)){
                depth = parseDepth(match_text, level);
            } else if (*tok == "color"){
                int r, g, b;
                tok->view() >> r >> g >> b;
                r = clamp(0, 255, r);
                g = clamp(0, 255, g);
                b = clamp(0, 255, b);
                low_r = high_r = r;
                low_g = high_g = g;
                low_b = high_b = b;
            } else if (tok->match("color-low", low_r, low_g, low_b)){
                low_r = clamp(0, 255, low_r);
                low_g = clamp(0, 255, low_g);
                low_b = clamp(0, 255, low_b);
            } else if (tok->match("color-high", high_r, high_g, high_b)){
                high_r = clamp(0, 255, high_r);
                high_g = clamp(0, 255, high_g);
                high_b = clamp(0, 255, high_b);
            } else if (tok->match("interpolate-distance", interpolateDistance)){
            } else if (tok->match("font", match_text, width, height)){
                font = Filesystem::AbsolutePath(match_text);
            } else if (tok->match("font-dimensions", width, height)){
            } else if (tok->match("justification", match_text)){
                if (match_text == "left"){
                    justification = Left;
                } else if (match_text == "center"){
                    justification = Center;
                } else if (match_text == "right"){
                    justification = Right;
                }
            } else if (tok->match("profile-association", profileAssociation)){
            } else {
                Global::debug(0) << "Unknown Text Message List property: " << token->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Text message parse error");
        }
    }
    
    gradient = Effects::Gradient(interpolateDistance, Graphics::makeColor(low_r, low_g, low_b), Graphics::makeColor(high_r, high_g, high_b));
}

TextMessage::~TextMessage(){
}

void TextMessage::act(){
    gradient.update();
}

void TextMessage::draw(const Graphics::Bitmap & work){
    // Our font
    const Font & useFont = !font.isEmpty() ? Font::getFont(font, width, height) : Font::getDefaultFont(width, height);
    char newString[255];
    sprintf(newString, message.c_str(), replace.c_str());
    const std::string & useMessage = std::string(newString);
    int modifier;
    switch (justification){
        case Left:
            modifier = useFont.textLength(useMessage.c_str());
            break;
        case Center:
            modifier = useFont.textLength(useMessage.c_str())/2;
            break;
        case Right:
        default:
            modifier = 0;
            break;
    }
    useFont.printf(x - modifier, y, gradient.current(), work, useMessage, 0);
}

const TextMessage & TextMessage::operator=(const TextMessage & copy){
    message = copy.message;
    replace = copy.replace;
    x = copy.x;
    y = copy.y;
    low_r = copy.low_r;
    low_g = copy.low_g;
    low_b = copy.low_b;
    high_r = copy.high_r;
    high_g = copy.high_g;
    high_b = copy.high_b;
    depth = copy.depth;
    font = copy.font;
    width = copy.width;
    height = copy.height;
    justification = copy.justification;
    profileAssociation = copy.profileAssociation;
    gradient = copy.gradient;
    
    return *this;
}


CharacterItem::CharacterItem(unsigned int index, const Util::ReferenceCount<Gui::SelectListInterface> parent):
index(index),
parent(parent),
r(random() % 255),
g(random() % 255),
b(random() % 255),
letter((int)'A'+random() % 26){ 
}

CharacterItem::~CharacterItem(){
}

/* NOTE assuming only one cursor */
void CharacterItem::draw(int x, int y, int width, int height, const Graphics::Bitmap & bmp, const Font & font) const{
    bmp.rectangleFill(x, y, x+width, y+height, Graphics::makeColor(255,255,255));
    const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(font.textLength("00"), font.getHeight());
    temp.clearToMask();
    font.printf(0,0, Graphics::makeColor(0,0,0), temp, "%2d", 0, index);
    temp.BlitMasked(0,0,width, height, x, y, bmp);
    if (parent->getCurrentIndex(0) == parent->getCurrentIndex(1) && parent->getCurrentIndex(0) == index){
        bmp.rectangle(x, y, x+width, y+height, Graphics::makeColor(random() % 255, random() % 255,0));
    } else if (parent->getCurrentIndex(0) == index){
        bmp.rectangle(x, y, x+width, y+height, Graphics::makeColor(255,0,0));
    } else if (parent->getCurrentIndex(1) == index){
        bmp.rectangle(x, y, x+width, y+height, Graphics::makeColor(0,255,0));
    }
}

void CharacterItem::drawProfile(int width, int height, const Graphics::Bitmap & bmp, const Font & font) const {
    bmp.clearToMask();
    std::ostringstream randomLetter;
    randomLetter << (char)letter;
    const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(font.textLength(randomLetter.str().c_str()), font.getHeight());
    temp.clearToMask();
    font.printf(0,0, Graphics::makeColor(r,g,b), temp, randomLetter.str().c_str(),0);
    temp.Stretch(bmp,0,0,temp.getWidth(),temp.getHeight(),0,0,width, height);
}

const std::string CharacterItem::getName(){
    std::ostringstream name;
    name << index;
    return name.str();
}

MessageCollection::MessageCollection(const Token * token){
    if ( *token != "messages" ){
        throw LoadException(__FILE__, __LINE__, "Not a Message Block");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            std::string match_text, level;
            if (*tok == "text"){
                Util::ReferenceCount<TextMessage> message(new TextMessage(tok));
                messages[message->getDepth()].push_back(message);
            } else {
                Global::debug(0) << "Unknown Text Message block property: " << token->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Message collection parse error");
        }
    }
}

MessageCollection::~MessageCollection(){
}

void MessageCollection::act(Util::ReferenceCount<Gui::SelectListInterface> & list){
    for (std::map<Gui::Animation::Depth, std::vector<Util::ReferenceCount<TextMessage> > >::iterator i = messages.begin(); i != messages.end(); ++i){
        for (std::vector<Util::ReferenceCount<TextMessage> >::iterator j = i->second.begin(); j != i->second.end(); ++j){
            Util::ReferenceCount<TextMessage> message = *j;
            
            // Act
            message->act();
            
            // if associated with a profile set the replacement string
            if (message->getProfileAssociation() != -1){
                Util::ReferenceCount<CharacterItem> item = list->getItems()[list->getCurrentIndex(message->getProfileAssociation())].convert<CharacterItem>();
                message->setReplaceMessage(item->getName());
            }
        }
    }
}

void MessageCollection::draw(const Gui::Animation::Depth & depth, const Graphics::Bitmap & work){
    for (std::vector<Util::ReferenceCount<TextMessage> >::iterator i = messages[depth].begin(); i != messages[depth].end(); ++i){
        Util::ReferenceCount<TextMessage> message = *i;
        if (message->getDepth() == depth){
            message->draw(work);
        }
    }
}

CharacterSelect::CharacterSelect():
listDepth(Gui::Animation::BackgroundTop),
autoPopulate(false),
fontWidth(15),
fontHeight(15),
currentMessages(0){
}

CharacterSelect::CharacterSelect(const std::string & filename):
listDepth(Gui::Animation::BackgroundTop),
autoPopulate(false),
fontWidth(15),
fontHeight(15),
currentMessages(0){
    Global::debug(1) << "Loading Character Select Screen: " << filename << std::endl;
    TokenReader tr(Filesystem::AbsolutePath(filename).path());
    Token * token = tr.readToken();
    load(token);
}

CharacterSelect::CharacterSelect(const Token * token):
listDepth(Gui::Animation::BackgroundTop),
autoPopulate(false),
fontWidth(15),
fontHeight(15),
currentMessages(0){
    load(token);
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::act(){
    backgrounds.act();
    
    if (list != NULL){
        list->act();
        messages[currentMessages]->act(list);
    }
}

void CharacterSelect::draw(const Graphics::Bitmap & work){
    // Backgrounds
    backgrounds.render(Gui::Animation::BackgroundBottom, work);
    render(Gui::Animation::BackgroundBottom, work);
    backgrounds.render(Gui::Animation::BackgroundMiddle, work);
    render(Gui::Animation::BackgroundMiddle, work);
    backgrounds.render(Gui::Animation::BackgroundTop, work);
    render(Gui::Animation::BackgroundTop, work);
    
    // Foregrounds
    backgrounds.render(Gui::Animation::ForegroundBottom, work);
    render(Gui::Animation::ForegroundBottom, work);
    backgrounds.render(Gui::Animation::ForegroundMiddle, work);
    render(Gui::Animation::ForegroundMiddle, work);
    backgrounds.render(Gui::Animation::ForegroundTop, work);
    render(Gui::Animation::ForegroundTop, work);
}

void CharacterSelect::nextMessages(){
    if (currentMessages < messages.size()-1){
        currentMessages++;
    } else {
        currentMessages = 0;
    }
}

void CharacterSelect::previousMessages(){
    if (currentMessages > 0){
        currentMessages--;
    } else {
        currentMessages = messages.size()-1;
    }
}

void CharacterSelect::load(const Token * token){
    try {
        if ( *token != "select-screen" ){
            throw LoadException(__FILE__, __LINE__, "Not a Character Select Screen");
        }
        TokenView view = token->view();
        while (view.hasMore()){
            try{
                const Token * tok;
                view >> tok;
                std::string string_match, level;
                if (tok->match("name", name)){
                } else if (tok->match("background", string_match)){
                    backgrounds.add(Util::ReferenceCount<Gui::Animation>(new Gui::Animation(string_match)));
                } else if (*tok == "animation"){
                    backgrounds.add(Util::ReferenceCount<Gui::Animation>(new Gui::Animation(tok)));
                } else if (*tok == "simple-list"){
                    Util::ReferenceCount<Gui::SimpleSelect> simpleList(new Gui::SimpleSelect());
                    parseSimpleList(simpleList, tok);
                    list = simpleList.convert<Gui::SelectListInterface>();
                } else if (*tok == "grid-list"){
                    Util::ReferenceCount<Gui::GridSelect> gridList(new Gui::GridSelect());
                    parseGridList(gridList, tok);
                    list = gridList.convert<Gui::SelectListInterface>();
                } else if (tok->match("auto-populate", autoPopulate)){
                } else if (tok->match("auto-populate-directory", string_match)){
                    populateFromDirectory = Filesystem::AbsolutePath(string_match);
                } else if (*tok == "list-window"){
                    tok->view() >> listWindow.x >> listWindow.y >> listWindow.width >> listWindow.height;
                } else if (tok->match("list-depth", string_match, level)){
                    listDepth = parseDepth(string_match, level);
                } else if (*tok =="profile-window"){
                    Util::ReferenceCount<Profile> profile(new Profile());
                    tok->view() >> profile->window.x >> profile->window.y >> profile->window.width >> profile->window.height;
                    profile->bitmap = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(profile->window.width, profile->window.height));
                    profiles.push_back(profile);
                /* TODO - For testing only, remove later or replace with a more elegant solution to adding elements */
                } else if (tok->match("profile-depth", string_match, level)){
                    Profile::depth = parseDepth(string_match, level);
                } else if (*tok == "text"){
                    //Util::ReferenceCount<TextMessage> message(new TextMessage(tok));
                    //messages[message->getDepth()].push_back(message);
                } else if (*tok == "messages"){
                    Util::ReferenceCount<MessageCollection> message(new MessageCollection(tok));
                    messages.push_back(message);
                } else if (*tok == "add"){
                    items.push_back(Util::ReferenceCount<Gui::SelectItem>(new CharacterItem(items.size(), list)));
                } else if (tok->match("font", string_match, fontWidth, fontHeight)){
                    font = Filesystem::AbsolutePath(string_match);
                } else if (tok->match("font-dimensions", fontWidth, fontHeight)){
                } else {
                    Global::debug(0) << "Unknown Character Select property: " << tok->getName() << std::endl;
                }
            } catch ( const TokenException & ex ) {
                throw LoadException(__FILE__, __LINE__, ex, "Character Select parse error");
            } catch ( const LoadException & ex ) {
                throw ex;
            }
        }
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading Character Select file.");
    }
    
    // Setup windows
    listBitmap = new Graphics::Bitmap(listWindow.width, listWindow.height);
    
    /* Add items
     * FIXME handle auto-populate prior to adding
     */
    if (autoPopulate){
        
    }
    list->addItems(items);
    
    Global::debug(0) << "List size is: " << list->getItems().size() << std::endl;
}

void CharacterSelect::render(const Gui::Animation::Depth & depth, const Graphics::Bitmap & work){
    // Our font
    const Font & listFont = !font.isEmpty() ? Font::getFont(font, fontWidth, fontHeight) : Font::getDefaultFont(fontWidth, fontHeight);
    
    // Lists first
    if (list != NULL && listDepth == depth){
        listBitmap->clearToMask();
        list->render(*listBitmap, listFont);
        listBitmap->draw(listWindow.x, listWindow.y, work);
    }
    
    // Profiles next
    for (unsigned int i = 0; i < profiles.size(); ++i){
        Util::ReferenceCount<Profile> profile = profiles[i];
        if (profile->depth == depth){
            Util::ReferenceCount<CharacterItem> item = list->getItems()[list->getCurrentIndex(i)].convert<CharacterItem>();
            item->drawProfile(profile->window.width, profile->window.height, *profile->bitmap, listFont);
            profile->bitmap->draw(profile->window.x, profile->window.y, work);
        }
    }
    
    messages[currentMessages]->draw(depth, work);
}