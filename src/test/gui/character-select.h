#ifndef character_select_h
#define character_select_h

#include <string>
#include <map>
#include <vector>
#include "util/file-system.h"
#include "util/pointer.h"
#include "util/gui/animation.h"
#include "util/gui/select-list.h"

class Font;
class Token;
namespace Graphics{
    class Bitmap;
}

struct Window{
    int x, y, width, height;
};

class CharacterItem : public Gui::SelectItem {
public:
    CharacterItem(unsigned int index, const Util::ReferenceCount<Gui::SelectListInterface> parent);
    ~CharacterItem();
    void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const;
    void drawProfile(int width, int height, const Graphics::Bitmap &, const Font &) const;
    inline bool isEmpty() const {
        return false;
    }
protected:
    unsigned int index;
    const Util::ReferenceCount<Gui::SelectListInterface> parent;
};

class CharacterSelect{
public:
    CharacterSelect();
    CharacterSelect(const std::string &);
    CharacterSelect(const Token *);
    virtual ~CharacterSelect();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual inline Util::ReferenceCount<Gui::SelectListInterface> & getList() {
        return this->list;
    }
    
protected:
    /*! Load */
    void load(const Token *);
    
    /*! Render backgrounds by depth */
    void renderBackgrounds(const Gui::Animation::Depth &, const Graphics::Bitmap &);
    
    /*! Name of Character Select Menu */
    std::string name;
    
    /*! Backgrounds (animations) */
    Gui::AnimationManager backgrounds;
    
    /*! List (Simple or Grid) */
    Util::ReferenceCount<Gui::SelectListInterface> list;
    
    /*! List Items */
    std::vector<Util::ReferenceCount<Gui::SelectItem> > items;
    
    /*! Populate the list automatically? */
    bool autoPopulate;
    
    /*! Directory in which to populate the list from */
    Filesystem::AbsolutePath populateFromDirectory;
    
    /*! List window */
    Window listWindow;
    
    /*! List window bitmap */
    Util::ReferenceCount<Graphics::Bitmap> listBitmap;
    
    /*! Profile Window (dependant on cursor total) */
    std::vector<Window> profileWindow;
    
    /*! Profile window Bitmap */
    std::vector<Util::ReferenceCount<Graphics::Bitmap> > profileBitmaps;
    
    /*! Font */
    Filesystem::AbsolutePath font;
    
    /*! Font width */
    int fontWidth;
    
    /*! Font height */
    int fontHeight;
    
};

#endif