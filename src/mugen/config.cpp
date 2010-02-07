#include "mugen/config.h"

#include <list>

#include "mugen/util.h"
#include "mugen/exception.h"

#include "globals.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;
using namespace std;
using namespace Mugen;

Data *Data::data = 0;

Data::Data(const std::string & configFile):
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
firstRun(){
    
    std::string baseDir = Mugen::Util::getFileDir(configFile);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(configFile) );
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate fight definition file for: " + configFile );
    }
    
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;

    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
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
                    if (simple == "dificulty"){
			simple >> self.difficulty;
                    } else if (simple == "life"){
                        simple >> self.life;
                    } else if (simple == "time"){
                        simple >> self.time;
                    } else if (simple == "gamespeed"){
                        simple >> self.gameSpeed;
                    } else if (simple == "team.1vs2life"){
                        simple >> self.team1vs2Life;
                    } else if (simple == "team.loseonko"){
                        simple >> self.teamLoseOnKO;
                    } else if (simple == "motif"){
                        simple >> self.motif;
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
	data = new Data(Filesystem::find("mugen/data/mugen.cfg")); 
    }
    return *data;
}

std::string Data::getDirectory(){
    return Filesystem::find("mugen/");
}

std::string Data::getDataDirectory(){
    return Filesystem::find(getDirectory() + "data/");
}

std::string Data::getMotifDirectory(){
    return Filesystem::find(getDirectory() + Util::getFileDir(getMotif()));
}

std::string Data::getCharDirectory(){
    return Filesystem::find(getDirectory() + "chars/");
}

std::string Data::getFontDirectory(){
    return Filesystem::find(getDirectory() + "font/");
}

std::string Data::getStageDirectory(){
    return Filesystem::find(getDirectory() + "stages/");
}

std::string Data::getFileFromMotif(const std::string & file){
    try{
	return Filesystem::find(getMotifDirectory() + Util::stripDir(file));
    } catch (const Filesystem::NotFound & nf){
	return Filesystem::find(getDataDirectory() + Util::stripDir(file));
    }
}

void Data::setMotif(const std::string & motif){
    this->motif = motif;
}

const std::string & Data::getMotif(){
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
