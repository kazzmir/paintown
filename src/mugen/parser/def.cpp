

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

    Result(const Result & r):
    position(r.position),
    value(r.value){
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
    Result chunk_section;
    Result chunk_section_line;
};

struct Chunk1{
    Result chunk_section_start;
    Result chunk_loopstart;
    Result chunk_name;
    Result chunk_line_end_or_comment;
    Result chunk_filename;
};

struct Chunk2{
    Result chunk_filename_char;
    Result chunk_attribute;
    Result chunk_identifier;
    Result chunk_identifier_list;
    Result chunk_valuelist;
};

struct Chunk3{
    Result chunk_value;
    Result chunk_keyword;
    Result chunk_date;
    Result chunk_string;
    Result chunk_number;
};

struct Chunk4{
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

    int hitCount(){
        return 
(chunk3 != NULL ? ((chunk3->chunk_value.calculated() ? 1 : 0)
+ (chunk3->chunk_keyword.calculated() ? 1 : 0)
+ (chunk3->chunk_date.calculated() ? 1 : 0)
+ (chunk3->chunk_string.calculated() ? 1 : 0)
+ (chunk3->chunk_number.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_filename_char.calculated() ? 1 : 0)
+ (chunk2->chunk_attribute.calculated() ? 1 : 0)
+ (chunk2->chunk_identifier.calculated() ? 1 : 0)
+ (chunk2->chunk_identifier_list.calculated() ? 1 : 0)
+ (chunk2->chunk_valuelist.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_section_start.calculated() ? 1 : 0)
+ (chunk1->chunk_loopstart.calculated() ? 1 : 0)
+ (chunk1->chunk_name.calculated() ? 1 : 0)
+ (chunk1->chunk_line_end_or_comment.calculated() ? 1 : 0)
+ (chunk1->chunk_filename.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)
+ (chunk0->chunk_section.calculated() ? 1 : 0)
+ (chunk0->chunk_section_line.calculated() ? 1 : 0)) : 0)
+
(chunk4 != NULL ? ((chunk4->chunk_float_or_integer.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 21;
    }

    ~Column(){
        delete chunk0;
        delete chunk1;
        delete chunk2;
        delete chunk3;
        delete chunk4;
    }
};


class ParseException: std::exception {
public:
    ParseException(const std::string & reason):
    std::exception(),
    message(reason){
    }

    std::string getReason() const;

    virtual ~ParseException() throw(){
    }

protected:
    std::string message;
};

class Stream{
public:
    Stream(const std::string & filename):
    temp(0),
    buffer(0),
    farthest(0){
        std::ifstream stream;
        /* ios::binary is needed on windows */
        stream.open(filename.c_str(), std::ios::in | std::ios::binary);
        if (stream.fail()){
            std::ostringstream out;
            out << "Cannot open '" << filename << "'";
            throw ParseException(out.str());
        }
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
        memo_size = 1024 * 2;
        memo = new Column*[memo_size];
        for (int i = 0; i < memo_size; i++){
            memo[i] = new Column();
        }
    }

    int length(){
        return max;
    }

    /* prints statistics about how often rules were fired and how
     * likely rules are to succeed
     */
    void printStats(){
        double min = 1;
        double max = 0;
        double average = 0;
        int count = 0;
        for (int i = 0; i < length(); i++){
            Column & c = getColumn(i);
            double rate = (double) c.hitCount() / (double) c.maxHits();
            if (rate != 0 && rate < min){
                min = rate;
            }
            if (rate > max){
                max = rate;
            }
            if (rate != 0){
                average += rate;
                count += 1;
            }
        }
        std::cout << "Min " << (100 * min) << " Max " << (100 * max) << " Average " << (100 * average / count) << " Count " << count << " Length " << length() << " Rule rate " << (100.0 * (double)count / (double) length()) << std::endl;
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
        while (position >= memo_size){
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

std::string ParseException::getReason() const {
    return message;
}

Result errorResult(-1);


Result rule_start(Stream &, const int);
Result rule_line(Stream &, const int, Value current);
Result rule_line_end(Stream &, const int);
Result rule_section(Stream &, const int);
Result rule_section_line(Stream &, const int, Value section);
Result rule_section_start(Stream &, const int);
Result rule_loopstart(Stream &, const int);
Result rule_name(Stream &, const int);
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
    Ast::Section * object = new Ast::Section(as<std::string*>(str));
    GC::save(object);
    return object;
}

SectionList * makeSectionList(){
    SectionList * object = new SectionList();
    GC::save(object);
    return object;
}

std::string * toString(const Value & input){
  std::ostringstream out;
  for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
    out << (char) (long) (*it).getValue();
  }
  std::string * object = new std::string(out.str());
  GC::save(object);
  return object;
}

double * parseDouble(const Value & value){
    std::string * str = toString(value);
    std::istringstream get(*str);
    double * number = new double;
    get >> *number;
    GC::save(number);
    return number;
}

double * parseDouble(const Value & left, const Value & right){
    std::string * str1 = toString(left);
    std::string * str2 = toString(right);
    std::istringstream get(*str1 + "." + *str2);
    double * number = new double;
    get >> *number;
    GC::save(number);
    return number;
}

std::string * toString(char front, const Value & input){
  std::string * str = toString(input);
  str->insert(str->begin(), front);
  return str;
}

Ast::Attribute * makeAttribute(const Value & id, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(as<Ast::Identifier*>(id), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttribute(const Value & id){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(as<Ast::Identifier*>(id));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributeFilename(const Value & id, const Value & data){
    /* TODO */
    exit(0);
    // return makeAttribute(id, data);
}

Ast::Attribute * makeIndexedAttribute(const Value & id, const Value & index, const Value & data){
    /* TODO: fix this */
    exit(0);
    /*
    Ast::Attribute * object = new Ast::Attribute(Ast::Attribute::None);
    GC::save(object);
    return object;
    */
}

Ast::Keyword * makeKeyword(const Value & value){
    Ast::Keyword * object = new Ast::Keyword(as<char*>(value));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributes(const Value & id, const Value & data){
    /* TODO */
    exit(0);
    /*
    Ast::Attribute * object = new Ast::Attribute(Ast::Attribute::None);
    GC::save(object);
    return object;
    */
}

/* TODO */
Ast::Value * makeValue(){
    Ast::Number * object = new Ast::Number(0);
    GC::save(object);
    return object;
}

Ast::Value * makeValueList(const Value & front, const Value & rest){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        Ast::Value * value = as<Ast::Value*>((*it).getValue());
        if (value == 0){
            /* FIXME! replace empty with a new node */
            value = makeKeyword("empty");
            values.push_back(value);
        } else {
            values.push_back(value);
        }
    }

    Ast::ValueList * object = new Ast::ValueList(values);
    GC::save(object);
    return object;
}

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string> ids;
    ids.push_back(*as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        ids.push_back(*as<std::string*>((*it).getValue()));
    }
    Ast::Identifier * object = new Ast::Identifier(ids);
    GC::save(object);
    return object;
}

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0){
        value = -value;
    }

    Ast::Number * object = new Ast::Number(value);
    GC::save(object);
    return object;
}

Ast::String * makeString(const Value & value){
    Ast::String * object = new Ast::String(toString(value));
    GC::save(object);
    return object;
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
    Ast::Number * object = new Ast::Number(0);
    GC::save(object);
    return object;
}




Result rule_start(Stream & stream, const int position){
    
    int myposition = position;
    
    Value current;
    Result result_peg_2(myposition);
    
    {
    
        {
                Value value((void*) 0);
                value = makeSectionList();
                result_peg_2.setValue(value);
            }
            current = result_peg_2.getValues();
        
        
        
        {
                
                result_peg_2.reset();
                do{
                    Result result_peg_99(result_peg_2.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_99.getPosition()))){
                                result_peg_99.nextPosition();
                            } else {
                                goto out_peg_102;
                            }
                        }
                        result_peg_99.setValue((void*) " ");
                            
                    }
                    goto success_peg_100;
                    out_peg_102:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_99.getPosition()))){
                                result_peg_99.nextPosition();
                            } else {
                                goto out_peg_104;
                            }
                        }
                        result_peg_99.setValue((void*) "\t");
                            
                    }
                    goto success_peg_100;
                    out_peg_104:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_99.getPosition()))){
                                            result_peg_99.nextPosition();
                                        } else {
                                            goto out_peg_151;
                                        }
                                    }
                                    result_peg_99.setValue((void*) ";");
                                
                                
                                
                                result_peg_99.reset();
                                    do{
                                        Result result_peg_160(result_peg_99.getPosition());
                                        {
                                        
                                            Result result_peg_163(result_peg_160);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_163.getPosition()))){
                                                        result_peg_163.nextPosition();
                                                    } else {
                                                        goto not_peg_162;
                                                    }
                                                }
                                                result_peg_163.setValue((void*) "\n");
                                                goto loop_peg_159;
                                                not_peg_162:
                                                result_peg_160.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_164 = stream.get(result_peg_160.getPosition());
                                                if (temp_peg_164 != '\0'){
                                                    result_peg_160.setValue((void*) temp_peg_164);
                                                    result_peg_160.nextPosition();
                                                } else {
                                                    goto loop_peg_159;
                                                }
                                            
                                            
                                        }
                                        result_peg_99.addResult(result_peg_160);
                                    } while (true);
                                    loop_peg_159:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_148;
                        out_peg_151:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_99.getPosition()))){
                                            result_peg_99.nextPosition();
                                        } else {
                                            goto out_peg_167;
                                        }
                                    }
                                    result_peg_99.setValue((void*) "=");
                                
                                
                                
                                result_peg_99.reset();
                                    do{
                                        Result result_peg_176(result_peg_99.getPosition());
                                        {
                                        
                                            Result result_peg_179(result_peg_176);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_179.getPosition()))){
                                                        result_peg_179.nextPosition();
                                                    } else {
                                                        goto not_peg_178;
                                                    }
                                                }
                                                result_peg_179.setValue((void*) "\n");
                                                goto loop_peg_175;
                                                not_peg_178:
                                                result_peg_176.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_180 = stream.get(result_peg_176.getPosition());
                                                if (temp_peg_180 != '\0'){
                                                    result_peg_176.setValue((void*) temp_peg_180);
                                                    result_peg_176.nextPosition();
                                                } else {
                                                    goto loop_peg_175;
                                                }
                                            
                                            
                                        }
                                        result_peg_99.addResult(result_peg_176);
                                    } while (true);
                                    loop_peg_175:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_148;
                        out_peg_167:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_99.getPosition()))){
                                            result_peg_99.nextPosition();
                                        } else {
                                            goto out_peg_183;
                                        }
                                    }
                                    result_peg_99.setValue((void*) "-");
                                
                                
                                
                                result_peg_99.reset();
                                    do{
                                        Result result_peg_188(result_peg_99.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_188.getPosition()))){
                                                result_peg_188.nextPosition();
                                            } else {
                                                goto loop_peg_187;
                                            }
                                        }
                                        result_peg_188.setValue((void*) "-");
                                        result_peg_99.addResult(result_peg_188);
                                    } while (true);
                                    loop_peg_187:
                                    if (result_peg_99.matches() == 0){
                                        goto out_peg_183;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_148;
                        out_peg_183:
                        goto out_peg_147;
                        success_peg_148:
                        ;
                        
                    }
                    goto success_peg_100;
                    out_peg_147:
                    goto loop_peg_98;
                    success_peg_100:
                    ;
                    result_peg_2.addResult(result_peg_99);
                } while (true);
                loop_peg_98:
                ;
                        
            }
            goto success_peg_5;
            goto out_peg_189;
            success_peg_5:
            ;
        
        
        
        result_peg_2.reset();
            do{
                Result result_peg_192(result_peg_2.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_192.getPosition()))){
                            result_peg_192.nextPosition();
                        } else {
                            goto out_peg_195;
                        }
                    }
                    result_peg_192.setValue((void*) "\n");
                        
                }
                goto success_peg_193;
                out_peg_195:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_192.getPosition()))){
                            result_peg_192.nextPosition();
                        } else {
                            goto out_peg_197;
                        }
                    }
                    result_peg_192.setValue((void*) "\r");
                        
                }
                goto success_peg_193;
                out_peg_197:
                goto loop_peg_191;
                success_peg_193:
                ;
                result_peg_2.addResult(result_peg_192);
            } while (true);
            loop_peg_191:
            ;
        
        
        
        result_peg_2.reset();
            do{
                Result result_peg_200(result_peg_2.getPosition());
                {
                
                    result_peg_200 = rule_line(stream, result_peg_200.getPosition(), current);
                        if (result_peg_200.error()){
                            goto loop_peg_199;
                        }
                    
                    
                    
                    {
                            
                            result_peg_200.reset();
                            do{
                                Result result_peg_297(result_peg_200.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_297.getPosition()))){
                                            result_peg_297.nextPosition();
                                        } else {
                                            goto out_peg_300;
                                        }
                                    }
                                    result_peg_297.setValue((void*) " ");
                                        
                                }
                                goto success_peg_298;
                                out_peg_300:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_297.getPosition()))){
                                            result_peg_297.nextPosition();
                                        } else {
                                            goto out_peg_302;
                                        }
                                    }
                                    result_peg_297.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_298;
                                out_peg_302:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_297.getPosition()))){
                                                        result_peg_297.nextPosition();
                                                    } else {
                                                        goto out_peg_349;
                                                    }
                                                }
                                                result_peg_297.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_297.reset();
                                                do{
                                                    Result result_peg_358(result_peg_297.getPosition());
                                                    {
                                                    
                                                        Result result_peg_361(result_peg_358);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_361.getPosition()))){
                                                                    result_peg_361.nextPosition();
                                                                } else {
                                                                    goto not_peg_360;
                                                                }
                                                            }
                                                            result_peg_361.setValue((void*) "\n");
                                                            goto loop_peg_357;
                                                            not_peg_360:
                                                            result_peg_358.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_362 = stream.get(result_peg_358.getPosition());
                                                            if (temp_peg_362 != '\0'){
                                                                result_peg_358.setValue((void*) temp_peg_362);
                                                                result_peg_358.nextPosition();
                                                            } else {
                                                                goto loop_peg_357;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_297.addResult(result_peg_358);
                                                } while (true);
                                                loop_peg_357:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_346;
                                    out_peg_349:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_297.getPosition()))){
                                                        result_peg_297.nextPosition();
                                                    } else {
                                                        goto out_peg_365;
                                                    }
                                                }
                                                result_peg_297.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_297.reset();
                                                do{
                                                    Result result_peg_374(result_peg_297.getPosition());
                                                    {
                                                    
                                                        Result result_peg_377(result_peg_374);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_377.getPosition()))){
                                                                    result_peg_377.nextPosition();
                                                                } else {
                                                                    goto not_peg_376;
                                                                }
                                                            }
                                                            result_peg_377.setValue((void*) "\n");
                                                            goto loop_peg_373;
                                                            not_peg_376:
                                                            result_peg_374.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_378 = stream.get(result_peg_374.getPosition());
                                                            if (temp_peg_378 != '\0'){
                                                                result_peg_374.setValue((void*) temp_peg_378);
                                                                result_peg_374.nextPosition();
                                                            } else {
                                                                goto loop_peg_373;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_297.addResult(result_peg_374);
                                                } while (true);
                                                loop_peg_373:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_346;
                                    out_peg_365:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_297.getPosition()))){
                                                        result_peg_297.nextPosition();
                                                    } else {
                                                        goto out_peg_381;
                                                    }
                                                }
                                                result_peg_297.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_297.reset();
                                                do{
                                                    Result result_peg_386(result_peg_297.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_386.getPosition()))){
                                                            result_peg_386.nextPosition();
                                                        } else {
                                                            goto loop_peg_385;
                                                        }
                                                    }
                                                    result_peg_386.setValue((void*) "-");
                                                    result_peg_297.addResult(result_peg_386);
                                                } while (true);
                                                loop_peg_385:
                                                if (result_peg_297.matches() == 0){
                                                    goto out_peg_381;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_346;
                                    out_peg_381:
                                    goto out_peg_345;
                                    success_peg_346:
                                    ;
                                    
                                }
                                goto success_peg_298;
                                out_peg_345:
                                goto loop_peg_296;
                                success_peg_298:
                                ;
                                result_peg_200.addResult(result_peg_297);
                            } while (true);
                            loop_peg_296:
                            ;
                                    
                        }
                        goto success_peg_203;
                        goto loop_peg_199;
                        success_peg_203:
                        ;
                    
                    
                    
                    int save_peg_387 = result_peg_200.getPosition();
                        
                        result_peg_200 = rule_line_end(stream, result_peg_200.getPosition());
                        if (result_peg_200.error()){
                            
                            result_peg_200 = Result(save_peg_387);
                            result_peg_200.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_2.addResult(result_peg_200);
            } while (true);
            loop_peg_199:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_2.getPosition())){
                result_peg_2.nextPosition();
                result_peg_2.setValue((void *) '\0');
            } else {
                goto out_peg_189;
            }
        
        
        
        {
                Value value((void*) 0);
                value = current; GC::cleanup(as<SectionList*>(value));
                result_peg_2.setValue(value);
            }
        
        
    }
    
    
    return result_peg_2;
    out_peg_189:
    
     GC::cleanup(0); 
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    int myposition = position;
    
    
    Result result_peg_390(myposition);
    
    {
    
        {
                
                result_peg_390.reset();
                do{
                    Result result_peg_402(result_peg_390.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_402.getPosition()))){
                                result_peg_402.nextPosition();
                            } else {
                                goto out_peg_405;
                            }
                        }
                        result_peg_402.setValue((void*) " ");
                            
                    }
                    goto success_peg_403;
                    out_peg_405:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_402.getPosition()))){
                                result_peg_402.nextPosition();
                            } else {
                                goto out_peg_407;
                            }
                        }
                        result_peg_402.setValue((void*) "\t");
                            
                    }
                    goto success_peg_403;
                    out_peg_407:
                    goto loop_peg_401;
                    success_peg_403:
                    ;
                    result_peg_390.addResult(result_peg_402);
                } while (true);
                loop_peg_401:
                ;
                        
            }
            goto success_peg_392;
            goto out_peg_408;
            success_peg_392:
            ;
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_390.getPosition()))){
                                result_peg_390.nextPosition();
                            } else {
                                goto out_peg_412;
                            }
                        }
                        result_peg_390.setValue((void*) ";");
                    
                    
                    
                    result_peg_390.reset();
                        do{
                            Result result_peg_421(result_peg_390.getPosition());
                            {
                            
                                Result result_peg_424(result_peg_421);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_424.getPosition()))){
                                            result_peg_424.nextPosition();
                                        } else {
                                            goto not_peg_423;
                                        }
                                    }
                                    result_peg_424.setValue((void*) "\n");
                                    goto loop_peg_420;
                                    not_peg_423:
                                    result_peg_421.setValue((void*)0);
                                
                                
                                
                                char temp_peg_425 = stream.get(result_peg_421.getPosition());
                                    if (temp_peg_425 != '\0'){
                                        result_peg_421.setValue((void*) temp_peg_425);
                                        result_peg_421.nextPosition();
                                    } else {
                                        goto loop_peg_420;
                                    }
                                
                                
                            }
                            result_peg_390.addResult(result_peg_421);
                        } while (true);
                        loop_peg_420:
                        ;
                    
                    
                }
            }
            goto success_peg_409;
            out_peg_412:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_390.getPosition()))){
                                result_peg_390.nextPosition();
                            } else {
                                goto out_peg_428;
                            }
                        }
                        result_peg_390.setValue((void*) "=");
                    
                    
                    
                    result_peg_390.reset();
                        do{
                            Result result_peg_437(result_peg_390.getPosition());
                            {
                            
                                Result result_peg_440(result_peg_437);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_440.getPosition()))){
                                            result_peg_440.nextPosition();
                                        } else {
                                            goto not_peg_439;
                                        }
                                    }
                                    result_peg_440.setValue((void*) "\n");
                                    goto loop_peg_436;
                                    not_peg_439:
                                    result_peg_437.setValue((void*)0);
                                
                                
                                
                                char temp_peg_441 = stream.get(result_peg_437.getPosition());
                                    if (temp_peg_441 != '\0'){
                                        result_peg_437.setValue((void*) temp_peg_441);
                                        result_peg_437.nextPosition();
                                    } else {
                                        goto loop_peg_436;
                                    }
                                
                                
                            }
                            result_peg_390.addResult(result_peg_437);
                        } while (true);
                        loop_peg_436:
                        ;
                    
                    
                }
            }
            goto success_peg_409;
            out_peg_428:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_390.getPosition()))){
                                result_peg_390.nextPosition();
                            } else {
                                goto out_peg_444;
                            }
                        }
                        result_peg_390.setValue((void*) "-");
                    
                    
                    
                    result_peg_390.reset();
                        do{
                            Result result_peg_449(result_peg_390.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_449.getPosition()))){
                                    result_peg_449.nextPosition();
                                } else {
                                    goto loop_peg_448;
                                }
                            }
                            result_peg_449.setValue((void*) "-");
                            result_peg_390.addResult(result_peg_449);
                        } while (true);
                        loop_peg_448:
                        if (result_peg_390.matches() == 0){
                            goto out_peg_444;
                        }
                    
                    
                }
            }
            goto success_peg_409;
            out_peg_444:
            goto out_peg_408;
            success_peg_409:
            ;
        
        
    }
    
    
    return result_peg_390;
    out_peg_408:
    Result result_peg_450(myposition);
    
    {
    
        {
                
                result_peg_450.reset();
                do{
                    Result result_peg_462(result_peg_450.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_462.getPosition()))){
                                result_peg_462.nextPosition();
                            } else {
                                goto out_peg_465;
                            }
                        }
                        result_peg_462.setValue((void*) " ");
                            
                    }
                    goto success_peg_463;
                    out_peg_465:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_462.getPosition()))){
                                result_peg_462.nextPosition();
                            } else {
                                goto out_peg_467;
                            }
                        }
                        result_peg_462.setValue((void*) "\t");
                            
                    }
                    goto success_peg_463;
                    out_peg_467:
                    goto loop_peg_461;
                    success_peg_463:
                    ;
                    result_peg_450.addResult(result_peg_462);
                } while (true);
                loop_peg_461:
                ;
                        
            }
            goto success_peg_452;
            goto out_peg_468;
            success_peg_452:
            ;
        
        
        
        result_peg_450 = rule_section(stream, result_peg_450.getPosition());
            if (result_peg_450.error()){
                goto out_peg_468;
            }
        
        Result result_peg_469 = result_peg_450;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_469.getValues());
                result_peg_450.setValue(value);
            }
        
        
    }
    
    
    return result_peg_450;
    out_peg_468:
    Result result_peg_470(myposition);
    
    result_peg_470.reset();
    do{
        Result result_peg_472(result_peg_470.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_472.getPosition()))){
                    result_peg_472.nextPosition();
                } else {
                    goto out_peg_475;
                }
            }
            result_peg_472.setValue((void*) " ");
                
        }
        goto success_peg_473;
        out_peg_475:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_472.getPosition()))){
                    result_peg_472.nextPosition();
                } else {
                    goto out_peg_477;
                }
            }
            result_peg_472.setValue((void*) "\t");
                
        }
        goto success_peg_473;
        out_peg_477:
        goto loop_peg_471;
        success_peg_473:
        ;
        result_peg_470.addResult(result_peg_472);
    } while (true);
    loop_peg_471:
    if (result_peg_470.matches() == 0){
        goto out_peg_478;
    }
    
    
    return result_peg_470;
    out_peg_478:
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_480(myposition);
    
    result_peg_480.reset();
    do{
        Result result_peg_482(result_peg_480.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_482.getPosition()))){
                    result_peg_482.nextPosition();
                } else {
                    goto out_peg_485;
                }
            }
            result_peg_482.setValue((void*) "\n");
                
        }
        goto success_peg_483;
        out_peg_485:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_482.getPosition()))){
                    result_peg_482.nextPosition();
                } else {
                    goto out_peg_487;
                }
            }
            result_peg_482.setValue((void*) "\r");
                
        }
        goto success_peg_483;
        out_peg_487:
        goto loop_peg_481;
        success_peg_483:
        ;
        result_peg_480.addResult(result_peg_482);
    } while (true);
    loop_peg_481:
    if (result_peg_480.matches() == 0){
        goto out_peg_488;
    }
    
    
    return result_peg_480;
    out_peg_488:
    Result result_peg_489(myposition);
    
    {
    
        Result result_peg_491(result_peg_489.getPosition());
            if ('\0' == stream.get(result_peg_491.getPosition())){
                result_peg_491.nextPosition();
                result_peg_491.setValue((void *) '\0');
            } else {
                goto out_peg_492;
            }
        
        
        
        
        
        
    }
    
    
    return result_peg_489;
    out_peg_492:
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    int myposition = position;
    
    Value ast;
    Result result_peg_494(myposition);
    
    {
    
        result_peg_494 = rule_section_start(stream, result_peg_494.getPosition());
            if (result_peg_494.error()){
                goto out_peg_496;
            }
        
        Result result_peg_495 = result_peg_494;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_495.getValues());
                result_peg_494.setValue(value);
            }
            ast = result_peg_494.getValues();
        
        
        
        {
                
                result_peg_494.reset();
                do{
                    Result result_peg_593(result_peg_494.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_593.getPosition()))){
                                result_peg_593.nextPosition();
                            } else {
                                goto out_peg_596;
                            }
                        }
                        result_peg_593.setValue((void*) " ");
                            
                    }
                    goto success_peg_594;
                    out_peg_596:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_593.getPosition()))){
                                result_peg_593.nextPosition();
                            } else {
                                goto out_peg_598;
                            }
                        }
                        result_peg_593.setValue((void*) "\t");
                            
                    }
                    goto success_peg_594;
                    out_peg_598:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_593.getPosition()))){
                                            result_peg_593.nextPosition();
                                        } else {
                                            goto out_peg_645;
                                        }
                                    }
                                    result_peg_593.setValue((void*) ";");
                                
                                
                                
                                result_peg_593.reset();
                                    do{
                                        Result result_peg_654(result_peg_593.getPosition());
                                        {
                                        
                                            Result result_peg_657(result_peg_654);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_657.getPosition()))){
                                                        result_peg_657.nextPosition();
                                                    } else {
                                                        goto not_peg_656;
                                                    }
                                                }
                                                result_peg_657.setValue((void*) "\n");
                                                goto loop_peg_653;
                                                not_peg_656:
                                                result_peg_654.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_658 = stream.get(result_peg_654.getPosition());
                                                if (temp_peg_658 != '\0'){
                                                    result_peg_654.setValue((void*) temp_peg_658);
                                                    result_peg_654.nextPosition();
                                                } else {
                                                    goto loop_peg_653;
                                                }
                                            
                                            
                                        }
                                        result_peg_593.addResult(result_peg_654);
                                    } while (true);
                                    loop_peg_653:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_642;
                        out_peg_645:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_593.getPosition()))){
                                            result_peg_593.nextPosition();
                                        } else {
                                            goto out_peg_661;
                                        }
                                    }
                                    result_peg_593.setValue((void*) "=");
                                
                                
                                
                                result_peg_593.reset();
                                    do{
                                        Result result_peg_670(result_peg_593.getPosition());
                                        {
                                        
                                            Result result_peg_673(result_peg_670);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_673.getPosition()))){
                                                        result_peg_673.nextPosition();
                                                    } else {
                                                        goto not_peg_672;
                                                    }
                                                }
                                                result_peg_673.setValue((void*) "\n");
                                                goto loop_peg_669;
                                                not_peg_672:
                                                result_peg_670.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_674 = stream.get(result_peg_670.getPosition());
                                                if (temp_peg_674 != '\0'){
                                                    result_peg_670.setValue((void*) temp_peg_674);
                                                    result_peg_670.nextPosition();
                                                } else {
                                                    goto loop_peg_669;
                                                }
                                            
                                            
                                        }
                                        result_peg_593.addResult(result_peg_670);
                                    } while (true);
                                    loop_peg_669:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_642;
                        out_peg_661:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_593.getPosition()))){
                                            result_peg_593.nextPosition();
                                        } else {
                                            goto out_peg_677;
                                        }
                                    }
                                    result_peg_593.setValue((void*) "-");
                                
                                
                                
                                result_peg_593.reset();
                                    do{
                                        Result result_peg_682(result_peg_593.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_682.getPosition()))){
                                                result_peg_682.nextPosition();
                                            } else {
                                                goto loop_peg_681;
                                            }
                                        }
                                        result_peg_682.setValue((void*) "-");
                                        result_peg_593.addResult(result_peg_682);
                                    } while (true);
                                    loop_peg_681:
                                    if (result_peg_593.matches() == 0){
                                        goto out_peg_677;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_642;
                        out_peg_677:
                        goto out_peg_641;
                        success_peg_642:
                        ;
                        
                    }
                    goto success_peg_594;
                    out_peg_641:
                    goto loop_peg_592;
                    success_peg_594:
                    ;
                    result_peg_494.addResult(result_peg_593);
                } while (true);
                loop_peg_592:
                ;
                        
            }
            goto success_peg_499;
            goto out_peg_496;
            success_peg_499:
            ;
        
        
        
        result_peg_494.reset();
            do{
                Result result_peg_685(result_peg_494.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_685.getPosition()))){
                            result_peg_685.nextPosition();
                        } else {
                            goto out_peg_688;
                        }
                    }
                    result_peg_685.setValue((void*) "\n");
                        
                }
                goto success_peg_686;
                out_peg_688:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_685.getPosition()))){
                            result_peg_685.nextPosition();
                        } else {
                            goto out_peg_690;
                        }
                    }
                    result_peg_685.setValue((void*) "\r");
                        
                }
                goto success_peg_686;
                out_peg_690:
                goto loop_peg_684;
                success_peg_686:
                ;
                result_peg_494.addResult(result_peg_685);
            } while (true);
            loop_peg_684:
            if (result_peg_494.matches() == 0){
                goto out_peg_496;
            }
        
        
        
        result_peg_494.reset();
            do{
                Result result_peg_693(result_peg_494.getPosition());
                {
                
                    result_peg_693 = rule_section_line(stream, result_peg_693.getPosition(), ast);
                        if (result_peg_693.error()){
                            goto loop_peg_692;
                        }
                    
                    
                    
                    {
                            
                            result_peg_693.reset();
                            do{
                                Result result_peg_790(result_peg_693.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_790.getPosition()))){
                                            result_peg_790.nextPosition();
                                        } else {
                                            goto out_peg_793;
                                        }
                                    }
                                    result_peg_790.setValue((void*) " ");
                                        
                                }
                                goto success_peg_791;
                                out_peg_793:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_790.getPosition()))){
                                            result_peg_790.nextPosition();
                                        } else {
                                            goto out_peg_795;
                                        }
                                    }
                                    result_peg_790.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_791;
                                out_peg_795:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_790.getPosition()))){
                                                        result_peg_790.nextPosition();
                                                    } else {
                                                        goto out_peg_842;
                                                    }
                                                }
                                                result_peg_790.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_790.reset();
                                                do{
                                                    Result result_peg_851(result_peg_790.getPosition());
                                                    {
                                                    
                                                        Result result_peg_854(result_peg_851);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_854.getPosition()))){
                                                                    result_peg_854.nextPosition();
                                                                } else {
                                                                    goto not_peg_853;
                                                                }
                                                            }
                                                            result_peg_854.setValue((void*) "\n");
                                                            goto loop_peg_850;
                                                            not_peg_853:
                                                            result_peg_851.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_855 = stream.get(result_peg_851.getPosition());
                                                            if (temp_peg_855 != '\0'){
                                                                result_peg_851.setValue((void*) temp_peg_855);
                                                                result_peg_851.nextPosition();
                                                            } else {
                                                                goto loop_peg_850;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_790.addResult(result_peg_851);
                                                } while (true);
                                                loop_peg_850:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_839;
                                    out_peg_842:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_790.getPosition()))){
                                                        result_peg_790.nextPosition();
                                                    } else {
                                                        goto out_peg_858;
                                                    }
                                                }
                                                result_peg_790.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_790.reset();
                                                do{
                                                    Result result_peg_867(result_peg_790.getPosition());
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
                                                    result_peg_790.addResult(result_peg_867);
                                                } while (true);
                                                loop_peg_866:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_839;
                                    out_peg_858:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_790.getPosition()))){
                                                        result_peg_790.nextPosition();
                                                    } else {
                                                        goto out_peg_874;
                                                    }
                                                }
                                                result_peg_790.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_790.reset();
                                                do{
                                                    Result result_peg_879(result_peg_790.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_879.getPosition()))){
                                                            result_peg_879.nextPosition();
                                                        } else {
                                                            goto loop_peg_878;
                                                        }
                                                    }
                                                    result_peg_879.setValue((void*) "-");
                                                    result_peg_790.addResult(result_peg_879);
                                                } while (true);
                                                loop_peg_878:
                                                if (result_peg_790.matches() == 0){
                                                    goto out_peg_874;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_839;
                                    out_peg_874:
                                    goto out_peg_838;
                                    success_peg_839:
                                    ;
                                    
                                }
                                goto success_peg_791;
                                out_peg_838:
                                goto loop_peg_789;
                                success_peg_791:
                                ;
                                result_peg_693.addResult(result_peg_790);
                            } while (true);
                            loop_peg_789:
                            ;
                                    
                        }
                        goto success_peg_696;
                        goto loop_peg_692;
                        success_peg_696:
                        ;
                    
                    
                    
                    result_peg_693 = rule_line_end(stream, result_peg_693.getPosition());
                        if (result_peg_693.error()){
                            goto loop_peg_692;
                        }
                    
                    
                }
                result_peg_494.addResult(result_peg_693);
            } while (true);
            loop_peg_692:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_494.setValue(value);
            }
        
        
    }
    
    
    return result_peg_494;
    out_peg_496:
    
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    int myposition = position;
    
    Value data;
    Result result_peg_881(myposition);
    
    {
    
        {
                
                result_peg_881.reset();
                do{
                    Result result_peg_893(result_peg_881.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_893.getPosition()))){
                                result_peg_893.nextPosition();
                            } else {
                                goto out_peg_896;
                            }
                        }
                        result_peg_893.setValue((void*) " ");
                            
                    }
                    goto success_peg_894;
                    out_peg_896:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_893.getPosition()))){
                                result_peg_893.nextPosition();
                            } else {
                                goto out_peg_898;
                            }
                        }
                        result_peg_893.setValue((void*) "\t");
                            
                    }
                    goto success_peg_894;
                    out_peg_898:
                    goto loop_peg_892;
                    success_peg_894:
                    ;
                    result_peg_881.addResult(result_peg_893);
                } while (true);
                loop_peg_892:
                ;
                        
            }
            goto success_peg_883;
            goto out_peg_899;
            success_peg_883:
            ;
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_881.getPosition()))){
                                result_peg_881.nextPosition();
                            } else {
                                goto out_peg_903;
                            }
                        }
                        result_peg_881.setValue((void*) ";");
                    
                    
                    
                    result_peg_881.reset();
                        do{
                            Result result_peg_912(result_peg_881.getPosition());
                            {
                            
                                Result result_peg_915(result_peg_912);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_915.getPosition()))){
                                            result_peg_915.nextPosition();
                                        } else {
                                            goto not_peg_914;
                                        }
                                    }
                                    result_peg_915.setValue((void*) "\n");
                                    goto loop_peg_911;
                                    not_peg_914:
                                    result_peg_912.setValue((void*)0);
                                
                                
                                
                                char temp_peg_916 = stream.get(result_peg_912.getPosition());
                                    if (temp_peg_916 != '\0'){
                                        result_peg_912.setValue((void*) temp_peg_916);
                                        result_peg_912.nextPosition();
                                    } else {
                                        goto loop_peg_911;
                                    }
                                
                                
                            }
                            result_peg_881.addResult(result_peg_912);
                        } while (true);
                        loop_peg_911:
                        ;
                    
                    
                }
            }
            goto success_peg_900;
            out_peg_903:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_881.getPosition()))){
                                result_peg_881.nextPosition();
                            } else {
                                goto out_peg_919;
                            }
                        }
                        result_peg_881.setValue((void*) "=");
                    
                    
                    
                    result_peg_881.reset();
                        do{
                            Result result_peg_928(result_peg_881.getPosition());
                            {
                            
                                Result result_peg_931(result_peg_928);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_931.getPosition()))){
                                            result_peg_931.nextPosition();
                                        } else {
                                            goto not_peg_930;
                                        }
                                    }
                                    result_peg_931.setValue((void*) "\n");
                                    goto loop_peg_927;
                                    not_peg_930:
                                    result_peg_928.setValue((void*)0);
                                
                                
                                
                                char temp_peg_932 = stream.get(result_peg_928.getPosition());
                                    if (temp_peg_932 != '\0'){
                                        result_peg_928.setValue((void*) temp_peg_932);
                                        result_peg_928.nextPosition();
                                    } else {
                                        goto loop_peg_927;
                                    }
                                
                                
                            }
                            result_peg_881.addResult(result_peg_928);
                        } while (true);
                        loop_peg_927:
                        ;
                    
                    
                }
            }
            goto success_peg_900;
            out_peg_919:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_881.getPosition()))){
                                result_peg_881.nextPosition();
                            } else {
                                goto out_peg_935;
                            }
                        }
                        result_peg_881.setValue((void*) "-");
                    
                    
                    
                    result_peg_881.reset();
                        do{
                            Result result_peg_940(result_peg_881.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_940.getPosition()))){
                                    result_peg_940.nextPosition();
                                } else {
                                    goto loop_peg_939;
                                }
                            }
                            result_peg_940.setValue((void*) "-");
                            result_peg_881.addResult(result_peg_940);
                        } while (true);
                        loop_peg_939:
                        if (result_peg_881.matches() == 0){
                            goto out_peg_935;
                        }
                    
                    
                }
            }
            goto success_peg_900;
            out_peg_935:
            goto out_peg_899;
            success_peg_900:
            ;
        
        
    }
    
    
    return result_peg_881;
    out_peg_899:
    Result result_peg_941(myposition);
    
    {
    
        {
                
                result_peg_941.reset();
                do{
                    Result result_peg_953(result_peg_941.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_953.getPosition()))){
                                result_peg_953.nextPosition();
                            } else {
                                goto out_peg_956;
                            }
                        }
                        result_peg_953.setValue((void*) " ");
                            
                    }
                    goto success_peg_954;
                    out_peg_956:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_953.getPosition()))){
                                result_peg_953.nextPosition();
                            } else {
                                goto out_peg_958;
                            }
                        }
                        result_peg_953.setValue((void*) "\t");
                            
                    }
                    goto success_peg_954;
                    out_peg_958:
                    goto loop_peg_952;
                    success_peg_954:
                    ;
                    result_peg_941.addResult(result_peg_953);
                } while (true);
                loop_peg_952:
                ;
                        
            }
            goto success_peg_943;
            goto out_peg_959;
            success_peg_943:
            ;
        
        
        
        result_peg_941 = rule_attribute(stream, result_peg_941.getPosition());
            if (result_peg_941.error()){
                goto out_peg_959;
            }
            data = result_peg_941.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addAttribute(asAttribute(data));
                result_peg_941.setValue(value);
            }
        
        
    }
    
    
    return result_peg_941;
    out_peg_959:
    Result result_peg_961(myposition);
    
    {
    
        {
                
                result_peg_961.reset();
                do{
                    Result result_peg_973(result_peg_961.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_973.getPosition()))){
                                result_peg_973.nextPosition();
                            } else {
                                goto out_peg_976;
                            }
                        }
                        result_peg_973.setValue((void*) " ");
                            
                    }
                    goto success_peg_974;
                    out_peg_976:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_973.getPosition()))){
                                result_peg_973.nextPosition();
                            } else {
                                goto out_peg_978;
                            }
                        }
                        result_peg_973.setValue((void*) "\t");
                            
                    }
                    goto success_peg_974;
                    out_peg_978:
                    goto loop_peg_972;
                    success_peg_974:
                    ;
                    result_peg_961.addResult(result_peg_973);
                } while (true);
                loop_peg_972:
                ;
                        
            }
            goto success_peg_963;
            goto out_peg_979;
            success_peg_963:
            ;
        
        
        
        result_peg_961 = rule_valuelist(stream, result_peg_961.getPosition());
            if (result_peg_961.error()){
                goto out_peg_979;
            }
            data = result_peg_961.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_961.setValue(value);
            }
        
        
    }
    
    
    return result_peg_961;
    out_peg_979:
    Result result_peg_981(myposition);
    
    {
    
        {
                
                result_peg_981.reset();
                do{
                    Result result_peg_993(result_peg_981.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_993.getPosition()))){
                                result_peg_993.nextPosition();
                            } else {
                                goto out_peg_996;
                            }
                        }
                        result_peg_993.setValue((void*) " ");
                            
                    }
                    goto success_peg_994;
                    out_peg_996:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_993.getPosition()))){
                                result_peg_993.nextPosition();
                            } else {
                                goto out_peg_998;
                            }
                        }
                        result_peg_993.setValue((void*) "\t");
                            
                    }
                    goto success_peg_994;
                    out_peg_998:
                    goto loop_peg_992;
                    success_peg_994:
                    ;
                    result_peg_981.addResult(result_peg_993);
                } while (true);
                loop_peg_992:
                ;
                        
            }
            goto success_peg_983;
            goto out_peg_999;
            success_peg_983:
            ;
        
        
        
        result_peg_981 = rule_loopstart(stream, result_peg_981.getPosition());
            if (result_peg_981.error()){
                goto out_peg_999;
            }
            data = result_peg_981.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_981.setValue(value);
            }
        
        
    }
    
    
    return result_peg_981;
    out_peg_999:
    Result result_peg_1001(myposition);
    
    {
    
        result_peg_1001.reset();
            do{
                Result result_peg_1004(result_peg_1001.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_1004.getPosition()))){
                            result_peg_1004.nextPosition();
                        } else {
                            goto out_peg_1007;
                        }
                    }
                    result_peg_1004.setValue((void*) " ");
                        
                }
                goto success_peg_1005;
                out_peg_1007:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_1004.getPosition()))){
                            result_peg_1004.nextPosition();
                        } else {
                            goto out_peg_1009;
                        }
                    }
                    result_peg_1004.setValue((void*) "\t");
                        
                }
                goto success_peg_1005;
                out_peg_1009:
                goto loop_peg_1003;
                success_peg_1005:
                ;
                result_peg_1001.addResult(result_peg_1004);
            } while (true);
            loop_peg_1003:
            if (result_peg_1001.matches() == 0){
                goto out_peg_1010;
            }
        
        
        
        Result result_peg_1012(result_peg_1001);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_1012.getPosition()))){
                    result_peg_1012.nextPosition();
                } else {
                    goto not_peg_1011;
                }
            }
            result_peg_1012.setValue((void*) "[");
            goto out_peg_1010;
            not_peg_1011:
            result_peg_1001.setValue((void*)0);
        
        
    }
    
    
    return result_peg_1001;
    out_peg_1010:
    
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    int myposition = position;
    
    Value data;
    Result result_peg_1014(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "[");
        
        
        
        {
                
                result_peg_1014.reset();
                do{
                    Result result_peg_1028(result_peg_1014.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1028.getPosition()))){
                                result_peg_1028.nextPosition();
                            } else {
                                goto out_peg_1031;
                            }
                        }
                        result_peg_1028.setValue((void*) " ");
                            
                    }
                    goto success_peg_1029;
                    out_peg_1031:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1028.getPosition()))){
                                result_peg_1028.nextPosition();
                            } else {
                                goto out_peg_1033;
                            }
                        }
                        result_peg_1028.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1029;
                    out_peg_1033:
                    goto loop_peg_1027;
                    success_peg_1029:
                    ;
                    result_peg_1014.addResult(result_peg_1028);
                } while (true);
                loop_peg_1027:
                ;
                        
            }
            goto success_peg_1018;
            goto out_peg_1016;
            success_peg_1018:
            ;
        
        
        
        result_peg_1014.reset();
            do{
                Result result_peg_1036(result_peg_1014.getPosition());
                {
                
                    Result result_peg_1039(result_peg_1036);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_1039.getPosition()))){
                                result_peg_1039.nextPosition();
                            } else {
                                goto not_peg_1038;
                            }
                        }
                        result_peg_1039.setValue((void*) "]");
                        goto loop_peg_1035;
                        not_peg_1038:
                        result_peg_1036.setValue((void*)0);
                    
                    
                    
                    char temp_peg_1040 = stream.get(result_peg_1036.getPosition());
                        if (temp_peg_1040 != '\0'){
                            result_peg_1036.setValue((void*) temp_peg_1040);
                            result_peg_1036.nextPosition();
                        } else {
                            goto loop_peg_1035;
                        }
                    
                    
                }
                result_peg_1014.addResult(result_peg_1036);
            } while (true);
            loop_peg_1035:
            if (result_peg_1014.matches() == 0){
                goto out_peg_1016;
            }
            data = result_peg_1014.getValues();
        
        
        
        {
                
                result_peg_1014.reset();
                do{
                    Result result_peg_1052(result_peg_1014.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1052.getPosition()))){
                                result_peg_1052.nextPosition();
                            } else {
                                goto out_peg_1055;
                            }
                        }
                        result_peg_1052.setValue((void*) " ");
                            
                    }
                    goto success_peg_1053;
                    out_peg_1055:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1052.getPosition()))){
                                result_peg_1052.nextPosition();
                            } else {
                                goto out_peg_1057;
                            }
                        }
                        result_peg_1052.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1053;
                    out_peg_1057:
                    goto loop_peg_1051;
                    success_peg_1053:
                    ;
                    result_peg_1014.addResult(result_peg_1052);
                } while (true);
                loop_peg_1051:
                ;
                        
            }
            goto success_peg_1042;
            goto out_peg_1016;
            success_peg_1042:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(data);
                result_peg_1014.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1014;
    out_peg_1016:
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1060(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_1060.getPosition()))){
                    result_peg_1060.nextPosition();
                } else {
                    goto out_peg_1062;
                }
            }
            result_peg_1060.setValue((void*) "loopstart");
        
        
        
        {
                Value value((void*) 0);
                value = makeValue();
                result_peg_1060.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1060;
    out_peg_1062:
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1064(myposition);
    
    {
    
        {
                
                char letter_peg_1070 = stream.get(result_peg_1064.getPosition());
                if (letter_peg_1070 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1070) != NULL){
                    result_peg_1064.nextPosition();
                    result_peg_1064.setValue((void*) letter_peg_1070);
                } else {
                    goto out_peg_1069;
                }
                
            }
            goto success_peg_1066;
            out_peg_1069:
            goto out_peg_1071;
            success_peg_1066:
            ;
        
        Result result_peg_1065 = result_peg_1064;
        
        result_peg_1064.reset();
            do{
                Result result_peg_1074(result_peg_1064.getPosition());
                {
                    
                    {
                        
                        char letter_peg_1087 = stream.get(result_peg_1074.getPosition());
                        if (letter_peg_1087 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1087) != NULL){
                            result_peg_1074.nextPosition();
                            result_peg_1074.setValue((void*) letter_peg_1087);
                        } else {
                            goto out_peg_1086;
                        }
                        
                    }
                    goto success_peg_1083;
                    out_peg_1086:
                    goto out_peg_1082;
                    success_peg_1083:
                    ;
                    
                }
                goto success_peg_1075;
                out_peg_1082:
                {
                    
                    {
                        
                        char letter_peg_1099 = stream.get(result_peg_1074.getPosition());
                        if (letter_peg_1099 != '\0' && strchr("0123456789", letter_peg_1099) != NULL){
                            result_peg_1074.nextPosition();
                            result_peg_1074.setValue((void*) letter_peg_1099);
                        } else {
                            goto out_peg_1098;
                        }
                        
                    }
                    goto success_peg_1095;
                    out_peg_1098:
                    goto out_peg_1094;
                    success_peg_1095:
                    ;
                    
                }
                goto success_peg_1075;
                out_peg_1094:
                goto loop_peg_1073;
                success_peg_1075:
                ;
                result_peg_1064.addResult(result_peg_1074);
            } while (true);
            loop_peg_1073:
            ;
        
        Result result_peg_1072 = result_peg_1064;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_1065.getValues().getValue(),result_peg_1072.getValues());
                result_peg_1064.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1064;
    out_peg_1071:
    
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1101(myposition);
    
    result_peg_1101 = rule_line_end(stream, result_peg_1101.getPosition());
    if (result_peg_1101.error()){
        goto out_peg_1102;
    }
    
    
    return result_peg_1101;
    out_peg_1102:
    Result result_peg_1103(myposition);
    
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_1103.getPosition()))){
                        result_peg_1103.nextPosition();
                    } else {
                        goto out_peg_1107;
                    }
                }
                result_peg_1103.setValue((void*) ";");
            
            
            
            result_peg_1103.reset();
                do{
                    Result result_peg_1116(result_peg_1103.getPosition());
                    {
                    
                        Result result_peg_1119(result_peg_1116);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_1119.getPosition()))){
                                    result_peg_1119.nextPosition();
                                } else {
                                    goto not_peg_1118;
                                }
                            }
                            result_peg_1119.setValue((void*) "\n");
                            goto loop_peg_1115;
                            not_peg_1118:
                            result_peg_1116.setValue((void*)0);
                        
                        
                        
                        char temp_peg_1120 = stream.get(result_peg_1116.getPosition());
                            if (temp_peg_1120 != '\0'){
                                result_peg_1116.setValue((void*) temp_peg_1120);
                                result_peg_1116.nextPosition();
                            } else {
                                goto loop_peg_1115;
                            }
                        
                        
                    }
                    result_peg_1103.addResult(result_peg_1116);
                } while (true);
                loop_peg_1115:
                ;
            
            
        }
    }
    goto success_peg_1104;
    out_peg_1107:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1103.getPosition()))){
                        result_peg_1103.nextPosition();
                    } else {
                        goto out_peg_1123;
                    }
                }
                result_peg_1103.setValue((void*) "=");
            
            
            
            result_peg_1103.reset();
                do{
                    Result result_peg_1132(result_peg_1103.getPosition());
                    {
                    
                        Result result_peg_1135(result_peg_1132);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_1135.getPosition()))){
                                    result_peg_1135.nextPosition();
                                } else {
                                    goto not_peg_1134;
                                }
                            }
                            result_peg_1135.setValue((void*) "\n");
                            goto loop_peg_1131;
                            not_peg_1134:
                            result_peg_1132.setValue((void*)0);
                        
                        
                        
                        char temp_peg_1136 = stream.get(result_peg_1132.getPosition());
                            if (temp_peg_1136 != '\0'){
                                result_peg_1132.setValue((void*) temp_peg_1136);
                                result_peg_1132.nextPosition();
                            } else {
                                goto loop_peg_1131;
                            }
                        
                        
                    }
                    result_peg_1103.addResult(result_peg_1132);
                } while (true);
                loop_peg_1131:
                ;
            
            
        }
    }
    goto success_peg_1104;
    out_peg_1123:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1103.getPosition()))){
                        result_peg_1103.nextPosition();
                    } else {
                        goto out_peg_1139;
                    }
                }
                result_peg_1103.setValue((void*) "-");
            
            
            
            result_peg_1103.reset();
                do{
                    Result result_peg_1144(result_peg_1103.getPosition());
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_1144.getPosition()))){
                            result_peg_1144.nextPosition();
                        } else {
                            goto loop_peg_1143;
                        }
                    }
                    result_peg_1144.setValue((void*) "-");
                    result_peg_1103.addResult(result_peg_1144);
                } while (true);
                loop_peg_1143:
                if (result_peg_1103.matches() == 0){
                    goto out_peg_1139;
                }
            
            
        }
    }
    goto success_peg_1104;
    out_peg_1139:
    goto out_peg_1145;
    success_peg_1104:
    ;
    
    
    return result_peg_1103;
    out_peg_1145:
    
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    int myposition = position;
    
    Value file;
    Result result_peg_1147(myposition);
    
    {
    
        Result result_peg_1150(result_peg_1147);
            for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1150.getPosition()))){
                    result_peg_1150.nextPosition();
                } else {
                    goto not_peg_1149;
                }
            }
            result_peg_1150.setValue((void*) "\"");
            goto out_peg_1151;
            not_peg_1149:
            result_peg_1147.setValue((void*)0);
        
        
        
        result_peg_1147.reset();
            do{
                Result result_peg_1154(result_peg_1147.getPosition());
                result_peg_1154 = rule_filename_char(stream, result_peg_1154.getPosition());
                if (result_peg_1154.error()){
                    goto loop_peg_1153;
                }
                result_peg_1147.addResult(result_peg_1154);
            } while (true);
            loop_peg_1153:
            if (result_peg_1147.matches() == 0){
                goto out_peg_1151;
            }
            file = result_peg_1147.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new Ast::Filename(toString(file)); GC::save(as<Ast::Filename*>(value));
                result_peg_1147.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1147;
    out_peg_1151:
    
    
    return errorResult;
}
        

Result rule_filename_char(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1156(myposition);
    
    {
    
        Result result_peg_1159(result_peg_1156);
            for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1159.getPosition()))){
                    result_peg_1159.nextPosition();
                } else {
                    goto not_peg_1158;
                }
            }
            result_peg_1159.setValue((void*) ",");
            goto out_peg_1160;
            not_peg_1158:
            result_peg_1156.setValue((void*)0);
        
        
        
        Result result_peg_1163(result_peg_1156);
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_1163.getPosition()))){
                    result_peg_1163.nextPosition();
                } else {
                    goto not_peg_1162;
                }
            }
            result_peg_1163.setValue((void*) "\n");
            goto out_peg_1160;
            not_peg_1162:
            result_peg_1156.setValue((void*)0);
        
        
        
        Result result_peg_1166(result_peg_1156);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_1166.getPosition()))){
                    result_peg_1166.nextPosition();
                } else {
                    goto not_peg_1165;
                }
            }
            result_peg_1166.setValue((void*) "[");
            goto out_peg_1160;
            not_peg_1165:
            result_peg_1156.setValue((void*)0);
        
        
        
        Result result_peg_1169(result_peg_1156);
            if ((unsigned char) stream.get(result_peg_1169.getPosition()) == (unsigned char) 13){
                result_peg_1169.nextPosition();
            } else {
                goto not_peg_1168;
            }
            result_peg_1169.setValue((void*) 13);
            goto out_peg_1160;
            not_peg_1168:
            result_peg_1156.setValue((void*)0);
        
        
        
        Result result_peg_1172(result_peg_1156);
            for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_1172.getPosition()))){
                    result_peg_1172.nextPosition();
                } else {
                    goto not_peg_1171;
                }
            }
            result_peg_1172.setValue((void*) ";");
            goto out_peg_1160;
            not_peg_1171:
            result_peg_1156.setValue((void*)0);
        
        
        
        char temp_peg_1173 = stream.get(result_peg_1156.getPosition());
            if (temp_peg_1173 != '\0'){
                result_peg_1156.setValue((void*) temp_peg_1173);
                result_peg_1156.nextPosition();
            } else {
                goto out_peg_1160;
            }
        
        
    }
    
    
    return result_peg_1156;
    out_peg_1160:
    
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    int myposition = position;
    
    Value id;
    Value data;
    Value index;
    Result result_peg_1175(myposition);
    
    {
    
        result_peg_1175 = rule_identifier(stream, result_peg_1175.getPosition());
            if (result_peg_1175.error()){
                goto out_peg_1177;
            }
            id = result_peg_1175.getValues();
        
        
        
        {
                
                result_peg_1175.reset();
                do{
                    Result result_peg_1189(result_peg_1175.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1189.getPosition()))){
                                result_peg_1189.nextPosition();
                            } else {
                                goto out_peg_1192;
                            }
                        }
                        result_peg_1189.setValue((void*) " ");
                            
                    }
                    goto success_peg_1190;
                    out_peg_1192:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1189.getPosition()))){
                                result_peg_1189.nextPosition();
                            } else {
                                goto out_peg_1194;
                            }
                        }
                        result_peg_1189.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1190;
                    out_peg_1194:
                    goto loop_peg_1188;
                    success_peg_1190:
                    ;
                    result_peg_1175.addResult(result_peg_1189);
                } while (true);
                loop_peg_1188:
                ;
                        
            }
            goto success_peg_1179;
            goto out_peg_1177;
            success_peg_1179:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1175.getPosition()))){
                    result_peg_1175.nextPosition();
                } else {
                    goto out_peg_1177;
                }
            }
            result_peg_1175.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1175.reset();
                do{
                    Result result_peg_1207(result_peg_1175.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1207.getPosition()))){
                                result_peg_1207.nextPosition();
                            } else {
                                goto out_peg_1210;
                            }
                        }
                        result_peg_1207.setValue((void*) " ");
                            
                    }
                    goto success_peg_1208;
                    out_peg_1210:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1207.getPosition()))){
                                result_peg_1207.nextPosition();
                            } else {
                                goto out_peg_1212;
                            }
                        }
                        result_peg_1207.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1208;
                    out_peg_1212:
                    goto loop_peg_1206;
                    success_peg_1208:
                    ;
                    result_peg_1175.addResult(result_peg_1207);
                } while (true);
                loop_peg_1206:
                ;
                        
            }
            goto success_peg_1197;
            goto out_peg_1177;
            success_peg_1197:
            ;
        
        
        
        Result result_peg_1214(result_peg_1175.getPosition());
            result_peg_1214 = rule_line_end_or_comment(stream, result_peg_1214.getPosition());
            if (result_peg_1214.error()){
                goto out_peg_1177;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id);
                result_peg_1175.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1175;
    out_peg_1177:
    Result result_peg_1215(myposition);
    
    {
    
        result_peg_1215 = rule_identifier(stream, result_peg_1215.getPosition());
            if (result_peg_1215.error()){
                goto out_peg_1217;
            }
            id = result_peg_1215.getValues();
        
        
        
        {
                
                result_peg_1215.reset();
                do{
                    Result result_peg_1229(result_peg_1215.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1229.getPosition()))){
                                result_peg_1229.nextPosition();
                            } else {
                                goto out_peg_1232;
                            }
                        }
                        result_peg_1229.setValue((void*) " ");
                            
                    }
                    goto success_peg_1230;
                    out_peg_1232:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1229.getPosition()))){
                                result_peg_1229.nextPosition();
                            } else {
                                goto out_peg_1234;
                            }
                        }
                        result_peg_1229.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1230;
                    out_peg_1234:
                    goto loop_peg_1228;
                    success_peg_1230:
                    ;
                    result_peg_1215.addResult(result_peg_1229);
                } while (true);
                loop_peg_1228:
                ;
                        
            }
            goto success_peg_1219;
            goto out_peg_1217;
            success_peg_1219:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1215.getPosition()))){
                    result_peg_1215.nextPosition();
                } else {
                    goto out_peg_1217;
                }
            }
            result_peg_1215.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1215.reset();
                do{
                    Result result_peg_1247(result_peg_1215.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1247.getPosition()))){
                                result_peg_1247.nextPosition();
                            } else {
                                goto out_peg_1250;
                            }
                        }
                        result_peg_1247.setValue((void*) " ");
                            
                    }
                    goto success_peg_1248;
                    out_peg_1250:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1247.getPosition()))){
                                result_peg_1247.nextPosition();
                            } else {
                                goto out_peg_1252;
                            }
                        }
                        result_peg_1247.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1248;
                    out_peg_1252:
                    goto loop_peg_1246;
                    success_peg_1248:
                    ;
                    result_peg_1215.addResult(result_peg_1247);
                } while (true);
                loop_peg_1246:
                ;
                        
            }
            goto success_peg_1237;
            goto out_peg_1217;
            success_peg_1237:
            ;
        
        
        
        result_peg_1215 = rule_valuelist(stream, result_peg_1215.getPosition());
            if (result_peg_1215.error()){
                goto out_peg_1217;
            }
            data = result_peg_1215.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id, data);
                result_peg_1215.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1215;
    out_peg_1217:
    Result result_peg_1254(myposition);
    
    {
    
        result_peg_1254 = rule_identifier(stream, result_peg_1254.getPosition());
            if (result_peg_1254.error()){
                goto out_peg_1256;
            }
            id = result_peg_1254.getValues();
        
        
        
        {
                
                result_peg_1254.reset();
                do{
                    Result result_peg_1268(result_peg_1254.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1268.getPosition()))){
                                result_peg_1268.nextPosition();
                            } else {
                                goto out_peg_1271;
                            }
                        }
                        result_peg_1268.setValue((void*) " ");
                            
                    }
                    goto success_peg_1269;
                    out_peg_1271:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1268.getPosition()))){
                                result_peg_1268.nextPosition();
                            } else {
                                goto out_peg_1273;
                            }
                        }
                        result_peg_1268.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1269;
                    out_peg_1273:
                    goto loop_peg_1267;
                    success_peg_1269:
                    ;
                    result_peg_1254.addResult(result_peg_1268);
                } while (true);
                loop_peg_1267:
                ;
                        
            }
            goto success_peg_1258;
            goto out_peg_1256;
            success_peg_1258:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1254.getPosition()))){
                    result_peg_1254.nextPosition();
                } else {
                    goto out_peg_1256;
                }
            }
            result_peg_1254.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1254.reset();
                do{
                    Result result_peg_1286(result_peg_1254.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1286.getPosition()))){
                                result_peg_1286.nextPosition();
                            } else {
                                goto out_peg_1289;
                            }
                        }
                        result_peg_1286.setValue((void*) " ");
                            
                    }
                    goto success_peg_1287;
                    out_peg_1289:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1286.getPosition()))){
                                result_peg_1286.nextPosition();
                            } else {
                                goto out_peg_1291;
                            }
                        }
                        result_peg_1286.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1287;
                    out_peg_1291:
                    goto loop_peg_1285;
                    success_peg_1287:
                    ;
                    result_peg_1254.addResult(result_peg_1286);
                } while (true);
                loop_peg_1285:
                ;
                        
            }
            goto success_peg_1276;
            goto out_peg_1256;
            success_peg_1276:
            ;
        
        
        
        result_peg_1254 = rule_filename(stream, result_peg_1254.getPosition());
            if (result_peg_1254.error()){
                goto out_peg_1256;
            }
            data = result_peg_1254.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeFilename(id, data);
                result_peg_1254.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1254;
    out_peg_1256:
    Result result_peg_1293(myposition);
    
    {
    
        result_peg_1293 = rule_identifier(stream, result_peg_1293.getPosition());
            if (result_peg_1293.error()){
                goto out_peg_1295;
            }
            id = result_peg_1293.getValues();
        
        
        
        {
                
                result_peg_1293.reset();
                do{
                    Result result_peg_1307(result_peg_1293.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1307.getPosition()))){
                                result_peg_1307.nextPosition();
                            } else {
                                goto out_peg_1310;
                            }
                        }
                        result_peg_1307.setValue((void*) " ");
                            
                    }
                    goto success_peg_1308;
                    out_peg_1310:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1307.getPosition()))){
                                result_peg_1307.nextPosition();
                            } else {
                                goto out_peg_1312;
                            }
                        }
                        result_peg_1307.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1308;
                    out_peg_1312:
                    goto loop_peg_1306;
                    success_peg_1308:
                    ;
                    result_peg_1293.addResult(result_peg_1307);
                } while (true);
                loop_peg_1306:
                ;
                        
            }
            goto success_peg_1297;
            goto out_peg_1295;
            success_peg_1297:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1293.getPosition()))){
                    result_peg_1293.nextPosition();
                } else {
                    goto out_peg_1295;
                }
            }
            result_peg_1293.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1293.reset();
                do{
                    Result result_peg_1325(result_peg_1293.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1325.getPosition()))){
                                result_peg_1325.nextPosition();
                            } else {
                                goto out_peg_1328;
                            }
                        }
                        result_peg_1325.setValue((void*) " ");
                            
                    }
                    goto success_peg_1326;
                    out_peg_1328:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1325.getPosition()))){
                                result_peg_1325.nextPosition();
                            } else {
                                goto out_peg_1330;
                            }
                        }
                        result_peg_1325.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1326;
                    out_peg_1330:
                    goto loop_peg_1324;
                    success_peg_1326:
                    ;
                    result_peg_1293.addResult(result_peg_1325);
                } while (true);
                loop_peg_1324:
                ;
                        
            }
            goto success_peg_1315;
            goto out_peg_1295;
            success_peg_1315:
            ;
        
        
        
        result_peg_1293 = rule_number(stream, result_peg_1293.getPosition());
            if (result_peg_1293.error()){
                goto out_peg_1295;
            }
            index = result_peg_1293.getValues();
        
        
        
        {
                
                result_peg_1293.reset();
                do{
                    Result result_peg_1343(result_peg_1293.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1343.getPosition()))){
                                result_peg_1343.nextPosition();
                            } else {
                                goto out_peg_1346;
                            }
                        }
                        result_peg_1343.setValue((void*) " ");
                            
                    }
                    goto success_peg_1344;
                    out_peg_1346:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1343.getPosition()))){
                                result_peg_1343.nextPosition();
                            } else {
                                goto out_peg_1348;
                            }
                        }
                        result_peg_1343.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1344;
                    out_peg_1348:
                    goto loop_peg_1342;
                    success_peg_1344:
                    ;
                    result_peg_1293.addResult(result_peg_1343);
                } while (true);
                loop_peg_1342:
                ;
                        
            }
            goto success_peg_1333;
            goto out_peg_1295;
            success_peg_1333:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1293.getPosition()))){
                    result_peg_1293.nextPosition();
                } else {
                    goto out_peg_1295;
                }
            }
            result_peg_1293.setValue((void*) ")");
        
        
        
        {
                
                result_peg_1293.reset();
                do{
                    Result result_peg_1361(result_peg_1293.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1361.getPosition()))){
                                result_peg_1361.nextPosition();
                            } else {
                                goto out_peg_1364;
                            }
                        }
                        result_peg_1361.setValue((void*) " ");
                            
                    }
                    goto success_peg_1362;
                    out_peg_1364:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1361.getPosition()))){
                                result_peg_1361.nextPosition();
                            } else {
                                goto out_peg_1366;
                            }
                        }
                        result_peg_1361.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1362;
                    out_peg_1366:
                    goto loop_peg_1360;
                    success_peg_1362:
                    ;
                    result_peg_1293.addResult(result_peg_1361);
                } while (true);
                loop_peg_1360:
                ;
                        
            }
            goto success_peg_1351;
            goto out_peg_1295;
            success_peg_1351:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1293.getPosition()))){
                    result_peg_1293.nextPosition();
                } else {
                    goto out_peg_1295;
                }
            }
            result_peg_1293.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1293.reset();
                do{
                    Result result_peg_1379(result_peg_1293.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1379.getPosition()))){
                                result_peg_1379.nextPosition();
                            } else {
                                goto out_peg_1382;
                            }
                        }
                        result_peg_1379.setValue((void*) " ");
                            
                    }
                    goto success_peg_1380;
                    out_peg_1382:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1379.getPosition()))){
                                result_peg_1379.nextPosition();
                            } else {
                                goto out_peg_1384;
                            }
                        }
                        result_peg_1379.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1380;
                    out_peg_1384:
                    goto loop_peg_1378;
                    success_peg_1380:
                    ;
                    result_peg_1293.addResult(result_peg_1379);
                } while (true);
                loop_peg_1378:
                ;
                        
            }
            goto success_peg_1369;
            goto out_peg_1295;
            success_peg_1369:
            ;
        
        
        
        result_peg_1293 = rule_valuelist(stream, result_peg_1293.getPosition());
            if (result_peg_1293.error()){
                goto out_peg_1295;
            }
            data = result_peg_1293.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(id, index, data);
                result_peg_1293.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1293;
    out_peg_1295:
    Result result_peg_1386(myposition);
    
    {
    
        result_peg_1386 = rule_identifier_list(stream, result_peg_1386.getPosition());
            if (result_peg_1386.error()){
                goto out_peg_1388;
            }
            id = result_peg_1386.getValues();
        
        
        
        {
                
                result_peg_1386.reset();
                do{
                    Result result_peg_1400(result_peg_1386.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1400.getPosition()))){
                                result_peg_1400.nextPosition();
                            } else {
                                goto out_peg_1403;
                            }
                        }
                        result_peg_1400.setValue((void*) " ");
                            
                    }
                    goto success_peg_1401;
                    out_peg_1403:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1400.getPosition()))){
                                result_peg_1400.nextPosition();
                            } else {
                                goto out_peg_1405;
                            }
                        }
                        result_peg_1400.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1401;
                    out_peg_1405:
                    goto loop_peg_1399;
                    success_peg_1401:
                    ;
                    result_peg_1386.addResult(result_peg_1400);
                } while (true);
                loop_peg_1399:
                ;
                        
            }
            goto success_peg_1390;
            goto out_peg_1388;
            success_peg_1390:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1386.getPosition()))){
                    result_peg_1386.nextPosition();
                } else {
                    goto out_peg_1388;
                }
            }
            result_peg_1386.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1386.reset();
                do{
                    Result result_peg_1418(result_peg_1386.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1418.getPosition()))){
                                result_peg_1418.nextPosition();
                            } else {
                                goto out_peg_1421;
                            }
                        }
                        result_peg_1418.setValue((void*) " ");
                            
                    }
                    goto success_peg_1419;
                    out_peg_1421:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1418.getPosition()))){
                                result_peg_1418.nextPosition();
                            } else {
                                goto out_peg_1423;
                            }
                        }
                        result_peg_1418.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1419;
                    out_peg_1423:
                    goto loop_peg_1417;
                    success_peg_1419:
                    ;
                    result_peg_1386.addResult(result_peg_1418);
                } while (true);
                loop_peg_1417:
                ;
                        
            }
            goto success_peg_1408;
            goto out_peg_1388;
            success_peg_1408:
            ;
        
        
        
        result_peg_1386 = rule_valuelist(stream, result_peg_1386.getPosition());
            if (result_peg_1386.error()){
                goto out_peg_1388;
            }
            data = result_peg_1386.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributes(id, data);
                result_peg_1386.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1386;
    out_peg_1388:
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1426(myposition);
    
    {
    
        result_peg_1426 = rule_name(stream, result_peg_1426.getPosition());
            if (result_peg_1426.error()){
                goto out_peg_1428;
            }
        
        Result result_peg_1427 = result_peg_1426;
        
        result_peg_1426.reset();
            do{
                Result result_peg_1431(result_peg_1426.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_1431.getPosition()))){
                                result_peg_1431.nextPosition();
                            } else {
                                goto loop_peg_1430;
                            }
                        }
                        result_peg_1431.setValue((void*) ".");
                    
                    
                    
                    result_peg_1431 = rule_name(stream, result_peg_1431.getPosition());
                        if (result_peg_1431.error()){
                            goto loop_peg_1430;
                        }
                    
                    
                }
                result_peg_1426.addResult(result_peg_1431);
            } while (true);
            loop_peg_1430:
            ;
        
        Result result_peg_1429 = result_peg_1426;
        
        {
                Value value((void*) 0);
                value = makeIdentifier(result_peg_1427.getValues(),result_peg_1429.getValues());
                result_peg_1426.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1426;
    out_peg_1428:
    
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1434(myposition);
    
    {
    
        result_peg_1434 = rule_identifier(stream, result_peg_1434.getPosition());
            if (result_peg_1434.error()){
                goto out_peg_1436;
            }
        
        
        
        result_peg_1434.reset();
            do{
                Result result_peg_1438(result_peg_1434.getPosition());
                {
                
                    {
                            
                            result_peg_1438.reset();
                            do{
                                Result result_peg_1450(result_peg_1438.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1450.getPosition()))){
                                            result_peg_1450.nextPosition();
                                        } else {
                                            goto out_peg_1453;
                                        }
                                    }
                                    result_peg_1450.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1451;
                                out_peg_1453:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1450.getPosition()))){
                                            result_peg_1450.nextPosition();
                                        } else {
                                            goto out_peg_1455;
                                        }
                                    }
                                    result_peg_1450.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1451;
                                out_peg_1455:
                                goto loop_peg_1449;
                                success_peg_1451:
                                ;
                                result_peg_1438.addResult(result_peg_1450);
                            } while (true);
                            loop_peg_1449:
                            ;
                                    
                        }
                        goto success_peg_1440;
                        goto loop_peg_1437;
                        success_peg_1440:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1438.getPosition()))){
                                result_peg_1438.nextPosition();
                            } else {
                                goto loop_peg_1437;
                            }
                        }
                        result_peg_1438.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_1438.reset();
                            do{
                                Result result_peg_1468(result_peg_1438.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1468.getPosition()))){
                                            result_peg_1468.nextPosition();
                                        } else {
                                            goto out_peg_1471;
                                        }
                                    }
                                    result_peg_1468.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1469;
                                out_peg_1471:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1468.getPosition()))){
                                            result_peg_1468.nextPosition();
                                        } else {
                                            goto out_peg_1473;
                                        }
                                    }
                                    result_peg_1468.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1469;
                                out_peg_1473:
                                goto loop_peg_1467;
                                success_peg_1469:
                                ;
                                result_peg_1438.addResult(result_peg_1468);
                            } while (true);
                            loop_peg_1467:
                            ;
                                    
                        }
                        goto success_peg_1458;
                        goto loop_peg_1437;
                        success_peg_1458:
                        ;
                    
                    
                    
                    result_peg_1438 = rule_filename(stream, result_peg_1438.getPosition());
                        if (result_peg_1438.error()){
                            goto loop_peg_1437;
                        }
                    
                    
                }
                result_peg_1434.addResult(result_peg_1438);
            } while (true);
            loop_peg_1437:
            if (result_peg_1434.matches() == 0){
                goto out_peg_1436;
            }
        
        
    }
    
    
    return result_peg_1434;
    out_peg_1436:
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1475(myposition);
    
    {
    
        result_peg_1475 = rule_value(stream, result_peg_1475.getPosition());
            if (result_peg_1475.error()){
                goto out_peg_1477;
            }
        
        Result result_peg_1476 = result_peg_1475;
        
        result_peg_1475.reset();
            do{
                Result result_peg_1480(result_peg_1475.getPosition());
                {
                
                    {
                            
                            result_peg_1480.reset();
                            do{
                                Result result_peg_1492(result_peg_1480.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1492.getPosition()))){
                                            result_peg_1492.nextPosition();
                                        } else {
                                            goto out_peg_1495;
                                        }
                                    }
                                    result_peg_1492.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1493;
                                out_peg_1495:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1492.getPosition()))){
                                            result_peg_1492.nextPosition();
                                        } else {
                                            goto out_peg_1497;
                                        }
                                    }
                                    result_peg_1492.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1493;
                                out_peg_1497:
                                goto loop_peg_1491;
                                success_peg_1493:
                                ;
                                result_peg_1480.addResult(result_peg_1492);
                            } while (true);
                            loop_peg_1491:
                            ;
                                    
                        }
                        goto success_peg_1482;
                        goto loop_peg_1479;
                        success_peg_1482:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1480.getPosition()))){
                                result_peg_1480.nextPosition();
                            } else {
                                goto loop_peg_1479;
                            }
                        }
                        result_peg_1480.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_1480.reset();
                            do{
                                Result result_peg_1510(result_peg_1480.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1510.getPosition()))){
                                            result_peg_1510.nextPosition();
                                        } else {
                                            goto out_peg_1513;
                                        }
                                    }
                                    result_peg_1510.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1511;
                                out_peg_1513:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1510.getPosition()))){
                                            result_peg_1510.nextPosition();
                                        } else {
                                            goto out_peg_1515;
                                        }
                                    }
                                    result_peg_1510.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1511;
                                out_peg_1515:
                                goto loop_peg_1509;
                                success_peg_1511:
                                ;
                                result_peg_1480.addResult(result_peg_1510);
                            } while (true);
                            loop_peg_1509:
                            ;
                                    
                        }
                        goto success_peg_1500;
                        goto loop_peg_1479;
                        success_peg_1500:
                        ;
                    
                    
                    
                    int save_peg_1516 = result_peg_1480.getPosition();
                        
                        result_peg_1480 = rule_value(stream, result_peg_1480.getPosition());
                        if (result_peg_1480.error()){
                            
                            result_peg_1480 = Result(save_peg_1516);
                            result_peg_1480.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1475.addResult(result_peg_1480);
            } while (true);
            loop_peg_1479:
            ;
        
        Result result_peg_1478 = result_peg_1475;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_1476.getValues(),result_peg_1478.getValues());
                result_peg_1475.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1475;
    out_peg_1477:
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1518(myposition);
    
    result_peg_1518 = rule_string(stream, result_peg_1518.getPosition());
    if (result_peg_1518.error()){
        goto out_peg_1519;
    }
    
    
    return result_peg_1518;
    out_peg_1519:
    Result result_peg_1520(myposition);
    
    result_peg_1520 = rule_date(stream, result_peg_1520.getPosition());
    if (result_peg_1520.error()){
        goto out_peg_1521;
    }
    
    
    return result_peg_1520;
    out_peg_1521:
    Result result_peg_1522(myposition);
    
    result_peg_1522 = rule_number(stream, result_peg_1522.getPosition());
    if (result_peg_1522.error()){
        goto out_peg_1523;
    }
    
    
    return result_peg_1522;
    out_peg_1523:
    Result result_peg_1524(myposition);
    
    {
    
        result_peg_1524 = rule_keyword(stream, result_peg_1524.getPosition());
            if (result_peg_1524.error()){
                goto out_peg_1526;
            }
        
        Result result_peg_1525 = result_peg_1524;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1525.getValues());
                result_peg_1524.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1524;
    out_peg_1526:
    Result result_peg_1527(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_1527.getPosition()))){
                    result_peg_1527.nextPosition();
                } else {
                    goto out_peg_1529;
                }
            }
            result_peg_1527.setValue((void*) "s");
        
        Result result_peg_1528 = result_peg_1527;
        
        Result result_peg_1532(result_peg_1527);
            {
                
                char letter_peg_1537 = stream.get(result_peg_1532.getPosition());
                if (letter_peg_1537 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1537) != NULL){
                    result_peg_1532.nextPosition();
                    result_peg_1532.setValue((void*) letter_peg_1537);
                } else {
                    goto out_peg_1536;
                }
                
            }
            goto success_peg_1533;
            out_peg_1536:
            goto not_peg_1531;
            success_peg_1533:
            ;
            goto out_peg_1529;
            not_peg_1531:
            result_peg_1527.setValue((void*)0);
        
        
        
        Result result_peg_1540(result_peg_1527);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1540.getPosition()))){
                    result_peg_1540.nextPosition();
                } else {
                    goto not_peg_1539;
                }
            }
            result_peg_1540.setValue((void*) ".");
            goto out_peg_1529;
            not_peg_1539:
            result_peg_1527.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1528.getValues());
                result_peg_1527.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1527;
    out_peg_1529:
    Result result_peg_1541(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_1541.getPosition()))){
                    result_peg_1541.nextPosition();
                } else {
                    goto out_peg_1543;
                }
            }
            result_peg_1541.setValue((void*) "h");
        
        Result result_peg_1542 = result_peg_1541;
        
        Result result_peg_1546(result_peg_1541);
            {
                
                char letter_peg_1551 = stream.get(result_peg_1546.getPosition());
                if (letter_peg_1551 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1551) != NULL){
                    result_peg_1546.nextPosition();
                    result_peg_1546.setValue((void*) letter_peg_1551);
                } else {
                    goto out_peg_1550;
                }
                
            }
            goto success_peg_1547;
            out_peg_1550:
            goto not_peg_1545;
            success_peg_1547:
            ;
            goto out_peg_1543;
            not_peg_1545:
            result_peg_1541.setValue((void*)0);
        
        
        
        Result result_peg_1554(result_peg_1541);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1554.getPosition()))){
                    result_peg_1554.nextPosition();
                } else {
                    goto not_peg_1553;
                }
            }
            result_peg_1554.setValue((void*) ".");
            goto out_peg_1543;
            not_peg_1553:
            result_peg_1541.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1542.getValues());
                result_peg_1541.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1541;
    out_peg_1543:
    Result result_peg_1555(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1555.getPosition()))){
                    result_peg_1555.nextPosition();
                } else {
                    goto out_peg_1557;
                }
            }
            result_peg_1555.setValue((void*) "a");
        
        Result result_peg_1556 = result_peg_1555;
        
        int save_peg_1559 = result_peg_1555.getPosition();
            
            result_peg_1555 = rule_number(stream, result_peg_1555.getPosition());
            if (result_peg_1555.error()){
                
                result_peg_1555 = Result(save_peg_1559);
                result_peg_1555.setValue((void*) 0);
                
            }
        
        
        
        Result result_peg_1562(result_peg_1555);
            {
                
                char letter_peg_1567 = stream.get(result_peg_1562.getPosition());
                if (letter_peg_1567 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1567) != NULL){
                    result_peg_1562.nextPosition();
                    result_peg_1562.setValue((void*) letter_peg_1567);
                } else {
                    goto out_peg_1566;
                }
                
            }
            goto success_peg_1563;
            out_peg_1566:
            goto not_peg_1561;
            success_peg_1563:
            ;
            goto out_peg_1557;
            not_peg_1561:
            result_peg_1555.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1556.getValues());
                result_peg_1555.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1555;
    out_peg_1557:
    Result result_peg_1568(myposition);
    
    result_peg_1568 = rule_filename(stream, result_peg_1568.getPosition());
    if (result_peg_1568.error()){
        goto out_peg_1569;
    }
    
    
    return result_peg_1568;
    out_peg_1569:
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1571(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareChar("normal"[i], stream.get(result_peg_1571.getPosition()))){
            result_peg_1571.nextPosition();
        } else {
            goto out_peg_1572;
        }
    }
    result_peg_1571.setValue((void*) "normal");
    
    
    return result_peg_1571;
    out_peg_1572:
    Result result_peg_1573(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("parallax"[i], stream.get(result_peg_1573.getPosition()))){
            result_peg_1573.nextPosition();
        } else {
            goto out_peg_1574;
        }
    }
    result_peg_1573.setValue((void*) "parallax");
    
    
    return result_peg_1573;
    out_peg_1574:
    Result result_peg_1575(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("addalpha"[i], stream.get(result_peg_1575.getPosition()))){
            result_peg_1575.nextPosition();
        } else {
            goto out_peg_1576;
        }
    }
    result_peg_1575.setValue((void*) "addalpha");
    
    
    return result_peg_1575;
    out_peg_1576:
    Result result_peg_1577(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareChar("add1"[i], stream.get(result_peg_1577.getPosition()))){
            result_peg_1577.nextPosition();
        } else {
            goto out_peg_1578;
        }
    }
    result_peg_1577.setValue((void*) "add1");
    
    
    return result_peg_1577;
    out_peg_1578:
    Result result_peg_1579(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("add"[i], stream.get(result_peg_1579.getPosition()))){
            result_peg_1579.nextPosition();
        } else {
            goto out_peg_1580;
        }
    }
    result_peg_1579.setValue((void*) "add");
    
    
    return result_peg_1579;
    out_peg_1580:
    Result result_peg_1581(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("sub"[i], stream.get(result_peg_1581.getPosition()))){
            result_peg_1581.nextPosition();
        } else {
            goto out_peg_1582;
        }
    }
    result_peg_1581.setValue((void*) "sub");
    
    
    return result_peg_1581;
    out_peg_1582:
    
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1584(myposition);
    
    {
    
        result_peg_1584.reset();
            do{
                Result result_peg_1587(result_peg_1584.getPosition());
                {
                    
                    char letter_peg_1592 = stream.get(result_peg_1587.getPosition());
                    if (letter_peg_1592 != '\0' && strchr("0123456789", letter_peg_1592) != NULL){
                        result_peg_1587.nextPosition();
                        result_peg_1587.setValue((void*) letter_peg_1592);
                    } else {
                        goto out_peg_1591;
                    }
                    
                }
                goto success_peg_1588;
                out_peg_1591:
                goto loop_peg_1586;
                success_peg_1588:
                ;
                result_peg_1584.addResult(result_peg_1587);
            } while (true);
            loop_peg_1586:
            if (result_peg_1584.matches() == 0){
                goto out_peg_1593;
            }
        
        Result result_peg_1585 = result_peg_1584;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1584.getPosition()))){
                    result_peg_1584.nextPosition();
                } else {
                    goto out_peg_1593;
                }
            }
            result_peg_1584.setValue((void*) ".");
        
        
        
        result_peg_1584.reset();
            do{
                Result result_peg_1597(result_peg_1584.getPosition());
                {
                    
                    char letter_peg_1602 = stream.get(result_peg_1597.getPosition());
                    if (letter_peg_1602 != '\0' && strchr("0123456789", letter_peg_1602) != NULL){
                        result_peg_1597.nextPosition();
                        result_peg_1597.setValue((void*) letter_peg_1602);
                    } else {
                        goto out_peg_1601;
                    }
                    
                }
                goto success_peg_1598;
                out_peg_1601:
                goto loop_peg_1596;
                success_peg_1598:
                ;
                result_peg_1584.addResult(result_peg_1597);
            } while (true);
            loop_peg_1596:
            if (result_peg_1584.matches() == 0){
                goto out_peg_1593;
            }
        
        Result result_peg_1595 = result_peg_1584;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1584.getPosition()))){
                    result_peg_1584.nextPosition();
                } else {
                    goto out_peg_1593;
                }
            }
            result_peg_1584.setValue((void*) ".");
        
        
        
        result_peg_1584.reset();
            do{
                Result result_peg_1606(result_peg_1584.getPosition());
                {
                    
                    char letter_peg_1611 = stream.get(result_peg_1606.getPosition());
                    if (letter_peg_1611 != '\0' && strchr("0123456789", letter_peg_1611) != NULL){
                        result_peg_1606.nextPosition();
                        result_peg_1606.setValue((void*) letter_peg_1611);
                    } else {
                        goto out_peg_1610;
                    }
                    
                }
                goto success_peg_1607;
                out_peg_1610:
                goto loop_peg_1605;
                success_peg_1607:
                ;
                result_peg_1584.addResult(result_peg_1606);
            } while (true);
            loop_peg_1605:
            if (result_peg_1584.matches() == 0){
                goto out_peg_1593;
            }
        
        Result result_peg_1604 = result_peg_1584;
        
        {
                Value value((void*) 0);
                value = makeDate(result_peg_1585.getValues(),result_peg_1595.getValues(),result_peg_1604.getValues());
                result_peg_1584.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1584;
    out_peg_1593:
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    int myposition = position;
    
    Value data;
    Result result_peg_1613(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1613.getPosition()))){
                    result_peg_1613.nextPosition();
                } else {
                    goto out_peg_1615;
                }
            }
            result_peg_1613.setValue((void*) "\"");
        
        
        
        result_peg_1613.reset();
            do{
                Result result_peg_1618(result_peg_1613.getPosition());
                {
                
                    Result result_peg_1621(result_peg_1618);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_1621.getPosition()))){
                                result_peg_1621.nextPosition();
                            } else {
                                goto not_peg_1620;
                            }
                        }
                        result_peg_1621.setValue((void*) "\"");
                        goto loop_peg_1617;
                        not_peg_1620:
                        result_peg_1618.setValue((void*)0);
                    
                    
                    
                    Result result_peg_1624(result_peg_1618);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_1624.getPosition()))){
                                result_peg_1624.nextPosition();
                            } else {
                                goto not_peg_1623;
                            }
                        }
                        result_peg_1624.setValue((void*) "\n");
                        goto loop_peg_1617;
                        not_peg_1623:
                        result_peg_1618.setValue((void*)0);
                    
                    
                    
                    char temp_peg_1625 = stream.get(result_peg_1618.getPosition());
                        if (temp_peg_1625 != '\0'){
                            result_peg_1618.setValue((void*) temp_peg_1625);
                            result_peg_1618.nextPosition();
                        } else {
                            goto loop_peg_1617;
                        }
                    
                    
                }
                result_peg_1613.addResult(result_peg_1618);
            } while (true);
            loop_peg_1617:
            ;
            data = result_peg_1613.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1613.getPosition()))){
                    result_peg_1613.nextPosition();
                } else {
                    goto out_peg_1615;
                }
            }
            result_peg_1613.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(data);
                result_peg_1613.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1613;
    out_peg_1615:
    
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    int myposition = position;
    
    
    Result result_peg_1628(myposition);
    
    {
    
        int save_peg_1630 = result_peg_1628.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1628.getPosition()))){
                        result_peg_1628.nextPosition();
                    } else {
                        goto out_peg_1633;
                    }
                }
                result_peg_1628.setValue((void*) "+");
                    
            }
            goto success_peg_1631;
            out_peg_1633:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1628.getPosition()))){
                        result_peg_1628.nextPosition();
                    } else {
                        goto out_peg_1635;
                    }
                }
                result_peg_1628.setValue((void*) "-");
                    
            }
            goto success_peg_1631;
            out_peg_1635:
            
            result_peg_1628 = Result(save_peg_1630);
            result_peg_1628.setValue((void*) 0);
            
            success_peg_1631:
            ;
        
        Result result_peg_1629 = result_peg_1628;
        
        result_peg_1628 = rule_float_or_integer(stream, result_peg_1628.getPosition());
            if (result_peg_1628.error()){
                goto out_peg_1637;
            }
        
        Result result_peg_1636 = result_peg_1628;
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_1629.getValues(),result_peg_1636.getValues());
                result_peg_1628.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1628;
    out_peg_1637:
    
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_1639(myposition);
    
    {
    
        result_peg_1639.reset();
            do{
                Result result_peg_1642(result_peg_1639.getPosition());
                {
                    
                    char letter_peg_1647 = stream.get(result_peg_1642.getPosition());
                    if (letter_peg_1647 != '\0' && strchr("0123456789", letter_peg_1647) != NULL){
                        result_peg_1642.nextPosition();
                        result_peg_1642.setValue((void*) letter_peg_1647);
                    } else {
                        goto out_peg_1646;
                    }
                    
                }
                goto success_peg_1643;
                out_peg_1646:
                goto loop_peg_1641;
                success_peg_1643:
                ;
                result_peg_1639.addResult(result_peg_1642);
            } while (true);
            loop_peg_1641:
            ;
            left = result_peg_1639.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1639.getPosition()))){
                    result_peg_1639.nextPosition();
                } else {
                    goto out_peg_1649;
                }
            }
            result_peg_1639.setValue((void*) ".");
        
        
        
        result_peg_1639.reset();
            do{
                Result result_peg_1652(result_peg_1639.getPosition());
                {
                    
                    char letter_peg_1657 = stream.get(result_peg_1652.getPosition());
                    if (letter_peg_1657 != '\0' && strchr("0123456789", letter_peg_1657) != NULL){
                        result_peg_1652.nextPosition();
                        result_peg_1652.setValue((void*) letter_peg_1657);
                    } else {
                        goto out_peg_1656;
                    }
                    
                }
                goto success_peg_1653;
                out_peg_1656:
                goto loop_peg_1651;
                success_peg_1653:
                ;
                result_peg_1639.addResult(result_peg_1652);
            } while (true);
            loop_peg_1651:
            if (result_peg_1639.matches() == 0){
                goto out_peg_1649;
            }
            right = result_peg_1639.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(left,right);
                result_peg_1639.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1639;
    out_peg_1649:
    Result result_peg_1658(myposition);
    
    {
    
        result_peg_1658.reset();
            do{
                Result result_peg_1661(result_peg_1658.getPosition());
                {
                    
                    char letter_peg_1666 = stream.get(result_peg_1661.getPosition());
                    if (letter_peg_1666 != '\0' && strchr("0123456789", letter_peg_1666) != NULL){
                        result_peg_1661.nextPosition();
                        result_peg_1661.setValue((void*) letter_peg_1666);
                    } else {
                        goto out_peg_1665;
                    }
                    
                }
                goto success_peg_1662;
                out_peg_1665:
                goto loop_peg_1660;
                success_peg_1662:
                ;
                result_peg_1658.addResult(result_peg_1661);
            } while (true);
            loop_peg_1660:
            if (result_peg_1658.matches() == 0){
                goto out_peg_1667;
            }
        
        Result result_peg_1659 = result_peg_1658;
        
        Result result_peg_1670(result_peg_1658);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1670.getPosition()))){
                    result_peg_1670.nextPosition();
                } else {
                    goto not_peg_1669;
                }
            }
            result_peg_1670.setValue((void*) ".");
            goto out_peg_1667;
            not_peg_1669:
            result_peg_1658.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_1659.getValues());
                result_peg_1658.setValue(value);
            }
        
        
    }
    
    
    return result_peg_1658;
    out_peg_1667:
    
    
    return errorResult;
}
        

const void * main(const std::string & filename, bool stats = false){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        // std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}

const void * main(const char * in, bool stats = false){
    Stream stream(in);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        // std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}



    
    } /* Def */
    
} /* Mugen */

        
