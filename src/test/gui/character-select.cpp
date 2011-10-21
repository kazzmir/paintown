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
            int offset=0;;
            std::string layout;
            bool viewable = false;
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
                Global::debug(0) << "Uknown Simple List property: " << token->getName() << std::endl;
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
                Global::debug(0) << "Uknown Grid List property: " << token->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Grid Select parse error");
        }
    }
}

CharacterItem::CharacterItem(unsigned int index, const Util::ReferenceCount<Gui::SelectListInterface> parent):
index(index),
parent(parent){ 
}

CharacterItem::~CharacterItem(){
}

void CharacterItem::draw(int x, int y, int width, int height, const Graphics::Bitmap & bmp, const Font & font) const{
    bmp.rectangleFill(x, y, x+width, y+height, Graphics::makeColor(255,255,255));
    font.printf( x + width/2, y + height/2, Graphics::makeColor(0,0,0), bmp, "%d", 0, index);
    if (parent->getCurrentIndex(0) == index){
        bmp.rectangle(x, y, x+width, y+height, Graphics::makeColor(255,0,0));
    }
}

void CharacterItem::drawProfile(int width, int height, const Graphics::Bitmap & bmp, const Font & font) const {
    bmp.clearToMask();
    bmp.rectangleFill(width/4, height/4, width/2, height/2, Graphics::makeColor(0,0,255));
    font.printf( width/2, height/2, Graphics::makeColor(0,0,0), bmp, "%d", 0, index);
}
    

CharacterSelect::CharacterSelect():
autoPopulate(false){
}

CharacterSelect::CharacterSelect(const std::string & filename):
autoPopulate(false){
    Global::debug(1) << "Loading Character Select Screen: " << filename << std::endl;
    TokenReader tr(Filesystem::AbsolutePath(filename).path());
    Token * token = tr.readToken();
    load(token);
}

CharacterSelect::CharacterSelect(const Token * token):
autoPopulate(false){
    load(token);
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::act(){
    backgrounds.act();
    
    if (list != NULL){
        list->act();
    }
}

void CharacterSelect::draw(const Graphics::Bitmap & work){
    // Backgrounds
    backgrounds.render(Gui::Animation::BackgroundBottom, work);
    backgrounds.render(Gui::Animation::BackgroundMiddle, work);
    backgrounds.render(Gui::Animation::BackgroundTop, work);
    
    // Our font
    const Font & listFont = !font.path().empty() ? Font::getFont(font, fontWidth, fontHeight) : Font::getDefaultFont();
    
    // Select List
    if (list != NULL){
        listBitmap->clearToMask();
        list->render(*listBitmap, listFont);
        listBitmap->draw(listWindow.x, listWindow.y, work);
    }
    
    // Profiles
    for (unsigned int i = 0; i < profileWindow.size(); ++i){
        Util::ReferenceCount<Graphics::Bitmap> bitmap = profileBitmaps[i];
        Util::ReferenceCount<CharacterItem> item = list->getItems()[list->getCurrentIndex(i)].convert<CharacterItem>();
        item->drawProfile(profileWindow[i].width, profileWindow[i].height, *bitmap, listFont);
        bitmap->draw(profileWindow[i].x, profileWindow[i].y, work);
    }
    
    // Foregrounds
    backgrounds.render(Gui::Animation::ForegroundBottom, work);
    backgrounds.render(Gui::Animation::ForegroundMiddle, work);
    backgrounds.render(Gui::Animation::ForegroundTop, work);
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
                std::string string_match;
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
                } else if (*tok =="profile-window"){
                    Window window;
                    tok->view() >> window.x >> window.y >> window.width >> window.height;
                    profileWindow.push_back(window);
                /* TODO - For testing only, remove later or replace with a more elegant solution to adding elements */
                } else if (*tok == "add"){
                    items.push_back(Util::ReferenceCount<Gui::SelectItem>(new CharacterItem(items.size(), list)));
                } else if (tok->match("font", string_match, fontWidth, fontHeight)){
                    font = Filesystem::AbsolutePath(string_match);
                } else {
                    Global::debug(0) << "Uknown Character Select property: " << tok->getName() << std::endl;
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
    for (std::vector<Window>::iterator i = profileWindow.begin(); i != profileWindow.end(); ++i){
        const Window & window = *i;
        Util::ReferenceCount<Graphics::Bitmap> bitmap;
        bitmap = new Graphics::Bitmap(window.width, window.height);
        profileBitmaps.push_back(bitmap);
    }
    /* Add items
     * FIXME handle auto-populate prior to adding
     */
    list->addItems(items);
    
    Global::debug(0) << "List size is: " << list->getItems().size() << std::endl;
}