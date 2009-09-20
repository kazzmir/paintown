#ifndef _paintown_level_utils_h
#define _paintown_level_utils_h

#include <string>
#include <vector>

class Token;

namespace Level{

class LevelInfo{
public:
    LevelInfo();
    LevelInfo(const LevelInfo & info);

    LevelInfo & operator=(const LevelInfo & info);

    virtual void addLevel(const std::string & s);
    virtual const std::vector<std::string> & getLevels() const;
    virtual const std::string & loadingMessage() const;
    virtual const std::string & loadingBackground() const;
    virtual const std::string & getPlayerPath() const;

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
    std::string _loadingBackground;
};

// LevelInfo readLevels(const std::string & filename);
LevelInfo readLevel(Token * level);

}

#endif
