

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
    Result chunk_space;
    Result chunk_comment;
    Result chunk_collision_default;
    Result chunk_collision;
    Result chunk_action_start;
};

struct Chunk3{
    Result chunk_digit;
    Result chunk_integer;
    Result chunk_sign;
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
(chunk3 != NULL ? ((chunk3->chunk_digit.calculated() ? 1 : 0)
+ (chunk3->chunk_integer.calculated() ? 1 : 0)
+ (chunk3->chunk_sign.calculated() ? 1 : 0)
+ (chunk3->chunk_valuelist.calculated() ? 1 : 0)
+ (chunk3->chunk_value.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_space.calculated() ? 1 : 0)
+ (chunk2->chunk_comment.calculated() ? 1 : 0)
+ (chunk2->chunk_collision_default.calculated() ? 1 : 0)
+ (chunk2->chunk_collision.calculated() ? 1 : 0)
+ (chunk2->chunk_action_start.calculated() ? 1 : 0)) : 0)
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
        return 20;
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
Result rule_space(Stream &, const int);
Result rule_comment(Stream &, const int);
Result rule_collision_default(Stream &, const int);
Result rule_collision(Stream &, const int);
Result rule_action_start(Stream &, const int);
Result rule_digit(Stream &, const int);
Result rule_integer(Stream &, const int);
Result rule_sign(Stream &, const int);
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
  return new Ast::Section(as<std::string*>(str));
}

Ast::Keyword * makeKeyword(const Value & value){
    return new Ast::Keyword(as<char*>(value));
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

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0){
        value = -value;
    }

    return new Ast::Number(value);
}

double * parseDouble(const Value & value){
    std::string * str = toString(value);
    std::istringstream get(*str);
    double * number = new double;
    get >> *number;
    delete str;
    return number;
}

std::string * makeHeader(const Value & begin, const Value & action, const Value & num){
    std::ostringstream out;
    out << as<char*>(begin) << " " << as<char*>(action) << " " << (as<Ast::Value*>(num))->toString();
    return new std::string(out.str()); 
}

SectionList * makeSectionList(){
  return new SectionList();
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
    return new Ast::AttributeKeyword(as<Ast::Keyword*>(id), as<Ast::Value*>(data));
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
                value = makeSection(result_peg_26.getValues()); GC::save(as<Ast::Section*>(value));
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
                value = makeKeyword(result_peg_71.getValues()); GC::save(as<Ast::Keyword*>(value));
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
    
    result_peg_78 = rule_space(stream, result_peg_78.getPosition());
    if (result_peg_78.error()){
        goto out_peg_79;
    }
    
    if (column_peg_77.chunk1 == 0){
        column_peg_77.chunk1 = new Chunk1();
    }
    column_peg_77.chunk1->chunk_sw = result_peg_78;
    stream.update(result_peg_78.getPosition());
    
    
    return result_peg_78;
    out_peg_79:
    Result result_peg_80(myposition);
    
    result_peg_80 = rule_comment(stream, result_peg_80.getPosition());
    if (result_peg_80.error()){
        goto out_peg_81;
    }
    
    if (column_peg_77.chunk1 == 0){
        column_peg_77.chunk1 = new Chunk1();
    }
    column_peg_77.chunk1->chunk_sw = result_peg_80;
    stream.update(result_peg_80.getPosition());
    
    
    return result_peg_80;
    out_peg_81:
    
    if (column_peg_77.chunk1 == 0){
        column_peg_77.chunk1 = new Chunk1();
    }
    column_peg_77.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_82 = stream.getColumn(position);
    if (column_peg_82.chunk1 != 0 && column_peg_82.chunk1->chunk_s.calculated()){
        return column_peg_82.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_83(myposition);
    
    result_peg_83.reset();
    do{
        Result result_peg_85(result_peg_83.getPosition());
        result_peg_85 = rule_space(stream, result_peg_85.getPosition());
        if (result_peg_85.error()){
            goto loop_peg_84;
        }
        result_peg_83.addResult(result_peg_85);
    } while (true);
    loop_peg_84:
    ;
    
    if (column_peg_82.chunk1 == 0){
        column_peg_82.chunk1 = new Chunk1();
    }
    column_peg_82.chunk1->chunk_s = result_peg_83;
    stream.update(result_peg_83.getPosition());
    
    
    return result_peg_83;
    
    if (column_peg_82.chunk1 == 0){
        column_peg_82.chunk1 = new Chunk1();
    }
    column_peg_82.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_space(Stream & stream, const int position){
    
    Column & column_peg_86 = stream.getColumn(position);
    if (column_peg_86.chunk2 != 0 && column_peg_86.chunk2->chunk_space.calculated()){
        return column_peg_86.chunk2->chunk_space;
    }
    
    int myposition = position;
    
    
    Result result_peg_87(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_87.getPosition()))){
            result_peg_87.nextPosition();
        } else {
            goto out_peg_88;
        }
    }
    result_peg_87.setValue((void*) " ");
    
    if (column_peg_86.chunk2 == 0){
        column_peg_86.chunk2 = new Chunk2();
    }
    column_peg_86.chunk2->chunk_space = result_peg_87;
    stream.update(result_peg_87.getPosition());
    
    
    return result_peg_87;
    out_peg_88:
    Result result_peg_89(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_89.getPosition()))){
            result_peg_89.nextPosition();
        } else {
            goto out_peg_90;
        }
    }
    result_peg_89.setValue((void*) "\t");
    
    if (column_peg_86.chunk2 == 0){
        column_peg_86.chunk2 = new Chunk2();
    }
    column_peg_86.chunk2->chunk_space = result_peg_89;
    stream.update(result_peg_89.getPosition());
    
    
    return result_peg_89;
    out_peg_90:
    Result result_peg_91(myposition);
    
    if ((unsigned char) stream.get(result_peg_91.getPosition()) == (unsigned char) 255){
        result_peg_91.nextPosition();
    } else {
        goto out_peg_92;
    }
    result_peg_91.setValue((void*) 255);
    
    if (column_peg_86.chunk2 == 0){
        column_peg_86.chunk2 = new Chunk2();
    }
    column_peg_86.chunk2->chunk_space = result_peg_91;
    stream.update(result_peg_91.getPosition());
    
    
    return result_peg_91;
    out_peg_92:
    
    if (column_peg_86.chunk2 == 0){
        column_peg_86.chunk2 = new Chunk2();
    }
    column_peg_86.chunk2->chunk_space = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_93 = stream.getColumn(position);
    if (column_peg_93.chunk2 != 0 && column_peg_93.chunk2->chunk_comment.calculated()){
        return column_peg_93.chunk2->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_94(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_94.getPosition()))){
                    result_peg_94.nextPosition();
                } else {
                    goto out_peg_96;
                }
            }
            result_peg_94.setValue((void*) ";");
        
        
        
        result_peg_94.reset();
            do{
                Result result_peg_98(result_peg_94.getPosition());
                {
                
                    Result result_peg_101(result_peg_98);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_101.getPosition()))){
                                result_peg_101.nextPosition();
                            } else {
                                goto not_peg_100;
                            }
                        }
                        result_peg_101.setValue((void*) "\n");
                        goto loop_peg_97;
                        not_peg_100:
                        result_peg_98.setValue((void*)0);
                    
                    
                    
                    char temp_peg_102 = stream.get(result_peg_98.getPosition());
                        if (temp_peg_102 != '\0'){
                            result_peg_98.setValue((void*) temp_peg_102);
                            result_peg_98.nextPosition();
                        } else {
                            goto loop_peg_97;
                        }
                    
                    
                }
                result_peg_94.addResult(result_peg_98);
            } while (true);
            loop_peg_97:
            ;
        
        
    }
    
    if (column_peg_93.chunk2 == 0){
        column_peg_93.chunk2 = new Chunk2();
    }
    column_peg_93.chunk2->chunk_comment = result_peg_94;
    stream.update(result_peg_94.getPosition());
    
    
    return result_peg_94;
    out_peg_96:
    Result result_peg_103(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) "=");
        
        
        
        result_peg_103.reset();
            do{
                Result result_peg_107(result_peg_103.getPosition());
                {
                
                    Result result_peg_110(result_peg_107);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_110.getPosition()))){
                                result_peg_110.nextPosition();
                            } else {
                                goto not_peg_109;
                            }
                        }
                        result_peg_110.setValue((void*) "\n");
                        goto loop_peg_106;
                        not_peg_109:
                        result_peg_107.setValue((void*)0);
                    
                    
                    
                    char temp_peg_111 = stream.get(result_peg_107.getPosition());
                        if (temp_peg_111 != '\0'){
                            result_peg_107.setValue((void*) temp_peg_111);
                            result_peg_107.nextPosition();
                        } else {
                            goto loop_peg_106;
                        }
                    
                    
                }
                result_peg_103.addResult(result_peg_107);
            } while (true);
            loop_peg_106:
            ;
        
        
    }
    
    if (column_peg_93.chunk2 == 0){
        column_peg_93.chunk2 = new Chunk2();
    }
    column_peg_93.chunk2->chunk_comment = result_peg_103;
    stream.update(result_peg_103.getPosition());
    
    
    return result_peg_103;
    out_peg_105:
    Result result_peg_112(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_112.getPosition()))){
                    result_peg_112.nextPosition();
                } else {
                    goto out_peg_114;
                }
            }
            result_peg_112.setValue((void*) "-");
        
        
        
        result_peg_112.reset();
            do{
                Result result_peg_116(result_peg_112.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_116.getPosition()))){
                        result_peg_116.nextPosition();
                    } else {
                        goto loop_peg_115;
                    }
                }
                result_peg_116.setValue((void*) "-");
                result_peg_112.addResult(result_peg_116);
            } while (true);
            loop_peg_115:
            if (result_peg_112.matches() == 0){
                goto out_peg_114;
            }
        
        
    }
    
    if (column_peg_93.chunk2 == 0){
        column_peg_93.chunk2 = new Chunk2();
    }
    column_peg_93.chunk2->chunk_comment = result_peg_112;
    stream.update(result_peg_112.getPosition());
    
    
    return result_peg_112;
    out_peg_114:
    
    if (column_peg_93.chunk2 == 0){
        column_peg_93.chunk2 = new Chunk2();
    }
    column_peg_93.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_117 = stream.getColumn(position);
    if (column_peg_117.chunk2 != 0 && column_peg_117.chunk2->chunk_collision_default.calculated()){
        return column_peg_117.chunk2->chunk_collision_default;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_118(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "Clsn2Default:");
        
        Result result_peg_119 = result_peg_118;
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_integer(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
            num = result_peg_118.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_119.getValues()); GC::save(as<Ast::Keyword*>(value)); value = makeAttributeKeyword(value, num); GC::save(as<Ast::Attribute*>(value));
                result_peg_118.setValue(value);
            }
        
        
    }
    
    if (column_peg_117.chunk2 == 0){
        column_peg_117.chunk2 = new Chunk2();
    }
    column_peg_117.chunk2->chunk_collision_default = result_peg_118;
    stream.update(result_peg_118.getPosition());
    
    
    return result_peg_118;
    out_peg_120:
    Result result_peg_123(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_123.getPosition()))){
                    result_peg_123.nextPosition();
                } else {
                    goto out_peg_125;
                }
            }
            result_peg_123.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_123 = rule_s(stream, result_peg_123.getPosition());
            if (result_peg_123.error()){
                goto out_peg_125;
            }
        
        
        
        result_peg_123 = rule_integer(stream, result_peg_123.getPosition());
            if (result_peg_123.error()){
                goto out_peg_125;
            }
            num = result_peg_123.getValues();
        
        
    }
    
    if (column_peg_117.chunk2 == 0){
        column_peg_117.chunk2 = new Chunk2();
    }
    column_peg_117.chunk2->chunk_collision_default = result_peg_123;
    stream.update(result_peg_123.getPosition());
    
    
    return result_peg_123;
    out_peg_125:
    Result result_peg_127(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_127.getPosition()))){
                    result_peg_127.nextPosition();
                } else {
                    goto out_peg_129;
                }
            }
            result_peg_127.setValue((void*) "Clsn2:");
        
        
        
        result_peg_127 = rule_s(stream, result_peg_127.getPosition());
            if (result_peg_127.error()){
                goto out_peg_129;
            }
        
        
        
        result_peg_127 = rule_integer(stream, result_peg_127.getPosition());
            if (result_peg_127.error()){
                goto out_peg_129;
            }
            num = result_peg_127.getValues();
        
        
    }
    
    if (column_peg_117.chunk2 == 0){
        column_peg_117.chunk2 = new Chunk2();
    }
    column_peg_117.chunk2->chunk_collision_default = result_peg_127;
    stream.update(result_peg_127.getPosition());
    
    
    return result_peg_127;
    out_peg_129:
    Result result_peg_131(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_131.getPosition()))){
                    result_peg_131.nextPosition();
                } else {
                    goto out_peg_133;
                }
            }
            result_peg_131.setValue((void*) "Clsn1:");
        
        
        
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
    
    if (column_peg_117.chunk2 == 0){
        column_peg_117.chunk2 = new Chunk2();
    }
    column_peg_117.chunk2->chunk_collision_default = result_peg_131;
    stream.update(result_peg_131.getPosition());
    
    
    return result_peg_131;
    out_peg_133:
    
    if (column_peg_117.chunk2 == 0){
        column_peg_117.chunk2 = new Chunk2();
    }
    column_peg_117.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_135 = stream.getColumn(position);
    if (column_peg_135.chunk2 != 0 && column_peg_135.chunk2->chunk_collision.calculated()){
        return column_peg_135.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    
    Result result_peg_136(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) "Clsn2");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) "[");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136.reset();
            do{
                Result result_peg_144(result_peg_136.getPosition());
                result_peg_144 = rule_digit(stream, result_peg_144.getPosition());
                if (result_peg_144.error()){
                    goto loop_peg_143;
                }
                result_peg_136.addResult(result_peg_144);
            } while (true);
            loop_peg_143:
            if (result_peg_136.matches() == 0){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) "]");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) "=");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_integer(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) ",");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_integer(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) ",");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_integer(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_136.getPosition()))){
                    result_peg_136.nextPosition();
                } else {
                    goto out_peg_138;
                }
            }
            result_peg_136.setValue((void*) ",");
        
        
        
        result_peg_136 = rule_s(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
        
        result_peg_136 = rule_integer(stream, result_peg_136.getPosition());
            if (result_peg_136.error()){
                goto out_peg_138;
            }
        
        
    }
    
    if (column_peg_135.chunk2 == 0){
        column_peg_135.chunk2 = new Chunk2();
    }
    column_peg_135.chunk2->chunk_collision = result_peg_136;
    stream.update(result_peg_136.getPosition());
    
    
    return result_peg_136;
    out_peg_138:
    Result result_peg_162(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) "Clsn1");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) "[");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162.reset();
            do{
                Result result_peg_170(result_peg_162.getPosition());
                result_peg_170 = rule_digit(stream, result_peg_170.getPosition());
                if (result_peg_170.error()){
                    goto loop_peg_169;
                }
                result_peg_162.addResult(result_peg_170);
            } while (true);
            loop_peg_169:
            if (result_peg_162.matches() == 0){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) "]");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) "=");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_integer(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) ",");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_integer(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) ",");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_integer(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_162.getPosition()))){
                    result_peg_162.nextPosition();
                } else {
                    goto out_peg_164;
                }
            }
            result_peg_162.setValue((void*) ",");
        
        
        
        result_peg_162 = rule_s(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
        
        result_peg_162 = rule_integer(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_164;
            }
        
        
    }
    
    if (column_peg_135.chunk2 == 0){
        column_peg_135.chunk2 = new Chunk2();
    }
    column_peg_135.chunk2->chunk_collision = result_peg_162;
    stream.update(result_peg_162.getPosition());
    
    
    return result_peg_162;
    out_peg_164:
    
    if (column_peg_135.chunk2 == 0){
        column_peg_135.chunk2 = new Chunk2();
    }
    column_peg_135.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_188 = stream.getColumn(position);
    if (column_peg_188.chunk2 != 0 && column_peg_188.chunk2->chunk_action_start.calculated()){
        return column_peg_188.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_189(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_189.getPosition()))){
                    result_peg_189.nextPosition();
                } else {
                    goto out_peg_191;
                }
            }
            result_peg_189.setValue((void*) "[");
        
        
        
        result_peg_189 = rule_s(stream, result_peg_189.getPosition());
            if (result_peg_189.error()){
                goto out_peg_191;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_189.getPosition()))){
                    result_peg_189.nextPosition();
                } else {
                    goto out_peg_191;
                }
            }
            result_peg_189.setValue((void*) "Begin");
            begin = result_peg_189.getValues();
        
        
        
        result_peg_189 = rule_s(stream, result_peg_189.getPosition());
            if (result_peg_189.error()){
                goto out_peg_191;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_189.getPosition()))){
                    result_peg_189.nextPosition();
                } else {
                    goto out_peg_191;
                }
            }
            result_peg_189.setValue((void*) "Action");
            action = result_peg_189.getValues();
        
        
        
        result_peg_189 = rule_s(stream, result_peg_189.getPosition());
            if (result_peg_189.error()){
                goto out_peg_191;
            }
        
        
        
        result_peg_189 = rule_integer(stream, result_peg_189.getPosition());
            if (result_peg_189.error()){
                goto out_peg_191;
            }
            num = result_peg_189.getValues();
        
        
        
        result_peg_189 = rule_s(stream, result_peg_189.getPosition());
            if (result_peg_189.error()){
                goto out_peg_191;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_189.getPosition()))){
                    result_peg_189.nextPosition();
                } else {
                    goto out_peg_191;
                }
            }
            result_peg_189.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num); GC::save(as<std::string*>(value));
                result_peg_189.setValue(value);
            }
        
        
    }
    
    if (column_peg_188.chunk2 == 0){
        column_peg_188.chunk2 = new Chunk2();
    }
    column_peg_188.chunk2->chunk_action_start = result_peg_189;
    stream.update(result_peg_189.getPosition());
    
    
    return result_peg_189;
    out_peg_191:
    
    if (column_peg_188.chunk2 == 0){
        column_peg_188.chunk2 = new Chunk2();
    }
    column_peg_188.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_digit(Stream & stream, const int position){
    
    Column & column_peg_200 = stream.getColumn(position);
    if (column_peg_200.chunk3 != 0 && column_peg_200.chunk3->chunk_digit.calculated()){
        return column_peg_200.chunk3->chunk_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_201(myposition);
    
    char letter_peg_202 = stream.get(result_peg_201.getPosition());
    if (letter_peg_202 != '\0' && strchr("0123456789", letter_peg_202) != NULL){
        result_peg_201.nextPosition();
        result_peg_201.setValue((void*) letter_peg_202);
    } else {
        goto out_peg_203;
    }
    
    if (column_peg_200.chunk3 == 0){
        column_peg_200.chunk3 = new Chunk3();
    }
    column_peg_200.chunk3->chunk_digit = result_peg_201;
    stream.update(result_peg_201.getPosition());
    
    
    return result_peg_201;
    out_peg_203:
    
    if (column_peg_200.chunk3 == 0){
        column_peg_200.chunk3 = new Chunk3();
    }
    column_peg_200.chunk3->chunk_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_204 = stream.getColumn(position);
    if (column_peg_204.chunk3 != 0 && column_peg_204.chunk3->chunk_integer.calculated()){
        return column_peg_204.chunk3->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_205(myposition);
    
    {
    
        int save_peg_207 = result_peg_205.getPosition();
            
            result_peg_205 = rule_sign(stream, result_peg_205.getPosition());
            if (result_peg_205.error()){
                
                result_peg_205 = Result(save_peg_207);
                result_peg_205.setValue((void*) 0);
                
            }
        
        Result result_peg_206 = result_peg_205;
        
        result_peg_205.reset();
            do{
                Result result_peg_210(result_peg_205.getPosition());
                result_peg_210 = rule_digit(stream, result_peg_210.getPosition());
                if (result_peg_210.error()){
                    goto loop_peg_209;
                }
                result_peg_205.addResult(result_peg_210);
            } while (true);
            loop_peg_209:
            if (result_peg_205.matches() == 0){
                goto out_peg_211;
            }
        
        Result result_peg_208 = result_peg_205;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_208.getValues()); GC::save(as<double*>(value)); value = makeNumber(result_peg_206.getValues(),value); GC::save(as<Ast::Number*>(value));
                result_peg_205.setValue(value);
            }
        
        
    }
    
    if (column_peg_204.chunk3 == 0){
        column_peg_204.chunk3 = new Chunk3();
    }
    column_peg_204.chunk3->chunk_integer = result_peg_205;
    stream.update(result_peg_205.getPosition());
    
    
    return result_peg_205;
    out_peg_211:
    
    if (column_peg_204.chunk3 == 0){
        column_peg_204.chunk3 = new Chunk3();
    }
    column_peg_204.chunk3->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sign(Stream & stream, const int position){
    
    Column & column_peg_212 = stream.getColumn(position);
    if (column_peg_212.chunk3 != 0 && column_peg_212.chunk3->chunk_sign.calculated()){
        return column_peg_212.chunk3->chunk_sign;
    }
    
    int myposition = position;
    
    
    Result result_peg_213(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_213.getPosition()))){
            result_peg_213.nextPosition();
        } else {
            goto out_peg_214;
        }
    }
    result_peg_213.setValue((void*) "-");
    
    if (column_peg_212.chunk3 == 0){
        column_peg_212.chunk3 = new Chunk3();
    }
    column_peg_212.chunk3->chunk_sign = result_peg_213;
    stream.update(result_peg_213.getPosition());
    
    
    return result_peg_213;
    out_peg_214:
    Result result_peg_215(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_215.getPosition()))){
            result_peg_215.nextPosition();
        } else {
            goto out_peg_216;
        }
    }
    result_peg_215.setValue((void*) "+");
    
    if (column_peg_212.chunk3 == 0){
        column_peg_212.chunk3 = new Chunk3();
    }
    column_peg_212.chunk3->chunk_sign = result_peg_215;
    stream.update(result_peg_215.getPosition());
    
    
    return result_peg_215;
    out_peg_216:
    
    if (column_peg_212.chunk3 == 0){
        column_peg_212.chunk3 = new Chunk3();
    }
    column_peg_212.chunk3->chunk_sign = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_217 = stream.getColumn(position);
    if (column_peg_217.chunk3 != 0 && column_peg_217.chunk3->chunk_valuelist.calculated()){
        return column_peg_217.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_218(myposition);
    
    {
    
        result_peg_218 = rule_value(stream, result_peg_218.getPosition());
            if (result_peg_218.error()){
                goto out_peg_220;
            }
        
        Result result_peg_219 = result_peg_218;
        
        result_peg_218.reset();
            do{
                Result result_peg_223(result_peg_218.getPosition());
                {
                
                    result_peg_223 = rule_s(stream, result_peg_223.getPosition());
                        if (result_peg_223.error()){
                            goto loop_peg_222;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_223.getPosition()))){
                                result_peg_223.nextPosition();
                            } else {
                                goto loop_peg_222;
                            }
                        }
                        result_peg_223.setValue((void*) ",");
                    
                    
                    
                    result_peg_223 = rule_s(stream, result_peg_223.getPosition());
                        if (result_peg_223.error()){
                            goto loop_peg_222;
                        }
                    
                    
                    
                    int save_peg_227 = result_peg_223.getPosition();
                        
                        result_peg_223 = rule_value(stream, result_peg_223.getPosition());
                        if (result_peg_223.error()){
                            
                            result_peg_223 = Result(save_peg_227);
                            result_peg_223.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_218.addResult(result_peg_223);
            } while (true);
            loop_peg_222:
            ;
        
        Result result_peg_221 = result_peg_218;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_219.getValues(),result_peg_221.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_218.setValue(value);
            }
        
        
    }
    
    if (column_peg_217.chunk3 == 0){
        column_peg_217.chunk3 = new Chunk3();
    }
    column_peg_217.chunk3->chunk_valuelist = result_peg_218;
    stream.update(result_peg_218.getPosition());
    
    
    return result_peg_218;
    out_peg_220:
    
    if (column_peg_217.chunk3 == 0){
        column_peg_217.chunk3 = new Chunk3();
    }
    column_peg_217.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_228 = stream.getColumn(position);
    if (column_peg_228.chunk3 != 0 && column_peg_228.chunk3->chunk_value.calculated()){
        return column_peg_228.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_229(myposition);
    
    result_peg_229 = rule_integer(stream, result_peg_229.getPosition());
    if (result_peg_229.error()){
        goto out_peg_230;
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_229;
    stream.update(result_peg_229.getPosition());
    
    
    return result_peg_229;
    out_peg_230:
    Result result_peg_231(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_231.getPosition()))){
                    result_peg_231.nextPosition();
                } else {
                    goto out_peg_233;
                }
            }
            result_peg_231.setValue((void*) "as");
        
        Result result_peg_232 = result_peg_231;
        
        result_peg_231 = rule_integer(stream, result_peg_231.getPosition());
            if (result_peg_231.error()){
                goto out_peg_233;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_231.getPosition()))){
                    result_peg_231.nextPosition();
                } else {
                    goto out_peg_233;
                }
            }
            result_peg_231.setValue((void*) "d");
        
        
        
        result_peg_231 = rule_integer(stream, result_peg_231.getPosition());
            if (result_peg_231.error()){
                goto out_peg_233;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_232.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_231.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_231;
    stream.update(result_peg_231.getPosition());
    
    
    return result_peg_231;
    out_peg_233:
    Result result_peg_237(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_237.getPosition()))){
                    result_peg_237.nextPosition();
                } else {
                    goto out_peg_239;
                }
            }
            result_peg_237.setValue((void*) "a");
        
        Result result_peg_238 = result_peg_237;
        
        result_peg_237 = rule_integer(stream, result_peg_237.getPosition());
            if (result_peg_237.error()){
                goto out_peg_239;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_238.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_237.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_237;
    stream.update(result_peg_237.getPosition());
    
    
    return result_peg_237;
    out_peg_239:
    Result result_peg_241(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_241.getPosition()))){
                    result_peg_241.nextPosition();
                } else {
                    goto out_peg_243;
                }
            }
            result_peg_241.setValue((void*) "a");
        
        Result result_peg_242 = result_peg_241;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_242.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_241.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_241;
    stream.update(result_peg_241.getPosition());
    
    
    return result_peg_241;
    out_peg_243:
    Result result_peg_244(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_244.getPosition()))){
                    result_peg_244.nextPosition();
                } else {
                    goto out_peg_246;
                }
            }
            result_peg_244.setValue((void*) "s");
        
        Result result_peg_245 = result_peg_244;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_245.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_244.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_244;
    stream.update(result_peg_244.getPosition());
    
    
    return result_peg_244;
    out_peg_246:
    Result result_peg_247(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_247.getPosition()))){
                    result_peg_247.nextPosition();
                } else {
                    goto out_peg_249;
                }
            }
            result_peg_247.setValue((void*) "vh");
        
        Result result_peg_248 = result_peg_247;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_248.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_247.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_247;
    stream.update(result_peg_247.getPosition());
    
    
    return result_peg_247;
    out_peg_249:
    Result result_peg_250(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_250.getPosition()))){
                    result_peg_250.nextPosition();
                } else {
                    goto out_peg_252;
                }
            }
            result_peg_250.setValue((void*) "hv");
        
        Result result_peg_251 = result_peg_250;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_251.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_250.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_250;
    stream.update(result_peg_250.getPosition());
    
    
    return result_peg_250;
    out_peg_252:
    Result result_peg_253(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) "v");
        
        Result result_peg_254 = result_peg_253;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_254.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_253.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_253;
    stream.update(result_peg_253.getPosition());
    
    
    return result_peg_253;
    out_peg_255:
    Result result_peg_256(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_256.getPosition()))){
                    result_peg_256.nextPosition();
                } else {
                    goto out_peg_258;
                }
            }
            result_peg_256.setValue((void*) "h");
        
        Result result_peg_257 = result_peg_256;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_257.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_256.setValue(value);
            }
        
        
    }
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = result_peg_256;
    stream.update(result_peg_256.getPosition());
    
    
    return result_peg_256;
    out_peg_258:
    
    if (column_peg_228.chunk3 == 0){
        column_peg_228.chunk3 = new Chunk3();
    }
    column_peg_228.chunk3->chunk_value = errorResult;
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

        
