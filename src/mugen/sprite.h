#ifndef mugen_sprite_h
#define mugen_sprite_h

#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>

#include "util.h"
#include "common.h"

namespace PaintownUtil = ::Util;

namespace Graphics{
class Bitmap;
}

namespace Mugen{

class Sprite{
    public:
	Sprite(bool defaultMask);
	Sprite(const Sprite &copy);
	virtual ~Sprite();
	
	Sprite & operator=(const Sprite &copy);
	// For map searching
	bool operator<(const Sprite &copy);
	
	// Reads in the sprite info from stream
	void read(std::ifstream & ifile, const int loc);
	
	// Render sprite
	//void render(const int xaxis, const int yaxis, Bitmap &, const double scalex=1, const double scaley=1);
	//void render(int facing, int vfacing, const int xaxis, const int yaxis, Bitmap &, const double scalex=1, const double scaley=1);
	void render(const int xaxis, const int yaxis, const Graphics::Bitmap &where, const Mugen::Effects &effects = Mugen::Effects());

        /* for parallax support */
        void drawPartStretched(int sourceX1, int sourceY, int sourceWidth, int sourceHeight, int destX, int destY, int destWidth, int destHeight, const Mugen::Effects & effects, const Graphics::Bitmap & work);
	
	// load/reload sprite
        PaintownUtil::ReferenceCount<Graphics::Bitmap> load(bool mask);
	void reload(bool mask=true);
        /* deletes raw pcx data */
        void unloadRaw();

        /* just copies the bitmap */
        void copyImage(const PaintownUtil::ReferenceCount<Mugen::Sprite> copy);

	int getWidth() const;
	int getHeight() const;

        bool isLoaded() const;
	
        /* FIXME: replace types with uintX_t */
	// Setters getters
	inline void setNext(const long n) { next = n; }
	inline void setLocation(const long l) { location = l; }
	inline void setLength(const long l) { length = l; }
	inline void setRealLength(const long l) { reallength = l; }
	inline void setNewLength(const long l) { reallength = l; }
	inline void setX(const short x){ this->x = x; }
	inline void setY(const short y){ this->y = y; }
	inline void setGroupNumber(const unsigned short gn){ groupNumber = gn; }
	inline void setImageNumber(const unsigned short in){ imageNumber = in; }
	inline void setPrevious(const unsigned short p){ prev = p; }
	inline void setSamePalette(const bool p){ samePalette = p; };
	
	void loadPCX(std::ifstream & ifile, bool islinked, bool useact, unsigned char palsave1[], bool mask);
	
	inline unsigned long getNext() const { return next; }
	inline unsigned long getLocation() const { return location; }
	inline unsigned long getLength() const { return length; }
	inline unsigned long getRealLength() const { return reallength; }
	inline unsigned long getNewLength() const { return newlength; }
	inline short getX() const { return x; }
	inline short getY() const { return y; }
	inline unsigned short getGroupNumber() const { return groupNumber; }
	inline unsigned short getImageNumber() const { return imageNumber; }
	inline unsigned short getPrevious() const { return prev; }
	inline bool getSamePalette() const { return samePalette; }
	inline const char *getComments() const { return comments; }
	
        // static void draw(const Graphics::Bitmap &bmp, const int xaxis, const int yaxis, const int x, const int y, const Graphics::Bitmap &where, const Mugen::Effects &effects);

    protected:
        /* destroy allocated things */
        void cleanup();
	
	/* get the internal bitmap */
        PaintownUtil::ReferenceCount<Graphics::Bitmap> getBitmap(bool mask);

        /* get the properly scaled sprite */
        PaintownUtil::ReferenceCount<Graphics::Bitmap> getFinalBitmap(const Mugen::Effects & effects);
	
    private:
	uint32_t next;
	uint32_t location;
	uint32_t length;
	uint32_t reallength;
	uint32_t newlength;
	int16_t x;
	int16_t y;
	uint16_t groupNumber;
	uint16_t imageNumber;
	uint16_t prev;
	bool samePalette;
	char comments[12];
	char * pcx;
        int maskColor;

        /* come straight from the pcx */
        int width, height;
        bool loaded;

        bool defaultMask;
	
        /* Loaded with a palette that may not be our own */
        PaintownUtil::ReferenceCount<Graphics::Bitmap> unmaskedBitmap;
        PaintownUtil::ReferenceCount<Graphics::Bitmap> maskedBitmap;
        
        void draw(const PaintownUtil::ReferenceCount<Graphics::Bitmap> &, const int xaxis, const int yaxis, const Graphics::Bitmap &, const Mugen::Effects &);
};

}

#endif
