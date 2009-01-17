#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_sound.h"
#include "mugen_snd_reader.h"

MugenSndReader::MugenSndReader( const char * file ){
  ifile.open( file );
  myfile = string( file );
}

MugenSndReader::MugenSndReader( const string & file ){
  ifile.open( file.c_str() );
  myfile = file;
}

MugenSndReader::~MugenSndReader(){
  
  for( std::map< int, std::map< int, MugenSound * > >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
      for( std::map< int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  delete j->second;
      }
  }
	
}

const std::map< int, std::map< int, MugenSound * > > & MugenSndReader::getCollection() throw(MugenException){
  
  if ( !ifile ){
      throw MugenException( std::string("Could not open ") + myfile );
  }
  
  return collection;
}

void MugenSndReader::addSound( int group, int number, MugenSound * sprite ){
  collection[group][number] = sprite;
}


