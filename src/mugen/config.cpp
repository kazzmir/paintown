#include "config.h"

#include <list>

#include "util.h"
#include "exception.h"
#include "parse-cache.h"

#include "globals.h"
#include "util/debug.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;
using namespace std;
using namespace Mugen;

Data *Data::data = 0;

std::string searchToString(const Data::SearchType & search){
    switch (search){
        case Data::SelectDefAndAuto:
        default:
            return "selectdef-auto";
            break;
        case Data::SelectDef:
            return "selectdef";
            break;
        case Data::Auto:
            return "auto";
            break;
    }
}

Data::SearchType searchToEnum(const std::string & search){
    if (search == "selectdef-auto"){
        return Data::SelectDefAndAuto;
    } else if (search == "selectdef"){
        return Data::SelectDef;
    } else if (search == "auto"){
        return Data::Auto;
    }
    return Data::NoUse;
}

Data::Data(const Filesystem::AbsolutePath & configFile):
motif(""),
difficulty(),
life(),
time(),
speed(),
team1vs2Life(),
teamLoseOnKO(),
gameType(),
defaultAttackLifeToPowerMultiplier(),
defaultGetHitLifeToPowerMultiplier(),
superTargetDefenceMultiplier(),
gameSpeed(),
drawShadows(),
afterImageMax(),
layeredSpriteMax(),
explodMax(),
sysExplodMax(),
helperMax(),
playerProjectileMax(),
firstRun(),
search(SelectDefAndAuto){
    
    Filesystem::AbsolutePath baseDir = configFile.getDirectory();
    const Filesystem::AbsolutePath ourDefFile = Mugen::Util::fixFileName(baseDir, configFile.getFilename().path());
    
    if (ourDefFile.isEmpty()){
        throw MugenException("Cannot locate definition file for: " + configFile.path(), __FILE__, __LINE__);
    }
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile.path() + " in" + diff.printTime("") << endl;

    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        if (head == "Options"){
            class OptionWalk: public Ast::Walker{
            public:
                OptionWalk(Data & self):
                self(self){
                }
                Data & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "difficulty"){
                        simple.view() >> self.difficulty;
                    } else if (simple == "life"){
                        simple.view() >> self.life;
                    } else if (simple == "time"){
                        simple.view() >> self.time;
                    } else if (simple == "gamespeed"){
                        double speed;
                        simple.view() >> speed;
                        if (speed < 0){
                            speed = ((speed*0.1)+0.9)+0.1;
                        } else if (speed == 0){
                            speed = 1.0;
                        } else if (speed > 0){
                            speed = (speed*0.1)+1;
                        }
                        self.gameSpeed = speed;
                    } else if (simple == "team.1vs2life"){
                        simple.view() >> self.team1vs2Life;
                    } else if (simple == "team.loseonko"){
                        simple.view() >> self.teamLoseOnKO;
                    } else if (simple == "motif"){
                        string out;
                        simple.view() >> out;
                        /* FIXME: read the motif properly */
                        self.motif = Filesystem::AbsolutePath(out);
                    } 
                }  
            };

            OptionWalk walk(*this);
            section->walk(walk);
        }
    }
    // Now Load from the configuration file if not set then initialize them to the defaults in mugen.cfg
    try {
        *Mugen::Configuration::get("difficulty") >> difficulty;
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("difficulty", difficulty);
    }
    try {
        *Mugen::Configuration::get("life") >> life;
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("life", life);
    }
    try {
        *Mugen::Configuration::get("time") >> time;
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("time", time);
    }
    try {
        *Mugen::Configuration::get("game-speed") >> gameSpeed;
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("game-speed", gameSpeed);
    }
    try {
        *Mugen::Configuration::get("team1-vs-2-life") >> team1vs2Life;
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("team1-vs-2-life", team1vs2Life);
    }
    try {
        *Mugen::Configuration::get("team-lose-on-ko") >> teamLoseOnKO;
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("team-lose-on-ko", teamLoseOnKO);
    }

#if 0
    try {
        Token * out;
        *Mugen::Configuration::get("motif") >> out;
        /* FIXME: read motif properly */
        motif = Filesystem::AbsolutePath(out);
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("motif", motif.path());
    }
#endif
    setMotif(Util::loadMotif());

    try {
        string out;
        *Mugen::Configuration::get("search") >> out;
        search = searchToEnum(out);
    } catch (const ios_base::failure & ex){
        Mugen::Configuration::set("search", searchToString(Data::SelectDefAndAuto));
    }
}

Data::~Data(){
}

Data & Data::getInstance(){
    if (!data){
        // Grab mugen.cfg
	data = new Data(Storage::instance().find(Filesystem::RelativePath("mugen/data/mugen.cfg"))); 
    }
    return *data;
}
        
void Data::destroy(){
    if (data){
        delete data;
    }
    data = NULL;
}

Filesystem::RelativePath Data::getDirectory(){
    // return Filesystem::find("mugen/");
    return Filesystem::RelativePath("mugen/");
}

Filesystem::RelativePath Data::getDataDirectory(){
    // return Filesystem::find(getDirectory() + "data/");
    return getDirectory().join(Filesystem::RelativePath("data/"));
}

Filesystem::AbsolutePath Data::getMotifDirectory(){
    // return Filesystem::find(getDirectory() + Util::getFileDir(getMotif()));
    return getMotif().getDirectory();
}

Filesystem::RelativePath Data::getCharDirectory(){
    // return Filesystem::find(getDirectory() + "chars/");
    return getDirectory().join(Filesystem::RelativePath("chars/"));
}

Filesystem::RelativePath Data::getFontDirectory(){
    // return Filesystem::find(getDirectory() + "font/");
    return getDirectory().join(Filesystem::RelativePath("font/"));
}

Filesystem::RelativePath Data::getStageDirectory(){
    // return Filesystem::find(getDirectory() + "stages/");
    return getDirectory().join(Filesystem::RelativePath("stages/"));
}

Filesystem::AbsolutePath Data::getFileFromMotif(const Filesystem::RelativePath & file){
    try{
        return getMotifDirectory().join(file.getFilename());
    } catch (const Filesystem::NotFound & nf){
	return Storage::instance().find(getDataDirectory().join(file.getFilename()));
    }
}

void Data::setMotif(const Filesystem::AbsolutePath & motif){
    this->motif = motif;
    ::Util::ReferenceCount<Storage::File> opened = Storage::instance().open(motif);
    if (opened != NULL){
        Global::debug(1) << "Motif path '" << opened->location()->toString() << "'" << std::endl;
        Mugen::Configuration::set("motif", opened->location());
    }
}

const Filesystem::AbsolutePath & Data::getMotif(){
    return motif;
}

void Data::setDifficulty(int difficulty){
    this->difficulty = difficulty;
    Mugen::Configuration::set("difficulty", difficulty);
}

int Data::getDifficulty(){
    return difficulty;
}

void Data::setLife(int life){
    this->life = life;
    Mugen::Configuration::set("life", life);
}

int Data::getLife(){
    return life;
}

void Data::setTime(int time){
    this->time = time;
    Mugen::Configuration::set("time", time);
}

int Data::getTime(){ 
    return time;
}

void Data::setSpeed(int speed){
    this->speed = speed;
    Mugen::Configuration::set("speed", speed);
}

int Data::getSpeed(){ 
    return speed;
}

void Data::setTeam1vs2Life(int life){
    this->team1vs2Life = life;
    Mugen::Configuration::set("team1-vs-2-life", team1vs2Life);
}

int Data::getTeam1vs2Life(){
    return team1vs2Life;
}

void Data::setTeamLoseOnKO(bool lose){
    this->teamLoseOnKO = lose;
    Mugen::Configuration::set("team-lose-on-ko", teamLoseOnKO);
}

bool Data::getTeamLoseOnKO(){
    return teamLoseOnKO;
}

const std::string & Data::getGameType(){
    return gameType;
}

double Data::getDefaultAttackLifeToPowerMultiplier(){ 
    return defaultAttackLifeToPowerMultiplier;
}

double Data::getDefaultGetHitLifeToPowerMultiplier(){ 
    return defaultGetHitLifeToPowerMultiplier;
}

double Data::getSuperTargetDefenceMultiplier(){ 
    return superTargetDefenceMultiplier;
}

void Data::setGameSpeed(double speed){
    this->gameSpeed = speed;
    Mugen::Configuration::set("game-speed", speed);
}

double Data::getGameSpeed(){
    return gameSpeed;
}

bool Data::getDrawShadows(){
    return drawShadows;
}
        
Filesystem::RelativePath Data::cleanse(const Filesystem::RelativePath & path){
    string str = path.path();
    if (str.find(getDirectory().path()) == 0){
        str.erase(0, getDirectory().path().length());
    }
    return Filesystem::RelativePath(str);

}

bool Data::autoSearch(){
    return (search == SelectDefAndAuto || search == Auto);
}

const Data::SearchType & Data::getSearchType(){
    return search;
}

void Data::setSearchType(const Data::SearchType & s){
    search = s;
    Mugen::Configuration::set("search", searchToString(search));
}
