#ifndef _rtech1_bitmap_h_
#define _rtech1_bitmap_h_

#include "../exceptions/exception.h"
#include "../pointer.h"
#include "../parameter.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

#ifdef USE_ALLEGRO
#include "allegro/bitmap.h"
#endif
#ifdef USE_SDL
#include "sdl/bitmap.h"
#endif
#ifdef USE_ALLEGRO5
#include "allegro5/bitmap.h"
struct ALLEGRO_SHADER;
#endif

#include "color.h"

#if !defined(USE_ALLEGRO) && !defined(USE_SDL) && !defined(USE_ALLEGRO5)
#error No backend specified. Define one of USE_ALLEGRO, USE_SDL, or USE_ALLEGRO5
#endif

namespace Storage{
    class File;
}

namespace Graphics{

class Bitmap;

class Color{
public:
    explicit Color(const INTERNAL_COLOR & color):
    color(color){
    }

    static INTERNAL_COLOR defaultColor();

    Color():
    color(defaultColor()){
    }

    Color & operator=(const Color & him){
        this->color = him.color;
        return *this;
    }

    bool operator==(const Color & him) const {
        return color == him.color;
    }

    bool operator!=(const Color & him) const {
        return !(*this == him);
    }

    bool operator<(const Color & him) const {
        return this->color < him.color;
    }

    INTERNAL_COLOR color;
};

enum ImageFormat{
    FormatPNG,
    FormatBMP,
    FormatJPG,
    FormatPCX,
    FormatTGA,
    FormatTIF,
    FormatXPM,
    FormatGIF,
    FormatUnknown
};

ImageFormat identifyImage(const unsigned char * data, int length);

class TranslucentBitmap;
class LitBitmap;
class Bitmap;

extern Util::Parameter<Bitmap*> screenParameter;

extern const int SPRITE_NORMAL;
extern const int SPRITE_LIT;
extern const int SPRITE_TRANS;

extern const int SPRITE_NO_FLIP;
extern const int SPRITE_V_FLIP;
extern const int SPRITE_H_FLIP;
        
/*
extern int SCALE_X;
extern int SCALE_Y;
*/

class BitmapException: public Exception::Base {
public:
    BitmapException(const std::string & file, int line, const std::string & reason):
    Base(file, line),
    reason(reason){
    }

    BitmapException(const BitmapException & copy):
    Base(copy),
    reason(copy.reason){
    }

    virtual void throwSelf() const {
        throw *this;
    }

    virtual ~BitmapException() throw () {
    }
    
protected:
    virtual const std::string getReason() const {
        return reason;
    }

    std::string reason;
};

/* create a color from components */
Color makeColor(int r, int g, int b);
Color darken(Color color, double factor);
void hsvToRGB( float h, float s, float v, int * r, int * g, int * b );

int setGfxModeText();
int setGfxModeFullscreen( int x, int y );
int setGfxModeWindowed( int x, int y );
/* Only call this method once, from init. After that call changeGraphicsMode */
int setGraphicsMode(int mode, int width, int height);
int changeGraphicsMode(int mode, int width, int height);

/* get color components */
int getRed(Color x);
int getBlue(Color x);
int getGreen(Color x);
int getAlpha(Color x);

Color MaskColor();

enum QualityFilter{
    NoFilter,
    HqxFilter,
    XbrFilter
};

QualityFilter qualityFilterName(const std::string & type);

/* FIXME: try to move the shader stuff to shader.h */
class Shader{
public:
    Shader();
    virtual ~Shader();

#ifdef USE_ALLEGRO5
    Shader(ALLEGRO_SHADER * shader);
    ALLEGRO_SHADER * getShader();
    ALLEGRO_SHADER * shader;
#endif
};

class ShaderManager{
public:
    ShaderManager();
    Util::ReferenceCount<Shader> getShader(const std::string & name, Util::ReferenceCount<Shader> (*create)());
    virtual ~ShaderManager();

protected:
    std::map<std::string, Util::ReferenceCount<Shader> > shaders;
};

extern Util::Parameter<Util::ReferenceCount<ShaderManager> > shaderManager;

#ifdef USE_ALLEGRO5
Util::ReferenceCount<Shader> create_shader(const std::string & version, const std::string & vertex, const std::string & pixel);
void setShaderSampler(ALLEGRO_SHADER * shader, const std::string & name, const Bitmap & texture, int unit);
void setShaderBool(ALLEGRO_SHADER * shader, const std::string & name, bool value);
void setShaderInt(ALLEGRO_SHADER * shader, const std::string & name, int value);
void setShaderFloat(ALLEGRO_SHADER * shader, const std::string & name, float value);
void setShaderVec4(ALLEGRO_SHADER * shader, const std::string & name, float v1, float v2, float v3, float v4);

std::string defaultVertexShader();
std::string defaultPixelShader();
#endif

class Bitmap{
private:
	
        /* these constructors don't really matter, get rid of them at some point */
        Bitmap( const Bitmap & copy, int sx, int sy, double accuracy );
	Bitmap( const char * load_file, int sx, int sy, double accuracy );
public:

        /* equivalent to a GPU shader */
        class Filter{
        public:
            virtual Color filter(Color pixel) const = 0;

            /* getShader should only return the Shader object and not set any uniforms/attributes */
            virtual Util::ReferenceCount<Shader> getShader() = 0;

            /* set the uniforms/attributes */
            virtual void setupShader(const Util::ReferenceCount<Shader> &) = 0;
            virtual ~Filter(){
            }
        };
        	
	/* default constructor makes 10x10 bitmap */
	Bitmap();
	Bitmap( int x, int y );
	Bitmap( const char * load_file );
        Bitmap(const char * data, int length);
	Bitmap( const std::string & load_file );
	Bitmap( const char * load_file, int sx, int sy );

        /* Load a bitmap from an abstract file */
        Bitmap(Storage::File & file);

        /* 4/24/2010: remove this at some point */
#ifdef USE_ALLEGRO
	explicit Bitmap( BITMAP * who, bool deep_copy = false );
#endif
#ifdef USE_SDL
	explicit Bitmap(SDL_Surface * who, bool deep_copy = false );
#endif
#ifdef USE_ALLEGRO5
	explicit Bitmap(ALLEGRO_BITMAP * who, bool deep_copy = false );
#endif
	Bitmap( const Bitmap & copy, bool deep_copy = false );
	Bitmap( const Bitmap & copy, int sx, int sy );
	Bitmap( const Bitmap & copy, int x, int y, int width, int height );
	virtual ~Bitmap();

        /* For opengl backends, creates a memory bitmap. For software renderers
         * just creates a regular bitmap.
         */
        static Bitmap createMemoryBitmap(int width, int height);

        virtual TranslucentBitmap translucent() const;
        /* will call transBlender() with the supplied values for you */
        virtual TranslucentBitmap translucent(int red, int green, int blue, int alpha) const;

        virtual Bitmap subBitmap(int x, int y, int width, int height);

        virtual LitBitmap lit() const;

	virtual void save( const std::string & str ) const;

	virtual Bitmap & operator=( const Bitmap & );

	virtual void load( const std::string & str );

	virtual int getWidth() const;
	virtual int getHeight() const;
        /* true if this bitmap doesn't contain an image of any sort. if so, don't
         * try to use this object!!
         */
        virtual bool isEmpty() const;

	/*
	inline const int getWidth() const{
		return getBitmap()->w;
	}

	inline const int getHeight() const{
		return getBitmap()->h;
	}
	*/

	/*
	inline const int getWidth() const{
		return my_bitmap->w;
	}

	inline const int getHeight() const{
		return my_bitmap->h;
	}
	*/

	void detach();

        /* replace all pixels that have value `original' with `replaced' */
        void replaceColor(const Color & original, const Color & replaced);
        void set8BitMaskColor(const Color & color);
        Color get8BitMaskColor();

        /* Blend between source pixel and destination pixel.
         * Source and dest should be 0-255.
         * source = 64, dest = 128
         * pixel = source_pixel * source / 255 + dest_pixel * dest / 255
         */
	static void alphaBlender(int source, int dest);
	static void transBlender( int r, int g, int b, int a );
	static void multiplyBlender( int r, int g, int b, int a );
	static void dissolveBlender( int r, int g, int b, int a );
	static void addBlender( int r, int g, int b, int a );
	static void burnBlender( int r, int g, int b, int a );
	static void colorBlender( int r, int g, int b, int a );
	static void differenceBlender( int r, int g, int b, int a );
	static void dodgeBlender( int r, int g, int b, int a );
	static void hueBlender( int r, int g, int b, int a );
	static void luminanceBlender( int r, int g, int b, int a );
	static void invertBlender( int r, int g, int b, int a );
	static void screenBlender( int r, int g, int b, int a );
        /* for testing */
	static void setFakeGraphicsMode(int width, int height);

        /* clean up any remaining state */
        static void shutdown();

	// static void drawingMode( int type );

        virtual Color blendColor(const Color & input) const;
        /* sets the blend state. try not to use these function unless
         * absolutely necessary.
         */
        virtual void startDrawing() const;
        virtual void endDrawing() const;

	void acquire();
	void release();

        void updateOnResize();

        /* Resizes the bitmap and stretches the old pixels to the new bitmap */
	void resize(const int width, const int height);

        /* Just resizes the bitmap without copying pixels */
        void updateSize(const int width, const int height);

        /* Makes this bitmap the same as the parameter.
         * Width/height is copied and then a Blit is called.
         */
        void copy(const Bitmap & him);

	void debugSelf() const;

        /* convert to a grey scale version */
        virtual Bitmap greyScale();

	virtual void triangle( int x1, int y1, int x2, int y2, int x3, int y3, Color color ) const;

        /* draws an equilateral triangle centered at (x,y) pointing at `angle'
         * where each side has `size' pixels using the color.
         */
        virtual void equilateralTriangle(int x, int y, int angle, int size, Color color) const;
	virtual void ellipse( int x, int y, int rx, int ry, Color color ) const;
	virtual void ellipseFill( int x, int y, int rx, int ry, Color color ) const;

        virtual void light(int x, int y, int width, int height, int start_y, int focus_alpha, int edge_alpha, Color focus_color, Color edge_color) const;
        virtual void applyTrans(const Color color) const;

	virtual void border( int min, int max, Color color ) const;
	virtual void rectangle( int x1, int y1, int x2, int y2, Color color ) const;
	virtual void rectangleFill( int x1, int y1, int x2, int y2, Color color ) const;
	virtual void circleFill( int x, int y, int radius, Color color ) const;
	virtual void circle( int x, int y, int radius, Color color ) const;
	virtual void circle(int x, int y, int radius, int thickness, Color color) const;
	virtual void line( const int x1, const int y1, const int x2, const int y2, const Color color ) const;
		
	virtual void floodfill( const int x, const int y, const Color color ) const;
	virtual void horizontalLine( const int x1, const int y, const int x2, const Color color ) const;
	virtual void hLine( const int x1, const int y, const int x2, const Color color ) const;
	virtual void vLine( const int y1, const int x, const int y2, const Color color ) const;
	virtual void polygon( const int * verts, const int nverts, const Color color ) const;
	virtual void arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const;
	virtual void arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const;

        /* x/y is in the center of the bitmap */
	virtual void drawCenter(const int x, const int y, const Bitmap & where) const;

	virtual void draw(const int x, const int y, const Bitmap & where) const;
	virtual void draw(const int x, const int y, Filter * filter, const Bitmap & where) const;
	virtual void draw(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, const Bitmap & where) const;
	virtual void draw(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, Filter * filter, const Bitmap & where) const;
	virtual void drawCharacter( const int x, const int y, const Color color, const int background, const Bitmap & where ) const;

        /* flip horizontally */
	virtual void drawHFlip(const int x, const int y, const Bitmap & where) const;
	virtual void drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const;
	virtual void drawHFlip(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, const Bitmap & where) const;
	virtual void drawHFlip(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, Filter * filter, const Bitmap & where) const;
        /* flip vertically */
	virtual void drawVFlip( const int x, const int y, const Bitmap & where ) const;
	virtual void drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;

        /* flip horizontal and vertical */
	virtual void drawHVFlip( const int x, const int y, const Bitmap & where ) const;
	virtual void drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
	// virtual void drawTrans( const int x, const int y, const Bitmap & where ) const;
	// virtual void drawTrans( const int x, const int y, Filter * filter, const Bitmap & where ) const;
	// virtual void drawTransHFlip( const int x, const int y, const Bitmap & where ) const;
	// virtual void drawTransHFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
	// virtual void drawTransVFlip( const int x, const int y, const Bitmap & where ) const;
	// virtual void drawTransVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
	// virtual void drawTransHVFlip( const int x, const int y, const Bitmap & where ) const;
	// virtual void drawTransHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
	virtual void drawMask( const int x, const int y, const Bitmap & where );
	virtual void drawStretched( const int x, const int y, const int new_width, const int new_height, const Bitmap & who ) const;
	virtual void drawStretched(const Bitmap & who) const;

        /* middle of the bitmap is at x, y */
	virtual void drawRotateCenter(const int x, const int y, const int angle, const Bitmap & where);
        /* upper left hand corner is at x, y*/
	virtual void drawRotate(const int x, const int y, const int angle, const Bitmap & where);
	virtual void drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const Bitmap & where );
	virtual void drawPivot( const int centerX, const int centerY, const int x, const int y, const int angle, const double scale, const Bitmap & where );

        /* Stretch using the hqx algorithm */
        virtual void StretchHqx(const Bitmap & where) const;
	virtual void StretchHqx(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const;
        
        /* Xbr is defined here:
         * http://board.byuu.org/viewtopic.php?f=10&t=2248
         */
        virtual void StretchXbr(const Bitmap & where) const;
	virtual void StretchXbr(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight) const;

        /* Just copies pixels (fast) */
	virtual void Stretch(const Bitmap & where) const;
	virtual void Stretch(const Bitmap & where, const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight ) const;
	virtual void StretchBy2( const Bitmap & where );
	virtual void StretchBy4( const Bitmap & where );

	virtual void Blit( const std::string & xpath ) const;
	virtual void Blit( const Bitmap & where ) const;
	virtual void Blit( const int x, const int y, const Bitmap & where ) const;
	virtual void Blit( const int mx, const int my, const int wx, const int wy, const Bitmap & where ) const;
	virtual void Blit( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const;
        virtual void BlitMasked( const int mx, const int my, const int width, const int height, const int wx, const int wy, const Bitmap & where ) const;
	virtual void BlitToScreen() const;
	virtual void BlitAreaToScreen(const int upper_left_x, const int upper_left_y) const;
	virtual void BlitToScreen(const int upper_left_x, const int upper_left_y) const;
        virtual void BlitFromScreen(const int x, const int y) const;

        virtual void roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const;
        virtual void roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const;

        virtual void drawShadow(Bitmap & where, int x, int y, int intensity, Color color, double scale, bool facingRight) const;

        /* Return a bitmap that has an aspect ratio the same as the given numbers */
        virtual Bitmap aspectRatio(int aspectWidth, int aspectHeight) const;

        /* returned a scaled version of this bitmap of the given dimensions */
        virtual Bitmap scaleTo(const int width, const int height) const;

        /* scale the image by the given ratios, i.e: a ratio of 1 means do nothing.
         * less than 1 will make it smaller and larger than 1 will make it larger.
         */
        virtual Bitmap scaleBy(const double widthRatio, const double heightRatio) const;

        /* try to call Global::getScreenWidth/Height() instead of these directly */
        static int getScreenWidth();
        static int getScreenHeight();

	virtual void fill(Color color) const;

	virtual void clear() const;
	
        inline void clearToMask() const{
		this->fill(MaskColor());
	}

	bool getError();

	inline const Util::ReferenceCount<BitmapData> & getData() const {
            return data;
	}
	
        inline Util::ReferenceCount<BitmapData> getData(){
            return data;
	}

        void setData(Util::ReferenceCount<BitmapData> data){
            this->data = data;
        }
	
	virtual void readLine( std::vector<Color> & vec, int y );
	Color getPixel( const int x, const int y ) const;

        /* lock video bitmap to memory */
        void lock() const;
        void lock(int x, int y, int width, int height) const;
        void unlock() const;

        /* true if the point is within the bounds of the bitmap */
        bool inRange(int x, int y) const;

        /* uses _putpixel16 underneath which ignores translucent behavior */
	void putPixel( int x, int y, Color col ) const;
        /* respects the current trans mode */
	virtual void putPixelNormal(int x, int y, Color col) const;

	/*
	inline int getPixel( int x, int y ) const{
		if ( x >= 0 && x < my_bitmap->w && y >= 0 && y <= my_bitmap->h )
			return _getpixel16( my_bitmap, x, y );
		return -1;
	}
	
	inline void putPixel( int x, int y, int col ) const{
		if ( x >= 0 && x < my_bitmap->w && y >= 0 && y <= my_bitmap->h )
			_putpixel16( my_bitmap, x, y, col );
	}
	*/
	
	void setClipRect( int x1, int y1, int x2, int y2 ) const;
	void getClipRect( int & x1, int & y1, int & x2, int & y2 ) const;

	inline const std::string & getPath() const{
		return path;
	}

        /* produce a temporary bitmap that is not guaranteed to be preserved
         * after your function returns. do *not* hold references to this bitmap
         * and if you make a temporary bitmap, do *not* call functions that
         * might also make temporary bitmaps.
         */
        /*
        static Bitmap temporaryBitmap(int w, int h);
        static Bitmap temporaryBitmap2(int w, int h);
        */

        /* call this method to delete all temporary bitmaps.
         * don't call this unless you know what you are doing
         */
        static void cleanupTemporaryBitmaps();

        static double getScale();

	
        /*
         * Convert color values between the HSV and RGB color spaces. The RGB values
         * range from 0 to 255, hue is from 0 to 360, and saturation and value are
         * from 0 to 1.
         */
        static void rgbToHSV(int r, int g, int b, float * h, float * s, float * v);

        /* convert cymk to rgb. values should be in the range 0-255 */
        static void cymkToRGB(int c, int y, int m, int k, int * r, int * g, int * b);

	/* Add two RGB16 colors together
         *  r = c1.r + c2.r
         *  g = c1.g + c2.g
         *  b = c1.b + c2.b
	 */
	static Color addColor( Color color1, Color color2 );

	/*
	inline static int makeColor( int r, int g, int b ){
		return makecol16( r, g, b );
	}
	*/

	// static const int MaskColor = MASK_COLOR_16;
        /*
	static const int MODE_TRANS;
	static const int MODE_SOLID;
        */

#ifdef USE_ALLEGRO5
        void convertToVideo();
#endif
protected:
#ifdef USE_ALLEGRO5
        virtual void draw(const int x, const int y, Filter * filter, const Bitmap & where, int flags) const;
#endif
        /* release a reference count, and possibly destroy data */
        // void releaseInternalBitmap();

        /* really destroy private data */
        // void destroyPrivateData();

        /*
        inline void setBitmap( BITMAP * bitmap ){
            if ( bitmap == NULL ){
                std::cout << "*FATAL* Setting null bitmap" << std::endl;
            }
            _my_bitmap = bitmap;
        }
        */

        void loadFromMemory(const char * data, int length);

        void internalLoadFile( const char * load_file );

        /* implementation specific data */
        Util::ReferenceCount<BitmapData> data;
        // int * own;
        bool mustResize;
        // bool own;
        bool error;
        std::string path;
        static Bitmap * temporary_bitmap;
        static Bitmap * temporary_bitmap2;
        Color bit8MaskColor;

        /* only used by allegro5 for now */
        int width, height;
};

/* 8-bit bitmap that supports a palette */
class PaletteBitmap: public Bitmap {
public:
    PaletteBitmap();
    virtual ~PaletteBitmap();
    using Bitmap::operator=;

protected:
    Color palette[256];
};

Bitmap memoryPCX(unsigned char * const data, const int length, const bool mask = true);

/* creates a bitmap that can be used as a buffer for the screen.
 * on opengl/allegro5 systems this will return the current backbuffer
 */
Bitmap * getScreenBuffer();

struct BlendPoint{
    BlendPoint(const Color & color, int length);
    Color color;
    int length;
};

void blend_palette(Color * pal, int mp, const Color & sc, const Color & ec);
std::vector<Color> blend_palette(const std::vector<BlendPoint> & in);

// bool sameColors(Color color1, Color color2);

class LitBitmap: public Bitmap {
public:
    LitBitmap( const Bitmap & b );
    LitBitmap();
    virtual ~LitBitmap();

    using Bitmap::draw;
    virtual void draw(const int x, const int y, const Bitmap & where) const;
    virtual void draw(const int x, const int y, Filter * filter, const Bitmap & where ) const;
    using Bitmap::drawHFlip;
    virtual void drawHFlip( const int x, const int y, const Bitmap & where ) const;
    virtual void drawHFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
    using Bitmap::drawVFlip;
    virtual void drawVFlip( const int x, const int y, const Bitmap & where ) const;
    virtual void drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
    using Bitmap::drawHVFlip;
    virtual void drawHVFlip( const int x, const int y, const Bitmap & where ) const;
    virtual void drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
    using Bitmap::operator=;

protected:
#ifdef USE_ALLEGRO5
    virtual void draw(const int x, const int y, Filter * filter, const Bitmap & where, int flags) const;
#endif
};

class StretchedBitmap: public Bitmap {
public:
    /* How to initialize the bitmap */
    enum Clear{
        /* Don't do anything */
        NoClear,
        /* Clear (to black) */
        FullClear,
        /* Fill with the masking color for software rendering and
         * do nothing for opengl
         */
        Mask
    };

    StretchedBitmap(int width, int height, const Bitmap & where, Clear = NoClear, QualityFilter filter = NoFilter);
    void finish();
    void start();
    virtual int getWidth() const;
    virtual int getHeight() const;
    using Bitmap::operator=;

    virtual double getScaleWidth() const;
    virtual double getScaleHeight() const;

protected:
    double width;
    double height;
    double scale_x, scale_y;
    const Bitmap & where;
    const QualityFilter filter;
    const Clear clearKind;
    Bitmap scaleToFilter;
};

class TranslatedBitmap: public Bitmap {
public:
    TranslatedBitmap(int x, int y, const Bitmap & where);
    using Bitmap::operator=;
    using Bitmap::BlitToScreen;
    virtual void BlitToScreen() const;
    virtual ~TranslatedBitmap();
public:
    int x, y;
};

class TranslucentBitmap: public Bitmap {
public:
    TranslucentBitmap(const Bitmap & b);
    TranslucentBitmap();
    virtual ~TranslucentBitmap();

    using Bitmap::operator=;
    virtual Color blendColor(const Color & color) const;
    virtual void startDrawing() const;
    virtual void endDrawing() const;

    virtual void putPixelNormal(int x, int y, Color col) const;
    virtual void rectangleFill(int x1, int y1, int x2, int y2, Color color) const;
    virtual void rectangle(int x1, int y1, int x2, int y2, Color color) const;
    virtual void fill(Color color) const;
    virtual void line( const int x1, const int y1, const int x2, const int y2, const Color color ) const;
    virtual void hLine( const int x1, const int y, const int x2, const Color color ) const;
    virtual void arc(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color) const;
    virtual void arcFilled(const int x, const int y, const double ang1, const double ang2, const int radius, const Color color ) const;
    virtual void roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const;
    virtual void roundRectFill(int radius, int x1, int y1, int x2, int y2, Graphics::Color color) const;
    virtual void circleFill( int x, int y, int radius, Color color ) const;
    virtual void ellipse( int x, int y, int rx, int ry, Color color ) const;
    virtual void ellipseFill( int x, int y, int rx, int ry, Color color ) const;

    using Bitmap::draw;
    virtual void draw(const int x, const int y, const Bitmap & where) const;
    virtual void draw(const int x, const int y, Filter * filter, const Bitmap & where) const;

    // virtual void draw(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, const Bitmap & where) const;
    // virtual void draw(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, Filter * filter, const Bitmap & where) const;

    using Bitmap::drawHFlip;
    virtual void drawHFlip(const int x, const int y, const Bitmap & where) const;
    virtual void drawHFlip(const int x, const int y, Filter * filter, const Bitmap & where) const;
    // virtual void drawHFlip(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, const Bitmap & where) const;
    // virtual void drawHFlip(const int x, const int y, const int startWidth, const int startHeight, const int width, const int height, Filter * filter, const Bitmap & where) const;
    virtual void drawVFlip( const int x, const int y, const Bitmap & where ) const;
    virtual void drawVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;
    virtual void drawHVFlip( const int x, const int y, const Bitmap & where ) const;
    virtual void drawHVFlip( const int x, const int y, Filter * filter, const Bitmap & where ) const;

protected:
#ifdef USE_ALLEGRO5
    virtual void draw(const int x, const int y, Filter * filter, const Bitmap & where, int flags) const;
#endif
};

/* Normal drawing operations but ultimately is drawn translucently onto the parent */
class SubTranslucentBitmap: public Bitmap {
public:
    /* How to initialize the bitmap */
    enum Clear{
        /* Don't do anything */
        NoClear,
        /* Clear (to black) */
        FullClear,
        /* Fill with the masking color for software rendering and
         * do nothing for opengl
         */
        Mask
    };

    SubTranslucentBitmap(const Bitmap & where, int x, int y, int width, int height, Clear = NoClear);
    using Bitmap::operator=;
    
    virtual void roundRect(int radius, int x1, int y1, int x2, int y2, Color color) const;
    virtual void roundRectFill(int radius, int x1, int y1, int x2, int y2, Color color) const;

    virtual void finish();
    virtual void start();

protected:
#ifdef USE_ALLEGRO5
    TranslucentBitmap translucent;
#else
    Bitmap translucent;
#endif
    const Clear clearKind;
};

}

#endif
