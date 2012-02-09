#ifndef mugen_versus_h
#define mugen_versus_h

#include "mugen/background.h"
#include "mugen/util.h"
#include "mugen/sprite.h"
#include "mugen/font.h"

#include "util/pointer.h"
#include "util/gui/fadetool.h"

namespace Graphics{
class Bitmap;
}

namespace Mugen{
class VersusMenu{
public:
    VersusMenu(const Filesystem::AbsolutePath &, bool demoMode = false);
    virtual ~VersusMenu();
    
    virtual void init(const Mugen::ArcadeData::CharacterCollection &, const Mugen::ArcadeData::CharacterCollection &);
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual void cancel();
    
    virtual void skip();
    
    virtual bool isDone();
    
    //! Get Logic class
    PaintownUtil::ReferenceCount<PaintownUtil::Logic> getLogic(InputMap<Mugen::Keys> &, InputMap<Mugen::Keys> &);
    
    //! Get Draw class
    PaintownUtil::ReferenceCount<PaintownUtil::Draw> getDraw();
    
    //! Was canceled
    inline bool wasCanceled() const {
        return this->canceled;
    }
    
    //! Is demo?
    inline bool inDemoMode() const {
        return this->demoMode;
    }
    
protected:
    //! Draw portrait
    void drawPortrait(const Mugen::ArcadeData::CharacterCollection &, const Mugen::Effects &, int x, int y, int fontx, int fonty, const Mugen::FontInfo &, const Graphics::Bitmap &);
    //! Get font
    PaintownUtil::ReferenceCount<MugenFont> getFont(int index) const;
    //! Path
    const Filesystem::AbsolutePath & path;
    //! Sprites
    Mugen::SpriteMap sprites;
    //! Fonts
    std::vector< PaintownUtil::ReferenceCount<MugenFont> > fonts;
    //! Versus background
    PaintownUtil::ReferenceCount<Mugen::Background> background;
    //! Time to show screen
    int time, ticker;
    //! Fade tool
    Gui::FadeTool fader;
    //! Position for portraits
    int x1, x2, y1, y2;
    //! Effects
    Mugen::Effects effects1, effects2;
    //! Fonts
    int fontx1, fontx2, fonty1, fonty2;
    Mugen::FontInfo font1, font2;
    //! CharacterCollection
    Mugen::ArcadeData::CharacterCollection player1;
    Mugen::ArcadeData::CharacterCollection player2;
    //! Canceled?
    bool canceled;
    //! Is this a demo mode versus screen?
    bool demoMode;
};
}
#endif