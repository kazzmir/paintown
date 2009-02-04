#ifndef mugen_util_h
#define mugen_util_h

#include <string>
#include <map>
#include <vector>
#include "mugen_exception.h"

class Bitmap;
class MugenSprite;
class MugenSound;
class MugenItemContent;

struct MugenUtil{
    static void fixCase( std::string &str );
    static void removeSpaces( std::string &str );
    static void invertSlashes( std::string &str );
    static inline std::string getHeadDir( const std::string &dir ){ return dir.substr( ( dir.find_last_of( '/' ) != std::string::npos ? dir.find_last_of( '/' )+1 : 0 ),dir.size() ); }
    static std::string fixFileName( const std::string &dir, std::string str );
    // If you use this, please delete the item after you use it, this isn't java ok
    static MugenItemContent *parseOpt( const std::string &opt );
    static void readSprites(const string & filename, const string & palette, map<unsigned int,map<unsigned int, MugenSprite *> > & sprites) throw (MugenException);
    static void readSounds(const string & filename, std::map<unsigned int,std::map<unsigned int, MugenSound *> > & sounds) throw (MugenException);
};

#endif
