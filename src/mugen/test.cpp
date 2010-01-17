#include <allegro.h>

#ifdef _WIN32
#define BITMAP dummyBITMAP
#include <windows.h>
#undef BITMAP
#endif

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <map>

#include "globals.h"
#include "init.h"
#include "configuration.h"
#include "music.h"

#include "mugen_reader.h"
#include "mugen_section.h"
#include "mugen_item_content.h"
#include "mugen_item.h"
#include "character.h"
#include "character_select.h"
#include "mugen_animation.h"
#include "mugen_sprite.h"
#include "mugen_stage.h"
#include "mugen_font.h"
#include "mugen_storyboard.h"
#include "state.h"
#include "parser/parsers.h"
#include "parser/parse-exception.h"

#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/file-system.h"

#include "level/utils.h"

#include "factory/font_render.h"

#include "gui/keyinput_manager.h"

#include "object/object.h"
#include "object/player.h"
#include "object/versus_player.h"
#include "object/versus_enemy.h"
#include "game.h"

#include "dumb/include/dumb.h"
#include "dumb/include/aldumb.h"
#include "loadpng/loadpng.h"

#include "ast/Configuration.h"
#include "input/input-manager.h"

using namespace std;

static void showCollision( const std::vector< MugenArea > &vec, Bitmap &bmp, int x, int y, int color, int &start ){
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
    Global::debug(0) << "-l <level>: Set debug level." << endl;
    Global::debug(0) << endl;
}

/* testing testing 1 2 3 */
void testPCX(){
    unsigned char data[1 << 18];
    FILE * f = fopen("x.pcx", "r");
    int length;
    length = fread(data, sizeof(char), 1<<18, f);
    Global::debug(0) << "Size is " << length << endl;
    fclose(f);
    Bitmap b = Bitmap::memoryPCX(data, length);
    // Bitmap b("x.pcx");
    if (b.getError()){
        Global::debug(0) << "what the hell" << endl;
    }
    Bitmap work(640, 480);
    work.circleFill(40, 40, 100, Bitmap::makeColor(255, 0, 0));
    b.draw(0, 0, work);
    Global::debug(0) << "Width " << b.getWidth() << " Height " << b.getHeight() << endl;
    work.BlitToScreen();
    Util::rest(3000);
}

/*
static void inc_speed_counter() {
	Global::speed_counter += 1;
}
END_OF_FUNCTION( inc_speed_counter );
*/

void showCharacter(const string & ourFile){
    /*set_color_depth(16);
    Bitmap::setGfxModeWindowed(640, 480);*/
    Global::debug(0) << "Trying to load character: " << ourFile << "..." << endl;
    Mugen::Character character( ourFile );
    character.load();
    Global::debug(0) << "Loaded character: \"" << character.getName() << "\" successfully." << endl;
    bool quit = false;
    bool animate = false;
    bool showClsn1 = false;
    bool showClsn2 = false;
    bool moveImage = false;
   
    map<int,MugenAnimation*>::const_iterator it = character.getAnimations().begin();
    unsigned int currentAnim = 0;
    unsigned int lastAnim = character.getAnimations().size() -1;
    unsigned int currentFrame = 0;
    
    if (it->second->getFrames().size() == 0){
        Global::debug(0, __FILE__) << "No frames!" << endl;
        exit(0);
    }

    Bitmap work( 640, 480 );
    
    int xaxis = 260;
    int yaxis = 230;

     double gameSpeed = .5;
    double runCounter = 0;
    /*
    LOCK_VARIABLE( speed_counter );
    LOCK_FUNCTION( (void *)inc_speed_counter );
    */
	while( !quit ){
	    bool draw = false;
	    
	    if ( Global::speed_counter > 0 ){
		runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
		while (runCounter > 1){
		    if( animate ) it->second->logic();
		    runCounter -= 1;
		    draw = true;
		    
		    if( keyInputManager::keyState(keys::UP, true) ){
			if( currentAnim < lastAnim ){
			    currentAnim++;
			    it++;
			}
			currentFrame = 0;
		    }
		    else if( keyInputManager::keyState(keys::DOWN, true) ){
			if( currentAnim > 0 ){
			    currentAnim--;
			    it--;
			}
			currentFrame = 0;
		    }
		    else if( keyInputManager::keyState(keys::LEFT, true) && !animate){
			it->second->forwardFrame();
			currentFrame = it->second->getCurrentPosition();
		    }
		    else if( keyInputManager::keyState(keys::RIGHT, true) && !animate){
			it->second->backFrame();
			currentFrame = it->second->getCurrentPosition();
		    }
		    else if( keyInputManager::keyState(keys::SPACE, true) ){
			animate = !animate;
		    }
		    else if( keyInputManager::keyState('a', true) ){
			showClsn1 = !showClsn1;
			it->second->toggleOffense();
		    }
		    else if( keyInputManager::keyState('d', true) ){
			showClsn2 = !showClsn2;
			it->second->toggleDefense();
		    }
		    else if( keyInputManager::keyState('1', true) ){
			character.priorPalette();
		    }
		    else if( keyInputManager::keyState('2', true) ){
			character.nextPalette();
		    }
		    
		    if( mouse_b & 1 )moveImage = true;
		    else if( !(mouse_b & 1) )moveImage = false;
		    
		    quit |= keyInputManager::keyState(keys::ESC, true );
		    
		    if( moveImage ){ xaxis=mouse_x; yaxis =mouse_y; }
		}
		Global::speed_counter = 0;
        }

        if (draw){
		    work.clear();
		    it->second->render( xaxis, yaxis, work );
		    int start = 10;
		    if( showClsn2 )showCollision( it->second->getCurrentFrame()->defenseCollision, work, xaxis, yaxis, Bitmap::makeColor( 0,255,0 ), start  );
		    if( showClsn1 )showCollision( it->second->getCurrentFrame()->attackCollision, work, xaxis, yaxis,  Bitmap::makeColor( 255,0,0 ), start  );
		    
		    Font::getDefaultFont().printf( 15, 310, Bitmap::makeColor( 0, 255, 0 ), work, "Name: %s",0, character.getName().c_str() );
		    Font::getDefaultFont().printf( 15, 320, Bitmap::makeColor( 255, 255, 255 ), work, "Current Animation: %i (%s) [%i/%i], Frame: %i, xoffset: %i, yoffset: %i", 0, it->first, MugenAnimation::getName(Mugen::AnimationType(it->first)).c_str() ,currentAnim,character.getAnimations().size(),currentFrame, it->second->getFrames()[currentFrame]->xoffset, it->second->getFrames()[currentFrame]->yoffset );
		    if(it->second->getCurrentFrame()->sprite!=0)Font::getDefaultFont().printf( 15, 330, Bitmap::makeColor( 255, 255, 255 ), work, "Length: %d | x-axis: %d | y-axis: %d | Group: %d | Image: %d",0, it->second->getCurrentFrame()->sprite->getLength(), it->second->getCurrentFrame()->sprite->getX(), it->second->getCurrentFrame()->sprite->getY(), it->second->getCurrentFrame()->sprite->getGroupNumber(), it->second->getCurrentFrame()->sprite->getImageNumber());
		    Font::getDefaultFont().printf( 15, 340, Bitmap::makeColor( 255, 255, 255 ), work, "Bitmap info - Width: %i Height: %i",0, it->second->getCurrentFrame()->sprite->getWidth(), it->second->getCurrentFrame()->sprite->getHeight() );
		    Font::getDefaultFont().printf( 15, 350, Bitmap::makeColor( 255, 255, 255 ), work, "(space) Animation enabled:            %i",0, animate );
		    Font::getDefaultFont().printf( 15, 360, Bitmap::makeColor( 255, 255, 255 ), work, "(d)     Show Defense enabled (green): %i",0, showClsn2 );
		    Font::getDefaultFont().printf( 15, 370, Bitmap::makeColor( 255, 255, 255 ), work, "(a)     Show Attack enabled (red):    %i",0, showClsn1 );
		   
		    show_mouse(work.getBitmap());

		    work.BlitToScreen();
		    Util::rest(1);
	}
	
        while (Global::speed_counter == 0){
            Util::rest(1);
            keyInputManager::update();
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
    /*set_color_depth(16);
    Bitmap::setGfxModeWindowed(640, 480);
    loadpng_init();*/
    /*
    LOCK_VARIABLE( speed_counter );
    LOCK_FUNCTION( (void *)inc_speed_counter );
    */
    /* set up the timers */
    //install_int_ex( inc_speed_counter, BPS_TO_TIMER( 40 ) );
    
    try{
	std::string file = ourFile;
	Global::debug(0) << "\"" << MugenStage::getStageName(file) << "\"" << endl;
    } catch (MugenException &ex){
	Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
	exit(1);
    }

    Global::debug(0) << "Trying to load stage: " << ourFile << "..." << endl;
    MugenStage stage( ourFile );
    stage.load();
    Global::debug(0) << "Loaded stage: \"" << stage.getName() << "\" successfully." << endl;
    bool quit = false;
    
    Bitmap work( 320, 240 );
    Bitmap back( 640, 480 );
    
    // Get players
    Global::debug(0) << "Loading player 1" << endl;
    //Object *p1 = new Player( "data/players/wolverine/wolverine.txt", 0 );//Game::selectPlayer( false, "Pick player1" );
    /*Level::LevelInfo info;
    Object *p1 = !p1_name.empty() ? new Player(p1_name,0) : Game::selectPlayer( false, "Pick player1", info);
    Global::debug(0) << "Loading player 2" << endl;
    //Object *p2 = new Player( "data/players/venom/venom.txt", 1 );//Game::selectPlayer( false, "Pick player2" );
    Object *p2 = !p2_name.empty() ? new Player(p2_name,1) : Game::selectPlayer( false, "Pick player2", info);
    ((Player *)p1)->setInvincible( false );
    //p1->setMap( remap );
    ((Player *)p1)->testAnimation();
    ((Player *)p1)->setConfig(0);
    //Configuration::config(0).setJump( Configuration::config(0).getUp() );
    ((Player *)p2)->setInvincible( false );
    //p2->setMap( remap );
    ((Player *)p2)->testAnimation();
    //((Player *)p2)->setConfig(1);
    // Make them versus players
    VersusPlayer p1v( *(Player *) p1 );
    VersusEnemy p2v( *(Player *) p2 );
    //VersusPlayer p2v( *(Player *) p2 );
    */
    int ticker = 0;
    Mugen::CharacterSelect selector(ticker,"data/mugen/data/system.def");
    selector.load();
    Mugen::SelectedChars *gameInfo = selector.run("Select a Character", 1, false, &back);
    stage.addp1(gameInfo->team1[0]);
    //stage.addp1(&p1v);
    //stage.addp2(&p2v);

    double gameSpeed = 1.0;
    double runCounter = 0;
    
    Music m(true);
    int track = rand() % (musicHits -1);
    if(Music::loadSong( Filesystem::find(string("mugen/music/") + music[track]) )){
		Music::pause();
		Music::play();
		Global::debug(0) << "Now playing track: " << music[track] << endl;
    }
    
    while( !quit ){
        bool draw = false;
        
        if ( Global::speed_counter > 0 ){
            runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
            while (runCounter > 1){
		InputManager::poll();
                keyInputManager::update();
                stage.logic();
                runCounter -= 1;
                draw = true;

                if( keyInputManager::keyState('r', true)){
                    stage.reset();
                }
		if( keyInputManager::keyState('`', true)){
		    stage.toggleConsole();
		}
		// Change music track
		if( keyInputManager::keyState('5', true)){
		    track++;
		    if(track == musicHits)track = 0;
		    if(Music::loadSong( Filesystem::find(string("mugen/music/") + music[track]) )){
			Music::pause();
			Music::play();
			Global::debug(0) << "Now playing track: " << music[track] << endl;
		    }
		}
                if( keyInputManager::keyState(keys::ENTER, false)){
                    stage.Quake( 5 );
                }
		if (keyInputManager::keyState('t', true)){
		    stage.toggleDebug();
		}

                quit |= keyInputManager::keyState(keys::ESC, true );
            }
            Global::speed_counter = 0;
        }

        if (draw){
            stage.render(&work);
	    work.Stretch(back);
            back.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            Util::rest(1);
            keyInputManager::update();
        }
    }
    
    //delete p1;
    //delete p2;

}

void showFont(const string & ourFile){
    Global::debug(0) << "Trying to load font: " << ourFile << "..." << endl;
    MugenFont font( Filesystem::find("mugen/font/" + ourFile ));
    Global::debug(0) << "Loaded font: \"" << ourFile << "\" successfully." << endl;
    
    bool quit = false;
    
    Bitmap work( 320, 240 );
    Bitmap back( 640, 480 );
    
    double gameSpeed = 1.0;
    double runCounter = 0;
    
    int currentBank = 0;
    
    while( !quit ){
        bool draw = false;
        
        if ( Global::speed_counter > 0 ){
            runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
            while (runCounter > 1){
                keyInputManager::update();
                runCounter -= 1;
                draw = true;
		if( keyInputManager::keyState(keys::DOWN, true) ){
                   if (font.getCurrentBank() != 0){
		       currentBank--;
		       font.changeBank(currentBank);
		   }
                }
                if( keyInputManager::keyState(keys::UP, true) ){
                   if (font.getCurrentBank() != font.getTotalBanks()-1){
		       currentBank++;
		       font.changeBank(currentBank);
		   }
                }
                quit |= keyInputManager::keyState(keys::ESC, true );
            }
            Global::speed_counter = 0;
        }

        if (draw){
	    work.clear();
            font.printf( 1, 50, 0, work, "0123456789",0);
	    font.printf( 1, 70, 0, work, "ABCDEFGHIJKLN",0);
	    font.printf( 1, 90, 0, work, "MNOPQRSTUVWXYZ",0);
	    font.printf( 1, 120, 0, work, "CURRENT BANK SET TO %i",0, currentBank);
	    work.Stretch(back);
            back.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            Util::rest(1);
            keyInputManager::update();
        }
    }
}

void showSFF(const string & ourFile){
    std::map< unsigned int, std::map< unsigned int, MugenSprite * > > sprites;
    int currentGroup = 0;
    int currentSprite = 0;
    Global::debug(0) << "Trying to load SFF File: " << ourFile << "..." << endl;
    Mugen::Util::readSprites(ourFile, "", sprites);
    Global::debug(0) << "Loaded SFF file: \"" << ourFile << "\" successfully." << endl;
    
    bool quit = false;
    
    /*Bitmap work( 320, 240 );*/
    Bitmap back( 640, 480 );
    
    double gameSpeed = 1.0;
    double runCounter = 0;
    
    while( !quit ){
        bool draw = false;
        
        if ( Global::speed_counter > 0 ){
            runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
            while (runCounter > 1){
                keyInputManager::update();
                runCounter -= 1;
                draw = true;
		if( keyInputManager::keyState(keys::DOWN, true) ){
                   currentGroup--;
                }
                if( keyInputManager::keyState(keys::UP, true) ){
		   currentGroup++;
                }
		if( keyInputManager::keyState(keys::LEFT, true) ){
                   currentSprite--;
                }
                if( keyInputManager::keyState(keys::RIGHT, true) ){
                   currentSprite++;
                }
		if( keyInputManager::keyState(keys::PGUP, true) ){
                   currentGroup+=500;
                }
                if( keyInputManager::keyState(keys::PGDN, true) ){
		   currentGroup-=500;
                }
                quit |= keyInputManager::keyState(keys::ESC, true );
            }
            Global::speed_counter = 0;
        }

        if (draw){
	    back.clear();
	    MugenSprite *ourSprite = sprites[currentGroup][currentSprite];
	    if (ourSprite){
		Mugen::Effects effects;
		ourSprite->render(320-(ourSprite->getWidth()/2),240-(ourSprite->getHeight()/2),back,effects);
		Font::getDefaultFont().printf( 15, 470, Bitmap::makeColor( 0, 255, 0 ), back, "Current Group: %d   -----   Current Sprite: %d ",0, currentGroup, currentSprite );
	    } else {
		Font::getDefaultFont().printf( 15, 470, Bitmap::makeColor( 0, 255, 0 ), back, "Not valid group or Sprite! Current Group: %d   -----   Current Sprite: %d ",0, currentGroup, currentSprite );
	    }
	    /*work.Stretch(back);*/
            back.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            Util::rest(1);
            keyInputManager::update();
        }
    }
}

static InputMap<Mugen::CharacterKeys> getPlayer1Keys(){
    InputMap<Mugen::CharacterKeys> input;
    input.set(Keyboard::Key_UP, 0, true, Mugen::UP);
    input.set(Keyboard::Key_DOWN, 0, true, Mugen::DOWN);
    input.set(Keyboard::Key_RIGHT, 0, true, Mugen::RIGHT);
    input.set(Keyboard::Key_LEFT, 0, true, Mugen::LEFT);

    input.set(Keyboard::Key_A, 0, true, Mugen::A);
    input.set(Keyboard::Key_S, 0, true, Mugen::B);
    input.set(Keyboard::Key_D, 0, true, Mugen::C);
    input.set(Keyboard::Key_Z, 0, true, Mugen::X);
    input.set(Keyboard::Key_X, 0, true, Mugen::Y);
    input.set(Keyboard::Key_C, 0, true, Mugen::Z);
    input.set(Keyboard::Key_ENTER, 0, true, Mugen::START);
    return input;
}

void doSelectScreen(const std::string &file){
    Mugen::New::CharacterSelect selector(file);
    try {
	selector.load();
	selector.setPlayer1Keys(getPlayer1Keys());
	selector.run("Test", false, false, Bitmap::temporaryBitmap(640,480));
    } catch (const MugenException &me){
	Global::debug(0) << "Error loading select screen. Reason: " << me.getReason() << endl;
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
	std::string ourFile;
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
	
        Global::init( GFX_AUTODETECT_WINDOWED );

        InputManager input;
	
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
                Global::debug(0) << "Could not find a file: " << found.getReason() << endl;
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
                MugenStoryboard story = MugenStoryboard(ourFile);
		story.load();
		// run it and repeat
		Bitmap screen(640, 480);
		story.run( &screen,true);
            } catch (const MugenException & ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    } catch (const Filesystem::NotFound & found){
                Global::debug(0) << "Could not find file: " << found.getReason() << endl;
            } catch (...){
		Global::debug(0) << "Unknown problem loading file" << endl;
		return 1;
	    }
	} else if ( configLoaded == 5 ){
	    try{
                showSFF(ourFile);
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
	} else showOptions();
	
	return 0;
}
END_OF_MAIN()
