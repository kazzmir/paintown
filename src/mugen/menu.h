#ifndef _mugen_menu_h
#define _mugen_menu_h

#include <vector>
#include <map>

#include "exception.h"
#include "util/gui/fadetool.h"
#include "util/gui/scroll-list.h"
#include "util/language-string.h"
#include "search.h"

/*
#include "menu/menu.h"
#include "menu/menu_option.h"
*/

#include "util.h"

/*
 * We already have a setup for menus, might as well as utilize it to accomodate mugen
 */

class MugenAnimation;
class MugenSection;

namespace Ast{
    class Section;
}

namespace Mugen{
    /*! ScrollAction options.h */
    class ScrollAction;
    
    class Font;
    class Sprite;
    class Background;
    class Character;
    class CharacterSelect;
    class Storyboard;
    
    /*! Mugen Menu */
    class Menu{
    public:
        Menu(const Filesystem::RelativePath &, Searcher &);
        //! For Sub menus
        Menu(const Menu &, const std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > &);
        
        virtual ~Menu();
        
        virtual void act();
        virtual void draw(const Graphics::Bitmap &);
    
        virtual void up();
        virtual void down();
        virtual void enter();
        virtual void cancel();
        virtual bool isDone();
        
    protected:
        //! Name of menu
        std::string name;
        
        //! List position
        int x;
        int y;
        
        //! List
        PaintownUtil::ReferenceCount<ScrollAction> list;
        
        //! List items
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > listItems;
        
        //! Background
        PaintownUtil::ReferenceCount<Background> background;
        
        //! Sound types
        enum Sounds{
            Move,
            Done,
            Cancel,
        };
        
        /*! Sprites */
        Mugen::SpriteMap sprites;
        
        //! Sound system
        Mugen::SoundSystem<Sounds> sounds;
        
        /*! Music files */
        std::string introMusic;
        std::string titleMusic;
        std::string selectMusic;
        std::string versusMusic;
        
        //! Fonts
        std::vector< PaintownUtil::ReferenceCount<Font> > fonts;
        
        //! Logo storyboard
        PaintownUtil::ReferenceCount<Mugen::Storyboard> logo;
        
        //! Intro storyboard
        PaintownUtil::ReferenceCount<Mugen::Storyboard> intro;
        
        //! Fade tool
        Gui::FadeTool fader;
        
        //! Fade enabled
        bool fadeEnabled;
        
        //! Menu state
        enum State{
            Intros,
            MainMenu,
            DemoMode,
        };
        
        //! Current menu state
        State state;
        
        //! Ticks (movement or action will reset ticks to 0)
        int ticks;
        
        //! Demo mode
        bool demoEnabled;
        
        //! Time till demo
        int startDemoTime;
        
        //! Demo cycles
        int demoCycles;
        
        //! Max cycles of demo before displaying next intro
        int nextIntroCycle;
        
        //! Searcher
        Searcher & searcher;
        
        //! is done?
        bool done;
    };

    /*! run the mugen menu */
    void run();
}

#endif
