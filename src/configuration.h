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

    typedef Joystick::Key JoystickInput;

    static Configuration & config(int x);

    static void loadConfigurations();
    static void saveConfiguration();

    enum PlayMode{
        Cooperative = 1,
        FreeForAll = 2,
    };

    /* return a Keyboard::Key_X based on some PAIN_KEY_X */
    int getKey( Input::PaintownInput which, int facing ) const;
    JoystickInput getJoystickKey(Input::PaintownInput which, int facing) const; 

    void setRight(int i);
    void setLeft(int i);
    void setUp(int i);
    void setDown(int i);
    void setAttack1(int i);
    void setAttack2(int i);
    void setAttack3(int i);
    void setAttack4(int i);
    void setAttack5(int i);
    void setAttack6(int i);
    void setJump(int i);

    int getRight() const;
    int getLeft() const;
    int getUp() const;
    int getDown() const;
    int getAttack1() const;
    int getAttack2() const;
    int getAttack3() const;
    int getAttack4() const;
    int getAttack5() const;
    int getAttack6() const;
    int getJump() const;

    void setJoystickRight(JoystickInput i);
    void setJoystickLeft(JoystickInput i);
    void setJoystickUp(JoystickInput i);
    void setJoystickDown(JoystickInput i);
    void setJoystickAttack1(JoystickInput i);
    void setJoystickAttack2(JoystickInput i);
    void setJoystickAttack3(JoystickInput i);
    void setJoystickAttack4(JoystickInput i);
    void setJoystickAttack5(JoystickInput i);
    void setJoystickAttack6(JoystickInput i);
    void setJoystickJump(JoystickInput i);
    void setJoystickQuit(JoystickInput i);

    JoystickInput getJoystickRight() const;
    JoystickInput getJoystickLeft() const;
    JoystickInput getJoystickUp() const;
    JoystickInput getJoystickDown() const;
    JoystickInput getJoystickAttack1() const;
    JoystickInput getJoystickAttack2() const;
    JoystickInput getJoystickAttack3() const;
    JoystickInput getJoystickAttack4() const;
    JoystickInput getJoystickAttack5() const;
    JoystickInput getJoystickAttack6() const;
    JoystickInput getJoystickJump() const;
    JoystickInput getJoystickQuit() const;

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

    static std::string getMugenMotif();
    static void setMugenMotif(const std::string & motif);

    static Util::ReferenceCount<Menu::FontInfo> getMenuFont();
    static void setMenuFont(const Util::ReferenceCount<Menu::FontInfo> & str);
    static bool hasMenuFont();
    static int getMenuFontWidth();
    static int getMenuFontHeight();
    static void setMenuFontWidth(int x);
    static void setMenuFontHeight(int x);
    static std::string getCurrentGame();
    static void setCurrentGame(const std::string & str);
    static std::string getLanguage();
    static void setLanguage(const std::string & str);

    static bool isJoystickEnabled();
    static void setJoystickEnabled(bool enabled);

    static void setStringProperty(const std::string & path, const std::string & value);
    static std::string getStringProperty(const std::string & path, const std::string & defaultValue);

    static void disableSave();
    static void setSave(bool what);
    static bool getSave();

protected:
    Configuration();
    Configuration( const Configuration & config );
    // Configuration( const int right, const int left, const int up, const int down, const int attack1, const int attack2, const int attack3, const int jump );

    ~Configuration();

    Configuration & operator=( const Configuration & config );

    static Configuration defaultPlayer1Keys();
    static Configuration defaultPlayer2Keys();

    static Token * saveKeyboard( int num, Configuration * configuration );
    static Token * saveJoystick( int num, Configuration * configuration );

    static void setProperty(std::string name, std::string value);
    void setKey(int * key, int value);
    void setJoystickKey(JoystickInput & key, const JoystickInput & what);

private:
    /* keyboard */
    int right;
    int left;
    int up;
    int down;
    int attack1;
    int attack2;
    int attack3;
    int attack4;
    int attack5;
    int attack6;
    int jump;

    /* joystick */
    JoystickInput joystick_right;
    JoystickInput joystick_left;
    JoystickInput joystick_up;
    JoystickInput joystick_down;
    JoystickInput joystick_attack1;
    JoystickInput joystick_attack2;
    JoystickInput joystick_attack3;
    JoystickInput joystick_attack4;
    JoystickInput joystick_attack5;
    JoystickInput joystick_attack6;
    JoystickInput joystick_jump;
    JoystickInput joystick_quit;

private:
    /* whether to save the configuration or not */
    static bool save;

    //! Game speed
    static double gamespeed;

    //! Invincibility
    static bool invincible;

    //! Fullscreen
    static bool fullscreen;

    //! Lives
    static int lives;

    static int npc_buddies;
    static PlayMode play_mode;

    static int screen_width;
    static int screen_height;

    static Util::ReferenceCount<Menu::FontInfo> menuFont;
    static int menuFontWidth;
    static int menuFontHeight;

    static int soundVolume;
    static int musicVolume;

    static bool joystickEnabled;

    /* directory of current game/mod */
    static std::string currentGameDir;
    static std::map<std::string, std::string> properties;

    static std::string language;
    static std::string mugenMotif;
};

#endif
