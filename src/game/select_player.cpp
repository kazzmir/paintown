#include "util/bitmap.h"
#include "util/lit_bitmap.h"
#include "object/object.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "object/player.h"
#include "globals.h"
#include "object/display_character.h"
#include "exceptions/shutdown_exception.h"
#include "util/thread.h"
#include "init.h"
// #include "select_player.h"
#include "game.h"
#include "mod.h"
#include "input/keyboard.h"
#include "input/input-manager.h"
#include "input/input-map.h"
#include "util/font.h"
#include "level/utils.h"
#include "util/file-system.h"
#include "world.h"
#include "exceptions/exception.h"
#include "loading.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

#include "configuration.h"

using namespace std;

/* some compilers probably won't like __FILE__. #define it in that case */
static const char * DEBUG_CONTEXT = __FILE__;

struct playerInfo{
    DisplayCharacter * guy;
    Filesystem::AbsolutePath path;
    playerInfo(DisplayCharacter * guy, const Filesystem::AbsolutePath & path ):
    guy(guy),
    path(path){
    }
};

typedef vector<playerInfo> PlayerVector;
static PlayerVector loadPlayers( const string & path ){
    PlayerVector players;
    vector<Filesystem::AbsolutePath> files = Filesystem::getFiles(Filesystem::find(Filesystem::RelativePath(path + "/")), "*" );
    std::sort(files.begin(), files.end());
    for ( vector<Filesystem::AbsolutePath>::iterator it = files.begin(); it != files.end(); it++ ){
        string path = (*it).path();
        Global::debug(2, DEBUG_CONTEXT) << "Found file " << path << endl;
        string file = path + "/" + path.substr(path.find_last_of('/') + 1) + ".txt";
        Global::debug(1, DEBUG_CONTEXT) << "Checking " << file << endl;
        if (Util::exists(file)){
            Global::debug(1, DEBUG_CONTEXT) << "Loading " << file << endl;
            try{
                players.push_back(playerInfo(new DisplayCharacter(file), Filesystem::AbsolutePath(file)));
            } catch (const LoadException & le){
                Global::debug(0, DEBUG_CONTEXT) << "Could not load " << file << " because " << le.getTrace() << endl;
            }
        }
    }
    return players;
}

/*
template< class Key, class Value >
Key getNth( const map< Key, Value > & m, int i ){
	int count = 0;
	for ( typename map<Key, Value>::const_iterator it = m.begin(); it != m.end(); it++ ){
		if ( count == i ){
			return it->first;
		}
		count += 1;
	}
	return m.begin()->first;
}
*/

static vector<DisplayCharacter*> getCharacters(PlayerVector players){
    vector<DisplayCharacter*> characters;
    for (PlayerVector::iterator it = players.begin(); it != players.end(); it++){
        characters.push_back(it->guy);
    }

    return characters;
}

/* run the loader in a separate thread */
static void * characterLoader(void * arg){
    DisplayCharacterLoader * loader = (DisplayCharacterLoader*) arg;
    loader->load();
    return NULL;
}

namespace Select{
    enum Input{
        Up, Down, Left,
        Right, Remap, Quit,
        Choose,
    };
}

static int choosePlayer(const PlayerVector & players, const string & message){
    DisplayCharacterLoader loader(getCharacters(players));
    InputMap<Select::Input> input;

    // Bitmap work( GFX_X / 2, GFX_Y / 2 );
    Bitmap work( GFX_X, GFX_Y );

    /* TODO: the background should be configurable */
    Bitmap background(Global::titleScreen().path());

    /* currently selected character */
    int current = 0;

    input.set(Configuration::config( 0 ).getRight(), 300, false, Select::Right);
    input.set(Configuration::config( 0 ).getUp(), 300, false, Select::Up);
    input.set(Configuration::config( 0 ).getDown(), 300, false, Select::Down);
    input.set(Configuration::config( 0 ).getLeft(), 300, false, Select::Left);
    input.set(Keyboard::Key_ESC, 0, true, Select::Quit);
    input.set(Joystick::Up, 300, false, Select::Up);
    input.set(Joystick::Down, 300, false, Select::Down);
    input.set(Joystick::Left, 300, false, Select::Left);
    input.set(Joystick::Right, 300, false, Select::Right);
    input.set(Joystick::Button1, 0, false, Select::Choose);
    input.set(Joystick::Button2, 0, true, Select::Quit);
    input.set(Joystick::Button3, 300, false, Select::Remap);

    input.set(Keyboard::Key_TAB, 200, false, Select::Remap);
    input.set(Keyboard::Key_ENTER, 0, false, Select::Choose);
    input.set(Keyboard::Key_SPACE, 0, false, Select::Choose);
    input.set(Configuration::config(0).getAttack1(), 0, false, Select::Choose);

    /* preview box for each character */
    Bitmap temp(120, 120);
    Bitmap preview(GFX_X / 2, GFX_Y / 2);
    Bitmap reflection(GFX_X / 2, GFX_Y / 2);
                    
    // const int unselectedColor = Bitmap::makeColor( 255, 0, 0 );
    // const int selectedColor = Bitmap::makeColor( 0, 255, 0 );

    const int maxColor = 40;
    int selectedGradient[ maxColor ];
    int unselectedGradient[3];

    /*
    Util::blend_palette( unselectedGradient, maxColor / 2, Bitmap::makeColor( 255, 0, 0 ), Bitmap::makeColor( 255, 0, 0 ) );
    Util::blend_palette( unselectedGradient + maxColor / 2, maxColor / 2, Bitmap::makeColor( 255, 0, 0 ), Bitmap::makeColor( 255, 0, 0 ) );
    */
    Util::blend_palette(unselectedGradient, 3, Bitmap::makeColor(0, 0, 0), Bitmap::makeColor(255, 0, 0));

    Util::blend_palette( selectedGradient, maxColor / 2, Bitmap::makeColor( 0, 128, 0 ), Bitmap::makeColor( 0, 255, 0 ) );
    Util::blend_palette( selectedGradient + maxColor / 2, maxColor / 2, Bitmap::makeColor( 0, 255, 0 ), Bitmap::makeColor( 0, 128, 0 ) );

    Global::speed_counter = 0;

    const int boxSize = 80;
    const int startX = 300;
    const int startY = 20;
    const int boxesPerLine = (work.getWidth() - startX) / (boxSize + 10);
    const int boxesPerColumn = (work.getHeight() - startY) / (boxSize + 10);
    int backgroundX = 0;
    int top = 0;

    const int maxGradient = 50;
    int gradient[ maxGradient ];	
    /* fade from yellow to some reddish color */
    Util::blend_palette( gradient, 25, Bitmap::makeColor( 255, 255, 0 ), Bitmap::makeColor( 0xff, 0x33, 0x11 ) );
    Util::blend_palette( gradient + 25, 25, Bitmap::makeColor( 0xff, 0x33, 0x11 ), Bitmap::makeColor( 255, 255, 0 ) );

    bool draw = true;
    unsigned int clock = 0;
    double runCounter = 0;
    double gameSpeed = 1;
    Sound beep(Filesystem::find(Filesystem::RelativePath("sounds/beep1.wav")).path());
    Util::Thread::Id loadingThread;

    if (!Util::Thread::createThread(&loadingThread, NULL, (Util::Thread::ThreadFunction) characterLoader, &loader )){
        throw LoadException(__FILE__, __LINE__, "Could not create loading thread");
    }

    bool done = false;
                        
    /* wait for the player to let go of the choose button */
    InputManager::waitForRelease(input, Select::Choose);

    try{
        while (! done){
            /* bad variable name */
            DisplayCharacter * ch = players[ current ].guy;

            if ( Global::speed_counter > 0 ){
                // double think = Global::speed_counter;
                runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
                while ( runCounter >= 1.0 ){
                    int old = current;
                    runCounter -= 1;
                    clock += 1;
                    InputManager::poll();
                    
                    InputMap<Select::Input>::Output inputState = InputManager::getMap(input);
                    if (Global::shutdown()){
                        loader.stop();
                        Util::Thread::joinThread(loadingThread);
                        throw ShutdownException();
                    }

                    if ( clock % 5 == 0 ){
                        backgroundX -= 1;
                        if ( backgroundX < - work.getWidth() ){
                            backgroundX = 0;
                        }
                    }

                    if (inputState[Select::Left]){
                        current = current - 1;
                        beep.play();
                    }

                    if (inputState[Select::Right]){
                        current = current + 1;
                        beep.play();
                    }

                    if (inputState[Select::Up]){
                        current = current - boxesPerLine;
                        beep.play();
                    }

                    if (inputState[Select::Down]){
                        current = current + boxesPerLine;
                        beep.play();
                    }

                    if (ch->isLoaded()){
                        if (inputState[Select::Remap]){
                            ch->nextMap();
                        }
                    }

                    if (inputState[Select::Quit]){
                        loader.stop();
                        Util::Thread::joinThread(loadingThread);
                        InputManager::waitForRelease(input, Select::Quit);
                        throw Exception::Return(__FILE__, __LINE__);
                    }

                    if ( current < 0 ){
                        current = 0;
                    }

                    if ( current >= (signed) players.size() ){
                        current = players.size() - 1;
                    }

                    if (ch->isLoaded()){
                        if ( ch->testAnimation() ){
                            ch->testReset();
                        }
                    }

                    while ( current < top ){
                        top -= boxesPerLine;
                    }

                    while ( current >= top + boxesPerLine * boxesPerColumn ){
                        top += boxesPerLine;
                    }

                    if (current != old){
                        loader.update(players[current].guy);
                    }

                    done |= inputState[Select::Choose] && ch->isLoaded();

                    // think--;
                }

                Global::speed_counter = 0;
                draw = true;
            }

            if ( draw ){

                // background.Stretch( work );
                background.Blit( backgroundX, 0, work );
                background.Blit( work.getWidth() + backgroundX, 0, work );
                const Font & font = Font::getFont(Global::DEFAULT_FONT);

                if (ch->isLoaded()){
                    const int stand = 100;
                    ch->setFacing( Object::FACING_RIGHT );
                    Character copy(*ch);
                    copy.setDrawShadow( false );
                    copy.setX( preview.getWidth() / 2 );
                    copy.setY( 0 );
                    // copy.setZ( preview.getHeight() - stand );
                    copy.setZ(preview.getHeight() - stand);
                    preview.fill(Bitmap::MaskColor());
                    reflection.fill(Bitmap::MaskColor());
                    // preview.fill( 0 );
                    // reflection.fill( 0 );

                    copy.draw( &preview, 0, 0 );
                    preview.drawVFlip( 0, 0, reflection );

                    Bitmap::transBlender( 0, 0, 0, 255 );

                    LitBitmap s2(reflection);
                    s2.draw( 0, preview.getHeight() - stand - stand, preview );

                    Bitmap::transBlender( 0, 0, 0, 128 );
                    /* TODO: implement a fading reflection. compute the alpha for each y line.
                     * start out with alpha of 128 then slowly move towards 64 at the middle
                     * of the sprite and then fade to 0 soon after, like after another 10
                     * pixels.
                     * alpha(y) = y < height/2 : 128 * y/height/2 * 100 ? alpha(64) * log(y)
                     */
                    reflection.drawTrans( 0, preview.getHeight() - stand - stand, preview );
                    copy.draw( &preview, 0, 0 );

                    // reflection.drawCharacter( 0, preview.getHeight() - stand - stand, 0, -1, preview );
                    // preview.floodfill( 0, 0, Bitmap::MaskColor );
                    // preview.drawTransVFlip( 0, preview.getHeight() - stand - stand, preview );

                    // preview.draw( 60, 0, work );
                    
                    preview.drawStretched(-GFX_X / 2 + startX / 2, -70 + stand, GFX_X, GFX_Y, work);
                    // preview.drawStretched(-320, 0, GFX_X, GFX_Y, work);
                    
                    // preview.draw( 60, 0, work );

                    for (int c = 1; c >= 0; c--){
                        int color = 255 - c * 190;
                        int x = 10 + 5 * c;
                        int y = font.getHeight() + 5 + c * 5;
                        font.printf( x, y, Bitmap::makeColor(color, color, color ), work, copy.getName(), 0 );
                    }
                }

                font.printf( 10, 10, Bitmap::makeColor( 255, 255, 255 ), work, message, 0 );

                if (!loader.done()){

                    const Font & font = Font::getFont(Global::DEFAULT_FONT, 10, 10 );
                    font.printf(1, 1, Bitmap::makeColor(200,0,0), work, "Loading...", 0);
                }

                int x = startX, y = startY;
                unsigned int i;
                for ( i = top; i < players.size() && y + boxSize < GFX_Y; i++ ){
                    Bitmap box(work, x, y, boxSize, boxSize);
                    DisplayCharacter * displayed = players[i].guy;
                    box.clear();
                    // int color = unselectedColor;
                    // int * color = i == (unsigned int) current ? selectedGradient : unselectedGradient;
                    if (displayed->isLoaded()){
                        temp.clear();
                        Character smaller(*displayed);

                        /* draw a border */
                        // box.border( 0, 3, color[ clock % maxColor ] );

                        smaller.setX( temp.getWidth() / 2 );
                        smaller.setY( 0 );
                        smaller.setZ( temp.getHeight() );
                        smaller.draw( &temp, 0, 0 );
                        temp.drawStretched(0, 0, box.getWidth(), box.getHeight(), box);
                    } else {
                        /* FIXME: center the text */

                        const Font & font = Font::getFont(Global::DEFAULT_FONT, 15, 15);
                        font.printf(box.getWidth() / 2 - font.textLength(displayed->getName().c_str()) / 2, box.getHeight() / 2 - font.getHeight() / 2, Bitmap::makeColor(255, 255, 255), box, displayed->getName(), 0);
                    }

                    if (i == (unsigned int) current){
                        box.border(0, 3, selectedGradient[clock % maxColor]);
                    } else {
                        for (int border = 0; border < 3; border++){
                            box.border(border, border + 1, unselectedGradient[border]);
                        }

                        /*
                        if (i % 2 == 0){
                            for (int border = 0; border < 3; border++){
                                box.border(border, border + 1, unselectedGradient[border]);
                            }
                            // box.border(0, 3, unselectedGradient[0]);
                        } else {
                            box.border(0, 3, unselectedGradient[2]);
                        }
                        */
                    }
                    x += boxSize + 10;
                    if ( x + boxSize + 10 > work.getWidth() ){
                        x = startX;
                        y += (boxSize + 10);
                    }
                }

                if ( top > 0 ){
                    int x1 = 80;
                    int x2 = 140;
                    work.triangle( startX + x1, 8, startX + x2, 8, startX + (x1 + x2) / 2, 3, gradient[ clock % maxGradient ] );
                }

                if ( i < players.size() ){
                    int x1 = 80;
                    int x2 = 140;
                    work.triangle( startX + x1, GFX_Y - 8, startX + x2, GFX_Y - 8, startX + (x1 + x2) / 2, GFX_Y - 3, gradient[ clock % maxGradient ] );
                }

                work.BlitToScreen();
                draw = false;
            }

            while ( Global::speed_counter == 0 ){
                InputManager::poll();
                Util::rest(1);
            }
        }
    } catch (const Filesystem::NotFound & ex){
        Global::debug(0, DEBUG_CONTEXT) << "Error during select player screen: " << ex.getTrace() << endl;
    }
    loader.stop();
    Util::Thread::joinThread(loadingThread);

    return current;
}

static Filesystem::AbsolutePath doSelectPlayer(const PlayerVector & players, const string & message, const Level::LevelInfo & info, int & remap){
    try{
        int current = 0;
        if (players.size() == 0){
            ostringstream out;
            out << "No players found in '" << info.getPlayerPath() << "'";
            throw LoadException(__FILE__, __LINE__,out.str());
        }
        /* only run the selection screen if there is more than 1 player to choose */
        if (players.size() > 1){
            current = choosePlayer(players, message);
        }

        /* set the map */
        remap = players[current].guy->getCurrentMap();

        /* delete all the preview characters. its ok to delete them
         * before looking up the selected player in the map
         * because 'delete' doesn't affect the map, it just changes
         * memory around.
         */
        Filesystem::AbsolutePath path = players[current].path;
        for (PlayerVector::const_iterator it = players.begin(); it != players.end(); it++){
            delete it->guy;
        }

        Global::debug(1, DEBUG_CONTEXT) << "Selected " << path.path() << ". Loading.." << endl;
        return path;
    } catch (const Exception::Return & r){
        for (PlayerVector::const_iterator it = players.begin(); it != players.end(); it++){
            delete it->guy;
        }
        throw r;
    }
}

namespace Paintown{

Filesystem::AbsolutePath Mod::selectPlayer(const string & message, const Level::LevelInfo & info, int & remap){
    /* hm, it would be nice to cache this I suppose */
    Util::Thread::Id wait;
    Loader::startLoading(&wait, NULL, Loader::SimpleCircle);
    PlayerVector players = loadPlayers(info.getPlayerPath());
    Loader::stopLoading(wait);
    return doSelectPlayer(players, message, info, remap);
}

}
