#include "globals.h"
#include "character.h"
#include "display_character.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "util/funcs.h"
#include "animation.h"
#include <string>
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

static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}

DisplayCharacter::DisplayCharacter( const string & path ) throw( LoadException ):
Character( ALLIANCE_NONE ){
	TokenReader reader( path );
	try{
		Token * head = reader.readToken();
		if ( *head != "character" ){
			throw LoadException( "First token is not 'character' in " + path );
		}
	
		map< string, string > remaps;
		
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
				remaps[ dataPath( second ) ] = dataPath( first );
			}
		}

		for ( map<string,string>::iterator it = remaps.begin(); it != remaps.end(); it++ ){
			const string & x1 = (*it).first;
			const string & alter = (*it).second;
			reMap( alter, x1, getMapper().size() );
		}
	} catch( const TokenException & ex ){
		cerr<< "Could not read " << path << " : " << ex.getReason() << endl;
		throw LoadException( "Could not open character file: " + path );
	}

	if ( getMovement( "idle" ) == NULL ){
		throw LoadException( "No 'idle' animation given for " + path );
	}
	animation_current = getMovement( "idle" );
}

DisplayCharacter::~DisplayCharacter(){
}
