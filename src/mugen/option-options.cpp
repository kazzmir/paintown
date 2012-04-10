#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/debug.h"
#include "util/menu/menu_option.h"
#include "util/menu/menu.h"
/* FIXME: only needed for OptionLevel */
#include "util/menu/options.h"
#include "util/loading.h"
#include "configuration.h"
#include "options.h"
#include "game.h"
#include "menu.h"
#include "config.h"
#include "font.h"
#include "sound.h"
#include "background.h"
#include "parse-cache.h"
#include "search.h"

#include <ostream>
#include <vector>

#include "ast/all.h"
#include "parser/all.h"
#include "util/events.h"
#include "util/init.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "util/exceptions/exception.h"

#include "util/gui/box.h"

#include "globals.h"

namespace PaintownUtil = ::Util;
using namespace std;
using namespace Gui;

namespace Mugen{

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;

/* EMPTY_STRING is a macro in Allegro4, so just add an X to avoid a name collision */
static const std::string EMPTY_STRINGX = "";

static std::string getString(int number){
    std::ostringstream str;
    str << number;
    return str.str();
}

ListFont::ListFont():
font(PaintownUtil::ReferenceCount<Mugen::Font>(NULL)),
bank(-1),
position(0){
}

ListFont::ListFont(PaintownUtil::ReferenceCount<Mugen::Font> font, int bank, int position):
font(font),
bank(bank),
position(position){
}

ListFont::ListFont(const ListFont & copy):
font(copy.font),
bank(copy.bank),
position(copy.position){
}

ListFont::~ListFont(){
}

const ListFont & ListFont::operator=(const ListFont & copy){
    font = copy.font;
    bank = copy.bank;
    position = copy.position;
    
    return *this;
}

void ListFont::draw(int x, int y, const std::string & message, const Graphics::Bitmap & work) const{
    if (font != NULL){
        font->render(x, y, position, bank, work, message);
    }
}

void ListFont::draw(int x, int y, int positionOverride, const std::string & message, const Graphics::Bitmap & work) const{
    if (font != NULL){
        font->render(x, y, positionOverride, bank, work, message);
    }
}

int ListFont::getHeight() const{
    if (font != NULL){
        return font->getHeight();
    }
    
    return 0;
}

int ListFont::getWidth(const std::string & text) const{
    if (font != NULL){
        return font->textLength(text.c_str());
    }
    return 0;
}

ListItem::ListItem(){
}

ListItem::~ListItem(){
}

void ListItem::draw(int x, int y, const Graphics::Bitmap &, const ::Font & font, int distance) const{
}

int ListItem::size(const ::Font & font) const{
    return 0;
}

void ListItem::run(){
}

bool ListItem::isRunnable() const {
    return false;
}

const std::string & ListItem::getInfo() const{
    return EMPTY_STRINGX;
}

ScrollAction::ScrollAction():
expandState(Disabled),
current(0),
itemTop(0),
itemBottom(0),
visibleItems(5),
autoSpacing(false),
startX(0),
startY(0),
spacingX(0),
spacingY(0),
offsetX(0),
currentOffsetX(0),
offsetY(0),
currentOffsetY(0),
scrollWaitX(5),
scrollWaitY(2),
marginTop(0),
marginBottom(0),
leftBoundary(0),
rightBoundary(0),
showCursor(false),
autoCursor(false),
cursorX1(0),
cursorX2(0),
cursorY1(0),
cursorY2(0),
cursorAlpha(0),
cursorAlphaMod(6){
}

ScrollAction::~ScrollAction(){
}

void ScrollAction::act(){
    if (autoSpacing){
        spacingY = font.getHeight() + font.getHeight()/2;
        setMargins(spacingY, spacingY);
    }
    
    if (showCursor){
        cursorAlpha+=cursorAlphaMod;
        if (cursorAlpha > 128){
            cursorAlpha = 128;
            cursorAlphaMod = -6;
        } else if (cursorAlpha < 0){
            cursorAlpha = 0;
            cursorAlphaMod = 6;
        }
    }
    
    if (scrollWaitX > 0){
        scrollWaitX--;
    } else {
        currentOffsetX = (currentOffsetX+offsetX)/2;
        scrollWaitX = 5;
    }

    if (scrollWaitY > 0){
        scrollWaitY--;
    } else {
        currentOffsetY = (currentOffsetY+offsetY)/2;
        scrollWaitY = 2;
    }
}

void ScrollAction::render(const Graphics::Bitmap & work, const ::Font &) const{
    int y = startY - currentOffsetY + marginTop;
    int x = startX;
    bool moveLeft = true;
    unsigned int index = 0;
    for (std::vector<PaintownUtil::ReferenceCount<ScrollItem> >::const_iterator i = text.begin(); i != text.end(); ++i, ++index){
        //if (index >= itemTop-1 && index <= itemBottom+1){
            const PaintownUtil::ReferenceCount<ListItem> item = (*i).convert<ListItem>();
            const ListFont & useFont = (index != current) ? font : activeFont;
            if (expandState == Disabled){
                item->render(x, y, work, useFont, leftBoundary, rightBoundary);
                if (index == current && showCursor){
                    Graphics::Bitmap::transBlender(0,0,0,cursorAlpha);
                    if (autoCursor){
                        work.translucent().rectangleFill(leftBoundary-2, y - useFont.getHeight()-1, rightBoundary+2, y+1, Graphics::makeColor(255,255,255));
                    } else {
                        work.translucent().rectangleFill(x + cursorX1, y + cursorY1, x + cursorX2, y + cursorY2, Graphics::makeColor(255,255,255));
                    }
                }
            } else {
                if (moveLeft){
                    item->render(x - currentOffsetX, y, work, useFont, leftBoundary, rightBoundary);
                } else {
                    item->render(x + currentOffsetX, y, work, useFont, leftBoundary, rightBoundary);
                }
                moveLeft = !moveLeft;
            }
        //}
        x+=spacingX;
        y+=spacingY;
    }
}

void ScrollAction::addItem(const PaintownUtil::ReferenceCount<Gui::ScrollItem> item){
    text.push_back(item);
}

void ScrollAction::addItems(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & items){
    text.insert(text.end(), items.begin(), items.end());
}

const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & ScrollAction::getItems() const{
    return text;
}

bool ScrollAction::updateItem(unsigned int index, const PaintownUtil::ReferenceCount<Gui::ScrollItem> item){
    if (index >= text.size()){
        return false;
    }
    text[index] = item;
    return true;
}

void ScrollAction::clearItems(){
    text.clear();
}

unsigned int ScrollAction::getCurrentIndex() const{
    return current;
}

void ScrollAction::setCurrentIndex(unsigned int index){
    if (index >= text.size() || index == current){
        return;
    }
    if (index > current){
        while (current != index){
            next();
        }
    } else if (index < current){
        while (current != index){
            previous();
        }
    }
}

bool ScrollAction::next(){
    if (current < text.size()-1){
        current++;
        if (current > itemBottom){
            itemTop++;
            itemBottom++;
            offsetY = (itemBottom+1 - getVisibleItems()) * spacingY;
        }
    } else {
        current = itemTop = 0;
        itemBottom = getVisibleItems()-1;
        offsetY = 0;
    }
    return true;
}

bool ScrollAction::previous(){
    if (current > 0){
        current--;
        if (current < itemTop){
            itemTop--;
            itemBottom--;
            offsetY = (itemBottom+1 - getVisibleItems()) * spacingY;
        }
    } else {
        current = itemBottom = text.size()-1;
        itemTop = text.size() - getVisibleItems();
        offsetY = (itemBottom+1 - getVisibleItems()) * spacingY;
    }
    return true;
}

bool ScrollAction::valueNext(){
    return getCurrent()->next();
}

bool ScrollAction::valuePrevious(){
    return getCurrent()->previous();
}

int ScrollAction::getMaxWidth(){
    int width = 0;
    for (std::vector<PaintownUtil::ReferenceCount<ScrollItem> >::const_iterator i = text.begin(); i != text.end(); ++i){
        const PaintownUtil::ReferenceCount<ListItem> item = (*i).convert<ListItem>();
        const int check = item->getWidth(font);
        if (check > width){
            width = check;
        }
    }
    return width;
}

int ScrollAction::getMaxHeight(){
    return spacingY * getVisibleItems();
}

void ScrollAction::recalculateVisibleItems(int height){
    unsigned int visible = (height/(spacingY));
    if (visibleItems != visible){
        setVisibleItems(visible);
    }
}

void ScrollAction::setExpandState(const ExpandState & state){
    expandState = state;
    switch (expandState){
        case Expand:
            // FIXME get the screen width and go off of that
            offsetX = 320;
            break;
        case Retract:
            offsetX = 0;
            break;
        case Disabled:
        default:
            offsetX = currentOffsetX = 0;
            break;
    }
}

void ScrollAction::setListFont(const ListFont & f){
    font = f;
}

void ScrollAction::setActiveFont(const ListFont & f){
    activeFont = f;
}

unsigned int ScrollAction::getVisibleItems(){
    if (visibleItems > text.size()-1){
        return text.size();
    }
    return visibleItems;
}

Option::Option():
selected(false),
alpha(0),
alphaMod(6){
}

Option::~Option(){
}

void Option::enter(){
}

void Option::render(Mugen::Font & font, int x1, int x2, int y, const Graphics::Bitmap & bmp){
    font.render(x1, y, 1, 0, bmp, optionName);
    font.render(x2, y, -1, 0, bmp, currentValue);
    if (selected){
        alpha += alphaMod;
        if (alpha <= 0){
            alpha = 0;
            alphaMod = 6;
        }
        else if (alpha >= 128){
            alpha = 128;
            alphaMod = -6;
        }
        Graphics::Bitmap::transBlender(0,0,0,alpha);
        bmp.translucent().rectangleFill(x1-2, y-font.getHeight()-2, x2+2, y+2, Graphics::makeColor(255,255,255));
    }
}

int Option::getWidth(Mugen::Font & font){
    return font.textLength(optionName.c_str()) + font.textLength(currentValue.c_str());
}

BaseMenuItem::BaseMenuItem(){
}

BaseMenuItem::~BaseMenuItem(){
}

void BaseMenuItem::render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
    if (displayInfo){
        font.draw(x, y, 0, optionName, work);
    } else {
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
}

int BaseMenuItem::getWidth(const ListFont & font) const {
    return (font.getWidth(optionName + "  " + currentValue));
}

const std::string & BaseMenuItem::getInfo() const {
    if (displayInfo){
        return currentValue;
    }
    return EMPTY_STRINGX;
}

bool BaseMenuItem::displayInfo = false;

class Difficulty: public BaseMenuItem {
public:
    Difficulty(){
        optionName = "Difficulty";
            int difficulty = Data::getInstance().getDifficulty();
            if (difficulty < 1){
                difficulty = 1;
                Data::getInstance().setDifficulty(difficulty);
            } else if (difficulty > 8){
                difficulty = 8;
                Data::getInstance().setDifficulty(difficulty);
            }
        currentValue = getDifficultyName(difficulty);
    }
    virtual ~Difficulty(){
    }
    bool next(){
            int difficulty = Data::getInstance().getDifficulty() + 1;
            if (difficulty > 8){
                difficulty = 8;
                return false;
            }
            Data::getInstance().setDifficulty(difficulty);
            currentValue = getDifficultyName(difficulty);
            return true;
        }
    bool previous(){
            int difficulty = Data::getInstance().getDifficulty() - 1;
            if (difficulty < 1){
                difficulty = 1;
                return false;
            }
            Data::getInstance().setDifficulty(difficulty);
            currentValue = getDifficultyName(difficulty);
            return true;
    }
        
    std::string getDifficultyName(int difficulty){
        
        switch (difficulty){
            case 1:
            case 2:
                return "Easy " + getString(difficulty);
                break;
            case 3:
            case 4:
            case 5:
                return "Medium " + getString(difficulty);
                break;
            case 6:
            case 7:
            case 8:
                return "Hard " + getString(difficulty);
                break;
            default:
                break;
        }
        return std::string();
    }
};

class Life : public BaseMenuItem {
    public:
	Life(){
	    optionName = "Life";
            int life = Data::getInstance().getLife();
            if (life < 30){
                life = 30;
                Data::getInstance().setLife(life);
            } else if (life > 300){
                life = 300;
                Data::getInstance().setLife(life);
            }
	    currentValue = getString(life)+"%%";
	}
	~Life(){
	}
	bool next(){
        int life = Data::getInstance().getLife()+10;
        if (life > 300){
            life = 300;
            return false;
        }
        Data::getInstance().setLife(life);
        currentValue = getString(life)+"%%";
        return true;
        }
	bool previous(){
        int life = Data::getInstance().getLife()-10;
        if (life < 30){
            life = 30;
            return false;
        }
        Data::getInstance().setLife(life);
        currentValue = getString(life)+"%%";
        return true;
	}
};

class TimeLimit : public BaseMenuItem {
    public:
	TimeLimit(){
	    optionName = "Time Limit";
        int time = Data::getInstance().getTime();
        if (time == -1){
            currentValue = "None";
        } else {
            currentValue = getString(time);
        }
	}
	~TimeLimit(){
	}
	bool next(){
	    int time = Data::getInstance().getTime();
	    if (time != -1){
		time+=20;
	    }
	    if (time == 100){
		time = 99;
	    } else if (time > 100){
		time = -1;
	    }
	    Data::getInstance().setTime(time);
	    if (time == -1){
		currentValue = "None";
	    } else {
		currentValue = getString(time);
	    }
	    return true;
	}
	bool previous(){
	    int time = Data::getInstance().getTime();
	    if (time == -1){
		time = 99;
	    } else if (time >= 20 && time != 99){
		time-=20;
	    } else if (time == 99){
		time = 80;
	    }
	    if (time < 20){
		time = 20;
	    }
	    Data::getInstance().setTime(time);
	    currentValue = getString(time);
        return true;
	}
};

class Speed : public BaseMenuItem {
    public:
	Speed(){
	    optionName = "Speed";
            double speed = checkSpeed(Data::getInstance().getGameSpeed());
            currentValue = getSpeedName(speed);
	}
	~Speed(){
	}
	bool next(){
            double speed = checkSpeed(Data::getInstance().getGameSpeed() + 0.1);
            Data::getInstance().setGameSpeed(speed);
	    currentValue = getSpeedName(speed);
        return true;
	}
	bool previous(){
            double speed = checkSpeed(Data::getInstance().getGameSpeed() - 0.1);
            Data::getInstance().setGameSpeed(speed);
	    currentValue = getSpeedName(speed);
        return true;
	}
	
	double checkSpeed(double speed){
            if (speed < 0.1){
                speed = 0.1;
            } else if ((speed > 0.9) && (speed < 1.1)){
                speed = 1;
            } else if (speed > 1.9){
                speed = 1.9;
            }
            return speed;
        }

        std::string getSpeedName(double speed){
            if (speed == 1){
                return "Normal";
            } else if (speed <= 0.9){
                int num = fabs(((speed-0.9)-0.1)*10);
                return "Slow " + getString(num);
            } else if (speed >= 1.1){
                int num = ((speed-1)*10);
                return "Fast " + getString(num);
            }
            return std::string();
        }
};

class OneVsTeam : public BaseMenuItem {
    public:
	OneVsTeam(){
	    optionName = "1P VS Team Advantage";
	    currentValue = getString(Data::getInstance().getTeam1vs2Life());
	}
	~OneVsTeam(){
	}
	bool next(){
        return false;
	}
	bool previous(){
        return false;
	}
};

class TeamLoseOnKO : public BaseMenuItem {
    public:
	TeamLoseOnKO(){
	    optionName = "If player KOed";
	    currentValue = getString(Data::getInstance().getTeamLoseOnKO());
	}
	~TeamLoseOnKO(){
	}
	bool next(){
        return false;
	}
	bool previous(){
        return false;
	}
};

class AutoSearch : public BaseMenuItem {
    public:
    AutoSearch(){
        optionName = "Search Chars/Stages";
        setValue();
    }
    ~AutoSearch(){
    }
    void setValue(){
        switch (Data::getInstance().getSearchType()){
            case Data::SelectDef:
                currentValue = "select.def";
                break;
            case Data::SelectDefAndAuto:
                currentValue = "select.def + auto";
                break;
            case Data::Auto:
                currentValue = "auto";
                break;
            default:
                break;
        }
    }
    bool next(){
        switch (Data::getInstance().getSearchType()){
            case Data::SelectDef:
                Data::getInstance().setSearchType(Data::SelectDefAndAuto);
                break;
            case Data::SelectDefAndAuto:
                Data::getInstance().setSearchType(Data::Auto);
                break;
            case Data::Auto:
                Data::getInstance().setSearchType(Data::SelectDef);
                break;
            default:
                break;
        }
        setValue();
        return true;
    }
    bool previous(){
        
        switch (Data::getInstance().getSearchType()){
            case Data::SelectDef:
                Data::getInstance().setSearchType(Data::Auto);
                break;
            case Data::SelectDefAndAuto:
                Data::getInstance().setSearchType(Data::SelectDef);
                break;
            case Data::Auto:
                Data::getInstance().setSearchType(Data::SelectDefAndAuto);
                break;
            default:
                break;
        }
        setValue();
        return true;
    }
    
    virtual int getWidth(const ListFont & font) const {
        return (font.getWidth(optionName + "  select.def + auto"));
    }
};

class Escape: public BaseMenuItem {
public:
    
    class EscapeException : public std::exception{
    public:
        EscapeException(){
        }
        ~EscapeException() throw() {
        }
    };
    Escape(){
        optionName = "Return to Main Menu";
        currentValue = "(Esc)";
    }

    virtual ~Escape(){
    }

    bool next(){
        return false;
    }

    bool previous(){
        return false;
    }
    
    bool isRunnable() const{
        // NOTE lets jump the gun here
        throw EscapeException();
        return true;
    }

    void run(){
        throw EscapeException();
    }
};

OptionMenu::OptionMenu(const std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > & items):
renderBackground(true),
clearColor(Graphics::makeColor(0,0,0)),
clearAlpha(255),
fadeEnabled(true),
recalculateHeight(160){
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
    fader.setFadeInTime(10);
    fader.setFadeOutTime(10);
    list.addItems(items);
    Filesystem::AbsolutePath systemFile = Data::getInstance().getFileFromMotif(Data::getInstance().getMotif());
    // Lets look for our def since some people think that all file systems are case insensitive
    Filesystem::AbsolutePath baseDir = systemFile.getDirectory();

    Global::debug(1) << baseDir.path() << endl;

    if (systemFile.isEmpty()){
        throw MugenException("Cannot locate character select definition file for: " + systemFile.path(), __FILE__, __LINE__);
    }

    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(systemFile.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + systemFile.path() + " in" + diff.printTime("") << endl;

    std::vector< PaintownUtil::ReferenceCount<Font> > fonts;
    
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        if (head == "Files"){
            class FileWalker: public Ast::Walker {
            public:
                FileWalker(OptionMenu & self, std::vector< PaintownUtil::ReferenceCount<Font> > & fonts, const Filesystem::AbsolutePath & baseDir):
                self(self),
                fonts(fonts),
                baseDir(baseDir){
                }
                
                OptionMenu & self;
                std::vector< PaintownUtil::ReferenceCount<Font> > & fonts;
                const Filesystem::AbsolutePath & baseDir;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "snd"){
                        std::string file;
                        simple.view() >> file;
                        self.sounds.init(file);
                    } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                        std::string temp;
                        simple.view() >> temp;
                        fonts.push_back(PaintownUtil::ReferenceCount<Font>(new Font(Util::findFile(Filesystem::RelativePath(temp)))));
                        Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
                    }
                }
            };
            FileWalker walker(*this, fonts, baseDir);
            section->walk(walker);
        } else if (head == "Option Info"){
            class InfoWalker: public Ast::Walker{
            public:
                InfoWalker(OptionMenu & self):
                self(self){
                }

                OptionMenu & self;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    int group=0, sound=0;
                    if (simple == "cursor.move.snd"){
                        try{
                            simple.view() >> group >> sound;
                            self.sounds.set(OptionMenu::Move, group, sound);
                        } catch (const Ast::Exception & e){
                        }
                    } else if (simple == "cursor.done.snd"){
                        try{
                            simple.view() >> group >> sound;
                            self.sounds.set(OptionMenu::Done, group, sound);
                        } catch (const Ast::Exception & e){
                        }
                    } else if (simple == "cancel.snd"){
                        try{
                            simple.view() >> group >> sound;
                            self.sounds.set(OptionMenu::Cancel, group, sound);
                        } catch (const Ast::Exception & e){
                        }
                    } 
                }
            };
            InfoWalker walker(*this);
            section->walk(walker);
        } else if (head == "OptionBGdef"){ 
            /* Background management */
            background = PaintownUtil::ReferenceCount<Background>(new Background(systemFile, "optionbg"));
        }
    }

    /* FIXME: do all other cleanup here too */
    if (background == NULL){
        throw MugenException("OptionBGDef was not specified", __FILE__, __LINE__);
    }
    
    
    // Our Font
    for (std::vector<PaintownUtil::ReferenceCount<Font> >::iterator i = fonts.begin(); i != fonts.end(); ++i){
        PaintownUtil::ReferenceCount<Font> ourFont = *i;
        // NOTE This should keep it in a reasonable range, although I don't think it's correct
        /*if (ourFont != NULL && (ourFont->getHeight() >= 8 && ourFont->getHeight() < 15)){
            if (ourFont->textLength(" ") <= 15){
                font = ourFont;
                break;
            }
        }*/
        if (ourFont != NULL && ourFont->getHeight() >= 8){
            font = ourFont;
            break;
        }
    }
    
    if (font == NULL){
        throw MugenException("Couldn't find suitable font to use in the Options Menu.", __FILE__, __LINE__);
    }
    
    // Setup list
    ListFont listFont(font, 0, 0);
    
    list.setListFont(listFont);
    list.setActiveFont(listFont);
    list.setVisibleItems(3);
    list.setAutoSpacing(true);
    list.setLocation(160, 0);
    list.setShowCursor(true);
    list.setAutoCursor(true);
    list.setCursorCoords(-20,-10,16,0);
    list.setCurrentIndex(0);
    //list.setExpandState(ScrollAction::Expand);
}

OptionMenu::OptionMenu(const OptionMenu & menu):
name(menu.name),
background(menu.background),
renderBackground(menu.renderBackground),
clearColor(menu.clearColor),
clearAlpha(menu.clearAlpha),
sounds(menu.sounds),
font(menu.font),
fadeEnabled(menu.fadeEnabled),
recalculateHeight(menu.recalculateHeight),
screenCapture(menu.screenCapture){
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
    fader.setFadeInTime(10);
    fader.setFadeOutTime(10);
    
    // Setup list
    ListFont listFont(font, 0, 0);
    
    list.setListFont(listFont);
    list.setActiveFont(listFont);
    list.setVisibleItems(3);
    list.setAutoSpacing(true);
    list.setLocation(160, 0);
    list.setShowCursor(true);
    list.setAutoCursor(true);
    list.setCurrentIndex(0);
}

OptionMenu::~OptionMenu(){
}

void OptionMenu::act(){
    background->act();
    fader.act();
    list.act();
    list.recalculateVisibleItems(recalculateHeight);
    // Check our base options
    if (list.getMaxWidth()+25 >= DEFAULT_WIDTH){
        BaseMenuItem::displayInfo = true;
    } else {
        BaseMenuItem::displayInfo = false;
    }
}

void OptionMenu::draw(const Graphics::Bitmap & work){
    Graphics::StretchedBitmap workArea(320, 240, work, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
    workArea.start();
    
    // Backgrounds
    if (renderBackground){
        background->renderBackground(0, 0, workArea);
    } else if (clearAlpha > 0){
        if (screenCapture != NULL){
            screenCapture->drawStretched(workArea);
        }
        Graphics::Bitmap::transBlender(0,0,0,clearAlpha);
        workArea.translucent().fill(clearColor);
    }
    
    const int width = list.getMaxWidth();
    const int height = list.getMaxHeight();
    const int x = 160 - width/2;
    const int y = 120 - height/2;
    
    // Name of options
    drawInfo(160, list.getFont().getHeight() + 2, name, workArea);
    
    Graphics::Bitmap::transBlender(0,0,0,150);
    workArea.translucent().roundRectFill(5, x-25, y-15, x+width+25, y+height+15,Graphics::makeColor(0,0,60));
    workArea.translucent().roundRect(5, x-25, y-15, x+width+25, y+height+15, Graphics::makeColor(0,0,20));
    
    Graphics::Bitmap temp(workArea, 0, y, work.getWidth(), height);
    list.setBoundaries(x-20, x+width+20);
    list.render(temp, ::Font::getDefaultFont());
    
    // Info
    const PaintownUtil::ReferenceCount<ListItem> item = list.getCurrent();
    if (!item->getInfo().empty()){
        drawInfo(160, 239, item->getInfo(), workArea);
    }
    
    // Foregrounds
    if (renderBackground){
        background->renderForeground(0, 0, workArea);
    }
    
    // Fader
    if (fadeEnabled){
        fader.draw(workArea);
    }
    
    workArea.finish();
}

void OptionMenu::drawList(const Graphics::Bitmap & work){
    
    const int width = list.getMaxWidth();
    const int height = list.getMaxHeight();
    const int x = 160 - width/2;
    const int y = 120 - height/2;
    
    // Background
    if (clearAlpha > 0){
        Graphics::Bitmap::transBlender(0,0,0,clearAlpha);
        work.translucent().fill(clearColor);
    }
    
    // Name of options
    drawInfo(160, list.getFont().getHeight() + 2, name, work);
    
    Graphics::Bitmap::transBlender(0,0,0,150);
    work.translucent().roundRectFill(5, x-25, y-15, x+width+25, y+height+15,Graphics::makeColor(0,0,60));
    work.translucent().roundRect(5, x-25, y-15, x+width+25, y+height+15, Graphics::makeColor(0,0,20));
    
    Graphics::Bitmap temp(work, 0, y, work.getWidth(), height);
    list.setBoundaries(x-20, x+width+20);
    list.render(temp, ::Font::getDefaultFont());
    
    // Info
    const PaintownUtil::ReferenceCount<ListItem> item = list.getCurrent();
    if (!item->getInfo().empty()){
        drawInfo(160, 239, item->getInfo(), work);
    }
    
    // Fader
    if (fadeEnabled){
        fader.draw(work);
    }
}

void OptionMenu::drawInfo(int x, int y, const std::string & text, const Graphics::Bitmap & work){
    Graphics::Bitmap::transBlender(0,0,0,150);
    const int infoWidth = list.getFont().getWidth(text);
    const int infoHeight = list.getFont().getHeight();
    work.translucent().roundRectFill(5, x-(infoWidth/2) -15, y-infoHeight-1, x+(infoWidth/2)+15, y,Graphics::makeColor(0,0,60));
    work.translucent().roundRect(5, x-(infoWidth/2) -15, y-infoHeight-1, x+(infoWidth/2)+15, y, Graphics::makeColor(0,0,20));
    list.getFont().draw(x, y, text, work);
}

void OptionMenu::drawInfoWithBackground(const std::string & title, int x, int y, const std::string & text, const Graphics::Bitmap & work){
    Graphics::StretchedBitmap workArea(320, 240, work, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
    workArea.start();
    
    // Backgrounds
    background->renderBackground(0, 0, workArea);
    
    // Title
    drawInfo(160, list.getFont().getHeight() + 2, title, workArea);
    
    // Info
    drawInfo(x, y, text, workArea);
    
    // Foregrounds
    background->renderForeground(0, 0, workArea);
    
    // Fader
    if (fadeEnabled){
        fader.draw(workArea);
    }
    
    workArea.finish();
}

void OptionMenu::updateList(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & newList){
    list.clearItems();
    list.addItems(newList);
}

void OptionMenu::addItem(PaintownUtil::ReferenceCount<Gui::ScrollItem> item){
    list.addItem(item);
}

void OptionMenu::updateItem(unsigned int index, PaintownUtil::ReferenceCount<Gui::ScrollItem> item){
    list.updateItem(index, item);
}

void OptionMenu::up(){
    if (list.previous()){
        sounds.play(Move);
    }
}

void OptionMenu::down(){
    
    if (list.next()){
        sounds.play(Move);
    }
}

void OptionMenu::left(){
    if (list.valuePrevious()){
        // Play sound? not in this house!
    }
}

void OptionMenu::right(){
    if (list.valueNext()){
    }
}
   
void OptionMenu::enter(){
    if (list.getCurrent()->isRunnable()){
        sounds.play(Done);
        list.getCurrent()->run();
    }
}

void OptionMenu::cancel(){
    sounds.play(Cancel);
    fader.setState(Gui::FadeTool::FadeOut);
}

void OptionMenu::reset(){
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
    fader.setFadeInTime(10);
    fader.setFadeOutTime(10);
    list.setCurrentIndex(0);
}

bool OptionMenu::isDone(){
    return (fader.getState() == Gui::FadeTool::EndFade);
}

void OptionMenu::setFirst(){
    list.setCurrentIndex(0);
}
    
void OptionMenu::setLast(){
    list.setCurrentIndex(list.getItems().size()-1);
}


// Box
class MenuLogic: public PaintownUtil::Logic {
public:
    MenuLogic(OptionMenu & menu):
    menu(menu),
    escaped(false){
        player1Input = getPlayer1Keys(20);
        player2Input = getPlayer2Keys(20);
    }
    
    OptionMenu & menu;

    InputMap<Keys> player1Input;
    InputMap<Keys> player2Input;

    bool escaped;
    
    double ticks(double system){
    return Util::gameTicks(system);
    }

    void run(){
        InputSource input1;
        InputSource input2;
        vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input, input1);
        vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input, input2);
        out1.insert(out1.end(), out2.begin(), out2.end());
        for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
            const InputMap<Mugen::Keys>::InputEvent & event = *it;
            if (!event.enabled){
                continue;
            }


            if (event[Esc]){
                if (!escaped){
                    escaped = true;
                    menu.cancel();
                    InputManager::waitForRelease(player1Input, input1, Esc);
                    InputManager::waitForRelease(player2Input, input2, Esc);
                }
            }

            if (event[Up]){
                menu.up();
            }
            if (event[Down]){
                menu.down();
            }
            if (event[Left]){
                menu.left();
            }
            if (event[Right]){
                menu.right();
            }
            if (event[Start]){
                try {
                    menu.enter();
                } catch (const Escape::EscapeException & ex){
                    escaped = true;
                    menu.cancel();
                }
            }
        }
        
        // Act out
        menu.act();
    }

    bool done(){
        return menu.isDone();
    }
};

class MenuDraw: public PaintownUtil::Draw {
public:
    MenuDraw(OptionMenu & menu):
    menu(menu){
    }

    OptionMenu & menu;
    
    void draw(const Graphics::Bitmap & screen){
        menu.draw(screen);
        screen.BlitToScreen();
    }
};


class Motif : public BaseMenuItem {
public:
    Motif(){
        optionName = "Switch Motif";
        currentValue = "(Enter)";
    }

    virtual ~Motif(){
    }

    bool next(){
        return false;
    }

    bool previous(){
        return false;
    }
    
    bool isRunnable() const{
        return true;
    }
    
    static bool isMugenMotif(const Filesystem::AbsolutePath & path){
        try{
            string name = Util::probeDef(path, "info", "name");
            return true;
        } catch (...){
            return false;
        }
    }

    static vector<Filesystem::AbsolutePath> listMotifs(){
        Filesystem::AbsolutePath data = Storage::instance().find(Filesystem::RelativePath("mugen/data"));
        vector<Filesystem::AbsolutePath> defs = Storage::instance().getFilesRecursive(data, "system.def");
        vector<Filesystem::AbsolutePath> good;
        for (vector<Filesystem::AbsolutePath>::iterator it = defs.begin(); it != defs.end(); it++){
            const Filesystem::AbsolutePath & file = *it;
            if (isMugenMotif(file)){
                Global::debug(1) << "Motif: " << file.path() << endl;
                good.push_back(file);
            }
        }
        return good;
    }
    
    class Item : public ListItem{
    public:
        class SelectException : public std::exception{
        public:
            SelectException(){
            }
            ~SelectException() throw() {
            }
        };
        Item(){
        }
        
        bool next(){
            return false;
        }

        bool previous(){
            return false;
        }
        
        bool isRunnable() const{
            return true;
        }

        void run(){
            throw SelectException();
        }
        
        void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
            font.draw(x, y, 0, name, work);
            //font.draw(left, y, 1, name, work);
            //font.draw(right, y, -1, Storage::instance().cleanse(path).removeFirstDirectory().getDirectory().path(), work);
        }
        
        int getWidth(const ListFont & font) const {
            return (font.getWidth(name));
            //return (font.getWidth(name + "  " + Storage::instance().cleanse(path).removeFirstDirectory().getDirectory().path()));
        }
        
        const std::string & getInfo() const {
            return path.path();
        }
        
        std::string name;
        Filesystem::AbsolutePath path;
 
    };

    class MotifException : public std::exception{
    public:
        MotifException(){
        }
        ~MotifException() throw(){
        }
    };
    
    void run(){
        class Context: public Loader::LoadingContext {
        public:
            Context(){
                }

            virtual void load(){
                vector<Filesystem::AbsolutePath> paths = listMotifs();
                for (unsigned int i = 0; i < paths.size(); i++){
                    PaintownUtil::ReferenceCount<Item> motif = PaintownUtil::ReferenceCount<Item>(new Item());
                    motif->name = Mugen::Util::probeDef(paths[i], "info", "name");
                    motif->path = paths[i];
                    list.push_back(motif.convert<Gui::ScrollItem>());
                }
            }
            
            Filesystem::AbsolutePath get(unsigned int index){
                return list[index].convert<Item>()->path;
            }
            
            const std::string & getName(unsigned int index){
                return list[index].convert<Item>()->name;
            }
            
            std::string error(unsigned int index){
                return getName(index) + " at location '" + get(index).path() + "'";
            }

            std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        };

        Context state;
        /* an empty Info object, we don't really care about it */
        Loader::Info level;
        Loader::loadScreen(state, level, Loader::SimpleCircle);

        if (state.list.size() <= 1){
            return;
        }
        
        while (true){
            Filesystem::RelativePath currentMotif = Mugen::Data::getInstance().getMotif();
            try {
                //OptionMenu menu(state.list);
                OptionMenu menu(*parent);
                menu.updateList(state.list);
                menu.setName(optionName);
                menu.setRecalculateHeight(160);
                
                try {
                    // Run options
                    bool escaped = false;

                    MenuLogic logic(menu);
                    MenuDraw draw(menu);
                    PaintownUtil::standardLoop(logic, draw);
                    
                    menu.reset();
                    return;
                } catch (const Item::SelectException & ex){
                    // Got our index otherwise don't change
                    Filesystem::AbsolutePath path = state.get(menu.getSelected());
                    Filesystem::RelativePath motif = Storage::instance().cleanse(path).removeFirstDirectory();
                    
                    Global::debug(1) << "Set mugen motif to " << motif.path() << endl;
                    Mugen::Data::getInstance().setMotif(motif);
                    
                    // Test if the def is valid otherwise reset the motif
                    testMotif(path, state.error(menu.getSelected()));
                    
                    // Reload
                    throw ReloadMugenException();
                } catch (const Escape::EscapeException & ex){
                    return;
                }
            } catch (const MotifException & ex){
                Mugen::Data::getInstance().setMotif(currentMotif);
                continue;
            }
        }
    }
    
    void runError(const std::string & reason, const std::string & error){
        InputManager::waitForClear();
        PaintownUtil::showError(*Graphics::getScreenBuffer(), MugenException(reason + "\n\nPress any key to continue...", __FILE__, __LINE__), "Problem setting the selected Motif: \n" + error + "\nReason: ");
        while (!InputManager::anyInput()){
            InputManager::poll();
        }
        InputManager::waitForClear();
    }
    
    void testMotif(Filesystem::AbsolutePath & path, const std::string & errorInfo){
        try {
            Searcher searcher;
            Mugen::Menu menu(Storage::instance().cleanse(path).removeFirstDirectory(), searcher);
            //menu.loadData();
        } catch (const Mugen::Def::ParseException & e){
            runError(e.getReason(), errorInfo);
            throw MotifException();
        } catch (const MugenException & ex){
            runError(ex.getReason(), errorInfo);
            throw MotifException();
        } catch (const Filesystem::NotFound & fail){
            runError(fail.getTrace(), errorInfo);
            throw MotifException();
        } catch (const Exception::Base & fail){
            runError(fail.getTrace(), errorInfo);
            throw MotifException();
        }
    }
    
    PaintownUtil::ReferenceCount<OptionMenu> parent;
};

class DummyItem : public ListItem{
public:
    DummyItem(const std::string name):
    name(name){
    }
    virtual void run(){
    }
    
    virtual void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(x, y, name, work);
    }
    
    virtual bool next(){
        return false;
    }
    
    virtual bool previous(){
        return false;
    }
    
    virtual int getWidth(const ListFont & font) const {
        return font.getWidth(name);
    }
    std::string name;
};

static bool confirmDialog(const std::string & title, PaintownUtil::ReferenceCount<OptionMenu> parent, bool renderBackground, Graphics::Color clearColor, int clearAlpha, bool disableFade){
    class YesNo : public ListItem{
    public:
        class ResponseException : public std::exception{
        public:
            ResponseException(bool yes):
            yes(yes){
            }
            ~ResponseException() throw(){
            }
            bool getConfirmation() const{
                return yes;
            }
        private:
            bool yes;
        };
        YesNo(bool yes):
        yes(yes){
            if (yes){
                optionName = "Yes";
            } else {
                optionName = "No";
            }
        }
        
        virtual ~YesNo(){
        }
        
        virtual void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
            font.draw(x, y, optionName, work);
        }
        
        virtual int getWidth(const ListFont & font) const {
            return font.getWidth(optionName);
        }
        
        bool next(){
            return false;
        }
        
        bool previous(){
            return false;
        }
        
        virtual bool isRunnable() const {
            return true;
        }
        
        virtual void run(){
            throw ResponseException(yes);
        }
        bool yes;
        std::string optionName;
    };
    try {
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new YesNo(true)));
        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new YesNo(false)));
        OptionMenu menu(*parent);
        menu.updateList(list);
        menu.setName(title);
        menu.setRenderBackground(renderBackground);
        menu.setClearColor(clearColor);
        menu.setClearAlpha(clearAlpha);
        if (disableFade){
            menu.setFadeEnabled(false);
        }
        MenuLogic logic(menu);
        MenuDraw draw(menu);
        PaintownUtil::standardLoop(logic, draw);
    } catch (const YesNo::ResponseException & ex){
        return ex.getConfirmation();
    } catch (const Escape::EscapeException & ex){
    }
    return false;
}

class PlayerKeys : public BaseMenuItem{
public:
    PlayerKeys(int player, const std::string & playerName):
    player(player),
    renderBackground(true),
    clearColor(Graphics::makeColor(0,0,0)),
    clearAlpha(128),
    disableFade(false),
    throwable(false){
        currentValue = "(Enter)";
        optionName = playerName;
    }
    virtual ~PlayerKeys(){
    }
    
    virtual bool isRunnable() const{
        return true;
    }
    
    virtual void run(){
        class ChangeKeys : public BaseMenuItem{
        public:
            ChangeKeys(PaintownUtil::ReferenceCount<OptionMenu> parent, int player, std::string playerName, bool renderBackground, Graphics::Color clearColor, int clearAlpha, bool disableFade, bool throwable):
            parent(parent),
            player(player),
            playerName(playerName),
            renderBackground(renderBackground),
            clearColor(clearColor),
            clearAlpha(clearAlpha),
            disableFade(disableFade),
            throwable(throwable){
                optionName = "Update/Change Keys";
                currentValue = "(Enter)";
            }
            virtual ~ChangeKeys(){
            }
            bool next(){
                return false;
            }
            
            bool previous(){
                return false;
            }
            
            virtual bool isRunnable() const {
                return true;
            }
            
            virtual void run(){
                class Key : public BaseMenuItem{
                public:
                    Key(const std::string & name, const std::string & key){
                        optionName = name;
                        currentValue = key;
                    }
                    
                    bool next(){
                        return false;
                    }
                    
                    bool previous(){
                        return false;
                    }
                };
                class Logic: public PaintownUtil::Logic {
                public:
                    Logic(OptionMenu & menu, int player, const std::string & playerName, bool throwable):
                    menu(menu),
                    player(player),
                    playerName(playerName),
                    escaped(false),
                    changingKeys(false),
                    currentValue(0),
                    scrollEnd(false),
                    throwable(throwable){
                        player1Input = getPlayer1Keys(20);
                        player2Input = getPlayer2Keys(20);
                        keyValues.push_back("Jump");
                        keyMap.push_back(Mugen::Up);
                        keyValues.push_back("Crouch");
                        keyMap.push_back(Mugen::Down);
                        keyValues.push_back("Left");
                        keyMap.push_back(Mugen::Left);
                        keyValues.push_back("Right");
                        keyMap.push_back(Mugen::Right);
                        keyValues.push_back("A");
                        keyMap.push_back(Mugen::A);
                        keyValues.push_back("B");
                        keyMap.push_back(Mugen::B);
                        keyValues.push_back("C");
                        keyMap.push_back(Mugen::C);
                        keyValues.push_back("X");
                        keyMap.push_back(Mugen::X);
                        keyValues.push_back("Y");
                        keyMap.push_back(Mugen::Y);
                        keyValues.push_back("Z");
                        keyMap.push_back(Mugen::Z);
                        //keyValues.push_back("Start");
                        //keyMap.push_back(Mugen::Start);
                        //keyValues.push_back("Quit");
                        //keyMap.push_back(Mugen::Quit);
                    }
                    
                    OptionMenu & menu;
                    int player;
                    const std::string & playerName;

                    InputMap<Keys> player1Input;
                    InputMap<Keys> player2Input;

                    bool escaped;
                    bool changingKeys;
                    unsigned int currentValue;
                    bool scrollEnd;
                    bool throwable;
                    std::vector< std::string > keyValues;
                    std::vector< Mugen::Keys > keyMap;
                    
                    static std::string getKeyName(int player, const Mugen::Keys & key){
                        switch(key){
                            case Mugen::Up:
                                return Keyboard::keyToName(::Configuration::config( player ).getUp());
                            case Mugen::Down:
                                return Keyboard::keyToName(::Configuration::config( player ).getDown());
                            case Mugen::Left:
                                return Keyboard::keyToName(::Configuration::config( player ).getLeft());
                            case Mugen::Right:
                                return Keyboard::keyToName(::Configuration::config( player ).getRight());
                            case Mugen::A:
                                return Keyboard::keyToName(::Configuration::config( player ).getAttack1());
                            case Mugen::B:
                                return Keyboard::keyToName(::Configuration::config( player ).getAttack2());
                            case Mugen::C:
                                return Keyboard::keyToName(::Configuration::config( player ).getAttack3());
                            case Mugen::X:
                                return Keyboard::keyToName(::Configuration::config(player).getAttack4());
                            case Mugen::Y:
                                return Keyboard::keyToName(::Configuration::config(player).getAttack5());
                            case Mugen::Z:
                                return Keyboard::keyToName(::Configuration::config(player).getAttack6());
                            case Mugen::Start:
                            case Mugen::Enter:
                            case Mugen::Esc:
                            default:
                                break;
                        }

                        return 0;
                    }
                    
                    static std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > getList(int player){
                        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Jump", getKeyName(player, Mugen::Up))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Crouch", getKeyName(player, Mugen::Down))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Left", getKeyName(player, Mugen::Left))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Right", getKeyName(player, Mugen::Right))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("A", getKeyName(player, Mugen::A))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("B", getKeyName(player, Mugen::B))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("C", getKeyName(player, Mugen::C))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("X", getKeyName(player, Mugen::X))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Y", getKeyName(player, Mugen::Y))));
                        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Z", getKeyName(player, Mugen::Z))));
                        //list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Key("Start", getKeyName(player, Mugen::Start))));
                        //list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new ResetDefault(player)));
                        return list;
                    }
                    
                    double ticks(double system){
                        return Util::gameTicks(system);
                    }
                    
                    void setKey(int player, const Mugen::Keys & key, int code){
                        switch(key){
                            case Mugen::Up:
                                ::Configuration::config(player).setUp(code);
                                break;
                            case Mugen::Down:
                                ::Configuration::config(player).setDown(code);
                                break;
                            case Mugen::Left:
                                ::Configuration::config(player).setLeft(code);
                                break;
                            case Mugen::Right:
                                ::Configuration::config(player).setRight(code);
                                break;
                            case Mugen::A:
                                ::Configuration::config(player).setAttack1(code);
                                break;
                            case Mugen::B:
                                ::Configuration::config(player).setAttack2(code);
                                break;
                            case Mugen::C:
                                ::Configuration::config(player).setAttack3(code);
                                break;
                            case Mugen::X:
                                ::Configuration::config(player).setAttack4(code);
                                break;
                            case Mugen::Y:
                                ::Configuration::config(player).setAttack5(code);
                                break;
                            case Mugen::Z:
                                ::Configuration::config(player).setAttack6(code);
                                break;
                            case Mugen::Enter:
                            case Mugen::Start:
                            case Mugen::Esc:
                            default:
                                break;
                        }
                    }

                    void run(){
                        if (!changingKeys){
                            InputSource input1;
                            InputSource input2;
                            vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input, input1);
                            vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input, input2);
                            out1.insert(out1.end(), out2.begin(), out2.end());
                            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                                if (!event.enabled){
                                    continue;
                                }
                                
                                if (event[Esc]){
                                    if (!escaped){
                                        escaped = true;
                                        menu.cancel();
                                        InputManager::waitForRelease(player1Input, input1, Esc);
                                        InputManager::waitForRelease(player2Input, input2, Esc);
                                    }
                                }
                                if (event[Start]){
                                    menu.setFirst();
                                    changingKeys = true;
                                    InputManager::waitForClear();
                                    menu.toggleCursor();
                                    menu.setName("Update " + playerName + " Keys");
                                }
                                if (event[Up] || event[Down]){
                                    if (!changingKeys){
                                        scrollEnd = !scrollEnd;
                                        if (scrollEnd){
                                            menu.setLast();
                                        } else {
                                            menu.setFirst();
                                        }
                                    }
                                }
                            }
                        } else {
                            InputManager::poll();
                            if (InputManager::anyInput()){
                                setKey(player, keyMap[currentValue], InputManager::readKey());
                                menu.updateItem(currentValue, getList(player)[currentValue].convert<Gui::ScrollItem>());
                                InputManager::waitForClear();
                                if (currentValue < keyValues.size()-1){
                                    currentValue++;
                                    menu.down();
                                } else {
                                    changingKeys = false;
                                    currentValue = 0;
                                    menu.setFirst();
                                    menu.toggleCursor();
                                    menu.setName(playerName + " Keys (Enter to change)");
                                    // Lets throw an exception if it is requested
                                    if (throwable){
                                        throw OptionMenu::KeysChangedException( player == 0 ? Mugen::Player1 : Mugen::Player2 );
                                    }
                                }
                            }
                        }
                        
                        // Act out
                        menu.act();
                        
                        if (menu.isDone()){
                            throw Escape::EscapeException();
                        }
                    }

                    bool done(){
                        return menu.isDone();
                    }
                };
                
                class Draw: public PaintownUtil::Draw {
                public:
                    Draw(OptionMenu & menu, Logic & logic):
                    menu(menu),
                    logic(logic){
                    }

                    OptionMenu & menu;
                    Logic & logic;
                    
                    void draw(const Graphics::Bitmap & screen){
                        menu.draw(screen);
                        if (logic.changingKeys){
                            menu.drawInfo(screen.getWidth()/2, screen.getHeight()/4, "Press new key for " + logic.keyValues[logic.currentValue], screen );
                        }
                        screen.BlitToScreen();
                    }
                };
                
                while (true){
                    try {
                        //OptionMenu menu(Logic::getList(player));
                        OptionMenu menu(*parent);
                        menu.updateList(Logic::getList(player));
                        menu.setName(playerName + " Keys (Enter to change)");
                        menu.toggleCursor();
                        menu.setRenderBackground(renderBackground);
                        menu.setClearColor(clearColor);
                        menu.setClearAlpha(clearAlpha);
                        if (disableFade){
                            menu.setFadeEnabled(false);
                        }
                        Logic logic(menu, player, playerName, throwable);
                        Draw draw(menu,logic);
                        PaintownUtil::standardLoop(logic, draw);
                    } catch (const Escape::EscapeException & ex){
                        break;
                    }
                }
            }
            
            PaintownUtil::ReferenceCount<OptionMenu> parent;
            int player;
            std::string playerName;
            bool renderBackground;
            Graphics::Color clearColor;
            int clearAlpha;
            bool disableFade;
            bool throwable;
        };
        
        class ResetDefault : public BaseMenuItem{
        public:
            ResetDefault(PaintownUtil::ReferenceCount<OptionMenu> parent, int player, bool renderBackground, Graphics::Color clearColor, int clearAlpha, bool disableFade, bool throwable):
            parent(parent),
            player(player),
            renderBackground(renderBackground),
            clearColor(clearColor),
            clearAlpha(clearAlpha),
            disableFade(disableFade),
            throwable(throwable){
                optionName = "Default Values";
                currentValue = "(Enter)";
            }
            
            virtual ~ResetDefault(){
            }
            
            bool next(){
                return false;
            }
            
            bool previous(){
                return false;
            }
            
            virtual bool isRunnable() const {
                return true;
            }
            
            virtual void run(){
                if (confirmDialog("Reset to default?", parent, renderBackground, clearColor, clearAlpha, disableFade)){
                    ::Configuration::setDefaultKeys(player);
                    ::Configuration::saveConfiguration();
                    if (throwable){
                        throw OptionMenu::KeysChangedException( player == 0 ? Mugen::Player1 : Mugen::Player2 );
                    }
                }
            }
            
            PaintownUtil::ReferenceCount<OptionMenu> parent;
            int player;
            bool renderBackground;
            Graphics::Color clearColor;
            int clearAlpha;
            bool disableFade;
            bool throwable;
        };
        try {
            
            std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
            list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new ChangeKeys(parent, player, optionName, renderBackground, clearColor, clearAlpha, disableFade, throwable)));
            list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new ResetDefault(parent, player, renderBackground, clearColor, clearAlpha, disableFade, throwable)));
            OptionMenu menu(*parent);
            menu.updateList(list);
            menu.setName(optionName + " Key Configuration");
            menu.setRenderBackground(renderBackground);
            menu.setClearColor(clearColor);
            menu.setClearAlpha(clearAlpha);
            if (disableFade){
                menu.setFadeEnabled(false);
            }
            
            MenuLogic logic(menu);
            MenuDraw draw(menu);
            PaintownUtil::standardLoop(logic, draw);
            
        } catch (const Escape::EscapeException & ex){
            // Ok no prob
        }
    }
    
    virtual void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        if (displayInfo){
            font.draw(x, y, 0, optionName + " Key Config", work);
        } else {
            font.draw(left, y, 1, optionName + " Key Config", work);
            font.draw(right, y, -1, currentValue, work);
        }
    }
    
    virtual int getWidth(const ListFont & font) const {
        return (font.getWidth(optionName + " Key Config  " + currentValue));
    }
    
    virtual bool next(){
        return false;
    }
    
    virtual bool previous(){
        return false;
    }
    
    int player;
    bool renderBackground;
    Graphics::Color clearColor;
    int clearAlpha;
    bool disableFade;
    bool throwable;
    PaintownUtil::ReferenceCount<OptionMenu> parent;
};

PaintownUtil::ReferenceCount<Gui::ScrollItem> OptionMenu::getPlayerKeys(int player, const std::string & text, PaintownUtil::ReferenceCount<OptionMenu> menu){
    PaintownUtil::ReferenceCount<PlayerKeys> playerKeys = PaintownUtil::ReferenceCount<PlayerKeys>(new PlayerKeys(player, text));
    playerKeys->renderBackground = false;
    playerKeys->clearAlpha = 128;
    playerKeys->throwable = true;
    playerKeys->parent = menu;
    playerKeys->disableFade = true;
    return playerKeys.convert<Gui::ScrollItem>();
}

OptionMenu::KeysChangedException::KeysChangedException(const Mugen::PlayerType & type):
type(type){
}

OptionMenu::KeysChangedException::~KeysChangedException() throw(){
}

void OptionsMenu::run(const std::string & name){
    // Add options
    std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::Difficulty()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::Life()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::TimeLimit()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::Speed()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::OneVsTeam()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::TeamLoseOnKO()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::AutoSearch()));
    PaintownUtil::ReferenceCount<Mugen::Motif> motif = PaintownUtil::ReferenceCount<Mugen::Motif>(new Mugen::Motif());
    list.push_back(motif.convert<Gui::ScrollItem>());
    PaintownUtil::ReferenceCount<Mugen::PlayerKeys> player1Keys = PaintownUtil::ReferenceCount<Mugen::PlayerKeys>(new Mugen::PlayerKeys(0, "Player1"));
    PaintownUtil::ReferenceCount<Mugen::PlayerKeys> player2Keys = PaintownUtil::ReferenceCount<Mugen::PlayerKeys>(new Mugen::PlayerKeys(1, "Player2"));
    list.push_back(player1Keys.convert<Gui::ScrollItem>());
    list.push_back(player2Keys.convert<Gui::ScrollItem>());
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Mugen::Escape()));
    /* Testing 
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new DummyItem("Dummy")));
    */
    
    PaintownUtil::ReferenceCount<Mugen::OptionMenu> optionMenu = PaintownUtil::ReferenceCount<Mugen::OptionMenu>(new Mugen::OptionMenu(list));
    
    // Setup parents so that we can borrow resources
    motif->parent = player1Keys->parent = player2Keys->parent = optionMenu;
    
    optionMenu->setName(name);
    
    Mugen::MenuLogic logic(*optionMenu);
    Mugen::MenuDraw draw(*optionMenu);
    PaintownUtil::standardLoop(logic, draw);
    
    optionMenu->reset();

    // **FIXME Hack figure something out
    throw Exception::Return(__FILE__, __LINE__);
}

OptionFactory::OptionFactory(){
}

/*! Handles sub menus */
class OptionMugenMenu: public MenuOption {
public:
    OptionMugenMenu(const Gui::ContextBox & parent, const Token *token):
    MenuOption(parent, token){
        if ( *token != "mugen" ){
            throw LoadException(__FILE__, __LINE__, "Not a mugen motif menu");
        }

        TokenView view = token->view();
        while (view.hasMore()){
            try{
                const Token * tok;
                view >> tok;
                if ( *tok == "name" ){
                    // Create an image and push it back on to vector
                    std::string temp;
                    tok->view() >> temp;
                    this->setText(temp);
                } else {
                    Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                    if (Global::getDebug() >= 3){
                        tok->print(" ");
                    }
                }
            } catch ( const TokenException & ex ) {
                throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
            } 
        }
    }
    
    virtual void loadMotif(){
        std::string motif;
        try {
            *Mugen::Configuration::get("motif") >> motif;
        } catch (const std::ios_base::failure & ex){
            motif.clear();
        }
        if (!motif.empty()){
            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(motif));
        } else {
            /* FIXME: search for a system.def file */
            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath("data/system.def"));
        }
    }

    // Do logic before run part
    virtual void logic(){
    }

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const ::Menu::Context & context){
        try{
            loadMotif();
            Mugen::run();
        } catch (const LoadException & le){
            ostringstream out;
            out << "Press ENTER to continue\n";
            out << "\n";
            out << "We are very sorry but an error has occured while trying to load MUGEN.";
            PaintownUtil::showError(le, out.str());
            InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
        }
        throw ::Menu::Reload(__FILE__, __LINE__);
        // throw Exception::Return(__FILE__, __LINE__);
    }

    virtual ~OptionMugenMenu(){
    }

private:
    // MugenMenu *_menu;
};

MenuOption * OptionFactory::getOption(const Gui::ContextBox & parent, const Token *token) const {
    const Token * child;
    token->view() >> child;

    if (*child == "mugen"){
        return new OptionMugenMenu(parent, child);
    }

    return NULL;
}

OptionFactory::~OptionFactory(){
}

}
