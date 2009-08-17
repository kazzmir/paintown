#ifndef mugen_sprite_h
#define mugen_sprite_h

#include <string>
#include <fstream>
#include <iostream>

class Bitmap;

enum TransType{
    NONE = 0,
    ADD,
    ADD1,
    SUB,
    ADDALPHA
};

class Effects{
    public:
	Effects();
	const Effects &operator=(const Effects &e);
	~Effects();
	TransType trans;
	int alphalow;
	int alphahigh;
	bool mask;
	int facing;
	int vfacing;
	int scalex;
	int scaley;
};

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
	void render(const int xaxis, const int yaxis, Bitmap &where, Effects &effects);
	
	// load/reload sprite
	void load(bool mask=true);
	void reload(bool mask=true);
	
	// get sprite
	Bitmap *getBitmap();
	const int getWidth();
	const int getHeight();
	
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
	
	void loadPCX(std::ifstream & ifile,bool islinked, bool useact, unsigned char palsave1[]);
	
	inline const unsigned long getNext() const { return next; }
	inline const unsigned long getLocation() const { return location; }
	inline const unsigned long getLength() const { return length; }
	inline const unsigned long getRealLength() const { return reallength; }
	inline const unsigned long getNewLength() const { return newlength; }
	inline const short getX() const { return x; }
	inline const short getY() const { return y; }
	inline const unsigned short getGroupNumber() const { return groupNumber; }
	inline const unsigned short getImageNumber() const { return imageNumber; }
	inline const unsigned short getPrevious() const { return prev; }
	inline const bool getSamePalette() const { return samePalette; }
	inline const char *getComments() const { return comments; }
	
    private:
	unsigned long next;
	unsigned long location;
	unsigned long length;
	unsigned long reallength;
	unsigned long newlength;
	short x;
	short y;
	unsigned short groupNumber;
	unsigned short imageNumber;
	unsigned short prev;
	bool samePalette;
	char comments[13];
	char * pcx;
	
	Bitmap *bitmap;
	
	void draw(const Bitmap &, const int xaxis, const int yaxis, const Bitmap &, const Effects &);
};

#endif
