/*
--------------
About
--------------
A Freetype wrapper for use with allegro
Feel free to do whatever you like with this, by all means enjoy!
Just add it to your project

--------------
Linking
--------------
on linux:
g++ `freetype-config --cflags` ftalleg.cpp myfiles.cpp `freetype-config --libs` `allegro-config --libs`

on windows (you may need to include the freetype dir location, ie -Ic:/mingw/include):
g++ ftalleg.cpp myfiles.cpp -lfreetype -lalleg

--------------
Disclaimer
--------------
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE 
SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FT_FONT_H
#define FT_FONT_H

// #include <allegro.h>
#include <map>
#include <string>
#include <math.h>
#include <exception>
#include "file-system.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "thread.h"
#include "graphics/color.h"

#ifdef USE_ALLEGRO5
struct ALLEGRO_FONT;
#endif
	
#define GLYPH_PI	3.14159265358979323846
#define GLYPH_SQRT2	1.41421356237309504880

namespace Graphics{
class Bitmap;
}

namespace ftalleg {

    class Exception: public std::exception {
    public:
        Exception();
	Exception(const std::string reason);

	inline const std::string & getReason() const {
		return reason;
	}

	virtual ~Exception() throw();

    protected:
        std::string reason;
    };

	//!  Internal class for freetype to use
	/*!  
	 * This holds necessary information regarding a character \n
	 * from Freetype.
	 */
    class character{
    public:
        //! Constructor
        character();

        //! Destructor
        ~character();

        //! Unicode representation of character
        signed long unicode;

        //! Width of character
        int width;

        //! Height of character
        int height;

        //! Space on the left of a character (assists on positioning the character)
        int left;

        //! Space on top of the character (assists on positioning the character)
        int top;

        //! Space on the right of a character (assists on positioning the character)
        int right;

        //! Pitch of a character (assists on positioning the character)
        int pitch;

        //! Amount of shades of grays the FT_Bitmap holds
        int grays;

        //! Entire rows of the FT_Bitmap
        int rows;

        //! Entire length of the character with spacing and all
        int length;

        //! FT_Bitmap raw data
        unsigned char *line;
    };
	
	//!  Internal class for freetype to use
	/*!  
	 * Properties used for indexing a given font according to its size and matrix \n
	 */
        class fontSize{
        public:
            fontSize();
            fontSize(int width, int height);
            ~fontSize();
            FT_UInt width;
            FT_UInt height;
            int italics;
            int angle;

            bool operator<(const fontSize &fs) const;

            int createKey() const;
        };
	
#ifdef USE_ALLEGRO5
        class freetype{
        public:
            freetype(const Filesystem::AbsolutePath & str, const int x, const int y);
            ~freetype();

            /* we need two versions of a font, one just for getting dimensions
             * of text and one for rendering.
             * the one that gets dimensions will be made up of a memory bitmap
             * and the one for rendering will use video bitmaps.
             * this means that the dimension one can be used from any thread
             * because memory bitmaps are safe in a threaded context whereas
             * video bitmaps can only be used in the main thread.
             */
            struct FontUse{
                ALLEGRO_FONT * memory;
                ALLEGRO_FONT * video;
            };

            enum ftAlign{
                ftLeft = 0,
                ftCenter = 1,
                ftRight = 2
            };

            void setSize( unsigned int w, unsigned int h);
            int getHeight(const std::string & str) const;
            int getLength(const std::string & text) const;
            void getSize(int * w, int * h) const;
            void render(int x, int y, const Graphics::Color & color, const Graphics::Bitmap & bmp, ftAlign alignment, const std::string & text, int marker, ...);
        private:
            ALLEGRO_FONT * currentMemoryFont() const;
            ALLEGRO_FONT * currentVideoFont() const;

            Graphics::Bitmap alive;
            const Filesystem::AbsolutePath path;
            int width;
            int height;
            int original_size;
            Util::Thread::LockObject lock;
            std::map<int, FontUse> fonts;
        };
#else
	//!  Freetype based font system
	/*!  
	 * Allows us to use freetype in allegro
	 */
        class freetype{
        private:
            //! Filename
            std::string currentFilename;

            //! Is the face loaded
            bool faceLoaded;

            //! Does the face have kerning
            bool kerning;

            //! Current index default 0
            int currentIndex;

            //! for internal use disregard
            bool internalFix;

            int maximumHeight;

            //! Font size
            fontSize size;

            //! Current Set system name
            std::string systemName;

            //! Face
            FT_Face face;

            //! Face Name
            std::string faceName;

            /*
            //! Current character
            character *currentChar;
            */

            Util::Thread::LockObject lock;

            //! Lookup Table by size
            std::map<int, std::map<signed long, character*> >fontTable;

            //! Extract glyph
            character * extractGlyph(signed long unicode);

            int calculateMaximumHeight();

            //! Create single index
            void createIndex();

            //! Render a character from the lookup table (utilizing the workBitmap)
            void drawCharacter(signed long unicode, int &x1, int &y1, const Graphics::Bitmap & bitmap, const Graphics::Color & color);

            int height( long code ) const;
            int calculateHeight( const std::string & str ) const;

            /* only called by the destructor */
            void destroyGlyphIndex();

        public:
            //! Constructor
            freetype(const Filesystem::AbsolutePath & str, const int x, const int y );

            //! Destructor
            ~freetype();

            //! Enum list of alignments
            enum ftAlign{
                ftLeft = 0,
                ftCenter = 1,
                ftRight = 2
            };

            //! Load font from memory
            bool load(const unsigned char *memoryFont, unsigned int length, int index, unsigned int width, unsigned int height);

            //! Load font from file
            bool load(const Filesystem::AbsolutePath & filename, int index, unsigned int width, unsigned int height);

            //! Get text length
            int getLength(const std::string & text);

            //! Render font to a bitmap
            void render(int x, int y, const Graphics::Color & color, const Graphics::Bitmap & bmp, ftAlign alignment, const std::string & text, int marker, ...);

            //! Set size
            void setSize( unsigned int w, unsigned int h);

            //! Set italics
            void setItalics(int i);

            /* get the size attributes (close to width/height) */
            void getSize(int * w, int * h) const;

            //! Get Width
            int getWidth() const;

            //! Get Height
            int getHeight( const std::string & str ) const;

            //! Get Italics
            int getItalics();
        };
#endif
}

#endif /* FT_FONT_H */
