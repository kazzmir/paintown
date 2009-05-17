#include <iostream>

#include "blockobject.h"
#include "factory/object_factory.h"
#include "util/load_exception.h"
#include "util/token_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "globals.h"

using namespace std;

static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}

BlockObject::BlockObject():
type(ObjectFactory::NoneType),
aggression( -1 ),
map( 0 ),
health( 1 ),
coords_x( 0 ),
coords_z( 0 ),
id(-1),
stimulationType( "none" ),
stimulationValue( 0 ){
}

BlockObject::BlockObject( Token * tok ) throw ( LoadException ):
type(ObjectFactory::NoneType),
aggression( -1 ),
map( 0 ),
health( 1 ),
coords_x( 0 ),
coords_z( 0 ),
id(-1),
stimulationType( "none" ),
stimulationValue( 0 ){
	while ( tok->hasTokens() ){
		try{
			Token * current;
			*tok >> current;

			if ( *current == "type" ){
				string k;
				*current >> k;
				if ( k == "item" ){
					type = ObjectFactory::ItemType;
				} else if ( k == "enemy" ){
					type = ObjectFactory::EnemyType;
				} else if ( k == "actor" ){
					type = ObjectFactory::ActorType;
				} else if ( k == "cat" ){
					type = ObjectFactory::CatType;
				} else {
					tok->print(" ");
					throw LoadException("Not a valid type");
				}
			} else if ( *current == "stimulation" ){
				string type;
				int value;
				Token * next;
				*current >> next;
				type = next->getName();
				*next >> value;
				setStimulationValue( value );
				setStimulationType( type );
                        } else if (*current == "id"){
                            int id;
                            *current >> id;
                            setId(id);
                            ObjectFactory::maxId(id);
			} else if ( *current == "path" ){
				string n;
				*current >> n;
				setPath( dataPath( n ) );
			} else if ( *current == "aggression" ){
				int a;
				*current >> a;
				setAggression( a );
			} else if ( *current == "name" ){
				string n;
				*current >> n;
				if ( n.length() > 0 && (n[0] >= 'a' && n[0] <= 'z') ){
					n[0] = n[0] - 'a' + 'A';
				}
				setName( n );
			} else if ( *current == "alias" ){
				string n;
				*current >> n;
				if ( n.length() > 0 && (n[0] >= 'a' && n[0] <= 'z') )
					n[0] = n[0] - 'a' + 'A';
				setAlias( n );
			} else if ( *current == "coords" ){
				int x, z;
				*current >> x >> z;
				setCoords( x, z );
			} else if ( *current == "health" ){
				int h;
				*current >> h;
				setHealth( h );
			} else if ( *current == "at" ){
				/* not quite sure what 'at' is */
			} else if ( *current == "map" ){
				int m;
				*current >> m;
				setMap( m );
			} else {
				cout<<"Unhandled blockobject token"<<endl;
				current->print(" ");
			}

		} catch ( const TokenException & te ){
			throw LoadException("Blockobject parse exception: " + te.getReason() );
		}
	}

	/* give it some stupid name */
	if ( getName() == "" ){
		name = "a";
		name += getType();
	}

	if ( getAlias() == "" ){
		setAlias( getName() );
        }

	if ( getPath() == "" ){
            Global::debug(0) << endl;
            Global::debug(0) << "**WARNING**"<<endl;
            tok->print(" ");
            string str("No path given for ");
            str += getName();
            throw LoadException( str );
	}
}

BlockObject::~BlockObject(){
}
