#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "globals.h"

#include "mugen_sprite.h"
#include "mugen_sff_reader.h"

MugenSffReader::MugenSffReader( const char * file ):
location(16){
  ifile.open( file, ios::binary );
  myfile = string( file );
}

MugenSffReader::MugenSffReader( const string & file ):
location(16){
  ifile.open( file.c_str(), ios::binary );
  myfile = file;
}

MugenSffReader::~MugenSffReader(){
  ifile.close();
  for( std::map< int, std::map< int, MugenSprite * > >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
      for( std::map< int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  delete j->second;
      }
  }
	
}

const std::map<int, std::map<int, MugenSprite *> > & MugenSffReader::getCollection() throw (MugenException){
    if (!ifile){
	throw MugenException( "Could not open SFF file: " + myfile );
    }

    /* Lets go ahead and skip the crap -> (Elecbyte signature and version)
     * start at the 16th byte
     */
    ifile.seekg(location,ios::beg);
    int totalGroups;
    int totalImages;
    
    /* this probably isn't endian safe.. */
    ifile.read((char *)&totalGroups, 4);
    ifile.read((char *)&totalImages, 4);
    ifile.read((char *)&location, 4);
    
    Global::debug(1) << "Got Total Groups: " << totalGroups << ", Total Images: " << totalImages << ", Next Location in file: " << location << endl;
    
    // We got some stuff
    if (totalImages > 0){
	for (int i = 0; i < totalImages; ++i){
	    getNext();
	}
    }
    
    return collection;
}

void MugenSffReader::addSprite( MugenSprite * sprite ){
    collection[ sprite->groupNumber ][ sprite->imageNumber ] = sprite;
}


// Get next sprite
void MugenSffReader::getNext(){
    if ( !location )return;
    // Go to next sprite
    ifile.seekg(location, ios::beg);
    // next sprite
    MugenSprite *temp = new MugenSprite();
    
    ifile.read((char *)&temp->next, 4);
    ifile.read((char *)&temp->length, 4);
    ifile.read((char *)&temp->x, 2);
    ifile.read((char *)&temp->y, 2);
    ifile.read((char *)&temp->groupNumber, 2);
    ifile.read((char *)&temp->imageNumber, 2);
    ifile.read((char *)&temp->prev, 2);
    ifile.read((char *)&temp->samePalette, 1);
    ifile.read((char *)&temp->comments, 14);
    temp->pcx = new char[temp->length];
    ifile.read((char *)temp->pcx, temp->length);
    
    // Set the next file location
    location = temp->next;
    
    // Add our sprite
    addSprite(temp);
}
