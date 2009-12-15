

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
  return new std::string(out.str());
}

Ast::String * makeString(std::string * str){
    return new Ast::String(str);
}

Ast::String * makeString(const Value & value){
    return new Ast::String(toString(value));
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
    delete str;
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
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_45;
    stream.update(result_peg_45.getPosition());
    
    
    return result_peg_45;
    out_peg_47:
    Result result_peg_48(myposition);
    
    {
    
        result_peg_48 = rule_s(stream, result_peg_48.getPosition());
            if (result_peg_48.error()){
                goto out_peg_50;
            }
        
        
        
        result_peg_48 = rule_valuelist(stream, result_peg_48.getPosition());
            if (result_peg_48.error()){
                goto out_peg_50;
            }
            data = result_peg_48.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                result_peg_48.setValue(value);
            }
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_48;
    stream.update(result_peg_48.getPosition());
    
    
    return result_peg_48;
    out_peg_50:
    Result result_peg_52(myposition);
    
    {
    
        result_peg_52 = rule_s(stream, result_peg_52.getPosition());
            if (result_peg_52.error()){
                goto out_peg_54;
            }
        
        
        
        result_peg_52 = rule_loopstart(stream, result_peg_52.getPosition());
            if (result_peg_52.error()){
                goto out_peg_54;
            }
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_52;
    stream.update(result_peg_52.getPosition());
    
    
    return result_peg_52;
    out_peg_54:
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_55 = stream.getColumn(position);
    if (column_peg_55.chunk0 != 0 && column_peg_55.chunk0->chunk_line_end.calculated()){
        return column_peg_55.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_56(myposition);
    
    result_peg_56.reset();
    do{
        Result result_peg_58(result_peg_56.getPosition());
        result_peg_58 = rule_newline(stream, result_peg_58.getPosition());
        if (result_peg_58.error()){
            goto loop_peg_57;
        }
        result_peg_56.addResult(result_peg_58);
    } while (true);
    loop_peg_57:
    if (result_peg_56.matches() == 0){
        goto out_peg_59;
    }
    
    if (column_peg_55.chunk0 == 0){
        column_peg_55.chunk0 = new Chunk0();
    }
    column_peg_55.chunk0->chunk_line_end = result_peg_56;
    stream.update(result_peg_56.getPosition());
    
    
    return result_peg_56;
    out_peg_59:
    Result result_peg_60(myposition);
    
    {
    
        Result result_peg_62(result_peg_60.getPosition());
            if ('\0' == stream.get(result_peg_62.getPosition())){
                result_peg_62.nextPosition();
                result_peg_62.setValue((void *) '\0');
            } else {
                goto out_peg_63;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_55.chunk0 == 0){
        column_peg_55.chunk0 = new Chunk0();
    }
    column_peg_55.chunk0->chunk_line_end = result_peg_60;
    stream.update(result_peg_60.getPosition());
    
    
    return result_peg_60;
    out_peg_63:
    
    if (column_peg_55.chunk0 == 0){
        column_peg_55.chunk0 = new Chunk0();
    }
    column_peg_55.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_64 = stream.getColumn(position);
    if (column_peg_64.chunk1 != 0 && column_peg_64.chunk1->chunk_newline.calculated()){
        return column_peg_64.chunk1->chunk_newline;
    }
    
    int myposition = position;
    
    
    Result result_peg_65(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_65.getPosition()))){
            result_peg_65.nextPosition();
        } else {
            goto out_peg_66;
        }
    }
    result_peg_65.setValue((void*) "\n");
    
    if (column_peg_64.chunk1 == 0){
        column_peg_64.chunk1 = new Chunk1();
    }
    column_peg_64.chunk1->chunk_newline = result_peg_65;
    stream.update(result_peg_65.getPosition());
    
    
    return result_peg_65;
    out_peg_66:
    Result result_peg_67(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_67.getPosition()))){
            result_peg_67.nextPosition();
        } else {
            goto out_peg_68;
        }
    }
    result_peg_67.setValue((void*) "\r");
    
    if (column_peg_64.chunk1 == 0){
        column_peg_64.chunk1 = new Chunk1();
    }
    column_peg_64.chunk1->chunk_newline = result_peg_67;
    stream.update(result_peg_67.getPosition());
    
    
    return result_peg_67;
    out_peg_68:
    
    if (column_peg_64.chunk1 == 0){
        column_peg_64.chunk1 = new Chunk1();
    }
    column_peg_64.chunk1->chunk_newline = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_69 = stream.getColumn(position);
    if (column_peg_69.chunk1 != 0 && column_peg_69.chunk1->chunk_loopstart.calculated()){
        return column_peg_69.chunk1->chunk_loopstart;
    }
    
    int myposition = position;
    
    
    Result result_peg_70(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_70.getPosition()))){
                    result_peg_70.nextPosition();
                } else {
                    goto out_peg_72;
                }
            }
            result_peg_70.setValue((void*) "loopstart");
        
        Result result_peg_71 = result_peg_70;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_71.getValues());
                result_peg_70.setValue(value);
            }
        
        
    }
    
    if (column_peg_69.chunk1 == 0){
        column_peg_69.chunk1 = new Chunk1();
    }
    column_peg_69.chunk1->chunk_loopstart = result_peg_70;
    stream.update(result_peg_70.getPosition());
    
    
    return result_peg_70;
    out_peg_72:
    
    if (column_peg_69.chunk1 == 0){
        column_peg_69.chunk1 = new Chunk1();
    }
    column_peg_69.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_73 = stream.getColumn(position);
    if (column_peg_73.chunk1 != 0 && column_peg_73.chunk1->chunk_whitespace.calculated()){
        return column_peg_73.chunk1->chunk_whitespace;
    }
    
    int myposition = position;
    
    
    Result result_peg_74(myposition);
    
    result_peg_74.reset();
    do{
        Result result_peg_76(result_peg_74.getPosition());
        result_peg_76 = rule_sw(stream, result_peg_76.getPosition());
        if (result_peg_76.error()){
            goto loop_peg_75;
        }
        result_peg_74.addResult(result_peg_76);
    } while (true);
    loop_peg_75:
    ;
    
    if (column_peg_73.chunk1 == 0){
        column_peg_73.chunk1 = new Chunk1();
    }
    column_peg_73.chunk1->chunk_whitespace = result_peg_74;
    stream.update(result_peg_74.getPosition());
    
    
    return result_peg_74;
    
    if (column_peg_73.chunk1 == 0){
        column_peg_73.chunk1 = new Chunk1();
    }
    column_peg_73.chunk1->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_77 = stream.getColumn(position);
    if (column_peg_77.chunk1 != 0 && column_peg_77.chunk1->chunk_sw.calculated()){
        return column_peg_77.chunk1->chunk_sw;
    }
    
    int myposition = position;
    
    
    Result result_peg_78(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_78.getPosition()))){
                result_peg_78.nextPosition();
            } else {
                goto out_peg_81;
            }
        }
        result_peg_78.setValue((void*) " ");
            
    }
    goto success_peg_79;
    out_peg_81:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_78.getPosition()))){
                result_peg_78.nextPosition();
            } else {
                goto out_peg_83;
            }
        }
        result_peg_78.setValue((void*) "\t");
            
    }
    goto success_peg_79;
    out_peg_83:
    {
        
        if ((unsigned char) stream.get(result_peg_78.getPosition()) == (unsigned char) 255){
            result_peg_78.nextPosition();
        } else {
            goto out_peg_85;
        }
        result_peg_78.setValue((void*) 255);
        
    }
    goto success_peg_79;
    out_peg_85:
    goto out_peg_86;
    success_peg_79:
    ;
    
    if (column_peg_77.chunk1 == 0){
        column_peg_77.chunk1 = new Chunk1();
    }
    column_peg_77.chunk1->chunk_sw = result_peg_78;
    stream.update(result_peg_78.getPosition());
    
    
    return result_peg_78;
    out_peg_86:
    Result result_peg_87(myposition);
    
    result_peg_87 = rule_comment(stream, result_peg_87.getPosition());
    if (result_peg_87.error()){
        goto out_peg_88;
    }
    
    if (column_peg_77.chunk1 == 0){
        column_peg_77.chunk1 = new Chunk1();
    }
    column_peg_77.chunk1->chunk_sw = result_peg_87;
    stream.update(result_peg_87.getPosition());
    
    
    return result_peg_87;
    out_peg_88:
    
    if (column_peg_77.chunk1 == 0){
        column_peg_77.chunk1 = new Chunk1();
    }
    column_peg_77.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_89 = stream.getColumn(position);
    if (column_peg_89.chunk1 != 0 && column_peg_89.chunk1->chunk_s.calculated()){
        return column_peg_89.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_90(myposition);
    
    result_peg_90.reset();
    do{
        Result result_peg_92(result_peg_90.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_92.getPosition()))){
                    result_peg_92.nextPosition();
                } else {
                    goto out_peg_95;
                }
            }
            result_peg_92.setValue((void*) " ");
                
        }
        goto success_peg_93;
        out_peg_95:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_92.getPosition()))){
                    result_peg_92.nextPosition();
                } else {
                    goto out_peg_97;
                }
            }
            result_peg_92.setValue((void*) "\t");
                
        }
        goto success_peg_93;
        out_peg_97:
        {
            
            if ((unsigned char) stream.get(result_peg_92.getPosition()) == (unsigned char) 255){
                result_peg_92.nextPosition();
            } else {
                goto out_peg_99;
            }
            result_peg_92.setValue((void*) 255);
            
        }
        goto success_peg_93;
        out_peg_99:
        goto loop_peg_91;
        success_peg_93:
        ;
        result_peg_90.addResult(result_peg_92);
    } while (true);
    loop_peg_91:
    ;
    
    if (column_peg_89.chunk1 == 0){
        column_peg_89.chunk1 = new Chunk1();
    }
    column_peg_89.chunk1->chunk_s = result_peg_90;
    stream.update(result_peg_90.getPosition());
    
    
    return result_peg_90;
    
    if (column_peg_89.chunk1 == 0){
        column_peg_89.chunk1 = new Chunk1();
    }
    column_peg_89.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_100 = stream.getColumn(position);
    if (column_peg_100.chunk2 != 0 && column_peg_100.chunk2->chunk_comment.calculated()){
        return column_peg_100.chunk2->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_101(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_101.getPosition()))){
                    result_peg_101.nextPosition();
                } else {
                    goto out_peg_103;
                }
            }
            result_peg_101.setValue((void*) ";");
        
        
        
        result_peg_101.reset();
            do{
                Result result_peg_105(result_peg_101.getPosition());
                {
                
                    Result result_peg_108(result_peg_105);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_108.getPosition()))){
                                result_peg_108.nextPosition();
                            } else {
                                goto not_peg_107;
                            }
                        }
                        result_peg_108.setValue((void*) "\n");
                        goto loop_peg_104;
                        not_peg_107:
                        result_peg_105.setValue((void*)0);
                    
                    
                    
                    char temp_peg_109 = stream.get(result_peg_105.getPosition());
                        if (temp_peg_109 != '\0'){
                            result_peg_105.setValue((void*) temp_peg_109);
                            result_peg_105.nextPosition();
                        } else {
                            goto loop_peg_104;
                        }
                    
                    
                }
                result_peg_101.addResult(result_peg_105);
            } while (true);
            loop_peg_104:
            ;
        
        
    }
    
    if (column_peg_100.chunk2 == 0){
        column_peg_100.chunk2 = new Chunk2();
    }
    column_peg_100.chunk2->chunk_comment = result_peg_101;
    stream.update(result_peg_101.getPosition());
    
    
    return result_peg_101;
    out_peg_103:
    Result result_peg_110(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_110.getPosition()))){
                    result_peg_110.nextPosition();
                } else {
                    goto out_peg_112;
                }
            }
            result_peg_110.setValue((void*) "=");
        
        
        
        result_peg_110.reset();
            do{
                Result result_peg_114(result_peg_110.getPosition());
                {
                
                    Result result_peg_117(result_peg_114);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_117.getPosition()))){
                                result_peg_117.nextPosition();
                            } else {
                                goto not_peg_116;
                            }
                        }
                        result_peg_117.setValue((void*) "\n");
                        goto loop_peg_113;
                        not_peg_116:
                        result_peg_114.setValue((void*)0);
                    
                    
                    
                    char temp_peg_118 = stream.get(result_peg_114.getPosition());
                        if (temp_peg_118 != '\0'){
                            result_peg_114.setValue((void*) temp_peg_118);
                            result_peg_114.nextPosition();
                        } else {
                            goto loop_peg_113;
                        }
                    
                    
                }
                result_peg_110.addResult(result_peg_114);
            } while (true);
            loop_peg_113:
            ;
        
        
    }
    
    if (column_peg_100.chunk2 == 0){
        column_peg_100.chunk2 = new Chunk2();
    }
    column_peg_100.chunk2->chunk_comment = result_peg_110;
    stream.update(result_peg_110.getPosition());
    
    
    return result_peg_110;
    out_peg_112:
    Result result_peg_119(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) "-");
        
        
        
        result_peg_119.reset();
            do{
                Result result_peg_123(result_peg_119.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_123.getPosition()))){
                        result_peg_123.nextPosition();
                    } else {
                        goto loop_peg_122;
                    }
                }
                result_peg_123.setValue((void*) "-");
                result_peg_119.addResult(result_peg_123);
            } while (true);
            loop_peg_122:
            if (result_peg_119.matches() == 0){
                goto out_peg_121;
            }
        
        
    }
    
    if (column_peg_100.chunk2 == 0){
        column_peg_100.chunk2 = new Chunk2();
    }
    column_peg_100.chunk2->chunk_comment = result_peg_119;
    stream.update(result_peg_119.getPosition());
    
    
    return result_peg_119;
    out_peg_121:
    
    if (column_peg_100.chunk2 == 0){
        column_peg_100.chunk2 = new Chunk2();
    }
    column_peg_100.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_124 = stream.getColumn(position);
    if (column_peg_124.chunk2 != 0 && column_peg_124.chunk2->chunk_collision_default.calculated()){
        return column_peg_124.chunk2->chunk_collision_default;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_125(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_125.getPosition()))){
                    result_peg_125.nextPosition();
                } else {
                    goto out_peg_127;
                }
            }
            result_peg_125.setValue((void*) "Clsn2Default:");
        
        Result result_peg_126 = result_peg_125;
        
        result_peg_125 = rule_s(stream, result_peg_125.getPosition());
            if (result_peg_125.error()){
                goto out_peg_127;
            }
        
        
        
        result_peg_125 = rule_integer(stream, result_peg_125.getPosition());
            if (result_peg_125.error()){
                goto out_peg_127;
            }
            num = result_peg_125.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_126.getValues()); value = makeAttributeKeyword(value, num);
                result_peg_125.setValue(value);
            }
        
        
    }
    
    if (column_peg_124.chunk2 == 0){
        column_peg_124.chunk2 = new Chunk2();
    }
    column_peg_124.chunk2->chunk_collision_default = result_peg_125;
    stream.update(result_peg_125.getPosition());
    
    
    return result_peg_125;
    out_peg_127:
    Result result_peg_130(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_130.getPosition()))){
                    result_peg_130.nextPosition();
                } else {
                    goto out_peg_132;
                }
            }
            result_peg_130.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_130 = rule_s(stream, result_peg_130.getPosition());
            if (result_peg_130.error()){
                goto out_peg_132;
            }
        
        
        
        result_peg_130 = rule_integer(stream, result_peg_130.getPosition());
            if (result_peg_130.error()){
                goto out_peg_132;
            }
            num = result_peg_130.getValues();
        
        
    }
    
    if (column_peg_124.chunk2 == 0){
        column_peg_124.chunk2 = new Chunk2();
    }
    column_peg_124.chunk2->chunk_collision_default = result_peg_130;
    stream.update(result_peg_130.getPosition());
    
    
    return result_peg_130;
    out_peg_132:
    Result result_peg_134(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) "Clsn2:");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_integer(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
            num = result_peg_134.getValues();
        
        
    }
    
    if (column_peg_124.chunk2 == 0){
        column_peg_124.chunk2 = new Chunk2();
    }
    column_peg_124.chunk2->chunk_collision_default = result_peg_134;
    stream.update(result_peg_134.getPosition());
    
    
    return result_peg_134;
    out_peg_136:
    Result result_peg_138(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_138.getPosition()))){
                    result_peg_138.nextPosition();
                } else {
                    goto out_peg_140;
                }
            }
            result_peg_138.setValue((void*) "Clsn1:");
        
        
        
        result_peg_138 = rule_s(stream, result_peg_138.getPosition());
            if (result_peg_138.error()){
                goto out_peg_140;
            }
        
        
        
        result_peg_138 = rule_integer(stream, result_peg_138.getPosition());
            if (result_peg_138.error()){
                goto out_peg_140;
            }
            num = result_peg_138.getValues();
        
        
    }
    
    if (column_peg_124.chunk2 == 0){
        column_peg_124.chunk2 = new Chunk2();
    }
    column_peg_124.chunk2->chunk_collision_default = result_peg_138;
    stream.update(result_peg_138.getPosition());
    
    
    return result_peg_138;
    out_peg_140:
    
    if (column_peg_124.chunk2 == 0){
        column_peg_124.chunk2 = new Chunk2();
    }
    column_peg_124.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_142 = stream.getColumn(position);
    if (column_peg_142.chunk2 != 0 && column_peg_142.chunk2->chunk_collision.calculated()){
        return column_peg_142.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    
    Result result_peg_143(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) "Clsn2");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) "[");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143.reset();
            do{
                Result result_peg_151(result_peg_143.getPosition());
                {
                    
                    char letter_peg_156 = stream.get(result_peg_151.getPosition());
                    if (letter_peg_156 != '\0' && strchr("0123456789", letter_peg_156) != NULL){
                        result_peg_151.nextPosition();
                        result_peg_151.setValue((void*) letter_peg_156);
                    } else {
                        goto out_peg_155;
                    }
                    
                }
                goto success_peg_152;
                out_peg_155:
                goto loop_peg_150;
                success_peg_152:
                ;
                result_peg_143.addResult(result_peg_151);
            } while (true);
            loop_peg_150:
            if (result_peg_143.matches() == 0){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) "]");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) "=");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_integer(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) ",");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_integer(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) ",");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_integer(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_143.getPosition()))){
                    result_peg_143.nextPosition();
                } else {
                    goto out_peg_145;
                }
            }
            result_peg_143.setValue((void*) ",");
        
        
        
        result_peg_143 = rule_s(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
        
        result_peg_143 = rule_integer(stream, result_peg_143.getPosition());
            if (result_peg_143.error()){
                goto out_peg_145;
            }
        
        
    }
    
    if (column_peg_142.chunk2 == 0){
        column_peg_142.chunk2 = new Chunk2();
    }
    column_peg_142.chunk2->chunk_collision = result_peg_143;
    stream.update(result_peg_143.getPosition());
    
    
    return result_peg_143;
    out_peg_145:
    Result result_peg_174(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) "Clsn1");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) "[");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174.reset();
            do{
                Result result_peg_182(result_peg_174.getPosition());
                {
                    
                    char letter_peg_187 = stream.get(result_peg_182.getPosition());
                    if (letter_peg_187 != '\0' && strchr("0123456789", letter_peg_187) != NULL){
                        result_peg_182.nextPosition();
                        result_peg_182.setValue((void*) letter_peg_187);
                    } else {
                        goto out_peg_186;
                    }
                    
                }
                goto success_peg_183;
                out_peg_186:
                goto loop_peg_181;
                success_peg_183:
                ;
                result_peg_174.addResult(result_peg_182);
            } while (true);
            loop_peg_181:
            if (result_peg_174.matches() == 0){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) "]");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) "=");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_integer(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) ",");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_integer(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) ",");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_integer(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_174.getPosition()))){
                    result_peg_174.nextPosition();
                } else {
                    goto out_peg_176;
                }
            }
            result_peg_174.setValue((void*) ",");
        
        
        
        result_peg_174 = rule_s(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
        
        result_peg_174 = rule_integer(stream, result_peg_174.getPosition());
            if (result_peg_174.error()){
                goto out_peg_176;
            }
        
        
    }
    
    if (column_peg_142.chunk2 == 0){
        column_peg_142.chunk2 = new Chunk2();
    }
    column_peg_142.chunk2->chunk_collision = result_peg_174;
    stream.update(result_peg_174.getPosition());
    
    
    return result_peg_174;
    out_peg_176:
    
    if (column_peg_142.chunk2 == 0){
        column_peg_142.chunk2 = new Chunk2();
    }
    column_peg_142.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_205 = stream.getColumn(position);
    if (column_peg_205.chunk2 != 0 && column_peg_205.chunk2->chunk_action_start.calculated()){
        return column_peg_205.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_206(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_206.getPosition()))){
                    result_peg_206.nextPosition();
                } else {
                    goto out_peg_208;
                }
            }
            result_peg_206.setValue((void*) "[");
        
        
        
        result_peg_206 = rule_s(stream, result_peg_206.getPosition());
            if (result_peg_206.error()){
                goto out_peg_208;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_206.getPosition()))){
                    result_peg_206.nextPosition();
                } else {
                    goto out_peg_208;
                }
            }
            result_peg_206.setValue((void*) "Begin");
            begin = result_peg_206.getValues();
        
        
        
        result_peg_206 = rule_s(stream, result_peg_206.getPosition());
            if (result_peg_206.error()){
                goto out_peg_208;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_206.getPosition()))){
                    result_peg_206.nextPosition();
                } else {
                    goto out_peg_208;
                }
            }
            result_peg_206.setValue((void*) "Action");
            action = result_peg_206.getValues();
        
        
        
        result_peg_206 = rule_s(stream, result_peg_206.getPosition());
            if (result_peg_206.error()){
                goto out_peg_208;
            }
        
        
        
        result_peg_206 = rule_integer(stream, result_peg_206.getPosition());
            if (result_peg_206.error()){
                goto out_peg_208;
            }
            num = result_peg_206.getValues();
        
        
        
        result_peg_206 = rule_s(stream, result_peg_206.getPosition());
            if (result_peg_206.error()){
                goto out_peg_208;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_206.getPosition()))){
                    result_peg_206.nextPosition();
                } else {
                    goto out_peg_208;
                }
            }
            result_peg_206.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num);
                result_peg_206.setValue(value);
            }
        
        
    }
    
    if (column_peg_205.chunk2 == 0){
        column_peg_205.chunk2 = new Chunk2();
    }
    column_peg_205.chunk2->chunk_action_start = result_peg_206;
    stream.update(result_peg_206.getPosition());
    
    
    return result_peg_206;
    out_peg_208:
    
    if (column_peg_205.chunk2 == 0){
        column_peg_205.chunk2 = new Chunk2();
    }
    column_peg_205.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_217 = stream.getColumn(position);
    if (column_peg_217.chunk2 != 0 && column_peg_217.chunk2->chunk_integer.calculated()){
        return column_peg_217.chunk2->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_218(myposition);
    
    {
    
        int save_peg_220 = result_peg_218.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_218.getPosition()))){
                        result_peg_218.nextPosition();
                    } else {
                        goto out_peg_223;
                    }
                }
                result_peg_218.setValue((void*) "-");
                    
            }
            goto success_peg_221;
            out_peg_223:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_218.getPosition()))){
                        result_peg_218.nextPosition();
                    } else {
                        goto out_peg_225;
                    }
                }
                result_peg_218.setValue((void*) "+");
                    
            }
            goto success_peg_221;
            out_peg_225:
            
            result_peg_218 = Result(save_peg_220);
            result_peg_218.setValue((void*) 0);
            
            success_peg_221:
            ;
        
        Result result_peg_219 = result_peg_218;
        
        result_peg_218.reset();
            do{
                Result result_peg_228(result_peg_218.getPosition());
                {
                    
                    char letter_peg_233 = stream.get(result_peg_228.getPosition());
                    if (letter_peg_233 != '\0' && strchr("0123456789", letter_peg_233) != NULL){
                        result_peg_228.nextPosition();
                        result_peg_228.setValue((void*) letter_peg_233);
                    } else {
                        goto out_peg_232;
                    }
                    
                }
                goto success_peg_229;
                out_peg_232:
                goto loop_peg_227;
                success_peg_229:
                ;
                result_peg_218.addResult(result_peg_228);
            } while (true);
            loop_peg_227:
            if (result_peg_218.matches() == 0){
                goto out_peg_234;
            }
        
        Result result_peg_226 = result_peg_218;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_226.getValues()); value = makeNumber(result_peg_219.getValues(),value);
                result_peg_218.setValue(value);
            }
        
        
    }
    
    if (column_peg_217.chunk2 == 0){
        column_peg_217.chunk2 = new Chunk2();
    }
    column_peg_217.chunk2->chunk_integer = result_peg_218;
    stream.update(result_peg_218.getPosition());
    
    
    return result_peg_218;
    out_peg_234:
    
    if (column_peg_217.chunk2 == 0){
        column_peg_217.chunk2 = new Chunk2();
    }
    column_peg_217.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_235 = stream.getColumn(position);
    if (column_peg_235.chunk3 != 0 && column_peg_235.chunk3->chunk_valuelist.calculated()){
        return column_peg_235.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_236(myposition);
    
    {
    
        result_peg_236 = rule_value(stream, result_peg_236.getPosition());
            if (result_peg_236.error()){
                goto out_peg_238;
            }
        
        Result result_peg_237 = result_peg_236;
        
        result_peg_236.reset();
            do{
                Result result_peg_241(result_peg_236.getPosition());
                {
                
                    result_peg_241 = rule_s(stream, result_peg_241.getPosition());
                        if (result_peg_241.error()){
                            goto loop_peg_240;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_241.getPosition()))){
                                result_peg_241.nextPosition();
                            } else {
                                goto loop_peg_240;
                            }
                        }
                        result_peg_241.setValue((void*) ",");
                    
                    
                    
                    result_peg_241 = rule_s(stream, result_peg_241.getPosition());
                        if (result_peg_241.error()){
                            goto loop_peg_240;
                        }
                    
                    
                    
                    int save_peg_245 = result_peg_241.getPosition();
                        
                        result_peg_241 = rule_value(stream, result_peg_241.getPosition());
                        if (result_peg_241.error()){
                            
                            result_peg_241 = Result(save_peg_245);
                            result_peg_241.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_236.addResult(result_peg_241);
            } while (true);
            loop_peg_240:
            ;
        
        Result result_peg_239 = result_peg_236;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_237.getValues(),result_peg_239.getValues());
                result_peg_236.setValue(value);
            }
        
        
    }
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_valuelist = result_peg_236;
    stream.update(result_peg_236.getPosition());
    
    
    return result_peg_236;
    out_peg_238:
    
    if (column_peg_235.chunk3 == 0){
        column_peg_235.chunk3 = new Chunk3();
    }
    column_peg_235.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_246 = stream.getColumn(position);
    if (column_peg_246.chunk3 != 0 && column_peg_246.chunk3->chunk_value.calculated()){
        return column_peg_246.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_247(myposition);
    
    result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
    if (result_peg_247.error()){
        goto out_peg_248;
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_247;
    stream.update(result_peg_247.getPosition());
    
    
    return result_peg_247;
    out_peg_248:
    Result result_peg_249(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_249.getPosition()))){
                    result_peg_249.nextPosition();
                } else {
                    goto out_peg_251;
                }
            }
            result_peg_249.setValue((void*) "as");
        
        Result result_peg_250 = result_peg_249;
        
        result_peg_249 = rule_integer(stream, result_peg_249.getPosition());
            if (result_peg_249.error()){
                goto out_peg_251;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_249.getPosition()))){
                    result_peg_249.nextPosition();
                } else {
                    goto out_peg_251;
                }
            }
            result_peg_249.setValue((void*) "d");
        
        
        
        result_peg_249 = rule_integer(stream, result_peg_249.getPosition());
            if (result_peg_249.error()){
                goto out_peg_251;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_250.getValues());
                result_peg_249.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_249;
    stream.update(result_peg_249.getPosition());
    
    
    return result_peg_249;
    out_peg_251:
    Result result_peg_255(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_255.getPosition()))){
                    result_peg_255.nextPosition();
                } else {
                    goto out_peg_257;
                }
            }
            result_peg_255.setValue((void*) "a");
        
        Result result_peg_256 = result_peg_255;
        
        result_peg_255 = rule_integer(stream, result_peg_255.getPosition());
            if (result_peg_255.error()){
                goto out_peg_257;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_256.getValues());
                result_peg_255.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_255;
    stream.update(result_peg_255.getPosition());
    
    
    return result_peg_255;
    out_peg_257:
    Result result_peg_259(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_259.getPosition()))){
                    result_peg_259.nextPosition();
                } else {
                    goto out_peg_261;
                }
            }
            result_peg_259.setValue((void*) "a");
        
        Result result_peg_260 = result_peg_259;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_260.getValues());
                result_peg_259.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_259;
    stream.update(result_peg_259.getPosition());
    
    
    return result_peg_259;
    out_peg_261:
    Result result_peg_262(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_262.getPosition()))){
                    result_peg_262.nextPosition();
                } else {
                    goto out_peg_264;
                }
            }
            result_peg_262.setValue((void*) "s");
        
        Result result_peg_263 = result_peg_262;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_263.getValues());
                result_peg_262.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_262;
    stream.update(result_peg_262.getPosition());
    
    
    return result_peg_262;
    out_peg_264:
    Result result_peg_265(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_265.getPosition()))){
                    result_peg_265.nextPosition();
                } else {
                    goto out_peg_267;
                }
            }
            result_peg_265.setValue((void*) "vh");
        
        Result result_peg_266 = result_peg_265;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_266.getValues());
                result_peg_265.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_265;
    stream.update(result_peg_265.getPosition());
    
    
    return result_peg_265;
    out_peg_267:
    Result result_peg_268(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_268.getPosition()))){
                    result_peg_268.nextPosition();
                } else {
                    goto out_peg_270;
                }
            }
            result_peg_268.setValue((void*) "hv");
        
        Result result_peg_269 = result_peg_268;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_269.getValues());
                result_peg_268.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_268;
    stream.update(result_peg_268.getPosition());
    
    
    return result_peg_268;
    out_peg_270:
    Result result_peg_271(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_271.getPosition()))){
                    result_peg_271.nextPosition();
                } else {
                    goto out_peg_273;
                }
            }
            result_peg_271.setValue((void*) "v");
        
        Result result_peg_272 = result_peg_271;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_272.getValues());
                result_peg_271.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_271;
    stream.update(result_peg_271.getPosition());
    
    
    return result_peg_271;
    out_peg_273:
    Result result_peg_274(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_274.getPosition()))){
                    result_peg_274.nextPosition();
                } else {
                    goto out_peg_276;
                }
            }
            result_peg_274.setValue((void*) "h");
        
        Result result_peg_275 = result_peg_274;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_275.getValues());
                result_peg_274.setValue(value);
            }
        
        
    }
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = result_peg_274;
    stream.update(result_peg_274.getPosition());
    
    
    return result_peg_274;
    out_peg_276:
    
    if (column_peg_246.chunk3 == 0){
        column_peg_246.chunk3 = new Chunk3();
    }
    column_peg_246.chunk3->chunk_value = errorResult;
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

        
