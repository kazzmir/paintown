#include <iostream>

#include "blockobject.h"
#include "../object/stimulation.h"
#include "../factory/object_factory.h"
#include "util/exceptions/load_exception.h"
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
id(-1){
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
triggers(copy.triggers),
stimulation(copy.stimulation){
}

static Util::ReferenceCount<Paintown::Trigger> getTrigger(const Token & token){
    const Token * spawn = token.findToken("_/spawn");
    if (spawn != NULL){
        class SpawnTrigger: public Paintown::Trigger {
        public:
            SpawnTrigger(const Token * block):
            spawn(block){
            }

            SpawnTrigger(const SpawnTrigger & copy):
            spawn(copy.spawn){
            }
    
            virtual Trigger * copy() const {
                return new SpawnTrigger(*this);
            }

            virtual ~SpawnTrigger(){
            }

            BlockObject spawn; 
        };

        const Token * object = spawn->findToken("_/object");
        if (object != NULL){
            return Util::ReferenceCount<Paintown::Trigger>(new SpawnTrigger(object));
        }
    }

    return Util::ReferenceCount<Paintown::Trigger>(NULL);
}

BlockObject::BlockObject(const Token * tok):
type(ObjectFactory::NoneType),
aggression( -1 ),
map( 0 ),
health( 1 ),
coords_x( 0 ),
coords_z( 0 ),
id(-1){
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
                } else if (k == "breakable-item"){
                    type = ObjectFactory::BreakableItemType;
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
                setStimulation(Util::ReferenceCount<Paintown::Stimulation>(Paintown::Stimulation::create(*current)));
                /*
                string type;
                double value;
                const Token * next;
                current->view() >> next;
                type = next->getName();
                next->view() >> value;
                setStimulationValue(value);
                setStimulationType(type);
                */
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
            } else if (*current == "on-death"){
                Paintown::TriggerType type = Paintown::OnDeath;
                Util::ReferenceCount<Paintown::Trigger> trigger = getTrigger(*current);
                triggers[type] = trigger;
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

Util::ReferenceCount<Paintown::Stimulation> BlockObject::getStimulation() const {
    return stimulation;
}

void BlockObject::setStimulation(const Util::ReferenceCount<Paintown::Stimulation> & stimulation){
    this->stimulation = stimulation;
}
    
std::map<Paintown::TriggerType, Util::ReferenceCount<Paintown::Trigger> > BlockObject::getTriggers(){
    return triggers;
}

BlockObject::~BlockObject(){
}
