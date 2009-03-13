#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>
#include <sstream>
#include <iostream>

// To aid in filesize
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util/funcs.h"

#include "mugen_animation.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_character.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "globals.h"

MugenCharacter::MugenCharacter( const string & s ){
    this->location = s;
}

MugenCharacter::MugenCharacter( const char * location ){
    this->location = std::string(location);
}

MugenCharacter::~MugenCharacter(){
     // Get rid of sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
}

void MugenCharacter::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Util::getDataPath() + "mugen/chars/" + location + "/";
    Global::debug(1) << baseDir << endl;
    std::string realstr = MugenUtil::getHeadDir( location );
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, std::string(realstr + ".def") );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate player definition file for: " + location );
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	const std::string &head = collection[i]->getHeader();
	if( head == "Info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		MugenUtil::fixCase( itemhead );
		if ( itemhead.find("name")!=std::string::npos ){
		    *content->getNext() >> name;
                    Global::debug(1) << "Read name '" << name << "'" << endl;
		} else if ( itemhead.find("displayname")!=std::string::npos ){
		    *content->getNext() >> displayName;
		} else if ( itemhead.find("versiondate")!=std::string::npos ){
		    *content->getNext() >> versionDate;
		} else if ( itemhead.find("mugenversion")!=std::string::npos ){
		    *content->getNext() >> mugenVersion;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    *content->getNext() >> author;
		} else if ( itemhead.find("pal.defaults")!=std::string::npos ){
		    // Add parse info
		    MugenItemContent *temp = MugenUtil::parseOpt( content->getNext()->query() );
		    for( int pal = 0; pal < 12; ++pal ){
			if( temp->hasItems() )*temp->getNext() >> palDefaults[pal];
			else break;
			Global::debug(1) << "Pal" << pal << ": " << palDefaults[pal] << endl;
		    }
		    delete temp;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "Files" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces( itemhead);
		if( itemhead.find("cmd")!=std::string::npos ){
		    *content->getNext() >> cmdFile;
		}
		else if (itemhead.find("cns")!=std::string::npos ){
		    *content->getNext() >> constantsFile;
		}
		else if ( itemhead.find("st")!=std::string::npos ){
		    MugenUtil::removeSpaces(itemhead);
		    if (itemhead.find("st0") !=std::string::npos )*content->getNext() >> stFile[0];
		    else if (itemhead.find("st1") !=std::string::npos )*content->getNext() >> stFile[1];
		    else if (itemhead.find("st2")!=std::string::npos )*content->getNext() >> stFile[2];
		    else if (itemhead.find("st3")!=std::string::npos )*content->getNext() >> stFile[3];
		    else if (itemhead.find("st4")!=std::string::npos )*content->getNext() >> stFile[4];
		    else if (itemhead.find("st5")!=std::string::npos )*content->getNext() >> stFile[5];
		    else if (itemhead.find("st6")!=std::string::npos )*content->getNext() >> stFile[6];
		    else if (itemhead.find("st7")!=std::string::npos )*content->getNext() >> stFile[7];
		    else if (itemhead.find("st8")!=std::string::npos )*content->getNext() >> stFile[8];
		    else if (itemhead.find("st9")!=std::string::npos )*content->getNext() >> stFile[9];
		    else if (itemhead.find("st10")!=std::string::npos )*content->getNext() >> stFile[10];
		    else if (itemhead.find("st11")!=std::string::npos )*content->getNext() >> stFile[11];
		    else if (itemhead.find("st12")!=std::string::npos )*content->getNext() >> stFile[12];
		    else if( itemhead.find("stcommon")!=std::string::npos )*content->getNext() >> commonStateFile;
		    else if( itemhead.find("st")!=std::string::npos )*content->getNext() >> stateFile;
		    else throw MugenException( "Unhandled state file in Files Section: " + itemhead );
		}
		else if (itemhead.find("sprite") != std::string::npos ){
		    *content->getNext() >> sffFile;
		}
		else if ( itemhead.find("anim") != std::string::npos ){
		    *content->getNext() >> airFile;
		}
		else if ( itemhead.find("sound") != std::string::npos ){
		    *content->getNext() >> sndFile;
		}
		else if( itemhead.find("pal") != std::string::npos ){
		    if (itemhead.find("pal1") != std::string::npos )*content->getNext() >> palFile[0];
		    else if (itemhead.find("pal2") != std::string::npos )*content->getNext() >> palFile[1];
		    else if (itemhead.find("pal3") != std::string::npos )*content->getNext() >> palFile[2];
		    else if (itemhead.find("pal4") != std::string::npos )*content->getNext() >> palFile[3];
		    else if (itemhead.find("pal5") != std::string::npos )*content->getNext() >> palFile[4];
		    else if (itemhead.find("pal6") != std::string::npos )*content->getNext() >> palFile[5];
		    else if (itemhead.find("pal7") != std::string::npos )*content->getNext() >> palFile[6];
		    else if (itemhead.find("pal8") != std::string::npos )*content->getNext() >> palFile[7];
		    else if (itemhead.find("pal9") != std::string::npos )*content->getNext() >> palFile[8];
		    else if (itemhead.find("pal10") != std::string::npos )*content->getNext() >> palFile[9];
		    else if (itemhead.find("pal11") != std::string::npos )*content->getNext() >> palFile[10];
		    else if (itemhead.find("pal12") != std::string::npos )*content->getNext() >> palFile[11];
		    else throw MugenException( "Unhandled Palette File in Files Section: " + itemhead );
		}
		else throw MugenException( "Unhandled option in Files Section: " + itemhead );
	    }
	}
	else if( head == "Arcade" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces( itemhead);
		if( itemhead.find("intro.storyboard") != std::string::npos ){
		    *content->getNext() >> introFile;
		}
		else if (itemhead.find("ending.storyboard") != std::string::npos ){
		    *content->getNext() >> endingFile;
		}
		else throw MugenException( "Unhandled option in Arcade Section: " + itemhead );
	    }    
	}
    }
    Global::debug(1) << "Reading Sff (sprite) Data..." << endl; 
    /* Sprites */
    MugenUtil::readSprites( MugenUtil::fixFileName(baseDir, sffFile), MugenUtil::fixFileName(baseDir, palFile[0]), sprites );
    Global::debug(1) << "Reading Air (animation) Data..." << endl;
    /* Animations */
    bundleAnimations();
    Global::debug(1) << "Reading Snd (sound) Data..." << endl; 
    /* Sounds */
    MugenUtil::readSounds( MugenUtil::fixFileName( baseDir, sndFile ), sounds );
}

// animations
void MugenCharacter::bundleAnimations() throw( MugenException){
    MugenReader reader( MugenUtil::fixFileName(baseDir, airFile) );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    if( collection.empty() ) throw MugenException( "Problem loading Animations from file: " + airFile );
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	MugenAnimation *animation = new MugenAnimation();
	std::string head = collection[i]->getHeader();
	head.replace(0,13,"");
	std::vector<MugenArea> clsn1Holder;
	std::vector<MugenArea> clsn2Holder;
	bool clsn1Reset = false;
	bool clsn2Reset = false;
	bool setloop = false;
	while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Global::debug(1) << "Item Head: " << itemhead << endl;
		// Attack boxes
		if( itemhead.find("Clsn1Default") != std::string::npos ){
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn1Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenArea area;
			*content->getNext() >> area.x1;
			*content->getNext() >> area.y1;
			*content->getNext() >> area.x2;
			*content->getNext() >> area.y2;
			Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
			clsn1Holder.push_back(area);
		    }
		}
		// defend boxes
		else if( itemhead.find("Clsn2Default") != std::string::npos ){
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn2Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenArea area;
			*content->getNext() >> area.x1;
			*content->getNext() >> area.y1;
			*content->getNext() >> area.x2;
			*content->getNext() >> area.y2;
			Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
			clsn2Holder.push_back(area);
		    }
		}
		// defend boxes
		else if( itemhead.find("Clsn2") != std::string::npos ){
		    clsn2Reset = true;
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn2Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenArea area;
			*content->getNext() >> area.x1;
			*content->getNext() >> area.y1;
			*content->getNext() >> area.x2;
			*content->getNext() >> area.y2;
			Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
			clsn2Holder.push_back(area);
		    }
		}
		// Attack boxes
		else if( itemhead.find("Clsn1") != std::string::npos ){
		    clsn1Reset = true;
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn1Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenArea area;
			*content->getNext() >> area.x1;
			*content->getNext() >> area.y1;
			*content->getNext() >> area.x2;
			*content->getNext() >> area.y2;
			Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
			clsn1Holder.push_back(area);
		    }
		}
		// defend boxes for this frame
		else if( itemhead.find("loopstart") != std::string::npos ){
		    setloop = true;
		}
		// This is where we get our frame
		else if( itemhead.find("Clsn") == std::string::npos ){
		    // This is the new frame
		    MugenFrame *frame = new MugenFrame();
		    frame->defenseCollision = clsn2Holder;
		    frame->attackCollision = clsn1Holder;
		    frame->loopstart = setloop;
		    /* Get sprite details */
		    int group, spriteNumber;
		    // Need to get the parsed data and populate these above items
		    *item >> group;
		    *content->getNext() >> spriteNumber;
		    *content->getNext() >> frame->xoffset;
		    *content->getNext() >> frame->yoffset;
		    *content->getNext() >> frame->time;
		    Global::debug(1) << "Group: " << group << " | Sprite: " << spriteNumber << " | x: " << frame->xoffset << " | y: " << frame->yoffset << " | time: " << frame->time << endl;
		    // Check for flips
		    if( content->hasItems() ){
			std::string flip;
			*content->getNext() >> flip;
			MugenUtil::fixCase( flip );
			if( flip.find("h") != std::string::npos )frame->flipHorizontal = true;
			if( flip.find("v") != std::string::npos )frame->flipVertical = true;
		    }
		     // Color add
		    if( content->hasItems() ){
			std::string temp;
			*content->getNext() >> temp;
			if (temp[0] == 'A'){
			    frame->colorAdd = ADD;
			} else if (temp[0] == 'B'){
			    frame->colorAdd = SUB;
			}
			// Check if we have specified additions
			if (temp.size() > 1){
			    // Source
			    frame->colorSource = atoi(temp.substr(2,4).c_str());
			    // Dest
			    frame->colorDestination = atoi(temp.substr(6,8).c_str());
			}
		    }
		    // Add sprite
		    frame->sprite = sprites[(unsigned short)group][(unsigned short)spriteNumber];
                    if (frame->sprite == 0){
                        Global::debug(0) << "No sprite for group " << group << " number " << spriteNumber << endl;
                    }
		    // Add frame
		    animation->addFrame(frame);
		    // Clear if it was not a default set, since it's valid for only one frame
		    if( clsn1Reset ){
			clsn1Holder.clear();
			clsn1Reset = false;
		    }
		    if( clsn2Reset ){
			clsn2Holder.clear();
			clsn2Reset = false;
		    }
		    if( setloop )setloop = false;
		}
	}
	int h;
	MugenItem(head) >> h;
	animation->setType(MugenAnimationType(h));
	Global::debug(1) << "Adding Animation 'Begin Action " << h << "' : '" << animation->getName(animation->getType()) << "'" << endl;
	animations[h] = animation;
    }
}


