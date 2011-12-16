

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
        out << makeBacktrace() << std::endl;
        throw ParseException(out.str(), line, column);
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
    
    
    RuleTrace trace_peg_25(stream, "start");
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
                        int position_peg_11 = result_peg_8.getPosition();
                        
                        result_peg_8.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                result_peg_8.setPosition(position_peg_11);
                                goto out_peg_12;
                            }
                        }
                            
                    }
                    goto success_peg_9;
                    out_peg_12:
                    {
                        int position_peg_14 = result_peg_8.getPosition();
                        
                        result_peg_8.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                result_peg_8.setPosition(position_peg_14);
                                goto out_peg_15;
                            }
                        }
                            
                    }
                    goto success_peg_9;
                    out_peg_15:
                    goto loop_peg_7;
                    success_peg_9:
                    ;
                    result_peg_2.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_18(result_peg_2.getPosition());
                    {
                    
                        result_peg_18 = rule_sn(stream, result_peg_18.getPosition());
                            if (result_peg_18.error()){
                                goto loop_peg_17;
                            }
                        
                        
                        
                        result_peg_18 = rule_line(stream, result_peg_18.getPosition(), current);
                            if (result_peg_18.error()){
                                goto loop_peg_17;
                            }
                        
                        
                        
                        result_peg_18 = rule_whitespace(stream, result_peg_18.getPosition());
                            if (result_peg_18.error()){
                                goto loop_peg_17;
                            }
                        
                        
                        
                        int save_peg_22 = result_peg_18.getPosition();
                            
                            result_peg_18 = rule_line_end(stream, result_peg_18.getPosition());
                            if (result_peg_18.error()){
                                
                                result_peg_18 = Result(save_peg_22);
                                result_peg_18.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_18);
                } while (true);
                loop_peg_17:
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
    
    
    RuleTrace trace_peg_63(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_20(result_peg_2.getPosition());
                        {
                            int position_peg_23 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_24:
                        {
                            int position_peg_26 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_26);
                                    goto out_peg_27;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_27:
                        {
                            int position_peg_29 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_20.getPosition()) == (unsigned char) 255){
                                result_peg_20.nextPosition();
                            } else {
                                result_peg_20.setPosition(position_peg_29);
                                goto out_peg_30;
                            }
                            
                        }
                        goto success_peg_21;
                        out_peg_30:
                        goto loop_peg_19;
                        success_peg_21:
                        ;
                        result_peg_2.addResult(result_peg_20);
                    } while (true);
                    loop_peg_19:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_31;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_31;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_31:
        Result result_peg_32(myposition);
        
        
        {
        
            {
                    int position_peg_36 = result_peg_32.getPosition();
                    
                    result_peg_32.reset();
                    do{
                        Result result_peg_50(result_peg_32.getPosition());
                        {
                            int position_peg_53 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_53);
                                    goto out_peg_54;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_54:
                        {
                            int position_peg_56 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_56);
                                    goto out_peg_57;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_57:
                        {
                            int position_peg_59 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_50.getPosition()) == (unsigned char) 255){
                                result_peg_50.nextPosition();
                            } else {
                                result_peg_50.setPosition(position_peg_59);
                                goto out_peg_60;
                            }
                            
                        }
                        goto success_peg_51;
                        out_peg_60:
                        goto loop_peg_49;
                        success_peg_51:
                        ;
                        result_peg_32.addResult(result_peg_50);
                    } while (true);
                    loop_peg_49:
                    ;
                            
                }
                goto success_peg_34;
                goto out_peg_61;
                success_peg_34:
                ;
            
            
            
            result_peg_32 = rule_action(stream, result_peg_32.getPosition());
                if (result_peg_32.error()){
                    goto out_peg_61;
                }
            
            Result result_peg_62 = result_peg_32;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_62.getValues());
                    result_peg_32.setValue(value);
                }
            
            
        }
        stream.update(result_peg_32.getPosition());
        
        
        return result_peg_32;
        out_peg_61:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_action.calculated()){
        return column_peg_1.chunk0->chunk_action;
    }
    
    
    RuleTrace trace_peg_22(stream, "action");
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
                        int position_peg_12 = result_peg_9.getPosition();
                        
                        result_peg_9.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_12);
                                goto out_peg_13;
                            }
                        }
                            
                    }
                    goto success_peg_10;
                    out_peg_13:
                    {
                        int position_peg_15 = result_peg_9.getPosition();
                        
                        result_peg_9.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_9.getPosition()))){
                                result_peg_9.nextPosition();
                            } else {
                                result_peg_9.setPosition(position_peg_15);
                                goto out_peg_16;
                            }
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
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_19(result_peg_2.getPosition());
                    {
                    
                        result_peg_19 = rule_action_line(stream, result_peg_19.getPosition(), ast);
                            if (result_peg_19.error()){
                                goto loop_peg_18;
                            }
                        
                        
                        
                        result_peg_19 = rule_whitespace(stream, result_peg_19.getPosition());
                            if (result_peg_19.error()){
                                goto loop_peg_18;
                            }
                        
                        
                        
                        result_peg_19 = rule_line_end(stream, result_peg_19.getPosition());
                            if (result_peg_19.error()){
                                goto loop_peg_18;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_19);
                } while (true);
                loop_peg_18:
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
    
    
    RuleTrace trace_peg_250(stream, "action_line");
    int myposition = position;
    
    
    Value data;
        Value line;
    Result result_peg_2(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_20(result_peg_2.getPosition());
                        {
                            int position_peg_23 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_24:
                        {
                            int position_peg_26 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_26);
                                    goto out_peg_27;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_27:
                        {
                            int position_peg_29 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_20.getPosition()) == (unsigned char) 255){
                                result_peg_20.nextPosition();
                            } else {
                                result_peg_20.setPosition(position_peg_29);
                                goto out_peg_30;
                            }
                            
                        }
                        goto success_peg_21;
                        out_peg_30:
                        goto loop_peg_19;
                        success_peg_21:
                        ;
                        result_peg_2.addResult(result_peg_20);
                    } while (true);
                    loop_peg_19:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_31;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_31;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_31:
        Result result_peg_32(myposition);
        
        
        {
        
            {
                    int position_peg_36 = result_peg_32.getPosition();
                    
                    result_peg_32.reset();
                    do{
                        Result result_peg_50(result_peg_32.getPosition());
                        {
                            int position_peg_53 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_53);
                                    goto out_peg_54;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_54:
                        {
                            int position_peg_56 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_56);
                                    goto out_peg_57;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_57:
                        {
                            int position_peg_59 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_50.getPosition()) == (unsigned char) 255){
                                result_peg_50.nextPosition();
                            } else {
                                result_peg_50.setPosition(position_peg_59);
                                goto out_peg_60;
                            }
                            
                        }
                        goto success_peg_51;
                        out_peg_60:
                        goto loop_peg_49;
                        success_peg_51:
                        ;
                        result_peg_32.addResult(result_peg_50);
                    } while (true);
                    loop_peg_49:
                    ;
                            
                }
                goto success_peg_34;
                goto out_peg_61;
                success_peg_34:
                ;
            
            
            
            result_peg_32 = rule_collision_default(stream, result_peg_32.getPosition());
                if (result_peg_32.error()){
                    goto out_peg_61;
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
        out_peg_61:
        Result result_peg_63(myposition);
        
        
        {
        
            {
                    int position_peg_67 = result_peg_63.getPosition();
                    
                    result_peg_63.reset();
                    do{
                        Result result_peg_81(result_peg_63.getPosition());
                        {
                            int position_peg_84 = result_peg_81.getPosition();
                            
                            result_peg_81.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_81.getPosition()))){
                                    result_peg_81.nextPosition();
                                } else {
                                    result_peg_81.setPosition(position_peg_84);
                                    goto out_peg_85;
                                }
                            }
                                
                        }
                        goto success_peg_82;
                        out_peg_85:
                        {
                            int position_peg_87 = result_peg_81.getPosition();
                            
                            result_peg_81.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_81.getPosition()))){
                                    result_peg_81.nextPosition();
                                } else {
                                    result_peg_81.setPosition(position_peg_87);
                                    goto out_peg_88;
                                }
                            }
                                
                        }
                        goto success_peg_82;
                        out_peg_88:
                        {
                            int position_peg_90 = result_peg_81.getPosition();
                            
                            result_peg_81.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_81.getPosition()) == (unsigned char) 255){
                                result_peg_81.nextPosition();
                            } else {
                                result_peg_81.setPosition(position_peg_90);
                                goto out_peg_91;
                            }
                            
                        }
                        goto success_peg_82;
                        out_peg_91:
                        goto loop_peg_80;
                        success_peg_82:
                        ;
                        result_peg_63.addResult(result_peg_81);
                    } while (true);
                    loop_peg_80:
                    ;
                            
                }
                goto success_peg_65;
                goto out_peg_92;
                success_peg_65:
                ;
            
            
            
            result_peg_63 = rule_collision(stream, result_peg_63.getPosition());
                if (result_peg_63.error()){
                    goto out_peg_92;
                }
                data = result_peg_63.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_63.setValue(value);
                }
            
            
        }
        stream.update(result_peg_63.getPosition());
        
        
        return result_peg_63;
        out_peg_92:
        Result result_peg_94(myposition);
        
        
        {
        
            {
                    int position_peg_98 = result_peg_94.getPosition();
                    
                    result_peg_94.reset();
                    do{
                        Result result_peg_112(result_peg_94.getPosition());
                        {
                            int position_peg_115 = result_peg_112.getPosition();
                            
                            result_peg_112.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_112.getPosition()))){
                                    result_peg_112.nextPosition();
                                } else {
                                    result_peg_112.setPosition(position_peg_115);
                                    goto out_peg_116;
                                }
                            }
                                
                        }
                        goto success_peg_113;
                        out_peg_116:
                        {
                            int position_peg_118 = result_peg_112.getPosition();
                            
                            result_peg_112.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_112.getPosition()))){
                                    result_peg_112.nextPosition();
                                } else {
                                    result_peg_112.setPosition(position_peg_118);
                                    goto out_peg_119;
                                }
                            }
                                
                        }
                        goto success_peg_113;
                        out_peg_119:
                        {
                            int position_peg_121 = result_peg_112.getPosition();
                            
                            result_peg_112.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_112.getPosition()) == (unsigned char) 255){
                                result_peg_112.nextPosition();
                            } else {
                                result_peg_112.setPosition(position_peg_121);
                                goto out_peg_122;
                            }
                            
                        }
                        goto success_peg_113;
                        out_peg_122:
                        goto loop_peg_111;
                        success_peg_113:
                        ;
                        result_peg_94.addResult(result_peg_112);
                    } while (true);
                    loop_peg_111:
                    ;
                            
                }
                goto success_peg_96;
                goto out_peg_123;
                success_peg_96:
                ;
            
            
            
            result_peg_94 = rule_valuelist(stream, result_peg_94.getPosition());
                if (result_peg_94.error()){
                    goto out_peg_123;
                }
                data = result_peg_94.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                    result_peg_94.setValue(value);
                }
            
            
        }
        stream.update(result_peg_94.getPosition());
        
        
        return result_peg_94;
        out_peg_123:
        Result result_peg_125(myposition);
        
        
        {
        
            {
                    int position_peg_129 = result_peg_125.getPosition();
                    
                    result_peg_125.reset();
                    do{
                        Result result_peg_143(result_peg_125.getPosition());
                        {
                            int position_peg_146 = result_peg_143.getPosition();
                            
                            result_peg_143.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    result_peg_143.setPosition(position_peg_146);
                                    goto out_peg_147;
                                }
                            }
                                
                        }
                        goto success_peg_144;
                        out_peg_147:
                        {
                            int position_peg_149 = result_peg_143.getPosition();
                            
                            result_peg_143.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    result_peg_143.setPosition(position_peg_149);
                                    goto out_peg_150;
                                }
                            }
                                
                        }
                        goto success_peg_144;
                        out_peg_150:
                        {
                            int position_peg_152 = result_peg_143.getPosition();
                            
                            result_peg_143.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_143.getPosition()) == (unsigned char) 255){
                                result_peg_143.nextPosition();
                            } else {
                                result_peg_143.setPosition(position_peg_152);
                                goto out_peg_153;
                            }
                            
                        }
                        goto success_peg_144;
                        out_peg_153:
                        goto loop_peg_142;
                        success_peg_144:
                        ;
                        result_peg_125.addResult(result_peg_143);
                    } while (true);
                    loop_peg_142:
                    ;
                            
                }
                goto success_peg_127;
                goto out_peg_154;
                success_peg_127:
                ;
            
            
            
            {
                    int position_peg_158 = result_peg_125.getPosition();
                    {
                    
                        result_peg_125.setValue(Value((void*) "loopstart"));
                            for (int i = 0; i < 9; i++){
                                if (compareCharCase("loopstart"[i], stream.get(result_peg_125.getPosition()))){
                                    result_peg_125.nextPosition();
                                } else {
                                    result_peg_125.setPosition(position_peg_158);
                                    goto out_peg_160;
                                }
                            }
                        
                        Result result_peg_163 = result_peg_125;
                        
                        int save_peg_165 = result_peg_125.getPosition();
                            
                            result_peg_125.setValue(Value((void*) ":"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(":"[i], stream.get(result_peg_125.getPosition()))){
                                    result_peg_125.nextPosition();
                                } else {
                                    
                                    result_peg_125 = Result(save_peg_165);
                                    result_peg_125.setValue(Value((void*) 0));
                                    
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = makeAttributeKeyword(Value(makeKeyword(result_peg_163.getValues())));
                                result_peg_125.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_156;
                out_peg_160:
                goto out_peg_154;
                success_peg_156:
                ;
                data = result_peg_125.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_125.setValue(value);
                }
            
            
        }
        stream.update(result_peg_125.getPosition());
        
        
        return result_peg_125;
        out_peg_154:
        Result result_peg_166(myposition);
        
        
        {
        
            {
                    int position_peg_170 = result_peg_166.getPosition();
                    
                    result_peg_166.setValue(Value((void*) " "));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(" "[i], stream.get(result_peg_166.getPosition()))){
                            result_peg_166.nextPosition();
                        } else {
                            result_peg_166.setPosition(position_peg_170);
                            goto out_peg_171;
                        }
                    }
                        
                }
                goto success_peg_168;
                out_peg_171:
                {
                    int position_peg_173 = result_peg_166.getPosition();
                    
                    result_peg_166.setValue(Value((void*) "\t"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\t"[i], stream.get(result_peg_166.getPosition()))){
                            result_peg_166.nextPosition();
                        } else {
                            result_peg_166.setPosition(position_peg_173);
                            goto out_peg_174;
                        }
                    }
                        
                }
                goto success_peg_168;
                out_peg_174:
                {
                    int position_peg_176 = result_peg_166.getPosition();
                    
                    result_peg_166.setValue(Value((void*) 255));
                    if ((unsigned char) stream.get(result_peg_166.getPosition()) == (unsigned char) 255){
                        result_peg_166.nextPosition();
                    } else {
                        result_peg_166.setPosition(position_peg_176);
                        goto out_peg_177;
                    }
                    
                }
                goto success_peg_168;
                out_peg_177:
                goto out_peg_178;
                success_peg_168:
                ;
            
            
            
            {
                    int position_peg_181 = result_peg_166.getPosition();
                    
                    result_peg_166.reset();
                    do{
                        Result result_peg_195(result_peg_166.getPosition());
                        {
                            int position_peg_198 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_195.getPosition()))){
                                    result_peg_195.nextPosition();
                                } else {
                                    result_peg_195.setPosition(position_peg_198);
                                    goto out_peg_199;
                                }
                            }
                                
                        }
                        goto success_peg_196;
                        out_peg_199:
                        {
                            int position_peg_201 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_195.getPosition()))){
                                    result_peg_195.nextPosition();
                                } else {
                                    result_peg_195.setPosition(position_peg_201);
                                    goto out_peg_202;
                                }
                            }
                                
                        }
                        goto success_peg_196;
                        out_peg_202:
                        {
                            int position_peg_204 = result_peg_195.getPosition();
                            
                            result_peg_195.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_195.getPosition()) == (unsigned char) 255){
                                result_peg_195.nextPosition();
                            } else {
                                result_peg_195.setPosition(position_peg_204);
                                goto out_peg_205;
                            }
                            
                        }
                        goto success_peg_196;
                        out_peg_205:
                        goto loop_peg_194;
                        success_peg_196:
                        ;
                        result_peg_166.addResult(result_peg_195);
                    } while (true);
                    loop_peg_194:
                    ;
                            
                }
                goto success_peg_179;
                goto out_peg_178;
                success_peg_179:
                ;
            
            
        }
        stream.update(result_peg_166.getPosition());
        
        
        return result_peg_166;
        out_peg_178:
        Result result_peg_206(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_208 = stream.getLineInfo(result_peg_206.getPosition());
                line = &line_info_peg_208;
            
            
            
            Result result_peg_211(result_peg_206);
                {
                
                    {
                            int position_peg_215 = result_peg_211.getPosition();
                            
                            result_peg_211.reset();
                            do{
                                Result result_peg_229(result_peg_211.getPosition());
                                {
                                    int position_peg_232 = result_peg_229.getPosition();
                                    
                                    result_peg_229.setValue(Value((void*) " "));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_229.getPosition()))){
                                            result_peg_229.nextPosition();
                                        } else {
                                            result_peg_229.setPosition(position_peg_232);
                                            goto out_peg_233;
                                        }
                                    }
                                        
                                }
                                goto success_peg_230;
                                out_peg_233:
                                {
                                    int position_peg_235 = result_peg_229.getPosition();
                                    
                                    result_peg_229.setValue(Value((void*) "\t"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_229.getPosition()))){
                                            result_peg_229.nextPosition();
                                        } else {
                                            result_peg_229.setPosition(position_peg_235);
                                            goto out_peg_236;
                                        }
                                    }
                                        
                                }
                                goto success_peg_230;
                                out_peg_236:
                                {
                                    int position_peg_238 = result_peg_229.getPosition();
                                    
                                    result_peg_229.setValue(Value((void*) 255));
                                    if ((unsigned char) stream.get(result_peg_229.getPosition()) == (unsigned char) 255){
                                        result_peg_229.nextPosition();
                                    } else {
                                        result_peg_229.setPosition(position_peg_238);
                                        goto out_peg_239;
                                    }
                                    
                                }
                                goto success_peg_230;
                                out_peg_239:
                                goto loop_peg_228;
                                success_peg_230:
                                ;
                                result_peg_211.addResult(result_peg_229);
                            } while (true);
                            loop_peg_228:
                            ;
                                    
                        }
                        goto success_peg_213;
                        goto not_peg_210;
                        success_peg_213:
                        ;
                    
                    
                    
                    result_peg_211 = rule_action_start(stream, result_peg_211.getPosition());
                        if (result_peg_211.error()){
                            goto not_peg_210;
                        }
                    
                    
                }
                goto out_peg_240;
                not_peg_210:
                result_peg_206.setValue(Value((void*)0));
            
            
            
            result_peg_206.reset();
                do{
                    Result result_peg_243(result_peg_206.getPosition());
                    {
                    
                        Result result_peg_246(result_peg_243);
                            result_peg_246 = rule_line_end(stream, result_peg_246.getPosition());
                            if (result_peg_246.error()){
                                goto not_peg_245;
                            }
                            goto loop_peg_242;
                            not_peg_245:
                            result_peg_243.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_247 = stream.get(result_peg_243.getPosition());
                            if (temp_peg_247 != '\0'){
                                result_peg_243.setValue(Value((void*) (long) temp_peg_247));
                                result_peg_243.nextPosition();
                            } else {
                                goto loop_peg_242;
                            }
                        
                        
                    }
                    result_peg_206.addResult(result_peg_243);
                } while (true);
                loop_peg_242:
                if (result_peg_206.matches() == 0){
                    goto out_peg_240;
                }
            
            
            
            Result result_peg_249(result_peg_206.getPosition());
                result_peg_249 = rule_line_end(stream, result_peg_249.getPosition());
                if (result_peg_249.error()){
                    goto out_peg_240;
                }
            
            
            
            {
                    Value value((void*) 0);
                    std::cout << "Warning: ignoring input at line " << getCurrentLine(line) << " column " << getCurrentColumn(line) << std::endl;
                    result_peg_206.setValue(value);
                }
            
            
        }
        stream.update(result_peg_206.getPosition());
        
        
        return result_peg_206;
        out_peg_240:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_line_end.calculated()){
        return column_peg_1.chunk0->chunk_line_end;
    }
    
    
    RuleTrace trace_peg_17(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            {
                int position_peg_7 = result_peg_4.getPosition();
                
                result_peg_4.setValue(Value((void*) "\n"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        result_peg_4.setPosition(position_peg_7);
                        goto out_peg_8;
                    }
                }
                    
            }
            goto success_peg_5;
            out_peg_8:
            {
                int position_peg_10 = result_peg_4.getPosition();
                
                result_peg_4.setValue(Value((void*) "\r"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        result_peg_4.setPosition(position_peg_10);
                        goto out_peg_11;
                    }
                }
                    
            }
            goto success_peg_5;
            out_peg_11:
            goto loop_peg_3;
            success_peg_5:
            ;
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        if (result_peg_2.matches() == 0){
            goto out_peg_12;
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        
        return result_peg_2;
        out_peg_12:
        Result result_peg_13(myposition);
        
        
        {
        
            Result result_peg_15(result_peg_13.getPosition());
                if ('\0' == stream.get(result_peg_15.getPosition())){
                    result_peg_15.nextPosition();
                    result_peg_15.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_16;
                }
            
            
            
            
            
            
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_13;
        stream.update(result_peg_13.getPosition());
        
        
        
        return result_peg_13;
        out_peg_16:
    
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
    
    
    RuleTrace trace_peg_23(stream, "space_newline");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
            int position_peg_5 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_5);
                    goto out_peg_6;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_6:
        {
            int position_peg_8 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_8);
                    goto out_peg_9;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_9:
        {
            int position_peg_11 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_2.getPosition()) == (unsigned char) 255){
                result_peg_2.nextPosition();
            } else {
                result_peg_2.setPosition(position_peg_11);
                goto out_peg_12;
            }
            
        }
        goto success_peg_3;
        out_peg_12:
        goto out_peg_13;
        success_peg_3:
        ;
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_space_newline = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        
        return result_peg_2;
        out_peg_13:
        Result result_peg_14(myposition);
        
        
        {
            int position_peg_17 = result_peg_14.getPosition();
            
            result_peg_14.setValue(Value((void*) "\n"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_14.getPosition()))){
                    result_peg_14.nextPosition();
                } else {
                    result_peg_14.setPosition(position_peg_17);
                    goto out_peg_18;
                }
            }
                
        }
        goto success_peg_15;
        out_peg_18:
        {
            int position_peg_20 = result_peg_14.getPosition();
            
            result_peg_14.setValue(Value((void*) "\r"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_14.getPosition()))){
                    result_peg_14.nextPosition();
                } else {
                    result_peg_14.setPosition(position_peg_20);
                    goto out_peg_21;
                }
            }
                
        }
        goto success_peg_15;
        out_peg_21:
        goto out_peg_22;
        success_peg_15:
        ;
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_space_newline = result_peg_14;
        stream.update(result_peg_14.getPosition());
        
        
        
        return result_peg_14;
        out_peg_22:
    
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
    
    
    RuleTrace trace_peg_16(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
            int position_peg_5 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_5);
                    goto out_peg_6;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_6:
        {
            int position_peg_8 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    result_peg_2.setPosition(position_peg_8);
                    goto out_peg_9;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_9:
        {
            int position_peg_11 = result_peg_2.getPosition();
            
            result_peg_2.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_2.getPosition()) == (unsigned char) 255){
                result_peg_2.nextPosition();
            } else {
                result_peg_2.setPosition(position_peg_11);
                goto out_peg_12;
            }
            
        }
        goto success_peg_3;
        out_peg_12:
        goto out_peg_13;
        success_peg_3:
        ;
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sw = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        
        return result_peg_2;
        out_peg_13:
        Result result_peg_14(myposition);
        
        
        result_peg_14 = rule_comment(stream, result_peg_14.getPosition());
        if (result_peg_14.error()){
            goto out_peg_15;
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_sw = result_peg_14;
        stream.update(result_peg_14.getPosition());
        
        
        
        return result_peg_14;
        out_peg_15:
    
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
    
    
    RuleTrace trace_peg_51(stream, "comment");
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
                                int position_peg_12 = result_peg_9.getPosition();
                                
                                result_peg_9.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                                        result_peg_9.nextPosition();
                                    } else {
                                        result_peg_9.setPosition(position_peg_12);
                                        goto out_peg_13;
                                    }
                                }
                                    
                            }
                            goto success_peg_10;
                            out_peg_13:
                            {
                                int position_peg_15 = result_peg_9.getPosition();
                                
                                result_peg_9.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_9.getPosition()))){
                                        result_peg_9.nextPosition();
                                    } else {
                                        result_peg_9.setPosition(position_peg_15);
                                        goto out_peg_16;
                                    }
                                }
                                    
                            }
                            goto success_peg_10;
                            out_peg_16:
                            goto not_peg_8;
                            success_peg_10:
                            ;
                            goto loop_peg_5;
                            not_peg_8:
                            result_peg_6.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_17 = stream.get(result_peg_6.getPosition());
                            if (temp_peg_17 != '\0'){
                                result_peg_6.setValue(Value((void*) (long) temp_peg_17));
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
        Result result_peg_18(myposition);
        
        
        {
        
            result_peg_18.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_18.getPosition()))){
                        result_peg_18.nextPosition();
                    } else {
                        goto out_peg_20;
                    }
                }
            
            
            
            result_peg_18.reset();
                do{
                    Result result_peg_22(result_peg_18.getPosition());
                    {
                    
                        Result result_peg_25(result_peg_22);
                            {
                                int position_peg_28 = result_peg_25.getPosition();
                                
                                result_peg_25.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_25.getPosition()))){
                                        result_peg_25.nextPosition();
                                    } else {
                                        result_peg_25.setPosition(position_peg_28);
                                        goto out_peg_29;
                                    }
                                }
                                    
                            }
                            goto success_peg_26;
                            out_peg_29:
                            {
                                int position_peg_31 = result_peg_25.getPosition();
                                
                                result_peg_25.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_25.getPosition()))){
                                        result_peg_25.nextPosition();
                                    } else {
                                        result_peg_25.setPosition(position_peg_31);
                                        goto out_peg_32;
                                    }
                                }
                                    
                            }
                            goto success_peg_26;
                            out_peg_32:
                            goto not_peg_24;
                            success_peg_26:
                            ;
                            goto loop_peg_21;
                            not_peg_24:
                            result_peg_22.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_33 = stream.get(result_peg_22.getPosition());
                            if (temp_peg_33 != '\0'){
                                result_peg_22.setValue(Value((void*) (long) temp_peg_33));
                                result_peg_22.nextPosition();
                            } else {
                                goto loop_peg_21;
                            }
                        
                        
                    }
                    result_peg_18.addResult(result_peg_22);
                } while (true);
                loop_peg_21:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_18;
        stream.update(result_peg_18.getPosition());
        
        
        
        return result_peg_18;
        out_peg_20:
        Result result_peg_34(myposition);
        
        
        {
        
            result_peg_34.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_34.getPosition()))){
                        result_peg_34.nextPosition();
                    } else {
                        goto out_peg_36;
                    }
                }
            
            
            
            result_peg_34.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_34.getPosition()))){
                        result_peg_34.nextPosition();
                    } else {
                        goto out_peg_36;
                    }
                }
            
            
            
            result_peg_34.reset();
                do{
                    Result result_peg_39(result_peg_34.getPosition());
                    {
                    
                        Result result_peg_42(result_peg_39);
                            {
                                int position_peg_45 = result_peg_42.getPosition();
                                
                                result_peg_42.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
                                        result_peg_42.nextPosition();
                                    } else {
                                        result_peg_42.setPosition(position_peg_45);
                                        goto out_peg_46;
                                    }
                                }
                                    
                            }
                            goto success_peg_43;
                            out_peg_46:
                            {
                                int position_peg_48 = result_peg_42.getPosition();
                                
                                result_peg_42.setValue(Value((void*) "\r"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_42.getPosition()))){
                                        result_peg_42.nextPosition();
                                    } else {
                                        result_peg_42.setPosition(position_peg_48);
                                        goto out_peg_49;
                                    }
                                }
                                    
                            }
                            goto success_peg_43;
                            out_peg_49:
                            goto not_peg_41;
                            success_peg_43:
                            ;
                            goto loop_peg_38;
                            not_peg_41:
                            result_peg_39.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_50 = stream.get(result_peg_39.getPosition());
                            if (temp_peg_50 != '\0'){
                                result_peg_39.setValue(Value((void*) (long) temp_peg_50));
                                result_peg_39.nextPosition();
                            } else {
                                goto loop_peg_38;
                            }
                        
                        
                    }
                    result_peg_34.addResult(result_peg_39);
                } while (true);
                loop_peg_38:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_34;
        stream.update(result_peg_34.getPosition());
        
        
        
        return result_peg_34;
        out_peg_36:
    
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
    
    
    RuleTrace trace_peg_134(stream, "collision_default");
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
                    int position_peg_9 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_23(result_peg_2.getPosition());
                        {
                            int position_peg_26 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_23.getPosition()))){
                                    result_peg_23.nextPosition();
                                } else {
                                    result_peg_23.setPosition(position_peg_26);
                                    goto out_peg_27;
                                }
                            }
                                
                        }
                        goto success_peg_24;
                        out_peg_27:
                        {
                            int position_peg_29 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_23.getPosition()))){
                                    result_peg_23.nextPosition();
                                } else {
                                    result_peg_23.setPosition(position_peg_29);
                                    goto out_peg_30;
                                }
                            }
                                
                        }
                        goto success_peg_24;
                        out_peg_30:
                        {
                            int position_peg_32 = result_peg_23.getPosition();
                            
                            result_peg_23.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_23.getPosition()) == (unsigned char) 255){
                                result_peg_23.nextPosition();
                            } else {
                                result_peg_23.setPosition(position_peg_32);
                                goto out_peg_33;
                            }
                            
                        }
                        goto success_peg_24;
                        out_peg_33:
                        goto loop_peg_22;
                        success_peg_24:
                        ;
                        result_peg_2.addResult(result_peg_23);
                    } while (true);
                    loop_peg_22:
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
        Result result_peg_35(myposition);
        
        
        {
        
            result_peg_35.setValue(Value((void*) "Clsn1Default"));
                for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_35.getPosition()))){
                        result_peg_35.nextPosition();
                    } else {
                        goto out_peg_37;
                    }
                }
            
            Result result_peg_36 = result_peg_35;
            
            result_peg_35.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_35.getPosition()))){
                        result_peg_35.nextPosition();
                    } else {
                        goto out_peg_37;
                    }
                }
            
            
            
            {
                    int position_peg_42 = result_peg_35.getPosition();
                    
                    result_peg_35.reset();
                    do{
                        Result result_peg_56(result_peg_35.getPosition());
                        {
                            int position_peg_59 = result_peg_56.getPosition();
                            
                            result_peg_56.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_56.getPosition()))){
                                    result_peg_56.nextPosition();
                                } else {
                                    result_peg_56.setPosition(position_peg_59);
                                    goto out_peg_60;
                                }
                            }
                                
                        }
                        goto success_peg_57;
                        out_peg_60:
                        {
                            int position_peg_62 = result_peg_56.getPosition();
                            
                            result_peg_56.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_56.getPosition()))){
                                    result_peg_56.nextPosition();
                                } else {
                                    result_peg_56.setPosition(position_peg_62);
                                    goto out_peg_63;
                                }
                            }
                                
                        }
                        goto success_peg_57;
                        out_peg_63:
                        {
                            int position_peg_65 = result_peg_56.getPosition();
                            
                            result_peg_56.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_56.getPosition()) == (unsigned char) 255){
                                result_peg_56.nextPosition();
                            } else {
                                result_peg_56.setPosition(position_peg_65);
                                goto out_peg_66;
                            }
                            
                        }
                        goto success_peg_57;
                        out_peg_66:
                        goto loop_peg_55;
                        success_peg_57:
                        ;
                        result_peg_35.addResult(result_peg_56);
                    } while (true);
                    loop_peg_55:
                    ;
                            
                }
                goto success_peg_40;
                goto out_peg_37;
                success_peg_40:
                ;
            
            
            
            result_peg_35 = rule_integer(stream, result_peg_35.getPosition());
                if (result_peg_35.error()){
                    goto out_peg_37;
                }
                num = result_peg_35.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_36.getValues())), num);
                    result_peg_35.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_35;
        stream.update(result_peg_35.getPosition());
        
        
        
        return result_peg_35;
        out_peg_37:
        Result result_peg_68(myposition);
        
        
        {
        
            result_peg_68.setValue(Value((void*) "Clsn2"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_68.getPosition()))){
                        result_peg_68.nextPosition();
                    } else {
                        goto out_peg_70;
                    }
                }
            
            Result result_peg_69 = result_peg_68;
            
            result_peg_68.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_68.getPosition()))){
                        result_peg_68.nextPosition();
                    } else {
                        goto out_peg_70;
                    }
                }
            
            
            
            {
                    int position_peg_75 = result_peg_68.getPosition();
                    
                    result_peg_68.reset();
                    do{
                        Result result_peg_89(result_peg_68.getPosition());
                        {
                            int position_peg_92 = result_peg_89.getPosition();
                            
                            result_peg_89.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_89.getPosition()))){
                                    result_peg_89.nextPosition();
                                } else {
                                    result_peg_89.setPosition(position_peg_92);
                                    goto out_peg_93;
                                }
                            }
                                
                        }
                        goto success_peg_90;
                        out_peg_93:
                        {
                            int position_peg_95 = result_peg_89.getPosition();
                            
                            result_peg_89.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_89.getPosition()))){
                                    result_peg_89.nextPosition();
                                } else {
                                    result_peg_89.setPosition(position_peg_95);
                                    goto out_peg_96;
                                }
                            }
                                
                        }
                        goto success_peg_90;
                        out_peg_96:
                        {
                            int position_peg_98 = result_peg_89.getPosition();
                            
                            result_peg_89.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_89.getPosition()) == (unsigned char) 255){
                                result_peg_89.nextPosition();
                            } else {
                                result_peg_89.setPosition(position_peg_98);
                                goto out_peg_99;
                            }
                            
                        }
                        goto success_peg_90;
                        out_peg_99:
                        goto loop_peg_88;
                        success_peg_90:
                        ;
                        result_peg_68.addResult(result_peg_89);
                    } while (true);
                    loop_peg_88:
                    ;
                            
                }
                goto success_peg_73;
                goto out_peg_70;
                success_peg_73:
                ;
            
            
            
            result_peg_68 = rule_integer(stream, result_peg_68.getPosition());
                if (result_peg_68.error()){
                    goto out_peg_70;
                }
                num = result_peg_68.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_69.getValues())), num);
                    result_peg_68.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_68;
        stream.update(result_peg_68.getPosition());
        
        
        
        return result_peg_68;
        out_peg_70:
        Result result_peg_101(myposition);
        
        
        {
        
            result_peg_101.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_101.getPosition()))){
                        result_peg_101.nextPosition();
                    } else {
                        goto out_peg_103;
                    }
                }
            
            Result result_peg_102 = result_peg_101;
            
            result_peg_101.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_101.getPosition()))){
                        result_peg_101.nextPosition();
                    } else {
                        goto out_peg_103;
                    }
                }
            
            
            
            {
                    int position_peg_108 = result_peg_101.getPosition();
                    
                    result_peg_101.reset();
                    do{
                        Result result_peg_122(result_peg_101.getPosition());
                        {
                            int position_peg_125 = result_peg_122.getPosition();
                            
                            result_peg_122.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_122.getPosition()))){
                                    result_peg_122.nextPosition();
                                } else {
                                    result_peg_122.setPosition(position_peg_125);
                                    goto out_peg_126;
                                }
                            }
                                
                        }
                        goto success_peg_123;
                        out_peg_126:
                        {
                            int position_peg_128 = result_peg_122.getPosition();
                            
                            result_peg_122.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_122.getPosition()))){
                                    result_peg_122.nextPosition();
                                } else {
                                    result_peg_122.setPosition(position_peg_128);
                                    goto out_peg_129;
                                }
                            }
                                
                        }
                        goto success_peg_123;
                        out_peg_129:
                        {
                            int position_peg_131 = result_peg_122.getPosition();
                            
                            result_peg_122.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_122.getPosition()) == (unsigned char) 255){
                                result_peg_122.nextPosition();
                            } else {
                                result_peg_122.setPosition(position_peg_131);
                                goto out_peg_132;
                            }
                            
                        }
                        goto success_peg_123;
                        out_peg_132:
                        goto loop_peg_121;
                        success_peg_123:
                        ;
                        result_peg_101.addResult(result_peg_122);
                    } while (true);
                    loop_peg_121:
                    ;
                            
                }
                goto success_peg_106;
                goto out_peg_103;
                success_peg_106:
                ;
            
            
            
            result_peg_101 = rule_integer(stream, result_peg_101.getPosition());
                if (result_peg_101.error()){
                    goto out_peg_103;
                }
                num = result_peg_101.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_102.getValues())), num);
                    result_peg_101.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_101;
        stream.update(result_peg_101.getPosition());
        
        
        
        return result_peg_101;
        out_peg_103:
    
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
    
    
    RuleTrace trace_peg_646(stream, "collision");
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
                    int position_peg_8 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_22(result_peg_2.getPosition());
                        {
                            int position_peg_25 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_26:
                        {
                            int position_peg_28 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_28);
                                    goto out_peg_29;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_29:
                        {
                            int position_peg_31 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_22.getPosition()) == (unsigned char) 255){
                                result_peg_22.nextPosition();
                            } else {
                                result_peg_22.setPosition(position_peg_31);
                                goto out_peg_32;
                            }
                            
                        }
                        goto success_peg_23;
                        out_peg_32:
                        goto loop_peg_21;
                        success_peg_23:
                        ;
                        result_peg_2.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
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
                    int position_peg_37 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_51(result_peg_2.getPosition());
                        {
                            int position_peg_54 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_51.getPosition()))){
                                    result_peg_51.nextPosition();
                                } else {
                                    result_peg_51.setPosition(position_peg_54);
                                    goto out_peg_55;
                                }
                            }
                                
                        }
                        goto success_peg_52;
                        out_peg_55:
                        {
                            int position_peg_57 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_51.getPosition()))){
                                    result_peg_51.nextPosition();
                                } else {
                                    result_peg_51.setPosition(position_peg_57);
                                    goto out_peg_58;
                                }
                            }
                                
                        }
                        goto success_peg_52;
                        out_peg_58:
                        {
                            int position_peg_60 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_51.getPosition()) == (unsigned char) 255){
                                result_peg_51.nextPosition();
                            } else {
                                result_peg_51.setPosition(position_peg_60);
                                goto out_peg_61;
                            }
                            
                        }
                        goto success_peg_52;
                        out_peg_61:
                        goto loop_peg_50;
                        success_peg_52:
                        ;
                        result_peg_2.addResult(result_peg_51);
                    } while (true);
                    loop_peg_50:
                    ;
                            
                }
                goto success_peg_35;
                goto out_peg_4;
                success_peg_35:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                index = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_66 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_80(result_peg_2.getPosition());
                        {
                            int position_peg_83 = result_peg_80.getPosition();
                            
                            result_peg_80.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_80.getPosition()))){
                                    result_peg_80.nextPosition();
                                } else {
                                    result_peg_80.setPosition(position_peg_83);
                                    goto out_peg_84;
                                }
                            }
                                
                        }
                        goto success_peg_81;
                        out_peg_84:
                        {
                            int position_peg_86 = result_peg_80.getPosition();
                            
                            result_peg_80.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_80.getPosition()))){
                                    result_peg_80.nextPosition();
                                } else {
                                    result_peg_80.setPosition(position_peg_86);
                                    goto out_peg_87;
                                }
                            }
                                
                        }
                        goto success_peg_81;
                        out_peg_87:
                        {
                            int position_peg_89 = result_peg_80.getPosition();
                            
                            result_peg_80.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_80.getPosition()) == (unsigned char) 255){
                                result_peg_80.nextPosition();
                            } else {
                                result_peg_80.setPosition(position_peg_89);
                                goto out_peg_90;
                            }
                            
                        }
                        goto success_peg_81;
                        out_peg_90:
                        goto loop_peg_79;
                        success_peg_81:
                        ;
                        result_peg_2.addResult(result_peg_80);
                    } while (true);
                    loop_peg_79:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_4;
                success_peg_64:
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
                    int position_peg_95 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_109(result_peg_2.getPosition());
                        {
                            int position_peg_112 = result_peg_109.getPosition();
                            
                            result_peg_109.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_109.getPosition()))){
                                    result_peg_109.nextPosition();
                                } else {
                                    result_peg_109.setPosition(position_peg_112);
                                    goto out_peg_113;
                                }
                            }
                                
                        }
                        goto success_peg_110;
                        out_peg_113:
                        {
                            int position_peg_115 = result_peg_109.getPosition();
                            
                            result_peg_109.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_109.getPosition()))){
                                    result_peg_109.nextPosition();
                                } else {
                                    result_peg_109.setPosition(position_peg_115);
                                    goto out_peg_116;
                                }
                            }
                                
                        }
                        goto success_peg_110;
                        out_peg_116:
                        {
                            int position_peg_118 = result_peg_109.getPosition();
                            
                            result_peg_109.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_109.getPosition()) == (unsigned char) 255){
                                result_peg_109.nextPosition();
                            } else {
                                result_peg_109.setPosition(position_peg_118);
                                goto out_peg_119;
                            }
                            
                        }
                        goto success_peg_110;
                        out_peg_119:
                        goto loop_peg_108;
                        success_peg_110:
                        ;
                        result_peg_2.addResult(result_peg_109);
                    } while (true);
                    loop_peg_108:
                    ;
                            
                }
                goto success_peg_93;
                goto out_peg_4;
                success_peg_93:
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
                    int position_peg_124 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_138(result_peg_2.getPosition());
                        {
                            int position_peg_141 = result_peg_138.getPosition();
                            
                            result_peg_138.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_138.getPosition()))){
                                    result_peg_138.nextPosition();
                                } else {
                                    result_peg_138.setPosition(position_peg_141);
                                    goto out_peg_142;
                                }
                            }
                                
                        }
                        goto success_peg_139;
                        out_peg_142:
                        {
                            int position_peg_144 = result_peg_138.getPosition();
                            
                            result_peg_138.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_138.getPosition()))){
                                    result_peg_138.nextPosition();
                                } else {
                                    result_peg_138.setPosition(position_peg_144);
                                    goto out_peg_145;
                                }
                            }
                                
                        }
                        goto success_peg_139;
                        out_peg_145:
                        {
                            int position_peg_147 = result_peg_138.getPosition();
                            
                            result_peg_138.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_138.getPosition()) == (unsigned char) 255){
                                result_peg_138.nextPosition();
                            } else {
                                result_peg_138.setPosition(position_peg_147);
                                goto out_peg_148;
                            }
                            
                        }
                        goto success_peg_139;
                        out_peg_148:
                        goto loop_peg_137;
                        success_peg_139:
                        ;
                        result_peg_2.addResult(result_peg_138);
                    } while (true);
                    loop_peg_137:
                    ;
                            
                }
                goto success_peg_122;
                goto out_peg_4;
                success_peg_122:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n1 = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_153 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_167(result_peg_2.getPosition());
                        {
                            int position_peg_170 = result_peg_167.getPosition();
                            
                            result_peg_167.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_167.getPosition()))){
                                    result_peg_167.nextPosition();
                                } else {
                                    result_peg_167.setPosition(position_peg_170);
                                    goto out_peg_171;
                                }
                            }
                                
                        }
                        goto success_peg_168;
                        out_peg_171:
                        {
                            int position_peg_173 = result_peg_167.getPosition();
                            
                            result_peg_167.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_167.getPosition()))){
                                    result_peg_167.nextPosition();
                                } else {
                                    result_peg_167.setPosition(position_peg_173);
                                    goto out_peg_174;
                                }
                            }
                                
                        }
                        goto success_peg_168;
                        out_peg_174:
                        {
                            int position_peg_176 = result_peg_167.getPosition();
                            
                            result_peg_167.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_167.getPosition()) == (unsigned char) 255){
                                result_peg_167.nextPosition();
                            } else {
                                result_peg_167.setPosition(position_peg_176);
                                goto out_peg_177;
                            }
                            
                        }
                        goto success_peg_168;
                        out_peg_177:
                        goto loop_peg_166;
                        success_peg_168:
                        ;
                        result_peg_2.addResult(result_peg_167);
                    } while (true);
                    loop_peg_166:
                    ;
                            
                }
                goto success_peg_151;
                goto out_peg_4;
                success_peg_151:
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
                    int position_peg_182 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_196(result_peg_2.getPosition());
                        {
                            int position_peg_199 = result_peg_196.getPosition();
                            
                            result_peg_196.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_196.getPosition()))){
                                    result_peg_196.nextPosition();
                                } else {
                                    result_peg_196.setPosition(position_peg_199);
                                    goto out_peg_200;
                                }
                            }
                                
                        }
                        goto success_peg_197;
                        out_peg_200:
                        {
                            int position_peg_202 = result_peg_196.getPosition();
                            
                            result_peg_196.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_196.getPosition()))){
                                    result_peg_196.nextPosition();
                                } else {
                                    result_peg_196.setPosition(position_peg_202);
                                    goto out_peg_203;
                                }
                            }
                                
                        }
                        goto success_peg_197;
                        out_peg_203:
                        {
                            int position_peg_205 = result_peg_196.getPosition();
                            
                            result_peg_196.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_196.getPosition()) == (unsigned char) 255){
                                result_peg_196.nextPosition();
                            } else {
                                result_peg_196.setPosition(position_peg_205);
                                goto out_peg_206;
                            }
                            
                        }
                        goto success_peg_197;
                        out_peg_206:
                        goto loop_peg_195;
                        success_peg_197:
                        ;
                        result_peg_2.addResult(result_peg_196);
                    } while (true);
                    loop_peg_195:
                    ;
                            
                }
                goto success_peg_180;
                goto out_peg_4;
                success_peg_180:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n2 = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_211 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_225(result_peg_2.getPosition());
                        {
                            int position_peg_228 = result_peg_225.getPosition();
                            
                            result_peg_225.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_225.getPosition()))){
                                    result_peg_225.nextPosition();
                                } else {
                                    result_peg_225.setPosition(position_peg_228);
                                    goto out_peg_229;
                                }
                            }
                                
                        }
                        goto success_peg_226;
                        out_peg_229:
                        {
                            int position_peg_231 = result_peg_225.getPosition();
                            
                            result_peg_225.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_225.getPosition()))){
                                    result_peg_225.nextPosition();
                                } else {
                                    result_peg_225.setPosition(position_peg_231);
                                    goto out_peg_232;
                                }
                            }
                                
                        }
                        goto success_peg_226;
                        out_peg_232:
                        {
                            int position_peg_234 = result_peg_225.getPosition();
                            
                            result_peg_225.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_225.getPosition()) == (unsigned char) 255){
                                result_peg_225.nextPosition();
                            } else {
                                result_peg_225.setPosition(position_peg_234);
                                goto out_peg_235;
                            }
                            
                        }
                        goto success_peg_226;
                        out_peg_235:
                        goto loop_peg_224;
                        success_peg_226:
                        ;
                        result_peg_2.addResult(result_peg_225);
                    } while (true);
                    loop_peg_224:
                    ;
                            
                }
                goto success_peg_209;
                goto out_peg_4;
                success_peg_209:
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
                    int position_peg_240 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_254(result_peg_2.getPosition());
                        {
                            int position_peg_257 = result_peg_254.getPosition();
                            
                            result_peg_254.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_254.getPosition()))){
                                    result_peg_254.nextPosition();
                                } else {
                                    result_peg_254.setPosition(position_peg_257);
                                    goto out_peg_258;
                                }
                            }
                                
                        }
                        goto success_peg_255;
                        out_peg_258:
                        {
                            int position_peg_260 = result_peg_254.getPosition();
                            
                            result_peg_254.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_254.getPosition()))){
                                    result_peg_254.nextPosition();
                                } else {
                                    result_peg_254.setPosition(position_peg_260);
                                    goto out_peg_261;
                                }
                            }
                                
                        }
                        goto success_peg_255;
                        out_peg_261:
                        {
                            int position_peg_263 = result_peg_254.getPosition();
                            
                            result_peg_254.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_254.getPosition()) == (unsigned char) 255){
                                result_peg_254.nextPosition();
                            } else {
                                result_peg_254.setPosition(position_peg_263);
                                goto out_peg_264;
                            }
                            
                        }
                        goto success_peg_255;
                        out_peg_264:
                        goto loop_peg_253;
                        success_peg_255:
                        ;
                        result_peg_2.addResult(result_peg_254);
                    } while (true);
                    loop_peg_253:
                    ;
                            
                }
                goto success_peg_238;
                goto out_peg_4;
                success_peg_238:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n3 = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_269 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_283(result_peg_2.getPosition());
                        {
                            int position_peg_286 = result_peg_283.getPosition();
                            
                            result_peg_283.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_283.getPosition()))){
                                    result_peg_283.nextPosition();
                                } else {
                                    result_peg_283.setPosition(position_peg_286);
                                    goto out_peg_287;
                                }
                            }
                                
                        }
                        goto success_peg_284;
                        out_peg_287:
                        {
                            int position_peg_289 = result_peg_283.getPosition();
                            
                            result_peg_283.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_283.getPosition()))){
                                    result_peg_283.nextPosition();
                                } else {
                                    result_peg_283.setPosition(position_peg_289);
                                    goto out_peg_290;
                                }
                            }
                                
                        }
                        goto success_peg_284;
                        out_peg_290:
                        {
                            int position_peg_292 = result_peg_283.getPosition();
                            
                            result_peg_283.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_283.getPosition()) == (unsigned char) 255){
                                result_peg_283.nextPosition();
                            } else {
                                result_peg_283.setPosition(position_peg_292);
                                goto out_peg_293;
                            }
                            
                        }
                        goto success_peg_284;
                        out_peg_293:
                        goto loop_peg_282;
                        success_peg_284:
                        ;
                        result_peg_2.addResult(result_peg_283);
                    } while (true);
                    loop_peg_282:
                    ;
                            
                }
                goto success_peg_267;
                goto out_peg_4;
                success_peg_267:
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
                    int position_peg_298 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_312(result_peg_2.getPosition());
                        {
                            int position_peg_315 = result_peg_312.getPosition();
                            
                            result_peg_312.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_312.getPosition()))){
                                    result_peg_312.nextPosition();
                                } else {
                                    result_peg_312.setPosition(position_peg_315);
                                    goto out_peg_316;
                                }
                            }
                                
                        }
                        goto success_peg_313;
                        out_peg_316:
                        {
                            int position_peg_318 = result_peg_312.getPosition();
                            
                            result_peg_312.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_312.getPosition()))){
                                    result_peg_312.nextPosition();
                                } else {
                                    result_peg_312.setPosition(position_peg_318);
                                    goto out_peg_319;
                                }
                            }
                                
                        }
                        goto success_peg_313;
                        out_peg_319:
                        {
                            int position_peg_321 = result_peg_312.getPosition();
                            
                            result_peg_312.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_312.getPosition()) == (unsigned char) 255){
                                result_peg_312.nextPosition();
                            } else {
                                result_peg_312.setPosition(position_peg_321);
                                goto out_peg_322;
                            }
                            
                        }
                        goto success_peg_313;
                        out_peg_322:
                        goto loop_peg_311;
                        success_peg_313:
                        ;
                        result_peg_2.addResult(result_peg_312);
                    } while (true);
                    loop_peg_311:
                    ;
                            
                }
                goto success_peg_296;
                goto out_peg_4;
                success_peg_296:
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
        Result result_peg_324(myposition);
        
        
        {
        
            result_peg_324.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
                name = result_peg_324.getValues();
            
            
            
            {
                    int position_peg_330 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_344(result_peg_324.getPosition());
                        {
                            int position_peg_347 = result_peg_344.getPosition();
                            
                            result_peg_344.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_344.getPosition()))){
                                    result_peg_344.nextPosition();
                                } else {
                                    result_peg_344.setPosition(position_peg_347);
                                    goto out_peg_348;
                                }
                            }
                                
                        }
                        goto success_peg_345;
                        out_peg_348:
                        {
                            int position_peg_350 = result_peg_344.getPosition();
                            
                            result_peg_344.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_344.getPosition()))){
                                    result_peg_344.nextPosition();
                                } else {
                                    result_peg_344.setPosition(position_peg_350);
                                    goto out_peg_351;
                                }
                            }
                                
                        }
                        goto success_peg_345;
                        out_peg_351:
                        {
                            int position_peg_353 = result_peg_344.getPosition();
                            
                            result_peg_344.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_344.getPosition()) == (unsigned char) 255){
                                result_peg_344.nextPosition();
                            } else {
                                result_peg_344.setPosition(position_peg_353);
                                goto out_peg_354;
                            }
                            
                        }
                        goto success_peg_345;
                        out_peg_354:
                        goto loop_peg_343;
                        success_peg_345:
                        ;
                        result_peg_324.addResult(result_peg_344);
                    } while (true);
                    loop_peg_343:
                    ;
                            
                }
                goto success_peg_328;
                goto out_peg_326;
                success_peg_328:
                ;
            
            
            
            result_peg_324.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
            
            
            
            {
                    int position_peg_359 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_373(result_peg_324.getPosition());
                        {
                            int position_peg_376 = result_peg_373.getPosition();
                            
                            result_peg_373.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_373.getPosition()))){
                                    result_peg_373.nextPosition();
                                } else {
                                    result_peg_373.setPosition(position_peg_376);
                                    goto out_peg_377;
                                }
                            }
                                
                        }
                        goto success_peg_374;
                        out_peg_377:
                        {
                            int position_peg_379 = result_peg_373.getPosition();
                            
                            result_peg_373.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_373.getPosition()))){
                                    result_peg_373.nextPosition();
                                } else {
                                    result_peg_373.setPosition(position_peg_379);
                                    goto out_peg_380;
                                }
                            }
                                
                        }
                        goto success_peg_374;
                        out_peg_380:
                        {
                            int position_peg_382 = result_peg_373.getPosition();
                            
                            result_peg_373.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_373.getPosition()) == (unsigned char) 255){
                                result_peg_373.nextPosition();
                            } else {
                                result_peg_373.setPosition(position_peg_382);
                                goto out_peg_383;
                            }
                            
                        }
                        goto success_peg_374;
                        out_peg_383:
                        goto loop_peg_372;
                        success_peg_374:
                        ;
                        result_peg_324.addResult(result_peg_373);
                    } while (true);
                    loop_peg_372:
                    ;
                            
                }
                goto success_peg_357;
                goto out_peg_326;
                success_peg_357:
                ;
            
            
            
            result_peg_324 = rule_integer(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto out_peg_326;
                }
                index = result_peg_324.getValues();
            
            
            
            {
                    int position_peg_388 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_402(result_peg_324.getPosition());
                        {
                            int position_peg_405 = result_peg_402.getPosition();
                            
                            result_peg_402.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_402.getPosition()))){
                                    result_peg_402.nextPosition();
                                } else {
                                    result_peg_402.setPosition(position_peg_405);
                                    goto out_peg_406;
                                }
                            }
                                
                        }
                        goto success_peg_403;
                        out_peg_406:
                        {
                            int position_peg_408 = result_peg_402.getPosition();
                            
                            result_peg_402.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_402.getPosition()))){
                                    result_peg_402.nextPosition();
                                } else {
                                    result_peg_402.setPosition(position_peg_408);
                                    goto out_peg_409;
                                }
                            }
                                
                        }
                        goto success_peg_403;
                        out_peg_409:
                        {
                            int position_peg_411 = result_peg_402.getPosition();
                            
                            result_peg_402.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_402.getPosition()) == (unsigned char) 255){
                                result_peg_402.nextPosition();
                            } else {
                                result_peg_402.setPosition(position_peg_411);
                                goto out_peg_412;
                            }
                            
                        }
                        goto success_peg_403;
                        out_peg_412:
                        goto loop_peg_401;
                        success_peg_403:
                        ;
                        result_peg_324.addResult(result_peg_402);
                    } while (true);
                    loop_peg_401:
                    ;
                            
                }
                goto success_peg_386;
                goto out_peg_326;
                success_peg_386:
                ;
            
            
            
            result_peg_324.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
            
            
            
            {
                    int position_peg_417 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_431(result_peg_324.getPosition());
                        {
                            int position_peg_434 = result_peg_431.getPosition();
                            
                            result_peg_431.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_431.getPosition()))){
                                    result_peg_431.nextPosition();
                                } else {
                                    result_peg_431.setPosition(position_peg_434);
                                    goto out_peg_435;
                                }
                            }
                                
                        }
                        goto success_peg_432;
                        out_peg_435:
                        {
                            int position_peg_437 = result_peg_431.getPosition();
                            
                            result_peg_431.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_431.getPosition()))){
                                    result_peg_431.nextPosition();
                                } else {
                                    result_peg_431.setPosition(position_peg_437);
                                    goto out_peg_438;
                                }
                            }
                                
                        }
                        goto success_peg_432;
                        out_peg_438:
                        {
                            int position_peg_440 = result_peg_431.getPosition();
                            
                            result_peg_431.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_431.getPosition()) == (unsigned char) 255){
                                result_peg_431.nextPosition();
                            } else {
                                result_peg_431.setPosition(position_peg_440);
                                goto out_peg_441;
                            }
                            
                        }
                        goto success_peg_432;
                        out_peg_441:
                        goto loop_peg_430;
                        success_peg_432:
                        ;
                        result_peg_324.addResult(result_peg_431);
                    } while (true);
                    loop_peg_430:
                    ;
                            
                }
                goto success_peg_415;
                goto out_peg_326;
                success_peg_415:
                ;
            
            
            
            result_peg_324.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
            
            
            
            {
                    int position_peg_446 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_460(result_peg_324.getPosition());
                        {
                            int position_peg_463 = result_peg_460.getPosition();
                            
                            result_peg_460.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_460.getPosition()))){
                                    result_peg_460.nextPosition();
                                } else {
                                    result_peg_460.setPosition(position_peg_463);
                                    goto out_peg_464;
                                }
                            }
                                
                        }
                        goto success_peg_461;
                        out_peg_464:
                        {
                            int position_peg_466 = result_peg_460.getPosition();
                            
                            result_peg_460.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_460.getPosition()))){
                                    result_peg_460.nextPosition();
                                } else {
                                    result_peg_460.setPosition(position_peg_466);
                                    goto out_peg_467;
                                }
                            }
                                
                        }
                        goto success_peg_461;
                        out_peg_467:
                        {
                            int position_peg_469 = result_peg_460.getPosition();
                            
                            result_peg_460.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_460.getPosition()) == (unsigned char) 255){
                                result_peg_460.nextPosition();
                            } else {
                                result_peg_460.setPosition(position_peg_469);
                                goto out_peg_470;
                            }
                            
                        }
                        goto success_peg_461;
                        out_peg_470:
                        goto loop_peg_459;
                        success_peg_461:
                        ;
                        result_peg_324.addResult(result_peg_460);
                    } while (true);
                    loop_peg_459:
                    ;
                            
                }
                goto success_peg_444;
                goto out_peg_326;
                success_peg_444:
                ;
            
            
            
            result_peg_324 = rule_integer(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto out_peg_326;
                }
                n1 = result_peg_324.getValues();
            
            
            
            {
                    int position_peg_475 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_489(result_peg_324.getPosition());
                        {
                            int position_peg_492 = result_peg_489.getPosition();
                            
                            result_peg_489.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_489.getPosition()))){
                                    result_peg_489.nextPosition();
                                } else {
                                    result_peg_489.setPosition(position_peg_492);
                                    goto out_peg_493;
                                }
                            }
                                
                        }
                        goto success_peg_490;
                        out_peg_493:
                        {
                            int position_peg_495 = result_peg_489.getPosition();
                            
                            result_peg_489.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_489.getPosition()))){
                                    result_peg_489.nextPosition();
                                } else {
                                    result_peg_489.setPosition(position_peg_495);
                                    goto out_peg_496;
                                }
                            }
                                
                        }
                        goto success_peg_490;
                        out_peg_496:
                        {
                            int position_peg_498 = result_peg_489.getPosition();
                            
                            result_peg_489.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_489.getPosition()) == (unsigned char) 255){
                                result_peg_489.nextPosition();
                            } else {
                                result_peg_489.setPosition(position_peg_498);
                                goto out_peg_499;
                            }
                            
                        }
                        goto success_peg_490;
                        out_peg_499:
                        goto loop_peg_488;
                        success_peg_490:
                        ;
                        result_peg_324.addResult(result_peg_489);
                    } while (true);
                    loop_peg_488:
                    ;
                            
                }
                goto success_peg_473;
                goto out_peg_326;
                success_peg_473:
                ;
            
            
            
            result_peg_324.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
            
            
            
            {
                    int position_peg_504 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_518(result_peg_324.getPosition());
                        {
                            int position_peg_521 = result_peg_518.getPosition();
                            
                            result_peg_518.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_518.getPosition()))){
                                    result_peg_518.nextPosition();
                                } else {
                                    result_peg_518.setPosition(position_peg_521);
                                    goto out_peg_522;
                                }
                            }
                                
                        }
                        goto success_peg_519;
                        out_peg_522:
                        {
                            int position_peg_524 = result_peg_518.getPosition();
                            
                            result_peg_518.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_518.getPosition()))){
                                    result_peg_518.nextPosition();
                                } else {
                                    result_peg_518.setPosition(position_peg_524);
                                    goto out_peg_525;
                                }
                            }
                                
                        }
                        goto success_peg_519;
                        out_peg_525:
                        {
                            int position_peg_527 = result_peg_518.getPosition();
                            
                            result_peg_518.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_518.getPosition()) == (unsigned char) 255){
                                result_peg_518.nextPosition();
                            } else {
                                result_peg_518.setPosition(position_peg_527);
                                goto out_peg_528;
                            }
                            
                        }
                        goto success_peg_519;
                        out_peg_528:
                        goto loop_peg_517;
                        success_peg_519:
                        ;
                        result_peg_324.addResult(result_peg_518);
                    } while (true);
                    loop_peg_517:
                    ;
                            
                }
                goto success_peg_502;
                goto out_peg_326;
                success_peg_502:
                ;
            
            
            
            result_peg_324 = rule_integer(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto out_peg_326;
                }
                n2 = result_peg_324.getValues();
            
            
            
            {
                    int position_peg_533 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_547(result_peg_324.getPosition());
                        {
                            int position_peg_550 = result_peg_547.getPosition();
                            
                            result_peg_547.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_547.getPosition()))){
                                    result_peg_547.nextPosition();
                                } else {
                                    result_peg_547.setPosition(position_peg_550);
                                    goto out_peg_551;
                                }
                            }
                                
                        }
                        goto success_peg_548;
                        out_peg_551:
                        {
                            int position_peg_553 = result_peg_547.getPosition();
                            
                            result_peg_547.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_547.getPosition()))){
                                    result_peg_547.nextPosition();
                                } else {
                                    result_peg_547.setPosition(position_peg_553);
                                    goto out_peg_554;
                                }
                            }
                                
                        }
                        goto success_peg_548;
                        out_peg_554:
                        {
                            int position_peg_556 = result_peg_547.getPosition();
                            
                            result_peg_547.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_547.getPosition()) == (unsigned char) 255){
                                result_peg_547.nextPosition();
                            } else {
                                result_peg_547.setPosition(position_peg_556);
                                goto out_peg_557;
                            }
                            
                        }
                        goto success_peg_548;
                        out_peg_557:
                        goto loop_peg_546;
                        success_peg_548:
                        ;
                        result_peg_324.addResult(result_peg_547);
                    } while (true);
                    loop_peg_546:
                    ;
                            
                }
                goto success_peg_531;
                goto out_peg_326;
                success_peg_531:
                ;
            
            
            
            result_peg_324.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
            
            
            
            {
                    int position_peg_562 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_576(result_peg_324.getPosition());
                        {
                            int position_peg_579 = result_peg_576.getPosition();
                            
                            result_peg_576.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_576.getPosition()))){
                                    result_peg_576.nextPosition();
                                } else {
                                    result_peg_576.setPosition(position_peg_579);
                                    goto out_peg_580;
                                }
                            }
                                
                        }
                        goto success_peg_577;
                        out_peg_580:
                        {
                            int position_peg_582 = result_peg_576.getPosition();
                            
                            result_peg_576.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_576.getPosition()))){
                                    result_peg_576.nextPosition();
                                } else {
                                    result_peg_576.setPosition(position_peg_582);
                                    goto out_peg_583;
                                }
                            }
                                
                        }
                        goto success_peg_577;
                        out_peg_583:
                        {
                            int position_peg_585 = result_peg_576.getPosition();
                            
                            result_peg_576.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_576.getPosition()) == (unsigned char) 255){
                                result_peg_576.nextPosition();
                            } else {
                                result_peg_576.setPosition(position_peg_585);
                                goto out_peg_586;
                            }
                            
                        }
                        goto success_peg_577;
                        out_peg_586:
                        goto loop_peg_575;
                        success_peg_577:
                        ;
                        result_peg_324.addResult(result_peg_576);
                    } while (true);
                    loop_peg_575:
                    ;
                            
                }
                goto success_peg_560;
                goto out_peg_326;
                success_peg_560:
                ;
            
            
            
            result_peg_324 = rule_integer(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto out_peg_326;
                }
                n3 = result_peg_324.getValues();
            
            
            
            {
                    int position_peg_591 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_605(result_peg_324.getPosition());
                        {
                            int position_peg_608 = result_peg_605.getPosition();
                            
                            result_peg_605.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_605.getPosition()))){
                                    result_peg_605.nextPosition();
                                } else {
                                    result_peg_605.setPosition(position_peg_608);
                                    goto out_peg_609;
                                }
                            }
                                
                        }
                        goto success_peg_606;
                        out_peg_609:
                        {
                            int position_peg_611 = result_peg_605.getPosition();
                            
                            result_peg_605.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_605.getPosition()))){
                                    result_peg_605.nextPosition();
                                } else {
                                    result_peg_605.setPosition(position_peg_611);
                                    goto out_peg_612;
                                }
                            }
                                
                        }
                        goto success_peg_606;
                        out_peg_612:
                        {
                            int position_peg_614 = result_peg_605.getPosition();
                            
                            result_peg_605.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_605.getPosition()) == (unsigned char) 255){
                                result_peg_605.nextPosition();
                            } else {
                                result_peg_605.setPosition(position_peg_614);
                                goto out_peg_615;
                            }
                            
                        }
                        goto success_peg_606;
                        out_peg_615:
                        goto loop_peg_604;
                        success_peg_606:
                        ;
                        result_peg_324.addResult(result_peg_605);
                    } while (true);
                    loop_peg_604:
                    ;
                            
                }
                goto success_peg_589;
                goto out_peg_326;
                success_peg_589:
                ;
            
            
            
            result_peg_324.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_324.getPosition()))){
                        result_peg_324.nextPosition();
                    } else {
                        goto out_peg_326;
                    }
                }
            
            
            
            {
                    int position_peg_620 = result_peg_324.getPosition();
                    
                    result_peg_324.reset();
                    do{
                        Result result_peg_634(result_peg_324.getPosition());
                        {
                            int position_peg_637 = result_peg_634.getPosition();
                            
                            result_peg_634.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_634.getPosition()))){
                                    result_peg_634.nextPosition();
                                } else {
                                    result_peg_634.setPosition(position_peg_637);
                                    goto out_peg_638;
                                }
                            }
                                
                        }
                        goto success_peg_635;
                        out_peg_638:
                        {
                            int position_peg_640 = result_peg_634.getPosition();
                            
                            result_peg_634.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_634.getPosition()))){
                                    result_peg_634.nextPosition();
                                } else {
                                    result_peg_634.setPosition(position_peg_640);
                                    goto out_peg_641;
                                }
                            }
                                
                        }
                        goto success_peg_635;
                        out_peg_641:
                        {
                            int position_peg_643 = result_peg_634.getPosition();
                            
                            result_peg_634.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_634.getPosition()) == (unsigned char) 255){
                                result_peg_634.nextPosition();
                            } else {
                                result_peg_634.setPosition(position_peg_643);
                                goto out_peg_644;
                            }
                            
                        }
                        goto success_peg_635;
                        out_peg_644:
                        goto loop_peg_633;
                        success_peg_635:
                        ;
                        result_peg_324.addResult(result_peg_634);
                    } while (true);
                    loop_peg_633:
                    ;
                            
                }
                goto success_peg_618;
                goto out_peg_326;
                success_peg_618:
                ;
            
            
            
            result_peg_324 = rule_integer(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto out_peg_326;
                }
                n4 = result_peg_324.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_324.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision = result_peg_324;
        stream.update(result_peg_324.getPosition());
        
        
        
        return result_peg_324;
        out_peg_326:
    
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
    
    
    RuleTrace trace_peg_128(stream, "action_start");
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
                    int position_peg_8 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_22(result_peg_2.getPosition());
                        {
                            int position_peg_25 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_26:
                        {
                            int position_peg_28 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_22.getPosition()))){
                                    result_peg_22.nextPosition();
                                } else {
                                    result_peg_22.setPosition(position_peg_28);
                                    goto out_peg_29;
                                }
                            }
                                
                        }
                        goto success_peg_23;
                        out_peg_29:
                        {
                            int position_peg_31 = result_peg_22.getPosition();
                            
                            result_peg_22.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_22.getPosition()) == (unsigned char) 255){
                                result_peg_22.nextPosition();
                            } else {
                                result_peg_22.setPosition(position_peg_31);
                                goto out_peg_32;
                            }
                            
                        }
                        goto success_peg_23;
                        out_peg_32:
                        goto loop_peg_21;
                        success_peg_23:
                        ;
                        result_peg_2.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
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
                    int position_peg_37 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_51(result_peg_2.getPosition());
                        {
                            int position_peg_54 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_51.getPosition()))){
                                    result_peg_51.nextPosition();
                                } else {
                                    result_peg_51.setPosition(position_peg_54);
                                    goto out_peg_55;
                                }
                            }
                                
                        }
                        goto success_peg_52;
                        out_peg_55:
                        {
                            int position_peg_57 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_51.getPosition()))){
                                    result_peg_51.nextPosition();
                                } else {
                                    result_peg_51.setPosition(position_peg_57);
                                    goto out_peg_58;
                                }
                            }
                                
                        }
                        goto success_peg_52;
                        out_peg_58:
                        {
                            int position_peg_60 = result_peg_51.getPosition();
                            
                            result_peg_51.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_51.getPosition()) == (unsigned char) 255){
                                result_peg_51.nextPosition();
                            } else {
                                result_peg_51.setPosition(position_peg_60);
                                goto out_peg_61;
                            }
                            
                        }
                        goto success_peg_52;
                        out_peg_61:
                        goto loop_peg_50;
                        success_peg_52:
                        ;
                        result_peg_2.addResult(result_peg_51);
                    } while (true);
                    loop_peg_50:
                    ;
                            
                }
                goto success_peg_35;
                goto out_peg_4;
                success_peg_35:
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
                    int position_peg_66 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_80(result_peg_2.getPosition());
                        {
                            int position_peg_83 = result_peg_80.getPosition();
                            
                            result_peg_80.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_80.getPosition()))){
                                    result_peg_80.nextPosition();
                                } else {
                                    result_peg_80.setPosition(position_peg_83);
                                    goto out_peg_84;
                                }
                            }
                                
                        }
                        goto success_peg_81;
                        out_peg_84:
                        {
                            int position_peg_86 = result_peg_80.getPosition();
                            
                            result_peg_80.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_80.getPosition()))){
                                    result_peg_80.nextPosition();
                                } else {
                                    result_peg_80.setPosition(position_peg_86);
                                    goto out_peg_87;
                                }
                            }
                                
                        }
                        goto success_peg_81;
                        out_peg_87:
                        {
                            int position_peg_89 = result_peg_80.getPosition();
                            
                            result_peg_80.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_80.getPosition()) == (unsigned char) 255){
                                result_peg_80.nextPosition();
                            } else {
                                result_peg_80.setPosition(position_peg_89);
                                goto out_peg_90;
                            }
                            
                        }
                        goto success_peg_81;
                        out_peg_90:
                        goto loop_peg_79;
                        success_peg_81:
                        ;
                        result_peg_2.addResult(result_peg_80);
                    } while (true);
                    loop_peg_79:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_4;
                success_peg_64:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                num = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_95 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_109(result_peg_2.getPosition());
                        {
                            int position_peg_112 = result_peg_109.getPosition();
                            
                            result_peg_109.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_109.getPosition()))){
                                    result_peg_109.nextPosition();
                                } else {
                                    result_peg_109.setPosition(position_peg_112);
                                    goto out_peg_113;
                                }
                            }
                                
                        }
                        goto success_peg_110;
                        out_peg_113:
                        {
                            int position_peg_115 = result_peg_109.getPosition();
                            
                            result_peg_109.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_109.getPosition()))){
                                    result_peg_109.nextPosition();
                                } else {
                                    result_peg_109.setPosition(position_peg_115);
                                    goto out_peg_116;
                                }
                            }
                                
                        }
                        goto success_peg_110;
                        out_peg_116:
                        {
                            int position_peg_118 = result_peg_109.getPosition();
                            
                            result_peg_109.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_109.getPosition()) == (unsigned char) 255){
                                result_peg_109.nextPosition();
                            } else {
                                result_peg_109.setPosition(position_peg_118);
                                goto out_peg_119;
                            }
                            
                        }
                        goto success_peg_110;
                        out_peg_119:
                        goto loop_peg_108;
                        success_peg_110:
                        ;
                        result_peg_2.addResult(result_peg_109);
                    } while (true);
                    loop_peg_108:
                    ;
                            
                }
                goto success_peg_93;
                goto out_peg_4;
                success_peg_93:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_122(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_125(result_peg_122);
                            result_peg_125.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_125.getPosition()))){
                                    result_peg_125.nextPosition();
                                } else {
                                    goto not_peg_124;
                                }
                            }
                            goto loop_peg_121;
                            not_peg_124:
                            result_peg_122.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_126 = stream.get(result_peg_122.getPosition());
                            if (temp_peg_126 != '\0'){
                                result_peg_122.setValue(Value((void*) (long) temp_peg_126));
                                result_peg_122.nextPosition();
                            } else {
                                goto loop_peg_121;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_122);
                } while (true);
                loop_peg_121:
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
    
    
    RuleTrace trace_peg_22(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    int position_peg_7 = result_peg_2.getPosition();
                    
                    result_peg_2.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_7);
                            goto out_peg_8;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_8:
                {
                    int position_peg_10 = result_peg_2.getPosition();
                    
                    result_peg_2.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_10);
                            goto out_peg_11;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_11:
                
                result_peg_2 = Result(save_peg_4);
                result_peg_2.setValue(Value((void*) 0));
                
                success_peg_5:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_14(result_peg_2.getPosition());
                    {
                        int position_peg_17 = result_peg_14.getPosition();
                        
                        char letter_peg_20 = stream.get(result_peg_14.getPosition());
                        if (letter_peg_20 != '\0' && strchr("0123456789", letter_peg_20) != NULL){
                            result_peg_14.nextPosition();
                            result_peg_14.setValue(Value((void*) (long) letter_peg_20));
                        } else {
                            result_peg_14.setPosition(position_peg_17);
                            goto out_peg_19;
                        }
                        
                    }
                    goto success_peg_15;
                    out_peg_19:
                    goto loop_peg_13;
                    success_peg_15:
                    ;
                    result_peg_2.addResult(result_peg_14);
                } while (true);
                loop_peg_13:
                if (result_peg_2.matches() == 0){
                    goto out_peg_21;
                }
            
            Result result_peg_12 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_12.getValues()); value = makeNumber(result_peg_3.getValues(),value);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_integer = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        
        return result_peg_2;
        out_peg_21:
    
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
    
    
    RuleTrace trace_peg_66(stream, "valuelist");
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
                                int position_peg_11 = result_peg_7.getPosition();
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_25(result_peg_7.getPosition());
                                    {
                                        int position_peg_28 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_25.getPosition()))){
                                                result_peg_25.nextPosition();
                                            } else {
                                                result_peg_25.setPosition(position_peg_28);
                                                goto out_peg_29;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_26;
                                    out_peg_29:
                                    {
                                        int position_peg_31 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_25.getPosition()))){
                                                result_peg_25.nextPosition();
                                            } else {
                                                result_peg_25.setPosition(position_peg_31);
                                                goto out_peg_32;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_26;
                                    out_peg_32:
                                    {
                                        int position_peg_34 = result_peg_25.getPosition();
                                        
                                        result_peg_25.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_25.getPosition()) == (unsigned char) 255){
                                            result_peg_25.nextPosition();
                                        } else {
                                            result_peg_25.setPosition(position_peg_34);
                                            goto out_peg_35;
                                        }
                                        
                                    }
                                    goto success_peg_26;
                                    out_peg_35:
                                    goto loop_peg_24;
                                    success_peg_26:
                                    ;
                                    result_peg_7.addResult(result_peg_25);
                                } while (true);
                                loop_peg_24:
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
                                int position_peg_40 = result_peg_7.getPosition();
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_54(result_peg_7.getPosition());
                                    {
                                        int position_peg_57 = result_peg_54.getPosition();
                                        
                                        result_peg_54.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_54.getPosition()))){
                                                result_peg_54.nextPosition();
                                            } else {
                                                result_peg_54.setPosition(position_peg_57);
                                                goto out_peg_58;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_55;
                                    out_peg_58:
                                    {
                                        int position_peg_60 = result_peg_54.getPosition();
                                        
                                        result_peg_54.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_54.getPosition()))){
                                                result_peg_54.nextPosition();
                                            } else {
                                                result_peg_54.setPosition(position_peg_60);
                                                goto out_peg_61;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_55;
                                    out_peg_61:
                                    {
                                        int position_peg_63 = result_peg_54.getPosition();
                                        
                                        result_peg_54.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_54.getPosition()) == (unsigned char) 255){
                                            result_peg_54.nextPosition();
                                        } else {
                                            result_peg_54.setPosition(position_peg_63);
                                            goto out_peg_64;
                                        }
                                        
                                    }
                                    goto success_peg_55;
                                    out_peg_64:
                                    goto loop_peg_53;
                                    success_peg_55:
                                    ;
                                    result_peg_7.addResult(result_peg_54);
                                } while (true);
                                loop_peg_53:
                                ;
                                        
                            }
                            goto success_peg_38;
                            goto loop_peg_6;
                            success_peg_38:
                            ;
                        
                        
                        
                        int save_peg_65 = result_peg_7.getPosition();
                            
                            result_peg_7 = rule_value(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                
                                result_peg_7 = Result(save_peg_65);
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
            
            Result result_peg_5 = result_peg_4;
            
            result_peg_4 = rule_integer(stream, result_peg_4.getPosition());
                if (result_peg_4.error()){
                    goto out_peg_6;
                }
            
            
            
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
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_5.getValues());
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
            
            Result result_peg_11 = result_peg_10;
            
            result_peg_10 = rule_integer(stream, result_peg_10.getPosition());
                if (result_peg_10.error()){
                    goto out_peg_12;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_11.getValues());
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
            
            Result result_peg_18 = result_peg_17;
            
            result_peg_17 = rule_integer(stream, result_peg_17.getPosition());
                if (result_peg_17.error()){
                    goto out_peg_19;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_18.getValues());
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

    
