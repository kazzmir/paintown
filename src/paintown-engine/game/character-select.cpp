#include "character-select.h"

#include "util/resource.h"
#include "util/graphics/bitmap.h"
#include "util/debug.h"
#include "util/font.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/exceptions/load_exception.h"

#include "util/gui/select-list.h"

#include "paintown-engine/object/player.h"
#include "globals.h"
#include "paintown-engine/object/display_character.h"

#include <sstream>

/* BORROWED from select_player.cpp */
static const char * DEBUG_CONTEXT = __FILE__;

struct Profile{
    Window window;
    Util::ReferenceCount<Graphics::Bitmap> bitmap;
    bool facingRight;
    double scale;
    Gui::Animation::Depth depth;
};

struct playerInfo{
    Util::ReferenceCount<Paintown::DisplayCharacter> guy;
    Filesystem::AbsolutePath path;
    playerInfo(Util::ReferenceCount<Paintown::DisplayCharacter> guy, const Filesystem::AbsolutePath & path ):
    guy(guy),
    path(path){
    }
};

static Util::ReferenceCount<playerInfo> getPlayer(const Filesystem::AbsolutePath & file){
    Global::debug(1, DEBUG_CONTEXT) << "Checking " << file.path() << std::endl;
    if (Storage::instance().exists(file)){
        Global::debug(1, DEBUG_CONTEXT) << "Loading " << file.path() << std::endl;
        return Util::ReferenceCount<playerInfo>(new playerInfo(Util::ReferenceCount<Paintown::DisplayCharacter>(new Paintown::DisplayCharacter(file)), file));
    }
    throw LoadException(__FILE__, __LINE__, "File '" + file.path() + "' not found.");
}

static void overlayFiles(const std::string & path){
    std::vector<Filesystem::AbsolutePath> files = Storage::instance().getContainerFiles(Storage::instance().find(Filesystem::RelativePath(path + "/")));
    Filesystem::AbsolutePath place = Storage::instance().find(Filesystem::RelativePath(path));
    for (std::vector<Filesystem::AbsolutePath>::iterator it = files.begin(); it != files.end(); it++ ){
        Filesystem::AbsolutePath container = *it;
        Storage::instance().addOverlay(container, place);
    }
}

typedef std::vector<Util::ReferenceCount<playerInfo> > PlayerVector;
static PlayerVector loadPlayers(const std::string & path){
    PlayerVector players;

    overlayFiles(path);

    std::vector<Filesystem::AbsolutePath> files = Storage::instance().getFiles(Storage::instance().find(Filesystem::RelativePath(path + "/")), "*" );
    std::sort(files.begin(), files.end());
    for ( std::vector<Filesystem::AbsolutePath>::iterator it = files.begin(); it != files.end(); it++ ){
        try{
            // std::string ourPath = (*it).path();
            Filesystem::AbsolutePath ourPath = it->join(Filesystem::RelativePath(it->getLastComponent() + ".txt"));
            // Global::debug(2, DEBUG_CONTEXT) << "Found file " << ourPath << std::endl;
            // std::string file = ourPath + "/" + ourPath.substr(ourPath.find_last_of('/') + 1) + ".txt";
            players.push_back(getPlayer(ourPath));
        } catch (const LoadException & le){
            Global::debug(0, DEBUG_CONTEXT) << "Could not load because " << le.getTrace() << std::endl;
        }
    }
    return players;
}

static std::vector<Paintown::DisplayCharacter *> getCharacters(PlayerVector players){
    std::vector<Paintown::DisplayCharacter *> characters;
    for (PlayerVector::iterator it = players.begin(); it != players.end(); it++){
        characters.push_back((*it)->guy.raw());
    }

    return characters;
}

/* END Borrow */

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

static bool parseBaseList(Util::ReferenceCount<Gui::SelectListInterface> list, std::map<int, unsigned int> & cursorLocations, const Token * token){
    int x = 0, y = 0;
    std::string bool_value;
    if (token->match("cell-dimensions", x, y)){
        list->setCellDimensions(x, y);
        return true;
    } else if (token->match("cell-spacing", x, y)){
        list->setCellSpacing(x, y);
        return true;
    } else if (token->match("cell-margins", x, y)){
        list->setCellMargins(x, y);
        return true;
    } else if (token->match("start-offset", x, y)){
        list->setStartingOffset(x, y);
        return true;
    } else if (token->match("cursors", x)){
        list->setCursors(x);
        return true;
    } else if (token->match("cursor-location", x, y)){
        cursorLocations[x] = y;
        return true;
    } else if (token->match("access-empty", bool_value)){
        bool value = false;
        if (bool_value == "true"){
            value = true;
        } else if (bool_value == "false"){
            value = false;
        }
        list->setAccessEmpty(value);
        return true;
    } else if (token->match("wrap", bool_value)){
        bool value = false;
        if (bool_value == "true"){
            value = true;
        } else if (bool_value == "false"){
            value = false;
        }
        list->setWrap(value);
        return true;
    }
    return false;
}

static void parseSimpleList(Util::ReferenceCount<Gui::SimpleSelect> list, std::map<int, unsigned int> & cursorLocations, const Token * token){
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
            if (parseBaseList(list.convert<Gui::SelectListInterface>(), cursorLocations, tok)){
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
                Global::debug(0) << "Unknown Simple List property: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Simple Select parse error");
        }
    }
}

static void parseGridList(Util::ReferenceCount<Gui::GridSelect> list, std::map<int, unsigned int> & cursorLocations, const Token * token){
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
            if (parseBaseList(list.convert<Gui::SelectListInterface>(), cursorLocations, tok)){
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
                Global::debug(0) << "Unknown Grid List property: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Grid Select parse error");
        }
    }
}

static bool handleGradient(const Token * token, Effects::Gradient & gradient){
    if (*token == "color"){
        int r, g, b;
        token->view() >> r >> g >> b;
        r = Util::clamp(r, 0, 255);
        g = Util::clamp(g, 0, 255);
        b = Util::clamp(b, 0, 255);
        gradient = Effects::Gradient(2, Graphics::makeColor(r, g, b), Graphics::makeColor(r, g, b));
        return true;
    } else if (*token =="gradient"){
        gradient = Effects::Gradient(token);
        return true;
    }
    return false;
}

static Effects::Gradient defaultGradient(){
    return Effects::Gradient(2, Graphics::makeColor(255, 255, 255), Graphics::makeColor(255, 255, 255));
}

TextMessage::TextMessage():
x(0),
y(0),
depth(Gui::Animation::BackgroundTop),
width(0),
height(0),
justification(Right),
cursorAssociation(-1),
gradient(defaultGradient()){
}

TextMessage::TextMessage(const TextMessage & copy):
name(copy.name),
message(copy.message),
replace(copy.replace),
x(copy.x),
y(copy.y),
depth(copy.depth),
font(copy.font),
width(copy.width),
height(copy.height),
justification(copy.justification),
cursorAssociation(copy.cursorAssociation),
gradient(copy.gradient){
}

TextMessage::TextMessage(const Token * token):
x(0),
y(0),
depth(Gui::Animation::BackgroundTop),
width(0),
height(0),
justification(Right),
cursorAssociation(-1),
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
            if (tok->match("name", name)){
            } else if (tok->match("message", message)){
            } else if (tok->match("replace-message", replace)){
            } else if (tok->match("location", x, y)){
            } else if (tok->match("depth", match_text, level)){
                depth = parseDepth(match_text, level);
            } else if (handleGradient(tok, gradient)){
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
            } else if (tok->match("cursor-association", cursorAssociation)){
            } else {
                Global::debug(0) << "Unknown Text Message List property: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Text message parse error");
        }
    }
}

TextMessage::~TextMessage(){
}

void TextMessage::act(){
    gradient.update();
}

void TextMessage::draw(const Graphics::Bitmap & work, const Font & passedFont){
    // Our font TODO get passedFont instead of defaultFont (resized)
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
    depth = copy.depth;
    font = copy.font;
    width = copy.width;
    height = copy.height;
    justification = copy.justification;
    cursorAssociation = copy.cursorAssociation;
    gradient = copy.gradient;
    
    return *this;
}

/*! Cell Data shape and drawings */
class CellData{
public:
    CellData(const Token *);
    ~CellData();
    void act();
    void draw(int x, int y, int width, int height, const Graphics::Bitmap &) const;
    enum Shape{
        SQUARE,
        ROUND,
    };
    inline const std::string & getName() const {
        return this->name;
    }
private:
    /*! Name */
    std::string name;
    /*! Shape of the cell */
    Shape shape;
    /*! Radius if it is round */
    int radius;
    /*! cell gradient */
    Effects::Gradient gradient;
    /*! cell alpha */
    int alpha;
    /*! color fill */
    bool fill;
    /*! Thickness of cell */
    int thickness;
    /*! cell */
    Util::ReferenceCount<Gui::Animation> cell;
};

CellData::CellData(const Token * token):
shape(SQUARE),
radius(0),
gradient(defaultGradient()),
alpha(255),
fill(false),
thickness(1){
    if ( *token != "cell" ){
        throw LoadException(__FILE__, __LINE__, "Not a cell");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            std::string string_match;
            if (tok->match("name", name)){
            } else if (tok->match("shape", string_match)){
                if (string_match == "square"){
                    shape = SQUARE;
                } else if (string_match == "round"){
                    shape = ROUND;
                }
            } else if (tok->match("radius",radius)){
            } else if (handleGradient(tok, gradient)){
            } else if (tok->match("alpha",alpha)){
            } else if (tok->match("thickness",thickness)){
            } else if (tok->match("color-fill",string_match)){
                if (string_match == "true"){
                    fill = true;
                } else if (string_match == "false"){
                    fill = false;
                }
            } else if (*tok == "animation"){
                cell = Util::ReferenceCount<Gui::Animation>(new Gui::Animation(tok));
            } else {
                Global::debug(0) << "Unknown Cell property: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Cell parse error");
        }
    }
}

CellData::~CellData(){
}

void CellData::act(){
    gradient.update();
    if (cell != NULL){
        cell->act();
    }
}

void CellData::draw(int x, int y, int width, int height, const Graphics::Bitmap & work) const {
    if (cell != NULL){
        Graphics::Bitmap::transBlender( 0, 0, 0, alpha );
        cell->draw(x,y,width,height,work);
    } else {
        Graphics::Bitmap::transBlender( 0, 0, 0, alpha );
        if (fill){
            switch (shape){
                case ROUND:
                    work.circleFill(x+width/2, y+height/2, radius, gradient.current());
                    break;
                case SQUARE:
                default:
                    work.rectangleFill(x, y, x+width, y+height, gradient.current());
                    break;
            }
        } else {
            switch (shape){
                case ROUND:{
                    Graphics::Bitmap temp = Graphics::Bitmap(work, x + width / 2 - radius, y + height / 2 - radius, radius*2 + thickness, radius*2 + thickness);
                    temp.circle(radius, radius, radius, thickness, gradient.current());
                    break;
                }
                case SQUARE:{
                default:
                    //const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(width+1,height+1);
                    Graphics::Bitmap temp = Graphics::Bitmap(work, x, y, width+1, height+1);
                    temp.border(0, thickness, gradient.current());
                    break;
                }
            }
        }
    }
}

class CharacterItem: public Gui::SelectItem {
public:
    CharacterItem(unsigned int index, const Util::ReferenceCount<Gui::SelectListInterface> parent, Util::ReferenceCount<playerInfo> player, const std::map<std::string, Util::ReferenceCount<CellData> > &);
    ~CharacterItem();
    void act();
    void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const;
    void drawProfile(const Profile &, const Graphics::Bitmap &, const Font &) const;
    inline bool isEmpty() const {
        return false;
    }
    const std::string getName();
    Util::ReferenceCount<playerInfo> getPlayer();
    
private:
    unsigned int index;
    const Util::ReferenceCount<Gui::SelectListInterface> parent;
    Util::ReferenceCount<playerInfo> player;
    const std::map<std::string, Util::ReferenceCount<CellData> > & cells;
};



CharacterItem::CharacterItem(unsigned int index, const Util::ReferenceCount<Gui::SelectListInterface> parent, Util::ReferenceCount<playerInfo> player, const std::map<std::string, Util::ReferenceCount<CellData> > & cells):
index(index),
parent(parent),
player(player),
cells(cells){ 
}

CharacterItem::~CharacterItem(){
}

void CharacterItem::act(){
    Util::ReferenceCount<Paintown::DisplayCharacter> character = player->guy;
    if (character->isLoaded()){
        if (character->testAnimation()){
            character->testReset();
        }
    }
}

typedef std::map<std::string, Util::ReferenceCount<CellData> > cellmap;
void CharacterItem::draw(int x, int y, int width, int height, const Graphics::Bitmap & bmp, const Font & font) const{
    cellmap::const_iterator back = cells.find("back");
    cellmap::const_iterator top = cells.find("top");
    cellmap::const_iterator select0 = cells.find("select0");
    cellmap::const_iterator select1 = cells.find("select1");
    cellmap::const_iterator selectAlt = cells.find("select-alternative");
    
    if (back != cells.end()){
        back->second->draw(x,y,width,height,bmp);
    }
    // Player 
    Util::ReferenceCount<Paintown::DisplayCharacter> displayed = player->guy;
    if (displayed->isLoaded()){
        Graphics::Bitmap temp = Graphics::Bitmap(width, height);
        temp.clearToMask();
        Paintown::Character smaller(*displayed);
        smaller.setX( width / 2 );
        smaller.setY( 0 );
        smaller.setZ( smaller.getHeight() );
        smaller.draw( &temp, 0, 0 );
        temp.drawStretched(x, y, width, height, bmp);
    } else {
        const int length = font.textLength(displayed->getName().c_str());
        const int middle = font.getHeight()/4;
        Graphics::Bitmap temp = Graphics::Bitmap(length, font.getHeight());
        temp.clearToMask();
        font.printf(0, middle, Graphics::makeColor(255, 255, 255), temp, player->guy->getName(), 0);
        // temp.draw(x, y, bmp);
        double widthRatio = (double) width / (double) temp.getWidth();
        double heightRatio = (double) temp.getHeight() / (double) height;
        double use = (double) height / (2.5 * temp.getHeight());
        Graphics::Bitmap area(bmp, x, y, width, height);
        temp.drawStretched(1, 0, temp.getWidth() * use, temp.getHeight() * use, area);
    }
    
    if (top != cells.end()){
        top->second->draw(x,y,width,height,bmp);
    }
    
    if (parent->totalCursors() > 1){
        if (parent->getCurrentIndex(0) == parent->getCurrentIndex(1) && parent->getCurrentIndex(0) == index){
            if (selectAlt != cells.end()){
                selectAlt->second->draw(x,y,width,height,bmp);
            }
        } else if (parent->getCurrentIndex(0) == index){
            if (select0 != cells.end()){
                select0->second->draw(x,y,width,height,bmp);
            }
        } else if (parent->getCurrentIndex(1) == index){
            if (select1 != cells.end()){
                select1->second->draw(x,y,width,height,bmp);
            }
        }
    } else {
        if (parent->getCurrentIndex(0) == index){
            if (select0 != cells.end()){
                select0->second->draw(x,y,width,height,bmp);
            }
        } 
    }
}

void CharacterItem::drawProfile(const Profile & profile, const Graphics::Bitmap & work, const Font & font) const {
    profile.bitmap->clearToMask();
    const int stand = 50;
    Util::ReferenceCount<Paintown::DisplayCharacter> character = player->guy;
    if (profile.facingRight){
        character->setFacing(Paintown::Object::FACING_RIGHT);
    } else {
        character->setFacing(Paintown::Object::FACING_LEFT);
    }
    Paintown::Character copy(*character);
    Graphics::Bitmap temp(copy.getWidth(), copy.getHeight() * 2);
    temp.clearToMask();
    copy.setDrawShadow(false);
    copy.setX(temp.getWidth()/2);
    copy.setY(0);
    copy.setZ(copy.getHeight());
    
    /* FIXME: the y parameter actually isn't used in the outline and reflection. */
    copy.drawOutline(&temp, 0, temp.getHeight() - stand - stand, 0, 0, 0, 255);
    copy.drawReflection(&temp, 0, temp.getHeight() - stand - stand, 128);
    copy.draw(&temp, 0, temp.getHeight()/2);

    /*
    double widthRatio = (double) profile.bitmap->getWidth() / temp.getWidth();
    double heightRatio = (double) profile.bitmap->getHeight() / temp.getHeight();

    / * use smallest ratio * /
    double use = widthRatio < heightRatio ? widthRatio : heightRatio;
    */

    // temp.draw(work.getWidth() / 2 - temp.getWidth() / 2, work.getHeight() / 2 - temp.getHeight() / 2, work);

    temp.drawStretched(profile.bitmap->getWidth() / 2 - temp.getWidth() * profile.scale / 2,
                       profile.bitmap->getHeight() / 2 - temp.getHeight() * profile.scale / 2,
                       temp.getWidth() * profile.scale, temp.getHeight() * profile.scale,
                       *profile.bitmap);
    
    profile.bitmap->draw(profile.window.x, profile.window.y, work);
}

const std::string CharacterItem::getName(){
    return player->guy->getName();
}

Util::ReferenceCount<playerInfo> CharacterItem::getPlayer(){
    return player;
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
            if (tok->match("name", name)){
            } else if (*tok == "text"){
                Util::ReferenceCount<TextMessage> message(new TextMessage(tok));
                messages[message->getDepth()].push_back(message);
            } else {
                Global::debug(0) << "Unknown Text Message block property: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Message collection parse error");
        }
    }
    if (name.empty()){
        throw LoadException(__FILE__, __LINE__, "Message collection requires a name");
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
            if (message->getCursorAssociation() != -1){
                Util::ReferenceCount<Gui::SelectItem> item = list->getItemByCursor(message->getCursorAssociation());
                if (item != NULL){
                    message->setReplaceMessage(item.convert<CharacterItem>()->getName());
                }
            }
        }
    }
}

void MessageCollection::draw(const Gui::Animation::Depth & depth, const Graphics::Bitmap & work, const Font & font){
    for (std::vector<Util::ReferenceCount<TextMessage> >::iterator i = messages[depth].begin(); i != messages[depth].end(); ++i){
        Util::ReferenceCount<TextMessage> message = *i;
        if (message->getDepth() == depth){
            message->draw(work, font);
        }
    }
}

void MessageCollection::setReplaceMessage(const std::string & name, const std::string & replace){
    for (std::map<Gui::Animation::Depth, std::vector<Util::ReferenceCount<TextMessage> > >::iterator i = messages.begin(); i != messages.end(); ++i){
        for (std::vector<Util::ReferenceCount<TextMessage> >::iterator j = i->second.begin(); j != i->second.end(); ++j){
            Util::ReferenceCount<TextMessage> message = *j;
            if (message->getName() == name){
                // Set replacement message
                message->setReplaceMessage(replace);
            }
        }
    }
}

class HasMore{
public:
    HasMore(const Token *);
    ~HasMore();
    void act();
    void draw(const Gui::Animation::Depth &, const Graphics::Bitmap &);
private:
    enum Direction{
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };
    /*! location */
    int x, y, width, height;
    /*! image */
    Util::ReferenceCount<Gui::Animation> image;
    /*! Direction */
    Direction direction;
    /*! Depth */
    Gui::Animation::Depth depth;
    /*! gradient */
    Effects::Gradient gradient;
};



HasMore::HasMore(const Token * token):
x(0),
y(0),
width(80),
height(80),
direction(UP),
depth(Gui::Animation::BackgroundTop),
gradient(defaultGradient()){
    if ( *token != "more" ){
        throw LoadException(__FILE__, __LINE__, "Not a Has More context");
    }
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            std::string match_text, level;
            if (*tok == "low" || *tok == "high"){
                // Ignore
            } else if (tok->match("location",x, y)){
            } else if (tok->match("dimensions", width, height)){
            } else if (tok->match("direction", match_text)){
                if (match_text == "up"){
                    direction = UP;
                } else if (match_text == "down"){
                    direction = DOWN;
                } else if (match_text == "left"){
                    direction = LEFT;
                } else if (match_text == "right"){
                    direction = RIGHT;
                }
            } else if (tok->match("depth", match_text, level)){
                depth = parseDepth(match_text, level);
            } else if (*tok == "animation"){
                image = Util::ReferenceCount<Gui::Animation>(new Gui::Animation(tok));
            } else if (handleGradient(tok, gradient)){
            } else {
                Global::debug(0) << "Unknown Has More property: " << tok->getName() << std::endl;
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Has More parse error");
        }
    }
}

HasMore::~HasMore(){
}

void HasMore::act(){
    if (image != NULL){
        image->act();
    }
    gradient.update();
}

void HasMore::draw(const Gui::Animation::Depth & depth, const Graphics::Bitmap & work){
    if (this->depth == depth){
        if (image != NULL){
            image->draw(x, y, width, height, work);
        } else {
            switch (direction){
                case DOWN:
                    // Point 1
                    work.triangle(x, y, x + width, y, x + width / 2, y + height, gradient.current());
                    break;
                case LEFT:
                    work.triangle(x + width, y, x + width, y + height, x, y + height / 2, gradient.current());
                    break;
                case RIGHT:
                    work.triangle(x, y, x, y + height, x + width, y + height / 2, gradient.current());
                    break;
                case UP:
                default:
                    work.triangle(x, y + height, x + width, y + height, x + width / 2, y, gradient.current());
                    break;
            }
        }
    }
}

CharacterSelect::CharacterSelect():
listDepth(Gui::Animation::BackgroundTop),
autoPopulate(false),
fontWidth(15),
fontHeight(15),
currentMessages(0){
    initializeDefaults();
}

CharacterSelect::CharacterSelect(const Filesystem::AbsolutePath & filename):
listDepth(Gui::Animation::BackgroundTop),
autoPopulate(false),
fontWidth(15),
fontHeight(15),
currentMessages(0){
    initializeDefaults();
    Global::debug(1) << "Loading Character Select Screen: " << filename.path() << std::endl;
    TokenReader tr;
    Token * token = tr.readTokenFromFile(filename.path());
    load(token);
}

CharacterSelect::CharacterSelect(const Token * token):
listDepth(Gui::Animation::BackgroundTop),
autoPopulate(false),
fontWidth(15),
fontHeight(15),
currentMessages(0){
    initializeDefaults();
    load(token);
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::initializeDefaults(){
    listWindow = Window(300, 0, 340, 480);
    list = Util::ReferenceCount<Gui::SelectListInterface>(new Gui::SimpleSelect());
}

void CharacterSelect::act(){
    backgrounds.act();
    
    if (list != NULL){
        list->act();
        if (currentMessages < messageOrder.size()){
            messages[messageOrder[currentMessages]]->act(list);
        }
        
        // Update loader
        for (int i = 0; i < list->totalCursors(); ++i){
            Util::ReferenceCount<Gui::SelectItem> item = list->getItemByCursor(i);
            if (item != NULL){
                Util::ReferenceCount<CharacterItem> ourItem = item.convert<CharacterItem>();
                loader->update(ourItem->getPlayer()->guy.raw());
                ourItem->act();
            }
        }
    }
    
    // Cells
    for (std::map<std::string, Util::ReferenceCount<CellData> >::iterator i = cells.begin(); i != cells.end(); ++i){
        if (i->second != NULL){
            i->second->act();
        }
    }
    
    if (hasMoreLow != NULL){
        hasMoreLow->act();
    }
    
    if (hasMoreHigh != NULL){
        hasMoreHigh->act();
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
    if (currentMessages < messageOrder.size()-1){
        currentMessages++;
    } else {
        currentMessages = 0;
    }
}

void CharacterSelect::previousMessages(){
    if (currentMessages > 0){
        currentMessages--;
    } else {
        currentMessages = messageOrder.size()-1;
    }
}

void CharacterSelect::changeToMessages(const std::string & name){
    int valid = -1;
    for (unsigned int i = 0; i < messageOrder.size(); ++i){
        if (name == messageOrder[i]){
            valid = i;
        }
    }
    if (valid != -1){
        currentMessages = valid;
    }
}

Util::ReferenceCount<MessageCollection> CharacterSelect::getMessages(const std::string & name){
    std::map<std::string, Util::ReferenceCount<MessageCollection> >::iterator message_iterator = messages.find(name);
    if (message_iterator != messages.end()){
        return message_iterator->second;
    }
    return Util::ReferenceCount<MessageCollection>(NULL);
}

void CharacterSelect::moveUp(int cursor){
    playSound(Movement);
    playSound(Up);
    getList()->up(cursor);
}

void CharacterSelect::moveDown(int cursor){
    playSound(Movement);
    playSound(Down);
    getList()->down(cursor);
}

void CharacterSelect::moveLeft(int cursor){
    playSound(Movement);
    playSound(Left);
    getList()->left(cursor);
}

void CharacterSelect::moveRight(int cursor){
    playSound(Movement);
    playSound(Right);
    getList()->right(cursor);
}
            
int CharacterSelect::getRemap(int cursor){
    Util::ReferenceCount<CharacterItem> selected = getList()->getItemByCursor(cursor).convert<CharacterItem>();
    Util::ReferenceCount<Paintown::DisplayCharacter> character = selected->getPlayer()->guy;
    return character->getCurrentMap();
}

void CharacterSelect::swap(int cursor){
    playSound(Swap);
    Util::ReferenceCount<CharacterItem> selected = getList()->getItemByCursor(cursor).convert<CharacterItem>();
    Util::ReferenceCount<Paintown::DisplayCharacter> character = selected->getPlayer()->guy;
    if (character->isLoaded()){
        character->nextMap();
    }
}

void CharacterSelect::playSound(const Sounds & sound){
    std::map<Sounds, std::string>::iterator play = sounds.find(sound);
    if (play != sounds.end()){
        Util::ReferenceCount<Sound> sound = Resource::getSound(Filesystem::RelativePath(play->second));
        if (sound != NULL){
            sound->play();
        }
    }
}

void CharacterSelect::load(const Token * token){
    PlayerVector players;
    std::map<int, unsigned int> cursorLocations;
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
                    parseSimpleList(simpleList, cursorLocations, tok);
                    list = simpleList.convert<Gui::SelectListInterface>();
                } else if (*tok == "grid-list"){
                    Util::ReferenceCount<Gui::GridSelect> gridList(new Gui::GridSelect());
                    parseGridList(gridList, cursorLocations, tok);
                    list = gridList.convert<Gui::SelectListInterface>();
                } else if (tok->match("auto-populate", autoPopulate)){
                } else if (tok->match("auto-populate-directory", string_match)){
                    autoPopulate = true;
                    populateFromDirectory = Filesystem::AbsolutePath(string_match);
                } else if (*tok == "list-window"){
                    tok->view() >> listWindow.x >> listWindow.y >> listWindow.width >> listWindow.height;
                } else if (tok->match("list-depth", string_match, level)){
                    listDepth = parseDepth(string_match, level);
                } else if (*tok == "profile"){
                    Util::ReferenceCount<Profile> profile(new Profile());
                    profile->facingRight = true;
                    profile->depth = Gui::Animation::BackgroundBottom;
                    tok->match("_/window", profile->window.x, profile->window.y, profile->window.width, profile->window.height);
                    std::string facing;
                    if (tok->match("_/facing", facing)){
                        if (facing == "left"){
                            profile->facingRight = false;
                        } else if (facing == "right"){
                            profile->facingRight = true;
                        }
                    }
                    tok->match("_/scale", profile->scale);
                    std::string depth, level;
                    if (tok->match("_/depth", depth, level)){
                        profile->depth = parseDepth(depth, level);
                    }
                    profile->bitmap = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(profile->window.width, profile->window.height));
                    profiles.push_back(profile);
                } else if (*tok == "messages"){
                    Util::ReferenceCount<MessageCollection> message(new MessageCollection(tok));
                    messages[message->getName()] = message;
                    messageOrder.push_back(message->getName());
                } else if (tok->match("add", string_match)){
                    players.push_back(getPlayer(Storage::instance().find(Filesystem::RelativePath(string_match))));
                } else if (tok->match("font", string_match, fontWidth, fontHeight)){
                    font = Filesystem::AbsolutePath(string_match);
                } else if (tok->match("font-dimensions", fontWidth, fontHeight)){
                } else if (*tok == "cell"){
                    Util::ReferenceCount<CellData> cell(new CellData(tok));
                    cells[cell->getName()] = cell;
                } else if (tok->match("more", string_match)){
                    if (string_match == "low"){
                        hasMoreLow = Util::ReferenceCount<HasMore>(new HasMore(tok));
                    } else if (string_match == "high"){
                        hasMoreHigh = Util::ReferenceCount<HasMore>(new HasMore(tok));
                    }
                } else if (tok->match("sound", string_match, level)){
                    Sounds sound = Nothing;
                    if (string_match == "up"){
                        sound = Up;
                    } else if (string_match == "down"){
                        sound = Down;
                    } else if (string_match == "left"){
                        sound = Left;
                    } else if (string_match == "right"){
                        sound = Right;
                    } else if (string_match == "movement"){
                        sound = Movement;
                    } else if (string_match == "select"){
                        sound = Select;
                    } else if (string_match == "quit"){
                        sound = Quit;
                    } else if (string_match == "swap"){
                        sound = Swap;
                    } else if (string_match == "misc"){
                        sound = Misc;
                    }
                    if (sound != Nothing){
                        sounds[sound] = level;
                    }
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
        PlayerVector autoload = loadPlayers(populateFromDirectory.path());
        players.insert(players.begin(), autoload.begin(), autoload.end());
    }
    
    loader = Util::ReferenceCount<Paintown::DisplayCharacterLoader>(new Paintown::DisplayCharacterLoader(getCharacters(players)));

    class DisplayThread: public Util::Thread::ThreadObject {
    public:
        DisplayThread(Util::ReferenceCount<Paintown::DisplayCharacterLoader> loader):
        ThreadObject(loader.raw(), characterLoader),
        loader(loader){
        }

        Util::ReferenceCount<Paintown::DisplayCharacterLoader> loader;

        /* run the loader in a separate thread */
        static void * characterLoader(void * arg){
            Paintown::DisplayCharacterLoader * loader = (Paintown::DisplayCharacterLoader*) arg;
            loader->load();
            return NULL;
        }

        virtual ~DisplayThread(){
            loader->stop();
        }
    };

    loadingThread = Util::ReferenceCount<Util::Thread::ThreadObject>(new DisplayThread(loader));
    
    if (!loadingThread->start()){
        throw LoadException(__FILE__, __LINE__, "Could not create loading thread");
    }
    
    for (PlayerVector::iterator i = players.begin(); i != players.end(); ++i){
        items.push_back(Util::ReferenceCount<Gui::SelectItem>(new CharacterItem(items.size(), list, *i, cells)));
    }

    list->addItems(items);
    
    // Setup cursors
    for (int i = 0; i < list->totalCursors(); ++i){
        list->setCurrentIndex(i, cursorLocations[i]);
    }
    
    Global::debug(1) << "List size is: " << list->getItems().size() << std::endl;
}
    
Filesystem::AbsolutePath CharacterSelect::getCurrentSelectedPath(int cursor){
    Util::ReferenceCount<CharacterItem> selected = getList()->getItemByCursor(cursor).convert<CharacterItem>();
    return selected->getPlayer()->path;
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
    
    // Directional arrows next
    if (hasMoreLow != NULL && list->hasMoreLow()){
        hasMoreLow->draw(depth, work);
    }
    
    if (hasMoreHigh != NULL && list->hasMoreHigh()){
        hasMoreHigh->draw(depth, work);
    }
    
    // Profiles next
    for (unsigned int i = 0; i < profiles.size(); ++i){
        Util::ReferenceCount<Profile> profile = profiles[i];
        if (profile->depth == depth){
            Util::ReferenceCount<Gui::SelectItem> item = list->getItemByCursor(i);
            if (item != NULL){
                item.convert<CharacterItem>()->drawProfile(*profile, work, listFont);
            }
        }
    }
    
    // Messages last
    if (currentMessages < messageOrder.size()){
        std::map<std::string, Util::ReferenceCount<MessageCollection> >::iterator message_iterator = messages.find(messageOrder[currentMessages]);
        if (message_iterator != messages.end()){
            message_iterator->second->draw(depth, work, listFont);
        }
    }
}
