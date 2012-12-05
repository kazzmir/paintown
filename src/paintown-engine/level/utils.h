#ifndef _paintown_level_utils_h
#define _paintown_level_utils_h

#include <string>
#include <vector>
#include "util/file-system.h"
#include "util/loading.h"

class Token;
namespace Graphics{
class Bitmap;
}

namespace Level{

class LevelInfo{
public:
    LevelInfo();
    LevelInfo(const LevelInfo & info);

    LevelInfo & operator=(const LevelInfo & info);

    virtual void addLevel(const std::string & s);
    virtual const std::vector<std::string> & getLevels() const;
    virtual const std::string & loadingMessage() const;
    virtual const Filesystem::AbsolutePath & loadingBackground() const;
    virtual const std::string & getPlayerPath() const;
    virtual void setPosition(int x, int y);

    virtual inline int getPositionX() const {
        return x;
    }

    virtual inline int getPositionY() const {
        return y;
    }

    virtual inline void setBackground(const Graphics::Bitmap * background){
        this->background = background;
    }

    virtual inline const Graphics::Bitmap * getBackground() const {
        return this->background;
    }

    virtual void setLoadingMessage(const std::string & str);

    virtual void setIntro(const Filesystem::RelativePath & path);
    virtual void setEnding(const Filesystem::RelativePath & path);

    virtual void playIntro() const;
    virtual void playEnding() const;

    /* descriptive name of this set of levels */
    virtual const std::string & getName() const;
    virtual void setPlayerPath(const std::string & s);
    virtual void setName(const std::string & s);

    virtual ~LevelInfo();

protected:
    std::vector<std::string> levels;

    /* Path to a list of players */
    std::string playerPath;

    /* Name of the level set which appears in the menu */
    std::string name;
    std::string _loadingMessage;
    Filesystem::AbsolutePath _loadingBackground;

    /* Played before the first level, at the start of the game */
    Filesystem::RelativePath intro;

    /* Played after the last level, at the end of the game */
    Filesystem::RelativePath ending;
    const Graphics::Bitmap * background;

    /* Position of the loading message on the screen */
    int x, y;
};

// LevelInfo readLevels(const std::string & filename);
LevelInfo readLevel(const Token * level);

Loader::Info convert(const LevelInfo & info);

}

#endif
