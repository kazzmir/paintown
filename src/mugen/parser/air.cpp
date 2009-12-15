

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
    if (sign.getValue() != 0){
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
    
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    Column & column_peg_16 = stream.getColumn(position);
    if (column_peg_16.chunk0 != 0 && column_peg_16.chunk0->chunk_line.calculated()){
        return column_peg_16.chunk0->chunk_line;
    }
    
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
    
    if (column_peg_16.chunk0 == 0){
        column_peg_16.chunk0 = new Chunk0();
    }
    column_peg_16.chunk0->chunk_line = result_peg_17;
    stream.update(result_peg_17.getPosition());
    
    
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
    
    if (column_peg_16.chunk0 == 0){
        column_peg_16.chunk0 = new Chunk0();
    }
    column_peg_16.chunk0->chunk_line = result_peg_20;
    stream.update(result_peg_20.getPosition());
    
    
    return result_peg_20;
    out_peg_22:
    
    if (column_peg_16.chunk0 == 0){
        column_peg_16.chunk0 = new Chunk0();
    }
    column_peg_16.chunk0->chunk_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
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
    
    Column & column_peg_38 = stream.getColumn(position);
    if (column_peg_38.chunk0 != 0 && column_peg_38.chunk0->chunk_action_line.calculated()){
        return column_peg_38.chunk0->chunk_action_line;
    }
    
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
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_39;
    stream.update(result_peg_39.getPosition());
    
    
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
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_42;
    stream.update(result_peg_42.getPosition());
    
    
    return result_peg_42;
    out_peg_44:
    Result result_peg_45(myposition);
    
    {
    
        result_peg_45 = rule_s(stream, result_peg_45.getPosition());
            if (result_peg_45.error()){
                goto out_peg_47;
            }
        
        
        
        result_peg_45 = rule_collision(stream, result_peg_45.getPosition());
            if (result_peg_45.error()){
                goto out_peg_47;
            }
            data = result_peg_45.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_45.setValue(value);
            }
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_45;
    stream.update(result_peg_45.getPosition());
    
    
    return result_peg_45;
    out_peg_47:
    Result result_peg_49(myposition);
    
    {
    
        result_peg_49 = rule_s(stream, result_peg_49.getPosition());
            if (result_peg_49.error()){
                goto out_peg_51;
            }
        
        
        
        result_peg_49 = rule_valuelist(stream, result_peg_49.getPosition());
            if (result_peg_49.error()){
                goto out_peg_51;
            }
            data = result_peg_49.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                result_peg_49.setValue(value);
            }
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_49;
    stream.update(result_peg_49.getPosition());
    
    
    return result_peg_49;
    out_peg_51:
    Result result_peg_53(myposition);
    
    {
    
        result_peg_53 = rule_s(stream, result_peg_53.getPosition());
            if (result_peg_53.error()){
                goto out_peg_55;
            }
        
        
        
        result_peg_53 = rule_loopstart(stream, result_peg_53.getPosition());
            if (result_peg_53.error()){
                goto out_peg_55;
            }
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_53;
    stream.update(result_peg_53.getPosition());
    
    
    return result_peg_53;
    out_peg_55:
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_56 = stream.getColumn(position);
    if (column_peg_56.chunk0 != 0 && column_peg_56.chunk0->chunk_line_end.calculated()){
        return column_peg_56.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_57(myposition);
    
    result_peg_57.reset();
    do{
        Result result_peg_59(result_peg_57.getPosition());
        result_peg_59 = rule_newline(stream, result_peg_59.getPosition());
        if (result_peg_59.error()){
            goto loop_peg_58;
        }
        result_peg_57.addResult(result_peg_59);
    } while (true);
    loop_peg_58:
    if (result_peg_57.matches() == 0){
        goto out_peg_60;
    }
    
    if (column_peg_56.chunk0 == 0){
        column_peg_56.chunk0 = new Chunk0();
    }
    column_peg_56.chunk0->chunk_line_end = result_peg_57;
    stream.update(result_peg_57.getPosition());
    
    
    return result_peg_57;
    out_peg_60:
    Result result_peg_61(myposition);
    
    {
    
        Result result_peg_63(result_peg_61.getPosition());
            if ('\0' == stream.get(result_peg_63.getPosition())){
                result_peg_63.nextPosition();
                result_peg_63.setValue((void *) '\0');
            } else {
                goto out_peg_64;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_56.chunk0 == 0){
        column_peg_56.chunk0 = new Chunk0();
    }
    column_peg_56.chunk0->chunk_line_end = result_peg_61;
    stream.update(result_peg_61.getPosition());
    
    
    return result_peg_61;
    out_peg_64:
    
    if (column_peg_56.chunk0 == 0){
        column_peg_56.chunk0 = new Chunk0();
    }
    column_peg_56.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_65 = stream.getColumn(position);
    if (column_peg_65.chunk1 != 0 && column_peg_65.chunk1->chunk_newline.calculated()){
        return column_peg_65.chunk1->chunk_newline;
    }
    
    int myposition = position;
    
    
    Result result_peg_66(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_66.getPosition()))){
            result_peg_66.nextPosition();
        } else {
            goto out_peg_67;
        }
    }
    result_peg_66.setValue((void*) "\n");
    
    if (column_peg_65.chunk1 == 0){
        column_peg_65.chunk1 = new Chunk1();
    }
    column_peg_65.chunk1->chunk_newline = result_peg_66;
    stream.update(result_peg_66.getPosition());
    
    
    return result_peg_66;
    out_peg_67:
    Result result_peg_68(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_68.getPosition()))){
            result_peg_68.nextPosition();
        } else {
            goto out_peg_69;
        }
    }
    result_peg_68.setValue((void*) "\r");
    
    if (column_peg_65.chunk1 == 0){
        column_peg_65.chunk1 = new Chunk1();
    }
    column_peg_65.chunk1->chunk_newline = result_peg_68;
    stream.update(result_peg_68.getPosition());
    
    
    return result_peg_68;
    out_peg_69:
    
    if (column_peg_65.chunk1 == 0){
        column_peg_65.chunk1 = new Chunk1();
    }
    column_peg_65.chunk1->chunk_newline = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_70 = stream.getColumn(position);
    if (column_peg_70.chunk1 != 0 && column_peg_70.chunk1->chunk_loopstart.calculated()){
        return column_peg_70.chunk1->chunk_loopstart;
    }
    
    int myposition = position;
    
    
    Result result_peg_71(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_71.getPosition()))){
                    result_peg_71.nextPosition();
                } else {
                    goto out_peg_73;
                }
            }
            result_peg_71.setValue((void*) "loopstart");
        
        Result result_peg_72 = result_peg_71;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_72.getValues());
                result_peg_71.setValue(value);
            }
        
        
    }
    
    if (column_peg_70.chunk1 == 0){
        column_peg_70.chunk1 = new Chunk1();
    }
    column_peg_70.chunk1->chunk_loopstart = result_peg_71;
    stream.update(result_peg_71.getPosition());
    
    
    return result_peg_71;
    out_peg_73:
    
    if (column_peg_70.chunk1 == 0){
        column_peg_70.chunk1 = new Chunk1();
    }
    column_peg_70.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_74 = stream.getColumn(position);
    if (column_peg_74.chunk1 != 0 && column_peg_74.chunk1->chunk_whitespace.calculated()){
        return column_peg_74.chunk1->chunk_whitespace;
    }
    
    int myposition = position;
    
    
    Result result_peg_75(myposition);
    
    result_peg_75.reset();
    do{
        Result result_peg_77(result_peg_75.getPosition());
        result_peg_77 = rule_sw(stream, result_peg_77.getPosition());
        if (result_peg_77.error()){
            goto loop_peg_76;
        }
        result_peg_75.addResult(result_peg_77);
    } while (true);
    loop_peg_76:
    ;
    
    if (column_peg_74.chunk1 == 0){
        column_peg_74.chunk1 = new Chunk1();
    }
    column_peg_74.chunk1->chunk_whitespace = result_peg_75;
    stream.update(result_peg_75.getPosition());
    
    
    return result_peg_75;
    
    if (column_peg_74.chunk1 == 0){
        column_peg_74.chunk1 = new Chunk1();
    }
    column_peg_74.chunk1->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_78 = stream.getColumn(position);
    if (column_peg_78.chunk1 != 0 && column_peg_78.chunk1->chunk_sw.calculated()){
        return column_peg_78.chunk1->chunk_sw;
    }
    
    int myposition = position;
    
    
    Result result_peg_79(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_79.getPosition()))){
                result_peg_79.nextPosition();
            } else {
                goto out_peg_82;
            }
        }
        result_peg_79.setValue((void*) " ");
            
    }
    goto success_peg_80;
    out_peg_82:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_79.getPosition()))){
                result_peg_79.nextPosition();
            } else {
                goto out_peg_84;
            }
        }
        result_peg_79.setValue((void*) "\t");
            
    }
    goto success_peg_80;
    out_peg_84:
    {
        
        if ((unsigned char) stream.get(result_peg_79.getPosition()) == (unsigned char) 255){
            result_peg_79.nextPosition();
        } else {
            goto out_peg_86;
        }
        result_peg_79.setValue((void*) 255);
        
    }
    goto success_peg_80;
    out_peg_86:
    goto out_peg_87;
    success_peg_80:
    ;
    
    if (column_peg_78.chunk1 == 0){
        column_peg_78.chunk1 = new Chunk1();
    }
    column_peg_78.chunk1->chunk_sw = result_peg_79;
    stream.update(result_peg_79.getPosition());
    
    
    return result_peg_79;
    out_peg_87:
    Result result_peg_88(myposition);
    
    result_peg_88 = rule_comment(stream, result_peg_88.getPosition());
    if (result_peg_88.error()){
        goto out_peg_89;
    }
    
    if (column_peg_78.chunk1 == 0){
        column_peg_78.chunk1 = new Chunk1();
    }
    column_peg_78.chunk1->chunk_sw = result_peg_88;
    stream.update(result_peg_88.getPosition());
    
    
    return result_peg_88;
    out_peg_89:
    
    if (column_peg_78.chunk1 == 0){
        column_peg_78.chunk1 = new Chunk1();
    }
    column_peg_78.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_90 = stream.getColumn(position);
    if (column_peg_90.chunk1 != 0 && column_peg_90.chunk1->chunk_s.calculated()){
        return column_peg_90.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_91(myposition);
    
    result_peg_91.reset();
    do{
        Result result_peg_93(result_peg_91.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_93.getPosition()))){
                    result_peg_93.nextPosition();
                } else {
                    goto out_peg_96;
                }
            }
            result_peg_93.setValue((void*) " ");
                
        }
        goto success_peg_94;
        out_peg_96:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_93.getPosition()))){
                    result_peg_93.nextPosition();
                } else {
                    goto out_peg_98;
                }
            }
            result_peg_93.setValue((void*) "\t");
                
        }
        goto success_peg_94;
        out_peg_98:
        {
            
            if ((unsigned char) stream.get(result_peg_93.getPosition()) == (unsigned char) 255){
                result_peg_93.nextPosition();
            } else {
                goto out_peg_100;
            }
            result_peg_93.setValue((void*) 255);
            
        }
        goto success_peg_94;
        out_peg_100:
        goto loop_peg_92;
        success_peg_94:
        ;
        result_peg_91.addResult(result_peg_93);
    } while (true);
    loop_peg_92:
    ;
    
    if (column_peg_90.chunk1 == 0){
        column_peg_90.chunk1 = new Chunk1();
    }
    column_peg_90.chunk1->chunk_s = result_peg_91;
    stream.update(result_peg_91.getPosition());
    
    
    return result_peg_91;
    
    if (column_peg_90.chunk1 == 0){
        column_peg_90.chunk1 = new Chunk1();
    }
    column_peg_90.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_101 = stream.getColumn(position);
    if (column_peg_101.chunk2 != 0 && column_peg_101.chunk2->chunk_comment.calculated()){
        return column_peg_101.chunk2->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_102(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_102.getPosition()))){
                    result_peg_102.nextPosition();
                } else {
                    goto out_peg_104;
                }
            }
            result_peg_102.setValue((void*) ";");
        
        
        
        result_peg_102.reset();
            do{
                Result result_peg_106(result_peg_102.getPosition());
                {
                
                    Result result_peg_109(result_peg_106);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_109.getPosition()))){
                                result_peg_109.nextPosition();
                            } else {
                                goto not_peg_108;
                            }
                        }
                        result_peg_109.setValue((void*) "\n");
                        goto loop_peg_105;
                        not_peg_108:
                        result_peg_106.setValue((void*)0);
                    
                    
                    
                    char temp_peg_110 = stream.get(result_peg_106.getPosition());
                        if (temp_peg_110 != '\0'){
                            result_peg_106.setValue((void*) temp_peg_110);
                            result_peg_106.nextPosition();
                        } else {
                            goto loop_peg_105;
                        }
                    
                    
                }
                result_peg_102.addResult(result_peg_106);
            } while (true);
            loop_peg_105:
            ;
        
        
    }
    
    if (column_peg_101.chunk2 == 0){
        column_peg_101.chunk2 = new Chunk2();
    }
    column_peg_101.chunk2->chunk_comment = result_peg_102;
    stream.update(result_peg_102.getPosition());
    
    
    return result_peg_102;
    out_peg_104:
    Result result_peg_111(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_111.getPosition()))){
                    result_peg_111.nextPosition();
                } else {
                    goto out_peg_113;
                }
            }
            result_peg_111.setValue((void*) "=");
        
        
        
        result_peg_111.reset();
            do{
                Result result_peg_115(result_peg_111.getPosition());
                {
                
                    Result result_peg_118(result_peg_115);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_118.getPosition()))){
                                result_peg_118.nextPosition();
                            } else {
                                goto not_peg_117;
                            }
                        }
                        result_peg_118.setValue((void*) "\n");
                        goto loop_peg_114;
                        not_peg_117:
                        result_peg_115.setValue((void*)0);
                    
                    
                    
                    char temp_peg_119 = stream.get(result_peg_115.getPosition());
                        if (temp_peg_119 != '\0'){
                            result_peg_115.setValue((void*) temp_peg_119);
                            result_peg_115.nextPosition();
                        } else {
                            goto loop_peg_114;
                        }
                    
                    
                }
                result_peg_111.addResult(result_peg_115);
            } while (true);
            loop_peg_114:
            ;
        
        
    }
    
    if (column_peg_101.chunk2 == 0){
        column_peg_101.chunk2 = new Chunk2();
    }
    column_peg_101.chunk2->chunk_comment = result_peg_111;
    stream.update(result_peg_111.getPosition());
    
    
    return result_peg_111;
    out_peg_113:
    Result result_peg_120(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_120.getPosition()))){
                    result_peg_120.nextPosition();
                } else {
                    goto out_peg_122;
                }
            }
            result_peg_120.setValue((void*) "-");
        
        
        
        result_peg_120.reset();
            do{
                Result result_peg_124(result_peg_120.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_124.getPosition()))){
                        result_peg_124.nextPosition();
                    } else {
                        goto loop_peg_123;
                    }
                }
                result_peg_124.setValue((void*) "-");
                result_peg_120.addResult(result_peg_124);
            } while (true);
            loop_peg_123:
            if (result_peg_120.matches() == 0){
                goto out_peg_122;
            }
        
        
    }
    
    if (column_peg_101.chunk2 == 0){
        column_peg_101.chunk2 = new Chunk2();
    }
    column_peg_101.chunk2->chunk_comment = result_peg_120;
    stream.update(result_peg_120.getPosition());
    
    
    return result_peg_120;
    out_peg_122:
    
    if (column_peg_101.chunk2 == 0){
        column_peg_101.chunk2 = new Chunk2();
    }
    column_peg_101.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_125 = stream.getColumn(position);
    if (column_peg_125.chunk2 != 0 && column_peg_125.chunk2->chunk_collision_default.calculated()){
        return column_peg_125.chunk2->chunk_collision_default;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_126(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_126.getPosition()))){
                    result_peg_126.nextPosition();
                } else {
                    goto out_peg_128;
                }
            }
            result_peg_126.setValue((void*) "Clsn2Default:");
        
        Result result_peg_127 = result_peg_126;
        
        result_peg_126 = rule_s(stream, result_peg_126.getPosition());
            if (result_peg_126.error()){
                goto out_peg_128;
            }
        
        
        
        result_peg_126 = rule_integer(stream, result_peg_126.getPosition());
            if (result_peg_126.error()){
                goto out_peg_128;
            }
            num = result_peg_126.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_127.getValues()); value = makeAttributeKeyword(value, num);
                result_peg_126.setValue(value);
            }
        
        
    }
    
    if (column_peg_125.chunk2 == 0){
        column_peg_125.chunk2 = new Chunk2();
    }
    column_peg_125.chunk2->chunk_collision_default = result_peg_126;
    stream.update(result_peg_126.getPosition());
    
    
    return result_peg_126;
    out_peg_128:
    Result result_peg_131(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_131.getPosition()))){
                    result_peg_131.nextPosition();
                } else {
                    goto out_peg_133;
                }
            }
            result_peg_131.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_131 = rule_s(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
        
        
        
        result_peg_131 = rule_integer(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
            num = result_peg_131.getValues();
        
        
    }
    
    if (column_peg_125.chunk2 == 0){
        column_peg_125.chunk2 = new Chunk2();
    }
    column_peg_125.chunk2->chunk_collision_default = result_peg_131;
    stream.update(result_peg_131.getPosition());
    
    
    return result_peg_131;
    out_peg_133:
    Result result_peg_135(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) "Clsn2:");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_integer(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
            num = result_peg_135.getValues();
        
        
    }
    
    if (column_peg_125.chunk2 == 0){
        column_peg_125.chunk2 = new Chunk2();
    }
    column_peg_125.chunk2->chunk_collision_default = result_peg_135;
    stream.update(result_peg_135.getPosition());
    
    
    return result_peg_135;
    out_peg_137:
    Result result_peg_139(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_139.getPosition()))){
                    result_peg_139.nextPosition();
                } else {
                    goto out_peg_141;
                }
            }
            result_peg_139.setValue((void*) "Clsn1:");
        
        
        
        result_peg_139 = rule_s(stream, result_peg_139.getPosition());
            if (result_peg_139.error()){
                goto out_peg_141;
            }
        
        
        
        result_peg_139 = rule_integer(stream, result_peg_139.getPosition());
            if (result_peg_139.error()){
                goto out_peg_141;
            }
            num = result_peg_139.getValues();
        
        
    }
    
    if (column_peg_125.chunk2 == 0){
        column_peg_125.chunk2 = new Chunk2();
    }
    column_peg_125.chunk2->chunk_collision_default = result_peg_139;
    stream.update(result_peg_139.getPosition());
    
    
    return result_peg_139;
    out_peg_141:
    
    if (column_peg_125.chunk2 == 0){
        column_peg_125.chunk2 = new Chunk2();
    }
    column_peg_125.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_143 = stream.getColumn(position);
    if (column_peg_143.chunk2 != 0 && column_peg_143.chunk2->chunk_collision.calculated()){
        return column_peg_143.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    Value name;
    Value index;
    Value n1;
    Value n2;
    Value n3;
    Value n4;
    Result result_peg_144(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) "Clsn2");
            name = result_peg_144.getValues();
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) "[");
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        result_peg_144 = rule_integer(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
            index = result_peg_144.getValues();
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) "]");
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) "=");
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        result_peg_144 = rule_integer(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
            n1 = result_peg_144.getValues();
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) ",");
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        result_peg_144 = rule_integer(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
            n2 = result_peg_144.getValues();
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) ",");
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        result_peg_144 = rule_integer(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
            n3 = result_peg_144.getValues();
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) ",");
        
        
        
        result_peg_144 = rule_s(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
        
        
        
        result_peg_144 = rule_integer(stream, result_peg_144.getPosition());
            if (result_peg_144.error()){
                goto out_peg_146;
            }
            n4 = result_peg_144.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_144.setValue(value);
            }
        
        
    }
    
    if (column_peg_143.chunk2 == 0){
        column_peg_143.chunk2 = new Chunk2();
    }
    column_peg_143.chunk2->chunk_collision = result_peg_144;
    stream.update(result_peg_144.getPosition());
    
    
    return result_peg_144;
    out_peg_146:
    Result result_peg_169(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) "Clsn1");
            name = result_peg_169.getValues();
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) "[");
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        result_peg_169 = rule_integer(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
            index = result_peg_169.getValues();
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) "]");
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) "=");
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        result_peg_169 = rule_integer(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
            n1 = result_peg_169.getValues();
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) ",");
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        result_peg_169 = rule_integer(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
            n2 = result_peg_169.getValues();
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) ",");
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        result_peg_169 = rule_integer(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
            n3 = result_peg_169.getValues();
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_169.getPosition()))){
                    result_peg_169.nextPosition();
                } else {
                    goto out_peg_171;
                }
            }
            result_peg_169.setValue((void*) ",");
        
        
        
        result_peg_169 = rule_s(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
        
        
        
        result_peg_169 = rule_integer(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_171;
            }
            n4 = result_peg_169.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_169.setValue(value);
            }
        
        
    }
    
    if (column_peg_143.chunk2 == 0){
        column_peg_143.chunk2 = new Chunk2();
    }
    column_peg_143.chunk2->chunk_collision = result_peg_169;
    stream.update(result_peg_169.getPosition());
    
    
    return result_peg_169;
    out_peg_171:
    
    if (column_peg_143.chunk2 == 0){
        column_peg_143.chunk2 = new Chunk2();
    }
    column_peg_143.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_194 = stream.getColumn(position);
    if (column_peg_194.chunk2 != 0 && column_peg_194.chunk2->chunk_action_start.calculated()){
        return column_peg_194.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_195(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_195.getPosition()))){
                    result_peg_195.nextPosition();
                } else {
                    goto out_peg_197;
                }
            }
            result_peg_195.setValue((void*) "[");
        
        
        
        result_peg_195 = rule_s(stream, result_peg_195.getPosition());
            if (result_peg_195.error()){
                goto out_peg_197;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_195.getPosition()))){
                    result_peg_195.nextPosition();
                } else {
                    goto out_peg_197;
                }
            }
            result_peg_195.setValue((void*) "Begin");
            begin = result_peg_195.getValues();
        
        
        
        result_peg_195 = rule_s(stream, result_peg_195.getPosition());
            if (result_peg_195.error()){
                goto out_peg_197;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_195.getPosition()))){
                    result_peg_195.nextPosition();
                } else {
                    goto out_peg_197;
                }
            }
            result_peg_195.setValue((void*) "Action");
            action = result_peg_195.getValues();
        
        
        
        result_peg_195 = rule_s(stream, result_peg_195.getPosition());
            if (result_peg_195.error()){
                goto out_peg_197;
            }
        
        
        
        result_peg_195 = rule_integer(stream, result_peg_195.getPosition());
            if (result_peg_195.error()){
                goto out_peg_197;
            }
            num = result_peg_195.getValues();
        
        
        
        result_peg_195 = rule_s(stream, result_peg_195.getPosition());
            if (result_peg_195.error()){
                goto out_peg_197;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_195.getPosition()))){
                    result_peg_195.nextPosition();
                } else {
                    goto out_peg_197;
                }
            }
            result_peg_195.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num);
                result_peg_195.setValue(value);
            }
        
        
    }
    
    if (column_peg_194.chunk2 == 0){
        column_peg_194.chunk2 = new Chunk2();
    }
    column_peg_194.chunk2->chunk_action_start = result_peg_195;
    stream.update(result_peg_195.getPosition());
    
    
    return result_peg_195;
    out_peg_197:
    
    if (column_peg_194.chunk2 == 0){
        column_peg_194.chunk2 = new Chunk2();
    }
    column_peg_194.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_206 = stream.getColumn(position);
    if (column_peg_206.chunk2 != 0 && column_peg_206.chunk2->chunk_integer.calculated()){
        return column_peg_206.chunk2->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_207(myposition);
    
    {
    
        int save_peg_209 = result_peg_207.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_207.getPosition()))){
                        result_peg_207.nextPosition();
                    } else {
                        goto out_peg_212;
                    }
                }
                result_peg_207.setValue((void*) "-");
                    
            }
            goto success_peg_210;
            out_peg_212:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_207.getPosition()))){
                        result_peg_207.nextPosition();
                    } else {
                        goto out_peg_214;
                    }
                }
                result_peg_207.setValue((void*) "+");
                    
            }
            goto success_peg_210;
            out_peg_214:
            
            result_peg_207 = Result(save_peg_209);
            result_peg_207.setValue((void*) 0);
            
            success_peg_210:
            ;
        
        Result result_peg_208 = result_peg_207;
        
        result_peg_207.reset();
            do{
                Result result_peg_217(result_peg_207.getPosition());
                {
                    
                    char letter_peg_222 = stream.get(result_peg_217.getPosition());
                    if (letter_peg_222 != '\0' && strchr("0123456789", letter_peg_222) != NULL){
                        result_peg_217.nextPosition();
                        result_peg_217.setValue((void*) letter_peg_222);
                    } else {
                        goto out_peg_221;
                    }
                    
                }
                goto success_peg_218;
                out_peg_221:
                goto loop_peg_216;
                success_peg_218:
                ;
                result_peg_207.addResult(result_peg_217);
            } while (true);
            loop_peg_216:
            if (result_peg_207.matches() == 0){
                goto out_peg_223;
            }
        
        Result result_peg_215 = result_peg_207;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_215.getValues()); value = makeNumber(result_peg_208.getValues(),value);
                result_peg_207.setValue(value);
            }
        
        
    }
    
    if (column_peg_206.chunk2 == 0){
        column_peg_206.chunk2 = new Chunk2();
    }
    column_peg_206.chunk2->chunk_integer = result_peg_207;
    stream.update(result_peg_207.getPosition());
    
    
    return result_peg_207;
    out_peg_223:
    
    if (column_peg_206.chunk2 == 0){
        column_peg_206.chunk2 = new Chunk2();
    }
    column_peg_206.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_224 = stream.getColumn(position);
    if (column_peg_224.chunk3 != 0 && column_peg_224.chunk3->chunk_valuelist.calculated()){
        return column_peg_224.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_225(myposition);
    
    {
    
        result_peg_225 = rule_value(stream, result_peg_225.getPosition());
            if (result_peg_225.error()){
                goto out_peg_227;
            }
        
        Result result_peg_226 = result_peg_225;
        
        result_peg_225.reset();
            do{
                Result result_peg_230(result_peg_225.getPosition());
                {
                
                    result_peg_230 = rule_s(stream, result_peg_230.getPosition());
                        if (result_peg_230.error()){
                            goto loop_peg_229;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_230.getPosition()))){
                                result_peg_230.nextPosition();
                            } else {
                                goto loop_peg_229;
                            }
                        }
                        result_peg_230.setValue((void*) ",");
                    
                    
                    
                    result_peg_230 = rule_s(stream, result_peg_230.getPosition());
                        if (result_peg_230.error()){
                            goto loop_peg_229;
                        }
                    
                    
                    
                    int save_peg_234 = result_peg_230.getPosition();
                        
                        result_peg_230 = rule_value(stream, result_peg_230.getPosition());
                        if (result_peg_230.error()){
                            
                            result_peg_230 = Result(save_peg_234);
                            result_peg_230.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_225.addResult(result_peg_230);
            } while (true);
            loop_peg_229:
            ;
        
        Result result_peg_228 = result_peg_225;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_226.getValues(),result_peg_228.getValues());
                result_peg_225.setValue(value);
            }
        
        
    }
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_valuelist = result_peg_225;
    stream.update(result_peg_225.getPosition());
    
    
    return result_peg_225;
    out_peg_227:
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_235 = stream.getColumn(position);
    if (column_peg_235.chunk3 != 0 && column_peg_235.chunk3->chunk_value.calculated()){
        return column_peg_235.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_236(myposition);
    
    result_peg_236 = rule_integer(stream, result_peg_236.getPosition());
    if (result_peg_236.error()){
        goto out_peg_237;
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_236;
    stream.update(result_peg_236.getPosition());
    
    
    return result_peg_236;
    out_peg_237:
    Result result_peg_238(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_238.getPosition()))){
                    result_peg_238.nextPosition();
                } else {
                    goto out_peg_240;
                }
            }
            result_peg_238.setValue((void*) "as");
        
        Result result_peg_239 = result_peg_238;
        
        result_peg_238 = rule_integer(stream, result_peg_238.getPosition());
            if (result_peg_238.error()){
                goto out_peg_240;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_238.getPosition()))){
                    result_peg_238.nextPosition();
                } else {
                    goto out_peg_240;
                }
            }
            result_peg_238.setValue((void*) "d");
        
        
        
        result_peg_238 = rule_integer(stream, result_peg_238.getPosition());
            if (result_peg_238.error()){
                goto out_peg_240;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_239.getValues());
                result_peg_238.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_238;
    stream.update(result_peg_238.getPosition());
    
    
    return result_peg_238;
    out_peg_240:
    Result result_peg_244(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_244.getPosition()))){
                    result_peg_244.nextPosition();
                } else {
                    goto out_peg_246;
                }
            }
            result_peg_244.setValue((void*) "a");
        
        Result result_peg_245 = result_peg_244;
        
        result_peg_244 = rule_integer(stream, result_peg_244.getPosition());
            if (result_peg_244.error()){
                goto out_peg_246;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_245.getValues());
                result_peg_244.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_244;
    stream.update(result_peg_244.getPosition());
    
    
    return result_peg_244;
    out_peg_246:
    Result result_peg_248(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_248.getPosition()))){
                    result_peg_248.nextPosition();
                } else {
                    goto out_peg_250;
                }
            }
            result_peg_248.setValue((void*) "a");
        
        Result result_peg_249 = result_peg_248;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_249.getValues());
                result_peg_248.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_248;
    stream.update(result_peg_248.getPosition());
    
    
    return result_peg_248;
    out_peg_250:
    Result result_peg_251(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_251.getPosition()))){
                    result_peg_251.nextPosition();
                } else {
                    goto out_peg_253;
                }
            }
            result_peg_251.setValue((void*) "s");
        
        Result result_peg_252 = result_peg_251;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_252.getValues());
                result_peg_251.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_251;
    stream.update(result_peg_251.getPosition());
    
    
    return result_peg_251;
    out_peg_253:
    Result result_peg_254(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_254.getPosition()))){
                    result_peg_254.nextPosition();
                } else {
                    goto out_peg_256;
                }
            }
            result_peg_254.setValue((void*) "vh");
        
        Result result_peg_255 = result_peg_254;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_255.getValues());
                result_peg_254.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_254;
    stream.update(result_peg_254.getPosition());
    
    
    return result_peg_254;
    out_peg_256:
    Result result_peg_257(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_257.getPosition()))){
                    result_peg_257.nextPosition();
                } else {
                    goto out_peg_259;
                }
            }
            result_peg_257.setValue((void*) "hv");
        
        Result result_peg_258 = result_peg_257;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_258.getValues());
                result_peg_257.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_257;
    stream.update(result_peg_257.getPosition());
    
    
    return result_peg_257;
    out_peg_259:
    Result result_peg_260(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_260.getPosition()))){
                    result_peg_260.nextPosition();
                } else {
                    goto out_peg_262;
                }
            }
            result_peg_260.setValue((void*) "v");
        
        Result result_peg_261 = result_peg_260;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_261.getValues());
                result_peg_260.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_260;
    stream.update(result_peg_260.getPosition());
    
    
    return result_peg_260;
    out_peg_262:
    Result result_peg_263(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_263.getPosition()))){
                    result_peg_263.nextPosition();
                } else {
                    goto out_peg_265;
                }
            }
            result_peg_263.setValue((void*) "h");
        
        Result result_peg_264 = result_peg_263;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_264.getValues());
                result_peg_263.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = result_peg_263;
    stream.update(result_peg_263.getPosition());
    
    
    return result_peg_263;
    out_peg_265:
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

const void * main(const std::string & filename, bool stats = false){
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

const void * main(const char * in, bool stats = false){
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



    
    } /* Air */
    
} /* Mugen */

        
