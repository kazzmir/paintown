#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>

#include "r-tech1/file-system.h"
#include "r-tech1/token.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/pointer.h"

#ifdef HAVE_YAML_CPP
#if defined(WIN32) || defined(WINDOWS)
#define YAML_CPP_STATIC_DEFINE
#endif
#include <yaml-cpp/yaml.h>
#endif

using namespace std;

/* tokenreader reads a file formatted with s-expressions. examples:
 * (hello)
 * (hello world)
 * (hello "world")
 * (hello (world))
 * (hello (world hi))
 */

//bool isYaml(const std::string & path, bool isFile){
bool isYaml(Storage::File & file){
#ifdef HAVE_YAML_CPP
    std::string content = file.readAsString();
    // DebugLog1 << "Got content: " << content << std::endl;

    // Check if we got data, ignore any node that starts with ( and treat it like the start of an s-expression otherwise try to load the yaml
    if (content[0] == '('){
        // DebugLog1 << "Starts like an s-expression..." << std::endl;
        return false;
    }
    try {
        // return (isFile ? YAML::LoadFile(path).size() > 0 : YAML::Load(path).size() > 0);
        return YAML::Load(content).size() > 0;
    } catch (YAML::Exception & ex){
        DebugLog1 << "Not a valid yaml source..." << std::endl;
    }
#endif
    return false;
}
    
TokenReader::TokenReader(){
}

TokenReader::TokenReader(const char * file){
    readTokenFromFile(file);
}

TokenReader::TokenReader(const string & file){
    readTokenFromFile(file);
}

Token * TokenReader::readTokenFromFile(const std::string & path){
    /*
    ifstream file(path.c_str());
    if (!file){
        ostringstream out;
        out << "Could not open '" << path << "'";
        throw TokenException(__FILE__, __LINE__, out.str());
    }
    file >> noskipws;
    */
    Filesystem::AbsolutePath realPath(path);
    Util::ReferenceCount<Storage::File> file = Storage::instance().open(realPath);
    if (file == NULL || !file->good()){
        throw TokenException(__FILE__, __LINE__, string("Could not read ") + realPath.path());
    }
    // FIXME use file instead of absolutePath
    //if (!isYaml(realPath.path(), true)){
    if (isYaml(*file.raw())){
        file->reset();
        // (*file.raw()).reset();
        readTokensFromYaml(*file.raw());
        // readTokens(*file.raw());
        // file.close();
    } else {
        //readTokensFromYaml(realPath.path(), true);
        // (*file.raw()).reset();
        file->reset();
        readTokens(*file.raw());
    }

    if (my_tokens.size() > 0){
        my_tokens[0]->setFile(path);
        return my_tokens[0];
    }
    ostringstream out;
    out << "No tokens read from " << path;
    throw TokenException(__FILE__, __LINE__, out.str());
}
    
Token * TokenReader::readTokenFromFile(Storage::File & file){
    //readTokens(file);
    // const std::string & filePath = file.location()->findToken("file")->getToken(0)->getName();
    // DebugLog1 << "Location: " << file.location()->findToken("file")->getToken(0)->getName() << std::endl;
    
    // FIXME use file instead of absolutePath
    //if (!isYaml(filePath, true)){
    if(!isYaml(file)){
        file.reset();
        readTokens(file);
        // file.close();
    } else {
        //readTokensFromYaml(filePath, true);
        file.reset();
        readTokensFromYaml(file);
    }

    // file.close();
    if (my_tokens.size() > 0){
        my_tokens[0]->setFile("");
        return my_tokens[0];
    }
    ostringstream out;
    out << "No tokens read from file";
    throw TokenException(__FILE__, __LINE__, out.str());

}

Token * TokenReader::readToken(){
    if (my_tokens.size() > 0){
        return my_tokens[0];
    }
    throw TokenException(__FILE__, __LINE__, "No tokens read");
}
    
Token * TokenReader::readToken(const string & path){
    return readTokenFromFile(path);
}

Token * TokenReader::readToken(const char * path){
    return readTokenFromFile(path);
}
    
Token * TokenReader::readTokenFromString(const string & stuff){
    /*
    istringstream input(stuff);
    input >> noskipws;
    */
    
    //if (!isYaml(stuff, false)){
    
    Storage::StringFile input(stuff);
    if (isYaml(input)){
        input.reset();
        readTokensFromYaml(input);
        // readTokens(input);
    } else {
        input.reset();
        readTokens(input);
    }
    if (my_tokens.size() > 0){
        return my_tokens[0];
    }
    throw TokenException(__FILE__, __LINE__, "No tokens read");
}

TokenReader::~TokenReader(){
    // ifile.close();

    /* tokenreader giveth, and tokenreader taketh */
    for ( vector< Token * >::iterator it = my_tokens.begin(); it != my_tokens.end(); it++ ){
        delete *it;
    }
}

static inline bool is_alpha(char c){
    // const char * alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./-_!:*";
    const char * alpha = "./-_!:*";
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           strchr(alpha, c) != NULL;
}

static inline bool is_nonalpha(char c){
    const char * nonalpha = " ;()#\"";
    return strchr(nonalpha, c) != NULL;
}

class BufferedStream{
public:
    BufferedStream(Storage::File & input):
    data(NULL),
    position(0),
    size(input.getSize()){
        if (input.getSize() <= 0){
            ostringstream error;
            error << "File had invalid size " << input.getSize();
            throw TokenException(__FILE__, __LINE__, error.str());
        }
        data = new unsigned char[size];
        input.readLine((char*) data, size);
    }

    BufferedStream & operator>>(unsigned char & n){
        if (position < size){
            n = data[position];
            position += 1;
        }
        return *this;
    }

    bool eof(){
        return position >= size;
    }

    ~BufferedStream(){
        delete[] data;
    }

    unsigned char * data;
    int position;
    int size;
};

void TokenReader::readTokens(Storage::File & input){

    /*
    if ( !ifile ){
        ostringstream out;
        out << "Could not open '" << myfile << "'";
        throw TokenException(__FILE__, __LINE__, out.str());
    }
    */
    // Token * t;

    // string token_string;

    // char open_paren = 'x';
    int parens = 0;
    // int position = 0;
    /*
    while (input.good() && open_paren != '('){
        input >> open_paren;
        position += 1;
    }
    */
    // token_string += '(';

    BufferedStream buffer(input);

    Token * currentToken = NULL;
    // Token * cur_token = new Token();
    // cur_token->setFile(myfile);
    // my_tokens.push_back(cur_token);
    // Token * first = cur_token;
    vector<Token *> token_stack;
    /* tokens that were ignored using ;@, and should be deleted */
    vector<Util::ReferenceCount<Token> > ignore_list;
    // token_stack.push_back(cur_token);

    /* when a ;@ is seen, read the next s-expression but throw it away */
    bool do_ignore = false;

    unsigned char n = 0;
    string cur_string = "";

    /* in_quote is true if a " is read and before another " is read */
    bool in_quote = false;

    /* escaped unconditionally adds the next character to the string */
    bool escaped = false;
    while (!buffer.eof()){
            // char n;
        // slow as we go
        buffer >> n;
        // position += 1;
        // printf("Read character '%c' %d at %d\n", n, n, input.tellg());
        // cout << "Read character '" << n << "' " << (int) n << " at " << input.tellg() << endl;

        // cout<<"Alpha char: "<<n<<endl;

        if (escaped){
            switch (n){
                case 'n' : {
                    cur_string += "\n";
                    break;
                }
                default : {
                    cur_string += n;
                    break;
                }
            }
            escaped = false;
            continue;
        }

        if (n == '\\'){
            escaped = true;
            continue;
        }

        if (in_quote){
            if (n == '"'){
                in_quote = false;

                Token * sub = new Token(cur_string, false);
                if (currentToken != NULL){
                    sub->setParent(currentToken);
                    if (do_ignore){
                        ignore_list.push_back(Util::ReferenceCount<Token>(sub));
                        do_ignore = false;
                    } else {
                        currentToken->addToken(sub);
                    }
                    cur_string = "";
                } else {
                    delete sub;
                }
            } else {
                cur_string += n;
            }
        } else {
            /* not in a quote */

            if (n == '"'){
                in_quote = true;
            } else if (is_alpha(n)){
                cur_string += n;
            } else if (cur_string != "" && is_nonalpha(n)){
                // cout<<"Made new token "<<cur_string<<endl;
                Token * sub = new Token(cur_string, false);
                if (currentToken != NULL){
                    sub->setParent(currentToken);
                    if (do_ignore){
                        do_ignore = false;
                        ignore_list.push_back(Util::ReferenceCount<Token>(sub));
                    } else {
                        currentToken->addToken(sub);
                    }
                } else {
                    delete sub;
                }
                cur_string = "";
            }

            if (n == '#' || n == ';'){
                buffer >> n;

                /* if the next character is a @ then ignore the next entire s-expression
                 * otherwise just ignore the current line
                 */
                if (n == '@'){
                    do_ignore = true;
                } else {
                    while (n != '\n' && !buffer.eof()){
                        buffer >> n;
                    }
                    continue;
                }
            } else if (n == '('){
                Token * another = new Token();
                if (currentToken != NULL){
                    another->setParent(currentToken);
                }

                if (do_ignore){
                    ignore_list.push_back(Util::ReferenceCount<Token>(another));
                    do_ignore = false;
                } else {
                    if (currentToken != NULL){
                        currentToken->addToken(another);
                    } else {
                        /* top level token */
                        my_tokens.push_back(another);
                    }
                }

                parens += 1;
                currentToken = another;
                token_stack.push_back(currentToken);
            } else if (n == ')'){
                parens -= 1;
                if (token_stack.empty()){
                    throw TokenException(__FILE__, __LINE__, "Saw a ) but there is no corresponding (");
                }
                token_stack.pop_back();

                if (! token_stack.empty()){
                    currentToken = token_stack.back();
                } else {
                    currentToken = NULL;
                }
            }
        }
    }

    if (!token_stack.empty()){
        ostringstream failure;
        failure << "Wrong number of parentheses. Open parens is " << parens;
        throw TokenException(__FILE__, __LINE__, failure.str());
    }

    /*
    for (vector<Token*>::iterator it = my_tokens.begin(); it != my_tokens.end(); it++){
        Token * token = *it;
        token->finalize();
    }
    */
}

void TokenReader::readTokensFromYaml(Storage::File & file){
#ifdef HAVE_YAML_CPP

    class YamlReader {
    public:
        YamlReader(Storage::File & file){
            origin = file.readAsString();
            head = YAML::Load(origin);
            DebugLog3 << "Reading tokens from yaml or file: " << origin << std::endl;
            //DebugLog2 << "Loaded content: " << std::endl;
            //DebugLog2 << "\n" << head << std::endl;
            load();
        }
        ~YamlReader(){

        }
        std::vector<Token *> getTokens(){
            return tokens;
        }
    private:
        Token * scalar(const YAML::Node & node){
            const std::string name = node.as<std::string>();
            //DebugLog2 << "Storing Scalar as Token with value: " << name << std::endl;
            // Do not enclose in an sexpression
            //Token::makeSExpression(Token::makeDatum(name));
            return Token::makeDatum(name); 
        }
        std::vector<Token *> sequence(const YAML::Node & node){
            std::vector<Token *> tokens;
            //DebugLog2 << "Found sequence.. aka an array" << std::endl;
            for (YAML::Node item : node) {
                if (item.Type() == YAML::NodeType::Scalar){
                    //DebugLog2 << "Found Scalar type." << std::endl;
                    Token * token = scalar(item);
                    //token->print("");
                    tokens.emplace_back(token);
                } else if (item.Type() == YAML::NodeType::Sequence){
                    //DebugLog2 << "Found Sequence type." << std::endl;
                    std::vector<Token *> sequenceOut = sequence(item);
                    Token * out = Token::makeSExpression(sequenceOut);
                    //out->print(" ");
                    tokens.emplace_back(out);
                } else if (item.Type() == YAML::NodeType::Map){
                    //DebugLog2 << "Found Map type." << std::endl;
                    std::vector<Token *> mapOut = map(item);
                    tokens.insert(tokens.end(), mapOut.begin(), mapOut.end());
                }
            }
            //return out;
            return tokens;
        }
        std::vector<Token *> map(const YAML::Node & node){
            //DebugLog3 << "Map content of size: " << node.size() << std::endl;
            std::vector<Token *> output;
            for (const std::pair<YAML::Node, YAML::Node>& keyValue : node) {
                const std::string & key = keyValue.first.as<std::string>();
                const YAML::Node & node = keyValue.second;
                //DebugLog3 << "Found node name: " << key << std::endl;

                if (node.Type() == YAML::NodeType::Scalar){
                    //DebugLog2 << "Found Scalar type." << std::endl;
                    Token * token = Token::makeSExpression(Token::makeDatum(key), 
                                                           scalar(node));
                    //token->print("");
                    output.emplace_back(token);
                } else if (node.Type() == YAML::NodeType::Sequence){
                    //DebugLog2 << "Found Sequence type." << std::endl;
                    std::vector<Token *> sequenceOut = sequence(node);
                    Token * out = Token::makeSExpression(Token::makeDatum(key), sequenceOut);
                    //out->print(" ");
                    output.emplace_back(out);
                } else if (node.Type() == YAML::NodeType::Map){
                    //DebugLog2 << "Found Map type." << std::endl;
                    std::vector<Token *> mapOut = map(node);
                    Token * out = Token::makeSExpression(Token::makeDatum(key), mapOut);
                    //out->print(" ");
                    output.emplace_back(out);

                }
            }
            return output;
        }
        void load(){
            // Start with map at head of yaml
            for (const std::pair<YAML::Node, YAML::Node>& keyValue : head) {
                const std::string & key = keyValue.first.as<std::string>();
                const YAML::Node & node = keyValue.second;

                if (node.Type() == YAML::NodeType::Scalar){
                    Token * token = Token::makeSExpression(Token::makeDatum(key), 
                                                           scalar(node));
                    tokens.emplace_back(token);
                } else if (node.Type() == YAML::NodeType::Sequence){
                    std::vector<Token *> sequenceOut = sequence(node);
                    Token * out = Token::makeSExpression(Token::makeDatum(key), sequenceOut);
                    //out->print(" ");
                    tokens.emplace_back(out);
                } else if (node.Type() == YAML::NodeType::Map){
                    std::vector<Token *> mapOut = map(node);
                    Token * out = Token::makeSExpression(Token::makeDatum(key), mapOut);
                    //out->print(" ");
                    tokens.emplace_back(out);
                }
            }
        }

        std::string origin;
        YAML::Node head;
        std::vector<Token *> tokens;
    };
    YamlReader reader(file);
    for (Token * token : reader.getTokens()){
        my_tokens.emplace_back(token);
    }
#else
    DebugLog2 << "No support for yaml in this build." << std::endl;
#endif
}
