

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

    int hitCount(){
        return 
(chunk3 != NULL ? ((chunk3->chunk_identifier_list.calculated() ? 1 : 0)
+ (chunk3->chunk_valuelist.calculated() ? 1 : 0)
+ (chunk3->chunk_value.calculated() ? 1 : 0)
+ (chunk3->chunk_keyword.calculated() ? 1 : 0)
+ (chunk3->chunk_date.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_line_end_or_comment.calculated() ? 1 : 0)
+ (chunk2->chunk_filename.calculated() ? 1 : 0)
+ (chunk2->chunk_filename_char.calculated() ? 1 : 0)
+ (chunk2->chunk_attribute.calculated() ? 1 : 0)
+ (chunk2->chunk_identifier.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_section_line.calculated() ? 1 : 0)
+ (chunk1->chunk_section_start.calculated() ? 1 : 0)
+ (chunk1->chunk_loopstart.calculated() ? 1 : 0)
+ (chunk1->chunk_name.calculated() ? 1 : 0)
+ (chunk1->chunk_alpha_digit.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)
+ (chunk0->chunk_s.calculated() ? 1 : 0)
+ (chunk0->chunk_section.calculated() ? 1 : 0)) : 0)
+
(chunk4 != NULL ? ((chunk4->chunk_string.calculated() ? 1 : 0)
+ (chunk4->chunk_number.calculated() ? 1 : 0)
+ (chunk4->chunk_float_or_integer.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 23;
    }

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

std::string ParseException::getReason() const {
    return message;
}

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
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_start.calculated()){
        return column_peg_1.chunk0->chunk_start;
    }
    
    int myposition = position;
    
    Value current;
    Result result_peg_2(myposition);
    
    {
    
        {
                Value value((void*) 0);
                value = makeSectionList(); GC::save(as<SectionList*>(value));
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
    
    if (column_peg_1.chunk0 == 0){
        column_peg_1.chunk0 = new Chunk0();
    }
    column_peg_1.chunk0->chunk_start = result_peg_2;
    stream.update(result_peg_2.getPosition());
    
    
    return result_peg_2;
    out_peg_189:
    
    if (column_peg_1.chunk0 == 0){
        column_peg_1.chunk0 = new Chunk0();
    }
    column_peg_1.chunk0->chunk_start = errorResult;
    stream.update(errorResult.getPosition());
    
     GC::cleanup(as<SectionList*>(current)); 
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    Column & column_peg_389 = stream.getColumn(position);
    if (column_peg_389.chunk0 != 0 && column_peg_389.chunk0->chunk_line.calculated()){
        return column_peg_389.chunk0->chunk_line;
    }
    
    int myposition = position;
    
    
    Result result_peg_390(myposition);
    
    {
    
        result_peg_390 = rule_s(stream, result_peg_390.getPosition());
            if (result_peg_390.error()){
                goto out_peg_392;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_390.getPosition()))){
                                result_peg_390.nextPosition();
                            } else {
                                goto out_peg_396;
                            }
                        }
                        result_peg_390.setValue((void*) ";");
                    
                    
                    
                    result_peg_390.reset();
                        do{
                            Result result_peg_405(result_peg_390.getPosition());
                            {
                            
                                Result result_peg_408(result_peg_405);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_408.getPosition()))){
                                            result_peg_408.nextPosition();
                                        } else {
                                            goto not_peg_407;
                                        }
                                    }
                                    result_peg_408.setValue((void*) "\n");
                                    goto loop_peg_404;
                                    not_peg_407:
                                    result_peg_405.setValue((void*)0);
                                
                                
                                
                                char temp_peg_409 = stream.get(result_peg_405.getPosition());
                                    if (temp_peg_409 != '\0'){
                                        result_peg_405.setValue((void*) temp_peg_409);
                                        result_peg_405.nextPosition();
                                    } else {
                                        goto loop_peg_404;
                                    }
                                
                                
                            }
                            result_peg_390.addResult(result_peg_405);
                        } while (true);
                        loop_peg_404:
                        ;
                    
                    
                }
            }
            goto success_peg_393;
            out_peg_396:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_390.getPosition()))){
                                result_peg_390.nextPosition();
                            } else {
                                goto out_peg_412;
                            }
                        }
                        result_peg_390.setValue((void*) "=");
                    
                    
                    
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
            goto success_peg_393;
            out_peg_412:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_390.getPosition()))){
                                result_peg_390.nextPosition();
                            } else {
                                goto out_peg_428;
                            }
                        }
                        result_peg_390.setValue((void*) "-");
                    
                    
                    
                    result_peg_390.reset();
                        do{
                            Result result_peg_433(result_peg_390.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_433.getPosition()))){
                                    result_peg_433.nextPosition();
                                } else {
                                    goto loop_peg_432;
                                }
                            }
                            result_peg_433.setValue((void*) "-");
                            result_peg_390.addResult(result_peg_433);
                        } while (true);
                        loop_peg_432:
                        if (result_peg_390.matches() == 0){
                            goto out_peg_428;
                        }
                    
                    
                }
            }
            goto success_peg_393;
            out_peg_428:
            goto out_peg_392;
            success_peg_393:
            ;
        
        
    }
    
    if (column_peg_389.chunk0 == 0){
        column_peg_389.chunk0 = new Chunk0();
    }
    column_peg_389.chunk0->chunk_line = result_peg_390;
    stream.update(result_peg_390.getPosition());
    
    
    return result_peg_390;
    out_peg_392:
    Result result_peg_434(myposition);
    
    {
    
        result_peg_434 = rule_s(stream, result_peg_434.getPosition());
            if (result_peg_434.error()){
                goto out_peg_436;
            }
        
        
        
        result_peg_434 = rule_section(stream, result_peg_434.getPosition());
            if (result_peg_434.error()){
                goto out_peg_436;
            }
        
        Result result_peg_437 = result_peg_434;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_437.getValues());
                result_peg_434.setValue(value);
            }
        
        
    }
    
    if (column_peg_389.chunk0 == 0){
        column_peg_389.chunk0 = new Chunk0();
    }
    column_peg_389.chunk0->chunk_line = result_peg_434;
    stream.update(result_peg_434.getPosition());
    
    
    return result_peg_434;
    out_peg_436:
    Result result_peg_438(myposition);
    
    result_peg_438.reset();
    do{
        Result result_peg_440(result_peg_438.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_440.getPosition()))){
                    result_peg_440.nextPosition();
                } else {
                    goto out_peg_443;
                }
            }
            result_peg_440.setValue((void*) " ");
                
        }
        goto success_peg_441;
        out_peg_443:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_440.getPosition()))){
                    result_peg_440.nextPosition();
                } else {
                    goto out_peg_445;
                }
            }
            result_peg_440.setValue((void*) "\t");
                
        }
        goto success_peg_441;
        out_peg_445:
        goto loop_peg_439;
        success_peg_441:
        ;
        result_peg_438.addResult(result_peg_440);
    } while (true);
    loop_peg_439:
    if (result_peg_438.matches() == 0){
        goto out_peg_446;
    }
    
    if (column_peg_389.chunk0 == 0){
        column_peg_389.chunk0 = new Chunk0();
    }
    column_peg_389.chunk0->chunk_line = result_peg_438;
    stream.update(result_peg_438.getPosition());
    
    
    return result_peg_438;
    out_peg_446:
    
    if (column_peg_389.chunk0 == 0){
        column_peg_389.chunk0 = new Chunk0();
    }
    column_peg_389.chunk0->chunk_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_447 = stream.getColumn(position);
    if (column_peg_447.chunk0 != 0 && column_peg_447.chunk0->chunk_line_end.calculated()){
        return column_peg_447.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_448(myposition);
    
    result_peg_448.reset();
    do{
        Result result_peg_450(result_peg_448.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_450.getPosition()))){
                    result_peg_450.nextPosition();
                } else {
                    goto out_peg_453;
                }
            }
            result_peg_450.setValue((void*) "\n");
                
        }
        goto success_peg_451;
        out_peg_453:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_450.getPosition()))){
                    result_peg_450.nextPosition();
                } else {
                    goto out_peg_455;
                }
            }
            result_peg_450.setValue((void*) "\r");
                
        }
        goto success_peg_451;
        out_peg_455:
        goto loop_peg_449;
        success_peg_451:
        ;
        result_peg_448.addResult(result_peg_450);
    } while (true);
    loop_peg_449:
    if (result_peg_448.matches() == 0){
        goto out_peg_456;
    }
    
    if (column_peg_447.chunk0 == 0){
        column_peg_447.chunk0 = new Chunk0();
    }
    column_peg_447.chunk0->chunk_line_end = result_peg_448;
    stream.update(result_peg_448.getPosition());
    
    
    return result_peg_448;
    out_peg_456:
    Result result_peg_457(myposition);
    
    {
    
        Result result_peg_459(result_peg_457.getPosition());
            if ('\0' == stream.get(result_peg_459.getPosition())){
                result_peg_459.nextPosition();
                result_peg_459.setValue((void *) '\0');
            } else {
                goto out_peg_460;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_447.chunk0 == 0){
        column_peg_447.chunk0 = new Chunk0();
    }
    column_peg_447.chunk0->chunk_line_end = result_peg_457;
    stream.update(result_peg_457.getPosition());
    
    
    return result_peg_457;
    out_peg_460:
    
    if (column_peg_447.chunk0 == 0){
        column_peg_447.chunk0 = new Chunk0();
    }
    column_peg_447.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_461 = stream.getColumn(position);
    if (column_peg_461.chunk0 != 0 && column_peg_461.chunk0->chunk_s.calculated()){
        return column_peg_461.chunk0->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_462(myposition);
    
    result_peg_462.reset();
    do{
        Result result_peg_464(result_peg_462.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_464.getPosition()))){
                    result_peg_464.nextPosition();
                } else {
                    goto out_peg_467;
                }
            }
            result_peg_464.setValue((void*) " ");
                
        }
        goto success_peg_465;
        out_peg_467:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_464.getPosition()))){
                    result_peg_464.nextPosition();
                } else {
                    goto out_peg_469;
                }
            }
            result_peg_464.setValue((void*) "\t");
                
        }
        goto success_peg_465;
        out_peg_469:
        goto loop_peg_463;
        success_peg_465:
        ;
        result_peg_462.addResult(result_peg_464);
    } while (true);
    loop_peg_463:
    ;
    
    if (column_peg_461.chunk0 == 0){
        column_peg_461.chunk0 = new Chunk0();
    }
    column_peg_461.chunk0->chunk_s = result_peg_462;
    stream.update(result_peg_462.getPosition());
    
    
    return result_peg_462;
    
    if (column_peg_461.chunk0 == 0){
        column_peg_461.chunk0 = new Chunk0();
    }
    column_peg_461.chunk0->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_470 = stream.getColumn(position);
    if (column_peg_470.chunk0 != 0 && column_peg_470.chunk0->chunk_section.calculated()){
        return column_peg_470.chunk0->chunk_section;
    }
    
    int myposition = position;
    
    Value ast;
    Result result_peg_471(myposition);
    
    {
    
        result_peg_471 = rule_section_start(stream, result_peg_471.getPosition());
            if (result_peg_471.error()){
                goto out_peg_473;
            }
        
        Result result_peg_472 = result_peg_471;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_472.getValues()); GC::save(as<Ast::Section*>(value));
                result_peg_471.setValue(value);
            }
            ast = result_peg_471.getValues();
        
        
        
        {
                
                result_peg_471.reset();
                do{
                    Result result_peg_570(result_peg_471.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_570.getPosition()))){
                                result_peg_570.nextPosition();
                            } else {
                                goto out_peg_573;
                            }
                        }
                        result_peg_570.setValue((void*) " ");
                            
                    }
                    goto success_peg_571;
                    out_peg_573:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_570.getPosition()))){
                                result_peg_570.nextPosition();
                            } else {
                                goto out_peg_575;
                            }
                        }
                        result_peg_570.setValue((void*) "\t");
                            
                    }
                    goto success_peg_571;
                    out_peg_575:
                    {
                        
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar(";"[i], stream.get(result_peg_570.getPosition()))){
                                            result_peg_570.nextPosition();
                                        } else {
                                            goto out_peg_622;
                                        }
                                    }
                                    result_peg_570.setValue((void*) ";");
                                
                                
                                
                                result_peg_570.reset();
                                    do{
                                        Result result_peg_631(result_peg_570.getPosition());
                                        {
                                        
                                            Result result_peg_634(result_peg_631);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_634.getPosition()))){
                                                        result_peg_634.nextPosition();
                                                    } else {
                                                        goto not_peg_633;
                                                    }
                                                }
                                                result_peg_634.setValue((void*) "\n");
                                                goto loop_peg_630;
                                                not_peg_633:
                                                result_peg_631.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_635 = stream.get(result_peg_631.getPosition());
                                                if (temp_peg_635 != '\0'){
                                                    result_peg_631.setValue((void*) temp_peg_635);
                                                    result_peg_631.nextPosition();
                                                } else {
                                                    goto loop_peg_630;
                                                }
                                            
                                            
                                        }
                                        result_peg_570.addResult(result_peg_631);
                                    } while (true);
                                    loop_peg_630:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_619;
                        out_peg_622:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("="[i], stream.get(result_peg_570.getPosition()))){
                                            result_peg_570.nextPosition();
                                        } else {
                                            goto out_peg_638;
                                        }
                                    }
                                    result_peg_570.setValue((void*) "=");
                                
                                
                                
                                result_peg_570.reset();
                                    do{
                                        Result result_peg_647(result_peg_570.getPosition());
                                        {
                                        
                                            Result result_peg_650(result_peg_647);
                                                for (int i = 0; i < 1; i++){
                                                    if (compareChar("\n"[i], stream.get(result_peg_650.getPosition()))){
                                                        result_peg_650.nextPosition();
                                                    } else {
                                                        goto not_peg_649;
                                                    }
                                                }
                                                result_peg_650.setValue((void*) "\n");
                                                goto loop_peg_646;
                                                not_peg_649:
                                                result_peg_647.setValue((void*)0);
                                            
                                            
                                            
                                            char temp_peg_651 = stream.get(result_peg_647.getPosition());
                                                if (temp_peg_651 != '\0'){
                                                    result_peg_647.setValue((void*) temp_peg_651);
                                                    result_peg_647.nextPosition();
                                                } else {
                                                    goto loop_peg_646;
                                                }
                                            
                                            
                                        }
                                        result_peg_570.addResult(result_peg_647);
                                    } while (true);
                                    loop_peg_646:
                                    ;
                                
                                
                            }
                        }
                        goto success_peg_619;
                        out_peg_638:
                        {
                            {
                            
                                for (int i = 0; i < 1; i++){
                                        if (compareChar("-"[i], stream.get(result_peg_570.getPosition()))){
                                            result_peg_570.nextPosition();
                                        } else {
                                            goto out_peg_654;
                                        }
                                    }
                                    result_peg_570.setValue((void*) "-");
                                
                                
                                
                                result_peg_570.reset();
                                    do{
                                        Result result_peg_659(result_peg_570.getPosition());
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_659.getPosition()))){
                                                result_peg_659.nextPosition();
                                            } else {
                                                goto loop_peg_658;
                                            }
                                        }
                                        result_peg_659.setValue((void*) "-");
                                        result_peg_570.addResult(result_peg_659);
                                    } while (true);
                                    loop_peg_658:
                                    if (result_peg_570.matches() == 0){
                                        goto out_peg_654;
                                    }
                                
                                
                            }
                        }
                        goto success_peg_619;
                        out_peg_654:
                        goto out_peg_618;
                        success_peg_619:
                        ;
                        
                    }
                    goto success_peg_571;
                    out_peg_618:
                    goto loop_peg_569;
                    success_peg_571:
                    ;
                    result_peg_471.addResult(result_peg_570);
                } while (true);
                loop_peg_569:
                ;
                        
            }
            goto success_peg_476;
            goto out_peg_473;
            success_peg_476:
            ;
        
        
        
        result_peg_471.reset();
            do{
                Result result_peg_662(result_peg_471.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_662.getPosition()))){
                            result_peg_662.nextPosition();
                        } else {
                            goto out_peg_665;
                        }
                    }
                    result_peg_662.setValue((void*) "\n");
                        
                }
                goto success_peg_663;
                out_peg_665:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_662.getPosition()))){
                            result_peg_662.nextPosition();
                        } else {
                            goto out_peg_667;
                        }
                    }
                    result_peg_662.setValue((void*) "\r");
                        
                }
                goto success_peg_663;
                out_peg_667:
                goto loop_peg_661;
                success_peg_663:
                ;
                result_peg_471.addResult(result_peg_662);
            } while (true);
            loop_peg_661:
            if (result_peg_471.matches() == 0){
                goto out_peg_473;
            }
        
        
        
        result_peg_471.reset();
            do{
                Result result_peg_670(result_peg_471.getPosition());
                {
                
                    result_peg_670 = rule_section_line(stream, result_peg_670.getPosition(), ast);
                        if (result_peg_670.error()){
                            goto loop_peg_669;
                        }
                    
                    
                    
                    {
                            
                            result_peg_670.reset();
                            do{
                                Result result_peg_767(result_peg_670.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_767.getPosition()))){
                                            result_peg_767.nextPosition();
                                        } else {
                                            goto out_peg_770;
                                        }
                                    }
                                    result_peg_767.setValue((void*) " ");
                                        
                                }
                                goto success_peg_768;
                                out_peg_770:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_767.getPosition()))){
                                            result_peg_767.nextPosition();
                                        } else {
                                            goto out_peg_772;
                                        }
                                    }
                                    result_peg_767.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_768;
                                out_peg_772:
                                {
                                    
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar(";"[i], stream.get(result_peg_767.getPosition()))){
                                                        result_peg_767.nextPosition();
                                                    } else {
                                                        goto out_peg_819;
                                                    }
                                                }
                                                result_peg_767.setValue((void*) ";");
                                            
                                            
                                            
                                            result_peg_767.reset();
                                                do{
                                                    Result result_peg_828(result_peg_767.getPosition());
                                                    {
                                                    
                                                        Result result_peg_831(result_peg_828);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_831.getPosition()))){
                                                                    result_peg_831.nextPosition();
                                                                } else {
                                                                    goto not_peg_830;
                                                                }
                                                            }
                                                            result_peg_831.setValue((void*) "\n");
                                                            goto loop_peg_827;
                                                            not_peg_830:
                                                            result_peg_828.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_832 = stream.get(result_peg_828.getPosition());
                                                            if (temp_peg_832 != '\0'){
                                                                result_peg_828.setValue((void*) temp_peg_832);
                                                                result_peg_828.nextPosition();
                                                            } else {
                                                                goto loop_peg_827;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_767.addResult(result_peg_828);
                                                } while (true);
                                                loop_peg_827:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_816;
                                    out_peg_819:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("="[i], stream.get(result_peg_767.getPosition()))){
                                                        result_peg_767.nextPosition();
                                                    } else {
                                                        goto out_peg_835;
                                                    }
                                                }
                                                result_peg_767.setValue((void*) "=");
                                            
                                            
                                            
                                            result_peg_767.reset();
                                                do{
                                                    Result result_peg_844(result_peg_767.getPosition());
                                                    {
                                                    
                                                        Result result_peg_847(result_peg_844);
                                                            for (int i = 0; i < 1; i++){
                                                                if (compareChar("\n"[i], stream.get(result_peg_847.getPosition()))){
                                                                    result_peg_847.nextPosition();
                                                                } else {
                                                                    goto not_peg_846;
                                                                }
                                                            }
                                                            result_peg_847.setValue((void*) "\n");
                                                            goto loop_peg_843;
                                                            not_peg_846:
                                                            result_peg_844.setValue((void*)0);
                                                        
                                                        
                                                        
                                                        char temp_peg_848 = stream.get(result_peg_844.getPosition());
                                                            if (temp_peg_848 != '\0'){
                                                                result_peg_844.setValue((void*) temp_peg_848);
                                                                result_peg_844.nextPosition();
                                                            } else {
                                                                goto loop_peg_843;
                                                            }
                                                        
                                                        
                                                    }
                                                    result_peg_767.addResult(result_peg_844);
                                                } while (true);
                                                loop_peg_843:
                                                ;
                                            
                                            
                                        }
                                    }
                                    goto success_peg_816;
                                    out_peg_835:
                                    {
                                        {
                                        
                                            for (int i = 0; i < 1; i++){
                                                    if (compareChar("-"[i], stream.get(result_peg_767.getPosition()))){
                                                        result_peg_767.nextPosition();
                                                    } else {
                                                        goto out_peg_851;
                                                    }
                                                }
                                                result_peg_767.setValue((void*) "-");
                                            
                                            
                                            
                                            result_peg_767.reset();
                                                do{
                                                    Result result_peg_856(result_peg_767.getPosition());
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_856.getPosition()))){
                                                            result_peg_856.nextPosition();
                                                        } else {
                                                            goto loop_peg_855;
                                                        }
                                                    }
                                                    result_peg_856.setValue((void*) "-");
                                                    result_peg_767.addResult(result_peg_856);
                                                } while (true);
                                                loop_peg_855:
                                                if (result_peg_767.matches() == 0){
                                                    goto out_peg_851;
                                                }
                                            
                                            
                                        }
                                    }
                                    goto success_peg_816;
                                    out_peg_851:
                                    goto out_peg_815;
                                    success_peg_816:
                                    ;
                                    
                                }
                                goto success_peg_768;
                                out_peg_815:
                                goto loop_peg_766;
                                success_peg_768:
                                ;
                                result_peg_670.addResult(result_peg_767);
                            } while (true);
                            loop_peg_766:
                            ;
                                    
                        }
                        goto success_peg_673;
                        goto loop_peg_669;
                        success_peg_673:
                        ;
                    
                    
                    
                    result_peg_670 = rule_line_end(stream, result_peg_670.getPosition());
                        if (result_peg_670.error()){
                            goto loop_peg_669;
                        }
                    
                    
                }
                result_peg_471.addResult(result_peg_670);
            } while (true);
            loop_peg_669:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_471.setValue(value);
            }
        
        
    }
    
    if (column_peg_470.chunk0 == 0){
        column_peg_470.chunk0 = new Chunk0();
    }
    column_peg_470.chunk0->chunk_section = result_peg_471;
    stream.update(result_peg_471.getPosition());
    
    
    return result_peg_471;
    out_peg_473:
    
    if (column_peg_470.chunk0 == 0){
        column_peg_470.chunk0 = new Chunk0();
    }
    column_peg_470.chunk0->chunk_section = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    Column & column_peg_857 = stream.getColumn(position);
    if (column_peg_857.chunk1 != 0 && column_peg_857.chunk1->chunk_section_line.calculated()){
        return column_peg_857.chunk1->chunk_section_line;
    }
    
    int myposition = position;
    
    Value data;
    Result result_peg_858(myposition);
    
    {
    
        result_peg_858 = rule_s(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar(";"[i], stream.get(result_peg_858.getPosition()))){
                                result_peg_858.nextPosition();
                            } else {
                                goto out_peg_864;
                            }
                        }
                        result_peg_858.setValue((void*) ";");
                    
                    
                    
                    result_peg_858.reset();
                        do{
                            Result result_peg_873(result_peg_858.getPosition());
                            {
                            
                                Result result_peg_876(result_peg_873);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_876.getPosition()))){
                                            result_peg_876.nextPosition();
                                        } else {
                                            goto not_peg_875;
                                        }
                                    }
                                    result_peg_876.setValue((void*) "\n");
                                    goto loop_peg_872;
                                    not_peg_875:
                                    result_peg_873.setValue((void*)0);
                                
                                
                                
                                char temp_peg_877 = stream.get(result_peg_873.getPosition());
                                    if (temp_peg_877 != '\0'){
                                        result_peg_873.setValue((void*) temp_peg_877);
                                        result_peg_873.nextPosition();
                                    } else {
                                        goto loop_peg_872;
                                    }
                                
                                
                            }
                            result_peg_858.addResult(result_peg_873);
                        } while (true);
                        loop_peg_872:
                        ;
                    
                    
                }
            }
            goto success_peg_861;
            out_peg_864:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("="[i], stream.get(result_peg_858.getPosition()))){
                                result_peg_858.nextPosition();
                            } else {
                                goto out_peg_880;
                            }
                        }
                        result_peg_858.setValue((void*) "=");
                    
                    
                    
                    result_peg_858.reset();
                        do{
                            Result result_peg_889(result_peg_858.getPosition());
                            {
                            
                                Result result_peg_892(result_peg_889);
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_892.getPosition()))){
                                            result_peg_892.nextPosition();
                                        } else {
                                            goto not_peg_891;
                                        }
                                    }
                                    result_peg_892.setValue((void*) "\n");
                                    goto loop_peg_888;
                                    not_peg_891:
                                    result_peg_889.setValue((void*)0);
                                
                                
                                
                                char temp_peg_893 = stream.get(result_peg_889.getPosition());
                                    if (temp_peg_893 != '\0'){
                                        result_peg_889.setValue((void*) temp_peg_893);
                                        result_peg_889.nextPosition();
                                    } else {
                                        goto loop_peg_888;
                                    }
                                
                                
                            }
                            result_peg_858.addResult(result_peg_889);
                        } while (true);
                        loop_peg_888:
                        ;
                    
                    
                }
            }
            goto success_peg_861;
            out_peg_880:
            {
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_858.getPosition()))){
                                result_peg_858.nextPosition();
                            } else {
                                goto out_peg_896;
                            }
                        }
                        result_peg_858.setValue((void*) "-");
                    
                    
                    
                    result_peg_858.reset();
                        do{
                            Result result_peg_901(result_peg_858.getPosition());
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_901.getPosition()))){
                                    result_peg_901.nextPosition();
                                } else {
                                    goto loop_peg_900;
                                }
                            }
                            result_peg_901.setValue((void*) "-");
                            result_peg_858.addResult(result_peg_901);
                        } while (true);
                        loop_peg_900:
                        if (result_peg_858.matches() == 0){
                            goto out_peg_896;
                        }
                    
                    
                }
            }
            goto success_peg_861;
            out_peg_896:
            goto out_peg_860;
            success_peg_861:
            ;
        
        
    }
    
    if (column_peg_857.chunk1 == 0){
        column_peg_857.chunk1 = new Chunk1();
    }
    column_peg_857.chunk1->chunk_section_line = result_peg_858;
    stream.update(result_peg_858.getPosition());
    
    
    return result_peg_858;
    out_peg_860:
    Result result_peg_902(myposition);
    
    {
    
        result_peg_902 = rule_s(stream, result_peg_902.getPosition());
            if (result_peg_902.error()){
                goto out_peg_904;
            }
        
        
        
        result_peg_902 = rule_attribute(stream, result_peg_902.getPosition());
            if (result_peg_902.error()){
                goto out_peg_904;
            }
            data = result_peg_902.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addAttribute(asAttribute(data));
                result_peg_902.setValue(value);
            }
        
        
    }
    
    if (column_peg_857.chunk1 == 0){
        column_peg_857.chunk1 = new Chunk1();
    }
    column_peg_857.chunk1->chunk_section_line = result_peg_902;
    stream.update(result_peg_902.getPosition());
    
    
    return result_peg_902;
    out_peg_904:
    Result result_peg_906(myposition);
    
    {
    
        result_peg_906 = rule_s(stream, result_peg_906.getPosition());
            if (result_peg_906.error()){
                goto out_peg_908;
            }
        
        
        
        result_peg_906 = rule_valuelist(stream, result_peg_906.getPosition());
            if (result_peg_906.error()){
                goto out_peg_908;
            }
            data = result_peg_906.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_906.setValue(value);
            }
        
        
    }
    
    if (column_peg_857.chunk1 == 0){
        column_peg_857.chunk1 = new Chunk1();
    }
    column_peg_857.chunk1->chunk_section_line = result_peg_906;
    stream.update(result_peg_906.getPosition());
    
    
    return result_peg_906;
    out_peg_908:
    Result result_peg_910(myposition);
    
    {
    
        result_peg_910 = rule_s(stream, result_peg_910.getPosition());
            if (result_peg_910.error()){
                goto out_peg_912;
            }
        
        
        
        result_peg_910 = rule_loopstart(stream, result_peg_910.getPosition());
            if (result_peg_910.error()){
                goto out_peg_912;
            }
            data = result_peg_910.getValues();
        
        
        
        {
                Value value((void*) 0);
                asSection(section)->addValue(asValue(data));
                result_peg_910.setValue(value);
            }
        
        
    }
    
    if (column_peg_857.chunk1 == 0){
        column_peg_857.chunk1 = new Chunk1();
    }
    column_peg_857.chunk1->chunk_section_line = result_peg_910;
    stream.update(result_peg_910.getPosition());
    
    
    return result_peg_910;
    out_peg_912:
    Result result_peg_914(myposition);
    
    {
    
        result_peg_914.reset();
            do{
                Result result_peg_917(result_peg_914.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_917.getPosition()))){
                            result_peg_917.nextPosition();
                        } else {
                            goto out_peg_920;
                        }
                    }
                    result_peg_917.setValue((void*) " ");
                        
                }
                goto success_peg_918;
                out_peg_920:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_917.getPosition()))){
                            result_peg_917.nextPosition();
                        } else {
                            goto out_peg_922;
                        }
                    }
                    result_peg_917.setValue((void*) "\t");
                        
                }
                goto success_peg_918;
                out_peg_922:
                goto loop_peg_916;
                success_peg_918:
                ;
                result_peg_914.addResult(result_peg_917);
            } while (true);
            loop_peg_916:
            if (result_peg_914.matches() == 0){
                goto out_peg_923;
            }
        
        
        
        Result result_peg_925(result_peg_914);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_925.getPosition()))){
                    result_peg_925.nextPosition();
                } else {
                    goto not_peg_924;
                }
            }
            result_peg_925.setValue((void*) "[");
            goto out_peg_923;
            not_peg_924:
            result_peg_914.setValue((void*)0);
        
        
    }
    
    if (column_peg_857.chunk1 == 0){
        column_peg_857.chunk1 = new Chunk1();
    }
    column_peg_857.chunk1->chunk_section_line = result_peg_914;
    stream.update(result_peg_914.getPosition());
    
    
    return result_peg_914;
    out_peg_923:
    
    if (column_peg_857.chunk1 == 0){
        column_peg_857.chunk1 = new Chunk1();
    }
    column_peg_857.chunk1->chunk_section_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    Column & column_peg_926 = stream.getColumn(position);
    if (column_peg_926.chunk1 != 0 && column_peg_926.chunk1->chunk_section_start.calculated()){
        return column_peg_926.chunk1->chunk_section_start;
    }
    
    int myposition = position;
    
    Value data;
    Result result_peg_927(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_927.getPosition()))){
                    result_peg_927.nextPosition();
                } else {
                    goto out_peg_929;
                }
            }
            result_peg_927.setValue((void*) "[");
        
        
        
        result_peg_927 = rule_s(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto out_peg_929;
            }
        
        
        
        result_peg_927.reset();
            do{
                Result result_peg_933(result_peg_927.getPosition());
                {
                
                    Result result_peg_936(result_peg_933);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_936.getPosition()))){
                                result_peg_936.nextPosition();
                            } else {
                                goto not_peg_935;
                            }
                        }
                        result_peg_936.setValue((void*) "]");
                        goto loop_peg_932;
                        not_peg_935:
                        result_peg_933.setValue((void*)0);
                    
                    
                    
                    char temp_peg_937 = stream.get(result_peg_933.getPosition());
                        if (temp_peg_937 != '\0'){
                            result_peg_933.setValue((void*) temp_peg_937);
                            result_peg_933.nextPosition();
                        } else {
                            goto loop_peg_932;
                        }
                    
                    
                }
                result_peg_927.addResult(result_peg_933);
            } while (true);
            loop_peg_932:
            if (result_peg_927.matches() == 0){
                goto out_peg_929;
            }
            data = result_peg_927.getValues();
        
        
        
        result_peg_927 = rule_s(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto out_peg_929;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_927.getPosition()))){
                    result_peg_927.nextPosition();
                } else {
                    goto out_peg_929;
                }
            }
            result_peg_927.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(data); GC::save(as<std::string*>(value));
                result_peg_927.setValue(value);
            }
        
        
    }
    
    if (column_peg_926.chunk1 == 0){
        column_peg_926.chunk1 = new Chunk1();
    }
    column_peg_926.chunk1->chunk_section_start = result_peg_927;
    stream.update(result_peg_927.getPosition());
    
    
    return result_peg_927;
    out_peg_929:
    
    if (column_peg_926.chunk1 == 0){
        column_peg_926.chunk1 = new Chunk1();
    }
    column_peg_926.chunk1->chunk_section_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_940 = stream.getColumn(position);
    if (column_peg_940.chunk1 != 0 && column_peg_940.chunk1->chunk_loopstart.calculated()){
        return column_peg_940.chunk1->chunk_loopstart;
    }
    
    int myposition = position;
    
    
    Result result_peg_941(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_941.getPosition()))){
                    result_peg_941.nextPosition();
                } else {
                    goto out_peg_943;
                }
            }
            result_peg_941.setValue((void*) "loopstart");
        
        
        
        {
                Value value((void*) 0);
                value = makeValue(); GC::save(as<Ast::Value*>(value));
                result_peg_941.setValue(value);
            }
        
        
    }
    
    if (column_peg_940.chunk1 == 0){
        column_peg_940.chunk1 = new Chunk1();
    }
    column_peg_940.chunk1->chunk_loopstart = result_peg_941;
    stream.update(result_peg_941.getPosition());
    
    
    return result_peg_941;
    out_peg_943:
    
    if (column_peg_940.chunk1 == 0){
        column_peg_940.chunk1 = new Chunk1();
    }
    column_peg_940.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_944 = stream.getColumn(position);
    if (column_peg_944.chunk1 != 0 && column_peg_944.chunk1->chunk_name.calculated()){
        return column_peg_944.chunk1->chunk_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_945(myposition);
    
    {
    
        {
                
                char letter_peg_951 = stream.get(result_peg_945.getPosition());
                if (letter_peg_951 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_951) != NULL){
                    result_peg_945.nextPosition();
                    result_peg_945.setValue((void*) letter_peg_951);
                } else {
                    goto out_peg_950;
                }
                
            }
            goto success_peg_947;
            out_peg_950:
            goto out_peg_952;
            success_peg_947:
            ;
        
        Result result_peg_946 = result_peg_945;
        
        result_peg_945.reset();
            do{
                Result result_peg_955(result_peg_945.getPosition());
                result_peg_955 = rule_alpha_digit(stream, result_peg_955.getPosition());
                if (result_peg_955.error()){
                    goto loop_peg_954;
                }
                result_peg_945.addResult(result_peg_955);
            } while (true);
            loop_peg_954:
            ;
        
        Result result_peg_953 = result_peg_945;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_946.getValues().getValue(),result_peg_953.getValues()); GC::save(as<std::string*>(value));
                result_peg_945.setValue(value);
            }
        
        
    }
    
    if (column_peg_944.chunk1 == 0){
        column_peg_944.chunk1 = new Chunk1();
    }
    column_peg_944.chunk1->chunk_name = result_peg_945;
    stream.update(result_peg_945.getPosition());
    
    
    return result_peg_945;
    out_peg_952:
    
    if (column_peg_944.chunk1 == 0){
        column_peg_944.chunk1 = new Chunk1();
    }
    column_peg_944.chunk1->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_956 = stream.getColumn(position);
    if (column_peg_956.chunk1 != 0 && column_peg_956.chunk1->chunk_alpha_digit.calculated()){
        return column_peg_956.chunk1->chunk_alpha_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_957(myposition);
    
    {
        
        char letter_peg_962 = stream.get(result_peg_957.getPosition());
        if (letter_peg_962 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_962) != NULL){
            result_peg_957.nextPosition();
            result_peg_957.setValue((void*) letter_peg_962);
        } else {
            goto out_peg_961;
        }
        
    }
    goto success_peg_958;
    out_peg_961:
    goto out_peg_963;
    success_peg_958:
    ;
    
    if (column_peg_956.chunk1 == 0){
        column_peg_956.chunk1 = new Chunk1();
    }
    column_peg_956.chunk1->chunk_alpha_digit = result_peg_957;
    stream.update(result_peg_957.getPosition());
    
    
    return result_peg_957;
    out_peg_963:
    Result result_peg_964(myposition);
    
    {
        
        char letter_peg_969 = stream.get(result_peg_964.getPosition());
        if (letter_peg_969 != '\0' && strchr("0123456789", letter_peg_969) != NULL){
            result_peg_964.nextPosition();
            result_peg_964.setValue((void*) letter_peg_969);
        } else {
            goto out_peg_968;
        }
        
    }
    goto success_peg_965;
    out_peg_968:
    goto out_peg_970;
    success_peg_965:
    ;
    
    if (column_peg_956.chunk1 == 0){
        column_peg_956.chunk1 = new Chunk1();
    }
    column_peg_956.chunk1->chunk_alpha_digit = result_peg_964;
    stream.update(result_peg_964.getPosition());
    
    
    return result_peg_964;
    out_peg_970:
    
    if (column_peg_956.chunk1 == 0){
        column_peg_956.chunk1 = new Chunk1();
    }
    column_peg_956.chunk1->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    Column & column_peg_971 = stream.getColumn(position);
    if (column_peg_971.chunk2 != 0 && column_peg_971.chunk2->chunk_line_end_or_comment.calculated()){
        return column_peg_971.chunk2->chunk_line_end_or_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_972(myposition);
    
    result_peg_972 = rule_line_end(stream, result_peg_972.getPosition());
    if (result_peg_972.error()){
        goto out_peg_973;
    }
    
    if (column_peg_971.chunk2 == 0){
        column_peg_971.chunk2 = new Chunk2();
    }
    column_peg_971.chunk2->chunk_line_end_or_comment = result_peg_972;
    stream.update(result_peg_972.getPosition());
    
    
    return result_peg_972;
    out_peg_973:
    Result result_peg_974(myposition);
    
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_974.getPosition()))){
                        result_peg_974.nextPosition();
                    } else {
                        goto out_peg_978;
                    }
                }
                result_peg_974.setValue((void*) ";");
            
            
            
            result_peg_974.reset();
                do{
                    Result result_peg_987(result_peg_974.getPosition());
                    {
                    
                        Result result_peg_990(result_peg_987);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_990.getPosition()))){
                                    result_peg_990.nextPosition();
                                } else {
                                    goto not_peg_989;
                                }
                            }
                            result_peg_990.setValue((void*) "\n");
                            goto loop_peg_986;
                            not_peg_989:
                            result_peg_987.setValue((void*)0);
                        
                        
                        
                        char temp_peg_991 = stream.get(result_peg_987.getPosition());
                            if (temp_peg_991 != '\0'){
                                result_peg_987.setValue((void*) temp_peg_991);
                                result_peg_987.nextPosition();
                            } else {
                                goto loop_peg_986;
                            }
                        
                        
                    }
                    result_peg_974.addResult(result_peg_987);
                } while (true);
                loop_peg_986:
                ;
            
            
        }
    }
    goto success_peg_975;
    out_peg_978:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_974.getPosition()))){
                        result_peg_974.nextPosition();
                    } else {
                        goto out_peg_994;
                    }
                }
                result_peg_974.setValue((void*) "=");
            
            
            
            result_peg_974.reset();
                do{
                    Result result_peg_1003(result_peg_974.getPosition());
                    {
                    
                        Result result_peg_1006(result_peg_1003);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_1006.getPosition()))){
                                    result_peg_1006.nextPosition();
                                } else {
                                    goto not_peg_1005;
                                }
                            }
                            result_peg_1006.setValue((void*) "\n");
                            goto loop_peg_1002;
                            not_peg_1005:
                            result_peg_1003.setValue((void*)0);
                        
                        
                        
                        char temp_peg_1007 = stream.get(result_peg_1003.getPosition());
                            if (temp_peg_1007 != '\0'){
                                result_peg_1003.setValue((void*) temp_peg_1007);
                                result_peg_1003.nextPosition();
                            } else {
                                goto loop_peg_1002;
                            }
                        
                        
                    }
                    result_peg_974.addResult(result_peg_1003);
                } while (true);
                loop_peg_1002:
                ;
            
            
        }
    }
    goto success_peg_975;
    out_peg_994:
    {
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_974.getPosition()))){
                        result_peg_974.nextPosition();
                    } else {
                        goto out_peg_1010;
                    }
                }
                result_peg_974.setValue((void*) "-");
            
            
            
            result_peg_974.reset();
                do{
                    Result result_peg_1015(result_peg_974.getPosition());
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_1015.getPosition()))){
                            result_peg_1015.nextPosition();
                        } else {
                            goto loop_peg_1014;
                        }
                    }
                    result_peg_1015.setValue((void*) "-");
                    result_peg_974.addResult(result_peg_1015);
                } while (true);
                loop_peg_1014:
                if (result_peg_974.matches() == 0){
                    goto out_peg_1010;
                }
            
            
        }
    }
    goto success_peg_975;
    out_peg_1010:
    goto out_peg_1016;
    success_peg_975:
    ;
    
    if (column_peg_971.chunk2 == 0){
        column_peg_971.chunk2 = new Chunk2();
    }
    column_peg_971.chunk2->chunk_line_end_or_comment = result_peg_974;
    stream.update(result_peg_974.getPosition());
    
    
    return result_peg_974;
    out_peg_1016:
    
    if (column_peg_971.chunk2 == 0){
        column_peg_971.chunk2 = new Chunk2();
    }
    column_peg_971.chunk2->chunk_line_end_or_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    Column & column_peg_1017 = stream.getColumn(position);
    if (column_peg_1017.chunk2 != 0 && column_peg_1017.chunk2->chunk_filename.calculated()){
        return column_peg_1017.chunk2->chunk_filename;
    }
    
    int myposition = position;
    
    Value file;
    Result result_peg_1018(myposition);
    
    {
    
        Result result_peg_1021(result_peg_1018);
            for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1021.getPosition()))){
                    result_peg_1021.nextPosition();
                } else {
                    goto not_peg_1020;
                }
            }
            result_peg_1021.setValue((void*) "\"");
            goto out_peg_1022;
            not_peg_1020:
            result_peg_1018.setValue((void*)0);
        
        
        
        result_peg_1018.reset();
            do{
                Result result_peg_1025(result_peg_1018.getPosition());
                result_peg_1025 = rule_filename_char(stream, result_peg_1025.getPosition());
                if (result_peg_1025.error()){
                    goto loop_peg_1024;
                }
                result_peg_1018.addResult(result_peg_1025);
            } while (true);
            loop_peg_1024:
            if (result_peg_1018.matches() == 0){
                goto out_peg_1022;
            }
            file = result_peg_1018.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new Ast::Filename(toString(file)); GC::save(as<Ast::Filename*>(value));
                result_peg_1018.setValue(value);
            }
        
        
    }
    
    if (column_peg_1017.chunk2 == 0){
        column_peg_1017.chunk2 = new Chunk2();
    }
    column_peg_1017.chunk2->chunk_filename = result_peg_1018;
    stream.update(result_peg_1018.getPosition());
    
    
    return result_peg_1018;
    out_peg_1022:
    
    if (column_peg_1017.chunk2 == 0){
        column_peg_1017.chunk2 = new Chunk2();
    }
    column_peg_1017.chunk2->chunk_filename = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_filename_char(Stream & stream, const int position){
    
    Column & column_peg_1026 = stream.getColumn(position);
    if (column_peg_1026.chunk2 != 0 && column_peg_1026.chunk2->chunk_filename_char.calculated()){
        return column_peg_1026.chunk2->chunk_filename_char;
    }
    
    int myposition = position;
    
    
    Result result_peg_1027(myposition);
    
    {
    
        Result result_peg_1030(result_peg_1027);
            for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1030.getPosition()))){
                    result_peg_1030.nextPosition();
                } else {
                    goto not_peg_1029;
                }
            }
            result_peg_1030.setValue((void*) ",");
            goto out_peg_1031;
            not_peg_1029:
            result_peg_1027.setValue((void*)0);
        
        
        
        Result result_peg_1034(result_peg_1027);
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_1034.getPosition()))){
                    result_peg_1034.nextPosition();
                } else {
                    goto not_peg_1033;
                }
            }
            result_peg_1034.setValue((void*) "\n");
            goto out_peg_1031;
            not_peg_1033:
            result_peg_1027.setValue((void*)0);
        
        
        
        Result result_peg_1037(result_peg_1027);
            for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_1037.getPosition()))){
                    result_peg_1037.nextPosition();
                } else {
                    goto not_peg_1036;
                }
            }
            result_peg_1037.setValue((void*) "[");
            goto out_peg_1031;
            not_peg_1036:
            result_peg_1027.setValue((void*)0);
        
        
        
        Result result_peg_1040(result_peg_1027);
            if ((unsigned char) stream.get(result_peg_1040.getPosition()) == (unsigned char) 13){
                result_peg_1040.nextPosition();
            } else {
                goto not_peg_1039;
            }
            result_peg_1040.setValue((void*) 13);
            goto out_peg_1031;
            not_peg_1039:
            result_peg_1027.setValue((void*)0);
        
        
        
        Result result_peg_1043(result_peg_1027);
            for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_1043.getPosition()))){
                    result_peg_1043.nextPosition();
                } else {
                    goto not_peg_1042;
                }
            }
            result_peg_1043.setValue((void*) ";");
            goto out_peg_1031;
            not_peg_1042:
            result_peg_1027.setValue((void*)0);
        
        
        
        char temp_peg_1044 = stream.get(result_peg_1027.getPosition());
            if (temp_peg_1044 != '\0'){
                result_peg_1027.setValue((void*) temp_peg_1044);
                result_peg_1027.nextPosition();
            } else {
                goto out_peg_1031;
            }
        
        
    }
    
    if (column_peg_1026.chunk2 == 0){
        column_peg_1026.chunk2 = new Chunk2();
    }
    column_peg_1026.chunk2->chunk_filename_char = result_peg_1027;
    stream.update(result_peg_1027.getPosition());
    
    
    return result_peg_1027;
    out_peg_1031:
    
    if (column_peg_1026.chunk2 == 0){
        column_peg_1026.chunk2 = new Chunk2();
    }
    column_peg_1026.chunk2->chunk_filename_char = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    Column & column_peg_1045 = stream.getColumn(position);
    if (column_peg_1045.chunk2 != 0 && column_peg_1045.chunk2->chunk_attribute.calculated()){
        return column_peg_1045.chunk2->chunk_attribute;
    }
    
    int myposition = position;
    
    Value id;
    Value data;
    Value index;
    Result result_peg_1046(myposition);
    
    {
    
        result_peg_1046 = rule_identifier(stream, result_peg_1046.getPosition());
            if (result_peg_1046.error()){
                goto out_peg_1048;
            }
            id = result_peg_1046.getValues();
        
        
        
        result_peg_1046 = rule_s(stream, result_peg_1046.getPosition());
            if (result_peg_1046.error()){
                goto out_peg_1048;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1046.getPosition()))){
                    result_peg_1046.nextPosition();
                } else {
                    goto out_peg_1048;
                }
            }
            result_peg_1046.setValue((void*) "=");
        
        
        
        result_peg_1046 = rule_s(stream, result_peg_1046.getPosition());
            if (result_peg_1046.error()){
                goto out_peg_1048;
            }
        
        
        
        Result result_peg_1053(result_peg_1046.getPosition());
            result_peg_1053 = rule_line_end_or_comment(stream, result_peg_1053.getPosition());
            if (result_peg_1053.error()){
                goto out_peg_1048;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(id); GC::save(as<Ast::Attribute*>(value));
                result_peg_1046.setValue(value);
            }
        
        
    }
    
    if (column_peg_1045.chunk2 == 0){
        column_peg_1045.chunk2 = new Chunk2();
    }
    column_peg_1045.chunk2->chunk_attribute = result_peg_1046;
    stream.update(result_peg_1046.getPosition());
    
    
    return result_peg_1046;
    out_peg_1048:
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
                value = makeAttribute(id, data); GC::save(as<Ast::Attribute*>(value));
                result_peg_1054.setValue(value);
            }
        
        
    }
    
    if (column_peg_1045.chunk2 == 0){
        column_peg_1045.chunk2 = new Chunk2();
    }
    column_peg_1045.chunk2->chunk_attribute = result_peg_1054;
    stream.update(result_peg_1054.getPosition());
    
    
    return result_peg_1054;
    out_peg_1056:
    Result result_peg_1061(myposition);
    
    {
    
        result_peg_1061 = rule_identifier(stream, result_peg_1061.getPosition());
            if (result_peg_1061.error()){
                goto out_peg_1063;
            }
            id = result_peg_1061.getValues();
        
        
        
        result_peg_1061 = rule_s(stream, result_peg_1061.getPosition());
            if (result_peg_1061.error()){
                goto out_peg_1063;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1061.getPosition()))){
                    result_peg_1061.nextPosition();
                } else {
                    goto out_peg_1063;
                }
            }
            result_peg_1061.setValue((void*) "=");
        
        
        
        result_peg_1061 = rule_s(stream, result_peg_1061.getPosition());
            if (result_peg_1061.error()){
                goto out_peg_1063;
            }
        
        
        
        result_peg_1061 = rule_filename(stream, result_peg_1061.getPosition());
            if (result_peg_1061.error()){
                goto out_peg_1063;
            }
            data = result_peg_1061.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeFilename(id, data); GC::save(as<Ast::Attribute*>(value));
                result_peg_1061.setValue(value);
            }
        
        
    }
    
    if (column_peg_1045.chunk2 == 0){
        column_peg_1045.chunk2 = new Chunk2();
    }
    column_peg_1045.chunk2->chunk_attribute = result_peg_1061;
    stream.update(result_peg_1061.getPosition());
    
    
    return result_peg_1061;
    out_peg_1063:
    Result result_peg_1068(myposition);
    
    {
    
        result_peg_1068 = rule_identifier(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
            id = result_peg_1068.getValues();
        
        
        
        result_peg_1068 = rule_s(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1068.getPosition()))){
                    result_peg_1068.nextPosition();
                } else {
                    goto out_peg_1070;
                }
            }
            result_peg_1068.setValue((void*) "(");
        
        
        
        result_peg_1068 = rule_s(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
        
        
        
        result_peg_1068 = rule_number(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
            index = result_peg_1068.getValues();
        
        
        
        result_peg_1068 = rule_s(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1068.getPosition()))){
                    result_peg_1068.nextPosition();
                } else {
                    goto out_peg_1070;
                }
            }
            result_peg_1068.setValue((void*) ")");
        
        
        
        result_peg_1068 = rule_s(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1068.getPosition()))){
                    result_peg_1068.nextPosition();
                } else {
                    goto out_peg_1070;
                }
            }
            result_peg_1068.setValue((void*) "=");
        
        
        
        result_peg_1068 = rule_s(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
        
        
        
        result_peg_1068 = rule_valuelist(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1070;
            }
            data = result_peg_1068.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(id, index, data); GC::save(as<Ast::Value*>(value));
                result_peg_1068.setValue(value);
            }
        
        
    }
    
    if (column_peg_1045.chunk2 == 0){
        column_peg_1045.chunk2 = new Chunk2();
    }
    column_peg_1045.chunk2->chunk_attribute = result_peg_1068;
    stream.update(result_peg_1068.getPosition());
    
    
    return result_peg_1068;
    out_peg_1070:
    Result result_peg_1081(myposition);
    
    {
    
        result_peg_1081 = rule_identifier_list(stream, result_peg_1081.getPosition());
            if (result_peg_1081.error()){
                goto out_peg_1083;
            }
            id = result_peg_1081.getValues();
        
        
        
        result_peg_1081 = rule_s(stream, result_peg_1081.getPosition());
            if (result_peg_1081.error()){
                goto out_peg_1083;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1081.getPosition()))){
                    result_peg_1081.nextPosition();
                } else {
                    goto out_peg_1083;
                }
            }
            result_peg_1081.setValue((void*) "=");
        
        
        
        result_peg_1081 = rule_s(stream, result_peg_1081.getPosition());
            if (result_peg_1081.error()){
                goto out_peg_1083;
            }
        
        
        
        result_peg_1081 = rule_valuelist(stream, result_peg_1081.getPosition());
            if (result_peg_1081.error()){
                goto out_peg_1083;
            }
            data = result_peg_1081.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributes(id, data); GC::save(as<Ast::Attribute*>(value));
                result_peg_1081.setValue(value);
            }
        
        
    }
    
    if (column_peg_1045.chunk2 == 0){
        column_peg_1045.chunk2 = new Chunk2();
    }
    column_peg_1045.chunk2->chunk_attribute = result_peg_1081;
    stream.update(result_peg_1081.getPosition());
    
    
    return result_peg_1081;
    out_peg_1083:
    
    if (column_peg_1045.chunk2 == 0){
        column_peg_1045.chunk2 = new Chunk2();
    }
    column_peg_1045.chunk2->chunk_attribute = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_1088 = stream.getColumn(position);
    if (column_peg_1088.chunk2 != 0 && column_peg_1088.chunk2->chunk_identifier.calculated()){
        return column_peg_1088.chunk2->chunk_identifier;
    }
    
    int myposition = position;
    
    
    Result result_peg_1089(myposition);
    
    {
    
        result_peg_1089 = rule_name(stream, result_peg_1089.getPosition());
            if (result_peg_1089.error()){
                goto out_peg_1091;
            }
        
        Result result_peg_1090 = result_peg_1089;
        
        result_peg_1089.reset();
            do{
                Result result_peg_1094(result_peg_1089.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_1094.getPosition()))){
                                result_peg_1094.nextPosition();
                            } else {
                                goto loop_peg_1093;
                            }
                        }
                        result_peg_1094.setValue((void*) ".");
                    
                    
                    
                    result_peg_1094 = rule_name(stream, result_peg_1094.getPosition());
                        if (result_peg_1094.error()){
                            goto loop_peg_1093;
                        }
                    
                    
                }
                result_peg_1089.addResult(result_peg_1094);
            } while (true);
            loop_peg_1093:
            ;
        
        Result result_peg_1092 = result_peg_1089;
        
        {
                Value value((void*) 0);
                value = makeIdentifier(result_peg_1090.getValues(),result_peg_1092.getValues()); GC::save(as<Ast::Identifier*>(value));
                result_peg_1089.setValue(value);
            }
        
        
    }
    
    if (column_peg_1088.chunk2 == 0){
        column_peg_1088.chunk2 = new Chunk2();
    }
    column_peg_1088.chunk2->chunk_identifier = result_peg_1089;
    stream.update(result_peg_1089.getPosition());
    
    
    return result_peg_1089;
    out_peg_1091:
    
    if (column_peg_1088.chunk2 == 0){
        column_peg_1088.chunk2 = new Chunk2();
    }
    column_peg_1088.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    Column & column_peg_1096 = stream.getColumn(position);
    if (column_peg_1096.chunk3 != 0 && column_peg_1096.chunk3->chunk_identifier_list.calculated()){
        return column_peg_1096.chunk3->chunk_identifier_list;
    }
    
    int myposition = position;
    
    
    Result result_peg_1097(myposition);
    
    {
    
        result_peg_1097 = rule_identifier(stream, result_peg_1097.getPosition());
            if (result_peg_1097.error()){
                goto out_peg_1099;
            }
        
        
        
        result_peg_1097.reset();
            do{
                Result result_peg_1101(result_peg_1097.getPosition());
                {
                
                    result_peg_1101 = rule_s(stream, result_peg_1101.getPosition());
                        if (result_peg_1101.error()){
                            goto loop_peg_1100;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1101.getPosition()))){
                                result_peg_1101.nextPosition();
                            } else {
                                goto loop_peg_1100;
                            }
                        }
                        result_peg_1101.setValue((void*) ",");
                    
                    
                    
                    result_peg_1101 = rule_s(stream, result_peg_1101.getPosition());
                        if (result_peg_1101.error()){
                            goto loop_peg_1100;
                        }
                    
                    
                    
                    result_peg_1101 = rule_filename(stream, result_peg_1101.getPosition());
                        if (result_peg_1101.error()){
                            goto loop_peg_1100;
                        }
                    
                    
                }
                result_peg_1097.addResult(result_peg_1101);
            } while (true);
            loop_peg_1100:
            if (result_peg_1097.matches() == 0){
                goto out_peg_1099;
            }
        
        
    }
    
    if (column_peg_1096.chunk3 == 0){
        column_peg_1096.chunk3 = new Chunk3();
    }
    column_peg_1096.chunk3->chunk_identifier_list = result_peg_1097;
    stream.update(result_peg_1097.getPosition());
    
    
    return result_peg_1097;
    out_peg_1099:
    
    if (column_peg_1096.chunk3 == 0){
        column_peg_1096.chunk3 = new Chunk3();
    }
    column_peg_1096.chunk3->chunk_identifier_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_1105 = stream.getColumn(position);
    if (column_peg_1105.chunk3 != 0 && column_peg_1105.chunk3->chunk_valuelist.calculated()){
        return column_peg_1105.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_1106(myposition);
    
    {
    
        result_peg_1106 = rule_value(stream, result_peg_1106.getPosition());
            if (result_peg_1106.error()){
                goto out_peg_1108;
            }
        
        Result result_peg_1107 = result_peg_1106;
        
        result_peg_1106.reset();
            do{
                Result result_peg_1111(result_peg_1106.getPosition());
                {
                
                    result_peg_1111 = rule_s(stream, result_peg_1111.getPosition());
                        if (result_peg_1111.error()){
                            goto loop_peg_1110;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1111.getPosition()))){
                                result_peg_1111.nextPosition();
                            } else {
                                goto loop_peg_1110;
                            }
                        }
                        result_peg_1111.setValue((void*) ",");
                    
                    
                    
                    result_peg_1111 = rule_s(stream, result_peg_1111.getPosition());
                        if (result_peg_1111.error()){
                            goto loop_peg_1110;
                        }
                    
                    
                    
                    int save_peg_1115 = result_peg_1111.getPosition();
                        
                        result_peg_1111 = rule_value(stream, result_peg_1111.getPosition());
                        if (result_peg_1111.error()){
                            
                            result_peg_1111 = Result(save_peg_1115);
                            result_peg_1111.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1106.addResult(result_peg_1111);
            } while (true);
            loop_peg_1110:
            ;
        
        Result result_peg_1109 = result_peg_1106;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_1107.getValues(),result_peg_1109.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1106.setValue(value);
            }
        
        
    }
    
    if (column_peg_1105.chunk3 == 0){
        column_peg_1105.chunk3 = new Chunk3();
    }
    column_peg_1105.chunk3->chunk_valuelist = result_peg_1106;
    stream.update(result_peg_1106.getPosition());
    
    
    return result_peg_1106;
    out_peg_1108:
    
    if (column_peg_1105.chunk3 == 0){
        column_peg_1105.chunk3 = new Chunk3();
    }
    column_peg_1105.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1116 = stream.getColumn(position);
    if (column_peg_1116.chunk3 != 0 && column_peg_1116.chunk3->chunk_value.calculated()){
        return column_peg_1116.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_1117(myposition);
    
    result_peg_1117 = rule_string(stream, result_peg_1117.getPosition());
    if (result_peg_1117.error()){
        goto out_peg_1118;
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1117;
    stream.update(result_peg_1117.getPosition());
    
    
    return result_peg_1117;
    out_peg_1118:
    Result result_peg_1119(myposition);
    
    result_peg_1119 = rule_date(stream, result_peg_1119.getPosition());
    if (result_peg_1119.error()){
        goto out_peg_1120;
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1119;
    stream.update(result_peg_1119.getPosition());
    
    
    return result_peg_1119;
    out_peg_1120:
    Result result_peg_1121(myposition);
    
    result_peg_1121 = rule_number(stream, result_peg_1121.getPosition());
    if (result_peg_1121.error()){
        goto out_peg_1122;
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1121;
    stream.update(result_peg_1121.getPosition());
    
    
    return result_peg_1121;
    out_peg_1122:
    Result result_peg_1123(myposition);
    
    {
    
        result_peg_1123 = rule_keyword(stream, result_peg_1123.getPosition());
            if (result_peg_1123.error()){
                goto out_peg_1125;
            }
        
        Result result_peg_1124 = result_peg_1123;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1124.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1123.setValue(value);
            }
        
        
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1123;
    stream.update(result_peg_1123.getPosition());
    
    
    return result_peg_1123;
    out_peg_1125:
    Result result_peg_1126(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_1126.getPosition()))){
                    result_peg_1126.nextPosition();
                } else {
                    goto out_peg_1128;
                }
            }
            result_peg_1126.setValue((void*) "s");
        
        Result result_peg_1127 = result_peg_1126;
        
        Result result_peg_1131(result_peg_1126);
            {
                
                char letter_peg_1136 = stream.get(result_peg_1131.getPosition());
                if (letter_peg_1136 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1136) != NULL){
                    result_peg_1131.nextPosition();
                    result_peg_1131.setValue((void*) letter_peg_1136);
                } else {
                    goto out_peg_1135;
                }
                
            }
            goto success_peg_1132;
            out_peg_1135:
            goto not_peg_1130;
            success_peg_1132:
            ;
            goto out_peg_1128;
            not_peg_1130:
            result_peg_1126.setValue((void*)0);
        
        
        
        Result result_peg_1139(result_peg_1126);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1139.getPosition()))){
                    result_peg_1139.nextPosition();
                } else {
                    goto not_peg_1138;
                }
            }
            result_peg_1139.setValue((void*) ".");
            goto out_peg_1128;
            not_peg_1138:
            result_peg_1126.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1127.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1126.setValue(value);
            }
        
        
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1126;
    stream.update(result_peg_1126.getPosition());
    
    
    return result_peg_1126;
    out_peg_1128:
    Result result_peg_1140(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_1140.getPosition()))){
                    result_peg_1140.nextPosition();
                } else {
                    goto out_peg_1142;
                }
            }
            result_peg_1140.setValue((void*) "h");
        
        Result result_peg_1141 = result_peg_1140;
        
        Result result_peg_1145(result_peg_1140);
            {
                
                char letter_peg_1150 = stream.get(result_peg_1145.getPosition());
                if (letter_peg_1150 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1150) != NULL){
                    result_peg_1145.nextPosition();
                    result_peg_1145.setValue((void*) letter_peg_1150);
                } else {
                    goto out_peg_1149;
                }
                
            }
            goto success_peg_1146;
            out_peg_1149:
            goto not_peg_1144;
            success_peg_1146:
            ;
            goto out_peg_1142;
            not_peg_1144:
            result_peg_1140.setValue((void*)0);
        
        
        
        Result result_peg_1153(result_peg_1140);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1153.getPosition()))){
                    result_peg_1153.nextPosition();
                } else {
                    goto not_peg_1152;
                }
            }
            result_peg_1153.setValue((void*) ".");
            goto out_peg_1142;
            not_peg_1152:
            result_peg_1140.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1141.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1140.setValue(value);
            }
        
        
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1140;
    stream.update(result_peg_1140.getPosition());
    
    
    return result_peg_1140;
    out_peg_1142:
    Result result_peg_1154(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1154.getPosition()))){
                    result_peg_1154.nextPosition();
                } else {
                    goto out_peg_1156;
                }
            }
            result_peg_1154.setValue((void*) "a");
        
        Result result_peg_1155 = result_peg_1154;
        
        int save_peg_1158 = result_peg_1154.getPosition();
            
            result_peg_1154 = rule_number(stream, result_peg_1154.getPosition());
            if (result_peg_1154.error()){
                
                result_peg_1154 = Result(save_peg_1158);
                result_peg_1154.setValue((void*) 0);
                
            }
        
        
        
        Result result_peg_1161(result_peg_1154);
            {
                
                char letter_peg_1166 = stream.get(result_peg_1161.getPosition());
                if (letter_peg_1166 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1166) != NULL){
                    result_peg_1161.nextPosition();
                    result_peg_1161.setValue((void*) letter_peg_1166);
                } else {
                    goto out_peg_1165;
                }
                
            }
            goto success_peg_1162;
            out_peg_1165:
            goto not_peg_1160;
            success_peg_1162:
            ;
            goto out_peg_1156;
            not_peg_1160:
            result_peg_1154.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1155.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1154.setValue(value);
            }
        
        
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1154;
    stream.update(result_peg_1154.getPosition());
    
    
    return result_peg_1154;
    out_peg_1156:
    Result result_peg_1167(myposition);
    
    result_peg_1167 = rule_filename(stream, result_peg_1167.getPosition());
    if (result_peg_1167.error()){
        goto out_peg_1168;
    }
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = result_peg_1167;
    stream.update(result_peg_1167.getPosition());
    
    
    return result_peg_1167;
    out_peg_1168:
    
    if (column_peg_1116.chunk3 == 0){
        column_peg_1116.chunk3 = new Chunk3();
    }
    column_peg_1116.chunk3->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_1169 = stream.getColumn(position);
    if (column_peg_1169.chunk3 != 0 && column_peg_1169.chunk3->chunk_keyword.calculated()){
        return column_peg_1169.chunk3->chunk_keyword;
    }
    
    int myposition = position;
    
    
    Result result_peg_1170(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareChar("normal"[i], stream.get(result_peg_1170.getPosition()))){
            result_peg_1170.nextPosition();
        } else {
            goto out_peg_1171;
        }
    }
    result_peg_1170.setValue((void*) "normal");
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = result_peg_1170;
    stream.update(result_peg_1170.getPosition());
    
    
    return result_peg_1170;
    out_peg_1171:
    Result result_peg_1172(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("parallax"[i], stream.get(result_peg_1172.getPosition()))){
            result_peg_1172.nextPosition();
        } else {
            goto out_peg_1173;
        }
    }
    result_peg_1172.setValue((void*) "parallax");
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = result_peg_1172;
    stream.update(result_peg_1172.getPosition());
    
    
    return result_peg_1172;
    out_peg_1173:
    Result result_peg_1174(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareChar("addalpha"[i], stream.get(result_peg_1174.getPosition()))){
            result_peg_1174.nextPosition();
        } else {
            goto out_peg_1175;
        }
    }
    result_peg_1174.setValue((void*) "addalpha");
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = result_peg_1174;
    stream.update(result_peg_1174.getPosition());
    
    
    return result_peg_1174;
    out_peg_1175:
    Result result_peg_1176(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareChar("add1"[i], stream.get(result_peg_1176.getPosition()))){
            result_peg_1176.nextPosition();
        } else {
            goto out_peg_1177;
        }
    }
    result_peg_1176.setValue((void*) "add1");
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = result_peg_1176;
    stream.update(result_peg_1176.getPosition());
    
    
    return result_peg_1176;
    out_peg_1177:
    Result result_peg_1178(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("add"[i], stream.get(result_peg_1178.getPosition()))){
            result_peg_1178.nextPosition();
        } else {
            goto out_peg_1179;
        }
    }
    result_peg_1178.setValue((void*) "add");
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = result_peg_1178;
    stream.update(result_peg_1178.getPosition());
    
    
    return result_peg_1178;
    out_peg_1179:
    Result result_peg_1180(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareChar("sub"[i], stream.get(result_peg_1180.getPosition()))){
            result_peg_1180.nextPosition();
        } else {
            goto out_peg_1181;
        }
    }
    result_peg_1180.setValue((void*) "sub");
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = result_peg_1180;
    stream.update(result_peg_1180.getPosition());
    
    
    return result_peg_1180;
    out_peg_1181:
    
    if (column_peg_1169.chunk3 == 0){
        column_peg_1169.chunk3 = new Chunk3();
    }
    column_peg_1169.chunk3->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    Column & column_peg_1182 = stream.getColumn(position);
    if (column_peg_1182.chunk3 != 0 && column_peg_1182.chunk3->chunk_date.calculated()){
        return column_peg_1182.chunk3->chunk_date;
    }
    
    int myposition = position;
    
    
    Result result_peg_1183(myposition);
    
    {
    
        result_peg_1183.reset();
            do{
                Result result_peg_1186(result_peg_1183.getPosition());
                {
                    
                    char letter_peg_1191 = stream.get(result_peg_1186.getPosition());
                    if (letter_peg_1191 != '\0' && strchr("0123456789", letter_peg_1191) != NULL){
                        result_peg_1186.nextPosition();
                        result_peg_1186.setValue((void*) letter_peg_1191);
                    } else {
                        goto out_peg_1190;
                    }
                    
                }
                goto success_peg_1187;
                out_peg_1190:
                goto loop_peg_1185;
                success_peg_1187:
                ;
                result_peg_1183.addResult(result_peg_1186);
            } while (true);
            loop_peg_1185:
            if (result_peg_1183.matches() == 0){
                goto out_peg_1192;
            }
        
        Result result_peg_1184 = result_peg_1183;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1183.getPosition()))){
                    result_peg_1183.nextPosition();
                } else {
                    goto out_peg_1192;
                }
            }
            result_peg_1183.setValue((void*) ".");
        
        
        
        result_peg_1183.reset();
            do{
                Result result_peg_1196(result_peg_1183.getPosition());
                {
                    
                    char letter_peg_1201 = stream.get(result_peg_1196.getPosition());
                    if (letter_peg_1201 != '\0' && strchr("0123456789", letter_peg_1201) != NULL){
                        result_peg_1196.nextPosition();
                        result_peg_1196.setValue((void*) letter_peg_1201);
                    } else {
                        goto out_peg_1200;
                    }
                    
                }
                goto success_peg_1197;
                out_peg_1200:
                goto loop_peg_1195;
                success_peg_1197:
                ;
                result_peg_1183.addResult(result_peg_1196);
            } while (true);
            loop_peg_1195:
            if (result_peg_1183.matches() == 0){
                goto out_peg_1192;
            }
        
        Result result_peg_1194 = result_peg_1183;
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1183.getPosition()))){
                    result_peg_1183.nextPosition();
                } else {
                    goto out_peg_1192;
                }
            }
            result_peg_1183.setValue((void*) ".");
        
        
        
        result_peg_1183.reset();
            do{
                Result result_peg_1205(result_peg_1183.getPosition());
                {
                    
                    char letter_peg_1210 = stream.get(result_peg_1205.getPosition());
                    if (letter_peg_1210 != '\0' && strchr("0123456789", letter_peg_1210) != NULL){
                        result_peg_1205.nextPosition();
                        result_peg_1205.setValue((void*) letter_peg_1210);
                    } else {
                        goto out_peg_1209;
                    }
                    
                }
                goto success_peg_1206;
                out_peg_1209:
                goto loop_peg_1204;
                success_peg_1206:
                ;
                result_peg_1183.addResult(result_peg_1205);
            } while (true);
            loop_peg_1204:
            if (result_peg_1183.matches() == 0){
                goto out_peg_1192;
            }
        
        Result result_peg_1203 = result_peg_1183;
        
        {
                Value value((void*) 0);
                value = makeDate(result_peg_1184.getValues(),result_peg_1194.getValues(),result_peg_1203.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1183.setValue(value);
            }
        
        
    }
    
    if (column_peg_1182.chunk3 == 0){
        column_peg_1182.chunk3 = new Chunk3();
    }
    column_peg_1182.chunk3->chunk_date = result_peg_1183;
    stream.update(result_peg_1183.getPosition());
    
    
    return result_peg_1183;
    out_peg_1192:
    
    if (column_peg_1182.chunk3 == 0){
        column_peg_1182.chunk3 = new Chunk3();
    }
    column_peg_1182.chunk3->chunk_date = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_1211 = stream.getColumn(position);
    if (column_peg_1211.chunk4 != 0 && column_peg_1211.chunk4->chunk_string.calculated()){
        return column_peg_1211.chunk4->chunk_string;
    }
    
    int myposition = position;
    
    Value data;
    Result result_peg_1212(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1212.getPosition()))){
                    result_peg_1212.nextPosition();
                } else {
                    goto out_peg_1214;
                }
            }
            result_peg_1212.setValue((void*) "\"");
        
        
        
        result_peg_1212.reset();
            do{
                Result result_peg_1217(result_peg_1212.getPosition());
                {
                
                    Result result_peg_1220(result_peg_1217);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_1220.getPosition()))){
                                result_peg_1220.nextPosition();
                            } else {
                                goto not_peg_1219;
                            }
                        }
                        result_peg_1220.setValue((void*) "\"");
                        goto loop_peg_1216;
                        not_peg_1219:
                        result_peg_1217.setValue((void*)0);
                    
                    
                    
                    Result result_peg_1223(result_peg_1217);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_1223.getPosition()))){
                                result_peg_1223.nextPosition();
                            } else {
                                goto not_peg_1222;
                            }
                        }
                        result_peg_1223.setValue((void*) "\n");
                        goto loop_peg_1216;
                        not_peg_1222:
                        result_peg_1217.setValue((void*)0);
                    
                    
                    
                    char temp_peg_1224 = stream.get(result_peg_1217.getPosition());
                        if (temp_peg_1224 != '\0'){
                            result_peg_1217.setValue((void*) temp_peg_1224);
                            result_peg_1217.nextPosition();
                        } else {
                            goto loop_peg_1216;
                        }
                    
                    
                }
                result_peg_1212.addResult(result_peg_1217);
            } while (true);
            loop_peg_1216:
            ;
            data = result_peg_1212.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_1212.getPosition()))){
                    result_peg_1212.nextPosition();
                } else {
                    goto out_peg_1214;
                }
            }
            result_peg_1212.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(data); GC::save(as<Ast::String*>(value));
                result_peg_1212.setValue(value);
            }
        
        
    }
    
    if (column_peg_1211.chunk4 == 0){
        column_peg_1211.chunk4 = new Chunk4();
    }
    column_peg_1211.chunk4->chunk_string = result_peg_1212;
    stream.update(result_peg_1212.getPosition());
    
    
    return result_peg_1212;
    out_peg_1214:
    
    if (column_peg_1211.chunk4 == 0){
        column_peg_1211.chunk4 = new Chunk4();
    }
    column_peg_1211.chunk4->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    Column & column_peg_1226 = stream.getColumn(position);
    if (column_peg_1226.chunk4 != 0 && column_peg_1226.chunk4->chunk_number.calculated()){
        return column_peg_1226.chunk4->chunk_number;
    }
    
    int myposition = position;
    
    
    Result result_peg_1227(myposition);
    
    {
    
        int save_peg_1229 = result_peg_1227.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1227.getPosition()))){
                        result_peg_1227.nextPosition();
                    } else {
                        goto out_peg_1232;
                    }
                }
                result_peg_1227.setValue((void*) "+");
                    
            }
            goto success_peg_1230;
            out_peg_1232:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1227.getPosition()))){
                        result_peg_1227.nextPosition();
                    } else {
                        goto out_peg_1234;
                    }
                }
                result_peg_1227.setValue((void*) "-");
                    
            }
            goto success_peg_1230;
            out_peg_1234:
            
            result_peg_1227 = Result(save_peg_1229);
            result_peg_1227.setValue((void*) 0);
            
            success_peg_1230:
            ;
        
        Result result_peg_1228 = result_peg_1227;
        
        result_peg_1227 = rule_float_or_integer(stream, result_peg_1227.getPosition());
            if (result_peg_1227.error()){
                goto out_peg_1236;
            }
        
        Result result_peg_1235 = result_peg_1227;
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_1228.getValues(),result_peg_1235.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_1227.setValue(value);
            }
        
        
    }
    
    if (column_peg_1226.chunk4 == 0){
        column_peg_1226.chunk4 = new Chunk4();
    }
    column_peg_1226.chunk4->chunk_number = result_peg_1227;
    stream.update(result_peg_1227.getPosition());
    
    
    return result_peg_1227;
    out_peg_1236:
    
    if (column_peg_1226.chunk4 == 0){
        column_peg_1226.chunk4 = new Chunk4();
    }
    column_peg_1226.chunk4->chunk_number = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    Column & column_peg_1237 = stream.getColumn(position);
    if (column_peg_1237.chunk4 != 0 && column_peg_1237.chunk4->chunk_float_or_integer.calculated()){
        return column_peg_1237.chunk4->chunk_float_or_integer;
    }
    
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_1238(myposition);
    
    {
    
        result_peg_1238.reset();
            do{
                Result result_peg_1241(result_peg_1238.getPosition());
                {
                    
                    char letter_peg_1246 = stream.get(result_peg_1241.getPosition());
                    if (letter_peg_1246 != '\0' && strchr("0123456789", letter_peg_1246) != NULL){
                        result_peg_1241.nextPosition();
                        result_peg_1241.setValue((void*) letter_peg_1246);
                    } else {
                        goto out_peg_1245;
                    }
                    
                }
                goto success_peg_1242;
                out_peg_1245:
                goto loop_peg_1240;
                success_peg_1242:
                ;
                result_peg_1238.addResult(result_peg_1241);
            } while (true);
            loop_peg_1240:
            ;
            left = result_peg_1238.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1238.getPosition()))){
                    result_peg_1238.nextPosition();
                } else {
                    goto out_peg_1248;
                }
            }
            result_peg_1238.setValue((void*) ".");
        
        
        
        result_peg_1238.reset();
            do{
                Result result_peg_1251(result_peg_1238.getPosition());
                {
                    
                    char letter_peg_1256 = stream.get(result_peg_1251.getPosition());
                    if (letter_peg_1256 != '\0' && strchr("0123456789", letter_peg_1256) != NULL){
                        result_peg_1251.nextPosition();
                        result_peg_1251.setValue((void*) letter_peg_1256);
                    } else {
                        goto out_peg_1255;
                    }
                    
                }
                goto success_peg_1252;
                out_peg_1255:
                goto loop_peg_1250;
                success_peg_1252:
                ;
                result_peg_1238.addResult(result_peg_1251);
            } while (true);
            loop_peg_1250:
            if (result_peg_1238.matches() == 0){
                goto out_peg_1248;
            }
            right = result_peg_1238.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(left,right); GC::save(as<double*>(value));
                result_peg_1238.setValue(value);
            }
        
        
    }
    
    if (column_peg_1237.chunk4 == 0){
        column_peg_1237.chunk4 = new Chunk4();
    }
    column_peg_1237.chunk4->chunk_float_or_integer = result_peg_1238;
    stream.update(result_peg_1238.getPosition());
    
    
    return result_peg_1238;
    out_peg_1248:
    Result result_peg_1257(myposition);
    
    {
    
        result_peg_1257.reset();
            do{
                Result result_peg_1260(result_peg_1257.getPosition());
                {
                    
                    char letter_peg_1265 = stream.get(result_peg_1260.getPosition());
                    if (letter_peg_1265 != '\0' && strchr("0123456789", letter_peg_1265) != NULL){
                        result_peg_1260.nextPosition();
                        result_peg_1260.setValue((void*) letter_peg_1265);
                    } else {
                        goto out_peg_1264;
                    }
                    
                }
                goto success_peg_1261;
                out_peg_1264:
                goto loop_peg_1259;
                success_peg_1261:
                ;
                result_peg_1257.addResult(result_peg_1260);
            } while (true);
            loop_peg_1259:
            if (result_peg_1257.matches() == 0){
                goto out_peg_1266;
            }
        
        Result result_peg_1258 = result_peg_1257;
        
        Result result_peg_1269(result_peg_1257);
            for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_1269.getPosition()))){
                    result_peg_1269.nextPosition();
                } else {
                    goto not_peg_1268;
                }
            }
            result_peg_1269.setValue((void*) ".");
            goto out_peg_1266;
            not_peg_1268:
            result_peg_1257.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_1258.getValues()); GC::save(as<double*>(value));
                result_peg_1257.setValue(value);
            }
        
        
    }
    
    if (column_peg_1237.chunk4 == 0){
        column_peg_1237.chunk4 = new Chunk4();
    }
    column_peg_1237.chunk4->chunk_float_or_integer = result_peg_1257;
    stream.update(result_peg_1257.getPosition());
    
    
    return result_peg_1257;
    out_peg_1266:
    
    if (column_peg_1237.chunk4 == 0){
        column_peg_1237.chunk4 = new Chunk4();
    }
    column_peg_1237.chunk4->chunk_float_or_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

const void * main(const std::string & filename, bool stats = false) throw (ParseException){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}

const void * main(const char * in, bool stats = false) throw (ParseException){
    Stream stream(in);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}



    
    } /* Def */
    
} /* Mugen */

        
