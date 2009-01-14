#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_reader.h"

// Check for Clsn
static bool checkClsn( const std::string &clsn ){
  return ( clsn.find("Clsn") != std::string::npos );
}

MugenReader::MugenReader( const char * file ){
  ifile.open( file );
  myfile = string( file );
}

MugenReader::MugenReader( const string & file ){
  ifile.open( file.c_str() );
  myfile = file;
}

MugenReader::~MugenReader(){
  
  for( std::vector< MugenSection * >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
      delete (*i);
  }
	
}

const std::vector< MugenSection * > & MugenReader::getCollection() throw(MugenException){
  
  if ( !ifile ){
      throw MugenException( std::string("Could not open ") + myfile );
  }
  
  std::string line;
  
  const char openbracket = '[';
  const char closebracket = ']';
  const char openparen = '(';
  const char closeparen = ')';
  const char comment = ';';
  const char seperator = ',';
  const char colon = ':';
  const char equal = '=';
  const char notequal = '!';
  const char quote = '"';
  const char orcondition = '|';
  const char andcondition = '&';
  const char greater = '>';
  const char less = '<';
  const char add = '+';
  const char multiply = '*';
  
  
  SearchState state = Section;
  // Our section and items
  MugenSection * sectionHolder = 0;
  // Marker for areas
  bool beginSection = false;
  bool inQuote = false;
      
  while( !ifile.eof() ){
      getline( ifile, line );
      // Use to hold section or content in the options
      std::string contentHolder = "";
      // Place holder to put back all the grabbed content
      MugenItemContent *itemHolder = new MugenItemContent();
      // Needed to kill loop
      bool breakLoop = false;
      
      for( unsigned int i = 0; i < line.size(); ++i ){
	    // Go to work
	    switch( state ){
	      case Section:{
		// Done with this
		if( line[i] == comment ){
		  breakLoop = true;
		  break;
		}
		else if ( line[i] == ' ' && !beginSection )continue;
		//Start grabbing our section
		else if( line[i] == openbracket){
		  beginSection = true;
		}
		//End of our section store and go to ContentGet
		else if( line[i] == closebracket){
		  sectionHolder = new MugenSection();
		  sectionHolder->setHeader( contentHolder );
		  state = ContentGet;
		  beginSection = false;
		  breakLoop = true;
		  break;
		}
		else if( beginSection )contentHolder += line[i];
	      }
	      break;
	      case ContentGet:
	      default:{
		// Done with this
		if( line[i] == comment ){
		  if( itemHolder->hasItems() ){
		    if( !contentHolder.empty() ){
		      *itemHolder << contentHolder;
		    }
		    *sectionHolder << itemHolder;
		  }
		  breakLoop = true;
		  break;
		}
		// Check if we are near the end to kill it
		if( i+1 == line.size() && !contentHolder.empty() ){
		    if ( line[i] != ' ' ) contentHolder += line[i];
		    *itemHolder << contentHolder;
		    *sectionHolder << itemHolder;
		    breakLoop = true;
		    break;
		}
		// Buh bye spaces
		if ( line[i] == ' ' && !inQuote ){
		  continue;
		}
		// Check if this section is done, push it on the stack and reset everything
		if( line[i] == openbracket && !checkClsn( contentHolder ) && !itemHolder->hasItems() ){
		  if( sectionHolder->hasItems() )addSection( sectionHolder );
		  beginSection = true;
		  state = Section;
		  contentHolder = "";
		  break;
		}
		// It's peanut butter bracket time
		else if( line[i] == openbracket && !checkClsn( contentHolder ) && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = openbracket;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		else if( line[i] == closebracket && !checkClsn( contentHolder )  && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = closebracket;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// We got one push back the other and reset the holder to get the next
		else if( line[i] == colon || line[i] == seperator ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// Conditional or
		else if( line[i] == orcondition && !inQuote ){
		  // Check to see if the next line is another |
		  if( i+1 != line.size() ){
		    if( line[i+1] == orcondition ){
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = orcondition;
		    }
		    else {
		      contentHolder += orcondition;
		      *itemHolder << contentHolder;
		      contentHolder = "";
		    }
		  }
		}
		// Conditional and
		else if( line[i] == andcondition && !inQuote ){
		  // Check to see if the next line is another &
		  if( i+1 != line.size() ){
		    if( line[i+1] == andcondition ){
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = andcondition;
		    }
		    else {
		      contentHolder += andcondition;
		      *itemHolder << contentHolder;
		      contentHolder = "";
		    }
		  }
		}
		// Parenthesis open
		else if(line[i] == openparen && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = openparen;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// Parenthesis closed
		else if(line[i] == closeparen && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = closeparen;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// Equal
		else if(line[i] == equal && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = equal;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// Not Equal
		else if( line[i] == notequal && !inQuote ){
		  // Check to see if the next line is =
		  if( i+1 != line.size() ){
		    if( line[i+1] == equal ){
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = "!=";
			*itemHolder << contentHolder;
			contentHolder = "";
			i++;
		    }
		  }
		}
		// Greater than
		else if(line[i] == greater && !inQuote ){
		   // Check to see if the next line is =
		  if( i+1 != line.size() ){
		    if( line[i+1] == equal ){
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = ">=";
			*itemHolder << contentHolder;
			contentHolder = "";
			i++;
		    } else{
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = greater;
			*itemHolder << contentHolder;
			contentHolder = "";
		    }
		  } 
		}
		// Less than
		else if(line[i] == less && !inQuote ){
		   // Check to see if the next line is =
		  if( i+1 != line.size() ){
		    if( line[i+1] == equal ){
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = "<=";
			*itemHolder << contentHolder;
			contentHolder = "";
			i++;
		    } else{
			if( !contentHolder.empty() ) *itemHolder << contentHolder;
			contentHolder = less;
			*itemHolder << contentHolder;
			contentHolder = "";
		    }
		  } 
		}
		// Add
		else if(line[i] == add && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = add;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// Multiply
		else if(line[i] == multiply && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = multiply;
		  *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// We has a quote begin or end it
		else if( line[i] == quote ){
		  inQuote = !inQuote;
		}
		//Start grabbing our item
		else contentHolder += line[i];
	      }
	      break;
	    }
	    if( breakLoop )break;
      }
  }
  // Add in last section
  if( sectionHolder->hasItems() )addSection( sectionHolder );
  
  return collection;
}

void MugenReader::addSection( MugenSection * section ){
  collection.push_back ( section );
}


