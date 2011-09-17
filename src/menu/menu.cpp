#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "menu.h"
#include "menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/events.h"
#include "util/tokenreader.h"
#include "util/file-system.h"
#include "util/resource.h"
#include "globals.h"
#include "util/debug.h"
#include "util/init.h"
#include "configuration.h"
#include "util/music.h"
#include "util/gradient.h"
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"

#include "optionfactory.h"
#include "actionfactory.h"

#include "util/input/input-manager.h"
#include "util/input/input-map.h"
#include "util/input/input-source.h"
#include "util/parameter.h"

#include <queue>
#include <map>
#include <ostream>
#include <sstream>

#include "util/gui/context-box.h"

using namespace std;
using namespace Gui;

/* the current font is a property of the dynamic execution. so it will
 * be modified by various functions that call Parameter::push
 */
static const Font & currentFont(){
    return Menu::menuFontParameter.current()->get();
}

Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > Menu::menuFontParameter;

static std::string sharedFont = "fonts/arial.ttf";
static int sharedFontWidth = 24;
static int sharedFontHeight = 24;

Effects::Gradient Menu::standardGradient(){
    return Gui::standardGradient();
}

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
fadeAlpha(0){
    popup.setFadeSpeed(20);
}

Menu::InfoBox::~InfoBox(){
}
 
void Menu::InfoBox::act(const Font & font){
    popup.act(font);
    
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
        
void Menu::InfoBox::render(const Graphics::Bitmap &){
    throw MenuException(__FILE__, __LINE__, "Don't call the render(Bitmap) function");
}

void Menu::InfoBox::render(const Graphics::Bitmap & bmp, const Font & vFont){
    popup.render(bmp);

    // const Font & vFont = Configuration::getMenuFont()->get(*font);
    
    const int x1 = popup.getArea().getX()+(int)(popup.getTransforms().getRadius()/2);
    const int y1 = popup.getArea().getY()+2;
    const int x2 = popup.getArea().getX2()-(int)(popup.getTransforms().getRadius()/2);
    const int y2 = popup.getArea().getY2()-2;
    bmp.setClipRect(x1, y1, x2, y2);
    
    // FIXME height is proportionally wrong in a majority of the cases, this is perhaps due to ftalleg.
    int sy = location.getY() - vFont.getHeight()/6;// - location.getHeight()/2 - vFont.getHeight()/2;
    static Graphics::Color white = Graphics::makeColor(255,255,255);
    unsigned int padding_index = 0;
    for (vector<string>::iterator it = text.begin(); it != text.end(); it++){
        string & str = *it;
        if (fadeAlpha < 255){
            Graphics::Bitmap::transBlender(0, 0, 0, fadeAlpha);
            vFont.printf(location.getX() + padding[padding_index]/2, sy, white, bmp.translucent(), str, 0 );
        } else {
            vFont.printf(location.getX() + padding[padding_index]/2, sy, white, bmp, str, 0 );
        }
        sy += vFont.getHeight();
        padding_index++;
    }
    bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
}

void Menu::InfoBox::open(){
    state = Opening;
    popup.location = location;
    popup.transforms = transforms;
    popup.colors = colors;
    popup.open();
    fadeAlpha = 0;
}

void Menu::InfoBox::close(){
    state = Closing;
    popup.close();
}

/* dimensions are computed lazily when we get a font, but only compute once per font */
void Menu::InfoBox::initialize(const Font & font){
    int maxWidth = 0;
    int height = 0;
    for (vector<string>::iterator it = text.begin(); it != text.end(); it++){
        // Add the padding
        ostringstream pad;
        pad << (*it)[0] << (*it)[(*it).size()-1];
        padding.push_back(font.textLength(pad.str().c_str()));
        int w = font.textLength((*it).c_str()) + padding.back();
        if (w > maxWidth){
            maxWidth = w;
        }
        height += font.getHeight();
    }

    location.setDimensions(maxWidth, height);
}

void Menu::InfoBox::setText(const std::string & info){
    if (info.empty()){
        return;
    }
    text.clear();
    // const Font & vFont = Configuration::getMenuFont()->get(*font);
    size_t start = 0;
    size_t last = 0;
    start = info.find("\n");
    while (start != string::npos){
        text.push_back(info.substr(last, start - last));
        last = start + 1;
        start = info.find("\n", last);
    }
    text.push_back(info.substr(last));

    /*
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
    */
}

static std::vector<Util::ReferenceCount<ContextItem> > toContextList(const std::vector<Util::ReferenceCount<MenuOption> > & list){
    std::vector<Util::ReferenceCount<ContextItem> > contextItems;
    for (std::vector<Util::ReferenceCount<MenuOption> >::const_iterator i = list.begin(); i != list.end(); ++i){
        const Util::ReferenceCount<MenuOption> & option = *i;
        contextItems.push_back(option.convert<ContextItem>());
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

Menu::Reload::Reload(const std::string & file, int line, const std::string reason):
MenuException(file, line, reason){
}

Menu::Reload::~Reload() throw() {
}
    
Exception::Base * Menu::Reload::copy() const {
    return new Reload(*this);
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

Menu::ValueHolder & Menu::ValueHolder::operator<<(TokenView & view){
    std::string temp;
    view >> temp;
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

void Menu::Background::render(const Gui::Animation::Depth & depth, const Graphics::Bitmap & bmp){
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
    // Kill info boxes
    for (std::vector< ::Menu::InfoBox *>::iterator i = info.begin(); i != info.end();++i){
        if (*i){
            delete *i;
        }
    }
}

/*
void Menu::Renderer::setFont(const Util::ReferenceCount<FontInfo> & font){
}
*/

void Menu::Renderer::addInfo(const std::string & text, const Gui::Widget & defaults, Context & context, const Font & font){
    if (text.empty()){
        return;
    }
    if (!info.empty()){
        info.back()->close();
    }
    ::Menu::InfoBox * temp = new ::Menu::InfoBox();
    // temp->setFont(context.getFont());
    temp->setText(text);
    temp->initialize(font);
    const int width = temp->location.getWidth();
    const int height = temp->location.getHeight();
    temp->location.setPosition(Gui::AbsolutePoint(context.getInfoLocation().getX() - width/2, context.getInfoLocation().getY() - height/2));
    // have to pass the dimensions back in to correct proper placement
    temp->location.setPosition2(Gui::AbsolutePoint(temp->location.getX() + width,temp->location.getY() + height));
    temp->transforms.setRadius(defaults.transforms.getRadius());
    temp->colors = defaults.colors;
    temp->open();
    info.push_back(temp);
}

void Menu::Renderer::actInfo(const Font & font){
    for (std::vector< ::Menu::InfoBox *>::iterator i = info.begin(); i != info.end();){
        ::Menu::InfoBox *box = *i;
        box->act(font);
        if (!box->isActive()){
            delete box;
            i = info.erase(i);
        } else {
            i++;
        }
    }
}

void Menu::Renderer::renderInfo(const Graphics::Bitmap & work, const Font & font){
    for (std::vector< ::Menu::InfoBox *>::iterator i = info.begin(); i != info.end(); ++i){
        ::Menu::InfoBox *box = *i;
        box->render(work, font);
    }
}

Menu::DefaultRenderer::DefaultRenderer():
hasOverride(false),
overrideIndex(0){
    // Default the menu to a certain size and details
    menu.transforms.setRadius(15);
    menu.location.set(-.6, -.3, .6, .8);
    menu.colors.body = Graphics::makeColor(0,0,0);
    menu.colors.bodyAlpha = 128;
    menu.colors.border = Graphics::makeColor(200,200,200);
    menu.colors.borderAlpha = 255;
}
        
vector<Util::ReferenceCount<MenuOption> > Menu::DefaultRenderer::getOptions() const {
    return options;
}

void Menu::DefaultRenderer::invokeOverride(const Context & context){
    if (hasOverride){
        options[overrideIndex]->run(context);
        throw Exception::Return(__FILE__, __LINE__);
    }
}

Menu::DefaultRenderer::~DefaultRenderer(){
    // Kill options
    /*
    for (std::vector<MenuOption *>::iterator i = options.begin(); i != options.end(); ++i){
        if (*i){
            delete *i;
        }
    }
    */
}
bool Menu::DefaultRenderer::readToken(const Token * token, const OptionFactory & factory){
    if( *token == "option" ) {
        try{
            MenuOption * temp = factory.getOption(menu, token);
            if (temp){
                options.push_back(temp);
                if (!hasOverride){
                    const Token * tok;
                    token->view() >> tok;
                    if (tok->findToken("_/override") != NULL){
                        overrideIndex = options.size()-1;
                        hasOverride = true;
                    }
                }
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
        token->view() >> speed;
        menu.setFadeSpeed(speed);
    } else {
        return false;
    }
    
    return true;
}

/*
void Menu::DefaultRenderer::setFont(const Util::ReferenceCount<FontInfo> & font){
    if (!font->empty()){
        menu.setFont(Configuration::getMenuFont()->getFont(*font),
                     Configuration::getMenuFont()->getWidth(*font),
                     Configuration::getMenuFont()->getHeight(*font));
        menuInfo.setFont(font);
    }
}
*/

void Menu::DefaultRenderer::initialize(Context & context){
    // FIXME This is wrong, move this over to FontInfo so that the overrides can be accounted for
    /*
    Filesystem::RelativePath localFont("fonts/arial.ttf");
    int width = 24, height = 24;
    */

    /*
    if (Configuration::getMenuFont() != NULL){
        localFont = Configuration::getMenuFont()->getFont();
        width = Configuration::getMenuFontWidth();
        height = Configuration::getMenuFontHeight();
    } else if (Filesystem::exists(context.getFont()->getFont())){
        localFont = context.getFont()->getFont();
        width = context.getFont()->getWidth();
        height = context.getFont()->getHeight();
    }
    */

    // setFont(new FontInfo(localFont, width, height));
    menu.setList(toContextList(options));
    menu.open();
    
    // const Font & font = Configuration::getMenuFont()->get(context.getFont()->get());
    const Font & font = currentFont();
    // Menu info
    if (!context.getMenuInfoText().empty()){
        menuInfo.setText(context.getMenuInfoText());
        menuInfo.initialize(font);
        const int width = menuInfo.location.getWidth();
        const int height = menuInfo.location.getHeight();
        menuInfo.location.setPosition(Gui::AbsolutePoint(context.getMenuInfoLocation().getX() - width/2, context.getMenuInfoLocation().getY() - height/2));
        // have to pass the dimensions back in to correct proper placement
        menuInfo.location.setPosition2(Gui::AbsolutePoint(menuInfo.location.getX() + width, menuInfo.location.getY() + height));
        menuInfo.transforms.setRadius(menu.transforms.getRadius());
        menuInfo.colors = menu.colors;
    }
    
    menuInfo.open();
    
    // Add first info option
    addInfo(options[menu.getCurrentIndex()]->getInfoText(), menu, context, font); 
}

void Menu::DefaultRenderer::finish(){
    menu.close();
    menuInfo.close();
}

bool Menu::DefaultRenderer::active(){
    return menu.isActive();
}

void Menu::DefaultRenderer::act(const Context & context){
    // const Font & font = Configuration::getMenuFont()->get(context.getFont()->get());
    const Font & font = currentFont();

    // FIXME find a better way to get options to update this is a waste
    for (std::vector<Util::ReferenceCount<MenuOption> >::iterator i = options.begin(); i != options.end(); ++i){
        Util::ReferenceCount<MenuOption> & option = *i;
        option->logic();
    }
    menu.act(font);
    menuInfo.act(font);
    actInfo(font);
}

void Menu::DefaultRenderer::render(const Graphics::Bitmap & bmp, const Font & font){
    menu.render(bmp, font);
    menuInfo.render(bmp, font);
    renderInfo(bmp, font);
    
    /*
    // Check for adjustability
    const Util::ReferenceCount<MenuOption> option = options[menu.getCurrentIndex()];
    
    // TODO FIXME ContextItem always returns false this overall needs to be changed to a better solution
    if (option->isAdjustable()){
        // NOTE consider adding getCenter X & Y to coordinate
        const int x = menu.location.getX()+(menu.location.getWidth()/2);
        const int y = menu.location.getY()+(menu.location.getHeight()/2);
        const int triangleSize = 14;
        // Get current option width
        const int startx = x - (font.textLength(option->getName().c_str())/2);
        int cx = startx - 15;

        // NOTE 1.3 is hardcoded from FONT_SPACER in scroll-list
        int cy = (int)(y + (font.getHeight()/1.3) / 2 + 2);

        bmp.equilateralTriangle(cx, cy, 180, triangleSize, option->getLeftColor());

        cx = (x + startx + font.textLength(option->getName().c_str()))+15;
        bmp.equilateralTriangle(cx, cy, 0, triangleSize, option->getRightColor());
    }
    */
}

void Menu::DefaultRenderer::addOption(MenuOption * opt){
    this->options.push_back(opt);
}

void Menu::DefaultRenderer::doAction(const Actions & action, Context & context){
    const Font & font = currentFont();
    switch(action){
        case Up:
            if (menu.previous(font)){
                context.playSound(Up);
                addInfo(options[menu.getCurrentIndex()]->getInfoText(), menu, context, font); 
            }
            break;
        case Down:
            if (menu.next(font)){
                context.playSound(Down);
                addInfo(options[menu.getCurrentIndex()]->getInfoText(), menu, context, font); 
            }
            break;
        case Left:
            if (options[menu.getCurrentIndex()]->leftKey()){
                // setFont(context.getFont());
                context.playSound(Left);
            }
            break;
        case Right:
            if (options[menu.getCurrentIndex()]->rightKey()){
                // setFont(context.getFont());
                context.playSound(Right);
            }
            break;
        case Select: {
            try{
                context.playSound(Select);
                options[menu.getCurrentIndex()]->run(context);
            } catch (const Reload & reload){
                menu.open();
                menuInfo.open();
            }
            // setFont(context.getFont());
            context.playMusic();
            /* font might have been recreated */
            const Font & font = currentFont();
            addInfo(options[menu.getCurrentIndex()]->getInfoText(), menu, context, font); 
            menuInfo.initialize(font);
            break;
        }
        case Cancel:
            context.playSound(Cancel);
            throw Exception::Return(__FILE__, __LINE__);
            break;
        default:
            break;
    }
}

Menu::TabInfo::TabInfo(){
}

Menu::TabInfo::~TabInfo(){
    // Kill options
    /*
    for (std::vector<MenuOption *>::iterator i = options.begin(); i != options.end(); ++i){
        if (*i){
            delete *i;
        }
    }
    */
}

void Menu::TabInfo::act(){
    // Miguel: FIXME find a better way to get options to update this is a waste
    // Jon: Whats wrong with it?
    for (std::vector<Util::ReferenceCount<MenuOption> >::iterator i = options.begin(); i != options.end(); ++i){
        Util::ReferenceCount<MenuOption> & option = *i;
        option->logic();
    }
}

Menu::TabRenderer::TabRenderer():
hasOverride(false),
overrideIndex(0){
    
    // Default the menu to a certain size and details
    //menu.transforms.setRadius(15);
    menu.location.set(-.6, -.3, .6, .8);
    menu.colors.body = Graphics::makeColor(0,0,0);
    menu.colors.bodyAlpha = 128;
    menu.colors.border = Graphics::makeColor(200,200,200);
    menu.colors.borderAlpha = 255;
}
        
vector<Util::ReferenceCount<MenuOption> > Menu::TabRenderer::getOptions() const {
    vector<Util::ReferenceCount<MenuOption> > options;
    for (vector<TabInfo *>::const_iterator it = tabs.begin(); it != tabs.end(); ++it){
        options.insert(options.end(), (*it)->options.begin(), (*it)->options.end());
    }
    return options;
}

/* FIXME need to implement override for tabs */
void Menu::TabRenderer::invokeOverride(const Context & context){
    if (hasOverride){
        //options[overrideIndex]->run(context);
        throw Exception::Return(__FILE__, __LINE__);
    }
}

Menu::TabRenderer::~TabRenderer(){
    // Kill tabs
    for (std::vector<TabInfo *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        if (*i){
            delete *i;
        }
    }
}

bool Menu::TabRenderer::readToken(const Token * token, const OptionFactory & factory){
    if (*token == "menu"){
        TabInfo * tabInfo = new TabInfo();
        Gui::Tab * tab = new Gui::Tab();
        TokenView view = token->view();
        while (view.hasMore()){
            const Token * tok;
            view >> tok;
            try{
                if (*tok == "name"){
                    tok->view() >> tabInfo->name;
                    tok->view() >> tab->name;
                } else if (*tok == "info"){
                    tok->view() >> tabInfo->info;
                } else if (*tok == "menuinfo"){
                    tok->view() >> tabInfo->menuInfo;
                } else if (*tok == "option"){
                    try {
                        MenuOption *temp = factory.getOption(tab->getContext(), tok);
                        if (temp){
                            Util::ReferenceCount<MenuOption> ref(temp);
                            tabInfo->options.push_back(ref);
                            tab->addOption(ref.convert<Gui::ContextItem>());
                        }
                    } catch (const LoadException & le){
                        tok->print(" ");
                    } 
                }
            } catch (const TokenException & ex){
                // Output something
            }
        }
        menu.addTab(tab);
        tabs.push_back(tabInfo);
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
    } else if ( *token == "tab-body" ) {
        int r,g,b;
        token->view() >> r >> g >> b >> menu.tabColors.bodyAlpha;
        menu.tabColors.body = Graphics::makeColor(r,g,b);
    } else if ( *token == "tab-border" ) {
        int r,g,b;
        token->view() >> r >> g >> b >> menu.tabColors.borderAlpha;
        menu.tabColors.border = Graphics::makeColor(r,g,b);
    } else if ( *token == "selectedtab-body" ) {
        int r,g,b;
        token->view() >> r >> g >> b >> menu.selectedTabColors.bodyAlpha;
        menu.selectedTabColors.body = Graphics::makeColor(r,g,b);
    } else if ( *token == "selectedtab-border" ) {
        int r,g,b;
        token->view() >> r >> g >> b >> menu.selectedTabColors.borderAlpha;
        menu.selectedTabColors.border = Graphics::makeColor(r,g,b);
    } else if ( *token == "runningtab-body" ) {
        int r,g,b;
        token->view() >> r >> g >> b >> menu.runningTabColors.bodyAlpha;
        menu.runningTabColors.body = Graphics::makeColor(r,g,b);
    } else if ( *token == "runningtab-border" ) {
        int r,g,b;
        token->view() >> r >> g >> b >> menu.runningTabColors.borderAlpha;
        menu.runningTabColors.border = Graphics::makeColor(r,g,b);
    } else if ( *token == "font-color" ) {
        int r,g,b;
        token->view() >> r >> g >> b;
        menu.setTabFontColor(Graphics::makeColor(r,g,b));
    } else if ( *token == "selectedfont-color" ) {
        int r,g,b;
        token->view() >> r >> g >> b;
        menu.setSelectedTabFontColor(Graphics::makeColor(r,g,b));
    } else if ( *token == "runningfont-color" ) {

    } else if ( *token == "fade-speed" ) {
        // Menu fade in speed
        int speed;
        token->view() >> speed;
        //menu.setFadeSpeed(speed);
    } else {
        return false;
    }

    return true;
}

/*
void Menu::TabRenderer::setFont(const Util::ReferenceCount<FontInfo> & font){
    if (!font->empty()){
        menu.setFont(Configuration::getMenuFont()->getFont(*font),
                     Configuration::getMenuFont()->getWidth(*font),
                     Configuration::getMenuFont()->getHeight(*font));
        menuInfo.setFont(font);
    }
}
*/

void Menu::TabRenderer::initialize(Context & context){
    // FIXME Redundant, see defaultRenderer
    /*
    Filesystem::RelativePath localFont("fonts/arial.ttf");
    int width = 24, height = 24;
    */
    /*
    if (Configuration::getMenuFont() != NULL){
        localFont = Configuration::getMenuFont()->getFont();
        width = Configuration::getMenuFontWidth();
        height = Configuration::getMenuFontHeight();
    } else if (Filesystem::exists(context.getFont()->getFont())){
        localFont = context.getFont()->getFont();
        width = context.getFont()->getWidth();
        height = context.getFont()->getHeight();
    }
    */
    // setFont(new FontInfo(localFont, width, height));
    /*
    for (std::vector<TabInfo *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        TabInfo * tab = *i;
        menu.addTab(tab->name, toContextList(tab->options));
    }
    */
    //menu.open();
    
    const Font & font = currentFont();
    // Menu info
    if (!context.getMenuInfoText().empty()){
        menuInfo.setText(context.getMenuInfoText());
        menuInfo.initialize(font);
        const int width = menuInfo.location.getWidth();
        const int height = menuInfo.location.getHeight();
        menuInfo.location.setPosition(Gui::AbsolutePoint(context.getMenuInfoLocation().getX() - width/2, context.getMenuInfoLocation().getY() - height/2));
        // have to pass the dimensions back in to correct proper placement
        menuInfo.location.setPosition2(Gui::AbsolutePoint(menuInfo.location.getX() + width,menuInfo.location.getY() + height));
        menuInfo.transforms.setRadius(menu.transforms.getRadius());
        menuInfo.colors = menu.colors;
    }

    menuInfo.open();
    
    if (tabs.size() > 0 && tabs[menu.getCurrentTab()]->options.size() > menu.getCurrentIndex()){
        // Add first info option
        addInfo(tabs[menu.getCurrentTab()]->options[menu.getCurrentIndex()]->getInfoText(), menu, context, font); 
    }
    
}

void Menu::TabRenderer::finish(){
    //menu.close();
    menuInfo.close();
}

bool Menu::TabRenderer::active(){
    return true;//menu.isActive();
}

void Menu::TabRenderer::act(const Context & context){
    const Font & font = currentFont();
    // FIXME find a better way to get options to update this is a waste
    for (std::vector<TabInfo *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        TabInfo * tab = *i;
        tab->act();
    }
    menu.act(font);
    menuInfo.act(font);
    actInfo(font);
}

void Menu::TabRenderer::render(const Graphics::Bitmap & bmp, const Font & font){
    menu.render(bmp, font);
    menuInfo.render(bmp, font);
    renderInfo(bmp, font);
}

void Menu::TabRenderer::addOption(MenuOption * opt){
    //this->options.push_back(opt);
}

void Menu::TabRenderer::doAction(const Actions & action, Context & context){
    const Font & font = currentFont();

    bool updateInfo = false;
    switch (action){
        case Up:
            menu.up(font);
            context.playSound(Up);
            updateInfo = true;
            break;
        case Down:
            menu.down(font);
            context.playSound(Down);
            updateInfo = true;
            break;
        case Left:
            menu.left(font);
            context.playSound(Up);
            updateInfo = true;
            break;
        case Right:
            menu.right(font);
            context.playSound(Down);
            updateInfo = true;
            break;
        case Select:
            try{
                if (!menu.isInTab()){
                    menu.toggleTabSelect();
                } else {
                    if (menu.getCurrentTab() < tabs.size() &&
                        menu.getCurrentIndex() < tabs[menu.getCurrentTab()]->options.size()){
                        tabs[menu.getCurrentTab()]->options[menu.getCurrentIndex()]->run(context);
                    }
                    // tabs[menu.getCurrentTab()]->run(menu.getCurrentIndex(), context);
                }
                context.playSound(Select);
            } catch (const Exception::Return & ex){
                menuInfo.open();
            }
            context.playMusic();
            updateInfo = true;
            break;
        case Cancel:
            context.playSound(Cancel);
            if (menu.isInTab()){
                menu.toggleTabSelect();
            } else {
                throw Exception::Return(__FILE__, __LINE__);
            }
            break;
        default:
            break;
    }

    if (updateInfo){
        if (tabs.size() > menu.getCurrentTab() &&
            tabs[menu.getCurrentTab()]->options.size() > menu.getCurrentIndex()){
            addInfo(tabs[menu.getCurrentTab()]->options[menu.getCurrentIndex()]->getInfoText(), menu, context, font);
        }
    }
}

Menu::Context::Context():
cleanup(true),
state(NotStarted),
fades(0),
background(0),
font(NULL),
infoLocation(0, -.5),
menuInfoLocation(0,.95){
}

Menu::Context::Context(const Context & parent, const Context & child):
cleanup(false),
state(NotStarted),
fades(NULL),
background(NULL),
font(NULL),
infoLocation(0,-.5),
menuInfoLocation(0,.95){
    // Update with parents info
    fades = parent.fades;
    background = parent.background;
    sounds = parent.sounds;
    music = parent.music;
    // font = parent.font;
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

    if (Storage::instance().exists(child.music)){
        music = child.music;
    }

    if (child.hasFont()){
        font = child.getFontInfo();
    } else if (parent.hasFont()){
        font = parent.getFontInfo();
    }

    /*
    const FontInfo & result = child.getFont()->get(*parent.getFont());
    if (child.getFont() == &result){
        font = child.getFont();
    } else {
        font = parent.getFont();
    }
    */

    /*
    if (child.getFont() != NULL){
        // Filesystem::exists(child.getFont()->getFont())){
        font = child.getFont();
    }
    */

    /* what are these magic numbers -.5 and .95? */
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

bool Menu::Context::hasFont() const {
    return font != NULL;
}

void Menu::Context::parseToken(const Token * token){
    if ( *token != "context" ){
        throw LoadException(__FILE__, __LINE__, "Not a menu context");
    } else if (!token->hasTokens()){
        return;
    } 
    // Token
    const Token * tok;
    token->view() >> tok;
    
    TokenView view = tok->view();
    while (view.hasMore()){
        const Token * context;
        view >> context;
        if (*context == "fade"){
            // Fade info
            if (!fades){
                fades = new Gui::FadeTool();
            }
            // Set fader default to white
            fades->setFadeInColor(Graphics::makeColor(255,255,255));
            fades->setFadeOutColor(Graphics::makeColor(255,255,255));
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

void Menu::Context::addBackground(const Token * token){
    // Backgrounds
    if (!background){
        background = new Background();
    }
    background->add(new Gui::Animation(token));
}

void Menu::Context::addBackground(const Graphics::Bitmap & image){
    if (!background){
        background = new Background();
    }
    background->add(new Gui::Animation(new Graphics::Bitmap(image)));
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

void Menu::Context::playSound(const Actions & sound) const {
    map<Actions, Filesystem::RelativePath>::const_iterator find = sounds.find(sound);
    if (find != sounds.end() && Storage::instance().exists(find->second)){
        tryPlaySound(find->second);
    }
}

void Menu::Context::addSound(const Actions & sound, const Filesystem::RelativePath & path){
    sounds[sound] = path;
}

void Menu::Context::playMusic(){
    if (Storage::instance().exists(music)){
        if (Music::loadSong(Storage::instance().find(music).path())){
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

void Menu::Context::render(Renderer * renderer, const Graphics::Bitmap & bmp){
    if (background){
        // background
        background->render(Gui::Animation::BackgroundBottom, bmp);
        background->render(Gui::Animation::BackgroundMiddle, bmp);
        background->render(Gui::Animation::BackgroundTop, bmp);
    } else {
        bmp.fill(Graphics::makeColor(0,0,0));
    }

    // Menu
    if (renderer){
        renderer->render(bmp, currentFont());
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
Menu::Menu::Menu(const Type & type):
renderer(0),
type(type){
    setRenderer(type);
}

Menu::Menu::Menu(const Filesystem::AbsolutePath & filename, const Type & type):
renderer(0),
type(type){
    // Load up tokenizer
    try{
        Global::debug(1,"menu") << "Loading menu " << filename.path() << endl;
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        OptionFactory defaultFactory;
        load(token, defaultFactory);
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading menu");
    }
}

Menu::Menu::Menu(const Filesystem::AbsolutePath & filename, const OptionFactory & factory, const Type & type):
renderer(0),
type(type){
    // Load up tokenizer
    try{
        Global::debug(1,"menu") << "Loading menu " << filename.path() << endl;
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        load(token, factory);
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading menu");
    }
}

Menu::Menu::Menu(const Token * token, const Type & type):
renderer(0),
type(type){
    OptionFactory defaultFactory;
    load(token, defaultFactory);
}

Menu::Menu::Menu(const Token * token, const OptionFactory & factory, const Type & type):
renderer(0),
type(type){
    load(token, factory);
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
        
void Menu::Menu::setFont(const Util::ReferenceCount<FontInfo> & font){
    context.setFont(font);
    /*
    if (renderer){
        renderer->setFont(font);
    }
    */
}

void Menu::Menu::load(const Token * token, const OptionFactory & factory){ 
    // version info;
    int major=0, minor=0, micro=0;
    if (!token->hasTokens()){
        throw LoadException(__FILE__, __LINE__, "Empty Menu");
    } else {
        // Get version
        // const Token * tok;
        // token->view() >> tok;
#if 0
        // Do any overrides that may take place before the menu
        const Token *ourToken = token->findToken("_/override");
        if (ourToken != NULL){
            // Handle any overrides of menu or otherwise
                std::string name;
                try {
                    ourToken->view() >> name;
                } catch (const TokenException & ex){
                }
                if (handleOverride(ourToken)){
                    throw LoadException(__FILE__, __LINE__, "Override \"" + name.empty() ? "Unknown" : name + "\" envoked.");
                }
        }
#endif
        const Token *ourToken = token->findToken("_/type");
        if (ourToken != NULL){
            try {
                std::string menuType;
                ourToken->view() >> menuType;
                if (menuType == "default"){
                    type = Default;
                } else if (menuType == "tabbed"){
                    type = Tabbed;
                }
            } catch (const TokenException & ex){
            }
        }
        ourToken = token->findToken("_/version");
        if (ourToken != NULL){
            try {
                ourToken->view() >> major >> minor >> micro;
            } catch (const TokenException & ex){
            }
        } else {
            Global::debug(0, "menu") << "No version indicated, assuming 3.3.1 or below." << endl;
            major = 3;
            minor = 3;
            micro = 1;
        }
    }

    setRenderer(type);

    if (Global::getVersion(major, minor, micro) != Global::getVersion()){
        // Do compatible translations if necessary
        handleCompatibility(token, Global::getVersion(major, minor, micro), factory);
    } else {
        handleCurrentVersion(token);
    }

}

#if 0
 void Menu::Menu::load(const Token * token, const OptionFactory & factory){
    // Check tokens for "override" and "menu" tags
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            tok->print(" ");
            if (*tok == "override"){
                // Handle any overrides of menu or otherwise
                std::string name;
                try {
                    tok->view() >> name;
                } catch (const TokenException & ex){
                }
                if (handleOverride(tok)){
                    throw LoadException(__FILE__, __LINE__, "Override \"" + name.empty() ? "Unknown" : name + "\" envoked.");
                }
            } else if (*tok == "menu"){
                // Commence menu sequence
                // version info;
                int major=0, minor=0, micro=0;
                if (!tok->hasTokens()){
                    throw LoadException(__FILE__, __LINE__, "Empty Menu");
                } else {
                    // Get version
                    // const Token * tok;
                    // token->view() >> tok;
                    const Token *ourToken = tok->findToken("_/type");
                    if (ourToken != NULL){
                        try {
                            std::string menuType;
                            ourToken->view() >> menuType;
                            if (menuType == "default"){
                                type = Default;
                            } else if (menuType == "tabbed"){
                                type = Tabbed;
                            }
                        } catch (const TokenException & ex){
                        }
                    }
                    ourToken = token->findToken("_/version");
                    if (ourToken != NULL){
                        try {
                            ourToken->view() >> major >> minor >> micro;
                        } catch (const TokenException & ex){
                        }
                    } else {
                        Global::debug(0, "menu") << "No version indicated, assuming 3.3.1 or below." << endl;
                        major = 3;
                        minor = 3;
                        micro = 1;
                    }
                }

                setRenderer(type);

                if (Global::getVersion(major, minor, micro) != Global::getVersion()){
                    // Do compatible translations if necessary
                    handleCompatibility(tok, Global::getVersion(major, minor, micro), factory);
                } else {
                    handleCurrentVersion(tok);
                }
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
    
    // Assuming no menu tag was found
    throw LoadException(__FILE__, __LINE__, "No \"menu\" tag was found.");
}
#endif

typedef Menu::Menu MenuClass;
class LanguageMenu: public Menu::Menu {
public:
    class LanguageOption: public MenuOption {
    public:
        LanguageOption(const Gui::ContextBox & box, const string & language):
        MenuOption(box, NULL){
            setText(language);
            setInfoText(language);
        }

        virtual void logic(){
        }

        virtual void run(const ::Menu::Context & context){
            Configuration::setLanguage(getText());
            Configuration::saveConfiguration();
            throw ::Menu::MenuException(__FILE__, __LINE__);
        }
    };

    static vector<string> findLanguages(const MenuClass & original){
        /* get all languages used in the menu */
        vector<Util::ReferenceCount<MenuOption> > options = original.getRenderer()->getOptions();
        map<string, bool> languages;
        for (vector<Util::ReferenceCount<MenuOption> >::iterator it = options.begin(); it != options.end(); it++){
            Util::ReferenceCount<MenuOption> & option = *it;
            
            const map<string, string> strings = option->getLanguageText().getLanguages();
            for (map<string, string>::const_iterator it = strings.begin(); it != strings.end(); it++){
                /* ad-hoc structure, maybe use a specific type for languages? */
                const string & language = (*it).first;
                if (language != ""){
                    languages[language] = true;
                }
            }
        }

        vector<string> out;
        for (map<string, bool>::iterator it = languages.begin(); it != languages.end(); it++){
            out.push_back((*it).first);
        }
        return out;
    }

    static vector<string> putEnglishFirst(vector<string> languages){
        vector<string> out;
        for (vector<string>::iterator it = languages.begin(); it != languages.end(); it++){
            const string & name = *it;
            if (name == "English"){
                out.insert(out.begin(), name);
            } else {
                out.push_back(name);
            }
        }
        return out;
    }

    LanguageMenu(const MenuClass & original){
        ::Menu::DefaultRenderer * renderer = (::Menu::DefaultRenderer*) getRenderer();
        vector<string> languages = putEnglishFirst(findLanguages(original));
        for (vector<string>::iterator it = languages.begin(); it != languages.end(); it++){
            addOption(new LanguageOption(renderer->getBox(), *it));
        }
    }
};

void Menu::Menu::setupDefaultLanguage(const Context & context, const MenuClass & parent){
    LanguageMenu menu(parent);
    menu.setFont(new RelativeFontInfo(Filesystem::RelativePath(sharedFont), sharedFontWidth, sharedFontHeight));
    Configuration::setLanguage("English");
    try{
        menu.run(context);
    } catch (const ::Menu::MenuException & ignore){
    }
}

void Menu::Menu::openOptions(){
    vector<Util::ReferenceCount<MenuOption> > options = getRenderer()->getOptions();
    for (vector<Util::ReferenceCount<MenuOption> >::iterator it = options.begin(); it != options.end(); it++){
        Util::ReferenceCount<MenuOption> & option = *it;
        option->open();
    }
}

void Menu::Menu::closeOptions(){
    vector<Util::ReferenceCount<MenuOption> > options = getRenderer()->getOptions();
    for (vector<Util::ReferenceCount<MenuOption> >::iterator it = options.begin(); it != options.end(); it++){
        Util::ReferenceCount<MenuOption> & option = *it;
        option->close();
    }
}

void Menu::Menu::run(const Context & parentContext){
    /* TODO: replace with Parameter */
    Keyboard::pushRepeatState(true);
    try{
        // Setup context from parent and this menu and initialize
        Context localContext(parentContext, context);
        Util::Parameter<Util::ReferenceCount<FontInfo> > currentFont(menuFontParameter);
        if (context.hasFont()){
            currentFont.push(context.getFontInfo());
        }
        localContext.initialize();

        // Setup menu fonts etc
        if (renderer){        
            renderer->initialize(localContext);
            
            // Invoke Override if available
            renderer->invokeOverride(localContext);
        }
        
        //Play music
        localContext.playMusic();

        if (Configuration::getLanguage() == ""){
            setupDefaultLanguage(localContext, *this);
        }

        /* do any lazy loading options want to do */
        openOptions();

        /* vi keys -- make these optional? */
        input.set(Keyboard::Key_J, 0, true, Down);
        input.set(Keyboard::Key_K, 0, true, Up);
        input.set(Keyboard::Key_H, 0, true, Left);
        input.set(Keyboard::Key_L, 0, true, Right);
        /* regular keys */
        input.set(Configuration::config(0).getUp(), 0, true, Up);
        input.set(Configuration::config(0).getDown(), 0, true, Down);
        input.set(Configuration::config(0).getLeft(), 0, true, Left);
        input.set(Configuration::config(0).getRight(), 0, true, Right);
        /* FIXME: use configuration keys */
        input.set(Configuration::config(0).getAttack1(), 0, true, Select);
        input.set(Keyboard::Key_ENTER, 0, true, Select);
        input.set(Keyboard::Key_ESC, 0, true, Cancel);
        /* joystick */
        input.set(Configuration::config(0).getJoystickUp(), 0, true, Up);
        input.set(Configuration::config(0).getJoystickDown(), 0, true, Down);
        input.set(Configuration::config(0).getJoystickLeft(), 0, true, Left);
        input.set(Configuration::config(0).getJoystickRight(), 0, true, Right);
        /*! FIXME this should be changed to Select/Cancel buttons, all other buttons should be Select */
        input.set(Configuration::config(0).getJoystickAttack1(), 0, true, Select);
        input.set(Configuration::config(0).getJoystickAttack2(), 0, true, Select);
        input.set(Configuration::config(0).getJoystickAttack3(), 0, true, Select);
        input.set(Configuration::config(0).getJoystickJump(), 0, true, Select);
        input.set(Configuration::config(0).getJoystickQuit(), 0, true, Cancel);
       
        /*
        Bitmap work(Global::getScreenWidth(), Global::getScreenHeight());
        work.updateOnResize();
        */
        
        // double runCounter = 0;
        // Global::speed_counter = 0;

        // InputManager::enableBufferInput();
            
        // MenuException or something
        // bool specialExit = false;

        class Logic: public Util::Logic {
        public:
            Logic(Menu & menu, Context & localContext, Renderer * renderer):
            menu(menu),
            localContext(localContext),
            renderer(renderer){
            }
        
            Menu & menu;
            Context & localContext;
            Renderer * renderer;

            void run(){
                try {
                    menu.act(localContext);
                } catch (const Exception::Return & ex){
                    // signaled to quit current menu, closing this one out
                    localContext.finish();
                    if (renderer){
                        renderer->finish();
                    }
                }
            }

            double ticks(double system){
                return system * Global::LOGIC_MULTIPLIER;
            }

            bool done(){
                return localContext.getState() == Context::Completed ||
                       !(renderer && renderer->active());
            }
        };

        class Draw: public Util::Draw {
        public:
            Draw(Menu & menu, Context & localContext):
            menu(menu),
            localContext(localContext){
            }

            Menu & menu;
            Context & localContext;

            void draw(const Graphics::Bitmap & work){
                Util::Parameter<Util::ReferenceCount<FontInfo> > currentFont(menuFontParameter);
                if (Configuration::hasMenuFont()){
                    currentFont.push(Configuration::getMenuFont());
                }
                menu.render(localContext, work);

                work.BlitToScreen();
            }
        };

        Logic logic(*this, localContext, renderer);
        Draw draw(*this, localContext);

        Util::standardLoop(logic, draw);
            
#if 0
        // Run while till the localContext is done
        while (localContext.getState() != Context::Completed &&
              (renderer && renderer->active())){

            if (Global::speed_counter > 0){
                runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
                Global::speed_counter = 0;
                
                /* Added to make the psp update more frequently. */
                /*
                if (runCounter > 3){
                    runCounter = 3;
                }
                */

                while (runCounter >= 1.0){
                    
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

                Util::Parameter<Util::ReferenceCount<FontInfo> > currentFont;
                if (Configuration::hasMenuFont()){
                    currentFont.push(Configuration::getMenuFont());
                }
                render(localContext, work);

                work.BlitToScreen();
            }

            while (Global::speed_counter < 1){
                Util::rest(1);
            }
        }
#endif

        closeOptions();
        
        // FIXME Menu is finished, lets return. Is this even required anymore?
        throw Exception::Return(__FILE__, __LINE__);
    } catch (...){
        Keyboard::popRepeatState();
        throw;
    }
}

void Menu::Menu::act(Context & ourContext){
    // Keys
    // InputManager::poll();
    // InputMap<Actions>::Output inputState = InputManager::getMap(input);
    vector<InputMap<Actions>::InputEvent> events = InputManager::getEvents(input, InputSource());

    for (vector<InputMap<Actions>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
        InputMap<Actions>::InputEvent event = *it;

        if (!event.enabled){
            continue;
        }

        if (event.out == Cancel){
            if (renderer){
                InputManager::waitForRelease(input, InputSource(), Cancel);
                renderer->doAction(Cancel, ourContext);
            } else {
                ourContext.playSound(Cancel);
                InputManager::waitForRelease(input, InputSource(), Cancel);
                throw Exception::Return(__FILE__, __LINE__);
            }
        }

        if (renderer){
            switch (event.out){
                case Up: renderer->doAction(Up, ourContext); break;
                case Down: renderer->doAction(Down, ourContext); break;
                case Left: renderer->doAction(Left, ourContext); break;
                case Right: renderer->doAction(Right, ourContext); break;
                case Select: renderer->doAction(Select, ourContext); break;
                default: break;
            }
        }
    }
            
    if (renderer){
        Util::Parameter<Util::ReferenceCount<FontInfo> > currentFont(menuFontParameter);
        if (Configuration::hasMenuFont()){
            currentFont.push(Configuration::getMenuFont());
        }
        renderer->act(ourContext);
    }

    // Act context
    ourContext.act();
}

void Menu::Menu::render(Context & ourContext, const Graphics::Bitmap & bmp){
    // Render context
    ourContext.render(renderer, bmp);
}

std::string Menu::Menu::getName(){
    std::string name;
    try {
        if (data["name"]){
            *data["name"] >> name;
        }
    } catch (const MenuException & ex){
    }
    return name;
}

std::string Menu::Menu::getInfo(){
    std::string name;
    try {
        if (data["info"]){
            *data["info"] >> name;
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

#if 0
bool Menu::Menu::handleOverride(const Token * token){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "mugen"){
                try{
                    Mugen::run();
                } catch (const LoadException & le){
                    ostringstream out;
                    out << "Press ENTER to continue\n";
                    out << "\n";
                    out << "We are very sorry but an error has occured while trying to load MUGEN.";
                    Util::showError(le, out.str());
                    InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC);
                }
                return true;
            } else {
                Global::debug(0,"menu") <<"Unhandled override: " <<endl;
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
    
    return false;
}
#endif

void Menu::Menu::handleCurrentVersion(const Token * token){
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            // Newer items
            if (*tok == "val" || *tok == "value"){
                const Token * val;
                tok->view() >> val;
                ValueHolder * value = new ValueHolder(val->getName());
                TokenView valueView = val->view();
                try {
                    while (true){
                        *value << valueView;
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

void Menu::Menu::handleCompatibility(const Token * token, int version, const OptionFactory & factory){
    Global::debug(1,"menu") << "Trying version: " << version << endl;
    if (version <= Global::getVersion(3, 3, 1)){
        TokenView view = token->view();
        while (view.hasMore()){
            try {
                const Token * tok;
                view >> tok;
                if ( *tok == "name" ){
                    ValueHolder * value = new ValueHolder("name");
                    TokenView nameView = tok->view();
                    *value << nameView;
                    addData(value);
                } else if ( *tok == "music" ) {
                    ValueHolder * value = new ValueHolder("music");
                    TokenView musicView = tok->view();
                    *value << musicView;
                    addData(value);
                    try {
                        std::string music;
                        *value >> music;
                        context.setMusic(Filesystem::RelativePath(music));
                    } catch (const MenuException & ex){
                    }
                } else if( *tok == "select-sound" ) {
                    ValueHolder * value = new ValueHolder("select-sound");
                    TokenView soundView = tok->view();
                    *value << soundView;
                    addData(value);
                    try{
                        std::string sound;
                        *value >> sound;
                        context.addSound(Up, Filesystem::RelativePath(sound));
                        context.addSound(Down, Filesystem::RelativePath(sound));
                    } catch (const MenuException & ex){
                    }
                } else if (*tok == "back-sound"){
                    ValueHolder * value = new ValueHolder("back-sound");
                    TokenView soundView = tok->view();
                    *value << soundView;
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
                    TokenView okView = tok->view();
                    *value << okView;
                    addData(value);
                    try{
                        std::string sound;
                        *value >> sound;
                        context.addSound(Select,Filesystem::RelativePath(sound));
                    } catch (const MenuException & ex){
                    }
                } else if ( *tok == "background" ) {
                    std::string temp;
                    tok->view() >> temp;
                    context.addBackground(temp);
                } else if (*tok == "anim"){
                    context.addBackground(tok);
                } else if ( *tok == "clear-color" ) {
                    // Not necessary ignore
                } else if (renderer && renderer->readToken(tok, factory)){
                    // Nothing checks compatible version of renderer
                } else if ( *tok == "font" ) {
                    ValueHolder * value = new ValueHolder("font");
                    TokenView fontView = tok->view();
                    *value << fontView << fontView << fontView;
                    addData(value);
                    try {
                        std::string font;
                        int w = 24, h = 24;
                        *value >> font >> w >> h;
                        /*context.setFont(Filesystem::RelativePath(font));
                          context.setFontWidth(w);
                          context.setFontHeight(h);*/
                        context.setFont(new RelativeFontInfo(Filesystem::RelativePath(font), w, h));
                    } catch (const MenuException & ex){
                    }
                } else if ( *tok == "action"){
                    // Set speed
                    //ActionAct(tok);
                } else if ( *tok == "info-position"){
                    ValueHolder * value = new ValueHolder("info-position");
                    TokenView infoView = tok->view();
                    *value << infoView << infoView;
                    addData(value);
                    try {
                        double x=0, y=-.5;
                        *value >> x >> y;
                        context.setInfoLocation(x,y);
                    } catch (const MenuException & ex){
                    } 
                } else if (*tok == "menuinfo"){
                    ValueHolder * value = new ValueHolder("menuinfo");
                    TokenView infoView = tok->view();
                    *value << infoView;
                    addData(value);
                    try {
                        std::string info;
                        *value >> info;
                        context.setMenuInfoText(info);
                    } catch (const MenuException & ex){
                    } 
                } else if (*tok == "menuinfo-position"){
                    ValueHolder * value = new ValueHolder("menuinfo-position");
                    TokenView infoView = tok->view();
                    *value << infoView << infoView;
                    addData(value);
                    try {
                        double x=0, y=.95;
                        *value >> x >> y;
                        context.setMenuInfoLocation(x,y);
                    } catch (const MenuException & ex){
                    } 
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
}
        
void Menu::Menu::setRenderer(const Type & type){
    if (renderer){
        delete renderer;
    }
    renderer = rendererType(type);
}

Menu::Renderer * Menu::Menu::rendererType(const Type & type){
    switch (type){
        case Tabbed: {
            return new TabRenderer();
            break;
        }
        case Default:
        default: {
            return new DefaultRenderer();
            break;
        }
    }
}
