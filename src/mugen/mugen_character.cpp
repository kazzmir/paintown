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
#include "util/bitmap.h"

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
    
     // Get rid of bitmaps
    for( std::map< unsigned int, std::map< unsigned int, Bitmap * > >::iterator i = bitmaps.begin() ; i != bitmaps.end() ; ++i ){
      for( std::map< unsigned int, Bitmap * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
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
    std::string realstr = MugenUtil::stripDir( location );
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

// Render sprite
void MugenCharacter::renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp , 
				   const int flip, const double scalex, const double scaley ){
    MugenSprite *sprite = sprites[group][image];
    if (sprite){
	Bitmap *bitmap = bitmaps[group][image];
	if (!bitmap){
	    bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength));
	    bitmaps[group][image] = bitmap;
	}
	if (flip == 1){
	    bitmap->drawStretched(x + sprite->x,y + sprite->y, (int)(bitmap->getWidth() * scalex), (int)(bitmap->getHeight() * scaley), *bmp);
	} else if (flip == -1){
	    // temp bitmap to flip and crap
	    Bitmap temp = Bitmap::temporaryBitmap(bitmap->getWidth(), bitmap->getHeight());
	    temp.fill(Bitmap::MaskColor);
	    bitmap->drawHFlip(0,0,temp);
	    temp.drawStretched(x + sprite->x,y + sprite->y, (int)(bitmap->getWidth() * scalex), (int)(bitmap->getHeight() * scaley), *bmp);
	}
    }
}

// animations
void MugenCharacter::bundleAnimations() throw( MugenException){
    MugenReader reader( MugenUtil::fixFileName(baseDir, airFile) );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    if( collection.empty() ) throw MugenException( "Problem loading Animations from file: " + airFile );
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	head.replace(0,13,"");
	int h;
	MugenItem(head) >> h;
	animations[h] = MugenUtil::getAnimation(collection[i], sprites);
	Global::debug(1) << "Added Animation 'Begin Action " << h << "' : '" << animations[h]->getName(animations[h]->getType()) << "'" << endl;
    }
}


