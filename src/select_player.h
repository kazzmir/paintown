#ifndef _paintown_select_player_h
#define _paintown_select_player_h

#include "object/object.h"
#include "util/load_exception.h"
#include "return_exception.h"

Object * selectPlayer( bool invincibile ) throw( LoadException, ReturnException );

#endif
