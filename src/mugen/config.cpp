#include "mugen/config.h"

#include <list>

#include "mugen/mugen_util.h"
#include "mugen/mugen_exception.h"

#include "globals.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;
using namespace std;
using namespace Mugen;

//! Data holder
static Data * data = 0;

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

void Data::initialize(){
    if (data){
        return;
    }
    // Grab mugen.cfg
    data = new Data(Filesystem::find("mugen/data/mugen.cfg")); 
}

std::string Data::getMugenDirectory(){
    return Filesystem::find("mugen/");
}

std::string Data::getMugenDataDirectory(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return Filesystem::find(data->getMugenDirectory() + Util::getFileDir(data->getMotif()));
}

std::string Data::getMugenCharDirectory(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return Filesystem::find(data->getMugenDirectory() + "chars/");
}

std::string Data::getMugenFontDirectory(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return Filesystem::find(data->getMugenDirectory() + "font/");
}

std::string Data::getMugenStageDirectory(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return Filesystem::find(data->getMugenDirectory() + "stages/");
}

void Data::setMotif(const std::string & motif){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->motif = motif;
}

const std::string & Data::getMotif(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->motif;
}

void Data::setDifficulty(int difficulty){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->difficulty = difficulty;
}

int Data::getDifficulty(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->difficulty;
}

void Data::setLife(int life){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->life = life;
}

int Data::getLife(){
    if (!data){
        throw MugenException("Data not initialized!");
    } 
    return data->life;
}

void Data::setTime(int time){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->time = time;
}

int Data::getTime(){ 
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->time;
}

void Data::setSpeed(int speed){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->speed = speed;
}

int Data::getSpeed(){ 
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->speed;
}

void Data::setTeam1vs2Life(int life){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->team1vs2Life = life;
}

int Data::getTeam1vs2Life(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->team1vs2Life;
}

void Data::setTeamLoseOnKO(bool lose){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->teamLoseOnKO = lose;
}

bool Data::getTeamLoseOnKO(){
    if (!data){
        throw MugenException("Data not initialized!");
    } 
    return data->teamLoseOnKO;
}

const std::string & Data::getGameType(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->gameType;
}

double Data::getDefaultAttackLifeToPowerMultiplier(){ 
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->defaultAttackLifeToPowerMultiplier;
}

double Data::getDefaultGetHitLifeToPowerMultiplier(){ 
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->defaultGetHitLifeToPowerMultiplier;
}

double Data::getSuperTargetDefenceMultiplier(){ 
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->superTargetDefenceMultiplier;
}

void Data::setGameSpeed(int speed){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    data->gameSpeed = speed;
}

int Data::getGameSpeed(){
    if (!data){
        throw MugenException("Data not initialized!");
    }
    return data->gameSpeed;
}

bool Data::getDrawShadows(){
    if (!data){
        throw MugenException("Data not initialized!");
    }    
    return data->drawShadows;
}


