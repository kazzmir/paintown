#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <map>

#include "globals.h"
#include "util/init.h"
#include "configuration.h"
#include "util/music.h"
#include "util/timedifference.h"

#include "util/exceptions/exception.h"
#include "mugen/config.h"
#include "mugen/reader.h"
#include "mugen/section.h"
#include "mugen/item-content.h"
#include "mugen/item.h"
#include "mugen/character.h"
#include "mugen/character-select.h"
#include "mugen/animation.h"
#include "mugen/sprite.h"
#include "mugen/stage.h"
#include "mugen/font.h"
#include "mugen/storyboard.h"
#include "mugen/state.h"
#include "mugen/parser/parsers.h"
#include "mugen/parser/parse-exception.h"
#include "mugen/background.h"
#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "factory/font_render.h"
#include "util/input/input-manager.h"

namespace PaintownUtil = ::Util;

using namespace std;

static void showCollision( const std::vector< Mugen::Area > &vec, Graphics::Bitmap &bmp, int x, int y, int color, int &start ){
    int next = start;
    for( unsigned int i = 0; i < vec.size(); ++i ){
	bmp.rectangle( x + vec[i].x1, y + vec[i].y1, x + vec[i].x2, y + vec[i].y2, color );
	Font::getDefaultFont().printf( 15, 380 + next, color, bmp, "CLSN: x1(%i),y1(%i),x2(%i),y2(%i)",0, vec[i].x1,vec[i].y1,vec[i].x2,vec[i].y2 );
	next+=10;
    }
    start = next;
}

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static void showOptions(){
    Global::debug(0) << "M.U.G.E.N Config Reader:" << endl;
    Global::debug(0) << "-f <file>: Load a M.U.G.E.N config file and output to screen." << endl;
    Global::debug(0) << "-c <name>: Load a M.U.G.E.N character and output some data about it.\n         ie: 'data/mugen/chars/name' only pass name." << endl;
    Global::debug(0) << "-s <name> (p1) (p2): Load a M.U.G.E.N stage and output some data about it.\n         ie: 'data/mugen/stages/name.def'.\n         (p1) and (p2) are player names and is optional." << endl;
    Global::debug(0) << "-font <file>: Load a M.U.G.E.N font and print out Hello World!" << endl;
    Global::debug(0) << "-sff <file>: Load a M.U.G.E.N SFF File and browse contents.\n         ie: 'data/mugen/data/some.sff'." << endl;
    Global::debug(0) << "-storyboard <file>: Load a M.U.G.E.N storyboard and render it!" << endl;
    Global::debug(0) << "-select <file>: Load a M.U.G.E.N select screen from a given system.def" << endl;
    Global::debug(0) << "-background <file> <section>: Load a M.U.G.E.N background from a given *.def" << endl;
    Global::debug(0) << "-l <level>: Set debug level." << endl;
    Global::debug(0) << endl;
}

namespace LocalKeyboard {
enum Keys {
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    Esc,
    Enter,
    Up,
    Down,
    Left,
    Right,
    Space,
    PageUp,
    PageDown,
    Action1,
    Action2
};

static InputMap<Keys> getKeys(){
    InputMap<Keys> input;
    input.set(Keyboard::Key_F1, 10, false, F1);
    input.set(Keyboard::Key_F2, 10, false, F2);
    input.set(Keyboard::Key_F3, 10, false, F3);
    input.set(Keyboard::Key_F4, 10, false, F4);
    input.set(Keyboard::Key_F5, 10, false, F5);
    input.set(Keyboard::Key_F6, 10, false, F6);
    input.set(Keyboard::Key_F7, 10, false, F7);
    input.set(Keyboard::Key_F8, 10, false, F8);
    input.set(Keyboard::Key_F9, 10, false, F9);
    input.set(Keyboard::Key_F10, 10, false, F10);
    input.set(Keyboard::Key_F11, 10, false, F11);
    input.set(Keyboard::Key_F12, 10, false, F12);
    input.set(Keyboard::Key_ESC, 10, false, Esc);
    input.set(Keyboard::Key_ENTER, 10, false, Enter);
    input.set(Keyboard::Key_UP, 10, false, Up);
    input.set(Keyboard::Key_DOWN, 10, false, Down);
    input.set(Keyboard::Key_LEFT, 10, false, Left);
    input.set(Keyboard::Key_RIGHT, 10, false, Right);
    input.set(Keyboard::Key_SPACE, 10, false, Space);
    input.set(Keyboard::Key_PGUP, 10, false, PageUp);
    input.set(Keyboard::Key_PGDN, 10, false, PageDown);
    input.set(Keyboard::Key_M, Action1);
    input.set(Keyboard::Key_P, Action2);
    
    return input;
}
}

/* testing testing 1 2 3 */
void testPCX(){
    unsigned char data[1 << 18];
    FILE * f = fopen("x.pcx", "r");
    int length;
    length = fread(data, sizeof(char), 1<<18, f);
    Global::debug(0) << "Size is " << length << endl;
    fclose(f);
    Graphics::Bitmap b = Graphics::memoryPCX(data, length);
    // Bitmap b("x.pcx");
    if (b.getError()){
        Global::debug(0) << "what the hell" << endl;
    }
    Graphics::Bitmap work(640, 480);
    work.circleFill(40, 40, 100, Graphics::makeColor(255, 0, 0));
    b.draw(0, 0, work);
    Global::debug(0) << "Width " << b.getWidth() << " Height " << b.getHeight() << endl;
    work.BlitToScreen();
    Util::rest(3000);
}

void showCharacter(const string & ourFile){
    /*set_color_depth(16);
      Bitmap::setGfxModeWindowed(640, 480);*/
    Global::debug(0) << "Trying to load character: " << ourFile << "..." << endl;
    Mugen::Character character = Mugen::Character(Filesystem::AbsolutePath(ourFile), 0);
    character.load();
    Global::debug(0) << "Loaded character: \"" << character.getName() << "\" successfully." << endl;
    bool quit = false;
    bool animate = false;
    bool showClsn1 = false;
    bool showClsn2 = false;
    bool moveImage = false;

    map<int,Util::ReferenceCount<Mugen::Animation> >::const_iterator it = character.getAnimations().begin();
    unsigned int currentAnim = 0;
    unsigned int lastAnim = character.getAnimations().size() -1;
    unsigned int currentFrame = 0;

    if (it->second->getFrames().size() == 0){
        Global::debug(0, __FILE__) << "No frames!" << endl;
        exit(0);
    }

    Graphics::Bitmap work( 640, 480 );

    int xaxis = 260;
    int yaxis = 230;

    double gameSpeed = .5;
    double runCounter = 0;
    InputMap<LocalKeyboard::Keys> input = LocalKeyboard::getKeys();
    while( !quit ){
        bool draw = false;

        if ( Global::speed_counter4 > 0 ){
            runCounter += Global::ticksPerSecond(60) * Global::speed_counter4 * gameSpeed;
            while (runCounter > 1){
                if( animate ) it->second->logic();
                runCounter -= 1;
                draw = true;

                /* FIXME: events */
                /*
                InputMap<LocalKeyboard::Keys>::Output out = InputManager::getMap(input);
                if( out[LocalKeyboard::Up] ){
                    if( currentAnim < lastAnim ){
                        currentAnim++;
                        it++;
                    }
                    currentFrame = 0;
                }
                else if( out[LocalKeyboard::Down] ){
                    if( currentAnim > 0 ){
                        currentAnim--;
                        it--;
                    }
                    currentFrame = 0;
                }
                else if( out[LocalKeyboard::Left] && !animate){
                    it->second->forwardFrame();
                    currentFrame = it->second->getCurrentPosition();
                }
                else if( out[LocalKeyboard::Right] && !animate){
                    it->second->backFrame();
                    currentFrame = it->second->getCurrentPosition();
                }
                else if( out[LocalKeyboard::Space] ){
                    animate = !animate;
                }
                else if( out[LocalKeyboard::F1] ){
                    showClsn1 = !showClsn1;
                    it->second->toggleOffense();
                }
                else if( out[LocalKeyboard::F2] ){
                    showClsn2 = !showClsn2;
                    it->second->toggleDefense();
                }
                else if( out[LocalKeyboard::F3] ){
                    character.priorPalette();
                }
                else if( out[LocalKeyboard::F4] ){
                    character.nextPalette();
                }

                if( mouse_b & 1 )moveImage = true;
                else if( !(mouse_b & 1) )moveImage = false;

                quit |= out[LocalKeyboard::Esc];

                if( moveImage ){ xaxis=mouse_x; yaxis =mouse_y; }
                */
            }
            Global::speed_counter4 = 0;
        }

        if (draw){
            work.clear();
            it->second->render( xaxis, yaxis, work );
            int start = 10;
            if( showClsn2 )showCollision( it->second->getCurrentFrame()->defenseCollision, work, xaxis, yaxis, Graphics::makeColor( 0,255,0 ), start  );
            if( showClsn1 )showCollision( it->second->getCurrentFrame()->attackCollision, work, xaxis, yaxis,  Graphics::makeColor( 255,0,0 ), start  );

            Font::getDefaultFont().printf( 15, 310, Graphics::makeColor( 0, 255, 0 ), work, "Name: %s",0, character.getName().c_str() );
            Font::getDefaultFont().printf( 15, 320, Graphics::makeColor( 255, 255, 255 ), work, "Current Animation: %i (%s) [%i/%i], Frame: %i, xoffset: %i, yoffset: %i", 0, it->first, Mugen::Animation::getName(Mugen::AnimationType(it->first)).c_str() ,currentAnim,character.getAnimations().size(),currentFrame, it->second->getFrames()[currentFrame]->xoffset, it->second->getFrames()[currentFrame]->yoffset );
            if(it->second->getCurrentFrame()->sprite!=0)Font::getDefaultFont().printf( 15, 330, Graphics::makeColor( 255, 255, 255 ), work, "Length: %d | x-axis: %d | y-axis: %d | Group: %d | Image: %d",0, it->second->getCurrentFrame()->sprite->getLength(), it->second->getCurrentFrame()->sprite->getX(), it->second->getCurrentFrame()->sprite->getY(), it->second->getCurrentFrame()->sprite->getGroupNumber(), it->second->getCurrentFrame()->sprite->getImageNumber());
            Font::getDefaultFont().printf( 15, 340, Graphics::makeColor( 255, 255, 255 ), work, "Bitmap info - Width: %i Height: %i",0, it->second->getCurrentFrame()->sprite->getWidth(), it->second->getCurrentFrame()->sprite->getHeight() );
            Font::getDefaultFont().printf( 15, 350, Graphics::makeColor( 255, 255, 255 ), work, "(space) Animation enabled:            %i",0, animate );
            Font::getDefaultFont().printf( 15, 360, Graphics::makeColor( 255, 255, 255 ), work, "(d)     Show Defense enabled (green): %i",0, showClsn2 );
            Font::getDefaultFont().printf( 15, 370, Graphics::makeColor( 255, 255, 255 ), work, "(a)     Show Attack enabled (red):    %i",0, showClsn1 );

            // show_mouse(work.getData().getBitmap());

            work.BlitToScreen();
            Util::rest(1);
        }

        while (Global::speed_counter4 == 0){
            Util::rest(1);
            InputManager::poll();
        }
    }

}

const char *music[] = {
    "tr3blanka.it",
    "tr3deejay.it",
    "tr3e-honda.it",
    "tr3guile3rd.it",
    "tr3ken3rd.it",
    "tr3ryu2nd.it"
};

const int musicHits = sizeof(music) / sizeof(char*);

void showStage(const string & ourFile, const string &p1_name, const string &p2_name){
    try{
	std::string file = ourFile;
	Global::debug(0) << "\"" << Mugen::Stage::getStageName(file) << "\"" << endl;
    } catch (MugenException &ex){
	Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
	exit(1);
    }

    Global::debug(0) << "Trying to load stage: " << ourFile << "..." << endl;
    Mugen::Stage stage = Mugen::Stage(Filesystem::AbsolutePath(ourFile));
    //stage.load();
    Global::debug(0) << "Loaded stage: \"" << stage.getName() << "\" successfully." << endl;
    bool quit = false;
    
    Graphics::Bitmap work( 320, 240 );
    Graphics::Bitmap back( 640, 480 );
    
    // Get players
    Global::debug(0) << "Loading player 1" << endl;
    const Filesystem::AbsolutePath motif = Mugen::Data::getInstance().getMotif();
#if 0
    Mugen::CharacterSelect selector(motif, Mugen::Player1, Mugen::Arcade);
    try {
	selector.load();
	selector.setPlayer1Keys(Mugen::getPlayer1Keys(20));
        /* FIXME: need a searcher */
	// selector.run("Test", Graphics::Bitmap::temporaryBitmap(640,480));
    } catch (const MugenException &me){
	Global::debug(0) << "Error loading select screen. Reason: " << me.getReason() << endl;
    } catch (const Exception::Return &e){
	return;
    }
    selector.setNextArcadeMatch();
    Mugen::Character *player1 = new Mugen::Character(selector.getPlayer1Def(), 0);
    Mugen::Character *player2 = new Mugen::Character(selector.getPlayer2Def(), 1);
    player1->load();
    player2->load();
    stage.addPlayer1(player1);
    stage.addPlayer2(player2);
    stage.load();
    stage.reset();
    double gameSpeed = 1.0;
    double runCounter = 0;
    
    Music m(true);
    int track = rand() % (musicHits -1);
    if(Music::loadSong( Storage::instance().find(Filesystem::RelativePath(string("mugen/music/") + music[track])).path())){
        Music::pause();
        Music::play();
        Global::debug(0) << "Now playing track: " << music[track] << endl;
    }
    
    
    InputMap<LocalKeyboard::Keys> input = LocalKeyboard::getKeys();
    
    while( !quit ){
        bool draw = false;
        
        if ( Global::speed_counter4 > 0 ){
            runCounter += Global::speed_counter4 * gameSpeed * Global::ticksPerSecond(60);
            while (runCounter > 1){
		InputManager::poll();
                stage.logic();
                runCounter -= 1;

                /*
		InputMap<LocalKeyboard::Keys>::Output out = InputManager::getMap(input);    
                draw = true;

                if( out[LocalKeyboard::F2] ){
                    stage.reset();
                }
		if( out[LocalKeyboard::F1] ){
		    stage.toggleConsole();
		}
		// Change music track
		if( out[LocalKeyboard::F3] ){
		    track++;
		    if(track == musicHits)track = 0;
		    if(Music::loadSong(Filesystem::find(Filesystem::RelativePath(string("mugen/music/") + music[track])).path())){
			Music::pause();
			Music::play();
			Global::debug(0) << "Now playing track: " << music[track] << endl;
		    }
		}
                if( out[LocalKeyboard::F4] ){
                    stage.Quake( 5 );
                }
		if (out[LocalKeyboard::F5] ){
		    stage.toggleDebug();
		}

                quit |= out[LocalKeyboard::Esc] ;
                */
            }
            Global::speed_counter4 = 0;
        }

        if (draw){
            stage.render(&work);
	    work.Stretch(back);
            back.BlitToScreen();
        }

        while (Global::speed_counter4 == 0){
            Util::rest(1);
        }
    }
    
    delete player1;
    delete player2;
#endif
}

void showFont(const string & ourFile){
    Global::debug(0) << "Trying to load font: " << ourFile << "..." << endl;
    Mugen::Font font( Storage::instance().find(Filesystem::RelativePath(ourFile)));
    Global::debug(0) << "Loaded font: \"" << ourFile << "\" successfully." << endl;
    
    bool quit = false;
    
    Graphics::Bitmap back( 640, 480 );
    
    double gameSpeed = 1.0;
    double runCounter = 0;
    
    int currentBank = 0;
     
    InputMap<LocalKeyboard::Keys> input = LocalKeyboard::getKeys();
    
    int backgroundChange = 1;
    int backgroundColor = 0;
   
    while( !quit ){
        bool draw = false;
        
        if ( Global::speed_counter4 > 0 ){
            runCounter += Global::speed_counter4 * gameSpeed * Global::ticksPerSecond(60);
            while (runCounter > 1){
                InputManager::poll();
                runCounter -= 1;
                draw = true;
                vector<InputMap<LocalKeyboard::Keys>::InputEvent> events = InputManager::getEvents(input, InputSource());
                for (vector<InputMap<LocalKeyboard::Keys>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                    const InputMap<LocalKeyboard::Keys>::InputEvent & event = *it;
                    if (event.enabled){
                        if (event.out == LocalKeyboard::Down || event.out == LocalKeyboard::Right){
                            if (currentBank > 0){
                                currentBank--;
                            }
                        } else if (event.out == LocalKeyboard::Up || event.out == LocalKeyboard::Left){
                            if (currentBank < font.getTotalBanks()-1){
                                currentBank++;
                            }
                        } else if (event.out == LocalKeyboard::Esc){
                            quit = true;
                        }
                    }
                }
                
                backgroundColor += backgroundChange;
                if (backgroundColor > 255){
                    backgroundColor = 255;
                    backgroundChange = -1;
                } else if (backgroundColor < 0){
                    backgroundColor = 0;
                    backgroundChange = 1;
                }
                
            }
        
        Global::speed_counter4 = 0;
        }
        
        if (draw){
            back.clear();
            Graphics::StretchedBitmap work(480, 480, back);
            work.fill(Graphics::makeColor(backgroundColor,backgroundColor,backgroundColor));
            work.start();
            font.render( 240, 120, 0, currentBank, work, "0123456789");
            work.line(0, 120, 480, 120, Graphics::makeColor(255,0,0));
            font.render( 240, 120 + font.getHeight(), 0, currentBank, work, "ABCDEFGHIJKLN");
            work.line(0, 120 + font.getHeight(), 480, 120 + font.getHeight(), Graphics::makeColor(255,0,0));
            font.render( 240, 120 + font.getHeight() * 2, 0, currentBank, work, "MNOPQRSTUVWXYZ");
            work.line(0, 120 + font.getHeight() * 2, 480, 120 + font.getHeight() * 2, Graphics::makeColor(255,0,0));
            std::ostringstream out;
            out << "CURRENT BANK SET TO " << currentBank;
            font.render( 240, 120 + font.getHeight() * 3, 0, currentBank, work, out.str());
            work.line(0, 120 + font.getHeight() * 3, 480, 120 + font.getHeight() * 3, Graphics::makeColor(255,0,0));
            work.finish();
            back.BlitToScreen();
        }

        while (Global::speed_counter4 == 0){
            Util::rest(1);
        }
    }
}

void showSFF(const string & ourFile, const std::string &actFile){
    std::map< unsigned int, std::map< unsigned int, Util::ReferenceCount<Mugen::Sprite> > > sprites;
    int currentGroup = 0;
    int currentSprite = 0;
    Global::debug(0) << "Trying to load SFF File: " << ourFile << "..." << endl;
    TimeDifference timer;
    timer.startTime();
    Mugen::Util::readSprites(Filesystem::AbsolutePath(ourFile), Filesystem::AbsolutePath(actFile), sprites, false);
    timer.endTime();
    Global::debug(0) << "Loaded SFF file: \"" << ourFile << "\" successfully " << timer.printTime("in") << endl;
    
    bool quit = false;

    /*Bitmap work( 320, 240 );*/
    Graphics::Bitmap back(640, 480);
    
    double gameSpeed = 1.0;
    double runCounter = 0;
    bool mask = false;
    
    InputMap<LocalKeyboard::Keys> input = LocalKeyboard::getKeys();
   
    while (!quit){
        bool draw = false;
        
        if (Global::speed_counter4 > 0){
            runCounter += Global::speed_counter4 * gameSpeed * Global::ticksPerSecond(60);
            while (runCounter > 1){
                InputManager::poll();
                runCounter -= 1;
                draw = true;

                vector<InputMap<LocalKeyboard::Keys>::InputEvent> events = InputManager::getEvents(input, InputSource());
                for (vector<InputMap<LocalKeyboard::Keys>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
                    const InputMap<LocalKeyboard::Keys>::InputEvent & event = *it;
                    if (event.enabled){
                        if (event.out == LocalKeyboard::Down){
                            int group = currentGroup;
                            bool found = false;
                            currentSprite = 0;
                            while (!found){
                                group--;
                                if (sprites[group][currentSprite] != NULL){
                                    found = true;
                                    currentGroup = group;
                                }
                                if (group < 0){
                                    group = 99999;
                                }
                                if (group == currentGroup){
                                    found = true;
                                }
                            }
                        } else if (event.out == LocalKeyboard::Up){
                            int group = currentGroup;
                            bool found = false;
                            currentSprite = 0;
                            while (!found){
                                group++;
                                if (sprites[group][currentSprite] != NULL){
                                    found = true;
                                    currentGroup = group;
                                }
                                if (group > 99999){
                                    group = -1;
                                }
                                if (group == currentGroup){
                                    found = true;
                                }
                            }
                        } else if (event.out == LocalKeyboard::Left){
                            int sprite = currentSprite;
                            bool found = false;
                            while (!found){
                                sprite--;
                                if (sprites[currentGroup][sprite] != NULL){
                                    found = true;
                                    currentSprite = sprite;
                                }
                                if (sprite < 0){
                                    sprite = 1000;
                                }
                                if (sprite == currentSprite){
                                    found = true;
                                }
                            }
                        } else if (event.out == LocalKeyboard::Right){
                            int sprite = currentSprite;
                            bool found = false;
                            while (!found){
                                sprite++;
                                if (sprites[currentGroup][sprite] != NULL){
                                    found = true;
                                    currentSprite = sprite;
                                }
                                if (sprite > 1000){
                                    sprite = -1;
                                }
                                if (sprite == currentSprite){
                                    found = true;
                                }
                            }
                        } else if (event.out == LocalKeyboard::PageUp){
                            currentGroup += 500;
                        } else if (event.out == LocalKeyboard::PageDown){
                            currentGroup -= 500;
                        } else if (event.out == LocalKeyboard::Esc){
                            quit = true;
                        } else if (event.out == LocalKeyboard::Action1){
                            mask = ! mask;
                        }
                    }
                }
            }

            Global::speed_counter4 = 0;
        }

        if (draw){
	    back.clear();
            PaintownUtil::ReferenceCount<Mugen::Sprite> ourSprite = sprites[currentGroup][currentSprite];

            back.rectangleFill(0, 0, back.getWidth(), back.getHeight() * 3 / 2, Graphics::makeColor(32, 32, 32));
            back.line(0, back.getHeight() / 2, back.getWidth(), back.getHeight() / 2, Graphics::makeColor(255, 255, 255));
            back.line(back.getWidth() / 2, 0, back.getWidth() / 2, back.getHeight(), Graphics::makeColor(255, 255, 255));

	    if (ourSprite != NULL){
		Mugen::Effects effects;
                effects.mask = mask;
                
		ourSprite->render(back.getWidth() / 2, back.getHeight() / 2, back, effects);
                int y = 400;
		Font::getDefaultFont().printf(15, y, Graphics::makeColor(0, 255, 0), back, "Current Group: %d/%d   -----   Current Sprite: %d/%d (M)ask %s Own (P)alette %s",0, currentGroup, sprites.rbegin()->first, currentSprite, sprites[currentGroup].rbegin()->first, mask ? "on" : "off"); y += Font::getDefaultFont().getHeight() + 3;
		Font::getDefaultFont().printf(15, y, Graphics::makeColor(0, 255, 0), back, "Same palette? %s Real Length %d New Length %d", 0, ourSprite->getSamePalette() ? "yes" : "no", ourSprite->getRealLength(), ourSprite->getNewLength()); y += Font::getDefaultFont().getHeight() + 3;
		Font::getDefaultFont().printf(15, y, Graphics::makeColor(0, 255, 0), back, "Width %d Height %d", 0, ourSprite->getWidth(), ourSprite->getHeight());
	    } else {
		Font::getDefaultFont().printf(15, 400, Graphics::makeColor(0, 255, 0), back, "Not valid group or Sprite! Current Group: %d   -----   Current Sprite: %d ",0, currentGroup, currentSprite );
	    }
	    /*work.Stretch(back);*/
            back.BlitToScreen();
        }

        while (Global::speed_counter4 == 0){
            Util::rest(1);
        }
    }
}

void doSelectScreen(const std::string &file){
#if 0
    Mugen::CharacterSelect selector = Mugen::CharacterSelect(Filesystem::AbsolutePath(file), Mugen::Player1, Mugen::Versus);
    try {
	selector.load();
	selector.setPlayer1Keys(Mugen::getPlayer1Keys());
	selector.setPlayer2Keys(Mugen::getPlayer2Keys());
        /* FIXME */
	// selector.run("Test", Graphics::Bitmap::temporaryBitmap(640,480));
	selector.renderVersusScreen();
    } catch (const MugenException &me){
	Global::debug(0) << "Error loading select screen. Reason: " << me.getReason() << endl;
    }
#endif
}

void doBackground(const std::string &file, const std::string &section){
    Mugen::Background background = Mugen::Background(Filesystem::AbsolutePath(file), section);
    Graphics::Bitmap workArea(320,240);
    Graphics::Bitmap screen(640,480);
    
    double runCounter = 0;
    Global::speed_counter4 = 0;
    Global::second_counter = 0;
    int game_time = 100;
    
    // Set game keys temporary
    InputMap<int> gameInput;
    gameInput.set(Keyboard::Key_ESC, 10, true, 0);
    gameInput.set(Keyboard::Key_UP, 1, false, 1);
    gameInput.set(Keyboard::Key_DOWN, 1, false, 2);
    gameInput.set(Keyboard::Key_LEFT, 1, false, 3);
    gameInput.set(Keyboard::Key_RIGHT, 1, false, 4);
    gameInput.set(Keyboard::Key_F1, 10, true, 5);
    gameInput.set(Keyboard::Key_LSHIFT, 1, false, 6);
    
    bool done = false;

    Mugen::Point camera;
    
    while ( ! done ){
    
	bool draw = false;
	
	if ( Global::speed_counter4 > 0 ){
	    draw = true;
	    runCounter += Global::speed_counter4 * Global::ticksPerSecond(60);
	    while ( runCounter >= 1.0 ){
		runCounter -= 1;
		// Key handler
		InputManager::poll();
		
                /*
		InputMap<int>::Output out = InputManager::getMap(gameInput);
		if (out[0]){
		    done = true;
		}

                int speed = 1;
                if (out[6]){
                    speed = 10;
                }

                if (out[1]){
		    camera.y -= speed;
		}
                if (out[2]){
		    camera.y += speed;
		}
                if (out[3]){
		    camera.x -= speed;
		}
                if (out[4]){
		    camera.x += speed;
		}
                if (out[5]){
                    // Reset camera
                    camera.x = camera.y = 0;
                }
                */
		
		// Backgrounds
		background.act();
	    }
	    
	    Global::speed_counter4 = 0;
	}
		
	while ( Global::second_counter > 0 ){
	    game_time--;
	    Global::second_counter--;
	    if ( game_time < 0 ){
		    game_time = 0;
	    }
	}

	if ( draw ){
	    // render backgrounds
	    background.renderBackground(camera.x, camera.y, workArea);
	    
	    // render Foregrounds
	    background.renderForeground(camera.x, camera.y, workArea);

            // This is a reminder of where the current 0,0 position is
            workArea.vLine(0,160,240,Graphics::makeColor(0,255,0));
            Font::getDefaultFont().printf( 5, 0, Graphics::makeColor(255,0,0), workArea, "Camera X: %i",0, camera.x );
            Font::getDefaultFont().printf( 5, 10, Graphics::makeColor(255,0,0), workArea, "Camera Y: %i",0, camera.y );
	    
	    // Finally render to screen
	    workArea.Stretch(screen);
	    screen.BlitToScreen();
	}

	while ( Global::speed_counter4 < 1 ){
		PaintownUtil::rest( 1 );
	}
    }
}

int main( int argc, char ** argv ){
	
	if(argc <= 1){
	    showOptions();
	    return 0;
	}

	const char * FILE_ARG = "-f";
	const char * CHAR_ARG = "-c";
        const char * DEBUG_ARG = "-l";
	const char * STAGE_ARG = "-s";
	const char * FONT_ARG = "-font";
        const char * PARSE_ARG = "-parse";
        const char * PARSE_AIR_ARG = "-parse-air";
        const char * PARSE_CNS_ARG = "-parse-cns";
        const char * PARSE_CMD_ARG = "-parse-cmd";
        const char * PARSE_DEF_ARG = "-parse-def";
	const char * STORY_ARG = "-storyboard";
	const char * SFF_ARG = "-sff";
	const char * SELECT_ARG = "-select";
	const char * BG_ARG = "-background";
	std::string ourFile;
	std::string other;
	int configLoaded = -1;
	
	std::string player1_name = "";
	std::string player2_name = "";

        /*allegro_init();
        install_timer();
	install_keyboard();
	install_mouse();*/

	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], FILE_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 0;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}

#if 0
        } else if (isArg(argv[q], PARSE_AIR_ARG)){
                    q += 1;
                    if (q < argc){
                        try{
                            Mugen::parseAir(argv[q]);
                        } catch (const Mugen::ParserException & p){
                            Global::debug(0) << "Error parsing: " << p.getReason() << endl;
                            return -1;
                        }
                        return 0;
                    }
                } else if (isArg(argv[q], PARSE_CNS_ARG)){
                    q += 1;
                    if (q < argc){
                        try{
                            Mugen::parseCns(argv[q]);
                        } catch (const Mugen::ParserException & p){
                            Global::debug(0) << "Error parsing: " << p.getReason() << endl;
                            return -1;
                        }
                        return 0;
                    }
                } else if (isArg(argv[q], PARSE_CMD_ARG)){
                    q += 1;
                    if (q < argc){
                        try{
                            Mugen::parseCmd(argv[q]);
                        } catch (const Mugen::ParserException & p){
                            Global::debug(0) << "Error parsing: " << p.getReason() << endl;
                            return -1;
                        }
                        return 0;
                    }
                } else if (isArg(argv[q], PARSE_DEF_ARG)){
                    q += 1;
                    if (q < argc){
                        // for (int i = 0; ; i++){
                            // printf("%d\n", i);
                            try{
                                Mugen::parseDef(argv[q]);
                            } catch (const Mugen::ParserException & p){
                                Global::debug(0) << "Error parsing: " << p.getReason() << endl;
                                return -1;
                            }
                        // }
                        return 0;
                    }
#endif
                    /*
                } else if (isArg(argv[q], PARSE_ARG)){
                    q += 1;
                    if (q < argc){
                        extern Ast::Configuration * (mugenParse)(std::string filename);
                        string s(argv[q]);
                        Ast::Configuration * configuration = mugenParse(s);
                        if (!configuration){
                            Global::debug(0) << "Could not parse " << s << endl;
                        } else {
                            configuration->debugExplain();
                        }
                    }
                    */
		} else if ( isArg( argv[ q ], CHAR_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 1;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if ( isArg( argv[ q ], STAGE_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 2;
				/*
				q += 1;
				if (isArg(argv[q], DEBUG_ARG))goto debuglevel;
				// player 1
				if (q < argc ){
				    player1_name = "data/players/" + std::string( argv[ q ] ) + "/" + std::string( argv[ q ] ) + ".txt";
				}
				q += 1;
				if (isArg(argv[q], DEBUG_ARG))goto debuglevel;
				// player 2
				if (q < argc ){
				    player2_name = "data/players/" + std::string( argv[ q ] ) + "/" + std::string( argv[ q ] ) + ".txt";
				}*/
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if ( isArg( argv[ q ], FONT_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 3;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if ( isArg( argv[ q ], STORY_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 4;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if ( isArg( argv[ q ], SFF_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 5;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
			q += 1;
			if ( q < argc ){
				other = std::string( argv[ q ] );
			}
		} else if ( isArg( argv[ q ], SELECT_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 6;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if ( isArg( argv[ q ], BG_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = 7;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
			q += 1;
			if ( q < argc ){
				other = std::string( argv[ q ] );
			}
			else{
                            Global::debug(0) << "Error no Section Given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if (isArg(argv[q], DEBUG_ARG)){
		    //debuglevel:
                    q += 1;
                    if (q < argc){
                        istringstream i( argv[ q ] );
                        int f;
                        i >> f;
                        Global::setDebug( f );
                    } else {
                        Global::debug(0) << "No number given for " << DEBUG_ARG << endl;
                    }
                } else {
		    // WHAT?
		    // showOptions();
		    // return 0;
                    Global::debug(0) << "Ignoring unrecognized option " << argv[q] << endl;
		}
	}
	
        Global::init(Global::WINDOWED);

        InputManager input;
    
        Keyboard::pushRepeatState(true);
	
	if( configLoaded == 0 ){
	    MugenReader reader( ourFile );
	    
	    std::vector< MugenSection * > collection;
	    
	    try{
		collection = reader.getCollection();
                Global::debug(0) << endl << "---------------------------------------------------------" << endl;
		for( unsigned int i = 0; i < collection.size(); ++i ){
                    Global::debug(0) << collection[i]->getHeader() << endl;
                    Global::debug(0) << "---------------------------------------------------------" << endl;
		    while( collection[i]->hasItems() ){
			MugenItemContent *content = collection[i]->getNext();
                        while( content->hasItems() ){
                            Global::debug(0) << content->getNext()->query();
                            if( content->hasItems() ) Global::debug(0) << ",";
                        }
                        Global::debug(0) << endl;
		    }
                    Global::debug(0) << "---------------------------------------------------------" << endl;
		}
                Global::debug(0) << endl;
	    }
	    catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    }
	}
	else if (configLoaded == 1){
	    try{
                showCharacter(ourFile);
            } catch(const MugenException & ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
            } catch (const Filesystem::NotFound & found){
                Global::debug(0) << "Could not find a file: " << found.getTrace() << endl;
                return 1;
	    } catch(...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	}
	else if ( configLoaded == 2 ){
	    try{
                showStage(ourFile, player1_name, player2_name);
            } catch(const MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch(...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	}
	else if ( configLoaded == 3 ){
	    try{
                showFont(ourFile);
            } catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch(...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	}
	else if ( configLoaded == 4 ){
	    try{
                Mugen::Storyboard story = Mugen::Storyboard(Filesystem::AbsolutePath(ourFile), true);
		story.setInput(Mugen::getPlayer1Keys());
		// run it and repeat
		story.run(true);
            } catch (const MugenException & ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch (const Filesystem::NotFound & found){
                Global::debug(0) << "Could not find file: " << found.getTrace() << endl;
            } catch (...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	} else if ( configLoaded == 5 ){
	    try{
                showSFF(ourFile, other);
            } catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch(...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	} else if ( configLoaded == 6 ){
	    try{
                doSelectScreen(ourFile);
            } catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch(...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	} else if ( configLoaded == 7 ){
	    try{
                doBackground(ourFile,other);
            } catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch(...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	} else showOptions();
	
	return 0;
}
#ifdef ALLEGRO
END_OF_MAIN()
#endif
