#ifndef _paintown_select_player_h
#define _paintown_select_player_h

#include "object/object.h"
#include "util/load_exception.h"
#include "return_exception.h"
#include <string>

Object * selectPlayer( bool invincibile, const std::string & message = "" ) throw( LoadException, ReturnException );

vector<Object *> versusSelect( bool invincible ) throw( LoadException, ReturnException );

#endif
