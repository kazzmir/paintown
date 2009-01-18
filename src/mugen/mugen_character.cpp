#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "util/funcs.h"

#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_character.h"
#include "mugen_sff_reader.h"
#include "mugen_snd_reader.h"
#include "mugen_reader.h"

MugenCharacter::MugenCharacter( const string & s ){
    this->location = s;
}

MugenCharacter::MugenCharacter( const char * location ){
    this->location = std::string(location);
}

MugenCharacter::~MugenCharacter(){
    
}

void MugenCharacter::load() throw( MugenException ){
    MugenReader reader( Util::getDataPath() + "mugen/chars/" + location + "/" + location + ".def" );
    std::vector< MugenSection * > collection;
    try{
	collection = reader.getCollection();
    }
    catch( MugenException &ex){
	throw ex;
    }
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	const std::string &head = collection[i]->getHeader();
	if( head == "Info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		const std::string &itemhead = item->query();
		if( itemhead == "name" ){
		    *content->getNext() >> name;
		}
		else if (itemhead == "displayname" ){
		    *content->getNext() >> displayName;
		}
		else if (itemhead == "versiondate" ){
		    *content->getNext() >> versionDate;
		}
		else if (itemhead == "mugenversion" ){
		    *content->getNext() >> mugenVersion;
		}
		else if (itemhead == "author" ){
		    *content->getNext() >> author;
		}
		else if (itemhead == "pal.defaults" ){
		}
		else throw MugenException( "Unhandled option in Info Section" );
	    }
	}
    }
}
