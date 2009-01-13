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
  
  const char openbracket = '[';
  const char closebracket = ']';
  const char comment = ';';
  const char seperator = ',';
  const char colon = ':';
  const char equal = '=';
  const char quote = '"';
  
  
  SearchState state = Section;
  // Our section and items
  MugenSection sectionHolder;
  // Marker for areas
  bool beginSection = false;
  bool inQuote = false;
      
  while( !ifile.eof() ){
      getline( ifile, line );
      // Use to hold section or content in the options
      std::string contentHolder = "";
      // Place holder to put back all the grabbed content
      MugenItemContent itemHolder;
      
      for( unsigned int i = 0; i < line.size(); ++i ){
	    if ( line[i] == ' ' && (!beginSection || !inQuote) )continue;
	    // Go to work
	    switch( state ){
	      case Section:{
		// Done with this
		if( line[i] == comment )break;
		//Start grabbing our section
		else if( line[i] == openbracket){
		  beginSection = true;
		}
		//End of our section store and go to ContentGet
		else if( line[i] == closebracket){
		  sectionHolder.setHeader( contentHolder );
		  state = ContentGet;
		  beginSection = false;
		  break;
		}
		else if( beginSection )contentHolder += line[i];
		break;
	      }
	      case ContentGet:{
	      default:
		// Done with this
		if( line[i] == comment ){
		  if( itemHolder.hasItems() )sectionHolder << itemHolder;
		  break;
		}
		// This section is done, push it on the stack and reset everything
		else if( line[i] == openbracket ){
		  collection.push_back(sectionHolder);
		  sectionHolder.reset();
		  beginSection = true;
		  state = Section;
		}
		// We got one push back the other and reset the holder to get the next
		else if( line[i] == colon || line[i] == seperator || line[i] == equal ){
		  itemHolder << contentHolder;
		  contentHolder = "";
		}
		// We has a quote begin or end it
		else if( line[i] == quote ){
		  inQuote = !inQuote;
		}
		//Start grabbing our item
		else contentHolder += line[i];
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


