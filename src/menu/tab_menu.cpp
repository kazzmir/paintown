#include "util/bitmap.h"
#include "tab_menu.h"
#include "menu_global.h"
#include "menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/file-system.h"
#include "exceptions/exception.h"
#include "resource.h"
#include "globals.h"
#include "init.h"
#include "configuration.h"
#include "music.h"
#include <math.h>

#include "menu/optionfactory.h"
#include "menu/actionfactory.h"
#include "menu/menu_global.h"
#include "menu/menu_animation.h"

#include "input/input-manager.h"
#include "input/input-map.h"

#include <queue>
#include <map>

using namespace std;
using namespace Gui;

static int FONT_W = 16;
static int FONT_H = 16;
static int TEXT_SPACING_W = 10;
static int TEXT_SPACING_H = 5;

ColorBuffer::ColorBuffer(int color1, int color2):
index(0),
forward(true){
    Util::blend_palette(colors,maxColors,color1,color2);
}

ColorBuffer::~ColorBuffer(){
}

int ColorBuffer::update(){
    // Going to color1 from color2
    if (forward){
        if (index<maxColors-1){
            index++;
        } else {
            forward=!forward;
        }
    } else {
        // Going to color2 from color1
        if (index>0){
            index--;
        } else {
            forward=!forward;
        }
    }

    return colors[index];
}

void ColorBuffer::reset(){
    index = 0;
}

MenuBox::MenuBox(int w, int h):
fontColor(Bitmap::makeColor(255,255,255)),
running(false){
    //position.radius=15;
    location.setRadius(15);
    //context.position.radius = 15;
    context.location.setRadius(15);
    context.colors.borderAlpha = 0;
    context.colors.bodyAlpha = 0;
}

MenuBox::~MenuBox(){
}

bool MenuBox::checkVisible(const Coordinate & area){
    return (context.location.getX() < area.getX2()
	    && context.location.getX2() > area.getX()
	    && context.location.getY() < area.getY2()
	    && context.location.getY2() > area.getY());
}

void MenuBox::setColors (const Gui::ColorInfo &info, const int fontColor){
    colors.body = info.body;
    colors.bodyAlpha = info.bodyAlpha;
    colors.border = info.border;
    colors.borderAlpha = info.borderAlpha;
    this->fontColor = fontColor;
}

void MenuBox::setColors (const int bodyColor, const int borderColor, const int fontColor){
    colors.body = bodyColor;
    colors.border = borderColor;
    this->fontColor = fontColor;
}

TabMenu::TabMenu():
fontColor(Bitmap::makeColor(150,150,150)),
selectedFontColor(Bitmap::makeColor(0,255,255)),
runningFontColor(Bitmap::makeColor(255,255,0)),
runningInfo(""),
location(0),
targetOffset(0),
totalOffset(0),
totalLines(1){
}

void TabMenu::load(Token *token) throw (LoadException){
    if ( *token != "tabmenu" )
        throw LoadException("Not a tabbed menu");
    else if ( ! token->hasTokens() )
        return;

    while ( token->hasTokens() ){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "name" ){
                // Set menu name
                std::string temp;
                *tok >> temp;
                setName(temp);
            } else if ( *tok == "position" ) {
                // This handles the placement of the menu list and surrounding box
                contentArea.setCoordinates(tok);
            } else if ( *tok == "relative-position"){
                contentArea.setCoordinates(tok);
            } else if ( *tok == "coordinate"){
                contentArea.setCoordinates(tok);
            } else if ( *tok == "position-body" ) {
                // This handles the body color of the menu box
                contentArea.setColors(tok);
                contextMenu.colors = contentArea.colors;
            } else if ( *tok == "position-border" ) {
                // This handles the border color of the menu box
                contentArea.setColors(tok);
                contextMenu.colors = contentArea.colors;
            } else if ( *tok == "tab-body" ) {
                // This handles the body color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> tabColors.bodyAlpha;
                tabColors.body = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "tab-border" ) {
                // This handles the border color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> tabColors.borderAlpha;
                tabColors.border = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "selectedtab-body" ) {
                // This handles the body color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> selectedTabColors.bodyAlpha;
                selectedTabColors.body = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "selectedtab-border" ) {
                // This handles the border color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> selectedTabColors.borderAlpha;
                selectedTabColors.border = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "runningtab-body" ) {
                // This handles the body color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> selectedTabColors.bodyAlpha;
                runningTabColors.body = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "runningtab-border" ) {
                // This handles the border color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> selectedTabColors.borderAlpha;
                runningTabColors.border = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "font-color" ) {
                // This handles the font color of the menu box
                int r,g,b;
                *tok >> r >> g >> b;
                fontColor = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "selectedfont-color" ) {
                // This handles the font color of the menu box
                int r,g,b;
                *tok >> r >> g >> b;
                selectedFontColor = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "runningfont-color" ) {
                // This handles the font color of the menu box
                int r,g,b;
                *tok >> r >> g >> b;
                runningFontColor = Bitmap::makeColor(r,g,b);
            } else if( *tok == "anim" ) {
                MenuAnimation *animation = new MenuAnimation(tok);
                if (animation->getLocation() == 0){
                    backgroundAnimations.push_back(animation);
                } else if (animation->getLocation() == 1){
                    foregroundAnimations.push_back(animation);
                }
            } else if (*tok == "menu"){
                //MenuBox *menu = new MenuBox(backboard.position.width, backboard.position.height);
                MenuBox *menu = new MenuBox(contentArea.location.getWidth(), contentArea.location.getHeight());
                if (menu){
                    // To avoid issues
                    menu->menu.setAsOption(true);
                    if (tok->numTokens() == 1){
                        std::string temp;
                        *tok >> temp;
                        menu->menu.load(Filesystem::find(Filesystem::RelativePath(temp)));
                    } else {
                        menu->menu.load(tok);
                    }

                    /* if the menu didn't have any options then we shouldn't include
                     * it. if we try to render a menu without options we will
                     * crash later on.
                     */
                    if (menu->menu.hasSomeOptions()){
                        tabs.push_back(menu);
                        const Font & vFont = Font::getFont(getFont(), FONT_W, FONT_H);
                        // set info on the box itself
                        menu->location.setDimensions(vFont.textLength(menu->menu.getName().c_str()) + TEXT_SPACING_W, vFont.getHeight() + TEXT_SPACING_H);
                        menu->context.location.setPosition(contentArea.location.getPosition());
                        menu->context.location.setPosition2(contentArea.location.getPosition2());
                        menu->context.setFont(getFont(),getFontWidth(),getFontHeight());
                        menu->context.setList(menu->menu.getContextList());
                        menu->context.setFadeSpeed(50);
                        menu->context.open();
                        while(!menu->context.isActive()){
                            menu->context.act();
                        }
                    } else {
                        delete menu;
                    }
                } else {
                    throw LoadException("Problem reading menu");
                }
            } else if( *tok == "menuinfo" ){
                *tok >> menuInfo;
            } else if( *tok == "menuinfo-position" ){
                *tok >> menuInfoLocation.x >> menuInfoLocation.y;
            } else if( *tok == "runninginfo" ){
                *tok >> runningInfo;
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if ( getName().empty() ){
        throw LoadException("No name set, the menu should have a name!");
    }

    if (!menuInfo.empty()){
        if (! menuInfoLocation.x || ! menuInfoLocation.y){
            throw LoadException("The position for the menu info box in \"" + getName() + "\" must be set since there menuinfo is set!"); 
        }
    }

    // Set totalLines
    calculateTabLines();
    
    // Set radius
    contentArea.location.setRadius(15);
}

/* FIXME: this method is a duplicate of Menu::load */
void TabMenu::load(const Filesystem::AbsolutePath & filename) throw (LoadException){
    // Must check for initial token, menu
    try{
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(e.getReason());
    }
}

/* c++ isn't smart enough for me to put the enum inside a function, run(),
 * so I have to put it in the global scope wrapped with a namespace.
 */
namespace Tab{
    enum Input{
        Up,
        Down,
        Left,
        Right,
        Select,
        Exit,
    };
}

static bool closeFloat(double a, double b){
    const double epsilon = 0.001;
    return fabs(a-b) < epsilon;
}

void TabMenu::run(){
    //bool endMenu = false;
    bool done = false;

    if ( tabs.empty() ){
        return;
    }

    double runCounter = 0;
    Global::speed_counter = 0;
    int scrollCounter = 0;

    InputMap<Tab::Input> input;
    input.set(Keyboard::Key_UP, 0, true, Tab::Up);
    input.set(Keyboard::Key_DOWN, 0, true, Tab::Down);
    input.set(Keyboard::Key_H, 0, true, Tab::Left);
    input.set(Keyboard::Key_L, 0, true, Tab::Right);
    input.set(Keyboard::Key_LEFT, 0, true, Tab::Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Tab::Right);
    input.set(Keyboard::Key_ENTER, 0, true, Tab::Select);
    input.set(Keyboard::Key_SPACE, 0, true, Tab::Select);
    input.set(Keyboard::Key_ESC, 0, true, Tab::Exit);
    input.set(Joystick::Left, 0, true, Tab::Left);
    input.set(Joystick::Right, 0, true, Tab::Right);
    input.set(Joystick::Button1, 0, true, Tab::Select);
    input.set(Joystick::Button2, 0, true, Tab::Exit);

    // Color effects
    ColorBuffer fontBuffer(selectedFontColor,runningFontColor);
    ColorBuffer borderBuffer(selectedTabColors.border,runningTabColors.border);
    ColorBuffer backgroundBuffer(selectedTabColors.body,runningTabColors.body);

    currentTab = tabs.begin();
    location = 0;
    targetOffset = 0;
    totalOffset = 0;
    // Set select color
    for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        if (i == currentTab){
            (*i)->setColors(selectedTabColors,selectedFontColor);
        } else {
            (*i)->setColors(tabColors,fontColor);
        }
    }

    // Reset fade stuff
    //resetFadeInfo();

    // Reset animations
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
        (*i)->reset();
    }
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
        (*i)->reset();
    }

    // fade in
    contentArea.open();
    
    // Set up in case fonts or something changed
    for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        MenuBox *tab = *i;
        const Font & vFont = Font::getFont(getFont(), FONT_W, FONT_H);
        // set info on the box itself
        tab->location.setDimensions(vFont.textLength(tab->menu.getName().c_str()) + TEXT_SPACING_W, vFont.getHeight() + TEXT_SPACING_H);
        
        tab->context.location.setPosition(contentArea.location.getPosition());
        tab->context.location.setPosition2(contentArea.location.getPosition2());
        tab->context.setFont(getFont(),getFontWidth(),getFontHeight());
        tab->context.open();
        while(!tab->context.isActive()){
            tab->context.act();
        }
    }

    // Recalculate lines
    calculateTabLines();
    
    // set menu info
    setMenuInfo(menuInfo);

    while (!done){

        bool draw = false;
        /*
        //const char vi_up = 'k';
        //const char vi_down = 'j';
        const char vi_left = 'h';
        const char vi_right = 'l';
        */

        InputManager::poll();

        if ( Global::speed_counter > 0 ){
            draw = true;
            runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
            while ( runCounter >= 1.0 ){
                runCounter -= 1;
                InputMap<Tab::Input>::Output inputState = InputManager::getMap(input);
                // Keys
                if (!(*currentTab)->running){
                    if (inputState[Tab::Left]){
                        MenuGlobals::playSelectSound();
                        // Reset color
                        (*currentTab)->setColors(tabColors,fontColor);
                        if (currentTab > tabs.begin()){
                            currentTab--;
                            location--;
                            //targetOffset+=backboard.position.width;
                            targetOffset+=contentArea.location.getWidth();
                        } else {
                            currentTab = tabs.end()-1;
                            location=tabs.size()-1;
                            //targetOffset = (location*backboard.position.width) * -1;
                            targetOffset = (location*contentArea.location.getWidth()) * -1;
                        }
                        (*currentTab)->setColors(selectedTabColors,selectedFontColor);
                    }

                    if (inputState[Tab::Right]){
                        MenuGlobals::playSelectSound();
                        // Reset color
                        (*currentTab)->setColors(tabColors,fontColor);
                        if (currentTab < tabs.begin()+tabs.size()-1){
                            currentTab++;
                            location++;
                            //targetOffset-=backboard.position.width;
                            targetOffset-=contentArea.location.getWidth();
                        } else {
                            currentTab = tabs.begin();
                            location = 0;
                            targetOffset = 0;
                        }
                        (*currentTab)->setColors(selectedTabColors,selectedFontColor);
                    }
                    
                    if (inputState[Tab::Select]){
                        (*currentTab)->running = true;
                        backgroundBuffer.reset();
                        borderBuffer.reset();
                        fontBuffer.reset();
                        setMenuInfo(runningInfo);
                        MenuOption *opt = (*currentTab)->menu.getOption((*currentTab)->context.getCurrentIndex());
                        addInfoBox(opt->getInfoText());
                    }

                    if (inputState[Tab::Exit]){
                        /* is there a reason to set done = true ? */
                        done = true;
                        InputManager::waitForRelease(input, Tab::Exit);
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                } else {
                    try{
                        if (inputState[Tab::Up]){
                            if ((*currentTab)->context.previous()){
                                MenuGlobals::playSelectSound();
                                MenuOption *opt = (*currentTab)->menu.getOption((*currentTab)->context.getCurrentIndex());
                                addInfoBox(opt->getInfoText());
                            }
                        }
                        if (inputState[Tab::Down]){
                            if ((*currentTab)->context.next()){
                                MenuGlobals::playSelectSound();
                                MenuOption *opt = (*currentTab)->menu.getOption((*currentTab)->context.getCurrentIndex());
                                addInfoBox(opt->getInfoText());
                            }
                        }
                        if (inputState[Tab::Select]){
                            InputManager::waitForRelease(input, Tab::Select);
                            // Run menu
                            (*currentTab)->menu.runOption((*currentTab)->context.getCurrentIndex());
                        }

                        if (inputState[Tab::Exit]){
                            /* is there a reason to set done = true ? */
                            InputManager::waitForRelease(input, Tab::Exit);
                            throw Exception::Return(__FILE__, __LINE__);
                        }
                        (*currentTab)->context.act();
                        (*currentTab)->setColors(backgroundBuffer.update(),borderBuffer.update(),fontBuffer.update());
                    } catch (const Exception::Return & re){
                        (*currentTab)->running = false;
                        (*currentTab)->setColors(selectedTabColors, selectedFontColor);
                        setMenuInfo(menuInfo);
                        closeInfoBoxes();
                    }
                }

                /* act tabs */
                const double incrementx = contentArea.location.getWidth();
                double startx = contentArea.location.getX() + totalOffset;
                for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
                    MenuBox *tab = *i;
                    tab->context.location.setPosition(Gui::AbsolutePoint((int)startx, contentArea.location.getY()));
                    tab->context.location.setPosition2(contentArea.location.getPosition2());
                    tab->context.setUseGradient(tab->running);
                    tab->context.act();
                    startx += incrementx;
                }

                // Animations
                for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                    (*i)->act();
                }
                for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                    (*i)->act();
                }

                // Lets do some logic for the box with text
                contentArea.act();
                
                menuInfoBox.act();
                
                actInfoBoxes();

                if (scrollCounter == 0 && !closeFloat(totalOffset, targetOffset)){
                    totalOffset = (totalOffset + targetOffset) / 2;
                    /* not sure if this is stricly necessary */
                    if (fabs(targetOffset - totalOffset) < 5){
                        totalOffset = targetOffset;
                    }
                }
                /* higher values of % X slow down scrolling */
                scrollCounter = (scrollCounter + 1) % 5;

            }

            Global::speed_counter = 0;
        }

        if ( draw ){
            // Draw
            drawBackground(work);

            // Do background animations
            for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                (*i)->draw(work);
            }

            // Draw menu area
            contentArea.render(*work);

            // Menus
            if (contentArea.isActive()){
                drawMenus(work);
            }

            // Draw menu info text
            menuInfoBox.render(*work);
            
            renderInfoBoxes(*work);

            // Draw foreground animations
            for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                (*i)->draw(work);
            }

            // Finally render to screen
            work->BlitToScreen();
        }

        while ( Global::speed_counter < 1 ){
            Util::rest( 1 );
            InputManager::poll();
        }
    }
}

void TabMenu::drawMenus(Bitmap *bmp){
    Gui::Coordinate & backboard = contentArea.location;
    // Drawing menus
    for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        MenuBox *tab = *i;
        if (tab->checkVisible(backboard)){
            tab->context.render(*bmp);
        }
    }
    const Font & vFont = Font::getFont(getFont(), FONT_W, FONT_H);
    int tabstartx = backboard.getX();
    int tabstarty = backboard.getY() - ((vFont.getHeight() + TEXT_SPACING_H) * totalLines);
    // Now draw tabs, has to be seperate from above since we need this to overlay the snaps
    for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        MenuBox *tab = *i;
        const int tabWidth = tab->location.getWidth();
        if ((tabstartx + tabWidth) > (backboard.getX2())){
            tabstartx = backboard.getX();
            tabstarty += tab->location.getHeight();
        }
        tab->location.setPosition(Gui::AbsolutePoint(tabstartx, tabstarty));
        tab->render(*bmp);
        // Draw text
        vFont.printf(tabstartx + ((tabWidth/2)-(vFont.textLength(tab->menu.getName().c_str())/2)), tabstarty, tab->fontColor, *bmp, tab->menu.getName(), 0 );
        tabstartx += tab->location.getWidth();
    }
}

// Calculate the amount of lines per tabs
void TabMenu::calculateTabLines(){
    int tabstartx = contentArea.location.getX();//backboard.position.x;
    for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        MenuBox *tab = *i;
        const int tabWidth = tab->location.getWidth();
        if ((tabstartx + tabWidth) > (contentArea.location.getX2())){//(backboard.position.x + backboard.position.width)){
            tabstartx = contentArea.location.getX();//backboard.position.x;
            totalLines++;
        }
        tabstartx+=tab->location.getWidth();
    }
}

TabMenu::~TabMenu(){
    // Rid menus
    for (std::vector<MenuBox *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        if (*i){
            delete *i;
        }
    }
}
