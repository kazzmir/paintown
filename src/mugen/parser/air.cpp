

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
Result rule_action_line(Stream &, const int);
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
    std::string * temp = toString(begin);
    out << *temp;
    delete temp;
    temp = toString(action);
    out << " " << *temp;
    delete temp;
    out << (as<Ast::Value*>(num))->toString();
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
                
                    result_peg_35 = rule_action_line(stream, result_peg_35.getPosition());
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
        

Result rule_action_line(Stream & stream, const int position){
    
    Column & column_peg_38 = stream.getColumn(position);
    if (column_peg_38.chunk0 != 0 && column_peg_38.chunk0->chunk_action_line.calculated()){
        return column_peg_38.chunk0->chunk_action_line;
    }
    
    int myposition = position;
    
    
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
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_48;
    stream.update(result_peg_48.getPosition());
    
    
    return result_peg_48;
    out_peg_50:
    Result result_peg_51(myposition);
    
    {
    
        result_peg_51 = rule_s(stream, result_peg_51.getPosition());
            if (result_peg_51.error()){
                goto out_peg_53;
            }
        
        
        
        result_peg_51 = rule_loopstart(stream, result_peg_51.getPosition());
            if (result_peg_51.error()){
                goto out_peg_53;
            }
        
        
    }
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = result_peg_51;
    stream.update(result_peg_51.getPosition());
    
    
    return result_peg_51;
    out_peg_53:
    
    if (column_peg_38.chunk0 == 0){
        column_peg_38.chunk0 = new Chunk0();
    }
    column_peg_38.chunk0->chunk_action_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_54 = stream.getColumn(position);
    if (column_peg_54.chunk0 != 0 && column_peg_54.chunk0->chunk_line_end.calculated()){
        return column_peg_54.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_55(myposition);
    
    result_peg_55.reset();
    do{
        Result result_peg_57(result_peg_55.getPosition());
        result_peg_57 = rule_newline(stream, result_peg_57.getPosition());
        if (result_peg_57.error()){
            goto loop_peg_56;
        }
        result_peg_55.addResult(result_peg_57);
    } while (true);
    loop_peg_56:
    if (result_peg_55.matches() == 0){
        goto out_peg_58;
    }
    
    if (column_peg_54.chunk0 == 0){
        column_peg_54.chunk0 = new Chunk0();
    }
    column_peg_54.chunk0->chunk_line_end = result_peg_55;
    stream.update(result_peg_55.getPosition());
    
    
    return result_peg_55;
    out_peg_58:
    Result result_peg_59(myposition);
    
    {
    
        Result result_peg_61(result_peg_59.getPosition());
            if ('\0' == stream.get(result_peg_61.getPosition())){
                result_peg_61.nextPosition();
                result_peg_61.setValue((void *) '\0');
            } else {
                goto out_peg_62;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_54.chunk0 == 0){
        column_peg_54.chunk0 = new Chunk0();
    }
    column_peg_54.chunk0->chunk_line_end = result_peg_59;
    stream.update(result_peg_59.getPosition());
    
    
    return result_peg_59;
    out_peg_62:
    
    if (column_peg_54.chunk0 == 0){
        column_peg_54.chunk0 = new Chunk0();
    }
    column_peg_54.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_63 = stream.getColumn(position);
    if (column_peg_63.chunk1 != 0 && column_peg_63.chunk1->chunk_newline.calculated()){
        return column_peg_63.chunk1->chunk_newline;
    }
    
    int myposition = position;
    
    
    Result result_peg_64(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_64.getPosition()))){
            result_peg_64.nextPosition();
        } else {
            goto out_peg_65;
        }
    }
    result_peg_64.setValue((void*) "\n");
    
    if (column_peg_63.chunk1 == 0){
        column_peg_63.chunk1 = new Chunk1();
    }
    column_peg_63.chunk1->chunk_newline = result_peg_64;
    stream.update(result_peg_64.getPosition());
    
    
    return result_peg_64;
    out_peg_65:
    Result result_peg_66(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_66.getPosition()))){
            result_peg_66.nextPosition();
        } else {
            goto out_peg_67;
        }
    }
    result_peg_66.setValue((void*) "\r");
    
    if (column_peg_63.chunk1 == 0){
        column_peg_63.chunk1 = new Chunk1();
    }
    column_peg_63.chunk1->chunk_newline = result_peg_66;
    stream.update(result_peg_66.getPosition());
    
    
    return result_peg_66;
    out_peg_67:
    
    if (column_peg_63.chunk1 == 0){
        column_peg_63.chunk1 = new Chunk1();
    }
    column_peg_63.chunk1->chunk_newline = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_68 = stream.getColumn(position);
    if (column_peg_68.chunk1 != 0 && column_peg_68.chunk1->chunk_loopstart.calculated()){
        return column_peg_68.chunk1->chunk_loopstart;
    }
    
    int myposition = position;
    
    
    Result result_peg_69(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_69.getPosition()))){
                    result_peg_69.nextPosition();
                } else {
                    goto out_peg_71;
                }
            }
            result_peg_69.setValue((void*) "loopstart");
        
        Result result_peg_70 = result_peg_69;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_70.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_69.setValue(value);
            }
        
        
    }
    
    if (column_peg_68.chunk1 == 0){
        column_peg_68.chunk1 = new Chunk1();
    }
    column_peg_68.chunk1->chunk_loopstart = result_peg_69;
    stream.update(result_peg_69.getPosition());
    
    
    return result_peg_69;
    out_peg_71:
    
    if (column_peg_68.chunk1 == 0){
        column_peg_68.chunk1 = new Chunk1();
    }
    column_peg_68.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_72 = stream.getColumn(position);
    if (column_peg_72.chunk1 != 0 && column_peg_72.chunk1->chunk_whitespace.calculated()){
        return column_peg_72.chunk1->chunk_whitespace;
    }
    
    int myposition = position;
    
    
    Result result_peg_73(myposition);
    
    result_peg_73.reset();
    do{
        Result result_peg_75(result_peg_73.getPosition());
        result_peg_75 = rule_sw(stream, result_peg_75.getPosition());
        if (result_peg_75.error()){
            goto loop_peg_74;
        }
        result_peg_73.addResult(result_peg_75);
    } while (true);
    loop_peg_74:
    ;
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_whitespace = result_peg_73;
    stream.update(result_peg_73.getPosition());
    
    
    return result_peg_73;
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_76 = stream.getColumn(position);
    if (column_peg_76.chunk1 != 0 && column_peg_76.chunk1->chunk_sw.calculated()){
        return column_peg_76.chunk1->chunk_sw;
    }
    
    int myposition = position;
    
    
    Result result_peg_77(myposition);
    
    result_peg_77 = rule_space(stream, result_peg_77.getPosition());
    if (result_peg_77.error()){
        goto out_peg_78;
    }
    
    if (column_peg_76.chunk1 == 0){
        column_peg_76.chunk1 = new Chunk1();
    }
    column_peg_76.chunk1->chunk_sw = result_peg_77;
    stream.update(result_peg_77.getPosition());
    
    
    return result_peg_77;
    out_peg_78:
    Result result_peg_79(myposition);
    
    result_peg_79 = rule_comment(stream, result_peg_79.getPosition());
    if (result_peg_79.error()){
        goto out_peg_80;
    }
    
    if (column_peg_76.chunk1 == 0){
        column_peg_76.chunk1 = new Chunk1();
    }
    column_peg_76.chunk1->chunk_sw = result_peg_79;
    stream.update(result_peg_79.getPosition());
    
    
    return result_peg_79;
    out_peg_80:
    
    if (column_peg_76.chunk1 == 0){
        column_peg_76.chunk1 = new Chunk1();
    }
    column_peg_76.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_81 = stream.getColumn(position);
    if (column_peg_81.chunk1 != 0 && column_peg_81.chunk1->chunk_s.calculated()){
        return column_peg_81.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_82(myposition);
    
    result_peg_82.reset();
    do{
        Result result_peg_84(result_peg_82.getPosition());
        result_peg_84 = rule_space(stream, result_peg_84.getPosition());
        if (result_peg_84.error()){
            goto loop_peg_83;
        }
        result_peg_82.addResult(result_peg_84);
    } while (true);
    loop_peg_83:
    ;
    
    if (column_peg_81.chunk1 == 0){
        column_peg_81.chunk1 = new Chunk1();
    }
    column_peg_81.chunk1->chunk_s = result_peg_82;
    stream.update(result_peg_82.getPosition());
    
    
    return result_peg_82;
    
    if (column_peg_81.chunk1 == 0){
        column_peg_81.chunk1 = new Chunk1();
    }
    column_peg_81.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_space(Stream & stream, const int position){
    
    Column & column_peg_85 = stream.getColumn(position);
    if (column_peg_85.chunk2 != 0 && column_peg_85.chunk2->chunk_space.calculated()){
        return column_peg_85.chunk2->chunk_space;
    }
    
    int myposition = position;
    
    
    Result result_peg_86(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_86.getPosition()))){
            result_peg_86.nextPosition();
        } else {
            goto out_peg_87;
        }
    }
    result_peg_86.setValue((void*) " ");
    
    if (column_peg_85.chunk2 == 0){
        column_peg_85.chunk2 = new Chunk2();
    }
    column_peg_85.chunk2->chunk_space = result_peg_86;
    stream.update(result_peg_86.getPosition());
    
    
    return result_peg_86;
    out_peg_87:
    Result result_peg_88(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_88.getPosition()))){
            result_peg_88.nextPosition();
        } else {
            goto out_peg_89;
        }
    }
    result_peg_88.setValue((void*) "\t");
    
    if (column_peg_85.chunk2 == 0){
        column_peg_85.chunk2 = new Chunk2();
    }
    column_peg_85.chunk2->chunk_space = result_peg_88;
    stream.update(result_peg_88.getPosition());
    
    
    return result_peg_88;
    out_peg_89:
    Result result_peg_90(myposition);
    
    if ((unsigned char) stream.get(result_peg_90.getPosition()) == (unsigned char) 255){
        result_peg_90.nextPosition();
    } else {
        goto out_peg_91;
    }
    result_peg_90.setValue((void*) 255);
    
    if (column_peg_85.chunk2 == 0){
        column_peg_85.chunk2 = new Chunk2();
    }
    column_peg_85.chunk2->chunk_space = result_peg_90;
    stream.update(result_peg_90.getPosition());
    
    
    return result_peg_90;
    out_peg_91:
    
    if (column_peg_85.chunk2 == 0){
        column_peg_85.chunk2 = new Chunk2();
    }
    column_peg_85.chunk2->chunk_space = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_92 = stream.getColumn(position);
    if (column_peg_92.chunk2 != 0 && column_peg_92.chunk2->chunk_comment.calculated()){
        return column_peg_92.chunk2->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_93(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_93.getPosition()))){
                    result_peg_93.nextPosition();
                } else {
                    goto out_peg_95;
                }
            }
            result_peg_93.setValue((void*) ";");
        
        
        
        result_peg_93.reset();
            do{
                Result result_peg_97(result_peg_93.getPosition());
                {
                
                    Result result_peg_100(result_peg_97);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_100.getPosition()))){
                                result_peg_100.nextPosition();
                            } else {
                                goto not_peg_99;
                            }
                        }
                        result_peg_100.setValue((void*) "\n");
                        goto loop_peg_96;
                        not_peg_99:
                        result_peg_97.setValue((void*)0);
                    
                    
                    
                    char temp_peg_101 = stream.get(result_peg_97.getPosition());
                        if (temp_peg_101 != '\0'){
                            result_peg_97.setValue((void*) temp_peg_101);
                            result_peg_97.nextPosition();
                        } else {
                            goto loop_peg_96;
                        }
                    
                    
                }
                result_peg_93.addResult(result_peg_97);
            } while (true);
            loop_peg_96:
            ;
        
        
    }
    
    if (column_peg_92.chunk2 == 0){
        column_peg_92.chunk2 = new Chunk2();
    }
    column_peg_92.chunk2->chunk_comment = result_peg_93;
    stream.update(result_peg_93.getPosition());
    
    
    return result_peg_93;
    out_peg_95:
    Result result_peg_102(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_102.getPosition()))){
                    result_peg_102.nextPosition();
                } else {
                    goto out_peg_104;
                }
            }
            result_peg_102.setValue((void*) "=");
        
        
        
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
    
    if (column_peg_92.chunk2 == 0){
        column_peg_92.chunk2 = new Chunk2();
    }
    column_peg_92.chunk2->chunk_comment = result_peg_102;
    stream.update(result_peg_102.getPosition());
    
    
    return result_peg_102;
    out_peg_104:
    Result result_peg_111(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_111.getPosition()))){
                    result_peg_111.nextPosition();
                } else {
                    goto out_peg_113;
                }
            }
            result_peg_111.setValue((void*) "-");
        
        
        
        result_peg_111.reset();
            do{
                Result result_peg_115(result_peg_111.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_115.getPosition()))){
                        result_peg_115.nextPosition();
                    } else {
                        goto loop_peg_114;
                    }
                }
                result_peg_115.setValue((void*) "-");
                result_peg_111.addResult(result_peg_115);
            } while (true);
            loop_peg_114:
            if (result_peg_111.matches() == 0){
                goto out_peg_113;
            }
        
        
    }
    
    if (column_peg_92.chunk2 == 0){
        column_peg_92.chunk2 = new Chunk2();
    }
    column_peg_92.chunk2->chunk_comment = result_peg_111;
    stream.update(result_peg_111.getPosition());
    
    
    return result_peg_111;
    out_peg_113:
    
    if (column_peg_92.chunk2 == 0){
        column_peg_92.chunk2 = new Chunk2();
    }
    column_peg_92.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_116 = stream.getColumn(position);
    if (column_peg_116.chunk2 != 0 && column_peg_116.chunk2->chunk_collision_default.calculated()){
        return column_peg_116.chunk2->chunk_collision_default;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_117(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_117.getPosition()))){
                    result_peg_117.nextPosition();
                } else {
                    goto out_peg_119;
                }
            }
            result_peg_117.setValue((void*) "Clsn2Default:");
        
        Result result_peg_118 = result_peg_117;
        
        result_peg_117 = rule_s(stream, result_peg_117.getPosition());
            if (result_peg_117.error()){
                goto out_peg_119;
            }
        
        
        
        result_peg_117 = rule_integer(stream, result_peg_117.getPosition());
            if (result_peg_117.error()){
                goto out_peg_119;
            }
            num = result_peg_117.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_118.getValues()); GC::save(as<Ast::Keyword*>(value)); value = makeAttributeKeyword(value, num); GC::save(as<Ast::Attribute*>(value));
                result_peg_117.setValue(value);
            }
        
        
    }
    
    if (column_peg_116.chunk2 == 0){
        column_peg_116.chunk2 = new Chunk2();
    }
    column_peg_116.chunk2->chunk_collision_default = result_peg_117;
    stream.update(result_peg_117.getPosition());
    
    
    return result_peg_117;
    out_peg_119:
    Result result_peg_122(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_122.getPosition()))){
                    result_peg_122.nextPosition();
                } else {
                    goto out_peg_124;
                }
            }
            result_peg_122.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_122 = rule_s(stream, result_peg_122.getPosition());
            if (result_peg_122.error()){
                goto out_peg_124;
            }
        
        
        
        result_peg_122 = rule_integer(stream, result_peg_122.getPosition());
            if (result_peg_122.error()){
                goto out_peg_124;
            }
            num = result_peg_122.getValues();
        
        
    }
    
    if (column_peg_116.chunk2 == 0){
        column_peg_116.chunk2 = new Chunk2();
    }
    column_peg_116.chunk2->chunk_collision_default = result_peg_122;
    stream.update(result_peg_122.getPosition());
    
    
    return result_peg_122;
    out_peg_124:
    Result result_peg_126(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_126.getPosition()))){
                    result_peg_126.nextPosition();
                } else {
                    goto out_peg_128;
                }
            }
            result_peg_126.setValue((void*) "Clsn2:");
        
        
        
        result_peg_126 = rule_s(stream, result_peg_126.getPosition());
            if (result_peg_126.error()){
                goto out_peg_128;
            }
        
        
        
        result_peg_126 = rule_integer(stream, result_peg_126.getPosition());
            if (result_peg_126.error()){
                goto out_peg_128;
            }
            num = result_peg_126.getValues();
        
        
    }
    
    if (column_peg_116.chunk2 == 0){
        column_peg_116.chunk2 = new Chunk2();
    }
    column_peg_116.chunk2->chunk_collision_default = result_peg_126;
    stream.update(result_peg_126.getPosition());
    
    
    return result_peg_126;
    out_peg_128:
    Result result_peg_130(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_130.getPosition()))){
                    result_peg_130.nextPosition();
                } else {
                    goto out_peg_132;
                }
            }
            result_peg_130.setValue((void*) "Clsn1:");
        
        
        
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
    
    if (column_peg_116.chunk2 == 0){
        column_peg_116.chunk2 = new Chunk2();
    }
    column_peg_116.chunk2->chunk_collision_default = result_peg_130;
    stream.update(result_peg_130.getPosition());
    
    
    return result_peg_130;
    out_peg_132:
    
    if (column_peg_116.chunk2 == 0){
        column_peg_116.chunk2 = new Chunk2();
    }
    column_peg_116.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_134 = stream.getColumn(position);
    if (column_peg_134.chunk2 != 0 && column_peg_134.chunk2->chunk_collision.calculated()){
        return column_peg_134.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    
    Result result_peg_135(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) "Clsn2");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) "[");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135.reset();
            do{
                Result result_peg_143(result_peg_135.getPosition());
                result_peg_143 = rule_digit(stream, result_peg_143.getPosition());
                if (result_peg_143.error()){
                    goto loop_peg_142;
                }
                result_peg_135.addResult(result_peg_143);
            } while (true);
            loop_peg_142:
            if (result_peg_135.matches() == 0){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) "]");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) "=");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_integer(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) ",");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_integer(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) ",");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_integer(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) ",");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_integer(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
    }
    
    if (column_peg_134.chunk2 == 0){
        column_peg_134.chunk2 = new Chunk2();
    }
    column_peg_134.chunk2->chunk_collision = result_peg_135;
    stream.update(result_peg_135.getPosition());
    
    
    return result_peg_135;
    out_peg_137:
    Result result_peg_161(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) "Clsn1");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) "[");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161.reset();
            do{
                Result result_peg_169(result_peg_161.getPosition());
                result_peg_169 = rule_digit(stream, result_peg_169.getPosition());
                if (result_peg_169.error()){
                    goto loop_peg_168;
                }
                result_peg_161.addResult(result_peg_169);
            } while (true);
            loop_peg_168:
            if (result_peg_161.matches() == 0){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) "]");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) "=");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_integer(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) ",");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_integer(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) ",");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_integer(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) ",");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_integer(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
    }
    
    if (column_peg_134.chunk2 == 0){
        column_peg_134.chunk2 = new Chunk2();
    }
    column_peg_134.chunk2->chunk_collision = result_peg_161;
    stream.update(result_peg_161.getPosition());
    
    
    return result_peg_161;
    out_peg_163:
    
    if (column_peg_134.chunk2 == 0){
        column_peg_134.chunk2 = new Chunk2();
    }
    column_peg_134.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_187 = stream.getColumn(position);
    if (column_peg_187.chunk2 != 0 && column_peg_187.chunk2->chunk_action_start.calculated()){
        return column_peg_187.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_188(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_188.getPosition()))){
                    result_peg_188.nextPosition();
                } else {
                    goto out_peg_190;
                }
            }
            result_peg_188.setValue((void*) "[");
        
        
        
        result_peg_188 = rule_s(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_188.getPosition()))){
                    result_peg_188.nextPosition();
                } else {
                    goto out_peg_190;
                }
            }
            result_peg_188.setValue((void*) "Begin");
            begin = result_peg_188.getValues();
        
        
        
        result_peg_188 = rule_s(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_188.getPosition()))){
                    result_peg_188.nextPosition();
                } else {
                    goto out_peg_190;
                }
            }
            result_peg_188.setValue((void*) "Action");
            action = result_peg_188.getValues();
        
        
        
        result_peg_188 = rule_s(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
        
        
        
        result_peg_188 = rule_integer(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
            num = result_peg_188.getValues();
        
        
        
        result_peg_188 = rule_s(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_188.getPosition()))){
                    result_peg_188.nextPosition();
                } else {
                    goto out_peg_190;
                }
            }
            result_peg_188.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num); GC::save(as<std::string*>(value));
                result_peg_188.setValue(value);
            }
        
        
    }
    
    if (column_peg_187.chunk2 == 0){
        column_peg_187.chunk2 = new Chunk2();
    }
    column_peg_187.chunk2->chunk_action_start = result_peg_188;
    stream.update(result_peg_188.getPosition());
    
    
    return result_peg_188;
    out_peg_190:
    
    if (column_peg_187.chunk2 == 0){
        column_peg_187.chunk2 = new Chunk2();
    }
    column_peg_187.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_digit(Stream & stream, const int position){
    
    Column & column_peg_199 = stream.getColumn(position);
    if (column_peg_199.chunk3 != 0 && column_peg_199.chunk3->chunk_digit.calculated()){
        return column_peg_199.chunk3->chunk_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_200(myposition);
    
    char letter_peg_201 = stream.get(result_peg_200.getPosition());
    if (letter_peg_201 != '\0' && strchr("0123456789", letter_peg_201) != NULL){
        result_peg_200.nextPosition();
        result_peg_200.setValue((void*) letter_peg_201);
    } else {
        goto out_peg_202;
    }
    
    if (column_peg_199.chunk3 == 0){
        column_peg_199.chunk3 = new Chunk3();
    }
    column_peg_199.chunk3->chunk_digit = result_peg_200;
    stream.update(result_peg_200.getPosition());
    
    
    return result_peg_200;
    out_peg_202:
    
    if (column_peg_199.chunk3 == 0){
        column_peg_199.chunk3 = new Chunk3();
    }
    column_peg_199.chunk3->chunk_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_203 = stream.getColumn(position);
    if (column_peg_203.chunk3 != 0 && column_peg_203.chunk3->chunk_integer.calculated()){
        return column_peg_203.chunk3->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_204(myposition);
    
    {
    
        int save_peg_206 = result_peg_204.getPosition();
            
            result_peg_204 = rule_sign(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                
                result_peg_204 = Result(save_peg_206);
                result_peg_204.setValue((void*) 0);
                
            }
        
        Result result_peg_205 = result_peg_204;
        
        result_peg_204.reset();
            do{
                Result result_peg_209(result_peg_204.getPosition());
                result_peg_209 = rule_digit(stream, result_peg_209.getPosition());
                if (result_peg_209.error()){
                    goto loop_peg_208;
                }
                result_peg_204.addResult(result_peg_209);
            } while (true);
            loop_peg_208:
            if (result_peg_204.matches() == 0){
                goto out_peg_210;
            }
        
        Result result_peg_207 = result_peg_204;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_207.getValues()); GC::save(as<double*>(value)); value = makeNumber(result_peg_205.getValues(),value); GC::save(as<Ast::Number*>(value));
                result_peg_204.setValue(value);
            }
        
        
    }
    
    if (column_peg_203.chunk3 == 0){
        column_peg_203.chunk3 = new Chunk3();
    }
    column_peg_203.chunk3->chunk_integer = result_peg_204;
    stream.update(result_peg_204.getPosition());
    
    
    return result_peg_204;
    out_peg_210:
    
    if (column_peg_203.chunk3 == 0){
        column_peg_203.chunk3 = new Chunk3();
    }
    column_peg_203.chunk3->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sign(Stream & stream, const int position){
    
    Column & column_peg_211 = stream.getColumn(position);
    if (column_peg_211.chunk3 != 0 && column_peg_211.chunk3->chunk_sign.calculated()){
        return column_peg_211.chunk3->chunk_sign;
    }
    
    int myposition = position;
    
    
    Result result_peg_212(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_212.getPosition()))){
            result_peg_212.nextPosition();
        } else {
            goto out_peg_213;
        }
    }
    result_peg_212.setValue((void*) "-");
    
    if (column_peg_211.chunk3 == 0){
        column_peg_211.chunk3 = new Chunk3();
    }
    column_peg_211.chunk3->chunk_sign = result_peg_212;
    stream.update(result_peg_212.getPosition());
    
    
    return result_peg_212;
    out_peg_213:
    Result result_peg_214(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_214.getPosition()))){
            result_peg_214.nextPosition();
        } else {
            goto out_peg_215;
        }
    }
    result_peg_214.setValue((void*) "+");
    
    if (column_peg_211.chunk3 == 0){
        column_peg_211.chunk3 = new Chunk3();
    }
    column_peg_211.chunk3->chunk_sign = result_peg_214;
    stream.update(result_peg_214.getPosition());
    
    
    return result_peg_214;
    out_peg_215:
    
    if (column_peg_211.chunk3 == 0){
        column_peg_211.chunk3 = new Chunk3();
    }
    column_peg_211.chunk3->chunk_sign = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_216 = stream.getColumn(position);
    if (column_peg_216.chunk3 != 0 && column_peg_216.chunk3->chunk_valuelist.calculated()){
        return column_peg_216.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_217(myposition);
    
    {
    
        result_peg_217 = rule_value(stream, result_peg_217.getPosition());
            if (result_peg_217.error()){
                goto out_peg_219;
            }
        
        Result result_peg_218 = result_peg_217;
        
        result_peg_217.reset();
            do{
                Result result_peg_222(result_peg_217.getPosition());
                {
                
                    result_peg_222 = rule_s(stream, result_peg_222.getPosition());
                        if (result_peg_222.error()){
                            goto loop_peg_221;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_222.getPosition()))){
                                result_peg_222.nextPosition();
                            } else {
                                goto loop_peg_221;
                            }
                        }
                        result_peg_222.setValue((void*) ",");
                    
                    
                    
                    result_peg_222 = rule_s(stream, result_peg_222.getPosition());
                        if (result_peg_222.error()){
                            goto loop_peg_221;
                        }
                    
                    
                    
                    int save_peg_226 = result_peg_222.getPosition();
                        
                        result_peg_222 = rule_value(stream, result_peg_222.getPosition());
                        if (result_peg_222.error()){
                            
                            result_peg_222 = Result(save_peg_226);
                            result_peg_222.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_217.addResult(result_peg_222);
            } while (true);
            loop_peg_221:
            ;
        
        Result result_peg_220 = result_peg_217;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_218.getValues(),result_peg_220.getValues()); GC::save(as<Ast::Value*>(value));
                result_peg_217.setValue(value);
            }
        
        
    }
    
    if (column_peg_216.chunk3 == 0){
        column_peg_216.chunk3 = new Chunk3();
    }
    column_peg_216.chunk3->chunk_valuelist = result_peg_217;
    stream.update(result_peg_217.getPosition());
    
    
    return result_peg_217;
    out_peg_219:
    
    if (column_peg_216.chunk3 == 0){
        column_peg_216.chunk3 = new Chunk3();
    }
    column_peg_216.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_227 = stream.getColumn(position);
    if (column_peg_227.chunk3 != 0 && column_peg_227.chunk3->chunk_value.calculated()){
        return column_peg_227.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_228(myposition);
    
    result_peg_228 = rule_integer(stream, result_peg_228.getPosition());
    if (result_peg_228.error()){
        goto out_peg_229;
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_228;
    stream.update(result_peg_228.getPosition());
    
    
    return result_peg_228;
    out_peg_229:
    Result result_peg_230(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_230.getPosition()))){
                    result_peg_230.nextPosition();
                } else {
                    goto out_peg_232;
                }
            }
            result_peg_230.setValue((void*) "as");
        
        Result result_peg_231 = result_peg_230;
        
        result_peg_230 = rule_integer(stream, result_peg_230.getPosition());
            if (result_peg_230.error()){
                goto out_peg_232;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_230.getPosition()))){
                    result_peg_230.nextPosition();
                } else {
                    goto out_peg_232;
                }
            }
            result_peg_230.setValue((void*) "d");
        
        
        
        result_peg_230 = rule_integer(stream, result_peg_230.getPosition());
            if (result_peg_230.error()){
                goto out_peg_232;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_231.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_230.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_230;
    stream.update(result_peg_230.getPosition());
    
    
    return result_peg_230;
    out_peg_232:
    Result result_peg_236(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_236.getPosition()))){
                    result_peg_236.nextPosition();
                } else {
                    goto out_peg_238;
                }
            }
            result_peg_236.setValue((void*) "a");
        
        Result result_peg_237 = result_peg_236;
        
        result_peg_236 = rule_integer(stream, result_peg_236.getPosition());
            if (result_peg_236.error()){
                goto out_peg_238;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_237.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_236.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_236;
    stream.update(result_peg_236.getPosition());
    
    
    return result_peg_236;
    out_peg_238:
    Result result_peg_240(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_240.getPosition()))){
                    result_peg_240.nextPosition();
                } else {
                    goto out_peg_242;
                }
            }
            result_peg_240.setValue((void*) "a");
        
        Result result_peg_241 = result_peg_240;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_241.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_240.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_240;
    stream.update(result_peg_240.getPosition());
    
    
    return result_peg_240;
    out_peg_242:
    Result result_peg_243(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_243.getPosition()))){
                    result_peg_243.nextPosition();
                } else {
                    goto out_peg_245;
                }
            }
            result_peg_243.setValue((void*) "s");
        
        Result result_peg_244 = result_peg_243;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_244.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_243.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_243;
    stream.update(result_peg_243.getPosition());
    
    
    return result_peg_243;
    out_peg_245:
    Result result_peg_246(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_246.getPosition()))){
                    result_peg_246.nextPosition();
                } else {
                    goto out_peg_248;
                }
            }
            result_peg_246.setValue((void*) "vh");
        
        Result result_peg_247 = result_peg_246;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_247.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_246.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_246;
    stream.update(result_peg_246.getPosition());
    
    
    return result_peg_246;
    out_peg_248:
    Result result_peg_249(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_249.getPosition()))){
                    result_peg_249.nextPosition();
                } else {
                    goto out_peg_251;
                }
            }
            result_peg_249.setValue((void*) "hv");
        
        Result result_peg_250 = result_peg_249;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_250.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_249.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_249;
    stream.update(result_peg_249.getPosition());
    
    
    return result_peg_249;
    out_peg_251:
    Result result_peg_252(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_252.getPosition()))){
                    result_peg_252.nextPosition();
                } else {
                    goto out_peg_254;
                }
            }
            result_peg_252.setValue((void*) "v");
        
        Result result_peg_253 = result_peg_252;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_253.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_252.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_252;
    stream.update(result_peg_252.getPosition());
    
    
    return result_peg_252;
    out_peg_254:
    Result result_peg_255(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_255.getPosition()))){
                    result_peg_255.nextPosition();
                } else {
                    goto out_peg_257;
                }
            }
            result_peg_255.setValue((void*) "h");
        
        Result result_peg_256 = result_peg_255;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_256.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_255.setValue(value);
            }
        
        
    }
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = result_peg_255;
    stream.update(result_peg_255.getPosition());
    
    
    return result_peg_255;
    out_peg_257:
    
    if (column_peg_227.chunk3 == 0){
        column_peg_227.chunk3 = new Chunk3();
    }
    column_peg_227.chunk3->chunk_value = errorResult;
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

        
