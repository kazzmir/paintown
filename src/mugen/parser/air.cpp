

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

struct Chunk4{
    Result chunk_valuev;
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
+
(chunk4 != NULL ? ((chunk4->chunk_valuev.calculated() ? 1 : 0)) : 0)
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
Result rule_valuev(Stream &, const int);


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

Ast::Keyword * makeKeyword(const Value & value){
    return new Ast::Keyword(as<char*>(value));
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
    
    Column & column_peg_23 = stream.getColumn(position);
    if (column_peg_23.chunk0 != 0 && column_peg_23.chunk0->chunk_action.calculated()){
        return column_peg_23.chunk0->chunk_action;
    }
    
    int myposition = position;
    
    Value ast;
    Result result_peg_24(myposition);
    
    {
    
        result_peg_24 = rule_action_start(stream, result_peg_24.getPosition());
            if (result_peg_24.error()){
                goto out_peg_26;
            }
        
        Result result_peg_25 = result_peg_24;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_25.getValues()); GC::save(as<Ast::Section*>(value));
                result_peg_24.setValue(value);
            }
            ast = result_peg_24.getValues();
        
        
        
        result_peg_24 = rule_whitespace(stream, result_peg_24.getPosition());
            if (result_peg_24.error()){
                goto out_peg_26;
            }
        
        
        
        result_peg_24.reset();
            do{
                Result result_peg_31(result_peg_24.getPosition());
                result_peg_31 = rule_newline(stream, result_peg_31.getPosition());
                if (result_peg_31.error()){
                    goto loop_peg_30;
                }
                result_peg_24.addResult(result_peg_31);
            } while (true);
            loop_peg_30:
            if (result_peg_24.matches() == 0){
                goto out_peg_26;
            }
        
        
        
        result_peg_24.reset();
            do{
                Result result_peg_34(result_peg_24.getPosition());
                {
                
                    result_peg_34 = rule_action_line(stream, result_peg_34.getPosition());
                        if (result_peg_34.error()){
                            goto loop_peg_33;
                        }
                    
                    
                    
                    result_peg_34 = rule_whitespace(stream, result_peg_34.getPosition());
                        if (result_peg_34.error()){
                            goto loop_peg_33;
                        }
                    
                    
                    
                    result_peg_34 = rule_line_end(stream, result_peg_34.getPosition());
                        if (result_peg_34.error()){
                            goto loop_peg_33;
                        }
                    
                    
                }
                result_peg_24.addResult(result_peg_34);
            } while (true);
            loop_peg_33:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_24.setValue(value);
            }
        
        
    }
    
    if (column_peg_23.chunk0 == 0){
        column_peg_23.chunk0 = new Chunk0();
    }
    column_peg_23.chunk0->chunk_action = result_peg_24;
    stream.update(result_peg_24.getPosition());
    
    
    return result_peg_24;
    out_peg_26:
    
    if (column_peg_23.chunk0 == 0){
        column_peg_23.chunk0 = new Chunk0();
    }
    column_peg_23.chunk0->chunk_action = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position){
    
    Column & column_peg_37 = stream.getColumn(position);
    if (column_peg_37.chunk0 != 0 && column_peg_37.chunk0->chunk_action_line.calculated()){
        return column_peg_37.chunk0->chunk_action_line;
    }
    
    int myposition = position;
    
    
    Result result_peg_38(myposition);
    
    {
    
        result_peg_38 = rule_s(stream, result_peg_38.getPosition());
            if (result_peg_38.error()){
                goto out_peg_40;
            }
        
        
        
        result_peg_38 = rule_comment(stream, result_peg_38.getPosition());
            if (result_peg_38.error()){
                goto out_peg_40;
            }
        
        
    }
    
    if (column_peg_37.chunk0 == 0){
        column_peg_37.chunk0 = new Chunk0();
    }
    column_peg_37.chunk0->chunk_action_line = result_peg_38;
    stream.update(result_peg_38.getPosition());
    
    
    return result_peg_38;
    out_peg_40:
    Result result_peg_41(myposition);
    
    {
    
        result_peg_41 = rule_s(stream, result_peg_41.getPosition());
            if (result_peg_41.error()){
                goto out_peg_43;
            }
        
        
        
        result_peg_41 = rule_collision_default(stream, result_peg_41.getPosition());
            if (result_peg_41.error()){
                goto out_peg_43;
            }
        
        
    }
    
    if (column_peg_37.chunk0 == 0){
        column_peg_37.chunk0 = new Chunk0();
    }
    column_peg_37.chunk0->chunk_action_line = result_peg_41;
    stream.update(result_peg_41.getPosition());
    
    
    return result_peg_41;
    out_peg_43:
    Result result_peg_44(myposition);
    
    {
    
        result_peg_44 = rule_s(stream, result_peg_44.getPosition());
            if (result_peg_44.error()){
                goto out_peg_46;
            }
        
        
        
        result_peg_44 = rule_collision(stream, result_peg_44.getPosition());
            if (result_peg_44.error()){
                goto out_peg_46;
            }
        
        
    }
    
    if (column_peg_37.chunk0 == 0){
        column_peg_37.chunk0 = new Chunk0();
    }
    column_peg_37.chunk0->chunk_action_line = result_peg_44;
    stream.update(result_peg_44.getPosition());
    
    
    return result_peg_44;
    out_peg_46:
    Result result_peg_47(myposition);
    
    {
    
        result_peg_47 = rule_s(stream, result_peg_47.getPosition());
            if (result_peg_47.error()){
                goto out_peg_49;
            }
        
        
        
        result_peg_47 = rule_valuelist(stream, result_peg_47.getPosition());
            if (result_peg_47.error()){
                goto out_peg_49;
            }
        
        
    }
    
    if (column_peg_37.chunk0 == 0){
        column_peg_37.chunk0 = new Chunk0();
    }
    column_peg_37.chunk0->chunk_action_line = result_peg_47;
    stream.update(result_peg_47.getPosition());
    
    
    return result_peg_47;
    out_peg_49:
    Result result_peg_50(myposition);
    
    {
    
        result_peg_50 = rule_s(stream, result_peg_50.getPosition());
            if (result_peg_50.error()){
                goto out_peg_52;
            }
        
        
        
        result_peg_50 = rule_loopstart(stream, result_peg_50.getPosition());
            if (result_peg_50.error()){
                goto out_peg_52;
            }
        
        
    }
    
    if (column_peg_37.chunk0 == 0){
        column_peg_37.chunk0 = new Chunk0();
    }
    column_peg_37.chunk0->chunk_action_line = result_peg_50;
    stream.update(result_peg_50.getPosition());
    
    
    return result_peg_50;
    out_peg_52:
    
    if (column_peg_37.chunk0 == 0){
        column_peg_37.chunk0 = new Chunk0();
    }
    column_peg_37.chunk0->chunk_action_line = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_53 = stream.getColumn(position);
    if (column_peg_53.chunk0 != 0 && column_peg_53.chunk0->chunk_line_end.calculated()){
        return column_peg_53.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_54(myposition);
    
    result_peg_54.reset();
    do{
        Result result_peg_56(result_peg_54.getPosition());
        result_peg_56 = rule_newline(stream, result_peg_56.getPosition());
        if (result_peg_56.error()){
            goto loop_peg_55;
        }
        result_peg_54.addResult(result_peg_56);
    } while (true);
    loop_peg_55:
    if (result_peg_54.matches() == 0){
        goto out_peg_57;
    }
    
    if (column_peg_53.chunk0 == 0){
        column_peg_53.chunk0 = new Chunk0();
    }
    column_peg_53.chunk0->chunk_line_end = result_peg_54;
    stream.update(result_peg_54.getPosition());
    
    
    return result_peg_54;
    out_peg_57:
    Result result_peg_58(myposition);
    
    {
    
        Result result_peg_60(result_peg_58.getPosition());
            if ('\0' == stream.get(result_peg_60.getPosition())){
                result_peg_60.nextPosition();
                result_peg_60.setValue((void *) '\0');
            } else {
                goto out_peg_61;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_53.chunk0 == 0){
        column_peg_53.chunk0 = new Chunk0();
    }
    column_peg_53.chunk0->chunk_line_end = result_peg_58;
    stream.update(result_peg_58.getPosition());
    
    
    return result_peg_58;
    out_peg_61:
    
    if (column_peg_53.chunk0 == 0){
        column_peg_53.chunk0 = new Chunk0();
    }
    column_peg_53.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_62 = stream.getColumn(position);
    if (column_peg_62.chunk1 != 0 && column_peg_62.chunk1->chunk_newline.calculated()){
        return column_peg_62.chunk1->chunk_newline;
    }
    
    int myposition = position;
    
    
    Result result_peg_63(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_63.getPosition()))){
            result_peg_63.nextPosition();
        } else {
            goto out_peg_64;
        }
    }
    result_peg_63.setValue((void*) "\n");
    
    if (column_peg_62.chunk1 == 0){
        column_peg_62.chunk1 = new Chunk1();
    }
    column_peg_62.chunk1->chunk_newline = result_peg_63;
    stream.update(result_peg_63.getPosition());
    
    
    return result_peg_63;
    out_peg_64:
    Result result_peg_65(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_65.getPosition()))){
            result_peg_65.nextPosition();
        } else {
            goto out_peg_66;
        }
    }
    result_peg_65.setValue((void*) "\r");
    
    if (column_peg_62.chunk1 == 0){
        column_peg_62.chunk1 = new Chunk1();
    }
    column_peg_62.chunk1->chunk_newline = result_peg_65;
    stream.update(result_peg_65.getPosition());
    
    
    return result_peg_65;
    out_peg_66:
    
    if (column_peg_62.chunk1 == 0){
        column_peg_62.chunk1 = new Chunk1();
    }
    column_peg_62.chunk1->chunk_newline = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_67 = stream.getColumn(position);
    if (column_peg_67.chunk1 != 0 && column_peg_67.chunk1->chunk_loopstart.calculated()){
        return column_peg_67.chunk1->chunk_loopstart;
    }
    
    int myposition = position;
    
    
    Result result_peg_68(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_68.getPosition()))){
                    result_peg_68.nextPosition();
                } else {
                    goto out_peg_70;
                }
            }
            result_peg_68.setValue((void*) "loopstart");
        
        Result result_peg_69 = result_peg_68;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_69.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_68.setValue(value);
            }
        
        
    }
    
    if (column_peg_67.chunk1 == 0){
        column_peg_67.chunk1 = new Chunk1();
    }
    column_peg_67.chunk1->chunk_loopstart = result_peg_68;
    stream.update(result_peg_68.getPosition());
    
    
    return result_peg_68;
    out_peg_70:
    
    if (column_peg_67.chunk1 == 0){
        column_peg_67.chunk1 = new Chunk1();
    }
    column_peg_67.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_71 = stream.getColumn(position);
    if (column_peg_71.chunk1 != 0 && column_peg_71.chunk1->chunk_whitespace.calculated()){
        return column_peg_71.chunk1->chunk_whitespace;
    }
    
    int myposition = position;
    
    
    Result result_peg_72(myposition);
    
    result_peg_72.reset();
    do{
        Result result_peg_74(result_peg_72.getPosition());
        result_peg_74 = rule_sw(stream, result_peg_74.getPosition());
        if (result_peg_74.error()){
            goto loop_peg_73;
        }
        result_peg_72.addResult(result_peg_74);
    } while (true);
    loop_peg_73:
    ;
    
    if (column_peg_71.chunk1 == 0){
        column_peg_71.chunk1 = new Chunk1();
    }
    column_peg_71.chunk1->chunk_whitespace = result_peg_72;
    stream.update(result_peg_72.getPosition());
    
    
    return result_peg_72;
    
    if (column_peg_71.chunk1 == 0){
        column_peg_71.chunk1 = new Chunk1();
    }
    column_peg_71.chunk1->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_75 = stream.getColumn(position);
    if (column_peg_75.chunk1 != 0 && column_peg_75.chunk1->chunk_sw.calculated()){
        return column_peg_75.chunk1->chunk_sw;
    }
    
    int myposition = position;
    
    
    Result result_peg_76(myposition);
    
    result_peg_76 = rule_space(stream, result_peg_76.getPosition());
    if (result_peg_76.error()){
        goto out_peg_77;
    }
    
    if (column_peg_75.chunk1 == 0){
        column_peg_75.chunk1 = new Chunk1();
    }
    column_peg_75.chunk1->chunk_sw = result_peg_76;
    stream.update(result_peg_76.getPosition());
    
    
    return result_peg_76;
    out_peg_77:
    Result result_peg_78(myposition);
    
    result_peg_78 = rule_comment(stream, result_peg_78.getPosition());
    if (result_peg_78.error()){
        goto out_peg_79;
    }
    
    if (column_peg_75.chunk1 == 0){
        column_peg_75.chunk1 = new Chunk1();
    }
    column_peg_75.chunk1->chunk_sw = result_peg_78;
    stream.update(result_peg_78.getPosition());
    
    
    return result_peg_78;
    out_peg_79:
    
    if (column_peg_75.chunk1 == 0){
        column_peg_75.chunk1 = new Chunk1();
    }
    column_peg_75.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_80 = stream.getColumn(position);
    if (column_peg_80.chunk1 != 0 && column_peg_80.chunk1->chunk_s.calculated()){
        return column_peg_80.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_81(myposition);
    
    result_peg_81.reset();
    do{
        Result result_peg_83(result_peg_81.getPosition());
        result_peg_83 = rule_space(stream, result_peg_83.getPosition());
        if (result_peg_83.error()){
            goto loop_peg_82;
        }
        result_peg_81.addResult(result_peg_83);
    } while (true);
    loop_peg_82:
    ;
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_s = result_peg_81;
    stream.update(result_peg_81.getPosition());
    
    
    return result_peg_81;
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_space(Stream & stream, const int position){
    
    Column & column_peg_84 = stream.getColumn(position);
    if (column_peg_84.chunk2 != 0 && column_peg_84.chunk2->chunk_space.calculated()){
        return column_peg_84.chunk2->chunk_space;
    }
    
    int myposition = position;
    
    
    Result result_peg_85(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_85.getPosition()))){
            result_peg_85.nextPosition();
        } else {
            goto out_peg_86;
        }
    }
    result_peg_85.setValue((void*) " ");
    
    if (column_peg_84.chunk2 == 0){
        column_peg_84.chunk2 = new Chunk2();
    }
    column_peg_84.chunk2->chunk_space = result_peg_85;
    stream.update(result_peg_85.getPosition());
    
    
    return result_peg_85;
    out_peg_86:
    Result result_peg_87(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_87.getPosition()))){
            result_peg_87.nextPosition();
        } else {
            goto out_peg_88;
        }
    }
    result_peg_87.setValue((void*) "\t");
    
    if (column_peg_84.chunk2 == 0){
        column_peg_84.chunk2 = new Chunk2();
    }
    column_peg_84.chunk2->chunk_space = result_peg_87;
    stream.update(result_peg_87.getPosition());
    
    
    return result_peg_87;
    out_peg_88:
    Result result_peg_89(myposition);
    
    if ((unsigned char) stream.get(result_peg_89.getPosition()) == (unsigned char) 255){
        result_peg_89.nextPosition();
    } else {
        goto out_peg_90;
    }
    result_peg_89.setValue((void*) 255);
    
    if (column_peg_84.chunk2 == 0){
        column_peg_84.chunk2 = new Chunk2();
    }
    column_peg_84.chunk2->chunk_space = result_peg_89;
    stream.update(result_peg_89.getPosition());
    
    
    return result_peg_89;
    out_peg_90:
    
    if (column_peg_84.chunk2 == 0){
        column_peg_84.chunk2 = new Chunk2();
    }
    column_peg_84.chunk2->chunk_space = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_91 = stream.getColumn(position);
    if (column_peg_91.chunk2 != 0 && column_peg_91.chunk2->chunk_comment.calculated()){
        return column_peg_91.chunk2->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_92(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_92.getPosition()))){
                    result_peg_92.nextPosition();
                } else {
                    goto out_peg_94;
                }
            }
            result_peg_92.setValue((void*) ";");
        
        
        
        result_peg_92.reset();
            do{
                Result result_peg_96(result_peg_92.getPosition());
                {
                
                    Result result_peg_99(result_peg_96);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_99.getPosition()))){
                                result_peg_99.nextPosition();
                            } else {
                                goto not_peg_98;
                            }
                        }
                        result_peg_99.setValue((void*) "\n");
                        goto loop_peg_95;
                        not_peg_98:
                        result_peg_96.setValue((void*)0);
                    
                    
                    
                    char temp_peg_100 = stream.get(result_peg_96.getPosition());
                        if (temp_peg_100 != '\0'){
                            result_peg_96.setValue((void*) temp_peg_100);
                            result_peg_96.nextPosition();
                        } else {
                            goto loop_peg_95;
                        }
                    
                    
                }
                result_peg_92.addResult(result_peg_96);
            } while (true);
            loop_peg_95:
            ;
        
        
    }
    
    if (column_peg_91.chunk2 == 0){
        column_peg_91.chunk2 = new Chunk2();
    }
    column_peg_91.chunk2->chunk_comment = result_peg_92;
    stream.update(result_peg_92.getPosition());
    
    
    return result_peg_92;
    out_peg_94:
    Result result_peg_101(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_101.getPosition()))){
                    result_peg_101.nextPosition();
                } else {
                    goto out_peg_103;
                }
            }
            result_peg_101.setValue((void*) "=");
        
        
        
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
    
    if (column_peg_91.chunk2 == 0){
        column_peg_91.chunk2 = new Chunk2();
    }
    column_peg_91.chunk2->chunk_comment = result_peg_101;
    stream.update(result_peg_101.getPosition());
    
    
    return result_peg_101;
    out_peg_103:
    Result result_peg_110(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_110.getPosition()))){
                    result_peg_110.nextPosition();
                } else {
                    goto out_peg_112;
                }
            }
            result_peg_110.setValue((void*) "-");
        
        
        
        result_peg_110.reset();
            do{
                Result result_peg_114(result_peg_110.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_114.getPosition()))){
                        result_peg_114.nextPosition();
                    } else {
                        goto loop_peg_113;
                    }
                }
                result_peg_114.setValue((void*) "-");
                result_peg_110.addResult(result_peg_114);
            } while (true);
            loop_peg_113:
            if (result_peg_110.matches() == 0){
                goto out_peg_112;
            }
        
        
    }
    
    if (column_peg_91.chunk2 == 0){
        column_peg_91.chunk2 = new Chunk2();
    }
    column_peg_91.chunk2->chunk_comment = result_peg_110;
    stream.update(result_peg_110.getPosition());
    
    
    return result_peg_110;
    out_peg_112:
    
    if (column_peg_91.chunk2 == 0){
        column_peg_91.chunk2 = new Chunk2();
    }
    column_peg_91.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_115 = stream.getColumn(position);
    if (column_peg_115.chunk2 != 0 && column_peg_115.chunk2->chunk_collision_default.calculated()){
        return column_peg_115.chunk2->chunk_collision_default;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_116(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_116.getPosition()))){
                    result_peg_116.nextPosition();
                } else {
                    goto out_peg_118;
                }
            }
            result_peg_116.setValue((void*) "Clsn2Default:");
        
        Result result_peg_117 = result_peg_116;
        
        result_peg_116 = rule_s(stream, result_peg_116.getPosition());
            if (result_peg_116.error()){
                goto out_peg_118;
            }
        
        
        
        result_peg_116 = rule_integer(stream, result_peg_116.getPosition());
            if (result_peg_116.error()){
                goto out_peg_118;
            }
            num = result_peg_116.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_117.getValues()); GC::save(as<Ast::Keyword*>(value)); value = makeAttributeKeyword(value, num); GC::save(as<Ast::Attribute*>(value));
                result_peg_116.setValue(value);
            }
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_116;
    stream.update(result_peg_116.getPosition());
    
    
    return result_peg_116;
    out_peg_118:
    Result result_peg_121(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_121.getPosition()))){
                    result_peg_121.nextPosition();
                } else {
                    goto out_peg_123;
                }
            }
            result_peg_121.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_121 = rule_s(stream, result_peg_121.getPosition());
            if (result_peg_121.error()){
                goto out_peg_123;
            }
        
        
        
        result_peg_121 = rule_integer(stream, result_peg_121.getPosition());
            if (result_peg_121.error()){
                goto out_peg_123;
            }
            num = result_peg_121.getValues();
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_121;
    stream.update(result_peg_121.getPosition());
    
    
    return result_peg_121;
    out_peg_123:
    Result result_peg_125(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_125.getPosition()))){
                    result_peg_125.nextPosition();
                } else {
                    goto out_peg_127;
                }
            }
            result_peg_125.setValue((void*) "Clsn2:");
        
        
        
        result_peg_125 = rule_s(stream, result_peg_125.getPosition());
            if (result_peg_125.error()){
                goto out_peg_127;
            }
        
        
        
        result_peg_125 = rule_integer(stream, result_peg_125.getPosition());
            if (result_peg_125.error()){
                goto out_peg_127;
            }
            num = result_peg_125.getValues();
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_125;
    stream.update(result_peg_125.getPosition());
    
    
    return result_peg_125;
    out_peg_127:
    Result result_peg_129(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) "Clsn1:");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_integer(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
            num = result_peg_129.getValues();
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_129;
    stream.update(result_peg_129.getPosition());
    
    
    return result_peg_129;
    out_peg_131:
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_133 = stream.getColumn(position);
    if (column_peg_133.chunk2 != 0 && column_peg_133.chunk2->chunk_collision.calculated()){
        return column_peg_133.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    
    Result result_peg_134(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) "Clsn2");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) "[");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134.reset();
            do{
                Result result_peg_142(result_peg_134.getPosition());
                result_peg_142 = rule_digit(stream, result_peg_142.getPosition());
                if (result_peg_142.error()){
                    goto loop_peg_141;
                }
                result_peg_134.addResult(result_peg_142);
            } while (true);
            loop_peg_141:
            if (result_peg_134.matches() == 0){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) "]");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) "=");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_integer(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) ",");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_integer(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) ",");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_integer(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_134.getPosition()))){
                    result_peg_134.nextPosition();
                } else {
                    goto out_peg_136;
                }
            }
            result_peg_134.setValue((void*) ",");
        
        
        
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_integer(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
    }
    
    if (column_peg_133.chunk2 == 0){
        column_peg_133.chunk2 = new Chunk2();
    }
    column_peg_133.chunk2->chunk_collision = result_peg_134;
    stream.update(result_peg_134.getPosition());
    
    
    return result_peg_134;
    out_peg_136:
    Result result_peg_160(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) "Clsn1");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) "[");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160.reset();
            do{
                Result result_peg_168(result_peg_160.getPosition());
                result_peg_168 = rule_digit(stream, result_peg_168.getPosition());
                if (result_peg_168.error()){
                    goto loop_peg_167;
                }
                result_peg_160.addResult(result_peg_168);
            } while (true);
            loop_peg_167:
            if (result_peg_160.matches() == 0){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) "]");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) "=");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_integer(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) ",");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_integer(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) ",");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_integer(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) ",");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        result_peg_160 = rule_integer(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
    }
    
    if (column_peg_133.chunk2 == 0){
        column_peg_133.chunk2 = new Chunk2();
    }
    column_peg_133.chunk2->chunk_collision = result_peg_160;
    stream.update(result_peg_160.getPosition());
    
    
    return result_peg_160;
    out_peg_162:
    
    if (column_peg_133.chunk2 == 0){
        column_peg_133.chunk2 = new Chunk2();
    }
    column_peg_133.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_186 = stream.getColumn(position);
    if (column_peg_186.chunk2 != 0 && column_peg_186.chunk2->chunk_action_start.calculated()){
        return column_peg_186.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_187(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_187.getPosition()))){
                    result_peg_187.nextPosition();
                } else {
                    goto out_peg_189;
                }
            }
            result_peg_187.setValue((void*) "[");
        
        
        
        result_peg_187 = rule_s(stream, result_peg_187.getPosition());
            if (result_peg_187.error()){
                goto out_peg_189;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_187.getPosition()))){
                    result_peg_187.nextPosition();
                } else {
                    goto out_peg_189;
                }
            }
            result_peg_187.setValue((void*) "Begin");
            begin = result_peg_187.getValues();
        
        
        
        result_peg_187 = rule_s(stream, result_peg_187.getPosition());
            if (result_peg_187.error()){
                goto out_peg_189;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_187.getPosition()))){
                    result_peg_187.nextPosition();
                } else {
                    goto out_peg_189;
                }
            }
            result_peg_187.setValue((void*) "Action");
            action = result_peg_187.getValues();
        
        
        
        result_peg_187 = rule_s(stream, result_peg_187.getPosition());
            if (result_peg_187.error()){
                goto out_peg_189;
            }
        
        
        
        result_peg_187 = rule_integer(stream, result_peg_187.getPosition());
            if (result_peg_187.error()){
                goto out_peg_189;
            }
            num = result_peg_187.getValues();
        
        
        
        result_peg_187 = rule_s(stream, result_peg_187.getPosition());
            if (result_peg_187.error()){
                goto out_peg_189;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_187.getPosition()))){
                    result_peg_187.nextPosition();
                } else {
                    goto out_peg_189;
                }
            }
            result_peg_187.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num); GC::save(as<std::string*>(value));
                result_peg_187.setValue(value);
            }
        
        
    }
    
    if (column_peg_186.chunk2 == 0){
        column_peg_186.chunk2 = new Chunk2();
    }
    column_peg_186.chunk2->chunk_action_start = result_peg_187;
    stream.update(result_peg_187.getPosition());
    
    
    return result_peg_187;
    out_peg_189:
    
    if (column_peg_186.chunk2 == 0){
        column_peg_186.chunk2 = new Chunk2();
    }
    column_peg_186.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_digit(Stream & stream, const int position){
    
    Column & column_peg_198 = stream.getColumn(position);
    if (column_peg_198.chunk3 != 0 && column_peg_198.chunk3->chunk_digit.calculated()){
        return column_peg_198.chunk3->chunk_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_199(myposition);
    
    char letter_peg_200 = stream.get(result_peg_199.getPosition());
    if (letter_peg_200 != '\0' && strchr("0123456789", letter_peg_200) != NULL){
        result_peg_199.nextPosition();
        result_peg_199.setValue((void*) letter_peg_200);
    } else {
        goto out_peg_201;
    }
    
    if (column_peg_198.chunk3 == 0){
        column_peg_198.chunk3 = new Chunk3();
    }
    column_peg_198.chunk3->chunk_digit = result_peg_199;
    stream.update(result_peg_199.getPosition());
    
    
    return result_peg_199;
    out_peg_201:
    
    if (column_peg_198.chunk3 == 0){
        column_peg_198.chunk3 = new Chunk3();
    }
    column_peg_198.chunk3->chunk_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_202 = stream.getColumn(position);
    if (column_peg_202.chunk3 != 0 && column_peg_202.chunk3->chunk_integer.calculated()){
        return column_peg_202.chunk3->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_203(myposition);
    
    {
    
        int save_peg_205 = result_peg_203.getPosition();
            
            result_peg_203 = rule_sign(stream, result_peg_203.getPosition());
            if (result_peg_203.error()){
                
                result_peg_203 = Result(save_peg_205);
                result_peg_203.setValue((void*) 0);
                
            }
        
        Result result_peg_204 = result_peg_203;
        
        result_peg_203.reset();
            do{
                Result result_peg_208(result_peg_203.getPosition());
                result_peg_208 = rule_digit(stream, result_peg_208.getPosition());
                if (result_peg_208.error()){
                    goto loop_peg_207;
                }
                result_peg_203.addResult(result_peg_208);
            } while (true);
            loop_peg_207:
            if (result_peg_203.matches() == 0){
                goto out_peg_209;
            }
        
        Result result_peg_206 = result_peg_203;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_206.getValues()); GC::save(as<double*>(value)); value = makeNumber(result_peg_204.getValues(),value); GC::save(as<Ast::Number*>(value));
                result_peg_203.setValue(value);
            }
        
        
    }
    
    if (column_peg_202.chunk3 == 0){
        column_peg_202.chunk3 = new Chunk3();
    }
    column_peg_202.chunk3->chunk_integer = result_peg_203;
    stream.update(result_peg_203.getPosition());
    
    
    return result_peg_203;
    out_peg_209:
    
    if (column_peg_202.chunk3 == 0){
        column_peg_202.chunk3 = new Chunk3();
    }
    column_peg_202.chunk3->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sign(Stream & stream, const int position){
    
    Column & column_peg_210 = stream.getColumn(position);
    if (column_peg_210.chunk3 != 0 && column_peg_210.chunk3->chunk_sign.calculated()){
        return column_peg_210.chunk3->chunk_sign;
    }
    
    int myposition = position;
    
    
    Result result_peg_211(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_211.getPosition()))){
            result_peg_211.nextPosition();
        } else {
            goto out_peg_212;
        }
    }
    result_peg_211.setValue((void*) "-");
    
    if (column_peg_210.chunk3 == 0){
        column_peg_210.chunk3 = new Chunk3();
    }
    column_peg_210.chunk3->chunk_sign = result_peg_211;
    stream.update(result_peg_211.getPosition());
    
    
    return result_peg_211;
    out_peg_212:
    Result result_peg_213(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_213.getPosition()))){
            result_peg_213.nextPosition();
        } else {
            goto out_peg_214;
        }
    }
    result_peg_213.setValue((void*) "+");
    
    if (column_peg_210.chunk3 == 0){
        column_peg_210.chunk3 = new Chunk3();
    }
    column_peg_210.chunk3->chunk_sign = result_peg_213;
    stream.update(result_peg_213.getPosition());
    
    
    return result_peg_213;
    out_peg_214:
    
    if (column_peg_210.chunk3 == 0){
        column_peg_210.chunk3 = new Chunk3();
    }
    column_peg_210.chunk3->chunk_sign = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_215 = stream.getColumn(position);
    if (column_peg_215.chunk3 != 0 && column_peg_215.chunk3->chunk_valuelist.calculated()){
        return column_peg_215.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_216(myposition);
    
    {
    
        result_peg_216 = rule_value(stream, result_peg_216.getPosition());
            if (result_peg_216.error()){
                goto out_peg_218;
            }
        
        
        
        result_peg_216.reset();
            do{
                Result result_peg_220(result_peg_216.getPosition());
                {
                
                    result_peg_220 = rule_s(stream, result_peg_220.getPosition());
                        if (result_peg_220.error()){
                            goto loop_peg_219;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_220.getPosition()))){
                                result_peg_220.nextPosition();
                            } else {
                                goto loop_peg_219;
                            }
                        }
                        result_peg_220.setValue((void*) ",");
                    
                    
                    
                    result_peg_220 = rule_s(stream, result_peg_220.getPosition());
                        if (result_peg_220.error()){
                            goto loop_peg_219;
                        }
                    
                    
                    
                    result_peg_220 = rule_valuev(stream, result_peg_220.getPosition());
                        if (result_peg_220.error()){
                            goto loop_peg_219;
                        }
                    
                    
                }
                result_peg_216.addResult(result_peg_220);
            } while (true);
            loop_peg_219:
            ;
        
        
    }
    
    if (column_peg_215.chunk3 == 0){
        column_peg_215.chunk3 = new Chunk3();
    }
    column_peg_215.chunk3->chunk_valuelist = result_peg_216;
    stream.update(result_peg_216.getPosition());
    
    
    return result_peg_216;
    out_peg_218:
    
    if (column_peg_215.chunk3 == 0){
        column_peg_215.chunk3 = new Chunk3();
    }
    column_peg_215.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_224 = stream.getColumn(position);
    if (column_peg_224.chunk3 != 0 && column_peg_224.chunk3->chunk_value.calculated()){
        return column_peg_224.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_225(myposition);
    
    result_peg_225 = rule_integer(stream, result_peg_225.getPosition());
    if (result_peg_225.error()){
        goto out_peg_226;
    }
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_225;
    stream.update(result_peg_225.getPosition());
    
    
    return result_peg_225;
    out_peg_226:
    Result result_peg_227(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_227.getPosition()))){
                    result_peg_227.nextPosition();
                } else {
                    goto out_peg_229;
                }
            }
            result_peg_227.setValue((void*) "as");
        
        
        
        result_peg_227 = rule_integer(stream, result_peg_227.getPosition());
            if (result_peg_227.error()){
                goto out_peg_229;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_227.getPosition()))){
                    result_peg_227.nextPosition();
                } else {
                    goto out_peg_229;
                }
            }
            result_peg_227.setValue((void*) "d");
        
        
        
        result_peg_227 = rule_integer(stream, result_peg_227.getPosition());
            if (result_peg_227.error()){
                goto out_peg_229;
            }
        
        
    }
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_227;
    stream.update(result_peg_227.getPosition());
    
    
    return result_peg_227;
    out_peg_229:
    Result result_peg_232(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_232.getPosition()))){
                    result_peg_232.nextPosition();
                } else {
                    goto out_peg_234;
                }
            }
            result_peg_232.setValue((void*) "a");
        
        
        
        result_peg_232 = rule_integer(stream, result_peg_232.getPosition());
            if (result_peg_232.error()){
                goto out_peg_234;
            }
        
        
    }
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_232;
    stream.update(result_peg_232.getPosition());
    
    
    return result_peg_232;
    out_peg_234:
    Result result_peg_235(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("a"[i], stream.get(result_peg_235.getPosition()))){
            result_peg_235.nextPosition();
        } else {
            goto out_peg_236;
        }
    }
    result_peg_235.setValue((void*) "a");
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_235;
    stream.update(result_peg_235.getPosition());
    
    
    return result_peg_235;
    out_peg_236:
    Result result_peg_237(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("s"[i], stream.get(result_peg_237.getPosition()))){
            result_peg_237.nextPosition();
        } else {
            goto out_peg_238;
        }
    }
    result_peg_237.setValue((void*) "s");
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_237;
    stream.update(result_peg_237.getPosition());
    
    
    return result_peg_237;
    out_peg_238:
    Result result_peg_239(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("vh"[i], stream.get(result_peg_239.getPosition()))){
            result_peg_239.nextPosition();
        } else {
            goto out_peg_240;
        }
    }
    result_peg_239.setValue((void*) "vh");
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_239;
    stream.update(result_peg_239.getPosition());
    
    
    return result_peg_239;
    out_peg_240:
    Result result_peg_241(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("hv"[i], stream.get(result_peg_241.getPosition()))){
            result_peg_241.nextPosition();
        } else {
            goto out_peg_242;
        }
    }
    result_peg_241.setValue((void*) "hv");
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_241;
    stream.update(result_peg_241.getPosition());
    
    
    return result_peg_241;
    out_peg_242:
    Result result_peg_243(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("v"[i], stream.get(result_peg_243.getPosition()))){
            result_peg_243.nextPosition();
        } else {
            goto out_peg_244;
        }
    }
    result_peg_243.setValue((void*) "v");
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_243;
    stream.update(result_peg_243.getPosition());
    
    
    return result_peg_243;
    out_peg_244:
    Result result_peg_245(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("h"[i], stream.get(result_peg_245.getPosition()))){
            result_peg_245.nextPosition();
        } else {
            goto out_peg_246;
        }
    }
    result_peg_245.setValue((void*) "h");
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = result_peg_245;
    stream.update(result_peg_245.getPosition());
    
    
    return result_peg_245;
    out_peg_246:
    
    if (column_peg_224.chunk3 == 0){
        column_peg_224.chunk3 = new Chunk3();
    }
    column_peg_224.chunk3->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuev(Stream & stream, const int position){
    
    Column & column_peg_247 = stream.getColumn(position);
    if (column_peg_247.chunk4 != 0 && column_peg_247.chunk4->chunk_valuev.calculated()){
        return column_peg_247.chunk4->chunk_valuev;
    }
    
    int myposition = position;
    
    
    Result result_peg_248(myposition);
    
    result_peg_248 = rule_value(stream, result_peg_248.getPosition());
    if (result_peg_248.error()){
        goto out_peg_249;
    }
    
    if (column_peg_247.chunk4 == 0){
        column_peg_247.chunk4 = new Chunk4();
    }
    column_peg_247.chunk4->chunk_valuev = result_peg_248;
    stream.update(result_peg_248.getPosition());
    
    
    return result_peg_248;
    out_peg_249:
    Result result_peg_250(myposition);
    
    
    
    if (column_peg_247.chunk4 == 0){
        column_peg_247.chunk4 = new Chunk4();
    }
    column_peg_247.chunk4->chunk_valuev = result_peg_250;
    stream.update(result_peg_250.getPosition());
    
    
    return result_peg_250;
    
    if (column_peg_247.chunk4 == 0){
        column_peg_247.chunk4 = new Chunk4();
    }
    column_peg_247.chunk4->chunk_valuev = errorResult;
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

        
