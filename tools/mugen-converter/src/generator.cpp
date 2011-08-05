#include "generator.h"

#include <iostream>
#include <fstream>

using namespace Mugen;

const std::string addTab(int total = 1){
    std::string tabAmount;
    for(int i = 0; i < total; ++i){
        tabAmount += "    ";
    }
    return tabAmount;
}

CharacterGenerator::CharacterGenerator(const std::string & filename):
filename(filename){
}

CharacterGenerator::~CharacterGenerator(){
}

void CharacterGenerator::output(const std::string & file){
    std::ofstream out;
    out.open(file.c_str());
    out << "import mugen" << std::endl;
    out <<  "class TestCharacter(mugen.Character):" << std::endl;
    out << addTab() << "def __init__(self):" << std::endl;
    out << addTab(2) << "self.setName('TestCharacter')" << std::endl;
    out.close();
}