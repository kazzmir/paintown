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
class MugenCharacterSelect;

namespace Mugen{
namespace Util{
    void fixCase( std::string &str );
    void removeSpaces( std::string &str );
    void invertSlashes( std::string &str );
    std::string stripDir( const std::string &str );
    std::string fixFileName( const std::string &dir, std::string str );
    std::string getFileDir( const std::string &dir );
    // If you use this, please delete the item after you use it, this isn't java ok
    MugenItemContent *parseOpt( const std::string &opt );
    bool readPalette(const std::string &filename, unsigned char *pal);
    void readSprites(const std::string & filename, const std::string & palette, std::map<unsigned int, std::map<unsigned int, MugenSprite *> > & sprites) throw (MugenException);
    void readSounds(const std::string & filename, std::map<unsigned int,std::map<unsigned int, MugenSound *> > & sounds) throw (MugenException);
    // Get background: The background must be deleted if used outside of stage/menus (Note: we give the background a ticker to whatever is running it)
    MugenBackground *getBackground( const unsigned long int &ticker, MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites );
    // Get animation: The animation must be deleted if used outside of stage/animation (stage and character do the deletion in this case)
    MugenAnimation *getAnimation( MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites );
    std::string getCorrectFileLocation( const std::string &dir, const std::string &file );
}

class Point{
    public:
	int x;
	int y;
	Point();
	Point(int x, int y);
	Point &operator=(const Point &p);
	~Point();
};

}

#endif
