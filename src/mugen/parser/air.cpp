

#include "mugen/ast/all.h"
#include <map>
#include "gc.h"
typedef std::list<Ast::Section*> SectionList;


#include <list>
#include <string>
#include <vector>
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
            out << __FILE__  << " cannot open '" << filename << "'";
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
        int context = 15;
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
        out << "Last successful rule trace" << std::endl;
        out << makeBacktrace() << std::endl;
        return out.str();
    }

    std::string makeBacktrace(){
        std::ostringstream out;

        bool first = true;
        for (std::vector<std::string>::iterator it = last_trace.begin(); it != last_trace.end(); it++){
            if (!first){
                out << " -> ";
            } else {
                first = false;
            }
            out << *it;
        }

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
            last_trace = rule_backtrace;
        }
    }

    void push_rule(const char * name){
        rule_backtrace.push_back(name);
    }

    void pop_rule(){
        rule_backtrace.pop_back();
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
    Column ** memo;
    int memo_size;
    int max;
    int farthest;
    std::vector<std::string> rule_backtrace;
    std::vector<std::string> last_trace;
};

class RuleTrace{
public:
    RuleTrace(Stream & stream, const char * name):
    stream(stream){
        stream.push_rule(name);
    }

    ~RuleTrace(){
        stream.pop_rule();
    }

    Stream & stream;
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
    
    RuleTrace trace_peg_16(stream, "start");
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
    
    RuleTrace trace_peg_25(stream, "line");
    int myposition = position;
    
    
    Result result_peg_18(myposition);
    
    {
    
        result_peg_18 = rule_s(stream, result_peg_18.getPosition());
            if (result_peg_18.error()){
                goto out_peg_20;
            }
        
        
        
        result_peg_18 = rule_comment(stream, result_peg_18.getPosition());
            if (result_peg_18.error()){
                goto out_peg_20;
            }
        
        
    }
    
    
    return result_peg_18;
    out_peg_20:
    Result result_peg_21(myposition);
    
    {
    
        result_peg_21 = rule_s(stream, result_peg_21.getPosition());
            if (result_peg_21.error()){
                goto out_peg_23;
            }
        
        
        
        result_peg_21 = rule_action(stream, result_peg_21.getPosition());
            if (result_peg_21.error()){
                goto out_peg_23;
            }
        
        Result result_peg_24 = result_peg_21;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_24.getValues());
                result_peg_21.setValue(value);
            }
        
        
    }
    
    
    return result_peg_21;
    out_peg_23:
    
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_26 = stream.getColumn(position);
    if (column_peg_26.chunk0 != 0 && column_peg_26.chunk0->chunk_action.calculated()){
        return column_peg_26.chunk0->chunk_action;
    }
    
    RuleTrace trace_peg_40(stream, "action");
    int myposition = position;
    
    Value ast;
    Result result_peg_27(myposition);
    
    {
    
        result_peg_27 = rule_action_start(stream, result_peg_27.getPosition());
            if (result_peg_27.error()){
                goto out_peg_29;
            }
        
        Result result_peg_28 = result_peg_27;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_28.getValues());
                result_peg_27.setValue(value);
            }
            ast = result_peg_27.getValues();
        
        
        
        result_peg_27 = rule_whitespace(stream, result_peg_27.getPosition());
            if (result_peg_27.error()){
                goto out_peg_29;
            }
        
        
        
        result_peg_27.reset();
            do{
                Result result_peg_34(result_peg_27.getPosition());
                result_peg_34 = rule_newline(stream, result_peg_34.getPosition());
                if (result_peg_34.error()){
                    goto loop_peg_33;
                }
                result_peg_27.addResult(result_peg_34);
            } while (true);
            loop_peg_33:
            if (result_peg_27.matches() == 0){
                goto out_peg_29;
            }
        
        
        
        result_peg_27.reset();
            do{
                Result result_peg_37(result_peg_27.getPosition());
                {
                
                    result_peg_37 = rule_action_line(stream, result_peg_37.getPosition(), ast);
                        if (result_peg_37.error()){
                            goto loop_peg_36;
                        }
                    
                    
                    
                    result_peg_37 = rule_whitespace(stream, result_peg_37.getPosition());
                        if (result_peg_37.error()){
                            goto loop_peg_36;
                        }
                    
                    
                    
                    result_peg_37 = rule_line_end(stream, result_peg_37.getPosition());
                        if (result_peg_37.error()){
                            goto loop_peg_36;
                        }
                    
                    
                }
                result_peg_27.addResult(result_peg_37);
            } while (true);
            loop_peg_36:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_27.setValue(value);
            }
        
        
    }
    
    if (column_peg_26.chunk0 == 0){
        column_peg_26.chunk0 = new Chunk0();
    }
    column_peg_26.chunk0->chunk_action = result_peg_27;
    stream.update(result_peg_27.getPosition());
    
    
    return result_peg_27;
    out_peg_29:
    
    if (column_peg_26.chunk0 == 0){
        column_peg_26.chunk0 = new Chunk0();
    }
    column_peg_26.chunk0->chunk_action = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position, Value section){
    
    RuleTrace trace_peg_61(stream, "action_line");
    int myposition = position;
    
    Value data;
    Result result_peg_42(myposition);
    
    {
    
        result_peg_42 = rule_s(stream, result_peg_42.getPosition());
            if (result_peg_42.error()){
                goto out_peg_44;
            }
        
        
        
        result_peg_42 = rule_comment(stream, result_peg_42.getPosition());
            if (result_peg_42.error()){
                goto out_peg_44;
            }
        
        
    }
    
    
    return result_peg_42;
    out_peg_44:
    Result result_peg_45(myposition);
    
    {
    
        result_peg_45 = rule_s(stream, result_peg_45.getPosition());
            if (result_peg_45.error()){
                goto out_peg_47;
            }
        
        
        
        result_peg_45 = rule_collision_default(stream, result_peg_45.getPosition());
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
    
    
    return result_peg_45;
    out_peg_47:
    Result result_peg_49(myposition);
    
    {
    
        result_peg_49 = rule_s(stream, result_peg_49.getPosition());
            if (result_peg_49.error()){
                goto out_peg_51;
            }
        
        
        
        result_peg_49 = rule_collision(stream, result_peg_49.getPosition());
            if (result_peg_49.error()){
                goto out_peg_51;
            }
            data = result_peg_49.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_49.setValue(value);
            }
        
        
    }
    
    
    return result_peg_49;
    out_peg_51:
    Result result_peg_53(myposition);
    
    {
    
        result_peg_53 = rule_s(stream, result_peg_53.getPosition());
            if (result_peg_53.error()){
                goto out_peg_55;
            }
        
        
        
        result_peg_53 = rule_valuelist(stream, result_peg_53.getPosition());
            if (result_peg_53.error()){
                goto out_peg_55;
            }
            data = result_peg_53.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                result_peg_53.setValue(value);
            }
        
        
    }
    
    
    return result_peg_53;
    out_peg_55:
    Result result_peg_57(myposition);
    
    {
    
        result_peg_57 = rule_s(stream, result_peg_57.getPosition());
            if (result_peg_57.error()){
                goto out_peg_59;
            }
        
        
        
        result_peg_57 = rule_loopstart(stream, result_peg_57.getPosition());
            if (result_peg_57.error()){
                goto out_peg_59;
            }
            data = result_peg_57.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_57.setValue(value);
            }
        
        
    }
    
    
    return result_peg_57;
    out_peg_59:
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_62 = stream.getColumn(position);
    if (column_peg_62.chunk0 != 0 && column_peg_62.chunk0->chunk_line_end.calculated()){
        return column_peg_62.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_71(stream, "line_end");
    int myposition = position;
    
    
    Result result_peg_63(myposition);
    
    result_peg_63.reset();
    do{
        Result result_peg_65(result_peg_63.getPosition());
        result_peg_65 = rule_newline(stream, result_peg_65.getPosition());
        if (result_peg_65.error()){
            goto loop_peg_64;
        }
        result_peg_63.addResult(result_peg_65);
    } while (true);
    loop_peg_64:
    if (result_peg_63.matches() == 0){
        goto out_peg_66;
    }
    
    if (column_peg_62.chunk0 == 0){
        column_peg_62.chunk0 = new Chunk0();
    }
    column_peg_62.chunk0->chunk_line_end = result_peg_63;
    stream.update(result_peg_63.getPosition());
    
    
    return result_peg_63;
    out_peg_66:
    Result result_peg_67(myposition);
    
    {
    
        Result result_peg_69(result_peg_67.getPosition());
            if ('\0' == stream.get(result_peg_69.getPosition())){
                result_peg_69.nextPosition();
                result_peg_69.setValue((void *) '\0');
            } else {
                goto out_peg_70;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_62.chunk0 == 0){
        column_peg_62.chunk0 = new Chunk0();
    }
    column_peg_62.chunk0->chunk_line_end = result_peg_67;
    stream.update(result_peg_67.getPosition());
    
    
    return result_peg_67;
    out_peg_70:
    
    if (column_peg_62.chunk0 == 0){
        column_peg_62.chunk0 = new Chunk0();
    }
    column_peg_62.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_72 = stream.getColumn(position);
    if (column_peg_72.chunk1 != 0 && column_peg_72.chunk1->chunk_newline.calculated()){
        return column_peg_72.chunk1->chunk_newline;
    }
    
    RuleTrace trace_peg_77(stream, "newline");
    int myposition = position;
    
    
    Result result_peg_73(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\n"[i], stream.get(result_peg_73.getPosition()))){
            result_peg_73.nextPosition();
        } else {
            goto out_peg_74;
        }
    }
    result_peg_73.setValue((void*) "\n");
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_newline = result_peg_73;
    stream.update(result_peg_73.getPosition());
    
    
    return result_peg_73;
    out_peg_74:
    Result result_peg_75(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("\r"[i], stream.get(result_peg_75.getPosition()))){
            result_peg_75.nextPosition();
        } else {
            goto out_peg_76;
        }
    }
    result_peg_75.setValue((void*) "\r");
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_newline = result_peg_75;
    stream.update(result_peg_75.getPosition());
    
    
    return result_peg_75;
    out_peg_76:
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_newline = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_78 = stream.getColumn(position);
    if (column_peg_78.chunk1 != 0 && column_peg_78.chunk1->chunk_loopstart.calculated()){
        return column_peg_78.chunk1->chunk_loopstart;
    }
    
    RuleTrace trace_peg_82(stream, "loopstart");
    int myposition = position;
    
    
    Result result_peg_79(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("loopstart"[i], stream.get(result_peg_79.getPosition()))){
                    result_peg_79.nextPosition();
                } else {
                    goto out_peg_81;
                }
            }
            result_peg_79.setValue((void*) "loopstart");
        
        Result result_peg_80 = result_peg_79;
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_80.getValues()));
                result_peg_79.setValue(value);
            }
        
        
    }
    
    if (column_peg_78.chunk1 == 0){
        column_peg_78.chunk1 = new Chunk1();
    }
    column_peg_78.chunk1->chunk_loopstart = result_peg_79;
    stream.update(result_peg_79.getPosition());
    
    
    return result_peg_79;
    out_peg_81:
    
    if (column_peg_78.chunk1 == 0){
        column_peg_78.chunk1 = new Chunk1();
    }
    column_peg_78.chunk1->chunk_loopstart = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_83 = stream.getColumn(position);
    if (column_peg_83.chunk1 != 0 && column_peg_83.chunk1->chunk_whitespace.calculated()){
        return column_peg_83.chunk1->chunk_whitespace;
    }
    
    RuleTrace trace_peg_87(stream, "whitespace");
    int myposition = position;
    
    
    Result result_peg_84(myposition);
    
    result_peg_84.reset();
    do{
        Result result_peg_86(result_peg_84.getPosition());
        result_peg_86 = rule_sw(stream, result_peg_86.getPosition());
        if (result_peg_86.error()){
            goto loop_peg_85;
        }
        result_peg_84.addResult(result_peg_86);
    } while (true);
    loop_peg_85:
    ;
    
    if (column_peg_83.chunk1 == 0){
        column_peg_83.chunk1 = new Chunk1();
    }
    column_peg_83.chunk1->chunk_whitespace = result_peg_84;
    stream.update(result_peg_84.getPosition());
    
    
    return result_peg_84;
    
    if (column_peg_83.chunk1 == 0){
        column_peg_83.chunk1 = new Chunk1();
    }
    column_peg_83.chunk1->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_88 = stream.getColumn(position);
    if (column_peg_88.chunk1 != 0 && column_peg_88.chunk1->chunk_sw.calculated()){
        return column_peg_88.chunk1->chunk_sw;
    }
    
    RuleTrace trace_peg_100(stream, "sw");
    int myposition = position;
    
    
    Result result_peg_89(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_89.getPosition()))){
                result_peg_89.nextPosition();
            } else {
                goto out_peg_92;
            }
        }
        result_peg_89.setValue((void*) " ");
            
    }
    goto success_peg_90;
    out_peg_92:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_89.getPosition()))){
                result_peg_89.nextPosition();
            } else {
                goto out_peg_94;
            }
        }
        result_peg_89.setValue((void*) "\t");
            
    }
    goto success_peg_90;
    out_peg_94:
    {
        
        if ((unsigned char) stream.get(result_peg_89.getPosition()) == (unsigned char) 255){
            result_peg_89.nextPosition();
        } else {
            goto out_peg_96;
        }
        result_peg_89.setValue((void*) 255);
        
    }
    goto success_peg_90;
    out_peg_96:
    goto out_peg_97;
    success_peg_90:
    ;
    
    if (column_peg_88.chunk1 == 0){
        column_peg_88.chunk1 = new Chunk1();
    }
    column_peg_88.chunk1->chunk_sw = result_peg_89;
    stream.update(result_peg_89.getPosition());
    
    
    return result_peg_89;
    out_peg_97:
    Result result_peg_98(myposition);
    
    result_peg_98 = rule_comment(stream, result_peg_98.getPosition());
    if (result_peg_98.error()){
        goto out_peg_99;
    }
    
    if (column_peg_88.chunk1 == 0){
        column_peg_88.chunk1 = new Chunk1();
    }
    column_peg_88.chunk1->chunk_sw = result_peg_98;
    stream.update(result_peg_98.getPosition());
    
    
    return result_peg_98;
    out_peg_99:
    
    if (column_peg_88.chunk1 == 0){
        column_peg_88.chunk1 = new Chunk1();
    }
    column_peg_88.chunk1->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_101 = stream.getColumn(position);
    if (column_peg_101.chunk1 != 0 && column_peg_101.chunk1->chunk_s.calculated()){
        return column_peg_101.chunk1->chunk_s;
    }
    
    RuleTrace trace_peg_112(stream, "s");
    int myposition = position;
    
    
    Result result_peg_102(myposition);
    
    result_peg_102.reset();
    do{
        Result result_peg_104(result_peg_102.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_104.getPosition()))){
                    result_peg_104.nextPosition();
                } else {
                    goto out_peg_107;
                }
            }
            result_peg_104.setValue((void*) " ");
                
        }
        goto success_peg_105;
        out_peg_107:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_104.getPosition()))){
                    result_peg_104.nextPosition();
                } else {
                    goto out_peg_109;
                }
            }
            result_peg_104.setValue((void*) "\t");
                
        }
        goto success_peg_105;
        out_peg_109:
        {
            
            if ((unsigned char) stream.get(result_peg_104.getPosition()) == (unsigned char) 255){
                result_peg_104.nextPosition();
            } else {
                goto out_peg_111;
            }
            result_peg_104.setValue((void*) 255);
            
        }
        goto success_peg_105;
        out_peg_111:
        goto loop_peg_103;
        success_peg_105:
        ;
        result_peg_102.addResult(result_peg_104);
    } while (true);
    loop_peg_103:
    ;
    
    if (column_peg_101.chunk1 == 0){
        column_peg_101.chunk1 = new Chunk1();
    }
    column_peg_101.chunk1->chunk_s = result_peg_102;
    stream.update(result_peg_102.getPosition());
    
    
    return result_peg_102;
    
    if (column_peg_101.chunk1 == 0){
        column_peg_101.chunk1 = new Chunk1();
    }
    column_peg_101.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_113 = stream.getColumn(position);
    if (column_peg_113.chunk2 != 0 && column_peg_113.chunk2->chunk_comment.calculated()){
        return column_peg_113.chunk2->chunk_comment;
    }
    
    RuleTrace trace_peg_137(stream, "comment");
    int myposition = position;
    
    
    Result result_peg_114(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_114.getPosition()))){
                    result_peg_114.nextPosition();
                } else {
                    goto out_peg_116;
                }
            }
            result_peg_114.setValue((void*) ";");
        
        
        
        result_peg_114.reset();
            do{
                Result result_peg_118(result_peg_114.getPosition());
                {
                
                    Result result_peg_121(result_peg_118);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_121.getPosition()))){
                                result_peg_121.nextPosition();
                            } else {
                                goto not_peg_120;
                            }
                        }
                        result_peg_121.setValue((void*) "\n");
                        goto loop_peg_117;
                        not_peg_120:
                        result_peg_118.setValue((void*)0);
                    
                    
                    
                    char temp_peg_122 = stream.get(result_peg_118.getPosition());
                        if (temp_peg_122 != '\0'){
                            result_peg_118.setValue((void*) (long) temp_peg_122);
                            result_peg_118.nextPosition();
                        } else {
                            goto loop_peg_117;
                        }
                    
                    
                }
                result_peg_114.addResult(result_peg_118);
            } while (true);
            loop_peg_117:
            ;
        
        
    }
    
    if (column_peg_113.chunk2 == 0){
        column_peg_113.chunk2 = new Chunk2();
    }
    column_peg_113.chunk2->chunk_comment = result_peg_114;
    stream.update(result_peg_114.getPosition());
    
    
    return result_peg_114;
    out_peg_116:
    Result result_peg_123(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_123.getPosition()))){
                    result_peg_123.nextPosition();
                } else {
                    goto out_peg_125;
                }
            }
            result_peg_123.setValue((void*) "=");
        
        
        
        result_peg_123.reset();
            do{
                Result result_peg_127(result_peg_123.getPosition());
                {
                
                    Result result_peg_130(result_peg_127);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_130.getPosition()))){
                                result_peg_130.nextPosition();
                            } else {
                                goto not_peg_129;
                            }
                        }
                        result_peg_130.setValue((void*) "\n");
                        goto loop_peg_126;
                        not_peg_129:
                        result_peg_127.setValue((void*)0);
                    
                    
                    
                    char temp_peg_131 = stream.get(result_peg_127.getPosition());
                        if (temp_peg_131 != '\0'){
                            result_peg_127.setValue((void*) (long) temp_peg_131);
                            result_peg_127.nextPosition();
                        } else {
                            goto loop_peg_126;
                        }
                    
                    
                }
                result_peg_123.addResult(result_peg_127);
            } while (true);
            loop_peg_126:
            ;
        
        
    }
    
    if (column_peg_113.chunk2 == 0){
        column_peg_113.chunk2 = new Chunk2();
    }
    column_peg_113.chunk2->chunk_comment = result_peg_123;
    stream.update(result_peg_123.getPosition());
    
    
    return result_peg_123;
    out_peg_125:
    Result result_peg_132(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_132.getPosition()))){
                    result_peg_132.nextPosition();
                } else {
                    goto out_peg_134;
                }
            }
            result_peg_132.setValue((void*) "-");
        
        
        
        result_peg_132.reset();
            do{
                Result result_peg_136(result_peg_132.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_136.getPosition()))){
                        result_peg_136.nextPosition();
                    } else {
                        goto loop_peg_135;
                    }
                }
                result_peg_136.setValue((void*) "-");
                result_peg_132.addResult(result_peg_136);
            } while (true);
            loop_peg_135:
            if (result_peg_132.matches() == 0){
                goto out_peg_134;
            }
        
        
    }
    
    if (column_peg_113.chunk2 == 0){
        column_peg_113.chunk2 = new Chunk2();
    }
    column_peg_113.chunk2->chunk_comment = result_peg_132;
    stream.update(result_peg_132.getPosition());
    
    
    return result_peg_132;
    out_peg_134:
    
    if (column_peg_113.chunk2 == 0){
        column_peg_113.chunk2 = new Chunk2();
    }
    column_peg_113.chunk2->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_138 = stream.getColumn(position);
    if (column_peg_138.chunk2 != 0 && column_peg_138.chunk2->chunk_collision_default.calculated()){
        return column_peg_138.chunk2->chunk_collision_default;
    }
    
    RuleTrace trace_peg_163(stream, "collision_default");
    int myposition = position;
    
    Value num;
    Result result_peg_139(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("Clsn2Default"[i], stream.get(result_peg_139.getPosition()))){
                    result_peg_139.nextPosition();
                } else {
                    goto out_peg_141;
                }
            }
            result_peg_139.setValue((void*) "Clsn2Default");
        
        Result result_peg_140 = result_peg_139;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_139.getPosition()))){
                    result_peg_139.nextPosition();
                } else {
                    goto out_peg_141;
                }
            }
            result_peg_139.setValue((void*) ":");
        
        
        
        result_peg_139 = rule_s(stream, result_peg_139.getPosition());
            if (result_peg_139.error()){
                goto out_peg_141;
            }
        
        
        
        result_peg_139 = rule_integer(stream, result_peg_139.getPosition());
            if (result_peg_139.error()){
                goto out_peg_141;
            }
            num = result_peg_139.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_140.getValues()), num);
                result_peg_139.setValue(value);
            }
        
        
    }
    
    if (column_peg_138.chunk2 == 0){
        column_peg_138.chunk2 = new Chunk2();
    }
    column_peg_138.chunk2->chunk_collision_default = result_peg_139;
    stream.update(result_peg_139.getPosition());
    
    
    return result_peg_139;
    out_peg_141:
    Result result_peg_145(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "Clsn1Default");
        
        Result result_peg_146 = result_peg_145;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) ":");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_integer(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
            num = result_peg_145.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_146.getValues()), num);
                result_peg_145.setValue(value);
            }
        
        
    }
    
    if (column_peg_138.chunk2 == 0){
        column_peg_138.chunk2 = new Chunk2();
    }
    column_peg_138.chunk2->chunk_collision_default = result_peg_145;
    stream.update(result_peg_145.getPosition());
    
    
    return result_peg_145;
    out_peg_147:
    Result result_peg_151(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_151.getPosition()))){
                    result_peg_151.nextPosition();
                } else {
                    goto out_peg_153;
                }
            }
            result_peg_151.setValue((void*) "Clsn2");
        
        Result result_peg_152 = result_peg_151;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_151.getPosition()))){
                    result_peg_151.nextPosition();
                } else {
                    goto out_peg_153;
                }
            }
            result_peg_151.setValue((void*) ":");
        
        
        
        result_peg_151 = rule_s(stream, result_peg_151.getPosition());
            if (result_peg_151.error()){
                goto out_peg_153;
            }
        
        
        
        result_peg_151 = rule_integer(stream, result_peg_151.getPosition());
            if (result_peg_151.error()){
                goto out_peg_153;
            }
            num = result_peg_151.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_152.getValues()), num);
                result_peg_151.setValue(value);
            }
        
        
    }
    
    if (column_peg_138.chunk2 == 0){
        column_peg_138.chunk2 = new Chunk2();
    }
    column_peg_138.chunk2->chunk_collision_default = result_peg_151;
    stream.update(result_peg_151.getPosition());
    
    
    return result_peg_151;
    out_peg_153:
    Result result_peg_157(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_157.getPosition()))){
                    result_peg_157.nextPosition();
                } else {
                    goto out_peg_159;
                }
            }
            result_peg_157.setValue((void*) "Clsn1");
        
        Result result_peg_158 = result_peg_157;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_157.getPosition()))){
                    result_peg_157.nextPosition();
                } else {
                    goto out_peg_159;
                }
            }
            result_peg_157.setValue((void*) ":");
        
        
        
        result_peg_157 = rule_s(stream, result_peg_157.getPosition());
            if (result_peg_157.error()){
                goto out_peg_159;
            }
        
        
        
        result_peg_157 = rule_integer(stream, result_peg_157.getPosition());
            if (result_peg_157.error()){
                goto out_peg_159;
            }
            num = result_peg_157.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_158.getValues()), num);
                result_peg_157.setValue(value);
            }
        
        
    }
    
    if (column_peg_138.chunk2 == 0){
        column_peg_138.chunk2 = new Chunk2();
    }
    column_peg_138.chunk2->chunk_collision_default = result_peg_157;
    stream.update(result_peg_157.getPosition());
    
    
    return result_peg_157;
    out_peg_159:
    
    if (column_peg_138.chunk2 == 0){
        column_peg_138.chunk2 = new Chunk2();
    }
    column_peg_138.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_164 = stream.getColumn(position);
    if (column_peg_164.chunk2 != 0 && column_peg_164.chunk2->chunk_collision.calculated()){
        return column_peg_164.chunk2->chunk_collision;
    }
    
    RuleTrace trace_peg_215(stream, "collision");
    int myposition = position;
    
    Value name;
    Value index;
    Value n1;
    Value n2;
    Value n3;
    Value n4;
    Result result_peg_165(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) "Clsn2");
            name = result_peg_165.getValues();
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) "[");
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        result_peg_165 = rule_integer(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
            index = result_peg_165.getValues();
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) "]");
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) "=");
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        result_peg_165 = rule_integer(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
            n1 = result_peg_165.getValues();
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) ",");
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        result_peg_165 = rule_integer(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
            n2 = result_peg_165.getValues();
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) ",");
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        result_peg_165 = rule_integer(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
            n3 = result_peg_165.getValues();
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_165.getPosition()))){
                    result_peg_165.nextPosition();
                } else {
                    goto out_peg_167;
                }
            }
            result_peg_165.setValue((void*) ",");
        
        
        
        result_peg_165 = rule_s(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
        
        
        
        result_peg_165 = rule_integer(stream, result_peg_165.getPosition());
            if (result_peg_165.error()){
                goto out_peg_167;
            }
            n4 = result_peg_165.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_165.setValue(value);
            }
        
        
    }
    
    if (column_peg_164.chunk2 == 0){
        column_peg_164.chunk2 = new Chunk2();
    }
    column_peg_164.chunk2->chunk_collision = result_peg_165;
    stream.update(result_peg_165.getPosition());
    
    
    return result_peg_165;
    out_peg_167:
    Result result_peg_190(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) "Clsn1");
            name = result_peg_190.getValues();
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) "[");
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        result_peg_190 = rule_integer(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
            index = result_peg_190.getValues();
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) "]");
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) "=");
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        result_peg_190 = rule_integer(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
            n1 = result_peg_190.getValues();
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) ",");
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        result_peg_190 = rule_integer(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
            n2 = result_peg_190.getValues();
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) ",");
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        result_peg_190 = rule_integer(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
            n3 = result_peg_190.getValues();
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_190.getPosition()))){
                    result_peg_190.nextPosition();
                } else {
                    goto out_peg_192;
                }
            }
            result_peg_190.setValue((void*) ",");
        
        
        
        result_peg_190 = rule_s(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
        
        
        
        result_peg_190 = rule_integer(stream, result_peg_190.getPosition());
            if (result_peg_190.error()){
                goto out_peg_192;
            }
            n4 = result_peg_190.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_190.setValue(value);
            }
        
        
    }
    
    if (column_peg_164.chunk2 == 0){
        column_peg_164.chunk2 = new Chunk2();
    }
    column_peg_164.chunk2->chunk_collision = result_peg_190;
    stream.update(result_peg_190.getPosition());
    
    
    return result_peg_190;
    out_peg_192:
    
    if (column_peg_164.chunk2 == 0){
        column_peg_164.chunk2 = new Chunk2();
    }
    column_peg_164.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_216 = stream.getColumn(position);
    if (column_peg_216.chunk2 != 0 && column_peg_216.chunk2->chunk_action_start.calculated()){
        return column_peg_216.chunk2->chunk_action_start;
    }
    
    RuleTrace trace_peg_228(stream, "action_start");
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_217(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_217.getPosition()))){
                    result_peg_217.nextPosition();
                } else {
                    goto out_peg_219;
                }
            }
            result_peg_217.setValue((void*) "[");
        
        
        
        result_peg_217 = rule_s(stream, result_peg_217.getPosition());
            if (result_peg_217.error()){
                goto out_peg_219;
            }
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_217.getPosition()))){
                    result_peg_217.nextPosition();
                } else {
                    goto out_peg_219;
                }
            }
            result_peg_217.setValue((void*) "Begin");
            begin = result_peg_217.getValues();
        
        
        
        result_peg_217 = rule_s(stream, result_peg_217.getPosition());
            if (result_peg_217.error()){
                goto out_peg_219;
            }
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_217.getPosition()))){
                    result_peg_217.nextPosition();
                } else {
                    goto out_peg_219;
                }
            }
            result_peg_217.setValue((void*) "Action");
            action = result_peg_217.getValues();
        
        
        
        result_peg_217 = rule_s(stream, result_peg_217.getPosition());
            if (result_peg_217.error()){
                goto out_peg_219;
            }
        
        
        
        result_peg_217 = rule_integer(stream, result_peg_217.getPosition());
            if (result_peg_217.error()){
                goto out_peg_219;
            }
            num = result_peg_217.getValues();
        
        
        
        result_peg_217 = rule_s(stream, result_peg_217.getPosition());
            if (result_peg_217.error()){
                goto out_peg_219;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_217.getPosition()))){
                    result_peg_217.nextPosition();
                } else {
                    goto out_peg_219;
                }
            }
            result_peg_217.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num);
                result_peg_217.setValue(value);
            }
        
        
    }
    
    if (column_peg_216.chunk2 == 0){
        column_peg_216.chunk2 = new Chunk2();
    }
    column_peg_216.chunk2->chunk_action_start = result_peg_217;
    stream.update(result_peg_217.getPosition());
    
    
    return result_peg_217;
    out_peg_219:
    
    if (column_peg_216.chunk2 == 0){
        column_peg_216.chunk2 = new Chunk2();
    }
    column_peg_216.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_229 = stream.getColumn(position);
    if (column_peg_229.chunk2 != 0 && column_peg_229.chunk2->chunk_integer.calculated()){
        return column_peg_229.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_247(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_230(myposition);
    
    {
    
        int save_peg_232 = result_peg_230.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_230.getPosition()))){
                        result_peg_230.nextPosition();
                    } else {
                        goto out_peg_235;
                    }
                }
                result_peg_230.setValue((void*) "-");
                    
            }
            goto success_peg_233;
            out_peg_235:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_230.getPosition()))){
                        result_peg_230.nextPosition();
                    } else {
                        goto out_peg_237;
                    }
                }
                result_peg_230.setValue((void*) "+");
                    
            }
            goto success_peg_233;
            out_peg_237:
            
            result_peg_230 = Result(save_peg_232);
            result_peg_230.setValue((void*) 0);
            
            success_peg_233:
            ;
        
        Result result_peg_231 = result_peg_230;
        
        result_peg_230.reset();
            do{
                Result result_peg_240(result_peg_230.getPosition());
                {
                    
                    char letter_peg_245 = stream.get(result_peg_240.getPosition());
                    if (letter_peg_245 != '\0' && strchr("0123456789", letter_peg_245) != NULL){
                        result_peg_240.nextPosition();
                        result_peg_240.setValue((void*) (long) letter_peg_245);
                    } else {
                        goto out_peg_244;
                    }
                    
                }
                goto success_peg_241;
                out_peg_244:
                goto loop_peg_239;
                success_peg_241:
                ;
                result_peg_230.addResult(result_peg_240);
            } while (true);
            loop_peg_239:
            if (result_peg_230.matches() == 0){
                goto out_peg_246;
            }
        
        Result result_peg_238 = result_peg_230;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_238.getValues()); value = makeNumber(result_peg_231.getValues(),value);
                result_peg_230.setValue(value);
            }
        
        
    }
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_integer = result_peg_230;
    stream.update(result_peg_230.getPosition());
    
    
    return result_peg_230;
    out_peg_246:
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_248 = stream.getColumn(position);
    if (column_peg_248.chunk3 != 0 && column_peg_248.chunk3->chunk_valuelist.calculated()){
        return column_peg_248.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_259(stream, "valuelist");
    int myposition = position;
    
    
    Result result_peg_249(myposition);
    
    {
    
        result_peg_249 = rule_value(stream, result_peg_249.getPosition());
            if (result_peg_249.error()){
                goto out_peg_251;
            }
        
        Result result_peg_250 = result_peg_249;
        
        result_peg_249.reset();
            do{
                Result result_peg_254(result_peg_249.getPosition());
                {
                
                    result_peg_254 = rule_s(stream, result_peg_254.getPosition());
                        if (result_peg_254.error()){
                            goto loop_peg_253;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_254.getPosition()))){
                                result_peg_254.nextPosition();
                            } else {
                                goto loop_peg_253;
                            }
                        }
                        result_peg_254.setValue((void*) ",");
                    
                    
                    
                    result_peg_254 = rule_s(stream, result_peg_254.getPosition());
                        if (result_peg_254.error()){
                            goto loop_peg_253;
                        }
                    
                    
                    
                    int save_peg_258 = result_peg_254.getPosition();
                        
                        result_peg_254 = rule_value(stream, result_peg_254.getPosition());
                        if (result_peg_254.error()){
                            
                            result_peg_254 = Result(save_peg_258);
                            result_peg_254.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_249.addResult(result_peg_254);
            } while (true);
            loop_peg_253:
            ;
        
        Result result_peg_252 = result_peg_249;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_250.getValues(),result_peg_252.getValues());
                result_peg_249.setValue(value);
            }
        
        
    }
    
    if (column_peg_248.chunk3 == 0){
        column_peg_248.chunk3 = new Chunk3();
    }
    column_peg_248.chunk3->chunk_valuelist = result_peg_249;
    stream.update(result_peg_249.getPosition());
    
    
    return result_peg_249;
    out_peg_251:
    
    if (column_peg_248.chunk3 == 0){
        column_peg_248.chunk3 = new Chunk3();
    }
    column_peg_248.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_260 = stream.getColumn(position);
    if (column_peg_260.chunk3 != 0 && column_peg_260.chunk3->chunk_value.calculated()){
        return column_peg_260.chunk3->chunk_value;
    }
    
    RuleTrace trace_peg_291(stream, "value");
    int myposition = position;
    
    
    Result result_peg_261(myposition);
    
    result_peg_261 = rule_integer(stream, result_peg_261.getPosition());
    if (result_peg_261.error()){
        goto out_peg_262;
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_261;
    stream.update(result_peg_261.getPosition());
    
    
    return result_peg_261;
    out_peg_262:
    Result result_peg_263(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_263.getPosition()))){
                    result_peg_263.nextPosition();
                } else {
                    goto out_peg_265;
                }
            }
            result_peg_263.setValue((void*) "as");
        
        Result result_peg_264 = result_peg_263;
        
        result_peg_263 = rule_integer(stream, result_peg_263.getPosition());
            if (result_peg_263.error()){
                goto out_peg_265;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_263.getPosition()))){
                    result_peg_263.nextPosition();
                } else {
                    goto out_peg_265;
                }
            }
            result_peg_263.setValue((void*) "d");
        
        
        
        result_peg_263 = rule_integer(stream, result_peg_263.getPosition());
            if (result_peg_263.error()){
                goto out_peg_265;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_264.getValues());
                result_peg_263.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_263;
    stream.update(result_peg_263.getPosition());
    
    
    return result_peg_263;
    out_peg_265:
    Result result_peg_269(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_269.getPosition()))){
                    result_peg_269.nextPosition();
                } else {
                    goto out_peg_271;
                }
            }
            result_peg_269.setValue((void*) "a");
        
        Result result_peg_270 = result_peg_269;
        
        result_peg_269 = rule_integer(stream, result_peg_269.getPosition());
            if (result_peg_269.error()){
                goto out_peg_271;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_270.getValues());
                result_peg_269.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_269;
    stream.update(result_peg_269.getPosition());
    
    
    return result_peg_269;
    out_peg_271:
    Result result_peg_273(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_273.getPosition()))){
                    result_peg_273.nextPosition();
                } else {
                    goto out_peg_275;
                }
            }
            result_peg_273.setValue((void*) "a");
        
        Result result_peg_274 = result_peg_273;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_274.getValues());
                result_peg_273.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_273;
    stream.update(result_peg_273.getPosition());
    
    
    return result_peg_273;
    out_peg_275:
    Result result_peg_276(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_276.getPosition()))){
                    result_peg_276.nextPosition();
                } else {
                    goto out_peg_278;
                }
            }
            result_peg_276.setValue((void*) "s");
        
        Result result_peg_277 = result_peg_276;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_277.getValues());
                result_peg_276.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_276;
    stream.update(result_peg_276.getPosition());
    
    
    return result_peg_276;
    out_peg_278:
    Result result_peg_279(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_281;
                }
            }
            result_peg_279.setValue((void*) "vh");
        
        Result result_peg_280 = result_peg_279;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_280.getValues());
                result_peg_279.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_279;
    stream.update(result_peg_279.getPosition());
    
    
    return result_peg_279;
    out_peg_281:
    Result result_peg_282(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_282.getPosition()))){
                    result_peg_282.nextPosition();
                } else {
                    goto out_peg_284;
                }
            }
            result_peg_282.setValue((void*) "hv");
        
        Result result_peg_283 = result_peg_282;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_283.getValues());
                result_peg_282.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_282;
    stream.update(result_peg_282.getPosition());
    
    
    return result_peg_282;
    out_peg_284:
    Result result_peg_285(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_285.getPosition()))){
                    result_peg_285.nextPosition();
                } else {
                    goto out_peg_287;
                }
            }
            result_peg_285.setValue((void*) "v");
        
        Result result_peg_286 = result_peg_285;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_286.getValues());
                result_peg_285.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_285;
    stream.update(result_peg_285.getPosition());
    
    
    return result_peg_285;
    out_peg_287:
    Result result_peg_288(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_288.getPosition()))){
                    result_peg_288.nextPosition();
                } else {
                    goto out_peg_290;
                }
            }
            result_peg_288.setValue((void*) "h");
        
        Result result_peg_289 = result_peg_288;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_289.getValues());
                result_peg_288.setValue(value);
            }
        
        
    }
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = result_peg_288;
    stream.update(result_peg_288.getPosition());
    
    
    return result_peg_288;
    out_peg_290:
    
    if (column_peg_260.chunk3 == 0){
        column_peg_260.chunk3 = new Chunk3();
    }
    column_peg_260.chunk3->chunk_value = errorResult;
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

        
