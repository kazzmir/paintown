#include "generator.h"

#include <iostream>
#include <fstream>

using namespace Mugen;

CharacterGenerator::CharacterGenerator(const std::string & filename):
filename(filename){
}

CharacterGenerator::~CharacterGenerator(){
}

void CharacterGenerator::output(const std::string & file){
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