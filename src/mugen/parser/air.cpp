

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
        if (position >= memo_size){
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
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("loopstart"[i], stream.get(result_peg_61.getPosition()))){
            result_peg_61.nextPosition();
        } else {
            goto out_peg_62;
        }
    }
    result_peg_61.setValue((void*) "loopstart");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_loopstart = result_peg_61;
        stream.update(result_peg_61.getPosition());
    }
    
    
    return result_peg_61;
    out_peg_62:
    
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
    
    
    Result result_peg_63(myposition);
    
    result_peg_63.reset();
    do{
        Result result_peg_65(result_peg_63.getPosition());
        result_peg_65 = rule_sw(stream, result_peg_65.getPosition());
        if (result_peg_65.error()){
            goto loop_peg_64;
        }
        result_peg_63.addResult(result_peg_65);
    } while (true);
    loop_peg_64:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_whitespace = result_peg_63;
        stream.update(result_peg_63.getPosition());
    }
    
    
    return result_peg_63;
    
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
    
    
    Result result_peg_66(myposition);
    
    result_peg_66 = rule_space(stream, result_peg_66.getPosition());
    if (result_peg_66.error()){
        goto out_peg_67;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = result_peg_66;
        stream.update(result_peg_66.getPosition());
    }
    
    
    return result_peg_66;
    out_peg_67:
    Result result_peg_68(myposition);
    
    result_peg_68 = rule_comment(stream, result_peg_68.getPosition());
    if (result_peg_68.error()){
        goto out_peg_69;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = result_peg_68;
        stream.update(result_peg_68.getPosition());
    }
    
    
    return result_peg_68;
    out_peg_69:
    
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
    
    
    Result result_peg_70(myposition);
    
    result_peg_70.reset();
    do{
        Result result_peg_72(result_peg_70.getPosition());
        result_peg_72 = rule_space(stream, result_peg_72.getPosition());
        if (result_peg_72.error()){
            goto loop_peg_71;
        }
        result_peg_70.addResult(result_peg_72);
    } while (true);
    loop_peg_71:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_s = result_peg_70;
        stream.update(result_peg_70.getPosition());
    }
    
    
    return result_peg_70;
    
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
    
    
    Result result_peg_73(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
            result_peg_73.nextPosition();
        } else {
            goto out_peg_74;
        }
    }
    result_peg_73.setValue((void*) " ");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = result_peg_73;
        stream.update(result_peg_73.getPosition());
    }
    
    
    return result_peg_73;
    out_peg_74:
    Result result_peg_75(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_75.getPosition()))){
            result_peg_75.nextPosition();
        } else {
            goto out_peg_76;
        }
    }
    result_peg_75.setValue((void*) "\t");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = result_peg_75;
        stream.update(result_peg_75.getPosition());
    }
    
    
    return result_peg_75;
    out_peg_76:
    Result result_peg_77(myposition);
    
    if ((unsigned char) stream.get(result_peg_77.getPosition()) == (unsigned char) 255){
        result_peg_77.nextPosition();
    } else {
        goto out_peg_78;
    }
    result_peg_77.setValue((void*) 255);
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_space = result_peg_77;
        stream.update(result_peg_77.getPosition());
    }
    
    
    return result_peg_77;
    out_peg_78:
    
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
    
    
    Result result_peg_79(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_79.getPosition()))){
                    result_peg_79.nextPosition();
                } else {
                    goto out_peg_81;
                }
            }
            result_peg_79.setValue((void*) ";");
        
        
        
        result_peg_79.reset();
            do{
                Result result_peg_83(result_peg_79.getPosition());
                {
                
                    Result result_peg_86(result_peg_83);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_86.getPosition()))){
                                result_peg_86.nextPosition();
                            } else {
                                goto not_peg_85;
                            }
                        }
                        result_peg_86.setValue((void*) "\n");
                        goto loop_peg_82;
                        not_peg_85:
                        result_peg_83.setValue((void*)0);
                    
                    
                    
                    char temp_peg_87 = stream.get(result_peg_83.getPosition());
                        if (temp_peg_87 != '\0'){
                            result_peg_83.setValue((void*) temp_peg_87);
                            result_peg_83.nextPosition();
                        } else {
                            goto loop_peg_82;
                        }
                    
                    
                }
                result_peg_79.addResult(result_peg_83);
            } while (true);
            loop_peg_82:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = result_peg_79;
        stream.update(result_peg_79.getPosition());
    }
    
    
    return result_peg_79;
    out_peg_81:
    Result result_peg_88(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_88.getPosition()))){
                    result_peg_88.nextPosition();
                } else {
                    goto out_peg_90;
                }
            }
            result_peg_88.setValue((void*) "=");
        
        
        
        result_peg_88.reset();
            do{
                Result result_peg_92(result_peg_88.getPosition());
                {
                
                    Result result_peg_95(result_peg_92);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_95.getPosition()))){
                                result_peg_95.nextPosition();
                            } else {
                                goto not_peg_94;
                            }
                        }
                        result_peg_95.setValue((void*) "\n");
                        goto loop_peg_91;
                        not_peg_94:
                        result_peg_92.setValue((void*)0);
                    
                    
                    
                    char temp_peg_96 = stream.get(result_peg_92.getPosition());
                        if (temp_peg_96 != '\0'){
                            result_peg_92.setValue((void*) temp_peg_96);
                            result_peg_92.nextPosition();
                        } else {
                            goto loop_peg_91;
                        }
                    
                    
                }
                result_peg_88.addResult(result_peg_92);
            } while (true);
            loop_peg_91:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = result_peg_88;
        stream.update(result_peg_88.getPosition());
    }
    
    
    return result_peg_88;
    out_peg_90:
    Result result_peg_97(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_97.getPosition()))){
                    result_peg_97.nextPosition();
                } else {
                    goto out_peg_99;
                }
            }
            result_peg_97.setValue((void*) "-");
        
        
        
        result_peg_97.reset();
            do{
                Result result_peg_101(result_peg_97.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_101.getPosition()))){
                        result_peg_101.nextPosition();
                    } else {
                        goto loop_peg_100;
                    }
                }
                result_peg_101.setValue((void*) "-");
                result_peg_97.addResult(result_peg_101);
            } while (true);
            loop_peg_100:
            if (result_peg_97.matches() == 0){
                goto out_peg_99;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_comment = result_peg_97;
        stream.update(result_peg_97.getPosition());
    }
    
    
    return result_peg_97;
    out_peg_99:
    
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
    
    
    Result result_peg_102(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_102.getPosition()))){
                    result_peg_102.nextPosition();
                } else {
                    goto out_peg_104;
                }
            }
            result_peg_102.setValue((void*) "Clsn2Default:");
        
        
        
        result_peg_102 = rule_s(stream, result_peg_102.getPosition());
            if (result_peg_102.error()){
                goto out_peg_104;
            }
        
        
        
        result_peg_102 = rule_integer(stream, result_peg_102.getPosition());
            if (result_peg_102.error()){
                goto out_peg_104;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_102;
        stream.update(result_peg_102.getPosition());
    }
    
    
    return result_peg_102;
    out_peg_104:
    Result result_peg_106(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_106.getPosition()))){
                    result_peg_106.nextPosition();
                } else {
                    goto out_peg_108;
                }
            }
            result_peg_106.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_106 = rule_s(stream, result_peg_106.getPosition());
            if (result_peg_106.error()){
                goto out_peg_108;
            }
        
        
        
        result_peg_106 = rule_integer(stream, result_peg_106.getPosition());
            if (result_peg_106.error()){
                goto out_peg_108;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_106;
        stream.update(result_peg_106.getPosition());
    }
    
    
    return result_peg_106;
    out_peg_108:
    Result result_peg_110(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_110.getPosition()))){
                    result_peg_110.nextPosition();
                } else {
                    goto out_peg_112;
                }
            }
            result_peg_110.setValue((void*) "Clsn2:");
        
        
        
        result_peg_110 = rule_s(stream, result_peg_110.getPosition());
            if (result_peg_110.error()){
                goto out_peg_112;
            }
        
        
        
        result_peg_110 = rule_integer(stream, result_peg_110.getPosition());
            if (result_peg_110.error()){
                goto out_peg_112;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_110;
        stream.update(result_peg_110.getPosition());
    }
    
    
    return result_peg_110;
    out_peg_112:
    Result result_peg_114(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_114.getPosition()))){
                    result_peg_114.nextPosition();
                } else {
                    goto out_peg_116;
                }
            }
            result_peg_114.setValue((void*) "Clsn1:");
        
        
        
        result_peg_114 = rule_s(stream, result_peg_114.getPosition());
            if (result_peg_114.error()){
                goto out_peg_116;
            }
        
        
        
        result_peg_114 = rule_integer(stream, result_peg_114.getPosition());
            if (result_peg_114.error()){
                goto out_peg_116;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_114;
        stream.update(result_peg_114.getPosition());
    }
    
    
    return result_peg_114;
    out_peg_116:
    
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
    
    
    Result result_peg_118(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "Clsn2");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "[");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118.reset();
            do{
                Result result_peg_126(result_peg_118.getPosition());
                result_peg_126 = rule_digit(stream, result_peg_126.getPosition());
                if (result_peg_126.error()){
                    goto loop_peg_125;
                }
                result_peg_118.addResult(result_peg_126);
            } while (true);
            loop_peg_125:
            if (result_peg_118.matches() == 0){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "]");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "=");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_integer(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) ",");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_integer(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) ",");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_integer(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) ",");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_integer(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision = result_peg_118;
        stream.update(result_peg_118.getPosition());
    }
    
    
    return result_peg_118;
    out_peg_120:
    Result result_peg_144(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_144.getPosition()))){
                    result_peg_144.nextPosition();
                } else {
                    goto out_peg_146;
                }
            }
            result_peg_144.setValue((void*) "Clsn1");
        
        
        
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
        
        
        
        result_peg_144.reset();
            do{
                Result result_peg_152(result_peg_144.getPosition());
                result_peg_152 = rule_digit(stream, result_peg_152.getPosition());
                if (result_peg_152.error()){
                    goto loop_peg_151;
                }
                result_peg_144.addResult(result_peg_152);
            } while (true);
            loop_peg_151:
            if (result_peg_144.matches() == 0){
                goto out_peg_146;
            }
        
        
        
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
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision = result_peg_144;
        stream.update(result_peg_144.getPosition());
    }
    
    
    return result_peg_144;
    out_peg_146:
    
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
    
    
    Result result_peg_170(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_170.getPosition()))){
                    result_peg_170.nextPosition();
                } else {
                    goto out_peg_172;
                }
            }
            result_peg_170.setValue((void*) "[");
        
        
        
        result_peg_170 = rule_s(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_170.getPosition()))){
                    result_peg_170.nextPosition();
                } else {
                    goto out_peg_172;
                }
            }
            result_peg_170.setValue((void*) "Begin");
        
        
        
        result_peg_170 = rule_s(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_170.getPosition()))){
                    result_peg_170.nextPosition();
                } else {
                    goto out_peg_172;
                }
            }
            result_peg_170.setValue((void*) "Action");
        
        
        
        result_peg_170 = rule_s(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        result_peg_170 = rule_integer(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        result_peg_170 = rule_s(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_170.getPosition()))){
                    result_peg_170.nextPosition();
                } else {
                    goto out_peg_172;
                }
            }
            result_peg_170.setValue((void*) "]");
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_action_start = result_peg_170;
        stream.update(result_peg_170.getPosition());
    }
    
    
    return result_peg_170;
    out_peg_172:
    
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
    
    
    Result result_peg_180(myposition);
    
    char letter_peg_181 = stream.get(result_peg_180.getPosition());
    if (letter_peg_181 != '\0' && strchr("0123456789", letter_peg_181) != NULL){
        result_peg_180.nextPosition();
        result_peg_180.setValue((void*) letter_peg_181);
    } else {
        goto out_peg_182;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_digit = result_peg_180;
        stream.update(result_peg_180.getPosition());
    }
    
    
    return result_peg_180;
    out_peg_182:
    
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
    
    
    Result result_peg_183(myposition);
    
    {
    
        int save_peg_185 = result_peg_183.getPosition();
            
            result_peg_183 = rule_sign(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                
                result_peg_183 = Result(save_peg_185);
                result_peg_183.setValue((void*) 0);
                
            }
        
        
        
        result_peg_183.reset();
            do{
                Result result_peg_187(result_peg_183.getPosition());
                result_peg_187 = rule_digit(stream, result_peg_187.getPosition());
                if (result_peg_187.error()){
                    goto loop_peg_186;
                }
                result_peg_183.addResult(result_peg_187);
            } while (true);
            loop_peg_186:
            if (result_peg_183.matches() == 0){
                goto out_peg_188;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_integer = result_peg_183;
        stream.update(result_peg_183.getPosition());
    }
    
    
    return result_peg_183;
    out_peg_188:
    
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
    
    
    Result result_peg_189(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_189.getPosition()))){
            result_peg_189.nextPosition();
        } else {
            goto out_peg_190;
        }
    }
    result_peg_189.setValue((void*) "-");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = result_peg_189;
        stream.update(result_peg_189.getPosition());
    }
    
    
    return result_peg_189;
    out_peg_190:
    Result result_peg_191(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_191.getPosition()))){
            result_peg_191.nextPosition();
        } else {
            goto out_peg_192;
        }
    }
    result_peg_191.setValue((void*) "+");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = result_peg_191;
        stream.update(result_peg_191.getPosition());
    }
    
    
    return result_peg_191;
    out_peg_192:
    
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
    
    
    Result result_peg_193(myposition);
    
    {
    
        result_peg_193 = rule_value(stream, result_peg_193.getPosition());
            if (result_peg_193.error()){
                goto out_peg_195;
            }
        
        
        
        result_peg_193.reset();
            do{
                Result result_peg_197(result_peg_193.getPosition());
                {
                
                    result_peg_197 = rule_s(stream, result_peg_197.getPosition());
                        if (result_peg_197.error()){
                            goto loop_peg_196;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_197.getPosition()))){
                                result_peg_197.nextPosition();
                            } else {
                                goto loop_peg_196;
                            }
                        }
                        result_peg_197.setValue((void*) ",");
                    
                    
                    
                    result_peg_197 = rule_s(stream, result_peg_197.getPosition());
                        if (result_peg_197.error()){
                            goto loop_peg_196;
                        }
                    
                    
                    
                    result_peg_197 = rule_valuev(stream, result_peg_197.getPosition());
                        if (result_peg_197.error()){
                            goto loop_peg_196;
                        }
                    
                    
                }
                result_peg_193.addResult(result_peg_197);
            } while (true);
            loop_peg_196:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_193;
        stream.update(result_peg_193.getPosition());
    }
    
    
    return result_peg_193;
    out_peg_195:
    
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
    
    
    Result result_peg_201(myposition);
    
    result_peg_201 = rule_integer(stream, result_peg_201.getPosition());
    if (result_peg_201.error()){
        goto out_peg_202;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_201;
        stream.update(result_peg_201.getPosition());
    }
    
    
    return result_peg_201;
    out_peg_202:
    Result result_peg_203(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_203.getPosition()))){
                    result_peg_203.nextPosition();
                } else {
                    goto out_peg_205;
                }
            }
            result_peg_203.setValue((void*) "as");
        
        
        
        result_peg_203 = rule_integer(stream, result_peg_203.getPosition());
            if (result_peg_203.error()){
                goto out_peg_205;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_203.getPosition()))){
                    result_peg_203.nextPosition();
                } else {
                    goto out_peg_205;
                }
            }
            result_peg_203.setValue((void*) "d");
        
        
        
        result_peg_203 = rule_integer(stream, result_peg_203.getPosition());
            if (result_peg_203.error()){
                goto out_peg_205;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_203;
        stream.update(result_peg_203.getPosition());
    }
    
    
    return result_peg_203;
    out_peg_205:
    Result result_peg_208(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_208.getPosition()))){
                    result_peg_208.nextPosition();
                } else {
                    goto out_peg_210;
                }
            }
            result_peg_208.setValue((void*) "a");
        
        
        
        result_peg_208 = rule_integer(stream, result_peg_208.getPosition());
            if (result_peg_208.error()){
                goto out_peg_210;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_208;
        stream.update(result_peg_208.getPosition());
    }
    
    
    return result_peg_208;
    out_peg_210:
    Result result_peg_211(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("a"[i], stream.get(result_peg_211.getPosition()))){
            result_peg_211.nextPosition();
        } else {
            goto out_peg_212;
        }
    }
    result_peg_211.setValue((void*) "a");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_211;
        stream.update(result_peg_211.getPosition());
    }
    
    
    return result_peg_211;
    out_peg_212:
    Result result_peg_213(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("s"[i], stream.get(result_peg_213.getPosition()))){
            result_peg_213.nextPosition();
        } else {
            goto out_peg_214;
        }
    }
    result_peg_213.setValue((void*) "s");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_213;
        stream.update(result_peg_213.getPosition());
    }
    
    
    return result_peg_213;
    out_peg_214:
    Result result_peg_215(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("vh"[i], stream.get(result_peg_215.getPosition()))){
            result_peg_215.nextPosition();
        } else {
            goto out_peg_216;
        }
    }
    result_peg_215.setValue((void*) "vh");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_215;
        stream.update(result_peg_215.getPosition());
    }
    
    
    return result_peg_215;
    out_peg_216:
    Result result_peg_217(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("hv"[i], stream.get(result_peg_217.getPosition()))){
            result_peg_217.nextPosition();
        } else {
            goto out_peg_218;
        }
    }
    result_peg_217.setValue((void*) "hv");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_217;
        stream.update(result_peg_217.getPosition());
    }
    
    
    return result_peg_217;
    out_peg_218:
    Result result_peg_219(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("v"[i], stream.get(result_peg_219.getPosition()))){
            result_peg_219.nextPosition();
        } else {
            goto out_peg_220;
        }
    }
    result_peg_219.setValue((void*) "v");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_219;
        stream.update(result_peg_219.getPosition());
    }
    
    
    return result_peg_219;
    out_peg_220:
    Result result_peg_221(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("h"[i], stream.get(result_peg_221.getPosition()))){
            result_peg_221.nextPosition();
        } else {
            goto out_peg_222;
        }
    }
    result_peg_221.setValue((void*) "h");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_221;
        stream.update(result_peg_221.getPosition());
    }
    
    
    return result_peg_221;
    out_peg_222:
    
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
    
    
    Result result_peg_223(myposition);
    
    result_peg_223 = rule_value(stream, result_peg_223.getPosition());
    if (result_peg_223.error()){
        goto out_peg_224;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_valuev = result_peg_223;
        stream.update(result_peg_223.getPosition());
    }
    
    
    return result_peg_223;
    out_peg_224:
    Result result_peg_225(myposition);
    
    
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk4 == 0){
            column.chunk4 = new Chunk4();
        }
        column.chunk4->chunk_valuev = result_peg_225;
        stream.update(result_peg_225.getPosition());
    }
    
    
    return result_peg_225;
    
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

        
