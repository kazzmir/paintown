

#include "mugen/ast/all.h"
#include <map>
#include "gc.h"
typedef std::list<Ast::Section*> SectionList;


#include <stdint.h>
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

    Value(const Value & him):
    which(him.which),
    value(0){
        if (him.isData()){
            value = him.value;
        }
        if (him.isList()){
            values = him.values;
        }
    }

    explicit Value(const void * value):
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

    Value & operator=(const void * what){
        this->value = what;
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

    void setPosition(int position){
        this->position = position;
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
Result chunk_whitespace;
    Result chunk_sn;
    Result chunk_space_newline;
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
        return 0;
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
    line(-1), column(-1),
    message(reason){
    }

    ParseException(const std::string & reason, int line, int column):
    std::exception(),
    line(line), column(column),
    message(reason){
    }

    std::string getReason() const;
    int getLine() const;
    int getColumn() const;

    virtual ~ParseException() throw(){
    }

protected:
    int line, column;
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

        setup();
    }

    /* for null-terminated strings */
    Stream(const char * in):
    temp(0),
    buffer(in),
    farthest(0),
    last_line_info(-1){
        max = strlen(buffer);
        line_info[-1] = LineInfo(1, 1);
        setup();
    }

    /* user-defined length */
    Stream(const char * in, int length):
    temp(0),
    buffer(in),
    farthest(0),
    last_line_info(-1){
        max = length;
        line_info[-1] = LineInfo(1, 1);
        setup();
    }

    void setup(){
        
        createMemo();
    }

    void createMemo(){
        memo_size = 1024 * 2;
        memo = new Column*[memo_size];
        /* dont create column objects before they are needed because transient
         * productions will never call for them so we can save some space by
         * not allocating columns at all.
         */
        memset(memo, 0, sizeof(Column*) * memo_size);
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
        /* Copy old memo table */
        memcpy(newMemo, memo, sizeof(Column*) * memo_size);

        /* Zero out new entries */
        memset(&newMemo[memo_size], 0, sizeof(Column*) * (newSize - memo_size));

        /* Delete old memo table */
        delete[] memo;

        /* Set up new memo table */
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

    /* throws a ParseException */
    void reportError(const std::string & parsingContext){
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
        out << "Error while parsing " << parsingContext << ". Read up till line " << line << " column " << column << std::endl;
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
        out << makeBacktrace(last_trace) << std::endl;
        throw ParseException(out.str(), line, column);
    }

    std::string makeBacktrace(){
        return makeBacktrace(rule_backtrace);
    }

    std::string makeBacktrace(const std::vector<std::string> & trace){
        std::ostringstream out;

        bool first = true;
        for (std::vector<std::string>::const_iterator it = trace.begin(); it != trace.end(); it++){
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
        /* create columns lazily because not every position will have a column. */
        if (memo[position] == NULL){
            memo[position] = new Column();
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
    /* an array is faster and uses less memory than std::map */
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

int ParseException::getLine() const {
    return line;
}

int ParseException::getColumn() const {
    return column;
}

Result errorResult(-1);

Result rule_start(Stream &, const int);
Result rule_line(Stream &, const int, Value current);
Result rule_action(Stream &, const int);
Result rule_action_line(Stream &, const int, Value section);
Result rule_line_end(Stream &, const int);
Result rule_whitespace(Stream &, const int);
Result rule_sn(Stream &, const int);
Result rule_space_newline(Stream &, const int);
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
    /* FIXME: fix line numbers here */
    Ast::String * object = new Ast::String(-1, -1, str);
    GC::save(object);
    return object;
}

Ast::String * makeString(const Value & value){
    /* FIXME: fix line numbers here */
    Ast::String * object = new Ast::String(-1, -1, toString(value));
    GC::save(object);
    return object;
}

Ast::Section * makeSection(const Value & str){
    Ast::Section * object = new Ast::Section(as<std::string*>(str));
    GC::save(object);
    return object;
}

Ast::Keyword * makeKeyword(const Value & value){
    /* FIXME: fix line numbers here */
    Ast::Keyword * object = new Ast::Keyword(-1, -1, as<char*>(value));
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

Ast::Value * makeAlphaSourceDest(const Value & source, const Value & dest){
    std::ostringstream out;
    out << "as" << as<Ast::Value*>(source)->toString() << "d" << as<Ast::Value*>(dest)->toString();

    Ast::Keyword * object = new Ast::Keyword(-1, -1, out.str());
    GC::save(object);
    return object;
}
	      
Ast::Value * makeAlpha(const char * start, const Value & source){
    std::ostringstream out;
    out << start << as<Ast::Value*>(source)->toString();

    Ast::Keyword * object = new Ast::Keyword(-1, -1, out.str());
    GC::save(object);
    return object;
}

Ast::Value * makeValueList(const Value & front, const Value & rest){
    std::list<Ast::Value*> values;
    values.push_back(as<Ast::Value*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        Ast::Value * value = as<Ast::Value*>(Value((*it).getValue()));
        if (value == 0){
            /* FIXME! replace empty with a new node */
            value = makeKeyword(Value("empty"));
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

    /* FIXME! replace empty with a new node */
    Ast::Number * object = new Ast::Number(-1, -1, value);
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
    /* FIXME! fix line numbers here */
    Ast::AttributeArray * object = new Ast::AttributeArray(-1, -1, name, index, value);
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
    /* FIXME: fix line numbers here */
    Ast::AttributeKeyword * object = new Ast::AttributeKeyword(-1, -1, as<Ast::Keyword*>(id), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributeKeyword(const Value & id){
    /* FIXME: fix line numbers here */
    Ast::AttributeKeyword * object = new Ast::AttributeKeyword(-1, -1, as<Ast::Keyword*>(id));
    GC::save(object);
    return object;
}




Result rule_start(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_start.calculated()){
        return column_peg_1.chunk0->chunk_start;
    }
    
    
    RuleTrace trace_peg_23(stream, "start");
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
                    {
                        int position_peg_10 = result_peg_8.getPosition();
                        
                        result_peg_8.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                result_peg_8.setPosition(position_peg_10);
                                goto out_peg_11;
                            }
                        }
                            
                    }
                    goto success_peg_9;
                    out_peg_11:
                    {
                        int position_peg_12 = result_peg_8.getPosition();
                        
                        result_peg_8.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                result_peg_8.setPosition(position_peg_12);
                                goto out_peg_13;
                            }
                        }
                            
                    }
                    goto success_peg_9;
                    out_peg_13:
                    goto loop_peg_7;
                    success_peg_9:
                    ;
                    result_peg_2.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_16(result_peg_2.getPosition());
                    {
                    
                        result_peg_16 = rule_sn(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        result_peg_16 = rule_line(stream, result_peg_16.getPosition(), current);
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        result_peg_16 = rule_whitespace(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        int save_peg_20 = result_peg_16.getPosition();
                            
                            result_peg_16 = rule_line_end(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                
                                result_peg_16 = Result(save_peg_20);
                                result_peg_16.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_16);
                } while (true);
                loop_peg_15:
                ;
            
            
            
            result_peg_2 = rule_sn(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_5;
                }
            
            
            
            if ('\0' == stream.get(result_peg_2.getPosition())){
                    result_peg_2.nextPosition();
                    result_peg_2.setValue(Value((void *) '\0'));
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
    
    
    RuleTrace trace_peg_31(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            {
                    int position_peg_5 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_7(result_peg_2.getPosition());
                        {
                            int position_peg_9 = result_peg_7.getPosition();
                            
                            result_peg_7.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    result_peg_7.setPosition(position_peg_9);
                                    goto out_peg_10;
                                }
                            }
                                
                        }
                        goto success_peg_8;
                        out_peg_10:
                        {
                            int position_peg_11 = result_peg_7.getPosition();
                            
                            result_peg_7.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    result_peg_7.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_8;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_7.getPosition();
                            
                            result_peg_7.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_7.getPosition()) == (unsigned char) 255){
                                result_peg_7.nextPosition();
                            } else {
                                result_peg_7.setPosition(position_peg_13);
                                goto out_peg_14;
                            }
                            
                        }
                        goto success_peg_8;
                        out_peg_14:
                        goto loop_peg_6;
                        success_peg_8:
                        ;
                        result_peg_2.addResult(result_peg_7);
                    } while (true);
                    loop_peg_6:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_15;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_15;
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_15:
        Result result_peg_16(myposition);
        
        
        {
        
            {
                    int position_peg_19 = result_peg_16.getPosition();
                    
                    result_peg_16.reset();
                    do{
                        Result result_peg_21(result_peg_16.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        {
                            int position_peg_27 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_21.getPosition()) == (unsigned char) 255){
                                result_peg_21.nextPosition();
                            } else {
                                result_peg_21.setPosition(position_peg_27);
                                goto out_peg_28;
                            }
                            
                        }
                        goto success_peg_22;
                        out_peg_28:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_16.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_29;
                success_peg_18:
                ;
            
            
            
            result_peg_16 = rule_action(stream, result_peg_16.getPosition());
                if (result_peg_16.error()){
                    goto out_peg_29;
                }
            
            Result result_peg_30 = result_peg_16;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_30.getValues());
                    result_peg_16.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_16.getPosition());
        
        return result_peg_16;
        out_peg_29:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_action.calculated()){
        return column_peg_1.chunk0->chunk_action;
    }
    
    
    RuleTrace trace_peg_20(stream, "action");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2 = rule_action_start(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_3.getValues());
                    result_peg_2.setValue(value);
                }
                ast = result_peg_2.getValues();
            
            
            
            result_peg_2 = rule_whitespace(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_9(result_peg_2.getPosition());
                    {
                        int position_peg_11 = result_peg_9.getPosition();
                        
                        result_peg_9.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_11);
                                goto out_peg_12;
                            }
                        }
                            
                    }
                    goto success_peg_10;
                    out_peg_12:
                    {
                        int position_peg_13 = result_peg_9.getPosition();
                        
                        result_peg_9.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_9.getPosition()))){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_13);
                                goto out_peg_14;
                            }
                        }
                            
                    }
                    goto success_peg_10;
                    out_peg_14:
                    goto loop_peg_8;
                    success_peg_10:
                    ;
                    result_peg_2.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_17(result_peg_2.getPosition());
                    {
                    
                        result_peg_17 = rule_action_line(stream, result_peg_17.getPosition(), ast);
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        result_peg_17 = rule_whitespace(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        result_peg_17 = rule_line_end(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_17);
                } while (true);
                loop_peg_16:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_action = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_action = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position, Value section){
    
    
    RuleTrace trace_peg_131(stream, "action_line");
    int myposition = position;
    
    
    Value data;
        Value line;
    Result result_peg_2(myposition);
        
        
        {
        
            {
                    int position_peg_5 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_7(result_peg_2.getPosition());
                        {
                            int position_peg_9 = result_peg_7.getPosition();
                            
                            result_peg_7.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    result_peg_7.setPosition(position_peg_9);
                                    goto out_peg_10;
                                }
                            }
                                
                        }
                        goto success_peg_8;
                        out_peg_10:
                        {
                            int position_peg_11 = result_peg_7.getPosition();
                            
                            result_peg_7.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    result_peg_7.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_8;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_7.getPosition();
                            
                            result_peg_7.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_7.getPosition()) == (unsigned char) 255){
                                result_peg_7.nextPosition();
                            } else {
                                result_peg_7.setPosition(position_peg_13);
                                goto out_peg_14;
                            }
                            
                        }
                        goto success_peg_8;
                        out_peg_14:
                        goto loop_peg_6;
                        success_peg_8:
                        ;
                        result_peg_2.addResult(result_peg_7);
                    } while (true);
                    loop_peg_6:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_15;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_15;
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_15:
        Result result_peg_16(myposition);
        
        
        {
        
            {
                    int position_peg_19 = result_peg_16.getPosition();
                    
                    result_peg_16.reset();
                    do{
                        Result result_peg_21(result_peg_16.getPosition());
                        {
                            int position_peg_23 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_24:
                        {
                            int position_peg_25 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_26:
                        {
                            int position_peg_27 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_21.getPosition()) == (unsigned char) 255){
                                result_peg_21.nextPosition();
                            } else {
                                result_peg_21.setPosition(position_peg_27);
                                goto out_peg_28;
                            }
                            
                        }
                        goto success_peg_22;
                        out_peg_28:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_16.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_29;
                success_peg_18:
                ;
            
            
            
            result_peg_16 = rule_collision_default(stream, result_peg_16.getPosition());
                if (result_peg_16.error()){
                    goto out_peg_29;
                }
                data = result_peg_16.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_16.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_16.getPosition());
        
        return result_peg_16;
        out_peg_29:
        Result result_peg_31(myposition);
        
        
        {
        
            {
                    int position_peg_34 = result_peg_31.getPosition();
                    
                    result_peg_31.reset();
                    do{
                        Result result_peg_36(result_peg_31.getPosition());
                        {
                            int position_peg_38 = result_peg_36.getPosition();
                            
                            result_peg_36.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_36.getPosition()))){
                                    result_peg_36.nextPosition();
                                } else {
                                    result_peg_36.setPosition(position_peg_38);
                                    goto out_peg_39;
                                }
                            }
                                
                        }
                        goto success_peg_37;
                        out_peg_39:
                        {
                            int position_peg_40 = result_peg_36.getPosition();
                            
                            result_peg_36.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_36.getPosition()))){
                                    result_peg_36.nextPosition();
                                } else {
                                    result_peg_36.setPosition(position_peg_40);
                                    goto out_peg_41;
                                }
                            }
                                
                        }
                        goto success_peg_37;
                        out_peg_41:
                        {
                            int position_peg_42 = result_peg_36.getPosition();
                            
                            result_peg_36.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_36.getPosition()) == (unsigned char) 255){
                                result_peg_36.nextPosition();
                            } else {
                                result_peg_36.setPosition(position_peg_42);
                                goto out_peg_43;
                            }
                            
                        }
                        goto success_peg_37;
                        out_peg_43:
                        goto loop_peg_35;
                        success_peg_37:
                        ;
                        result_peg_31.addResult(result_peg_36);
                    } while (true);
                    loop_peg_35:
                    ;
                            
                }
                goto success_peg_33;
                goto out_peg_44;
                success_peg_33:
                ;
            
            
            
            result_peg_31 = rule_collision(stream, result_peg_31.getPosition());
                if (result_peg_31.error()){
                    goto out_peg_44;
                }
                data = result_peg_31.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_31.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_31.getPosition());
        
        return result_peg_31;
        out_peg_44:
        Result result_peg_46(myposition);
        
        
        {
        
            {
                    int position_peg_49 = result_peg_46.getPosition();
                    
                    result_peg_46.reset();
                    do{
                        Result result_peg_51(result_peg_46.getPosition());
                        {
                            int position_peg_53 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_51.getPosition()))){
                                    result_peg_51.nextPosition();
                                } else {
                                    result_peg_51.setPosition(position_peg_53);
                                    goto out_peg_54;
                                }
                            }
                                
                        }
                        goto success_peg_52;
                        out_peg_54:
                        {
                            int position_peg_55 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_51.getPosition()))){
                                    result_peg_51.nextPosition();
                                } else {
                                    result_peg_51.setPosition(position_peg_55);
                                    goto out_peg_56;
                                }
                            }
                                
                        }
                        goto success_peg_52;
                        out_peg_56:
                        {
                            int position_peg_57 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_51.getPosition()) == (unsigned char) 255){
                                result_peg_51.nextPosition();
                            } else {
                                result_peg_51.setPosition(position_peg_57);
                                goto out_peg_58;
                            }
                            
                        }
                        goto success_peg_52;
                        out_peg_58:
                        goto loop_peg_50;
                        success_peg_52:
                        ;
                        result_peg_46.addResult(result_peg_51);
                    } while (true);
                    loop_peg_50:
                    ;
                            
                }
                goto success_peg_48;
                goto out_peg_59;
                success_peg_48:
                ;
            
            
            
            result_peg_46 = rule_valuelist(stream, result_peg_46.getPosition());
                if (result_peg_46.error()){
                    goto out_peg_59;
                }
                data = result_peg_46.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                    result_peg_46.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_46.getPosition());
        
        return result_peg_46;
        out_peg_59:
        Result result_peg_61(myposition);
        
        
        {
        
            {
                    int position_peg_64 = result_peg_61.getPosition();
                    
                    result_peg_61.reset();
                    do{
                        Result result_peg_66(result_peg_61.getPosition());
                        {
                            int position_peg_68 = result_peg_66.getPosition();
                            
                            result_peg_66.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_66.getPosition()))){
                                    result_peg_66.nextPosition();
                                } else {
                                    result_peg_66.setPosition(position_peg_68);
                                    goto out_peg_69;
                                }
                            }
                                
                        }
                        goto success_peg_67;
                        out_peg_69:
                        {
                            int position_peg_70 = result_peg_66.getPosition();
                            
                            result_peg_66.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_66.getPosition()))){
                                    result_peg_66.nextPosition();
                                } else {
                                    result_peg_66.setPosition(position_peg_70);
                                    goto out_peg_71;
                                }
                            }
                                
                        }
                        goto success_peg_67;
                        out_peg_71:
                        {
                            int position_peg_72 = result_peg_66.getPosition();
                            
                            result_peg_66.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_66.getPosition()) == (unsigned char) 255){
                                result_peg_66.nextPosition();
                            } else {
                                result_peg_66.setPosition(position_peg_72);
                                goto out_peg_73;
                            }
                            
                        }
                        goto success_peg_67;
                        out_peg_73:
                        goto loop_peg_65;
                        success_peg_67:
                        ;
                        result_peg_61.addResult(result_peg_66);
                    } while (true);
                    loop_peg_65:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_74;
                success_peg_63:
                ;
            
            
            
            {
                    int position_peg_77 = result_peg_61.getPosition();
                    {
                    
                        result_peg_61.setValue(Value((void*) "loopstart"));
                            for (int i = 0; i < 9; i++){
                                if (compareCharCase("loopstart"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    result_peg_61.setPosition(position_peg_77);
                                    goto out_peg_79;
                                }
                            }
                        
                        Result result_peg_78 = result_peg_61;
                        
                        int save_peg_81 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) ":"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(":"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    
                                    result_peg_61 = Result(save_peg_81);
                                    result_peg_61.setValue(Value((void*) 0));
                                    
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = makeAttributeKeyword(Value(makeKeyword(result_peg_78.getValues())));
                                result_peg_61.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_76;
                out_peg_79:
                goto out_peg_74;
                success_peg_76:
                ;
                data = result_peg_61.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_61.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_61.getPosition());
        
        return result_peg_61;
        out_peg_74:
        Result result_peg_82(myposition);
        
        
        {
        
            {
                    int position_peg_85 = result_peg_82.getPosition();
                    
                    result_peg_82.setValue(Value((void*) " "));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_82.getPosition()))){
                            result_peg_82.nextPosition();
                        } else {
                            result_peg_82.setPosition(position_peg_85);
                            goto out_peg_86;
                        }
                    }
                        
                }
                goto success_peg_84;
                out_peg_86:
                {
                    int position_peg_87 = result_peg_82.getPosition();
                    
                    result_peg_82.setValue(Value((void*) "\t"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_82.getPosition()))){
                            result_peg_82.nextPosition();
                        } else {
                            result_peg_82.setPosition(position_peg_87);
                            goto out_peg_88;
                        }
                    }
                        
                }
                goto success_peg_84;
                out_peg_88:
                {
                    int position_peg_89 = result_peg_82.getPosition();
                    
                    result_peg_82.setValue(Value((void*) 255));
                    if ((unsigned char) stream.get(result_peg_82.getPosition()) == (unsigned char) 255){
                        result_peg_82.nextPosition();
                    } else {
                        result_peg_82.setPosition(position_peg_89);
                        goto out_peg_90;
                    }
                    
                }
                goto success_peg_84;
                out_peg_90:
                goto out_peg_91;
                success_peg_84:
                ;
            
            
            
            {
                    int position_peg_93 = result_peg_82.getPosition();
                    
                    result_peg_82.reset();
                    do{
                        Result result_peg_95(result_peg_82.getPosition());
                        {
                            int position_peg_97 = result_peg_95.getPosition();
                            
                            result_peg_95.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    result_peg_95.setPosition(position_peg_97);
                                    goto out_peg_98;
                                }
                            }
                                
                        }
                        goto success_peg_96;
                        out_peg_98:
                        {
                            int position_peg_99 = result_peg_95.getPosition();
                            
                            result_peg_95.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    result_peg_95.setPosition(position_peg_99);
                                    goto out_peg_100;
                                }
                            }
                                
                        }
                        goto success_peg_96;
                        out_peg_100:
                        {
                            int position_peg_101 = result_peg_95.getPosition();
                            
                            result_peg_95.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_95.getPosition()) == (unsigned char) 255){
                                result_peg_95.nextPosition();
                            } else {
                                result_peg_95.setPosition(position_peg_101);
                                goto out_peg_102;
                            }
                            
                        }
                        goto success_peg_96;
                        out_peg_102:
                        goto loop_peg_94;
                        success_peg_96:
                        ;
                        result_peg_82.addResult(result_peg_95);
                    } while (true);
                    loop_peg_94:
                    ;
                            
                }
                goto success_peg_92;
                goto out_peg_91;
                success_peg_92:
                ;
            
            
        }
        
        stream.update(result_peg_82.getPosition());
        
        return result_peg_82;
        out_peg_91:
        Result result_peg_103(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_105 = stream.getLineInfo(result_peg_103.getPosition());
                line = &line_info_peg_105;
            
            
            
            Result result_peg_108(result_peg_103);
                {
                
                    {
                            int position_peg_111 = result_peg_108.getPosition();
                            
                            result_peg_108.reset();
                            do{
                                Result result_peg_113(result_peg_108.getPosition());
                                {
                                    int position_peg_115 = result_peg_113.getPosition();
                                    
                                    result_peg_113.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_113.getPosition()))){
                                            result_peg_113.nextPosition();
                                        } else {
                                            result_peg_113.setPosition(position_peg_115);
                                            goto out_peg_116;
                                        }
                                    }
                                        
                                }
                                goto success_peg_114;
                                out_peg_116:
                                {
                                    int position_peg_117 = result_peg_113.getPosition();
                                    
                                    result_peg_113.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_113.getPosition()))){
                                            result_peg_113.nextPosition();
                                        } else {
                                            result_peg_113.setPosition(position_peg_117);
                                            goto out_peg_118;
                                        }
                                    }
                                        
                                }
                                goto success_peg_114;
                                out_peg_118:
                                {
                                    int position_peg_119 = result_peg_113.getPosition();
                                    
                                    result_peg_113.setValue(Value((void*) 255));
                                    if ((unsigned char) stream.get(result_peg_113.getPosition()) == (unsigned char) 255){
                                        result_peg_113.nextPosition();
                                    } else {
                                        result_peg_113.setPosition(position_peg_119);
                                        goto out_peg_120;
                                    }
                                    
                                }
                                goto success_peg_114;
                                out_peg_120:
                                goto loop_peg_112;
                                success_peg_114:
                                ;
                                result_peg_108.addResult(result_peg_113);
                            } while (true);
                            loop_peg_112:
                            ;
                                    
                        }
                        goto success_peg_110;
                        goto not_peg_107;
                        success_peg_110:
                        ;
                    
                    
                    
                    result_peg_108 = rule_action_start(stream, result_peg_108.getPosition());
                        if (result_peg_108.error()){
                            goto not_peg_107;
                        }
                    
                    
                }
                goto out_peg_121;
                not_peg_107:
                result_peg_103.setValue(Value((void*)0));
            
            
            
            result_peg_103.reset();
                do{
                    Result result_peg_124(result_peg_103.getPosition());
                    {
                    
                        Result result_peg_127(result_peg_124);
                            result_peg_127 = rule_line_end(stream, result_peg_127.getPosition());
                            if (result_peg_127.error()){
                                goto not_peg_126;
                            }
                            goto loop_peg_123;
                            not_peg_126:
                            result_peg_124.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_128 = stream.get(result_peg_124.getPosition());
                            if (temp_peg_128 != '\0'){
                                result_peg_124.setValue(Value((void*) (intptr_t) temp_peg_128));
                                result_peg_124.nextPosition();
                            } else {
                                goto loop_peg_123;
                            }
                        
                        
                    }
                    result_peg_103.addResult(result_peg_124);
                } while (true);
                loop_peg_123:
                if (result_peg_103.matches() == 0){
                    goto out_peg_121;
                }
            
            
            
            Result result_peg_130(result_peg_103.getPosition());
                result_peg_130 = rule_line_end(stream, result_peg_130.getPosition());
                if (result_peg_130.error()){
                    goto out_peg_121;
                }
            
            
            
            {
                    Value value((void*) 0);
                    std::cout << "Warning: ignoring input at line " << getCurrentLine(line) << " column " << getCurrentColumn(line) << std::endl;
                    result_peg_103.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_103.getPosition());
        
        return result_peg_103;
        out_peg_121:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_line_end.calculated()){
        return column_peg_1.chunk0->chunk_line_end;
    }
    
    
    RuleTrace trace_peg_15(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            {
                int position_peg_6 = result_peg_4.getPosition();
                
                result_peg_4.setValue(Value((void*) "\n"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        result_peg_4.setPosition(position_peg_6);
                        goto out_peg_7;
                    }
                }
                    
            }
            goto success_peg_5;
            out_peg_7:
            {
                int position_peg_8 = result_peg_4.getPosition();
                
                result_peg_4.setValue(Value((void*) "\r"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        result_peg_4.setPosition(position_peg_8);
                        goto out_peg_9;
                    }
                }
                    
            }
            goto success_peg_5;
            out_peg_9:
            goto loop_peg_3;
            success_peg_5:
            ;
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        if (result_peg_2.matches() == 0){
            goto out_peg_10;
        }
        
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_10:
        Result result_peg_11(myposition);
        
        
        {
        
            Result result_peg_13(result_peg_11.getPosition());
                if ('\0' == stream.get(result_peg_13.getPosition())){
                    result_peg_13.nextPosition();
                    result_peg_13.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_14;
                }
            
            
            
            
            
            
        }
        
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_14:
    
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_whitespace.calculated()){
        return column_peg_1.chunk1->chunk_whitespace;
    }
    
    
    RuleTrace trace_peg_5(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            result_peg_4 = rule_sw(stream, result_peg_4.getPosition());
            if (result_peg_4.error()){
                goto loop_peg_3;
            }
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        ;
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_whitespace = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_sn(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_sn.calculated()){
        return column_peg_1.chunk1->chunk_sn;
    }
    
    
    RuleTrace trace_peg_5(stream, "sn");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            result_peg_4 = rule_space_newline(stream, result_peg_4.getPosition());
            if (result_peg_4.error()){
                goto loop_peg_3;
            }
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        ;
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sn = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sn = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_space_newline(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_space_newline.calculated()){
        return column_peg_1.chunk1->chunk_space_newline;
    }
    
    
    RuleTrace trace_peg_18(stream, "space_newline");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
            int position_peg_4 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_4);
                    goto out_peg_5;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_5:
        {
            int position_peg_6 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_6);
                    goto out_peg_7;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_7:
        {
            int position_peg_8 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_2.getPosition()) == (unsigned char) 255){
                result_peg_2.nextPosition();
            } else {
                result_peg_2.setPosition(position_peg_8);
                goto out_peg_9;
            }
            
        }
        goto success_peg_3;
        out_peg_9:
        goto out_peg_10;
        success_peg_3:
        ;
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_space_newline = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_10:
        Result result_peg_11(myposition);
        
        
        {
            int position_peg_13 = result_peg_11.getPosition();
            
            result_peg_11.setValue(Value((void*) "\n"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_11.getPosition()))){
                    result_peg_11.nextPosition();
                } else {
                    result_peg_11.setPosition(position_peg_13);
                    goto out_peg_14;
                }
            }
                
        }
        goto success_peg_12;
        out_peg_14:
        {
            int position_peg_15 = result_peg_11.getPosition();
            
            result_peg_11.setValue(Value((void*) "\r"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_11.getPosition()))){
                    result_peg_11.nextPosition();
                } else {
                    result_peg_11.setPosition(position_peg_15);
                    goto out_peg_16;
                }
            }
                
        }
        goto success_peg_12;
        out_peg_16:
        goto out_peg_17;
        success_peg_12:
        ;
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_space_newline = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_17:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_space_newline = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_sw.calculated()){
        return column_peg_1.chunk1->chunk_sw;
    }
    
    
    RuleTrace trace_peg_13(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
            int position_peg_4 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_4);
                    goto out_peg_5;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_5:
        {
            int position_peg_6 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_6);
                    goto out_peg_7;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_7:
        {
            int position_peg_8 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_2.getPosition()) == (unsigned char) 255){
                result_peg_2.nextPosition();
            } else {
                result_peg_2.setPosition(position_peg_8);
                goto out_peg_9;
            }
            
        }
        goto success_peg_3;
        out_peg_9:
        goto out_peg_10;
        success_peg_3:
        ;
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sw = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_10:
        Result result_peg_11(myposition);
        
        
        result_peg_11 = rule_comment(stream, result_peg_11.getPosition());
        if (result_peg_11.error()){
            goto out_peg_12;
        }
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sw = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_12:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_comment.calculated()){
        return column_peg_1.chunk1->chunk_comment;
    }
    
    
    RuleTrace trace_peg_45(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2.setValue(Value((void*) ";"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_6(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_9(result_peg_6);
                            {
                                int position_peg_11 = result_peg_9.getPosition();
                                
                                result_peg_9.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                                        result_peg_9.nextPosition();
                                    } else {
                                        result_peg_9.setPosition(position_peg_11);
                                        goto out_peg_12;
                                    }
                                }
                                    
                            }
                            goto success_peg_10;
                            out_peg_12:
                            {
                                int position_peg_13 = result_peg_9.getPosition();
                                
                                result_peg_9.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_9.getPosition()))){
                                        result_peg_9.nextPosition();
                                    } else {
                                        result_peg_9.setPosition(position_peg_13);
                                        goto out_peg_14;
                                    }
                                }
                                    
                            }
                            goto success_peg_10;
                            out_peg_14:
                            goto not_peg_8;
                            success_peg_10:
                            ;
                            goto loop_peg_5;
                            not_peg_8:
                            result_peg_6.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_15 = stream.get(result_peg_6.getPosition());
                            if (temp_peg_15 != '\0'){
                                result_peg_6.setValue(Value((void*) (intptr_t) temp_peg_15));
                                result_peg_6.nextPosition();
                            } else {
                                goto loop_peg_5;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                ;
            
            
        }
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_16(myposition);
        
        
        {
        
            result_peg_16.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_16.getPosition()))){
                        result_peg_16.nextPosition();
                    } else {
                        goto out_peg_18;
                    }
                }
            
            
            
            result_peg_16.reset();
                do{
                    Result result_peg_20(result_peg_16.getPosition());
                    {
                    
                        Result result_peg_23(result_peg_20);
                            {
                                int position_peg_25 = result_peg_23.getPosition();
                                
                                result_peg_23.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_23.getPosition()))){
                                        result_peg_23.nextPosition();
                                    } else {
                                        result_peg_23.setPosition(position_peg_25);
                                        goto out_peg_26;
                                    }
                                }
                                    
                            }
                            goto success_peg_24;
                            out_peg_26:
                            {
                                int position_peg_27 = result_peg_23.getPosition();
                                
                                result_peg_23.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_23.getPosition()))){
                                        result_peg_23.nextPosition();
                                    } else {
                                        result_peg_23.setPosition(position_peg_27);
                                        goto out_peg_28;
                                    }
                                }
                                    
                            }
                            goto success_peg_24;
                            out_peg_28:
                            goto not_peg_22;
                            success_peg_24:
                            ;
                            goto loop_peg_19;
                            not_peg_22:
                            result_peg_20.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_29 = stream.get(result_peg_20.getPosition());
                            if (temp_peg_29 != '\0'){
                                result_peg_20.setValue(Value((void*) (intptr_t) temp_peg_29));
                                result_peg_20.nextPosition();
                            } else {
                                goto loop_peg_19;
                            }
                        
                        
                    }
                    result_peg_16.addResult(result_peg_20);
                } while (true);
                loop_peg_19:
                ;
            
            
        }
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_16;
        stream.update(result_peg_16.getPosition());
        
        
        return result_peg_16;
        out_peg_18:
        Result result_peg_30(myposition);
        
        
        {
        
            result_peg_30.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_32;
                    }
                }
            
            
            
            result_peg_30.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_32;
                    }
                }
            
            
            
            result_peg_30.reset();
                do{
                    Result result_peg_35(result_peg_30.getPosition());
                    {
                    
                        Result result_peg_38(result_peg_35);
                            {
                                int position_peg_40 = result_peg_38.getPosition();
                                
                                result_peg_38.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_38.getPosition()))){
                                        result_peg_38.nextPosition();
                                    } else {
                                        result_peg_38.setPosition(position_peg_40);
                                        goto out_peg_41;
                                    }
                                }
                                    
                            }
                            goto success_peg_39;
                            out_peg_41:
                            {
                                int position_peg_42 = result_peg_38.getPosition();
                                
                                result_peg_38.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_38.getPosition()))){
                                        result_peg_38.nextPosition();
                                    } else {
                                        result_peg_38.setPosition(position_peg_42);
                                        goto out_peg_43;
                                    }
                                }
                                    
                            }
                            goto success_peg_39;
                            out_peg_43:
                            goto not_peg_37;
                            success_peg_39:
                            ;
                            goto loop_peg_34;
                            not_peg_37:
                            result_peg_35.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_44 = stream.get(result_peg_35.getPosition());
                            if (temp_peg_44 != '\0'){
                                result_peg_35.setValue(Value((void*) (intptr_t) temp_peg_44));
                                result_peg_35.nextPosition();
                            } else {
                                goto loop_peg_34;
                            }
                        
                        
                    }
                    result_peg_30.addResult(result_peg_35);
                } while (true);
                loop_peg_34:
                ;
            
            
        }
        
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_30;
        stream.update(result_peg_30.getPosition());
        
        
        return result_peg_30;
        out_peg_32:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_collision_default.calculated()){
        return column_peg_1.chunk2->chunk_collision_default;
    }
    
    
    RuleTrace trace_peg_70(stream, "collision_default");
    int myposition = position;
    
    
    Value num;
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2.setValue(Value((void*) "Clsn2Default"));
                for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn2Default"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_10(result_peg_2.getPosition());
                        {
                            int position_peg_12 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    result_peg_10.setPosition(position_peg_14);
                                    goto out_peg_15;
                                }
                            }
                                
                        }
                        goto success_peg_11;
                        out_peg_15:
                        {
                            int position_peg_16 = result_peg_10.getPosition();
                            
                            result_peg_10.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_10.getPosition()) == (unsigned char) 255){
                                result_peg_10.nextPosition();
                            } else {
                                result_peg_10.setPosition(position_peg_16);
                                goto out_peg_17;
                            }
                            
                        }
                        goto success_peg_11;
                        out_peg_17:
                        goto loop_peg_9;
                        success_peg_11:
                        ;
                        result_peg_2.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_4;
                success_peg_7:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                num = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_3.getValues())), num);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_19(myposition);
        
        
        {
        
            result_peg_19.setValue(Value((void*) "Clsn1Default"));
                for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_19.getPosition()))){
                        result_peg_19.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
            
            Result result_peg_20 = result_peg_19;
            
            result_peg_19.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_19.getPosition()))){
                        result_peg_19.nextPosition();
                    } else {
                        goto out_peg_21;
                    }
                }
            
            
            
            {
                    int position_peg_25 = result_peg_19.getPosition();
                    
                    result_peg_19.reset();
                    do{
                        Result result_peg_27(result_peg_19.getPosition());
                        {
                            int position_peg_29 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_29);
                                    goto out_peg_30;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_30:
                        {
                            int position_peg_31 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    result_peg_27.setPosition(position_peg_31);
                                    goto out_peg_32;
                                }
                            }
                                
                        }
                        goto success_peg_28;
                        out_peg_32:
                        {
                            int position_peg_33 = result_peg_27.getPosition();
                            
                            result_peg_27.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_27.getPosition()) == (unsigned char) 255){
                                result_peg_27.nextPosition();
                            } else {
                                result_peg_27.setPosition(position_peg_33);
                                goto out_peg_34;
                            }
                            
                        }
                        goto success_peg_28;
                        out_peg_34:
                        goto loop_peg_26;
                        success_peg_28:
                        ;
                        result_peg_19.addResult(result_peg_27);
                    } while (true);
                    loop_peg_26:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_21;
                success_peg_24:
                ;
            
            
            
            result_peg_19 = rule_integer(stream, result_peg_19.getPosition());
                if (result_peg_19.error()){
                    goto out_peg_21;
                }
                num = result_peg_19.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_20.getValues())), num);
                    result_peg_19.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_19;
        stream.update(result_peg_19.getPosition());
        
        
        return result_peg_19;
        out_peg_21:
        Result result_peg_36(myposition);
        
        
        {
        
            result_peg_36.setValue(Value((void*) "Clsn2"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_36.getPosition()))){
                        result_peg_36.nextPosition();
                    } else {
                        goto out_peg_38;
                    }
                }
            
            Result result_peg_37 = result_peg_36;
            
            result_peg_36.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_36.getPosition()))){
                        result_peg_36.nextPosition();
                    } else {
                        goto out_peg_38;
                    }
                }
            
            
            
            {
                    int position_peg_42 = result_peg_36.getPosition();
                    
                    result_peg_36.reset();
                    do{
                        Result result_peg_44(result_peg_36.getPosition());
                        {
                            int position_peg_46 = result_peg_44.getPosition();
                            
                            result_peg_44.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_44.getPosition()))){
                                    result_peg_44.nextPosition();
                                } else {
                                    result_peg_44.setPosition(position_peg_46);
                                    goto out_peg_47;
                                }
                            }
                                
                        }
                        goto success_peg_45;
                        out_peg_47:
                        {
                            int position_peg_48 = result_peg_44.getPosition();
                            
                            result_peg_44.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_44.getPosition()))){
                                    result_peg_44.nextPosition();
                                } else {
                                    result_peg_44.setPosition(position_peg_48);
                                    goto out_peg_49;
                                }
                            }
                                
                        }
                        goto success_peg_45;
                        out_peg_49:
                        {
                            int position_peg_50 = result_peg_44.getPosition();
                            
                            result_peg_44.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_44.getPosition()) == (unsigned char) 255){
                                result_peg_44.nextPosition();
                            } else {
                                result_peg_44.setPosition(position_peg_50);
                                goto out_peg_51;
                            }
                            
                        }
                        goto success_peg_45;
                        out_peg_51:
                        goto loop_peg_43;
                        success_peg_45:
                        ;
                        result_peg_36.addResult(result_peg_44);
                    } while (true);
                    loop_peg_43:
                    ;
                            
                }
                goto success_peg_41;
                goto out_peg_38;
                success_peg_41:
                ;
            
            
            
            result_peg_36 = rule_integer(stream, result_peg_36.getPosition());
                if (result_peg_36.error()){
                    goto out_peg_38;
                }
                num = result_peg_36.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_37.getValues())), num);
                    result_peg_36.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_36;
        stream.update(result_peg_36.getPosition());
        
        
        return result_peg_36;
        out_peg_38:
        Result result_peg_53(myposition);
        
        
        {
        
            result_peg_53.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_53.getPosition()))){
                        result_peg_53.nextPosition();
                    } else {
                        goto out_peg_55;
                    }
                }
            
            Result result_peg_54 = result_peg_53;
            
            result_peg_53.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_53.getPosition()))){
                        result_peg_53.nextPosition();
                    } else {
                        goto out_peg_55;
                    }
                }
            
            
            
            {
                    int position_peg_59 = result_peg_53.getPosition();
                    
                    result_peg_53.reset();
                    do{
                        Result result_peg_61(result_peg_53.getPosition());
                        {
                            int position_peg_63 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    result_peg_61.setPosition(position_peg_63);
                                    goto out_peg_64;
                                }
                            }
                                
                        }
                        goto success_peg_62;
                        out_peg_64:
                        {
                            int position_peg_65 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    result_peg_61.setPosition(position_peg_65);
                                    goto out_peg_66;
                                }
                            }
                                
                        }
                        goto success_peg_62;
                        out_peg_66:
                        {
                            int position_peg_67 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_61.getPosition()) == (unsigned char) 255){
                                result_peg_61.nextPosition();
                            } else {
                                result_peg_61.setPosition(position_peg_67);
                                goto out_peg_68;
                            }
                            
                        }
                        goto success_peg_62;
                        out_peg_68:
                        goto loop_peg_60;
                        success_peg_62:
                        ;
                        result_peg_53.addResult(result_peg_61);
                    } while (true);
                    loop_peg_60:
                    ;
                            
                }
                goto success_peg_58;
                goto out_peg_55;
                success_peg_58:
                ;
            
            
            
            result_peg_53 = rule_integer(stream, result_peg_53.getPosition());
                if (result_peg_53.error()){
                    goto out_peg_55;
                }
                num = result_peg_53.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_54.getValues())), num);
                    result_peg_53.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_53;
        stream.update(result_peg_53.getPosition());
        
        
        return result_peg_53;
        out_peg_55:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_collision.calculated()){
        return column_peg_1.chunk2->chunk_collision;
    }
    
    
    RuleTrace trace_peg_440(stream, "collision");
    int myposition = position;
    
    
    Value name;
        Value index;
        Value n1;
        Value n2;
        Value n3;
        Value n4;
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2.setValue(Value((void*) "Clsn2"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                name = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_7 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_9(result_peg_2.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        {
                            int position_peg_15 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_9.getPosition()) == (unsigned char) 255){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_15);
                                goto out_peg_16;
                            }
                            
                        }
                        goto success_peg_10;
                        out_peg_16:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_2.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_20 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_22(result_peg_2.getPosition());
                        {
                            int position_peg_24 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_24);
                                    goto out_peg_25;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_25:
                        {
                            int position_peg_26 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_26);
                                    goto out_peg_27;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_27:
                        {
                            int position_peg_28 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_22.getPosition()) == (unsigned char) 255){
                                result_peg_22.nextPosition();
                            } else {
                                result_peg_22.setPosition(position_peg_28);
                                goto out_peg_29;
                            }
                            
                        }
                        goto success_peg_23;
                        out_peg_29:
                        goto loop_peg_21;
                        success_peg_23:
                        ;
                        result_peg_2.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                            
                }
                goto success_peg_19;
                goto out_peg_4;
                success_peg_19:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                index = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_33 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_35(result_peg_2.getPosition());
                        {
                            int position_peg_37 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_37);
                                    goto out_peg_38;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_38:
                        {
                            int position_peg_39 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_39);
                                    goto out_peg_40;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_40:
                        {
                            int position_peg_41 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_35.getPosition()) == (unsigned char) 255){
                                result_peg_35.nextPosition();
                            } else {
                                result_peg_35.setPosition(position_peg_41);
                                goto out_peg_42;
                            }
                            
                        }
                        goto success_peg_36;
                        out_peg_42:
                        goto loop_peg_34;
                        success_peg_36:
                        ;
                        result_peg_2.addResult(result_peg_35);
                    } while (true);
                    loop_peg_34:
                    ;
                            
                }
                goto success_peg_32;
                goto out_peg_4;
                success_peg_32:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_46 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_48(result_peg_2.getPosition());
                        {
                            int position_peg_50 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_51:
                        {
                            int position_peg_52 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_52);
                                    goto out_peg_53;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_53:
                        {
                            int position_peg_54 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_48.getPosition()) == (unsigned char) 255){
                                result_peg_48.nextPosition();
                            } else {
                                result_peg_48.setPosition(position_peg_54);
                                goto out_peg_55;
                            }
                            
                        }
                        goto success_peg_49;
                        out_peg_55:
                        goto loop_peg_47;
                        success_peg_49:
                        ;
                        result_peg_2.addResult(result_peg_48);
                    } while (true);
                    loop_peg_47:
                    ;
                            
                }
                goto success_peg_45;
                goto out_peg_4;
                success_peg_45:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_59 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_61(result_peg_2.getPosition());
                        {
                            int position_peg_63 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    result_peg_61.setPosition(position_peg_63);
                                    goto out_peg_64;
                                }
                            }
                                
                        }
                        goto success_peg_62;
                        out_peg_64:
                        {
                            int position_peg_65 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    result_peg_61.setPosition(position_peg_65);
                                    goto out_peg_66;
                                }
                            }
                                
                        }
                        goto success_peg_62;
                        out_peg_66:
                        {
                            int position_peg_67 = result_peg_61.getPosition();
                            
                            result_peg_61.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_61.getPosition()) == (unsigned char) 255){
                                result_peg_61.nextPosition();
                            } else {
                                result_peg_61.setPosition(position_peg_67);
                                goto out_peg_68;
                            }
                            
                        }
                        goto success_peg_62;
                        out_peg_68:
                        goto loop_peg_60;
                        success_peg_62:
                        ;
                        result_peg_2.addResult(result_peg_61);
                    } while (true);
                    loop_peg_60:
                    ;
                            
                }
                goto success_peg_58;
                goto out_peg_4;
                success_peg_58:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n1 = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_72 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_74(result_peg_2.getPosition());
                        {
                            int position_peg_76 = result_peg_74.getPosition();
                            
                            result_peg_74.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    result_peg_74.setPosition(position_peg_76);
                                    goto out_peg_77;
                                }
                            }
                                
                        }
                        goto success_peg_75;
                        out_peg_77:
                        {
                            int position_peg_78 = result_peg_74.getPosition();
                            
                            result_peg_74.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    result_peg_74.setPosition(position_peg_78);
                                    goto out_peg_79;
                                }
                            }
                                
                        }
                        goto success_peg_75;
                        out_peg_79:
                        {
                            int position_peg_80 = result_peg_74.getPosition();
                            
                            result_peg_74.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_74.getPosition()) == (unsigned char) 255){
                                result_peg_74.nextPosition();
                            } else {
                                result_peg_74.setPosition(position_peg_80);
                                goto out_peg_81;
                            }
                            
                        }
                        goto success_peg_75;
                        out_peg_81:
                        goto loop_peg_73;
                        success_peg_75:
                        ;
                        result_peg_2.addResult(result_peg_74);
                    } while (true);
                    loop_peg_73:
                    ;
                            
                }
                goto success_peg_71;
                goto out_peg_4;
                success_peg_71:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_85 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_87(result_peg_2.getPosition());
                        {
                            int position_peg_89 = result_peg_87.getPosition();
                            
                            result_peg_87.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    result_peg_87.setPosition(position_peg_89);
                                    goto out_peg_90;
                                }
                            }
                                
                        }
                        goto success_peg_88;
                        out_peg_90:
                        {
                            int position_peg_91 = result_peg_87.getPosition();
                            
                            result_peg_87.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    result_peg_87.setPosition(position_peg_91);
                                    goto out_peg_92;
                                }
                            }
                                
                        }
                        goto success_peg_88;
                        out_peg_92:
                        {
                            int position_peg_93 = result_peg_87.getPosition();
                            
                            result_peg_87.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_87.getPosition()) == (unsigned char) 255){
                                result_peg_87.nextPosition();
                            } else {
                                result_peg_87.setPosition(position_peg_93);
                                goto out_peg_94;
                            }
                            
                        }
                        goto success_peg_88;
                        out_peg_94:
                        goto loop_peg_86;
                        success_peg_88:
                        ;
                        result_peg_2.addResult(result_peg_87);
                    } while (true);
                    loop_peg_86:
                    ;
                            
                }
                goto success_peg_84;
                goto out_peg_4;
                success_peg_84:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n2 = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_98 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_100(result_peg_2.getPosition());
                        {
                            int position_peg_102 = result_peg_100.getPosition();
                            
                            result_peg_100.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_100.getPosition()))){
                                    result_peg_100.nextPosition();
                                } else {
                                    result_peg_100.setPosition(position_peg_102);
                                    goto out_peg_103;
                                }
                            }
                                
                        }
                        goto success_peg_101;
                        out_peg_103:
                        {
                            int position_peg_104 = result_peg_100.getPosition();
                            
                            result_peg_100.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_100.getPosition()))){
                                    result_peg_100.nextPosition();
                                } else {
                                    result_peg_100.setPosition(position_peg_104);
                                    goto out_peg_105;
                                }
                            }
                                
                        }
                        goto success_peg_101;
                        out_peg_105:
                        {
                            int position_peg_106 = result_peg_100.getPosition();
                            
                            result_peg_100.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_100.getPosition()) == (unsigned char) 255){
                                result_peg_100.nextPosition();
                            } else {
                                result_peg_100.setPosition(position_peg_106);
                                goto out_peg_107;
                            }
                            
                        }
                        goto success_peg_101;
                        out_peg_107:
                        goto loop_peg_99;
                        success_peg_101:
                        ;
                        result_peg_2.addResult(result_peg_100);
                    } while (true);
                    loop_peg_99:
                    ;
                            
                }
                goto success_peg_97;
                goto out_peg_4;
                success_peg_97:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_111 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_113(result_peg_2.getPosition());
                        {
                            int position_peg_115 = result_peg_113.getPosition();
                            
                            result_peg_113.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_113.getPosition()))){
                                    result_peg_113.nextPosition();
                                } else {
                                    result_peg_113.setPosition(position_peg_115);
                                    goto out_peg_116;
                                }
                            }
                                
                        }
                        goto success_peg_114;
                        out_peg_116:
                        {
                            int position_peg_117 = result_peg_113.getPosition();
                            
                            result_peg_113.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_113.getPosition()))){
                                    result_peg_113.nextPosition();
                                } else {
                                    result_peg_113.setPosition(position_peg_117);
                                    goto out_peg_118;
                                }
                            }
                                
                        }
                        goto success_peg_114;
                        out_peg_118:
                        {
                            int position_peg_119 = result_peg_113.getPosition();
                            
                            result_peg_113.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_113.getPosition()) == (unsigned char) 255){
                                result_peg_113.nextPosition();
                            } else {
                                result_peg_113.setPosition(position_peg_119);
                                goto out_peg_120;
                            }
                            
                        }
                        goto success_peg_114;
                        out_peg_120:
                        goto loop_peg_112;
                        success_peg_114:
                        ;
                        result_peg_2.addResult(result_peg_113);
                    } while (true);
                    loop_peg_112:
                    ;
                            
                }
                goto success_peg_110;
                goto out_peg_4;
                success_peg_110:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n3 = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_124 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_126(result_peg_2.getPosition());
                        {
                            int position_peg_128 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    result_peg_126.setPosition(position_peg_128);
                                    goto out_peg_129;
                                }
                            }
                                
                        }
                        goto success_peg_127;
                        out_peg_129:
                        {
                            int position_peg_130 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    result_peg_126.setPosition(position_peg_130);
                                    goto out_peg_131;
                                }
                            }
                                
                        }
                        goto success_peg_127;
                        out_peg_131:
                        {
                            int position_peg_132 = result_peg_126.getPosition();
                            
                            result_peg_126.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_126.getPosition()) == (unsigned char) 255){
                                result_peg_126.nextPosition();
                            } else {
                                result_peg_126.setPosition(position_peg_132);
                                goto out_peg_133;
                            }
                            
                        }
                        goto success_peg_127;
                        out_peg_133:
                        goto loop_peg_125;
                        success_peg_127:
                        ;
                        result_peg_2.addResult(result_peg_126);
                    } while (true);
                    loop_peg_125:
                    ;
                            
                }
                goto success_peg_123;
                goto out_peg_4;
                success_peg_123:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_137 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_139(result_peg_2.getPosition());
                        {
                            int position_peg_141 = result_peg_139.getPosition();
                            
                            result_peg_139.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_139.getPosition()))){
                                    result_peg_139.nextPosition();
                                } else {
                                    result_peg_139.setPosition(position_peg_141);
                                    goto out_peg_142;
                                }
                            }
                                
                        }
                        goto success_peg_140;
                        out_peg_142:
                        {
                            int position_peg_143 = result_peg_139.getPosition();
                            
                            result_peg_139.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_139.getPosition()))){
                                    result_peg_139.nextPosition();
                                } else {
                                    result_peg_139.setPosition(position_peg_143);
                                    goto out_peg_144;
                                }
                            }
                                
                        }
                        goto success_peg_140;
                        out_peg_144:
                        {
                            int position_peg_145 = result_peg_139.getPosition();
                            
                            result_peg_139.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_139.getPosition()) == (unsigned char) 255){
                                result_peg_139.nextPosition();
                            } else {
                                result_peg_139.setPosition(position_peg_145);
                                goto out_peg_146;
                            }
                            
                        }
                        goto success_peg_140;
                        out_peg_146:
                        goto loop_peg_138;
                        success_peg_140:
                        ;
                        result_peg_2.addResult(result_peg_139);
                    } while (true);
                    loop_peg_138:
                    ;
                            
                }
                goto success_peg_136;
                goto out_peg_4;
                success_peg_136:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n4 = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_148(myposition);
        
        
        {
        
            result_peg_148.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
                name = result_peg_148.getValues();
            
            
            
            {
                    int position_peg_153 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_155(result_peg_148.getPosition());
                        {
                            int position_peg_157 = result_peg_155.getPosition();
                            
                            result_peg_155.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_155.getPosition()))){
                                    result_peg_155.nextPosition();
                                } else {
                                    result_peg_155.setPosition(position_peg_157);
                                    goto out_peg_158;
                                }
                            }
                                
                        }
                        goto success_peg_156;
                        out_peg_158:
                        {
                            int position_peg_159 = result_peg_155.getPosition();
                            
                            result_peg_155.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_155.getPosition()))){
                                    result_peg_155.nextPosition();
                                } else {
                                    result_peg_155.setPosition(position_peg_159);
                                    goto out_peg_160;
                                }
                            }
                                
                        }
                        goto success_peg_156;
                        out_peg_160:
                        {
                            int position_peg_161 = result_peg_155.getPosition();
                            
                            result_peg_155.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_155.getPosition()) == (unsigned char) 255){
                                result_peg_155.nextPosition();
                            } else {
                                result_peg_155.setPosition(position_peg_161);
                                goto out_peg_162;
                            }
                            
                        }
                        goto success_peg_156;
                        out_peg_162:
                        goto loop_peg_154;
                        success_peg_156:
                        ;
                        result_peg_148.addResult(result_peg_155);
                    } while (true);
                    loop_peg_154:
                    ;
                            
                }
                goto success_peg_152;
                goto out_peg_150;
                success_peg_152:
                ;
            
            
            
            result_peg_148.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
            
            
            
            {
                    int position_peg_166 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_168(result_peg_148.getPosition());
                        {
                            int position_peg_170 = result_peg_168.getPosition();
                            
                            result_peg_168.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_168.getPosition()))){
                                    result_peg_168.nextPosition();
                                } else {
                                    result_peg_168.setPosition(position_peg_170);
                                    goto out_peg_171;
                                }
                            }
                                
                        }
                        goto success_peg_169;
                        out_peg_171:
                        {
                            int position_peg_172 = result_peg_168.getPosition();
                            
                            result_peg_168.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_168.getPosition()))){
                                    result_peg_168.nextPosition();
                                } else {
                                    result_peg_168.setPosition(position_peg_172);
                                    goto out_peg_173;
                                }
                            }
                                
                        }
                        goto success_peg_169;
                        out_peg_173:
                        {
                            int position_peg_174 = result_peg_168.getPosition();
                            
                            result_peg_168.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_168.getPosition()) == (unsigned char) 255){
                                result_peg_168.nextPosition();
                            } else {
                                result_peg_168.setPosition(position_peg_174);
                                goto out_peg_175;
                            }
                            
                        }
                        goto success_peg_169;
                        out_peg_175:
                        goto loop_peg_167;
                        success_peg_169:
                        ;
                        result_peg_148.addResult(result_peg_168);
                    } while (true);
                    loop_peg_167:
                    ;
                            
                }
                goto success_peg_165;
                goto out_peg_150;
                success_peg_165:
                ;
            
            
            
            result_peg_148 = rule_integer(stream, result_peg_148.getPosition());
                if (result_peg_148.error()){
                    goto out_peg_150;
                }
                index = result_peg_148.getValues();
            
            
            
            {
                    int position_peg_179 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_181(result_peg_148.getPosition());
                        {
                            int position_peg_183 = result_peg_181.getPosition();
                            
                            result_peg_181.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_181.getPosition()))){
                                    result_peg_181.nextPosition();
                                } else {
                                    result_peg_181.setPosition(position_peg_183);
                                    goto out_peg_184;
                                }
                            }
                                
                        }
                        goto success_peg_182;
                        out_peg_184:
                        {
                            int position_peg_185 = result_peg_181.getPosition();
                            
                            result_peg_181.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_181.getPosition()))){
                                    result_peg_181.nextPosition();
                                } else {
                                    result_peg_181.setPosition(position_peg_185);
                                    goto out_peg_186;
                                }
                            }
                                
                        }
                        goto success_peg_182;
                        out_peg_186:
                        {
                            int position_peg_187 = result_peg_181.getPosition();
                            
                            result_peg_181.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_181.getPosition()) == (unsigned char) 255){
                                result_peg_181.nextPosition();
                            } else {
                                result_peg_181.setPosition(position_peg_187);
                                goto out_peg_188;
                            }
                            
                        }
                        goto success_peg_182;
                        out_peg_188:
                        goto loop_peg_180;
                        success_peg_182:
                        ;
                        result_peg_148.addResult(result_peg_181);
                    } while (true);
                    loop_peg_180:
                    ;
                            
                }
                goto success_peg_178;
                goto out_peg_150;
                success_peg_178:
                ;
            
            
            
            result_peg_148.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
            
            
            
            {
                    int position_peg_192 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_194(result_peg_148.getPosition());
                        {
                            int position_peg_196 = result_peg_194.getPosition();
                            
                            result_peg_194.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_194.getPosition()))){
                                    result_peg_194.nextPosition();
                                } else {
                                    result_peg_194.setPosition(position_peg_196);
                                    goto out_peg_197;
                                }
                            }
                                
                        }
                        goto success_peg_195;
                        out_peg_197:
                        {
                            int position_peg_198 = result_peg_194.getPosition();
                            
                            result_peg_194.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_194.getPosition()))){
                                    result_peg_194.nextPosition();
                                } else {
                                    result_peg_194.setPosition(position_peg_198);
                                    goto out_peg_199;
                                }
                            }
                                
                        }
                        goto success_peg_195;
                        out_peg_199:
                        {
                            int position_peg_200 = result_peg_194.getPosition();
                            
                            result_peg_194.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_194.getPosition()) == (unsigned char) 255){
                                result_peg_194.nextPosition();
                            } else {
                                result_peg_194.setPosition(position_peg_200);
                                goto out_peg_201;
                            }
                            
                        }
                        goto success_peg_195;
                        out_peg_201:
                        goto loop_peg_193;
                        success_peg_195:
                        ;
                        result_peg_148.addResult(result_peg_194);
                    } while (true);
                    loop_peg_193:
                    ;
                            
                }
                goto success_peg_191;
                goto out_peg_150;
                success_peg_191:
                ;
            
            
            
            result_peg_148.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
            
            
            
            {
                    int position_peg_205 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_207(result_peg_148.getPosition());
                        {
                            int position_peg_209 = result_peg_207.getPosition();
                            
                            result_peg_207.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_207.getPosition()))){
                                    result_peg_207.nextPosition();
                                } else {
                                    result_peg_207.setPosition(position_peg_209);
                                    goto out_peg_210;
                                }
                            }
                                
                        }
                        goto success_peg_208;
                        out_peg_210:
                        {
                            int position_peg_211 = result_peg_207.getPosition();
                            
                            result_peg_207.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_207.getPosition()))){
                                    result_peg_207.nextPosition();
                                } else {
                                    result_peg_207.setPosition(position_peg_211);
                                    goto out_peg_212;
                                }
                            }
                                
                        }
                        goto success_peg_208;
                        out_peg_212:
                        {
                            int position_peg_213 = result_peg_207.getPosition();
                            
                            result_peg_207.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_207.getPosition()) == (unsigned char) 255){
                                result_peg_207.nextPosition();
                            } else {
                                result_peg_207.setPosition(position_peg_213);
                                goto out_peg_214;
                            }
                            
                        }
                        goto success_peg_208;
                        out_peg_214:
                        goto loop_peg_206;
                        success_peg_208:
                        ;
                        result_peg_148.addResult(result_peg_207);
                    } while (true);
                    loop_peg_206:
                    ;
                            
                }
                goto success_peg_204;
                goto out_peg_150;
                success_peg_204:
                ;
            
            
            
            result_peg_148 = rule_integer(stream, result_peg_148.getPosition());
                if (result_peg_148.error()){
                    goto out_peg_150;
                }
                n1 = result_peg_148.getValues();
            
            
            
            {
                    int position_peg_218 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_220(result_peg_148.getPosition());
                        {
                            int position_peg_222 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    result_peg_220.setPosition(position_peg_222);
                                    goto out_peg_223;
                                }
                            }
                                
                        }
                        goto success_peg_221;
                        out_peg_223:
                        {
                            int position_peg_224 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_220.getPosition()))){
                                    result_peg_220.nextPosition();
                                } else {
                                    result_peg_220.setPosition(position_peg_224);
                                    goto out_peg_225;
                                }
                            }
                                
                        }
                        goto success_peg_221;
                        out_peg_225:
                        {
                            int position_peg_226 = result_peg_220.getPosition();
                            
                            result_peg_220.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_220.getPosition()) == (unsigned char) 255){
                                result_peg_220.nextPosition();
                            } else {
                                result_peg_220.setPosition(position_peg_226);
                                goto out_peg_227;
                            }
                            
                        }
                        goto success_peg_221;
                        out_peg_227:
                        goto loop_peg_219;
                        success_peg_221:
                        ;
                        result_peg_148.addResult(result_peg_220);
                    } while (true);
                    loop_peg_219:
                    ;
                            
                }
                goto success_peg_217;
                goto out_peg_150;
                success_peg_217:
                ;
            
            
            
            result_peg_148.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
            
            
            
            {
                    int position_peg_231 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_233(result_peg_148.getPosition());
                        {
                            int position_peg_235 = result_peg_233.getPosition();
                            
                            result_peg_233.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_233.getPosition()))){
                                    result_peg_233.nextPosition();
                                } else {
                                    result_peg_233.setPosition(position_peg_235);
                                    goto out_peg_236;
                                }
                            }
                                
                        }
                        goto success_peg_234;
                        out_peg_236:
                        {
                            int position_peg_237 = result_peg_233.getPosition();
                            
                            result_peg_233.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_233.getPosition()))){
                                    result_peg_233.nextPosition();
                                } else {
                                    result_peg_233.setPosition(position_peg_237);
                                    goto out_peg_238;
                                }
                            }
                                
                        }
                        goto success_peg_234;
                        out_peg_238:
                        {
                            int position_peg_239 = result_peg_233.getPosition();
                            
                            result_peg_233.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_233.getPosition()) == (unsigned char) 255){
                                result_peg_233.nextPosition();
                            } else {
                                result_peg_233.setPosition(position_peg_239);
                                goto out_peg_240;
                            }
                            
                        }
                        goto success_peg_234;
                        out_peg_240:
                        goto loop_peg_232;
                        success_peg_234:
                        ;
                        result_peg_148.addResult(result_peg_233);
                    } while (true);
                    loop_peg_232:
                    ;
                            
                }
                goto success_peg_230;
                goto out_peg_150;
                success_peg_230:
                ;
            
            
            
            result_peg_148 = rule_integer(stream, result_peg_148.getPosition());
                if (result_peg_148.error()){
                    goto out_peg_150;
                }
                n2 = result_peg_148.getValues();
            
            
            
            {
                    int position_peg_244 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_246(result_peg_148.getPosition());
                        {
                            int position_peg_248 = result_peg_246.getPosition();
                            
                            result_peg_246.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_246.getPosition()))){
                                    result_peg_246.nextPosition();
                                } else {
                                    result_peg_246.setPosition(position_peg_248);
                                    goto out_peg_249;
                                }
                            }
                                
                        }
                        goto success_peg_247;
                        out_peg_249:
                        {
                            int position_peg_250 = result_peg_246.getPosition();
                            
                            result_peg_246.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_246.getPosition()))){
                                    result_peg_246.nextPosition();
                                } else {
                                    result_peg_246.setPosition(position_peg_250);
                                    goto out_peg_251;
                                }
                            }
                                
                        }
                        goto success_peg_247;
                        out_peg_251:
                        {
                            int position_peg_252 = result_peg_246.getPosition();
                            
                            result_peg_246.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_246.getPosition()) == (unsigned char) 255){
                                result_peg_246.nextPosition();
                            } else {
                                result_peg_246.setPosition(position_peg_252);
                                goto out_peg_253;
                            }
                            
                        }
                        goto success_peg_247;
                        out_peg_253:
                        goto loop_peg_245;
                        success_peg_247:
                        ;
                        result_peg_148.addResult(result_peg_246);
                    } while (true);
                    loop_peg_245:
                    ;
                            
                }
                goto success_peg_243;
                goto out_peg_150;
                success_peg_243:
                ;
            
            
            
            result_peg_148.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
            
            
            
            {
                    int position_peg_257 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_259(result_peg_148.getPosition());
                        {
                            int position_peg_261 = result_peg_259.getPosition();
                            
                            result_peg_259.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_259.getPosition()))){
                                    result_peg_259.nextPosition();
                                } else {
                                    result_peg_259.setPosition(position_peg_261);
                                    goto out_peg_262;
                                }
                            }
                                
                        }
                        goto success_peg_260;
                        out_peg_262:
                        {
                            int position_peg_263 = result_peg_259.getPosition();
                            
                            result_peg_259.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_259.getPosition()))){
                                    result_peg_259.nextPosition();
                                } else {
                                    result_peg_259.setPosition(position_peg_263);
                                    goto out_peg_264;
                                }
                            }
                                
                        }
                        goto success_peg_260;
                        out_peg_264:
                        {
                            int position_peg_265 = result_peg_259.getPosition();
                            
                            result_peg_259.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_259.getPosition()) == (unsigned char) 255){
                                result_peg_259.nextPosition();
                            } else {
                                result_peg_259.setPosition(position_peg_265);
                                goto out_peg_266;
                            }
                            
                        }
                        goto success_peg_260;
                        out_peg_266:
                        goto loop_peg_258;
                        success_peg_260:
                        ;
                        result_peg_148.addResult(result_peg_259);
                    } while (true);
                    loop_peg_258:
                    ;
                            
                }
                goto success_peg_256;
                goto out_peg_150;
                success_peg_256:
                ;
            
            
            
            result_peg_148 = rule_integer(stream, result_peg_148.getPosition());
                if (result_peg_148.error()){
                    goto out_peg_150;
                }
                n3 = result_peg_148.getValues();
            
            
            
            {
                    int position_peg_270 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_272(result_peg_148.getPosition());
                        {
                            int position_peg_274 = result_peg_272.getPosition();
                            
                            result_peg_272.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_272.getPosition()))){
                                    result_peg_272.nextPosition();
                                } else {
                                    result_peg_272.setPosition(position_peg_274);
                                    goto out_peg_275;
                                }
                            }
                                
                        }
                        goto success_peg_273;
                        out_peg_275:
                        {
                            int position_peg_276 = result_peg_272.getPosition();
                            
                            result_peg_272.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_272.getPosition()))){
                                    result_peg_272.nextPosition();
                                } else {
                                    result_peg_272.setPosition(position_peg_276);
                                    goto out_peg_277;
                                }
                            }
                                
                        }
                        goto success_peg_273;
                        out_peg_277:
                        {
                            int position_peg_278 = result_peg_272.getPosition();
                            
                            result_peg_272.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_272.getPosition()) == (unsigned char) 255){
                                result_peg_272.nextPosition();
                            } else {
                                result_peg_272.setPosition(position_peg_278);
                                goto out_peg_279;
                            }
                            
                        }
                        goto success_peg_273;
                        out_peg_279:
                        goto loop_peg_271;
                        success_peg_273:
                        ;
                        result_peg_148.addResult(result_peg_272);
                    } while (true);
                    loop_peg_271:
                    ;
                            
                }
                goto success_peg_269;
                goto out_peg_150;
                success_peg_269:
                ;
            
            
            
            result_peg_148.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_148.getPosition()))){
                        result_peg_148.nextPosition();
                    } else {
                        goto out_peg_150;
                    }
                }
            
            
            
            {
                    int position_peg_283 = result_peg_148.getPosition();
                    
                    result_peg_148.reset();
                    do{
                        Result result_peg_285(result_peg_148.getPosition());
                        {
                            int position_peg_287 = result_peg_285.getPosition();
                            
                            result_peg_285.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_285.getPosition()))){
                                    result_peg_285.nextPosition();
                                } else {
                                    result_peg_285.setPosition(position_peg_287);
                                    goto out_peg_288;
                                }
                            }
                                
                        }
                        goto success_peg_286;
                        out_peg_288:
                        {
                            int position_peg_289 = result_peg_285.getPosition();
                            
                            result_peg_285.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_285.getPosition()))){
                                    result_peg_285.nextPosition();
                                } else {
                                    result_peg_285.setPosition(position_peg_289);
                                    goto out_peg_290;
                                }
                            }
                                
                        }
                        goto success_peg_286;
                        out_peg_290:
                        {
                            int position_peg_291 = result_peg_285.getPosition();
                            
                            result_peg_285.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_285.getPosition()) == (unsigned char) 255){
                                result_peg_285.nextPosition();
                            } else {
                                result_peg_285.setPosition(position_peg_291);
                                goto out_peg_292;
                            }
                            
                        }
                        goto success_peg_286;
                        out_peg_292:
                        goto loop_peg_284;
                        success_peg_286:
                        ;
                        result_peg_148.addResult(result_peg_285);
                    } while (true);
                    loop_peg_284:
                    ;
                            
                }
                goto success_peg_282;
                goto out_peg_150;
                success_peg_282:
                ;
            
            
            
            result_peg_148 = rule_integer(stream, result_peg_148.getPosition());
                if (result_peg_148.error()){
                    goto out_peg_150;
                }
                n4 = result_peg_148.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_148.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision = result_peg_148;
        stream.update(result_peg_148.getPosition());
        
        
        return result_peg_148;
        out_peg_150:
        Result result_peg_294(myposition);
        
        
        {
        
            result_peg_294.setValue(Value((void*) "Clsn"));
                for (int i = 0; i < 4; i++){
                    if (compareCharCase("Clsn"[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
                name = result_peg_294.getValues();
            
            
            
            {
                    int position_peg_299 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_301(result_peg_294.getPosition());
                        {
                            int position_peg_303 = result_peg_301.getPosition();
                            
                            result_peg_301.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_301.getPosition()))){
                                    result_peg_301.nextPosition();
                                } else {
                                    result_peg_301.setPosition(position_peg_303);
                                    goto out_peg_304;
                                }
                            }
                                
                        }
                        goto success_peg_302;
                        out_peg_304:
                        {
                            int position_peg_305 = result_peg_301.getPosition();
                            
                            result_peg_301.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_301.getPosition()))){
                                    result_peg_301.nextPosition();
                                } else {
                                    result_peg_301.setPosition(position_peg_305);
                                    goto out_peg_306;
                                }
                            }
                                
                        }
                        goto success_peg_302;
                        out_peg_306:
                        {
                            int position_peg_307 = result_peg_301.getPosition();
                            
                            result_peg_301.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_301.getPosition()) == (unsigned char) 255){
                                result_peg_301.nextPosition();
                            } else {
                                result_peg_301.setPosition(position_peg_307);
                                goto out_peg_308;
                            }
                            
                        }
                        goto success_peg_302;
                        out_peg_308:
                        goto loop_peg_300;
                        success_peg_302:
                        ;
                        result_peg_294.addResult(result_peg_301);
                    } while (true);
                    loop_peg_300:
                    ;
                            
                }
                goto success_peg_298;
                goto out_peg_296;
                success_peg_298:
                ;
            
            
            
            result_peg_294.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
            
            
            
            {
                    int position_peg_312 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_314(result_peg_294.getPosition());
                        {
                            int position_peg_316 = result_peg_314.getPosition();
                            
                            result_peg_314.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_314.getPosition()))){
                                    result_peg_314.nextPosition();
                                } else {
                                    result_peg_314.setPosition(position_peg_316);
                                    goto out_peg_317;
                                }
                            }
                                
                        }
                        goto success_peg_315;
                        out_peg_317:
                        {
                            int position_peg_318 = result_peg_314.getPosition();
                            
                            result_peg_314.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_314.getPosition()))){
                                    result_peg_314.nextPosition();
                                } else {
                                    result_peg_314.setPosition(position_peg_318);
                                    goto out_peg_319;
                                }
                            }
                                
                        }
                        goto success_peg_315;
                        out_peg_319:
                        {
                            int position_peg_320 = result_peg_314.getPosition();
                            
                            result_peg_314.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_314.getPosition()) == (unsigned char) 255){
                                result_peg_314.nextPosition();
                            } else {
                                result_peg_314.setPosition(position_peg_320);
                                goto out_peg_321;
                            }
                            
                        }
                        goto success_peg_315;
                        out_peg_321:
                        goto loop_peg_313;
                        success_peg_315:
                        ;
                        result_peg_294.addResult(result_peg_314);
                    } while (true);
                    loop_peg_313:
                    ;
                            
                }
                goto success_peg_311;
                goto out_peg_296;
                success_peg_311:
                ;
            
            
            
            result_peg_294 = rule_integer(stream, result_peg_294.getPosition());
                if (result_peg_294.error()){
                    goto out_peg_296;
                }
                index = result_peg_294.getValues();
            
            
            
            {
                    int position_peg_325 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_327(result_peg_294.getPosition());
                        {
                            int position_peg_329 = result_peg_327.getPosition();
                            
                            result_peg_327.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_327.getPosition()))){
                                    result_peg_327.nextPosition();
                                } else {
                                    result_peg_327.setPosition(position_peg_329);
                                    goto out_peg_330;
                                }
                            }
                                
                        }
                        goto success_peg_328;
                        out_peg_330:
                        {
                            int position_peg_331 = result_peg_327.getPosition();
                            
                            result_peg_327.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_327.getPosition()))){
                                    result_peg_327.nextPosition();
                                } else {
                                    result_peg_327.setPosition(position_peg_331);
                                    goto out_peg_332;
                                }
                            }
                                
                        }
                        goto success_peg_328;
                        out_peg_332:
                        {
                            int position_peg_333 = result_peg_327.getPosition();
                            
                            result_peg_327.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_327.getPosition()) == (unsigned char) 255){
                                result_peg_327.nextPosition();
                            } else {
                                result_peg_327.setPosition(position_peg_333);
                                goto out_peg_334;
                            }
                            
                        }
                        goto success_peg_328;
                        out_peg_334:
                        goto loop_peg_326;
                        success_peg_328:
                        ;
                        result_peg_294.addResult(result_peg_327);
                    } while (true);
                    loop_peg_326:
                    ;
                            
                }
                goto success_peg_324;
                goto out_peg_296;
                success_peg_324:
                ;
            
            
            
            result_peg_294.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
            
            
            
            {
                    int position_peg_338 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_340(result_peg_294.getPosition());
                        {
                            int position_peg_342 = result_peg_340.getPosition();
                            
                            result_peg_340.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_340.getPosition()))){
                                    result_peg_340.nextPosition();
                                } else {
                                    result_peg_340.setPosition(position_peg_342);
                                    goto out_peg_343;
                                }
                            }
                                
                        }
                        goto success_peg_341;
                        out_peg_343:
                        {
                            int position_peg_344 = result_peg_340.getPosition();
                            
                            result_peg_340.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_340.getPosition()))){
                                    result_peg_340.nextPosition();
                                } else {
                                    result_peg_340.setPosition(position_peg_344);
                                    goto out_peg_345;
                                }
                            }
                                
                        }
                        goto success_peg_341;
                        out_peg_345:
                        {
                            int position_peg_346 = result_peg_340.getPosition();
                            
                            result_peg_340.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_340.getPosition()) == (unsigned char) 255){
                                result_peg_340.nextPosition();
                            } else {
                                result_peg_340.setPosition(position_peg_346);
                                goto out_peg_347;
                            }
                            
                        }
                        goto success_peg_341;
                        out_peg_347:
                        goto loop_peg_339;
                        success_peg_341:
                        ;
                        result_peg_294.addResult(result_peg_340);
                    } while (true);
                    loop_peg_339:
                    ;
                            
                }
                goto success_peg_337;
                goto out_peg_296;
                success_peg_337:
                ;
            
            
            
            result_peg_294.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
            
            
            
            {
                    int position_peg_351 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_353(result_peg_294.getPosition());
                        {
                            int position_peg_355 = result_peg_353.getPosition();
                            
                            result_peg_353.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_353.getPosition()))){
                                    result_peg_353.nextPosition();
                                } else {
                                    result_peg_353.setPosition(position_peg_355);
                                    goto out_peg_356;
                                }
                            }
                                
                        }
                        goto success_peg_354;
                        out_peg_356:
                        {
                            int position_peg_357 = result_peg_353.getPosition();
                            
                            result_peg_353.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_353.getPosition()))){
                                    result_peg_353.nextPosition();
                                } else {
                                    result_peg_353.setPosition(position_peg_357);
                                    goto out_peg_358;
                                }
                            }
                                
                        }
                        goto success_peg_354;
                        out_peg_358:
                        {
                            int position_peg_359 = result_peg_353.getPosition();
                            
                            result_peg_353.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_353.getPosition()) == (unsigned char) 255){
                                result_peg_353.nextPosition();
                            } else {
                                result_peg_353.setPosition(position_peg_359);
                                goto out_peg_360;
                            }
                            
                        }
                        goto success_peg_354;
                        out_peg_360:
                        goto loop_peg_352;
                        success_peg_354:
                        ;
                        result_peg_294.addResult(result_peg_353);
                    } while (true);
                    loop_peg_352:
                    ;
                            
                }
                goto success_peg_350;
                goto out_peg_296;
                success_peg_350:
                ;
            
            
            
            result_peg_294 = rule_integer(stream, result_peg_294.getPosition());
                if (result_peg_294.error()){
                    goto out_peg_296;
                }
                n1 = result_peg_294.getValues();
            
            
            
            {
                    int position_peg_364 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_366(result_peg_294.getPosition());
                        {
                            int position_peg_368 = result_peg_366.getPosition();
                            
                            result_peg_366.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_366.getPosition()))){
                                    result_peg_366.nextPosition();
                                } else {
                                    result_peg_366.setPosition(position_peg_368);
                                    goto out_peg_369;
                                }
                            }
                                
                        }
                        goto success_peg_367;
                        out_peg_369:
                        {
                            int position_peg_370 = result_peg_366.getPosition();
                            
                            result_peg_366.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_366.getPosition()))){
                                    result_peg_366.nextPosition();
                                } else {
                                    result_peg_366.setPosition(position_peg_370);
                                    goto out_peg_371;
                                }
                            }
                                
                        }
                        goto success_peg_367;
                        out_peg_371:
                        {
                            int position_peg_372 = result_peg_366.getPosition();
                            
                            result_peg_366.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_366.getPosition()) == (unsigned char) 255){
                                result_peg_366.nextPosition();
                            } else {
                                result_peg_366.setPosition(position_peg_372);
                                goto out_peg_373;
                            }
                            
                        }
                        goto success_peg_367;
                        out_peg_373:
                        goto loop_peg_365;
                        success_peg_367:
                        ;
                        result_peg_294.addResult(result_peg_366);
                    } while (true);
                    loop_peg_365:
                    ;
                            
                }
                goto success_peg_363;
                goto out_peg_296;
                success_peg_363:
                ;
            
            
            
            result_peg_294.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
            
            
            
            {
                    int position_peg_377 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_379(result_peg_294.getPosition());
                        {
                            int position_peg_381 = result_peg_379.getPosition();
                            
                            result_peg_379.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_379.getPosition()))){
                                    result_peg_379.nextPosition();
                                } else {
                                    result_peg_379.setPosition(position_peg_381);
                                    goto out_peg_382;
                                }
                            }
                                
                        }
                        goto success_peg_380;
                        out_peg_382:
                        {
                            int position_peg_383 = result_peg_379.getPosition();
                            
                            result_peg_379.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_379.getPosition()))){
                                    result_peg_379.nextPosition();
                                } else {
                                    result_peg_379.setPosition(position_peg_383);
                                    goto out_peg_384;
                                }
                            }
                                
                        }
                        goto success_peg_380;
                        out_peg_384:
                        {
                            int position_peg_385 = result_peg_379.getPosition();
                            
                            result_peg_379.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_379.getPosition()) == (unsigned char) 255){
                                result_peg_379.nextPosition();
                            } else {
                                result_peg_379.setPosition(position_peg_385);
                                goto out_peg_386;
                            }
                            
                        }
                        goto success_peg_380;
                        out_peg_386:
                        goto loop_peg_378;
                        success_peg_380:
                        ;
                        result_peg_294.addResult(result_peg_379);
                    } while (true);
                    loop_peg_378:
                    ;
                            
                }
                goto success_peg_376;
                goto out_peg_296;
                success_peg_376:
                ;
            
            
            
            result_peg_294 = rule_integer(stream, result_peg_294.getPosition());
                if (result_peg_294.error()){
                    goto out_peg_296;
                }
                n2 = result_peg_294.getValues();
            
            
            
            {
                    int position_peg_390 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_392(result_peg_294.getPosition());
                        {
                            int position_peg_394 = result_peg_392.getPosition();
                            
                            result_peg_392.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_392.getPosition()))){
                                    result_peg_392.nextPosition();
                                } else {
                                    result_peg_392.setPosition(position_peg_394);
                                    goto out_peg_395;
                                }
                            }
                                
                        }
                        goto success_peg_393;
                        out_peg_395:
                        {
                            int position_peg_396 = result_peg_392.getPosition();
                            
                            result_peg_392.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_392.getPosition()))){
                                    result_peg_392.nextPosition();
                                } else {
                                    result_peg_392.setPosition(position_peg_396);
                                    goto out_peg_397;
                                }
                            }
                                
                        }
                        goto success_peg_393;
                        out_peg_397:
                        {
                            int position_peg_398 = result_peg_392.getPosition();
                            
                            result_peg_392.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_392.getPosition()) == (unsigned char) 255){
                                result_peg_392.nextPosition();
                            } else {
                                result_peg_392.setPosition(position_peg_398);
                                goto out_peg_399;
                            }
                            
                        }
                        goto success_peg_393;
                        out_peg_399:
                        goto loop_peg_391;
                        success_peg_393:
                        ;
                        result_peg_294.addResult(result_peg_392);
                    } while (true);
                    loop_peg_391:
                    ;
                            
                }
                goto success_peg_389;
                goto out_peg_296;
                success_peg_389:
                ;
            
            
            
            result_peg_294.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
            
            
            
            {
                    int position_peg_403 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_405(result_peg_294.getPosition());
                        {
                            int position_peg_407 = result_peg_405.getPosition();
                            
                            result_peg_405.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_405.getPosition()))){
                                    result_peg_405.nextPosition();
                                } else {
                                    result_peg_405.setPosition(position_peg_407);
                                    goto out_peg_408;
                                }
                            }
                                
                        }
                        goto success_peg_406;
                        out_peg_408:
                        {
                            int position_peg_409 = result_peg_405.getPosition();
                            
                            result_peg_405.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_405.getPosition()))){
                                    result_peg_405.nextPosition();
                                } else {
                                    result_peg_405.setPosition(position_peg_409);
                                    goto out_peg_410;
                                }
                            }
                                
                        }
                        goto success_peg_406;
                        out_peg_410:
                        {
                            int position_peg_411 = result_peg_405.getPosition();
                            
                            result_peg_405.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_405.getPosition()) == (unsigned char) 255){
                                result_peg_405.nextPosition();
                            } else {
                                result_peg_405.setPosition(position_peg_411);
                                goto out_peg_412;
                            }
                            
                        }
                        goto success_peg_406;
                        out_peg_412:
                        goto loop_peg_404;
                        success_peg_406:
                        ;
                        result_peg_294.addResult(result_peg_405);
                    } while (true);
                    loop_peg_404:
                    ;
                            
                }
                goto success_peg_402;
                goto out_peg_296;
                success_peg_402:
                ;
            
            
            
            result_peg_294 = rule_integer(stream, result_peg_294.getPosition());
                if (result_peg_294.error()){
                    goto out_peg_296;
                }
                n3 = result_peg_294.getValues();
            
            
            
            {
                    int position_peg_416 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_418(result_peg_294.getPosition());
                        {
                            int position_peg_420 = result_peg_418.getPosition();
                            
                            result_peg_418.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_418.getPosition()))){
                                    result_peg_418.nextPosition();
                                } else {
                                    result_peg_418.setPosition(position_peg_420);
                                    goto out_peg_421;
                                }
                            }
                                
                        }
                        goto success_peg_419;
                        out_peg_421:
                        {
                            int position_peg_422 = result_peg_418.getPosition();
                            
                            result_peg_418.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_418.getPosition()))){
                                    result_peg_418.nextPosition();
                                } else {
                                    result_peg_418.setPosition(position_peg_422);
                                    goto out_peg_423;
                                }
                            }
                                
                        }
                        goto success_peg_419;
                        out_peg_423:
                        {
                            int position_peg_424 = result_peg_418.getPosition();
                            
                            result_peg_418.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_418.getPosition()) == (unsigned char) 255){
                                result_peg_418.nextPosition();
                            } else {
                                result_peg_418.setPosition(position_peg_424);
                                goto out_peg_425;
                            }
                            
                        }
                        goto success_peg_419;
                        out_peg_425:
                        goto loop_peg_417;
                        success_peg_419:
                        ;
                        result_peg_294.addResult(result_peg_418);
                    } while (true);
                    loop_peg_417:
                    ;
                            
                }
                goto success_peg_415;
                goto out_peg_296;
                success_peg_415:
                ;
            
            
            
            result_peg_294.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_294.getPosition()))){
                        result_peg_294.nextPosition();
                    } else {
                        goto out_peg_296;
                    }
                }
            
            
            
            {
                    int position_peg_429 = result_peg_294.getPosition();
                    
                    result_peg_294.reset();
                    do{
                        Result result_peg_431(result_peg_294.getPosition());
                        {
                            int position_peg_433 = result_peg_431.getPosition();
                            
                            result_peg_431.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_431.getPosition()))){
                                    result_peg_431.nextPosition();
                                } else {
                                    result_peg_431.setPosition(position_peg_433);
                                    goto out_peg_434;
                                }
                            }
                                
                        }
                        goto success_peg_432;
                        out_peg_434:
                        {
                            int position_peg_435 = result_peg_431.getPosition();
                            
                            result_peg_431.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_431.getPosition()))){
                                    result_peg_431.nextPosition();
                                } else {
                                    result_peg_431.setPosition(position_peg_435);
                                    goto out_peg_436;
                                }
                            }
                                
                        }
                        goto success_peg_432;
                        out_peg_436:
                        {
                            int position_peg_437 = result_peg_431.getPosition();
                            
                            result_peg_431.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_431.getPosition()) == (unsigned char) 255){
                                result_peg_431.nextPosition();
                            } else {
                                result_peg_431.setPosition(position_peg_437);
                                goto out_peg_438;
                            }
                            
                        }
                        goto success_peg_432;
                        out_peg_438:
                        goto loop_peg_430;
                        success_peg_432:
                        ;
                        result_peg_294.addResult(result_peg_431);
                    } while (true);
                    loop_peg_430:
                    ;
                            
                }
                goto success_peg_428;
                goto out_peg_296;
                success_peg_428:
                ;
            
            
            
            result_peg_294 = rule_integer(stream, result_peg_294.getPosition());
                if (result_peg_294.error()){
                    goto out_peg_296;
                }
                n4 = result_peg_294.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_294.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision = result_peg_294;
        stream.update(result_peg_294.getPosition());
        
        
        return result_peg_294;
        out_peg_296:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_action_start.calculated()){
        return column_peg_1.chunk2->chunk_action_start;
    }
    
    
    RuleTrace trace_peg_64(stream, "action_start");
    int myposition = position;
    
    
    Value begin;
        Value action;
        Value num;
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_7 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_9(result_peg_2.getPosition());
                        {
                            int position_peg_11 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_11);
                                    goto out_peg_12;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_12:
                        {
                            int position_peg_13 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    result_peg_9.setPosition(position_peg_13);
                                    goto out_peg_14;
                                }
                            }
                                
                        }
                        goto success_peg_10;
                        out_peg_14:
                        {
                            int position_peg_15 = result_peg_9.getPosition();
                            
                            result_peg_9.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_9.getPosition()) == (unsigned char) 255){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_15);
                                goto out_peg_16;
                            }
                            
                        }
                        goto success_peg_10;
                        out_peg_16:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_2.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "Begin"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Begin"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                begin = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_20 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_22(result_peg_2.getPosition());
                        {
                            int position_peg_24 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_24);
                                    goto out_peg_25;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_25:
                        {
                            int position_peg_26 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_26);
                                    goto out_peg_27;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_27:
                        {
                            int position_peg_28 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_22.getPosition()) == (unsigned char) 255){
                                result_peg_22.nextPosition();
                            } else {
                                result_peg_22.setPosition(position_peg_28);
                                goto out_peg_29;
                            }
                            
                        }
                        goto success_peg_23;
                        out_peg_29:
                        goto loop_peg_21;
                        success_peg_23:
                        ;
                        result_peg_2.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                            
                }
                goto success_peg_19;
                goto out_peg_4;
                success_peg_19:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "Action"));
                for (int i = 0; i < 6; i++){
                    if (compareCharCase("Action"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                action = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_33 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_35(result_peg_2.getPosition());
                        {
                            int position_peg_37 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_37);
                                    goto out_peg_38;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_38:
                        {
                            int position_peg_39 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_35.getPosition()))){
                                    result_peg_35.nextPosition();
                                } else {
                                    result_peg_35.setPosition(position_peg_39);
                                    goto out_peg_40;
                                }
                            }
                                
                        }
                        goto success_peg_36;
                        out_peg_40:
                        {
                            int position_peg_41 = result_peg_35.getPosition();
                            
                            result_peg_35.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_35.getPosition()) == (unsigned char) 255){
                                result_peg_35.nextPosition();
                            } else {
                                result_peg_35.setPosition(position_peg_41);
                                goto out_peg_42;
                            }
                            
                        }
                        goto success_peg_36;
                        out_peg_42:
                        goto loop_peg_34;
                        success_peg_36:
                        ;
                        result_peg_2.addResult(result_peg_35);
                    } while (true);
                    loop_peg_34:
                    ;
                            
                }
                goto success_peg_32;
                goto out_peg_4;
                success_peg_32:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                num = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_46 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_48(result_peg_2.getPosition());
                        {
                            int position_peg_50 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_51:
                        {
                            int position_peg_52 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_52);
                                    goto out_peg_53;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_53:
                        {
                            int position_peg_54 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_48.getPosition()) == (unsigned char) 255){
                                result_peg_48.nextPosition();
                            } else {
                                result_peg_48.setPosition(position_peg_54);
                                goto out_peg_55;
                            }
                            
                        }
                        goto success_peg_49;
                        out_peg_55:
                        goto loop_peg_47;
                        success_peg_49:
                        ;
                        result_peg_2.addResult(result_peg_48);
                    } while (true);
                    loop_peg_47:
                    ;
                            
                }
                goto success_peg_45;
                goto out_peg_4;
                success_peg_45:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_58(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_61(result_peg_58);
                            result_peg_61.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    goto not_peg_60;
                                }
                            }
                            goto loop_peg_57;
                            not_peg_60:
                            result_peg_58.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_62 = stream.get(result_peg_58.getPosition());
                            if (temp_peg_62 != '\0'){
                                result_peg_58.setValue(Value((void*) (intptr_t) temp_peg_62));
                                result_peg_58.nextPosition();
                            } else {
                                goto loop_peg_57;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_58);
                } while (true);
                loop_peg_57:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHeader(begin, action, num);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_action_start = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_action_start = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_integer.calculated()){
        return column_peg_1.chunk2->chunk_integer;
    }
    
    
    RuleTrace trace_peg_18(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_6);
                            goto out_peg_7;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    int position_peg_8 = result_peg_2.getPosition();
                    
                    result_peg_2.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_8);
                            goto out_peg_9;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_9:
                
                result_peg_2 = Result(save_peg_4);
                result_peg_2.setValue(Value((void*) 0));
                
                success_peg_5:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_12(result_peg_2.getPosition());
                    {
                        int position_peg_14 = result_peg_12.getPosition();
                        
                        char letter_peg_15 = stream.get(result_peg_12.getPosition());
                        if (letter_peg_15 != '\0' && strchr("0123456789", letter_peg_15) != NULL){
                            result_peg_12.nextPosition();
                            result_peg_12.setValue(Value((void*) (intptr_t) letter_peg_15));
                        } else {
                            result_peg_12.setPosition(position_peg_14);
                            goto out_peg_16;
                        }
                        
                    }
                    goto success_peg_13;
                    out_peg_16:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_2.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                if (result_peg_2.matches() == 0){
                    goto out_peg_17;
                }
            
            Result result_peg_10 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_10.getValues()); value = makeNumber(result_peg_3.getValues(),value);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_integer = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_17:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk2 != 0 && column_peg_1.chunk2->chunk_valuelist.calculated()){
        return column_peg_1.chunk2->chunk_valuelist;
    }
    
    
    RuleTrace trace_peg_34(stream, "valuelist");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2 = rule_value(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        {
                                int position_peg_10 = result_peg_7.getPosition();
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_12(result_peg_7.getPosition());
                                    {
                                        int position_peg_14 = result_peg_12.getPosition();
                                        
                                        result_peg_12.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_12.getPosition()))){
                                                result_peg_12.nextPosition();
                                            } else {
                                                result_peg_12.setPosition(position_peg_14);
                                                goto out_peg_15;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_13;
                                    out_peg_15:
                                    {
                                        int position_peg_16 = result_peg_12.getPosition();
                                        
                                        result_peg_12.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_12.getPosition()))){
                                                result_peg_12.nextPosition();
                                            } else {
                                                result_peg_12.setPosition(position_peg_16);
                                                goto out_peg_17;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_13;
                                    out_peg_17:
                                    {
                                        int position_peg_18 = result_peg_12.getPosition();
                                        
                                        result_peg_12.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_12.getPosition()) == (unsigned char) 255){
                                            result_peg_12.nextPosition();
                                        } else {
                                            result_peg_12.setPosition(position_peg_18);
                                            goto out_peg_19;
                                        }
                                        
                                    }
                                    goto success_peg_13;
                                    out_peg_19:
                                    goto loop_peg_11;
                                    success_peg_13:
                                    ;
                                    result_peg_7.addResult(result_peg_12);
                                } while (true);
                                loop_peg_11:
                                ;
                                        
                            }
                            goto success_peg_9;
                            goto loop_peg_6;
                            success_peg_9:
                            ;
                        
                        
                        
                        result_peg_7.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_23 = result_peg_7.getPosition();
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_25(result_peg_7.getPosition());
                                    {
                                        int position_peg_27 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_25.getPosition()))){
                                                result_peg_25.nextPosition();
                                            } else {
                                                result_peg_25.setPosition(position_peg_27);
                                                goto out_peg_28;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_26;
                                    out_peg_28:
                                    {
                                        int position_peg_29 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_25.getPosition()))){
                                                result_peg_25.nextPosition();
                                            } else {
                                                result_peg_25.setPosition(position_peg_29);
                                                goto out_peg_30;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_26;
                                    out_peg_30:
                                    {
                                        int position_peg_31 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_25.getPosition()) == (unsigned char) 255){
                                            result_peg_25.nextPosition();
                                        } else {
                                            result_peg_25.setPosition(position_peg_31);
                                            goto out_peg_32;
                                        }
                                        
                                    }
                                    goto success_peg_26;
                                    out_peg_32:
                                    goto loop_peg_24;
                                    success_peg_26:
                                    ;
                                    result_peg_7.addResult(result_peg_25);
                                } while (true);
                                loop_peg_24:
                                ;
                                        
                            }
                            goto success_peg_22;
                            goto loop_peg_6;
                            success_peg_22:
                            ;
                        
                        
                        
                        int save_peg_33 = result_peg_7.getPosition();
                            
                            result_peg_7 = rule_value(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                
                                result_peg_7 = Result(save_peg_33);
                                result_peg_7.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
            
            Result result_peg_5 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeValueList(result_peg_3.getValues(),result_peg_5.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_valuelist = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk3 != 0 && column_peg_1.chunk3->chunk_value.calculated()){
        return column_peg_1.chunk3->chunk_value;
    }
    
    
    RuleTrace trace_peg_36(stream, "value");
    int myposition = position;
    
    
    Value source;
        Value dest;
    Result result_peg_2(myposition);
        
        
        result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        
        {
        
            result_peg_4.setValue(Value((void*) "as"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("as"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        goto out_peg_6;
                    }
                }
            
            
            
            result_peg_4 = rule_integer(stream, result_peg_4.getPosition());
                if (result_peg_4.error()){
                    goto out_peg_6;
                }
                source = result_peg_4.getValues();
            
            
            
            result_peg_4.setValue(Value((void*) "d"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("d"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        goto out_peg_6;
                    }
                }
            
            
            
            result_peg_4 = rule_integer(stream, result_peg_4.getPosition());
                if (result_peg_4.error()){
                    goto out_peg_6;
                }
                dest = result_peg_4.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAlphaSourceDest(source, dest);
                    result_peg_4.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_6:
        Result result_peg_10(myposition);
        
        
        {
        
            result_peg_10.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_10.getPosition()))){
                        result_peg_10.nextPosition();
                    } else {
                        goto out_peg_12;
                    }
                }
            
            
            
            result_peg_10 = rule_integer(stream, result_peg_10.getPosition());
                if (result_peg_10.error()){
                    goto out_peg_12;
                }
                source = result_peg_10.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAlpha("a", source);
                    result_peg_10.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_10;
        stream.update(result_peg_10.getPosition());
        
        
        return result_peg_10;
        out_peg_12:
        Result result_peg_14(myposition);
        
        
        {
        
            result_peg_14.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_14.getPosition()))){
                        result_peg_14.nextPosition();
                    } else {
                        goto out_peg_16;
                    }
                }
            
            Result result_peg_15 = result_peg_14;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_15.getValues());
                    result_peg_14.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_14;
        stream.update(result_peg_14.getPosition());
        
        
        return result_peg_14;
        out_peg_16:
        Result result_peg_17(myposition);
        
        
        {
        
            result_peg_17.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_17.getPosition()))){
                        result_peg_17.nextPosition();
                    } else {
                        goto out_peg_19;
                    }
                }
            
            
            
            result_peg_17 = rule_integer(stream, result_peg_17.getPosition());
                if (result_peg_17.error()){
                    goto out_peg_19;
                }
                source = result_peg_17.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAlpha("s", source);
                    result_peg_17.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_17;
        stream.update(result_peg_17.getPosition());
        
        
        return result_peg_17;
        out_peg_19:
        Result result_peg_21(myposition);
        
        
        {
        
            result_peg_21.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_21.getPosition()))){
                        result_peg_21.nextPosition();
                    } else {
                        goto out_peg_23;
                    }
                }
            
            Result result_peg_22 = result_peg_21;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_22.getValues());
                    result_peg_21.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_21;
        stream.update(result_peg_21.getPosition());
        
        
        return result_peg_21;
        out_peg_23:
        Result result_peg_24(myposition);
        
        
        {
        
            result_peg_24.setValue(Value((void*) "vh"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("vh"[i], stream.get(result_peg_24.getPosition()))){
                        result_peg_24.nextPosition();
                    } else {
                        goto out_peg_26;
                    }
                }
            
            Result result_peg_25 = result_peg_24;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_25.getValues());
                    result_peg_24.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_24;
        stream.update(result_peg_24.getPosition());
        
        
        return result_peg_24;
        out_peg_26:
        Result result_peg_27(myposition);
        
        
        {
        
            result_peg_27.setValue(Value((void*) "hv"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("hv"[i], stream.get(result_peg_27.getPosition()))){
                        result_peg_27.nextPosition();
                    } else {
                        goto out_peg_29;
                    }
                }
            
            Result result_peg_28 = result_peg_27;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_28.getValues());
                    result_peg_27.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_27;
        stream.update(result_peg_27.getPosition());
        
        
        return result_peg_27;
        out_peg_29:
        Result result_peg_30(myposition);
        
        
        {
        
            result_peg_30.setValue(Value((void*) "v"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("v"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_32;
                    }
                }
            
            Result result_peg_31 = result_peg_30;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_31.getValues());
                    result_peg_30.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_30;
        stream.update(result_peg_30.getPosition());
        
        
        return result_peg_30;
        out_peg_32:
        Result result_peg_33(myposition);
        
        
        {
        
            result_peg_33.setValue(Value((void*) "h"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
            
            Result result_peg_34 = result_peg_33;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_34.getValues());
                    result_peg_33.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_33;
        stream.update(result_peg_33.getPosition());
        
        
        return result_peg_33;
        out_peg_35:
    
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

static const void * doParse(Stream & stream, bool stats, const std::string & context){
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        stream.reportError(context);
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}

const void * parse(const std::string & filename, bool stats = false){
    Stream stream(filename);
    return doParse(stream, stats, filename);
}

const void * parse(const char * in, bool stats = false){
    Stream stream(in);
    return doParse(stream, stats, "memory");
}

const void * parse(const char * in, int length, bool stats = false){
    Stream stream(in, length);
    return doParse(stream, stats, "memory");
}



    
    } /* Air */
    
} /* Mugen */

    
