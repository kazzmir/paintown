#ifndef mugen_command_h
#define mugen_command_h

#include <string>

class MugenItemContent;

/*
Holds mugen commands, ie: player.cmd
*/
class MugenCommand{
public:
    MugenCommand();
    MugenCommand( const MugenCommand &copy );
    ~MugenCommand();
    
    std::string name;
    MugenItemContent *commandList;
    int time;
    int bufferTime;

};

#endif
