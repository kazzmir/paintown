

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
        
        
        
        result_peg_1 = rule_whitespace(stream, result_peg_1.getPosition());
            if (result_peg_1.error()){
                goto out_peg_4;
            }
        
        
        
        result_peg_1.reset();
            do{
                Result result_peg_7(result_peg_1.getPosition());
                result_peg_7 = rule_newline(stream, result_peg_7.getPosition());
                if (result_peg_7.error()){
                    goto loop_peg_6;
                }
                result_peg_1.addResult(result_peg_7);
            } while (true);
            loop_peg_6:
            ;
        
        
        
        result_peg_1.reset();
            do{
                Result result_peg_10(result_peg_1.getPosition());
                {
                
                    result_peg_10 = rule_line(stream, result_peg_10.getPosition(), current);
                        if (result_peg_10.error()){
                            goto loop_peg_9;
                        }
                    
                    
                    
                    result_peg_10 = rule_whitespace(stream, result_peg_10.getPosition());
                        if (result_peg_10.error()){
                            goto loop_peg_9;
                        }
                    
                    
                    
                    int save_peg_13 = result_peg_10.getPosition();
                        
                        result_peg_10 = rule_line_end(stream, result_peg_10.getPosition());
                        if (result_peg_10.error()){
                            
                            result_peg_10 = Result(save_peg_13);
                            result_peg_10.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_1.addResult(result_peg_10);
            } while (true);
            loop_peg_9:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_1.getPosition())){
                result_peg_1.nextPosition();
                result_peg_1.setValue((void *) '\0');
            } else {
                goto out_peg_4;
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
    out_peg_4:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_start = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
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
    
    
    Result result_peg_15(myposition);
    
    {
    
        result_peg_15 = rule_s(stream, result_peg_15.getPosition());
            if (result_peg_15.error()){
                goto out_peg_17;
            }
        
        
        
        result_peg_15 = rule_comment(stream, result_peg_15.getPosition());
            if (result_peg_15.error()){
                goto out_peg_17;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_15;
        stream.update(result_peg_15.getPosition());
    }
    
    
    return result_peg_15;
    out_peg_17:
    Result result_peg_18(myposition);
    
    {
    
        result_peg_18 = rule_s(stream, result_peg_18.getPosition());
            if (result_peg_18.error()){
                goto out_peg_20;
            }
        
        
        
        result_peg_18 = rule_action(stream, result_peg_18.getPosition());
            if (result_peg_18.error()){
                goto out_peg_20;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_18;
        stream.update(result_peg_18.getPosition());
    }
    
    
    return result_peg_18;
    out_peg_20:
    
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
        

Result rule_action(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_action.calculated()){
            return column.chunk0->chunk_action;
        }
    }
    
    int myposition = position;
    
    Value ast;
    Result result_peg_21(myposition);
    
    {
    
        result_peg_21 = rule_action_start(stream, result_peg_21.getPosition());
            if (result_peg_21.error()){
                goto out_peg_23;
            }
        
        Result result_peg_22 = result_peg_21;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_22.getValues()); GC::save(as<Ast::Section*>(value));
                result_peg_21.setValue(value);
            }
            ast = result_peg_21.getValues();
        
        
        
        result_peg_21 = rule_whitespace(stream, result_peg_21.getPosition());
            if (result_peg_21.error()){
                goto out_peg_23;
            }
        
        
        
        result_peg_21.reset();
            do{
                Result result_peg_28(result_peg_21.getPosition());
                result_peg_28 = rule_newline(stream, result_peg_28.getPosition());
                if (result_peg_28.error()){
                    goto loop_peg_27;
                }
                result_peg_21.addResult(result_peg_28);
            } while (true);
            loop_peg_27:
            if (result_peg_21.matches() == 0){
                goto out_peg_23;
            }
        
        
        
        result_peg_21.reset();
            do{
                Result result_peg_31(result_peg_21.getPosition());
                {
                
                    result_peg_31 = rule_action_line(stream, result_peg_31.getPosition());
                        if (result_peg_31.error()){
                            goto loop_peg_30;
                        }
                    
                    
                    
                    result_peg_31 = rule_whitespace(stream, result_peg_31.getPosition());
                        if (result_peg_31.error()){
                            goto loop_peg_30;
                        }
                    
                    
                    
                    result_peg_31 = rule_line_end(stream, result_peg_31.getPosition());
                        if (result_peg_31.error()){
                            goto loop_peg_30;
                        }
                    
                    
                }
                result_peg_21.addResult(result_peg_31);
            } while (true);
            loop_peg_30:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_21.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action = result_peg_21;
        stream.update(result_peg_21.getPosition());
    }
    
    
    return result_peg_21;
    out_peg_23:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_action_line.calculated()){
            return column.chunk0->chunk_action_line;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_34(myposition);
    
    {
    
        result_peg_34 = rule_s(stream, result_peg_34.getPosition());
            if (result_peg_34.error()){
                goto out_peg_36;
            }
        
        
        
        result_peg_34 = rule_comment(stream, result_peg_34.getPosition());
            if (result_peg_34.error()){
                goto out_peg_36;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action_line = result_peg_34;
        stream.update(result_peg_34.getPosition());
    }
    
    
    return result_peg_34;
    out_peg_36:
    Result result_peg_37(myposition);
    
    {
    
        result_peg_37 = rule_s(stream, result_peg_37.getPosition());
            if (result_peg_37.error()){
                goto out_peg_39;
            }
        
        
        
        result_peg_37 = rule_collision_default(stream, result_peg_37.getPosition());
            if (result_peg_37.error()){
                goto out_peg_39;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action_line = result_peg_37;
        stream.update(result_peg_37.getPosition());
    }
    
    
    return result_peg_37;
    out_peg_39:
    Result result_peg_40(myposition);
    
    {
    
        result_peg_40 = rule_s(stream, result_peg_40.getPosition());
            if (result_peg_40.error()){
                goto out_peg_42;
            }
        
        
        
        result_peg_40 = rule_collision(stream, result_peg_40.getPosition());
            if (result_peg_40.error()){
                goto out_peg_42;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action_line = result_peg_40;
        stream.update(result_peg_40.getPosition());
    }
    
    
    return result_peg_40;
    out_peg_42:
    Result result_peg_43(myposition);
    
    {
    
        result_peg_43 = rule_s(stream, result_peg_43.getPosition());
            if (result_peg_43.error()){
                goto out_peg_45;
            }
        
        
        
        result_peg_43 = rule_valuelist(stream, result_peg_43.getPosition());
            if (result_peg_43.error()){
                goto out_peg_45;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action_line = result_peg_43;
        stream.update(result_peg_43.getPosition());
    }
    
    
    return result_peg_43;
    out_peg_45:
    Result result_peg_46(myposition);
    
    {
    
        result_peg_46 = rule_s(stream, result_peg_46.getPosition());
            if (result_peg_46.error()){
                goto out_peg_48;
            }
        
        
        
        result_peg_46 = rule_loopstart(stream, result_peg_46.getPosition());
            if (result_peg_46.error()){
                goto out_peg_48;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action_line = result_peg_46;
        stream.update(result_peg_46.getPosition());
    }
    
    
    return result_peg_46;
    out_peg_48:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_action_line = errorResult;
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
    
    
    Result result_peg_49(myposition);
    
    result_peg_49.reset();
    do{
        Result result_peg_51(result_peg_49.getPosition());
        result_peg_51 = rule_newline(stream, result_peg_51.getPosition());
        if (result_peg_51.error()){
            goto loop_peg_50;
        }
        result_peg_49.addResult(result_peg_51);
    } while (true);
    loop_peg_50:
    if (result_peg_49.matches() == 0){
        goto out_peg_52;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_49;
        stream.update(result_peg_49.getPosition());
    }
    
    
    return result_peg_49;
    out_peg_52:
    Result result_peg_53(myposition);
    
    {
    
        Result result_peg_55(result_peg_53.getPosition());
            if ('\0' == stream.get(result_peg_55.getPosition())){
                result_peg_55.nextPosition();
                result_peg_55.setValue((void *) '\0');
            } else {
                goto out_peg_56;
            }
        
        
        
        
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_53;
        stream.update(result_peg_53.getPosition());
    }
    
    
    return result_peg_53;
    out_peg_56:
    
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
        

Result rule_newline(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_newline.calculated()){
            return column.chunk1->chunk_newline;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_57(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_57.getPosition()))){
            result_peg_57.nextPosition();
        } else {
            goto out_peg_58;
        }
    }
    result_peg_57.setValue((void*) "\n");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_newline = result_peg_57;
        stream.update(result_peg_57.getPosition());
    }
    
    
    return result_peg_57;
    out_peg_58:
    Result result_peg_59(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_59.getPosition()))){
            result_peg_59.nextPosition();
        } else {
            goto out_peg_60;
        }
    }
    result_peg_59.setValue((void*) "\r");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_newline = result_peg_59;
        stream.update(result_peg_59.getPosition());
    }
    
    
    return result_peg_59;
    out_peg_60:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_newline = errorResult;
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
    
    
    Result result_peg_61(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_61.getPosition()))){
                    result_peg_61.nextPosition();
                } else {
                    goto out_peg_63;
                }
            }
            result_peg_61.setValue((void*) "loopstart");
        
        Result result_peg_62 = result_peg_61;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_62.getValues()); GC::save(as<Ast::Keyword*>(value));
                result_peg_61.setValue(value);
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = result_peg_61;
        stream.update(result_peg_61.getPosition());
    }
    
    
    return result_peg_61;
    out_peg_63:
    
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
        

Result rule_whitespace(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_whitespace.calculated()){
            return column.chunk1->chunk_whitespace;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_64(myposition);
    
    result_peg_64.reset();
    do{
        Result result_peg_66(result_peg_64.getPosition());
        result_peg_66 = rule_sw(stream, result_peg_66.getPosition());
        if (result_peg_66.error()){
            goto loop_peg_65;
        }
        result_peg_64.addResult(result_peg_66);
    } while (true);
    loop_peg_65:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_whitespace = result_peg_64;
        stream.update(result_peg_64.getPosition());
    }
    
    
    return result_peg_64;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_sw.calculated()){
            return column.chunk1->chunk_sw;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_67(myposition);
    
    result_peg_67 = rule_space(stream, result_peg_67.getPosition());
    if (result_peg_67.error()){
        goto out_peg_68;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = result_peg_67;
        stream.update(result_peg_67.getPosition());
    }
    
    
    return result_peg_67;
    out_peg_68:
    Result result_peg_69(myposition);
    
    result_peg_69 = rule_comment(stream, result_peg_69.getPosition());
    if (result_peg_69.error()){
        goto out_peg_70;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = result_peg_69;
        stream.update(result_peg_69.getPosition());
    }
    
    
    return result_peg_69;
    out_peg_70:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_s.calculated()){
            return column.chunk1->chunk_s;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_71(myposition);
    
    result_peg_71.reset();
    do{
        Result result_peg_73(result_peg_71.getPosition());
        result_peg_73 = rule_space(stream, result_peg_73.getPosition());
        if (result_peg_73.error()){
            goto loop_peg_72;
        }
        result_peg_71.addResult(result_peg_73);
    } while (true);
    loop_peg_72:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_s = result_peg_71;
        stream.update(result_peg_71.getPosition());
    }
    
    
    return result_peg_71;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_s = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_space(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_space.calculated()){
            return column.chunk2->chunk_space;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_74(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_74.getPosition()))){
            result_peg_74.nextPosition();
        } else {
            goto out_peg_75;
        }
    }
    result_peg_74.setValue((void*) " ");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = result_peg_74;
        stream.update(result_peg_74.getPosition());
    }
    
    
    return result_peg_74;
    out_peg_75:
    Result result_peg_76(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_76.getPosition()))){
            result_peg_76.nextPosition();
        } else {
            goto out_peg_77;
        }
    }
    result_peg_76.setValue((void*) "\t");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = result_peg_76;
        stream.update(result_peg_76.getPosition());
    }
    
    
    return result_peg_76;
    out_peg_77:
    Result result_peg_78(myposition);
    
    if ((unsigned char) stream.get(result_peg_78.getPosition()) == (unsigned char) 255){
        result_peg_78.nextPosition();
    } else {
        goto out_peg_79;
    }
    result_peg_78.setValue((void*) 255);
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = result_peg_78;
        stream.update(result_peg_78.getPosition());
    }
    
    
    return result_peg_78;
    out_peg_79:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_comment.calculated()){
            return column.chunk2->chunk_comment;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_80(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_80.getPosition()))){
                    result_peg_80.nextPosition();
                } else {
                    goto out_peg_82;
                }
            }
            result_peg_80.setValue((void*) ";");
        
        
        
        result_peg_80.reset();
            do{
                Result result_peg_84(result_peg_80.getPosition());
                {
                
                    Result result_peg_87(result_peg_84);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_87.getPosition()))){
                                result_peg_87.nextPosition();
                            } else {
                                goto not_peg_86;
                            }
                        }
                        result_peg_87.setValue((void*) "\n");
                        goto loop_peg_83;
                        not_peg_86:
                        result_peg_84.setValue((void*)0);
                    
                    
                    
                    char temp_peg_88 = stream.get(result_peg_84.getPosition());
                        if (temp_peg_88 != '\0'){
                            result_peg_84.setValue((void*) temp_peg_88);
                            result_peg_84.nextPosition();
                        } else {
                            goto loop_peg_83;
                        }
                    
                    
                }
                result_peg_80.addResult(result_peg_84);
            } while (true);
            loop_peg_83:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = result_peg_80;
        stream.update(result_peg_80.getPosition());
    }
    
    
    return result_peg_80;
    out_peg_82:
    Result result_peg_89(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_89.getPosition()))){
                    result_peg_89.nextPosition();
                } else {
                    goto out_peg_91;
                }
            }
            result_peg_89.setValue((void*) "=");
        
        
        
        result_peg_89.reset();
            do{
                Result result_peg_93(result_peg_89.getPosition());
                {
                
                    Result result_peg_96(result_peg_93);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_96.getPosition()))){
                                result_peg_96.nextPosition();
                            } else {
                                goto not_peg_95;
                            }
                        }
                        result_peg_96.setValue((void*) "\n");
                        goto loop_peg_92;
                        not_peg_95:
                        result_peg_93.setValue((void*)0);
                    
                    
                    
                    char temp_peg_97 = stream.get(result_peg_93.getPosition());
                        if (temp_peg_97 != '\0'){
                            result_peg_93.setValue((void*) temp_peg_97);
                            result_peg_93.nextPosition();
                        } else {
                            goto loop_peg_92;
                        }
                    
                    
                }
                result_peg_89.addResult(result_peg_93);
            } while (true);
            loop_peg_92:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = result_peg_89;
        stream.update(result_peg_89.getPosition());
    }
    
    
    return result_peg_89;
    out_peg_91:
    Result result_peg_98(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_98.getPosition()))){
                    result_peg_98.nextPosition();
                } else {
                    goto out_peg_100;
                }
            }
            result_peg_98.setValue((void*) "-");
        
        
        
        result_peg_98.reset();
            do{
                Result result_peg_102(result_peg_98.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_102.getPosition()))){
                        result_peg_102.nextPosition();
                    } else {
                        goto loop_peg_101;
                    }
                }
                result_peg_102.setValue((void*) "-");
                result_peg_98.addResult(result_peg_102);
            } while (true);
            loop_peg_101:
            if (result_peg_98.matches() == 0){
                goto out_peg_100;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = result_peg_98;
        stream.update(result_peg_98.getPosition());
    }
    
    
    return result_peg_98;
    out_peg_100:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_collision_default.calculated()){
            return column.chunk2->chunk_collision_default;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_103(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) "Clsn2Default:");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_integer(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_103;
        stream.update(result_peg_103.getPosition());
    }
    
    
    return result_peg_103;
    out_peg_105:
    Result result_peg_107(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_107.getPosition()))){
                    result_peg_107.nextPosition();
                } else {
                    goto out_peg_109;
                }
            }
            result_peg_107.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_107 = rule_s(stream, result_peg_107.getPosition());
            if (result_peg_107.error()){
                goto out_peg_109;
            }
        
        
        
        result_peg_107 = rule_integer(stream, result_peg_107.getPosition());
            if (result_peg_107.error()){
                goto out_peg_109;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_107;
        stream.update(result_peg_107.getPosition());
    }
    
    
    return result_peg_107;
    out_peg_109:
    Result result_peg_111(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_111.getPosition()))){
                    result_peg_111.nextPosition();
                } else {
                    goto out_peg_113;
                }
            }
            result_peg_111.setValue((void*) "Clsn2:");
        
        
        
        result_peg_111 = rule_s(stream, result_peg_111.getPosition());
            if (result_peg_111.error()){
                goto out_peg_113;
            }
        
        
        
        result_peg_111 = rule_integer(stream, result_peg_111.getPosition());
            if (result_peg_111.error()){
                goto out_peg_113;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_111;
        stream.update(result_peg_111.getPosition());
    }
    
    
    return result_peg_111;
    out_peg_113:
    Result result_peg_115(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_115.getPosition()))){
                    result_peg_115.nextPosition();
                } else {
                    goto out_peg_117;
                }
            }
            result_peg_115.setValue((void*) "Clsn1:");
        
        
        
        result_peg_115 = rule_s(stream, result_peg_115.getPosition());
            if (result_peg_115.error()){
                goto out_peg_117;
            }
        
        
        
        result_peg_115 = rule_integer(stream, result_peg_115.getPosition());
            if (result_peg_115.error()){
                goto out_peg_117;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_115;
        stream.update(result_peg_115.getPosition());
    }
    
    
    return result_peg_115;
    out_peg_117:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_collision.calculated()){
            return column.chunk2->chunk_collision;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_119(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) "Clsn2");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) "[");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119.reset();
            do{
                Result result_peg_127(result_peg_119.getPosition());
                result_peg_127 = rule_digit(stream, result_peg_127.getPosition());
                if (result_peg_127.error()){
                    goto loop_peg_126;
                }
                result_peg_119.addResult(result_peg_127);
            } while (true);
            loop_peg_126:
            if (result_peg_119.matches() == 0){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) "]");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) "=");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_integer(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) ",");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_integer(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) ",");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_integer(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) ",");
        
        
        
        result_peg_119 = rule_s(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
        
        result_peg_119 = rule_integer(stream, result_peg_119.getPosition());
            if (result_peg_119.error()){
                goto out_peg_121;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision = result_peg_119;
        stream.update(result_peg_119.getPosition());
    }
    
    
    return result_peg_119;
    out_peg_121:
    Result result_peg_145(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "Clsn1");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "[");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145.reset();
            do{
                Result result_peg_153(result_peg_145.getPosition());
                result_peg_153 = rule_digit(stream, result_peg_153.getPosition());
                if (result_peg_153.error()){
                    goto loop_peg_152;
                }
                result_peg_145.addResult(result_peg_153);
            } while (true);
            loop_peg_152:
            if (result_peg_145.matches() == 0){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "]");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "=");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_integer(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) ",");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_integer(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) ",");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_integer(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) ",");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_integer(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision = result_peg_145;
        stream.update(result_peg_145.getPosition());
    }
    
    
    return result_peg_145;
    out_peg_147:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_action_start.calculated()){
            return column.chunk2->chunk_action_start;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_171(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_171.getPosition()))){
                    result_peg_171.nextPosition();
                } else {
                    goto out_peg_173;
                }
            }
            result_peg_171.setValue((void*) "[");
        
        
        
        result_peg_171 = rule_s(stream, result_peg_171.getPosition());
            if (result_peg_171.error()){
                goto out_peg_173;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_171.getPosition()))){
                    result_peg_171.nextPosition();
                } else {
                    goto out_peg_173;
                }
            }
            result_peg_171.setValue((void*) "Begin");
        
        
        
        result_peg_171 = rule_s(stream, result_peg_171.getPosition());
            if (result_peg_171.error()){
                goto out_peg_173;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_171.getPosition()))){
                    result_peg_171.nextPosition();
                } else {
                    goto out_peg_173;
                }
            }
            result_peg_171.setValue((void*) "Action");
        
        
        
        result_peg_171 = rule_s(stream, result_peg_171.getPosition());
            if (result_peg_171.error()){
                goto out_peg_173;
            }
        
        
        
        result_peg_171 = rule_integer(stream, result_peg_171.getPosition());
            if (result_peg_171.error()){
                goto out_peg_173;
            }
        
        
        
        result_peg_171 = rule_s(stream, result_peg_171.getPosition());
            if (result_peg_171.error()){
                goto out_peg_173;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_171.getPosition()))){
                    result_peg_171.nextPosition();
                } else {
                    goto out_peg_173;
                }
            }
            result_peg_171.setValue((void*) "]");
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_action_start = result_peg_171;
        stream.update(result_peg_171.getPosition());
    }
    
    
    return result_peg_171;
    out_peg_173:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_action_start = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_digit(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_digit.calculated()){
            return column.chunk3->chunk_digit;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_181(myposition);
    
    char letter_peg_182 = stream.get(result_peg_181.getPosition());
    if (letter_peg_182 != '\0' && strchr("0123456789", letter_peg_182) != NULL){
        result_peg_181.nextPosition();
        result_peg_181.setValue((void*) letter_peg_182);
    } else {
        goto out_peg_183;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_digit = result_peg_181;
        stream.update(result_peg_181.getPosition());
    }
    
    
    return result_peg_181;
    out_peg_183:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_digit = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_integer.calculated()){
            return column.chunk3->chunk_integer;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_184(myposition);
    
    {
    
        int save_peg_186 = result_peg_184.getPosition();
            
            result_peg_184 = rule_sign(stream, result_peg_184.getPosition());
            if (result_peg_184.error()){
                
                result_peg_184 = Result(save_peg_186);
                result_peg_184.setValue((void*) 0);
                
            }
        
        
        
        result_peg_184.reset();
            do{
                Result result_peg_188(result_peg_184.getPosition());
                result_peg_188 = rule_digit(stream, result_peg_188.getPosition());
                if (result_peg_188.error()){
                    goto loop_peg_187;
                }
                result_peg_184.addResult(result_peg_188);
            } while (true);
            loop_peg_187:
            if (result_peg_184.matches() == 0){
                goto out_peg_189;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_integer = result_peg_184;
        stream.update(result_peg_184.getPosition());
    }
    
    
    return result_peg_184;
    out_peg_189:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_sign(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 != 0 && column.chunk3->chunk_sign.calculated()){
            return column.chunk3->chunk_sign;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_190(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_190.getPosition()))){
            result_peg_190.nextPosition();
        } else {
            goto out_peg_191;
        }
    }
    result_peg_190.setValue((void*) "-");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = result_peg_190;
        stream.update(result_peg_190.getPosition());
    }
    
    
    return result_peg_190;
    out_peg_191:
    Result result_peg_192(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_192.getPosition()))){
            result_peg_192.nextPosition();
        } else {
            goto out_peg_193;
        }
    }
    result_peg_192.setValue((void*) "+");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = result_peg_192;
        stream.update(result_peg_192.getPosition());
    }
    
    
    return result_peg_192;
    out_peg_193:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = errorResult;
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
    
    
    Result result_peg_194(myposition);
    
    {
    
        result_peg_194 = rule_value(stream, result_peg_194.getPosition());
            if (result_peg_194.error()){
                goto out_peg_196;
            }
        
        
        
        result_peg_194.reset();
            do{
                Result result_peg_198(result_peg_194.getPosition());
                {
                
                    result_peg_198 = rule_s(stream, result_peg_198.getPosition());
                        if (result_peg_198.error()){
                            goto loop_peg_197;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_198.getPosition()))){
                                result_peg_198.nextPosition();
                            } else {
                                goto loop_peg_197;
                            }
                        }
                        result_peg_198.setValue((void*) ",");
                    
                    
                    
                    result_peg_198 = rule_s(stream, result_peg_198.getPosition());
                        if (result_peg_198.error()){
                            goto loop_peg_197;
                        }
                    
                    
                    
                    result_peg_198 = rule_valuev(stream, result_peg_198.getPosition());
                        if (result_peg_198.error()){
                            goto loop_peg_197;
                        }
                    
                    
                }
                result_peg_194.addResult(result_peg_198);
            } while (true);
            loop_peg_197:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_194;
        stream.update(result_peg_194.getPosition());
    }
    
    
    return result_peg_194;
    out_peg_196:
    
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
    
    
    Result result_peg_202(myposition);
    
    result_peg_202 = rule_integer(stream, result_peg_202.getPosition());
    if (result_peg_202.error()){
        goto out_peg_203;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_202;
        stream.update(result_peg_202.getPosition());
    }
    
    
    return result_peg_202;
    out_peg_203:
    Result result_peg_204(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_204.getPosition()))){
                    result_peg_204.nextPosition();
                } else {
                    goto out_peg_206;
                }
            }
            result_peg_204.setValue((void*) "as");
        
        
        
        result_peg_204 = rule_integer(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_204.getPosition()))){
                    result_peg_204.nextPosition();
                } else {
                    goto out_peg_206;
                }
            }
            result_peg_204.setValue((void*) "d");
        
        
        
        result_peg_204 = rule_integer(stream, result_peg_204.getPosition());
            if (result_peg_204.error()){
                goto out_peg_206;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_204;
        stream.update(result_peg_204.getPosition());
    }
    
    
    return result_peg_204;
    out_peg_206:
    Result result_peg_209(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_209.getPosition()))){
                    result_peg_209.nextPosition();
                } else {
                    goto out_peg_211;
                }
            }
            result_peg_209.setValue((void*) "a");
        
        
        
        result_peg_209 = rule_integer(stream, result_peg_209.getPosition());
            if (result_peg_209.error()){
                goto out_peg_211;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_209;
        stream.update(result_peg_209.getPosition());
    }
    
    
    return result_peg_209;
    out_peg_211:
    Result result_peg_212(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("a"[i], stream.get(result_peg_212.getPosition()))){
            result_peg_212.nextPosition();
        } else {
            goto out_peg_213;
        }
    }
    result_peg_212.setValue((void*) "a");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_212;
        stream.update(result_peg_212.getPosition());
    }
    
    
    return result_peg_212;
    out_peg_213:
    Result result_peg_214(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("s"[i], stream.get(result_peg_214.getPosition()))){
            result_peg_214.nextPosition();
        } else {
            goto out_peg_215;
        }
    }
    result_peg_214.setValue((void*) "s");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_214;
        stream.update(result_peg_214.getPosition());
    }
    
    
    return result_peg_214;
    out_peg_215:
    Result result_peg_216(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("vh"[i], stream.get(result_peg_216.getPosition()))){
            result_peg_216.nextPosition();
        } else {
            goto out_peg_217;
        }
    }
    result_peg_216.setValue((void*) "vh");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_216;
        stream.update(result_peg_216.getPosition());
    }
    
    
    return result_peg_216;
    out_peg_217:
    Result result_peg_218(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("hv"[i], stream.get(result_peg_218.getPosition()))){
            result_peg_218.nextPosition();
        } else {
            goto out_peg_219;
        }
    }
    result_peg_218.setValue((void*) "hv");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_218;
        stream.update(result_peg_218.getPosition());
    }
    
    
    return result_peg_218;
    out_peg_219:
    Result result_peg_220(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("v"[i], stream.get(result_peg_220.getPosition()))){
            result_peg_220.nextPosition();
        } else {
            goto out_peg_221;
        }
    }
    result_peg_220.setValue((void*) "v");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_220;
        stream.update(result_peg_220.getPosition());
    }
    
    
    return result_peg_220;
    out_peg_221:
    Result result_peg_222(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("h"[i], stream.get(result_peg_222.getPosition()))){
            result_peg_222.nextPosition();
        } else {
            goto out_peg_223;
        }
    }
    result_peg_222.setValue((void*) "h");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_222;
        stream.update(result_peg_222.getPosition());
    }
    
    
    return result_peg_222;
    out_peg_223:
    
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
        

Result rule_valuev(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 != 0 && column.chunk4->chunk_valuev.calculated()){
            return column.chunk4->chunk_valuev;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_224(myposition);
    
    result_peg_224 = rule_value(stream, result_peg_224.getPosition());
    if (result_peg_224.error()){
        goto out_peg_225;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_valuev = result_peg_224;
        stream.update(result_peg_224.getPosition());
    }
    
    
    return result_peg_224;
    out_peg_225:
    Result result_peg_226(myposition);
    
    
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_valuev = result_peg_226;
        stream.update(result_peg_226.getPosition());
    }
    
    
    return result_peg_226;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_valuev = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
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

        
