

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
    
    RuleTrace trace_peg_265(stream, "comment");
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
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_249.getPosition()))){
                                result_peg_249.nextPosition();
                            } else {
                                goto not_peg_248;
                            }
                        }
                        result_peg_249.setValue((void*) "\n");
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
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_258.getPosition()))){
                                result_peg_258.nextPosition();
                            } else {
                                goto not_peg_257;
                            }
                        }
                        result_peg_258.setValue((void*) "\n");
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
        
        
        
        result_peg_260.reset();
            do{
                Result result_peg_264(result_peg_260.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_264.getPosition()))){
                        result_peg_264.nextPosition();
                    } else {
                        goto loop_peg_263;
                    }
                }
                result_peg_264.setValue((void*) "-");
                result_peg_260.addResult(result_peg_264);
            } while (true);
            loop_peg_263:
            if (result_peg_260.matches() == 0){
                goto out_peg_262;
            }
        
        
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
    
    Column & column_peg_266 = stream.getColumn(position);
    if (column_peg_266.chunk2 != 0 && column_peg_266.chunk2->chunk_collision_default.calculated()){
        return column_peg_266.chunk2->chunk_collision_default;
    }
    
    RuleTrace trace_peg_371(stream, "collision_default");
    int myposition = position;
    
    Value num;
    Result result_peg_267(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("Clsn2Default"[i], stream.get(result_peg_267.getPosition()))){
                    result_peg_267.nextPosition();
                } else {
                    goto out_peg_269;
                }
            }
            result_peg_267.setValue((void*) "Clsn2Default");
        
        Result result_peg_268 = result_peg_267;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_267.getPosition()))){
                    result_peg_267.nextPosition();
                } else {
                    goto out_peg_269;
                }
            }
            result_peg_267.setValue((void*) ":");
        
        
        
        {
                
                result_peg_267.reset();
                do{
                    Result result_peg_284(result_peg_267.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_284.getPosition()))){
                                result_peg_284.nextPosition();
                            } else {
                                goto out_peg_287;
                            }
                        }
                        result_peg_284.setValue((void*) " ");
                            
                    }
                    goto success_peg_285;
                    out_peg_287:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_284.getPosition()))){
                                result_peg_284.nextPosition();
                            } else {
                                goto out_peg_289;
                            }
                        }
                        result_peg_284.setValue((void*) "\t");
                            
                    }
                    goto success_peg_285;
                    out_peg_289:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_284.getPosition()) == (unsigned char) 255){
                            result_peg_284.nextPosition();
                        } else {
                            goto out_peg_291;
                        }
                        result_peg_284.setValue((void*) 255);
                        
                    }
                    goto success_peg_285;
                    out_peg_291:
                    goto loop_peg_283;
                    success_peg_285:
                    ;
                    result_peg_267.addResult(result_peg_284);
                } while (true);
                loop_peg_283:
                ;
                        
            }
            goto success_peg_272;
            goto out_peg_269;
            success_peg_272:
            ;
        
        
        
        result_peg_267 = rule_integer(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
            num = result_peg_267.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_268.getValues()), num);
                result_peg_267.setValue(value);
            }
        
        
    }
    
    if (column_peg_266.chunk2 == 0){
        column_peg_266.chunk2 = new Chunk2();
    }
    column_peg_266.chunk2->chunk_collision_default = result_peg_267;
    stream.update(result_peg_267.getPosition());
    
    
    return result_peg_267;
    out_peg_269:
    Result result_peg_293(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_293.getPosition()))){
                    result_peg_293.nextPosition();
                } else {
                    goto out_peg_295;
                }
            }
            result_peg_293.setValue((void*) "Clsn1Default");
        
        Result result_peg_294 = result_peg_293;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_293.getPosition()))){
                    result_peg_293.nextPosition();
                } else {
                    goto out_peg_295;
                }
            }
            result_peg_293.setValue((void*) ":");
        
        
        
        {
                
                result_peg_293.reset();
                do{
                    Result result_peg_310(result_peg_293.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_310.getPosition()))){
                                result_peg_310.nextPosition();
                            } else {
                                goto out_peg_313;
                            }
                        }
                        result_peg_310.setValue((void*) " ");
                            
                    }
                    goto success_peg_311;
                    out_peg_313:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_310.getPosition()))){
                                result_peg_310.nextPosition();
                            } else {
                                goto out_peg_315;
                            }
                        }
                        result_peg_310.setValue((void*) "\t");
                            
                    }
                    goto success_peg_311;
                    out_peg_315:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_310.getPosition()) == (unsigned char) 255){
                            result_peg_310.nextPosition();
                        } else {
                            goto out_peg_317;
                        }
                        result_peg_310.setValue((void*) 255);
                        
                    }
                    goto success_peg_311;
                    out_peg_317:
                    goto loop_peg_309;
                    success_peg_311:
                    ;
                    result_peg_293.addResult(result_peg_310);
                } while (true);
                loop_peg_309:
                ;
                        
            }
            goto success_peg_298;
            goto out_peg_295;
            success_peg_298:
            ;
        
        
        
        result_peg_293 = rule_integer(stream, result_peg_293.getPosition());
            if (result_peg_293.error()){
                goto out_peg_295;
            }
            num = result_peg_293.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_294.getValues()), num);
                result_peg_293.setValue(value);
            }
        
        
    }
    
    if (column_peg_266.chunk2 == 0){
        column_peg_266.chunk2 = new Chunk2();
    }
    column_peg_266.chunk2->chunk_collision_default = result_peg_293;
    stream.update(result_peg_293.getPosition());
    
    
    return result_peg_293;
    out_peg_295:
    Result result_peg_319(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_319.getPosition()))){
                    result_peg_319.nextPosition();
                } else {
                    goto out_peg_321;
                }
            }
            result_peg_319.setValue((void*) "Clsn2");
        
        Result result_peg_320 = result_peg_319;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_319.getPosition()))){
                    result_peg_319.nextPosition();
                } else {
                    goto out_peg_321;
                }
            }
            result_peg_319.setValue((void*) ":");
        
        
        
        {
                
                result_peg_319.reset();
                do{
                    Result result_peg_336(result_peg_319.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_336.getPosition()))){
                                result_peg_336.nextPosition();
                            } else {
                                goto out_peg_339;
                            }
                        }
                        result_peg_336.setValue((void*) " ");
                            
                    }
                    goto success_peg_337;
                    out_peg_339:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_336.getPosition()))){
                                result_peg_336.nextPosition();
                            } else {
                                goto out_peg_341;
                            }
                        }
                        result_peg_336.setValue((void*) "\t");
                            
                    }
                    goto success_peg_337;
                    out_peg_341:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_336.getPosition()) == (unsigned char) 255){
                            result_peg_336.nextPosition();
                        } else {
                            goto out_peg_343;
                        }
                        result_peg_336.setValue((void*) 255);
                        
                    }
                    goto success_peg_337;
                    out_peg_343:
                    goto loop_peg_335;
                    success_peg_337:
                    ;
                    result_peg_319.addResult(result_peg_336);
                } while (true);
                loop_peg_335:
                ;
                        
            }
            goto success_peg_324;
            goto out_peg_321;
            success_peg_324:
            ;
        
        
        
        result_peg_319 = rule_integer(stream, result_peg_319.getPosition());
            if (result_peg_319.error()){
                goto out_peg_321;
            }
            num = result_peg_319.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_320.getValues()), num);
                result_peg_319.setValue(value);
            }
        
        
    }
    
    if (column_peg_266.chunk2 == 0){
        column_peg_266.chunk2 = new Chunk2();
    }
    column_peg_266.chunk2->chunk_collision_default = result_peg_319;
    stream.update(result_peg_319.getPosition());
    
    
    return result_peg_319;
    out_peg_321:
    Result result_peg_345(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_345.getPosition()))){
                    result_peg_345.nextPosition();
                } else {
                    goto out_peg_347;
                }
            }
            result_peg_345.setValue((void*) "Clsn1");
        
        Result result_peg_346 = result_peg_345;
        
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_345.getPosition()))){
                    result_peg_345.nextPosition();
                } else {
                    goto out_peg_347;
                }
            }
            result_peg_345.setValue((void*) ":");
        
        
        
        {
                
                result_peg_345.reset();
                do{
                    Result result_peg_362(result_peg_345.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_362.getPosition()))){
                                result_peg_362.nextPosition();
                            } else {
                                goto out_peg_365;
                            }
                        }
                        result_peg_362.setValue((void*) " ");
                            
                    }
                    goto success_peg_363;
                    out_peg_365:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_362.getPosition()))){
                                result_peg_362.nextPosition();
                            } else {
                                goto out_peg_367;
                            }
                        }
                        result_peg_362.setValue((void*) "\t");
                            
                    }
                    goto success_peg_363;
                    out_peg_367:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_362.getPosition()) == (unsigned char) 255){
                            result_peg_362.nextPosition();
                        } else {
                            goto out_peg_369;
                        }
                        result_peg_362.setValue((void*) 255);
                        
                    }
                    goto success_peg_363;
                    out_peg_369:
                    goto loop_peg_361;
                    success_peg_363:
                    ;
                    result_peg_345.addResult(result_peg_362);
                } while (true);
                loop_peg_361:
                ;
                        
            }
            goto success_peg_350;
            goto out_peg_347;
            success_peg_350:
            ;
        
        
        
        result_peg_345 = rule_integer(stream, result_peg_345.getPosition());
            if (result_peg_345.error()){
                goto out_peg_347;
            }
            num = result_peg_345.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeKeyword(makeKeyword(result_peg_346.getValues()), num);
                result_peg_345.setValue(value);
            }
        
        
    }
    
    if (column_peg_266.chunk2 == 0){
        column_peg_266.chunk2 = new Chunk2();
    }
    column_peg_266.chunk2->chunk_collision_default = result_peg_345;
    stream.update(result_peg_345.getPosition());
    
    
    return result_peg_345;
    out_peg_347:
    
    if (column_peg_266.chunk2 == 0){
        column_peg_266.chunk2 = new Chunk2();
    }
    column_peg_266.chunk2->chunk_collision_default = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_372 = stream.getColumn(position);
    if (column_peg_372.chunk2 != 0 && column_peg_372.chunk2->chunk_collision.calculated()){
        return column_peg_372.chunk2->chunk_collision;
    }
    
    RuleTrace trace_peg_863(stream, "collision");
    int myposition = position;
    
    Value name;
    Value index;
    Value n1;
    Value n2;
    Value n3;
    Value n4;
    Result result_peg_373(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn2"[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) "Clsn2");
            name = result_peg_373.getValues();
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_389(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_389.getPosition()))){
                                result_peg_389.nextPosition();
                            } else {
                                goto out_peg_392;
                            }
                        }
                        result_peg_389.setValue((void*) " ");
                            
                    }
                    goto success_peg_390;
                    out_peg_392:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_389.getPosition()))){
                                result_peg_389.nextPosition();
                            } else {
                                goto out_peg_394;
                            }
                        }
                        result_peg_389.setValue((void*) "\t");
                            
                    }
                    goto success_peg_390;
                    out_peg_394:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_389.getPosition()) == (unsigned char) 255){
                            result_peg_389.nextPosition();
                        } else {
                            goto out_peg_396;
                        }
                        result_peg_389.setValue((void*) 255);
                        
                    }
                    goto success_peg_390;
                    out_peg_396:
                    goto loop_peg_388;
                    success_peg_390:
                    ;
                    result_peg_373.addResult(result_peg_389);
                } while (true);
                loop_peg_388:
                ;
                        
            }
            goto success_peg_377;
            goto out_peg_375;
            success_peg_377:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) "[");
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_411(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_411.getPosition()))){
                                result_peg_411.nextPosition();
                            } else {
                                goto out_peg_414;
                            }
                        }
                        result_peg_411.setValue((void*) " ");
                            
                    }
                    goto success_peg_412;
                    out_peg_414:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_411.getPosition()))){
                                result_peg_411.nextPosition();
                            } else {
                                goto out_peg_416;
                            }
                        }
                        result_peg_411.setValue((void*) "\t");
                            
                    }
                    goto success_peg_412;
                    out_peg_416:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_411.getPosition()) == (unsigned char) 255){
                            result_peg_411.nextPosition();
                        } else {
                            goto out_peg_418;
                        }
                        result_peg_411.setValue((void*) 255);
                        
                    }
                    goto success_peg_412;
                    out_peg_418:
                    goto loop_peg_410;
                    success_peg_412:
                    ;
                    result_peg_373.addResult(result_peg_411);
                } while (true);
                loop_peg_410:
                ;
                        
            }
            goto success_peg_399;
            goto out_peg_375;
            success_peg_399:
            ;
        
        
        
        result_peg_373 = rule_integer(stream, result_peg_373.getPosition());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
            index = result_peg_373.getValues();
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_433(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_433.getPosition()))){
                                result_peg_433.nextPosition();
                            } else {
                                goto out_peg_436;
                            }
                        }
                        result_peg_433.setValue((void*) " ");
                            
                    }
                    goto success_peg_434;
                    out_peg_436:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_433.getPosition()))){
                                result_peg_433.nextPosition();
                            } else {
                                goto out_peg_438;
                            }
                        }
                        result_peg_433.setValue((void*) "\t");
                            
                    }
                    goto success_peg_434;
                    out_peg_438:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_433.getPosition()) == (unsigned char) 255){
                            result_peg_433.nextPosition();
                        } else {
                            goto out_peg_440;
                        }
                        result_peg_433.setValue((void*) 255);
                        
                    }
                    goto success_peg_434;
                    out_peg_440:
                    goto loop_peg_432;
                    success_peg_434:
                    ;
                    result_peg_373.addResult(result_peg_433);
                } while (true);
                loop_peg_432:
                ;
                        
            }
            goto success_peg_421;
            goto out_peg_375;
            success_peg_421:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) "]");
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_455(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_455.getPosition()))){
                                result_peg_455.nextPosition();
                            } else {
                                goto out_peg_458;
                            }
                        }
                        result_peg_455.setValue((void*) " ");
                            
                    }
                    goto success_peg_456;
                    out_peg_458:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_455.getPosition()))){
                                result_peg_455.nextPosition();
                            } else {
                                goto out_peg_460;
                            }
                        }
                        result_peg_455.setValue((void*) "\t");
                            
                    }
                    goto success_peg_456;
                    out_peg_460:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_455.getPosition()) == (unsigned char) 255){
                            result_peg_455.nextPosition();
                        } else {
                            goto out_peg_462;
                        }
                        result_peg_455.setValue((void*) 255);
                        
                    }
                    goto success_peg_456;
                    out_peg_462:
                    goto loop_peg_454;
                    success_peg_456:
                    ;
                    result_peg_373.addResult(result_peg_455);
                } while (true);
                loop_peg_454:
                ;
                        
            }
            goto success_peg_443;
            goto out_peg_375;
            success_peg_443:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) "=");
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_477(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_477.getPosition()))){
                                result_peg_477.nextPosition();
                            } else {
                                goto out_peg_480;
                            }
                        }
                        result_peg_477.setValue((void*) " ");
                            
                    }
                    goto success_peg_478;
                    out_peg_480:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_477.getPosition()))){
                                result_peg_477.nextPosition();
                            } else {
                                goto out_peg_482;
                            }
                        }
                        result_peg_477.setValue((void*) "\t");
                            
                    }
                    goto success_peg_478;
                    out_peg_482:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_477.getPosition()) == (unsigned char) 255){
                            result_peg_477.nextPosition();
                        } else {
                            goto out_peg_484;
                        }
                        result_peg_477.setValue((void*) 255);
                        
                    }
                    goto success_peg_478;
                    out_peg_484:
                    goto loop_peg_476;
                    success_peg_478:
                    ;
                    result_peg_373.addResult(result_peg_477);
                } while (true);
                loop_peg_476:
                ;
                        
            }
            goto success_peg_465;
            goto out_peg_375;
            success_peg_465:
            ;
        
        
        
        result_peg_373 = rule_integer(stream, result_peg_373.getPosition());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
            n1 = result_peg_373.getValues();
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_499(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_499.getPosition()))){
                                result_peg_499.nextPosition();
                            } else {
                                goto out_peg_502;
                            }
                        }
                        result_peg_499.setValue((void*) " ");
                            
                    }
                    goto success_peg_500;
                    out_peg_502:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_499.getPosition()))){
                                result_peg_499.nextPosition();
                            } else {
                                goto out_peg_504;
                            }
                        }
                        result_peg_499.setValue((void*) "\t");
                            
                    }
                    goto success_peg_500;
                    out_peg_504:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_499.getPosition()) == (unsigned char) 255){
                            result_peg_499.nextPosition();
                        } else {
                            goto out_peg_506;
                        }
                        result_peg_499.setValue((void*) 255);
                        
                    }
                    goto success_peg_500;
                    out_peg_506:
                    goto loop_peg_498;
                    success_peg_500:
                    ;
                    result_peg_373.addResult(result_peg_499);
                } while (true);
                loop_peg_498:
                ;
                        
            }
            goto success_peg_487;
            goto out_peg_375;
            success_peg_487:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) ",");
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_521(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_521.getPosition()))){
                                result_peg_521.nextPosition();
                            } else {
                                goto out_peg_524;
                            }
                        }
                        result_peg_521.setValue((void*) " ");
                            
                    }
                    goto success_peg_522;
                    out_peg_524:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_521.getPosition()))){
                                result_peg_521.nextPosition();
                            } else {
                                goto out_peg_526;
                            }
                        }
                        result_peg_521.setValue((void*) "\t");
                            
                    }
                    goto success_peg_522;
                    out_peg_526:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_521.getPosition()) == (unsigned char) 255){
                            result_peg_521.nextPosition();
                        } else {
                            goto out_peg_528;
                        }
                        result_peg_521.setValue((void*) 255);
                        
                    }
                    goto success_peg_522;
                    out_peg_528:
                    goto loop_peg_520;
                    success_peg_522:
                    ;
                    result_peg_373.addResult(result_peg_521);
                } while (true);
                loop_peg_520:
                ;
                        
            }
            goto success_peg_509;
            goto out_peg_375;
            success_peg_509:
            ;
        
        
        
        result_peg_373 = rule_integer(stream, result_peg_373.getPosition());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
            n2 = result_peg_373.getValues();
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_543(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_543.getPosition()))){
                                result_peg_543.nextPosition();
                            } else {
                                goto out_peg_546;
                            }
                        }
                        result_peg_543.setValue((void*) " ");
                            
                    }
                    goto success_peg_544;
                    out_peg_546:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_543.getPosition()))){
                                result_peg_543.nextPosition();
                            } else {
                                goto out_peg_548;
                            }
                        }
                        result_peg_543.setValue((void*) "\t");
                            
                    }
                    goto success_peg_544;
                    out_peg_548:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_543.getPosition()) == (unsigned char) 255){
                            result_peg_543.nextPosition();
                        } else {
                            goto out_peg_550;
                        }
                        result_peg_543.setValue((void*) 255);
                        
                    }
                    goto success_peg_544;
                    out_peg_550:
                    goto loop_peg_542;
                    success_peg_544:
                    ;
                    result_peg_373.addResult(result_peg_543);
                } while (true);
                loop_peg_542:
                ;
                        
            }
            goto success_peg_531;
            goto out_peg_375;
            success_peg_531:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) ",");
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_565(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_565.getPosition()))){
                                result_peg_565.nextPosition();
                            } else {
                                goto out_peg_568;
                            }
                        }
                        result_peg_565.setValue((void*) " ");
                            
                    }
                    goto success_peg_566;
                    out_peg_568:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_565.getPosition()))){
                                result_peg_565.nextPosition();
                            } else {
                                goto out_peg_570;
                            }
                        }
                        result_peg_565.setValue((void*) "\t");
                            
                    }
                    goto success_peg_566;
                    out_peg_570:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_565.getPosition()) == (unsigned char) 255){
                            result_peg_565.nextPosition();
                        } else {
                            goto out_peg_572;
                        }
                        result_peg_565.setValue((void*) 255);
                        
                    }
                    goto success_peg_566;
                    out_peg_572:
                    goto loop_peg_564;
                    success_peg_566:
                    ;
                    result_peg_373.addResult(result_peg_565);
                } while (true);
                loop_peg_564:
                ;
                        
            }
            goto success_peg_553;
            goto out_peg_375;
            success_peg_553:
            ;
        
        
        
        result_peg_373 = rule_integer(stream, result_peg_373.getPosition());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
            n3 = result_peg_373.getValues();
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_587(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_587.getPosition()))){
                                result_peg_587.nextPosition();
                            } else {
                                goto out_peg_590;
                            }
                        }
                        result_peg_587.setValue((void*) " ");
                            
                    }
                    goto success_peg_588;
                    out_peg_590:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_587.getPosition()))){
                                result_peg_587.nextPosition();
                            } else {
                                goto out_peg_592;
                            }
                        }
                        result_peg_587.setValue((void*) "\t");
                            
                    }
                    goto success_peg_588;
                    out_peg_592:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_587.getPosition()) == (unsigned char) 255){
                            result_peg_587.nextPosition();
                        } else {
                            goto out_peg_594;
                        }
                        result_peg_587.setValue((void*) 255);
                        
                    }
                    goto success_peg_588;
                    out_peg_594:
                    goto loop_peg_586;
                    success_peg_588:
                    ;
                    result_peg_373.addResult(result_peg_587);
                } while (true);
                loop_peg_586:
                ;
                        
            }
            goto success_peg_575;
            goto out_peg_375;
            success_peg_575:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_373.getPosition()))){
                    result_peg_373.nextPosition();
                } else {
                    goto out_peg_375;
                }
            }
            result_peg_373.setValue((void*) ",");
        
        
        
        {
                
                result_peg_373.reset();
                do{
                    Result result_peg_609(result_peg_373.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_609.getPosition()))){
                                result_peg_609.nextPosition();
                            } else {
                                goto out_peg_612;
                            }
                        }
                        result_peg_609.setValue((void*) " ");
                            
                    }
                    goto success_peg_610;
                    out_peg_612:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_609.getPosition()))){
                                result_peg_609.nextPosition();
                            } else {
                                goto out_peg_614;
                            }
                        }
                        result_peg_609.setValue((void*) "\t");
                            
                    }
                    goto success_peg_610;
                    out_peg_614:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_609.getPosition()) == (unsigned char) 255){
                            result_peg_609.nextPosition();
                        } else {
                            goto out_peg_616;
                        }
                        result_peg_609.setValue((void*) 255);
                        
                    }
                    goto success_peg_610;
                    out_peg_616:
                    goto loop_peg_608;
                    success_peg_610:
                    ;
                    result_peg_373.addResult(result_peg_609);
                } while (true);
                loop_peg_608:
                ;
                        
            }
            goto success_peg_597;
            goto out_peg_375;
            success_peg_597:
            ;
        
        
        
        result_peg_373 = rule_integer(stream, result_peg_373.getPosition());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
            n4 = result_peg_373.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_373.setValue(value);
            }
        
        
    }
    
    if (column_peg_372.chunk2 == 0){
        column_peg_372.chunk2 = new Chunk2();
    }
    column_peg_372.chunk2->chunk_collision = result_peg_373;
    stream.update(result_peg_373.getPosition());
    
    
    return result_peg_373;
    out_peg_375:
    Result result_peg_618(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Clsn1"[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) "Clsn1");
            name = result_peg_618.getValues();
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_634(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_634.getPosition()))){
                                result_peg_634.nextPosition();
                            } else {
                                goto out_peg_637;
                            }
                        }
                        result_peg_634.setValue((void*) " ");
                            
                    }
                    goto success_peg_635;
                    out_peg_637:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_634.getPosition()))){
                                result_peg_634.nextPosition();
                            } else {
                                goto out_peg_639;
                            }
                        }
                        result_peg_634.setValue((void*) "\t");
                            
                    }
                    goto success_peg_635;
                    out_peg_639:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_634.getPosition()) == (unsigned char) 255){
                            result_peg_634.nextPosition();
                        } else {
                            goto out_peg_641;
                        }
                        result_peg_634.setValue((void*) 255);
                        
                    }
                    goto success_peg_635;
                    out_peg_641:
                    goto loop_peg_633;
                    success_peg_635:
                    ;
                    result_peg_618.addResult(result_peg_634);
                } while (true);
                loop_peg_633:
                ;
                        
            }
            goto success_peg_622;
            goto out_peg_620;
            success_peg_622:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) "[");
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_656(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_656.getPosition()))){
                                result_peg_656.nextPosition();
                            } else {
                                goto out_peg_659;
                            }
                        }
                        result_peg_656.setValue((void*) " ");
                            
                    }
                    goto success_peg_657;
                    out_peg_659:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_656.getPosition()))){
                                result_peg_656.nextPosition();
                            } else {
                                goto out_peg_661;
                            }
                        }
                        result_peg_656.setValue((void*) "\t");
                            
                    }
                    goto success_peg_657;
                    out_peg_661:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_656.getPosition()) == (unsigned char) 255){
                            result_peg_656.nextPosition();
                        } else {
                            goto out_peg_663;
                        }
                        result_peg_656.setValue((void*) 255);
                        
                    }
                    goto success_peg_657;
                    out_peg_663:
                    goto loop_peg_655;
                    success_peg_657:
                    ;
                    result_peg_618.addResult(result_peg_656);
                } while (true);
                loop_peg_655:
                ;
                        
            }
            goto success_peg_644;
            goto out_peg_620;
            success_peg_644:
            ;
        
        
        
        result_peg_618 = rule_integer(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
            index = result_peg_618.getValues();
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_678(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_678.getPosition()))){
                                result_peg_678.nextPosition();
                            } else {
                                goto out_peg_681;
                            }
                        }
                        result_peg_678.setValue((void*) " ");
                            
                    }
                    goto success_peg_679;
                    out_peg_681:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_678.getPosition()))){
                                result_peg_678.nextPosition();
                            } else {
                                goto out_peg_683;
                            }
                        }
                        result_peg_678.setValue((void*) "\t");
                            
                    }
                    goto success_peg_679;
                    out_peg_683:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_678.getPosition()) == (unsigned char) 255){
                            result_peg_678.nextPosition();
                        } else {
                            goto out_peg_685;
                        }
                        result_peg_678.setValue((void*) 255);
                        
                    }
                    goto success_peg_679;
                    out_peg_685:
                    goto loop_peg_677;
                    success_peg_679:
                    ;
                    result_peg_618.addResult(result_peg_678);
                } while (true);
                loop_peg_677:
                ;
                        
            }
            goto success_peg_666;
            goto out_peg_620;
            success_peg_666:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) "]");
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_700(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_700.getPosition()))){
                                result_peg_700.nextPosition();
                            } else {
                                goto out_peg_703;
                            }
                        }
                        result_peg_700.setValue((void*) " ");
                            
                    }
                    goto success_peg_701;
                    out_peg_703:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_700.getPosition()))){
                                result_peg_700.nextPosition();
                            } else {
                                goto out_peg_705;
                            }
                        }
                        result_peg_700.setValue((void*) "\t");
                            
                    }
                    goto success_peg_701;
                    out_peg_705:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_700.getPosition()) == (unsigned char) 255){
                            result_peg_700.nextPosition();
                        } else {
                            goto out_peg_707;
                        }
                        result_peg_700.setValue((void*) 255);
                        
                    }
                    goto success_peg_701;
                    out_peg_707:
                    goto loop_peg_699;
                    success_peg_701:
                    ;
                    result_peg_618.addResult(result_peg_700);
                } while (true);
                loop_peg_699:
                ;
                        
            }
            goto success_peg_688;
            goto out_peg_620;
            success_peg_688:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) "=");
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_722(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_722.getPosition()))){
                                result_peg_722.nextPosition();
                            } else {
                                goto out_peg_725;
                            }
                        }
                        result_peg_722.setValue((void*) " ");
                            
                    }
                    goto success_peg_723;
                    out_peg_725:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_722.getPosition()))){
                                result_peg_722.nextPosition();
                            } else {
                                goto out_peg_727;
                            }
                        }
                        result_peg_722.setValue((void*) "\t");
                            
                    }
                    goto success_peg_723;
                    out_peg_727:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_722.getPosition()) == (unsigned char) 255){
                            result_peg_722.nextPosition();
                        } else {
                            goto out_peg_729;
                        }
                        result_peg_722.setValue((void*) 255);
                        
                    }
                    goto success_peg_723;
                    out_peg_729:
                    goto loop_peg_721;
                    success_peg_723:
                    ;
                    result_peg_618.addResult(result_peg_722);
                } while (true);
                loop_peg_721:
                ;
                        
            }
            goto success_peg_710;
            goto out_peg_620;
            success_peg_710:
            ;
        
        
        
        result_peg_618 = rule_integer(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
            n1 = result_peg_618.getValues();
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_744(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_744.getPosition()))){
                                result_peg_744.nextPosition();
                            } else {
                                goto out_peg_747;
                            }
                        }
                        result_peg_744.setValue((void*) " ");
                            
                    }
                    goto success_peg_745;
                    out_peg_747:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_744.getPosition()))){
                                result_peg_744.nextPosition();
                            } else {
                                goto out_peg_749;
                            }
                        }
                        result_peg_744.setValue((void*) "\t");
                            
                    }
                    goto success_peg_745;
                    out_peg_749:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_744.getPosition()) == (unsigned char) 255){
                            result_peg_744.nextPosition();
                        } else {
                            goto out_peg_751;
                        }
                        result_peg_744.setValue((void*) 255);
                        
                    }
                    goto success_peg_745;
                    out_peg_751:
                    goto loop_peg_743;
                    success_peg_745:
                    ;
                    result_peg_618.addResult(result_peg_744);
                } while (true);
                loop_peg_743:
                ;
                        
            }
            goto success_peg_732;
            goto out_peg_620;
            success_peg_732:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) ",");
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_766(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_766.getPosition()))){
                                result_peg_766.nextPosition();
                            } else {
                                goto out_peg_769;
                            }
                        }
                        result_peg_766.setValue((void*) " ");
                            
                    }
                    goto success_peg_767;
                    out_peg_769:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_766.getPosition()))){
                                result_peg_766.nextPosition();
                            } else {
                                goto out_peg_771;
                            }
                        }
                        result_peg_766.setValue((void*) "\t");
                            
                    }
                    goto success_peg_767;
                    out_peg_771:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_766.getPosition()) == (unsigned char) 255){
                            result_peg_766.nextPosition();
                        } else {
                            goto out_peg_773;
                        }
                        result_peg_766.setValue((void*) 255);
                        
                    }
                    goto success_peg_767;
                    out_peg_773:
                    goto loop_peg_765;
                    success_peg_767:
                    ;
                    result_peg_618.addResult(result_peg_766);
                } while (true);
                loop_peg_765:
                ;
                        
            }
            goto success_peg_754;
            goto out_peg_620;
            success_peg_754:
            ;
        
        
        
        result_peg_618 = rule_integer(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
            n2 = result_peg_618.getValues();
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_788(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_788.getPosition()))){
                                result_peg_788.nextPosition();
                            } else {
                                goto out_peg_791;
                            }
                        }
                        result_peg_788.setValue((void*) " ");
                            
                    }
                    goto success_peg_789;
                    out_peg_791:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_788.getPosition()))){
                                result_peg_788.nextPosition();
                            } else {
                                goto out_peg_793;
                            }
                        }
                        result_peg_788.setValue((void*) "\t");
                            
                    }
                    goto success_peg_789;
                    out_peg_793:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_788.getPosition()) == (unsigned char) 255){
                            result_peg_788.nextPosition();
                        } else {
                            goto out_peg_795;
                        }
                        result_peg_788.setValue((void*) 255);
                        
                    }
                    goto success_peg_789;
                    out_peg_795:
                    goto loop_peg_787;
                    success_peg_789:
                    ;
                    result_peg_618.addResult(result_peg_788);
                } while (true);
                loop_peg_787:
                ;
                        
            }
            goto success_peg_776;
            goto out_peg_620;
            success_peg_776:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) ",");
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_810(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_810.getPosition()))){
                                result_peg_810.nextPosition();
                            } else {
                                goto out_peg_813;
                            }
                        }
                        result_peg_810.setValue((void*) " ");
                            
                    }
                    goto success_peg_811;
                    out_peg_813:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_810.getPosition()))){
                                result_peg_810.nextPosition();
                            } else {
                                goto out_peg_815;
                            }
                        }
                        result_peg_810.setValue((void*) "\t");
                            
                    }
                    goto success_peg_811;
                    out_peg_815:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_810.getPosition()) == (unsigned char) 255){
                            result_peg_810.nextPosition();
                        } else {
                            goto out_peg_817;
                        }
                        result_peg_810.setValue((void*) 255);
                        
                    }
                    goto success_peg_811;
                    out_peg_817:
                    goto loop_peg_809;
                    success_peg_811:
                    ;
                    result_peg_618.addResult(result_peg_810);
                } while (true);
                loop_peg_809:
                ;
                        
            }
            goto success_peg_798;
            goto out_peg_620;
            success_peg_798:
            ;
        
        
        
        result_peg_618 = rule_integer(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
            n3 = result_peg_618.getValues();
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_832(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_832.getPosition()))){
                                result_peg_832.nextPosition();
                            } else {
                                goto out_peg_835;
                            }
                        }
                        result_peg_832.setValue((void*) " ");
                            
                    }
                    goto success_peg_833;
                    out_peg_835:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_832.getPosition()))){
                                result_peg_832.nextPosition();
                            } else {
                                goto out_peg_837;
                            }
                        }
                        result_peg_832.setValue((void*) "\t");
                            
                    }
                    goto success_peg_833;
                    out_peg_837:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_832.getPosition()) == (unsigned char) 255){
                            result_peg_832.nextPosition();
                        } else {
                            goto out_peg_839;
                        }
                        result_peg_832.setValue((void*) 255);
                        
                    }
                    goto success_peg_833;
                    out_peg_839:
                    goto loop_peg_831;
                    success_peg_833:
                    ;
                    result_peg_618.addResult(result_peg_832);
                } while (true);
                loop_peg_831:
                ;
                        
            }
            goto success_peg_820;
            goto out_peg_620;
            success_peg_820:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) ",");
        
        
        
        {
                
                result_peg_618.reset();
                do{
                    Result result_peg_854(result_peg_618.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_854.getPosition()))){
                                result_peg_854.nextPosition();
                            } else {
                                goto out_peg_857;
                            }
                        }
                        result_peg_854.setValue((void*) " ");
                            
                    }
                    goto success_peg_855;
                    out_peg_857:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_854.getPosition()))){
                                result_peg_854.nextPosition();
                            } else {
                                goto out_peg_859;
                            }
                        }
                        result_peg_854.setValue((void*) "\t");
                            
                    }
                    goto success_peg_855;
                    out_peg_859:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_854.getPosition()) == (unsigned char) 255){
                            result_peg_854.nextPosition();
                        } else {
                            goto out_peg_861;
                        }
                        result_peg_854.setValue((void*) 255);
                        
                    }
                    goto success_peg_855;
                    out_peg_861:
                    goto loop_peg_853;
                    success_peg_855:
                    ;
                    result_peg_618.addResult(result_peg_854);
                } while (true);
                loop_peg_853:
                ;
                        
            }
            goto success_peg_842;
            goto out_peg_620;
            success_peg_842:
            ;
        
        
        
        result_peg_618 = rule_integer(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
            n4 = result_peg_618.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                result_peg_618.setValue(value);
            }
        
        
    }
    
    if (column_peg_372.chunk2 == 0){
        column_peg_372.chunk2 = new Chunk2();
    }
    column_peg_372.chunk2->chunk_collision = result_peg_618;
    stream.update(result_peg_618.getPosition());
    
    
    return result_peg_618;
    out_peg_620:
    
    if (column_peg_372.chunk2 == 0){
        column_peg_372.chunk2 = new Chunk2();
    }
    column_peg_372.chunk2->chunk_collision = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_864 = stream.getColumn(position);
    if (column_peg_864.chunk2 != 0 && column_peg_864.chunk2->chunk_action_start.calculated()){
        return column_peg_864.chunk2->chunk_action_start;
    }
    
    RuleTrace trace_peg_956(stream, "action_start");
    int myposition = position;
    
    Value begin;
    Value action;
    Value num;
    Result result_peg_865(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_865.getPosition()))){
                    result_peg_865.nextPosition();
                } else {
                    goto out_peg_867;
                }
            }
            result_peg_865.setValue((void*) "[");
        
        
        
        {
                
                result_peg_865.reset();
                do{
                    Result result_peg_881(result_peg_865.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_881.getPosition()))){
                                result_peg_881.nextPosition();
                            } else {
                                goto out_peg_884;
                            }
                        }
                        result_peg_881.setValue((void*) " ");
                            
                    }
                    goto success_peg_882;
                    out_peg_884:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_881.getPosition()))){
                                result_peg_881.nextPosition();
                            } else {
                                goto out_peg_886;
                            }
                        }
                        result_peg_881.setValue((void*) "\t");
                            
                    }
                    goto success_peg_882;
                    out_peg_886:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_881.getPosition()) == (unsigned char) 255){
                            result_peg_881.nextPosition();
                        } else {
                            goto out_peg_888;
                        }
                        result_peg_881.setValue((void*) 255);
                        
                    }
                    goto success_peg_882;
                    out_peg_888:
                    goto loop_peg_880;
                    success_peg_882:
                    ;
                    result_peg_865.addResult(result_peg_881);
                } while (true);
                loop_peg_880:
                ;
                        
            }
            goto success_peg_869;
            goto out_peg_867;
            success_peg_869:
            ;
        
        
        
        for (int i = 0; i < 5; i++){
                if (compareCharCase("Begin"[i], stream.get(result_peg_865.getPosition()))){
                    result_peg_865.nextPosition();
                } else {
                    goto out_peg_867;
                }
            }
            result_peg_865.setValue((void*) "Begin");
            begin = result_peg_865.getValues();
        
        
        
        {
                
                result_peg_865.reset();
                do{
                    Result result_peg_903(result_peg_865.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_903.getPosition()))){
                                result_peg_903.nextPosition();
                            } else {
                                goto out_peg_906;
                            }
                        }
                        result_peg_903.setValue((void*) " ");
                            
                    }
                    goto success_peg_904;
                    out_peg_906:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_903.getPosition()))){
                                result_peg_903.nextPosition();
                            } else {
                                goto out_peg_908;
                            }
                        }
                        result_peg_903.setValue((void*) "\t");
                            
                    }
                    goto success_peg_904;
                    out_peg_908:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_903.getPosition()) == (unsigned char) 255){
                            result_peg_903.nextPosition();
                        } else {
                            goto out_peg_910;
                        }
                        result_peg_903.setValue((void*) 255);
                        
                    }
                    goto success_peg_904;
                    out_peg_910:
                    goto loop_peg_902;
                    success_peg_904:
                    ;
                    result_peg_865.addResult(result_peg_903);
                } while (true);
                loop_peg_902:
                ;
                        
            }
            goto success_peg_891;
            goto out_peg_867;
            success_peg_891:
            ;
        
        
        
        for (int i = 0; i < 6; i++){
                if (compareCharCase("Action"[i], stream.get(result_peg_865.getPosition()))){
                    result_peg_865.nextPosition();
                } else {
                    goto out_peg_867;
                }
            }
            result_peg_865.setValue((void*) "Action");
            action = result_peg_865.getValues();
        
        
        
        {
                
                result_peg_865.reset();
                do{
                    Result result_peg_925(result_peg_865.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_925.getPosition()))){
                                result_peg_925.nextPosition();
                            } else {
                                goto out_peg_928;
                            }
                        }
                        result_peg_925.setValue((void*) " ");
                            
                    }
                    goto success_peg_926;
                    out_peg_928:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_925.getPosition()))){
                                result_peg_925.nextPosition();
                            } else {
                                goto out_peg_930;
                            }
                        }
                        result_peg_925.setValue((void*) "\t");
                            
                    }
                    goto success_peg_926;
                    out_peg_930:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_925.getPosition()) == (unsigned char) 255){
                            result_peg_925.nextPosition();
                        } else {
                            goto out_peg_932;
                        }
                        result_peg_925.setValue((void*) 255);
                        
                    }
                    goto success_peg_926;
                    out_peg_932:
                    goto loop_peg_924;
                    success_peg_926:
                    ;
                    result_peg_865.addResult(result_peg_925);
                } while (true);
                loop_peg_924:
                ;
                        
            }
            goto success_peg_913;
            goto out_peg_867;
            success_peg_913:
            ;
        
        
        
        result_peg_865 = rule_integer(stream, result_peg_865.getPosition());
            if (result_peg_865.error()){
                goto out_peg_867;
            }
            num = result_peg_865.getValues();
        
        
        
        {
                
                result_peg_865.reset();
                do{
                    Result result_peg_947(result_peg_865.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_947.getPosition()))){
                                result_peg_947.nextPosition();
                            } else {
                                goto out_peg_950;
                            }
                        }
                        result_peg_947.setValue((void*) " ");
                            
                    }
                    goto success_peg_948;
                    out_peg_950:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_947.getPosition()))){
                                result_peg_947.nextPosition();
                            } else {
                                goto out_peg_952;
                            }
                        }
                        result_peg_947.setValue((void*) "\t");
                            
                    }
                    goto success_peg_948;
                    out_peg_952:
                    {
                        
                        if ((unsigned char) stream.get(result_peg_947.getPosition()) == (unsigned char) 255){
                            result_peg_947.nextPosition();
                        } else {
                            goto out_peg_954;
                        }
                        result_peg_947.setValue((void*) 255);
                        
                    }
                    goto success_peg_948;
                    out_peg_954:
                    goto loop_peg_946;
                    success_peg_948:
                    ;
                    result_peg_865.addResult(result_peg_947);
                } while (true);
                loop_peg_946:
                ;
                        
            }
            goto success_peg_935;
            goto out_peg_867;
            success_peg_935:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_865.getPosition()))){
                    result_peg_865.nextPosition();
                } else {
                    goto out_peg_867;
                }
            }
            result_peg_865.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeHeader(begin, action, num);
                result_peg_865.setValue(value);
            }
        
        
    }
    
    if (column_peg_864.chunk2 == 0){
        column_peg_864.chunk2 = new Chunk2();
    }
    column_peg_864.chunk2->chunk_action_start = result_peg_865;
    stream.update(result_peg_865.getPosition());
    
    
    return result_peg_865;
    out_peg_867:
    
    if (column_peg_864.chunk2 == 0){
        column_peg_864.chunk2 = new Chunk2();
    }
    column_peg_864.chunk2->chunk_action_start = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_957 = stream.getColumn(position);
    if (column_peg_957.chunk2 != 0 && column_peg_957.chunk2->chunk_integer.calculated()){
        return column_peg_957.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_975(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_958(myposition);
    
    {
    
        int save_peg_960 = result_peg_958.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_958.getPosition()))){
                        result_peg_958.nextPosition();
                    } else {
                        goto out_peg_963;
                    }
                }
                result_peg_958.setValue((void*) "-");
                    
            }
            goto success_peg_961;
            out_peg_963:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_958.getPosition()))){
                        result_peg_958.nextPosition();
                    } else {
                        goto out_peg_965;
                    }
                }
                result_peg_958.setValue((void*) "+");
                    
            }
            goto success_peg_961;
            out_peg_965:
            
            result_peg_958 = Result(save_peg_960);
            result_peg_958.setValue((void*) 0);
            
            success_peg_961:
            ;
        
        Result result_peg_959 = result_peg_958;
        
        result_peg_958.reset();
            do{
                Result result_peg_968(result_peg_958.getPosition());
                {
                    
                    char letter_peg_973 = stream.get(result_peg_968.getPosition());
                    if (letter_peg_973 != '\0' && strchr("0123456789", letter_peg_973) != NULL){
                        result_peg_968.nextPosition();
                        result_peg_968.setValue((void*) (long) letter_peg_973);
                    } else {
                        goto out_peg_972;
                    }
                    
                }
                goto success_peg_969;
                out_peg_972:
                goto loop_peg_967;
                success_peg_969:
                ;
                result_peg_958.addResult(result_peg_968);
            } while (true);
            loop_peg_967:
            if (result_peg_958.matches() == 0){
                goto out_peg_974;
            }
        
        Result result_peg_966 = result_peg_958;
        
        {
                Value value((void*) 0);
                value = parseDouble(result_peg_966.getValues()); value = makeNumber(result_peg_959.getValues(),value);
                result_peg_958.setValue(value);
            }
        
        
    }
    
    if (column_peg_957.chunk2 == 0){
        column_peg_957.chunk2 = new Chunk2();
    }
    column_peg_957.chunk2->chunk_integer = result_peg_958;
    stream.update(result_peg_958.getPosition());
    
    
    return result_peg_958;
    out_peg_974:
    
    if (column_peg_957.chunk2 == 0){
        column_peg_957.chunk2 = new Chunk2();
    }
    column_peg_957.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_976 = stream.getColumn(position);
    if (column_peg_976.chunk2 != 0 && column_peg_976.chunk2->chunk_valuelist.calculated()){
        return column_peg_976.chunk2->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1027(stream, "valuelist");
    int myposition = position;
    
    
    Result result_peg_977(myposition);
    
    {
    
        result_peg_977 = rule_value(stream, result_peg_977.getPosition());
            if (result_peg_977.error()){
                goto out_peg_979;
            }
        
        Result result_peg_978 = result_peg_977;
        
        result_peg_977.reset();
            do{
                Result result_peg_982(result_peg_977.getPosition());
                {
                
                    {
                            
                            result_peg_982.reset();
                            do{
                                Result result_peg_996(result_peg_982.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_996.getPosition()))){
                                            result_peg_996.nextPosition();
                                        } else {
                                            goto out_peg_999;
                                        }
                                    }
                                    result_peg_996.setValue((void*) " ");
                                        
                                }
                                goto success_peg_997;
                                out_peg_999:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_996.getPosition()))){
                                            result_peg_996.nextPosition();
                                        } else {
                                            goto out_peg_1001;
                                        }
                                    }
                                    result_peg_996.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_997;
                                out_peg_1001:
                                {
                                    
                                    if ((unsigned char) stream.get(result_peg_996.getPosition()) == (unsigned char) 255){
                                        result_peg_996.nextPosition();
                                    } else {
                                        goto out_peg_1003;
                                    }
                                    result_peg_996.setValue((void*) 255);
                                    
                                }
                                goto success_peg_997;
                                out_peg_1003:
                                goto loop_peg_995;
                                success_peg_997:
                                ;
                                result_peg_982.addResult(result_peg_996);
                            } while (true);
                            loop_peg_995:
                            ;
                                    
                        }
                        goto success_peg_984;
                        goto loop_peg_981;
                        success_peg_984:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_982.getPosition()))){
                                result_peg_982.nextPosition();
                            } else {
                                goto loop_peg_981;
                            }
                        }
                        result_peg_982.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_982.reset();
                            do{
                                Result result_peg_1018(result_peg_982.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1018.getPosition()))){
                                            result_peg_1018.nextPosition();
                                        } else {
                                            goto out_peg_1021;
                                        }
                                    }
                                    result_peg_1018.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1019;
                                out_peg_1021:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1018.getPosition()))){
                                            result_peg_1018.nextPosition();
                                        } else {
                                            goto out_peg_1023;
                                        }
                                    }
                                    result_peg_1018.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1019;
                                out_peg_1023:
                                {
                                    
                                    if ((unsigned char) stream.get(result_peg_1018.getPosition()) == (unsigned char) 255){
                                        result_peg_1018.nextPosition();
                                    } else {
                                        goto out_peg_1025;
                                    }
                                    result_peg_1018.setValue((void*) 255);
                                    
                                }
                                goto success_peg_1019;
                                out_peg_1025:
                                goto loop_peg_1017;
                                success_peg_1019:
                                ;
                                result_peg_982.addResult(result_peg_1018);
                            } while (true);
                            loop_peg_1017:
                            ;
                                    
                        }
                        goto success_peg_1006;
                        goto loop_peg_981;
                        success_peg_1006:
                        ;
                    
                    
                    
                    int save_peg_1026 = result_peg_982.getPosition();
                        
                        result_peg_982 = rule_value(stream, result_peg_982.getPosition());
                        if (result_peg_982.error()){
                            
                            result_peg_982 = Result(save_peg_1026);
                            result_peg_982.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_977.addResult(result_peg_982);
            } while (true);
            loop_peg_981:
            ;
        
        Result result_peg_980 = result_peg_977;
        
        {
                Value value((void*) 0);
                value = makeValueList(result_peg_978.getValues(),result_peg_980.getValues());
                result_peg_977.setValue(value);
            }
        
        
    }
    
    if (column_peg_976.chunk2 == 0){
        column_peg_976.chunk2 = new Chunk2();
    }
    column_peg_976.chunk2->chunk_valuelist = result_peg_977;
    stream.update(result_peg_977.getPosition());
    
    
    return result_peg_977;
    out_peg_979:
    
    if (column_peg_976.chunk2 == 0){
        column_peg_976.chunk2 = new Chunk2();
    }
    column_peg_976.chunk2->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1028 = stream.getColumn(position);
    if (column_peg_1028.chunk3 != 0 && column_peg_1028.chunk3->chunk_value.calculated()){
        return column_peg_1028.chunk3->chunk_value;
    }
    
    RuleTrace trace_peg_1059(stream, "value");
    int myposition = position;
    
    
    Result result_peg_1029(myposition);
    
    result_peg_1029 = rule_integer(stream, result_peg_1029.getPosition());
    if (result_peg_1029.error()){
        goto out_peg_1030;
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1029;
    stream.update(result_peg_1029.getPosition());
    
    
    return result_peg_1029;
    out_peg_1030:
    Result result_peg_1031(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("as"[i], stream.get(result_peg_1031.getPosition()))){
                    result_peg_1031.nextPosition();
                } else {
                    goto out_peg_1033;
                }
            }
            result_peg_1031.setValue((void*) "as");
        
        Result result_peg_1032 = result_peg_1031;
        
        result_peg_1031 = rule_integer(stream, result_peg_1031.getPosition());
            if (result_peg_1031.error()){
                goto out_peg_1033;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("d"[i], stream.get(result_peg_1031.getPosition()))){
                    result_peg_1031.nextPosition();
                } else {
                    goto out_peg_1033;
                }
            }
            result_peg_1031.setValue((void*) "d");
        
        
        
        result_peg_1031 = rule_integer(stream, result_peg_1031.getPosition());
            if (result_peg_1031.error()){
                goto out_peg_1033;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1032.getValues());
                result_peg_1031.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1031;
    stream.update(result_peg_1031.getPosition());
    
    
    return result_peg_1031;
    out_peg_1033:
    Result result_peg_1037(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1037.getPosition()))){
                    result_peg_1037.nextPosition();
                } else {
                    goto out_peg_1039;
                }
            }
            result_peg_1037.setValue((void*) "a");
        
        Result result_peg_1038 = result_peg_1037;
        
        result_peg_1037 = rule_integer(stream, result_peg_1037.getPosition());
            if (result_peg_1037.error()){
                goto out_peg_1039;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1038.getValues());
                result_peg_1037.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1037;
    stream.update(result_peg_1037.getPosition());
    
    
    return result_peg_1037;
    out_peg_1039:
    Result result_peg_1041(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("a"[i], stream.get(result_peg_1041.getPosition()))){
                    result_peg_1041.nextPosition();
                } else {
                    goto out_peg_1043;
                }
            }
            result_peg_1041.setValue((void*) "a");
        
        Result result_peg_1042 = result_peg_1041;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1042.getValues());
                result_peg_1041.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1041;
    stream.update(result_peg_1041.getPosition());
    
    
    return result_peg_1041;
    out_peg_1043:
    Result result_peg_1044(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_1044.getPosition()))){
                    result_peg_1044.nextPosition();
                } else {
                    goto out_peg_1046;
                }
            }
            result_peg_1044.setValue((void*) "s");
        
        Result result_peg_1045 = result_peg_1044;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1045.getValues());
                result_peg_1044.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1044;
    stream.update(result_peg_1044.getPosition());
    
    
    return result_peg_1044;
    out_peg_1046:
    Result result_peg_1047(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("vh"[i], stream.get(result_peg_1047.getPosition()))){
                    result_peg_1047.nextPosition();
                } else {
                    goto out_peg_1049;
                }
            }
            result_peg_1047.setValue((void*) "vh");
        
        Result result_peg_1048 = result_peg_1047;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1048.getValues());
                result_peg_1047.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1047;
    stream.update(result_peg_1047.getPosition());
    
    
    return result_peg_1047;
    out_peg_1049:
    Result result_peg_1050(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareCharCase("hv"[i], stream.get(result_peg_1050.getPosition()))){
                    result_peg_1050.nextPosition();
                } else {
                    goto out_peg_1052;
                }
            }
            result_peg_1050.setValue((void*) "hv");
        
        Result result_peg_1051 = result_peg_1050;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1051.getValues());
                result_peg_1050.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1050;
    stream.update(result_peg_1050.getPosition());
    
    
    return result_peg_1050;
    out_peg_1052:
    Result result_peg_1053(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("v"[i], stream.get(result_peg_1053.getPosition()))){
                    result_peg_1053.nextPosition();
                } else {
                    goto out_peg_1055;
                }
            }
            result_peg_1053.setValue((void*) "v");
        
        Result result_peg_1054 = result_peg_1053;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1054.getValues());
                result_peg_1053.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1053;
    stream.update(result_peg_1053.getPosition());
    
    
    return result_peg_1053;
    out_peg_1055:
    Result result_peg_1056(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareCharCase("h"[i], stream.get(result_peg_1056.getPosition()))){
                    result_peg_1056.nextPosition();
                } else {
                    goto out_peg_1058;
                }
            }
            result_peg_1056.setValue((void*) "h");
        
        Result result_peg_1057 = result_peg_1056;
        
        {
                Value value((void*) 0);
                value = makeKeyword(result_peg_1057.getValues());
                result_peg_1056.setValue(value);
            }
        
        
    }
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = result_peg_1056;
    stream.update(result_peg_1056.getPosition());
    
    
    return result_peg_1056;
    out_peg_1058:
    
    if (column_peg_1028.chunk3 == 0){
        column_peg_1028.chunk3 = new Chunk3();
    }
    column_peg_1028.chunk3->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

const void * parse(const std::string & filename, bool stats = false){
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

        
