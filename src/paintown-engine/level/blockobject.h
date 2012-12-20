#ifndef _block_object_h
#define _block_object_h

#include "util/file-system.h"
#include "../object/trigger.h"
#include "../factory/object_factory.h"
#include <string>

class Token;

namespace Paintown{
    class Stimulation;
}

class BlockObject{
public:
    BlockObject();
    BlockObject(const BlockObject & copy);
    BlockObject(const Token * tok);

public: 
    /* getters */
    inline const std::string & getAlias() const {
        return alias;
    }

    inline ObjectFactory::ObjectType getType() const {
        return type;
    }

    inline const std::string & getName() const {
        return name;
    }

    inline const Filesystem::AbsolutePath & getPath() const {
        return path;
    }

    Util::ReferenceCount<Paintown::Stimulation> getStimulation() const;

    inline int getMap() const {
        return map;
    }

    inline int getAggression() const {
        return aggression;
    }

    inline int getHealth() const {
        return health;
    }

    inline void getCoords( int & x, int & z ) const {
        x = coords_x;
        z = coords_z;
    }

public:
    /* setters */
    inline void setType(const ObjectFactory::ObjectType l) {
        type = l;
    }

    inline void setPath(const Filesystem::AbsolutePath & p){
        path = p;
    }

    inline void setAggression( const int a ){
        aggression = a;
    }

    inline void setName( const std::string & s ) {
        name = s;
    }

    inline void setAlias(const std::string & s){
        alias = s;
    }

    inline void setMap(int m){
        map = m;
    }

    inline void setHealth(int h){
        health = h;
    }

    inline void setCoords( int x, int z ) {
        coords_x = x;
        coords_z = z;
    }

    virtual void setStimulation(const Util::ReferenceCount<Paintown::Stimulation> & stimulation);

    virtual std::map<Paintown::TriggerType, Util::ReferenceCount<Paintown::Trigger> > getTriggers() const;

    inline void setId(const int id){
        this->id = id;
    }

    inline int getId() const{
        return id;
    }

    virtual ~BlockObject();

protected:

    /* the type of object this is */
    ObjectFactory::ObjectType type;

    int aggression;

    std::string name;
    std::string alias;
    Filesystem::AbsolutePath path;

    /* the sprite to map to */
    int map;

    int health;

    int coords_x, coords_z;
    int id;

    std::map<Paintown::TriggerType, Util::ReferenceCount<Paintown::Trigger> > triggers;

    Util::ReferenceCount<Paintown::Stimulation> stimulation;
};

#endif
