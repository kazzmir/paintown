

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

    namespace Air{
    
    



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
    Result chunk_action;
    Result chunk_action_line;
    Result chunk_line_end;
};

struct Chunk1{
    Result chunk_newline;
    Result chunk_loopstart;
    Result chunk_whitespace;
    Result chunk_sw;
    Result chunk_s;
};

struct Chunk2{
    Result chunk_comment;
    Result chunk_collision_default;
    Result chunk_collision;
    Result chunk_action_start;
    Result chunk_integer;
};

struct Chunk3{
    Result chunk_valuelist;
    Result chunk_value;
};

struct Column{
    Column():
        chunk0(0)
        ,chunk1(0)
        ,chunk2(0)
        ,chunk3(0){
    }

    Chunk0 * chunk0;
    Chunk1 * chunk1;
    Chunk2 * chunk2;
    Chunk3 * chunk3;

    int hitCount(){
        return 
(chunk3 != NULL ? ((chunk3->chunk_valuelist.calculated() ? 1 : 0)
+ (chunk3->chunk_value.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_comment.calculated() ? 1 : 0)
+ (chunk2->chunk_collision_default.calculated() ? 1 : 0)
+ (chunk2->chunk_collision.calculated() ? 1 : 0)
+ (chunk2->chunk_action_start.calculated() ? 1 : 0)
+ (chunk2->chunk_integer.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_newline.calculated() ? 1 : 0)
+ (chunk1->chunk_loopstart.calculated() ? 1 : 0)
+ (chunk1->chunk_whitespace.calculated() ? 1 : 0)
+ (chunk1->chunk_sw.calculated() ? 1 : 0)
+ (chunk1->chunk_s.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_action.calculated() ? 1 : 0)
+ (chunk0->chunk_action_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 17;
    }

    ~Column(){
        delete chunk0;
        delete chunk1;
        delete chunk2;
        delete chunk3;
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
Result rule_action(Stream &, const int);
Result rule_action_line(Stream &, const int, Value section);
Result rule_line_end(Stream &, const int);
Result rule_newline(Stream &, const int);
Result rule_loopstart(Stream &, const int);
Result rule_whitespace(Stream &, const int);
Result rule_sw(Stream &, const int);
Result rule_s(Stream &, const int);
Result rule_comment(Stream &, const int);
Result rule_collision_default(Stream &, const int);
Result rule_collision(Stream &, const int);
Result rule_action_start(Stream &, const int);
Result rule_integer(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_value(Stream &, const int);


template<class X>
X as(const Value & value){
    return (X) value.getValue();
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

Ast::String * makeString(std::string * str){
    Ast::String * object = new Ast::String(str);
    GC::save(object);
    return object;
}

Ast::String * makeString(const Value & value){
    Ast::String * object = new Ast::String(toString(value));
    GC::save(object);
    return object;
}

Ast::Section * makeSection(const Value & str){
    Ast::Section * object = new Ast::Section(as<std::string*>(str));
    GC::save(object);
    return object;
}

Ast::Keyword * makeKeyword(const Value & value){
    Ast::Keyword * object = new Ast::Keyword(as<char*>(value));
    GC::save(object);
    return object;
}

Ast::Value * makeValueList(const Value & n1, const Value & n2, const Value & n3, const Value & n4){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(n1));
    values.push_back(as<Ast::Value*>(n2));
    values.push_back(as<Ast::Value*>(n3));
    values.push_back(as<Ast::Value*>(n4));

    Ast::ValueList * object = new Ast::ValueList(values);
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

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0 && strcmp(as<const char *>(sign), "-") == 0){
        value = -value;
    }

    Ast::Number * object = new Ast::Number(value);
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

std::string * makeHeader(const Value & begin, const Value & action, const Value & num){
    std::ostringstream out;
    out << as<char*>(begin) << " " << as<char*>(action) << " " << (as<Ast::Value*>(num))->toString();
    std::string * object = new std::string(out.str());
    GC::save(object);
    return object;
}

SectionList * makeSectionList(){
    SectionList * object = new SectionList();
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributeArray(Ast::Keyword * name, Ast::Value * index, Ast::Value * value){
    Ast::AttributeArray * object = new Ast::AttributeArray(name, index, value);
    GC::save(object);
    return object;
}

void addSection(const Value & section_list_value, const Value & section_value){
  SectionList * sections = (SectionList*) section_list_value.getValue();
  Ast::Section * section = (Ast::Section*) section_value.getValue();
  if (section == 0){
    throw ParseException("Cannot add null section");
  }
  sections->push_back(section);
}

Ast::Attribute * makeAttributeKeyword(const Value & id, const Value & data){
    Ast::AttributeKeyword * object = new Ast::AttributeKeyword(as<Ast::Keyword*>(id), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributeKeyword(const Value & id){
    Ast::AttributeKeyword * object = new Ast::AttributeKeyword(as<Ast::Keyword*>(id));
    GC::save(object);
    return object;
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
                value = makeSectionList();
                result_peg_2.setValue(value);
            }
            current = result_peg_2.getValues();
        
        
        
        result_peg_2 = rule_whitespace(stream, result_peg_2.getPosition());
            if (result_peg_2.error()){
                goto out_peg_5;
            }
        
        
        
        result_peg_2.reset();
            do{
                Result result_peg_8(result_peg_2.getPosition());
                result_peg_8 = rule_newline(stream, result_peg_8.getPosition());
                if (result_peg_8.error()){
                    goto loop_peg_7;
                }
                result_peg_2.addResult(result_peg_8);
            } while (true);
            loop_peg_7:
            ;
        
        
        
        result_peg_2.reset();
            do{
                Result result_peg_11(result_peg_2.getPosition());
                {
                
                    result_peg_11 = rule_line(stream, result_peg_11.getPosition(), current);
                        if (result_peg_11.error()){
                            goto loop_peg_10;
                        }
                    
                    
                    
                    result_peg_11 = rule_whitespace(stream, result_peg_11.getPosition());
                        if (result_peg_11.error()){
                            goto loop_peg_10;
                        }
                    
                    
                    
                    int save_peg_14 = result_peg_11.getPosition();
                        
                        result_peg_11 = rule_line_end(stream, result_peg_11.getPosition());
                        if (result_peg_11.error()){
                            
                            result_peg_11 = Result(save_peg_14);
                            result_peg_11.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_2.addResult(result_peg_11);
            } while (true);
            loop_peg_10:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_2.getPosition())){
                result_peg_2.nextPosition();
                result_peg_2.setValue((void *) '\0');
            } else {
                goto out_peg_5;
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
    out_peg_5:
    
    if (column_peg_1.chunk0 == 0){
        column_peg_1.chunk0 = new Chunk0();
    }
    column_peg_1.chunk0->chunk_start = errorResult;
    stream.update(errorResult.getPosition());
    
     GC::cleanup(0); 
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    int myposition = position;
    
    
    Result result_peg_17(myposition);
    
    {
    
        result_peg_17 = rule_s(stream, result_peg_17.getPosition());
            if (result_peg_17.error()){
                goto out_peg_19;
            }
        
        
        
        result_peg_17 = rule_comment(stream, result_peg_17.getPosition());
            if (result_peg_17.error()){
                goto out_peg_19;
            }
        
        
    }
    
    
    return result_peg_17;
    out_peg_19:
    Result result_peg_20(myposition);
    
    {
    
        result_peg_20 = rule_s(stream, result_peg_20.getPosition());
            if (result_peg_20.error()){
                goto out_peg_22;
            }
        
        
        
        result_peg_20 = rule_action(stream, result_peg_20.getPosition());
            if (result_peg_20.error()){
                goto out_peg_22;
            }
        
        Result result_peg_23 = result_peg_20;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_23.getValues());
                result_peg_20.setValue(value);
            }
        
        
    }
    
    
    return result_peg_20;
    out_peg_22:
    
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_24 = stream.getColumn(position);
    if (column_peg_24.chunk0 != 0 && column_peg_24.chunk0->chunk_action.calculated()){
        return column_peg_24.chunk0->chunk_action;
    }
    
    int myposition = position;
    
    Value ast;
    Result result_peg_25(myposition);
    
    {
    
        result_peg_25 = rule_action_start(stream, result_peg_25.getPosition());
            if (result_peg_25.error()){
                goto out_peg_27;
            }
        
        Result result_peg_26 = result_peg_25;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_26.getValues());
                result_peg_25.setValue(value);
            }
            ast = result_peg_25.getValues();
        
        
        
        result_peg_25 = rule_whitespace(stream, result_peg_25.getPosition());
            if (result_peg_25.error()){
                goto out_peg_27;
            }
        
        
        
        result_peg_25.reset();
            do{
                Result result_peg_32(result_peg_25.getPosition());
                result_peg_32 = rule_newline(stream, result_peg_32.getPosition());
                if (result_peg_32.error()){
                    goto loop_peg_31;
                }
                result_peg_25.addResult(result_peg_32);
            } while (true);
            loop_peg_31:
            if (result_peg_25.matches() == 0){
                goto out_peg_27;
            }
        
        
        
        result_peg_25.reset();
            do{
                Result result_peg_35(result_peg_25.getPosition());
                {
                
                    result_peg_35 = rule_action_line(stream, result_peg_35.getPosition(), ast);
                        if (result_peg_35.error()){
                            goto loop_peg_34;
                        }
                    
                    
                    
                    result_peg_35 = rule_whitespace(stream, result_peg_35.getPosition());
                        if (result_peg_35.error()){
                            goto loop_peg_34;
                        }
                    
                    
                    
                    result_peg_35 = rule_line_end(stream, result_peg_35.getPosition());
                        if (result_peg_35.error()){
                            goto loop_peg_34;
                        }
                    
                    
                }
                result_peg_25.addResult(result_peg_35);
            } while (true);
            loop_peg_34:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_25.setValue(value);
            }
        
        
    }
    
    if (column_peg_24.chunk0 == 0){
        column_peg_24.chunk0 = new Chunk0();
    }
    column_peg_24.chunk0->chunk_action = result_peg_25;
    stream.update(result_peg_25.getPosition());
    
    
    return result_peg_25;
    out_peg_27:
    
    if (column_peg_24.chunk0 == 0){
        column_peg_24.chunk0 = new Chunk0();
    }
    column_peg_24.chunk0->chunk_action = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position, Value section){
    
    int myposition = position;
    
    Value data;
    Result result_peg_39(myposition);
    
    {
    
        result_peg_39 = rule_s(stream, result_peg_39.getPosition());
            if (result_peg_39.error()){
                goto out_peg_41;
            }
        
        
        
        result_peg_39 = rule_comment(stream, result_peg_39.getPosition());
            if (result_peg_39.error()){
                goto out_peg_41;
            }
        
        
    }
    
    
    return result_peg_39;
    out_peg_41:
    Result result_peg_42(myposition);
    
    {
    
        result_peg_42 = rule_s(stream, result_peg_42.getPosition());
            if (result_peg_42.error()){
                goto out_peg_44;
            }
        
        
        
        result_peg_42 = rule_collision_default(stream, result_peg_42.getPosition());
            if (result_peg_42.error()){
                goto out_peg_44;
            }
            data = result_peg_42.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_42.setValue(value);
            }
        
        
    }
    
    
    return result_peg_42;
    out_peg_44:
    Result result_peg_46(myposition);
    
    {
    
        result_peg_46 = rule_s(stream, result_peg_46.getPosition());
            if (result_peg_46.error()){
                goto out_peg_48;
            }
        
        
        
        result_peg_46 = rule_collision(stream, result_peg_46.getPosition());
            if (result_peg_46.error()){
                goto out_peg_48;
            }
            data = result_peg_46.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_46.setValue(value);
            }
        
        
    }
    
    
    return result_peg_46;
    out_peg_48:
    Result result_peg_50(myposition);
    
    {
    
        result_peg_50 = rule_s(stream, result_peg_50.getPosition());
            if (result_peg_50.error()){
                goto out_peg_52;
            }
        
        
        
        result_peg_50 = rule_valuelist(stream, result_peg_50.getPosition());
            if (result_peg_50.error()){
                goto out_peg_52;
            }
            data = result_peg_50.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                result_peg_50.setValue(value);
            }
        
        
    }
    
    
    return result_peg_50;
    out_peg_52:
    Result result_peg_54(myposition);
    
    {
    
        result_peg_54 = rule_s(stream, result_peg_54.getPosition());
            if (result_peg_54.error()){
                goto out_peg_56;
            }
        
        
        
        result_peg_54 = rule_loopstart(stream, result_peg_54.getPosition());
            if (result_peg_54.error()){
                goto out_peg_56;
            }
            data = result_peg_54.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_54.setValue(value);
            }
        
        
    }
    
    
    return result_peg_54;
    out_peg_56:
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_58 = stream.getColumn(position);
    if (column_peg_58.chunk0 != 0 && column_peg_58.chunk0->chunk_line_end.calculated()){
        return column_peg_58.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_59(myposition);
    
    result_peg_59.reset();
    do{
        Result result_peg_61(result_peg_59.getPosition());
        result_peg_61 = rule_newline(stream, result_peg_61.getPosition());
        if (result_peg_61.error()){
            goto loop_peg_60;
        }
        result_peg_59.addResult(result_peg_61);
    } while (true);
    loop_peg_60:
    if (result_peg_59.matches() == 0){
        goto out_peg_62;
    }
    
    if (column_peg_58.chunk0 == 0){
        column_peg_58.chunk0 = new Chunk0();
    }
    column_peg_58.chunk0->chunk_line_end = result_peg_59;
    stream.update(result_peg_59.getPosition());
    
    
    return result_peg_59;
    out_peg_62:
    Result result_peg_63(myposition);
    
    {
    
        Result result_peg_65(result_peg_63.getPosition());
            if ('\0' == stream.get(result_peg_65.getPosition())){
                result_peg_65.nextPosition();
                result_peg_65.setValue((void *) '\0');
            } else {
                goto out_peg_66;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_58.chunk0 == 0){
        column_peg_58.chunk0 = new Chunk0();
    }
    column_peg_58.chunk0->chunk_line_end = result_peg_63;
    stream.update(result_peg_63.getPosition());
    
    
    return result_peg_63;
    out_peg_66:
    
    if (column_peg_58.chunk0 == 0){
        column_peg_58.chunk0 = new Chunk0();
    }
    column_peg_58.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_67 = stream.getColumn(position);
    if (column_peg_67.chunk1 != 0 && column_peg_67.chunk1->chunk_newline.calculated()){
        return column_peg_67.chunk1->chunk_newline;
    }
    
    int myposition = position;
    
    
    Result result_peg_68(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_68.getPosition()))){
            result_peg_68.nextPosition();
        } else {
            goto out_peg_69;
        }
    }
    result_peg_68.setValue((void*) "\n");
    
    if (column_peg_67.chunk1 == 0){
        column_peg_67.chunk1 = new Chunk1();
    }
    column_peg_67.chunk1->chunk_newline = result_peg_68;
    stream.update(result_peg_68.getPosition());
    
    
    return result_peg_68;
    out_peg_69:
    Result result_peg_70(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_70.getPosition()))){
            result_peg_70.nextPosition();
        } else {
            goto out_peg_71;
        }
    }
    result_peg_70.setValue((void*) "\r");
    
    if (column_peg_67.chunk1 == 0){
        column_peg_67.chunk1 = new Chunk1();
    }
    column_peg_67.chunk1->chunk_newline = result_peg_70;
    stream.update(result_peg_70.getPosition());
    
    
    return result_peg_70;
    out_peg_71:
    
    if (column_peg_67.chunk1 == 0){
        column_peg_67.chunk1 = new Chunk1();
    }
    column_peg_67.chunk1->chunk_newline = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_72 = stream.getColumn(position);
    if (column_peg_72.chunk1 != 0 && column_peg_72.chunk1->chunk_loopstart.calculated()){
        return column_peg_72.chunk1->chunk_loopstart;
    }
    
    int myposition = position;
    
    
    Result result_peg_73(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_73.getPosition()))){
                    result_peg_73.nextPosition();
                } else {
                    goto out_peg_75;
                }
            }
            result_peg_73.setValue((void*) "loopstart");
        
        Result result_peg_74 = result_peg_73;
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_74.getValues()));
                result_peg_73.setValue(value);
            }
        
        
    }
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_loopstart = result_peg_73;
    stream.update(result_peg_73.getPosition());
    
    
    return result_peg_73;
    out_peg_75:
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_76 = stream.getColumn(position);
    if (column_peg_76.chunk1 != 0 && column_peg_76.chunk1->chunk_whitespace.calculated()){
        return column_peg_76.chunk1->chunk_whitespace;
    }
    
    int myposition = position;
    
    
    Result result_peg_77(myposition);
    
    result_peg_77.reset();
    do{
        Result result_peg_79(result_peg_77.getPosition());
        result_peg_79 = rule_sw(stream, result_peg_79.getPosition());
        if (result_peg_79.error()){
            goto loop_peg_78;
        }
        result_peg_77.addResult(result_peg_79);
    } while (true);
    loop_peg_78:
    ;
    
    if (column_peg_76.chunk1 == 0){
        column_peg_76.chunk1 = new Chunk1();
    }
    column_peg_76.chunk1->chunk_whitespace = result_peg_77;
    stream.update(result_peg_77.getPosition());
    
    
    return result_peg_77;
    
    if (column_peg_76.chunk1 == 0){
        column_peg_76.chunk1 = new Chunk1();
    }
    column_peg_76.chunk1->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_80 = stream.getColumn(position);
    if (column_peg_80.chunk1 != 0 && column_peg_80.chunk1->chunk_sw.calculated()){
        return column_peg_80.chunk1->chunk_sw;
    }
    
    int myposition = position;
    
    
    Result result_peg_81(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_81.getPosition()))){
                result_peg_81.nextPosition();
            } else {
                goto out_peg_84;
            }
        }
        result_peg_81.setValue((void*) " ");
            
    }
    goto success_peg_82;
    out_peg_84:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_81.getPosition()))){
                result_peg_81.nextPosition();
            } else {
                goto out_peg_86;
            }
        }
        result_peg_81.setValue((void*) "\t");
            
    }
    goto success_peg_82;
    out_peg_86:
    {
        
        if ((unsigned char) stream.get(result_peg_81.getPosition()) == (unsigned char) 255){
            result_peg_81.nextPosition();
        } else {
            goto out_peg_88;
        }
        result_peg_81.setValue((void*) 255);
        
    }
    goto success_peg_82;
    out_peg_88:
    goto out_peg_89;
    success_peg_82:
    ;
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_sw = result_peg_81;
    stream.update(result_peg_81.getPosition());
    
    
    return result_peg_81;
    out_peg_89:
    Result result_peg_90(myposition);
    
    result_peg_90 = rule_comment(stream, result_peg_90.getPosition());
    if (result_peg_90.error()){
        goto out_peg_91;
    }
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_sw = result_peg_90;
    stream.update(result_peg_90.getPosition());
    
    
    return result_peg_90;
    out_peg_91:
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_92 = stream.getColumn(position);
    if (column_peg_92.chunk1 != 0 && column_peg_92.chunk1->chunk_s.calculated()){
        return column_peg_92.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_93(myposition);
    
    result_peg_93.reset();
    do{
        Result result_peg_95(result_peg_93.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_95.getPosition()))){
                    result_peg_95.nextPosition();
                } else {
                    goto out_peg_98;
                }
            }
            result_peg_95.setValue((void*) " ");
                
        }
        goto success_peg_96;
        out_peg_98:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_95.getPosition()))){
                    result_peg_95.nextPosition();
                } else {
                    goto out_peg_100;
                }
            }
            result_peg_95.setValue((void*) "\t");
                
        }
        goto success_peg_96;
        out_peg_100:
        {
            
            if ((unsigned char) stream.get(result_peg_95.getPosition()) == (unsigned char) 255){
                result_peg_95.nextPosition();
            } else {
                goto out_peg_102;
            }
            result_peg_95.setValue((void*) 255);
            
        }
        goto success_peg_96;
        out_peg_102:
        goto loop_peg_94;
        success_peg_96:
        ;
        result_peg_93.addResult(result_peg_95);
    } while (true);
    loop_peg_94:
    ;
    
    if (column_peg_92.chunk1 == 0){
        column_peg_92.chunk1 = new Chunk1();
    }
    column_peg_92.chunk1->chunk_s = result_peg_93;
    stream.update(result_peg_93.getPosition());
    
    
    return result_peg_93;
    
    if (column_peg_92.chunk1 == 0){
        column_peg_92.chunk1 = new Chunk1();
    }
    column_peg_92.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_103 = stream.getColumn(position);
    if (column_peg_103.chunk2 != 0 && column_peg_103.chunk2->chunk_comment.calculated()){
        return column_peg_103.chunk2->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_104(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_104.getPosition()))){
                    result_peg_104.nextPosition();
                } else {
                    goto out_peg_106;
                }
            }
            result_peg_104.setValue((void*) ";");
        
        
        
        result_peg_104.reset();
            do{
                Result result_peg_108(result_peg_104.getPosition());
                {
                
                    Result result_peg_111(result_peg_108);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_111.getPosition()))){
                                result_peg_111.nextPosition();
                            } else {
                                goto not_peg_110;
                            }
                        }
                        result_peg_111.setValue((void*) "\n");
                        goto loop_peg_107;
                        not_peg_110:
                        result_peg_108.setValue((void*)0);
                    
                    
                    
                    char temp_peg_112 = stream.get(result_peg_108.getPosition());
                        if (temp_peg_112 != '\0'){
                            result_peg_108.setValue((void*) temp_peg_112);
                            result_peg_108.nextPosition();
                        } else {
                            goto loop_peg_107;
                        }
                    
                    
                }
                result_peg_104.addResult(result_peg_108);
            } while (true);
            loop_peg_107:
            ;
        
        
    }
    
    if (column_peg_103.chunk2 == 0){
        column_peg_103.chunk2 = new Chunk2();
    }
    column_peg_103.chunk2->chunk_comment = result_peg_104;
    stream.update(result_peg_104.getPosition());
    
    
    return result_peg_104;
    out_peg_106:
    Result result_peg_113(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_113.getPosition()))){
                    result_peg_113.nextPosition();
                } else {
                    goto out_peg_115;
                }
            }
            result_peg_113.setValue((void*) "=");
        
        
        
        result_peg_113.reset();
            do{
                Result result_peg_117(result_peg_113.getPosition());
                {
                
                    Result result_peg_120(result_peg_117);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_120.getPosition()))){
                                result_peg_120.nextPosition();
                            } else {
                                goto not_peg_119;
                            }
                        }
                        result_peg_120.setValue((void*) "\n");
                        goto loop_peg_116;
                        not_peg_119:
                        result_peg_117.setValue((void*)0);
                    
                    
                    
                    char temp_peg_121 = stream.get(result_peg_117.getPosition());
                        if (temp_peg_121 != '\0'){
                            result_peg_117.setValue((void*) temp_peg_121);
                            result_peg_117.nextPosition();
                        } else {
                            goto loop_peg_116;
                        }
                    
                    
                }
                result_peg_113.addResult(result_peg_117);
            } while (true);
            loop_peg_116:
            ;
        
        
    }
    
    if (column_peg_103.chunk2 == 0){
        column_peg_103.chunk2 = new Chunk2();
    }
    column_peg_103.chunk2->chunk_comment = result_peg_113;
    stream.update(result_peg_113.getPosition());
    
    
    return result_peg_113;
    out_peg_115:
    Result result_peg_122(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_122.getPosition()))){
                    result_peg_122.nextPosition();
                } else {
                    goto out_peg_124;
                }
            }
            result_peg_122.setValue((void*) "-");
        
        
        
        result_peg_122.reset();
            do{
                Result result_peg_126(result_peg_122.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_126.getPosition()))){
                        result_peg_126.nextPosition();
                    } else {
                        goto loop_peg_125;
                    }
                }
                result_peg_126.setValue((void*) "-");
                result_peg_122.addResult(result_peg_126);
            } while (true);
            loop_peg_125:
            if (result_peg_122.matches() == 0){
                goto out_peg_124;
            }
        
        
    }
    
    if (column_peg_103.chunk2 == 0){
        column_peg_103.chunk2 = new Chunk2();
    }
    column_peg_103.chunk2->chunk_comment = result_peg_122;
    stream.update(result_peg_122.getPosition());
    
    
    return result_peg_122;
    out_peg_124:
    
    if (column_peg_103.chunk2 == 0){
        column_peg_103.chunk2 = new Chunk2();
    }
    column_peg_103.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_127 = stream.getColumn(position);
    if (column_peg_127.chunk2 != 0 && column_peg_127.chunk2->chunk_collision_default.calculated()){
        return column_peg_127.chunk2->chunk_collision_default;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_128(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("Clsn2Default"[i], stream.get(result_peg_128.getPosition()))){
                    result_peg_128.nextPosition();
                } else {
                    goto out_peg_130;
                }
            }
            result_peg_128.setValue((void*) "Clsn2Default");
        
        Result result_peg_129 = result_peg_128;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_128.getPosition()))){
                    result_peg_128.nextPosition();
                } else {
                    goto out_peg_130;
                }
            }
            result_peg_128.setValue((void*) ":");
        
        
        
        result_peg_128 = rule_s(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
        
        
        
        result_peg_128 = rule_integer(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
            num = result_peg_128.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_129.getValues()), num);
                result_peg_128.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_collision_default = result_peg_128;
    stream.update(result_peg_128.getPosition());
    
    
    return result_peg_128;
    out_peg_130:
    Result result_peg_134(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) "Clsn1Default");
        
        Result result_peg_135 = result_peg_134;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) ":");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_integer(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
            num = result_peg_134.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_135.getValues()), num);
                result_peg_134.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_collision_default = result_peg_134;
    stream.update(result_peg_134.getPosition());
    
    
    return result_peg_134;
    out_peg_136:
    Result result_peg_140(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_140.getPosition()))){
                    result_peg_140.nextPosition();
                } else {
                    goto out_peg_142;
                }
            }
            result_peg_140.setValue((void*) "Clsn2");
        
        Result result_peg_141 = result_peg_140;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_140.getPosition()))){
                    result_peg_140.nextPosition();
                } else {
                    goto out_peg_142;
                }
            }
            result_peg_140.setValue((void*) ":");
        
        
        
        result_peg_140 = rule_s(stream, result_peg_140.getPosition());
            if (result_peg_140.error()){
                goto out_peg_142;
            }
        
        
        
        result_peg_140 = rule_integer(stream, result_peg_140.getPosition());
            if (result_peg_140.error()){
                goto out_peg_142;
            }
            num = result_peg_140.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_141.getValues()), num);
                result_peg_140.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_collision_default = result_peg_140;
    stream.update(result_peg_140.getPosition());
    
    
    return result_peg_140;
    out_peg_142:
    Result result_peg_146(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_146.getPosition()))){
                    result_peg_146.nextPosition();
                } else {
                    goto out_peg_148;
                }
            }
            result_peg_146.setValue((void*) "Clsn1");
        
        Result result_peg_147 = result_peg_146;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_146.getPosition()))){
                    result_peg_146.nextPosition();
                } else {
                    goto out_peg_148;
                }
            }
            result_peg_146.setValue((void*) ":");
        
        
        
        result_peg_146 = rule_s(stream, result_peg_146.getPosition());
            if (result_peg_146.error()){
                goto out_peg_148;
            }
        
        
        
        result_peg_146 = rule_integer(stream, result_peg_146.getPosition());
            if (result_peg_146.error()){
                goto out_peg_148;
            }
            num = result_peg_146.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_147.getValues()), num);
                result_peg_146.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_collision_default = result_peg_146;
    stream.update(result_peg_146.getPosition());
    
    
    return result_peg_146;
    out_peg_148:
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_152 = stream.getColumn(position);
    if (column_peg_152.chunk2 != 0 && column_peg_152.chunk2->chunk_collision.calculated()){
        return column_peg_152.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    Value name;
    Value index;
    Value n1;
    Value n2;
    Value n3;
    Value n4;
    Result result_peg_153(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) "Clsn2");
            name = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) "[");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_integer(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            index = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) "]");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) "=");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_integer(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            n1 = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) ",");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_integer(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            n2 = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) ",");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_integer(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            n3 = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) ",");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_integer(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            n4 = result_peg_153.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_153.setValue(value);
            }
        
        
    }
    
    if (column_peg_152.chunk2 == 0){
        column_peg_152.chunk2 = new Chunk2();
    }
    column_peg_152.chunk2->chunk_collision = result_peg_153;
    stream.update(result_peg_153.getPosition());
    
    
    return result_peg_153;
    out_peg_155:
    Result result_peg_178(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) "Clsn1");
            name = result_peg_178.getValues();
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) "[");
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        result_peg_178 = rule_integer(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
            index = result_peg_178.getValues();
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) "]");
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) "=");
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        result_peg_178 = rule_integer(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
            n1 = result_peg_178.getValues();
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) ",");
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        result_peg_178 = rule_integer(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
            n2 = result_peg_178.getValues();
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) ",");
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        result_peg_178 = rule_integer(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
            n3 = result_peg_178.getValues();
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_178.getPosition()))){
                    result_peg_178.nextPosition();
                } else {
                    goto out_peg_180;
                }
            }
            result_peg_178.setValue((void*) ",");
        
        
        
        result_peg_178 = rule_s(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        result_peg_178 = rule_integer(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
            n4 = result_peg_178.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_178.setValue(value);
            }
        
        
    }
    
    if (column_peg_152.chunk2 == 0){
        column_peg_152.chunk2 = new Chunk2();
    }
    column_peg_152.chunk2->chunk_collision = result_peg_178;
    stream.update(result_peg_178.getPosition());
    
    
    return result_peg_178;
    out_peg_180:
    
    if (column_peg_152.chunk2 == 0){
        column_peg_152.chunk2 = new Chunk2();
    }
    column_peg_152.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_203 = stream.getColumn(position);
    if (column_peg_203.chunk2 != 0 && column_peg_203.chunk2->chunk_action_start.calculated()){
        return column_peg_203.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_204(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_204.getPosition()))){
                    result_peg_204.nextPosition();
                } else {
                    goto out_peg_206;
                }
            }
            result_peg_204.setValue((void*) "[");
        
        
        
        result_peg_204 = rule_s(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_204.getPosition()))){
                    result_peg_204.nextPosition();
                } else {
                    goto out_peg_206;
                }
            }
            result_peg_204.setValue((void*) "Begin");
            begin = result_peg_204.getValues();
        
        
        
        result_peg_204 = rule_s(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_204.getPosition()))){
                    result_peg_204.nextPosition();
                } else {
                    goto out_peg_206;
                }
            }
            result_peg_204.setValue((void*) "Action");
            action = result_peg_204.getValues();
        
        
        
        result_peg_204 = rule_s(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
        
        
        
        result_peg_204 = rule_integer(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
            num = result_peg_204.getValues();
        
        
        
        result_peg_204 = rule_s(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_204.getPosition()))){
                    result_peg_204.nextPosition();
                } else {
                    goto out_peg_206;
                }
            }
            result_peg_204.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num);
                result_peg_204.setValue(value);
            }
        
        
    }
    
    if (column_peg_203.chunk2 == 0){
        column_peg_203.chunk2 = new Chunk2();
    }
    column_peg_203.chunk2->chunk_action_start = result_peg_204;
    stream.update(result_peg_204.getPosition());
    
    
    return result_peg_204;
    out_peg_206:
    
    if (column_peg_203.chunk2 == 0){
        column_peg_203.chunk2 = new Chunk2();
    }
    column_peg_203.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_215 = stream.getColumn(position);
    if (column_peg_215.chunk2 != 0 && column_peg_215.chunk2->chunk_integer.calculated()){
        return column_peg_215.chunk2->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_216(myposition);
    
    {
    
        int save_peg_218 = result_peg_216.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_216.getPosition()))){
                        result_peg_216.nextPosition();
                    } else {
                        goto out_peg_221;
                    }
                }
                result_peg_216.setValue((void*) "-");
                    
            }
            goto success_peg_219;
            out_peg_221:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_216.getPosition()))){
                        result_peg_216.nextPosition();
                    } else {
                        goto out_peg_223;
                    }
                }
                result_peg_216.setValue((void*) "+");
                    
            }
            goto success_peg_219;
            out_peg_223:
            
            result_peg_216 = Result(save_peg_218);
            result_peg_216.setValue((void*) 0);
            
            success_peg_219:
            ;
        
        Result result_peg_217 = result_peg_216;
        
        result_peg_216.reset();
            do{
                Result result_peg_226(result_peg_216.getPosition());
                {
                    
                    char letter_peg_231 = stream.get(result_peg_226.getPosition());
                    if (letter_peg_231 != '\0' && strchr("0123456789", letter_peg_231) != NULL){
                        result_peg_226.nextPosition();
                        result_peg_226.setValue((void*) letter_peg_231);
                    } else {
                        goto out_peg_230;
                    }
                    
                }
                goto success_peg_227;
                out_peg_230:
                goto loop_peg_225;
                success_peg_227:
                ;
                result_peg_216.addResult(result_peg_226);
            } while (true);
            loop_peg_225:
            if (result_peg_216.matches() == 0){
                goto out_peg_232;
            }
        
        Result result_peg_224 = result_peg_216;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_224.getValues()); value = makeNumber(result_peg_217.getValues(),value);
                result_peg_216.setValue(value);
            }
        
        
    }
    
    if (column_peg_215.chunk2 == 0){
        column_peg_215.chunk2 = new Chunk2();
    }
    column_peg_215.chunk2->chunk_integer = result_peg_216;
    stream.update(result_peg_216.getPosition());
    
    
    return result_peg_216;
    out_peg_232:
    
    if (column_peg_215.chunk2 == 0){
        column_peg_215.chunk2 = new Chunk2();
    }
    column_peg_215.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_233 = stream.getColumn(position);
    if (column_peg_233.chunk3 != 0 && column_peg_233.chunk3->chunk_valuelist.calculated()){
        return column_peg_233.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_234(myposition);
    
    {
    
        result_peg_234 = rule_value(stream, result_peg_234.getPosition());
            if (result_peg_234.error()){
                goto out_peg_236;
            }
        
        Result result_peg_235 = result_peg_234;
        
        result_peg_234.reset();
            do{
                Result result_peg_239(result_peg_234.getPosition());
                {
                
                    result_peg_239 = rule_s(stream, result_peg_239.getPosition());
                        if (result_peg_239.error()){
                            goto loop_peg_238;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_239.getPosition()))){
                                result_peg_239.nextPosition();
                            } else {
                                goto loop_peg_238;
                            }
                        }
                        result_peg_239.setValue((void*) ",");
                    
                    
                    
                    result_peg_239 = rule_s(stream, result_peg_239.getPosition());
                        if (result_peg_239.error()){
                            goto loop_peg_238;
                        }
                    
                    
                    
                    int save_peg_243 = result_peg_239.getPosition();
                        
                        result_peg_239 = rule_value(stream, result_peg_239.getPosition());
                        if (result_peg_239.error()){
                            
                            result_peg_239 = Result(save_peg_243);
                            result_peg_239.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_234.addResult(result_peg_239);
            } while (true);
            loop_peg_238:
            ;
        
        Result result_peg_237 = result_peg_234;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_235.getValues(),result_peg_237.getValues());
                result_peg_234.setValue(value);
            }
        
        
    }
    
    if (column_peg_233.chunk3 == 0){
        column_peg_233.chunk3 = new Chunk3();
    }
    column_peg_233.chunk3->chunk_valuelist = result_peg_234;
    stream.update(result_peg_234.getPosition());
    
    
    return result_peg_234;
    out_peg_236:
    
    if (column_peg_233.chunk3 == 0){
        column_peg_233.chunk3 = new Chunk3();
    }
    column_peg_233.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_244 = stream.getColumn(position);
    if (column_peg_244.chunk3 != 0 && column_peg_244.chunk3->chunk_value.calculated()){
        return column_peg_244.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_245(myposition);
    
    result_peg_245 = rule_integer(stream, result_peg_245.getPosition());
    if (result_peg_245.error()){
        goto out_peg_246;
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_245;
    stream.update(result_peg_245.getPosition());
    
    
    return result_peg_245;
    out_peg_246:
    Result result_peg_247(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_247.getPosition()))){
                    result_peg_247.nextPosition();
                } else {
                    goto out_peg_249;
                }
            }
            result_peg_247.setValue((void*) "as");
        
        Result result_peg_248 = result_peg_247;
        
        result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
            if (result_peg_247.error()){
                goto out_peg_249;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_247.getPosition()))){
                    result_peg_247.nextPosition();
                } else {
                    goto out_peg_249;
                }
            }
            result_peg_247.setValue((void*) "d");
        
        
        
        result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
            if (result_peg_247.error()){
                goto out_peg_249;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_248.getValues());
                result_peg_247.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_247;
    stream.update(result_peg_247.getPosition());
    
    
    return result_peg_247;
    out_peg_249:
    Result result_peg_253(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) "a");
        
        Result result_peg_254 = result_peg_253;
        
        result_peg_253 = rule_integer(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_254.getValues());
                result_peg_253.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_253;
    stream.update(result_peg_253.getPosition());
    
    
    return result_peg_253;
    out_peg_255:
    Result result_peg_257(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_257.getPosition()))){
                    result_peg_257.nextPosition();
                } else {
                    goto out_peg_259;
                }
            }
            result_peg_257.setValue((void*) "a");
        
        Result result_peg_258 = result_peg_257;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_258.getValues());
                result_peg_257.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_257;
    stream.update(result_peg_257.getPosition());
    
    
    return result_peg_257;
    out_peg_259:
    Result result_peg_260(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_260.getPosition()))){
                    result_peg_260.nextPosition();
                } else {
                    goto out_peg_262;
                }
            }
            result_peg_260.setValue((void*) "s");
        
        Result result_peg_261 = result_peg_260;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_261.getValues());
                result_peg_260.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_260;
    stream.update(result_peg_260.getPosition());
    
    
    return result_peg_260;
    out_peg_262:
    Result result_peg_263(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_263.getPosition()))){
                    result_peg_263.nextPosition();
                } else {
                    goto out_peg_265;
                }
            }
            result_peg_263.setValue((void*) "vh");
        
        Result result_peg_264 = result_peg_263;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_264.getValues());
                result_peg_263.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_263;
    stream.update(result_peg_263.getPosition());
    
    
    return result_peg_263;
    out_peg_265:
    Result result_peg_266(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_266.getPosition()))){
                    result_peg_266.nextPosition();
                } else {
                    goto out_peg_268;
                }
            }
            result_peg_266.setValue((void*) "hv");
        
        Result result_peg_267 = result_peg_266;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_267.getValues());
                result_peg_266.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_266;
    stream.update(result_peg_266.getPosition());
    
    
    return result_peg_266;
    out_peg_268:
    Result result_peg_269(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_269.getPosition()))){
                    result_peg_269.nextPosition();
                } else {
                    goto out_peg_271;
                }
            }
            result_peg_269.setValue((void*) "v");
        
        Result result_peg_270 = result_peg_269;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_270.getValues());
                result_peg_269.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_269;
    stream.update(result_peg_269.getPosition());
    
    
    return result_peg_269;
    out_peg_271:
    Result result_peg_272(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_272.getPosition()))){
                    result_peg_272.nextPosition();
                } else {
                    goto out_peg_274;
                }
            }
            result_peg_272.setValue((void*) "h");
        
        Result result_peg_273 = result_peg_272;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_273.getValues());
                result_peg_272.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = result_peg_272;
    stream.update(result_peg_272.getPosition());
    
    
    return result_peg_272;
    out_peg_274:
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
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



    
    } /* Air */
    
} /* Mugen */

        
