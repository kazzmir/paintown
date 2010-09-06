

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

    inline bool isList() const {
        return which == 1;
    }

    inline bool isData() const {
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

    inline int getPosition() const {
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

    inline int matches() const {
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
    Result chunk_comment;
};

struct Chunk2{
    Result chunk_collision_default;
    Result chunk_collision;
    Result chunk_action_start;
    Result chunk_integer;
    Result chunk_valuelist;
};

struct Chunk3{
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
(chunk3 != NULL ? ((chunk3->chunk_value.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_collision_default.calculated() ? 1 : 0)
+ (chunk2->chunk_collision.calculated() ? 1 : 0)
+ (chunk2->chunk_action_start.calculated() ? 1 : 0)
+ (chunk2->chunk_integer.calculated() ? 1 : 0)
+ (chunk2->chunk_valuelist.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_newline.calculated() ? 1 : 0)
+ (chunk1->chunk_loopstart.calculated() ? 1 : 0)
+ (chunk1->chunk_whitespace.calculated() ? 1 : 0)
+ (chunk1->chunk_sw.calculated() ? 1 : 0)
+ (chunk1->chunk_comment.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_action.calculated() ? 1 : 0)
+ (chunk0->chunk_action_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 16;
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
    struct LineInfo{
        LineInfo(int line, int column):
        line(line),
        column(column){
        }

        LineInfo(const LineInfo & copy):
        line(copy.line),
        column(copy.column){
        }

        LineInfo():
        line(-1),
        column(-1){
        }

        int line;
        int column;
    };

public:
    /* read from a file */
    Stream(const std::string & filename):
    temp(0),
    buffer(0),
    farthest(0),
    last_line_info(-1){
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

        line_info[-1] = LineInfo(1, 1);

        createMemo();
    }

    /* for null-terminated strings */
    Stream(const char * in):
    temp(0),
    buffer(in),
    farthest(0),
    last_line_info(-1){
        max = strlen(buffer);
        line_info[-1] = LineInfo(1, 1);
        createMemo();
    }

    /* user-defined length */
    Stream(const char * in, int length):
    temp(0),
    buffer(in),
    farthest(0),
    last_line_info(-1){
        max = length;
        line_info[-1] = LineInfo(1, 1);
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

    /* I'm sure this can be optimized. It only takes into account
     * the last position used to get line information rather than
     * finding a position closest to the one asked for.
     * So if the last position is 20 and the current position being requested
     * is 15 then this function will compute the information starting from 0.
     * If the information for 10 was computed then that should be used instead.
     * Maybe something like, sort the positions, find closest match lower
     * than the position and start from there.
     */
    LineInfo makeLineInfo(int last_line_position, int position){
        int line = line_info[last_line_position].line;
        int column = line_info[last_line_position].column;
        for (int i = last_line_position + 1; i < position; i++){
            if (buffer[i] == '\n'){
                line += 1;
                column = 1;
            } else {
                column += 1;
            }
        }
        return LineInfo(line, column);
    }

    void updateLineInfo(int position){
        if (line_info.find(position) == line_info.end()){
            if (position > last_line_info){
                line_info[position] = makeLineInfo(last_line_info, position);
            } else {
                line_info[position] = makeLineInfo(0, position);
            }
            last_line_info = position;
        }
    }

    const LineInfo & getLineInfo(int position){
        updateLineInfo(position);
        return line_info[position];
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
    int last_line_info;
    std::map<int, LineInfo> line_info;
};

static int getCurrentLine(const Value & value){
    Stream::LineInfo * info = (Stream::LineInfo*) value.getValue();
    return info->line;
}

static int getCurrentColumn(const Value & value){
    Stream::LineInfo * info = (Stream::LineInfo*) value.getValue();
    return info->column;
}

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
    
    try{
        
    
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
        
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    RuleTrace trace_peg_65(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_18(myposition);
        
        {
        
            {
                    
                    result_peg_18.reset();
                    do{
                        Result result_peg_32(result_peg_18.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_32.getPosition()))){
                                    result_peg_32.nextPosition();
                                } else {
                                    goto out_peg_35;
                                }
                            }
                            result_peg_32.setValue((void*) " ");
                                
                        }
                        goto success_peg_33;
                        out_peg_35:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_32.getPosition()))){
                                    result_peg_32.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_32.setValue((void*) "\t");
                                
                        }
                        goto success_peg_33;
                        out_peg_37:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_32.getPosition()) == (unsigned char) 255){
                                result_peg_32.nextPosition();
                            } else {
                                goto out_peg_39;
                            }
                            result_peg_32.setValue((void*) 255);
                            
                        }
                        goto success_peg_33;
                        out_peg_39:
                        goto loop_peg_31;
                        success_peg_33:
                        ;
                        result_peg_18.addResult(result_peg_32);
                    } while (true);
                    loop_peg_31:
                    ;
                            
                }
                goto success_peg_20;
                goto out_peg_40;
                success_peg_20:
                ;
            
            
            
            result_peg_18 = rule_comment(stream, result_peg_18.getPosition());
                if (result_peg_18.error()){
                    goto out_peg_40;
                }
            
            
        }
        stream.update(result_peg_18.getPosition());
        
        return result_peg_18;
        out_peg_40:
        Result result_peg_41(myposition);
        
        {
        
            {
                    
                    result_peg_41.reset();
                    do{
                        Result result_peg_55(result_peg_41.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    goto out_peg_58;
                                }
                            }
                            result_peg_55.setValue((void*) " ");
                                
                        }
                        goto success_peg_56;
                        out_peg_58:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_55.getPosition()))){
                                    result_peg_55.nextPosition();
                                } else {
                                    goto out_peg_60;
                                }
                            }
                            result_peg_55.setValue((void*) "\t");
                                
                        }
                        goto success_peg_56;
                        out_peg_60:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_55.getPosition()) == (unsigned char) 255){
                                result_peg_55.nextPosition();
                            } else {
                                goto out_peg_62;
                            }
                            result_peg_55.setValue((void*) 255);
                            
                        }
                        goto success_peg_56;
                        out_peg_62:
                        goto loop_peg_54;
                        success_peg_56:
                        ;
                        result_peg_41.addResult(result_peg_55);
                    } while (true);
                    loop_peg_54:
                    ;
                            
                }
                goto success_peg_43;
                goto out_peg_63;
                success_peg_43:
                ;
            
            
            
            result_peg_41 = rule_action(stream, result_peg_41.getPosition());
                if (result_peg_41.error()){
                    goto out_peg_63;
                }
            
            Result result_peg_64 = result_peg_41;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_64.getValues());
                    result_peg_41.setValue(value);
                }
            
            
        }
        stream.update(result_peg_41.getPosition());
        
        return result_peg_41;
        out_peg_63:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_66 = stream.getColumn(position);
    if (column_peg_66.chunk0 != 0 && column_peg_66.chunk0->chunk_action.calculated()){
        return column_peg_66.chunk0->chunk_action;
    }
    
    RuleTrace trace_peg_80(stream, "action");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_67(myposition);
        
        {
        
            result_peg_67 = rule_action_start(stream, result_peg_67.getPosition());
                if (result_peg_67.error()){
                    goto out_peg_69;
                }
            
            Result result_peg_68 = result_peg_67;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_68.getValues());
                    result_peg_67.setValue(value);
                }
                ast = result_peg_67.getValues();
            
            
            
            result_peg_67 = rule_whitespace(stream, result_peg_67.getPosition());
                if (result_peg_67.error()){
                    goto out_peg_69;
                }
            
            
            
            result_peg_67.reset();
                do{
                    Result result_peg_74(result_peg_67.getPosition());
                    result_peg_74 = rule_newline(stream, result_peg_74.getPosition());
                    if (result_peg_74.error()){
                        goto loop_peg_73;
                    }
                    result_peg_67.addResult(result_peg_74);
                } while (true);
                loop_peg_73:
                if (result_peg_67.matches() == 0){
                    goto out_peg_69;
                }
            
            
            
            result_peg_67.reset();
                do{
                    Result result_peg_77(result_peg_67.getPosition());
                    {
                    
                        result_peg_77 = rule_action_line(stream, result_peg_77.getPosition(), ast);
                            if (result_peg_77.error()){
                                goto loop_peg_76;
                            }
                        
                        
                        
                        result_peg_77 = rule_whitespace(stream, result_peg_77.getPosition());
                            if (result_peg_77.error()){
                                goto loop_peg_76;
                            }
                        
                        
                        
                        result_peg_77 = rule_line_end(stream, result_peg_77.getPosition());
                            if (result_peg_77.error()){
                                goto loop_peg_76;
                            }
                        
                        
                    }
                    result_peg_67.addResult(result_peg_77);
                } while (true);
                loop_peg_76:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_67.setValue(value);
                }
            
            
        }
        
        if (column_peg_66.chunk0 == 0){
            column_peg_66.chunk0 = new Chunk0();
        }
        column_peg_66.chunk0->chunk_action = result_peg_67;
        stream.update(result_peg_67.getPosition());
        
        
        return result_peg_67;
        out_peg_69:
    
        if (column_peg_66.chunk0 == 0){
            column_peg_66.chunk0 = new Chunk0();
        }
        column_peg_66.chunk0->chunk_action = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position, Value section){
    
    RuleTrace trace_peg_201(stream, "action_line");
    int myposition = position;
    
    
    Value data;
    Result result_peg_82(myposition);
        
        {
        
            {
                    
                    result_peg_82.reset();
                    do{
                        Result result_peg_96(result_peg_82.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    goto out_peg_99;
                                }
                            }
                            result_peg_96.setValue((void*) " ");
                                
                        }
                        goto success_peg_97;
                        out_peg_99:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    goto out_peg_101;
                                }
                            }
                            result_peg_96.setValue((void*) "\t");
                                
                        }
                        goto success_peg_97;
                        out_peg_101:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_96.getPosition()) == (unsigned char) 255){
                                result_peg_96.nextPosition();
                            } else {
                                goto out_peg_103;
                            }
                            result_peg_96.setValue((void*) 255);
                            
                        }
                        goto success_peg_97;
                        out_peg_103:
                        goto loop_peg_95;
                        success_peg_97:
                        ;
                        result_peg_82.addResult(result_peg_96);
                    } while (true);
                    loop_peg_95:
                    ;
                            
                }
                goto success_peg_84;
                goto out_peg_104;
                success_peg_84:
                ;
            
            
            
            result_peg_82 = rule_comment(stream, result_peg_82.getPosition());
                if (result_peg_82.error()){
                    goto out_peg_104;
                }
            
            
        }
        stream.update(result_peg_82.getPosition());
        
        return result_peg_82;
        out_peg_104:
        Result result_peg_105(myposition);
        
        {
        
            {
                    
                    result_peg_105.reset();
                    do{
                        Result result_peg_119(result_peg_105.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_119.getPosition()))){
                                    result_peg_119.nextPosition();
                                } else {
                                    goto out_peg_122;
                                }
                            }
                            result_peg_119.setValue((void*) " ");
                                
                        }
                        goto success_peg_120;
                        out_peg_122:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_119.getPosition()))){
                                    result_peg_119.nextPosition();
                                } else {
                                    goto out_peg_124;
                                }
                            }
                            result_peg_119.setValue((void*) "\t");
                                
                        }
                        goto success_peg_120;
                        out_peg_124:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_119.getPosition()) == (unsigned char) 255){
                                result_peg_119.nextPosition();
                            } else {
                                goto out_peg_126;
                            }
                            result_peg_119.setValue((void*) 255);
                            
                        }
                        goto success_peg_120;
                        out_peg_126:
                        goto loop_peg_118;
                        success_peg_120:
                        ;
                        result_peg_105.addResult(result_peg_119);
                    } while (true);
                    loop_peg_118:
                    ;
                            
                }
                goto success_peg_107;
                goto out_peg_127;
                success_peg_107:
                ;
            
            
            
            result_peg_105 = rule_collision_default(stream, result_peg_105.getPosition());
                if (result_peg_105.error()){
                    goto out_peg_127;
                }
                data = result_peg_105.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_105.setValue(value);
                }
            
            
        }
        stream.update(result_peg_105.getPosition());
        
        return result_peg_105;
        out_peg_127:
        Result result_peg_129(myposition);
        
        {
        
            {
                    
                    result_peg_129.reset();
                    do{
                        Result result_peg_143(result_peg_129.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    goto out_peg_146;
                                }
                            }
                            result_peg_143.setValue((void*) " ");
                                
                        }
                        goto success_peg_144;
                        out_peg_146:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    goto out_peg_148;
                                }
                            }
                            result_peg_143.setValue((void*) "\t");
                                
                        }
                        goto success_peg_144;
                        out_peg_148:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_143.getPosition()) == (unsigned char) 255){
                                result_peg_143.nextPosition();
                            } else {
                                goto out_peg_150;
                            }
                            result_peg_143.setValue((void*) 255);
                            
                        }
                        goto success_peg_144;
                        out_peg_150:
                        goto loop_peg_142;
                        success_peg_144:
                        ;
                        result_peg_129.addResult(result_peg_143);
                    } while (true);
                    loop_peg_142:
                    ;
                            
                }
                goto success_peg_131;
                goto out_peg_151;
                success_peg_131:
                ;
            
            
            
            result_peg_129 = rule_collision(stream, result_peg_129.getPosition());
                if (result_peg_129.error()){
                    goto out_peg_151;
                }
                data = result_peg_129.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_129.setValue(value);
                }
            
            
        }
        stream.update(result_peg_129.getPosition());
        
        return result_peg_129;
        out_peg_151:
        Result result_peg_153(myposition);
        
        {
        
            {
                    
                    result_peg_153.reset();
                    do{
                        Result result_peg_167(result_peg_153.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_167.getPosition()))){
                                    result_peg_167.nextPosition();
                                } else {
                                    goto out_peg_170;
                                }
                            }
                            result_peg_167.setValue((void*) " ");
                                
                        }
                        goto success_peg_168;
                        out_peg_170:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_167.getPosition()))){
                                    result_peg_167.nextPosition();
                                } else {
                                    goto out_peg_172;
                                }
                            }
                            result_peg_167.setValue((void*) "\t");
                                
                        }
                        goto success_peg_168;
                        out_peg_172:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_167.getPosition()) == (unsigned char) 255){
                                result_peg_167.nextPosition();
                            } else {
                                goto out_peg_174;
                            }
                            result_peg_167.setValue((void*) 255);
                            
                        }
                        goto success_peg_168;
                        out_peg_174:
                        goto loop_peg_166;
                        success_peg_168:
                        ;
                        result_peg_153.addResult(result_peg_167);
                    } while (true);
                    loop_peg_166:
                    ;
                            
                }
                goto success_peg_155;
                goto out_peg_175;
                success_peg_155:
                ;
            
            
            
            result_peg_153 = rule_valuelist(stream, result_peg_153.getPosition());
                if (result_peg_153.error()){
                    goto out_peg_175;
                }
                data = result_peg_153.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                    result_peg_153.setValue(value);
                }
            
            
        }
        stream.update(result_peg_153.getPosition());
        
        return result_peg_153;
        out_peg_175:
        Result result_peg_177(myposition);
        
        {
        
            {
                    
                    result_peg_177.reset();
                    do{
                        Result result_peg_191(result_peg_177.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_191.getPosition()))){
                                    result_peg_191.nextPosition();
                                } else {
                                    goto out_peg_194;
                                }
                            }
                            result_peg_191.setValue((void*) " ");
                                
                        }
                        goto success_peg_192;
                        out_peg_194:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_191.getPosition()))){
                                    result_peg_191.nextPosition();
                                } else {
                                    goto out_peg_196;
                                }
                            }
                            result_peg_191.setValue((void*) "\t");
                                
                        }
                        goto success_peg_192;
                        out_peg_196:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_191.getPosition()) == (unsigned char) 255){
                                result_peg_191.nextPosition();
                            } else {
                                goto out_peg_198;
                            }
                            result_peg_191.setValue((void*) 255);
                            
                        }
                        goto success_peg_192;
                        out_peg_198:
                        goto loop_peg_190;
                        success_peg_192:
                        ;
                        result_peg_177.addResult(result_peg_191);
                    } while (true);
                    loop_peg_190:
                    ;
                            
                }
                goto success_peg_179;
                goto out_peg_199;
                success_peg_179:
                ;
            
            
            
            result_peg_177 = rule_loopstart(stream, result_peg_177.getPosition());
                if (result_peg_177.error()){
                    goto out_peg_199;
                }
                data = result_peg_177.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_177.setValue(value);
                }
            
            
        }
        stream.update(result_peg_177.getPosition());
        
        return result_peg_177;
        out_peg_199:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_202 = stream.getColumn(position);
    if (column_peg_202.chunk0 != 0 && column_peg_202.chunk0->chunk_line_end.calculated()){
        return column_peg_202.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_211(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_203(myposition);
        
        result_peg_203.reset();
        do{
            Result result_peg_205(result_peg_203.getPosition());
            result_peg_205 = rule_newline(stream, result_peg_205.getPosition());
            if (result_peg_205.error()){
                goto loop_peg_204;
            }
            result_peg_203.addResult(result_peg_205);
        } while (true);
        loop_peg_204:
        if (result_peg_203.matches() == 0){
            goto out_peg_206;
        }
        
        if (column_peg_202.chunk0 == 0){
            column_peg_202.chunk0 = new Chunk0();
        }
        column_peg_202.chunk0->chunk_line_end = result_peg_203;
        stream.update(result_peg_203.getPosition());
        
        
        return result_peg_203;
        out_peg_206:
        Result result_peg_207(myposition);
        
        {
        
            Result result_peg_209(result_peg_207.getPosition());
                if ('\0' == stream.get(result_peg_209.getPosition())){
                    result_peg_209.nextPosition();
                    result_peg_209.setValue((void *) '\0');
                } else {
                    goto out_peg_210;
                }
            
            
            
            
            
            
        }
        
        if (column_peg_202.chunk0 == 0){
            column_peg_202.chunk0 = new Chunk0();
        }
        column_peg_202.chunk0->chunk_line_end = result_peg_207;
        stream.update(result_peg_207.getPosition());
        
        
        return result_peg_207;
        out_peg_210:
    
        if (column_peg_202.chunk0 == 0){
            column_peg_202.chunk0 = new Chunk0();
        }
        column_peg_202.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_212 = stream.getColumn(position);
    if (column_peg_212.chunk1 != 0 && column_peg_212.chunk1->chunk_newline.calculated()){
        return column_peg_212.chunk1->chunk_newline;
    }
    
    RuleTrace trace_peg_217(stream, "newline");
    int myposition = position;
    
    
    
    Result result_peg_213(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\n"[i], stream.get(result_peg_213.getPosition()))){
                result_peg_213.nextPosition();
            } else {
                goto out_peg_214;
            }
        }
        result_peg_213.setValue((void*) "\n");
        
        if (column_peg_212.chunk1 == 0){
            column_peg_212.chunk1 = new Chunk1();
        }
        column_peg_212.chunk1->chunk_newline = result_peg_213;
        stream.update(result_peg_213.getPosition());
        
        
        return result_peg_213;
        out_peg_214:
        Result result_peg_215(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\r"[i], stream.get(result_peg_215.getPosition()))){
                result_peg_215.nextPosition();
            } else {
                goto out_peg_216;
            }
        }
        result_peg_215.setValue((void*) "\r");
        
        if (column_peg_212.chunk1 == 0){
            column_peg_212.chunk1 = new Chunk1();
        }
        column_peg_212.chunk1->chunk_newline = result_peg_215;
        stream.update(result_peg_215.getPosition());
        
        
        return result_peg_215;
        out_peg_216:
    
        if (column_peg_212.chunk1 == 0){
            column_peg_212.chunk1 = new Chunk1();
        }
        column_peg_212.chunk1->chunk_newline = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_218 = stream.getColumn(position);
    if (column_peg_218.chunk1 != 0 && column_peg_218.chunk1->chunk_loopstart.calculated()){
        return column_peg_218.chunk1->chunk_loopstart;
    }
    
    RuleTrace trace_peg_222(stream, "loopstart");
    int myposition = position;
    
    
    
    Result result_peg_219(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("loopstart"[i], stream.get(result_peg_219.getPosition()))){
                        result_peg_219.nextPosition();
                    } else {
                        goto out_peg_221;
                    }
                }
                result_peg_219.setValue((void*) "loopstart");
            
            Result result_peg_220 = result_peg_219;
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(makeKeyword(result_peg_220.getValues()));
                    result_peg_219.setValue(value);
                }
            
            
        }
        
        if (column_peg_218.chunk1 == 0){
            column_peg_218.chunk1 = new Chunk1();
        }
        column_peg_218.chunk1->chunk_loopstart = result_peg_219;
        stream.update(result_peg_219.getPosition());
        
        
        return result_peg_219;
        out_peg_221:
    
        if (column_peg_218.chunk1 == 0){
            column_peg_218.chunk1 = new Chunk1();
        }
        column_peg_218.chunk1->chunk_loopstart = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_223 = stream.getColumn(position);
    if (column_peg_223.chunk1 != 0 && column_peg_223.chunk1->chunk_whitespace.calculated()){
        return column_peg_223.chunk1->chunk_whitespace;
    }
    
    RuleTrace trace_peg_227(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_224(myposition);
        
        result_peg_224.reset();
        do{
            Result result_peg_226(result_peg_224.getPosition());
            result_peg_226 = rule_sw(stream, result_peg_226.getPosition());
            if (result_peg_226.error()){
                goto loop_peg_225;
            }
            result_peg_224.addResult(result_peg_226);
        } while (true);
        loop_peg_225:
        ;
        
        if (column_peg_223.chunk1 == 0){
            column_peg_223.chunk1 = new Chunk1();
        }
        column_peg_223.chunk1->chunk_whitespace = result_peg_224;
        stream.update(result_peg_224.getPosition());
        
        
        return result_peg_224;
    
        if (column_peg_223.chunk1 == 0){
            column_peg_223.chunk1 = new Chunk1();
        }
        column_peg_223.chunk1->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_228 = stream.getColumn(position);
    if (column_peg_228.chunk1 != 0 && column_peg_228.chunk1->chunk_sw.calculated()){
        return column_peg_228.chunk1->chunk_sw;
    }
    
    RuleTrace trace_peg_240(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_229(myposition);
        
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_229.getPosition()))){
                    result_peg_229.nextPosition();
                } else {
                    goto out_peg_232;
                }
            }
            result_peg_229.setValue((void*) " ");
                
        }
        goto success_peg_230;
        out_peg_232:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_229.getPosition()))){
                    result_peg_229.nextPosition();
                } else {
                    goto out_peg_234;
                }
            }
            result_peg_229.setValue((void*) "\t");
                
        }
        goto success_peg_230;
        out_peg_234:
        {
            
            if ((unsigned char) stream.get(result_peg_229.getPosition()) == (unsigned char) 255){
                result_peg_229.nextPosition();
            } else {
                goto out_peg_236;
            }
            result_peg_229.setValue((void*) 255);
            
        }
        goto success_peg_230;
        out_peg_236:
        goto out_peg_237;
        success_peg_230:
        ;
        
        if (column_peg_228.chunk1 == 0){
            column_peg_228.chunk1 = new Chunk1();
        }
        column_peg_228.chunk1->chunk_sw = result_peg_229;
        stream.update(result_peg_229.getPosition());
        
        
        return result_peg_229;
        out_peg_237:
        Result result_peg_238(myposition);
        
        result_peg_238 = rule_comment(stream, result_peg_238.getPosition());
        if (result_peg_238.error()){
            goto out_peg_239;
        }
        
        if (column_peg_228.chunk1 == 0){
            column_peg_228.chunk1 = new Chunk1();
        }
        column_peg_228.chunk1->chunk_sw = result_peg_238;
        stream.update(result_peg_238.getPosition());
        
        
        return result_peg_238;
        out_peg_239:
    
        if (column_peg_228.chunk1 == 0){
            column_peg_228.chunk1 = new Chunk1();
        }
        column_peg_228.chunk1->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_241 = stream.getColumn(position);
    if (column_peg_241.chunk1 != 0 && column_peg_241.chunk1->chunk_comment.calculated()){
        return column_peg_241.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_270(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_242(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_242.getPosition()))){
                        result_peg_242.nextPosition();
                    } else {
                        goto out_peg_244;
                    }
                }
                result_peg_242.setValue((void*) ";");
            
            
            
            result_peg_242.reset();
                do{
                    Result result_peg_246(result_peg_242.getPosition());
                    {
                    
                        Result result_peg_249(result_peg_246);
                            result_peg_249 = rule_newline(stream, result_peg_249.getPosition());
                            if (result_peg_249.error()){
                                goto not_peg_248;
                            }
                            goto loop_peg_245;
                            not_peg_248:
                            result_peg_246.setValue((void*)0);
                        
                        
                        
                        char temp_peg_250 = stream.get(result_peg_246.getPosition());
                            if (temp_peg_250 != '\0'){
                                result_peg_246.setValue((void*) (long) temp_peg_250);
                                result_peg_246.nextPosition();
                            } else {
                                goto loop_peg_245;
                            }
                        
                        
                    }
                    result_peg_242.addResult(result_peg_246);
                } while (true);
                loop_peg_245:
                ;
            
            
        }
        
        if (column_peg_241.chunk1 == 0){
            column_peg_241.chunk1 = new Chunk1();
        }
        column_peg_241.chunk1->chunk_comment = result_peg_242;
        stream.update(result_peg_242.getPosition());
        
        
        return result_peg_242;
        out_peg_244:
        Result result_peg_251(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_251.getPosition()))){
                        result_peg_251.nextPosition();
                    } else {
                        goto out_peg_253;
                    }
                }
                result_peg_251.setValue((void*) "=");
            
            
            
            result_peg_251.reset();
                do{
                    Result result_peg_255(result_peg_251.getPosition());
                    {
                    
                        Result result_peg_258(result_peg_255);
                            result_peg_258 = rule_newline(stream, result_peg_258.getPosition());
                            if (result_peg_258.error()){
                                goto not_peg_257;
                            }
                            goto loop_peg_254;
                            not_peg_257:
                            result_peg_255.setValue((void*)0);
                        
                        
                        
                        char temp_peg_259 = stream.get(result_peg_255.getPosition());
                            if (temp_peg_259 != '\0'){
                                result_peg_255.setValue((void*) (long) temp_peg_259);
                                result_peg_255.nextPosition();
                            } else {
                                goto loop_peg_254;
                            }
                        
                        
                    }
                    result_peg_251.addResult(result_peg_255);
                } while (true);
                loop_peg_254:
                ;
            
            
        }
        
        if (column_peg_241.chunk1 == 0){
            column_peg_241.chunk1 = new Chunk1();
        }
        column_peg_241.chunk1->chunk_comment = result_peg_251;
        stream.update(result_peg_251.getPosition());
        
        
        return result_peg_251;
        out_peg_253:
        Result result_peg_260(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_260.getPosition()))){
                        result_peg_260.nextPosition();
                    } else {
                        goto out_peg_262;
                    }
                }
                result_peg_260.setValue((void*) "-");
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_260.getPosition()))){
                        result_peg_260.nextPosition();
                    } else {
                        goto out_peg_262;
                    }
                }
                result_peg_260.setValue((void*) "-");
            
            
            
            result_peg_260.reset();
                do{
                    Result result_peg_265(result_peg_260.getPosition());
                    {
                    
                        Result result_peg_268(result_peg_265);
                            result_peg_268 = rule_newline(stream, result_peg_268.getPosition());
                            if (result_peg_268.error()){
                                goto not_peg_267;
                            }
                            goto loop_peg_264;
                            not_peg_267:
                            result_peg_265.setValue((void*)0);
                        
                        
                        
                        char temp_peg_269 = stream.get(result_peg_265.getPosition());
                            if (temp_peg_269 != '\0'){
                                result_peg_265.setValue((void*) (long) temp_peg_269);
                                result_peg_265.nextPosition();
                            } else {
                                goto loop_peg_264;
                            }
                        
                        
                    }
                    result_peg_260.addResult(result_peg_265);
                } while (true);
                loop_peg_264:
                ;
            
            
        }
        
        if (column_peg_241.chunk1 == 0){
            column_peg_241.chunk1 = new Chunk1();
        }
        column_peg_241.chunk1->chunk_comment = result_peg_260;
        stream.update(result_peg_260.getPosition());
        
        
        return result_peg_260;
        out_peg_262:
    
        if (column_peg_241.chunk1 == 0){
            column_peg_241.chunk1 = new Chunk1();
        }
        column_peg_241.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_271 = stream.getColumn(position);
    if (column_peg_271.chunk2 != 0 && column_peg_271.chunk2->chunk_collision_default.calculated()){
        return column_peg_271.chunk2->chunk_collision_default;
    }
    
    RuleTrace trace_peg_376(stream, "collision_default");
    int myposition = position;
    
    
    Value num;
    Result result_peg_272(myposition);
        
        {
        
            for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn2Default"[i], stream.get(result_peg_272.getPosition()))){
                        result_peg_272.nextPosition();
                    } else {
                        goto out_peg_274;
                    }
                }
                result_peg_272.setValue((void*) "Clsn2Default");
            
            Result result_peg_273 = result_peg_272;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_272.getPosition()))){
                        result_peg_272.nextPosition();
                    } else {
                        goto out_peg_274;
                    }
                }
                result_peg_272.setValue((void*) ":");
            
            
            
            {
                    
                    result_peg_272.reset();
                    do{
                        Result result_peg_289(result_peg_272.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_289.getPosition()))){
                                    result_peg_289.nextPosition();
                                } else {
                                    goto out_peg_292;
                                }
                            }
                            result_peg_289.setValue((void*) " ");
                                
                        }
                        goto success_peg_290;
                        out_peg_292:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_289.getPosition()))){
                                    result_peg_289.nextPosition();
                                } else {
                                    goto out_peg_294;
                                }
                            }
                            result_peg_289.setValue((void*) "\t");
                                
                        }
                        goto success_peg_290;
                        out_peg_294:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_289.getPosition()) == (unsigned char) 255){
                                result_peg_289.nextPosition();
                            } else {
                                goto out_peg_296;
                            }
                            result_peg_289.setValue((void*) 255);
                            
                        }
                        goto success_peg_290;
                        out_peg_296:
                        goto loop_peg_288;
                        success_peg_290:
                        ;
                        result_peg_272.addResult(result_peg_289);
                    } while (true);
                    loop_peg_288:
                    ;
                            
                }
                goto success_peg_277;
                goto out_peg_274;
                success_peg_277:
                ;
            
            
            
            result_peg_272 = rule_integer(stream, result_peg_272.getPosition());
                if (result_peg_272.error()){
                    goto out_peg_274;
                }
                num = result_peg_272.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(makeKeyword(result_peg_273.getValues()), num);
                    result_peg_272.setValue(value);
                }
            
            
        }
        
        if (column_peg_271.chunk2 == 0){
            column_peg_271.chunk2 = new Chunk2();
        }
        column_peg_271.chunk2->chunk_collision_default = result_peg_272;
        stream.update(result_peg_272.getPosition());
        
        
        return result_peg_272;
        out_peg_274:
        Result result_peg_298(myposition);
        
        {
        
            for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_298.getPosition()))){
                        result_peg_298.nextPosition();
                    } else {
                        goto out_peg_300;
                    }
                }
                result_peg_298.setValue((void*) "Clsn1Default");
            
            Result result_peg_299 = result_peg_298;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_298.getPosition()))){
                        result_peg_298.nextPosition();
                    } else {
                        goto out_peg_300;
                    }
                }
                result_peg_298.setValue((void*) ":");
            
            
            
            {
                    
                    result_peg_298.reset();
                    do{
                        Result result_peg_315(result_peg_298.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_315.getPosition()))){
                                    result_peg_315.nextPosition();
                                } else {
                                    goto out_peg_318;
                                }
                            }
                            result_peg_315.setValue((void*) " ");
                                
                        }
                        goto success_peg_316;
                        out_peg_318:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_315.getPosition()))){
                                    result_peg_315.nextPosition();
                                } else {
                                    goto out_peg_320;
                                }
                            }
                            result_peg_315.setValue((void*) "\t");
                                
                        }
                        goto success_peg_316;
                        out_peg_320:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_315.getPosition()) == (unsigned char) 255){
                                result_peg_315.nextPosition();
                            } else {
                                goto out_peg_322;
                            }
                            result_peg_315.setValue((void*) 255);
                            
                        }
                        goto success_peg_316;
                        out_peg_322:
                        goto loop_peg_314;
                        success_peg_316:
                        ;
                        result_peg_298.addResult(result_peg_315);
                    } while (true);
                    loop_peg_314:
                    ;
                            
                }
                goto success_peg_303;
                goto out_peg_300;
                success_peg_303:
                ;
            
            
            
            result_peg_298 = rule_integer(stream, result_peg_298.getPosition());
                if (result_peg_298.error()){
                    goto out_peg_300;
                }
                num = result_peg_298.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(makeKeyword(result_peg_299.getValues()), num);
                    result_peg_298.setValue(value);
                }
            
            
        }
        
        if (column_peg_271.chunk2 == 0){
            column_peg_271.chunk2 = new Chunk2();
        }
        column_peg_271.chunk2->chunk_collision_default = result_peg_298;
        stream.update(result_peg_298.getPosition());
        
        
        return result_peg_298;
        out_peg_300:
        Result result_peg_324(myposition);
        
        {
        
            for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
                result_peg_324.setValue((void*) "Clsn2");
            
            Result result_peg_325 = result_peg_324;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
                result_peg_324.setValue((void*) ":");
            
            
            
            {
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_341(result_peg_324.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_341.getPosition()))){
                                    result_peg_341.nextPosition();
                                } else {
                                    goto out_peg_344;
                                }
                            }
                            result_peg_341.setValue((void*) " ");
                                
                        }
                        goto success_peg_342;
                        out_peg_344:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_341.getPosition()))){
                                    result_peg_341.nextPosition();
                                } else {
                                    goto out_peg_346;
                                }
                            }
                            result_peg_341.setValue((void*) "\t");
                                
                        }
                        goto success_peg_342;
                        out_peg_346:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_341.getPosition()) == (unsigned char) 255){
                                result_peg_341.nextPosition();
                            } else {
                                goto out_peg_348;
                            }
                            result_peg_341.setValue((void*) 255);
                            
                        }
                        goto success_peg_342;
                        out_peg_348:
                        goto loop_peg_340;
                        success_peg_342:
                        ;
                        result_peg_324.addResult(result_peg_341);
                    } while (true);
                    loop_peg_340:
                    ;
                            
                }
                goto success_peg_329;
                goto out_peg_326;
                success_peg_329:
                ;
            
            
            
            result_peg_324 = rule_integer(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto out_peg_326;
                }
                num = result_peg_324.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(makeKeyword(result_peg_325.getValues()), num);
                    result_peg_324.setValue(value);
                }
            
            
        }
        
        if (column_peg_271.chunk2 == 0){
            column_peg_271.chunk2 = new Chunk2();
        }
        column_peg_271.chunk2->chunk_collision_default = result_peg_324;
        stream.update(result_peg_324.getPosition());
        
        
        return result_peg_324;
        out_peg_326:
        Result result_peg_350(myposition);
        
        {
        
            for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_350.getPosition()))){
                        result_peg_350.nextPosition();
                    } else {
                        goto out_peg_352;
                    }
                }
                result_peg_350.setValue((void*) "Clsn1");
            
            Result result_peg_351 = result_peg_350;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_350.getPosition()))){
                        result_peg_350.nextPosition();
                    } else {
                        goto out_peg_352;
                    }
                }
                result_peg_350.setValue((void*) ":");
            
            
            
            {
                    
                    result_peg_350.reset();
                    do{
                        Result result_peg_367(result_peg_350.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_367.getPosition()))){
                                    result_peg_367.nextPosition();
                                } else {
                                    goto out_peg_370;
                                }
                            }
                            result_peg_367.setValue((void*) " ");
                                
                        }
                        goto success_peg_368;
                        out_peg_370:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_367.getPosition()))){
                                    result_peg_367.nextPosition();
                                } else {
                                    goto out_peg_372;
                                }
                            }
                            result_peg_367.setValue((void*) "\t");
                                
                        }
                        goto success_peg_368;
                        out_peg_372:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_367.getPosition()) == (unsigned char) 255){
                                result_peg_367.nextPosition();
                            } else {
                                goto out_peg_374;
                            }
                            result_peg_367.setValue((void*) 255);
                            
                        }
                        goto success_peg_368;
                        out_peg_374:
                        goto loop_peg_366;
                        success_peg_368:
                        ;
                        result_peg_350.addResult(result_peg_367);
                    } while (true);
                    loop_peg_366:
                    ;
                            
                }
                goto success_peg_355;
                goto out_peg_352;
                success_peg_355:
                ;
            
            
            
            result_peg_350 = rule_integer(stream, result_peg_350.getPosition());
                if (result_peg_350.error()){
                    goto out_peg_352;
                }
                num = result_peg_350.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(makeKeyword(result_peg_351.getValues()), num);
                    result_peg_350.setValue(value);
                }
            
            
        }
        
        if (column_peg_271.chunk2 == 0){
            column_peg_271.chunk2 = new Chunk2();
        }
        column_peg_271.chunk2->chunk_collision_default = result_peg_350;
        stream.update(result_peg_350.getPosition());
        
        
        return result_peg_350;
        out_peg_352:
    
        if (column_peg_271.chunk2 == 0){
            column_peg_271.chunk2 = new Chunk2();
        }
        column_peg_271.chunk2->chunk_collision_default = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_377 = stream.getColumn(position);
    if (column_peg_377.chunk2 != 0 && column_peg_377.chunk2->chunk_collision.calculated()){
        return column_peg_377.chunk2->chunk_collision;
    }
    
    RuleTrace trace_peg_868(stream, "collision");
    int myposition = position;
    
    
    Value name;
        Value index;
        Value n1;
        Value n2;
        Value n3;
        Value n4;
    Result result_peg_378(myposition);
        
        {
        
            for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) "Clsn2");
                name = result_peg_378.getValues();
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_394(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_394.getPosition()))){
                                    result_peg_394.nextPosition();
                                } else {
                                    goto out_peg_397;
                                }
                            }
                            result_peg_394.setValue((void*) " ");
                                
                        }
                        goto success_peg_395;
                        out_peg_397:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_394.getPosition()))){
                                    result_peg_394.nextPosition();
                                } else {
                                    goto out_peg_399;
                                }
                            }
                            result_peg_394.setValue((void*) "\t");
                                
                        }
                        goto success_peg_395;
                        out_peg_399:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_394.getPosition()) == (unsigned char) 255){
                                result_peg_394.nextPosition();
                            } else {
                                goto out_peg_401;
                            }
                            result_peg_394.setValue((void*) 255);
                            
                        }
                        goto success_peg_395;
                        out_peg_401:
                        goto loop_peg_393;
                        success_peg_395:
                        ;
                        result_peg_378.addResult(result_peg_394);
                    } while (true);
                    loop_peg_393:
                    ;
                            
                }
                goto success_peg_382;
                goto out_peg_380;
                success_peg_382:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_416(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_416.getPosition()))){
                                    result_peg_416.nextPosition();
                                } else {
                                    goto out_peg_419;
                                }
                            }
                            result_peg_416.setValue((void*) " ");
                                
                        }
                        goto success_peg_417;
                        out_peg_419:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_416.getPosition()))){
                                    result_peg_416.nextPosition();
                                } else {
                                    goto out_peg_421;
                                }
                            }
                            result_peg_416.setValue((void*) "\t");
                                
                        }
                        goto success_peg_417;
                        out_peg_421:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_416.getPosition()) == (unsigned char) 255){
                                result_peg_416.nextPosition();
                            } else {
                                goto out_peg_423;
                            }
                            result_peg_416.setValue((void*) 255);
                            
                        }
                        goto success_peg_417;
                        out_peg_423:
                        goto loop_peg_415;
                        success_peg_417:
                        ;
                        result_peg_378.addResult(result_peg_416);
                    } while (true);
                    loop_peg_415:
                    ;
                            
                }
                goto success_peg_404;
                goto out_peg_380;
                success_peg_404:
                ;
            
            
            
            result_peg_378 = rule_integer(stream, result_peg_378.getPosition());
                if (result_peg_378.error()){
                    goto out_peg_380;
                }
                index = result_peg_378.getValues();
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_438(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_438.getPosition()))){
                                    result_peg_438.nextPosition();
                                } else {
                                    goto out_peg_441;
                                }
                            }
                            result_peg_438.setValue((void*) " ");
                                
                        }
                        goto success_peg_439;
                        out_peg_441:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_438.getPosition()))){
                                    result_peg_438.nextPosition();
                                } else {
                                    goto out_peg_443;
                                }
                            }
                            result_peg_438.setValue((void*) "\t");
                                
                        }
                        goto success_peg_439;
                        out_peg_443:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_438.getPosition()) == (unsigned char) 255){
                                result_peg_438.nextPosition();
                            } else {
                                goto out_peg_445;
                            }
                            result_peg_438.setValue((void*) 255);
                            
                        }
                        goto success_peg_439;
                        out_peg_445:
                        goto loop_peg_437;
                        success_peg_439:
                        ;
                        result_peg_378.addResult(result_peg_438);
                    } while (true);
                    loop_peg_437:
                    ;
                            
                }
                goto success_peg_426;
                goto out_peg_380;
                success_peg_426:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) "]");
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_460(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_460.getPosition()))){
                                    result_peg_460.nextPosition();
                                } else {
                                    goto out_peg_463;
                                }
                            }
                            result_peg_460.setValue((void*) " ");
                                
                        }
                        goto success_peg_461;
                        out_peg_463:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_460.getPosition()))){
                                    result_peg_460.nextPosition();
                                } else {
                                    goto out_peg_465;
                                }
                            }
                            result_peg_460.setValue((void*) "\t");
                                
                        }
                        goto success_peg_461;
                        out_peg_465:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_460.getPosition()) == (unsigned char) 255){
                                result_peg_460.nextPosition();
                            } else {
                                goto out_peg_467;
                            }
                            result_peg_460.setValue((void*) 255);
                            
                        }
                        goto success_peg_461;
                        out_peg_467:
                        goto loop_peg_459;
                        success_peg_461:
                        ;
                        result_peg_378.addResult(result_peg_460);
                    } while (true);
                    loop_peg_459:
                    ;
                            
                }
                goto success_peg_448;
                goto out_peg_380;
                success_peg_448:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_482(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_482.getPosition()))){
                                    result_peg_482.nextPosition();
                                } else {
                                    goto out_peg_485;
                                }
                            }
                            result_peg_482.setValue((void*) " ");
                                
                        }
                        goto success_peg_483;
                        out_peg_485:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_482.getPosition()))){
                                    result_peg_482.nextPosition();
                                } else {
                                    goto out_peg_487;
                                }
                            }
                            result_peg_482.setValue((void*) "\t");
                                
                        }
                        goto success_peg_483;
                        out_peg_487:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_482.getPosition()) == (unsigned char) 255){
                                result_peg_482.nextPosition();
                            } else {
                                goto out_peg_489;
                            }
                            result_peg_482.setValue((void*) 255);
                            
                        }
                        goto success_peg_483;
                        out_peg_489:
                        goto loop_peg_481;
                        success_peg_483:
                        ;
                        result_peg_378.addResult(result_peg_482);
                    } while (true);
                    loop_peg_481:
                    ;
                            
                }
                goto success_peg_470;
                goto out_peg_380;
                success_peg_470:
                ;
            
            
            
            result_peg_378 = rule_integer(stream, result_peg_378.getPosition());
                if (result_peg_378.error()){
                    goto out_peg_380;
                }
                n1 = result_peg_378.getValues();
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_504(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_504.getPosition()))){
                                    result_peg_504.nextPosition();
                                } else {
                                    goto out_peg_507;
                                }
                            }
                            result_peg_504.setValue((void*) " ");
                                
                        }
                        goto success_peg_505;
                        out_peg_507:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_504.getPosition()))){
                                    result_peg_504.nextPosition();
                                } else {
                                    goto out_peg_509;
                                }
                            }
                            result_peg_504.setValue((void*) "\t");
                                
                        }
                        goto success_peg_505;
                        out_peg_509:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_504.getPosition()) == (unsigned char) 255){
                                result_peg_504.nextPosition();
                            } else {
                                goto out_peg_511;
                            }
                            result_peg_504.setValue((void*) 255);
                            
                        }
                        goto success_peg_505;
                        out_peg_511:
                        goto loop_peg_503;
                        success_peg_505:
                        ;
                        result_peg_378.addResult(result_peg_504);
                    } while (true);
                    loop_peg_503:
                    ;
                            
                }
                goto success_peg_492;
                goto out_peg_380;
                success_peg_492:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_526(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_526.getPosition()))){
                                    result_peg_526.nextPosition();
                                } else {
                                    goto out_peg_529;
                                }
                            }
                            result_peg_526.setValue((void*) " ");
                                
                        }
                        goto success_peg_527;
                        out_peg_529:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_526.getPosition()))){
                                    result_peg_526.nextPosition();
                                } else {
                                    goto out_peg_531;
                                }
                            }
                            result_peg_526.setValue((void*) "\t");
                                
                        }
                        goto success_peg_527;
                        out_peg_531:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_526.getPosition()) == (unsigned char) 255){
                                result_peg_526.nextPosition();
                            } else {
                                goto out_peg_533;
                            }
                            result_peg_526.setValue((void*) 255);
                            
                        }
                        goto success_peg_527;
                        out_peg_533:
                        goto loop_peg_525;
                        success_peg_527:
                        ;
                        result_peg_378.addResult(result_peg_526);
                    } while (true);
                    loop_peg_525:
                    ;
                            
                }
                goto success_peg_514;
                goto out_peg_380;
                success_peg_514:
                ;
            
            
            
            result_peg_378 = rule_integer(stream, result_peg_378.getPosition());
                if (result_peg_378.error()){
                    goto out_peg_380;
                }
                n2 = result_peg_378.getValues();
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_548(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_548.getPosition()))){
                                    result_peg_548.nextPosition();
                                } else {
                                    goto out_peg_551;
                                }
                            }
                            result_peg_548.setValue((void*) " ");
                                
                        }
                        goto success_peg_549;
                        out_peg_551:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_548.getPosition()))){
                                    result_peg_548.nextPosition();
                                } else {
                                    goto out_peg_553;
                                }
                            }
                            result_peg_548.setValue((void*) "\t");
                                
                        }
                        goto success_peg_549;
                        out_peg_553:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_548.getPosition()) == (unsigned char) 255){
                                result_peg_548.nextPosition();
                            } else {
                                goto out_peg_555;
                            }
                            result_peg_548.setValue((void*) 255);
                            
                        }
                        goto success_peg_549;
                        out_peg_555:
                        goto loop_peg_547;
                        success_peg_549:
                        ;
                        result_peg_378.addResult(result_peg_548);
                    } while (true);
                    loop_peg_547:
                    ;
                            
                }
                goto success_peg_536;
                goto out_peg_380;
                success_peg_536:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_570(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_570.getPosition()))){
                                    result_peg_570.nextPosition();
                                } else {
                                    goto out_peg_573;
                                }
                            }
                            result_peg_570.setValue((void*) " ");
                                
                        }
                        goto success_peg_571;
                        out_peg_573:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_570.getPosition()))){
                                    result_peg_570.nextPosition();
                                } else {
                                    goto out_peg_575;
                                }
                            }
                            result_peg_570.setValue((void*) "\t");
                                
                        }
                        goto success_peg_571;
                        out_peg_575:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_570.getPosition()) == (unsigned char) 255){
                                result_peg_570.nextPosition();
                            } else {
                                goto out_peg_577;
                            }
                            result_peg_570.setValue((void*) 255);
                            
                        }
                        goto success_peg_571;
                        out_peg_577:
                        goto loop_peg_569;
                        success_peg_571:
                        ;
                        result_peg_378.addResult(result_peg_570);
                    } while (true);
                    loop_peg_569:
                    ;
                            
                }
                goto success_peg_558;
                goto out_peg_380;
                success_peg_558:
                ;
            
            
            
            result_peg_378 = rule_integer(stream, result_peg_378.getPosition());
                if (result_peg_378.error()){
                    goto out_peg_380;
                }
                n3 = result_peg_378.getValues();
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_592(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_592.getPosition()))){
                                    result_peg_592.nextPosition();
                                } else {
                                    goto out_peg_595;
                                }
                            }
                            result_peg_592.setValue((void*) " ");
                                
                        }
                        goto success_peg_593;
                        out_peg_595:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_592.getPosition()))){
                                    result_peg_592.nextPosition();
                                } else {
                                    goto out_peg_597;
                                }
                            }
                            result_peg_592.setValue((void*) "\t");
                                
                        }
                        goto success_peg_593;
                        out_peg_597:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_592.getPosition()) == (unsigned char) 255){
                                result_peg_592.nextPosition();
                            } else {
                                goto out_peg_599;
                            }
                            result_peg_592.setValue((void*) 255);
                            
                        }
                        goto success_peg_593;
                        out_peg_599:
                        goto loop_peg_591;
                        success_peg_593:
                        ;
                        result_peg_378.addResult(result_peg_592);
                    } while (true);
                    loop_peg_591:
                    ;
                            
                }
                goto success_peg_580;
                goto out_peg_380;
                success_peg_580:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_378.getPosition()))){
                        result_peg_378.nextPosition();
                    } else {
                        goto out_peg_380;
                    }
                }
                result_peg_378.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_378.reset();
                    do{
                        Result result_peg_614(result_peg_378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_614.getPosition()))){
                                    result_peg_614.nextPosition();
                                } else {
                                    goto out_peg_617;
                                }
                            }
                            result_peg_614.setValue((void*) " ");
                                
                        }
                        goto success_peg_615;
                        out_peg_617:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_614.getPosition()))){
                                    result_peg_614.nextPosition();
                                } else {
                                    goto out_peg_619;
                                }
                            }
                            result_peg_614.setValue((void*) "\t");
                                
                        }
                        goto success_peg_615;
                        out_peg_619:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_614.getPosition()) == (unsigned char) 255){
                                result_peg_614.nextPosition();
                            } else {
                                goto out_peg_621;
                            }
                            result_peg_614.setValue((void*) 255);
                            
                        }
                        goto success_peg_615;
                        out_peg_621:
                        goto loop_peg_613;
                        success_peg_615:
                        ;
                        result_peg_378.addResult(result_peg_614);
                    } while (true);
                    loop_peg_613:
                    ;
                            
                }
                goto success_peg_602;
                goto out_peg_380;
                success_peg_602:
                ;
            
            
            
            result_peg_378 = rule_integer(stream, result_peg_378.getPosition());
                if (result_peg_378.error()){
                    goto out_peg_380;
                }
                n4 = result_peg_378.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_378.setValue(value);
                }
            
            
        }
        
        if (column_peg_377.chunk2 == 0){
            column_peg_377.chunk2 = new Chunk2();
        }
        column_peg_377.chunk2->chunk_collision = result_peg_378;
        stream.update(result_peg_378.getPosition());
        
        
        return result_peg_378;
        out_peg_380:
        Result result_peg_623(myposition);
        
        {
        
            for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) "Clsn1");
                name = result_peg_623.getValues();
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_639(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_639.getPosition()))){
                                    result_peg_639.nextPosition();
                                } else {
                                    goto out_peg_642;
                                }
                            }
                            result_peg_639.setValue((void*) " ");
                                
                        }
                        goto success_peg_640;
                        out_peg_642:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_639.getPosition()))){
                                    result_peg_639.nextPosition();
                                } else {
                                    goto out_peg_644;
                                }
                            }
                            result_peg_639.setValue((void*) "\t");
                                
                        }
                        goto success_peg_640;
                        out_peg_644:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_639.getPosition()) == (unsigned char) 255){
                                result_peg_639.nextPosition();
                            } else {
                                goto out_peg_646;
                            }
                            result_peg_639.setValue((void*) 255);
                            
                        }
                        goto success_peg_640;
                        out_peg_646:
                        goto loop_peg_638;
                        success_peg_640:
                        ;
                        result_peg_623.addResult(result_peg_639);
                    } while (true);
                    loop_peg_638:
                    ;
                            
                }
                goto success_peg_627;
                goto out_peg_625;
                success_peg_627:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_661(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_661.getPosition()))){
                                    result_peg_661.nextPosition();
                                } else {
                                    goto out_peg_664;
                                }
                            }
                            result_peg_661.setValue((void*) " ");
                                
                        }
                        goto success_peg_662;
                        out_peg_664:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_661.getPosition()))){
                                    result_peg_661.nextPosition();
                                } else {
                                    goto out_peg_666;
                                }
                            }
                            result_peg_661.setValue((void*) "\t");
                                
                        }
                        goto success_peg_662;
                        out_peg_666:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_661.getPosition()) == (unsigned char) 255){
                                result_peg_661.nextPosition();
                            } else {
                                goto out_peg_668;
                            }
                            result_peg_661.setValue((void*) 255);
                            
                        }
                        goto success_peg_662;
                        out_peg_668:
                        goto loop_peg_660;
                        success_peg_662:
                        ;
                        result_peg_623.addResult(result_peg_661);
                    } while (true);
                    loop_peg_660:
                    ;
                            
                }
                goto success_peg_649;
                goto out_peg_625;
                success_peg_649:
                ;
            
            
            
            result_peg_623 = rule_integer(stream, result_peg_623.getPosition());
                if (result_peg_623.error()){
                    goto out_peg_625;
                }
                index = result_peg_623.getValues();
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_683(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_683.getPosition()))){
                                    result_peg_683.nextPosition();
                                } else {
                                    goto out_peg_686;
                                }
                            }
                            result_peg_683.setValue((void*) " ");
                                
                        }
                        goto success_peg_684;
                        out_peg_686:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_683.getPosition()))){
                                    result_peg_683.nextPosition();
                                } else {
                                    goto out_peg_688;
                                }
                            }
                            result_peg_683.setValue((void*) "\t");
                                
                        }
                        goto success_peg_684;
                        out_peg_688:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_683.getPosition()) == (unsigned char) 255){
                                result_peg_683.nextPosition();
                            } else {
                                goto out_peg_690;
                            }
                            result_peg_683.setValue((void*) 255);
                            
                        }
                        goto success_peg_684;
                        out_peg_690:
                        goto loop_peg_682;
                        success_peg_684:
                        ;
                        result_peg_623.addResult(result_peg_683);
                    } while (true);
                    loop_peg_682:
                    ;
                            
                }
                goto success_peg_671;
                goto out_peg_625;
                success_peg_671:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) "]");
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_705(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_705.getPosition()))){
                                    result_peg_705.nextPosition();
                                } else {
                                    goto out_peg_708;
                                }
                            }
                            result_peg_705.setValue((void*) " ");
                                
                        }
                        goto success_peg_706;
                        out_peg_708:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_705.getPosition()))){
                                    result_peg_705.nextPosition();
                                } else {
                                    goto out_peg_710;
                                }
                            }
                            result_peg_705.setValue((void*) "\t");
                                
                        }
                        goto success_peg_706;
                        out_peg_710:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_705.getPosition()) == (unsigned char) 255){
                                result_peg_705.nextPosition();
                            } else {
                                goto out_peg_712;
                            }
                            result_peg_705.setValue((void*) 255);
                            
                        }
                        goto success_peg_706;
                        out_peg_712:
                        goto loop_peg_704;
                        success_peg_706:
                        ;
                        result_peg_623.addResult(result_peg_705);
                    } while (true);
                    loop_peg_704:
                    ;
                            
                }
                goto success_peg_693;
                goto out_peg_625;
                success_peg_693:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_727(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_727.getPosition()))){
                                    result_peg_727.nextPosition();
                                } else {
                                    goto out_peg_730;
                                }
                            }
                            result_peg_727.setValue((void*) " ");
                                
                        }
                        goto success_peg_728;
                        out_peg_730:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_727.getPosition()))){
                                    result_peg_727.nextPosition();
                                } else {
                                    goto out_peg_732;
                                }
                            }
                            result_peg_727.setValue((void*) "\t");
                                
                        }
                        goto success_peg_728;
                        out_peg_732:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_727.getPosition()) == (unsigned char) 255){
                                result_peg_727.nextPosition();
                            } else {
                                goto out_peg_734;
                            }
                            result_peg_727.setValue((void*) 255);
                            
                        }
                        goto success_peg_728;
                        out_peg_734:
                        goto loop_peg_726;
                        success_peg_728:
                        ;
                        result_peg_623.addResult(result_peg_727);
                    } while (true);
                    loop_peg_726:
                    ;
                            
                }
                goto success_peg_715;
                goto out_peg_625;
                success_peg_715:
                ;
            
            
            
            result_peg_623 = rule_integer(stream, result_peg_623.getPosition());
                if (result_peg_623.error()){
                    goto out_peg_625;
                }
                n1 = result_peg_623.getValues();
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_749(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_749.getPosition()))){
                                    result_peg_749.nextPosition();
                                } else {
                                    goto out_peg_752;
                                }
                            }
                            result_peg_749.setValue((void*) " ");
                                
                        }
                        goto success_peg_750;
                        out_peg_752:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_749.getPosition()))){
                                    result_peg_749.nextPosition();
                                } else {
                                    goto out_peg_754;
                                }
                            }
                            result_peg_749.setValue((void*) "\t");
                                
                        }
                        goto success_peg_750;
                        out_peg_754:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_749.getPosition()) == (unsigned char) 255){
                                result_peg_749.nextPosition();
                            } else {
                                goto out_peg_756;
                            }
                            result_peg_749.setValue((void*) 255);
                            
                        }
                        goto success_peg_750;
                        out_peg_756:
                        goto loop_peg_748;
                        success_peg_750:
                        ;
                        result_peg_623.addResult(result_peg_749);
                    } while (true);
                    loop_peg_748:
                    ;
                            
                }
                goto success_peg_737;
                goto out_peg_625;
                success_peg_737:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_771(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_771.getPosition()))){
                                    result_peg_771.nextPosition();
                                } else {
                                    goto out_peg_774;
                                }
                            }
                            result_peg_771.setValue((void*) " ");
                                
                        }
                        goto success_peg_772;
                        out_peg_774:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_771.getPosition()))){
                                    result_peg_771.nextPosition();
                                } else {
                                    goto out_peg_776;
                                }
                            }
                            result_peg_771.setValue((void*) "\t");
                                
                        }
                        goto success_peg_772;
                        out_peg_776:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_771.getPosition()) == (unsigned char) 255){
                                result_peg_771.nextPosition();
                            } else {
                                goto out_peg_778;
                            }
                            result_peg_771.setValue((void*) 255);
                            
                        }
                        goto success_peg_772;
                        out_peg_778:
                        goto loop_peg_770;
                        success_peg_772:
                        ;
                        result_peg_623.addResult(result_peg_771);
                    } while (true);
                    loop_peg_770:
                    ;
                            
                }
                goto success_peg_759;
                goto out_peg_625;
                success_peg_759:
                ;
            
            
            
            result_peg_623 = rule_integer(stream, result_peg_623.getPosition());
                if (result_peg_623.error()){
                    goto out_peg_625;
                }
                n2 = result_peg_623.getValues();
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_793(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_793.getPosition()))){
                                    result_peg_793.nextPosition();
                                } else {
                                    goto out_peg_796;
                                }
                            }
                            result_peg_793.setValue((void*) " ");
                                
                        }
                        goto success_peg_794;
                        out_peg_796:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_793.getPosition()))){
                                    result_peg_793.nextPosition();
                                } else {
                                    goto out_peg_798;
                                }
                            }
                            result_peg_793.setValue((void*) "\t");
                                
                        }
                        goto success_peg_794;
                        out_peg_798:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_793.getPosition()) == (unsigned char) 255){
                                result_peg_793.nextPosition();
                            } else {
                                goto out_peg_800;
                            }
                            result_peg_793.setValue((void*) 255);
                            
                        }
                        goto success_peg_794;
                        out_peg_800:
                        goto loop_peg_792;
                        success_peg_794:
                        ;
                        result_peg_623.addResult(result_peg_793);
                    } while (true);
                    loop_peg_792:
                    ;
                            
                }
                goto success_peg_781;
                goto out_peg_625;
                success_peg_781:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_815(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_815.getPosition()))){
                                    result_peg_815.nextPosition();
                                } else {
                                    goto out_peg_818;
                                }
                            }
                            result_peg_815.setValue((void*) " ");
                                
                        }
                        goto success_peg_816;
                        out_peg_818:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_815.getPosition()))){
                                    result_peg_815.nextPosition();
                                } else {
                                    goto out_peg_820;
                                }
                            }
                            result_peg_815.setValue((void*) "\t");
                                
                        }
                        goto success_peg_816;
                        out_peg_820:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_815.getPosition()) == (unsigned char) 255){
                                result_peg_815.nextPosition();
                            } else {
                                goto out_peg_822;
                            }
                            result_peg_815.setValue((void*) 255);
                            
                        }
                        goto success_peg_816;
                        out_peg_822:
                        goto loop_peg_814;
                        success_peg_816:
                        ;
                        result_peg_623.addResult(result_peg_815);
                    } while (true);
                    loop_peg_814:
                    ;
                            
                }
                goto success_peg_803;
                goto out_peg_625;
                success_peg_803:
                ;
            
            
            
            result_peg_623 = rule_integer(stream, result_peg_623.getPosition());
                if (result_peg_623.error()){
                    goto out_peg_625;
                }
                n3 = result_peg_623.getValues();
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_837(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_837.getPosition()))){
                                    result_peg_837.nextPosition();
                                } else {
                                    goto out_peg_840;
                                }
                            }
                            result_peg_837.setValue((void*) " ");
                                
                        }
                        goto success_peg_838;
                        out_peg_840:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_837.getPosition()))){
                                    result_peg_837.nextPosition();
                                } else {
                                    goto out_peg_842;
                                }
                            }
                            result_peg_837.setValue((void*) "\t");
                                
                        }
                        goto success_peg_838;
                        out_peg_842:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_837.getPosition()) == (unsigned char) 255){
                                result_peg_837.nextPosition();
                            } else {
                                goto out_peg_844;
                            }
                            result_peg_837.setValue((void*) 255);
                            
                        }
                        goto success_peg_838;
                        out_peg_844:
                        goto loop_peg_836;
                        success_peg_838:
                        ;
                        result_peg_623.addResult(result_peg_837);
                    } while (true);
                    loop_peg_836:
                    ;
                            
                }
                goto success_peg_825;
                goto out_peg_625;
                success_peg_825:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_623.getPosition()))){
                        result_peg_623.nextPosition();
                    } else {
                        goto out_peg_625;
                    }
                }
                result_peg_623.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_623.reset();
                    do{
                        Result result_peg_859(result_peg_623.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_859.getPosition()))){
                                    result_peg_859.nextPosition();
                                } else {
                                    goto out_peg_862;
                                }
                            }
                            result_peg_859.setValue((void*) " ");
                                
                        }
                        goto success_peg_860;
                        out_peg_862:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_859.getPosition()))){
                                    result_peg_859.nextPosition();
                                } else {
                                    goto out_peg_864;
                                }
                            }
                            result_peg_859.setValue((void*) "\t");
                                
                        }
                        goto success_peg_860;
                        out_peg_864:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_859.getPosition()) == (unsigned char) 255){
                                result_peg_859.nextPosition();
                            } else {
                                goto out_peg_866;
                            }
                            result_peg_859.setValue((void*) 255);
                            
                        }
                        goto success_peg_860;
                        out_peg_866:
                        goto loop_peg_858;
                        success_peg_860:
                        ;
                        result_peg_623.addResult(result_peg_859);
                    } while (true);
                    loop_peg_858:
                    ;
                            
                }
                goto success_peg_847;
                goto out_peg_625;
                success_peg_847:
                ;
            
            
            
            result_peg_623 = rule_integer(stream, result_peg_623.getPosition());
                if (result_peg_623.error()){
                    goto out_peg_625;
                }
                n4 = result_peg_623.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_623.setValue(value);
                }
            
            
        }
        
        if (column_peg_377.chunk2 == 0){
            column_peg_377.chunk2 = new Chunk2();
        }
        column_peg_377.chunk2->chunk_collision = result_peg_623;
        stream.update(result_peg_623.getPosition());
        
        
        return result_peg_623;
        out_peg_625:
    
        if (column_peg_377.chunk2 == 0){
            column_peg_377.chunk2 = new Chunk2();
        }
        column_peg_377.chunk2->chunk_collision = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_869 = stream.getColumn(position);
    if (column_peg_869.chunk2 != 0 && column_peg_869.chunk2->chunk_action_start.calculated()){
        return column_peg_869.chunk2->chunk_action_start;
    }
    
    RuleTrace trace_peg_961(stream, "action_start");
    int myposition = position;
    
    
    Value begin;
        Value action;
        Value num;
    Result result_peg_870(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_870.getPosition()))){
                        result_peg_870.nextPosition();
                    } else {
                        goto out_peg_872;
                    }
                }
                result_peg_870.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_870.reset();
                    do{
                        Result result_peg_886(result_peg_870.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_886.getPosition()))){
                                    result_peg_886.nextPosition();
                                } else {
                                    goto out_peg_889;
                                }
                            }
                            result_peg_886.setValue((void*) " ");
                                
                        }
                        goto success_peg_887;
                        out_peg_889:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_886.getPosition()))){
                                    result_peg_886.nextPosition();
                                } else {
                                    goto out_peg_891;
                                }
                            }
                            result_peg_886.setValue((void*) "\t");
                                
                        }
                        goto success_peg_887;
                        out_peg_891:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_886.getPosition()) == (unsigned char) 255){
                                result_peg_886.nextPosition();
                            } else {
                                goto out_peg_893;
                            }
                            result_peg_886.setValue((void*) 255);
                            
                        }
                        goto success_peg_887;
                        out_peg_893:
                        goto loop_peg_885;
                        success_peg_887:
                        ;
                        result_peg_870.addResult(result_peg_886);
                    } while (true);
                    loop_peg_885:
                    ;
                            
                }
                goto success_peg_874;
                goto out_peg_872;
                success_peg_874:
                ;
            
            
            
            for (int i = 0; i < 5; i++){
                    if (compareCharCase("Begin"[i], stream.get(result_peg_870.getPosition()))){
                        result_peg_870.nextPosition();
                    } else {
                        goto out_peg_872;
                    }
                }
                result_peg_870.setValue((void*) "Begin");
                begin = result_peg_870.getValues();
            
            
            
            {
                    
                    result_peg_870.reset();
                    do{
                        Result result_peg_908(result_peg_870.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_908.getPosition()))){
                                    result_peg_908.nextPosition();
                                } else {
                                    goto out_peg_911;
                                }
                            }
                            result_peg_908.setValue((void*) " ");
                                
                        }
                        goto success_peg_909;
                        out_peg_911:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_908.getPosition()))){
                                    result_peg_908.nextPosition();
                                } else {
                                    goto out_peg_913;
                                }
                            }
                            result_peg_908.setValue((void*) "\t");
                                
                        }
                        goto success_peg_909;
                        out_peg_913:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_908.getPosition()) == (unsigned char) 255){
                                result_peg_908.nextPosition();
                            } else {
                                goto out_peg_915;
                            }
                            result_peg_908.setValue((void*) 255);
                            
                        }
                        goto success_peg_909;
                        out_peg_915:
                        goto loop_peg_907;
                        success_peg_909:
                        ;
                        result_peg_870.addResult(result_peg_908);
                    } while (true);
                    loop_peg_907:
                    ;
                            
                }
                goto success_peg_896;
                goto out_peg_872;
                success_peg_896:
                ;
            
            
            
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("Action"[i], stream.get(result_peg_870.getPosition()))){
                        result_peg_870.nextPosition();
                    } else {
                        goto out_peg_872;
                    }
                }
                result_peg_870.setValue((void*) "Action");
                action = result_peg_870.getValues();
            
            
            
            {
                    
                    result_peg_870.reset();
                    do{
                        Result result_peg_930(result_peg_870.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_930.getPosition()))){
                                    result_peg_930.nextPosition();
                                } else {
                                    goto out_peg_933;
                                }
                            }
                            result_peg_930.setValue((void*) " ");
                                
                        }
                        goto success_peg_931;
                        out_peg_933:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_930.getPosition()))){
                                    result_peg_930.nextPosition();
                                } else {
                                    goto out_peg_935;
                                }
                            }
                            result_peg_930.setValue((void*) "\t");
                                
                        }
                        goto success_peg_931;
                        out_peg_935:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_930.getPosition()) == (unsigned char) 255){
                                result_peg_930.nextPosition();
                            } else {
                                goto out_peg_937;
                            }
                            result_peg_930.setValue((void*) 255);
                            
                        }
                        goto success_peg_931;
                        out_peg_937:
                        goto loop_peg_929;
                        success_peg_931:
                        ;
                        result_peg_870.addResult(result_peg_930);
                    } while (true);
                    loop_peg_929:
                    ;
                            
                }
                goto success_peg_918;
                goto out_peg_872;
                success_peg_918:
                ;
            
            
            
            result_peg_870 = rule_integer(stream, result_peg_870.getPosition());
                if (result_peg_870.error()){
                    goto out_peg_872;
                }
                num = result_peg_870.getValues();
            
            
            
            {
                    
                    result_peg_870.reset();
                    do{
                        Result result_peg_952(result_peg_870.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_952.getPosition()))){
                                    result_peg_952.nextPosition();
                                } else {
                                    goto out_peg_955;
                                }
                            }
                            result_peg_952.setValue((void*) " ");
                                
                        }
                        goto success_peg_953;
                        out_peg_955:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_952.getPosition()))){
                                    result_peg_952.nextPosition();
                                } else {
                                    goto out_peg_957;
                                }
                            }
                            result_peg_952.setValue((void*) "\t");
                                
                        }
                        goto success_peg_953;
                        out_peg_957:
                        {
                            
                            if ((unsigned char) stream.get(result_peg_952.getPosition()) == (unsigned char) 255){
                                result_peg_952.nextPosition();
                            } else {
                                goto out_peg_959;
                            }
                            result_peg_952.setValue((void*) 255);
                            
                        }
                        goto success_peg_953;
                        out_peg_959:
                        goto loop_peg_951;
                        success_peg_953:
                        ;
                        result_peg_870.addResult(result_peg_952);
                    } while (true);
                    loop_peg_951:
                    ;
                            
                }
                goto success_peg_940;
                goto out_peg_872;
                success_peg_940:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_870.getPosition()))){
                        result_peg_870.nextPosition();
                    } else {
                        goto out_peg_872;
                    }
                }
                result_peg_870.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHeader(begin, action, num);
                    result_peg_870.setValue(value);
                }
            
            
        }
        
        if (column_peg_869.chunk2 == 0){
            column_peg_869.chunk2 = new Chunk2();
        }
        column_peg_869.chunk2->chunk_action_start = result_peg_870;
        stream.update(result_peg_870.getPosition());
        
        
        return result_peg_870;
        out_peg_872:
    
        if (column_peg_869.chunk2 == 0){
            column_peg_869.chunk2 = new Chunk2();
        }
        column_peg_869.chunk2->chunk_action_start = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_962 = stream.getColumn(position);
    if (column_peg_962.chunk2 != 0 && column_peg_962.chunk2->chunk_integer.calculated()){
        return column_peg_962.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_980(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_963(myposition);
        
        {
        
            int save_peg_965 = result_peg_963.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_963.getPosition()))){
                            result_peg_963.nextPosition();
                        } else {
                            goto out_peg_968;
                        }
                    }
                    result_peg_963.setValue((void*) "-");
                        
                }
                goto success_peg_966;
                out_peg_968:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_963.getPosition()))){
                            result_peg_963.nextPosition();
                        } else {
                            goto out_peg_970;
                        }
                    }
                    result_peg_963.setValue((void*) "+");
                        
                }
                goto success_peg_966;
                out_peg_970:
                
                result_peg_963 = Result(save_peg_965);
                result_peg_963.setValue((void*) 0);
                
                success_peg_966:
                ;
            
            Result result_peg_964 = result_peg_963;
            
            result_peg_963.reset();
                do{
                    Result result_peg_973(result_peg_963.getPosition());
                    {
                        
                        char letter_peg_978 = stream.get(result_peg_973.getPosition());
                        if (letter_peg_978 != '\0' && strchr("0123456789", letter_peg_978) != NULL){
                            result_peg_973.nextPosition();
                            result_peg_973.setValue((void*) (long) letter_peg_978);
                        } else {
                            goto out_peg_977;
                        }
                        
                    }
                    goto success_peg_974;
                    out_peg_977:
                    goto loop_peg_972;
                    success_peg_974:
                    ;
                    result_peg_963.addResult(result_peg_973);
                } while (true);
                loop_peg_972:
                if (result_peg_963.matches() == 0){
                    goto out_peg_979;
                }
            
            Result result_peg_971 = result_peg_963;
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_971.getValues()); value = makeNumber(result_peg_964.getValues(),value);
                    result_peg_963.setValue(value);
                }
            
            
        }
        
        if (column_peg_962.chunk2 == 0){
            column_peg_962.chunk2 = new Chunk2();
        }
        column_peg_962.chunk2->chunk_integer = result_peg_963;
        stream.update(result_peg_963.getPosition());
        
        
        return result_peg_963;
        out_peg_979:
    
        if (column_peg_962.chunk2 == 0){
            column_peg_962.chunk2 = new Chunk2();
        }
        column_peg_962.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_981 = stream.getColumn(position);
    if (column_peg_981.chunk2 != 0 && column_peg_981.chunk2->chunk_valuelist.calculated()){
        return column_peg_981.chunk2->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1032(stream, "valuelist");
    int myposition = position;
    
    
    
    Result result_peg_982(myposition);
        
        {
        
            result_peg_982 = rule_value(stream, result_peg_982.getPosition());
                if (result_peg_982.error()){
                    goto out_peg_984;
                }
            
            Result result_peg_983 = result_peg_982;
            
            result_peg_982.reset();
                do{
                    Result result_peg_987(result_peg_982.getPosition());
                    {
                    
                        {
                                
                                result_peg_987.reset();
                                do{
                                    Result result_peg_1001(result_peg_987.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1001.getPosition()))){
                                                result_peg_1001.nextPosition();
                                            } else {
                                                goto out_peg_1004;
                                            }
                                        }
                                        result_peg_1001.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1002;
                                    out_peg_1004:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1001.getPosition()))){
                                                result_peg_1001.nextPosition();
                                            } else {
                                                goto out_peg_1006;
                                            }
                                        }
                                        result_peg_1001.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1002;
                                    out_peg_1006:
                                    {
                                        
                                        if ((unsigned char) stream.get(result_peg_1001.getPosition()) == (unsigned char) 255){
                                            result_peg_1001.nextPosition();
                                        } else {
                                            goto out_peg_1008;
                                        }
                                        result_peg_1001.setValue((void*) 255);
                                        
                                    }
                                    goto success_peg_1002;
                                    out_peg_1008:
                                    goto loop_peg_1000;
                                    success_peg_1002:
                                    ;
                                    result_peg_987.addResult(result_peg_1001);
                                } while (true);
                                loop_peg_1000:
                                ;
                                        
                            }
                            goto success_peg_989;
                            goto loop_peg_986;
                            success_peg_989:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_987.getPosition()))){
                                    result_peg_987.nextPosition();
                                } else {
                                    goto loop_peg_986;
                                }
                            }
                            result_peg_987.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_987.reset();
                                do{
                                    Result result_peg_1023(result_peg_987.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1023.getPosition()))){
                                                result_peg_1023.nextPosition();
                                            } else {
                                                goto out_peg_1026;
                                            }
                                        }
                                        result_peg_1023.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1024;
                                    out_peg_1026:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1023.getPosition()))){
                                                result_peg_1023.nextPosition();
                                            } else {
                                                goto out_peg_1028;
                                            }
                                        }
                                        result_peg_1023.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1024;
                                    out_peg_1028:
                                    {
                                        
                                        if ((unsigned char) stream.get(result_peg_1023.getPosition()) == (unsigned char) 255){
                                            result_peg_1023.nextPosition();
                                        } else {
                                            goto out_peg_1030;
                                        }
                                        result_peg_1023.setValue((void*) 255);
                                        
                                    }
                                    goto success_peg_1024;
                                    out_peg_1030:
                                    goto loop_peg_1022;
                                    success_peg_1024:
                                    ;
                                    result_peg_987.addResult(result_peg_1023);
                                } while (true);
                                loop_peg_1022:
                                ;
                                        
                            }
                            goto success_peg_1011;
                            goto loop_peg_986;
                            success_peg_1011:
                            ;
                        
                        
                        
                        int save_peg_1031 = result_peg_987.getPosition();
                            
                            result_peg_987 = rule_value(stream, result_peg_987.getPosition());
                            if (result_peg_987.error()){
                                
                                result_peg_987 = Result(save_peg_1031);
                                result_peg_987.setValue((void*) 0);
                                
                            }
                        
                        
                    }
                    result_peg_982.addResult(result_peg_987);
                } while (true);
                loop_peg_986:
                ;
            
            Result result_peg_985 = result_peg_982;
            
            {
                    Value value((void*) 0);
                    value = makeValueList(result_peg_983.getValues(),result_peg_985.getValues());
                    result_peg_982.setValue(value);
                }
            
            
        }
        
        if (column_peg_981.chunk2 == 0){
            column_peg_981.chunk2 = new Chunk2();
        }
        column_peg_981.chunk2->chunk_valuelist = result_peg_982;
        stream.update(result_peg_982.getPosition());
        
        
        return result_peg_982;
        out_peg_984:
    
        if (column_peg_981.chunk2 == 0){
            column_peg_981.chunk2 = new Chunk2();
        }
        column_peg_981.chunk2->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1033 = stream.getColumn(position);
    if (column_peg_1033.chunk3 != 0 && column_peg_1033.chunk3->chunk_value.calculated()){
        return column_peg_1033.chunk3->chunk_value;
    }
    
    RuleTrace trace_peg_1064(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_1034(myposition);
        
        result_peg_1034 = rule_integer(stream, result_peg_1034.getPosition());
        if (result_peg_1034.error()){
            goto out_peg_1035;
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1034;
        stream.update(result_peg_1034.getPosition());
        
        
        return result_peg_1034;
        out_peg_1035:
        Result result_peg_1036(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareCharCase("as"[i], stream.get(result_peg_1036.getPosition()))){
                        result_peg_1036.nextPosition();
                    } else {
                        goto out_peg_1038;
                    }
                }
                result_peg_1036.setValue((void*) "as");
            
            Result result_peg_1037 = result_peg_1036;
            
            result_peg_1036 = rule_integer(stream, result_peg_1036.getPosition());
                if (result_peg_1036.error()){
                    goto out_peg_1038;
                }
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("d"[i], stream.get(result_peg_1036.getPosition()))){
                        result_peg_1036.nextPosition();
                    } else {
                        goto out_peg_1038;
                    }
                }
                result_peg_1036.setValue((void*) "d");
            
            
            
            result_peg_1036 = rule_integer(stream, result_peg_1036.getPosition());
                if (result_peg_1036.error()){
                    goto out_peg_1038;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1037.getValues());
                    result_peg_1036.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1036;
        stream.update(result_peg_1036.getPosition());
        
        
        return result_peg_1036;
        out_peg_1038:
        Result result_peg_1042(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_1042.getPosition()))){
                        result_peg_1042.nextPosition();
                    } else {
                        goto out_peg_1044;
                    }
                }
                result_peg_1042.setValue((void*) "a");
            
            Result result_peg_1043 = result_peg_1042;
            
            result_peg_1042 = rule_integer(stream, result_peg_1042.getPosition());
                if (result_peg_1042.error()){
                    goto out_peg_1044;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1043.getValues());
                    result_peg_1042.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1042;
        stream.update(result_peg_1042.getPosition());
        
        
        return result_peg_1042;
        out_peg_1044:
        Result result_peg_1046(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_1046.getPosition()))){
                        result_peg_1046.nextPosition();
                    } else {
                        goto out_peg_1048;
                    }
                }
                result_peg_1046.setValue((void*) "a");
            
            Result result_peg_1047 = result_peg_1046;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1047.getValues());
                    result_peg_1046.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1046;
        stream.update(result_peg_1046.getPosition());
        
        
        return result_peg_1046;
        out_peg_1048:
        Result result_peg_1049(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_1049.getPosition()))){
                        result_peg_1049.nextPosition();
                    } else {
                        goto out_peg_1051;
                    }
                }
                result_peg_1049.setValue((void*) "s");
            
            Result result_peg_1050 = result_peg_1049;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1050.getValues());
                    result_peg_1049.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1049;
        stream.update(result_peg_1049.getPosition());
        
        
        return result_peg_1049;
        out_peg_1051:
        Result result_peg_1052(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareCharCase("vh"[i], stream.get(result_peg_1052.getPosition()))){
                        result_peg_1052.nextPosition();
                    } else {
                        goto out_peg_1054;
                    }
                }
                result_peg_1052.setValue((void*) "vh");
            
            Result result_peg_1053 = result_peg_1052;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1053.getValues());
                    result_peg_1052.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1052;
        stream.update(result_peg_1052.getPosition());
        
        
        return result_peg_1052;
        out_peg_1054:
        Result result_peg_1055(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareCharCase("hv"[i], stream.get(result_peg_1055.getPosition()))){
                        result_peg_1055.nextPosition();
                    } else {
                        goto out_peg_1057;
                    }
                }
                result_peg_1055.setValue((void*) "hv");
            
            Result result_peg_1056 = result_peg_1055;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1056.getValues());
                    result_peg_1055.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1055;
        stream.update(result_peg_1055.getPosition());
        
        
        return result_peg_1055;
        out_peg_1057:
        Result result_peg_1058(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("v"[i], stream.get(result_peg_1058.getPosition()))){
                        result_peg_1058.nextPosition();
                    } else {
                        goto out_peg_1060;
                    }
                }
                result_peg_1058.setValue((void*) "v");
            
            Result result_peg_1059 = result_peg_1058;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1059.getValues());
                    result_peg_1058.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1058;
        stream.update(result_peg_1058.getPosition());
        
        
        return result_peg_1058;
        out_peg_1060:
        Result result_peg_1061(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_1061.getPosition()))){
                        result_peg_1061.nextPosition();
                    } else {
                        goto out_peg_1063;
                    }
                }
                result_peg_1061.setValue((void*) "h");
            
            Result result_peg_1062 = result_peg_1061;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1062.getValues());
                    result_peg_1061.setValue(value);
                }
            
            
        }
        
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = result_peg_1061;
        stream.update(result_peg_1061.getPosition());
        
        
        return result_peg_1061;
        out_peg_1063:
    
        if (column_peg_1033.chunk3 == 0){
            column_peg_1033.chunk3 = new Chunk3();
        }
        column_peg_1033.chunk3->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

const void * parse(const std::string & filename, bool stats = false){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::ostringstream out;
        out << "Error while parsing " << filename << " " << stream.reportError();
        throw ParseException(out.str());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}

const void * parse(const char * in, bool stats = false){
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

const void * parse(const char * in, int length, bool stats = false){
    Stream stream(in, length);
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

        
