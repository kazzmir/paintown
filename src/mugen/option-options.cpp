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
                        //const int width = item->getWidth(useFont)/2 + 5;
                        const int height = useFont.getHeight()/2 + 5;
                        work.translucent().rectangleFill(leftBoundary-2, y - useFont.getHeight(), rightBoundary+2, y, Graphics::makeColor(255,255,255));
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

void ScrollAction::addItem(const PaintownUtil::ReferenceCount<Gui::ScrollItem> & item){
    text.push_back(item);
}

void ScrollAction::addItems(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & items){
    text.insert(text.end(), items.begin(), items.end());
}

const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & ScrollAction::getItems() const{
    return text;
}

void ScrollAction::clearItems(){
}

unsigned int ScrollAction::getCurrentIndex() const{
    return current;
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
        itemBottom = getVisibleItems();
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
    int visible = (height/(spacingY)) + 1;
    setVisibleItems(visible);
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

class Difficulty: public ListItem {
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
    
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
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
    
    std::string optionName;
    std::string currentValue;
};

class Life : public ListItem {
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
    
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

class TimeLimit : public ListItem {
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

    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

class Speed : public ListItem {
    public:
	Speed(){
	    optionName = "Speed";
            int speed = Data::getInstance().getSpeed();
            if (speed < -9){
                speed = -9;
                Data::getInstance().setSpeed(speed);
            } else if (speed > 9){
                speed = 9;
                Data::getInstance().setSpeed(speed);
            }
	    currentValue = getSpeedName(speed);
	}
	~Speed(){
	}
	bool next(){
            int speed = Data::getInstance().getSpeed() + 1;
            if (speed > 9){
                speed = 9;
            }
            Data::getInstance().setSpeed(speed);
	    currentValue = getSpeedName(speed);
        return true;
	}
	bool previous(){
            int speed = Data::getInstance().getSpeed() - 1;
            if (speed < -9){
                speed = -9;
            }
            Data::getInstance().setSpeed(speed);
	    currentValue = getSpeedName(speed);
        return true;
	}

        std::string getSpeedName(int speed){
            if (speed == 0){
                return "Normal";
            } else if (speed < 0){
                return "Slow " + getString(abs(speed));
            } else if (speed > 0){
                return "Fast " + getString(speed);
            }
            return std::string();
        }
     
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

class OneVsTeam : public ListItem {
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

    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

class TeamLoseOnKO : public ListItem {
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
	
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

class AutoSearch : public ListItem {
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
                currentValue = "select.def+auto";
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
    
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  select.def+auto"));
    }
    
    std::string optionName;
    std::string currentValue;
};

class Escape: public ListItem {
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
    
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

OptionMenu::OptionMenu(const std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > & items){
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
        if (ourFont != NULL && (ourFont->getHeight() >= 8 && ourFont->getHeight() < 15)){
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
    //list.setExpandState(ScrollAction::Expand);
}

OptionMenu::~OptionMenu(){
}

void OptionMenu::act(){
    background->act();
    fader.act();
    list.act();
    list.recalculateVisibleItems(180);
}

void OptionMenu::draw(const Graphics::Bitmap & work){
    Graphics::StretchedBitmap workArea(320, 240, work, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
    workArea.start();
    
    // Backgrounds
    background->renderBackground(0, 0, workArea);
    
    const int width = list.getMaxWidth();
    const int height = list.getMaxHeight();// + list.getTopMargin() + list.getBottomMargin();
    const int x = 160 - width/2;
    const int y = 120 - height/2;
    
    list.getFont().draw(160, 15, name, workArea);
    
    Graphics::Bitmap::transBlender(0,0,0,150);
    workArea.translucent().roundRectFill(5, x-25, y-15, x+width+25, y+height+15,Graphics::makeColor(0,0,60));
    workArea.translucent().roundRect(5, x-25, y-15, x+width+25, y+height+15, Graphics::makeColor(0,0,20));
    
    Graphics::Bitmap temp(workArea, 0, y, work.getWidth(), height);
    list.setBoundaries(x-20, x+width+20);
    list.render(temp, ::Font::getDefaultFont());
    
    // Foregrounds
    background->renderForeground(0, 0, workArea);
    
    // Fader
    fader.draw(workArea);
    
    workArea.finish();
    
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
}

bool OptionMenu::isDone(){
    return (fader.getState() == Gui::FadeTool::EndFade);
}

// Box
class MenuLogic: public PaintownUtil::Logic {
public:
    MenuLogic(OptionMenu & menu, bool & escaped):
    menu(menu),
    escaped(escaped){
        player1Input = getPlayer1Keys(20);
        player2Input = getPlayer2Keys(20);
    }
    
    OptionMenu & menu;

    InputMap<Keys> player1Input;
    InputMap<Keys> player2Input;

    bool & escaped;
    
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


class Motif : public ListItem {
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
        }
        
        int getWidth(const ListFont & font){
            return (font.getWidth(name));
        }
        
        std::string name;
        Filesystem::AbsolutePath path;
 
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

            std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
        };

        Context state;
        /* an empty Info object, we don't really care about it */
        Loader::Info level;
        Loader::loadScreen(state, level, Loader::SimpleCircle);

        if (state.list.size() <= 1){
            return;
        }
        
        OptionMenu menu(state.list);
        menu.setName(optionName);
        
        try {
            // Run options
            bool escaped = false;

            MenuLogic logic(menu, escaped);
            MenuDraw draw(menu);
            PaintownUtil::standardLoop(logic, draw);
            
            menu.reset();
        } catch (const Item::SelectException & ex){
            // Got our index otherwise don't change
            Filesystem::RelativePath motif = Storage::instance().cleanse(state.get(menu.getSelected())).removeFirstDirectory();
            Global::debug(1) << "Set mugen motif to " << motif.path() << endl;
            ::Configuration::setMugenMotif(motif.path());
            Mugen::Data::getInstance().setMotif(motif);
        } catch (const Escape::EscapeException & ex){
            return;
        }
    }
    
    void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const{
        font.draw(left, y, 1, optionName, work);
        font.draw(right, y, -1, currentValue, work);
    }
    
    int getWidth(const ListFont & font){
        return (font.getWidth(optionName + "  " + currentValue));
    }
    
    std::string optionName;
    std::string currentValue;
};

OptionOptions::OptionOptions( const std::string &name ){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to Options");
    }
    this->setText(name);
    
    // Add options
    std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Difficulty()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Life()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new TimeLimit()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Speed()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new OneVsTeam()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new TeamLoseOnKO()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new AutoSearch()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Motif()));
    list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Escape()));
    
    optionMenu = PaintownUtil::ReferenceCount<OptionMenu>(new OptionMenu(list));
    optionMenu->setName(name);
}

OptionOptions::~OptionOptions(){
}

void OptionOptions::executeOption(const PlayerType & player, bool &endGame){
    
    // Run options
    bool escaped = false;

    MenuLogic logic(*optionMenu, escaped);
    MenuDraw draw(*optionMenu);
    PaintownUtil::standardLoop(logic, draw);
    
    optionMenu->reset();

    // **FIXME Hack figure something out
    if (escaped){
        throw Exception::Return(__FILE__, __LINE__);
    }
}

OptionArcade::OptionArcade(const string & name){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to versus");
    }

    this->setText(name);
}

OptionArcade::~OptionArcade(){
	// Nothing
}

/*
static void runGame(const PlayerType & player, GameType kind, Searcher & searcher){
    Game versus(player, kind, Data::getInstance().getFileFromMotif(Data::getInstance().getMotif()));
    versus.run(searcher);
}
*/

void OptionArcade::executeOption(const Mugen::PlayerType & player, bool &endGame){
    /* Get default motif system.def */
    /*
    Game versus(player, Arcade, Data::getInstance().getFileFromMotif(Data::getInstance().getMotif()));
    versus.run();
    */
    // runGame(player, Arcade, searcher);
    throw StartGame(player, Arcade);
}

OptionVersus::OptionVersus(const std::string &name){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to versus");
    }
    this->setText(name);
}

OptionVersus::~OptionVersus(){
	// Nothing
}

void OptionVersus::executeOption(const Mugen::PlayerType & player, bool &endGame){
    throw StartGame(player, Versus);
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
                } else if (*tok == "motif"){
                    // Load Motif from file
                    std::string temp;
                    // Filename
                    tok->view() >> temp;
                    // Set the default motif
                    try{
                        if (::Configuration::getMugenMotif() == "default"){
                            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(temp));
                        } else {
                            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(::Configuration::getMugenMotif()));
                        }
                    } catch (const Filesystem::NotFound & fail){
                        throw LoadException(__FILE__, __LINE__, fail, "Can't load the MUGEN menu");
                    } catch (const MugenException & fail){
                        throw LoadException(__FILE__, __LINE__, fail, "Can't load the MUGEN menu");
                    }
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

        /*
        // Load menu with default motif
        _menu = new MugenMenu(Mugen::Data::getInstance().getMotif());
        // Set this menu as an option
        _menu->setAsOption(true);
        */

        // Lets check if this menu is going bye bye
        //if ( _menu->checkRemoval() ) setForRemoval(true);
    }

    // Do logic before run part
    virtual void logic(){
    }

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context & context){
        try{
            Mugen::run();
        } catch (const LoadException & le){
            ostringstream out;
            out << "Press ENTER to continue\n";
            out << "\n";
            out << "We are very sorry but an error has occured while trying to load MUGEN.";
            PaintownUtil::showError(le, out.str());
            InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
        }
        throw Menu::Reload(__FILE__, __LINE__);
        // throw Exception::Return(__FILE__, __LINE__);
    }

    virtual ~OptionMugenMenu(){
    }

private:
    // MugenMenu *_menu;
};

class OptionMugenMotif: public MenuOption {
public:
    OptionMugenMotif(const Gui::ContextBox & parent, const Token * token):
    MenuOption(parent, token){
        readName(token);
    }

    virtual ~OptionMugenMotif(){
    }

    virtual void logic(){
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

    virtual void run(const Menu::Context & context){
        class Context: public Loader::LoadingContext {
        public:
            Context():
                index(-1){
                }

            virtual void load(){
                paths = listMotifs();
                vector<Filesystem::AbsolutePath> paths = listMotifs();
                const Gui::ContextBox & box = ((Menu::DefaultRenderer*) menu.getRenderer())->getBox();
                for (unsigned int i = 0; i < paths.size(); i++){
                    /* FIXME: its a hack/lazy to use OptionLevel here */
                    OptionLevel *option = new OptionLevel(box, 0, &index, i);
                    option->setText(Mugen::Util::probeDef(paths[i], "info", "name"));
                    option->setInfoText(Storage::instance().cleanse(paths[i]).path());
                    menu.addOption(option);
                }
            }

            vector<Filesystem::AbsolutePath> paths;
            int index;
            Menu::Menu menu;
        };

        Context state;
        /* an empty Info object, we don't really care about it */
        Loader::Info level;
        Loader::loadScreen(state, level, Loader::SimpleCircle);

        if (state.paths.size() <= 1){
            return;
        }

        try {
            state.menu.run(context);
        } catch (const Menu::MenuException & ex){
        } catch (const Exception::Return & ok){
        }

        if (state.index != -1){
            Filesystem::RelativePath motif = Storage::instance().cleanse(state.paths[state.index]).removeFirstDirectory();
            Global::debug(1) << "Set muge motif to " << motif.path() << endl;
            ::Configuration::setMugenMotif(motif.path());
            Mugen::Data::getInstance().setMotif(motif);
        }
    }
};

MenuOption * OptionFactory::getOption(const Gui::ContextBox & parent, const Token *token) const {
    const Token * child;
    token->view() >> child;

    if (*child == "mugen"){
        return new OptionMugenMenu(parent, child);
    } else if (*child == "mugen-motif"){
        return new OptionMugenMotif(parent, child);
    }

    return NULL;
}

OptionFactory::~OptionFactory(){
}

}
