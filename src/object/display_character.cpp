#include "globals.h"
#include "character.h"
#include "display_character.h"
#include "draw-normal-effect.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "animation.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

static Token * findNameToken( Token * token ){
	Token * next;
	while ( token->hasTokens() ){
		*token >> next;
		if ( *next == "name" ){
			return next;
		}
	}
	return NULL;
}

DisplayCharacter::DisplayCharacter(const string & path) throw (LoadException):
Character(ALLIANCE_NONE),
path(path),
loaded(false){
    setName(Filesystem::removeExtension(Filesystem::stripDir(path)));
    /* throws load-exception if the file can't be read */
    pthread_mutex_init(&load_lock, NULL );
    TokenReader reader(path);
}

void DisplayCharacter::load() throw (LoadException) {
    Global::debug(1) << "Loading " << path << endl;
    TokenReader reader( path );
    try{
        Token * head = reader.readToken();
        if ( *head != "character" ){
            throw LoadException( "First token is not 'character' in " + path );
        }

        map<string, Filesystem::AbsolutePath> remaps;

        Token * current;
        while ( head->hasTokens() ){
            *head >> current;
            if ( *current == "name" ){
                string n;
                *current >> n;
                setName( n );
            } else if ( *current == "anim" ){

                Token * name = findNameToken( current );
                if ( name == NULL ){
                    /* screw it */
                    continue;
                }
                current->resetToken();
                string xname;
                *name >> xname;
                /* only care about the 'idle' animation */
                if ( xname == "idle" ){
                    name->resetToken();
                    Animation * ani = new Animation( current, this );

                    if ( getMovement( "idle" ) != NULL ){
                        delete getMovement( "idle" );
                    }

                    setMovement( ani, "idle" );
                }
            } else if ( *current == "remap" ){
                string first;
                string second;
                *current >> first >> second;
                remaps[Filesystem::find(Filesystem::RelativePath(second)).path()] = Filesystem::find(Filesystem::RelativePath(first));
            }
        }

        for ( map<string, Filesystem::AbsolutePath>::iterator it = remaps.begin(); it != remaps.end(); it++ ){
            const string & x1 = (*it).first;
            const Filesystem::AbsolutePath & alter = (*it).second;
            reMap(alter.path(), x1, getMapper().size() );
        }
    } catch (const TokenException & ex){
        Global::debug(0) << "Could not read " << path << " : " << ex.getReason() << endl;
        throw LoadException( "Could not open character file: " + path );
    } catch (const Filesystem::NotFound & ex){
        throw LoadException("Could not load character " + path + " because " + ex.getReason());
    }

    if ( getMovement( "idle" ) == NULL ){
        throw LoadException( "No 'idle' animation given for " + path );
    }

    animation_current = getMovement( "idle" );

    effects.push_back(new DrawNormalEffect(this));

    pthread_mutex_lock(&load_lock);
    loaded = true;
    pthread_mutex_unlock(&load_lock);
}
        
bool DisplayCharacter::isLoaded(){
    bool ok = false;
    pthread_mutex_lock(&load_lock);
    ok = loaded;
    pthread_mutex_unlock(&load_lock);
    return ok;
}

DisplayCharacter::~DisplayCharacter(){
}

DisplayCharacterLoader::DisplayCharacterLoader(const vector<DisplayCharacter*> & characters):
forceQuit(false){
    this->characters = characters;
    pthread_mutex_init(&data_lock, NULL );
}

void DisplayCharacterLoader::load(){
    Global::debug(1) << "Starting display character loader" << endl;
    while (!done()){
        DisplayCharacter * character = nextCharacter();
        try{
            character->load();
        } catch (const LoadException & le){
            Global::debug(0) << "Problem loading character: " << le.getReason() << endl;
        }
    }
    Global::debug(1) << "Character display loader done" << endl;
}

bool DisplayCharacterLoader::done(){
    bool result = false;
    pthread_mutex_lock(&data_lock);
    result = forceQuit || (characters.size() == 0);
    pthread_mutex_unlock(&data_lock);
    return result;
}

void DisplayCharacterLoader::stop(){
    pthread_mutex_lock(&data_lock);
    forceQuit = true;
    pthread_mutex_unlock(&data_lock);
}

DisplayCharacter * DisplayCharacterLoader::nextCharacter(){
    DisplayCharacter * result = NULL;
    pthread_mutex_lock(&data_lock);
    result = characters.front();
    characters.erase(characters.begin());
    pthread_mutex_unlock(&data_lock);
    return result;
}

/* put the character in the front of the list if it has yet to be loaded */
void DisplayCharacterLoader::update(DisplayCharacter* character){
    pthread_mutex_lock(&data_lock);
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
    pthread_mutex_unlock(&data_lock);
}

DisplayCharacterLoader::~DisplayCharacterLoader(){
}
