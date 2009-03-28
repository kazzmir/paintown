#ifndef mugen_util_h
#define mugen_util_h

#include <string>
#include <map>
#include <vector>
#include "mugen_exception.h"

class Bitmap;
class MugenAnimation;
class MugenBackground;
class MugenSprite;
class MugenSound;
class MugenItemContent;
class MugenSection;

struct MugenUtil{
    static void fixCase( std::string &str );
    static void removeSpaces( std::string &str );
    static void invertSlashes( std::string &str );
    static std::string stripDir( const std::string &str );
    static std::string fixFileName( const std::string &dir, std::string str );
    static std::string getFileDir( const std::string &dir );
    // If you use this, please delete the item after you use it, this isn't java ok
    static MugenItemContent *parseOpt( const std::string &opt );
    static bool readPalette(const string &filename, unsigned char *pal);
    static void readSprites(const string & filename, const string & palette, map<unsigned int,map<unsigned int, MugenSprite *> > & sprites) throw (MugenException);
    static void readSounds(const string & filename, std::map<unsigned int,std::map<unsigned int, MugenSound *> > & sounds) throw (MugenException);
    // Reads and compiles collections from a string rather than using a file ( You will need to delete the MugenSection's independently if you use this
    static std::vector< MugenSection * > configReader(  const std::vector<std::string> &configdata );
    // Get background: The background must be deleted if used outside of stage/menus (Note: we give the background a ticker to whatever is running it)
    static MugenBackground *getBackground( const unsigned long int &ticker, MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites );
    // Get animation: The animation must be deleted if used outside of stage/animation (stage and character do the deletion in this case)
    static MugenAnimation *getAnimation( MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites );
    static std::string getCorrectFileLocation( const std::string &dir, const std::string &file );
};

#endif
