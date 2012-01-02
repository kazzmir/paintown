#include <vector>

#include "mugen/sprite.h"

#include "util/pointer.h"
#include "util/gui/select-list.h"

namespace Mugen{
    
class Cell : public Gui::SelectItem {
public:
    Cell();
    virtual ~Cell();
    
    virtual void act();
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const;
    virtual bool isEmpty() const;
    
    virtual void setRandom(bool r);
    
    static void setStaticData(::Util::ReferenceCount<MugenSprite> background, ::Util::ReferenceCount<MugenSprite> randomIcon);
    
protected:
    static ::Util::ReferenceCount<MugenSprite> background;
    static ::Util::ReferenceCount<MugenSprite> randomIcon;
    
    bool empty;
    bool isRandom;
};

class CharacterSelect{
public:
    CharacterSelect();
    ~CharacterSelect();
};

}
