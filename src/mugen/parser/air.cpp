

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
    Result chunk_line_end;
    Result chunk_newline;
    Result chunk_loopstart;
};

struct Chunk1{
    Result chunk_whitespace;
    Result chunk_sw;
    Result chunk_s;
    Result chunk_space;
    Result chunk_comment;
};

struct Chunk2{
    Result chunk_collision_default;
    Result chunk_collision;
    Result chunk_action_section;
    Result chunk_digit;
    Result chunk_integer;
};

struct Chunk3{
    Result chunk_sign;
    Result chunk_valuelist;
    Result chunk_value;
    Result chunk_valuev;
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
        memo_size = 1024;
        memo = new Column*[memo_size];
        for (int i = 0; i < memo_size; i++){
            memo[i] = new Column();
        }
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
Result rule_line(Stream &, const int);
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
Result rule_action_section(Stream &, const int);
Result rule_digit(Stream &, const int);
Result rule_integer(Stream &, const int);
Result rule_sign(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_valuev(Stream &, const int);




Result rule_start(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_start.calculated()){
            return column.chunk0->chunk_start;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_1(myposition);
    
    {
    
        result_peg_1 = rule_whitespace(stream, result_peg_1.getPosition());
            if (result_peg_1.error()){
                goto out_peg_3;
            }
        
        
        
        result_peg_1.reset();
            do{
                Result result_peg_6(result_peg_1.getPosition());
                result_peg_6 = rule_newline(stream, result_peg_6.getPosition());
                if (result_peg_6.error()){
                    goto loop_peg_5;
                }
                result_peg_1.addResult(result_peg_6);
            } while (true);
            loop_peg_5:
            ;
        
        
        
        result_peg_1.reset();
            do{
                Result result_peg_9(result_peg_1.getPosition());
                {
                
                    result_peg_9 = rule_line(stream, result_peg_9.getPosition());
                        if (result_peg_9.error()){
                            goto loop_peg_8;
                        }
                    
                    
                    
                    result_peg_9 = rule_whitespace(stream, result_peg_9.getPosition());
                        if (result_peg_9.error()){
                            goto loop_peg_8;
                        }
                    
                    
                    
                    result_peg_9 = rule_line_end(stream, result_peg_9.getPosition());
                        if (result_peg_9.error()){
                            goto loop_peg_8;
                        }
                    
                    
                }
                result_peg_1.addResult(result_peg_9);
            } while (true);
            loop_peg_8:
            ;
        
        
        
        if ('\0' == stream.get(result_peg_1.getPosition())){
                result_peg_1.nextPosition();
                result_peg_1.setValue((void *) '\0');
            } else {
                goto out_peg_3;
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
    out_peg_3:
    
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
        

Result rule_line(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_line.calculated()){
            return column.chunk0->chunk_line;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_12(myposition);
    
    {
    
        result_peg_12 = rule_s(stream, result_peg_12.getPosition());
            if (result_peg_12.error()){
                goto out_peg_14;
            }
        
        
        
        result_peg_12 = rule_comment(stream, result_peg_12.getPosition());
            if (result_peg_12.error()){
                goto out_peg_14;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_12;
        stream.update(result_peg_12.getPosition());
    }
    
    
    return result_peg_12;
    out_peg_14:
    Result result_peg_15(myposition);
    
    {
    
        result_peg_15 = rule_s(stream, result_peg_15.getPosition());
            if (result_peg_15.error()){
                goto out_peg_17;
            }
        
        
        
        result_peg_15 = rule_action_section(stream, result_peg_15.getPosition());
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
        
        
        
        result_peg_18 = rule_collision_default(stream, result_peg_18.getPosition());
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
    Result result_peg_21(myposition);
    
    {
    
        result_peg_21 = rule_s(stream, result_peg_21.getPosition());
            if (result_peg_21.error()){
                goto out_peg_23;
            }
        
        
        
        result_peg_21 = rule_collision(stream, result_peg_21.getPosition());
            if (result_peg_21.error()){
                goto out_peg_23;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_21;
        stream.update(result_peg_21.getPosition());
    }
    
    
    return result_peg_21;
    out_peg_23:
    Result result_peg_24(myposition);
    
    {
    
        result_peg_24 = rule_s(stream, result_peg_24.getPosition());
            if (result_peg_24.error()){
                goto out_peg_26;
            }
        
        
        
        result_peg_24 = rule_valuelist(stream, result_peg_24.getPosition());
            if (result_peg_24.error()){
                goto out_peg_26;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_24;
        stream.update(result_peg_24.getPosition());
    }
    
    
    return result_peg_24;
    out_peg_26:
    Result result_peg_27(myposition);
    
    {
    
        result_peg_27 = rule_s(stream, result_peg_27.getPosition());
            if (result_peg_27.error()){
                goto out_peg_29;
            }
        
        
        
        result_peg_27 = rule_loopstart(stream, result_peg_27.getPosition());
            if (result_peg_27.error()){
                goto out_peg_29;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_27;
        stream.update(result_peg_27.getPosition());
    }
    
    
    return result_peg_27;
    out_peg_29:
    Result result_peg_30(myposition);
    
    result_peg_30.reset();
    do{
        Result result_peg_32(result_peg_30.getPosition());
        result_peg_32 = rule_space(stream, result_peg_32.getPosition());
        if (result_peg_32.error()){
            goto loop_peg_31;
        }
        result_peg_30.addResult(result_peg_32);
    } while (true);
    loop_peg_31:
    if (result_peg_30.matches() == 0){
        goto out_peg_33;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line = result_peg_30;
        stream.update(result_peg_30.getPosition());
    }
    
    
    return result_peg_30;
    out_peg_33:
    
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
        

Result rule_line_end(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_line_end.calculated()){
            return column.chunk0->chunk_line_end;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_34(myposition);
    
    result_peg_34.reset();
    do{
        Result result_peg_36(result_peg_34.getPosition());
        result_peg_36 = rule_newline(stream, result_peg_36.getPosition());
        if (result_peg_36.error()){
            goto loop_peg_35;
        }
        result_peg_34.addResult(result_peg_36);
    } while (true);
    loop_peg_35:
    if (result_peg_34.matches() == 0){
        goto out_peg_37;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_34;
        stream.update(result_peg_34.getPosition());
    }
    
    
    return result_peg_34;
    out_peg_37:
    Result result_peg_38(myposition);
    
    {
    
        Result result_peg_40(result_peg_38.getPosition());
            if ('\0' == stream.get(result_peg_40.getPosition())){
                result_peg_40.nextPosition();
                result_peg_40.setValue((void *) '\0');
            } else {
                goto out_peg_41;
            }
        
        
        
        
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_line_end = result_peg_38;
        stream.update(result_peg_38.getPosition());
    }
    
    
    return result_peg_38;
    out_peg_41:
    
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
        if (column.chunk0 != 0 && column.chunk0->chunk_newline.calculated()){
            return column.chunk0->chunk_newline;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_42(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
            result_peg_42.nextPosition();
        } else {
            goto out_peg_43;
        }
    }
    result_peg_42.setValue((void*) "\n");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_newline = result_peg_42;
        stream.update(result_peg_42.getPosition());
    }
    
    
    return result_peg_42;
    out_peg_43:
    Result result_peg_44(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_44.getPosition()))){
            result_peg_44.nextPosition();
        } else {
            goto out_peg_45;
        }
    }
    result_peg_44.setValue((void*) "\r");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_newline = result_peg_44;
        stream.update(result_peg_44.getPosition());
    }
    
    
    return result_peg_44;
    out_peg_45:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_newline = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 != 0 && column.chunk0->chunk_loopstart.calculated()){
            return column.chunk0->chunk_loopstart;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_46(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("loopstart"[i], stream.get(result_peg_46.getPosition()))){
            result_peg_46.nextPosition();
        } else {
            goto out_peg_47;
        }
    }
    result_peg_46.setValue((void*) "loopstart");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_loopstart = result_peg_46;
        stream.update(result_peg_46.getPosition());
    }
    
    
    return result_peg_46;
    out_peg_47:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk0 == 0){
            column.chunk0 = new Chunk0();
        }
        column.chunk0->chunk_loopstart = errorResult;
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
    
    
    Result result_peg_48(myposition);
    
    result_peg_48.reset();
    do{
        Result result_peg_50(result_peg_48.getPosition());
        result_peg_50 = rule_sw(stream, result_peg_50.getPosition());
        if (result_peg_50.error()){
            goto loop_peg_49;
        }
        result_peg_48.addResult(result_peg_50);
    } while (true);
    loop_peg_49:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_whitespace = result_peg_48;
        stream.update(result_peg_48.getPosition());
    }
    
    
    return result_peg_48;
    
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
    
    
    Result result_peg_51(myposition);
    
    result_peg_51 = rule_space(stream, result_peg_51.getPosition());
    if (result_peg_51.error()){
        goto out_peg_52;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = result_peg_51;
        stream.update(result_peg_51.getPosition());
    }
    
    
    return result_peg_51;
    out_peg_52:
    Result result_peg_53(myposition);
    
    result_peg_53 = rule_comment(stream, result_peg_53.getPosition());
    if (result_peg_53.error()){
        goto out_peg_54;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_sw = result_peg_53;
        stream.update(result_peg_53.getPosition());
    }
    
    
    return result_peg_53;
    out_peg_54:
    
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
    
    
    Result result_peg_55(myposition);
    
    result_peg_55.reset();
    do{
        Result result_peg_57(result_peg_55.getPosition());
        result_peg_57 = rule_space(stream, result_peg_57.getPosition());
        if (result_peg_57.error()){
            goto loop_peg_56;
        }
        result_peg_55.addResult(result_peg_57);
    } while (true);
    loop_peg_56:
    ;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_s = result_peg_55;
        stream.update(result_peg_55.getPosition());
    }
    
    
    return result_peg_55;
    
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
        if (column.chunk1 != 0 && column.chunk1->chunk_space.calculated()){
            return column.chunk1->chunk_space;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_58(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar(" "[i], stream.get(result_peg_58.getPosition()))){
            result_peg_58.nextPosition();
        } else {
            goto out_peg_59;
        }
    }
    result_peg_58.setValue((void*) " ");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_space = result_peg_58;
        stream.update(result_peg_58.getPosition());
    }
    
    
    return result_peg_58;
    out_peg_59:
    Result result_peg_60(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\t"[i], stream.get(result_peg_60.getPosition()))){
            result_peg_60.nextPosition();
        } else {
            goto out_peg_61;
        }
    }
    result_peg_60.setValue((void*) "\t");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_space = result_peg_60;
        stream.update(result_peg_60.getPosition());
    }
    
    
    return result_peg_60;
    out_peg_61:
    Result result_peg_62(myposition);
    
    if ((unsigned char) stream.get(result_peg_62.getPosition()) == (unsigned char) 255){
        result_peg_62.nextPosition();
    } else {
        goto out_peg_63;
    }
    result_peg_62.setValue((void*) 255);
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_space = result_peg_62;
        stream.update(result_peg_62.getPosition());
    }
    
    
    return result_peg_62;
    out_peg_63:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_space = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 != 0 && column.chunk1->chunk_comment.calculated()){
            return column.chunk1->chunk_comment;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_64(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_64.getPosition()))){
                    result_peg_64.nextPosition();
                } else {
                    goto out_peg_66;
                }
            }
            result_peg_64.setValue((void*) ";");
        
        
        
        result_peg_64.reset();
            do{
                Result result_peg_68(result_peg_64.getPosition());
                {
                
                    Result result_peg_71(result_peg_68);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_71.getPosition()))){
                                result_peg_71.nextPosition();
                            } else {
                                goto not_peg_70;
                            }
                        }
                        result_peg_71.setValue((void*) "\n");
                        goto loop_peg_67;
                        not_peg_70:
                        result_peg_68.setValue((void*)0);
                    
                    
                    
                    char temp_peg_72 = stream.get(result_peg_68.getPosition());
                        if (temp_peg_72 != '\0'){
                            result_peg_68.setValue((void*) temp_peg_72);
                            result_peg_68.nextPosition();
                        } else {
                            goto loop_peg_67;
                        }
                    
                    
                }
                result_peg_64.addResult(result_peg_68);
            } while (true);
            loop_peg_67:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_comment = result_peg_64;
        stream.update(result_peg_64.getPosition());
    }
    
    
    return result_peg_64;
    out_peg_66:
    Result result_peg_73(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_73.getPosition()))){
                    result_peg_73.nextPosition();
                } else {
                    goto out_peg_75;
                }
            }
            result_peg_73.setValue((void*) "=");
        
        
        
        result_peg_73.reset();
            do{
                Result result_peg_77(result_peg_73.getPosition());
                {
                
                    Result result_peg_80(result_peg_77);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_80.getPosition()))){
                                result_peg_80.nextPosition();
                            } else {
                                goto not_peg_79;
                            }
                        }
                        result_peg_80.setValue((void*) "\n");
                        goto loop_peg_76;
                        not_peg_79:
                        result_peg_77.setValue((void*)0);
                    
                    
                    
                    char temp_peg_81 = stream.get(result_peg_77.getPosition());
                        if (temp_peg_81 != '\0'){
                            result_peg_77.setValue((void*) temp_peg_81);
                            result_peg_77.nextPosition();
                        } else {
                            goto loop_peg_76;
                        }
                    
                    
                }
                result_peg_73.addResult(result_peg_77);
            } while (true);
            loop_peg_76:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_comment = result_peg_73;
        stream.update(result_peg_73.getPosition());
    }
    
    
    return result_peg_73;
    out_peg_75:
    Result result_peg_82(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_82.getPosition()))){
                    result_peg_82.nextPosition();
                } else {
                    goto out_peg_84;
                }
            }
            result_peg_82.setValue((void*) "-");
        
        
        
        result_peg_82.reset();
            do{
                Result result_peg_86(result_peg_82.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_86.getPosition()))){
                        result_peg_86.nextPosition();
                    } else {
                        goto loop_peg_85;
                    }
                }
                result_peg_86.setValue((void*) "-");
                result_peg_82.addResult(result_peg_86);
            } while (true);
            loop_peg_85:
            if (result_peg_82.matches() == 0){
                goto out_peg_84;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_comment = result_peg_82;
        stream.update(result_peg_82.getPosition());
    }
    
    
    return result_peg_82;
    out_peg_84:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk1 == 0){
            column.chunk1 = new Chunk1();
        }
        column.chunk1->chunk_comment = errorResult;
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
    
    
    Result result_peg_87(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn2Default:"[i], stream.get(result_peg_87.getPosition()))){
                    result_peg_87.nextPosition();
                } else {
                    goto out_peg_89;
                }
            }
            result_peg_87.setValue((void*) "Clsn2Default:");
        
        
        
        result_peg_87 = rule_s(stream, result_peg_87.getPosition());
            if (result_peg_87.error()){
                goto out_peg_89;
            }
        
        
        
        result_peg_87 = rule_integer(stream, result_peg_87.getPosition());
            if (result_peg_87.error()){
                goto out_peg_89;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_87;
        stream.update(result_peg_87.getPosition());
    }
    
    
    return result_peg_87;
    out_peg_89:
    Result result_peg_91(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("Clsn1Default:"[i], stream.get(result_peg_91.getPosition()))){
                    result_peg_91.nextPosition();
                } else {
                    goto out_peg_93;
                }
            }
            result_peg_91.setValue((void*) "Clsn1Default:");
        
        
        
        result_peg_91 = rule_s(stream, result_peg_91.getPosition());
            if (result_peg_91.error()){
                goto out_peg_93;
            }
        
        
        
        result_peg_91 = rule_integer(stream, result_peg_91.getPosition());
            if (result_peg_91.error()){
                goto out_peg_93;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_91;
        stream.update(result_peg_91.getPosition());
    }
    
    
    return result_peg_91;
    out_peg_93:
    Result result_peg_95(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn2:"[i], stream.get(result_peg_95.getPosition()))){
                    result_peg_95.nextPosition();
                } else {
                    goto out_peg_97;
                }
            }
            result_peg_95.setValue((void*) "Clsn2:");
        
        
        
        result_peg_95 = rule_s(stream, result_peg_95.getPosition());
            if (result_peg_95.error()){
                goto out_peg_97;
            }
        
        
        
        result_peg_95 = rule_integer(stream, result_peg_95.getPosition());
            if (result_peg_95.error()){
                goto out_peg_97;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_95;
        stream.update(result_peg_95.getPosition());
    }
    
    
    return result_peg_95;
    out_peg_97:
    Result result_peg_99(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Clsn1:"[i], stream.get(result_peg_99.getPosition()))){
                    result_peg_99.nextPosition();
                } else {
                    goto out_peg_101;
                }
            }
            result_peg_99.setValue((void*) "Clsn1:");
        
        
        
        result_peg_99 = rule_s(stream, result_peg_99.getPosition());
            if (result_peg_99.error()){
                goto out_peg_101;
            }
        
        
        
        result_peg_99 = rule_integer(stream, result_peg_99.getPosition());
            if (result_peg_99.error()){
                goto out_peg_101;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision_default = result_peg_99;
        stream.update(result_peg_99.getPosition());
    }
    
    
    return result_peg_99;
    out_peg_101:
    
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
    
    
    Result result_peg_103(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) "Clsn2");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) "[");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103.reset();
            do{
                Result result_peg_111(result_peg_103.getPosition());
                result_peg_111 = rule_digit(stream, result_peg_111.getPosition());
                if (result_peg_111.error()){
                    goto loop_peg_110;
                }
                result_peg_103.addResult(result_peg_111);
            } while (true);
            loop_peg_110:
            if (result_peg_103.matches() == 0){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) "]");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) "=");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_integer(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) ",");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_integer(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) ",");
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_integer(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103 = rule_s(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_103.getPosition()))){
                    result_peg_103.nextPosition();
                } else {
                    goto out_peg_105;
                }
            }
            result_peg_103.setValue((void*) ",");
        
        
        
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
        column.chunk2->chunk_collision = result_peg_103;
        stream.update(result_peg_103.getPosition());
    }
    
    
    return result_peg_103;
    out_peg_105:
    Result result_peg_129(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) "Clsn1");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) "[");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129.reset();
            do{
                Result result_peg_137(result_peg_129.getPosition());
                result_peg_137 = rule_digit(stream, result_peg_137.getPosition());
                if (result_peg_137.error()){
                    goto loop_peg_136;
                }
                result_peg_129.addResult(result_peg_137);
            } while (true);
            loop_peg_136:
            if (result_peg_129.matches() == 0){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) "]");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) "=");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_integer(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) ",");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_integer(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) ",");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_integer(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_129.getPosition()))){
                    result_peg_129.nextPosition();
                } else {
                    goto out_peg_131;
                }
            }
            result_peg_129.setValue((void*) ",");
        
        
        
        result_peg_129 = rule_s(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
        
        result_peg_129 = rule_integer(stream, result_peg_129.getPosition());
            if (result_peg_129.error()){
                goto out_peg_131;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_collision = result_peg_129;
        stream.update(result_peg_129.getPosition());
    }
    
    
    return result_peg_129;
    out_peg_131:
    
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
        

Result rule_action_section(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_action_section.calculated()){
            return column.chunk2->chunk_action_section;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_155(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_155.getPosition()))){
                    result_peg_155.nextPosition();
                } else {
                    goto out_peg_157;
                }
            }
            result_peg_155.setValue((void*) "[");
        
        
        
        result_peg_155 = rule_s(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_155.getPosition()))){
                    result_peg_155.nextPosition();
                } else {
                    goto out_peg_157;
                }
            }
            result_peg_155.setValue((void*) "Begin");
        
        
        
        result_peg_155 = rule_s(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_155.getPosition()))){
                    result_peg_155.nextPosition();
                } else {
                    goto out_peg_157;
                }
            }
            result_peg_155.setValue((void*) "Action");
        
        
        
        result_peg_155 = rule_s(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        result_peg_155 = rule_integer(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        result_peg_155 = rule_s(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_155.getPosition()))){
                    result_peg_155.nextPosition();
                } else {
                    goto out_peg_157;
                }
            }
            result_peg_155.setValue((void*) "]");
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_action_section = result_peg_155;
        stream.update(result_peg_155.getPosition());
    }
    
    
    return result_peg_155;
    out_peg_157:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_action_section = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_digit(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_digit.calculated()){
            return column.chunk2->chunk_digit;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_165(myposition);
    
    char letter_peg_166 = stream.get(result_peg_165.getPosition());
    if (letter_peg_166 != '\0' && strchr("0123456789", letter_peg_166) != NULL){
        result_peg_165.nextPosition();
        result_peg_165.setValue((void*) letter_peg_166);
    } else {
        goto out_peg_167;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_digit = result_peg_165;
        stream.update(result_peg_165.getPosition());
    }
    
    
    return result_peg_165;
    out_peg_167:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_digit = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 != 0 && column.chunk2->chunk_integer.calculated()){
            return column.chunk2->chunk_integer;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_168(myposition);
    
    {
    
        int save_peg_170 = result_peg_168.getPosition();
            
            result_peg_168 = rule_sign(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                
                result_peg_168 = Result(save_peg_170);
                result_peg_168.setValue((void*) 0);
                
            }
        
        
        
        result_peg_168.reset();
            do{
                Result result_peg_172(result_peg_168.getPosition());
                result_peg_172 = rule_digit(stream, result_peg_172.getPosition());
                if (result_peg_172.error()){
                    goto loop_peg_171;
                }
                result_peg_168.addResult(result_peg_172);
            } while (true);
            loop_peg_171:
            if (result_peg_168.matches() == 0){
                goto out_peg_173;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_integer = result_peg_168;
        stream.update(result_peg_168.getPosition());
    }
    
    
    return result_peg_168;
    out_peg_173:
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk2 == 0){
            column.chunk2 = new Chunk2();
        }
        column.chunk2->chunk_integer = errorResult;
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
    
    
    Result result_peg_174(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("-"[i], stream.get(result_peg_174.getPosition()))){
            result_peg_174.nextPosition();
        } else {
            goto out_peg_175;
        }
    }
    result_peg_174.setValue((void*) "-");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = result_peg_174;
        stream.update(result_peg_174.getPosition());
    }
    
    
    return result_peg_174;
    out_peg_175:
    Result result_peg_176(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("+"[i], stream.get(result_peg_176.getPosition()))){
            result_peg_176.nextPosition();
        } else {
            goto out_peg_177;
        }
    }
    result_peg_176.setValue((void*) "+");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_sign = result_peg_176;
        stream.update(result_peg_176.getPosition());
    }
    
    
    return result_peg_176;
    out_peg_177:
    
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
    
    
    Result result_peg_178(myposition);
    
    {
    
        result_peg_178 = rule_value(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
        
        
        
        result_peg_178.reset();
            do{
                Result result_peg_182(result_peg_178.getPosition());
                {
                
                    result_peg_182 = rule_s(stream, result_peg_182.getPosition());
                        if (result_peg_182.error()){
                            goto loop_peg_181;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_182.getPosition()))){
                                result_peg_182.nextPosition();
                            } else {
                                goto loop_peg_181;
                            }
                        }
                        result_peg_182.setValue((void*) ",");
                    
                    
                    
                    result_peg_182 = rule_s(stream, result_peg_182.getPosition());
                        if (result_peg_182.error()){
                            goto loop_peg_181;
                        }
                    
                    
                    
                    result_peg_182 = rule_valuev(stream, result_peg_182.getPosition());
                        if (result_peg_182.error()){
                            goto loop_peg_181;
                        }
                    
                    
                }
                result_peg_178.addResult(result_peg_182);
            } while (true);
            loop_peg_181:
            ;
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuelist = result_peg_178;
        stream.update(result_peg_178.getPosition());
    }
    
    
    return result_peg_178;
    out_peg_180:
    
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
    
    
    Result result_peg_186(myposition);
    
    result_peg_186 = rule_integer(stream, result_peg_186.getPosition());
    if (result_peg_186.error()){
        goto out_peg_187;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_186;
        stream.update(result_peg_186.getPosition());
    }
    
    
    return result_peg_186;
    out_peg_187:
    Result result_peg_188(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_188.getPosition()))){
                    result_peg_188.nextPosition();
                } else {
                    goto out_peg_190;
                }
            }
            result_peg_188.setValue((void*) "as");
        
        
        
        result_peg_188 = rule_integer(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_188.getPosition()))){
                    result_peg_188.nextPosition();
                } else {
                    goto out_peg_190;
                }
            }
            result_peg_188.setValue((void*) "d");
        
        
        
        result_peg_188 = rule_integer(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_190;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_188;
        stream.update(result_peg_188.getPosition());
    }
    
    
    return result_peg_188;
    out_peg_190:
    Result result_peg_193(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_193.getPosition()))){
                    result_peg_193.nextPosition();
                } else {
                    goto out_peg_195;
                }
            }
            result_peg_193.setValue((void*) "a");
        
        
        
        result_peg_193 = rule_integer(stream, result_peg_193.getPosition());
            if (result_peg_193.error()){
                goto out_peg_195;
            }
        
        
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_193;
        stream.update(result_peg_193.getPosition());
    }
    
    
    return result_peg_193;
    out_peg_195:
    Result result_peg_196(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("a"[i], stream.get(result_peg_196.getPosition()))){
            result_peg_196.nextPosition();
        } else {
            goto out_peg_197;
        }
    }
    result_peg_196.setValue((void*) "a");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_196;
        stream.update(result_peg_196.getPosition());
    }
    
    
    return result_peg_196;
    out_peg_197:
    Result result_peg_198(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("s"[i], stream.get(result_peg_198.getPosition()))){
            result_peg_198.nextPosition();
        } else {
            goto out_peg_199;
        }
    }
    result_peg_198.setValue((void*) "s");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_198;
        stream.update(result_peg_198.getPosition());
    }
    
    
    return result_peg_198;
    out_peg_199:
    Result result_peg_200(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("vh"[i], stream.get(result_peg_200.getPosition()))){
            result_peg_200.nextPosition();
        } else {
            goto out_peg_201;
        }
    }
    result_peg_200.setValue((void*) "vh");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_200;
        stream.update(result_peg_200.getPosition());
    }
    
    
    return result_peg_200;
    out_peg_201:
    Result result_peg_202(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("hv"[i], stream.get(result_peg_202.getPosition()))){
            result_peg_202.nextPosition();
        } else {
            goto out_peg_203;
        }
    }
    result_peg_202.setValue((void*) "hv");
    
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
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("v"[i], stream.get(result_peg_204.getPosition()))){
            result_peg_204.nextPosition();
        } else {
            goto out_peg_205;
        }
    }
    result_peg_204.setValue((void*) "v");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_204;
        stream.update(result_peg_204.getPosition());
    }
    
    
    return result_peg_204;
    out_peg_205:
    Result result_peg_206(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareCharCase("h"[i], stream.get(result_peg_206.getPosition()))){
            result_peg_206.nextPosition();
        } else {
            goto out_peg_207;
        }
    }
    result_peg_206.setValue((void*) "h");
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_value = result_peg_206;
        stream.update(result_peg_206.getPosition());
    }
    
    
    return result_peg_206;
    out_peg_207:
    
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
        if (column.chunk3 != 0 && column.chunk3->chunk_valuev.calculated()){
            return column.chunk3->chunk_valuev;
        }
    }
    
    int myposition = position;
    
    
    Result result_peg_208(myposition);
    
    result_peg_208 = rule_value(stream, result_peg_208.getPosition());
    if (result_peg_208.error()){
        goto out_peg_209;
    }
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuev = result_peg_208;
        stream.update(result_peg_208.getPosition());
    }
    
    
    return result_peg_208;
    out_peg_209:
    Result result_peg_210(myposition);
    
    
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuev = result_peg_210;
        stream.update(result_peg_210.getPosition());
    }
    
    
    return result_peg_210;
    
    {
        Column & column = stream.getColumn(position);
        if (column.chunk3 == 0){
            column.chunk3 = new Chunk3();
        }
        column.chunk3->chunk_valuev = errorResult;
        stream.update(errorResult.getPosition());
    }
    
    
    return errorResult;
}
        

const void * main(const std::string & filename) throw (ParseException){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    return done.getValues().getValue();
}

const void * main(const char * in) throw (ParseException){
    Stream stream(in);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    return done.getValues().getValue();
}



    
    } /* Air */
    
} /* Mugen */

        
