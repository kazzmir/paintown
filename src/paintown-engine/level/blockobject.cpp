#include <iostream>

#include "blockobject.h"
#include "../factory/object_factory.h"
#include "util/load_exception.h"
#include "util/token_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/debug.h"

using namespace std;

BlockObject::BlockObject():
type(ObjectFactory::NoneType),
aggression( -1 ),
map( 0 ),
health( 1 ),
coords_x( 0 ),
coords_z( 0 ),
id(-1),
stimulationType("none"),
stimulationValue(0){
}

BlockObject::BlockObject(const BlockObject & copy):
type(copy.type),
aggression(copy.aggression),
name(copy.name),
alias(copy.alias),
path(copy.path),
map(copy.map),
health(copy.health),
coords_x(copy.coords_x),
coords_z(copy.coords_z),
id(copy.id),
stimulationType(copy.stimulationType),
stimulationValue(copy.stimulationValue){
}

BlockObject::BlockObject(const Token * tok):
type(ObjectFactory::NoneType),
aggression( -1 ),
map( 0 ),
health( 1 ),
coords_x( 0 ),
coords_z( 0 ),
id(-1),
stimulationType( "none" ),
stimulationValue( 0 ){
    TokenView view = tok->view();
    while (view.hasMore()){
        try{
            const Token * current;
            view >> current;

            if ( *current == "type" ){
                string k;
                current->view() >> k;
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
                    throw LoadException(__FILE__, __LINE__, "Not a valid type");
                }
            } else if ( *current == "stimulation" ){
                string type;
                double value;
                const Token * next;
                current->view() >> next;
                type = next->getName();
                next->view() >> value;
                setStimulationValue(value);
                setStimulationType(type);
            } else if (*current == "id"){
                int id;
                current->view() >> id;
                setId(id);
                ObjectFactory::maxId(id);
            } else if ( *current == "path" ){
                string n;
                current->view() >> n;
                setPath(Storage::instance().find(Filesystem::RelativePath(n)));
            } else if ( *current == "aggression" ){
                int a;
                current->view() >> a;
                setAggression(a);
            } else if ( *current == "name" ){
                string n;
                current->view() >> n;
                if ( n.length() > 0 && (n[0] >= 'a' && n[0] <= 'z') ){
                    n[0] = (char)(n[0] - 'a' + 'A');
                }
                setName( n );
            } else if ( *current == "alias" ){
                string n;
                current->view() >> n;
                if ( n.length() > 0 && (n[0] >= 'a' && n[0] <= 'z') )
                    n[0] = (char)(n[0] - 'a' + 'A');
                setAlias( n );
            } else if ( *current == "coords" ){
                int x, z;
                current->view() >> x >> z;
                setCoords( x, z );
            } else if ( *current == "health" ){
                int h;
                current->view() >> h;
                setHealth( h );
            } else if ( *current == "at" ){
                /* not quite sure what 'at' is */
            } else if ( *current == "map" ){
                int m;
                current->view() >> m;
                setMap( m );
            } else {
                cout<<"Unhandled blockobject token"<<endl;
                current->print(" ");
            }

        } catch ( const TokenException & te ){
            throw LoadException(__FILE__, __LINE__, te, "Blockobject parse exception: ");
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

    if (getPath().isEmpty()){
        Global::debug(0) << endl;
        Global::debug(0) << "**WARNING**"<<endl;
        tok->print(" ");
        string str("No path given for ");
        str += getName();
        throw LoadException(__FILE__, __LINE__, str);
    }
}

BlockObject::~BlockObject(){
}
