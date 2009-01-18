#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_sound.h"
#include "mugen_snd_reader.h"

MugenSndReader::MugenSndReader( const char * file ):
location(16){
  ifile.open( file, ios::binary );
  myfile = string( file );
}

MugenSndReader::MugenSndReader( const string & file ):
location(16){
  ifile.open( file.c_str(), ios::binary );
  myfile = file;
}

MugenSndReader::~MugenSndReader(){
  ifile.close();
  for( std::map< int, std::map< int, MugenSound * > >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
      for( std::map< int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  delete j->second;
      }
  }
	
}

const std::map< int, std::map< int, MugenSound * > > & MugenSndReader::getCollection() throw(MugenException){
  
  if ( !ifile ){
      throw MugenException( std::string("Could not open SND file: ") + myfile );
  }
  
   // Lets go ahead and skip the crap -> (Elecbyte signature and version) start at the 16th byte
    ifile.seekg(location,ios::beg);
    int totalSounds;
    
    ifile.read( (char *)&totalSounds, 4 );
    ifile.read( (char *)&location, 4 );
    
    // We got some stuff
    if( totalSounds > 0){
	for( int i = 0; i < totalSounds; ++i ){
	    getNext();
	}
    }
  
  return collection;
}

void MugenSndReader::addSound( MugenSound * sound ){
  collection[ sound->groupNumber ][ sound->sampleNumber ] = sound;
}


// Get next sound data
void MugenSndReader::getNext(){
    if ( !location )return;
    // Go to next sprite
    ifile.seekg(location, ios::beg);
    // next sprite
    MugenSound *temp = new MugenSound();
    
    ifile.read( (char *)&temp->next, 4 );
    ifile.read( (char *)&temp->length, 4 );
    ifile.read( (char *)&temp->groupNumber, 4 );
    ifile.read( (char *)&temp->sampleNumber, 4 );
    temp->sample = new char[temp->length];
    ifile.read( (char *)temp->sample, temp->length );
    
    // Set the next file location
    location = temp->next;
    
    // Add our sound
    addSound( temp );
}

