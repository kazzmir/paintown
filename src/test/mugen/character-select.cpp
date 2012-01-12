#include "character-select.h"

#include <iostream>

#include "util/timedifference.h"
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

void SelectFont::render(int x, int y, const std::string & text, const Graphics::Bitmap & work){
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

void FontHandler::render(const Graphics::Bitmap & work){
    render(text, work);
}

void FontHandler::render(const std::string & text, const Graphics::Bitmap & work){
    switch(state){
        default:
        case Normal:
            active.render(x, y, text, work);
            break;
        case Blink:
            if (ticker < blinkTime){
                active.render(x, y, text, work);
            } else if (ticker >= blinkTime){
                active2.render(x, y, text, work);
            }
            break;
        case Done:
            done.render(x, y, text, work);
            break;
    }
}

Cell::Cell(unsigned int index, const Gui::SelectListInterface * parent):
index(index),
parent(parent),
empty(true),
isRandom(false){
}

Cell::~Cell(){
}

void Cell::act(){
    
}

void Cell::draw(int x, int y, int width, int height, const Graphics::Bitmap & work, const Font & font) const{
    if (background != NULL){
        background->render(x, y, work);
    }
    
    // If random draw random icon otherwise draw character image if available
    if (isRandom){
        if (randomIcon != NULL){
            randomIcon->render(x, y, work);
        }
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
}

bool Cell::isEmpty() const{
    return empty;
}

void Cell::setRandom(bool r){
    isRandom = r;
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


TeamMenu::TeamMenu(const Side & side):
side(side),
current(Simultaneous){
}

TeamMenu::~TeamMenu(){
}

void TeamMenu::act(){
}

void TeamMenu::draw(const Graphics::Bitmap &, bool enemy){
    if (!enemy){
        switch (side){
            case Left:
                break;
            case Right:
            default:
                break;
        }
    } else {
        // Opposite side
        switch (side){
            case Left:
                break;
            case Right:
            default:
                break;
        }
    }
}

void TeamMenu::up(){
}

void TeamMenu::down(){
}

void TeamMenu::left(){
}

void TeamMenu::right(){
}

const TeamMenu::FightType & TeamMenu::select(){
    return current;
}

CharacterSelect::CharacterSelect(const Filesystem::AbsolutePath & file):
player1TeamMenu(TeamMenu::Left),
player2TeamMenu(TeamMenu::Right),
file(file){
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
                            simple.view() >> select.selectFile;
                            Global::debug(1) << "Got Select File: '" << select.selectFile << "'" << std::endl;
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
                                //self.grid.setCellRandomSwitchTime(time);
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
                                //self.grid.setPortraitOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.scale"){
                            try{
                                double x,y;
                                simple.view() >> x >> y;
                                //self.grid.setPortraitScale(x,y);
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
                                self.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.face.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                //self.player1Cursor.setFaceOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.scale"){
                            try{
                                double x, y;
                                simple.view() >> x >> y;
                                //self.player1Cursor.setFaceScale(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.facing"){
                            try{
                                int f;
                                simple.view() >> f;
                                //self.player1Cursor.setFacing(f);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                //self.player2Cursor.setFaceOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.scale"){
                            try{
                                double x, y;
                                simple.view() >> x >> y;
                                //self.player2Cursor.setFaceScale(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.facing"){
                            try{
                                int f;
                                simple.view() >> f;
                                //self.player2Cursor.setFacing(f);
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
                                self.player1Font.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p2.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2Font.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.name.font"){
                            try{
                                int index, bank, position;
                                simple.view() >> index >> bank >> position;
                                self.player2Font.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                            }
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
                                self.stageFont.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "stage.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.stageFont.setActive2(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "stage.done.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.stageFont.setDone(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "teammenu.move.wrapping"){
                        } else if ( simple == "p1.teammenu.pos"){
                        } else if ( simple == "p1.teammenu.spr"){
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
                        } else if ( simple == "p1.teammenu.value.snd"){
                        } else if ( simple == "p1.teammenu.done.snd"){
                        } else if ( simple == "p1.teammenu.item.offset"){
                        } else if ( simple == "p1.teammenu.item.spacing"){
                        } else if ( simple == "p1.teammenu.item.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.itemFont.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.itemCurrentFont.setActive(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.itemCurrentFont.setActive2(SelectFont(self.getFont(index), bank, position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.item.cursor.offset"){
                        } else if ( simple == "p1.teammenu.item.cursor.anim"){
                        } else if ( simple == "p1.teammenu.value.icon.offset"){
                        } else if ( simple == "p1.teammenu.value.icon.spr"){
                        } else if ( simple == "p1.teammenu.value.empty.icon.offset"){
                        } else if ( simple == "p1.teammenu.value.empty.icon.spr"){
                        } else if ( simple == "p1.teammenu.value.spacing"){
                        } else if ( simple == "p2.teammenu.pos"){
                        } else if ( simple == "p2.teammenu.bg.spr"){
                        } else if ( simple == "p2.teammenu.selftitle.font"){
                        } else if ( simple == "p2.teammenu.selftitle.text"){
                        } else if ( simple == "p2.teammenu.enemytitle.font"){
                        } else if ( simple == "p2.teammenu.enemytitle.text"){
                        } else if ( simple == "p2.teammenu.move.snd"){
                        } else if ( simple == "p2.teammenu.value.snd"){
                        } else if ( simple == "p2.teammenu.done.snd"){
                        } else if ( simple == "p2.teammenu.item.offset"){
                        } else if ( simple == "p2.teammenu.item.spacing"){
                        } else if ( simple == "p2.teammenu.item.font"){
                        } else if ( simple == "p2.teammenu.item.active.font"){
                        } else if ( simple == "p2.teammenu.item.active2.font"){
                        } else if ( simple == "p2.teammenu.item.cursor.offset"){
                        } else if ( simple == "p2.teammenu.item.cursor.anim"){
                        } else if ( simple == "p2.teammenu.value.icon.offset"){
                        } else if ( simple == "p2.teammenu.value.icon.spr"){
                        } else if ( simple == "p2.teammenu.value.empty.icon.offset"){
                        } else if ( simple == "p2.teammenu.value.empty.icon.spr"){
                        } else if ( simple == "p2.teammenu.value.spacing"){
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
#if 0
                class VersusWalker: public Ast::Walker{
                public:
                    VersusWalker(VersusScreen & self, const CharacterSelect & select):
                        self(self),
                        select(select){
                        }

                    VersusScreen & self;
                    const CharacterSelect & select;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "time" ){
                            try{
                                int time;
                                simple.view() >> time;
                                self.setTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadein.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.getFadeTool().setFadeInTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.getFadeTool().setFadeOutTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.pos"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (Ast::Exception & e){
                            }
                            self.setPlayer1Position(Mugen::Point(x,y));
                        } else if (simple == "p1.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                self.setPlayer1Facing(face);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.scale"){
                            double x,y;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.setPlayer1Scale(x,y);
                        } else if (simple == "p2.pos"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.setPlayer2Position(Mugen::Point(x,y));
                        } else if (simple == "p2.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                self.setPlayer2Facing(face);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.scale"){
                            double x,y;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.setPlayer2Scale(x,y);
                        } else if (simple == "p1.name.pos"){
                            int x, y;
                            try {
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.getPlayer1Font().setLocation(x,y);
                        } else if (simple == "p1.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            if (index > 0){
                                self.getPlayer1Font().setPrimary(select.getFont(index),bank,position);
                            }
                        } else if (simple == "p2.name.pos"){
                            int x, y;
                            try {
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.getPlayer2Font().setLocation(x,y);
                        } else if (simple == "p2.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            if (index > 0){
                                self.getPlayer2Font().setPrimary(select.getFont(index),bank,position);
                            }
                        }
                    }
                };

                VersusWalker walker(versus, *this);
                section->walk(walker);
#endif
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
        //grid.setCurrentState(0, Gui::SelectListInterface::Disabled);
        grid.setCurrentIndex(1, player2Start);
        grid.setCurrentState(1, Gui::SelectListInterface::Disabled);

    } catch (const Filesystem::NotFound & fail){
        std::ostringstream out;
        out << "Could not load select screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

void CharacterSelect::act(){
    background->act();
    grid.act();
}

void CharacterSelect::draw(const Graphics::Bitmap & work){
    background->renderBackground(0,0,work);
    const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(grid.getWidth(), grid.getHeight());
    temp.clearToMask();
    grid.render(temp, Font::getDefaultFont());
    // Minus 1 since it's been offset
    temp.draw(gridPositionX-1, gridPositionY-1, work);
    // render title FIXME set title
    titleFont.render("Test", work);
    background->renderForeground(0,0,work);
}

void CharacterSelect::up(unsigned int cursor){
    if (grid.up(cursor)){
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
}

void CharacterSelect::down(unsigned int cursor){
    if (grid.down(cursor)){
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
}

void CharacterSelect::left(unsigned int cursor){
    if (grid.left(cursor)){
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
}

void CharacterSelect::right(unsigned int cursor){
    if (grid.right(cursor)){
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