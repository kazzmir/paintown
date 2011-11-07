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

namespace Util{
namespace Thread{
    class ThreadObject;
}}

namespace Paintown{
    class DisplayCharacterLoader;
}

struct Window{
    int x, y, width, height;
};

struct Profile{
    Window window;
    Util::ReferenceCount<Graphics::Bitmap> bitmap;
    bool facingRight;
    double scale;
    static Gui::Animation::Depth depth;
};

/*! Text Message */
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

/*! Cell Data shape and drawings */
class CellData{
public:
    CellData(const Token *);
    ~CellData();
    void act();
    void draw(int x, int y, int width, int height, const Graphics::Bitmap &) const;
    enum Shape{
        SQUARE,
        ROUND,
    };
    inline const std::string & getName() const {
        return this->name;
    }
private:
    /*! Name */
    std::string name;
    /*! Shape of the cell */
    Shape shape;
    /*! Radius if it is round */
    int radius;
    /*! cell colors */
    int r, g, b, alpha;
    /*! color fill */
    bool fill;
    /*! cell */
    Util::ReferenceCount<Gui::Animation> cell;
};

struct playerInfo;

class CharacterItem: public Gui::SelectItem {
public:
    CharacterItem(unsigned int index, const Util::ReferenceCount<Gui::SelectListInterface> parent, Util::ReferenceCount<playerInfo> player, const std::map<std::string, Util::ReferenceCount<CellData> > &);
    ~CharacterItem();
    void act();
    void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const;
    void drawProfile(const Profile &, const Graphics::Bitmap &, const Font &) const;
    inline bool isEmpty() const {
        return false;
    }
    const std::string getName();
    Util::ReferenceCount<playerInfo> getPlayer();
    
private:
    unsigned int index;
    const Util::ReferenceCount<Gui::SelectListInterface> parent;
    Util::ReferenceCount<playerInfo> player;
    const std::map<std::string, Util::ReferenceCount<CellData> > & cells;
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

class HasMore{
public:
    HasMore(const Token *);
    ~HasMore();
    void act();
    void draw(const Gui::Animation::Depth &, const Graphics::Bitmap &);
private:
    enum Direction{
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };
    /*! location */
    int x, y, width, height;
    /*! image */
    Util::ReferenceCount<Gui::Animation> image;
    /*! Direction */
    Direction direction;
    /*! Depth */
    Gui::Animation::Depth depth;
    /*! gradient */
    Effects::Gradient gradient;
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

    virtual void moveUp(int cursor);
    virtual void moveDown(int cursor);
    virtual void moveLeft(int cursor);
    virtual void moveRight(int cursor);
    virtual void swap(int cursor);

    Filesystem::AbsolutePath getCurrentSelectedPath(int cursor);
    
    enum Sounds{
        Nothing,
        /* for any movement */
        Movement,
        /* for specific directions */
        Up,
        Down,
        Left,
        Right,

        Select,
        Quit,
        Swap,
        Misc,
    };
    
    virtual void playSound(const Sounds &);
    
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
    
    /*! CellData Collections */
    std::map<std::string, Util::ReferenceCount<CellData> > cells;
    
    /*! DisplayLoader */
    Util::ReferenceCount<Paintown::DisplayCharacterLoader> loader;
    
    /*! Display thread */
    Util::ReferenceCount<Util::Thread::ThreadObject> loadingThread;
    
    /*! Has More Low */
    Util::ReferenceCount<HasMore> hasMoreLow;
    
    /*! Has More High */
    Util::ReferenceCount<HasMore> hasMoreHigh;
    
    /*! Sounds */
    std::map<Sounds, std::string> sounds;
};

#endif
