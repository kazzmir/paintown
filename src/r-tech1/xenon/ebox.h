/* ebox version 3:
 * by Jon Rafkind
 */

#ifndef _ebox_3_h
#define _ebox_3_h

#include "graphics/bitmap.h"
#include <vector>

class EQuad{
public:

	EQuad( EQuad * const head );
	EQuad( int w, int h, EQuad * _parent );
	EQuad( const Graphics::Bitmap * who, int min_size, Graphics::Color mask_pixel, int min_x, int min_y, EQuad * _parent );

	void draw( const Graphics::Bitmap & work, int x, int y, Graphics::Color color, bool flipped = false );
	void draw( const Graphics::Bitmap & work, int x, int y, Graphics::Color color, EQuad * who );

	inline int getWidth() const{
		return width;
	}

	inline int getHeight() const{
		return height;
	}

	inline void setFull( bool x ){
		full = x;
	}

	EQuad * getQuad( int x ) const;

	bool addQuad( EQuad * who );

	inline int getPosX() const {
		if ( parent )
			return min_x + parent->getPosX();
		return min_x;
	}

	inline int getPosY() const {
		if ( parent )
			return min_y + parent->getPosY();
		return min_y;
	}

	void gather( int mx, int my, int x1, int y1, int x2, int y2, std::vector< EQuad * > & collides, bool xflipped, bool yflipped );

	// bool collide( int mx, int my, int x1, int y1, int x2, int y2, EQuad ** last );
	bool collide( int mx, int my, int x1, int y1, int x2, int y2, EQuad ** last, bool xflipped = false, bool yflipped = false );
	// int collide( int mx, int my, int x1, int y1, int x2, int y2, EQuad ** last, int depth );
	
	int calcSize();

	void setMinX( int x );
	void setMinY( int y );
	
	int getFullX1( bool xflipped = false );
	int getFullY1( bool yflipped = false );

	inline int getMinX() const{
            return min_x;
	}

	inline int getMinY() const{
            return min_y;
	}

	int totalQuads();

	~EQuad();

protected:

	int getX1( bool xflipped = false );
	int getY1( bool yflipped = false );

	EQuad * getQuad() const;
	void detach( EQuad * const who );
	void checkQuad( EQuad *& q );

	inline int numQuads() const{
		int total = 0;
		if ( quads[0] != NULL ) total++;
		if ( quads[1] != NULL ) total++;
		if ( quads[2] != NULL ) total++;
		if ( quads[3] != NULL ) total++;
		// for ( int total = 0; total < 4 && quads[total] != NULL; total++ );
		return total;
	}
	
	bool empty();
	// bool boxCollide( int zx1, int zy1, int zx2, int zy2, int zx3, int zy3, int zx4, int zy4 );

protected:

	int width, height;
	// EQuad * quad1, * quad2, * quad3, * quad4;

	bool full;

	int min_x, min_y;
	EQuad * quads[ 4 ];
	EQuad * parent;
	int num_quads;

};

class ECollide{
public:
	ECollide( EQuad * const head );
	// ECollide( Graphics::BITMAP * who, int mask_pixel = Bitmap::MaskColor );
	ECollide( const Graphics::Bitmap * who, Graphics::Color mask_pixel = Graphics::MaskColor());
	ECollide( const Graphics::Bitmap & who, Graphics::Color mask_pixel = Graphics::MaskColor());
	ECollide( const ECollide & e );
	ECollide( const ECollide * e );
	ECollide( int width, int height );

	void draw( const Graphics::Bitmap & work, int x, int y, Graphics::Color color, bool flipped = false );
	void draw( const Graphics::Bitmap & work, int x, int y, Graphics::Color color, EQuad * who );

	bool Collision( ECollide * col, int mx, int my, int ax, int ay, bool my_xflipped = false, bool my_yflipped = false, bool him_xflipped = false, bool him_flipped = false );
	bool Collision( int mx, int my, int ax, int ay, bool xflipped = false, bool yflipped = false );
	bool Collision( int mx, int my, int x1, int y1, int x2, int y2, bool xflipped = false, bool yflipped = false );

	int getMinHeight();
	int getMaxHeight();
	int getMinWidth();
	int getMaxWidth();
	

	ECollide * copy();

	EQuad * getLast();

	EQuad * getHead() const{
		return head_quad;
	}

	void gather( int mx, int my, int x1, int y1, int x2, int y2, std::vector< EQuad * > & e, bool xflipped, bool yflipped );

	int calcSize();

	inline int getWidth() const{
		return head_quad->getWidth();
	}

	inline int getHeight() const{
		return head_quad->getHeight();
	}

	int numQuads() const;

	~ECollide();

private:
	void initECollide( const Graphics::Bitmap * who, Graphics::Color mask_pixel );
	void initQuad( EQuad * const head );


	static long long totalTime;

protected:

	bool collide( int mx, int my, int x1, int y1, int x2, int y2, bool xflipped = false, bool yflipped = false );
	bool collide( int mx, int my, int x1, int y1, int x2, int y2, EQuad ** last, bool xflipped, bool yflipped );
	// bool collide( int mx, int my, int x1, int y1 );

	EQuad * head_quad;
	EQuad * last_collide;

};

#endif
