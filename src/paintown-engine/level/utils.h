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

    /* descriptive name of this set of levels */
    virtual const std::string & getName() const;
    virtual void setPlayerPath(const std::string & s);
    virtual void setName(const std::string & s);

    virtual ~LevelInfo();

protected:
    std::vector<std::string> levels;
    std::string playerPath;
    std::string name;
    std::string _loadingMessage;
    Filesystem::AbsolutePath _loadingBackground;
    const Graphics::Bitmap * background;

    int x, y;
};

// LevelInfo readLevels(const std::string & filename);
LevelInfo readLevel(const Token * level);

Loader::Info convert(const LevelInfo & info);

}

#endif
