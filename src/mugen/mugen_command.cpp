#include "mugen_command.h"
#include "mugen_item_content.h"

MugenCommand::MugenCommand():
name(""),
commandList(0),
time(0),
bufferTime(0){
    //Nothing
}

MugenCommand::MugenCommand( const MugenCommand &copy ){
    this->name = copy.name;
    this->commandList = copy.commandList;
    this->time = copy.time;
    this->bufferTime = copy.bufferTime;
}

MugenCommand::~MugenCommand(){
    if( commandList ) delete commandList;
}

