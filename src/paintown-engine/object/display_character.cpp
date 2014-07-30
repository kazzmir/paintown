#include "character.h"
#include "display_character.h"
#include "draw-effect.h"
#include "util/thread.h"
#include "util/exceptions/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "animation.h"
#include "alliance.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

namespace Paintown{

DisplayCharacter::DisplayCharacter(const Filesystem::AbsolutePath & path):
Character(ALLIANCE_NONE),
path(path),
loaded(false){
    setName(Path::removeExtension(path.getFilename().path()));
    /* throws load-exception if the file can't be read */
    // Util::Thread::initializeLock(&load_lock);
    // TokenReader reader(path);
}

void DisplayCharacter::load(){
    Global::debug(1, PAINTOWN_DEBUG_CONTEXT) << "Loading " << path.path() << endl;
    TokenReader reader;
    try{
        Token * head = reader.readTokenFromFile(*Storage::instance().open(path));
        if ( *head != "character" ){
            throw LoadException(__FILE__, __LINE__, "First token is not 'character' in " + path.path());
        }

        // map<string, Filesystem::AbsolutePath> remaps;

        TokenView view = head->view();
        const Token * current;
        while (view.hasMore()){
            view >> current;
            if ( *current == "name" ){
                string n;
                current->view() >> n;
                setName(n);
            } else if (*current == "scale"){
                double scale;
                current->view() >> scale;
                if (scale < 0.01){
                    scale = 0.01;
                }
                setSpriteScale(scale);
            } else if (*current == "anim"){

                const Token * name = current->findToken("_/name");
                if (name == NULL){
                    /* screw it */
                    continue;
                }
                // current->resetToken();
                string xname;
                name->view() >> xname;
                /* only care about the 'idle' animation */
                if ( xname == "idle" ){
                    // name->resetToken();
                    Animation * ani = new Animation(current, this);
                    setMovement(ani, "idle");
                }
            } else if (*current == "remap"){
                string first;
                string second;
                current->view() >> first >> second;
                if (newRemap(first, second)){
                    addRemap(new Remap(Filesystem::RelativePath(first), Filesystem::RelativePath(second)));
                }
                // remaps[Filesystem::find(Filesystem::RelativePath(second)).path()] = Filesystem::find(Filesystem::RelativePath(first));
            }
        }

        /*
        for ( map<string, Filesystem::AbsolutePath>::iterator it = remaps.begin(); it != remaps.end(); it++ ){
            const string & x1 = (*it).first;
            const Filesystem::AbsolutePath & alter = (*it).second;
            reMap(alter.path(), x1, getMapper().size() );
        }
        */
    } catch (const TokenException & ex){
        // Global::debug(0) << "Could not read " << path << " : " << ex.getReason() << endl;
        throw LoadException(__FILE__, __LINE__, ex, "Could not open character file: " + path.path());
    } catch (const Filesystem::NotFound & ex){
        throw LoadException(__FILE__, __LINE__, ex, "Could not load character " + path.path());
    }

    if ( getMovement( "idle" ) == NULL ){
        throw LoadException(__FILE__, __LINE__, "No 'idle' animation given for " + path.path());
    }

    animation_current = getMovement("idle");
    animation_current->Act();

    effects.push_back(new DrawNormalEffect(this));
}

void DisplayCharacter::loadDone(){
    Util::Thread::ScopedLock locked(load_lock);
    loaded = true;
}

bool DisplayCharacter::isLoaded(){
    Util::Thread::ScopedLock locked(load_lock);
    bool ok = false;
    ok = loaded;
    return ok;
}

DisplayCharacter::~DisplayCharacter(){
}

DisplayCharacterLoader::DisplayCharacterLoader(const vector<DisplayCharacter*> & characters):
forceQuit(false){
    this->characters = characters;
}

void DisplayCharacterLoader::load(){
    Global::debug(1, PAINTOWN_DEBUG_CONTEXT) << "Starting display character loader" << endl;
    while (!done()){
        DisplayCharacter * character = nextCharacter();
        try{
            character->load();
            character->loadDone();
        } catch (const LoadException & le){
            Global::debug(0, PAINTOWN_DEBUG_CONTEXT) << "Problem loading character: " << le.getTrace() << endl;
        } catch (const Graphics::BitmapException & fail){
            Global::debug(0, PAINTOWN_DEBUG_CONTEXT) << "Problem loading character: " << fail.getTrace() << endl;
        }
        /* yield the timeslice for slow systems */
        Util::rest(0);
    }
    Global::debug(1, PAINTOWN_DEBUG_CONTEXT) << "Character display loader done" << endl;
}

bool DisplayCharacterLoader::done(){
    Util::Thread::ScopedLock locked(data_lock);
    bool result = false;
    result = forceQuit || (characters.size() == 0);
    return result;
}

void DisplayCharacterLoader::stop(){
    Util::Thread::ScopedLock locked(data_lock);
    forceQuit = true;
}

DisplayCharacter * DisplayCharacterLoader::nextCharacter(){
    Util::Thread::ScopedLock locked(data_lock);
    DisplayCharacter * result = NULL;
    result = characters.front();
    characters.erase(characters.begin());
    return result;
}

/* put the character in the front of the list if it has yet to be loaded */
void DisplayCharacterLoader::update(DisplayCharacter* character){
    Util::Thread::ScopedLock locked(data_lock);
    bool found = false;
    for (vector<DisplayCharacter*>::iterator it = characters.begin(); it != characters.end(); ){
        DisplayCharacter * who = *it;
        if (who == character){
            found = true;
            it = characters.erase(it);
            break;
        } else {
            it++;
        }
    }
    if (found){
        characters.insert(characters.begin(), character);
    }
}

DisplayCharacterLoader::~DisplayCharacterLoader(){
}

}
