

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

    void push_rule(const std::string & name){
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

    void setName(const std::string & name){
        stream.pop_rule();
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
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_start.calculated()){
        return column_peg_2.chunk0->chunk_start;
    }
    
    
    RuleTrace trace_peg_1(stream, "start");
    int myposition = position;
    
    try{
        
    
    Value current;
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    Value value((void*) 0);
                    value = makeSectionList();
                    result_peg_3.setValue(value);
                }
                current = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_whitespace(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_6;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_9(result_peg_3.getPosition());
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
                    result_peg_3.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_17(result_peg_3.getPosition());
                    {
                    
                        result_peg_17 = rule_sn(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        result_peg_17 = rule_line(stream, result_peg_17.getPosition(), current);
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        result_peg_17 = rule_whitespace(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                goto loop_peg_16;
                            }
                        
                        
                        
                        int save_peg_21 = result_peg_17.getPosition();
                            
                            result_peg_17 = rule_line_end(stream, result_peg_17.getPosition());
                            if (result_peg_17.error()){
                                
                                result_peg_17 = Result(save_peg_21);
                                result_peg_17.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_17);
                } while (true);
                loop_peg_16:
                ;
            
            
            
            result_peg_3 = rule_sn(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_6;
                }
            
            
            
            if ('\0' == stream.get(result_peg_3.getPosition())){
                    result_peg_3.nextPosition();
                    result_peg_3.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_6;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = current; GC::cleanup(as<SectionList*>(value));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_start = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_6:
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_start = errorResult;
        stream.update(errorResult.getPosition());
        
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    
    RuleTrace trace_peg_1(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_8(result_peg_3.getPosition());
                        {
                            int position_peg_10 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_8.getPosition()))){
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
                            
                            result_peg_8.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_8.getPosition()) == (unsigned char) 255){
                                result_peg_8.nextPosition();
                            } else {
                                result_peg_8.setPosition(position_peg_14);
                                goto out_peg_15;
                            }
                            
                        }
                        goto success_peg_9;
                        out_peg_15:
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_16;
                success_peg_5:
                ;
            
            
            
            result_peg_3 = rule_comment(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_16;
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_16:
        Result result_peg_17(myposition);
        
        
        {
        
            {
                    int position_peg_20 = result_peg_17.getPosition();
                    
                    result_peg_17.reset();
                    do{
                        Result result_peg_22(result_peg_17.getPosition());
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
                        result_peg_17.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                            
                }
                goto success_peg_19;
                goto out_peg_30;
                success_peg_19:
                ;
            
            
            
            result_peg_17 = rule_action(stream, result_peg_17.getPosition());
                if (result_peg_17.error()){
                    goto out_peg_30;
                }
            
            Result result_peg_31 = result_peg_17;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_31.getValues());
                    result_peg_17.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_17.getPosition());
        
        return result_peg_17;
        out_peg_30:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_action.calculated()){
        return column_peg_2.chunk0->chunk_action;
    }
    
    
    RuleTrace trace_peg_1(stream, "action");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_action_start(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            Result result_peg_4 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_4.getValues());
                    result_peg_3.setValue(value);
                }
                ast = result_peg_3.getValues();
            
            
            
            result_peg_3 = rule_whitespace(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_10(result_peg_3.getPosition());
                    {
                        int position_peg_12 = result_peg_10.getPosition();
                        
                        result_peg_10.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_10.getPosition()))){
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
                        
                        result_peg_10.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_10.getPosition()))){
                                result_peg_10.nextPosition();
                            } else {
                                result_peg_10.setPosition(position_peg_14);
                                goto out_peg_15;
                            }
                        }
                            
                    }
                    goto success_peg_11;
                    out_peg_15:
                    goto loop_peg_9;
                    success_peg_11:
                    ;
                    result_peg_3.addResult(result_peg_10);
                } while (true);
                loop_peg_9:
                if (result_peg_3.matches() == 0){
                    goto out_peg_5;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_18(result_peg_3.getPosition());
                    {
                    
                        result_peg_18 = rule_action_line(stream, result_peg_18.getPosition(), ast);
                            if (result_peg_18.error()){
                                goto loop_peg_17;
                            }
                        
                        
                        
                        result_peg_18 = rule_whitespace(stream, result_peg_18.getPosition());
                            if (result_peg_18.error()){
                                goto loop_peg_17;
                            }
                        
                        
                        
                        result_peg_18 = rule_line_end(stream, result_peg_18.getPosition());
                            if (result_peg_18.error()){
                                goto loop_peg_17;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_18);
                } while (true);
                loop_peg_17:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_action = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_action = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_action_line(Stream & stream, const int position, Value section){
    
    
    RuleTrace trace_peg_1(stream, "action_line");
    int myposition = position;
    
    
    Value data;
        Value line;
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_8(result_peg_3.getPosition());
                        {
                            int position_peg_10 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_8.getPosition()))){
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
                            
                            result_peg_8.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    result_peg_8.setPosition(position_peg_12);
                                    goto out_peg_13;
                                }
                            }
                                
                        }
                        goto success_peg_9;
                        out_peg_13:
                        {
                            int position_peg_14 = result_peg_8.getPosition();
                            
                            result_peg_8.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_8.getPosition()) == (unsigned char) 255){
                                result_peg_8.nextPosition();
                            } else {
                                result_peg_8.setPosition(position_peg_14);
                                goto out_peg_15;
                            }
                            
                        }
                        goto success_peg_9;
                        out_peg_15:
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_16;
                success_peg_5:
                ;
            
            
            
            result_peg_3 = rule_comment(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_16;
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_16:
        Result result_peg_17(myposition);
        
        
        {
        
            {
                    int position_peg_20 = result_peg_17.getPosition();
                    
                    result_peg_17.reset();
                    do{
                        Result result_peg_22(result_peg_17.getPosition());
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
                        result_peg_17.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                            
                }
                goto success_peg_19;
                goto out_peg_30;
                success_peg_19:
                ;
            
            
            
            result_peg_17 = rule_collision_default(stream, result_peg_17.getPosition());
                if (result_peg_17.error()){
                    goto out_peg_30;
                }
                data = result_peg_17.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_17.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_17.getPosition());
        
        return result_peg_17;
        out_peg_30:
        Result result_peg_32(myposition);
        
        
        {
        
            {
                    int position_peg_35 = result_peg_32.getPosition();
                    
                    result_peg_32.reset();
                    do{
                        Result result_peg_37(result_peg_32.getPosition());
                        {
                            int position_peg_39 = result_peg_37.getPosition();
                            
                            result_peg_37.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_37.getPosition()))){
                                    result_peg_37.nextPosition();
                                } else {
                                    result_peg_37.setPosition(position_peg_39);
                                    goto out_peg_40;
                                }
                            }
                                
                        }
                        goto success_peg_38;
                        out_peg_40:
                        {
                            int position_peg_41 = result_peg_37.getPosition();
                            
                            result_peg_37.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_37.getPosition()))){
                                    result_peg_37.nextPosition();
                                } else {
                                    result_peg_37.setPosition(position_peg_41);
                                    goto out_peg_42;
                                }
                            }
                                
                        }
                        goto success_peg_38;
                        out_peg_42:
                        {
                            int position_peg_43 = result_peg_37.getPosition();
                            
                            result_peg_37.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_37.getPosition()) == (unsigned char) 255){
                                result_peg_37.nextPosition();
                            } else {
                                result_peg_37.setPosition(position_peg_43);
                                goto out_peg_44;
                            }
                            
                        }
                        goto success_peg_38;
                        out_peg_44:
                        goto loop_peg_36;
                        success_peg_38:
                        ;
                        result_peg_32.addResult(result_peg_37);
                    } while (true);
                    loop_peg_36:
                    ;
                            
                }
                goto success_peg_34;
                goto out_peg_45;
                success_peg_34:
                ;
            
            
            
            result_peg_32 = rule_collision(stream, result_peg_32.getPosition());
                if (result_peg_32.error()){
                    goto out_peg_45;
                }
                data = result_peg_32.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_32.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_32.getPosition());
        
        return result_peg_32;
        out_peg_45:
        Result result_peg_47(myposition);
        
        
        {
        
            {
                    int position_peg_50 = result_peg_47.getPosition();
                    
                    result_peg_47.reset();
                    do{
                        Result result_peg_52(result_peg_47.getPosition());
                        {
                            int position_peg_54 = result_peg_52.getPosition();
                            
                            result_peg_52.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_52.getPosition()))){
                                    result_peg_52.nextPosition();
                                } else {
                                    result_peg_52.setPosition(position_peg_54);
                                    goto out_peg_55;
                                }
                            }
                                
                        }
                        goto success_peg_53;
                        out_peg_55:
                        {
                            int position_peg_56 = result_peg_52.getPosition();
                            
                            result_peg_52.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_52.getPosition()))){
                                    result_peg_52.nextPosition();
                                } else {
                                    result_peg_52.setPosition(position_peg_56);
                                    goto out_peg_57;
                                }
                            }
                                
                        }
                        goto success_peg_53;
                        out_peg_57:
                        {
                            int position_peg_58 = result_peg_52.getPosition();
                            
                            result_peg_52.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_52.getPosition()) == (unsigned char) 255){
                                result_peg_52.nextPosition();
                            } else {
                                result_peg_52.setPosition(position_peg_58);
                                goto out_peg_59;
                            }
                            
                        }
                        goto success_peg_53;
                        out_peg_59:
                        goto loop_peg_51;
                        success_peg_53:
                        ;
                        result_peg_47.addResult(result_peg_52);
                    } while (true);
                    loop_peg_51:
                    ;
                            
                }
                goto success_peg_49;
                goto out_peg_60;
                success_peg_49:
                ;
            
            
            
            result_peg_47 = rule_valuelist(stream, result_peg_47.getPosition());
                if (result_peg_47.error()){
                    goto out_peg_60;
                }
                data = result_peg_47.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                    result_peg_47.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_47.getPosition());
        
        return result_peg_47;
        out_peg_60:
        Result result_peg_62(myposition);
        
        
        {
        
            {
                    int position_peg_65 = result_peg_62.getPosition();
                    
                    result_peg_62.reset();
                    do{
                        Result result_peg_67(result_peg_62.getPosition());
                        {
                            int position_peg_69 = result_peg_67.getPosition();
                            
                            result_peg_67.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_67.getPosition()))){
                                    result_peg_67.nextPosition();
                                } else {
                                    result_peg_67.setPosition(position_peg_69);
                                    goto out_peg_70;
                                }
                            }
                                
                        }
                        goto success_peg_68;
                        out_peg_70:
                        {
                            int position_peg_71 = result_peg_67.getPosition();
                            
                            result_peg_67.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_67.getPosition()))){
                                    result_peg_67.nextPosition();
                                } else {
                                    result_peg_67.setPosition(position_peg_71);
                                    goto out_peg_72;
                                }
                            }
                                
                        }
                        goto success_peg_68;
                        out_peg_72:
                        {
                            int position_peg_73 = result_peg_67.getPosition();
                            
                            result_peg_67.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_67.getPosition()) == (unsigned char) 255){
                                result_peg_67.nextPosition();
                            } else {
                                result_peg_67.setPosition(position_peg_73);
                                goto out_peg_74;
                            }
                            
                        }
                        goto success_peg_68;
                        out_peg_74:
                        goto loop_peg_66;
                        success_peg_68:
                        ;
                        result_peg_62.addResult(result_peg_67);
                    } while (true);
                    loop_peg_66:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_75;
                success_peg_64:
                ;
            
            
            
            {
                    int position_peg_78 = result_peg_62.getPosition();
                    {
                    
                        result_peg_62.setValue(Value((void*) "loopstart"));
                            for (int i = 0; i < 9; i++){
                                if (compareCharCase("loopstart"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_78);
                                    goto out_peg_80;
                                }
                            }
                        
                        Result result_peg_79 = result_peg_62;
                        
                        int save_peg_82 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) ":"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(":"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    
                                    result_peg_62 = Result(save_peg_82);
                                    result_peg_62.setValue(Value((void*) 0));
                                    
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = makeAttributeKeyword(Value(makeKeyword(result_peg_79.getValues())));
                                result_peg_62.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_77;
                out_peg_80:
                goto out_peg_75;
                success_peg_77:
                ;
                data = result_peg_62.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_62.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_62.getPosition());
        
        return result_peg_62;
        out_peg_75:
        Result result_peg_83(myposition);
        
        
        {
        
            {
                    int position_peg_86 = result_peg_83.getPosition();
                    
                    result_peg_83.setValue(Value((void*) " "));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_83.getPosition()))){
                            result_peg_83.nextPosition();
                        } else {
                            result_peg_83.setPosition(position_peg_86);
                            goto out_peg_87;
                        }
                    }
                        
                }
                goto success_peg_85;
                out_peg_87:
                {
                    int position_peg_88 = result_peg_83.getPosition();
                    
                    result_peg_83.setValue(Value((void*) "\t"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_83.getPosition()))){
                            result_peg_83.nextPosition();
                        } else {
                            result_peg_83.setPosition(position_peg_88);
                            goto out_peg_89;
                        }
                    }
                        
                }
                goto success_peg_85;
                out_peg_89:
                {
                    int position_peg_90 = result_peg_83.getPosition();
                    
                    result_peg_83.setValue(Value((void*) 255));
                    if ((unsigned char) stream.get(result_peg_83.getPosition()) == (unsigned char) 255){
                        result_peg_83.nextPosition();
                    } else {
                        result_peg_83.setPosition(position_peg_90);
                        goto out_peg_91;
                    }
                    
                }
                goto success_peg_85;
                out_peg_91:
                goto out_peg_92;
                success_peg_85:
                ;
            
            
            
            {
                    int position_peg_94 = result_peg_83.getPosition();
                    
                    result_peg_83.reset();
                    do{
                        Result result_peg_96(result_peg_83.getPosition());
                        {
                            int position_peg_98 = result_peg_96.getPosition();
                            
                            result_peg_96.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    result_peg_96.setPosition(position_peg_98);
                                    goto out_peg_99;
                                }
                            }
                                
                        }
                        goto success_peg_97;
                        out_peg_99:
                        {
                            int position_peg_100 = result_peg_96.getPosition();
                            
                            result_peg_96.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    result_peg_96.setPosition(position_peg_100);
                                    goto out_peg_101;
                                }
                            }
                                
                        }
                        goto success_peg_97;
                        out_peg_101:
                        {
                            int position_peg_102 = result_peg_96.getPosition();
                            
                            result_peg_96.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_96.getPosition()) == (unsigned char) 255){
                                result_peg_96.nextPosition();
                            } else {
                                result_peg_96.setPosition(position_peg_102);
                                goto out_peg_103;
                            }
                            
                        }
                        goto success_peg_97;
                        out_peg_103:
                        goto loop_peg_95;
                        success_peg_97:
                        ;
                        result_peg_83.addResult(result_peg_96);
                    } while (true);
                    loop_peg_95:
                    ;
                            
                }
                goto success_peg_93;
                goto out_peg_92;
                success_peg_93:
                ;
            
            
        }
        
        stream.update(result_peg_83.getPosition());
        
        return result_peg_83;
        out_peg_92:
        Result result_peg_104(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_106 = stream.getLineInfo(result_peg_104.getPosition());
                line = &line_info_peg_106;
            
            
            
            Result result_peg_109(result_peg_104);
                {
                
                    {
                            int position_peg_112 = result_peg_109.getPosition();
                            
                            result_peg_109.reset();
                            do{
                                Result result_peg_114(result_peg_109.getPosition());
                                {
                                    int position_peg_116 = result_peg_114.getPosition();
                                    
                                    result_peg_114.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_114.getPosition()))){
                                            result_peg_114.nextPosition();
                                        } else {
                                            result_peg_114.setPosition(position_peg_116);
                                            goto out_peg_117;
                                        }
                                    }
                                        
                                }
                                goto success_peg_115;
                                out_peg_117:
                                {
                                    int position_peg_118 = result_peg_114.getPosition();
                                    
                                    result_peg_114.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_114.getPosition()))){
                                            result_peg_114.nextPosition();
                                        } else {
                                            result_peg_114.setPosition(position_peg_118);
                                            goto out_peg_119;
                                        }
                                    }
                                        
                                }
                                goto success_peg_115;
                                out_peg_119:
                                {
                                    int position_peg_120 = result_peg_114.getPosition();
                                    
                                    result_peg_114.setValue(Value((void*) 255));
                                    if ((unsigned char) stream.get(result_peg_114.getPosition()) == (unsigned char) 255){
                                        result_peg_114.nextPosition();
                                    } else {
                                        result_peg_114.setPosition(position_peg_120);
                                        goto out_peg_121;
                                    }
                                    
                                }
                                goto success_peg_115;
                                out_peg_121:
                                goto loop_peg_113;
                                success_peg_115:
                                ;
                                result_peg_109.addResult(result_peg_114);
                            } while (true);
                            loop_peg_113:
                            ;
                                    
                        }
                        goto success_peg_111;
                        goto not_peg_108;
                        success_peg_111:
                        ;
                    
                    
                    
                    result_peg_109 = rule_action_start(stream, result_peg_109.getPosition());
                        if (result_peg_109.error()){
                            goto not_peg_108;
                        }
                    
                    
                }
                goto out_peg_122;
                not_peg_108:
                result_peg_104.setValue(Value((void*)0));
            
            
            
            result_peg_104.reset();
                do{
                    Result result_peg_125(result_peg_104.getPosition());
                    {
                    
                        Result result_peg_128(result_peg_125);
                            result_peg_128 = rule_line_end(stream, result_peg_128.getPosition());
                            if (result_peg_128.error()){
                                goto not_peg_127;
                            }
                            goto loop_peg_124;
                            not_peg_127:
                            result_peg_125.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_129 = stream.get(result_peg_125.getPosition());
                            if (temp_peg_129 != '\0'){
                                result_peg_125.setValue(Value((void*) (intptr_t) temp_peg_129));
                                result_peg_125.nextPosition();
                            } else {
                                goto loop_peg_124;
                            }
                        
                        
                    }
                    result_peg_104.addResult(result_peg_125);
                } while (true);
                loop_peg_124:
                if (result_peg_104.matches() == 0){
                    goto out_peg_122;
                }
            
            
            
            Result result_peg_131(result_peg_104.getPosition());
                result_peg_131 = rule_line_end(stream, result_peg_131.getPosition());
                if (result_peg_131.error()){
                    goto out_peg_122;
                }
            
            
            
            {
                    Value value((void*) 0);
                    std::cout << "Warning: ignoring input at line " << getCurrentLine(line) << " column " << getCurrentColumn(line) << std::endl;
                    result_peg_104.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_104.getPosition());
        
        return result_peg_104;
        out_peg_122:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk0 != 0 && column_peg_2.chunk0->chunk_line_end.calculated()){
        return column_peg_2.chunk0->chunk_line_end;
    }
    
    
    RuleTrace trace_peg_1(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.reset();
        do{
            Result result_peg_5(result_peg_3.getPosition());
            {
                int position_peg_7 = result_peg_5.getPosition();
                
                result_peg_5.setValue(Value((void*) "\n"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        result_peg_5.setPosition(position_peg_7);
                        goto out_peg_8;
                    }
                }
                    
            }
            goto success_peg_6;
            out_peg_8:
            {
                int position_peg_9 = result_peg_5.getPosition();
                
                result_peg_5.setValue(Value((void*) "\r"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        result_peg_5.setPosition(position_peg_9);
                        goto out_peg_10;
                    }
                }
                    
            }
            goto success_peg_6;
            out_peg_10:
            goto loop_peg_4;
            success_peg_6:
            ;
            result_peg_3.addResult(result_peg_5);
        } while (true);
        loop_peg_4:
        if (result_peg_3.matches() == 0){
            goto out_peg_11;
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_line_end = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_11:
        Result result_peg_12(myposition);
        
        
        {
        
            Result result_peg_14(result_peg_12.getPosition());
                if ('\0' == stream.get(result_peg_14.getPosition())){
                    result_peg_14.nextPosition();
                    result_peg_14.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_15;
                }
            
            
            
            
            
            
        }
        
        
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_line_end = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_15:
    
        if (column_peg_2.chunk0 == 0){
            column_peg_2.chunk0 = new Chunk0();
        }
        column_peg_2.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_whitespace.calculated()){
        return column_peg_2.chunk1->chunk_whitespace;
    }
    
    
    RuleTrace trace_peg_1(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.reset();
        do{
            Result result_peg_5(result_peg_3.getPosition());
            result_peg_5 = rule_sw(stream, result_peg_5.getPosition());
            if (result_peg_5.error()){
                goto loop_peg_4;
            }
            result_peg_3.addResult(result_peg_5);
        } while (true);
        loop_peg_4:
        ;
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_whitespace = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_sn(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_sn.calculated()){
        return column_peg_2.chunk1->chunk_sn;
    }
    
    
    RuleTrace trace_peg_1(stream, "sn");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3.reset();
        do{
            Result result_peg_5(result_peg_3.getPosition());
            result_peg_5 = rule_space_newline(stream, result_peg_5.getPosition());
            if (result_peg_5.error()){
                goto loop_peg_4;
            }
            result_peg_3.addResult(result_peg_5);
        } while (true);
        loop_peg_4:
        ;
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_sn = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_sn = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_space_newline(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_space_newline.calculated()){
        return column_peg_2.chunk1->chunk_space_newline;
    }
    
    
    RuleTrace trace_peg_1(stream, "space_newline");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
            int position_peg_5 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_3.getPosition()))){
                    result_peg_3.nextPosition();
                } else {
                    result_peg_3.setPosition(position_peg_5);
                    goto out_peg_6;
                }
            }
                
        }
        goto success_peg_4;
        out_peg_6:
        {
            int position_peg_7 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_3.getPosition()))){
                    result_peg_3.nextPosition();
                } else {
                    result_peg_3.setPosition(position_peg_7);
                    goto out_peg_8;
                }
            }
                
        }
        goto success_peg_4;
        out_peg_8:
        {
            int position_peg_9 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_3.getPosition()) == (unsigned char) 255){
                result_peg_3.nextPosition();
            } else {
                result_peg_3.setPosition(position_peg_9);
                goto out_peg_10;
            }
            
        }
        goto success_peg_4;
        out_peg_10:
        goto out_peg_11;
        success_peg_4:
        ;
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_space_newline = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_11:
        Result result_peg_12(myposition);
        
        
        {
            int position_peg_14 = result_peg_12.getPosition();
            
            result_peg_12.setValue(Value((void*) "\n"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_12.getPosition()))){
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
            
            result_peg_12.setValue(Value((void*) "\r"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_12.getPosition()))){
                    result_peg_12.nextPosition();
                } else {
                    result_peg_12.setPosition(position_peg_16);
                    goto out_peg_17;
                }
            }
                
        }
        goto success_peg_13;
        out_peg_17:
        goto out_peg_18;
        success_peg_13:
        ;
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_space_newline = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_18:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_space_newline = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_sw.calculated()){
        return column_peg_2.chunk1->chunk_sw;
    }
    
    
    RuleTrace trace_peg_1(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
            int position_peg_5 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_3.getPosition()))){
                    result_peg_3.nextPosition();
                } else {
                    result_peg_3.setPosition(position_peg_5);
                    goto out_peg_6;
                }
            }
                
        }
        goto success_peg_4;
        out_peg_6:
        {
            int position_peg_7 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_3.getPosition()))){
                    result_peg_3.nextPosition();
                } else {
                    result_peg_3.setPosition(position_peg_7);
                    goto out_peg_8;
                }
            }
                
        }
        goto success_peg_4;
        out_peg_8:
        {
            int position_peg_9 = result_peg_3.getPosition();
            
            result_peg_3.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_3.getPosition()) == (unsigned char) 255){
                result_peg_3.nextPosition();
            } else {
                result_peg_3.setPosition(position_peg_9);
                goto out_peg_10;
            }
            
        }
        goto success_peg_4;
        out_peg_10:
        goto out_peg_11;
        success_peg_4:
        ;
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_sw = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_11:
        Result result_peg_12(myposition);
        
        
        result_peg_12 = rule_comment(stream, result_peg_12.getPosition());
        if (result_peg_12.error()){
            goto out_peg_13;
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_sw = result_peg_12;
        stream.update(result_peg_12.getPosition());
        
        
        return result_peg_12;
        out_peg_13:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk1 != 0 && column_peg_2.chunk1->chunk_comment.calculated()){
        return column_peg_2.chunk1->chunk_comment;
    }
    
    
    RuleTrace trace_peg_1(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) ";"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_7(result_peg_3.getPosition());
                    {
                    
                        Result result_peg_10(result_peg_7);
                            {
                                int position_peg_12 = result_peg_10.getPosition();
                                
                                result_peg_10.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_10.getPosition()))){
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
                                
                                result_peg_10.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_10.getPosition()))){
                                        result_peg_10.nextPosition();
                                    } else {
                                        result_peg_10.setPosition(position_peg_14);
                                        goto out_peg_15;
                                    }
                                }
                                    
                            }
                            goto success_peg_11;
                            out_peg_15:
                            goto not_peg_9;
                            success_peg_11:
                            ;
                            goto loop_peg_6;
                            not_peg_9:
                            result_peg_7.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_16 = stream.get(result_peg_7.getPosition());
                            if (temp_peg_16 != '\0'){
                                result_peg_7.setValue(Value((void*) (intptr_t) temp_peg_16));
                                result_peg_7.nextPosition();
                            } else {
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_17(myposition);
        
        
        {
        
            result_peg_17.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_17.getPosition()))){
                        result_peg_17.nextPosition();
                    } else {
                        goto out_peg_19;
                    }
                }
            
            
            
            result_peg_17.reset();
                do{
                    Result result_peg_21(result_peg_17.getPosition());
                    {
                    
                        Result result_peg_24(result_peg_21);
                            {
                                int position_peg_26 = result_peg_24.getPosition();
                                
                                result_peg_24.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_24.getPosition()))){
                                        result_peg_24.nextPosition();
                                    } else {
                                        result_peg_24.setPosition(position_peg_26);
                                        goto out_peg_27;
                                    }
                                }
                                    
                            }
                            goto success_peg_25;
                            out_peg_27:
                            {
                                int position_peg_28 = result_peg_24.getPosition();
                                
                                result_peg_24.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_24.getPosition()))){
                                        result_peg_24.nextPosition();
                                    } else {
                                        result_peg_24.setPosition(position_peg_28);
                                        goto out_peg_29;
                                    }
                                }
                                    
                            }
                            goto success_peg_25;
                            out_peg_29:
                            goto not_peg_23;
                            success_peg_25:
                            ;
                            goto loop_peg_20;
                            not_peg_23:
                            result_peg_21.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_30 = stream.get(result_peg_21.getPosition());
                            if (temp_peg_30 != '\0'){
                                result_peg_21.setValue(Value((void*) (intptr_t) temp_peg_30));
                                result_peg_21.nextPosition();
                            } else {
                                goto loop_peg_20;
                            }
                        
                        
                    }
                    result_peg_17.addResult(result_peg_21);
                } while (true);
                loop_peg_20:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_17;
        stream.update(result_peg_17.getPosition());
        
        
        return result_peg_17;
        out_peg_19:
        Result result_peg_31(myposition);
        
        
        {
        
            result_peg_31.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto out_peg_33;
                    }
                }
            
            
            
            result_peg_31.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto out_peg_33;
                    }
                }
            
            
            
            result_peg_31.reset();
                do{
                    Result result_peg_36(result_peg_31.getPosition());
                    {
                    
                        Result result_peg_39(result_peg_36);
                            {
                                int position_peg_41 = result_peg_39.getPosition();
                                
                                result_peg_39.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_39.getPosition()))){
                                        result_peg_39.nextPosition();
                                    } else {
                                        result_peg_39.setPosition(position_peg_41);
                                        goto out_peg_42;
                                    }
                                }
                                    
                            }
                            goto success_peg_40;
                            out_peg_42:
                            {
                                int position_peg_43 = result_peg_39.getPosition();
                                
                                result_peg_39.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_39.getPosition()))){
                                        result_peg_39.nextPosition();
                                    } else {
                                        result_peg_39.setPosition(position_peg_43);
                                        goto out_peg_44;
                                    }
                                }
                                    
                            }
                            goto success_peg_40;
                            out_peg_44:
                            goto not_peg_38;
                            success_peg_40:
                            ;
                            goto loop_peg_35;
                            not_peg_38:
                            result_peg_36.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_45 = stream.get(result_peg_36.getPosition());
                            if (temp_peg_45 != '\0'){
                                result_peg_36.setValue(Value((void*) (intptr_t) temp_peg_45));
                                result_peg_36.nextPosition();
                            } else {
                                goto loop_peg_35;
                            }
                        
                        
                    }
                    result_peg_31.addResult(result_peg_36);
                } while (true);
                loop_peg_35:
                ;
            
            
        }
        
        
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = result_peg_31;
        stream.update(result_peg_31.getPosition());
        
        
        return result_peg_31;
        out_peg_33:
    
        if (column_peg_2.chunk1 == 0){
            column_peg_2.chunk1 = new Chunk1();
        }
        column_peg_2.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_collision_default(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_collision_default.calculated()){
        return column_peg_2.chunk2->chunk_collision_default;
    }
    
    
    RuleTrace trace_peg_1(stream, "collision_default");
    int myposition = position;
    
    
    Value num;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "Clsn2Default"));
                for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn2Default"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_9 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_11(result_peg_3.getPosition());
                        {
                            int position_peg_13 = result_peg_11.getPosition();
                            
                            result_peg_11.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_11.getPosition()))){
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
                            
                            result_peg_11.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_11.getPosition()))){
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_15);
                                    goto out_peg_16;
                                }
                            }
                                
                        }
                        goto success_peg_12;
                        out_peg_16:
                        {
                            int position_peg_17 = result_peg_11.getPosition();
                            
                            result_peg_11.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_11.getPosition()) == (unsigned char) 255){
                                result_peg_11.nextPosition();
                            } else {
                                result_peg_11.setPosition(position_peg_17);
                                goto out_peg_18;
                            }
                            
                        }
                        goto success_peg_12;
                        out_peg_18:
                        goto loop_peg_10;
                        success_peg_12:
                        ;
                        result_peg_3.addResult(result_peg_11);
                    } while (true);
                    loop_peg_10:
                    ;
                            
                }
                goto success_peg_8;
                goto out_peg_5;
                success_peg_8:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                num = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_4.getValues())), num);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision_default = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_20(myposition);
        
        
        {
        
            result_peg_20.setValue(Value((void*) "Clsn1Default"));
                for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_20.getPosition()))){
                        result_peg_20.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            Result result_peg_21 = result_peg_20;
            
            result_peg_20.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_20.getPosition()))){
                        result_peg_20.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            
            
            {
                    int position_peg_26 = result_peg_20.getPosition();
                    
                    result_peg_20.reset();
                    do{
                        Result result_peg_28(result_peg_20.getPosition());
                        {
                            int position_peg_30 = result_peg_28.getPosition();
                            
                            result_peg_28.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_28.getPosition()))){
                                    result_peg_28.nextPosition();
                                } else {
                                    result_peg_28.setPosition(position_peg_30);
                                    goto out_peg_31;
                                }
                            }
                                
                        }
                        goto success_peg_29;
                        out_peg_31:
                        {
                            int position_peg_32 = result_peg_28.getPosition();
                            
                            result_peg_28.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_28.getPosition()))){
                                    result_peg_28.nextPosition();
                                } else {
                                    result_peg_28.setPosition(position_peg_32);
                                    goto out_peg_33;
                                }
                            }
                                
                        }
                        goto success_peg_29;
                        out_peg_33:
                        {
                            int position_peg_34 = result_peg_28.getPosition();
                            
                            result_peg_28.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_28.getPosition()) == (unsigned char) 255){
                                result_peg_28.nextPosition();
                            } else {
                                result_peg_28.setPosition(position_peg_34);
                                goto out_peg_35;
                            }
                            
                        }
                        goto success_peg_29;
                        out_peg_35:
                        goto loop_peg_27;
                        success_peg_29:
                        ;
                        result_peg_20.addResult(result_peg_28);
                    } while (true);
                    loop_peg_27:
                    ;
                            
                }
                goto success_peg_25;
                goto out_peg_22;
                success_peg_25:
                ;
            
            
            
            result_peg_20 = rule_integer(stream, result_peg_20.getPosition());
                if (result_peg_20.error()){
                    goto out_peg_22;
                }
                num = result_peg_20.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_21.getValues())), num);
                    result_peg_20.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision_default = result_peg_20;
        stream.update(result_peg_20.getPosition());
        
        
        return result_peg_20;
        out_peg_22:
        Result result_peg_37(myposition);
        
        
        {
        
            result_peg_37.setValue(Value((void*) "Clsn2"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_37.getPosition()))){
                        result_peg_37.nextPosition();
                    } else {
                        goto out_peg_39;
                    }
                }
            
            Result result_peg_38 = result_peg_37;
            
            result_peg_37.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_37.getPosition()))){
                        result_peg_37.nextPosition();
                    } else {
                        goto out_peg_39;
                    }
                }
            
            
            
            {
                    int position_peg_43 = result_peg_37.getPosition();
                    
                    result_peg_37.reset();
                    do{
                        Result result_peg_45(result_peg_37.getPosition());
                        {
                            int position_peg_47 = result_peg_45.getPosition();
                            
                            result_peg_45.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_45.getPosition()))){
                                    result_peg_45.nextPosition();
                                } else {
                                    result_peg_45.setPosition(position_peg_47);
                                    goto out_peg_48;
                                }
                            }
                                
                        }
                        goto success_peg_46;
                        out_peg_48:
                        {
                            int position_peg_49 = result_peg_45.getPosition();
                            
                            result_peg_45.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_45.getPosition()))){
                                    result_peg_45.nextPosition();
                                } else {
                                    result_peg_45.setPosition(position_peg_49);
                                    goto out_peg_50;
                                }
                            }
                                
                        }
                        goto success_peg_46;
                        out_peg_50:
                        {
                            int position_peg_51 = result_peg_45.getPosition();
                            
                            result_peg_45.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_45.getPosition()) == (unsigned char) 255){
                                result_peg_45.nextPosition();
                            } else {
                                result_peg_45.setPosition(position_peg_51);
                                goto out_peg_52;
                            }
                            
                        }
                        goto success_peg_46;
                        out_peg_52:
                        goto loop_peg_44;
                        success_peg_46:
                        ;
                        result_peg_37.addResult(result_peg_45);
                    } while (true);
                    loop_peg_44:
                    ;
                            
                }
                goto success_peg_42;
                goto out_peg_39;
                success_peg_42:
                ;
            
            
            
            result_peg_37 = rule_integer(stream, result_peg_37.getPosition());
                if (result_peg_37.error()){
                    goto out_peg_39;
                }
                num = result_peg_37.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_38.getValues())), num);
                    result_peg_37.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision_default = result_peg_37;
        stream.update(result_peg_37.getPosition());
        
        
        return result_peg_37;
        out_peg_39:
        Result result_peg_54(myposition);
        
        
        {
        
            result_peg_54.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_54.getPosition()))){
                        result_peg_54.nextPosition();
                    } else {
                        goto out_peg_56;
                    }
                }
            
            Result result_peg_55 = result_peg_54;
            
            result_peg_54.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_54.getPosition()))){
                        result_peg_54.nextPosition();
                    } else {
                        goto out_peg_56;
                    }
                }
            
            
            
            {
                    int position_peg_60 = result_peg_54.getPosition();
                    
                    result_peg_54.reset();
                    do{
                        Result result_peg_62(result_peg_54.getPosition());
                        {
                            int position_peg_64 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_64);
                                    goto out_peg_65;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_65:
                        {
                            int position_peg_66 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_66);
                                    goto out_peg_67;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_67:
                        {
                            int position_peg_68 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_62.getPosition()) == (unsigned char) 255){
                                result_peg_62.nextPosition();
                            } else {
                                result_peg_62.setPosition(position_peg_68);
                                goto out_peg_69;
                            }
                            
                        }
                        goto success_peg_63;
                        out_peg_69:
                        goto loop_peg_61;
                        success_peg_63:
                        ;
                        result_peg_54.addResult(result_peg_62);
                    } while (true);
                    loop_peg_61:
                    ;
                            
                }
                goto success_peg_59;
                goto out_peg_56;
                success_peg_59:
                ;
            
            
            
            result_peg_54 = rule_integer(stream, result_peg_54.getPosition());
                if (result_peg_54.error()){
                    goto out_peg_56;
                }
                num = result_peg_54.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_55.getValues())), num);
                    result_peg_54.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision_default = result_peg_54;
        stream.update(result_peg_54.getPosition());
        
        
        return result_peg_54;
        out_peg_56:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision_default = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_collision(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_collision.calculated()){
        return column_peg_2.chunk2->chunk_collision;
    }
    
    
    RuleTrace trace_peg_1(stream, "collision");
    int myposition = position;
    
    
    Value name;
        Value index;
        Value n1;
        Value n2;
        Value n3;
        Value n4;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "Clsn2"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
                name = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
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
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_21 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_23(result_peg_3.getPosition());
                        {
                            int position_peg_25 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_23.getPosition()))){
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
                            
                            result_peg_23.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_23.getPosition()))){
                                    result_peg_23.nextPosition();
                                } else {
                                    result_peg_23.setPosition(position_peg_27);
                                    goto out_peg_28;
                                }
                            }
                                
                        }
                        goto success_peg_24;
                        out_peg_28:
                        {
                            int position_peg_29 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_23.getPosition()) == (unsigned char) 255){
                                result_peg_23.nextPosition();
                            } else {
                                result_peg_23.setPosition(position_peg_29);
                                goto out_peg_30;
                            }
                            
                        }
                        goto success_peg_24;
                        out_peg_30:
                        goto loop_peg_22;
                        success_peg_24:
                        ;
                        result_peg_3.addResult(result_peg_23);
                    } while (true);
                    loop_peg_22:
                    ;
                            
                }
                goto success_peg_20;
                goto out_peg_5;
                success_peg_20:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                index = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_34 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_36(result_peg_3.getPosition());
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
                        result_peg_3.addResult(result_peg_36);
                    } while (true);
                    loop_peg_35:
                    ;
                            
                }
                goto success_peg_33;
                goto out_peg_5;
                success_peg_33:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_47 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_49(result_peg_3.getPosition());
                        {
                            int position_peg_51 = result_peg_49.getPosition();
                            
                            result_peg_49.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_49.getPosition()))){
                                    result_peg_49.nextPosition();
                                } else {
                                    result_peg_49.setPosition(position_peg_51);
                                    goto out_peg_52;
                                }
                            }
                                
                        }
                        goto success_peg_50;
                        out_peg_52:
                        {
                            int position_peg_53 = result_peg_49.getPosition();
                            
                            result_peg_49.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_49.getPosition()))){
                                    result_peg_49.nextPosition();
                                } else {
                                    result_peg_49.setPosition(position_peg_53);
                                    goto out_peg_54;
                                }
                            }
                                
                        }
                        goto success_peg_50;
                        out_peg_54:
                        {
                            int position_peg_55 = result_peg_49.getPosition();
                            
                            result_peg_49.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_49.getPosition()) == (unsigned char) 255){
                                result_peg_49.nextPosition();
                            } else {
                                result_peg_49.setPosition(position_peg_55);
                                goto out_peg_56;
                            }
                            
                        }
                        goto success_peg_50;
                        out_peg_56:
                        goto loop_peg_48;
                        success_peg_50:
                        ;
                        result_peg_3.addResult(result_peg_49);
                    } while (true);
                    loop_peg_48:
                    ;
                            
                }
                goto success_peg_46;
                goto out_peg_5;
                success_peg_46:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_60 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_62(result_peg_3.getPosition());
                        {
                            int position_peg_64 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_64);
                                    goto out_peg_65;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_65:
                        {
                            int position_peg_66 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_66);
                                    goto out_peg_67;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_67:
                        {
                            int position_peg_68 = result_peg_62.getPosition();
                            
                            result_peg_62.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_62.getPosition()) == (unsigned char) 255){
                                result_peg_62.nextPosition();
                            } else {
                                result_peg_62.setPosition(position_peg_68);
                                goto out_peg_69;
                            }
                            
                        }
                        goto success_peg_63;
                        out_peg_69:
                        goto loop_peg_61;
                        success_peg_63:
                        ;
                        result_peg_3.addResult(result_peg_62);
                    } while (true);
                    loop_peg_61:
                    ;
                            
                }
                goto success_peg_59;
                goto out_peg_5;
                success_peg_59:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                n1 = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_73 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_75(result_peg_3.getPosition());
                        {
                            int position_peg_77 = result_peg_75.getPosition();
                            
                            result_peg_75.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_75.getPosition()))){
                                    result_peg_75.nextPosition();
                                } else {
                                    result_peg_75.setPosition(position_peg_77);
                                    goto out_peg_78;
                                }
                            }
                                
                        }
                        goto success_peg_76;
                        out_peg_78:
                        {
                            int position_peg_79 = result_peg_75.getPosition();
                            
                            result_peg_75.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_75.getPosition()))){
                                    result_peg_75.nextPosition();
                                } else {
                                    result_peg_75.setPosition(position_peg_79);
                                    goto out_peg_80;
                                }
                            }
                                
                        }
                        goto success_peg_76;
                        out_peg_80:
                        {
                            int position_peg_81 = result_peg_75.getPosition();
                            
                            result_peg_75.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_75.getPosition()) == (unsigned char) 255){
                                result_peg_75.nextPosition();
                            } else {
                                result_peg_75.setPosition(position_peg_81);
                                goto out_peg_82;
                            }
                            
                        }
                        goto success_peg_76;
                        out_peg_82:
                        goto loop_peg_74;
                        success_peg_76:
                        ;
                        result_peg_3.addResult(result_peg_75);
                    } while (true);
                    loop_peg_74:
                    ;
                            
                }
                goto success_peg_72;
                goto out_peg_5;
                success_peg_72:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_86 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_88(result_peg_3.getPosition());
                        {
                            int position_peg_90 = result_peg_88.getPosition();
                            
                            result_peg_88.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_88.getPosition()))){
                                    result_peg_88.nextPosition();
                                } else {
                                    result_peg_88.setPosition(position_peg_90);
                                    goto out_peg_91;
                                }
                            }
                                
                        }
                        goto success_peg_89;
                        out_peg_91:
                        {
                            int position_peg_92 = result_peg_88.getPosition();
                            
                            result_peg_88.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_88.getPosition()))){
                                    result_peg_88.nextPosition();
                                } else {
                                    result_peg_88.setPosition(position_peg_92);
                                    goto out_peg_93;
                                }
                            }
                                
                        }
                        goto success_peg_89;
                        out_peg_93:
                        {
                            int position_peg_94 = result_peg_88.getPosition();
                            
                            result_peg_88.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_88.getPosition()) == (unsigned char) 255){
                                result_peg_88.nextPosition();
                            } else {
                                result_peg_88.setPosition(position_peg_94);
                                goto out_peg_95;
                            }
                            
                        }
                        goto success_peg_89;
                        out_peg_95:
                        goto loop_peg_87;
                        success_peg_89:
                        ;
                        result_peg_3.addResult(result_peg_88);
                    } while (true);
                    loop_peg_87:
                    ;
                            
                }
                goto success_peg_85;
                goto out_peg_5;
                success_peg_85:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                n2 = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_99 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_101(result_peg_3.getPosition());
                        {
                            int position_peg_103 = result_peg_101.getPosition();
                            
                            result_peg_101.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_101.getPosition()))){
                                    result_peg_101.nextPosition();
                                } else {
                                    result_peg_101.setPosition(position_peg_103);
                                    goto out_peg_104;
                                }
                            }
                                
                        }
                        goto success_peg_102;
                        out_peg_104:
                        {
                            int position_peg_105 = result_peg_101.getPosition();
                            
                            result_peg_101.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_101.getPosition()))){
                                    result_peg_101.nextPosition();
                                } else {
                                    result_peg_101.setPosition(position_peg_105);
                                    goto out_peg_106;
                                }
                            }
                                
                        }
                        goto success_peg_102;
                        out_peg_106:
                        {
                            int position_peg_107 = result_peg_101.getPosition();
                            
                            result_peg_101.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_101.getPosition()) == (unsigned char) 255){
                                result_peg_101.nextPosition();
                            } else {
                                result_peg_101.setPosition(position_peg_107);
                                goto out_peg_108;
                            }
                            
                        }
                        goto success_peg_102;
                        out_peg_108:
                        goto loop_peg_100;
                        success_peg_102:
                        ;
                        result_peg_3.addResult(result_peg_101);
                    } while (true);
                    loop_peg_100:
                    ;
                            
                }
                goto success_peg_98;
                goto out_peg_5;
                success_peg_98:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_112 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_114(result_peg_3.getPosition());
                        {
                            int position_peg_116 = result_peg_114.getPosition();
                            
                            result_peg_114.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_114.getPosition()))){
                                    result_peg_114.nextPosition();
                                } else {
                                    result_peg_114.setPosition(position_peg_116);
                                    goto out_peg_117;
                                }
                            }
                                
                        }
                        goto success_peg_115;
                        out_peg_117:
                        {
                            int position_peg_118 = result_peg_114.getPosition();
                            
                            result_peg_114.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_114.getPosition()))){
                                    result_peg_114.nextPosition();
                                } else {
                                    result_peg_114.setPosition(position_peg_118);
                                    goto out_peg_119;
                                }
                            }
                                
                        }
                        goto success_peg_115;
                        out_peg_119:
                        {
                            int position_peg_120 = result_peg_114.getPosition();
                            
                            result_peg_114.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_114.getPosition()) == (unsigned char) 255){
                                result_peg_114.nextPosition();
                            } else {
                                result_peg_114.setPosition(position_peg_120);
                                goto out_peg_121;
                            }
                            
                        }
                        goto success_peg_115;
                        out_peg_121:
                        goto loop_peg_113;
                        success_peg_115:
                        ;
                        result_peg_3.addResult(result_peg_114);
                    } while (true);
                    loop_peg_113:
                    ;
                            
                }
                goto success_peg_111;
                goto out_peg_5;
                success_peg_111:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                n3 = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_125 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_127(result_peg_3.getPosition());
                        {
                            int position_peg_129 = result_peg_127.getPosition();
                            
                            result_peg_127.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    result_peg_127.setPosition(position_peg_129);
                                    goto out_peg_130;
                                }
                            }
                                
                        }
                        goto success_peg_128;
                        out_peg_130:
                        {
                            int position_peg_131 = result_peg_127.getPosition();
                            
                            result_peg_127.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    result_peg_127.setPosition(position_peg_131);
                                    goto out_peg_132;
                                }
                            }
                                
                        }
                        goto success_peg_128;
                        out_peg_132:
                        {
                            int position_peg_133 = result_peg_127.getPosition();
                            
                            result_peg_127.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_127.getPosition()) == (unsigned char) 255){
                                result_peg_127.nextPosition();
                            } else {
                                result_peg_127.setPosition(position_peg_133);
                                goto out_peg_134;
                            }
                            
                        }
                        goto success_peg_128;
                        out_peg_134:
                        goto loop_peg_126;
                        success_peg_128:
                        ;
                        result_peg_3.addResult(result_peg_127);
                    } while (true);
                    loop_peg_126:
                    ;
                            
                }
                goto success_peg_124;
                goto out_peg_5;
                success_peg_124:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_138 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_140(result_peg_3.getPosition());
                        {
                            int position_peg_142 = result_peg_140.getPosition();
                            
                            result_peg_140.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_140.getPosition()))){
                                    result_peg_140.nextPosition();
                                } else {
                                    result_peg_140.setPosition(position_peg_142);
                                    goto out_peg_143;
                                }
                            }
                                
                        }
                        goto success_peg_141;
                        out_peg_143:
                        {
                            int position_peg_144 = result_peg_140.getPosition();
                            
                            result_peg_140.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_140.getPosition()))){
                                    result_peg_140.nextPosition();
                                } else {
                                    result_peg_140.setPosition(position_peg_144);
                                    goto out_peg_145;
                                }
                            }
                                
                        }
                        goto success_peg_141;
                        out_peg_145:
                        {
                            int position_peg_146 = result_peg_140.getPosition();
                            
                            result_peg_140.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_140.getPosition()) == (unsigned char) 255){
                                result_peg_140.nextPosition();
                            } else {
                                result_peg_140.setPosition(position_peg_146);
                                goto out_peg_147;
                            }
                            
                        }
                        goto success_peg_141;
                        out_peg_147:
                        goto loop_peg_139;
                        success_peg_141:
                        ;
                        result_peg_3.addResult(result_peg_140);
                    } while (true);
                    loop_peg_139:
                    ;
                            
                }
                goto success_peg_137;
                goto out_peg_5;
                success_peg_137:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                n4 = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
        Result result_peg_149(myposition);
        
        
        {
        
            result_peg_149.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
                name = result_peg_149.getValues();
            
            
            
            {
                    int position_peg_154 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_156(result_peg_149.getPosition());
                        {
                            int position_peg_158 = result_peg_156.getPosition();
                            
                            result_peg_156.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_156.getPosition()))){
                                    result_peg_156.nextPosition();
                                } else {
                                    result_peg_156.setPosition(position_peg_158);
                                    goto out_peg_159;
                                }
                            }
                                
                        }
                        goto success_peg_157;
                        out_peg_159:
                        {
                            int position_peg_160 = result_peg_156.getPosition();
                            
                            result_peg_156.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_156.getPosition()))){
                                    result_peg_156.nextPosition();
                                } else {
                                    result_peg_156.setPosition(position_peg_160);
                                    goto out_peg_161;
                                }
                            }
                                
                        }
                        goto success_peg_157;
                        out_peg_161:
                        {
                            int position_peg_162 = result_peg_156.getPosition();
                            
                            result_peg_156.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_156.getPosition()) == (unsigned char) 255){
                                result_peg_156.nextPosition();
                            } else {
                                result_peg_156.setPosition(position_peg_162);
                                goto out_peg_163;
                            }
                            
                        }
                        goto success_peg_157;
                        out_peg_163:
                        goto loop_peg_155;
                        success_peg_157:
                        ;
                        result_peg_149.addResult(result_peg_156);
                    } while (true);
                    loop_peg_155:
                    ;
                            
                }
                goto success_peg_153;
                goto out_peg_151;
                success_peg_153:
                ;
            
            
            
            result_peg_149.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
            
            
            
            {
                    int position_peg_167 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_169(result_peg_149.getPosition());
                        {
                            int position_peg_171 = result_peg_169.getPosition();
                            
                            result_peg_169.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_169.getPosition()))){
                                    result_peg_169.nextPosition();
                                } else {
                                    result_peg_169.setPosition(position_peg_171);
                                    goto out_peg_172;
                                }
                            }
                                
                        }
                        goto success_peg_170;
                        out_peg_172:
                        {
                            int position_peg_173 = result_peg_169.getPosition();
                            
                            result_peg_169.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_169.getPosition()))){
                                    result_peg_169.nextPosition();
                                } else {
                                    result_peg_169.setPosition(position_peg_173);
                                    goto out_peg_174;
                                }
                            }
                                
                        }
                        goto success_peg_170;
                        out_peg_174:
                        {
                            int position_peg_175 = result_peg_169.getPosition();
                            
                            result_peg_169.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_169.getPosition()) == (unsigned char) 255){
                                result_peg_169.nextPosition();
                            } else {
                                result_peg_169.setPosition(position_peg_175);
                                goto out_peg_176;
                            }
                            
                        }
                        goto success_peg_170;
                        out_peg_176:
                        goto loop_peg_168;
                        success_peg_170:
                        ;
                        result_peg_149.addResult(result_peg_169);
                    } while (true);
                    loop_peg_168:
                    ;
                            
                }
                goto success_peg_166;
                goto out_peg_151;
                success_peg_166:
                ;
            
            
            
            result_peg_149 = rule_integer(stream, result_peg_149.getPosition());
                if (result_peg_149.error()){
                    goto out_peg_151;
                }
                index = result_peg_149.getValues();
            
            
            
            {
                    int position_peg_180 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_182(result_peg_149.getPosition());
                        {
                            int position_peg_184 = result_peg_182.getPosition();
                            
                            result_peg_182.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_182.getPosition()))){
                                    result_peg_182.nextPosition();
                                } else {
                                    result_peg_182.setPosition(position_peg_184);
                                    goto out_peg_185;
                                }
                            }
                                
                        }
                        goto success_peg_183;
                        out_peg_185:
                        {
                            int position_peg_186 = result_peg_182.getPosition();
                            
                            result_peg_182.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_182.getPosition()))){
                                    result_peg_182.nextPosition();
                                } else {
                                    result_peg_182.setPosition(position_peg_186);
                                    goto out_peg_187;
                                }
                            }
                                
                        }
                        goto success_peg_183;
                        out_peg_187:
                        {
                            int position_peg_188 = result_peg_182.getPosition();
                            
                            result_peg_182.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_182.getPosition()) == (unsigned char) 255){
                                result_peg_182.nextPosition();
                            } else {
                                result_peg_182.setPosition(position_peg_188);
                                goto out_peg_189;
                            }
                            
                        }
                        goto success_peg_183;
                        out_peg_189:
                        goto loop_peg_181;
                        success_peg_183:
                        ;
                        result_peg_149.addResult(result_peg_182);
                    } while (true);
                    loop_peg_181:
                    ;
                            
                }
                goto success_peg_179;
                goto out_peg_151;
                success_peg_179:
                ;
            
            
            
            result_peg_149.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
            
            
            
            {
                    int position_peg_193 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_195(result_peg_149.getPosition());
                        {
                            int position_peg_197 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_195.getPosition()))){
                                    result_peg_195.nextPosition();
                                } else {
                                    result_peg_195.setPosition(position_peg_197);
                                    goto out_peg_198;
                                }
                            }
                                
                        }
                        goto success_peg_196;
                        out_peg_198:
                        {
                            int position_peg_199 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_195.getPosition()))){
                                    result_peg_195.nextPosition();
                                } else {
                                    result_peg_195.setPosition(position_peg_199);
                                    goto out_peg_200;
                                }
                            }
                                
                        }
                        goto success_peg_196;
                        out_peg_200:
                        {
                            int position_peg_201 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_195.getPosition()) == (unsigned char) 255){
                                result_peg_195.nextPosition();
                            } else {
                                result_peg_195.setPosition(position_peg_201);
                                goto out_peg_202;
                            }
                            
                        }
                        goto success_peg_196;
                        out_peg_202:
                        goto loop_peg_194;
                        success_peg_196:
                        ;
                        result_peg_149.addResult(result_peg_195);
                    } while (true);
                    loop_peg_194:
                    ;
                            
                }
                goto success_peg_192;
                goto out_peg_151;
                success_peg_192:
                ;
            
            
            
            result_peg_149.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
            
            
            
            {
                    int position_peg_206 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_208(result_peg_149.getPosition());
                        {
                            int position_peg_210 = result_peg_208.getPosition();
                            
                            result_peg_208.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_208.getPosition()))){
                                    result_peg_208.nextPosition();
                                } else {
                                    result_peg_208.setPosition(position_peg_210);
                                    goto out_peg_211;
                                }
                            }
                                
                        }
                        goto success_peg_209;
                        out_peg_211:
                        {
                            int position_peg_212 = result_peg_208.getPosition();
                            
                            result_peg_208.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_208.getPosition()))){
                                    result_peg_208.nextPosition();
                                } else {
                                    result_peg_208.setPosition(position_peg_212);
                                    goto out_peg_213;
                                }
                            }
                                
                        }
                        goto success_peg_209;
                        out_peg_213:
                        {
                            int position_peg_214 = result_peg_208.getPosition();
                            
                            result_peg_208.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_208.getPosition()) == (unsigned char) 255){
                                result_peg_208.nextPosition();
                            } else {
                                result_peg_208.setPosition(position_peg_214);
                                goto out_peg_215;
                            }
                            
                        }
                        goto success_peg_209;
                        out_peg_215:
                        goto loop_peg_207;
                        success_peg_209:
                        ;
                        result_peg_149.addResult(result_peg_208);
                    } while (true);
                    loop_peg_207:
                    ;
                            
                }
                goto success_peg_205;
                goto out_peg_151;
                success_peg_205:
                ;
            
            
            
            result_peg_149 = rule_integer(stream, result_peg_149.getPosition());
                if (result_peg_149.error()){
                    goto out_peg_151;
                }
                n1 = result_peg_149.getValues();
            
            
            
            {
                    int position_peg_219 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_221(result_peg_149.getPosition());
                        {
                            int position_peg_223 = result_peg_221.getPosition();
                            
                            result_peg_221.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_221.getPosition()))){
                                    result_peg_221.nextPosition();
                                } else {
                                    result_peg_221.setPosition(position_peg_223);
                                    goto out_peg_224;
                                }
                            }
                                
                        }
                        goto success_peg_222;
                        out_peg_224:
                        {
                            int position_peg_225 = result_peg_221.getPosition();
                            
                            result_peg_221.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_221.getPosition()))){
                                    result_peg_221.nextPosition();
                                } else {
                                    result_peg_221.setPosition(position_peg_225);
                                    goto out_peg_226;
                                }
                            }
                                
                        }
                        goto success_peg_222;
                        out_peg_226:
                        {
                            int position_peg_227 = result_peg_221.getPosition();
                            
                            result_peg_221.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_221.getPosition()) == (unsigned char) 255){
                                result_peg_221.nextPosition();
                            } else {
                                result_peg_221.setPosition(position_peg_227);
                                goto out_peg_228;
                            }
                            
                        }
                        goto success_peg_222;
                        out_peg_228:
                        goto loop_peg_220;
                        success_peg_222:
                        ;
                        result_peg_149.addResult(result_peg_221);
                    } while (true);
                    loop_peg_220:
                    ;
                            
                }
                goto success_peg_218;
                goto out_peg_151;
                success_peg_218:
                ;
            
            
            
            result_peg_149.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
            
            
            
            {
                    int position_peg_232 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_234(result_peg_149.getPosition());
                        {
                            int position_peg_236 = result_peg_234.getPosition();
                            
                            result_peg_234.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_234.getPosition()))){
                                    result_peg_234.nextPosition();
                                } else {
                                    result_peg_234.setPosition(position_peg_236);
                                    goto out_peg_237;
                                }
                            }
                                
                        }
                        goto success_peg_235;
                        out_peg_237:
                        {
                            int position_peg_238 = result_peg_234.getPosition();
                            
                            result_peg_234.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_234.getPosition()))){
                                    result_peg_234.nextPosition();
                                } else {
                                    result_peg_234.setPosition(position_peg_238);
                                    goto out_peg_239;
                                }
                            }
                                
                        }
                        goto success_peg_235;
                        out_peg_239:
                        {
                            int position_peg_240 = result_peg_234.getPosition();
                            
                            result_peg_234.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_234.getPosition()) == (unsigned char) 255){
                                result_peg_234.nextPosition();
                            } else {
                                result_peg_234.setPosition(position_peg_240);
                                goto out_peg_241;
                            }
                            
                        }
                        goto success_peg_235;
                        out_peg_241:
                        goto loop_peg_233;
                        success_peg_235:
                        ;
                        result_peg_149.addResult(result_peg_234);
                    } while (true);
                    loop_peg_233:
                    ;
                            
                }
                goto success_peg_231;
                goto out_peg_151;
                success_peg_231:
                ;
            
            
            
            result_peg_149 = rule_integer(stream, result_peg_149.getPosition());
                if (result_peg_149.error()){
                    goto out_peg_151;
                }
                n2 = result_peg_149.getValues();
            
            
            
            {
                    int position_peg_245 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_247(result_peg_149.getPosition());
                        {
                            int position_peg_249 = result_peg_247.getPosition();
                            
                            result_peg_247.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_247.getPosition()))){
                                    result_peg_247.nextPosition();
                                } else {
                                    result_peg_247.setPosition(position_peg_249);
                                    goto out_peg_250;
                                }
                            }
                                
                        }
                        goto success_peg_248;
                        out_peg_250:
                        {
                            int position_peg_251 = result_peg_247.getPosition();
                            
                            result_peg_247.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_247.getPosition()))){
                                    result_peg_247.nextPosition();
                                } else {
                                    result_peg_247.setPosition(position_peg_251);
                                    goto out_peg_252;
                                }
                            }
                                
                        }
                        goto success_peg_248;
                        out_peg_252:
                        {
                            int position_peg_253 = result_peg_247.getPosition();
                            
                            result_peg_247.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_247.getPosition()) == (unsigned char) 255){
                                result_peg_247.nextPosition();
                            } else {
                                result_peg_247.setPosition(position_peg_253);
                                goto out_peg_254;
                            }
                            
                        }
                        goto success_peg_248;
                        out_peg_254:
                        goto loop_peg_246;
                        success_peg_248:
                        ;
                        result_peg_149.addResult(result_peg_247);
                    } while (true);
                    loop_peg_246:
                    ;
                            
                }
                goto success_peg_244;
                goto out_peg_151;
                success_peg_244:
                ;
            
            
            
            result_peg_149.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
            
            
            
            {
                    int position_peg_258 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_260(result_peg_149.getPosition());
                        {
                            int position_peg_262 = result_peg_260.getPosition();
                            
                            result_peg_260.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_260.getPosition()))){
                                    result_peg_260.nextPosition();
                                } else {
                                    result_peg_260.setPosition(position_peg_262);
                                    goto out_peg_263;
                                }
                            }
                                
                        }
                        goto success_peg_261;
                        out_peg_263:
                        {
                            int position_peg_264 = result_peg_260.getPosition();
                            
                            result_peg_260.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_260.getPosition()))){
                                    result_peg_260.nextPosition();
                                } else {
                                    result_peg_260.setPosition(position_peg_264);
                                    goto out_peg_265;
                                }
                            }
                                
                        }
                        goto success_peg_261;
                        out_peg_265:
                        {
                            int position_peg_266 = result_peg_260.getPosition();
                            
                            result_peg_260.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_260.getPosition()) == (unsigned char) 255){
                                result_peg_260.nextPosition();
                            } else {
                                result_peg_260.setPosition(position_peg_266);
                                goto out_peg_267;
                            }
                            
                        }
                        goto success_peg_261;
                        out_peg_267:
                        goto loop_peg_259;
                        success_peg_261:
                        ;
                        result_peg_149.addResult(result_peg_260);
                    } while (true);
                    loop_peg_259:
                    ;
                            
                }
                goto success_peg_257;
                goto out_peg_151;
                success_peg_257:
                ;
            
            
            
            result_peg_149 = rule_integer(stream, result_peg_149.getPosition());
                if (result_peg_149.error()){
                    goto out_peg_151;
                }
                n3 = result_peg_149.getValues();
            
            
            
            {
                    int position_peg_271 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_273(result_peg_149.getPosition());
                        {
                            int position_peg_275 = result_peg_273.getPosition();
                            
                            result_peg_273.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_273.getPosition()))){
                                    result_peg_273.nextPosition();
                                } else {
                                    result_peg_273.setPosition(position_peg_275);
                                    goto out_peg_276;
                                }
                            }
                                
                        }
                        goto success_peg_274;
                        out_peg_276:
                        {
                            int position_peg_277 = result_peg_273.getPosition();
                            
                            result_peg_273.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_273.getPosition()))){
                                    result_peg_273.nextPosition();
                                } else {
                                    result_peg_273.setPosition(position_peg_277);
                                    goto out_peg_278;
                                }
                            }
                                
                        }
                        goto success_peg_274;
                        out_peg_278:
                        {
                            int position_peg_279 = result_peg_273.getPosition();
                            
                            result_peg_273.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_273.getPosition()) == (unsigned char) 255){
                                result_peg_273.nextPosition();
                            } else {
                                result_peg_273.setPosition(position_peg_279);
                                goto out_peg_280;
                            }
                            
                        }
                        goto success_peg_274;
                        out_peg_280:
                        goto loop_peg_272;
                        success_peg_274:
                        ;
                        result_peg_149.addResult(result_peg_273);
                    } while (true);
                    loop_peg_272:
                    ;
                            
                }
                goto success_peg_270;
                goto out_peg_151;
                success_peg_270:
                ;
            
            
            
            result_peg_149.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_149.getPosition()))){
                        result_peg_149.nextPosition();
                    } else {
                        goto out_peg_151;
                    }
                }
            
            
            
            {
                    int position_peg_284 = result_peg_149.getPosition();
                    
                    result_peg_149.reset();
                    do{
                        Result result_peg_286(result_peg_149.getPosition());
                        {
                            int position_peg_288 = result_peg_286.getPosition();
                            
                            result_peg_286.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_286.getPosition()))){
                                    result_peg_286.nextPosition();
                                } else {
                                    result_peg_286.setPosition(position_peg_288);
                                    goto out_peg_289;
                                }
                            }
                                
                        }
                        goto success_peg_287;
                        out_peg_289:
                        {
                            int position_peg_290 = result_peg_286.getPosition();
                            
                            result_peg_286.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_286.getPosition()))){
                                    result_peg_286.nextPosition();
                                } else {
                                    result_peg_286.setPosition(position_peg_290);
                                    goto out_peg_291;
                                }
                            }
                                
                        }
                        goto success_peg_287;
                        out_peg_291:
                        {
                            int position_peg_292 = result_peg_286.getPosition();
                            
                            result_peg_286.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_286.getPosition()) == (unsigned char) 255){
                                result_peg_286.nextPosition();
                            } else {
                                result_peg_286.setPosition(position_peg_292);
                                goto out_peg_293;
                            }
                            
                        }
                        goto success_peg_287;
                        out_peg_293:
                        goto loop_peg_285;
                        success_peg_287:
                        ;
                        result_peg_149.addResult(result_peg_286);
                    } while (true);
                    loop_peg_285:
                    ;
                            
                }
                goto success_peg_283;
                goto out_peg_151;
                success_peg_283:
                ;
            
            
            
            result_peg_149 = rule_integer(stream, result_peg_149.getPosition());
                if (result_peg_149.error()){
                    goto out_peg_151;
                }
                n4 = result_peg_149.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_149.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision = result_peg_149;
        stream.update(result_peg_149.getPosition());
        
        
        return result_peg_149;
        out_peg_151:
        Result result_peg_295(myposition);
        
        
        {
        
            result_peg_295.setValue(Value((void*) "Clsn"));
                for (int i = 0; i < 4; i++){
                    if (compareCharCase("Clsn"[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
                name = result_peg_295.getValues();
            
            
            
            {
                    int position_peg_300 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_302(result_peg_295.getPosition());
                        {
                            int position_peg_304 = result_peg_302.getPosition();
                            
                            result_peg_302.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_302.getPosition()))){
                                    result_peg_302.nextPosition();
                                } else {
                                    result_peg_302.setPosition(position_peg_304);
                                    goto out_peg_305;
                                }
                            }
                                
                        }
                        goto success_peg_303;
                        out_peg_305:
                        {
                            int position_peg_306 = result_peg_302.getPosition();
                            
                            result_peg_302.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_302.getPosition()))){
                                    result_peg_302.nextPosition();
                                } else {
                                    result_peg_302.setPosition(position_peg_306);
                                    goto out_peg_307;
                                }
                            }
                                
                        }
                        goto success_peg_303;
                        out_peg_307:
                        {
                            int position_peg_308 = result_peg_302.getPosition();
                            
                            result_peg_302.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_302.getPosition()) == (unsigned char) 255){
                                result_peg_302.nextPosition();
                            } else {
                                result_peg_302.setPosition(position_peg_308);
                                goto out_peg_309;
                            }
                            
                        }
                        goto success_peg_303;
                        out_peg_309:
                        goto loop_peg_301;
                        success_peg_303:
                        ;
                        result_peg_295.addResult(result_peg_302);
                    } while (true);
                    loop_peg_301:
                    ;
                            
                }
                goto success_peg_299;
                goto out_peg_297;
                success_peg_299:
                ;
            
            
            
            result_peg_295.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
            
            
            
            {
                    int position_peg_313 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_315(result_peg_295.getPosition());
                        {
                            int position_peg_317 = result_peg_315.getPosition();
                            
                            result_peg_315.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_315.getPosition()))){
                                    result_peg_315.nextPosition();
                                } else {
                                    result_peg_315.setPosition(position_peg_317);
                                    goto out_peg_318;
                                }
                            }
                                
                        }
                        goto success_peg_316;
                        out_peg_318:
                        {
                            int position_peg_319 = result_peg_315.getPosition();
                            
                            result_peg_315.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_315.getPosition()))){
                                    result_peg_315.nextPosition();
                                } else {
                                    result_peg_315.setPosition(position_peg_319);
                                    goto out_peg_320;
                                }
                            }
                                
                        }
                        goto success_peg_316;
                        out_peg_320:
                        {
                            int position_peg_321 = result_peg_315.getPosition();
                            
                            result_peg_315.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_315.getPosition()) == (unsigned char) 255){
                                result_peg_315.nextPosition();
                            } else {
                                result_peg_315.setPosition(position_peg_321);
                                goto out_peg_322;
                            }
                            
                        }
                        goto success_peg_316;
                        out_peg_322:
                        goto loop_peg_314;
                        success_peg_316:
                        ;
                        result_peg_295.addResult(result_peg_315);
                    } while (true);
                    loop_peg_314:
                    ;
                            
                }
                goto success_peg_312;
                goto out_peg_297;
                success_peg_312:
                ;
            
            
            
            result_peg_295 = rule_integer(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
                index = result_peg_295.getValues();
            
            
            
            {
                    int position_peg_326 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_328(result_peg_295.getPosition());
                        {
                            int position_peg_330 = result_peg_328.getPosition();
                            
                            result_peg_328.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_328.getPosition()))){
                                    result_peg_328.nextPosition();
                                } else {
                                    result_peg_328.setPosition(position_peg_330);
                                    goto out_peg_331;
                                }
                            }
                                
                        }
                        goto success_peg_329;
                        out_peg_331:
                        {
                            int position_peg_332 = result_peg_328.getPosition();
                            
                            result_peg_328.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_328.getPosition()))){
                                    result_peg_328.nextPosition();
                                } else {
                                    result_peg_328.setPosition(position_peg_332);
                                    goto out_peg_333;
                                }
                            }
                                
                        }
                        goto success_peg_329;
                        out_peg_333:
                        {
                            int position_peg_334 = result_peg_328.getPosition();
                            
                            result_peg_328.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_328.getPosition()) == (unsigned char) 255){
                                result_peg_328.nextPosition();
                            } else {
                                result_peg_328.setPosition(position_peg_334);
                                goto out_peg_335;
                            }
                            
                        }
                        goto success_peg_329;
                        out_peg_335:
                        goto loop_peg_327;
                        success_peg_329:
                        ;
                        result_peg_295.addResult(result_peg_328);
                    } while (true);
                    loop_peg_327:
                    ;
                            
                }
                goto success_peg_325;
                goto out_peg_297;
                success_peg_325:
                ;
            
            
            
            result_peg_295.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
            
            
            
            {
                    int position_peg_339 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_341(result_peg_295.getPosition());
                        {
                            int position_peg_343 = result_peg_341.getPosition();
                            
                            result_peg_341.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_341.getPosition()))){
                                    result_peg_341.nextPosition();
                                } else {
                                    result_peg_341.setPosition(position_peg_343);
                                    goto out_peg_344;
                                }
                            }
                                
                        }
                        goto success_peg_342;
                        out_peg_344:
                        {
                            int position_peg_345 = result_peg_341.getPosition();
                            
                            result_peg_341.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_341.getPosition()))){
                                    result_peg_341.nextPosition();
                                } else {
                                    result_peg_341.setPosition(position_peg_345);
                                    goto out_peg_346;
                                }
                            }
                                
                        }
                        goto success_peg_342;
                        out_peg_346:
                        {
                            int position_peg_347 = result_peg_341.getPosition();
                            
                            result_peg_341.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_341.getPosition()) == (unsigned char) 255){
                                result_peg_341.nextPosition();
                            } else {
                                result_peg_341.setPosition(position_peg_347);
                                goto out_peg_348;
                            }
                            
                        }
                        goto success_peg_342;
                        out_peg_348:
                        goto loop_peg_340;
                        success_peg_342:
                        ;
                        result_peg_295.addResult(result_peg_341);
                    } while (true);
                    loop_peg_340:
                    ;
                            
                }
                goto success_peg_338;
                goto out_peg_297;
                success_peg_338:
                ;
            
            
            
            result_peg_295.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
            
            
            
            {
                    int position_peg_352 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_354(result_peg_295.getPosition());
                        {
                            int position_peg_356 = result_peg_354.getPosition();
                            
                            result_peg_354.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_354.getPosition()))){
                                    result_peg_354.nextPosition();
                                } else {
                                    result_peg_354.setPosition(position_peg_356);
                                    goto out_peg_357;
                                }
                            }
                                
                        }
                        goto success_peg_355;
                        out_peg_357:
                        {
                            int position_peg_358 = result_peg_354.getPosition();
                            
                            result_peg_354.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_354.getPosition()))){
                                    result_peg_354.nextPosition();
                                } else {
                                    result_peg_354.setPosition(position_peg_358);
                                    goto out_peg_359;
                                }
                            }
                                
                        }
                        goto success_peg_355;
                        out_peg_359:
                        {
                            int position_peg_360 = result_peg_354.getPosition();
                            
                            result_peg_354.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_354.getPosition()) == (unsigned char) 255){
                                result_peg_354.nextPosition();
                            } else {
                                result_peg_354.setPosition(position_peg_360);
                                goto out_peg_361;
                            }
                            
                        }
                        goto success_peg_355;
                        out_peg_361:
                        goto loop_peg_353;
                        success_peg_355:
                        ;
                        result_peg_295.addResult(result_peg_354);
                    } while (true);
                    loop_peg_353:
                    ;
                            
                }
                goto success_peg_351;
                goto out_peg_297;
                success_peg_351:
                ;
            
            
            
            result_peg_295 = rule_integer(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
                n1 = result_peg_295.getValues();
            
            
            
            {
                    int position_peg_365 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_367(result_peg_295.getPosition());
                        {
                            int position_peg_369 = result_peg_367.getPosition();
                            
                            result_peg_367.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_367.getPosition()))){
                                    result_peg_367.nextPosition();
                                } else {
                                    result_peg_367.setPosition(position_peg_369);
                                    goto out_peg_370;
                                }
                            }
                                
                        }
                        goto success_peg_368;
                        out_peg_370:
                        {
                            int position_peg_371 = result_peg_367.getPosition();
                            
                            result_peg_367.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_367.getPosition()))){
                                    result_peg_367.nextPosition();
                                } else {
                                    result_peg_367.setPosition(position_peg_371);
                                    goto out_peg_372;
                                }
                            }
                                
                        }
                        goto success_peg_368;
                        out_peg_372:
                        {
                            int position_peg_373 = result_peg_367.getPosition();
                            
                            result_peg_367.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_367.getPosition()) == (unsigned char) 255){
                                result_peg_367.nextPosition();
                            } else {
                                result_peg_367.setPosition(position_peg_373);
                                goto out_peg_374;
                            }
                            
                        }
                        goto success_peg_368;
                        out_peg_374:
                        goto loop_peg_366;
                        success_peg_368:
                        ;
                        result_peg_295.addResult(result_peg_367);
                    } while (true);
                    loop_peg_366:
                    ;
                            
                }
                goto success_peg_364;
                goto out_peg_297;
                success_peg_364:
                ;
            
            
            
            result_peg_295.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
            
            
            
            {
                    int position_peg_378 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_380(result_peg_295.getPosition());
                        {
                            int position_peg_382 = result_peg_380.getPosition();
                            
                            result_peg_380.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_380.getPosition()))){
                                    result_peg_380.nextPosition();
                                } else {
                                    result_peg_380.setPosition(position_peg_382);
                                    goto out_peg_383;
                                }
                            }
                                
                        }
                        goto success_peg_381;
                        out_peg_383:
                        {
                            int position_peg_384 = result_peg_380.getPosition();
                            
                            result_peg_380.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_380.getPosition()))){
                                    result_peg_380.nextPosition();
                                } else {
                                    result_peg_380.setPosition(position_peg_384);
                                    goto out_peg_385;
                                }
                            }
                                
                        }
                        goto success_peg_381;
                        out_peg_385:
                        {
                            int position_peg_386 = result_peg_380.getPosition();
                            
                            result_peg_380.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_380.getPosition()) == (unsigned char) 255){
                                result_peg_380.nextPosition();
                            } else {
                                result_peg_380.setPosition(position_peg_386);
                                goto out_peg_387;
                            }
                            
                        }
                        goto success_peg_381;
                        out_peg_387:
                        goto loop_peg_379;
                        success_peg_381:
                        ;
                        result_peg_295.addResult(result_peg_380);
                    } while (true);
                    loop_peg_379:
                    ;
                            
                }
                goto success_peg_377;
                goto out_peg_297;
                success_peg_377:
                ;
            
            
            
            result_peg_295 = rule_integer(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
                n2 = result_peg_295.getValues();
            
            
            
            {
                    int position_peg_391 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_393(result_peg_295.getPosition());
                        {
                            int position_peg_395 = result_peg_393.getPosition();
                            
                            result_peg_393.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_393.getPosition()))){
                                    result_peg_393.nextPosition();
                                } else {
                                    result_peg_393.setPosition(position_peg_395);
                                    goto out_peg_396;
                                }
                            }
                                
                        }
                        goto success_peg_394;
                        out_peg_396:
                        {
                            int position_peg_397 = result_peg_393.getPosition();
                            
                            result_peg_393.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_393.getPosition()))){
                                    result_peg_393.nextPosition();
                                } else {
                                    result_peg_393.setPosition(position_peg_397);
                                    goto out_peg_398;
                                }
                            }
                                
                        }
                        goto success_peg_394;
                        out_peg_398:
                        {
                            int position_peg_399 = result_peg_393.getPosition();
                            
                            result_peg_393.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_393.getPosition()) == (unsigned char) 255){
                                result_peg_393.nextPosition();
                            } else {
                                result_peg_393.setPosition(position_peg_399);
                                goto out_peg_400;
                            }
                            
                        }
                        goto success_peg_394;
                        out_peg_400:
                        goto loop_peg_392;
                        success_peg_394:
                        ;
                        result_peg_295.addResult(result_peg_393);
                    } while (true);
                    loop_peg_392:
                    ;
                            
                }
                goto success_peg_390;
                goto out_peg_297;
                success_peg_390:
                ;
            
            
            
            result_peg_295.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
            
            
            
            {
                    int position_peg_404 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_406(result_peg_295.getPosition());
                        {
                            int position_peg_408 = result_peg_406.getPosition();
                            
                            result_peg_406.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_406.getPosition()))){
                                    result_peg_406.nextPosition();
                                } else {
                                    result_peg_406.setPosition(position_peg_408);
                                    goto out_peg_409;
                                }
                            }
                                
                        }
                        goto success_peg_407;
                        out_peg_409:
                        {
                            int position_peg_410 = result_peg_406.getPosition();
                            
                            result_peg_406.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_406.getPosition()))){
                                    result_peg_406.nextPosition();
                                } else {
                                    result_peg_406.setPosition(position_peg_410);
                                    goto out_peg_411;
                                }
                            }
                                
                        }
                        goto success_peg_407;
                        out_peg_411:
                        {
                            int position_peg_412 = result_peg_406.getPosition();
                            
                            result_peg_406.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_406.getPosition()) == (unsigned char) 255){
                                result_peg_406.nextPosition();
                            } else {
                                result_peg_406.setPosition(position_peg_412);
                                goto out_peg_413;
                            }
                            
                        }
                        goto success_peg_407;
                        out_peg_413:
                        goto loop_peg_405;
                        success_peg_407:
                        ;
                        result_peg_295.addResult(result_peg_406);
                    } while (true);
                    loop_peg_405:
                    ;
                            
                }
                goto success_peg_403;
                goto out_peg_297;
                success_peg_403:
                ;
            
            
            
            result_peg_295 = rule_integer(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
                n3 = result_peg_295.getValues();
            
            
            
            {
                    int position_peg_417 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_419(result_peg_295.getPosition());
                        {
                            int position_peg_421 = result_peg_419.getPosition();
                            
                            result_peg_419.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_419.getPosition()))){
                                    result_peg_419.nextPosition();
                                } else {
                                    result_peg_419.setPosition(position_peg_421);
                                    goto out_peg_422;
                                }
                            }
                                
                        }
                        goto success_peg_420;
                        out_peg_422:
                        {
                            int position_peg_423 = result_peg_419.getPosition();
                            
                            result_peg_419.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_419.getPosition()))){
                                    result_peg_419.nextPosition();
                                } else {
                                    result_peg_419.setPosition(position_peg_423);
                                    goto out_peg_424;
                                }
                            }
                                
                        }
                        goto success_peg_420;
                        out_peg_424:
                        {
                            int position_peg_425 = result_peg_419.getPosition();
                            
                            result_peg_419.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_419.getPosition()) == (unsigned char) 255){
                                result_peg_419.nextPosition();
                            } else {
                                result_peg_419.setPosition(position_peg_425);
                                goto out_peg_426;
                            }
                            
                        }
                        goto success_peg_420;
                        out_peg_426:
                        goto loop_peg_418;
                        success_peg_420:
                        ;
                        result_peg_295.addResult(result_peg_419);
                    } while (true);
                    loop_peg_418:
                    ;
                            
                }
                goto success_peg_416;
                goto out_peg_297;
                success_peg_416:
                ;
            
            
            
            result_peg_295.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_295.getPosition()))){
                        result_peg_295.nextPosition();
                    } else {
                        goto out_peg_297;
                    }
                }
            
            
            
            {
                    int position_peg_430 = result_peg_295.getPosition();
                    
                    result_peg_295.reset();
                    do{
                        Result result_peg_432(result_peg_295.getPosition());
                        {
                            int position_peg_434 = result_peg_432.getPosition();
                            
                            result_peg_432.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_432.getPosition()))){
                                    result_peg_432.nextPosition();
                                } else {
                                    result_peg_432.setPosition(position_peg_434);
                                    goto out_peg_435;
                                }
                            }
                                
                        }
                        goto success_peg_433;
                        out_peg_435:
                        {
                            int position_peg_436 = result_peg_432.getPosition();
                            
                            result_peg_432.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_432.getPosition()))){
                                    result_peg_432.nextPosition();
                                } else {
                                    result_peg_432.setPosition(position_peg_436);
                                    goto out_peg_437;
                                }
                            }
                                
                        }
                        goto success_peg_433;
                        out_peg_437:
                        {
                            int position_peg_438 = result_peg_432.getPosition();
                            
                            result_peg_432.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_432.getPosition()) == (unsigned char) 255){
                                result_peg_432.nextPosition();
                            } else {
                                result_peg_432.setPosition(position_peg_438);
                                goto out_peg_439;
                            }
                            
                        }
                        goto success_peg_433;
                        out_peg_439:
                        goto loop_peg_431;
                        success_peg_433:
                        ;
                        result_peg_295.addResult(result_peg_432);
                    } while (true);
                    loop_peg_431:
                    ;
                            
                }
                goto success_peg_429;
                goto out_peg_297;
                success_peg_429:
                ;
            
            
            
            result_peg_295 = rule_integer(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
                n4 = result_peg_295.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_295.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision = result_peg_295;
        stream.update(result_peg_295.getPosition());
        
        
        return result_peg_295;
        out_peg_297:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_collision = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_action_start(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_action_start.calculated()){
        return column_peg_2.chunk2->chunk_action_start;
    }
    
    
    RuleTrace trace_peg_1(stream, "action_start");
    int myposition = position;
    
    
    Value begin;
        Value action;
        Value num;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_8 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
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
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_5;
                success_peg_7:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "Begin"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Begin"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
                begin = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_21 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_23(result_peg_3.getPosition());
                        {
                            int position_peg_25 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_23.getPosition()))){
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
                            
                            result_peg_23.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_23.getPosition()))){
                                    result_peg_23.nextPosition();
                                } else {
                                    result_peg_23.setPosition(position_peg_27);
                                    goto out_peg_28;
                                }
                            }
                                
                        }
                        goto success_peg_24;
                        out_peg_28:
                        {
                            int position_peg_29 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_23.getPosition()) == (unsigned char) 255){
                                result_peg_23.nextPosition();
                            } else {
                                result_peg_23.setPosition(position_peg_29);
                                goto out_peg_30;
                            }
                            
                        }
                        goto success_peg_24;
                        out_peg_30:
                        goto loop_peg_22;
                        success_peg_24:
                        ;
                        result_peg_3.addResult(result_peg_23);
                    } while (true);
                    loop_peg_22:
                    ;
                            
                }
                goto success_peg_20;
                goto out_peg_5;
                success_peg_20:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "Action"));
                for (int i = 0; i < 6; i++){
                    if (compareCharCase("Action"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
                action = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_34 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_36(result_peg_3.getPosition());
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
                        result_peg_3.addResult(result_peg_36);
                    } while (true);
                    loop_peg_35:
                    ;
                            
                }
                goto success_peg_33;
                goto out_peg_5;
                success_peg_33:
                ;
            
            
            
            result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                num = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_47 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_49(result_peg_3.getPosition());
                        {
                            int position_peg_51 = result_peg_49.getPosition();
                            
                            result_peg_49.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_49.getPosition()))){
                                    result_peg_49.nextPosition();
                                } else {
                                    result_peg_49.setPosition(position_peg_51);
                                    goto out_peg_52;
                                }
                            }
                                
                        }
                        goto success_peg_50;
                        out_peg_52:
                        {
                            int position_peg_53 = result_peg_49.getPosition();
                            
                            result_peg_49.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_49.getPosition()))){
                                    result_peg_49.nextPosition();
                                } else {
                                    result_peg_49.setPosition(position_peg_53);
                                    goto out_peg_54;
                                }
                            }
                                
                        }
                        goto success_peg_50;
                        out_peg_54:
                        {
                            int position_peg_55 = result_peg_49.getPosition();
                            
                            result_peg_49.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_49.getPosition()) == (unsigned char) 255){
                                result_peg_49.nextPosition();
                            } else {
                                result_peg_49.setPosition(position_peg_55);
                                goto out_peg_56;
                            }
                            
                        }
                        goto success_peg_50;
                        out_peg_56:
                        goto loop_peg_48;
                        success_peg_50:
                        ;
                        result_peg_3.addResult(result_peg_49);
                    } while (true);
                    loop_peg_48:
                    ;
                            
                }
                goto success_peg_46;
                goto out_peg_5;
                success_peg_46:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_59(result_peg_3.getPosition());
                    {
                    
                        Result result_peg_62(result_peg_59);
                            result_peg_62.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    goto not_peg_61;
                                }
                            }
                            goto loop_peg_58;
                            not_peg_61:
                            result_peg_59.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_63 = stream.get(result_peg_59.getPosition());
                            if (temp_peg_63 != '\0'){
                                result_peg_59.setValue(Value((void*) (intptr_t) temp_peg_63));
                                result_peg_59.nextPosition();
                            } else {
                                goto loop_peg_58;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_59);
                } while (true);
                loop_peg_58:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHeader(begin, action, num);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_action_start = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_action_start = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_integer.calculated()){
        return column_peg_2.chunk2->chunk_integer;
    }
    
    
    RuleTrace trace_peg_1(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            int save_peg_5 = result_peg_3.getPosition();
                
                {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_7);
                            goto out_peg_8;
                        }
                    }
                        
                }
                goto success_peg_6;
                out_peg_8:
                {
                    int position_peg_9 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_9);
                            goto out_peg_10;
                        }
                    }
                        
                }
                goto success_peg_6;
                out_peg_10:
                
                result_peg_3 = Result(save_peg_5);
                result_peg_3.setValue(Value((void*) 0));
                
                success_peg_6:
                ;
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_13(result_peg_3.getPosition());
                    {
                        int position_peg_15 = result_peg_13.getPosition();
                        
                        char letter_peg_16 = stream.get(result_peg_13.getPosition());
                        if (letter_peg_16 != '\0' && strchr("0123456789", letter_peg_16) != NULL){
                            result_peg_13.nextPosition();
                            result_peg_13.setValue(Value((void*) (intptr_t) letter_peg_16));
                        } else {
                            result_peg_13.setPosition(position_peg_15);
                            goto out_peg_17;
                        }
                        
                    }
                    goto success_peg_14;
                    out_peg_17:
                    goto loop_peg_12;
                    success_peg_14:
                    ;
                    result_peg_3.addResult(result_peg_13);
                } while (true);
                loop_peg_12:
                if (result_peg_3.matches() == 0){
                    goto out_peg_18;
                }
            
            Result result_peg_11 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_11.getValues()); value = makeNumber(result_peg_4.getValues(),value);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_integer = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_18:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk2 != 0 && column_peg_2.chunk2->chunk_valuelist.calculated()){
        return column_peg_2.chunk2->chunk_valuelist;
    }
    
    
    RuleTrace trace_peg_1(stream, "valuelist");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_value(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_8(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_11 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_13(result_peg_8.getPosition());
                                    {
                                        int position_peg_15 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_15);
                                                goto out_peg_16;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_16:
                                    {
                                        int position_peg_17 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_17);
                                                goto out_peg_18;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_14;
                                    out_peg_18:
                                    {
                                        int position_peg_19 = result_peg_13.getPosition();
                                        
                                        result_peg_13.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_13.getPosition()) == (unsigned char) 255){
                                            result_peg_13.nextPosition();
                                        } else {
                                            result_peg_13.setPosition(position_peg_19);
                                            goto out_peg_20;
                                        }
                                        
                                    }
                                    goto success_peg_14;
                                    out_peg_20:
                                    goto loop_peg_12;
                                    success_peg_14:
                                    ;
                                    result_peg_8.addResult(result_peg_13);
                                } while (true);
                                loop_peg_12:
                                ;
                                        
                            }
                            goto success_peg_10;
                            goto loop_peg_7;
                            success_peg_10:
                            ;
                        
                        
                        
                        result_peg_8.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    goto loop_peg_7;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_24 = result_peg_8.getPosition();
                                
                                result_peg_8.reset();
                                do{
                                    Result result_peg_26(result_peg_8.getPosition());
                                    {
                                        int position_peg_28 = result_peg_26.getPosition();
                                        
                                        result_peg_26.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_26.getPosition()))){
                                                result_peg_26.nextPosition();
                                            } else {
                                                result_peg_26.setPosition(position_peg_28);
                                                goto out_peg_29;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_27;
                                    out_peg_29:
                                    {
                                        int position_peg_30 = result_peg_26.getPosition();
                                        
                                        result_peg_26.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_26.getPosition()))){
                                                result_peg_26.nextPosition();
                                            } else {
                                                result_peg_26.setPosition(position_peg_30);
                                                goto out_peg_31;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_27;
                                    out_peg_31:
                                    {
                                        int position_peg_32 = result_peg_26.getPosition();
                                        
                                        result_peg_26.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_26.getPosition()) == (unsigned char) 255){
                                            result_peg_26.nextPosition();
                                        } else {
                                            result_peg_26.setPosition(position_peg_32);
                                            goto out_peg_33;
                                        }
                                        
                                    }
                                    goto success_peg_27;
                                    out_peg_33:
                                    goto loop_peg_25;
                                    success_peg_27:
                                    ;
                                    result_peg_8.addResult(result_peg_26);
                                } while (true);
                                loop_peg_25:
                                ;
                                        
                            }
                            goto success_peg_23;
                            goto loop_peg_7;
                            success_peg_23:
                            ;
                        
                        
                        
                        int save_peg_34 = result_peg_8.getPosition();
                            
                            result_peg_8 = rule_value(stream, result_peg_8.getPosition());
                            if (result_peg_8.error()){
                                
                                result_peg_8 = Result(save_peg_34);
                                result_peg_8.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
            
            Result result_peg_6 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeValueList(result_peg_4.getValues(),result_peg_6.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_valuelist = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_5:
    
        if (column_peg_2.chunk2 == 0){
            column_peg_2.chunk2 = new Chunk2();
        }
        column_peg_2.chunk2->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2 = stream.getColumn(position);
    if (column_peg_2.chunk3 != 0 && column_peg_2.chunk3->chunk_value.calculated()){
        return column_peg_2.chunk3->chunk_value;
    }
    
    
    RuleTrace trace_peg_1(stream, "value");
    int myposition = position;
    
    
    Value source;
        Value dest;
    Result result_peg_3(myposition);
        
        
        result_peg_3 = rule_integer(stream, result_peg_3.getPosition());
        if (result_peg_3.error()){
            goto out_peg_4;
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_3;
        stream.update(result_peg_3.getPosition());
        
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        {
        
            result_peg_5.setValue(Value((void*) "as"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("as"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
            
            
            
            result_peg_5 = rule_integer(stream, result_peg_5.getPosition());
                if (result_peg_5.error()){
                    goto out_peg_7;
                }
                source = result_peg_5.getValues();
            
            
            
            result_peg_5.setValue(Value((void*) "d"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("d"[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
            
            
            
            result_peg_5 = rule_integer(stream, result_peg_5.getPosition());
                if (result_peg_5.error()){
                    goto out_peg_7;
                }
                dest = result_peg_5.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAlphaSourceDest(source, dest);
                    result_peg_5.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_5;
        stream.update(result_peg_5.getPosition());
        
        
        return result_peg_5;
        out_peg_7:
        Result result_peg_11(myposition);
        
        
        {
        
            result_peg_11.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_11.getPosition()))){
                        result_peg_11.nextPosition();
                    } else {
                        goto out_peg_13;
                    }
                }
            
            
            
            result_peg_11 = rule_integer(stream, result_peg_11.getPosition());
                if (result_peg_11.error()){
                    goto out_peg_13;
                }
                source = result_peg_11.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAlpha("a", source);
                    result_peg_11.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_13:
        Result result_peg_15(myposition);
        
        
        {
        
            result_peg_15.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_15.getPosition()))){
                        result_peg_15.nextPosition();
                    } else {
                        goto out_peg_17;
                    }
                }
            
            Result result_peg_16 = result_peg_15;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_16.getValues());
                    result_peg_15.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_15;
        stream.update(result_peg_15.getPosition());
        
        
        return result_peg_15;
        out_peg_17:
        Result result_peg_18(myposition);
        
        
        {
        
            result_peg_18.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_18.getPosition()))){
                        result_peg_18.nextPosition();
                    } else {
                        goto out_peg_20;
                    }
                }
            
            
            
            result_peg_18 = rule_integer(stream, result_peg_18.getPosition());
                if (result_peg_18.error()){
                    goto out_peg_20;
                }
                source = result_peg_18.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAlpha("s", source);
                    result_peg_18.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        return result_peg_18;
        out_peg_20:
        Result result_peg_22(myposition);
        
        
        {
        
            result_peg_22.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_22.getPosition()))){
                        result_peg_22.nextPosition();
                    } else {
                        goto out_peg_24;
                    }
                }
            
            Result result_peg_23 = result_peg_22;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_23.getValues());
                    result_peg_22.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_22;
        stream.update(result_peg_22.getPosition());
        
        
        return result_peg_22;
        out_peg_24:
        Result result_peg_25(myposition);
        
        
        {
        
            result_peg_25.setValue(Value((void*) "vh"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("vh"[i], stream.get(result_peg_25.getPosition()))){
                        result_peg_25.nextPosition();
                    } else {
                        goto out_peg_27;
                    }
                }
            
            Result result_peg_26 = result_peg_25;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_26.getValues());
                    result_peg_25.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_25;
        stream.update(result_peg_25.getPosition());
        
        
        return result_peg_25;
        out_peg_27:
        Result result_peg_28(myposition);
        
        
        {
        
            result_peg_28.setValue(Value((void*) "hv"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("hv"[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_30;
                    }
                }
            
            Result result_peg_29 = result_peg_28;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_29.getValues());
                    result_peg_28.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_30:
        Result result_peg_31(myposition);
        
        
        {
        
            result_peg_31.setValue(Value((void*) "v"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("v"[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto out_peg_33;
                    }
                }
            
            Result result_peg_32 = result_peg_31;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_32.getValues());
                    result_peg_31.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_31;
        stream.update(result_peg_31.getPosition());
        
        
        return result_peg_31;
        out_peg_33:
        Result result_peg_34(myposition);
        
        
        {
        
            result_peg_34.setValue(Value((void*) "h"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_34.getPosition()))){
                        result_peg_34.nextPosition();
                    } else {
                        goto out_peg_36;
                    }
                }
            
            Result result_peg_35 = result_peg_34;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_35.getValues());
                    result_peg_34.setValue(value);
                }
            
            
        }
        
        
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = result_peg_34;
        stream.update(result_peg_34.getPosition());
        
        
        return result_peg_34;
        out_peg_36:
    
        if (column_peg_2.chunk3 == 0){
            column_peg_2.chunk3 = new Chunk3();
        }
        column_peg_2.chunk3->chunk_value = errorResult;
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

    
