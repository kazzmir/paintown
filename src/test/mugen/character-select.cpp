#include "character-select.h"

#include <iostream>

#include "util/timedifference.h"
#include "util/trans-bitmap.h"
#include "mugen/ast/all.h"
#include "mugen/sound.h"

using namespace Mugen;

// Cell static members
PaintownUtil::ReferenceCount<MugenSprite> Cell::background = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::randomIcon = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player1ActiveCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player1DoneCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player2ActiveCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player2DoneCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
bool Cell::blinkCursor = false;
int Cell::blinkTime = 0;
Mugen::Effects Cell::effects = Mugen::Effects();
int Cell::offsetX = 0;
int Cell::offsetY = 0;

bool TeamMenu::wrapping = false;

SelectFont::SelectFont():
font(PaintownUtil::ReferenceCount<MugenFont>(NULL)),
bank(0),
position(0){
}

SelectFont::SelectFont(PaintownUtil::ReferenceCount<MugenFont> font, int bank, int position):
font(font),
bank(bank),
position(position){
}

SelectFont::SelectFont(const SelectFont & copy):
font(copy.font),
bank(copy.bank),
position(copy.position){
}

SelectFont::~SelectFont(){
}

const SelectFont & SelectFont::operator=(const SelectFont & copy){
    font = copy.font;
    bank = copy.bank;
    position = copy.position;
    return * this;
}

void SelectFont::draw(int x, int y, const std::string & text, const Graphics::Bitmap & work){
    if (font != NULL){
        font->render(x, y, position, bank, work, text);
    }
}

FontHandler::FontHandler():
state(Normal),
x(0),
y(0),
ticker(0),
blinkTime(10){
}

FontHandler::~FontHandler(){
}

void FontHandler::act(){
    switch(state){
        case Blink:
            ticker++;
            if (ticker >= (blinkTime*2)){
                ticker = 0;
            }
            break;
        case Normal:
        case Done:
        default:
            break;
    }
}

void FontHandler::draw(const Graphics::Bitmap & work){
    draw(text, work);
}

void FontHandler::draw(const std::string & text, const Graphics::Bitmap & work){
    switch(state){
        default:
        case Normal:
            active.draw(x, y, text, work);
            break;
        case Blink:
            if (ticker < blinkTime){
                active.draw(x, y, text, work);
            } else if (ticker >= blinkTime){
                active2.draw(x, y, text, work);
            }
            break;
        case Done:
            done.draw(x, y, text, work);
            break;
    }
}

Cell::Cell(unsigned int index, const Gui::SelectListInterface * parent):
index(index),
parent(parent),
empty(true),
isRandom(false),
flash(0){
}

Cell::~Cell(){
}

void Cell::act(){
    if (flash){
        flash--;
    }
}

void Cell::draw(int x, int y, int width, int height, const Graphics::Bitmap & work, const Font & font) const{
    if (background != NULL){
        background->render(x, y, work);
    }
    
    // If random draw random icon otherwise draw character image if available
    if (isRandom){
        if (randomIcon != NULL){
            randomIcon->render(x + offsetX, y + offsetY, work, effects);
        }
    } else if (!empty) {
        character.drawIcon(x + offsetX, y + offsetY, work, effects);
    }
    
    if (parent->getCurrentIndex(0) == parent->getCurrentIndex(1) && parent->getCurrentIndex(0) == index){
        // Blink cursors
        if (parent->getCurrentState(0) == Gui::SelectListInterface::Active && parent->getCurrentState(1) == Gui::SelectListInterface::Active){
            drawPlayer1Cursor(x, y, work);
            drawPlayer2Cursor(x, y, work, true);
        } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Active && parent->getCurrentState(1) == Gui::SelectListInterface::Done){
            drawPlayer2Cursor(x, y, work);
            drawPlayer1Cursor(x, y, work);
        } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Done && parent->getCurrentState(1) == Gui::SelectListInterface::Active){
            drawPlayer1Cursor(x, y, work);
            drawPlayer2Cursor(x, y, work);
        } else {
            drawPlayer2Cursor(x, y, work);
            drawPlayer1Cursor(x, y, work);
        }
    } else if (parent->getCurrentIndex(0) == index){
        drawPlayer1Cursor(x, y, work);
    } else if (parent->getCurrentIndex(1) == index){
        drawPlayer2Cursor(x, y, work);
    }
    
    // Flash
    if (flash){
        // FIXME hot pink shows up after white
        Graphics::Bitmap::transBlender( 0, 0, 0, int(25.5 * flash) );
        work.translucent().rectangleFill(x-1, y-1, x-1+width, y-1+height, Graphics::makeColor(255,255,255));
    }
}

bool Cell::isEmpty() const{
    return empty;
}

void Cell::setRandom(bool r){
    empty = !r;
    isRandom = r;
}

void Cell::setCharacter(const Mugen::ArcadeData::CharacterInfo & character){
    this->character = character;
    empty = false;
}

void Cell::select(){
    flash = 10;
}

void Cell::setBackground(PaintownUtil::ReferenceCount<MugenSprite> background){
    Cell::background = background;
}

void Cell::setRandomIcon(PaintownUtil::ReferenceCount<MugenSprite> randomIcon){
    Cell::randomIcon = randomIcon;
}

void Cell::setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player1ActiveCursor = cursor;
}

void Cell::setPlayer1DoneCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player1DoneCursor = cursor;
}

void Cell::setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player2ActiveCursor = cursor;
}

void Cell::setPlayer2DoneCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player2DoneCursor = cursor;
}

void Cell::drawPlayer1Cursor(int x, int y, const Graphics::Bitmap & work) const {
    if (parent->getCurrentState(0) == Gui::SelectListInterface::Disabled){
    } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Active){
        if (player1ActiveCursor != NULL){
            player1ActiveCursor->render(x, y, work);
        }
    } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Done){
        if (player1DoneCursor != NULL){
            player1DoneCursor->render(x, y, work);
        }
    }
}

void Cell::drawPlayer2Cursor(int x, int y, const Graphics::Bitmap & work, bool blink) const {
    if (parent->getCurrentState(1) == Gui::SelectListInterface::Disabled){
    } else if (parent->getCurrentState(1) == Gui::SelectListInterface::Active){
        if (blinkCursor && blink){
            // Blink
            blinkTime++;
            if (blinkTime < 10){
                if (player2ActiveCursor != NULL){
                    player2ActiveCursor->render(x, y, work);
                }
            } else if (blinkTime >= 20){
                blinkTime=0;
            }
        } else {
            if (player2ActiveCursor != NULL){
                player2ActiveCursor->render(x, y, work);
            }
        }
    } else if (parent->getCurrentState(1) == Gui::SelectListInterface::Done){
        if (player2DoneCursor != NULL){
            player2DoneCursor->render(x, y, work);
        }
    }
}


TeamMenu::TeamMenu():
current(Mugen::ArcadeData::CharacterCollection::Simultaneous),
turns(Mugen::ArcadeData::CharacterCollection::Turns2),
x(0),
y(0),
background(PaintownUtil::ReferenceCount<MugenSprite>(NULL)),
itemOffsetX(0),
itemOffsetY(0),
itemSpacingX(0),
itemSpacingY(0),
valueIconOffsetX(0),
valueIconOffsetY(0),
icon(PaintownUtil::ReferenceCount<MugenSprite>(NULL)),
emptyValueIconOffsetX(0),
emptyValueIconOffsetY(0),
emptyIcon(PaintownUtil::ReferenceCount<MugenSprite>(NULL)),
valueSpacingX(0),
valueSpacingY(0){
    itemCurrentFont.setState(FontHandler::Blink);
}

TeamMenu::~TeamMenu(){
}

void TeamMenu::act(){
    itemCurrentFont.act();
}

void TeamMenu::draw(const Graphics::Bitmap & work, bool enemy){
    // Set location
    int currentX = x, currentY = y;
    if (background != NULL){
        background->render(currentX, currentY, work);
    }
    // Regular font or enemy ?
    if (!enemy){
        titleFont.setLocation(currentX, currentY);
        titleFont.draw(work);
    } else {
        enemyTitleFont.setLocation(currentX, currentY);
        enemyTitleFont.draw(work);
    }
    
    // First item (single)
    currentX+=itemOffsetX;
    currentY+=itemOffsetY;
    if (current == Mugen::ArcadeData::CharacterCollection::Single){
        itemCurrentFont.setLocation(currentX, currentY);
        itemCurrentFont.draw("Single", work);
    } else {
        itemFont.setLocation(currentX, currentY);
        itemFont.draw("Single", work);
    }
    
    // Second item (simultaneous)
    currentX+=itemSpacingX;
    currentY+=itemSpacingY;
    if (current == Mugen::ArcadeData::CharacterCollection::Simultaneous){
        itemCurrentFont.setLocation(currentX, currentY);
        itemCurrentFont.draw("Simul", work);
    } else {
        itemFont.setLocation(currentX, currentY);
        itemFont.draw("Simul", work);
    }
    if (emptyIcon != NULL){
        // Empty icons draw two
        int emptyX = currentX + emptyValueIconOffsetX, emptyY = currentY + emptyValueIconOffsetY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
    }
    if (icon != NULL){
        // Icons draw two for Simul
        int valueX = currentX + valueIconOffsetX, valueY = currentY + valueIconOffsetY;
        icon->render(valueX, valueY, work);
        valueX += valueSpacingX;
        valueY += valueSpacingY;
        icon->render(valueX, valueY, work);
    }
    // Third item (Turns)
    currentX+=itemSpacingX;
    currentY+=itemSpacingY;
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
        case Mugen::ArcadeData::CharacterCollection::Turns3:
        case Mugen::ArcadeData::CharacterCollection::Turns4:
            itemCurrentFont.setLocation(currentX, currentY);
            itemCurrentFont.draw("Turns", work);
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            itemFont.setLocation(currentX, currentY);
            itemFont.draw("Turns", work);
            break;
    }
    if (emptyIcon != NULL){
        // Empty icons draw four
        int emptyX = currentX + emptyValueIconOffsetX, emptyY = currentY + emptyValueIconOffsetY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
    }
    if (icon != NULL){
        // Icons draw two by defualt
        int valueX = currentX + valueIconOffsetX, valueY = currentY + valueIconOffsetY;
        icon->render(valueX, valueY, work);
        valueX += valueSpacingX;
        valueY += valueSpacingY;
        icon->render(valueX, valueY, work);
        // Check which position it is on
        switch (turns){
            case Mugen::ArcadeData::CharacterCollection::Turns3:
                valueX += valueSpacingX;
                valueY += valueSpacingY;
                icon->render(valueX, valueY, work);
                break;
            case Mugen::ArcadeData::CharacterCollection::Turns4:
                valueX += valueSpacingX;
                valueY += valueSpacingY;
                icon->render(valueX, valueY, work);
                valueX += valueSpacingX;
                valueY += valueSpacingY;
                icon->render(valueX, valueY, work);
                break;
            case Mugen::ArcadeData::CharacterCollection::Turns2:
            default:
                break;
        }
    }
}

bool TeamMenu::up(){
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Single:
            if (wrapping){
                current = Mugen::ArcadeData::CharacterCollection::Turns2;
                return true;
            }
            break;
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
            current = Mugen::ArcadeData::CharacterCollection::Single;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            current = Mugen::ArcadeData::CharacterCollection::Simultaneous;
            return true;
            break;
        default:
            break;
    };
    return false;
}

bool TeamMenu::down(){
    
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Single:
            current = Mugen::ArcadeData::CharacterCollection::Simultaneous;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
            current = Mugen::ArcadeData::CharacterCollection::Turns2;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            if (wrapping){
                current = Mugen::ArcadeData::CharacterCollection::Single;
                return true;
            }
            break;
        default:
            break;
    };
    return false;
}

bool TeamMenu::left(){
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            if (valueSpacingX >= 0){
                return valueLess();
            } else if (valueSpacingX < 0){
                return valueMore();
            }
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            return false;
            break;
    };
    return false;
}

bool TeamMenu::right(){
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            if (valueSpacingX >= 0){
                return valueMore();
            } else if (valueSpacingX < 0){
                return valueLess();
            }
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            return false;
            break;
    };
    return false;
}

const Mugen::ArcadeData::CharacterCollection::Type & TeamMenu::select(){
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            return turns;
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            return current;
            break;
    }
}

bool TeamMenu::valueLess(){
    switch (turns){
        case Mugen::ArcadeData::CharacterCollection::Turns4:
            turns = Mugen::ArcadeData::CharacterCollection::Turns3;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns3:
            turns = Mugen::ArcadeData::CharacterCollection::Turns2;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns2:
        default:
            return false;
            break;
    }
}

bool TeamMenu::valueMore(){
    switch (turns){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            turns = Mugen::ArcadeData::CharacterCollection::Turns3;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns3:
            turns = Mugen::ArcadeData::CharacterCollection::Turns4;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns4:
        default:
            return false;
            break;
    }
}

CharacterSelect::CharacterSelect(const Filesystem::AbsolutePath & file):
file(file),
gridX(0),
gridY(0),
gridPositionX(0),
gridPositionY(0),
player1Start(0),
player2Start(0),
portrait1OffsetX(0),
portrait1OffsetY(0),
portrait2OffsetX(0),
portrait2OffsetY(0),
randomSwitchTime(4),
player1SwitchTime(0),
player2SwitchTime(0),
player1CurrentRandom(0),
player2CurrentRandom(0),
nextCell(0),
randomStage(true),
currentStage(0){
    Global::debug(0) << "Got file: " << file.path() << std::endl;
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::init(){
    try{
        Filesystem::AbsolutePath baseDir = file.getDirectory();

        Global::debug(1) << baseDir.path() << std::endl;

        if (file.isEmpty()){
            std::ostringstream out;
            out << "Cannot locate character select definition file for: " << file.path();
            throw MugenException( out.str(), __FILE__, __LINE__);
        }

        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Util::parseDef(file.path()));
        diff.endTime();
        Global::debug(1) << "Parsed mugen file " + file.path() + " in" + diff.printTime("") << std::endl;

        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            head = Mugen::Util::fixCase(head);
            if (head == "info"){
                /* Nothing right now */
            } else if (head == "files"){
                class FileWalker: public Ast::Walker{
                public:
                    FileWalker(Mugen::CharacterSelect & select, const Filesystem::AbsolutePath & baseDir):
                        select(select),
                        baseDir(baseDir){
                        }

                    Mugen::CharacterSelect & select;
                    const Filesystem::AbsolutePath & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            std::string sffFile;
                            simple.view() >> sffFile;
                            Global::debug(1) << "Got Sprite File: '" << sffFile << "'" << std::endl;
                            Mugen::Util::readSprites(Mugen::Util::findFile(Filesystem::RelativePath(sffFile)), Filesystem::AbsolutePath(), select.sprites, true);
                            /*for( Mugen::SpriteMap::iterator i = select.sprites.begin() ; i != select.sprites.end() ; ++i ){
                                // Load these sprites so they are ready to use
                                for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
                                    if( j->second )j->second->load();
                                }
                            }*/
                        } else if (simple == "snd"){
                            std::string sndFile;
                            simple.view() >> sndFile;
                            Mugen::Util::readSounds(Util::findFile(Filesystem::RelativePath(sndFile)), select.sounds);
                            Global::debug(1) << "Got Sound File: '" << sndFile << "'" << std::endl;
                        } else if (simple == "logo.storyboard"){
                            // Ignore
                        } else if (simple == "intro.storyboard"){
                            // Ignore
                        } else if (simple == "select"){
                            std::string file;
                            simple.view() >> file;
                            select.selectFile = Mugen::Util::fixFileName(baseDir, Mugen::Util::stripDir(file));
                            Global::debug(1) << "Got Select File: '" << file << "'" << std::endl;
                        } else if (simple == "fight"){
                            // Ignore
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            std::string fontPath;
                            simple.view() >> fontPath;
                            select.fonts.push_back(PaintownUtil::ReferenceCount<MugenFont>(new MugenFont(Util::findFile(Filesystem::RelativePath(fontPath)))));
                            Global::debug(1) << "Got Font File: '" << fontPath << "'" << std::endl;

                        } else {
                            throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                        }
                    }
                };

                FileWalker walker(*this, baseDir);
                section->walk(walker);
            } else if (head == "title info"){
                /* Nothing */
            } else if (PaintownUtil::matchRegex(head, "^titlebg")){
                /* Nothing */
            } else if (head == "select info"){ 
                class SelectInfoWalker: public Ast::Walker{
                public:
                    SelectInfoWalker(CharacterSelect & self, Mugen::SpriteMap & sprites):
                        self(self),
                        sprites(sprites){
                        }

                    CharacterSelect & self;
                    Mugen::SpriteMap & sprites;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "fadein.time" ){
                            try{
                                int time;
                                simple.view() >> time;
                                self.fader.setFadeInTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadein.color" ){
                            try{
                                int r,g,b;
                                simple.view() >> r >> g >> b;
                                self.fader.setFadeInColor(Graphics::makeColor(r,g,b));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.fader.setFadeOutTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.color"){
                            try{
                                int r,g,b;
                                simple.view() >> r >> g >> b;
                                self.fader.setFadeOutColor(Graphics::makeColor(r,g,b));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "rows"){
                            try{
                                simple.view() >> self.gridY;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "columns"){
                            try{
                                simple.view() >> self.gridX;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "wrapping"){
                            try{
                                bool wrap;
                                simple.view() >> wrap;
                                self.grid.setWrap(wrap);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "pos"){
                            try{
                                int x,y;
                                simple.view() >> self.gridPositionX >> self.gridPositionY;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "showemptyboxes"){
                            try{
                                bool boxes;
                                simple.view() >> boxes;
                                self.grid.setDrawEmpty(boxes);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "moveoveremptyboxes"){
                            try{
                                bool boxes;
                                simple.view() >> boxes;
                                self.grid.setAccessEmpty(boxes);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.size"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.grid.setCellDimensions(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.spacing"){
                            try{
                                int spacing;
                                simple.view() >> spacing;
                                self.grid.setCellMargins(spacing, spacing);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.bg.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setBackground(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setRandomIcon(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.switchtime"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.randomSwitchTime = time;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                int index = 0;
                                for (int i = 0; i <= x; ++i){
                                    for (int j = 0; j < y; ++j){
                                        index++;
                                    }
                                }
                                self.player1Start = index;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer1DoneCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player1Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player1Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player1Random, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                int index = 0;
                                for (int i = 0; i <= x; ++i){
                                    for (int j = 0; j < y; ++j){
                                        index++;
                                    }
                                }
                                self.player2Start = index;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer2DoneCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.blink"){
                            try{
                                bool blink;
                                simple.view() >> blink;
                                Cell::setBlinkCursor(blink);
                            } catch (const Ast::Exception & e){
                            }
                        } 
                        else if ( simple == "p2.cursor.move.snd"){ 
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player2Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player2Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player2Random, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "random.move.snd.cancel"){
                            try{
                                bool cancel;
                                simple.view() >> cancel;
                                //FIXME
                                // Set cancel sound flag
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(StageMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(StageDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "cancel.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Cancel, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.offset"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                Cell::setOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.scale"){
                            try{
                                Mugen::Effects effects;
                                simple.view() >> effects.scalex >> effects.scaley;
                                Cell::setEffects(effects);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "title.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.titleFont.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "title.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p1.face.offset"){
                            try{
                                simple.view() >> self.portrait1OffsetX >> self.portrait1OffsetY;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.scale"){
                            try{
                                simple.view() >> self.portrait1Effects.scalex >> self.portrait1Effects.scaley;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.facing"){
                            try{
                                simple.view() >> self.portrait1Effects.facing;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.offset"){
                            try{
                                simple.view() >> self.portrait2OffsetX >> self.portrait2OffsetY;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.scale"){
                            try{
                                simple.view() >> self.portrait2Effects.scalex >> self.portrait2Effects.scaley;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.facing"){
                            try{
                                simple.view() >> self.portrait2Effects.facing;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1Font.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        }  else if ( simple == "p1.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1Font.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2Font.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.name.font"){
                            int index=0, bank=0, position=0;
                            try{
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                            }
                            self.player2Font.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "stage.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.stageFont.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stageFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "stage.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stageFont.setActive2(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "stage.done.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.stageFont.setDone(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "teammenu.move.wrapping"){
                            try {
                                bool wrap;
                                simple.view() >> wrap;
                                TeamMenu::setWrapping(wrap);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setPosition(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1TeamMenu.setBackgroundSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.selftitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.selftitle.text"){
                            std::string text;
                            try {
                                simple.view() >> text;
                                self.player1TeamMenu.titleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.enemytitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.enemyTitleFont.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.enemytitle.text"){
                            std::string text;
                            try {
                                simple.view() >> text;
                                self.player1TeamMenu.enemyTitleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player1TeamMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player1TeamValue, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player1TeamDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.item.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setItemOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.item.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setItemSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.item.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1TeamMenu.itemFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p1.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1TeamMenu.itemCurrentFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p1.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.itemCurrentFont.setActive2(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.item.cursor.offset"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p1.teammenu.item.cursor.anim"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p1.teammenu.value.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1TeamMenu.setValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.empty.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setEmptyValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.empty.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1TeamMenu.setEmptyValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setValueSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setPosition(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.bg.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2TeamMenu.setBackgroundSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.selftitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.selftitle.text"){
                             std::string text;
                            try {
                                simple.view() >> text;
                                self.player2TeamMenu.titleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.enemytitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.enemyTitleFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.enemytitle.text"){
                             std::string text;
                            try {
                                simple.view() >> text;
                                self.player2TeamMenu.enemyTitleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player2TeamMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player2TeamValue, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.setSound(Player2TeamDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.item.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setItemOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.item.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setItemSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.item.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemCurrentFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemCurrentFont.setActive2(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.item.cursor.offset"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p2.teammenu.item.cursor.anim"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p2.teammenu.value.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2TeamMenu.setValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.empty.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setEmptyValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.empty.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2TeamMenu.setEmptyValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setValueSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        }
                    }
                };

                SelectInfoWalker walker(*this, sprites);
                section->walk(walker);
            } else if (head == "selectbgdef"){ 
                /* Background management */
                background = PaintownUtil::ReferenceCount<Mugen::Background>(new Mugen::Background(file, "selectbg"));
            } else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "vs screen" ){
                /* Don't handle versus here */
            }
            else if (head == "versusbgdef" ){
                /* Background management */
                //Mugen::Background *manager = new Mugen::Background(systemFile, "versusbg");
                //versus.setBackground(manager);
            }
            else if (head.find("versusbg" ) != std::string::npos ){ /* Ignore for now */ }
            else if (head == "demo mode" ){ /* Ignore for now */ }
            else if (head == "continue screen" ){ /* Ignore for now */ }
            else if (head == "game over screen" ){ /* Ignore for now */ }
            else if (head == "win screen" ){ /* Ignore for now */ }
            else if (head == "default ending" ){ /* Ignore for now */ }
            else if (head == "end credits" ){ /* Ignore for now */ }
            else if (head == "survival results screen" ){ /* Ignore for now */ }
            else if (head == "option info" ){ /* Ignore for now */ }
            else if (head == "optionbgdef" ){ /* Ignore for now */ }
            else if (head.find("optionbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "music" ){ /* Ignore for now */ }
            else if (head.find("begin action") != std::string::npos ){ /* Ignore for now */ }
            else {
                // throw MugenException("Unhandled Section in '" + systemFile.path() + "': " + head, __FILE__, __LINE__ ); 
                std::ostringstream context;
                context << __FILE__ << ":" << __LINE__;
                Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + file.path() + "': " + head << std::endl;
            }
        }

        // Set up Grid set starting offset to 1 so it doesn't clip
        grid.setStartingOffset(1, 1);
        grid.setGridSize(gridX, gridY);
        // Set up cells
        for (unsigned int i = 0; i < (unsigned int)(gridX * gridY); ++i){
            PaintownUtil::ReferenceCount<Cell> cell = PaintownUtil::ReferenceCount<Cell>(new Cell(i, &grid));
            cells.push_back(cell);
            grid.addItem(cell.convert<Gui::SelectItem>());
        }
        // Set up cursors
        grid.setCursors(2);
        Global::debug(0) << "Player 1 start: " << player1Start << std::endl;
        Global::debug(0) << "Player 2 start: " << player2Start << std::endl;
        grid.setCurrentIndex(0, player1Start);
        grid.setCurrentState(0, Gui::SelectListInterface::Disabled);
        grid.setCurrentIndex(1, player2Start);
        grid.setCurrentState(1, Gui::SelectListInterface::Disabled);

    } catch (const Filesystem::NotFound & fail){
        std::ostringstream out;
        out << "Could not load select screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
    
    // Set stage to blink
    stageFont.setState(FontHandler::Blink);
    
    parseSelect();
}

void CharacterSelect::act(){
    for (std::vector< PaintownUtil::ReferenceCount<Cell> >::iterator i = cells.begin(); i != cells.end(); ++i){
        PaintownUtil::ReferenceCount<Cell> cell = *i;
        cell->act();
    }
    background->act();
    grid.act();
    // Player 1 random?
    if (cells[grid.getCurrentIndex(0)]->getRandom()){
        player1SwitchTime++;
        if (player1SwitchTime >= randomSwitchTime){
            player1SwitchTime = 0;
            player1CurrentRandom = PaintownUtil::rnd(0,characters.size());
        }
    }
    // Player 2 random?
    if (cells[grid.getCurrentIndex(1)]->getRandom()){
        player2SwitchTime++;
        if (player2SwitchTime >= randomSwitchTime){
            player2SwitchTime = 0;
            player2CurrentRandom = PaintownUtil::rnd(0,characters.size());
        }
    }
    player1TeamMenu.act();
    player2TeamMenu.act();
    stageFont.act();
}

void CharacterSelect::draw(const Graphics::Bitmap & work){
    // Render Background
    background->renderBackground(0,0,work);
    
    // Temporary bitmap for grid
    const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(grid.getWidth(), grid.getHeight());
    temp.clearToMask();
    grid.render(temp, Font::getDefaultFont());
    // Minus 1 since it's been offset
    temp.draw(gridPositionX-1, gridPositionY-1, work);
    
    // render title FIXME set title
    titleFont.draw("Test", work);
    
    // Draw portrait and name
    if (grid.getCurrentState(0) != Gui::SelectListInterface::Disabled){
        if (cells[grid.getCurrentIndex(0)]->getRandom()){
            const Mugen::ArcadeData::CharacterInfo & character = characters[player1CurrentRandom];
            character.drawPortrait(portrait1OffsetX, portrait1OffsetY, work, portrait1Effects);
            player1Font.draw(character.getName(), work);
        } else {
            const Mugen::ArcadeData::CharacterInfo & character = cells[grid.getCurrentIndex(0)]->getCharacter();
            character.drawPortrait(portrait1OffsetX, portrait1OffsetY, work, portrait1Effects);
            player1Font.draw(character.getName(), work);
        }
    }
    if (grid.getCurrentState(1) != Gui::SelectListInterface::Disabled){
        if (cells[grid.getCurrentIndex(1)]->getRandom()){
            const Mugen::ArcadeData::CharacterInfo & character = characters[player2CurrentRandom];
            character.drawPortrait(portrait1OffsetX, portrait1OffsetY, work, portrait1Effects);
            player1Font.draw(character.getName(), work);
        } else {
            const Mugen::ArcadeData::CharacterInfo & character = cells[grid.getCurrentIndex(1)]->getCharacter();
            character.drawPortrait(portrait2OffsetX, portrait2OffsetY, work, portrait2Effects);
            player2Font.draw(character.getName(), work);
        }
    }
    
    
    // FIXME remove and use properly, testing only now
    player1TeamMenu.draw(work);
    player2TeamMenu.draw(work);
    
   
    // Stage
    if (randomStage){
        stageFont.draw("Stage: Random", work);
    } else {
        stageFont.draw("Stage: " + stageNames[currentStage], work);
    }
    
    background->renderForeground(0,0,work);
}

void CharacterSelect::up(unsigned int cursor){
    if (grid.getCurrentState(cursor) != Gui::SelectListInterface::Disabled && grid.up(cursor)){
        if (cursor == 0){
            MugenSound * sound = sounds[soundLookup[Player1Move].group][soundLookup[Player1Move].index];
            if (sound){
                sound->play();
            }
        } else if (cursor == 1){
            MugenSound * sound = sounds[soundLookup[Player2Move].group][soundLookup[Player2Move].index];
            if (sound){
                sound->play();
            }
        }
    }
    player1TeamMenu.up();
    player2TeamMenu.up();
}

void CharacterSelect::down(unsigned int cursor){
    if (grid.getCurrentState(cursor) != Gui::SelectListInterface::Disabled && grid.down(cursor)){
        if (cursor == 0){
            MugenSound * sound = sounds[soundLookup[Player1Move].group][soundLookup[Player1Move].index];
            if (sound){
                sound->play();
            }
        } else if (cursor == 1){
            MugenSound * sound = sounds[soundLookup[Player2Move].group][soundLookup[Player2Move].index];
            if (sound){
                sound->play();
            }
        }
    }
    player1TeamMenu.down();
    player2TeamMenu.down();
}

void CharacterSelect::left(unsigned int cursor){
    if (grid.getCurrentState(cursor) != Gui::SelectListInterface::Disabled && grid.left(cursor)){
        if (cursor == 0){
            MugenSound * sound = sounds[soundLookup[Player1Move].group][soundLookup[Player1Move].index];
            if (sound){
                sound->play();
            }
        } else if (cursor == 1){
            MugenSound * sound = sounds[soundLookup[Player2Move].group][soundLookup[Player2Move].index];
            if (sound){
                sound->play();
            }
        }
    }
    player1TeamMenu.left();
    player2TeamMenu.left();
    previousStage();
}

void CharacterSelect::right(unsigned int cursor){
    if (grid.getCurrentState(cursor) != Gui::SelectListInterface::Disabled && grid.right(cursor)){
        if (cursor == 0){
            MugenSound * sound = sounds[soundLookup[Player1Move].group][soundLookup[Player1Move].index];
            if (sound){
                sound->play();
            }
        } else if (cursor == 1){
            MugenSound * sound = sounds[soundLookup[Player2Move].group][soundLookup[Player2Move].index];
            if (sound){
                sound->play();
            }
        }
    }
    player1TeamMenu.right();
    player2TeamMenu.right();
    nextStage();
}

void CharacterSelect::select(unsigned int cursor){
    cells[grid.getCurrentIndex(cursor)]->select();
}

void CharacterSelect::addCharacter(const Mugen::ArcadeData::CharacterInfo & character){
    // Add to list
    characters.push_back(character);
    // Include stage if required
    if (character.getIncludeStage()){
        addStage(character.getStage());
    }
    // Check if we don't exceed the cell count of the current grid
    if (nextCell < cells.size()){
        // Add to current cell
        cells[nextCell]->setCharacter(character);
        // Increment cell
        nextCell++;
    }
}

void CharacterSelect::addEmpty(){
    if (nextCell < cells.size()){
        cells[nextCell]->setEmpty(true);
        nextCell++;
    }
}

void CharacterSelect::addRandom(){
    if (nextCell < cells.size()){
        cells[nextCell]->setRandom(true);
        nextCell++;
    }
}

void CharacterSelect::addStage(const Filesystem::AbsolutePath & stage){
    try {
        for (std::vector<Filesystem::AbsolutePath>::iterator i = stages.begin(); i != stages.end(); ++i){
            const Filesystem::AbsolutePath & check = *i;
            if (stage == check){
                return;
            }
        }
        AstRef parsed(Util::parseDef(stage.path()));
        const std::string & name = Util::probeDef(parsed, "info", "name");
        stages.push_back(stage);
        stageNames.push_back(name);
    } catch (const MugenException & ex){
        Global::debug(0) << "Warning! Tried to load file: '" << stage.path() << "'. Message: " << ex.getReason() << std::endl;
    }
}

void CharacterSelect::setSound(const SoundType & type, int group, int sound){
    IndexValue values;
    values.group = group;
    values.index = sound;
    soundLookup[type] = values;
}

/* indexes start at 1 */
PaintownUtil::ReferenceCount<MugenFont> CharacterSelect::getFont(int index) const {
    if (index - 1 >= 0 && index - 1 < (signed) fonts.size()){
        return fonts[index - 1];
    } else {
        std::ostringstream out;
        out << "No font for index " << index;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

void CharacterSelect::parseSelect(){
    const Filesystem::AbsolutePath file = Util::findFile(Filesystem::RelativePath(selectFile.getFilename().path()));
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(file.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file.path() + " in" + diff.printTime("") << std::endl;
    
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        
        head = Mugen::Util::fixCase(head);

        if (head == "characters"){
            class CharacterWalker: public Ast::Walker{
            public:
                CharacterWalker(CharacterSelect & self):
                self(self){}
                virtual ~CharacterWalker(){}
            
                CharacterSelect & self;
                
                virtual void onValueList(const Ast::ValueList & list){
                    // Grab Character
                    std::string temp;
                    Ast::View view = list.view();
                    view >> temp;

                    if (temp == "blank"){
                        self.addEmpty();
                    } else if (temp == "randomselect"){
                        self.addRandom();
                    } else {
                        Mugen::ArcadeData::CharacterInfo character(Util::findCharacterDef(temp));              
                        try{
                            // Grab stage
                            view >> temp;
                            if (PaintownUtil::matchRegex(temp, "order = ")){
                                temp.replace(0,std::string("order = ").size(),"");
                                character.setOrder((int)atoi(temp.c_str()));
                            } else if (temp == "random"){
                                character.setRandomStage(true);
                            } else {
                                character.setStage(Util::findFile(Filesystem::RelativePath(temp)));
                            }
                            // Grab options
                            /* TODO: make the parser turn these into better AST nodes.
                            * something like Assignment(Id(music), Filename(whatever))
                            */
                            while(true){
                                view >> temp;
                                if (PaintownUtil::matchRegex(temp,"includestage = ")){
                                    temp.replace(0,std::string("includestage = ").size(),"");
                                    character.setIncludeStage((bool)atoi(temp.c_str()));
                                } else if (PaintownUtil::matchRegex(temp,"music = ")){
                                    temp.replace(0,std::string("music = ").size(),"");
                                    character.setMusic(Util::findFile(Filesystem::RelativePath(temp)));
                                } else if (PaintownUtil::matchRegex(temp,"order = ")){
                                    temp.replace(0,std::string("order = ").size(),"");
                                    character.setOrder((int)atoi(temp.c_str()));
                                }
                            }
                        } catch (const Ast::Exception & e){
                        }
                        self.addCharacter(character);
                    }
                }
            };

            CharacterWalker walk(*this);
            section->walk(walk);
        } else if (head == "extrastages"){
            class StageWalker: public Ast::Walker{
            public:
                StageWalker(CharacterSelect & self):
                self(self){
                }

                virtual ~StageWalker(){
                }

                CharacterSelect & self;

                virtual void onValueList(const Ast::ValueList & list){
                    // Get Stage info and save it
                    try {
                        std::string temp;
                        list.view() >> temp;
                        Global::debug(1) << "stage: " << temp << std::endl;
                        self.addStage(Util::findFile(Filesystem::RelativePath(temp)));
                    } catch (const Ast::Exception & e){
                    }
                }
            };
            StageWalker walk(*this);
            section->walk(walk);
        } else if (head == "options"){
            class OptionWalker: public Ast::Walker{
            public:
                OptionWalker(std::vector<int> & arcade, std::vector<int> & team):
                arcade(arcade),
                team(team){
                }
                virtual ~OptionWalker(){}
                std::vector<int> & arcade;
                std::vector<int> & team;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "arcade.maxmatches"){
                        Ast::View view = simple.view();
                        // only 10 max matches
                        for (int i = 0; i < 10; ++i){
                            try{
                                int matches;
                                view >> matches;
                                // No need to save the remaining of the matchup setup
                                if (matches == 0){
                                    break;
                                }
                                arcade.push_back(matches);
                            } catch (const Ast::Exception & e){
                                break;
                            }
                        }
                    } else if (simple == "team.maxmatches"){
                        Ast::View view = simple.view();
                        // only 10 max matches
                        for (int i = 0; i < 10; ++i){
                            try{
                                int matches;
                                view >> matches;
                                // No need to save the remaining of the matchup setup
                                if (matches == 0){
                                    break;
                                }
                                team.push_back(matches);
                            } catch (const Ast::Exception & e){
                                break;
                            }
                        }
                    }
                }
            };
            OptionWalker walk(arcadeOrder,teamArcadeOrder);
            section->walk(walk);
        } else {
            // throw MugenException("Unhandled Section in '" + file.path() + "': " + head, __FILE__, __LINE__); 
            std::ostringstream context;
            context << __FILE__ << ":" << __LINE__;
            Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + file.path() + "': " + head << std::endl;
        }
    }
}

bool CharacterSelect::previousStage(){
    if (stages.size() >= 1){
        if (randomStage){
            randomStage = false;
            currentStage = stages.size()-1;
        } else if (currentStage > 0){
            currentStage--;
        } else if (!randomStage){
            randomStage = true;
        }
        return true;
    }
    return false;
}

bool CharacterSelect::nextStage(){
    if (stages.size() >= 1){
        if (randomStage){
            randomStage = false;
            currentStage = 0;
        } else if (currentStage < stages.size()-1){
            currentStage++;
        } else if (!randomStage){
            randomStage = true;
        }
        return true;
    }
    return false;
}