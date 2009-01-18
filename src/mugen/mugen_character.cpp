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
#include "globals.h"

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
    std::string baseDir = Util::getDataPath() + "mugen/chars/" + location + "/";
    Global::debug(1) << baseDir << endl;
    std::vector< string > files = Util::getFiles( baseDir, "*" );
    Global::debug(1) << "Got files " << files.size() << endl;
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
    
    cout << baseDir << ourDefFile << endl;
    
    MugenReader reader( baseDir + ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
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
		    // Add parse info
		}
		else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "Files" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		const std::string &itemhead = item->query();
		if( itemhead == "cmd" ){
		    *content->getNext() >> cmdFile;
		}
		else if (itemhead == "cns" ){
		    *content->getNext() >> constantsFile;
		}
		else if (itemhead == "st" ){
		    *content->getNext() >> stateFile;
		}
		else if (itemhead == "stcommon" ){
		    *content->getNext() >> commonStateFile;
		}
		else if (itemhead == "sprite" ){
		    *content->getNext() >> sffFile;
		}
		else if (itemhead == "anim" ){
		    *content->getNext() >> airFile;
		}
		else if (itemhead == "sound" ){
		    *content->getNext() >> sndFile;
		}
		else if (itemhead == "pal1" ){
		    *content->getNext() >> palFile[0];
		}
		else if (itemhead == "pal2" ){
		    *content->getNext() >> palFile[1];
		}
		else if (itemhead == "pal3" ){
		    *content->getNext() >> palFile[2];
		}
		else if (itemhead == "pal4" ){
		    *content->getNext() >> palFile[3];
		}
		else if (itemhead == "pal5" ){
		    *content->getNext() >> palFile[4];
		}
		else if (itemhead == "pal6" ){
		    *content->getNext() >> palFile[5];
		}
		else if (itemhead == "pal7" ){
		    *content->getNext() >> palFile[6];
		}
		else if (itemhead == "pal8" ){
		    *content->getNext() >> palFile[7];
		}
		else if (itemhead == "pal9" ){
		    *content->getNext() >> palFile[8];
		}
		else if (itemhead == "pal10" ){
		    *content->getNext() >> palFile[9];
		}
		else if (itemhead == "pal11" ){
		    *content->getNext() >> palFile[10];
		}
		else if (itemhead == "pal12" ){
		    *content->getNext() >> palFile[11];
		}
		else throw MugenException( "Unhandled option in Files Section: " + itemhead );
	    }
	}
	else if( head == "Arcade" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		const std::string &itemhead = item->query();
		if( itemhead == "intro.storyboard" ){
		    *content->getNext() >> introFile;
		}
		else if (itemhead == "ending.storyboard" ){
		    *content->getNext() >> endingFile;
		}
		else throw MugenException( "Unhandled option in Arcade Section: " + itemhead );
	    }
	}
    }
    
    /* Sprites */
    MugenSffReader spriteReader( baseDir + sffFile );
    sprites = spriteReader.getCollection();
    
    /* Animations */
    // Need to build the air reader
    
    /* Sounds */
    MugenSndReader soundReader( baseDir + sndFile );
    sounds = soundReader.getCollection();
    
}
