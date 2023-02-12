#ifndef character_select_h
#define character_select_h

#include <string>
#include <map>
#include <vector>
#include "r-tech1/file-system.h"
#include "r-tech1/graphics/gradient.h"
#include "r-tech1/pointer.h"
#include "r-tech1/gui/animation.h"
#include "r-tech1/gui/select-list.h"

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
    Window():
        x(0), y(0), width(0), height(0){
        }

    Window(int x, int y, int width, int height):
        x(x), y(y), width(width), height(height){
        }

    int x, y, width, height;
};

struct Profile;

/*! Text Message */
class TextMessage{
public:
    TextMessage();
    TextMessage(const Token *);
    TextMessage(const TextMessage &);
    ~TextMessage();
    
    const TextMessage & operator=(const TextMessage &);
    
    void act();
    void draw(const Graphics::Bitmap &, const Font &);
    
    inline const std::string & getName() const {
        return this->name;
    }
    
    inline void setReplaceMessage(const std::string & message){
        this->replace = message;
    }
    
    inline const Gui::Animation::Depth & getDepth() const {
        return this->depth;
    }
    
    inline int getCursorAssociation() const {
        return this->cursorAssociation;
    }
    
private:
    /*! ID */
    std::string name;
    
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
    
    /*! Cursor association */
    int cursorAssociation;
    
    /*! gradient */
    Effects::Gradient gradient;
};

class CellData;

class MessageCollection{
public:
    MessageCollection(const Token *);
    ~MessageCollection();
    void act(Util::ReferenceCount<Gui::SelectListInterface> &);
    void draw(const Gui::Animation::Depth &, const Graphics::Bitmap &, const Font &);
    void setReplaceMessage(const std::string &, const std::string &);
    inline const std::string & getName() const {
        return this->name;
    }
private:
    std::string name;
    std::map<Gui::Animation::Depth, std::vector<Util::ReferenceCount<TextMessage> > > messages;
};

class HasMore;

class CharacterSelect{
public:
    CharacterSelect();
    CharacterSelect(const Filesystem::AbsolutePath &);
    CharacterSelect(const Token *);
    virtual ~CharacterSelect();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual void nextMessages();
    virtual void previousMessages();
    virtual void changeToMessages(const std::string &);
    virtual Util::ReferenceCount<MessageCollection> getMessages(const std::string &);

    virtual void moveUp(int cursor);
    virtual void moveDown(int cursor);
    virtual void moveLeft(int cursor);
    virtual void moveRight(int cursor);
    virtual void swap(int cursor);

    /* the current map of the player under the cursor */
    virtual int getRemap(int cursor);

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
    void initializeDefaults();

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
    
    /*! Profile Windows (dependant on cursor total) */
    std::vector<Util::ReferenceCount<Profile> > profiles;
    
    /*! Font */
    Filesystem::AbsolutePath font;
    
    /*! Font width */
    int fontWidth;
    
    /*! Font height */
    int fontHeight;
    
    /*! Messages */
    std::vector<std::string> messageOrder;
    std::map<std::string, Util::ReferenceCount<MessageCollection> > messages;
    
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
