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
autoSearch(true){
    
    Filesystem::AbsolutePath baseDir = configFile.getDirectory();
    const Filesystem::AbsolutePath ourDefFile = Mugen::Util::fixFileName(baseDir, configFile.getFilename().path());
    
    if (ourDefFile.isEmpty()){
        throw MugenException("Cannot locate definition file for: " + configFile.path(), __FILE__, __LINE__);
    }
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(ourDefFile.path()));
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
                        simple.view() >> self.gameSpeed;
                    } else if (simple == "team.1vs2life"){
                        simple.view() >> self.team1vs2Life;
                    } else if (simple == "team.loseonko"){
                        simple.view() >> self.teamLoseOnKO;
                    } else if (simple == "motif"){
                        string out;
                        simple.view() >> out;
                        self.motif = Filesystem::RelativePath(out);
                    } 
                }  
            };

            OptionWalk walk(*this);
            section->walk(walk);
        }
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

Filesystem::RelativePath Data::getMotifDirectory(){
    // return Filesystem::find(getDirectory() + Util::getFileDir(getMotif()));
    return getDirectory().join(getMotif().getDirectory());
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
        return Storage::instance().find(getMotifDirectory().join(file.getFilename()));
    } catch (const Filesystem::NotFound & nf){
	return Storage::instance().find(getDataDirectory().join(file.getFilename()));
    }
}

void Data::setMotif(const Filesystem::RelativePath & motif){
    this->motif = motif;
}

const Filesystem::RelativePath & Data::getMotif(){
    return motif;
}

void Data::setDifficulty(int difficulty){
    this->difficulty = difficulty;
}

int Data::getDifficulty(){
    return difficulty;
}

void Data::setLife(int life){
    this->life = life;
}

int Data::getLife(){
    return life;
}

void Data::setTime(int time){
    this->time = time;
}

int Data::getTime(){ 
    return time;
}

void Data::setSpeed(int speed){
    this->speed = speed;
}

int Data::getSpeed(){ 
    return speed;
}

void Data::setTeam1vs2Life(int life){
    this->team1vs2Life = life;
}

int Data::getTeam1vs2Life(){
    return team1vs2Life;
}

void Data::setTeamLoseOnKO(bool lose){
    this->teamLoseOnKO = lose;
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

void Data::setGameSpeed(int speed){
    this->gameSpeed = speed;
}

int Data::getGameSpeed(){
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

bool Data::getAutoSearch(){
    return autoSearch;
}

void Data::setAutoSearch(bool search){
    autoSearch = search;
}