

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
        memo_size = 1024;
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

Ast::Section * makeSection(const Value & str){
  return new Ast::Section(as<std::string*>(str));
}

SectionList * makeSectionList(){
  return new SectionList();
}

Ast::Keyword * makeKeyword(const Value & value){
    return new Ast::Keyword(as<char*>(value));
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
        
        
        
        result_peg_116 = rule_s(stream, result_peg_116.getPosition());
            if (result_peg_116.error()){
                goto out_peg_118;
            }
        
        
        
        result_peg_116 = rule_integer(stream, result_peg_116.getPosition());
            if (result_peg_116.error()){
                goto out_peg_118;
            }
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_116;
    stream.update(result_peg_116.getPosition());
    
    
    return result_peg_116;
    out_peg_118:
    Result result_peg_120(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_120.getPosition()))){
                    result_peg_120.nextPosition();
                } else {
                    goto out_peg_122;
                }
            }
            result_peg_120.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_120 = rule_s(stream, result_peg_120.getPosition());
            if (result_peg_120.error()){
                goto out_peg_122;
            }
        
        
        
        result_peg_120 = rule_integer(stream, result_peg_120.getPosition());
            if (result_peg_120.error()){
                goto out_peg_122;
            }
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_120;
    stream.update(result_peg_120.getPosition());
    
    
    return result_peg_120;
    out_peg_122:
    Result result_peg_124(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_124.getPosition()))){
                    result_peg_124.nextPosition();
                } else {
                    goto out_peg_126;
                }
            }
            result_peg_124.setValue((void*) "Clsn2:");
        
        
        
        result_peg_124 = rule_s(stream, result_peg_124.getPosition());
            if (result_peg_124.error()){
                goto out_peg_126;
            }
        
        
        
        result_peg_124 = rule_integer(stream, result_peg_124.getPosition());
            if (result_peg_124.error()){
                goto out_peg_126;
            }
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_124;
    stream.update(result_peg_124.getPosition());
    
    
    return result_peg_124;
    out_peg_126:
    Result result_peg_128(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_128.getPosition()))){
                    result_peg_128.nextPosition();
                } else {
                    goto out_peg_130;
                }
            }
            result_peg_128.setValue((void*) "Clsn1:");
        
        
        
        result_peg_128 = rule_s(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
        
        
        
        result_peg_128 = rule_integer(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
        
        
    }
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = result_peg_128;
    stream.update(result_peg_128.getPosition());
    
    
    return result_peg_128;
    out_peg_130:
    
    if (column_peg_115.chunk2 == 0){
        column_peg_115.chunk2 = new Chunk2();
    }
    column_peg_115.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_132 = stream.getColumn(position);
    if (column_peg_132.chunk2 != 0 && column_peg_132.chunk2->chunk_collision.calculated()){
        return column_peg_132.chunk2->chunk_collision;
    }
    
    int myposition = position;
    
    
    Result result_peg_133(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) "Clsn2");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) "[");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133.reset();
            do{
                Result result_peg_141(result_peg_133.getPosition());
                result_peg_141 = rule_digit(stream, result_peg_141.getPosition());
                if (result_peg_141.error()){
                    goto loop_peg_140;
                }
                result_peg_133.addResult(result_peg_141);
            } while (true);
            loop_peg_140:
            if (result_peg_133.matches() == 0){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) "]");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) "=");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_integer(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) ",");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_integer(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) ",");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_integer(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_133.getPosition()))){
                    result_peg_133.nextPosition();
                } else {
                    goto out_peg_135;
                }
            }
            result_peg_133.setValue((void*) ",");
        
        
        
        result_peg_133 = rule_s(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
        
        result_peg_133 = rule_integer(stream, result_peg_133.getPosition());
            if (result_peg_133.error()){
                goto out_peg_135;
            }
        
        
    }
    
    if (column_peg_132.chunk2 == 0){
        column_peg_132.chunk2 = new Chunk2();
    }
    column_peg_132.chunk2->chunk_collision = result_peg_133;
    stream.update(result_peg_133.getPosition());
    
    
    return result_peg_133;
    out_peg_135:
    Result result_peg_159(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) "Clsn1");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) "[");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159.reset();
            do{
                Result result_peg_167(result_peg_159.getPosition());
                result_peg_167 = rule_digit(stream, result_peg_167.getPosition());
                if (result_peg_167.error()){
                    goto loop_peg_166;
                }
                result_peg_159.addResult(result_peg_167);
            } while (true);
            loop_peg_166:
            if (result_peg_159.matches() == 0){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) "]");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) "=");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_integer(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) ",");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_integer(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) ",");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_integer(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_159.getPosition()))){
                    result_peg_159.nextPosition();
                } else {
                    goto out_peg_161;
                }
            }
            result_peg_159.setValue((void*) ",");
        
        
        
        result_peg_159 = rule_s(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
        
        result_peg_159 = rule_integer(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
        
        
    }
    
    if (column_peg_132.chunk2 == 0){
        column_peg_132.chunk2 = new Chunk2();
    }
    column_peg_132.chunk2->chunk_collision = result_peg_159;
    stream.update(result_peg_159.getPosition());
    
    
    return result_peg_159;
    out_peg_161:
    
    if (column_peg_132.chunk2 == 0){
        column_peg_132.chunk2 = new Chunk2();
    }
    column_peg_132.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_185 = stream.getColumn(position);
    if (column_peg_185.chunk2 != 0 && column_peg_185.chunk2->chunk_action_start.calculated()){
        return column_peg_185.chunk2->chunk_action_start;
    }
    
    int myposition = position;
    
    
    Result result_peg_186(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_186.getPosition()))){
                    result_peg_186.nextPosition();
                } else {
                    goto out_peg_188;
                }
            }
            result_peg_186.setValue((void*) "[");
        
        
        
        result_peg_186 = rule_s(stream, result_peg_186.getPosition());
            if (result_peg_186.error()){
                goto out_peg_188;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_186.getPosition()))){
                    result_peg_186.nextPosition();
                } else {
                    goto out_peg_188;
                }
            }
            result_peg_186.setValue((void*) "Begin");
        
        
        
        result_peg_186 = rule_s(stream, result_peg_186.getPosition());
            if (result_peg_186.error()){
                goto out_peg_188;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_186.getPosition()))){
                    result_peg_186.nextPosition();
                } else {
                    goto out_peg_188;
                }
            }
            result_peg_186.setValue((void*) "Action");
        
        
        
        result_peg_186 = rule_s(stream, result_peg_186.getPosition());
            if (result_peg_186.error()){
                goto out_peg_188;
            }
        
        
        
        result_peg_186 = rule_integer(stream, result_peg_186.getPosition());
            if (result_peg_186.error()){
                goto out_peg_188;
            }
        
        
        
        result_peg_186 = rule_s(stream, result_peg_186.getPosition());
            if (result_peg_186.error()){
                goto out_peg_188;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_186.getPosition()))){
                    result_peg_186.nextPosition();
                } else {
                    goto out_peg_188;
                }
            }
            result_peg_186.setValue((void*) "]");
        
        
    }
    
    if (column_peg_185.chunk2 == 0){
        column_peg_185.chunk2 = new Chunk2();
    }
    column_peg_185.chunk2->chunk_action_start = result_peg_186;
    stream.update(result_peg_186.getPosition());
    
    
    return result_peg_186;
    out_peg_188:
    
    if (column_peg_185.chunk2 == 0){
        column_peg_185.chunk2 = new Chunk2();
    }
    column_peg_185.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_digit(Stream & stream, const int position){
    
    Column & column_peg_196 = stream.getColumn(position);
    if (column_peg_196.chunk3 != 0 && column_peg_196.chunk3->chunk_digit.calculated()){
        return column_peg_196.chunk3->chunk_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_197(myposition);
    
    char letter_peg_198 = stream.get(result_peg_197.getPosition());
    if (letter_peg_198 != '\0' && strchr("0123456789", letter_peg_198) != NULL){
        result_peg_197.nextPosition();
        result_peg_197.setValue((void*) letter_peg_198);
    } else {
        goto out_peg_199;
    }
    
    if (column_peg_196.chunk3 == 0){
        column_peg_196.chunk3 = new Chunk3();
    }
    column_peg_196.chunk3->chunk_digit = result_peg_197;
    stream.update(result_peg_197.getPosition());
    
    
    return result_peg_197;
    out_peg_199:
    
    if (column_peg_196.chunk3 == 0){
        column_peg_196.chunk3 = new Chunk3();
    }
    column_peg_196.chunk3->chunk_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_200 = stream.getColumn(position);
    if (column_peg_200.chunk3 != 0 && column_peg_200.chunk3->chunk_integer.calculated()){
        return column_peg_200.chunk3->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_201(myposition);
    
    {
    
        int save_peg_203 = result_peg_201.getPosition();
            
            result_peg_201 = rule_sign(stream, result_peg_201.getPosition());
            if (result_peg_201.error()){
                
                result_peg_201 = Result(save_peg_203);
                result_peg_201.setValue((void*) 0);
                
            }
        
        
        
        result_peg_201.reset();
            do{
                Result result_peg_205(result_peg_201.getPosition());
                result_peg_205 = rule_digit(stream, result_peg_205.getPosition());
                if (result_peg_205.error()){
                    goto loop_peg_204;
                }
                result_peg_201.addResult(result_peg_205);
            } while (true);
            loop_peg_204:
            if (result_peg_201.matches() == 0){
                goto out_peg_206;
            }
        
        
    }
    
    if (column_peg_200.chunk3 == 0){
        column_peg_200.chunk3 = new Chunk3();
    }
    column_peg_200.chunk3->chunk_integer = result_peg_201;
    stream.update(result_peg_201.getPosition());
    
    
    return result_peg_201;
    out_peg_206:
    
    if (column_peg_200.chunk3 == 0){
        column_peg_200.chunk3 = new Chunk3();
    }
    column_peg_200.chunk3->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sign(Stream & stream, const int position){
    
    Column & column_peg_207 = stream.getColumn(position);
    if (column_peg_207.chunk3 != 0 && column_peg_207.chunk3->chunk_sign.calculated()){
        return column_peg_207.chunk3->chunk_sign;
    }
    
    int myposition = position;
    
    
    Result result_peg_208(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_208.getPosition()))){
            result_peg_208.nextPosition();
        } else {
            goto out_peg_209;
        }
    }
    result_peg_208.setValue((void*) "-");
    
    if (column_peg_207.chunk3 == 0){
        column_peg_207.chunk3 = new Chunk3();
    }
    column_peg_207.chunk3->chunk_sign = result_peg_208;
    stream.update(result_peg_208.getPosition());
    
    
    return result_peg_208;
    out_peg_209:
    Result result_peg_210(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_210.getPosition()))){
            result_peg_210.nextPosition();
        } else {
            goto out_peg_211;
        }
    }
    result_peg_210.setValue((void*) "+");
    
    if (column_peg_207.chunk3 == 0){
        column_peg_207.chunk3 = new Chunk3();
    }
    column_peg_207.chunk3->chunk_sign = result_peg_210;
    stream.update(result_peg_210.getPosition());
    
    
    return result_peg_210;
    out_peg_211:
    
    if (column_peg_207.chunk3 == 0){
        column_peg_207.chunk3 = new Chunk3();
    }
    column_peg_207.chunk3->chunk_sign = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_212 = stream.getColumn(position);
    if (column_peg_212.chunk3 != 0 && column_peg_212.chunk3->chunk_valuelist.calculated()){
        return column_peg_212.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    
    Result result_peg_213(myposition);
    
    {
    
        result_peg_213 = rule_value(stream, result_peg_213.getPosition());
            if (result_peg_213.error()){
                goto out_peg_215;
            }
        
        
        
        result_peg_213.reset();
            do{
                Result result_peg_217(result_peg_213.getPosition());
                {
                
                    result_peg_217 = rule_s(stream, result_peg_217.getPosition());
                        if (result_peg_217.error()){
                            goto loop_peg_216;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_217.getPosition()))){
                                result_peg_217.nextPosition();
                            } else {
                                goto loop_peg_216;
                            }
                        }
                        result_peg_217.setValue((void*) ",");
                    
                    
                    
                    result_peg_217 = rule_s(stream, result_peg_217.getPosition());
                        if (result_peg_217.error()){
                            goto loop_peg_216;
                        }
                    
                    
                    
                    result_peg_217 = rule_valuev(stream, result_peg_217.getPosition());
                        if (result_peg_217.error()){
                            goto loop_peg_216;
                        }
                    
                    
                }
                result_peg_213.addResult(result_peg_217);
            } while (true);
            loop_peg_216:
            ;
        
        
    }
    
    if (column_peg_212.chunk3 == 0){
        column_peg_212.chunk3 = new Chunk3();
    }
    column_peg_212.chunk3->chunk_valuelist = result_peg_213;
    stream.update(result_peg_213.getPosition());
    
    
    return result_peg_213;
    out_peg_215:
    
    if (column_peg_212.chunk3 == 0){
        column_peg_212.chunk3 = new Chunk3();
    }
    column_peg_212.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_221 = stream.getColumn(position);
    if (column_peg_221.chunk3 != 0 && column_peg_221.chunk3->chunk_value.calculated()){
        return column_peg_221.chunk3->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_222(myposition);
    
    result_peg_222 = rule_integer(stream, result_peg_222.getPosition());
    if (result_peg_222.error()){
        goto out_peg_223;
    }
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_222;
    stream.update(result_peg_222.getPosition());
    
    
    return result_peg_222;
    out_peg_223:
    Result result_peg_224(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_224.getPosition()))){
                    result_peg_224.nextPosition();
                } else {
                    goto out_peg_226;
                }
            }
            result_peg_224.setValue((void*) "as");
        
        
        
        result_peg_224 = rule_integer(stream, result_peg_224.getPosition());
            if (result_peg_224.error()){
                goto out_peg_226;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_224.getPosition()))){
                    result_peg_224.nextPosition();
                } else {
                    goto out_peg_226;
                }
            }
            result_peg_224.setValue((void*) "d");
        
        
        
        result_peg_224 = rule_integer(stream, result_peg_224.getPosition());
            if (result_peg_224.error()){
                goto out_peg_226;
            }
        
        
    }
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_224;
    stream.update(result_peg_224.getPosition());
    
    
    return result_peg_224;
    out_peg_226:
    Result result_peg_229(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_229.getPosition()))){
                    result_peg_229.nextPosition();
                } else {
                    goto out_peg_231;
                }
            }
            result_peg_229.setValue((void*) "a");
        
        
        
        result_peg_229 = rule_integer(stream, result_peg_229.getPosition());
            if (result_peg_229.error()){
                goto out_peg_231;
            }
        
        
    }
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_229;
    stream.update(result_peg_229.getPosition());
    
    
    return result_peg_229;
    out_peg_231:
    Result result_peg_232(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("a"[i], stream.get(result_peg_232.getPosition()))){
            result_peg_232.nextPosition();
        } else {
            goto out_peg_233;
        }
    }
    result_peg_232.setValue((void*) "a");
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_232;
    stream.update(result_peg_232.getPosition());
    
    
    return result_peg_232;
    out_peg_233:
    Result result_peg_234(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("s"[i], stream.get(result_peg_234.getPosition()))){
            result_peg_234.nextPosition();
        } else {
            goto out_peg_235;
        }
    }
    result_peg_234.setValue((void*) "s");
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_234;
    stream.update(result_peg_234.getPosition());
    
    
    return result_peg_234;
    out_peg_235:
    Result result_peg_236(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("vh"[i], stream.get(result_peg_236.getPosition()))){
            result_peg_236.nextPosition();
        } else {
            goto out_peg_237;
        }
    }
    result_peg_236.setValue((void*) "vh");
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_236;
    stream.update(result_peg_236.getPosition());
    
    
    return result_peg_236;
    out_peg_237:
    Result result_peg_238(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("hv"[i], stream.get(result_peg_238.getPosition()))){
            result_peg_238.nextPosition();
        } else {
            goto out_peg_239;
        }
    }
    result_peg_238.setValue((void*) "hv");
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_238;
    stream.update(result_peg_238.getPosition());
    
    
    return result_peg_238;
    out_peg_239:
    Result result_peg_240(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("v"[i], stream.get(result_peg_240.getPosition()))){
            result_peg_240.nextPosition();
        } else {
            goto out_peg_241;
        }
    }
    result_peg_240.setValue((void*) "v");
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_240;
    stream.update(result_peg_240.getPosition());
    
    
    return result_peg_240;
    out_peg_241:
    Result result_peg_242(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("h"[i], stream.get(result_peg_242.getPosition()))){
            result_peg_242.nextPosition();
        } else {
            goto out_peg_243;
        }
    }
    result_peg_242.setValue((void*) "h");
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = result_peg_242;
    stream.update(result_peg_242.getPosition());
    
    
    return result_peg_242;
    out_peg_243:
    
    if (column_peg_221.chunk3 == 0){
        column_peg_221.chunk3 = new Chunk3();
    }
    column_peg_221.chunk3->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuev(Stream & stream, const int position){
    
    Column & column_peg_244 = stream.getColumn(position);
    if (column_peg_244.chunk4 != 0 && column_peg_244.chunk4->chunk_valuev.calculated()){
        return column_peg_244.chunk4->chunk_valuev;
    }
    
    int myposition = position;
    
    
    Result result_peg_245(myposition);
    
    result_peg_245 = rule_value(stream, result_peg_245.getPosition());
    if (result_peg_245.error()){
        goto out_peg_246;
    }
    
    if (column_peg_244.chunk4 == 0){
        column_peg_244.chunk4 = new Chunk4();
    }
    column_peg_244.chunk4->chunk_valuev = result_peg_245;
    stream.update(result_peg_245.getPosition());
    
    
    return result_peg_245;
    out_peg_246:
    Result result_peg_247(myposition);
    
    
    
    if (column_peg_244.chunk4 == 0){
        column_peg_244.chunk4 = new Chunk4();
    }
    column_peg_244.chunk4->chunk_valuev = result_peg_247;
    stream.update(result_peg_247.getPosition());
    
    
    return result_peg_247;
    
    if (column_peg_244.chunk4 == 0){
        column_peg_244.chunk4 = new Chunk4();
    }
    column_peg_244.chunk4->chunk_valuev = errorResult;
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



    
    } /* Air */
    
} /* Mugen */

        
