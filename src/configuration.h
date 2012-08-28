#ifndef _paintown_configuration_h
#define _paintown_configuration_h

#include <string>
#include <map>
#include "util/input/input.h"
#include "util/pointer.h"
#include "util/menu/font-info.h"

namespace Menu{
class FontInfo;
}

class Token;

class Configuration{
public:
    
    virtual ~Configuration();

    typedef Joystick::Key JoystickInput;

    static void setDefaultKeys(int x);

    static void loadConfigurations();
    static void saveConfiguration();

    enum PlayMode{
        Cooperative = 1,
        FreeForAll = 2,
    };

    static void setRight(int config, int i);
    static void setLeft(int config, int i);
    static void setUp(int config, int i);
    static void setDown(int config, int i);
    static void setAttack1(int config, int i);
    static void setAttack2(int config, int i);
    static void setAttack3(int config, int i);
    static void setAttack4(int config, int i);
    static void setAttack5(int config, int i);
    static void setAttack6(int config, int i);
    static void setJump(int config, int i);

    /* Config is the associated player */
    static int getRight(int config);
    static int getLeft(int config);
    static int getUp(int config);
    static int getDown(int config);
    static int getAttack1(int config);
    static int getAttack2(int config);
    static int getAttack3(int config);
    static int getAttack4(int config);
    static int getAttack5(int config);
    static int getAttack6(int config);
    static int getJump(int config);

    static void setJoystickRight(int config, JoystickInput i);
    static void setJoystickLeft(int config, JoystickInput i);
    static void setJoystickUp(int config, JoystickInput i);
    static void setJoystickDown(int config, JoystickInput i);
    static void setJoystickAttack1(int config, JoystickInput i);
    static void setJoystickAttack2(int config, JoystickInput i);
    static void setJoystickAttack3(int config, JoystickInput i);
    static void setJoystickAttack4(int config, JoystickInput i);
    static void setJoystickAttack5(int config, JoystickInput i);
    static void setJoystickAttack6(int config, JoystickInput i);
    static void setJoystickJump(int config, JoystickInput i);
    static void setJoystickStart(int config, JoystickInput i);
    static void setJoystickQuit(int config, JoystickInput i);

    static JoystickInput getJoystickRight(int config);
    static JoystickInput getJoystickLeft(int config);
    static JoystickInput getJoystickUp(int config);
    static JoystickInput getJoystickDown(int config);
    static JoystickInput getJoystickAttack1(int config);
    static JoystickInput getJoystickAttack2(int config);
    static JoystickInput getJoystickAttack3(int config);
    static JoystickInput getJoystickAttack4(int config);
    static JoystickInput getJoystickAttack5(int config);
    static JoystickInput getJoystickAttack6(int config);
    static JoystickInput getJoystickJump(int config);
    static JoystickInput getJoystickStart(int config);
    static JoystickInput getJoystickQuit(int config);

public:
    static double getGameSpeed();
    static void setGameSpeed(double s);
    static bool getInvincible();
    static void setInvincible(bool i);
    static bool getFullscreen();
    static void setFullscreen(bool f);
    static int getLives();
    static void setLives(int l);
    static int getSoundVolume();
    static void setSoundVolume(int volume);
    static int getMusicVolume();
    static void setMusicVolume(int volume);
    static int getNpcBuddies();
    static void setNpcBuddies(int i);
    static PlayMode getPlayMode();
    static void setPlayMode(PlayMode mode);
    static void setScreenWidth(int i);
    static int getScreenWidth();
    static void setScreenHeight(int i);
    static int getScreenHeight();

    static std::string getQualityFilter();
    static void setQualityFilter(const std::string & filter);

    static int getFps();
    static void setFps(int fps);

    static Util::ReferenceCount<Menu::FontInfo> getMenuFont();
    static void setMenuFont(const Util::ReferenceCount<Menu::FontInfo> & str);
    static bool hasMenuFont();
    static int getMenuFontWidth();
    static int getMenuFontHeight();
    static void setMenuFontWidth(int x);
    static void setMenuFontHeight(int x);

    /* directory of current game/mod */
    /*
    static std::string getCurrentGame();
    static void setCurrentGame(const std::string & str);
    */

    static std::string getLanguage();
    static void setLanguage(const std::string & str);

    static bool isJoystickEnabled();
    static void setJoystickEnabled(bool enabled);

    static void setProperty(const std::string & path, const std::string & value);
    static std::string getProperty(const std::string & path, const std::string & defaultValue);

    static void setProperty(const std::string & path, int value);
    static int getProperty(const std::string & path, int value);
    
    static void setProperty(const std::string & path, double value);
    static double getProperty(const std::string & path, double value);
    
    static void setProperty(const std::string & path, bool value);
    static bool getProperty(const std::string & path, bool value);

    static void setProperty(const std::string & path, Token * value);
    /* Might return NULL */
    static Token * getProperty(const std::string & path);

    static void disableSave();
    static void setSave(bool what);
    static bool getSave();
    
protected:
    Configuration();
    Configuration(const Configuration & config);

    Configuration & operator=(const Configuration & config);

    static int getKey(int config, const std::string & name, int defaultValue);
    static JoystickInput getJoystickKey(int config, const std::string & name, JoystickInput defaultValue);

    static void setKey(int config, const std::string & name, int value);
    static void setJoystickKey(int config, const std::string & name, const JoystickInput & what);

private:
    /* whether to save the configuration or not */
    static bool save;

    static Util::ReferenceCount<Menu::FontInfo> menuFont;

    static bool joystickEnabled;

    static Util::ReferenceCount<Token> data;
};

#endif
