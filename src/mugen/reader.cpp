#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "item-content.h"
#include "section.h"
#include "reader.h"
#include "util/debug.h"

using namespace std;

MugenReader::MugenReader( const char * file ):
usingText(false){
    Global::debug(2) << "[mugen reader] Opening file '" << file << "'" << endl;
    ifile.open( file );
    if (!ifile){
        perror("cant open file");
    }
    myfile = string( file );
}

MugenReader::MugenReader( const string & file ):
usingText(false){
    Global::debug(2) << "[mugen reader] Opening file '" << file << "'" << endl;
    ifile.open( file.c_str() );
    if (!ifile){
        perror("cant open file");
    }
    myfile = file;
}

MugenReader::MugenReader( const std::vector<std::string> &text ):
configData(text),
usingText(true){
}

MugenReader::~MugenReader(){
    if(!usingText){
	ifile.close();
    }

    for( std::vector< MugenSection * >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
        if(*i)delete (*i);
    }

}

const std::vector< MugenSection * > & MugenReader::getCollection() throw(MugenException){
    if ( configData.empty() && !ifile ){
	throw MugenException( std::string("Could not open file: \"") + myfile + std::string("\" or Read Text!"), __FILE__, __LINE__);
    }
    
    // get lines
    std::vector<std::string> temp;
    if (ifile && !usingText){
	std::string line;
	while( !ifile.eof() ){
	    std::string line;
	    getline( ifile, line );
	    temp.push_back(line);
	}
    } else {
	temp.assign(configData.begin(),configData.end());
    }
    
    for( std::vector<std::string>::const_iterator i = temp.begin(); i != temp.end(); ++i ){
	// get line
	const std::string line = *i;
	
	if (isSection(line)){
	    // Grab section and add it to the stack
	    addSection(extractSection(line));
	} else if (hasContent(line)){
	    // Extract the content add it to the stack
	    addContent(extractContent(line));
	}
    }
    return collection;
}

void MugenReader::addSection( MugenSection * section ){
    collection.push_back ( section );
}

void MugenReader::addContent( MugenItemContent *content ){
    *collection.back() << content;
}

bool MugenReader::isSection( const std::string &line ){
    const char comment = ';';
    const char openbracket = '[';
    const char closebracket = ']';
    bool foundSection = false;
    int charCount = 0;
    
    for (unsigned int i = 0; i < line.size(); ++i){
	if ( line[i] == ' ' && !foundSection ){ 
	    continue;
	} else if ( line[i] == openbracket){
	    // Check if we have characters if so we might have clsn
	    if (charCount){
		return false;
	    }
	    // Found start of a section
	    foundSection = true;
	} else if ( foundSection && line[i] == closebracket){
	    // Found close of section it's a section
	    return true;
	} else if ( line[i] == comment ){
	    // oops comment before a closebracket, there is no section
	    return false;
	} else {
	    // Found something other than a bracket this isn't a section
	    charCount++;
	}
    }
    // No section here
    return false;
}

bool MugenReader::hasContent( const std::string &line ){
    const char comment = ';';
    const char * eol = "\r\n";
    bool foundContent = false;
    for (unsigned int i = 0; i < line.size(); ++i){
	if ( line[i] == ' ' ){ 
	    continue;
	} else if ( !foundContent && line[i] == comment ){
	    // oops comment before content, there is no content
	    return false;
	} else if ( !foundContent && strchr(eol, line[i]) ){
	    // Found end of line and nothing else, no content
	    return false;
	} else {
	    // There's something usefull allow to be parsed
	    return true;
	}
    }
    // No content here
    return false;
}
	
MugenSection *MugenReader::extractSection( const std::string &line ){
    Global::debug(3) << line << endl;
    const char comment = ';';
    const char openbracket = '[';
    const char closebracket = ']';
    bool foundSection = false;
    std::string contentHolder = "";
    
    for (unsigned int i = 0; i < line.size(); ++i){
	if ( line[i] == ' ' && !foundSection ){ 
	    continue;
	} else if ( line[i] == openbracket){
	    // Found start of a section
	    foundSection = true;
	} else if ( foundSection && line[i] == closebracket){
	    // Found close of section it's a section make a new one and return it
	    MugenSection *temp = new MugenSection();
	    temp->setHeader( contentHolder );
	    return temp;
	} else if ( line[i] == comment ){
	    // oops shouldn't happen, but return 0 anyhow
	    return 0;
	} else {
	    // Grab our section name
	    contentHolder += line[i];
	}
    }
    
    return 0;
}

MugenItemContent *MugenReader::extractContent( const std::string &line ){
    Global::debug(3) << line << endl;
    const char openbracket = '[';
    const char closebracket = ']';
    const char comment = ';';
    const char seperator = ',';
    const char colon = ':';
    const char equal = '=';
    const char quote = '"';
    const char * ignored = "\r\n";
    
    // Use to hold content
    std::string contentHolder = "";
    // Place holder to put grabbed content
    MugenItemContent *itemHolder = new MugenItemContent();
    
    bool inQuote = false;
    
    for (unsigned int i = 0; i < line.size(); ++i){
	if (line[i] == ' ' && !inQuote){ 
	    continue;
	} else if (line[i] == comment){
	    // found comment... store what remains and get out
	    if( !contentHolder.empty() ){
		*itemHolder << contentHolder;
	    }
	    return itemHolder;
	} else if ( strchr(ignored, line[i]) ){
	    // Found end of line.. as above store what remains and get out
	    if( !contentHolder.empty() ){
		*itemHolder << contentHolder;
	    }
	    return itemHolder;
	} else if( line[i] == equal && !inQuote ){
	    if( !contentHolder.empty() ){
		*itemHolder << contentHolder;
	    }
	    contentHolder = "";
	} else if( line[i] == quote ){
	    inQuote = !inQuote;
	} else if( line[i] == colon || line[i] == seperator ){
	    // Grab an item
	    if( !contentHolder.empty() ){
		*itemHolder << contentHolder;
	    }
	    contentHolder = "";
	} else {
	    contentHolder+=line[i];
	}
    }
    
    if (!contentHolder.empty()){
	*itemHolder << contentHolder;
    }
    
    if (itemHolder->hasItems()){
	return itemHolder;
    }
    
    return 0;
}
