#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_reader.h"

MugenReader::MugenReader( const char * file ){
  ifile.open( file );
  myfile = string( file );
}

MugenReader::MugenReader( const string & file ){
  ifile.open( file.c_str() );
  myfile = file;
}

MugenReader::~MugenReader(){
	
}

const std::vector< MugenSection > & MugenReader::getCollection() throw(MugenException){
  
  if ( !ifile ){
      throw MugenException( std::string("Could not open ") + myfile );
  }
  
  std::string line;
  /*
  const char openbracket = '[';
  const char closebracket = ']';
  const char comment = ';';
  const char seperator = ',';
  const char
  */
  
  searchState state = section;
  
  while( !ifile.eof() ){
      getline( ifile, line );
      // Use to put the section name in
      std::string sectionName;
      // Use to hold content in the options
      std::string contentHolder;
      // Our section and items
      MugenSection sectionHolder;
      // Place holder to put back all the grabbed content
      MugenItemContent itemHolder;
      
      for( unsigned int i = 0; i < line.size(); ++i ){
	    if ( line[i] == ' ' )continue;
	    // Go to work
	    switch( state ){
	      case section:{
		
		break;
	      }
	      case contentNext:{
		
		break;
	      }
	      case contentGet:{
	      default:
		
		break;
	      }
	    }
      }
  }
  
  return collection;
}

void MugenReader::addSection( MugenSection section ){
  collection.push_back ( section );
}


