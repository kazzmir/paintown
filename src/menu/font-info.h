#ifndef _paintown_font_info_h
#define _paintown_font_info_h

#include <string>
#include "util/file-system.h"
#include "util/font.h"

class Font;

namespace Menu{

/*
 * class FontInfo
 * class RelativeFontInfo
 * class AbsoluteFontInfo
 */

class AbsoluteFontInfo;
class RelativeFontInfo;

class FontInfo {
    public:
	FontInfo();
	virtual ~FontInfo();
	
        /*
	inline void set(const Filesystem::RelativePath & font, int width, int height){
	    this->font = font;
	    this->width = width;
	    this->height = height;
	}
        */
	
	virtual const Font & get() const = 0;
	virtual const Font & get(const Font & next) const = 0;
        virtual const FontInfo & get(const FontInfo & next) const = 0;

        virtual bool operator==(const FontInfo & who) const = 0;
        virtual bool operator==(const AbsoluteFontInfo & who) const = 0;
        virtual bool operator==(const RelativeFontInfo & who) const = 0;
	
        /*
	inline void setFont(const Filesystem::RelativePath & font){
	    this->font = font;
	}
        */

        virtual std::string getName() const = 0;

	// virtual const Filesystem::RelativePath getFont(const FontInfo & next) const;
	
        /*
	inline void setWidth(int width){
	    this->width = width;
	}
        */

	virtual const int getWidth(const FontInfo & next) const = 0;
	
        /*
	inline void setHeight(int height){
	    this->height = height;
	}
        */

	virtual const int getHeight(const FontInfo & next) const = 0;
	
	virtual const bool empty() const = 0;
    };

template <class Type>
class PathFontInfo: public FontInfo {
public:
    PathFontInfo(const Type & path, int width, int height):
    path(path),
    width(width),
    height(height){
    }

    virtual const Font & get() const {
        return Font::getFont(path, width, height);
    }
    
    virtual const Font & get(const Font & next) const {
        return get();
    }

    virtual const FontInfo & get(const FontInfo & next) const {
        return *this;
    }
        
    virtual std::string getName() const {
        return path.getFilename().path();
    }
	
    virtual const int getWidth(const FontInfo & next) const {
        return width;
    }
    
    virtual const int getHeight(const FontInfo & next) const {
        return height;
    }
	
    virtual const bool empty() const {
        return path.path().empty();
    }

protected:
    Type path;
    int width;
    int height;
};

class RelativeFontInfo: public PathFontInfo<Filesystem::RelativePath> {
public:
    RelativeFontInfo(const Filesystem::RelativePath & path, int width, int height):
    PathFontInfo<Filesystem::RelativePath>(path, width, height){
    }

    using FontInfo::operator==;
    virtual bool operator==(const FontInfo & who) const;
    virtual bool operator==(const AbsoluteFontInfo & who) const;
    virtual bool operator==(const RelativeFontInfo & who) const;
};

class AbsoluteFontInfo: public PathFontInfo<Filesystem::AbsolutePath> {
public:
    AbsoluteFontInfo(const Filesystem::AbsolutePath & path, int width, int height):
    PathFontInfo<Filesystem::AbsolutePath>(path, width, height){
    }
    
    using FontInfo::operator==;
    virtual bool operator==(const FontInfo & who) const;
    virtual bool operator==(const AbsoluteFontInfo & who) const;
    virtual bool operator==(const RelativeFontInfo & who) const;
};

/*
class RelativeFontInfo: public FontInfo {
public:
    RelativeFontInfo();
    RelativeFontInfo(const Filesystem::RelativePath & font, int width, int height);

    virtual const bool empty() const;

private:
    Filesystem::RelativePath font;
    int width;
    int height;
};

class AbsoluteFontInfo: public FontInfo {
public:
    AbsoluteFontInfo(const Filesystem::AbsolutePath & font, int width, int height);

    using FontInfo::get;
    virtual const Font & get() const;
    virtual const Font & get(const Font & next) const;
    virtual std::string getName() const;

protected:
    Filesystem::AbsolutePath font;
    int width;
    int height;
};
*/

}

#endif
