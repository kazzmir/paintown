#include "r-tech1/graphics/bitmap.h"

#include "r-tech1/menu/options.h"
#include "r-tech1/token.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/parameter.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/menu/menu.h"
#include "r-tech1/configuration.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/menu/menu-exception.h"
#include "r-tech1/init.h"
#include "r-tech1/events.h"
#include "r-tech1/version.h"
#include "r-tech1/menu/optionfactory.h"

#include "r-tech1/sound/music.h"

#include "r-tech1/input/keyboard.h"
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/system.h"
#include "r-tech1/font_factory.h"
#include "r-tech1/exceptions/shutdown_exception.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/font.h"
#include "r-tech1/gui/box.h"
#include "r-tech1/thread.h"

#include "r-tech1/loading.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/input/input-manager.h"

#include <sstream>
#include <algorithm>
#include <time.h>
#include <math.h>

using namespace std;
using namespace Gui;

/* true if the arguments passed in match todays date.
 * pass 0 for any argument that you don't care about (it will match any date)
 */
static bool todaysDate(int month, int day, int year){
    time_t result = time(NULL);
    struct tm * local = localtime(&result);
    return (month == 0 || month == (local->tm_mon + 1)) &&
           (day == 0 || day == local->tm_mday) &&
           (year == 0 || year == local->tm_year + 1900);
}

static bool jonBirthday(){
    return todaysDate(3, 25, 0);
}

static bool miguelBirthday(){
    return todaysDate(8, 11, 0);
}

OptionCredits::Block::Block(const std::string & title):
title(title),
titleColorOverride(false),
titleColor(Graphics::makeColor(0,255,255)),
colorOverride(false),
color(Graphics::makeColor(255,255,255)),
spacing(0){
}

OptionCredits::Block::Block(const Token * token):
titleColorOverride(false),
titleColor(Graphics::makeColor(0,255,255)),
colorOverride(false),
color(Graphics::makeColor(255,255,255)),
topWidth(0),
topHeight(0),
bottomWidth(0),
bottomHeight(0),
spacing(0){
    if ( *token != "block" ){
        throw LoadException(__FILE__, __LINE__, "Not a credit block");
    }
    
    TokenView view = token->view();
    
    while (view.hasMore()){
        std::string match;
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "title" ) {
                tok->view() >> title;
            } else if (*tok == "credit"){
                std::string credit;
                tok->view() >> credit;
                credits.push_back(credit);
            } else if ( *tok == "titlecolor" ) {
                try{
                    int r,b,g;
                    tok->view() >> r >> g >> b;
                    titleColor = Graphics::makeColor( r, g, b );
                    titleColorOverride = true;
                } catch (const TokenException & ex){
                }
            } else if ( *tok == "color" ) {
                try{
                    int r,b,g;
                    tok->view() >> r >> g >> b;
                    color = Graphics::makeColor( r, g, b );
                    colorOverride = true;
                } catch (const TokenException & ex){
                }
            } else if ( *tok == "animation" ) {
                TokenView animView = tok->view();
                while (animView.hasMore()){
                    const Token * animTok;
                    animView >> animTok;
                    if (*animTok == "top"){
                        tok->match("_/width", topWidth);
                        tok->match("_/height", topHeight);
                        topAnimation = Util::ReferenceCount<Gui::Animation>(new Animation(tok));
                    } else if (*animTok == "bottom"){
                        tok->match("_/width", bottomWidth);
                        tok->match("_/height", bottomHeight);
                        bottomAnimation = Util::ReferenceCount<Gui::Animation>(new Animation(tok));
                    }
                }
            } else if (*tok == "spacing"){
                tok->view() >> spacing;
            } else {
                Global::debug( 3 ) <<"Unhandled Credit Block attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Credit Block parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
}

OptionCredits::Block::Block(const OptionCredits::Block & copy):
title(copy.title),
credits(copy.credits),
titleColorOverride(copy.titleColorOverride),
titleColor(copy.titleColor),
colorOverride(copy.colorOverride),
color(copy.color),
topAnimation(copy.topAnimation),
topWidth(copy.topWidth),
topHeight(copy.topHeight),
bottomAnimation(copy.bottomAnimation),
bottomWidth(copy.bottomWidth),
bottomHeight(copy.bottomHeight),
spacing(copy.spacing){
}

OptionCredits::Block::~Block(){
}

const OptionCredits::Block & OptionCredits::Block::operator=(const OptionCredits::Block & copy){
    title = copy.title;
    credits = copy.credits;
    titleColor = copy.titleColor;
    titleColorOverride = copy.titleColorOverride;
    color = copy.color;
    colorOverride = copy.colorOverride;
    topAnimation = copy.topAnimation;
    topWidth =copy.topWidth;
    topHeight = copy.topHeight;
    bottomAnimation = copy.bottomAnimation;
    bottomWidth = copy.bottomWidth;
    bottomHeight = copy.bottomHeight;
    spacing = copy.spacing;
    return *this;
}

void OptionCredits::Block::addCredit(const std::string & credit){
    credits.push_back(credit);
}

void OptionCredits::Block::act(){
    // Top animation
    if (topAnimation != NULL){
        topAnimation->act();
    }
    // Bottom animation
    if (bottomAnimation != NULL){
        bottomAnimation->act();
    }
}

int OptionCredits::Block::print(int x, int y, Graphics::Color defaultTitleColor, Graphics::Color defaultColor, const Font & font, const Graphics::Bitmap & work, const Justification & justification) const {
    int currentY = y;
    
    // Top animation
    if (topAnimation != NULL){
        int xmod = 0;
        switch (justification){
            default:
            case Left:
                xmod = 0;
                break;
            case Center:
                xmod = topWidth/2;
                break;
            case Right:
                xmod = topWidth;
                break;
        }
        // FIXME temporary solution
        const Graphics::Bitmap temp(topWidth, topHeight);
        //topAnimation->draw(x - xmod, y, topWidth, topHeight, work);
        topAnimation->draw(0, 0, topWidth, topHeight, temp);
        /* FIXME: what should alpha be here? */
        temp.translucent(128).draw(x-xmod, y, work);
        currentY += topHeight;
    }
    
    if (!title.empty()){
        int xmod = 0;
        switch (justification){
            default:
            case Left:
                xmod = 0;
                break;
            case Center:
                xmod = font.textLength(title.c_str())/2;
                break;
            case Right:
                xmod = font.textLength(title.c_str());
                break;
        }
        font.printf(x - xmod, currentY, (titleColorOverride ? titleColor : defaultTitleColor), work, title, 0);
        currentY += font.getHeight();
    }
    
    for (std::vector<std::string>::const_iterator i = credits.begin(); i != credits.end(); ++i){
        const std::string & credit = *i;
        int xmod = 0;
        switch (justification){
            default:
            case Left:
                xmod = 0;
                break;
            case Center:
                xmod = font.textLength(credit.c_str())/2;
                break;
            case Right:
                xmod = font.textLength(credit.c_str());
                break;
        }
        font.printf(x - xmod, currentY, (colorOverride ? color : defaultColor), work, credit, 0);
        currentY += font.getHeight();
    }
    
    // Bottom animation
    if (bottomAnimation != NULL){
        int xmod = 0;
        switch (justification){
            default:
            case Left:
                xmod = 0;
                break;
            case Center:
                xmod = bottomWidth/2;
                break;
            case Right:
                xmod = bottomWidth;
                break;
        }
        // FIXME temporary solution
        const Graphics::Bitmap temp(topWidth, topHeight);
        //bottomAnimation->draw(x - xmod, y, bottomWidth, bottomHeight, work);
        bottomAnimation->draw(0, 0, bottomWidth, bottomHeight, temp);
        /* FIXME: what should alpha be here? */
        temp.translucent(128).draw(x-xmod, y, work);
        currentY += bottomHeight;
    }
    
    currentY += font.getHeight() + spacing;
    
    return currentY;
}

int OptionCredits::Block::size(const Font & font) const {
    // Counts title and space in between
    int total = 0;
    if (topAnimation != NULL){
        total += topHeight;
    }
    if (!title.empty()){
        total+= font.getHeight();
    }
    total += credits.size() * font.getHeight();
    if (bottomAnimation != NULL){
        total += bottomHeight;
    }
    total += font.getHeight();
    
    total += spacing;
    return total;
}

OptionCredits::Sequence::Sequence(const Token * token):
type(Primary),
x(0),
y(0),
startx(0),
endx(0),
starty(0),
endy(0),
ticks(0),
duration(250),
speed(0),
alpha(0),
alphaMultiplier(0),
justification(Block::Center),
current(0),
done(false),
creditLength(0){
    if ( *token != "sequence" ){
        throw LoadException(__FILE__, __LINE__, "Not a credit sequence");
    }
    
    TokenView view = token->view();
    
    while (view.hasMore()){
        std::string match;
        try{
            const Token * tok;
            view >> tok;
            if (*tok == "type"){
                std::string sequenceType;
                tok->view() >> sequenceType;
                if (sequenceType == "roll"){
                    type = Roll;
                } else if (sequenceType == "primary"){
                    type = Primary;
                }
            } else if (*tok == "start-x"){
                tok->view() >> startx;
            } else if (*tok == "end-x"){
                tok->view() >> endx;
            } else if (*tok == "start-y"){
                tok->view() >> starty;
            } else if (*tok == "end-y"){
                tok->view() >> endy;
            } else if (*tok == "duration"){
                tok->view() >> duration;
            } else if (*tok == "speed"){
                tok->view() >> speed;
            } else if (*tok == "alpha-multiplier"){
                tok->view() >> alphaMultiplier;
            } else if ( *tok == "justification" ) {
                std::string justify;
                tok->view() >> justify;
                if (justify == "left"){
                    justification = Block::Left;
                } else if (justify == "center"){
                    justification = Block::Center;
                } else if (justify == "right"){
                    justification = Block::Right;
                }
            } else if (*tok == "block"){
                credits.push_back(Block(tok));
            } else {
                Global::debug( 3 ) <<"Unhandled Credit Sequence attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Credit Sequence parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
    
    // Initial
    reset();
    for (std::vector<OptionCredits::Block>::const_iterator i = credits.begin(); i != credits.end(); ++i){
        const OptionCredits::Block & block = *i;
        creditLength += block.size(Menu::menuFontParameter.current()->get());
    }
}

OptionCredits::Sequence::Sequence(const Sequence & copy):
type(copy.type),
x(copy.x),
y(copy.y),
startx(copy.startx),
endx(copy.endx),
starty(copy.starty),
endy(copy.endy),
ticks(copy.ticks),
duration(copy.duration),
speed(copy.speed),
alpha(copy.alpha),
alphaMultiplier(copy.alphaMultiplier),
justification(copy.justification),
credits(copy.credits),
current(copy.current),
done(false),
creditLength(copy.creditLength){
}

OptionCredits::Sequence::~Sequence(){
}

const OptionCredits::Sequence & OptionCredits::Sequence::operator=(const OptionCredits::Sequence & copy){
    type = copy.type;
    x = copy.x;
    y = copy.y;
    startx = copy.startx;
    endx = copy.endx;
    starty = copy.starty;
    endy = copy.endy;
    ticks = copy.ticks;
    duration = copy.duration;
    speed = copy.speed;
    alpha = copy.alpha;
    alphaMultiplier = copy.alphaMultiplier;
    justification = copy.justification;
    credits = copy.credits;
    current = copy.current;
    done = false;
    creditLength = copy.creditLength;
    return *this;
}

static int alphaClamp(int x, double multiplier){
    int clamp = x * multiplier;
    if (clamp < 0){
        clamp = 0;
    } else if (clamp > 255){
        clamp = 255;
    }
    return clamp;
}

void OptionCredits::Sequence::act(){
    if (!done && !credits.empty()){
        if (type == Roll){
            y += speed;
            if (starty > endy){
                if ((y + (creditLength * 1.1)) < endy){
                    done = true;
                }
            } else if (starty < endy){
                if ((y * 1.1) > endy){
                    done = true;
                }
            }
        } else if (type == Primary){
            credits[current].act();
            if (startx != endx){
                x += speed;
                if (startx > endx){
                    const double midpoint = (startx+endx)/2;
                    const int mid = x > midpoint ? startx -x : x - endx;
                    alpha = alphaClamp(mid, alphaMultiplier);
                    if (x < endx){
                        next();
                    }
                } else if (startx < endx){
                    const double midpoint = (startx+endx)/2;
                    const int mid = x < midpoint ? x - startx : endx - x;
                    alpha = alphaClamp(mid, alphaMultiplier);
                    //Global::debug(0) << "alpha: " << alpha << " midpoint: " << midpoint << " mid: " << mid << std::endl;
                    if (x > endx){
                        next();
                    }
                }
            } else {
                const double midpoint = duration/2;
                const int mid = ticks < midpoint ? ticks : duration - ticks;
                alpha = alphaClamp(mid, alphaMultiplier);
                ticks++;
                if (ticks >= duration){
                    ticks = 0;
                    next();
                }
            }
        }
    }
}

void OptionCredits::Sequence::draw(Graphics::Color title, Graphics::Color color, const Graphics::Bitmap & work){
    if (!done && !credits.empty()){
        if (type == Roll){
            int rollY = (int) y;
            for (std::vector<OptionCredits::Block>::const_iterator i = credits.begin(); i != credits.end(); ++i){
                const OptionCredits::Block & block = *i;
                rollY = block.print(x, rollY, title, color, Menu::menuFontParameter.current()->get(), work, justification);
            }
        } else if (type == Primary){
            // Graphics::Bitmap::transBlender(0, 0, 0, alpha);
            credits[current].print(x, y, title, color, Menu::menuFontParameter.current()->get(), work.translucent(alpha), justification);
        }
    }
}

void OptionCredits::Sequence::reset(){
    done = false;
    current = 0;
    ticks = 0;
    if (!credits.empty()){
        if (type == Roll){
            x = startx;
            y = starty;
        } else if (type == Primary){
            x = startx;
            y = starty - (credits[current].size(Menu::menuFontParameter.current()->get())/2);
        }
    }
}

void OptionCredits::Sequence::next(){
    if (type == Primary){
        if (current < credits.size()){
            current++;
            if (current == credits.size()){
                done = true;
            } else {
                x = startx;
                y = starty - (credits[current].size(Menu::menuFontParameter.current()->get())/2);
            }
        }
    }
}

static std::string defaultPositions(){
    const int width = Configuration::getScreenWidth();
    const int height = Configuration::getScreenHeight();
    std::ostringstream out;
    out << "(start-x " << width/2.3 << ") (end-x " << width/1.8 << ") (start-y " << height/2 << ") ";
    //out << "(start-x " << width/2 << ") (end-x " << width/2 << ") (start-y " << height/2 << ") (duration 250) ";
    return out.str();
}

OptionCredits::OptionCredits(const Gui::ContextBox & parent, const Token * token):
MenuOption(parent, token),
creditsContext(new Menu::Context()),
music(""),
color(Graphics::makeColor(255,255,255)),
title(Graphics::makeColor(0,255,255)),
clearColor(Graphics::makeColor(0,0,0)){
    std::string defaultSequence = "(sequence (type primary) (speed 0.3) (alpha-multiplier 20) (justification center) " + defaultPositions();
    
    /* Always */
    if (jonBirthday()){
        defaultSequence += "(block (title \"Happy birthday, Jon!\"))";
    }

    if (miguelBirthday()){
        defaultSequence += "(block (title \"Happy birthday, Miguel!\"))";
    }

    if (Storage::instance().exists(Filesystem::RelativePath("sprites/paintown.png"))){
        defaultSequence += "(block (animation (top) (width 350) (height 65) (image 0 \"sprites/paintown.png\") (frame (image 0) (time -1))) (credit \"Version " + Version::getVersionString() + "\"))";
    } else {
        defaultSequence += "(block (title \"R-Tech1\") (credit \"Version " + Version::getVersionString() + "\"))";
    }
    defaultSequence += "(block (title \"Programming\") (credit \"Jon Rafkind\") (credit \"Miguel Gavidia\"))";
    
    defaultSequence += "(block (title \"Level design\") (credit \"Jon Rafkind\") (credit \"Miguel Gavidia\"))";
    
    defaultSequence += "(block (title \"Contact\") (credit \"Website: http://paintown.org\") (credit \"Email: jon@rafkind.com\")))";
    TokenReader reader;
    Sequence sequence(reader.readTokenFromString(defaultSequence));
    sequences.push_back(sequence);
    
    //Global::debug(0) << defaultSequence << std::endl;
    
    if ( *token != "credits" ){
        throw LoadException(__FILE__, __LINE__, "Not a credit menu");
    }

    readName(token);
    
    TokenView view = token->view();
    
    // NOTE Use this to handle legacy additional blocks for the time being
    Block legacyAdditional("");
    bool additionalTitle = true;
    
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "music" ) {
                /* Set music for credits */
                tok->view() >> music;
            } else if ( *tok == "background" ) {
                /* Create an image and push it back on to vector */
                std::string temp;
                tok->view() >> temp;
                creditsContext->addBackground(temp);
            } else if ( *tok == "anim" || *tok == "animation" ){
                creditsContext->addBackground(tok);
            } else if ( *tok == "additional" ) {
                std::string str;
                TokenView additionalView = tok->view();
                while (additionalView.hasMore()){
                    additionalView >> str;
                    if (additionalTitle){
                        legacyAdditional = Block(str);
                        additionalTitle = false;
                    } else {
                        legacyAdditional.addCredit(str);
                    }
                }
            } else if (*tok == "sequence"){
                sequences.push_back(OptionCredits::Sequence(tok));
            } else if ( *tok == "titlecolor" ) {
                int r,b,g;
                tok->view() >> r >> g >> b;
                title = Graphics::makeColor( r, g, b );
            } else if ( *tok == "color" ) {
                    int r,b,g;
                    tok->view() >> r >> g >> b;
                    color = Graphics::makeColor( r, g, b );
            } else if ( *tok == "clear-color" ) {
                    int r,b,g;
                    tok->view() >> r >> g >> b;
                    clearColor = Graphics::makeColor( r, g, b );
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            throw ex;
        }
    }
    
    if (!legacyAdditional.empty()){
        //creditsRoll.push_back(legacyAdditional);
    }
	
    input.set(Keyboard::Key_ESC, 0, true, Exit);
    input.set(Joystick::Button2, 0, true, Exit);
    input.set(DeviceInput::Touch::Quit, Exit);
}

OptionCredits::~OptionCredits(){
}

void OptionCredits::logic(){
}

class CreditsLogicDraw : public Util::Logic, public Util::Draw {
public:
    CreditsLogicDraw(std::vector<OptionCredits::Sequence> & sequences, Graphics::Color clearColor, Graphics::Color title, Graphics::Color color, const Font & font, InputMap<OptionCredits::CreditKey> & input, Menu::Context & context):
    sequences(sequences),
    clearColor(clearColor),
    title(title),
    color(color),
    font(font),
    input(input),
    quit(false),
    context(context),
    current(0){
    }

    std::vector<OptionCredits::Sequence> & sequences;
    Graphics::Color clearColor, title, color;
    const Font & font;
    InputMap<OptionCredits::CreditKey> & input;
    bool quit;
    Menu::Context & context;
    unsigned int current;

    void run(){
        vector<InputMap<OptionCredits::CreditKey>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
        for (vector<InputMap<OptionCredits::CreditKey>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<OptionCredits::CreditKey>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == OptionCredits::Exit){
                    quit = true;
                    context.finish();
                }
            }
        }
        
        sequences[current].act();
        if (sequences[current].isDone()){
            sequences[current].reset();
            current++;
            if (current >= sequences.size()){
                current = 0;
            }
        }   
        context.act();
    }

    bool done(){
        return quit;
    }

    double ticks(double system){
        return Global::ticksPerSecond(system) * 30;
    }
    
    void draw(const Graphics::Bitmap & buffer){
        /* FIXME: hard coded resolution */
        Graphics::StretchedBitmap work(640, 480, buffer, Graphics::StretchedBitmap::NoClear, Graphics::qualityFilterName(Configuration::getQualityFilter()));
        work.fill(clearColor);
        work.start();
        //background.Blit(work);
        context.render(Util::ReferenceCount<Menu::Renderer>(NULL), work);
        
        sequences[current].draw(title, color, work);
        work.finish();
        
        // buffer.BlitToScreen();
    }
    
};

void OptionCredits::run(const Menu::Context & context){
    Menu::Context localContext(context, *creditsContext);
    localContext.initialize();

    if (!music.empty()){
        if (Music::loadSong(Storage::instance().find(Filesystem::RelativePath(music)).path())){
            Music::pause();
            Music::play();
        }
    }

    const Font & vFont = Menu::menuFontParameter.current()->get();
    
    CreditsLogicDraw loop(sequences, clearColor, title, color, vFont, input, localContext);
    Util::standardLoop(loop, loop);

    InputManager::waitForRelease(input, InputSource(true), Exit);
    throw Menu::Reload(__FILE__, __LINE__);
}

OptionDummy::OptionDummy(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token){
    if ( *token != "dummy" ){
        throw LoadException(__FILE__, __LINE__, "Not dummy option");
    }

    readName(token);

    if (getText().empty()){
        this->setText("Dummy");
    }
    
    setRunnable(false);
}

OptionDummy::OptionDummy(const Gui::ContextBox & parent, const std::string &name):
MenuOption(parent, 0){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to dummy");
    }
    this->setText(name);
    
    
    setRunnable(false);
}

OptionDummy::~OptionDummy(){
}

void OptionDummy::logic(){
}

void OptionDummy::run(const Menu::Context & context){
}

OptionFullscreen::OptionFullscreen(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token)
    /*
lblue(255),
lgreen(255),
rblue(255),
rgreen(255)
*/{
	setRunnable(false);
	
	if ( *token != "fullscreen" )
		throw LoadException(__FILE__, __LINE__, "Not fullscreen option");

        readName(token);
}

OptionFullscreen::~OptionFullscreen()
{
	// Nothing
}
    
std::string OptionFullscreen::getText() const {
    ostringstream out;
    out << MenuOption::getText() << ": " << (Configuration::getFullscreen() ? "Yes" : "No");
    return out.str();
}

void OptionFullscreen::logic(){;
}

static void changeScreenMode(){
    Configuration::setFullscreen(!Configuration::getFullscreen());
    int gfx = (Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
    Graphics::changeGraphicsMode(gfx, Graphics::Bitmap::getScreenWidth(), Graphics::Bitmap::getScreenHeight());
}

void OptionFullscreen::run(const Menu::Context & context){
    changeScreenMode();
}

bool OptionFullscreen::leftKey(){
    changeScreenMode();
    return true;
}
bool OptionFullscreen::rightKey(){
    changeScreenMode();
    return true;
}

OptionFps::OptionFps(const Gui::ContextBox & parent, const Token * token):
MenuOption(parent, token){
    readName(token);
    setRunnable(false);
}

void OptionFps::logic(){
}

void OptionFps::run(const Menu::Context & context){
}

std::string OptionFps::getText() const {
    ostringstream out;
    out << "Frames per second: " << Global::TICS_PER_SECOND;
    return out.str();
}

bool OptionFps::leftKey(){
    Global::setTicksPerSecond(Global::TICS_PER_SECOND - 1);
    Configuration::setFps(Global::TICS_PER_SECOND);
    return true;
}

bool OptionFps::rightKey(){
    Global::setTicksPerSecond(Global::TICS_PER_SECOND + 1);
    Configuration::setFps(Global::TICS_PER_SECOND);
    return true;
}

OptionFps::~OptionFps(){
}

OptionQualityFilter::OptionQualityFilter(const Gui::ContextBox & parent, const Token * token):
MenuOption(parent, token){
    readName(token);
    setRunnable(false);
}
              
std::string OptionQualityFilter::getText() const {
    ostringstream out;
    out << MenuOption::getText() << ": " << Configuration::getQualityFilter();
    return out.str();
}

void OptionQualityFilter::logic(){
}
    
bool OptionQualityFilter::leftKey(){
    string quality = Configuration::getQualityFilter();
    if (quality == "none"){
        quality = "hqx";
    } else if (quality == "hqx"){
        quality = "xbr";
    } else if (quality == "xbr"){
        quality = "none";
    }
    Configuration::setQualityFilter(quality);
    return true;
}

bool OptionQualityFilter::rightKey(){
    string quality = Configuration::getQualityFilter();
    if (quality == "none"){
        quality = "xbr";
    } else if (quality == "hqx"){
        quality = "none";
    } else if (quality == "xbr"){
        quality = "hqx";
    }
    Configuration::setQualityFilter(quality);
    return true;

}

void OptionQualityFilter::run(const Menu::Context & context){
}

OptionQualityFilter::~OptionQualityFilter(){
}


#if 0
static OptionJoystick::JoystickType convertToKey(const std::string &k){
    std::string temp = k;
    for(unsigned int i=0;i<temp.length();i++){
        temp[i] = tolower(temp[i]);
    }

    if (temp == "up") return OptionJoystick::Up;
    if (temp == "down") return OptionJoystick::Down;
    if (temp == "left") return OptionJoystick::Left;
/*
    if (temp == "right") return OptionJoystick::Right;
    if (temp == "jump") return OptionJoystick::Jump;
    if (temp == "attack1") return OptionJoystick::Attack1;
    if (temp == "attack2") return OptionJoystick::Attack2;
    if (temp == "attack3") return OptionJoystick::Attack3;
    if (temp == "attack4") return OptionJoystick::Attack4;
    if (temp == "attack5") return OptionJoystick::Attack5;
    if (temp == "attack6") return OptionJoystick::Attack6;

    return OptionJoystick::Invalidkey;
}

static Configuration::JoystickInput getKey(int player, OptionJoystick::JoystickType k){
    switch(k){
        case OptionJoystick::Up:
            return Joystick::Up;
        case OptionJoystick::Down:
            return Joystick::Down;
        case OptionJoystick::Left:
            return Joystick::Left;
        case OptionJoystick::Right:
            return Joystick::Right;
        case OptionJoystick::Jump:
            return Joystick::Button4;
        case OptionJoystick::Attack1:
            return Joystick::Button1;
        case OptionJoystick::Attack2:
            return Joystick::Button2;
        case OptionJoystick::Attack3:
            return Joystick::Button3;
        case OptionJoystick::Attack4:
            return Joystick::Button4;
        case OptionJoystick::Attack5:
            return Joystick::Button5;
        case OptionJoystick::Attack6:
            return Joystick::Button6;
        default:
            break;
    }

    return Joystick::Up;
}

static void setKey(int player, OptionJoystick::JoystickType k, Configuration::JoystickInput key){
    / *
    switch(k){
        case OptionJoystick::Up:
            Configuration::setJoystickUp(player, key);
            break;
        case OptionJoystick::Down:
            Configuration::setJoystickDown(player, key);
            break;
        case OptionJoystick::Left:
            Configuration::setJoystickLeft(player, key);
            break;
        case OptionJoystick::Right:
            Configuration::setJoystickRight(player, key);
            break;
        case OptionJoystick::Jump:
            Configuration::setJoystickJump(player, key);
            break;
        case OptionJoystick::Attack1:
            Configuration::setJoystickAttack1(player, key);
            break;
        case OptionJoystick::Attack2:
            Configuration::setJoystickAttack2(player, key);
            break;
        case OptionJoystick::Attack3:
            Configuration::setJoystickAttack3(player, key);
            break;
        case OptionJoystick::Attack4:
            Configuration::setJoystickAttack4(player, key);
            break;
        case OptionJoystick::Attack5:
            Configuration::setJoystickAttack5(player, key);
            break;
        case OptionJoystick::Attack6:
            Configuration::setJoystickAttack6(player, key);
            break;
        default:
            break;
    }
    */
}

static Configuration::JoystickInput readJoystick(){
    vector<Joystick::Key> keys;
    keys.push_back(Joystick::Up);
    keys.push_back(Joystick::Down);
    keys.push_back(Joystick::Left);
    keys.push_back(Joystick::Right);
    keys.push_back(Joystick::Button1);
    keys.push_back(Joystick::Button2);
    keys.push_back(Joystick::Button3);
    keys.push_back(Joystick::Button4);
    keys.push_back(Joystick::Button5);
    keys.push_back(Joystick::Button6);

    InputMap<Joystick::Key> input;
    for (vector<Joystick::Key>::iterator it = keys.begin(); it != keys.end(); it++){
        input.set(*it, 0, true, *it);
    }
    input.set(Keyboard::Key_ESC, 0, true, Joystick::Invalid);

    while (true){
        InputManager::poll();
        vector<InputMap<Joystick::Key>::InputEvent> out = InputManager::getEvents(input, InputSource());
        for (vector<InputMap<Joystick::Key>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Joystick::Key>::InputEvent & event = *it;
            if (event.enabled){
                Global::debug(1) << "Press: " << event.out << std::endl;
                if (event.out == Joystick::Invalid){
                    InputManager::waitForRelease(input, InputSource(), Joystick::Invalid);
                    throw Exception::Return(__FILE__, __LINE__);
                }
                return event.out;
            }
        }
        Util::rest(1);
    }

    /* control probably shouldn't get here.. */
    return Joystick::Up;
}

OptionJoystick::OptionJoystick(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token),
name(""),
player(-1),
type(Invalidkey),
keyCode(0){
    if (*token != "joystick"){
        throw LoadException(__FILE__, __LINE__, "Not joystick option");
    }

    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "name" ){
                tok->view() >> name;
            } else if ( *tok == "player" ) {
                tok->view() >> player;
            } else if ( *tok == "type" ) {
                std::string temp;
                tok->view() >> temp;
                type = convertToKey(temp);
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                tok->print(" ");
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if (name.empty()){
        throw LoadException(__FILE__, __LINE__, "No name set, this option should have a name!");
    }

    if (type == Invalidkey){
        throw LoadException(__FILE__, __LINE__, "Invalid joystick button, should be up, down, left, right, up, down, jump, attack1-6!");
    }

    if (player == -1){
        throw LoadException(__FILE__, __LINE__, "Player not specified in joystick configuration");
    }

    ostringstream out;
    out << name << ": " << Joystick::keyToName(getKey(player, type));
    setText(out.str());
}

OptionJoystick::~OptionJoystick(){
    // Nothing
}

void OptionJoystick::logic(){
    /*
    char temp[255];
    sprintf( temp, "%s: %s", name.c_str(), Joystick::keyToName(getKey(player,type)));
    setText(std::string(temp));
    */
}

void OptionJoystick::run(const Menu::Context & context){
    /*
    //int x, y, width, height;
    const Font & vFont = Menu::menuFontParameter.current()->get();
    const char * message = "Press a joystick button!";
    const int width = vFont.textLength(message) + 10;
    const int height = vFont.getHeight() + 10;
    // const int x = (getParent()->getWork()->getWidth()/2) - (width/2);
    // const int y = (getParent()->getWork()->getHeight()/2) - (height/2);
    const int x = Menu::Menu::Width / 2 - width/2;
    const int y = Menu::Menu::Height / 2 - height/2;
    Box dialog;
    dialog.location.setPosition(Gui::AbsolutePoint(0,0));
    dialog.location.setDimensions(vFont.textLength(message) + 10, vFont.getHeight() + 10);
    dialog.transforms.setRadius(0);
    dialog.colors.body = Graphics::makeColor(0,0,0);
    dialog.colors.bodyAlpha = 200;
    dialog.colors.border = Graphics::makeColor(255,255,255);
    dialog.colors.borderAlpha = 255;
    Graphics::Bitmap temp = Graphics::Bitmap::temporaryBitmap(width,height);
    dialog.render(temp, vFont);
    vFont.printf( 5, 5, Graphics::makeColor(255,255,255), temp, message, -1);
    temp.BlitToScreen(x,y);
    
    setKey(player, type, readJoystick());

    ostringstream out;
    out << name << ": " << Joystick::keyToName(getKey(player, type));
    setText(out.str());
    */

    Graphics::Bitmap temp(Menu::Menu::Width, Menu::Menu::Height);
    // Menu::Context tempContext = context;
    Menu::Context tempContext(context);
    tempContext.initialize();
    Menu::InfoBox keyDialog;
    // keyDialog.setFont(tempContext.getFont());
    //keyDialog.location.set(-1,-1,1,1);
    const int width = temp.getWidth();
    const int height = temp.getHeight();
    const Font & font = Menu::menuFontParameter.current()->get();
    const int radius = 15;
    keyDialog.setText("Press a joystick button!");
    keyDialog.initialize(font);
    keyDialog.location.setDimensions(font.textLength("Press a joystick button!") + radius, font.getHeight() + radius);
    keyDialog.location.setCenterPosition(Gui::RelativePoint(0, 0));
    // keyDialog.location.setPosition(Gui::AbsolutePoint((width/2)-(keyDialog.location.getWidth()/2), (height/2)-(keyDialog.location.getHeight()/2)));
    // keyDialog.location.setPosition2(Gui::AbsolutePoint((
    keyDialog.transforms.setRadius(radius);
    keyDialog.colors.body = Graphics::makeColor(0,0,0);
    keyDialog.colors.bodyAlpha = 180;
    keyDialog.colors.border = Graphics::makeColor(255,255,255);
    keyDialog.colors.borderAlpha = 255;
    keyDialog.open();
    InputManager::waitForClear();
    while (!InputManager::anyInput() && keyDialog.isActive()){
        InputManager::poll();
	keyDialog.act(font);
        /*
	if (keyDialog.isActive()){
            InputManager::poll();
	}
        */
	tempContext.act();
	tempContext.render(0, temp);
	keyDialog.render(temp, font);
	temp.BlitToScreen();
    }
    tempContext.finish();
    setKey(player, type, readJoystick());
    InputManager::waitForClear();
    
    ostringstream out;
    out << name << ": " << Joystick::keyToName(getKey(player, type));
    setText(out.str());

    /*
    Keyboard key;
    keyCode = readKey(key);
    setKey(player,type, keyCode);
    */
}
#endif

static OptionKey::keyType convertToKeyboardKey(const std::string &k){
    std::string temp = k;
    for(unsigned int i=0;i<temp.length();i++){
        temp[i] = tolower(temp[i]);
    }
    if (temp == "up") return OptionKey::up;
    if (temp == "down") return OptionKey::down;
    if (temp == "left") return OptionKey::left;
    if (temp == "right") return OptionKey::right;
    if (temp == "jump") return OptionKey::jump;
    if (temp == "attack1") return OptionKey::attack1;
    if (temp == "attack2") return OptionKey::attack2;
    if (temp == "attack3") return OptionKey::attack3;
    if (temp == "attack4") return OptionKey::attack4;
    if (temp == "attack5") return OptionKey::attack5;
    if (temp == "attack6") return OptionKey::attack6;

    return OptionKey::invalidkey;
}

static int getKey(int player, OptionKey::keyType k){
    switch(k){
        case OptionKey::up:
            return Configuration::getUp(player);
            break;
        case OptionKey::down:
            return Configuration::getDown(player);
            break;
        case OptionKey::left:
            return Configuration::getLeft(player);
            break;
        case OptionKey::right:
            return Configuration::getRight(player);
            break;
        case OptionKey::jump:
            return Configuration::getJump(player);
            break;
        case OptionKey::attack1:
            return Configuration::getAttack1(player);
            break;
        case OptionKey::attack2:
            return Configuration::getAttack2(player);
            break;
        case OptionKey::attack3:
            return Configuration::getAttack3(player);
            break;
        case OptionKey::attack4:
            return Configuration::getAttack4(player);
        case OptionKey::attack5:
            return Configuration::getAttack5(player);
        case OptionKey::attack6:
            return Configuration::getAttack6(player);
        default:
            break;
    }

    return 0;
}

static void setKey(int player, OptionKey::keyType k, int key){
    switch(k){
        case OptionKey::up:
            Configuration::setUp(player, key);
            break;
        case OptionKey::down:
            Configuration::setDown(player, key);
            break;
        case OptionKey::left:
            Configuration::setLeft(player, key);
            break;
        case OptionKey::right:
            Configuration::setRight(player, key);
            break;
        case OptionKey::jump:
            Configuration::setJump(player, key);
            break;
        case OptionKey::attack1:
            Configuration::setAttack1(player, key);
            break;
        case OptionKey::attack2:
            Configuration::setAttack2(player, key);
            break;
        case OptionKey::attack3:
            Configuration::setAttack3(player, key);
            break;
        case OptionKey::attack4:
            Configuration::setAttack4(player, key);
            break;
        case OptionKey::attack5:
            Configuration::setAttack5(player, key);
            break;
        case OptionKey::attack6:
            Configuration::setAttack6(player, key);
            break;
        default:
            break;
    }
}

/*
static int readKey( Keyboard & key ){
    int k = key.readKey();
    key.wait();
    return k;
}
*/

OptionKey::OptionKey(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token),
name(""),
player(-1),
type(invalidkey),
keyCode(0){
    if ( *token != "key" )
        throw LoadException(__FILE__, __LINE__, "Not key option");

    TokenView view = token->view();
    while (view.hasMore()) {
        try {
            const Token * tok;
            view >> tok;
            if ( *tok == "name" ) {
                tok->view() >> name;
            } else if ( *tok == "player" ) {
                tok->view() >> player;
            } else if ( *tok == "type" ) {
                std::string temp;
                tok->view() >> temp;
                type = convertToKeyboardKey(temp);
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                tok->print(" ");
            }
        } catch ( const TokenException & ex ){
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if(name.empty())throw LoadException(__FILE__, __LINE__, "No name set, this option should have a name!");
    if(type == invalidkey)throw LoadException(__FILE__, __LINE__, "Invalid key, should be up, down, left, right, up, down, jump, attack1-6!");
    if(player == -1)throw LoadException(__FILE__, __LINE__, "Player not specified in key configuration");

    char temp[255];
    sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
    setText(std::string(temp));
}

OptionKey::~OptionKey(){
	// Nothing
}

void OptionKey::logic(){
    char temp[255];
    sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
    setText(std::string(temp));
}

void OptionKey::run(const Menu::Context & context){
    // Do dialog
    //Box::messageDialog(Menu::Menu::Width, Menu::Menu::Height, "Press a Key!",2);


    class WaitForKey: public Util::Logic, public Util::Draw {
    public:
        WaitForKey(const Menu::Context & context):
        font(Menu::menuFontParameter.current()->get()),
        tempContext(context),
        speed(30),
        listener(*this),
        quit(false),
        out(Keyboard::Key_ESC),
        startingTime(System::currentMilliseconds()),
        maxTime(5000){
            tempContext.initialize();

            // keyDialog.setFont(tempContext.getFont());
            //keyDialog.location.set(-1,-1,1,1);
            const int width = 320;
            const int height = 240;
            const int radius = 15;
            keyDialog.setText("Press a Key!");
            keyDialog.initialize(font);
            keyDialog.location.setDimensions(font.textLength("Press a Key!") + radius, font.getHeight() + radius);
            keyDialog.location.setCenterPosition(Gui::RelativePoint(0, 0));
            // keyDialog.location.setPosition(Gui::AbsolutePoint((width/2)-(keyDialog.location.getWidth()/2), (height/2)-(keyDialog.location.getHeight()/2)));
            // keyDialog.location.setPosition2(Gui::AbsolutePoint((
            keyDialog.transforms.setRadius(radius);
            keyDialog.colors.body = Graphics::makeColor(0,0,0);
            keyDialog.colors.bodyAlpha = 180;
            keyDialog.colors.border = Graphics::makeColor(255,255,255);
            keyDialog.colors.borderAlpha = 255;
            keyDialog.open();

            Keyboard::pushRepeatState(false);
            Keyboard::addListener(&listener);

            vector<Graphics::BlendPoint> points;
            points.push_back(Graphics::BlendPoint(Graphics::makeColor(255, 0, 0), 30));
            points.push_back(Graphics::BlendPoint(Graphics::makeColor(0, 255, 0), 25));
            points.push_back(Graphics::BlendPoint(Graphics::makeColor(255, 255, 255), 0));
            colors = Graphics::blend_palette(points);
        }

        virtual ~WaitForKey(){
            Keyboard::removeListener(&listener);
            Keyboard::popRepeatState();
        }

        class Listener: public KeyboardListener {
        public:
            Listener(WaitForKey & parent):
            parent(parent){
            }

            WaitForKey & parent;

            virtual void press(Keyboard * keyboard, Keyboard::KeyType key, Keyboard::unicode_t code){
                parent.press(key);
            }

            virtual void release(Keyboard * keyboard, Keyboard::KeyType key){
                parent.release(key);
            }
        };

        const Font & font;
        Menu::Context tempContext;
        Menu::InfoBox keyDialog;
        const int speed;
        Listener listener;
        bool quit;
        Keyboard::KeyType out;
        /* keep track of how much time we are in this loop */
        uint64_t startingTime;
        /* maximum number of milliseconds to wait */
        const uint64_t maxTime;

        vector<Graphics::Color> colors;

        virtual void press(Keyboard::KeyType key){
            quit = true;

            if (key != Keyboard::Key_ESC){
                this->out = key;
            }
        }

        virtual void release(Keyboard::KeyType key){
        }
    
        virtual void run(){
            for (int i = 0; i < 90 / speed; i++){
                tempContext.act();
                keyDialog.act(font);
            }

            /* exit after 5 seconds */
            quit = quit || (System::currentMilliseconds() - startingTime > maxTime);
        }
    
        virtual bool done(){
            return quit;
        }
    
        virtual double ticks(double system){
            return Global::ticksPerSecond(system) * speed;
        }

        virtual void showTimeLeft(const Graphics::Bitmap & screen){
            int x1 = 1;
            int y1 = 1;
            int x2 = screen.getWidth() * (1 - (double) (System::currentMilliseconds() - startingTime) / (double) maxTime);
            if (x2 < x1){
                x2 = x1;
            }

            int y2 = 10;

            int color = (1.0 - (double) (System::currentMilliseconds() - startingTime) / maxTime) * colors.size();
            if (color < 0){
                color = 0;
            }
            if (color >= (int) colors.size()){
                color = colors.size() - 1;
            }

            screen.rectangleFill(x1, y1, x2, y2, colors[color]);
        }
    
        virtual void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap work(640, 480, screen);
            work.start();
            tempContext.render(Util::ReferenceCount<Menu::Renderer>(NULL), work);
            keyDialog.render(work, font);
            showTimeLeft(work);
            work.finish();
        }
    };

    WaitForKey run(context);
    Util::standardLoop(run, run);

    Keyboard::KeyType key = run.out;
    if (key != Keyboard::Key_ESC){
        setKey(player, type, key);
    }

    /*
    Keyboard key;
    key.wait();
    */
    /*
    Graphics::Bitmap temp(Menu::Menu::Width, Menu::Menu::Height);
    // Menu::Context tempContext = context;
    Menu::Context tempContext(context);
    tempContext.initialize();
    Menu::InfoBox keyDialog;
    // keyDialog.setFont(tempContext.getFont());
    //keyDialog.location.set(-1,-1,1,1);
    const int width = temp.getWidth();
    const int height = temp.getHeight();
    const Font & font = Menu::menuFontParameter.current()->get();
    const int radius = 15;
    keyDialog.setText("Press a Key!");
    keyDialog.initialize(font);
    keyDialog.location.setDimensions(font.textLength("Press a Key!") + radius, font.getHeight() + radius);
    keyDialog.location.setCenterPosition(Gui::RelativePoint(0, 0));
    // keyDialog.location.setPosition(Gui::AbsolutePoint((width/2)-(keyDialog.location.getWidth()/2), (height/2)-(keyDialog.location.getHeight()/2)));
    // keyDialog.location.setPosition2(Gui::AbsolutePoint((
    keyDialog.transforms.setRadius(radius);
    keyDialog.colors.body = Graphics::makeColor(0,0,0);
    keyDialog.colors.bodyAlpha = 180;
    keyDialog.colors.border = Graphics::makeColor(255,255,255);
    keyDialog.colors.borderAlpha = 255;
    keyDialog.open();
    InputManager::waitForClear();
    while (!InputManager::anyInput() && keyDialog.isActive()){
        InputManager::poll();
	keyDialog.act(font);
        / *
	if (keyDialog.isActive()){
            InputManager::poll();
	}
        * /
	tempContext.act();
	tempContext.render(Util::ReferenceCount<Menu::Renderer>(NULL), temp);
	keyDialog.render(temp, font);
	temp.BlitToScreen();
    }
    tempContext.finish();
    keyCode = InputManager::readKey();
    setKey(player,type, keyCode);
    InputManager::waitForClear();
    */
}

OptionLevel::OptionLevel(const Gui::ContextBox & parent, const Token *token, int * set, int value):
MenuOption(parent, token),
set(set),
value(value){
  // Nothing
}

OptionLevel::~OptionLevel(){
}

void OptionLevel::logic(){
}

void OptionLevel::run(const Menu::Context & context){
    *set = value;
    throw Menu::MenuException(__FILE__, __LINE__);
}

OptionMenu::OptionMenu(const Gui::ContextBox & parent, const Token *token, const Menu::OptionFactory & factory):
MenuOption(parent, token),
menu(0){
    if (*token != "menu"){
        throw LoadException(__FILE__, __LINE__, "Not a menu");
    }

    if (token->numTokens() == 1){
        std::string temp;
        token->view() >> temp;
        menu = new Menu::Menu(Storage::instance().find(Filesystem::RelativePath(temp)), factory);
    } else {
        menu = new Menu::Menu(token, factory);
    }

    this->setText(menu->getName());
    this->setInfoText(menu->getInfo());
    
    // Lets check if this menu is going bye bye
    //if ( menu->checkRemoval() ) setForRemoval(true);
}

OptionMenu::~OptionMenu(){
    // Delete our menu
    if (menu){
        delete menu;
    }
}

void OptionMenu::logic(){
    // Nothing
}

void OptionMenu::run(const Menu::Context & context){
    // Do our new menu
    try{
        menu->run(context);
    } catch (const Exception::Return & ignore){
        throw Menu::Reload(__FILE__, __LINE__);
    }
}

OptionReturn::OptionReturn(const Gui::ContextBox & parent, const Token * token):
MenuOption(parent, token){
    if (*token != "return"){
        throw LoadException(__FILE__, __LINE__, "Not a return option");
    }
    readName(token);
}

void OptionReturn::logic(){
}

/* maybe this option is misnamed, but its supposed to quit the current game
 * and go back to the main menu
 */
void OptionReturn::run(const Menu::Context & context){
    throw Exception::Quit(__FILE__, __LINE__);
}

OptionReturn::~OptionReturn(){
}

OptionContinue::OptionContinue(const Gui::ContextBox & parent, const Token * token):
MenuOption(parent, token){
    if (*token != "continue"){
        throw LoadException(__FILE__, __LINE__, "Not a continue option");
    }
    readName(token);
}

void OptionContinue::logic(){
}

void OptionContinue::run(const Menu::Context & context){
    throw Exception::Return(__FILE__, __LINE__);
}

OptionContinue::~OptionContinue(){
}

OptionQuit::OptionQuit(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token){
    if ( *token != "quit" ){
        throw LoadException(__FILE__, __LINE__, "Not quit option");
    }

    readName(token);
}

OptionQuit::OptionQuit(const Gui::ContextBox & parent, const std::string &name):
MenuOption(parent, 0){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to quit");
    }
    this->setText(name);
}

OptionQuit::~OptionQuit(){
}

void OptionQuit::logic(){
}

void OptionQuit::run(const Menu::Context & context){
    throw ShutdownException();
}


#if defined(WINDOWS) && defined(doesnt_work_yet)
#include <windows.h>
#include <stdio.h>

/* contributed by Roy Underthump from allegro.cc */
static vector<ScreenSize> getScreenResolutions(){
    HWND hwnd;
    HDC  hdc;

    // int iPixelFormat;
    int descerr;
    int retval;

    DEVMODE d;

    PIXELFORMATDESCRIPTOR pfd;

    hwnd = GetDesktopWindow();
    hdc  = GetDC(hwnd);

    vector<ScreenSize> modes;

    for (int i = 0;; i++){
        retval = EnumDisplaySettings(0,i,&d);
        if (!retval){
            break;
        }

        descerr = DescribePixelFormat(hdc, i+1, sizeof(pfd), &pfd);
        if(!descerr){
            continue;
        }

        /*
           printf("\n#%d bpp %d width %d height %d colorbits %d fps %d",i,d.dmBitsPerPel,
           d.dmPelsWidth, d.dmPelsHeight,pfd.cColorBits,d.dmDisplayFrequency);

           if(pfd.dwFlags & PFD_SUPPORT_OPENGL)printf(" OGL OK");
         */
        modes.push_back(ScreenSize(d.dmPelsWidth, d.dmPelsHeight));
    }

    if (modes.empty()){
        modes.push_back(ScreenSize(640,480));
    }

    return modes;
}
#else
static vector<ScreenSize> getScreenResolutions(){
	vector<ScreenSize> modes;
	modes.push_back(ScreenSize(320, 240));
	modes.push_back(ScreenSize(640, 480));
	modes.push_back(ScreenSize(800, 600));
	modes.push_back(ScreenSize(960, 720));
	modes.push_back(ScreenSize(1024, 768));
	modes.push_back(ScreenSize(1280, 960));
	modes.push_back(ScreenSize(1600, 1200));
	return modes;
}
#endif

static bool doSort(const ScreenSize & a, const ScreenSize & b){
    return (a.w * a.h) < (b.w * b.h);
}

static vector<ScreenSize> sortResolutions(const vector<ScreenSize> & modes){
    vector<ScreenSize> copy(modes);
    std::sort(copy.begin(), copy.end(), doSort);
    return copy;
}

OptionScreenSize::OptionScreenSize(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token),
name(""),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    Global::debug(1) << "Get screen resolution" << endl;
    modes = sortResolutions(getScreenResolutions());

    if (Global::getDebug() >= 1){
        for (vector<ScreenSize>::iterator it = modes.begin(); it != modes.end(); it++){
            Global::debug(1) << "Screen size: " << it->w << " x " << it->h << endl;
        }
    }

    if ( *token != "screen-size" ){
        throw LoadException(__FILE__, __LINE__, "Not a screen-size");
    }

    readName(token);
}

OptionScreenSize::~OptionScreenSize(){
    // Nothing
}

void OptionScreenSize::logic(){
    ostringstream temp;
    temp << "Screen size: " << Configuration::getScreenWidth() << " x " << Configuration::getScreenHeight();
    setText(temp.str());

}

void OptionScreenSize::run(const Menu::Context & context){
}

void OptionScreenSize::setMode(int width, int height){
    if (width != Configuration::getScreenWidth() ||
        height != Configuration::getScreenHeight()){

        Global::debug(1) << "Changing mode to " << width << " x " << height << endl;
        int gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
        int ok = Graphics::changeGraphicsMode(gfx, width, height);
        if (ok == 0){
            Global::debug(1) << "Success" << endl;
            Configuration::setScreenWidth(width);
            Configuration::setScreenHeight(height);
        } else {
            Global::debug(1) << "Fail" << endl;
            int ok = Graphics::changeGraphicsMode(gfx, Configuration::getScreenWidth(), Configuration::getScreenHeight());
            Global::debug(1) << "Set mode back " << ok << endl;
        }
    }
}

/*
static int modes[][2] = {{640,480}, {800,600}, {1024,768}, {1280,1024}, {1600,1200}};
// static int max_modes = sizeof(modes) / sizeof(int[]);
static int max_modes = 5;
*/

int OptionScreenSize::findMode(int width, int height){
    for (int mode = 0; mode < (int) modes.size(); mode++){
        if (modes[mode].w == width && modes[mode].h == height){
            return mode;
        }
    }
    return -1;
}

bool OptionScreenSize::leftKey(){
    int mode = findMode(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    if (mode >= 1 && mode < (int)modes.size()){
        mode -= 1;
    } else {
        mode = 0;
    }

    setMode(modes[mode].w, modes[mode].h);

    lblue = lgreen = 0;
    return true;
}

bool OptionScreenSize::rightKey(){
    int mode = findMode(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    if (mode >= 0 && mode < (int)modes.size() - 1){
        mode += 1;
    } else {
        mode = 0;
    }

    setMode(modes[mode].w, modes[mode].h);

    rblue = rgreen = 0;
    return true;
}

static string joinPaths(const vector<Filesystem::AbsolutePath> & strings, const string & middle){
    ostringstream out;

    for (vector<Filesystem::AbsolutePath>::const_iterator it = strings.begin(); it != strings.end(); it++){
        out << (*it).path() << middle;
    }

    return out.str();
}

static bool sortInfo(const Util::ReferenceCount<Menu::FontInfo> & info1, 
                     const Util::ReferenceCount<Menu::FontInfo> & info2){
    string name1 = Util::lowerCaseAll(info1->getName());
    string name2 = Util::lowerCaseAll(info2->getName());
    return name1 < name2;
}

static bool isWindows(){
#ifdef WINDOWS
    return true;
#else
    return false;
#endif
}

static bool isOSX(){
#ifdef MACOSX
    return true;
#else
    return false;
#endif
}

template <class X>
static vector<X> operator+(const vector<X> & v1, const vector<X> & v2){
    vector<X> out;
    for (typename vector<X>::const_iterator it = v1.begin(); it != v1.end(); it++){
        out.push_back(*it);
    }
    for (typename vector<X>::const_iterator it = v2.begin(); it != v2.end(); it++){
        out.push_back(*it);
    }
    return out;
}

static vector<Filesystem::AbsolutePath> findSystemFonts(){
    if (isWindows()){
        const char * windows = getenv("windir");
        if (windows != NULL){
            return Storage::instance().getFilesRecursive(Filesystem::AbsolutePath(string(windows) + "/fonts"), "*.ttf");
        }
        return vector<Filesystem::AbsolutePath>();
    } else if (isOSX()){
        return Storage::instance().getFilesRecursive(Filesystem::AbsolutePath("/Library/Fonts"), "*.ttf");
    } else {
        /* assume unix/linux conventions */
        return Storage::instance().getFilesRecursive(Filesystem::AbsolutePath("/usr/share/fonts/truetype"), "*.ttf") + 
               Storage::instance().getFilesRecursive(Filesystem::AbsolutePath("/usr/local/share/fonts/truetype"), "*.ttf");

    }
}

static vector<Util::ReferenceCount<Menu::FontInfo> > findFonts(){
    vector<Util::ReferenceCount<Menu::FontInfo> > fonts;
    try{
        Filesystem::AbsolutePath fontsDirectory = Storage::instance().find(Filesystem::RelativePath("fonts"));
        Global::debug(1, "fonts") << "Font directory " << fontsDirectory.path() << endl;
        vector<Filesystem::AbsolutePath> ttfFonts = Storage::instance().getFiles(fontsDirectory, "*.ttf");
        Global::debug(1, "fonts") << "Found ttf fonts " << joinPaths(ttfFonts, ", ") << endl;
        vector<Filesystem::AbsolutePath> otfFonts = Storage::instance().getFiles(fontsDirectory, "*.otf");
        Global::debug(1, "fonts") << "Found otf fonts " << joinPaths(otfFonts, ", ") << endl;

        for (vector<Filesystem::AbsolutePath>::iterator it = ttfFonts.begin(); it != ttfFonts.end(); it++){
            fonts.push_back(Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(Storage::instance().cleanse(*it), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));
        }

        for (vector<Filesystem::AbsolutePath>::iterator it = otfFonts.begin(); it != otfFonts.end(); it++){
            fonts.push_back(Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(Storage::instance().cleanse(*it), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));
        }
        
        /* linux specific fonts */
        vector<Filesystem::AbsolutePath> systemFonts = findSystemFonts();
        for (vector<Filesystem::AbsolutePath>::iterator it = systemFonts.begin(); it != systemFonts.end(); it++){
            Global::debug(1) << "Adding system font `" << (*it).path() << "'" << endl;
            fonts.push_back(Util::ReferenceCount<Menu::FontInfo>(new Menu::AbsoluteFontInfo(*it, Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));
        }

        sort(fonts.begin(), fonts.end(), sortInfo);
        
        // DEFAULT (blank)
        // fonts.insert(fonts.begin(), new Menu::DefaultFontInfo());
        fonts.insert(fonts.begin(), Util::ReferenceCount<Menu::FontInfo>(NULL));
    } catch (const Filesystem::NotFound & e){
        throw LoadException(__FILE__, __LINE__, e, "Could not load font");
    }

    return fonts;
}

OptionSelectFont::OptionSelectFont(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token),
typeAdjust(fontName),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "font-select" ){
        throw LoadException(__FILE__, __LINE__, "Not a font selector");
    }

    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "adjust" ){
                std::string temp;
                tok->view() >> temp;
                if ( temp == "name" ) typeAdjust = fontName;
                else if ( temp == "width" ) typeAdjust = fontWidth;
                else if ( temp == "height" ) typeAdjust = fontHeight;
                else throw LoadException(__FILE__, __LINE__, "Incorrect value \"" + temp + "\" in font-select");
            } else {
                Global::debug(3) << "Unhandled menu attribute: " << endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }
}

void OptionSelectFont::open(){
    // Find and set fonts now
    if (typeAdjust == fontName){
        fonts = findFonts();
    }
}
    
void OptionSelectFont::close(){
    if (typeAdjust == fontName){
        /* the user probably loaded a bunch of different fonts that will
         * never be used again, so clear the font cache
         * TODO: dont clear the currently selected font
         */
        FontFactory::clear();
    }
}

OptionSelectFont::~OptionSelectFont(){
    // Nothing
}

void OptionSelectFont::logic(){
    /* FIXME Get current font and display info */
    switch (typeAdjust){
        case fontName:{
	    std::string name;
            if (Configuration::hasMenuFont()){
                name = Configuration::getMenuFont()->getName();
            } else {
                name = "Default";
            }
            setText("Current Font: " + name);
            break;
	}
        case fontWidth:{
            ostringstream temp;
            temp << "Font Width: " << Configuration::getMenuFontWidth();
            setText(temp.str());
            break;
        }
        case fontHeight:{
            ostringstream temp;
            temp << "Font Height: " << Configuration::getMenuFontHeight();
            setText(temp.str());
            break;
        }
        default: break;
    }
    if (lblue < 255){
        lblue += 5;
    }

    if (rblue < 255){
        rblue += 5;
    }

    if (lgreen < 255){
        lgreen += 5;
    }

    if (rgreen < 255){
        rgreen += 5;
    }
}

void OptionSelectFont::run(const Menu::Context & context){
    // throw Menu::MenuException(__FILE__, __LINE__);
    /* throw something to quit back to the previous menu */
}

bool OptionSelectFont::leftKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(false);
            break;
        case fontWidth:
            Configuration::setMenuFontWidth(Configuration::getMenuFontWidth() - 1);
            break;
        case fontHeight:
            Configuration::setMenuFontHeight(Configuration::getMenuFontHeight() - 1);
            break;
        default:
            break;
    }
    lblue = lgreen = 0;
    return true;
}

bool OptionSelectFont::rightKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(true);
            break;
        case fontWidth:
            Configuration::setMenuFontWidth(Configuration::getMenuFontWidth() + 1);
            break;
        case fontHeight:
            Configuration::setMenuFontHeight(Configuration::getMenuFontHeight() + 1);
            break;
        default:
            break;
    }
    rblue = rgreen = 0;
    return true;
}

static bool saneFont(const Util::ReferenceCount<Menu::FontInfo> & info){
    class Context: public Loader::LoadingContext {
    public:
        Context(const Util::ReferenceCount<Menu::FontInfo> & info):
            info(info),
            isok(false){
            }

        bool ok(){
            try{
                const Font & font = info->get();
                return font.textLength("A") != 0 &&
                    font.getHeight() != 0;
            } catch (const Exception::Base & ignore){
                return true;
            }
        }

        virtual void load(){
            isok = ok();
        }

        const Util::ReferenceCount<Menu::FontInfo> & info;
        bool isok;
    };

    if (info == NULL){
        return true;
    }

    Context context(info);
    /* an empty Info object, we don't really care about it */
    Loader::Info level("Loading Font", Filesystem::AbsolutePath());
    Loader::loadScreen(context, level, Loader::SimpleCircle);
    return context.isok;
}

void OptionSelectFont::nextIndex(bool forward){
    if (fonts.size() == 0){
        return;
    }
    
    int index = 0;
    for (unsigned int i = 0 ; i < fonts.size() ; ++i){
        if ((Configuration::getMenuFont() == NULL && fonts[i] == NULL) ||
            ((Configuration::getMenuFont() != NULL && fonts[i] != NULL) &&
             (*Configuration::getMenuFont() == *fonts[i]))){
            index = i;
        }
    }

    if (forward){
	index++;
	if (index >= (int) fonts.size()){
	    index = 0;
	}
    } else {
	index--;
	if (index < 0){
	    index = (int)fonts.size()-1;
	}
    }

    while (!saneFont(fonts[index])){
        Global::debug(0) << "Warning: erasing font `" << fonts[index]->getName() << "'" << endl;
        int where = 0;
        vector<Util::ReferenceCount<Menu::FontInfo> >::iterator it;
        for (it = fonts.begin(); it != fonts.end() && where != index; it++, where++){
        }
        fonts.erase(it);
        if (index >= (int) fonts.size()){
            index = fonts.size() - 1;
        }
    }

    Configuration::setMenuFont(fonts[index]);

    /* FIXME */
    /*
    if (fonts[index] == "Default"){
	Configuration::setMenuFont("");
    } else {
	Configuration::setMenuFont(fonts[index]);
    }
    */
}

OptionSpeed::OptionSpeed(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token),
name("")
    /*
lblue(255),
lgreen(255),
rblue(255),
rgreen(255)
*/{
    setRunnable(false);

    if ( *token != "speed" )
        throw LoadException(__FILE__, __LINE__, "Not speed option");

    readName(token);
}

OptionSpeed::~OptionSpeed(){
	// Nothing
}


std::string OptionSpeed::getText() const {
    ostringstream out;
    out << MenuOption::getText() << ": " << Configuration::getGameSpeed();
    return out.str();
}

void OptionSpeed::logic(){
    /*
	//ostringstream temp;
	char temp[255];
	sprintf( temp, "%s: %0.2f", name.c_str(), MenuGlobals::getGameSpeed() );
	setText(std::string(temp));
        */
	
}

void OptionSpeed::run(const Menu::Context & context){
}

bool OptionSpeed::leftKey(){
    Configuration::setGameSpeed(Configuration::getGameSpeed() - 0.05);
    if (Configuration::getGameSpeed() < 0.1){
        Configuration::setGameSpeed(0.1);
    }
    return false;
}
bool OptionSpeed::rightKey(){
    Configuration::setGameSpeed(Configuration::getGameSpeed() + 0.05);
    // rblue = rgreen = 0;
    return false;
}

OptionTabMenu::OptionTabMenu(const Gui::ContextBox & parent, const Token *token, const Menu::OptionFactory & factory):
MenuOption(parent, token),
menu(0){
    if (token->numTokens() == 1){
        std::string temp;
        token->view() >> temp;
        menu = new Menu::Menu(Storage::instance().find(Filesystem::RelativePath(temp)), factory, Menu::Renderer::Tabbed);
    } else {
        menu = new Menu::Menu(token, factory, Menu::Renderer::Tabbed);
    }

    // this->setText(menu->getName());
    // token->print("Menu: ");
    const Token * tok = token->findToken("_/name");
    if (tok != NULL){
        std::string name;
        tok->view() >> name;
        // Global::debug(0, "menu") << "Menu name: " << name << endl;
        this->setText(name);
    } else {
        // No name?
        throw LoadException(__FILE__, __LINE__, "Menu has no name");
    }
}

OptionTabMenu::~OptionTabMenu(){
    // Delete our menu
    if (menu){
        delete menu;
    }
}

void OptionTabMenu::logic(){
	// Nothing
}

void OptionTabMenu::run(const Menu::Context & context){
    // Do our new menu
    // menu->run(context);
    try{
        menu->run(context);
    } catch (const Exception::Return & ignore){
        throw Menu::Reload(__FILE__, __LINE__);
    }
}

OptionSound::OptionSound(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if (*token != "sound" ){
        throw LoadException(__FILE__, __LINE__, "Not a sound option");
    }

    readName(token);
    originalName = getName();
}

OptionSound::~OptionSound(){
}

void OptionSound::logic(){
    ostringstream temp;
    temp << originalName << ": " << Configuration::getSoundVolume();
    setText(temp.str());
}

void OptionSound::run(const Menu::Context & context){
}

void OptionSound::changeSound(int much){
    int volume = Configuration::getSoundVolume();
    volume += much;
    if (volume < 0){
        volume = 0;
    }

    if (volume > 100){
        volume = 100;
    }

    Configuration::setSoundVolume(volume);
}

bool OptionSound::leftKey(){
    changeSound(-1);
    
    lblue = lgreen = 0;
    return true;
}

bool OptionSound::rightKey(){
    changeSound(+1);

    rblue = rgreen = 0;
    return true;
}

OptionMusic::OptionMusic(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token)
    /*
    ,
lblue(255),
lgreen(255),
rblue(255),
rgreen(255)
*/
{
    setRunnable(false);

    if (*token != "music" ){
        throw LoadException(__FILE__, __LINE__, "Not a music option");
    }

    readName(token);
    originalName = getName();
}

void OptionMusic::logic(){
    ostringstream temp;
    temp << originalName << ": " << Configuration::getMusicVolume();
    setText(temp.str());
}

void OptionMusic::run(const Menu::Context & context){
}

void OptionMusic::changeMusic(int much){
    int volume = Configuration::getMusicVolume();
    volume += much;
    if (volume < 0){
        volume = 0;
    }

    if (volume > 100){
        volume = 100;
    }

    Configuration::setMusicVolume(volume);
    Music::setVolume((double) volume / 100.0);
}

bool OptionMusic::leftKey(){
    changeMusic(-1);
    
    // lblue = lgreen = 0;
    return true;
}

bool OptionMusic::rightKey(){
    changeMusic(+1);
    
    // lblue = lgreen = 0;
    return true;
}

OptionMusic::~OptionMusic(){
}

OptionLanguage::OptionLanguage(const Gui::ContextBox & parent, const Token * token):
MenuOption(parent, token){
    readName(token);
#if 0
    const Token * start = token->getRootParent();
    vector<const Token*> tokens = start->findTokens("*/language");
    vector<string> all;
    for (vector<const Token*>::iterator it = tokens.begin(); it != tokens.end(); it++){
        string language;
        const Token * token = *it;
        if (token->match("language", language)){
            all.push_back(language);
        }
    }
    sort(all.begin(), all.end());
    unique_copy(all.begin(), all.end(), back_insert_iterator<vector<string> >(languages));
    // Global::debug(0) << "Found " << languages.size() << " languages" << endl;
#endif
}

void OptionLanguage::run(const Menu::Context & context){
    class LanguageOption: public MenuOption {
    public:
        LanguageOption(const Gui::ContextBox & parent, const string & language):
        MenuOption(parent, NULL){
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

    Util::NewReferenceCount<Menu::DefaultRenderer> renderer;
    Menu::Menu temp(renderer);
    Util::ReferenceCount<Menu::FontInfo> info(new Menu::RelativeFontInfo(Font::getDefaultFontPath(), 24, 24));
    temp.setFont(info);

    const Gui::ContextBox & box = renderer->getBox();

    vector<string> languages = context.getLanguages();
    for (vector<string>::iterator it = languages.begin(); it != languages.end(); it++){
        temp.addOption(new LanguageOption(box, *it));
    }

    try {
        temp.run(context);
    } catch (const Exception::Return & ignore){
    } catch (const Menu::MenuException & ex){
    }

    throw Menu::Reload(__FILE__, __LINE__);
    // throw Exception::Return(__FILE__, __LINE__);
}
    
void OptionLanguage::logic(){
}


OptionJoystick::OptionJoystick(const Gui::ContextBox & parent, const Token *token):
MenuOption(parent, token){
    setRunnable(true);

    if (*token != "joystick" ){
        throw LoadException(__FILE__, __LINE__, "Not a joystick option");
    }

    readName(token);
}

void OptionJoystick::logic(){
}

class JoystickLogicDraw: public Util::Logic, public Util::Draw {
public:
    enum Inputs{
        Exit
    };

    static const int marginX = 20;

    JoystickLogicDraw(int id, const Util::ReferenceCount<Joystick> & joystick, const ::Menu::Context & context):
    id(id),
    joystick(joystick),
    quit(false),
    context(context, Menu::Context()){
        input.set(Keyboard::Key_ESC, Exit);
    }

    const int id;
    Util::ReferenceCount<Joystick> joystick;
    bool quit;
    Menu::Context context;
    InputMap<Inputs> input;

    void doInput(){
        vector<InputMap<Inputs>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
        for (vector<InputMap<Inputs>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Inputs>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Exit){
                    quit = true;
                    // context.finish();
                }
            }
        }
    }

    virtual void run(){
        doInput();
        context.act();
    }

    bool done(){
        return quit;
    }

    double ticks(double system){
        return Global::ticksPerSecond(system) * 30;
    }

    void drawButtons(const Font & font, const Graphics::Bitmap & buffer, int y){
        Graphics::Color color = Graphics::makeColor(255, 255, 255);
        font.printf(marginX, y, color, buffer, "Up: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Down: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Left: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Right: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Button1: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Button2: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Button3: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Button4: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Button5: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Button6: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Select: ", 0); y += font.getHeight() + 5;
        font.printf(marginX, y, color, buffer, "Quit: ", 0); y += font.getHeight() + 5;
    }
    
    void draw(const Graphics::Bitmap & buffer){
        const Font & font = Menu::menuFontParameter.current()->get();
        Graphics::StretchedBitmap work(640, 480, buffer, Graphics::StretchedBitmap::NoClear, Graphics::qualityFilterName(Configuration::getQualityFilter()));
        work.start();
        context.renderBackground(work);
        
        /* FIXME: scale the joystck name down to fit */
        font.printf(marginX, 1, Graphics::makeColor(255, 255, 255), work, "Joystick %d: %s", 0, id, joystick->getName().c_str());

        drawButtons(font, work, 1 + font.getHeight() + 5);
        context.renderForeground(work);

        work.finish();
    }
};

namespace{
    struct Axis{
        Axis():
            stick(0),
            axis(0),
            first(0),
            set(false),
            last(0),
            lastMotion(0){
            }

        int stick;
        int axis;

        /* first value from this axis. we assume that
         * the first value is sufficiently close to 'zero' which 
         * can be any value, but most likely will either be
         * -1, 0, or 1
         */
        double first;
        /* true if first has been set */
        bool set;

        /* keep track of last value for this axis */
        double last;

        /* last time an event was produced (or at least last
         * time we read it). it might be better to get the actual
         * time from the event itself.
         */
        uint64_t lastMotion;
    };
}

static void runJoystickMenu(int joystickId, const Util::ReferenceCount<Joystick> & joystick, const ::Menu::Context & context){
    Util::NewReferenceCount<Menu::DefaultRenderer> renderer;
    Menu::Menu menu(renderer);

    Gui::ContextBox & box = renderer->getBox();
    box.setListType(ContextBox::Normal);
    Gui::ListValues attributes(box.getListValues());
    attributes.setDistanceFade(false);
    box.setListValues(attributes);
    menu.setPosition(Gui::Coordinate(Gui::RelativePoint(-0.8, -0.3),
                                     Gui::RelativePoint(0, 0.8)));

#define WAIT_TIME_MS (0.7 * 1000)
#define WAIT_TIME_AXIS_MS (1 * 1000)
#define AXIS_THRESHOLD 0.7

    class JoystickButton: public MenuOption {
    public:
        JoystickButton(const Menu::Menu & menu, const Gui::ContextBox & parent, const Util::ReferenceCount<Joystick> & joystick, const string & name, Joystick::Key key):
        MenuOption(parent, NULL),
        menu(menu),
        name(name),
        joystick(joystick),
        key(key){
            setText(name);
            setInfoText(name);
        }

        const Menu::Menu & menu;
        string name;
        Util::ReferenceCount<Joystick> joystick;
        Joystick::Key key;

        class ButtonListener: public JoystickListener {
        public:
            ButtonListener(const Util::ReferenceCount<Joystick> & joystick):
            touched(false),
            done(false),
            chosen(-1),
            chosenAxis(NULL){
                map<int, map<int, double> > axisValues = joystick->getCurrentAxisValues();
                for (map<int, map<int, double> >::iterator it = axisValues.begin(); it != axisValues.end(); it++){
                    int stick = it->first;
                    const map<int, double> & subMap = it->second;
                    for (map<int, double>::const_iterator it = subMap.begin(); it != subMap.end(); it++){
                        int axis = it->first;
                        double value = it->second;

                        Axis use;
                        use.stick = stick;
                        use.axis = axis;
                        use.first = value;
                        use.set = true;
                        use.last = value;
                        use.lastMotion = 0;
                        this->axis.push_back(use);
                    }
                }
            }

            map<int, uint64_t> presses;
            map<int, bool> pressed;

            /* true if a button/axis is moved. flips to false when queried */
            bool touched;

            vector<Axis> axis;

            bool done;
            int chosen;

            Axis * chosenAxis;

            bool wasTouched(){
                bool out = touched;
                touched = false;
                return out;
            }

            Axis & getAxis(int stick, int axis){
                for (vector<Axis>::iterator it = this->axis.begin(); it != this->axis.end(); it++){
                    Axis & use = *it;
                    if (use.stick == stick && use.axis == axis){
                        return use;
                    }
                }
                Axis out;
                out.stick = stick;
                out.axis = axis;
                this->axis.push_back(out);
                return getAxis(stick, axis);
            }

            const vector<Axis> & getAllAxis() const {
                return axis;
            }

            const map<int, uint64_t> & getPresses() const {
                return presses;
            }

            int getButton() const {
                return chosen;
            }

            Axis * getChosenAxis() const {
                return chosenAxis;
            }

            virtual ~ButtonListener(){
            }

            bool isDone() const {
                return (getButton() != -1 && !anyPressed()) ||
                       (getChosenAxis() != NULL);
            }

            void choose(){
                uint64_t now = System::currentMilliseconds();
                for (map<int, uint64_t>::const_iterator it = presses.begin(); it != presses.end(); it++){
                    uint64_t what = it->second;
                    if (what != 0 && now - what > WAIT_TIME_MS){
                        chosen = it->first;
                    }
                }

                for (vector<Axis>::iterator it = axis.begin(); it != axis.end(); it++){
                    Axis & use = *it;
                    if (fabs(use.last - use.first) > AXIS_THRESHOLD &&
                        now - use.lastMotion > WAIT_TIME_AXIS_MS){
                        chosenAxis = &use;
                    }
                }
            }

            bool anyPressed() const {
                for (map<int, bool>::const_iterator it = pressed.begin(); it != pressed.end(); it++){
                    if (it->second){
                        return true;
                    }
                }
                return false;
            }

            virtual void pressButton(Joystick * from, int button){
                touched = true;
                pressed[button] = true;
                presses[button] = System::currentMilliseconds();
            }

            virtual void releaseButton(Joystick * from, int button){
                presses[button] = 0;
                pressed[button] = false;
            }

            /* either all increasing or all decreasing */
            bool monotonic(const vector<double> & what){
                if (what.size() == 0){
                    return true;
                }

                double first = what[0];
                int direction = -1;
                vector<double>::const_iterator it = what.begin();
                it++;
                for (/**/; it != what.end(); it++){
                    switch (direction){
                        case -1: {
                            if (*it < first){
                                direction = 1;
                            } else if (*it > first){
                                direction = 2;
                            }

                            first = *it;
                            break;
                        }
                        case 1: {
                            if (*it > first){
                                return false;
                            }
                            break;
                        }
                        case 2: {
                            if (*it < first){
                                return false;
                            }
                            break;
                        }
                    }
                }

                return true;
            }

            virtual void axisMotion(Joystick * from, int stick, int axis, double motion){
                touched = true;
                Axis & use = getAxis(stick, axis);
                if (!use.set){
                    use.first = motion;
                    use.set = true;
                }
                    
                use.last = motion;
                use.lastMotion = System::currentMilliseconds();

                /*
                const double AXIS_THRESHOLD = 0.5;

                Global::debug(0) << "stick " << stick << " axis " << axis << " first " << use.first << " last " << use.last << " diff " << fabs(use.last - use.first) << std::endl;
                if (fabs(use.last - use.first) > AXIS_THRESHOLD){
                    Global::debug(0) << "stick " << stick << " axis " << axis << " motion " << motion << std::endl;
                }
                */
            }

            virtual void hatMotion(Joystick * from, int motion){
            }
        };

        void logic(){
            ostringstream out;

            int stick, axis;
            double low, high;
            if (joystick->getAxis(key, stick, axis, low, high)){
                out << name << ": " << stick << "/" << axis << "/";
                if (low < 0){
                    out << "-";
                } else {
                    out << "+";
                }
            } else {
                int button = joystick->getButton(key);
                out << name << ":  ";
                if (button != -1){
                    out << joystick->getButton(key);
                } else {
                    out << "unset";
                }
            }
            setText(out.str());
        }

        void run(const Menu::Context & context){

            class SetButton: public Util::Logic, public Util::Draw {
            public:
                SetButton(const Menu::Context & context, const Menu::Menu & menu, const string & name, const Util::ReferenceCount<Joystick> & joystick):
                context(context),
                menu(menu),
                name(name),
                listener(joystick),
                joystick(joystick),
                startingTime(System::currentMilliseconds()),
                maxTime(5000){
                    input.set(Keyboard::Key_ESC, 0);
                    joystick->addListener(&listener);

                    vector<Graphics::BlendPoint> points;
                    points.push_back(Graphics::BlendPoint(Graphics::makeColor(255, 0, 0), 30));
                    points.push_back(Graphics::BlendPoint(Graphics::makeColor(0, 255, 0), 25));
                    points.push_back(Graphics::BlendPoint(Graphics::makeColor(255, 255, 255), 0));
                    colors = Graphics::blend_palette(points);
                }
            
                const Menu::Context & context;
                const Menu::Menu & menu;
                string name;
                ButtonListener listener;
                InputMap<int> input;
                Util::ReferenceCount<Joystick> joystick;
                vector<Graphics::Color> colors;
                uint64_t startingTime;
                const uint64_t maxTime;

                virtual ~SetButton(){
                    joystick->removeListener(&listener);
                }

                virtual void showTimeLeft(const Graphics::Bitmap & screen){
                    int x1 = 1;
                    int y1 = 1;
                    int x2 = screen.getWidth() * (1 - (double) (System::currentMilliseconds() - startingTime) / (double) maxTime);
                    if (x2 < x1){
                        x2 = x1;
                    }

                    int y2 = 10;

                    int color = (1.0 - (double) (System::currentMilliseconds() - startingTime) / maxTime) * colors.size();
                    if (color < 0){
                        color = 0;
                    }
                    if (color >= (int) colors.size()){
                        color = colors.size() - 1;
                    }

                    screen.rectangleFill(x1, y1, x2, y2, colors[color]);
                }

                void setButton(Joystick::Key key){
                    int button = listener.getButton();
                    if (button != -1){
                        Global::debug(1) << "Chosen button " << listener.getButton() << std::endl;
                        joystick->setCustomButton(listener.getButton(), key);
                    } else {
                        Axis * axis = listener.getChosenAxis();
                        double rangeLow = 0;
                        double rangeHigh = 0;
                        /* stick went negative and went to -1 */
                        if (axis->first <= 0 && axis->last < axis->first){
                            rangeLow = -1;
                            rangeHigh = -AXIS_THRESHOLD;
                        /* stick started at negative and went positive, possibly
                         * not above 0.
                         */
                        } else if (axis->first < 0 && axis->last > axis->first){
                            rangeLow = 0;
                            rangeHigh = 1;
                        /* stick started positive and went to 1 */
                        } else if (axis->first >= 0 && axis->last > axis->first){
                            rangeLow = AXIS_THRESHOLD;
                            rangeHigh = 1;
                        /* stick started positive and went towards -1 */
                        } else if (axis->first > 0 && axis->last < axis->first){
                            rangeLow = -1;
                            rangeHigh = 0;
                        }

                        Global::debug(0) << "Set stick " << axis->stick << " axis " << axis->axis << " [" << rangeLow << ", " << rangeHigh << "]" << std::endl;

                        joystick->setCustomAxis(key, axis->stick, axis->axis, rangeLow, rangeHigh);
                    }
                }

                double ticks(double system){
                    return system * Global::ticksPerSecond(30);
                }

                bool done(){
                    return listener.isDone();
                }

                void run(){
                    vector<InputMap<int>::InputEvent> out = InputManager::getEvents(input, InputSource(true));
                    for (vector<InputMap<int>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                        const InputMap<int>::InputEvent & event = *it;
                        if (event.enabled){
                            if (event.out == 0){
                                throw Exception::Return(__FILE__, __LINE__);
                            }
                        }
                    }

                    listener.choose();

                    if (listener.wasTouched()){
                        startingTime = System::currentMilliseconds();
                    }

                    if (System::currentMilliseconds() - startingTime >= maxTime){
                        throw Exception::Return(__FILE__, __LINE__);
                    }
                }
    
                void draw(const Graphics::Bitmap & buffer){
                    Graphics::StretchedBitmap work(640, 480, buffer, Graphics::StretchedBitmap::NoClear, Graphics::qualityFilterName(Configuration::getQualityFilter()));
                    work.start();
                    menu.render(context, work);
                    const Font & font = Menu::menuFontParameter.current()->get();
                    Gui::RelativePoint start(0.2, -0.3);
                    Gui::RelativePoint end(0.85, 0.8);
                    int x = start.getX();
                    int y = start.getY();
                    // font.printfWrap(x, y - font.getHeight() * 2 - 5, Graphics::makeColor(255, 255, 255), work, end.getX() - start.getX(), "Press and hold a button", 0);
                    font.printf(x, y - 18 - 5, 18, 18, Graphics::makeColor(255, 255, 255), work, "Press and hold a button", 0);
                    work.translucent(0, 0, 0, 128).rectangleFill(x, y, end.getX(), end.getY(), Graphics::makeColor(0, 0, 0));
                    uint64_t now = System::currentMilliseconds();
                    const map<int, uint64_t> & presses = listener.getPresses();
                    for (map<int, uint64_t>::const_iterator it = presses.begin(); it != presses.end(); it++){
                        int button = it->first;
                        uint64_t time = it->second;
                        if (time > 0){
                            int delta = now - time;
                            if (delta > WAIT_TIME_MS){
                                delta = WAIT_TIME_MS;
                            }

                            /* this shouldn't happen... */
                            if (delta < 0){
                                delta = 0;
                            }
                            Graphics::Color color;
                            color = Graphics::makeColor((int)(255.0 * (double) delta / (WAIT_TIME_MS)),
                                                        0, 255);
                            if (button == listener.getButton()){
                                color = Graphics::makeColor(255, 255, 255);
                            }
                            ostringstream text;
                            text << name << ":  " << button;
                            font.printf(x, y, color, work, text.str(), 0);
                            y += font.getHeight() + 5;
                        }
                    }
                    
                    const vector<Axis> & axis = listener.getAllAxis();
                    for (vector<Axis>::const_iterator it = axis.begin(); it != axis.end(); it++){
                        const Axis & use = *it;

                        if (fabs(use.last - use.first) > AXIS_THRESHOLD){
                            int delta = now - use.lastMotion;
                            if (delta > WAIT_TIME_AXIS_MS){
                                delta = WAIT_TIME_AXIS_MS;
                            }

                            /* this shouldn't happen... */
                            if (delta < 0){
                                delta = 0;
                            }
                            Graphics::Color color;
                            color = Graphics::makeColor((int)(255.0 * (double) delta / (WAIT_TIME_AXIS_MS)),
                                                        0, 255);
                            if (&use == listener.getChosenAxis()){
                                color = Graphics::makeColor(255, 255, 255);
                            }
                            ostringstream text;
                            text << name << ":  stick " << use.stick << " axis " << use.axis;
                            if (use.last > use.first){
                                text << " +";
                            } else {
                                text << " -";
                            }
                            font.printf(x, y, 18, 18, color, work, text.str(), 0);
                            y += font.getHeight() + 5;

                        }

                    }

                    showTimeLeft(work);

                    work.finish();
                }

                void wait(){
                    while (listener.anyPressed()){
                        InputManager::poll();
                        Util::rest(1);
                    }
                }
            };

            Global::debug(1) << "Set button " << getName() << std::endl;
            SetButton set(context, menu, name, joystick);
            try{
                Util::standardLoop(set, set);
                set.setButton(key);
            } catch (const Exception::Return & quit){
            }

            set.wait();
        }
    };

    menu.addOption(new JoystickButton(menu, box, joystick, "Up", Joystick::Up));
    menu.addOption(new JoystickButton(menu, box, joystick, "Down", Joystick::Down));
    menu.addOption(new JoystickButton(menu, box, joystick, "Left", Joystick::Left));
    menu.addOption(new JoystickButton(menu, box, joystick, "Right", Joystick::Right));
    menu.addOption(new JoystickButton(menu, box, joystick, "Button1", Joystick::Button1));
    menu.addOption(new JoystickButton(menu, box, joystick, "Button2", Joystick::Button2));
    menu.addOption(new JoystickButton(menu, box, joystick, "Button3", Joystick::Button3));
    menu.addOption(new JoystickButton(menu, box, joystick, "Button4", Joystick::Button4));
    menu.addOption(new JoystickButton(menu, box, joystick, "Button5", Joystick::Button5));
    menu.addOption(new JoystickButton(menu, box, joystick, "Button6", Joystick::Button6));
    menu.addOption(new JoystickButton(menu, box, joystick, "Start", Joystick::Start));
    menu.addOption(new JoystickButton(menu, box, joystick, "Quit", Joystick::Quit));

    try {
        menu.run(context);
    } catch (const Exception::Return & ignore){
    } catch (const Menu::MenuException & ex){
    }
}

void OptionJoystick::run(const Menu::Context & context){
    class JoystickOption: public MenuOption {
    public:
        JoystickOption(const Gui::ContextBox & parent, int id, const Util::ReferenceCount<Joystick> & joystick):
        MenuOption(parent, NULL),
        joystick(joystick),
        id(id){
            ostringstream out;
            out << "Joystick " << (id + 1);
            setText(out.str());
            setInfoText(joystick->getName());
        }
        
        const Util::ReferenceCount<Joystick> joystick;
        const int id;

        virtual void logic(){
        }

        virtual void run(const ::Menu::Context & context){
            runJoystickMenu(id, joystick, context);
            /*
            JoystickLogicDraw mainLoop(id, joystick, context);
            Util::standardLoop(mainLoop, mainLoop);
            */
            throw ::Menu::MenuException(__FILE__, __LINE__);
        }
    };

    Util::NewReferenceCount<Menu::DefaultRenderer> renderer;
    Menu::Menu menu(renderer);
    /*
    Util::ReferenceCount<Menu::FontInfo> info(new Menu::RelativeFontInfo(Global::DEFAULT_FONT, 24, 24));
    temp.setFont(info);
    */

    Gui::ContextBox & box = renderer->getBox();
    box.setListType(ContextBox::Normal);

    map<int, Util::ReferenceCount<Joystick> > joysticks = InputManager::getJoysticks();
    for (map<int, Util::ReferenceCount<Joystick> >::iterator it = joysticks.begin(); it != joysticks.end(); it++){
        menu.addOption(new JoystickOption(box, it->first, it->second));
    }

    if (joysticks.size() == 0){
        menu.addOption(new OptionDummy(box, "No joysticks found!"));
    }

    try {
        menu.run(context);
    } catch (const Exception::Return & ignore){
    } catch (const Menu::MenuException & ex){
    }
}

OptionJoystick::~OptionJoystick(){
}
