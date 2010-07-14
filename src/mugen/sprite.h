#ifndef mugen_sprite_h
#define mugen_sprite_h

#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>

#include "mugen/util.h"

class Bitmap;

class MugenSprite{
    public:
	MugenSprite();
	MugenSprite( const MugenSprite &copy );
	~MugenSprite();
	
	MugenSprite & operator=( const MugenSprite &copy );
	// For map searching
	bool operator<( const MugenSprite &copy );
	
	// Reads in the sprite info from stream
	void read(std::ifstream & ifile, const int loc);
	
	// Render sprite
	//void render(const int xaxis, const int yaxis, Bitmap &, const double scalex=1, const double scaley=1);
	//void render(int facing, int vfacing, const int xaxis, const int yaxis, Bitmap &, const double scalex=1, const double scaley=1);
	void render(const int xaxis, const int yaxis, const Bitmap &where, const Mugen::Effects &effects = Mugen::Effects());
	
	// load/reload sprite
	void load(bool mask=true);
	void reload(bool mask=true);

        /* just copies the bitmap */
        void copyImage(const MugenSprite * copy);
	
	// get sprite
	Bitmap *getBitmap() const;
	int getWidth();
	int getHeight();
	
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
	
	void loadPCX(std::ifstream & ifile, bool islinked, bool useact, unsigned char palsave1[]);
	
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

    protected:
        /* destroy allocated things */
        void cleanup();
	
    private:
        /* FIXME: replace these types with explicitly sized types like
         * unsigned long -> uint32
         * short -> int16
         */
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
	
	Bitmap *bitmap;
	
	void draw(const Bitmap &, const int xaxis, const int yaxis, const Bitmap &, const Mugen::Effects &);
};

#endif
