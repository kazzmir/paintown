#ifndef _paintown_mod_h
#define _paintown_mod_h

#include <string>
#include <vector>

#include "util/file-system.h"
#include "util/pointer.h"

namespace Level{
    class LevelInfo;
}

namespace Graphics{
class Bitmap;
}

class InputSource;

/* encapsulates a configuration for a game */
namespace Paintown{

class Mod{
public:
    Mod(const Util::ReferenceCount<Storage::File> & path);

    virtual const std::string getMenu();
    /* use the heap */
    virtual Graphics::Bitmap * createBitmap(const Filesystem::RelativePath & path);

    /* use the stack */
    virtual Graphics::Bitmap makeBitmap(const Filesystem::RelativePath & path);

    /* Play a cutscene when the mod starts up (either when the game starts or when
     * the mod is selected for the first time)
     */
    virtual void playIntro();

    virtual std::vector<Level::LevelInfo> getLevels();

    virtual std::string getName() const;

    virtual ~Mod();

    virtual Filesystem::AbsolutePath selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap, const InputSource & source);

    /* tries to find files in the mod path first and then the regular
     * places next.
     */
    virtual Filesystem::AbsolutePath find(const Filesystem::RelativePath & path);

    static void loadPaintownMod(const Filesystem::AbsolutePath & path);
    static void loadOpenborMod(const Filesystem::AbsolutePath & path);
    static void loadDefaultMod();
    static Mod * getCurrentMod();
    
protected:
    Mod();

    static void setMod(Mod * mod);

    std::vector<Level::LevelInfo> levels;
    std::string menu;
    std::string name;
    std::string intro;

    static Mod * currentMod;
};

}

#endif
