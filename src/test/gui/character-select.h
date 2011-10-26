#ifndef character_select_h
#define character_select_h

#include <string>
#include <map>
#include <vector>
#include "util/file-system.h"
#include "util/gradient.h"
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

struct Profile{
    Window window;
    Util::ReferenceCount<Graphics::Bitmap> bitmap;
    static Gui::Animation::Depth depth;
};

class TextMessage{
public:
    TextMessage();
    TextMessage(const Token *);
    TextMessage(const TextMessage &);
    ~TextMessage();
    
    const TextMessage & operator=(const TextMessage &);
    
    void act();
    void draw(const Graphics::Bitmap &);
    
    inline void setReplaceMessage(const std::string & message){
        this->replace = message;
    }
    
    inline const Gui::Animation::Depth & getDepth() const {
        return this->depth;
    }
    
    inline int getProfileAssociation() const {
        return this->profileAssociation;
    }
    
private:
    /*! Message */
    std::string message;
    
    /*! Extra Replace message */
    std::string replace;
    
    /*! Location */
    int x, y;
    
    /*! colors */
    int low_r, low_g, low_b;
    int high_r, high_g, high_b;
    
    /*! Interpolate distance */
    int interpolateDistance;
    
    /*! Depth */
    Gui::Animation::Depth depth;
    
    /*! Font */
    Filesystem::AbsolutePath font;
    
    /*! Font width */
    int width;
    
    /*! Font height */
    int height;
    
    enum Justification{
        Left,
        Center,
        Right,
    };
    
    /*! Justification */
    Justification justification;
    
    /*! Profile association */
    int profileAssociation;
    
    /*! gradient */
    Effects::Gradient gradient;
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
    const std::string getName();
    
private:
    unsigned int index;
    const Util::ReferenceCount<Gui::SelectListInterface> parent;
    int r,g,b;
    int letter;
};

class MessageCollection{
public:
    MessageCollection(const Token *);
    ~MessageCollection();
    void act(Util::ReferenceCount<Gui::SelectListInterface> &);
    void draw(const Gui::Animation::Depth &, const Graphics::Bitmap &);
private:
    std::map<Gui::Animation::Depth, std::vector<Util::ReferenceCount<TextMessage> > > messages;
};

class CharacterSelect{
public:
    CharacterSelect();
    CharacterSelect(const std::string &);
    CharacterSelect(const Token *);
    virtual ~CharacterSelect();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual void nextMessages();
    virtual void previousMessages();
    
    virtual inline Util::ReferenceCount<Gui::SelectListInterface> & getList() {
        return this->list;
    }
    
protected:
    /*! Load */
    void load(const Token *);
    
    /*! Render List, Profiles and Messages by depth */
    void render(const Gui::Animation::Depth &, const Graphics::Bitmap &);
    
    /*! Name of Character Select Menu */
    std::string name;
    
    /*! Backgrounds (animations) */
    Gui::AnimationManager backgrounds;
    
    /*! List (Simple or Grid) */
    Util::ReferenceCount<Gui::SelectListInterface> list;
    
    /*! List Items */
    std::vector<Util::ReferenceCount<Gui::SelectItem> > items;
    
    /*! List Depth */
    Gui::Animation::Depth listDepth;
    
    /*! Populate the list automatically? */
    bool autoPopulate;
    
    /*! Directory in which to populate the list from */
    Filesystem::AbsolutePath populateFromDirectory;
    
    /*! List window */
    Window listWindow;
    
    /*! List window bitmap */
    Util::ReferenceCount<Graphics::Bitmap> listBitmap;
    
    /*! Profile Windows (dependant on cursor total) */
    std::vector<Util::ReferenceCount<Profile> > profiles;
    
    /*! Font */
    Filesystem::AbsolutePath font;
    
    /*! Font width */
    int fontWidth;
    
    /*! Font height */
    int fontHeight;
    
    /*! Messages */
    std::vector<Util::ReferenceCount<MessageCollection> > messages;
    
    /*! Current Messages */
    unsigned int currentMessages;
};

#endif