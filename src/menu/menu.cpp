#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "menu.h"
#include "menu_global.h"
#include "menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/file-system.h"
#include "resource.h"
#include "globals.h"
#include "init.h"
#include "configuration.h"
#include "music.h"
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"

#include "optionfactory.h"
#include "actionfactory.h"
#include "menu_global.h"

#include "input/input-manager.h"
#include "input/input-map.h"

#include <queue>
#include <map>
#include <ostream>
#include <sstream>

#include "gui/context-box.h"

using namespace std;
using namespace Gui;

static std::string sharedFont = "fonts/arial.ttf";
static int sharedFontWidth = 24;
static int sharedFontHeight = 24;

static const int GradientMax = 50;

Menu::Point::Point():
x(0),
y(0){
}

Menu::Point::Point(int x, int y):
x(x),
y(y){
}

Menu::Point::~Point(){
}

Menu::InfoBox::InfoBox():
state(NotActive),
font(Filesystem::RelativePath(sharedFont)),
fontWidth(sharedFontWidth),
fontHeight(sharedFontHeight),
fadeAlpha(0){
    popup.setFadeSpeed(20);
}

Menu::InfoBox::~InfoBox(){
}
 
void Menu::InfoBox::act(){
    popup.act();
    
    int speed = 9;
    switch (state){
        case Opening: {
            if (fadeAlpha < 255){
                fadeAlpha += speed;
            }

            if (fadeAlpha >= 255){
                fadeAlpha = 255;
                if (popup.isActive()){
                    state = Active;
                }
            }
            break;
        }
        case Closing: {
            if (fadeAlpha > 0){
                fadeAlpha -= speed;
            }

            if (fadeAlpha <= 0){
                fadeAlpha = 0;
                if (!popup.isActive()){
                    state = NotActive;
                }
            }
            break;
        }
        case Active:
        case NotActive:
        default:
            break;
    }
}

void Menu::InfoBox::render(const Bitmap & bmp){
    popup.render(bmp);
    
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    
    const int x1 = popup.getArea().getX()+(int)(popup.getArea().getRadius()/2);
    const int y1 = popup.getArea().getY()+2;
    const int x2 = popup.getArea().getX2()-(int)(popup.getArea().getRadius()/2);
    const int y2 = popup.getArea().getY2()-2;
    bmp.setClipRect(x1, y1, x2, y2);
    
    int sy = location.getY() - 5;
    static int white = Bitmap::makeColor(255,255,255);
    for (vector<string>::iterator it = text.begin(); it != text.end(); it++){
        string & str = *it;
        if (fadeAlpha < 255){
            Bitmap::transBlender(0, 0, 0, fadeAlpha);
            // Bitmap::drawingMode( Bitmap::MODE_TRANS );
            vFont.printf(location.getX() + 5, sy, white, bmp.translucent(), str, 0 );
            // Bitmap::drawingMode(Bitmap::MODE_SOLID);
        } else {
            vFont.printf(location.getX() + 5, sy, white, bmp, str, 0 );
        }
        sy += vFont.getHeight();
    }
    bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
}

void Menu::InfoBox::open(){
    state = Opening;
    popup.location = location;
    popup.colors = colors;
    popup.open();
    fadeAlpha = 0;
}

void Menu::InfoBox::close(){
    state = Closing;
    popup.close();
}

void Menu::InfoBox::setText(const std::string & info){
    if (info.empty()){
        return;
    }
    text.clear();
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    size_t start = 0;
    size_t last = 0;
    start = info.find("\n");
    while (start != string::npos){
        text.push_back(info.substr(last, start - last));
        last = start + 1;
        start = info.find("\n", last);
    }
    text.push_back(info.substr(last));

    int maxWidth = 0;
    int height = 0;
    for (vector<string>::iterator it = text.begin(); it != text.end(); it++){
        int w = vFont.textLength((*it).c_str()) + 10;
        if (w > maxWidth){
            maxWidth = w;
        }
        height += vFont.getHeight();
    }

    location.setDimensions(maxWidth, height);
}

static std::vector<ContextItem *> toContextList(const std::vector<MenuOption *> & list){
    std::vector<ContextItem *> contextItems;
    for (std::vector<MenuOption *>::const_iterator i = list.begin(); i != list.end(); ++i){
        contextItems.push_back(*i);
    }
    return contextItems;
}

static void tryPlaySound(const Filesystem::RelativePath & path){
    Sound * ok = Resource::getSound(path);
    if (ok != NULL){
        ok->play();
    }
}
/*
 * FIXME Exception handling for ValueHolder*/
Menu::MenuException::MenuException(const std::string & file, int line, const std::string reason ):
Exception::Base(file, line),
reason(reason){
}
Menu::MenuException::MenuException(const MenuException & copy):
Exception::Base(copy),
reason(copy.reason){
}
Menu::MenuException::MenuException(const Exception::Base & copy):
Exception::Base(copy),
reason("unknown"){
}
Menu::MenuException::~MenuException() throw(){
}
Exception::Base * Menu::MenuException::copy() const {
            return new MenuException(*this);
}

Menu::ValueHolder::ValueHolder(const std::string & name):
name(name),
location(0){
}
Menu::ValueHolder::~ValueHolder(){
}

Menu::ValueHolder::ValueHolder(const ValueHolder & copy){
    // reset position
    this->location = 0;
    this->name = copy.name;
    this->values = copy.values;
}

Menu::ValueHolder & Menu::ValueHolder::operator=(const ValueHolder & copy){
    // reset position
    this->location = 0;
    this->name = copy.name;
    this->values = copy.values;
    return *this;
}
        
Menu::ValueHolder & Menu::ValueHolder::operator<<(const std::string & val){
    values.push_back(val);
    return *this;
}
Menu::ValueHolder & Menu::ValueHolder::operator<<(bool val){
    std::ostringstream o;
    o << val;
    return *this << o.str();
}
Menu::ValueHolder & Menu::ValueHolder::operator<<(int val){
    std::ostringstream o;
    o << val;
    return *this << o.str();
}
Menu::ValueHolder & Menu::ValueHolder::operator<<(double val){
    std::ostringstream o;
    o << val;
    return *this << o.str();
}

Menu::ValueHolder & Menu::ValueHolder::operator<<(Token * tok){
    std::string temp;
    *tok >> temp;
    return *this << temp;
}


Menu::ValueHolder & Menu::ValueHolder::operator>>(std::string & val){
    if (values[location].empty()){
        throw MenuException(__FILE__, __LINE__, "Empty value.");
    }
    val = values[location];
    next();
    return *this;
}
Menu::ValueHolder & Menu::ValueHolder::operator>>(bool & val){
    if (values[location].empty()){
        throw MenuException(__FILE__, __LINE__, "Empty value.");
    }
    std::istringstream i(values[location]);
    i >> val;
    next();
    return *this;
}
Menu::ValueHolder & Menu::ValueHolder::operator>>(int & val){
    if (values[location].empty()){
        throw MenuException(__FILE__, __LINE__, "Empty value.");
    }
    std::istringstream i(values[location]);
    i >> val;
    next();
    return *this;
}
Menu::ValueHolder & Menu::ValueHolder::operator>>(double & val){
    if (values[location].empty()){
        throw MenuException(__FILE__, __LINE__, "Empty value.");
    }
    std::istringstream i(values[location]);
    i >> val;
    next();
    return *this;
}

void Menu::ValueHolder::next(){
    location++;
    if (location >= values.size()){
        location = 0;
    }
}

const std::string Menu::ValueHolder::getValues() {
    std::string temp;
    for (std::vector<std::string>::iterator i = values.begin(); i != values.end(); ++i){
        temp += *i + "; ";
    }
    return temp;
}

/* backgrounds */
Menu::Background::Background(){
}

Menu::Background::~Background(){
    for (std::map<Gui::Animation::Depth, std::vector<Gui::Animation *> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
        std::vector<Gui::Animation *> & animations = i->second;
        for (std::vector<Gui::Animation *>::iterator j = animations.begin(); j != animations.end(); ++j){
            Gui::Animation * anim = *j;
            if (anim){
                delete anim;
            }
        }
    }
}

void Menu::Background::act(const Gui::Coordinate & coord){
    for (std::map<Gui::Animation::Depth, std::vector<Gui::Animation *> >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
        std::vector<Gui::Animation *> & animations = i->second;
        for (std::vector<Gui::Animation *>::iterator j = animations.begin(); j != animations.end(); ++j){
            Gui::Animation * anim = *j;
            if (anim){
                anim->act();
            }
        }
    }
}

void Menu::Background::render(const Gui::Animation::Depth & depth, const Bitmap & bmp){
    for (std::vector<Gui::Animation *>::iterator i = backgrounds[depth].begin(); i != backgrounds[depth].end(); ++i){
        Gui::Animation * anim = *i;
        if (anim){
            anim->draw(bmp);
        }   
    }
}

void Menu::Background::add(Gui::Animation * anim){
    backgrounds[anim->getDepth()].push_back(anim);
}

Menu::Renderer::Renderer(){
}
Menu::Renderer::~Renderer(){
}

Menu::DefaultRenderer::DefaultRenderer(){
    
    // Default the menu to a certain size and details
    menu.location.setRadius(15);
    menu.location.set(-.6, -.3, .6, .8);
    menu.colors.body = Bitmap::makeColor(0,0,0);
    menu.colors.bodyAlpha = 128;
    menu.colors.border = Bitmap::makeColor(200,200,200);
    menu.colors.borderAlpha = 255;
}
Menu::DefaultRenderer::~DefaultRenderer(){
    // Kill options
    for (std::vector<MenuOption *>::iterator i = options.begin(); i != options.end(); ++i){
        if (*i){
            delete *i;
        }
    }
}
bool Menu::DefaultRenderer::readToken(Token * token){
    if( *token == "option" ) {
        try{
            MenuOption *temp = OptionFactory::getOption(token);
            if (temp){
                options.push_back(temp);
            }
        } catch (const LoadException & le){
            Global::debug(0) << "Could not read option: " << le.getTrace() << endl;
            token->print(" ");
        }
    } else if ( *token == "position" ) {
        // This handles the placement of the menu list and surrounding box
        menu.setCoordinates(token);
    } else if ( *token == "relative-position"){
        menu.setCoordinates(token);
    } else if ( *token == "coordinate"){
        menu.setCoordinates(token);
    } else if ( *token == "position-body" ) {
        // This handles the body color of the menu box
        menu.setColors(token);
    } else if ( *token == "position-border" ) {
        // This handles the border color of the menu box
        menu.setColors(token);
    } else if ( *token == "fade-speed" ) {
        // Menu fade in speed
        int speed;
        *token >> speed;
        menu.setFadeSpeed(speed);
    } else {
        return false;
    }
    
    return true;
}
void Menu::DefaultRenderer::initialize(Context & context){
    // Setup menu fonts etc
    Filesystem::RelativePath localFont("fonts/arial.ttf");
    int width = 24, height = 24;
    if (Configuration::getMenuFont() != "" && Filesystem::exists(Filesystem::RelativePath(Configuration::getMenuFont()))){
        localFont = Filesystem::RelativePath(Configuration::getMenuFont());
        width = Configuration::getMenuFontWidth();
        height = Configuration::getMenuFontHeight();
    } else if (Filesystem::exists(context.getFont())){
        localFont = context.getFont();
        width = context.getFontWidth();
        height = context.getFontHeight();
    }
    menu.setFont(localFont, width, height);
    menu.setList(toContextList(options));
    menu.open();
    
    // Menu info
    if (!context.getMenuInfoText().empty()){
        Global::debug(0,"menu") << "Menu Text: " << context.getMenuInfoText() << endl;
        menuInfo.setFont(context.getFont(),context.getFontWidth(),context.getFontHeight());
        menuInfo.setText(context.getMenuInfoText());
        const int width = menuInfo.location.getWidth();
        const int height = menuInfo.location.getHeight();
        menuInfo.location.setPosition(Gui::AbsolutePoint(context.getMenuInfoLocation().getX() - width/2, context.getMenuInfoLocation().getY() - height/2));
        // have to pass the dimensions back in to correct proper placement
        menuInfo.location.setPosition2(Gui::AbsolutePoint(menuInfo.location.getX() + width,menuInfo.location.getY() + height));
        menuInfo.location.setRadius(menu.location.getRadius());
        menuInfo.colors = menu.colors;
    }
    menuInfo.open();
    
    // Add first info option
    addInfo(options[menu.getCurrentIndex()]->getInfoText(), context); 
    
}
void Menu::DefaultRenderer::finish(){
    menu.close();
    menuInfo.close();
}
bool Menu::DefaultRenderer::active(){
    return menu.isActive();
}
void Menu::DefaultRenderer::act(){
    // FIXME find a better way to get options to update this is a waste
    for (std::vector<MenuOption *>::iterator i = options.begin(); i != options.end(); ++i){
        MenuOption * option = *i;
        option->logic();
    }
    menu.act();
    menuInfo.act();
    actInfo();
}
void Menu::DefaultRenderer::render(const Bitmap & bmp){
    menu.render(bmp);
    menuInfo.render(bmp);
    renderInfo(bmp);
}
void Menu::DefaultRenderer::addOption(MenuOption * opt){
    this->options.push_back(opt);
}
void Menu::DefaultRenderer::doAction(const Actions & action, Context & context){
    switch(action){
        case Up:
            if (menu.previous()){
                context.playSound(Up);
                addInfo(options[menu.getCurrentIndex()]->getInfoText(), context); 
            }
            break;
        case Down:
            if (menu.next()){
                context.playSound(Down);
                addInfo(options[menu.getCurrentIndex()]->getInfoText(), context); 
            }
            break;
        case Left:
            if (options[menu.getCurrentIndex()]->leftKey()){
                context.playSound(Left);
            }
            break;
        case Right:
            if (options[menu.getCurrentIndex()]->rightKey()){
                context.playSound(Right);
            }
            break;
        case Select:
            try{
                context.playSound(Select);
                options[menu.getCurrentIndex()]->run(context);
            } catch (const Exception::Return & ex){
                menu.open();
                menuInfo.open();
            }
            context.playMusic();
            addInfo(options[menu.getCurrentIndex()]->getInfoText(), context); 
            break;
        case Cancel:
            context.playSound(Cancel);
            throw Exception::Return(__FILE__, __LINE__);
            break;
        default:
            break;
    }
}

void Menu::DefaultRenderer::addInfo(const std::string & text, Context & context){
    if (text.empty()){
        return;
    }
    if (!info.empty()){
        info.back()->close();
    }
    ::Menu::InfoBox * temp = new ::Menu::InfoBox();
    temp->setFont(context.getFont(),context.getFontWidth(),context.getFontHeight());
    temp->setText(text);
    const int width = temp->location.getWidth();
    const int height = temp->location.getHeight();
    temp->location.setPosition(Gui::AbsolutePoint(context.getInfoLocation().getX() - width/2, context.getInfoLocation().getY() - height/2));
    // have to pass the dimensions back in to correct proper placement
    temp->location.setPosition2(Gui::AbsolutePoint(temp->location.getX() + width,temp->location.getY() + height));
    temp->location.setRadius(menu.location.getRadius());
    temp->colors = menu.colors;
    temp->open();
    info.push_back(temp);
}

void Menu::DefaultRenderer::actInfo(){
    for (std::vector< ::Menu::InfoBox *>::iterator i = info.begin(); i != info.end();){
        ::Menu::InfoBox *box = *i;
        box->act();
        if (!box->isActive()){
            delete box;
            i = info.erase(i);
        } else {
            i++;
        }
    }
}

void Menu::DefaultRenderer::renderInfo(const Bitmap & work){
    for (std::vector< ::Menu::InfoBox *>::iterator i = info.begin(); i != info.end(); ++i){
        ::Menu::InfoBox *box = *i;
        box->render(work);
    }
}


Menu::Context::Context():
cleanup(true),
state(NotStarted),
fades(0),
background(0),
fontWidth(24),
fontHeight(24),
infoLocation(0,-.5),
menuInfoLocation(0,.95){
}

Menu::Context::Context(const Context & parent, const Context & child):
cleanup(false),
state(NotStarted),
fades(NULL),
background(NULL),
fontWidth(24),
fontHeight(24),
infoLocation(0,-.5),
menuInfoLocation(0,.95){
    // Update with parents info
    fades = parent.fades;
    background = parent.background;
    sounds = parent.sounds;
    music = parent.music;
    font = parent.font;
    fontWidth = parent.fontWidth;
    fontHeight = parent.fontHeight;
    infoLocation = parent.infoLocation;
    menuInfoLocation = parent.menuInfoLocation;

    // Then overwrite with childs
    if (child.fades != NULL){
        fades = child.fades;
    }
    if (child.background != NULL){
        background = child.background;
    }
    if (!child.sounds.empty()){
        sounds = child.sounds;
    }
    if (Filesystem::exists(child.music)){
        music = child.music;
    }
    if (Filesystem::exists(child.font)){
        font = child.font;
        fontWidth = child.fontWidth;
        fontHeight = child.fontHeight;
    }
    if (child.infoLocation.getRelativeX() != 0 || child.infoLocation.getRelativeY() != -.5){
        infoLocation = child.infoLocation;
    }
    if (child.menuInfoLocation.getRelativeX() != 0 || child.menuInfoLocation.getRelativeY() != .95){
        infoLocation = child.infoLocation;
    }
    if (!child.menuInfo.empty()){
        menuInfo = child.menuInfo;
    }
}

Menu::Context::~Context(){
    // Only delete if required
    if (cleanup){
        if (fades != NULL){
            delete fades;
        }
        if (background != NULL){
            delete background;
        }
    }
}

void Menu::Context::parseToken(Token * token){
    if ( *token != "context" ){
        throw LoadException(__FILE__, __LINE__, "Not a menu context");
    } else if (!token->hasTokens()){
        return;
    } 
    // Token
    Token * tok;
    *token >> tok;
    
    while (tok->hasTokens()){
        Token * context;
        *tok >> context;
        if (*context == "fade"){
            // Fade info
            if (!fades){
                fades = new Gui::FadeTool();
            }
            // Set fader default to white
            fades->setFadeInColor(Bitmap::makeColor(255,255,255));
            fades->setFadeOutColor(Bitmap::makeColor(255,255,255));
            fades->setFadeInTime(25);
            fades->setFadeOutTime(12);

            // Load defaults
            fades->parseDefaults(context);
        } else if (*context == "animation" || *context == "background"){  
            // Backgrounds
            addBackground(context);
        } else if (*context == ""){
        }
    }

}

void Menu::Context::addBackground(Token * token){
    // Backgrounds
    if (!background){
        background = new Background();
    }
    background->add(new Gui::Animation(token));
}

void Menu::Context::addBackground(const std::string & image){
    // Backgrounds
    if (!background){
        background = new Background();
    }
    background->add(new Gui::Animation(image));
}

void Menu::Context::initialize(){
    if (fades){
        // state
        state = Initializing;
        // set fader state
        fades->setState(FadeTool::FadeIn);
    } else {
        // Running
        state = Running;
    }
}
void Menu::Context::finish(){
    if (fades){
        // state
        state = Finishing;
        // set fader state
        fades->setState(FadeTool::FadeOut);
    } else {
        // Completed
        state = Completed;
    }
}

void Menu::Context::playSound(const Actions & sound){
    if (Filesystem::exists(sounds[sound])){
        tryPlaySound(sounds[sound]);
    }
}

void Menu::Context::addSound(const Actions & sound, const Filesystem::RelativePath & path){
    sounds[sound] = path;
}

void Menu::Context::playMusic(){
    if (Filesystem::exists(music)){
        if (Music::loadSong(Filesystem::find(music).path())){
            Music::pause();
            Music::play();
        }
    }
}

void Menu::Context::act(){
    // fader
    if (fades){
       fades->act();
       if (state == Initializing){
           if(fades->getState() == Gui::FadeTool::NoFade){
               state = Running;
           }
       } else if (state == Finishing){
           if(fades->getState() == Gui::FadeTool::EndFade){
               state = Completed;
           }
       }
    }
    // Backgrounds
    if (background){    
        background->act(Gui::Coordinate(Gui::AbsolutePoint(0,0),Gui::AbsolutePoint(Menu::Width, Menu::Height)));
    }
}
void Menu::Context::render(Renderer * renderer, const Bitmap & bmp){
    if (background){
        // background
        background->render(Gui::Animation::BackgroundBottom, bmp);
        background->render(Gui::Animation::BackgroundMiddle, bmp);
        background->render(Gui::Animation::BackgroundTop, bmp);
    } else {
        bmp.fill(Bitmap::makeColor(0,0,0));
    }
    
    // Menu
    if (renderer){
        renderer->render(bmp);
    }
    
    if (background){
        // foreground
        background->render(Gui::Animation::ForegroundBottom, bmp);
        background->render(Gui::Animation::ForegroundMiddle, bmp);
        background->render(Gui::Animation::ForegroundTop, bmp);
    }
    
    // Fades
    if (fades){
        fades->draw(bmp);
    }
}
void Menu::Context::setFadeTool(Gui::FadeTool *fade){
    fades = fade;
}
void Menu::Context::setBackground(Background *bg){
    background = bg;
}

/* New Menu */

// Utilizes default renderer
Menu::Menu::Menu():
renderer(new DefaultRenderer()){
}
Menu::Menu::Menu(const Filesystem::AbsolutePath & filename):
renderer(0){
    // Load up tokenizer
    try{
        Global::debug(1,"menu") << "Loading menu " << filename.path() << endl;
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading menu");
    }
}

Menu::Menu::Menu(Token * token):
renderer(0){
    load(token);
}

Menu::Menu::~Menu(){
    // Kill values
    for (std::map<string,ValueHolder *>::iterator i = data.begin(); i != data.end(); ++i){
        if (i->second){
            delete i->second;
        }
    }

    // Kill renderer
    if (renderer){
        delete renderer;
    }
}

void Menu::Menu::load(Token * token){ 
    // version info;
    int major=0, minor=0, micro=0;
    if ( *token != "menu" ){
        throw LoadException(__FILE__, __LINE__, "Not a menu");
    } else if (!token->hasTokens()){
        return;
    } else {
        // Get version
        Token * tok;
        *token >> tok;
        Token *ourToken = tok->findToken("version");
        if (ourToken != NULL){
            try {
                *ourToken >> major >> minor >> micro;
            } catch (const TokenException & ex){
            }
        } else {
                Global::debug(0, "menu") << "No version indicated, assuming 3.3.1 or below." << endl;
                major = 3;
                minor = 3;
                micro = 1; 
                // Create default rendere which is compatible with 3.3.1 and below
                renderer = new DefaultRenderer();
        }
    }

    while ( token->hasTokens() ){
        try{
            Token * tok;
            *token >> tok;
            if (Global::getVersion(major, minor, micro) != Global::getVersion()){
                // Do compatible translations if necessary
                handleCompatibility(tok, Global::getVersion(major, minor, micro));
            } 
            // Newer items
            else if (*tok == "val" || *tok == "value"){
                Token * val;
                *tok >> val;
                ValueHolder * value = new ValueHolder(val->getName());
                try {
                    while (true){
                        *value << val;
                    }
                } catch (const TokenException & ex){
                }
                addData(value);
            } else if (*tok == "context"){
                context.parseToken(tok);
            } else {
                Global::debug(3,"menu") <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch (const LoadException & ex){
            throw ex;
        } catch (const Filesystem::NotFound & ex){
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        }
    }
}

void Menu::Menu::run(const Context & parentContext){
    // TODO Keys need a home
    // Set keys
    input.set(Keyboard::Key_J, 0, true, Down);
    input.set(Keyboard::Key_K, 0, true, Up);
    input.set(Keyboard::Key_H, 0, true, Left);
    input.set(Keyboard::Key_L, 0, true, Right);
    input.set(Keyboard::Key_UP, 0, true, Up);
    /* regular keys */
    input.set(Keyboard::Key_DOWN, 0, true, Down);
    input.set(Keyboard::Key_LEFT, 0, true, Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Right);
    input.set(Keyboard::Key_ENTER, 0, true, Select);
    input.set(Keyboard::Key_ESC, 0, true, Cancel);
    /* joystick */
    input.set(Joystick::Up, 0, true, Up);
    input.set(Joystick::Down, 0, true, Down);
    input.set(Joystick::Left, 0, true, Left);
    input.set(Joystick::Right, 0, true, Right);
    /*! FIXME this should be changed to Select/Cancel buttons, all other buttons should be Select */
    input.set(Joystick::Button1, 0, true, Select);
    input.set(Joystick::Button2, 0, true, Cancel);
   
    /*
    Bitmap work(Global::getScreenWidth(), Global::getScreenHeight());
    work.updateOnResize();
    */
    Bitmap work(Menu::Width, Menu::Height);
    
    double runCounter = 0;
    Global::speed_counter = 0;

    InputManager::enableBufferInput();
     
    // Setup context from parent and this menu and initialize
    Context localContext(parentContext, context);
    localContext.initialize();

    // Setup menu fonts etc
    if (renderer){        
        renderer->initialize(localContext);
    }
    
    //Play music
    localContext.playMusic();
    
    // MenuException or something
    bool specialExit = false;
    
    // Run while till the localContext is done
    while( localContext.getState() != Context::Completed && (renderer && renderer->active()) ){
        bool draw = false;

        if ( Global::speed_counter > 0 ){
            draw = true;
            runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
            
            /* Added to make the psp update more frequently. */
            if (runCounter > 3){
                runCounter = 3;
            }
            while ( runCounter >= 1.0 ){
                runCounter -= 1;
                try {
                    act(localContext);
                } catch (const Exception::Return & ex){
                    // signaled to quit current menu, closing this one out
                    localContext.finish();
                    if (renderer){
                        renderer->finish();
                    }
                }
            }

            Global::speed_counter = 0;
        }

        if (draw){
            // Render
            render(localContext, work);
            
            // screen
            work.BlitToScreen();
        }

        while ( Global::speed_counter < 1 ){
            Util::rest( 1 );
            InputManager::poll();
        }
    }
    
    // FIXME Menu is finished, lets return is this even required anymore?
    throw Exception::Return(__FILE__, __LINE__);
}

void Menu::Menu::act(Context & ourContext){
    // Keys
    InputManager::poll();
    InputMap<Actions>::Output inputState = InputManager::getMap(input);
    if (inputState[Cancel]){
        if (renderer){
            InputManager::waitForRelease(input, Cancel);
            renderer->doAction(Cancel, ourContext);
        } else {
            ourContext.playSound(Cancel);
            InputManager::waitForRelease(input, Cancel);
            throw Exception::Return(__FILE__, __LINE__);
        }
    }
    if (renderer){
        if (inputState[Up]){
            renderer->doAction(Up, ourContext);
        }
        if (inputState[Down]){
            renderer->doAction(Down, ourContext);
        }
        if (inputState[Left]){
            renderer->doAction(Left, ourContext);
        }
        if (inputState[Right]){
            renderer->doAction(Right, ourContext);
        }
        if (inputState[Select]){
            renderer->doAction(Select, ourContext);
        }
        // Menu act
        renderer->act();
    }

    // Act context
    ourContext.act();
}

void Menu::Menu::render(Context & ourContext, const Bitmap & bmp){
    // Render context
    ourContext.render(renderer, bmp);
}

std::string Menu::Menu::getName(){
    std::string name;
    try {
        if (data[name]){
            *data[name] >> name;
        }
    } catch (const MenuException & ex){
    }
    return name;
}

void Menu::Menu::addData(ValueHolder * item){
    std::pair<std::map<std::string,ValueHolder *>::iterator,bool> check;
    check = data.insert( std::pair<std::string,ValueHolder *>(item->getName(),item) );
    if (check.second == false){
        Global::debug(0,"menu") << "Value \"" << check.first->second->getName() << "\" already exists - (" << check.first->second->getValues() << ")." << endl;        
        Global::debug(0,"menu") << "Replacing with value \"" << item->getName() << "\" -  (" << item->getValues() << ")." << endl;
        data[item->getName()] = item;
    }
}

void Menu::Menu::handleCompatibility(Token * tok, int version){
    Global::debug(1,"menu") << "Trying version: " << version << endl;
    if (version <= Global::getVersion(3, 3, 1)){
        if ( *tok == "name" ){
            ValueHolder * value = new ValueHolder("name");
            *value << tok;
            addData(value);
        } else if ( *tok == "music" ) {
            ValueHolder * value = new ValueHolder("music");
            *value << tok;
            addData(value);
            try {
                std::string music;
                *value >> music;
                context.setMusic(Filesystem::RelativePath(music));
            } catch (const MenuException & ex){
            }
        } else if( *tok == "select-sound" ) {
            ValueHolder * value = new ValueHolder("select-sound");
            *value << tok;
            addData(value);
            try{
                std::string sound;
                *value >> sound;
                context.addSound(Up,Filesystem::RelativePath(sound));
                context.addSound(Down,Filesystem::RelativePath(sound));
            } catch (const MenuException & ex){
            }
        } else if (*tok == "back-sound"){
            ValueHolder * value = new ValueHolder("back-sound");
            *value << tok;
            addData(value);
            try{
                std::string sound;
                *value >> sound;
                context.addSound(Back,Filesystem::RelativePath(sound));
                context.addSound(Cancel,Filesystem::RelativePath(sound));
            } catch (const MenuException & ex){
            }
        } else if (*tok == "ok-sound"){
            ValueHolder * value = new ValueHolder("ok-sound");
            *value << tok;
            addData(value);
            try{
                std::string sound;
                *value >> sound;
                context.addSound(Select,Filesystem::RelativePath(sound));
            } catch (const MenuException & ex){
            }
        } else if ( *tok == "background" ) {
            std::string temp;
            *tok >> temp;
            context.addBackground(temp);
        } else if (*tok == "anim"){
            context.addBackground(tok);
        } else if ( *tok == "clear-color" ) {
            // Not necessary ignore
        } else if ( renderer && renderer->readToken(tok) ) {
            // Nothing checks compatible version of renderer
        } else if ( *tok == "font" ) {
            ValueHolder * value = new ValueHolder("font");
            *value << tok << tok << tok;
            addData(value);
            try {
                std::string font;
                int w=24,h=24;
                *value >> font >> w >> h;
                context.setFont(Filesystem::RelativePath(font));
                context.setFontWidth(w);
                context.setFontHeight(h);
            } catch (const MenuException & ex){
            }
        } else if (*tok == "action"){
            // Set speed
            //ActionAct(tok);
        } else if (*tok == "info-position"){
            ValueHolder * value = new ValueHolder("info-position");
            *value << tok << tok;
            addData(value);
            try {
                double x=0, y=-.5;
                *value >> x >> y;
                context.setInfoLocation(x,y);
            } catch (const MenuException & ex){
            } 
        } else if (*tok == "menuinfo"){
            ValueHolder * value = new ValueHolder("menuinfo");
            *value << tok;
            addData(value);
            try {
                std::string info;
                *value >> info;
                context.setMenuInfoText(info);
            } catch (const MenuException & ex){
            } 
        } else if (*tok == "menuinfo-position"){
            ValueHolder * value = new ValueHolder("menuinfo-position");
            *value << tok << tok;
            addData(value);
            try {
                double x=0, y=.95;
                *value >> x >> y;
                context.setMenuInfoLocation(x,y);
            } catch (const MenuException & ex){
            } 
        } 
    }
    
    
    Global::debug(3,"menu") <<"Unhandled menu attribute: "<<endl;
    if (Global::getDebug() >= 3){
        tok->print(" ");
    }
}



/* Old Menu */
OldMenu::Menu::Menu(const Filesystem::AbsolutePath & str){
    load(str);
}

OldMenu::Menu::Menu(Token * token){
    load(token);
}

OldMenu::Menu::Menu(bool fade, int fadeColor):
music(""),
selectSound(""),
longestTextLength(0),
currentDrawState( NoFade ),
// work(new Bitmap(Global::getScreenWidth(), Global::getScreenHeight())),
// work(new Bitmap(640, 480)),
menuInfo(""),
parent(0),
_name(""),
hasOptions(false),
removeOption(false),
background(0),
clearColor(Bitmap::makeColor(0,0,0)),
option(false){

    // work->updateOnResize();

    if (fade){
        this->fade.fade = 255;
        this->fade.color = fadeColor;
    }

    // Make menus rounded (remove later
    contextMenu.location.setRadius(15);

	optionInfoTextLocation.x = 320;
	optionInfoTextLocation.y = 100;
	menuInfoLocation.x = 320;
	menuInfoLocation.y = 465;
        /*
    if (work.getError()){
        Global::debug(-1) << "*BUG* Could not allocate bitmap for menu" << endl;
    }
    */

    /* key, delay, block, output */
    /* vi keys */
    input.set(Keyboard::Key_J, 0, true, Down);
    input.set(Keyboard::Key_K, 0, true, Up);
    input.set(Keyboard::Key_H, 0, true, Left);
    input.set(Keyboard::Key_L, 0, true, Right);
    input.set(Keyboard::Key_UP, 0, true, Up);
    /* regular keys */
    input.set(Keyboard::Key_DOWN, 0, true, Down);
    input.set(Keyboard::Key_LEFT, 0, true, Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Right);
    input.set(Keyboard::Key_ENTER, 0, true, Select);
    input.set(Keyboard::Key_ESC, 0, true, Exit);
    /* joystick */
    input.set(Joystick::Up, 0, true, Up);
    input.set(Joystick::Down, 0, true, Down);
    input.set(Joystick::Left, 0, true, Left);
    input.set(Joystick::Right, 0, true, Right);
    input.set(Joystick::Button1, 0, true, Select);
    input.set(Joystick::Button2, 0, true, Exit);
}

void OldMenu::Menu::waitForSelect(){
    InputManager::waitForRelease(input, Select);
}

static map<string, Gui::Space::Space*> parseSpaces(const vector<Token*> & tokens){
    map<string, Gui::Space::Space*> spaces;
    Global::debug(2) << "Have " << tokens.size() << " spaces to parse" << endl;
    return spaces;
}

void OldMenu::Menu::load(Token *token){
    if ( *token != "menu" ){
        throw LoadException(__FILE__, __LINE__, "Not a menu");
    } else if (!token->hasTokens()){
        return;
    }

    sharedFontWidth = Configuration::getMenuFontWidth();
    sharedFontHeight = Configuration::getMenuFontHeight();
    
    coordinateSpaces = parseSpaces(token->findTokens("menu/space"));

    while ( token->hasTokens() ){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "name" ){
                // Set menu name
                *tok >> _name;
            } else if ( *tok == "music" ) {
                // Set music and push onto the stack
                *tok >> music;
            } else if( *tok == "select-sound" ) {
                *tok >> selectSound;
            } else if (*tok == "back-sound"){
                *tok >> backSound;
                try{
                    /* try to load it */
                    Resource::getSound(Filesystem::RelativePath(backSound));
                } catch (const LoadException & le){
                    Global::debug(0) << "Could not load sound " << backSound << " because " << le.getTrace() << endl;
                    /* we failed, so set the backSound to nothing */
                    backSound = "";
                }
            } else if (*tok == "ok-sound"){
                *tok >> okSound;
                try{
                    /* try to load it */
                    Resource::getSound(Filesystem::RelativePath(okSound));
                } catch (const LoadException & le){
                    Global::debug(0) << "Could not load sound " << okSound << " because " << le.getTrace() << endl;
                    /* we failed, so set the backSound to nothing */
                    okSound = "";
                }
            } else if ( *tok == "background" ) {
                std::string temp;
                *tok >> temp;
                if ( background ){
                    delete background;
                }
                Filesystem::AbsolutePath full = Filesystem::find(Filesystem::RelativePath(temp));
                background = new Bitmap(full.path());
                if ( background->getError() ){
                    Global::debug(0) << "Problem loading Bitmap: " << full.path() << endl;
                    delete background;
                    background = 0;
                }
            } else if ( *tok == "clear-color" ) {
                /* This is the clear color of the background if there is no background image specified within the menu
                 * Although if this is a submenu, and parent has a background that will be used instead...*/
                int r=0,g=0,b=0;
                *tok >> r >> g >> b;
                clearColor = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "position" ) {
                // This handles the placement of the menu list and surrounding box
                contextMenu.setCoordinates(tok);
            } else if ( *tok == "relative-position"){
                contextMenu.setCoordinates(tok);
            } else if ( *tok == "coordinate"){
                contextMenu.setCoordinates(tok);
            } else if ( *tok == "position-body" ) {
                // This handles the body color of the menu box
                contextMenu.setColors(tok);
            } else if ( *tok == "position-border" ) {
                // This handles the border color of the menu box
                contextMenu.setColors(tok);
            } else if ( *tok == "fade-speed" ) {
                // Menu fade in speed
                int speed;
                *tok >> speed;
                contextMenu.setFadeSpeed(speed);
            } else if ( *tok == "font" ) {
                string str;
                *tok >> str >> sharedFontWidth >> sharedFontHeight; 
                /* FIXME: make sharedFont a RelativePath */
                sharedFont = Filesystem::RelativePath(str).path();
            } else if( *tok == "option" ) {
                try{
                    MenuOption *temp = OptionFactory::getOption(tok);
                    if (temp){
                        addOption(temp);
                    }
                } catch (const LoadException & le){
                    Global::debug(0) << "Could not read option: " << le.getTrace() << endl;
                    tok->print(" ");
                }
            } else if (*tok == "action"){
                ActionAct(tok);
            } else if (*tok == "info-position"){
                *tok >> optionInfoTextLocation.x >> optionInfoTextLocation.y;
            } else if (*tok == "menuinfo"){
                *tok >> menuInfo;
            } else if (*tok == "menuinfo-position"){
                *tok >> menuInfoLocation.x >> menuInfoLocation.y;
            } else if (*tok == "anim"){
                Gui::Animation *animation = new Gui::Animation(tok);
                if (animation->getDepth() == Gui::Animation::BackgroundBottom){
                    backgroundAnimations.push_back(animation);
                } else if (animation->getDepth() == Gui::Animation::ForegroundBottom){
                    foregroundAnimations.push_back(animation);
                }
            } else {
                Global::debug(3) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch (const LoadException & ex){
            throw ex;
        } catch (const Filesystem::NotFound & ex){
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        }
    }

    if ( _name.empty() ){
        throw LoadException(__FILE__, __LINE__, "No name set, the menu should have a name!");
    }

    if ( contextMenu.location.empty() ){
        throw LoadException(__FILE__, __LINE__, "The position for the menu '" + getName() + "' list must be set!");
    }
    // Omit menu if no options are available
    if (! hasOptions) {
        Global::debug(1) << "The menu \"" << getName() << "\" has no options & will be omitted from the top level menu!" << endl;
        removeOption = true;
    }

    // Set the sharedFont
    if (Configuration::getMenuFont() != "" && Util::exists(Configuration::getMenuFont())){
        sharedFont = Configuration::getMenuFont();
    }

    // Finally lets assign list order numbering and some other stuff
    // First length
    if (hasOptions){
        setupOptions();
    }   
}

void OldMenu::Menu::setupOptions(){
    longestTextLength = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(menuOptions[0]->getText().c_str());
    // Before we finish lets get rid of the cruft
    for (std::vector< MenuOption *>::iterator optBegin = menuOptions.begin() ; optBegin != menuOptions.end(); /**/){
        if( (*optBegin)->scheduledForRemoval() ){
            Global::debug(0) << "Removed option: " << (*optBegin)->getText() << endl;
            delete (*optBegin);
            optBegin = menuOptions.erase(optBegin);
        }
        else optBegin++;
    }

    // Figure out text length
    for( unsigned int i = 0; i < menuOptions.size(); i++ ){
        checkTextLength(menuOptions[i]);
    }

    // Set initial location
    contextMenu.setList(toContextList(menuOptions));
    selectedOption = menuOptions[contextMenu.getCurrentIndex()];
    selectedOption->setState(MenuOption::Selected);
    addInfoBox(selectedOption->getInfoText());
    
    // Set context menu stuff font and fade in
    contextMenu.setFont(getFont(), getFontWidth(), getFontHeight());
    contextMenu.open();
}

std::vector<Gui::ContextItem *> OldMenu::Menu::getContextList(){
    return toContextList(menuOptions);
}

void OldMenu::Menu::load(const Filesystem::AbsolutePath & filename){
    // Must check for initial token, menu
    try{
        Global::debug(1) << "Loading menu " << filename.path() << endl;
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Menu parse error");
    }
}

/*! Logic */
void OldMenu::Menu::act(bool &endGame, bool reset){

    InputMap<MenuInput>::Output inputState = InputManager::getMap(input);

    if (inputState[Up]){
        selectedOption->setState(MenuOption::Deselected);
        bool moved = contextMenu.previous();
        selectedOption = menuOptions[contextMenu.getCurrentIndex()];
        selectedOption->setState(MenuOption::Selected);
        selectedOption->resetAnimations();

        if (moved && menuOptions.size() > 1){
            MenuGlobals::playSelectSound();
            addInfoBox(selectedOption->getInfoText());
        }
    }

    if (inputState[Down]){
        selectedOption->setState(MenuOption::Deselected);
        bool moved = contextMenu.next();
        selectedOption = menuOptions[contextMenu.getCurrentIndex()];
        selectedOption->setState(MenuOption::Selected);
        selectedOption->resetAnimations();

        if (moved && menuOptions.size() > 1){
            MenuGlobals::playSelectSound();
            addInfoBox(selectedOption->getInfoText());
        }
    }

    if (inputState[Left]){
        if (selectedOption->leftKey()){
            /* ??? */
        }
    }

    if (inputState[Right]){
        if (selectedOption->rightKey()){
            /* ??? */
        }
    }

    if (inputState[Exit]){
        selectedOption->setState(MenuOption::Deselected);
        InputManager::waitForRelease(input, Exit);
        
        throw Exception::Return(__FILE__, __LINE__);
    }

    if (inputState[Select]){
        if (selectedOption->isRunnable()){
            selectedOption->setState(MenuOption::Run);
            tryPlaySound(Filesystem::RelativePath(okSound));
            contextMenu.close();
        }
    }

    for ( std::vector <MenuOption *>::iterator b = menuOptions.begin() ; b != menuOptions.end(); b++ ){
        (*b)->logic();
    }

    // Current option animation logic
    selectedOption->updateAnimations();

    // Animations
    for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
        (*i)->act();
    }
    for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
        (*i)->act();
    }
    
    /* FIXME: this is a hack to forcifully update the contextMenu's font
     * in case the font option changes the menu font.
     */
    contextMenu.setFont(getFont(), getFontWidth(), getFontHeight());
    
    // Menu
    contextMenu.act();

    // Menu info
    menuInfoBox.act();

    // Info boxes
    actInfoBoxes();
}

bool OldMenu::Menu::isFading(){
    return fade.fade > 0;
}

void OldMenu::Menu::doFade(Bitmap * work){
    Bitmap::transBlender(0, 0, 0, fade.fade);
    work->translucent().fill(fade.color);
    // fade.fade -= 8;
    fade.fade *= 0.8;
    if (fade.fade < 4){
        fade.fade = 0;
    }
}

void OldMenu::Menu::run(){
    bool done = false;
    bool endGame = false;

    Bitmap work(OldMenu::Menu::Width, OldMenu::Menu::Height);
    // setWork(&work);

    if ( menuOptions.empty() ){
        return;
    }

    if ( !selectSound.empty() ){
        MenuGlobals::setSelectSound(selectSound);
    }

    double runCounter = 0;
    while( ! endGame ){
        Global::speed_counter = 0;
        
        // Reset animations
        for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
            (*i)->reset();
        }
        for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
            (*i)->reset();
        }

        // Set music
        if ( !music.empty() ){
            MenuGlobals::setMusic(music);
        }

        // Set font and fade in
        contextMenu.setFont(getFont(), getFontWidth(), getFontHeight());
        contextMenu.open();

        // Setup menu info
        setMenuInfo(menuInfo);
        
        done = false;
        
        bool requestsReturn = false;

        while ( !done && contextMenu.isActive() ){

            bool draw = false;

            if (Global::shutdown()){
                throw ShutdownException();
            }

            if ( Global::speed_counter > 0 ){
                draw = true;
                runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
                /* Added to make the psp update more frequently. */
                if (runCounter > 3){
                    runCounter = 3;
                }
                while ( runCounter >= 1.0 ){
                    runCounter -= 1;
                    InputManager::poll();
                    try{
                        act(endGame);
                    } catch (const Exception::Return &ex){
                        done = requestsReturn = true;
                    }
                    if (selectedOption->getState() == MenuOption::Run){
                        done = true;
                    }
                }

                Global::speed_counter = 0;
            }

            if ( draw && selectedOption->getState() != MenuOption::Run ){
                // Draw

                // Do the background
                drawBackground(&work);
                // Do background animations
                for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the background of the menu of selected object 
                selectedOption->drawBelow(&work);
                // Draw menu
                contextMenu.render(work);
                // Draw option info text
                renderInfoBoxes(work);
                // Draw menu info text
                menuInfoBox.render(work);
                // Draw foreground animations
                for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the foreground of the menu of selected object 
                selectedOption->drawAbove(&work);

                if (isFading()){
                    doFade(&work);
                }

                // Finally render to screen
                work.BlitToScreen();
            }

            while ( Global::speed_counter < 1 ){
                Util::rest( 1 );
                InputManager::poll();
            }

            done |= endGame;
        }
        // Check if requesting return
        if (requestsReturn){
            throw Exception::Return(__FILE__, __LINE__);
        }
        // lets run it
        runOption(endGame);
        // Reset music
        if ( !music.empty() ){
            MenuGlobals::setMusic(music);
        }
        if ( !selectSound.empty() ){
            MenuGlobals::setSelectSound(selectSound);
        }

        if (!selectSound.empty()){
            if(MenuGlobals::currentSelectSound() != selectSound){
                MenuGlobals::popSelectSound();
            }
        }
    }

    // setWork(NULL);
}

void OldMenu::Menu::runOption(bool &endGame){
    if (selectedOption->getState() == MenuOption::Run){
        // lets run it
        try{
            selectedOption->run(::Menu::Context());
        } catch (const Exception::Return & re){
            tryPlaySound(Filesystem::RelativePath(backSound));
        }
        selectedOption->setState(MenuOption::Selected);
        selectedOption->resetAnimations();
        
        // Set font and fade in
        contextMenu.setFont(getFont(), getFontWidth(), getFontHeight());
        contextMenu.open();
    }
}

void OldMenu::Menu::runOption(unsigned int index){
    if (index >= menuOptions.size()){
        return;
    }
    try{
        bool endGame = false;
        menuOptions[index]->run(::Menu::Context());
    } catch (const Exception::Return & re){
        tryPlaySound(Filesystem::RelativePath(backSound));
    }
}

MenuOption * OldMenu::Menu::getOption(unsigned int index){
    if (index >= menuOptions.size()){
        return 0;
    }
    return menuOptions[index];
}

/*! set parent */
void OldMenu::Menu::setParent(Menu *menu){
    this->parent = menu;
}

/*! get background */
Bitmap *OldMenu::Menu::getBackground(){
    if (!background){
	Menu *p = getParent();
	if (p){
	    return p->getBackground();
	} else {
	    return 0;
	}
    }
    return background;
}

/*! Add options to menu */
void OldMenu::Menu::addOption(MenuOption *opt){
  if (opt){
      opt->setParent(this);
      hasOptions = true;
      menuOptions.push_back(opt);
  }
}

/*! Get working bitmap */
/*
Bitmap *Menu::getWork(){
    return currentWork;
}

void Menu::setWork(Bitmap * work){
    currentWork = work;
}
*/

const Filesystem::RelativePath OldMenu::Menu::getFont(){
    return Filesystem::RelativePath(sharedFont);
}

//! get font width
int OldMenu::Menu::getFontWidth(){
  return sharedFontWidth;
}

//! get font height
int OldMenu::Menu::getFontHeight(){
  return sharedFontHeight;
}
                
void OldMenu::Menu::setFontName(const std::string & str){
    Configuration::setMenuFont(str);
    sharedFont = str;
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

void OldMenu::Menu::setFontWidth(int w){
    if (w < 1){
        w = 1;
    }

    sharedFontWidth = w;
    Configuration::setMenuFontWidth(w);
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

void OldMenu::Menu::setFontHeight(int h){
    if (h < 1){
        h = 1;
    }

    sharedFontHeight = h;
    Configuration::setMenuFontHeight(h);
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

//! set new font menu wide
void OldMenu::Menu::setFont(const std::string &font, int w, int h){
    if (Util::exists(font) == true){
        sharedFont = font;
        sharedFontWidth = w;
        sharedFontHeight = h;
    }
}

//! Set longest length
void OldMenu::Menu::checkTextLength(MenuOption *opt){
	// Set longest text length depending on type
	switch(opt->getType()){
		case MenuOption::AdjustableOption : {
			int len = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(opt->getText().c_str()) + 10;
			if(len > longestTextLength)longestTextLength = len;
			break;
		}
		case MenuOption::Option:
		default : {
			int len = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(opt->getText().c_str());
			if ( len > longestTextLength ){
				longestTextLength = len;
			}
			break;
		}
	}
}

void OldMenu::Menu::drawBackground(Bitmap *bmp){
    Bitmap *temp = getBackground();
    if ( !temp ){
        bmp->fill(clearColor);
    } else {
	temp->Stretch(*bmp);
    }
}

void OldMenu::Menu::setMenuInfo(const std::string & text){
    // Setup menu info
    menuInfoBox.setFont(getFont(), getFontWidth(), getFontHeight());
    menuInfoBox.setText(text);
    const int width = menuInfoBox.location.getWidth();
    const int height = menuInfoBox.location.getHeight();
    menuInfoBox.location.setPosition(Gui::AbsolutePoint(menuInfoLocation.x - width/2, menuInfoLocation.y - height/2));
    // have to pass the dimensions back in to correct proper placement
    menuInfoBox.location.setPosition2(Gui::AbsolutePoint(menuInfoBox.location.getX() + width, menuInfoBox.location.getY() + height));
    menuInfoBox.location.setRadius(contextMenu.location.getRadius());
    menuInfoBox.colors.body = Bitmap::makeColor(32,32,0);
    menuInfoBox.colors.bodyAlpha = contextMenu.colors.bodyAlpha;
    menuInfoBox.colors.border = contextMenu.colors.border;
    menuInfoBox.colors.borderAlpha = contextMenu.colors.borderAlpha;
    menuInfoBox.open();
}

void OldMenu::Menu::addInfoBox(const std::string & text){
    if (text.empty()){
        return;
    }
    if (!optionInfoBoxes.empty()){
        optionInfoBoxes.back()->close();
    }
    ::Menu::InfoBox * temp = new ::Menu::InfoBox();
    temp->setFont(getFont(),getFontWidth(),getFontHeight());
    temp->setText(text);
    const int width = temp->location.getWidth();
    const int height = temp->location.getHeight();
    temp->location.setPosition(Gui::AbsolutePoint(optionInfoTextLocation.x - width/2, optionInfoTextLocation.y - height/2));
    // have to pass the dimensions back in to correct proper placement
    temp->location.setPosition2(Gui::AbsolutePoint(temp->location.getX() + width,temp->location.getY() + height));
    temp->location.setRadius(contextMenu.location.getRadius());
    temp->colors.body = Bitmap::makeColor(32,32,0);
    temp->colors.bodyAlpha = contextMenu.colors.bodyAlpha;
    temp->colors.border = contextMenu.colors.border;
    temp->colors.borderAlpha = contextMenu.colors.borderAlpha;
    temp->open();
    optionInfoBoxes.push_back(temp);
}

//! Update info boxes
void OldMenu::Menu::actInfoBoxes(){
    // Global::debug(0) << "Info boxes " << optionInfoBoxes.size() << endl;
    for (std::vector< ::Menu::InfoBox *>::iterator i = optionInfoBoxes.begin(); i != optionInfoBoxes.end();){
        ::Menu::InfoBox *box = *i;
        box->act();
        if (!box->isActive()){
            delete box;
            i = optionInfoBoxes.erase(i);
        } else {
            i++;
        }
    }
}

//! Render info boxes
void OldMenu::Menu::renderInfoBoxes(const Bitmap & work){
    for (std::vector< ::Menu::InfoBox *>::iterator i = optionInfoBoxes.begin(); i != optionInfoBoxes.end(); ++i){
        ::Menu::InfoBox *box = *i;
        box->render(work);
    }
}

void OldMenu::Menu::closeInfoBoxes(){
    for (std::vector< ::Menu::InfoBox *>::iterator i = optionInfoBoxes.begin(); i != optionInfoBoxes.end(); ++i){
        ::Menu::InfoBox *box = *i;
        box->close();
    }
}


OldMenu::Menu::~Menu(){
    // cleanup
    /*
    if (work){
        delete work;
    }
    */

    for (std::vector< ::Menu::InfoBox *>::iterator i = optionInfoBoxes.begin(); i != optionInfoBoxes.end(); ++i){
        ::Menu::InfoBox *box = *i;
        if (box){
            delete box;
        }
    }


    vector <MenuOption *>::iterator b = menuOptions.begin();
    vector <MenuOption *>::iterator e = menuOptions.end();
    for(;b!=e;++b){
        if((*b))delete (*b);
    }

    if( background )delete background;

    for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
        if (*i){
            delete *i;
        }
    }
    for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
        if (*i){
            delete *i;
        }
    }
}
