

#include "../ast/all.h"
#include <map>
#include "gc.h"
typedef std::list<Ast::Section*> SectionList;


#include <list>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>


namespace Mugen{

    namespace Def{
    
    



struct Value{
    typedef std::list<Value>::const_iterator iterator;

    Value():
        which(1),
        value(0){
    }

    Value(const Value & him){
        which = him.which;
        if (him.isData()){
            value = him.value;
        }
        if (him.isList()){
            values = him.values;
        }
    }

    Value(const void * value):
        which(0),
        value(value){
    }

    Value & operator=(const Value & him){
        which = him.which;
        if (him.isData()){
            value = him.value;
        }
        if (him.isList()){
            values = him.values;
        }
        return *this;
    }

    void reset(){
        this->value = 0;
        this->values.clear();
        this->which = 1;
    }

    int which; // 0 is value, 1 is values

    inline const bool isList() const {
        return which == 1;
    }

    inline const bool isData() const {
        return which == 0;
    }

    inline const void * getValue() const {
        return value;
    }

    inline void setValue(const void * value){
        which = 0;
        this->value = value;
    }

    inline const std::list<Value> & getValues() const {
        return values;
    }

    /*
    inline void setValues(std::list<Value> values){
        which = 1;
        values = values;
    }
    */

    const void * value;
    std::list<Value> values;
};

class Result{
public:
    Result():
    position(-2){
    }

    Result(const int position):
    position(position){
    }

    Result(const Result & r){
        position = r.position;
        value = r.value;
    }

    Result & operator=(const Result & r){
        position = r.position;
        value = r.value;
        return *this;
    }

    void reset(){
        value.reset();
    }

    inline const int getPosition() const {
        return position;
    }

    inline bool error(){
        return position == -1;
    }

    inline bool calculated(){
        return position != -2;
    }

    inline void nextPosition(){
        position += 1;
    }

    void setError(){
        position = -1;
    }

    inline void setValue(const Value & value){
        this->value = value;
    }

    /*
    Value getLastValue() const {
        if (value.isList()){
            if (value.values.size() == 0){
                std::cout << "[peg] No last value to get!" << std::endl;
            }
            return value.values[value.values.size()-1];
        } else {
            return value;
        }
    }
    */

    inline const int matches() const {
        if (value.isList()){
            return this->value.values.size();
        } else {
            return 1;
        }
    }

    inline const Value & getValues() const {
        return this->value;
    }

    void addResult(const Result & result){
        std::list<Value> & mine = this->value.values;
        mine.push_back(result.getValues());
        this->position = result.getPosition();
        this->value.which = 1;
    }

private:
    int position;
    Value value;
};



struct Chunk0{
    Result chunk_start;
    Result chunk_line;
    Result chunk_line_end;
    Result chunk_s;
    Result chunk_section;
};

struct Chunk1{
    Result chunk_section_line;
    Result chunk_section_start;
    Result chunk_loopstart;
    Result chunk_name;
    Result chunk_alpha_digit;
};

struct Chunk2{
    Result chunk_line_end_or_comment;
    Result chunk_filename;
    Result chunk_filename_char;
    Result chunk_attribute;
    Result chunk_identifier;
};

struct Chunk3{
    Result chunk_identifier_list;
    Result chunk_valuelist;
    Result chunk_value;
    Result chunk_keyword;
    Result chunk_date;
};

struct Chunk4{
    Result chunk_string;
    Result chunk_number;
    Result chunk_float_or_integer;
};

struct Column{
    Column():
        chunk0(0)
        ,chunk1(0)
        ,chunk2(0)
        ,chunk3(0)
        ,chunk4(0){
    }

    Chunk0 * chunk0;
    Chunk1 * chunk1;
    Chunk2 * chunk2;
    Chunk3 * chunk3;
    Chunk4 * chunk4;

    ~Column(){
        delete chunk0;
        delete chunk1;
        delete chunk2;
        delete chunk3;
        delete chunk4;
    }
};


class Stream{
public:
    Stream(const std::string & filename):
    temp(0),
    buffer(0),
    farthest(0){
        std::ifstream stream;
        stream.open(filename.c_str());
        stream.seekg(0, std::ios_base::end);
        max = stream.tellg();
        stream.seekg(0, std::ios_base::beg);
        temp = new char[max];
        stream.read(temp, max);
        buffer = temp;
        stream.close();

        createMemo();
    }

    Stream(const char * in):
    temp(0),
    buffer(in),
    farthest(0){
        max = strlen(buffer);
        createMemo();
    }

    void createMemo(){
        memo_size = 1024;
        memo = new Column*[memo_size];
        for (int i = 0; i < memo_size; i++){
            memo[i] = new Column();
        }
    }

    char get(const int position){
        if (position >= max || position < 0){
            return '\0';
        }

        // std::cout << "Read char '" << buffer[position] << "'" << std::endl;

        return buffer[position];
        /*
        char z;
        stream.seekg(position, std::ios_base::beg);
        stream >> z;
        return z;
        */
    }

    bool find(const char * str, const int position){
        if (position >= max || position < 0){
            return false;
        }
        return strncmp(&buffer[position], str, max - position) == 0;
    }

    void growMemo(){
        int newSize = memo_size * 2;
        Column ** newMemo = new Column*[newSize];
        memcpy(newMemo, memo, sizeof(Column*) * memo_size);
        for (int i = memo_size; i < newSize; i++){
            newMemo[i] = new Column();
        }
        delete[] memo;
        memo = newMemo;
        memo_size = newSize;
    }

    std::string reportError(){
        std::ostringstream out;
        int line = 1;
        int column = 1;
        for (int i = 0; i < farthest; i++){
            if (buffer[i] == '\n'){
                line += 1;
                column = 1;
            } else {
                column += 1;
            }
        }
        int context = 10;
        int left = farthest - context;
        int right = farthest + context;
        if (left < 0){
            left = 0;
        }
        if (right >= max){
            right = max;
        }
        out << "Read up till line " << line << " column " << column << std::endl;
        std::ostringstream show;
        for (int i = left; i < right; i++){
            char c = buffer[i];
            switch (buffer[i]){
                case '\n' : {
                    show << '\\';
                    show << 'n';
                    break;
                }
                case '\r' : {
                    show << '\\';
                    show << 'r';
                    break;
                }
                case '\t' : {
                    show << '\\';
                    show << 't';
                    break;
                }
                default : show << c; break;
            }
        }
        out << "'" << show.str() << "'" << std::endl;
        for (int i = 0; i < farthest - left; i++){
            out << " ";
        }
        out << "^" << std::endl;
        return out.str();
    }

    inline Column & getColumn(const int position){
        if (position >= memo_size){
            growMemo();
        }
        return *(memo[position]);
    }

    void update(const int position){
        if (position > farthest){
            farthest = position;
        }
    }

    ~Stream(){
        delete[] temp;
        for (int i = 0; i < memo_size; i++){
            delete memo[i];
        }
        delete[] memo;
    }

private:
    char * temp;
    const char * buffer;
    // std::map<const int, Column> memo;
    Column ** memo;
    int memo_size;
    // std::vector<Column> memo;
    int max;
    int farthest;
};

static inline bool compareChar(const char a, const char b){
    return a == b;
}

static inline char lower(const char x){
    if (x >= 'A' && x <= 'Z'){
        return x - 'A' + 'a';
    }
    return x;
}

static inline bool compareCharCase(const char a, const char b){
    return lower(a) == lower(b);
}

class ParseException: std::exception {
public:
    ParseException(const std::string & reason):
    std::exception(),
    message(reason){
    }

    std::string getReason() const {
        return message;
    }

    virtual ~ParseException() throw(){
    }

protected:
    std::string message;
};

Result errorResult(-1);


Result rule_start(Stream &, const int);
Result rule_line(Stream &, const int, Value current);
Result rule_line_end(Stream &, const int);
Result rule_s(Stream &, const int);
Result rule_section(Stream &, const int);
Result rule_section_line(Stream &, const int, Value section);
Result rule_section_start(Stream &, const int);
Result rule_loopstart(Stream &, const int);
Result rule_name(Stream &, const int);
Result rule_alpha_digit(Stream &, const int);
Result rule_line_end_or_comment(Stream &, const int);
Result rule_filename(Stream &, const int);
Result rule_filename_char(Stream &, const int);
Result rule_attribute(Stream &, const int);
Result rule_identifier(Stream &, const int);
Result rule_identifier_list(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_keyword(Stream &, const int);
Result rule_date(Stream &, const int);
Result rule_string(Stream &, const int);
Result rule_number(Stream &, const int);
Result rule_float_or_integer(Stream &, const int);


template<class X>
X as(const Value & value){
    return (X) value.getValue();
}

void addSection(const Value & section_list_value, const Value & section_value){
  SectionList * sections = (SectionList*) section_list_value.getValue();
  Ast::Section * section = (Ast::Section*) section_value.getValue();
  if (section == 0){
    throw ParseException("Cannot add null section");
  }
  sections->push_back(section);
}

Ast::Section * makeSection(const Value & str){
  return new Ast::Section(as<std::string*>(str));
}

SectionList * makeSectionList(){
  return new SectionList();
}

std::string * toString(const Value & input){
  std::ostringstream out;
  for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
    out << (char) (long) (*it).getValue();
  }
  return new std::string(out.str());
}

double * parseDouble(const Value & value){
    std::string * str = toString(value);
    std::istringstream get(*str);
    double * number = new double;
    get >> *number;
    delete str;
    return number;
}

double * parseDouble(const Value & left, const Value & right){
    std::string * str1 = toString(left);
    std::string * str2 = toString(right);
    std::istringstream get(*str1 + "." + *str2);
    double * number = new double;
    get >> *number;
    delete str1;
    delete str2;
    return number;
}

std::string * toString(char front, const Value & input){
  std::string * str = toString(input);
  str->insert(str->begin(), front);
  return str;
}

Ast::Attribute * makeAttribute(const Value & id, const Value & data){
    return new Ast::AttributeSimple(as<Ast::Identifier*>(id), as<Ast::Value*>(data));
}

Ast::Attribute * makeAttribute(const Value & id){
    return new Ast::AttributeSimple(as<Ast::Identifier*>(id));
}

Ast::Attribute * makeAttributeFilename(const Value & id, const Value & data){
    return makeAttribute(id, data);
}

Ast::Attribute * makeIndexedAttribute(const Value & id, const Value & index, const Value & data){
    /* wrong, fix this */
    return new Ast::Attribute(Ast::Attribute::None);
}

Ast::Keyword * makeKeyword(const Value & value){
    return new Ast::Keyword(as<char*>(value));
}

Ast::Attribute * makeAttributes(const Value & id, const Value & data){
    /* wrong, fix this */
    return new Ast::Attribute(Ast::Attribute::None);
}

/* FIXME */
Ast::Value * makeValue(){
    return new Ast::Number(0);
}

Ast::Value * makeValueList(const Value & front, const Value & rest){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        Ast::Value * value = as<Ast::Value*>((*it).getValue());
        if (value == 0){
            /* FIXME! replace empty with a new node */
            value = makeKeyword("empty");
            GC::save(as<Ast::Keyword*>(value));
            values.push_back(value);
        } else {
            values.push_back(value);
        }
    }
    return new Ast::ValueList(values);
}

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string*> ids;
    ids.push_back(as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        ids.push_back(as<std::string*>((*it).getValue()));
    }
    return new Ast::Identifier(ids);
}

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0){
        value = -value;
    }

    return new Ast::Number(value);
}

Ast::String * makeString(const Value & value){
    return new Ast::String(toString(value));
}

Ast::Section * asSection(const Value & value){
    return as<Ast::Section*>(value);
}

Ast::Attribute * asAttribute(const Value & value){
    return as<Ast::Attribute*>(value);
}

Ast::Value * asValue(const Value & value){
    return as<Ast::Value*>(value);
}

/* FIXME */
Ast::Value * makeDate(const Value & month, const Value & day, const Value & year){
    return new Ast::Number(0);
}




Result rule_start(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_start.calculated()){
            return column.chunk0->chunk_start;
        }
    }
    
    int myposition = position;
    
    Value current;
    Result result_peg_1(myposition);
    
    {
    
        {
                Value value((void*) 0);
                value = makeSectionList(); GC::save(as<SectionList*>(value));
                result_peg_1.setValue(value);
            }
            current = result_peg_1.getValues();
        
        
        
        {
                
                result_peg_1.reset();
                do{
                    Result result_peg_98(result_peg_1.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_98.getPosition()))){
                                result_peg_98.nextPosition();
                            } else {
                                goto out_peg_101;
                            }
                        }
                        result_peg_98.setValue((void*) " ");
                            
                    }
                    goto success_peg_99;
                    out_peg_101:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_98.getPosition()))){
                                result_peg_98.nextPosition();
                            } else {
                                goto out_peg_103;
                            }
                        }
                        result_peg_98.setValue((void*) "\t");
                            
                    }
                    goto success_peg_99;
                    out_peg_103:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_98.getPosition()))){
                                            result_peg_98.nextPosition();
                                        } else {
                                            goto out_peg_150;
                                        }
                                    }
                                    result_peg_98.setValue((void*) ";");
                                
                                
                                
                                result_peg_98.reset();
                                    do{
                                        Result result_peg_159(result_peg_98.getPosition());
                                        {
                                        
                                            Result result_peg_162(result_peg_159);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_162.getPosition()))){
                                                        result_peg_162.nextPosition();
                                                    } else {
                                                        goto not_peg_161;
                                                    }
                                                }
                                                result_peg_162.setValue((void*) "\n");
                                                goto loop_peg_158;
                                                not_peg_161:
                                                result_peg_159.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_163 = stream.get(result_peg_159.getPosition());
                                                if (temp_peg_163 != '\0'){
                                                    result_peg_159.setValue((void*) temp_peg_163);
                                                    result_peg_159.nextPosition();
                                                } else {
                                                    goto loop_peg_158;
                                                }
                                            
                                            
                                        }
                                        result_peg_98.addResult(result_peg_159);
                                    } while (true);
                                    loop_peg_158:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_147;
                        out_peg_150:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_98.getPosition()))){
                                            result_peg_98.nextPosition();
                                        } else {
                                            goto out_peg_166;
                                        }
                                    }
                                    result_peg_98.setValue((void*) "=");
                                
                                
                                
                                result_peg_98.reset();
                                    do{
                                        Result result_peg_175(result_peg_98.getPosition());
                                        {
                                        
                                            Result result_peg_178(result_peg_175);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_178.getPosition()))){
                                                        result_peg_178.nextPosition();
                                                    } else {
                                                        goto not_peg_177;
                                                    }
                                                }
                                                result_peg_178.setValue((void*) "\n");
                                                goto loop_peg_174;
                                                not_peg_177:
                                                result_peg_175.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_179 = stream.get(result_peg_175.getPosition());
                                                if (temp_peg_179 != '\0'){
                                                    result_peg_175.setValue((void*) temp_peg_179);
                                                    result_peg_175.nextPosition();
                                                } else {
                                                    goto loop_peg_174;
                                                }
                                            
                                            
                                        }
                                        result_peg_98.addResult(result_peg_175);
                                    } while (true);
                                    loop_peg_174:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_147;
                        out_peg_166:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_98.getPosition()))){
                                            result_peg_98.nextPosition();
                                        } else {
                                            goto out_peg_182;
                                        }
                                    }
                                    result_peg_98.setValue((void*) "-");
                                
                                
                                
                                result_peg_98.reset();
                                    do{
                                        Result result_peg_187(result_peg_98.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_187.getPosition()))){
                                                result_peg_187.nextPosition();
                                            } else {
                                                goto loop_peg_186;
                                            }
                                        }
                                        result_peg_187.setValue((void*) "-");
                                        result_peg_98.addResult(result_peg_187);
                                    } while (true);
                                    loop_peg_186:
                                    if (result_peg_98.matches() == 0){
                                        goto out_peg_182;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_147;
                        out_peg_182:
                        goto out_peg_146;
                        success_peg_147:
                        ;
                        
                    }
                    goto success_peg_99;
                    out_peg_146:
                    goto loop_peg_97;
                    success_peg_99:
                    ;
                    result_peg_1.addResult(result_peg_98);
                } while (true);
                loop_peg_97:
                ;
                        
            }
            goto success_peg_4;
            goto out_peg_188;
            success_peg_4:
            ;
        
        
        
        result_peg_1.reset();
            do{
                Result result_peg_191(result_peg_1.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_191.getPosition()))){
                            result_peg_191.nextPosition();
                        } else {
                            goto out_peg_194;
                        }
                    }
                    result_peg_191.setValue((void*) "\n");
                        
                }
                goto success_peg_192;
                out_peg_194:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_191.getPosition()))){
                            result_peg_191.nextPosition();
                        } else {
                            goto out_peg_196;
                        }
                    }
                    result_peg_191.setValue((void*) "\r");
                        
                }
                goto success_peg_192;
                out_peg_196:
                goto loop_peg_190;
                success_peg_192:
                ;
                result_peg_1.addResult(result_peg_191);
            } while (true);
            loop_peg_190:
            ;
        
        
        
        result_peg_1.reset();
            do{
                Result result_peg_199(result_peg_1.getPosition());
                {
                
                    result_peg_199 = rule_line(stream, result_peg_199.getPosition(), current);
                        if (result_peg_199.error()){
                            goto loop_peg_198;
                        }
                    
                    
                    
                    {
                            
                            result_peg_199.reset();
                            do{
                                Result result_peg_296(result_peg_199.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_296.getPosition()))){
                                            result_peg_296.nextPosition();
                                        } else {
                                            goto out_peg_299;
                                        }
                                    }
                                    result_peg_296.setValue((void*) " ");
                                        
                                }
                                goto success_peg_297;
                                out_peg_299:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_296.getPosition()))){
                                            result_peg_296.nextPosition();
                                        } else {
                                            goto out_peg_301;
                                        }
                                    }
                                    result_peg_296.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_297;
                                out_peg_301:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_296.getPosition()))){
                                                        result_peg_296.nextPosition();
                                                    } else {
                                                        goto out_peg_348;
                                                    }
                                                }
                                                result_peg_296.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_296.reset();
                                                do{
                                                    Result result_peg_357(result_peg_296.getPosition());
                                                    {
                                                    
                                                        Result result_peg_360(result_peg_357);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_360.getPosition()))){
                                                                    result_peg_360.nextPosition();
                                                                } else {
                                                                    goto not_peg_359;
                                                                }
                                                            }
                                                            result_peg_360.setValue((void*) "\n");
                                                            goto loop_peg_356;
                                                            not_peg_359:
                                                            result_peg_357.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_361 = stream.get(result_peg_357.getPosition());
                                                            if (temp_peg_361 != '\0'){
                                                                result_peg_357.setValue((void*) temp_peg_361);
                                                                result_peg_357.nextPosition();
                                                            } else {
                                                                goto loop_peg_356;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_296.addResult(result_peg_357);
                                                } while (true);
                                                loop_peg_356:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_345;
                                    out_peg_348:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_296.getPosition()))){
                                                        result_peg_296.nextPosition();
                                                    } else {
                                                        goto out_peg_364;
                                                    }
                                                }
                                                result_peg_296.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_296.reset();
                                                do{
                                                    Result result_peg_373(result_peg_296.getPosition());
                                                    {
                                                    
                                                        Result result_peg_376(result_peg_373);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_376.getPosition()))){
                                                                    result_peg_376.nextPosition();
                                                                } else {
                                                                    goto not_peg_375;
                                                                }
                                                            }
                                                            result_peg_376.setValue((void*) "\n");
                                                            goto loop_peg_372;
                                                            not_peg_375:
                                                            result_peg_373.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_377 = stream.get(result_peg_373.getPosition());
                                                            if (temp_peg_377 != '\0'){
                                                                result_peg_373.setValue((void*) temp_peg_377);
                                                                result_peg_373.nextPosition();
                                                            } else {
                                                                goto loop_peg_372;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_296.addResult(result_peg_373);
                                                } while (true);
                                                loop_peg_372:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_345;
                                    out_peg_364:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_296.getPosition()))){
                                                        result_peg_296.nextPosition();
                                                    } else {
                                                        goto out_peg_380;
                                                    }
                                                }
                                                result_peg_296.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_296.reset();
                                                do{
                                                    Result result_peg_385(result_peg_296.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_385.getPosition()))){
                                                            result_peg_385.nextPosition();
                                                        } else {
                                                            goto loop_peg_384;
                                                        }
                                                    }
                                                    result_peg_385.setValue((void*) "-");
                                                    result_peg_296.addResult(result_peg_385);
                                                } while (true);
                                                loop_peg_384:
                                                if (result_peg_296.matches() == 0){
                                                    goto out_peg_380;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_345;
                                    out_peg_380:
                                    goto out_peg_344;
                                    success_peg_345:
                                    ;
                                    
                                }
                                goto success_peg_297;
                                out_peg_344:
                                goto loop_peg_295;
                                success_peg_297:
                                ;
                                result_peg_199.addResult(result_peg_296);
                            } while (true);
                            loop_peg_295:
                            ;
                                    
                        }
                        goto success_peg_202;
                        goto loop_peg_198;
                        success_peg_202:
                        ;
                    
                    
                    
                    int save_peg_386 = result_peg_199.getPosition();
                        
                        result_peg_199 = rule_line_end(stream, result_peg_199.getPosition());
                        if (result_peg_199.error()){
                            
                            result_peg_199 = Result(save_peg_386);
                            result_peg_199.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1.addResult(result_peg_199);
            } while (true);
            loop_peg_198:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_1.getPosition())){
                result_peg_1.nextPosition();
                result_peg_1.setValue((void *) '\0');
            } else {
                goto out_peg_188;
            }
        
        
        
        {
                Value value((void*) 0);
                value = current; GC::cleanup(as<SectionList*>(value));
                result_peg_1.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_start = result_peg_1;
        stream.update(result_peg_1.getPosition());
    }
    
    
    return result_peg_1;
    out_peg_188:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_start = errorResult;
        stream.update(errorResult.getPosition());
    }
    
     GC::cleanup(as<SectionList*>(current)); 
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_line.calculated()){
            return column.chunk0->chunk_line;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_388(myposition);
    
    {
    
        result_peg_388 = rule_s(stream, result_peg_388.getPosition());
            if (result_peg_388.error()){
                goto out_peg_390;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_388.getPosition()))){
                                result_peg_388.nextPosition();
                            } else {
                                goto out_peg_394;
                            }
                        }
                        result_peg_388.setValue((void*) ";");
                    
                    
                    
                    result_peg_388.reset();
                        do{
                            Result result_peg_403(result_peg_388.getPosition());
                            {
                            
                                Result result_peg_406(result_peg_403);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_406.getPosition()))){
                                            result_peg_406.nextPosition();
                                        } else {
                                            goto not_peg_405;
                                        }
                                    }
                                    result_peg_406.setValue((void*) "\n");
                                    goto loop_peg_402;
                                    not_peg_405:
                                    result_peg_403.setValue((void*)0);
                                
                                
                                
                                char temp_peg_407 = stream.get(result_peg_403.getPosition());
                                    if (temp_peg_407 != '\0'){
                                        result_peg_403.setValue((void*) temp_peg_407);
                                        result_peg_403.nextPosition();
                                    } else {
                                        goto loop_peg_402;
                                    }
                                
                                
                            }
                            result_peg_388.addResult(result_peg_403);
                        } while (true);
                        loop_peg_402:
                        ;
                    
                    
                }
            }
            goto success_peg_391;
            out_peg_394:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_388.getPosition()))){
                                result_peg_388.nextPosition();
                            } else {
                                goto out_peg_410;
                            }
                        }
                        result_peg_388.setValue((void*) "=");
                    
                    
                    
                    result_peg_388.reset();
                        do{
                            Result result_peg_419(result_peg_388.getPosition());
                            {
                            
                                Result result_peg_422(result_peg_419);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_422.getPosition()))){
                                            result_peg_422.nextPosition();
                                        } else {
                                            goto not_peg_421;
                                        }
                                    }
                                    result_peg_422.setValue((void*) "\n");
                                    goto loop_peg_418;
                                    not_peg_421:
                                    result_peg_419.setValue((void*)0);
                                
                                
                                
                                char temp_peg_423 = stream.get(result_peg_419.getPosition());
                                    if (temp_peg_423 != '\0'){
                                        result_peg_419.setValue((void*) temp_peg_423);
                                        result_peg_419.nextPosition();
                                    } else {
                                        goto loop_peg_418;
                                    }
                                
                                
                            }
                            result_peg_388.addResult(result_peg_419);
                        } while (true);
                        loop_peg_418:
                        ;
                    
                    
                }
            }
            goto success_peg_391;
            out_peg_410:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_388.getPosition()))){
                                result_peg_388.nextPosition();
                            } else {
                                goto out_peg_426;
                            }
                        }
                        result_peg_388.setValue((void*) "-");
                    
                    
                    
                    result_peg_388.reset();
                        do{
                            Result result_peg_431(result_peg_388.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_431.getPosition()))){
                                    result_peg_431.nextPosition();
                                } else {
                                    goto loop_peg_430;
                                }
                            }
                            result_peg_431.setValue((void*) "-");
                            result_peg_388.addResult(result_peg_431);
                        } while (true);
                        loop_peg_430:
                        if (result_peg_388.matches() == 0){
                            goto out_peg_426;
                        }
                    
                    
                }
            }
            goto success_peg_391;
            out_peg_426:
            goto out_peg_390;
            success_peg_391:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_388;
        stream.update(result_peg_388.getPosition());
    }
    
    
    return result_peg_388;
    out_peg_390:
    Result result_peg_432(myposition);
    
    {
    
        result_peg_432 = rule_s(stream, result_peg_432.getPosition());
            if (result_peg_432.error()){
                goto out_peg_434;
            }
        
        
        
        result_peg_432 = rule_section(stream, result_peg_432.getPosition());
            if (result_peg_432.error()){
                goto out_peg_434;
            }
        
        Result result_peg_435 = result_peg_432;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_435.getValues());
                result_peg_432.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_432;
        stream.update(result_peg_432.getPosition());
    }
    
    
    return result_peg_432;
    out_peg_434:
    Result result_peg_436(myposition);
    
    result_peg_436.reset();
    do{
        Result result_peg_438(result_peg_436.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_438.getPosition()))){
                    result_peg_438.nextPosition();
                } else {
                    goto out_peg_441;
                }
            }
            result_peg_438.setValue((void*) " ");
                
        }
        goto success_peg_439;
        out_peg_441:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_438.getPosition()))){
                    result_peg_438.nextPosition();
                } else {
                    goto out_peg_443;
                }
            }
            result_peg_438.setValue((void*) "\t");
                
        }
        goto success_peg_439;
        out_peg_443:
        goto loop_peg_437;
        success_peg_439:
        ;
        result_peg_436.addResult(result_peg_438);
    } while (true);
    loop_peg_437:
    if (result_peg_436.matches() == 0){
        goto out_peg_444;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_436;
        stream.update(result_peg_436.getPosition());
    }
    
    
    return result_peg_436;
    out_peg_444:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_line_end.calculated()){
            return column.chunk0->chunk_line_end;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_445(myposition);
    
    result_peg_445.reset();
    do{
        Result result_peg_447(result_peg_445.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_447.getPosition()))){
                    result_peg_447.nextPosition();
                } else {
                    goto out_peg_450;
                }
            }
            result_peg_447.setValue((void*) "\n");
                
        }
        goto success_peg_448;
        out_peg_450:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_447.getPosition()))){
                    result_peg_447.nextPosition();
                } else {
                    goto out_peg_452;
                }
            }
            result_peg_447.setValue((void*) "\r");
                
        }
        goto success_peg_448;
        out_peg_452:
        goto loop_peg_446;
        success_peg_448:
        ;
        result_peg_445.addResult(result_peg_447);
    } while (true);
    loop_peg_446:
    if (result_peg_445.matches() == 0){
        goto out_peg_453;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_445;
        stream.update(result_peg_445.getPosition());
    }
    
    
    return result_peg_445;
    out_peg_453:
    Result result_peg_454(myposition);
    
    {
    
        Result result_peg_456(result_peg_454.getPosition());
            if ('\0' == stream.get(result_peg_456.getPosition())){
                result_peg_456.nextPosition();
                result_peg_456.setValue((void *) '\0');
            } else {
                goto out_peg_457;
            }
        
        
        
        
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_454;
        stream.update(result_peg_454.getPosition());
    }
    
    
    return result_peg_454;
    out_peg_457:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_s.calculated()){
            return column.chunk0->chunk_s;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_458(myposition);
    
    result_peg_458.reset();
    do{
        Result result_peg_460(result_peg_458.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_460.getPosition()))){
                    result_peg_460.nextPosition();
                } else {
                    goto out_peg_463;
                }
            }
            result_peg_460.setValue((void*) " ");
                
        }
        goto success_peg_461;
        out_peg_463:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_460.getPosition()))){
                    result_peg_460.nextPosition();
                } else {
                    goto out_peg_465;
                }
            }
            result_peg_460.setValue((void*) "\t");
                
        }
        goto success_peg_461;
        out_peg_465:
        goto loop_peg_459;
        success_peg_461:
        ;
        result_peg_458.addResult(result_peg_460);
    } while (true);
    loop_peg_459:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_s = result_peg_458;
        stream.update(result_peg_458.getPosition());
    }
    
    
    return result_peg_458;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_s = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_section.calculated()){
            return column.chunk0->chunk_section;
        }
    }
    
    int myposition = position;
    
    Value ast;
    Result result_peg_466(myposition);
    
    {
    
        result_peg_466 = rule_section_start(stream, result_peg_466.getPosition());
            if (result_peg_466.error()){
                goto out_peg_468;
            }
        
        Result result_peg_467 = result_peg_466;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_467.getValues()); GC::save(as<Ast::Section*>(value));
                result_peg_466.setValue(value);
            }
            ast = result_peg_466.getValues();
        
        
        
        {
                
                result_peg_466.reset();
                do{
                    Result result_peg_565(result_peg_466.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_565.getPosition()))){
                                result_peg_565.nextPosition();
                            } else {
                                goto out_peg_568;
                            }
                        }
                        result_peg_565.setValue((void*) " ");
                            
                    }
                    goto success_peg_566;
                    out_peg_568:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_565.getPosition()))){
                                result_peg_565.nextPosition();
                            } else {
                                goto out_peg_570;
                            }
                        }
                        result_peg_565.setValue((void*) "\t");
                            
                    }
                    goto success_peg_566;
                    out_peg_570:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_565.getPosition()))){
                                            result_peg_565.nextPosition();
                                        } else {
                                            goto out_peg_617;
                                        }
                                    }
                                    result_peg_565.setValue((void*) ";");
                                
                                
                                
                                result_peg_565.reset();
                                    do{
                                        Result result_peg_626(result_peg_565.getPosition());
                                        {
                                        
                                            Result result_peg_629(result_peg_626);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_629.getPosition()))){
                                                        result_peg_629.nextPosition();
                                                    } else {
                                                        goto not_peg_628;
                                                    }
                                                }
                                                result_peg_629.setValue((void*) "\n");
                                                goto loop_peg_625;
                                                not_peg_628:
                                                result_peg_626.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_630 = stream.get(result_peg_626.getPosition());
                                                if (temp_peg_630 != '\0'){
                                                    result_peg_626.setValue((void*) temp_peg_630);
                                                    result_peg_626.nextPosition();
                                                } else {
                                                    goto loop_peg_625;
                                                }
                                            
                                            
                                        }
                                        result_peg_565.addResult(result_peg_626);
                                    } while (true);
                                    loop_peg_625:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_614;
                        out_peg_617:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_565.getPosition()))){
                                            result_peg_565.nextPosition();
                                        } else {
                                            goto out_peg_633;
                                        }
                                    }
                                    result_peg_565.setValue((void*) "=");
                                
                                
                                
                                result_peg_565.reset();
                                    do{
                                        Result result_peg_642(result_peg_565.getPosition());
                                        {
                                        
                                            Result result_peg_645(result_peg_642);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_645.getPosition()))){
                                                        result_peg_645.nextPosition();
                                                    } else {
                                                        goto not_peg_644;
                                                    }
                                                }
                                                result_peg_645.setValue((void*) "\n");
                                                goto loop_peg_641;
                                                not_peg_644:
                                                result_peg_642.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_646 = stream.get(result_peg_642.getPosition());
                                                if (temp_peg_646 != '\0'){
                                                    result_peg_642.setValue((void*) temp_peg_646);
                                                    result_peg_642.nextPosition();
                                                } else {
                                                    goto loop_peg_641;
                                                }
                                            
                                            
                                        }
                                        result_peg_565.addResult(result_peg_642);
                                    } while (true);
                                    loop_peg_641:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_614;
                        out_peg_633:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_565.getPosition()))){
                                            result_peg_565.nextPosition();
                                        } else {
                                            goto out_peg_649;
                                        }
                                    }
                                    result_peg_565.setValue((void*) "-");
                                
                                
                                
                                result_peg_565.reset();
                                    do{
                                        Result result_peg_654(result_peg_565.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_654.getPosition()))){
                                                result_peg_654.nextPosition();
                                            } else {
                                                goto loop_peg_653;
                                            }
                                        }
                                        result_peg_654.setValue((void*) "-");
                                        result_peg_565.addResult(result_peg_654);
                                    } while (true);
                                    loop_peg_653:
                                    if (result_peg_565.matches() == 0){
                                        goto out_peg_649;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_614;
                        out_peg_649:
                        goto out_peg_613;
                        success_peg_614:
                        ;
                        
                    }
                    goto success_peg_566;
                    out_peg_613:
                    goto loop_peg_564;
                    success_peg_566:
                    ;
                    result_peg_466.addResult(result_peg_565);
                } while (true);
                loop_peg_564:
                ;
                        
            }
            goto success_peg_471;
            goto out_peg_468;
            success_peg_471:
            ;
        
        
        
        result_peg_466.reset();
            do{
                Result result_peg_657(result_peg_466.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_657.getPosition()))){
                            result_peg_657.nextPosition();
                        } else {
                            goto out_peg_660;
                        }
                    }
                    result_peg_657.setValue((void*) "\n");
                        
                }
                goto success_peg_658;
                out_peg_660:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_657.getPosition()))){
                            result_peg_657.nextPosition();
                        } else {
                            goto out_peg_662;
                        }
                    }
                    result_peg_657.setValue((void*) "\r");
                        
                }
                goto success_peg_658;
                out_peg_662:
                goto loop_peg_656;
                success_peg_658:
                ;
                result_peg_466.addResult(result_peg_657);
            } while (true);
            loop_peg_656:
            if (result_peg_466.matches() == 0){
                goto out_peg_468;
            }
        
        
        
        result_peg_466.reset();
            do{
                Result result_peg_665(result_peg_466.getPosition());
                {
                
                    result_peg_665 = rule_section_line(stream, result_peg_665.getPosition(), ast);
                        if (result_peg_665.error()){
                            goto loop_peg_664;
                        }
                    
                    
                    
                    {
                            
                            result_peg_665.reset();
                            do{
                                Result result_peg_762(result_peg_665.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_762.getPosition()))){
                                            result_peg_762.nextPosition();
                                        } else {
                                            goto out_peg_765;
                                        }
                                    }
                                    result_peg_762.setValue((void*) " ");
                                        
                                }
                                goto success_peg_763;
                                out_peg_765:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_762.getPosition()))){
                                            result_peg_762.nextPosition();
                                        } else {
                                            goto out_peg_767;
                                        }
                                    }
                                    result_peg_762.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_763;
                                out_peg_767:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_762.getPosition()))){
                                                        result_peg_762.nextPosition();
                                                    } else {
                                                        goto out_peg_814;
                                                    }
                                                }
                                                result_peg_762.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_762.reset();
                                                do{
                                                    Result result_peg_823(result_peg_762.getPosition());
                                                    {
                                                    
                                                        Result result_peg_826(result_peg_823);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_826.getPosition()))){
                                                                    result_peg_826.nextPosition();
                                                                } else {
                                                                    goto not_peg_825;
                                                                }
                                                            }
                                                            result_peg_826.setValue((void*) "\n");
                                                            goto loop_peg_822;
                                                            not_peg_825:
                                                            result_peg_823.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_827 = stream.get(result_peg_823.getPosition());
                                                            if (temp_peg_827 != '\0'){
                                                                result_peg_823.setValue((void*) temp_peg_827);
                                                                result_peg_823.nextPosition();
                                                            } else {
                                                                goto loop_peg_822;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_762.addResult(result_peg_823);
                                                } while (true);
                                                loop_peg_822:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_811;
                                    out_peg_814:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_762.getPosition()))){
                                                        result_peg_762.nextPosition();
                                                    } else {
                                                        goto out_peg_830;
                                                    }
                                                }
                                                result_peg_762.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_762.reset();
                                                do{
                                                    Result result_peg_839(result_peg_762.getPosition());
                                                    {
                                                    
                                                        Result result_peg_842(result_peg_839);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_842.getPosition()))){
                                                                    result_peg_842.nextPosition();
                                                                } else {
                                                                    goto not_peg_841;
                                                                }
                                                            }
                                                            result_peg_842.setValue((void*) "\n");
                                                            goto loop_peg_838;
                                                            not_peg_841:
                                                            result_peg_839.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_843 = stream.get(result_peg_839.getPosition());
                                                            if (temp_peg_843 != '\0'){
                                                                result_peg_839.setValue((void*) temp_peg_843);
                                                                result_peg_839.nextPosition();
                                                            } else {
                                                                goto loop_peg_838;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_762.addResult(result_peg_839);
                                                } while (true);
                                                loop_peg_838:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_811;
                                    out_peg_830:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_762.getPosition()))){
                                                        result_peg_762.nextPosition();
                                                    } else {
                                                        goto out_peg_846;
                                                    }
                                                }
                                                result_peg_762.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_762.reset();
                                                do{
                                                    Result result_peg_851(result_peg_762.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_851.getPosition()))){
                                                            result_peg_851.nextPosition();
                                                        } else {
                                                            goto loop_peg_850;
                                                        }
                                                    }
                                                    result_peg_851.setValue((void*) "-");
                                                    result_peg_762.addResult(result_peg_851);
                                                } while (true);
                                                loop_peg_850:
                                                if (result_peg_762.matches() == 0){
                                                    goto out_peg_846;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_811;
                                    out_peg_846:
                                    goto out_peg_810;
                                    success_peg_811:
                                    ;
                                    
                                }
                                goto success_peg_763;
                                out_peg_810:
                                goto loop_peg_761;
                                success_peg_763:
                                ;
                                result_peg_665.addResult(result_peg_762);
                            } while (true);
                            loop_peg_761:
                            ;
                                    
                        }
                        goto success_peg_668;
                        goto loop_peg_664;
                        success_peg_668:
                        ;
                    
                    
                    
                    result_peg_665 = rule_line_end(stream, result_peg_665.getPosition());
                        if (result_peg_665.error()){
                            goto loop_peg_664;
                        }
                    
                    
                }
                result_peg_466.addResult(result_peg_665);
            } while (true);
            loop_peg_664:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_466.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_section = result_peg_466;
        stream.update(result_peg_466.getPosition());
    }
    
    
    return result_peg_466;
    out_peg_468:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_section = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_section_line.calculated()){
            return column.chunk1->chunk_section_line;
        }
    }
    
    int myposition = position;
    
    Value data;
    Result result_peg_852(myposition);
    
    {
    
        result_peg_852 = rule_s(stream, result_peg_852.getPosition());
            if (result_peg_852.error()){
                goto out_peg_854;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_852.getPosition()))){
                                result_peg_852.nextPosition();
                            } else {
                                goto out_peg_858;
                            }
                        }
                        result_peg_852.setValue((void*) ";");
                    
                    
                    
                    result_peg_852.reset();
                        do{
                            Result result_peg_867(result_peg_852.getPosition());
                            {
                            
                                Result result_peg_870(result_peg_867);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_870.getPosition()))){
                                            result_peg_870.nextPosition();
                                        } else {
                                            goto not_peg_869;
                                        }
                                    }
                                    result_peg_870.setValue((void*) "\n");
                                    goto loop_peg_866;
                                    not_peg_869:
                                    result_peg_867.setValue((void*)0);
                                
                                
                                
                                char temp_peg_871 = stream.get(result_peg_867.getPosition());
                                    if (temp_peg_871 != '\0'){
                                        result_peg_867.setValue((void*) temp_peg_871);
                                        result_peg_867.nextPosition();
                                    } else {
                                        goto loop_peg_866;
                                    }
                                
                                
                            }
                            result_peg_852.addResult(result_peg_867);
                        } while (true);
                        loop_peg_866:
                        ;
                    
                    
                }
            }
            goto success_peg_855;
            out_peg_858:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_852.getPosition()))){
                                result_peg_852.nextPosition();
                            } else {
                                goto out_peg_874;
                            }
                        }
                        result_peg_852.setValue((void*) "=");
                    
                    
                    
                    result_peg_852.reset();
                        do{
                            Result result_peg_883(result_peg_852.getPosition());
                            {
                            
                                Result result_peg_886(result_peg_883);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_886.getPosition()))){
                                            result_peg_886.nextPosition();
                                        } else {
                                            goto not_peg_885;
                                        }
                                    }
                                    result_peg_886.setValue((void*) "\n");
                                    goto loop_peg_882;
                                    not_peg_885:
                                    result_peg_883.setValue((void*)0);
                                
                                
                                
                                char temp_peg_887 = stream.get(result_peg_883.getPosition());
                                    if (temp_peg_887 != '\0'){
                                        result_peg_883.setValue((void*) temp_peg_887);
                                        result_peg_883.nextPosition();
                                    } else {
                                        goto loop_peg_882;
                                    }
                                
                                
                            }
                            result_peg_852.addResult(result_peg_883);
                        } while (true);
                        loop_peg_882:
                        ;
                    
                    
                }
            }
            goto success_peg_855;
            out_peg_874:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_852.getPosition()))){
                                result_peg_852.nextPosition();
                            } else {
                                goto out_peg_890;
                            }
                        }
                        result_peg_852.setValue((void*) "-");
                    
                    
                    
                    result_peg_852.reset();
                        do{
                            Result result_peg_895(result_peg_852.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_895.getPosition()))){
                                    result_peg_895.nextPosition();
                                } else {
                                    goto loop_peg_894;
                                }
                            }
                            result_peg_895.setValue((void*) "-");
                            result_peg_852.addResult(result_peg_895);
                        } while (true);
                        loop_peg_894:
                        if (result_peg_852.matches() == 0){
                            goto out_peg_890;
                        }
                    
                    
                }
            }
            goto success_peg_855;
            out_peg_890:
            goto out_peg_854;
            success_peg_855:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_852;
        stream.update(result_peg_852.getPosition());
    }
    
    
    return result_peg_852;
    out_peg_854:
    Result result_peg_896(myposition);
    
    {
    
        result_peg_896 = rule_s(stream, result_peg_896.getPosition());
            if (result_peg_896.error()){
                goto out_peg_898;
            }
        
        
        
        result_peg_896 = rule_attribute(stream, result_peg_896.getPosition());
            if (result_peg_896.error()){
                goto out_peg_898;
            }
            data = result_peg_896.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addAttribute(asAttribute(data));
                result_peg_896.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_896;
        stream.update(result_peg_896.getPosition());
    }
    
    
    return result_peg_896;
    out_peg_898:
    Result result_peg_900(myposition);
    
    {
    
        result_peg_900 = rule_s(stream, result_peg_900.getPosition());
            if (result_peg_900.error()){
                goto out_peg_902;
            }
        
        
        
        result_peg_900 = rule_valuelist(stream, result_peg_900.getPosition());
            if (result_peg_900.error()){
                goto out_peg_902;
            }
            data = result_peg_900.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_900.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_900;
        stream.update(result_peg_900.getPosition());
    }
    
    
    return result_peg_900;
    out_peg_902:
    Result result_peg_904(myposition);
    
    {
    
        result_peg_904 = rule_s(stream, result_peg_904.getPosition());
            if (result_peg_904.error()){
                goto out_peg_906;
            }
        
        
        
        result_peg_904 = rule_loopstart(stream, result_peg_904.getPosition());
            if (result_peg_904.error()){
                goto out_peg_906;
            }
            data = result_peg_904.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_904.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_904;
        stream.update(result_peg_904.getPosition());
    }
    
    
    return result_peg_904;
    out_peg_906:
    Result result_peg_908(myposition);
    
    {
    
        result_peg_908.reset();
            do{
                Result result_peg_911(result_peg_908.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_911.getPosition()))){
                            result_peg_911.nextPosition();
                        } else {
                            goto out_peg_914;
                        }
                    }
                    result_peg_911.setValue((void*) " ");
                        
                }
                goto success_peg_912;
                out_peg_914:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_911.getPosition()))){
                            result_peg_911.nextPosition();
                        } else {
                            goto out_peg_916;
                        }
                    }
                    result_peg_911.setValue((void*) "\t");
                        
                }
                goto success_peg_912;
                out_peg_916:
                goto loop_peg_910;
                success_peg_912:
                ;
                result_peg_908.addResult(result_peg_911);
            } while (true);
            loop_peg_910:
            if (result_peg_908.matches() == 0){
                goto out_peg_917;
            }
        
        
        
        Result result_peg_919(result_peg_908);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_919.getPosition()))){
                    result_peg_919.nextPosition();
                } else {
                    goto not_peg_918;
                }
            }
            result_peg_919.setValue((void*) "[");
            goto out_peg_917;
            not_peg_918:
            result_peg_908.setValue((void*)0);
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = result_peg_908;
        stream.update(result_peg_908.getPosition());
    }
    
    
    return result_peg_908;
    out_peg_917:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_line = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_section_start.calculated()){
            return column.chunk1->chunk_section_start;
        }
    }
    
    int myposition = position;
    
    Value data;
    Result result_peg_920(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_920.getPosition()))){
                    result_peg_920.nextPosition();
                } else {
                    goto out_peg_922;
                }
            }
            result_peg_920.setValue((void*) "[");
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        result_peg_920.reset();
            do{
                Result result_peg_926(result_peg_920.getPosition());
                {
                
                    Result result_peg_929(result_peg_926);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_929.getPosition()))){
                                result_peg_929.nextPosition();
                            } else {
                                goto not_peg_928;
                            }
                        }
                        result_peg_929.setValue((void*) "]");
                        goto loop_peg_925;
                        not_peg_928:
                        result_peg_926.setValue((void*)0);
                    
                    
                    
                    char temp_peg_930 = stream.get(result_peg_926.getPosition());
                        if (temp_peg_930 != '\0'){
                            result_peg_926.setValue((void*) temp_peg_930);
                            result_peg_926.nextPosition();
                        } else {
                            goto loop_peg_925;
                        }
                    
                    
                }
                result_peg_920.addResult(result_peg_926);
            } while (true);
            loop_peg_925:
            if (result_peg_920.matches() == 0){
                goto out_peg_922;
            }
            data = result_peg_920.getValues();
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_920.getPosition()))){
                    result_peg_920.nextPosition();
                } else {
                    goto out_peg_922;
                }
            }
            result_peg_920.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(data); GC::save(as<std::string*>(value));
                result_peg_920.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_start = result_peg_920;
        stream.update(result_peg_920.getPosition());
    }
    
    
    return result_peg_920;
    out_peg_922:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_section_start = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_loopstart.calculated()){
            return column.chunk1->chunk_loopstart;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_933(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_933.getPosition()))){
                    result_peg_933.nextPosition();
                } else {
                    goto out_peg_935;
                }
            }
            result_peg_933.setValue((void*) "loopstart");
        
        
        
        {
                Value value((void*) 0);
                value = makeValue(); GC::save(as<Ast::Value*>(value));
                result_peg_933.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = result_peg_933;
        stream.update(result_peg_933.getPosition());
    }
    
    
    return result_peg_933;
    out_peg_935:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_name.calculated()){
            return column.chunk1->chunk_name;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_936(myposition);
    
    {
    
        {
                
                char letter_peg_942 = stream.get(result_peg_936.getPosition());
                if (letter_peg_942 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_942) != NULL){
                    result_peg_936.nextPosition();
                    result_peg_936.setValue((void*) letter_peg_942);
                } else {
                    goto out_peg_941;
                }
                
            }
            goto success_peg_938;
            out_peg_941:
            goto out_peg_943;
            success_peg_938:
            ;
        
        Result result_peg_937 = result_peg_936;
        
        result_peg_936.reset();
            do{
                Result result_peg_946(result_peg_936.getPosition());
                result_peg_946 = rule_alpha_digit(stream, result_peg_946.getPosition());
                if (result_peg_946.error()){
                    goto loop_peg_945;
                }
                result_peg_936.addResult(result_peg_946);
            } while (true);
            loop_peg_945:
            ;
        
        Result result_peg_944 = result_peg_936;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_937.getValues().getValue(),result_peg_944.getValues()); GC::save(as<std::string*>(value));
                result_peg_936.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_name = result_peg_936;
        stream.update(result_peg_936.getPosition());
    }
    
    
    return result_peg_936;
    out_peg_943:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_alpha_digit.calculated()){
            return column.chunk1->chunk_alpha_digit;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_947(myposition);
    
    {
        
        char letter_peg_952 = stream.get(result_peg_947.getPosition());
        if (letter_peg_952 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_952) != NULL){
            result_peg_947.nextPosition();
            result_peg_947.setValue((void*) letter_peg_952);
        } else {
            goto out_peg_951;
        }
        
    }
    goto success_peg_948;
    out_peg_951:
    goto out_peg_953;
    success_peg_948:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_alpha_digit = result_peg_947;
        stream.update(result_peg_947.getPosition());
    }
    
    
    return result_peg_947;
    out_peg_953:
    Result result_peg_954(myposition);
    
    {
        
        char letter_peg_959 = stream.get(result_peg_954.getPosition());
        if (letter_peg_959 != '\0' && strchr("0123456789", letter_peg_959) != NULL){
            result_peg_954.nextPosition();
            result_peg_954.setValue((void*) letter_peg_959);
        } else {
            goto out_peg_958;
        }
        
    }
    goto success_peg_955;
    out_peg_958:
    goto out_peg_960;
    success_peg_955:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_alpha_digit = result_peg_954;
        stream.update(result_peg_954.getPosition());
    }
    
    
    return result_peg_954;
    out_peg_960:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_alpha_digit = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_line_end_or_comment.calculated()){
            return column.chunk2->chunk_line_end_or_comment;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_961(myposition);
    
    result_peg_961 = rule_line_end(stream, result_peg_961.getPosition());
    if (result_peg_961.error()){
        goto out_peg_962;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = result_peg_961;
        stream.update(result_peg_961.getPosition());
    }
    
    
    return result_peg_961;
    out_peg_962:
    Result result_peg_963(myposition);
    
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_963.getPosition()))){
                        result_peg_963.nextPosition();
                    } else {
                        goto out_peg_967;
                    }
                }
                result_peg_963.setValue((void*) ";");
            
            
            
            result_peg_963.reset();
                do{
                    Result result_peg_976(result_peg_963.getPosition());
                    {
                    
                        Result result_peg_979(result_peg_976);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_979.getPosition()))){
                                    result_peg_979.nextPosition();
                                } else {
                                    goto not_peg_978;
                                }
                            }
                            result_peg_979.setValue((void*) "\n");
                            goto loop_peg_975;
                            not_peg_978:
                            result_peg_976.setValue((void*)0);
                        
                        
                        
                        char temp_peg_980 = stream.get(result_peg_976.getPosition());
                            if (temp_peg_980 != '\0'){
                                result_peg_976.setValue((void*) temp_peg_980);
                                result_peg_976.nextPosition();
                            } else {
                                goto loop_peg_975;
                            }
                        
                        
                    }
                    result_peg_963.addResult(result_peg_976);
                } while (true);
                loop_peg_975:
                ;
            
            
        }
    }
    goto success_peg_964;
    out_peg_967:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_963.getPosition()))){
                        result_peg_963.nextPosition();
                    } else {
                        goto out_peg_983;
                    }
                }
                result_peg_963.setValue((void*) "=");
            
            
            
            result_peg_963.reset();
                do{
                    Result result_peg_992(result_peg_963.getPosition());
                    {
                    
                        Result result_peg_995(result_peg_992);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_995.getPosition()))){
                                    result_peg_995.nextPosition();
                                } else {
                                    goto not_peg_994;
                                }
                            }
                            result_peg_995.setValue((void*) "\n");
                            goto loop_peg_991;
                            not_peg_994:
                            result_peg_992.setValue((void*)0);
                        
                        
                        
                        char temp_peg_996 = stream.get(result_peg_992.getPosition());
                            if (temp_peg_996 != '\0'){
                                result_peg_992.setValue((void*) temp_peg_996);
                                result_peg_992.nextPosition();
                            } else {
                                goto loop_peg_991;
                            }
                        
                        
                    }
                    result_peg_963.addResult(result_peg_992);
                } while (true);
                loop_peg_991:
                ;
            
            
        }
    }
    goto success_peg_964;
    out_peg_983:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_963.getPosition()))){
                        result_peg_963.nextPosition();
                    } else {
                        goto out_peg_999;
                    }
                }
                result_peg_963.setValue((void*) "-");
            
            
            
            result_peg_963.reset();
                do{
                    Result result_peg_1004(result_peg_963.getPosition());
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_1004.getPosition()))){
                            result_peg_1004.nextPosition();
                        } else {
                            goto loop_peg_1003;
                        }
                    }
                    result_peg_1004.setValue((void*) "-");
                    result_peg_963.addResult(result_peg_1004);
                } while (true);
                loop_peg_1003:
                if (result_peg_963.matches() == 0){
                    goto out_peg_999;
                }
            
            
        }
    }
    goto success_peg_964;
    out_peg_999:
    goto out_peg_1005;
    success_peg_964:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = result_peg_963;
        stream.update(result_peg_963.getPosition());
    }
    
    
    return result_peg_963;
    out_peg_1005:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_line_end_or_comment = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_filename.calculated()){
            return column.chunk2->chunk_filename;
        }
    }
    
    int myposition = position;
    
    Value file;
    Result result_peg_1006(myposition);
    
    {
    
        Result result_peg_1009(result_peg_1006);
            for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1009.getPosition()))){
                    result_peg_1009.nextPosition();
                } else {
                    goto not_peg_1008;
                }
            }
            result_peg_1009.setValue((void*) "\"");
            goto out_peg_1010;
            not_peg_1008:
            result_peg_1006.setValue((void*)0);
        
        
        
        result_peg_1006.reset();
            do{
                Result result_peg_1013(result_peg_1006.getPosition());
                result_peg_1013 = rule_filename_char(stream, result_peg_1013.getPosition());
                if (result_peg_1013.error()){
                    goto loop_peg_1012;
                }
                result_peg_1006.addResult(result_peg_1013);
            } while (true);
            loop_peg_1012:
            if (result_peg_1006.matches() == 0){
                goto out_peg_1010;
            }
            file = result_peg_1006.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new Ast::Filename(toString(file)); GC::save(as<Ast::Filename*>(value));
                result_peg_1006.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename = result_peg_1006;
        stream.update(result_peg_1006.getPosition());
    }
    
    
    return result_peg_1006;
    out_peg_1010:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_filename_char(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_filename_char.calculated()){
            return column.chunk2->chunk_filename_char;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1014(myposition);
    
    {
    
        Result result_peg_1017(result_peg_1014);
            for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1017.getPosition()))){
                    result_peg_1017.nextPosition();
                } else {
                    goto not_peg_1016;
                }
            }
            result_peg_1017.setValue((void*) ",");
            goto out_peg_1018;
            not_peg_1016:
            result_peg_1014.setValue((void*)0);
        
        
        
        Result result_peg_1021(result_peg_1014);
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_1021.getPosition()))){
                    result_peg_1021.nextPosition();
                } else {
                    goto not_peg_1020;
                }
            }
            result_peg_1021.setValue((void*) "\n");
            goto out_peg_1018;
            not_peg_1020:
            result_peg_1014.setValue((void*)0);
        
        
        
        Result result_peg_1024(result_peg_1014);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_1024.getPosition()))){
                    result_peg_1024.nextPosition();
                } else {
                    goto not_peg_1023;
                }
            }
            result_peg_1024.setValue((void*) "[");
            goto out_peg_1018;
            not_peg_1023:
            result_peg_1014.setValue((void*)0);
        
        
        
        Result result_peg_1027(result_peg_1014);
            if ((unsigned char) stream.get(result_peg_1027.getPosition()) == (unsigned char) 13){
                result_peg_1027.nextPosition();
            } else {
                goto not_peg_1026;
            }
            result_peg_1027.setValue((void*) 13);
            goto out_peg_1018;
            not_peg_1026:
            result_peg_1014.setValue((void*)0);
        
        
        
        Result result_peg_1030(result_peg_1014);
            for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_1030.getPosition()))){
                    result_peg_1030.nextPosition();
                } else {
                    goto not_peg_1029;
                }
            }
            result_peg_1030.setValue((void*) ";");
            goto out_peg_1018;
            not_peg_1029:
            result_peg_1014.setValue((void*)0);
        
        
        
        char temp_peg_1031 = stream.get(result_peg_1014.getPosition());
            if (temp_peg_1031 != '\0'){
                result_peg_1014.setValue((void*) temp_peg_1031);
                result_peg_1014.nextPosition();
            } else {
                goto out_peg_1018;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename_char = result_peg_1014;
        stream.update(result_peg_1014.getPosition());
    }
    
    
    return result_peg_1014;
    out_peg_1018:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_filename_char = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_attribute.calculated()){
            return column.chunk2->chunk_attribute;
        }
    }
    
    int myposition = position;
    
    Value id;
    Value data;
    Value index;
    Result result_peg_1032(myposition);
    
    {
    
        result_peg_1032 = rule_identifier(stream, result_peg_1032.getPosition());
            if (result_peg_1032.error()){
                goto out_peg_1034;
            }
            id = result_peg_1032.getValues();
        
        
        
        result_peg_1032 = rule_s(stream, result_peg_1032.getPosition());
            if (result_peg_1032.error()){
                goto out_peg_1034;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1032.getPosition()))){
                    result_peg_1032.nextPosition();
                } else {
                    goto out_peg_1034;
                }
            }
            result_peg_1032.setValue((void*) "=");
        
        
        
        result_peg_1032 = rule_s(stream, result_peg_1032.getPosition());
            if (result_peg_1032.error()){
                goto out_peg_1034;
            }
        
        
        
        Result result_peg_1039(result_peg_1032.getPosition());
            result_peg_1039 = rule_line_end_or_comment(stream, result_peg_1039.getPosition());
            if (result_peg_1039.error()){
                goto out_peg_1034;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id); GC::save(as<Ast::Attribute*>(value));
                result_peg_1032.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1032;
        stream.update(result_peg_1032.getPosition());
    }
    
    
    return result_peg_1032;
    out_peg_1034:
    Result result_peg_1040(myposition);
    
    {
    
        result_peg_1040 = rule_identifier(stream, result_peg_1040.getPosition());
            if (result_peg_1040.error()){
                goto out_peg_1042;
            }
            id = result_peg_1040.getValues();
        
        
        
        result_peg_1040 = rule_s(stream, result_peg_1040.getPosition());
            if (result_peg_1040.error()){
                goto out_peg_1042;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1040.getPosition()))){
                    result_peg_1040.nextPosition();
                } else {
                    goto out_peg_1042;
                }
            }
            result_peg_1040.setValue((void*) "=");
        
        
        
        result_peg_1040 = rule_s(stream, result_peg_1040.getPosition());
            if (result_peg_1040.error()){
                goto out_peg_1042;
            }
        
        
        
        result_peg_1040 = rule_valuelist(stream, result_peg_1040.getPosition());
            if (result_peg_1040.error()){
                goto out_peg_1042;
            }
            data = result_peg_1040.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id, data); GC::save(as<Ast::Attribute*>(value));
                result_peg_1040.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1040;
        stream.update(result_peg_1040.getPosition());
    }
    
    
    return result_peg_1040;
    out_peg_1042:
    Result result_peg_1047(myposition);
    
    {
    
        result_peg_1047 = rule_identifier(stream, result_peg_1047.getPosition());
            if (result_peg_1047.error()){
                goto out_peg_1049;
            }
            id = result_peg_1047.getValues();
        
        
        
        result_peg_1047 = rule_s(stream, result_peg_1047.getPosition());
            if (result_peg_1047.error()){
                goto out_peg_1049;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1047.getPosition()))){
                    result_peg_1047.nextPosition();
                } else {
                    goto out_peg_1049;
                }
            }
            result_peg_1047.setValue((void*) "=");
        
        
        
        result_peg_1047 = rule_s(stream, result_peg_1047.getPosition());
            if (result_peg_1047.error()){
                goto out_peg_1049;
            }
        
        
        
        result_peg_1047 = rule_filename(stream, result_peg_1047.getPosition());
            if (result_peg_1047.error()){
                goto out_peg_1049;
            }
            data = result_peg_1047.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeFilename(id, data); GC::save(as<Ast::Attribute*>(value));
                result_peg_1047.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1047;
        stream.update(result_peg_1047.getPosition());
    }
    
    
    return result_peg_1047;
    out_peg_1049:
    Result result_peg_1054(myposition);
    
    {
    
        result_peg_1054 = rule_identifier(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
            id = result_peg_1054.getValues();
        
        
        
        result_peg_1054 = rule_s(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1054.getPosition()))){
                    result_peg_1054.nextPosition();
                } else {
                    goto out_peg_1056;
                }
            }
            result_peg_1054.setValue((void*) "(");
        
        
        
        result_peg_1054 = rule_s(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
        
        
        
        result_peg_1054 = rule_number(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
            index = result_peg_1054.getValues();
        
        
        
        result_peg_1054 = rule_s(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1054.getPosition()))){
                    result_peg_1054.nextPosition();
                } else {
                    goto out_peg_1056;
                }
            }
            result_peg_1054.setValue((void*) ")");
        
        
        
        result_peg_1054 = rule_s(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1054.getPosition()))){
                    result_peg_1054.nextPosition();
                } else {
                    goto out_peg_1056;
                }
            }
            result_peg_1054.setValue((void*) "=");
        
        
        
        result_peg_1054 = rule_s(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
        
        
        
        result_peg_1054 = rule_valuelist(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
            data = result_peg_1054.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(id, index, data); GC::save(as<Ast::Value*>(value));
                result_peg_1054.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1054;
        stream.update(result_peg_1054.getPosition());
    }
    
    
    return result_peg_1054;
    out_peg_1056:
    Result result_peg_1067(myposition);
    
    {
    
        result_peg_1067 = rule_identifier_list(stream, result_peg_1067.getPosition());
            if (result_peg_1067.error()){
                goto out_peg_1069;
            }
            id = result_peg_1067.getValues();
        
        
        
        result_peg_1067 = rule_s(stream, result_peg_1067.getPosition());
            if (result_peg_1067.error()){
                goto out_peg_1069;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1067.getPosition()))){
                    result_peg_1067.nextPosition();
                } else {
                    goto out_peg_1069;
                }
            }
            result_peg_1067.setValue((void*) "=");
        
        
        
        result_peg_1067 = rule_s(stream, result_peg_1067.getPosition());
            if (result_peg_1067.error()){
                goto out_peg_1069;
            }
        
        
        
        result_peg_1067 = rule_valuelist(stream, result_peg_1067.getPosition());
            if (result_peg_1067.error()){
                goto out_peg_1069;
            }
            data = result_peg_1067.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributes(id, data); GC::save(as<Ast::Attribute*>(value));
                result_peg_1067.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = result_peg_1067;
        stream.update(result_peg_1067.getPosition());
    }
    
    
    return result_peg_1067;
    out_peg_1069:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_attribute = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_identifier.calculated()){
            return column.chunk2->chunk_identifier;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1074(myposition);
    
    {
    
        result_peg_1074 = rule_name(stream, result_peg_1074.getPosition());
            if (result_peg_1074.error()){
                goto out_peg_1076;
            }
        
        Result result_peg_1075 = result_peg_1074;
        
        result_peg_1074.reset();
            do{
                Result result_peg_1079(result_peg_1074.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_1079.getPosition()))){
                                result_peg_1079.nextPosition();
                            } else {
                                goto loop_peg_1078;
                            }
                        }
                        result_peg_1079.setValue((void*) ".");
                    
                    
                    
                    result_peg_1079 = rule_name(stream, result_peg_1079.getPosition());
                        if (result_peg_1079.error()){
                            goto loop_peg_1078;
                        }
                    
                    
                }
                result_peg_1074.addResult(result_peg_1079);
            } while (true);
            loop_peg_1078:
            ;
        
        Result result_peg_1077 = result_peg_1074;
        
        {
                Value value((void*) 0);
                value = makeIdentifier(result_peg_1075.getValues(),result_peg_1077.getValues()); GC::save(as<Ast::Identifier*>(value));
                result_peg_1074.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_identifier = result_peg_1074;
        stream.update(result_peg_1074.getPosition());
    }
    
    
    return result_peg_1074;
    out_peg_1076:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_identifier_list.calculated()){
            return column.chunk3->chunk_identifier_list;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1081(myposition);
    
    {
    
        result_peg_1081 = rule_identifier(stream, result_peg_1081.getPosition());
            if (result_peg_1081.error()){
                goto out_peg_1083;
            }
        
        
        
        result_peg_1081.reset();
            do{
                Result result_peg_1085(result_peg_1081.getPosition());
                {
                
                    result_peg_1085 = rule_s(stream, result_peg_1085.getPosition());
                        if (result_peg_1085.error()){
                            goto loop_peg_1084;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1085.getPosition()))){
                                result_peg_1085.nextPosition();
                            } else {
                                goto loop_peg_1084;
                            }
                        }
                        result_peg_1085.setValue((void*) ",");
                    
                    
                    
                    result_peg_1085 = rule_s(stream, result_peg_1085.getPosition());
                        if (result_peg_1085.error()){
                            goto loop_peg_1084;
                        }
                    
                    
                    
                    result_peg_1085 = rule_filename(stream, result_peg_1085.getPosition());
                        if (result_peg_1085.error()){
                            goto loop_peg_1084;
                        }
                    
                    
                }
                result_peg_1081.addResult(result_peg_1085);
            } while (true);
            loop_peg_1084:
            if (result_peg_1081.matches() == 0){
                goto out_peg_1083;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier_list = result_peg_1081;
        stream.update(result_peg_1081.getPosition());
    }
    
    
    return result_peg_1081;
    out_peg_1083:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_identifier_list = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_valuelist.calculated()){
            return column.chunk3->chunk_valuelist;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1089(myposition);
    
    {
    
        result_peg_1089 = rule_value(stream, result_peg_1089.getPosition());
            if (result_peg_1089.error()){
                goto out_peg_1091;
            }
        
        Result result_peg_1090 = result_peg_1089;
        
        result_peg_1089.reset();
            do{
                Result result_peg_1094(result_peg_1089.getPosition());
                {
                
                    result_peg_1094 = rule_s(stream, result_peg_1094.getPosition());
                        if (result_peg_1094.error()){
                            goto loop_peg_1093;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1094.getPosition()))){
                                result_peg_1094.nextPosition();
                            } else {
                                goto loop_peg_1093;
                            }
                        }
                        result_peg_1094.setValue((void*) ",");
                    
                    
                    
                    result_peg_1094 = rule_s(stream, result_peg_1094.getPosition());
                        if (result_peg_1094.error()){
                            goto loop_peg_1093;
                        }
                    
                    
                    
                    int save_peg_1098 = result_peg_1094.getPosition();
                        
                        result_peg_1094 = rule_value(stream, result_peg_1094.getPosition());
                        if (result_peg_1094.error()){
                            
                            result_peg_1094 = Result(save_peg_1098);
                            result_peg_1094.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1089.addResult(result_peg_1094);
            } while (true);
            loop_peg_1093:
            ;
        
        Result result_peg_1092 = result_peg_1089;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_1090.getValues(),result_peg_1092.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1089.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_1089;
        stream.update(result_peg_1089.getPosition());
    }
    
    
    return result_peg_1089;
    out_peg_1091:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_value.calculated()){
            return column.chunk3->chunk_value;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1099(myposition);
    
    result_peg_1099 = rule_string(stream, result_peg_1099.getPosition());
    if (result_peg_1099.error()){
        goto out_peg_1100;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1099;
        stream.update(result_peg_1099.getPosition());
    }
    
    
    return result_peg_1099;
    out_peg_1100:
    Result result_peg_1101(myposition);
    
    result_peg_1101 = rule_date(stream, result_peg_1101.getPosition());
    if (result_peg_1101.error()){
        goto out_peg_1102;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1101;
        stream.update(result_peg_1101.getPosition());
    }
    
    
    return result_peg_1101;
    out_peg_1102:
    Result result_peg_1103(myposition);
    
    result_peg_1103 = rule_number(stream, result_peg_1103.getPosition());
    if (result_peg_1103.error()){
        goto out_peg_1104;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1103;
        stream.update(result_peg_1103.getPosition());
    }
    
    
    return result_peg_1103;
    out_peg_1104:
    Result result_peg_1105(myposition);
    
    {
    
        result_peg_1105 = rule_keyword(stream, result_peg_1105.getPosition());
            if (result_peg_1105.error()){
                goto out_peg_1107;
            }
        
        Result result_peg_1106 = result_peg_1105;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1106.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1105.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1105;
        stream.update(result_peg_1105.getPosition());
    }
    
    
    return result_peg_1105;
    out_peg_1107:
    Result result_peg_1108(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_1108.getPosition()))){
                    result_peg_1108.nextPosition();
                } else {
                    goto out_peg_1110;
                }
            }
            result_peg_1108.setValue((void*) "s");
        
        Result result_peg_1109 = result_peg_1108;
        
        Result result_peg_1113(result_peg_1108);
            {
                
                char letter_peg_1118 = stream.get(result_peg_1113.getPosition());
                if (letter_peg_1118 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1118) != NULL){
                    result_peg_1113.nextPosition();
                    result_peg_1113.setValue((void*) letter_peg_1118);
                } else {
                    goto out_peg_1117;
                }
                
            }
            goto success_peg_1114;
            out_peg_1117:
            goto not_peg_1112;
            success_peg_1114:
            ;
            goto out_peg_1110;
            not_peg_1112:
            result_peg_1108.setValue((void*)0);
        
        
        
        Result result_peg_1121(result_peg_1108);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1121.getPosition()))){
                    result_peg_1121.nextPosition();
                } else {
                    goto not_peg_1120;
                }
            }
            result_peg_1121.setValue((void*) ".");
            goto out_peg_1110;
            not_peg_1120:
            result_peg_1108.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1109.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1108.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1108;
        stream.update(result_peg_1108.getPosition());
    }
    
    
    return result_peg_1108;
    out_peg_1110:
    Result result_peg_1122(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_1122.getPosition()))){
                    result_peg_1122.nextPosition();
                } else {
                    goto out_peg_1124;
                }
            }
            result_peg_1122.setValue((void*) "h");
        
        Result result_peg_1123 = result_peg_1122;
        
        Result result_peg_1127(result_peg_1122);
            {
                
                char letter_peg_1132 = stream.get(result_peg_1127.getPosition());
                if (letter_peg_1132 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1132) != NULL){
                    result_peg_1127.nextPosition();
                    result_peg_1127.setValue((void*) letter_peg_1132);
                } else {
                    goto out_peg_1131;
                }
                
            }
            goto success_peg_1128;
            out_peg_1131:
            goto not_peg_1126;
            success_peg_1128:
            ;
            goto out_peg_1124;
            not_peg_1126:
            result_peg_1122.setValue((void*)0);
        
        
        
        Result result_peg_1135(result_peg_1122);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1135.getPosition()))){
                    result_peg_1135.nextPosition();
                } else {
                    goto not_peg_1134;
                }
            }
            result_peg_1135.setValue((void*) ".");
            goto out_peg_1124;
            not_peg_1134:
            result_peg_1122.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1123.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1122.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1122;
        stream.update(result_peg_1122.getPosition());
    }
    
    
    return result_peg_1122;
    out_peg_1124:
    Result result_peg_1136(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1136.getPosition()))){
                    result_peg_1136.nextPosition();
                } else {
                    goto out_peg_1138;
                }
            }
            result_peg_1136.setValue((void*) "a");
        
        Result result_peg_1137 = result_peg_1136;
        
        int save_peg_1140 = result_peg_1136.getPosition();
            
            result_peg_1136 = rule_number(stream, result_peg_1136.getPosition());
            if (result_peg_1136.error()){
                
                result_peg_1136 = Result(save_peg_1140);
                result_peg_1136.setValue((void*) 0);
                
            }
        
        
        
        Result result_peg_1143(result_peg_1136);
            {
                
                char letter_peg_1148 = stream.get(result_peg_1143.getPosition());
                if (letter_peg_1148 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1148) != NULL){
                    result_peg_1143.nextPosition();
                    result_peg_1143.setValue((void*) letter_peg_1148);
                } else {
                    goto out_peg_1147;
                }
                
            }
            goto success_peg_1144;
            out_peg_1147:
            goto not_peg_1142;
            success_peg_1144:
            ;
            goto out_peg_1138;
            not_peg_1142:
            result_peg_1136.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1137.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1136.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1136;
        stream.update(result_peg_1136.getPosition());
    }
    
    
    return result_peg_1136;
    out_peg_1138:
    Result result_peg_1149(myposition);
    
    result_peg_1149 = rule_filename(stream, result_peg_1149.getPosition());
    if (result_peg_1149.error()){
        goto out_peg_1150;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_1149;
        stream.update(result_peg_1149.getPosition());
    }
    
    
    return result_peg_1149;
    out_peg_1150:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_keyword.calculated()){
            return column.chunk3->chunk_keyword;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1151(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareChar("normal"[i], stream.get(result_peg_1151.getPosition()))){
            result_peg_1151.nextPosition();
        } else {
            goto out_peg_1152;
        }
    }
    result_peg_1151.setValue((void*) "normal");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1151;
        stream.update(result_peg_1151.getPosition());
    }
    
    
    return result_peg_1151;
    out_peg_1152:
    Result result_peg_1153(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("parallax"[i], stream.get(result_peg_1153.getPosition()))){
            result_peg_1153.nextPosition();
        } else {
            goto out_peg_1154;
        }
    }
    result_peg_1153.setValue((void*) "parallax");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1153;
        stream.update(result_peg_1153.getPosition());
    }
    
    
    return result_peg_1153;
    out_peg_1154:
    Result result_peg_1155(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("addalpha"[i], stream.get(result_peg_1155.getPosition()))){
            result_peg_1155.nextPosition();
        } else {
            goto out_peg_1156;
        }
    }
    result_peg_1155.setValue((void*) "addalpha");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1155;
        stream.update(result_peg_1155.getPosition());
    }
    
    
    return result_peg_1155;
    out_peg_1156:
    Result result_peg_1157(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareChar("add1"[i], stream.get(result_peg_1157.getPosition()))){
            result_peg_1157.nextPosition();
        } else {
            goto out_peg_1158;
        }
    }
    result_peg_1157.setValue((void*) "add1");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1157;
        stream.update(result_peg_1157.getPosition());
    }
    
    
    return result_peg_1157;
    out_peg_1158:
    Result result_peg_1159(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("add"[i], stream.get(result_peg_1159.getPosition()))){
            result_peg_1159.nextPosition();
        } else {
            goto out_peg_1160;
        }
    }
    result_peg_1159.setValue((void*) "add");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1159;
        stream.update(result_peg_1159.getPosition());
    }
    
    
    return result_peg_1159;
    out_peg_1160:
    Result result_peg_1161(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("sub"[i], stream.get(result_peg_1161.getPosition()))){
            result_peg_1161.nextPosition();
        } else {
            goto out_peg_1162;
        }
    }
    result_peg_1161.setValue((void*) "sub");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = result_peg_1161;
        stream.update(result_peg_1161.getPosition());
    }
    
    
    return result_peg_1161;
    out_peg_1162:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_date.calculated()){
            return column.chunk3->chunk_date;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1163(myposition);
    
    {
    
        result_peg_1163.reset();
            do{
                Result result_peg_1166(result_peg_1163.getPosition());
                {
                    
                    char letter_peg_1171 = stream.get(result_peg_1166.getPosition());
                    if (letter_peg_1171 != '\0' && strchr("0123456789", letter_peg_1171) != NULL){
                        result_peg_1166.nextPosition();
                        result_peg_1166.setValue((void*) letter_peg_1171);
                    } else {
                        goto out_peg_1170;
                    }
                    
                }
                goto success_peg_1167;
                out_peg_1170:
                goto loop_peg_1165;
                success_peg_1167:
                ;
                result_peg_1163.addResult(result_peg_1166);
            } while (true);
            loop_peg_1165:
            if (result_peg_1163.matches() == 0){
                goto out_peg_1172;
            }
        
        Result result_peg_1164 = result_peg_1163;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1163.getPosition()))){
                    result_peg_1163.nextPosition();
                } else {
                    goto out_peg_1172;
                }
            }
            result_peg_1163.setValue((void*) ".");
        
        
        
        result_peg_1163.reset();
            do{
                Result result_peg_1176(result_peg_1163.getPosition());
                {
                    
                    char letter_peg_1181 = stream.get(result_peg_1176.getPosition());
                    if (letter_peg_1181 != '\0' && strchr("0123456789", letter_peg_1181) != NULL){
                        result_peg_1176.nextPosition();
                        result_peg_1176.setValue((void*) letter_peg_1181);
                    } else {
                        goto out_peg_1180;
                    }
                    
                }
                goto success_peg_1177;
                out_peg_1180:
                goto loop_peg_1175;
                success_peg_1177:
                ;
                result_peg_1163.addResult(result_peg_1176);
            } while (true);
            loop_peg_1175:
            if (result_peg_1163.matches() == 0){
                goto out_peg_1172;
            }
        
        Result result_peg_1174 = result_peg_1163;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1163.getPosition()))){
                    result_peg_1163.nextPosition();
                } else {
                    goto out_peg_1172;
                }
            }
            result_peg_1163.setValue((void*) ".");
        
        
        
        result_peg_1163.reset();
            do{
                Result result_peg_1185(result_peg_1163.getPosition());
                {
                    
                    char letter_peg_1190 = stream.get(result_peg_1185.getPosition());
                    if (letter_peg_1190 != '\0' && strchr("0123456789", letter_peg_1190) != NULL){
                        result_peg_1185.nextPosition();
                        result_peg_1185.setValue((void*) letter_peg_1190);
                    } else {
                        goto out_peg_1189;
                    }
                    
                }
                goto success_peg_1186;
                out_peg_1189:
                goto loop_peg_1184;
                success_peg_1186:
                ;
                result_peg_1163.addResult(result_peg_1185);
            } while (true);
            loop_peg_1184:
            if (result_peg_1163.matches() == 0){
                goto out_peg_1172;
            }
        
        Result result_peg_1183 = result_peg_1163;
        
        {
                Value value((void*) 0);
                value = makeDate(result_peg_1164.getValues(),result_peg_1174.getValues(),result_peg_1183.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1163.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_date = result_peg_1163;
        stream.update(result_peg_1163.getPosition());
    }
    
    
    return result_peg_1163;
    out_peg_1172:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_date = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_string.calculated()){
            return column.chunk4->chunk_string;
        }
    }
    
    int myposition = position;
    
    Value data;
    Result result_peg_1191(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1191.getPosition()))){
                    result_peg_1191.nextPosition();
                } else {
                    goto out_peg_1193;
                }
            }
            result_peg_1191.setValue((void*) "\"");
        
        
        
        result_peg_1191.reset();
            do{
                Result result_peg_1196(result_peg_1191.getPosition());
                {
                
                    Result result_peg_1199(result_peg_1196);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_1199.getPosition()))){
                                result_peg_1199.nextPosition();
                            } else {
                                goto not_peg_1198;
                            }
                        }
                        result_peg_1199.setValue((void*) "\"");
                        goto loop_peg_1195;
                        not_peg_1198:
                        result_peg_1196.setValue((void*)0);
                    
                    
                    
                    Result result_peg_1202(result_peg_1196);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_1202.getPosition()))){
                                result_peg_1202.nextPosition();
                            } else {
                                goto not_peg_1201;
                            }
                        }
                        result_peg_1202.setValue((void*) "\n");
                        goto loop_peg_1195;
                        not_peg_1201:
                        result_peg_1196.setValue((void*)0);
                    
                    
                    
                    char temp_peg_1203 = stream.get(result_peg_1196.getPosition());
                        if (temp_peg_1203 != '\0'){
                            result_peg_1196.setValue((void*) temp_peg_1203);
                            result_peg_1196.nextPosition();
                        } else {
                            goto loop_peg_1195;
                        }
                    
                    
                }
                result_peg_1191.addResult(result_peg_1196);
            } while (true);
            loop_peg_1195:
            ;
            data = result_peg_1191.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1191.getPosition()))){
                    result_peg_1191.nextPosition();
                } else {
                    goto out_peg_1193;
                }
            }
            result_peg_1191.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(data); GC::save(as<Ast::String*>(value));
                result_peg_1191.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_string = result_peg_1191;
        stream.update(result_peg_1191.getPosition());
    }
    
    
    return result_peg_1191;
    out_peg_1193:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_string = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_number.calculated()){
            return column.chunk4->chunk_number;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1205(myposition);
    
    {
    
        int save_peg_1207 = result_peg_1205.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1205.getPosition()))){
                        result_peg_1205.nextPosition();
                    } else {
                        goto out_peg_1210;
                    }
                }
                result_peg_1205.setValue((void*) "+");
                    
            }
            goto success_peg_1208;
            out_peg_1210:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1205.getPosition()))){
                        result_peg_1205.nextPosition();
                    } else {
                        goto out_peg_1212;
                    }
                }
                result_peg_1205.setValue((void*) "-");
                    
            }
            goto success_peg_1208;
            out_peg_1212:
            
            result_peg_1205 = Result(save_peg_1207);
            result_peg_1205.setValue((void*) 0);
            
            success_peg_1208:
            ;
        
        Result result_peg_1206 = result_peg_1205;
        
        result_peg_1205 = rule_float_or_integer(stream, result_peg_1205.getPosition());
            if (result_peg_1205.error()){
                goto out_peg_1214;
            }
        
        Result result_peg_1213 = result_peg_1205;
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_1206.getValues(),result_peg_1213.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1205.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_number = result_peg_1205;
        stream.update(result_peg_1205.getPosition());
    }
    
    
    return result_peg_1205;
    out_peg_1214:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_number = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_float_or_integer.calculated()){
            return column.chunk4->chunk_float_or_integer;
        }
    }
    
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_1215(myposition);
    
    {
    
        result_peg_1215.reset();
            do{
                Result result_peg_1218(result_peg_1215.getPosition());
                {
                    
                    char letter_peg_1223 = stream.get(result_peg_1218.getPosition());
                    if (letter_peg_1223 != '\0' && strchr("0123456789", letter_peg_1223) != NULL){
                        result_peg_1218.nextPosition();
                        result_peg_1218.setValue((void*) letter_peg_1223);
                    } else {
                        goto out_peg_1222;
                    }
                    
                }
                goto success_peg_1219;
                out_peg_1222:
                goto loop_peg_1217;
                success_peg_1219:
                ;
                result_peg_1215.addResult(result_peg_1218);
            } while (true);
            loop_peg_1217:
            ;
            left = result_peg_1215.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1215.getPosition()))){
                    result_peg_1215.nextPosition();
                } else {
                    goto out_peg_1225;
                }
            }
            result_peg_1215.setValue((void*) ".");
        
        
        
        result_peg_1215.reset();
            do{
                Result result_peg_1228(result_peg_1215.getPosition());
                {
                    
                    char letter_peg_1233 = stream.get(result_peg_1228.getPosition());
                    if (letter_peg_1233 != '\0' && strchr("0123456789", letter_peg_1233) != NULL){
                        result_peg_1228.nextPosition();
                        result_peg_1228.setValue((void*) letter_peg_1233);
                    } else {
                        goto out_peg_1232;
                    }
                    
                }
                goto success_peg_1229;
                out_peg_1232:
                goto loop_peg_1227;
                success_peg_1229:
                ;
                result_peg_1215.addResult(result_peg_1228);
            } while (true);
            loop_peg_1227:
            if (result_peg_1215.matches() == 0){
                goto out_peg_1225;
            }
            right = result_peg_1215.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(left,right); GC::save(as<double*>(value));
                result_peg_1215.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_1215;
        stream.update(result_peg_1215.getPosition());
    }
    
    
    return result_peg_1215;
    out_peg_1225:
    Result result_peg_1234(myposition);
    
    {
    
        result_peg_1234.reset();
            do{
                Result result_peg_1237(result_peg_1234.getPosition());
                {
                    
                    char letter_peg_1242 = stream.get(result_peg_1237.getPosition());
                    if (letter_peg_1242 != '\0' && strchr("0123456789", letter_peg_1242) != NULL){
                        result_peg_1237.nextPosition();
                        result_peg_1237.setValue((void*) letter_peg_1242);
                    } else {
                        goto out_peg_1241;
                    }
                    
                }
                goto success_peg_1238;
                out_peg_1241:
                goto loop_peg_1236;
                success_peg_1238:
                ;
                result_peg_1234.addResult(result_peg_1237);
            } while (true);
            loop_peg_1236:
            if (result_peg_1234.matches() == 0){
                goto out_peg_1243;
            }
        
        Result result_peg_1235 = result_peg_1234;
        
        Result result_peg_1246(result_peg_1234);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1246.getPosition()))){
                    result_peg_1246.nextPosition();
                } else {
                    goto not_peg_1245;
                }
            }
            result_peg_1246.setValue((void*) ".");
            goto out_peg_1243;
            not_peg_1245:
            result_peg_1234.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_1235.getValues()); GC::save(as<double*>(value));
                result_peg_1234.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = result_peg_1234;
        stream.update(result_peg_1234.getPosition());
    }
    
    
    return result_peg_1234;
    out_peg_1243:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_float_or_integer = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

const void * main(const std::string & filename) throw (ParseException){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    return done.getValues().getValue();
}

const void * main(const char * in) throw (ParseException){
    Stream stream(in);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    return done.getValues().getValue();
}



    
    } /* Def */
    
} /* Mugen */

        
