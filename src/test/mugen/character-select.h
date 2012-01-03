#include <vector>

#include "mugen/background.h"
#include "mugen/util.h"
#include "mugen/sprite.h"
#include "mugen/font.h"

#include "util/pointer.h"
#include "util/gui/select-list.h"
#include "util/gui/fadetool.h"

namespace Mugen{
    
class Cell : public Gui::SelectItem {
public:
    Cell(unsigned int index, const ::Util::ReferenceCount<Gui::SelectListInterface> parent);
    virtual ~Cell();
    
    virtual void act();
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const;
    virtual bool isEmpty() const;
    
    virtual void setRandom(bool r);
    
    virtual inline unsigned int getIndex() const {
        return index;
    }
    
    static void setBackground(::Util::ReferenceCount<MugenSprite> background);
    static void setRandomIcon(::Util::ReferenceCount<MugenSprite> randomIcon);
    
protected:
    static ::Util::ReferenceCount<MugenSprite> background;
    static ::Util::ReferenceCount<MugenSprite> randomIcon;
    
    unsigned int index;
    const ::Util::ReferenceCount<Gui::SelectListInterface> parent;
    bool empty;
    bool isRandom;
};

class CharacterSelect{
public:
    CharacterSelect(const Filesystem::AbsolutePath &);
    virtual ~CharacterSelect();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
protected:
    //! Grid
    Gui::GridSelect grid;
    //! Grid dimensions
    int gridX, gridY;
    //! Starting positions
    int player1Start, player2Start;
    //! Cells
    std::vector< ::Util::ReferenceCount<Cell> > cells;
    //! Sprites
    Mugen::SpriteMap sprites;
    //! Sounds
    Mugen::SoundMap sounds;
    //! Select file
    std::string selectFile;
    //! Fonts
    std::vector< ::Util::ReferenceCount<MugenFont> > fonts;
    //! Fade tool
    Gui::FadeTool fader;
    //! Select background
    ::Util::ReferenceCount<Mugen::Background> background;
};

}
