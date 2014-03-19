#include "versus.h"

#include <iostream>

#include "util/graphics/bitmap.h"
#include "util/timedifference.h"
#include "ast/all.h"
#include "sound.h"
#include "font.h"

using namespace Mugen;

VersusMenu::VersusMenu(const Filesystem::AbsolutePath & file, bool demoMode):
path(file),
time(0),
ticker(0),
x1(0),
x2(0),
y1(0),
y2(0),
fontx1(0),
fontx2(0),
fonty1(0),
fonty2(0),
player1(Mugen::ArcadeData::CharacterCollection::Single),
player2(Mugen::ArcadeData::CharacterCollection::Single),
canceled(false),
demoMode(demoMode){
    try{
        Filesystem::AbsolutePath baseDir = path.getDirectory();

        Global::debug(1) << baseDir.path() << std::endl;

        if (path.isEmpty()){
            throw MugenException("Cannot locate character select definition file for: " + path.path(), __FILE__, __LINE__);
        }

        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Util::parseDef(path));
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
                    FileWalker(VersusMenu & self, const Filesystem::AbsolutePath & baseDir):
                        self(self),
                        baseDir(baseDir){
                        }

                    VersusMenu & self;
                    const Filesystem::AbsolutePath & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            std::string sffFile;
                            simple.view() >> sffFile;
                            Global::debug(1) << "Got Sprite File: '" << sffFile << "'" << std::endl;
                            Mugen::Util::readSprites(Mugen::Util::findFile(Filesystem::RelativePath(sffFile)), Filesystem::AbsolutePath(), self.sprites, true);
                        } else if (PaintownUtil::matchRegex(simple.idString(), PaintownUtil::Regex("^font"))){
                            std::string fontPath;
                            simple.view() >> fontPath;
                            self.fonts.add(fontPath);
                        }
                    }
                };

                FileWalker walker(*this, baseDir);
                section->walk(walker);
            } else if (head == "vs screen" ){
                class VersusMenuWalker: public Ast::Walker{
                public:
                    VersusMenuWalker(VersusMenu & self):
                        self(self){
                        }

                    VersusMenu & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "time" ){
                            try{
                                int time;
                                simple.view() >> self.time;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadein.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.fader.setFadeInTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.fader.setFadeOutTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.pos"){
                            try{
                                simple.view() >> self.x1 >> self.y1;
                            } catch (Ast::Exception & e){
                            }
                        } else if (simple == "p1.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                switch (face){
                                    case 1: self.effects1.facing = false; break;
                                    case -1: self.effects1.facing = true; break;
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.scale"){
                            try{
                                simple.view() >> self.effects1.scalex >> self.effects1.scaley;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.pos"){
                            try{
                                simple.view() >> self.x2 >> self.y2;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                switch (face){
                                    case 1: self.effects2.facing = false; break;
                                    case -1: self.effects2.facing = true; break;
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.scale"){
                            try{
                                simple.view() >> self.effects2.scalex >> self.effects2.scaley;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.name.pos"){
                            try {
                                simple.view() >> self.fontx1 >> self.fonty1;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.name.font"){
                            int index=-1, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.font1 = self.fonts.getFont(index, bank, position);
                        } else if (simple == "p2.name.pos"){
                            try {
                                simple.view() >> self.fontx2 >> self.fonty2;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.name.font"){
                            int index=-1, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.font2 = self.fonts.getFont(index, bank, position);
                        }
                    }
                };

                VersusMenuWalker walker(*this);
                section->walk(walker);
            }
            else if (head == "versusbgdef" ){
                /* Background management */
                background = PaintownUtil::ReferenceCount<Mugen::Background>(new Mugen::Background(path, "versusbg"));
            }
        }
    } catch (const Filesystem::NotFound & fail){
        std::ostringstream out;
        out << "Could not load versus screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

VersusMenu::~VersusMenu(){
}

void VersusMenu::init(const Mugen::ArcadeData::CharacterCollection & player1, const Mugen::ArcadeData::CharacterCollection & player2){
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
    ticker = 0;
    
    this->player1 = player1;
    this->player2 = player2;
}
    
void VersusMenu::act(){
    ticker++;
    if (ticker == (time - fader.getFadeOutTime())){
        fader.setState(Gui::FadeTool::FadeOut);
    }
    background->act();
    fader.act();
}

void VersusMenu::draw(const Graphics::Bitmap & work){
    // Draw background
    background->renderBackground(0, 0, work);

    // render portraits and fonts
    drawPortrait(player1, effects1, x1, y1, fontx1, fonty1, font1, work);
    drawPortrait(player2, effects2, x2, y2, fontx2, fonty2, font2, work);

    // Draw Foreground
    background->renderForeground(0,0,work);
    // Fader
    fader.draw(work);
}

void VersusMenu::cancel(){
    // Fade out
    fader.setState(Gui::FadeTool::FadeOut);
    canceled = true;
}

void VersusMenu::skip(){
    fader.setState(Gui::FadeTool::FadeOut);
}

bool VersusMenu::isDone(){
    return (fader.getState() == Gui::FadeTool::EndFade);
}

void VersusMenu::drawPortrait(const Mugen::ArcadeData::CharacterCollection & collection, const Mugen::Effects & effects, int x, int y, int fontx, int fonty, const Mugen::FontSystem::Font & font, const Graphics::Bitmap & work){
    
    //PaintownUtil::ReferenceCount<Font> font;
    
    int heightMod = 0;
    if (collection.getFirstSet()){
        const Mugen::ArcadeData::CharacterInfo & character = collection.getFirst();
        character.drawPortrait(x, y, work, effects);
        /*font = getFont(fontInfo.index);
        if (font == NULL){
            return;
        }*/
        //font->render(fontx, fonty + heightMod, fontInfo.position, fontInfo.bank, work, collection.getFirst().getName());
        font.draw(fontx, fonty + heightMod, collection.getFirst().getName(), work);
        heightMod += 15;
    }
    if (collection.getSecondSet()){
        //font->render(fontx, fonty + heightMod, fontInfo.position, fontInfo.bank, work, collection.getSecond().getName());
        font.draw(fontx, fonty + heightMod, collection.getSecond().getName(), work);
        heightMod += 15;
    }
    if (collection.getThirdSet()){
        //font->render(fontx, fonty + heightMod, fontInfo.position, fontInfo.bank, work, collection.getThird().getName());
        font.draw(fontx, fonty + heightMod, collection.getThird().getName(), work);
        heightMod += 15;
    }
    if (collection.getFourthSet()){
        //font->render(fontx, fonty + heightMod, fontInfo.position, fontInfo.bank, work, collection.getFourth().getName());
        font.draw(fontx, fonty + heightMod, collection.getFourth().getName(), work);
    }
}

class VersusLogic: public PaintownUtil::Logic {
public:
    VersusLogic(InputMap<Mugen::Keys> & input1, InputMap<Mugen::Keys> & input2, Mugen::VersusMenu & versus):
    is_done(false),
    canceled(false),
    input1(input1),
    input2(input2),
    versus(versus){
    }

    bool deinit, is_done, canceled;
    InputMap<Mugen::Keys> & input1;
    InputMap<Mugen::Keys> & input2;
    Mugen::VersusMenu & versus;
    
    bool done(){
        return is_done;
    }

    void run(){
        std::vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(input1, InputSource(true));
        for (std::vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc){
                    if (!canceled){
                        canceled = true;
                        versus.cancel();
                    }
                }
                if (event.out == Enter || event.out == A || event.out == B || event.out == C || event.out == X || event.out == Y || event.out == Z){
                    if (!canceled){
                        canceled = true;
                        versus.skip();
                    }
                }
            }
        }
        out = InputManager::getEvents(input2, InputSource(true));
        for (std::vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc){
                    if (!canceled){
                        canceled = true;
                        versus.cancel();
                    }
                }
                if (event.out == Enter || event.out == A || event.out == B || event.out == C || event.out == X || event.out == Y || event.out == Z){
                    if (!canceled){
                        canceled = true;
                        versus.skip();
                    }
                }
            }
        }
        if (versus.inDemoMode()){
            if (InputManager::anyInput()){
                if (!canceled){
                    canceled = true;
                    versus.cancel();
                }
            }
        }
        versus.act();
        is_done = versus.isDone();
    }

    double ticks(double system){
        return system;
    }
};

class VersusDraw: public PaintownUtil::Draw {
public:
    VersusDraw(Mugen::VersusMenu & versus):
    versus(versus){
    }
    
    Mugen::VersusMenu & versus;

    void draw(const Graphics::Bitmap & buffer){
        buffer.clear();
        Graphics::StretchedBitmap work(320, 240, buffer, Graphics::StretchedBitmap::NoClear, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
        work.start();
        versus.draw(work);
        work.finish();
        // buffer.BlitToScreen();
    }
};


PaintownUtil::ReferenceCount<PaintownUtil::Logic> VersusMenu::getLogic(InputMap<Mugen::Keys> & input1, InputMap<Mugen::Keys> & input2){
    PaintownUtil::ReferenceCount<VersusLogic> logic = PaintownUtil::ReferenceCount<VersusLogic>(new VersusLogic(input1, input2, *this));
    return logic;
}

PaintownUtil::ReferenceCount<PaintownUtil::Draw> VersusMenu::getDraw(){
    PaintownUtil::ReferenceCount<VersusDraw> draw = PaintownUtil::ReferenceCount<VersusDraw>(new VersusDraw(*this));
    return draw;
}
