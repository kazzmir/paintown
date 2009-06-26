#ifndef _paintown_level_utils_h
#define _paintown_level_utils_h

#include <string>
#include <vector>

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

protected:
    std::vector<std::string> levels;
    std::string _loadingMessage;
    std::string _loadingBackground;
};

LevelInfo readLevels(const std::string & filename);

}

#endif
