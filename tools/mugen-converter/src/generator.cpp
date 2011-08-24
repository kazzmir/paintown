#include "generator.h"
#include "controllers.h"
#include "triggers.h"

#include "ast/all.h"
#include "util/regex.h"

#include <ctime>
#include <exception>
#include <list>
#include <map>

using namespace Mugen;

std::string getCurrentDate(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    strftime (buffer,80,"%A - %m/%d/%Y - %I:%M%p.",timeinfo);
    
    return std::string(buffer);
}

/* Example on how to use the PythonStream */
void testStream(const std::string & file){
    Mugen::PythonStream out;
    out.open(file);
    out << "import mugen" << PythonStream::endl;
    out <<  "class TestCharacter(mugen.Character):" << PythonStream::endl;
        out << PythonStream::indent;
        out << "def __init__(self):" << PythonStream::endl;
            out << PythonStream::indent;
            out << "self.setName('TestCharacter')" << PythonStream::endl;
    out.clearIndents();
    out << "char = TestCharacter()" << PythonStream::endl;
    out << "print char.name" << PythonStream::endl;
    out.close();
}

/* Simplify use */
PythonStream::IndentType indent = PythonStream::indent;
PythonStream::IndentType unindent = PythonStream::unindent;
StreamEnd endl = PythonStream::endl;

CharacterGenerator::CharacterGenerator(const std::string & filename):
filename(filename),
directory(Mugen::stripFilename(filename)){
}

CharacterGenerator::~CharacterGenerator(){
}

void CharacterGenerator::output(const std::string & file){
    try {
        std::cout << "Preparing character definition file " << filename << " for output to file: " << file << std::endl;
        
        Mugen::PythonStream out;
        out.open(file);;
        
        out << "# File generated from original file '" << Mugen::stripDir(filename) << "' on " << getCurrentDate() << endl;
        out << endl;
        
        out << "import mugen" << endl;
        out << endl;
        // Create character class
        Mugen::Content classLine(0, "class " + Mugen::stripExtension(file) + "(mugen.Character):");
        Mugen::Content init(1,"");
        init.addLine(1,"# Construct");
        init.addLine(1,"def __init__(self):");
        init.addSpace();
        init.addLine(2,"# Initialize mugen.Character");
        init.addLine(2,"mugen.Character.__init__(self)");
        PythonClass character(classLine, init);
        handleBaseDef(character);
        handleCmdFile(character);
        handleStateFiles(character);
        
        // Initialize -3, -2, -1
        character.getInit().addSpace();
        Content negativeStates(0, "# Initialize states -3, -2, -1");
        negativeStates.addLine(0, "try:");
            negativeStates.addLine(1, "self.neg3State = self.getState(-3)(self, None)");
        negativeStates.addLine(0, "except mugen.MugenException:");
            negativeStates.addLine(1, "self.neg3State = mugen.StateDef(self, None)");
        negativeStates.addLine(0, "try:");
            negativeStates.addLine(1, "self.neg2State = self.getState(-2)(self, None)");
        negativeStates.addLine(0, "except mugen.MugenException:");
            negativeStates.addLine(1, "self.neg2State = mugen.StateDef(self, None)");
        negativeStates.addLine(0, "try:");
            negativeStates.addLine(1, "self.neg1State = self.getState(-1)(self, None)");
        negativeStates.addLine(0, "except mugen.MugenException:");
            negativeStates.addLine(1, "self.neg1State = mugen.StateDef(self, None)");
        character.getInit().addContent(negativeStates);
        
        character.output(out, 0);
        outputStateClasses(out);
        out.close();
    } catch (const Mugen::Cmd::ParseException & fail){
        std::cout << "Failed to parse " << filename << " because " << fail.getReason() << std::endl;
    } catch (...){
        std::cout << "Unknown Exception Caught!\nAborting..." << std::endl;
    }
}

void CharacterGenerator::addStateFile(const std::string & stateFile){
    // Don't add the same file twice
    for (std::vector<std::string>::iterator i = stateFiles.begin(); i != stateFiles.end(); ++i){
        if (stateFile == *i){
            // File already added, don't save
            return;
        }
    }
    stateFiles.push_back(stateFile);
}

void CharacterGenerator::handleBaseDef(PythonClass & character){
    class BaseWalker: public Ast::Walker {
        public:
            BaseWalker(CharacterGenerator & generator, PythonClass & character):
            generator(generator),
            character(character){
                
            }
            
            CharacterGenerator & generator;
            PythonClass & character;
            std::string sectionName;
            Content content;
            
            virtual void onSection(const Ast::Section & section){
                sectionName = section.getName();
                std::cout << "Parsing Section: " << sectionName << std::endl;
            }
            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "name"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.name = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "displayname"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.displayName = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "versiondate"){
                    try{
                        std::vector<int> numbers;
                        simple >> numbers;
                        std::stringstream out;
                        out << "self.versionDate = [";
                        unsigned int count = 0;
                        for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            out << *it << (count != numbers.size()-1 ? "," : "");
                            count++;
                        }
                        out << "]";
                        addComment(simple.getLine());
                        content.addLine(0,out.str());
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "mugenversion"){
                    try{
                        std::vector<int> numbers;
                        simple >> numbers;
                        std::stringstream out;
                        out << "self.mugenVersion = [";
                        unsigned int count = 0;
                        for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            out << *it << (count != numbers.size()-1 ? "," : "");
                            count++;
                        }
                        out << "]";
                        addComment(simple.getLine());
                        content.addLine(0, out.str());
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "author"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.author = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "pal.defaults"){
                        std::vector<int> numbers;
                        simple >> numbers;
                        std::stringstream out;
                        out << "self.paletteDefaults = [";
                        unsigned int count = 0;
                        for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            out << *it << (count != numbers.size()-1 ? "," : "");
                            count++;
                        }
                        out << "]";
                        addComment(simple.getLine());
                        content.addLine(0,out.str());
                } else if (simple == "cmd"){
                    try{
                        simple >> generator.commandFile;
                        addComment(simple.getLine());
                        content.addLine(0,"self.cmdFile = '" + generator.commandFile + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "cns"){
                    try{
                        generator.addStateFile(simple.valueAsString());
                        addComment(simple.getLine());
                        content.addLine(0,"self.addStateFile('" + generator.stateFiles.back() + "')");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "st"){
                    try{
                        generator.addStateFile(simple.valueAsString());
                        addComment(simple.getLine());
                        content.addLine(0,"self.addStateFile('" + generator.stateFiles.back() + "')");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "stcommon"){
                    try{
                        generator.addStateFile(simple.valueAsString());
                        addComment(simple.getLine());
                        content.addLine(0,"self.addStateFile('" + generator.stateFiles.back() + "')");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (Util::matchRegex(simple.idString(), "st[0-9]+")){
                    try{
                        generator.addStateFile(simple.valueAsString());
                        addComment(simple.getLine());
                        content.addLine(0,"self.addStateFile('" + generator.stateFiles.back() + "')");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "sprite"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.spriteFile = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "anim"){
                    try{
                        simple >> generator.airFile;
                        addComment(simple.getLine());
                        content.addLine(0,"self.animFile = '" + generator.airFile + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "sound"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.soundFile = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (Util::matchRegex(simple.idString(), "pal[0-9]+")){
                    std::string id = simple.idString();
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.paletteFiles[" + id.substr(3) + "] = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "intro.storyboard"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.introStoryboard = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "ending.storyboard"){
                    try{
                        addComment(simple.getLine());
                        content.addLine(0,"self.endingStoryboard = '" + simple.valueAsString() + "'");
                    } catch (const Ast::Exception & fail){
                    }
                } else {
                    std::cout << "Unhandled option in " << sectionName << " Section: " << simple.toString() << std::endl;
                }
            }
            
            virtual void onAttributeKeyword (const Ast::AttributeKeyword &simple){
                std::cout << "Attribute keyword: " << simple.toString() << std::endl;
            }
            
            virtual void onAttributeArray (const Ast::AttributeArray &simple){
                std::cout << "Attribute array: " << simple.toString() << std::endl;
            }

            virtual void onNumber (const Ast::Number &simple){
                std::cout << "number: " << simple.toString() << std::endl;
            }
            
            virtual void complete (){
                // Add content
                character.getInit().addContent(content);
            }
            
            virtual void addComment (int line){
                std::stringstream out;
                out << line;
                content.addSpace();
                content.addLine(0,"# [" + Mugen::stripDir(generator.filename) + "] Section - " + sectionName + " on line " + out.str());
            }
    };
    
    BaseWalker walker(*this, character);
    std::list<Ast::Section*> * sections = parseDef(filename);
    for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
        Ast::Section * section = *it;
        section->walk(walker);
        std::list<Ast::Attribute*>  attributes = section->getAttributes();
    }
    destroy(sections);
    walker.complete();
}

class StateException : public std::exception{
    virtual const char* what() const throw()
    {
        return "";
    }
};

class StateHandler{
    public:
        StateHandler(const std::string & section, int line):
        inDef(true),
        stateDef(Content(1,""), Content(1, "")),
        stateDefinition(Content(1,"")){
            if (Util::matchRegex(Mugen::lowercase(section), "statedef")){
                sectionName = section;
                std::string func = "state";
                if (Util::matchRegex(Mugen::lowercase(section), "-[0-9]+")){
                    // Found negative, it's either -3, -2 or -1 (but check and store anyways)
                    std::string num = Util::captureRegex(Mugen::lowercase(sectionName), "statedef -([0-9]+)", 0);
                    stateNumber = "-" + num;
                    func += "neg" + num;
                } else {
                    std::string num = Util::captureRegex(Mugen::lowercase(sectionName), "statedef ([0-9]+)", 0);
                    stateNumber = num;
                    func += num;
                }
                
                function = func;
                
                std::ostringstream sectionLine;
                sectionLine << line;
                definition = "def evaluate(self, world):";
                stateDefinition.addContent(Content(0, "# State File function for " + section + " on line " + sectionLine.str()));
                stateDefinition.addContent(Content(0, definition));
                
                initContent.addSpace();
                                
            } else throw StateException();
        }
        ~StateHandler(){
        }
        
        std::string sectionName;
        std::string currentSection;
        std::string definition;
        std::string function;
        std::string stateNumber;
        Content stateClassLine;
        Content stateInitLine;
        Content initContent;
        bool inDef;
        StateControllerStore stateControllers;
        
        void toggleSection(){
            inDef = !inDef;
        }
        
        void parseSection(const Ast::AttributeSimple & simple, const std::string & section){
            currentSection = section;
            // In Statedef
            if (inDef){
                handleDef(simple);
            } else {
                handleState(simple);
            }
        }
        
        const Content getInitEntry(){
            initContent.addLine(0, "self.addState(" + stateNumber + ", StateDef" + function.substr(5) + ")");
            return initContent;
        }
        
        void init(const std::string & file, int line){
                std::stringstream out;
                out << line;
                initContent.addLine(0,"# [" + Mugen::stripDir(file) + "] Section - " + sectionName + " on line " + out.str());
                
                
                /* Construct state class */
                stateClassLine.addSpace();
                stateClassLine.addLine(0, "# [" + Mugen::stripDir(file) + "] Section - " + sectionName + " on line " + out.str());
                stateClassLine.addLine(0, "class StateDef" + function.substr(5) + "(mugen.StateDef):");
                    stateInitLine.addLine(1, "def __init__(self, player, world):");
                        stateInitLine.addLine(2, "mugen.StateDef.__init__(self, player, world)");
        }

        void addStateClass(std::vector<PythonClass> & classes){
            // Do init line
            stateDef = PythonClass(stateClassLine, stateInitLine);
            stateControllers.addToDefinition(stateDefinition);
            stateDef.add(stateDefinition);
            classes.push_back(stateDef);
        }
        
    protected:
        
        PythonClass stateDef;
        PythonDefinition stateDefinition;
        
        void addDefConstants(const std::string & name, const Ast::AttributeSimple & simple){
            ExpressionBuilder builder = TriggerHandler::convert(*simple.getValue());
            Trigger trigger = Trigger(builder);
            const std::vector<Content> & functions = trigger.getFunctions();
            for (std::vector<Content>::const_iterator j = functions.begin(); j != functions.end(); ++j){
                stateInitLine.append(*j);
            }
            stateInitLine.addLine(2, "self." + name + " = " + trigger.getName());
        }
        
        void handleDef(const Ast::AttributeSimple & simple){
            if (simple == "type"){
                addDefConstants("stateType", simple);
            } else if (simple == "movetype"){
                addDefConstants("moveType", simple);
            } else if (simple == "physics"){
                addDefConstants("physics", simple);
            } else if (simple == "anim"){
                addDefConstants("anim", simple);
            } else if (simple == "velset"){
                addDefConstants("velset", simple);
            } else if (simple == "ctrl"){
                addDefConstants("ctrl", simple);
            } else if (simple == "poweradd"){
                addDefConstants("poweraff", simple);
            } else if (simple == "juggle"){
                addDefConstants("stateType", simple);
            } else if (simple == "facep2"){
                addDefConstants("facep2", simple);
            } else if (simple == "hitdefpersist"){
                addDefConstants("hitdefpersist", simple);
            } else if (simple == "movehitpersist"){
                addDefConstants("movehitpersist", simple);
            } else if (simple == "hitcounterpersist"){
                addDefConstants("hitcounterpersist", simple);
            } else if (simple == "sprpriority"){
                addDefConstants("sprpriority", simple);
            } else {
                std::cout << "Unhandled option in [" << currentSection << "] Section: " << simple.toString() << std::endl;
            }
        }
        
        void handleState(const Ast::AttributeSimple & simple){
            if (simple == "triggerall"){
                stateControllers.getCurrentController().addTriggerall((Ast::AttributeSimple *)simple.copy());
            } else if (Util::matchRegex(simple.idString(), "trigger[0-9]+")){
                int number = atoi(Util::captureRegex(simple.idString(), "trigger([0-9]+)",0).c_str());
                stateControllers.getCurrentController().addTrigger(number, (Ast::AttributeSimple *)simple.copy());
            } else if (Util::matchRegex(simple.idString(), "var([0-9]+)")){
                stateControllers.getCurrentController().addVar((Ast::AttributeSimple *)simple.copy());
            } else {
                std::string id = simple.idString();
                stateControllers.getCurrentController().add(id, (Ast::AttributeSimple *)simple.copy());
            }
        }
};

class StateCollection{
    public:
        StateCollection(const std::string & filename):
        filename(filename){
        }
        ~StateCollection(){
            for (std::vector<StateHandler *>::iterator i = states.begin(); i != states.end(); ++i){
                if (*i){
                    delete *i;
                }
            }
        }
        void add(const Ast::Section & section){
            if (Util::matchRegex(Mugen::lowercase(section.getName()), "statedef")){
                // Create state handler
                try {
                    StateHandler * state = new StateHandler(section.getName(), section.getLine());
                    state->init(filename, section.getLine());
                    states.push_back(state);
                    stateDefDeclared[state->stateNumber] = true;
                } catch (const StateException & fail){
                }
            } else if (Util::matchRegex(Mugen::lowercase(section.getName()), "state ")){
                if (states.empty() || !(stateDefDeclared.find(states.back()->stateNumber) != stateDefDeclared.end())){
                    throw StateException();
                }
                if (states.back()->inDef){
                    states.back()->toggleSection();
                }
                // Create new controller
                states.back()->stateControllers.newController();
            }
        }
        
        void parseState(const std::string file, const Ast::AttributeSimple & simple, const std::string & section){
            if (!states.empty() && (stateDefDeclared.find(states.back()->stateNumber) != stateDefDeclared.end())){
                states.back()->parseSection(simple, section);
            } else {
                throw StateException();
            }
        }
        
        void addToClass(PythonClass & cl){
            for (std::vector<StateHandler *>::iterator i = states.begin(); i != states.end(); ++i){
                if (*i){
                    // Adding state defs to list
                    cl.getInit().addContent((*i)->getInitEntry());
                }
            }
        }
        
        void addStateClasses(std::vector<PythonClass> & classes){
            // State Classes
            for (std::vector<StateHandler *>::iterator i = states.begin(); i != states.end(); ++i){
                if (*i){
                    // Add state definition class
                    (*i)->addStateClass(classes);
                }
            }
        }
        
    private:
        const std::string & filename;
        std::vector<StateHandler *> states;
        std::map<std::string, bool> stateDefDeclared;
        
};

void CharacterGenerator::handleCmdFile(PythonClass & character){
    
    class Command{
        public:
            Command(){
            }
            std::string name;
            std::string command;
            std::string time;
            std::string bufferTime;
            
            std::string get(){
                return "self.addCommand('" + name + "', " + (command.empty() ? "''" : "[" + command + "]") + ", " 
                                            + (time.empty() ? "0" : time) + ", " + (bufferTime.empty() ? "0" : bufferTime) + ")";
            }
    };
        
    class CmdWalker: public Ast::Walker {
        public:
            CmdWalker(CharacterGenerator & generator, PythonClass & character, const std::string & filename ):
            generator(generator),
            character(character),
            currentCommand(NULL),
            states(filename){
                
            }
            
            ~CmdWalker(){
                if (currentCommand != NULL){
                    delete currentCommand;
                }
            }
            
            CharacterGenerator & generator;
            PythonClass & character;
            std::string sectionName;
            
            Content constantsContent;
            
            Command * currentCommand;
            Content commandContent;
            
            StateCollection states;
            
            virtual void onSection(const Ast::Section & section){
                sectionName = section.getName();
                
                if (Mugen::lowercase(sectionName) == "command"){
                    // Create new command class to put away and store the previous one
                    if (currentCommand == NULL){
                        currentCommand = new Command();
                    } else if (currentCommand != NULL){
                        // Add command
                        commandContent.addLine(0,currentCommand->get());
                        delete currentCommand;
                        currentCommand = new Command();
                    }
                } else {
                    try {
                        states.add(section);
                    } catch (const StateException & fail){
                        std::cout << "Tried to add a state [" << section.getName() << "] without a statedef at: " << section.getLine() << "." << std::endl;
                    } 
                }
                std::cout << "Parsing Section: " << section.getName() << std::endl;   
            }
            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (!Util::matchRegex(Mugen::lowercase(sectionName), "state")){
                    if (simple == "command.time"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.commandTime = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "command.buffer.time"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.commandBufferTime = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "name"){
                        if (currentCommand != NULL){
                            addCommandComment(simple.getLine());
                            currentCommand->name = simple.valueAsString();
                        }
                    } else if (simple == "command"){
                        if (currentCommand != NULL){
                            std::string command;
                            try {
                                class KeyWalker : public Ast::Walker{
                                public:
                                    KeyWalker(std::string & command):
                                    command(command){
                                    }
                                    std::string & command;
                                    virtual void onKeySingle(const Ast::KeySingle & key){
                                        command+= "'" + key.toString() + "',";
                                    }
                                    virtual void onKeyModifier(const Ast::KeyModifier & key){
                                        command+= "'" + key.toString() + "',";
                                    }
                                    virtual void onKeyCombined(const Ast::KeyCombined & key){
                                        command+= "'" + key.toString() + "',";
                                    }
                                };
                                KeyWalker walker(command);
                                Ast::KeyList * key = (Ast::KeyList*) simple.getValue()->copy();
                                const std::vector<Ast::Key*> & keys = key->getKeys();
                                for (std::vector<Ast::Key*>::const_iterator it = keys.begin(); it != keys.end(); ++it){
                                    Ast::Key * ourKey = *it;
                                    ourKey->walk(walker);
                                }
                                delete key;
                            } catch (Ast::Exception &ex){
                            }
                            currentCommand->command = command.substr(0,command.size()-1);
                        }
                    } else if (simple == "time"){
                        //std::cout << TriggerHandler::convert(*simple.getValue()).get() << std::endl;
                        if (currentCommand != NULL){
                            currentCommand->time = simple.valueAsString();
                        }
                    } else if (simple == "buffer.time"){
                        //std::cout << TriggerHandler::convert(*simple.getValue()).get() << std::endl;
                        if (currentCommand != NULL){
                            currentCommand->bufferTime = simple.valueAsString();
                        }
                    } else {
                        std::cout << "Unhandled option in " << sectionName << " Section: " << simple.toString() << std::endl;
                    }
                } else {
                    try {
                        states.parseState(generator.commandFile, simple, sectionName);
                    } catch (const StateException & fail){
                        std::cout << "Ignoring option in [" << sectionName << "] Section: " << simple.toString() << std::endl;
                    }
                }
            }
            
            virtual void onAttributeKeyword (const Ast::AttributeKeyword &simple){
                std::cout << "Attribute keyword: " << simple.toString() << std::endl;
            }
            
            virtual void onAttributeArray (const Ast::AttributeArray &simple){
                std::cout << "Attribute array: " << simple.toString() << std::endl;
            }

            virtual void onNumber (const Ast::Number &simple){
                std::cout << "number: " << simple.toString() << std::endl;
            }
            
            virtual void addConstantsComment (int line){
                std::stringstream out;
                out << line;
                constantsContent.addSpace();
                constantsContent.addLine(0,"# [" + Mugen::stripDir(generator.commandFile) + "] Section - " + sectionName + " on line " + out.str());
            }
            
            virtual void addCommandComment (int line){
                std::stringstream out;
                out << line;
                commandContent.addSpace();
                commandContent.addLine(0,"# [" + Mugen::stripDir(generator.commandFile) + "] Section - " + sectionName + " on line " + out.str());
            }
            
            virtual void complete (std::vector<PythonClass> & classes){
                // Add constants content
                character.getInit().addContent(constantsContent);
                
                // Add command content
                commandContent.addLine(0,currentCommand->get());
                character.getInit().addContent(commandContent);
                
                // Do states
                states.addToClass(character);
                states.addStateClasses(classes);
            }
    };
    
    // Command Walker
    CmdWalker cmd(*this, character, commandFile);
    std::list<Ast::Section*> * sections = parseCmd(directory + commandFile);
    for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); ++it){
        Ast::Section * section = *it;
        section->walk(cmd);
        std::list<Ast::Attribute*>  attributes = section->getAttributes();
    }
    destroy(sections);
    cmd.complete(stateClasses);
}

/* Handle cns files
 * TODO need to add statedef/state information into class */
void CharacterGenerator::handleStateFiles(PythonClass & character){
    class StateWalker: public Ast::Walker {
        public:
            StateWalker(CharacterGenerator & generator, PythonClass & character, const std::string & currentFile ):
            generator(generator),
            character(character),
            currentFile(currentFile),
            states(currentFile){
                
            }
            
            CharacterGenerator & generator;
            PythonClass & character;
            const std::string & currentFile;
            std::string sectionName;
            
            Content constantsContent;
            
            StateCollection states;
            
            virtual void onSection(const Ast::Section & section){
                sectionName = section.getName();
                
                try {
                    states.add(section);
                } catch (const StateException & fail){
                    std::cout << "Tried to add a state [" << section.getName() << "] without a statedef at: " << section.getLine() << "." << std::endl;
                }
                std::cout << "Parsing Section: " << section.getName() << std::endl;   
            }
            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (!Util::matchRegex(Mugen::lowercase(sectionName), "state")){
                    if (simple == "life"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.life = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "attack"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.attack = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "defence"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.defence = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "fall.defence_up"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.fallDefenceUp = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "liedown.time"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.liedownTime = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "airjuggle"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.airjuggle = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "sparkno" && !Util::matchRegex(sectionName, "State")){
                        // Do not include state
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.sparkno = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "guard.sparkno"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.guardSparkno = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "KO.echo"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.koEcho = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "volume" && !Util::matchRegex(sectionName, "State")){
                        // Do not include state
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.volume = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "IntPersistIndex"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.intPersistIndex = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "FloatPersistIndex"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.floatPersistIndex = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "xscale"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.xscale = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "yscale"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.yscale = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "ground.back"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.groundBack = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "ground.front"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.groundFront = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "air.back"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.airBack = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "air.front"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.airFront = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "height"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.height = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "attack.dist"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.attackDist = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "proj.attack.dist"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.projAttackDist = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "proj.doscale"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.projDoscale = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "head.pos"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.headPos.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.headPos.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "mid.pos"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.midPos.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.midPos.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "shadowoffset"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.shadowoffset = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "draw.offset"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.drawOffset.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.drawOffset.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "walk.fwd"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.walkFwd.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.walkFwd.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "walk.back"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.walkBack.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.walkBack.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "run.fwd"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.runFwd.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.runFwd.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "run.back"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.runBack.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.runBack.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "jump.neu"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.jumpNeu.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.jumpNeu.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "jump.back"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.jumpBack.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.jumpBack.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "jump.fwd"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.jumpFwd.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.jumpFwd.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "runjump.back"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.runjumpBack.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.runjumpBack.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "runjump.fwd"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.runjumpFwd.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.runjumpFwd.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "airjump.neu"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.airjumpNeu.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.airjumpNeu.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "airjump.back"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.airjumpBack.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.airjumpBack.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "airjump.fwd"){
                        int x=0, y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & fail){
                        }
                        std::stringstream out;
                        addConstantsComment(simple.getLine());
                        out << "self.airjumpFwd.append(" << x << ")";
                        constantsContent.addLine(0,out.str());
                        out.str("");
                        out << "self.airjumpFwd.append(" << y << ")";
                        constantsContent.addLine(0,out.str());
                    } else if (simple == "airjump.num"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.airjumpNum = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "airjump.height"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.airjumpHeight = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "yaccel" && !Util::matchRegex(sectionName, "State")){
                        // Do not include state
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.yaccel = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "stand.friction"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.standFriction = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else if (simple == "crouch.friction"){
                        try{
                            addConstantsComment(simple.getLine());
                            constantsContent.addLine(0,"self.crouchFriction = " + simple.valueAsString());
                        } catch (const Ast::Exception & fail){
                        }
                    } else {
                        std::cout << "Unhandled option in " << sectionName << " Section: " << simple.toString() << std::endl;
                    }
                } else {
                    try {
                        states.parseState(currentFile, simple, sectionName);
                    } catch (const StateException & fail){
                        std::cout << "Ignoring option in [" << sectionName << "] Section: " << simple.toString() << std::endl;
                    }
                }
            }
            
            virtual void onAttributeKeyword (const Ast::AttributeKeyword &simple){
                std::cout << "Attribute keyword: " << simple.toString() << std::endl;
            }
            
            virtual void onAttributeArray (const Ast::AttributeArray &simple){
                std::cout << "Attribute array: " << simple.toString() << std::endl;
            }

            virtual void onNumber (const Ast::Number &simple){
                std::cout << "number: " << simple.toString() << std::endl;
            }
            
            virtual void addConstantsComment (int line){
                std::stringstream out;
                out << line;
                constantsContent.addSpace();
                constantsContent.addLine(0,"# [" + Mugen::stripDir(currentFile) + "] Section - " + sectionName + " on line " + out.str());
            }
            
            virtual void complete (std::vector<PythonClass> & classes){
                // Add constants content
                character.getInit().addContent(constantsContent);
                
                // Do states
                states.addToClass(character);
                states.addStateClasses(classes);
            }
    };
    
    for (std::vector<std::string>::iterator i = stateFiles.begin(); i != stateFiles.end(); ++i){
        StateWalker state(*this, character, *i);
        std::list<Ast::Section*> * sections = parseCmd(directory + *i);
        for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
            Ast::Section * section = *it;
            section->walk(state);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
        }
        destroy(sections);
        state.complete(stateClasses);
    }
    
}

void CharacterGenerator::outputStateClasses(PythonStream & out){
    for(std::vector<PythonClass>::iterator i = stateClasses.begin(); i != stateClasses.end(); ++i){
        (*i).output(out);
    }
}
