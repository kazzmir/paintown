#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_sprite.h"
#include "mugen_sff_reader.h"

MugenSffReader::MugenSffReader( const char * file ){
  ifile.open( file );
  myfile = string( file );
}

MugenSffReader::MugenSffReader( const string & file ){
  ifile.open( file.c_str() );
  myfile = file;
}

MugenSffReader::~MugenSffReader(){
  
  for( std::map< int, std::map< int, MugenSprite * > >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
      for( std::map< int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  delete j->second;
      }
  }
	
}

const std::map< int, std::map< int, MugenSprite * > > & MugenSffReader::getCollection() throw(MugenException){
  
  if ( !ifile ){
      throw MugenException( std::string("Could not open ") + myfile );
  }
  
  return collection;
}

void MugenSffReader::addSprite( int group, int number, MugenSprite * sprite ){
  collection[group][number] = sprite;
}


