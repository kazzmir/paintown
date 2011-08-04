#include "example.h"
#include "ast/all.h"

using namespace std;

void Mugen::exampleParseCmd(const string & file){
    try{
        class Walker: public Ast::Walker {
        public:
            Walker(){
            }
	    
            virtual void onSection(const Ast::Section & section){
                //cout << "Section: ";
            }
	    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		string attribute = simple.toString();
		string::size_type loc = attribute.find( "=", 0 );
		loc += 2;
		while(loc < attribute.size()) {
		    string word = findword(loc, attribute);
		    if (word == "") {
		        if (attribute[loc] == '"') 
		            loc = attribute.find('"', ++loc);
		        loc++;
		        continue;
		     }
		
		     loc = attribute.find( word, loc );
		     attribute.replace(loc ,word.size(),"self." + word + "()");
		     loc += word.size() + 8;
		
		}

		for(string::size_type pos = attribute.find("||", 0); pos != string::npos; pos = attribute.find("||", pos))
		     attribute.replace(pos, 2, "or");

		for(string::size_type pos = attribute.find("&&", 0); pos != string::npos; pos = attribute.find("&&", pos))
		     attribute.replace(pos, 2, "and");

		string::size_type pos = attribute.find("=", 0);
		pos++;
		//we want to replace 2 places ahead to avoid infinite loop
		for(pos = attribute.find("=", pos); pos != string::npos; pos = attribute.find("=", pos + 2)) 
		     attribute.replace(pos, 1, "==");
	
		string word = findword(0,attribute);
		loc = attribute.find( word, 0);
		attribute.replace(loc ,word.size(),"self." + word);
		
		cout << "Attribute simple: " << attribute << endl;
	    }
	    
	    virtual void onAttributeKeyword (const Ast::AttributeKeyword &simple){
		cout << "Attribute keyword: " << simple.toString() << endl;
	    }
	    
	    virtual void onAttributeArray (const Ast::AttributeArray &simple){
		cout << "Attribute array: " << simple.toString() << endl;
	    }

	    virtual void onNumber (const Ast::Number &simple){
		cout << "number: " << simple.toString() << endl;
	    }
	    
	    private:

	    bool filter(char c) {
		string list = "1234567890=()|&![],<>";
		for(string::size_type pos = 0;pos < list.size();pos++) {
		    if (list[pos] == c) return true;
		}
	        return false;
	    }
	    
	    string findword(string::size_type starpos, string phrase) {
		string word = "";
		for (;starpos < phrase.size();starpos++) {
		    if (phrase[starpos] == '"') {
		        string::size_type pos = phrase.find('"', ++starpos);
			if (pos != string::npos) {
			    starpos = pos;
			}
			continue;
		    }

		    if(filter(phrase[starpos]) && word == "") {
		        continue;
		    }

		    if (phrase[starpos] == ' ') break;
		    word += phrase[starpos];
		}  

		return word;
	    }
        };

        Walker walker;
        list<Ast::Section*> * sections = parseCmd(file);
        for (list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
            Ast::Section * section = *it;
	    cout << "Section: "<< section->getName() << endl;
	    section->walk(walker);
	
	    list<Ast::Attribute*>  attributes = section->getAttributes();
	    
	    /*
	    for (list<Ast::Attribute*>::iterator it2 = attributes.begin(); it2 != attributes.end(); it2++){
	        Ast::Attribute * attrib = *it2;
		
		cout << attrib->toString() << endl;
	    }
	    */
        }
        destroy(sections);
        
    } catch (const Mugen::Cmd::ParseException & fail){
        cout << "Failed to parse " << file << " because " << fail.getReason() << endl;
    }
}
