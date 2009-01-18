#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
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

static int lowerCase( int c ){ return tolower( c );}

MugenCharacter::MugenCharacter( const string & s ){
    this->location = s;
}

MugenCharacter::MugenCharacter( const char * location ){
    this->location = std::string(location);
}

MugenCharacter::~MugenCharacter(){
    
}

void MugenCharacter::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string loc = Util::getDataPath() + "mugen/chars/" + location + "/";
    cout << loc << endl;
    std::vector< string > files = Util::getFiles( loc, "*" );
    std::string ourDefFile = "";
    for( unsigned int i = 0; i < files.size(); ++i ){
	std::string temp = files[i].c_str();
	transform( temp.begin(), temp.end(), temp.begin(), lowerCase );
	if( std::string( location + ".def" ) == temp ){
	    // We got number one chinese retaurant
	    ourDefFile = files[i];
	    break;
	}
    }
    if( ourDefFile.empty() )throw MugenException( "Cannot locate player definition file for: " + location );
    
    cout << loc << ourDefFile << endl;
    
    MugenReader reader( loc + ourDefFile );
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
