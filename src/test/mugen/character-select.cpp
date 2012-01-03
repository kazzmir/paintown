#include "character-select.h"

#include <iostream>

#include "util/timedifference.h"
#include "mugen/ast/all.h"

using namespace Mugen;

::Util::ReferenceCount<MugenSprite> Cell::background = ::Util::ReferenceCount<MugenSprite>(NULL);
::Util::ReferenceCount<MugenSprite> Cell::randomIcon = ::Util::ReferenceCount<MugenSprite>(NULL);

Cell::Cell(unsigned int index, const ::Util::ReferenceCount<Gui::SelectListInterface> parent):
index(index),
parent(parent),
empty(true),
isRandom(false){
}

Cell::~Cell(){
}

void Cell::act(){
}

void Cell::draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const{
}

bool Cell::isEmpty() const{
    return empty;
}

void Cell::setRandom(bool r){
    isRandom = r;
}

void Cell::setBackground(::Util::ReferenceCount<MugenSprite> background){
    Cell::background = background;
}

void Cell::setRandomIcon(::Util::ReferenceCount<MugenSprite> randomIcon){
    Cell::randomIcon = randomIcon;
}

CharacterSelect::CharacterSelect(const Filesystem::AbsolutePath & path){
    try{
        Filesystem::AbsolutePath baseDir = path.getDirectory();

        Global::debug(1) << baseDir.path() << std::endl;

        if (path.isEmpty()){
            throw MugenException( "Cannot locate character select definition file for: " + path.path());
        }

        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Util::parseDef(path.path()));
        diff.endTime();
        Global::debug(1) << "Parsed mugen file " + path.path() + " in" + diff.printTime("") << std::endl;

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
                            select.fonts.push_back(::Util::ReferenceCount<MugenFont>(new MugenFont(Util::findFile(Filesystem::RelativePath(fontPath)))));
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
                                simple.view() >> self.gridX;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "columns"){
                            try{
                                simple.view() >> self.gridY;
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
                                simple.view() >> x >> y;
                                //self.grid.setPosition(x,y);
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
                                self.grid.setCellSpacing(spacing, spacing);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.bg.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setBackground(::Util::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setRandomIcon(::Util::ReferenceCount<MugenSprite>(sprites[group][sprite]));
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
                                self.player1Start = x * y;
                                //self.grid.setPlayer1Start(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                //self.player1Cursor.setActiveSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                //self.player1Cursor.setDoneSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.player1Cursor.setMoveSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.player1Cursor.setSelectSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.player1Cursor.setRandomSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                self.player2Start = x * y;
                                //self.grid.setPlayer2Start(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                //self.player2Cursor.setActiveSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                //self.player2Cursor.setDoneSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.blink"){
                            try{
                                bool blink;
                                simple.view() >> blink;
                                //self.player2Cursor.setBlink(blink);
                            } catch (const Ast::Exception & e){
                            }
                        } 
                        else if ( simple == "p2.cursor.move.snd"){ 
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.player2Cursor.setMoveSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.player2Cursor.setSelectSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.player2Cursor.setRandomSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "random.move.snd.cancel"){
                            try{
                                bool cancel;
                                simple.view() >> cancel;
                                //self.player1Cursor.setRandomCancel(cancel);
                                //self.player2Cursor.setRandomCancel(cancel);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.grid.getStageHandler().setMoveSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.grid.getStageHandler().setSelectSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "cancel.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                //self.cancelSound = self.sounds[group][sound];
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
                                //self.titleFont.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "title.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            /* -1 indicates no font */
                            if (index != -1){
                                /* should this be set even if the parse fails? */
                                //self.titleFont.setPrimary(self.getFont(index), bank, position);
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
                                //self.player1Cursor.getFontHandler().setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        }  else if ( simple == "p1.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }

                            if (index > 0){
                                //self.player1Cursor.getFontHandler().setPrimary(self.getFont(index),bank,position);
                            }
                        } else if ( simple == "p2.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                //self.player2Cursor.getFontHandler().setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.name.font"){
                            try{
                                int index, bank, position;
                                simple.view() >> index >> bank >> position;
                                if (index > 0){
                                    //self.player2Cursor.getFontHandler().setPrimary(self.getFont(index),bank,position);
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                //self.grid.getStageHandler().getFontHandler().setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }

                            if (index > 0){
                                //self.grid.getStageHandler().getFontHandler().setPrimary(self.getFont(index),bank,position);
                            }
                        } else if ( simple == "stage.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }

                            if (index > 0){
                                //self.grid.getStageHandler().getFontHandler().setBlink(self.getFont(index),bank,position);
                            }
                        } else if ( simple == "stage.done.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            if (index > 0){
                                //self.grid.getStageHandler().getFontHandler().setDone(self.getFont(index),bank,position);
                            }
                        }
                    }
                };

                SelectInfoWalker walker(*this, sprites);
                section->walk(walker);
            } else if (head == "selectbgdef"){ 
                /* Background management */
                background = ::Util::ReferenceCount<Mugen::Background>(new Mugen::Background(path, "selectbg"));
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
                Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + path.path() + "': " + head << std::endl;
            }
        }

        // Set up Grid
        grid.setGridSize(gridX, gridY);
        for (unsigned int i = 0; i < (unsigned int)(gridX * gridY); ++i){
            ::Util::ReferenceCount<Cell> cell = ::Util::ReferenceCount<Cell>(new Cell(i, ::Util::ReferenceCount<Gui::SelectListInterface>(&grid)));
            cells.push_back(cell);
            grid.addItem(cell.convert<Gui::SelectItem>());
        }

    } catch (const Filesystem::NotFound & fail){
        std::ostringstream out;
        out << "Could not load select screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::act(){
}

void CharacterSelect::draw(const Graphics::Bitmap & work){
}