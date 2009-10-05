#include "option_change_mod.h"
#include "option_level.h"
#include "util/keyboard.h"
#include "util/load_exception.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/funcs.h"
#include "game/mod.h"
#include "globals.h"
#include "configuration.h"

#include "menu/menu.h"

#include <iostream>

using namespace std;

OptionChangeMod::OptionChangeMod(Token *token) throw (LoadException):
MenuOption(token, Event){
	if ( *token != "change-mod" ){
		throw LoadException("Not a change mod");
	}

    setText("Change mod");
}

OptionChangeMod::~OptionChangeMod(){
	// Nothing
}

void OptionChangeMod::logic(){
}

static bool isModFile(const std::string & path){
    try{
        TokenReader reader(path);
        Global::debug(1) << "Checking for a mod in " << path << endl;
        if (*reader.readToken() == "game"){
            return true;
        }
    } catch (const TokenException & e){
        return false;
    }
    return false;
}

static vector<string> findMods(){
    vector<string> mods;

    vector<string> directories = Filesystem::findDirectories(".");
    for (vector<string>::iterator dir = directories.begin(); dir != directories.end(); dir++){
        string file = *dir + "/" + Filesystem::cleanse(*dir) + ".txt";
        if (isModFile(file)){
            mods.push_back(file);
        }
    }

    return mods;
}

static string modName(const std::string & path){
    try{
        TokenReader reader(path);
        Global::debug(1) << "Checking for a mod in " << path << endl;
        Token * name_token = reader.readToken()->findToken("game/name");
        if (name_token != NULL){
            string name;
            *name_token >> name;
            return name;
        }
        return Filesystem::cleanse(path);
    } catch (const TokenException & e){
        return Filesystem::cleanse(path);
    }
}

static void changeMod(const std::string & path){
    size_t slash = path.rfind('/');
    size_t txt = path.rfind(".txt");
    if (slash != string::npos && txt != string::npos){
        string name = path.substr(slash, path.length() - slash - 4);
        Configuration::setCurrentGame(name);
        Paintown::Mod::loadMod(name);
    }
}

void OptionChangeMod::run(bool &endGame) throw (ReturnException) {
    try{
        int select = 0;
        // Menu menu(Filesystem::find("menu/change-mod.txt"));
        // menu.addOption(new OptionLevel(0, &select, 0));
        // menu.setupOptions();
        Menu menu;
        menu.setParent(getParent());
        vector<string> mods = findMods();
        int index = 0;
        for (vector<string>::iterator it = mods.begin(); it != mods.end(); it++){
            // menu.addOption(new OptionLevel(0, &select, 0));
            OptionLevel *opt = new OptionLevel(0, &select, index);
            opt->setText(modName(*it));
            opt->setInfoText("Choose this mod");
            menu.addOption(opt);
            index += 1;
        }

        if (index == 0){
            Global::debug(0) << "No mods found!" << endl;
            return;
        }
        
        menu.load(Filesystem::find("menu/change-mod.txt"));
        menu.run();
        changeMod(mods[select]);
    } catch (const LoadException & le){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << le.getReason() << endl;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << e.getReason() << endl;
    }
}
