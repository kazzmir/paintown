

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
        /*
        for (int i = 0; i < memo_size; i++){
            memo[i] = new Column();
        }
        */
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
        memset(&newMemo[memo_size], 0, sizeof(Column*) * (newSize - memo_size));
        /*
        for (int i = memo_size; i < newSize; i++){
            newMemo[i] = new Column();
        }
        */
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
                                result_peg_11.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_11);
                } while (true);
                loop_peg_10:
                ;
            
            
            
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
    
    RuleTrace trace_peg_49(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            result_peg_16.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            result_peg_16.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        {
                            
                            result_peg_16.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_16.getPosition()) == (unsigned char) 255){
                                result_peg_16.nextPosition();
                            } else {
                                goto out_peg_23;
                            }
                            
                        }
                        goto success_peg_17;
                        out_peg_23:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_24;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_24;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_24:
        Result result_peg_25(myposition);
        
        {
        
            {
                    
                    result_peg_25.reset();
                    do{
                        Result result_peg_39(result_peg_25.getPosition());
                        {
                            
                            result_peg_39.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    goto out_peg_42;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_42:
                        {
                            
                            result_peg_39.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    goto out_peg_44;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_44:
                        {
                            
                            result_peg_39.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_39.getPosition()) == (unsigned char) 255){
                                result_peg_39.nextPosition();
                            } else {
                                goto out_peg_46;
                            }
                            
                        }
                        goto success_peg_40;
                        out_peg_46:
                        goto loop_peg_38;
                        success_peg_40:
                        ;
                        result_peg_25.addResult(result_peg_39);
                    } while (true);
                    loop_peg_38:
                    ;
                            
                }
                goto success_peg_27;
                goto out_peg_47;
                success_peg_27:
                ;
            
            
            
            result_peg_25 = rule_action(stream, result_peg_25.getPosition());
                if (result_peg_25.error()){
                    goto out_peg_47;
                }
            
            Result result_peg_48 = result_peg_25;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_48.getValues());
                    result_peg_25.setValue(value);
                }
            
            
        }
        stream.update(result_peg_25.getPosition());
        
        return result_peg_25;
        out_peg_47:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_action(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_action.calculated()){
        return column_peg_1.chunk0->chunk_action;
    }
    
    RuleTrace trace_peg_15(stream, "action");
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
                    result_peg_9 = rule_newline(stream, result_peg_9.getPosition());
                    if (result_peg_9.error()){
                        goto loop_peg_8;
                    }
                    result_peg_2.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_12(result_peg_2.getPosition());
                    {
                    
                        result_peg_12 = rule_action_line(stream, result_peg_12.getPosition(), ast);
                            if (result_peg_12.error()){
                                goto loop_peg_11;
                            }
                        
                        
                        
                        result_peg_12 = rule_whitespace(stream, result_peg_12.getPosition());
                            if (result_peg_12.error()){
                                goto loop_peg_11;
                            }
                        
                        
                        
                        result_peg_12 = rule_line_end(stream, result_peg_12.getPosition());
                            if (result_peg_12.error()){
                                goto loop_peg_11;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
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
    
    RuleTrace trace_peg_121(stream, "action_line");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            result_peg_16.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            result_peg_16.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        {
                            
                            result_peg_16.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_16.getPosition()) == (unsigned char) 255){
                                result_peg_16.nextPosition();
                            } else {
                                goto out_peg_23;
                            }
                            
                        }
                        goto success_peg_17;
                        out_peg_23:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_24;
                success_peg_4:
                ;
            
            
            
            result_peg_2 = rule_comment(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_24;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_24:
        Result result_peg_25(myposition);
        
        {
        
            {
                    
                    result_peg_25.reset();
                    do{
                        Result result_peg_39(result_peg_25.getPosition());
                        {
                            
                            result_peg_39.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    goto out_peg_42;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_42:
                        {
                            
                            result_peg_39.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    goto out_peg_44;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_44:
                        {
                            
                            result_peg_39.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_39.getPosition()) == (unsigned char) 255){
                                result_peg_39.nextPosition();
                            } else {
                                goto out_peg_46;
                            }
                            
                        }
                        goto success_peg_40;
                        out_peg_46:
                        goto loop_peg_38;
                        success_peg_40:
                        ;
                        result_peg_25.addResult(result_peg_39);
                    } while (true);
                    loop_peg_38:
                    ;
                            
                }
                goto success_peg_27;
                goto out_peg_47;
                success_peg_27:
                ;
            
            
            
            result_peg_25 = rule_collision_default(stream, result_peg_25.getPosition());
                if (result_peg_25.error()){
                    goto out_peg_47;
                }
                data = result_peg_25.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_25.setValue(value);
                }
            
            
        }
        stream.update(result_peg_25.getPosition());
        
        return result_peg_25;
        out_peg_47:
        Result result_peg_49(myposition);
        
        {
        
            {
                    
                    result_peg_49.reset();
                    do{
                        Result result_peg_63(result_peg_49.getPosition());
                        {
                            
                            result_peg_63.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_63.getPosition()))){
                                    result_peg_63.nextPosition();
                                } else {
                                    goto out_peg_66;
                                }
                            }
                                
                        }
                        goto success_peg_64;
                        out_peg_66:
                        {
                            
                            result_peg_63.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_63.getPosition()))){
                                    result_peg_63.nextPosition();
                                } else {
                                    goto out_peg_68;
                                }
                            }
                                
                        }
                        goto success_peg_64;
                        out_peg_68:
                        {
                            
                            result_peg_63.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_63.getPosition()) == (unsigned char) 255){
                                result_peg_63.nextPosition();
                            } else {
                                goto out_peg_70;
                            }
                            
                        }
                        goto success_peg_64;
                        out_peg_70:
                        goto loop_peg_62;
                        success_peg_64:
                        ;
                        result_peg_49.addResult(result_peg_63);
                    } while (true);
                    loop_peg_62:
                    ;
                            
                }
                goto success_peg_51;
                goto out_peg_71;
                success_peg_51:
                ;
            
            
            
            result_peg_49 = rule_collision(stream, result_peg_49.getPosition());
                if (result_peg_49.error()){
                    goto out_peg_71;
                }
                data = result_peg_49.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_49.setValue(value);
                }
            
            
        }
        stream.update(result_peg_49.getPosition());
        
        return result_peg_49;
        out_peg_71:
        Result result_peg_73(myposition);
        
        {
        
            {
                    
                    result_peg_73.reset();
                    do{
                        Result result_peg_87(result_peg_73.getPosition());
                        {
                            
                            result_peg_87.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    goto out_peg_90;
                                }
                            }
                                
                        }
                        goto success_peg_88;
                        out_peg_90:
                        {
                            
                            result_peg_87.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    goto out_peg_92;
                                }
                            }
                                
                        }
                        goto success_peg_88;
                        out_peg_92:
                        {
                            
                            result_peg_87.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_87.getPosition()) == (unsigned char) 255){
                                result_peg_87.nextPosition();
                            } else {
                                goto out_peg_94;
                            }
                            
                        }
                        goto success_peg_88;
                        out_peg_94:
                        goto loop_peg_86;
                        success_peg_88:
                        ;
                        result_peg_73.addResult(result_peg_87);
                    } while (true);
                    loop_peg_86:
                    ;
                            
                }
                goto success_peg_75;
                goto out_peg_95;
                success_peg_75:
                ;
            
            
            
            result_peg_73 = rule_valuelist(stream, result_peg_73.getPosition());
                if (result_peg_73.error()){
                    goto out_peg_95;
                }
                data = result_peg_73.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addValue(as<Ast::Value*>(data));
                    result_peg_73.setValue(value);
                }
            
            
        }
        stream.update(result_peg_73.getPosition());
        
        return result_peg_73;
        out_peg_95:
        Result result_peg_97(myposition);
        
        {
        
            {
                    
                    result_peg_97.reset();
                    do{
                        Result result_peg_111(result_peg_97.getPosition());
                        {
                            
                            result_peg_111.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_111.getPosition()))){
                                    result_peg_111.nextPosition();
                                } else {
                                    goto out_peg_114;
                                }
                            }
                                
                        }
                        goto success_peg_112;
                        out_peg_114:
                        {
                            
                            result_peg_111.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_111.getPosition()))){
                                    result_peg_111.nextPosition();
                                } else {
                                    goto out_peg_116;
                                }
                            }
                                
                        }
                        goto success_peg_112;
                        out_peg_116:
                        {
                            
                            result_peg_111.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_111.getPosition()) == (unsigned char) 255){
                                result_peg_111.nextPosition();
                            } else {
                                goto out_peg_118;
                            }
                            
                        }
                        goto success_peg_112;
                        out_peg_118:
                        goto loop_peg_110;
                        success_peg_112:
                        ;
                        result_peg_97.addResult(result_peg_111);
                    } while (true);
                    loop_peg_110:
                    ;
                            
                }
                goto success_peg_99;
                goto out_peg_119;
                success_peg_99:
                ;
            
            
            
            result_peg_97 = rule_loopstart(stream, result_peg_97.getPosition());
                if (result_peg_97.error()){
                    goto out_peg_119;
                }
                data = result_peg_97.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(section)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_97.setValue(value);
                }
            
            
        }
        stream.update(result_peg_97.getPosition());
        
        return result_peg_97;
        out_peg_119:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_line_end.calculated()){
        return column_peg_1.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_10(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2.reset();
        do{
            Result result_peg_4(result_peg_2.getPosition());
            result_peg_4 = rule_newline(stream, result_peg_4.getPosition());
            if (result_peg_4.error()){
                goto loop_peg_3;
            }
            result_peg_2.addResult(result_peg_4);
        } while (true);
        loop_peg_3:
        if (result_peg_2.matches() == 0){
            goto out_peg_5;
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_5:
        Result result_peg_6(myposition);
        
        {
        
            Result result_peg_8(result_peg_6.getPosition());
                if ('\0' == stream.get(result_peg_8.getPosition())){
                    result_peg_8.nextPosition();
                    result_peg_8.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_9;
                }
            
            
            
            
            
            
        }
        
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = result_peg_6;
        stream.update(result_peg_6.getPosition());
        
        
        return result_peg_6;
        out_peg_9:
    
        if (column_peg_1.chunk0 == 0){
            column_peg_1.chunk0 = new Chunk0();
        }
        column_peg_1.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_newline(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_newline.calculated()){
        return column_peg_1.chunk1->chunk_newline;
    }
    
    RuleTrace trace_peg_6(stream, "newline");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2.setValue(Value((void*) "\n"));
        for (int i = 0; i < 1; i++){
            if (compareChar("\n"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_newline = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        result_peg_4.setValue(Value((void*) "\r"));
        for (int i = 0; i < 1; i++){
            if (compareChar("\r"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_newline = result_peg_4;
        stream.update(result_peg_4.getPosition());
        
        
        return result_peg_4;
        out_peg_5:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_newline = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_loopstart.calculated()){
        return column_peg_1.chunk1->chunk_loopstart;
    }
    
    RuleTrace trace_peg_5(stream, "loopstart");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.setValue(Value((void*) "loopstart"));
                for (int i = 0; i < 9; i++){
                    if (compareCharCase("loopstart"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            Result result_peg_3 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_3.getValues())));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_loopstart = result_peg_2;
        stream.update(result_peg_2.getPosition());
        
        
        return result_peg_2;
        out_peg_4:
    
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_loopstart = errorResult;
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
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk1 != 0 && column_peg_1.chunk1->chunk_sw.calculated()){
        return column_peg_1.chunk1->chunk_sw;
    }
    
    RuleTrace trace_peg_13(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
            
            result_peg_2.setValue(Value((void*) " "));
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    goto out_peg_5;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_5:
        {
            
            result_peg_2.setValue(Value((void*) "\t"));
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_2.getPosition()))){
                    result_peg_2.nextPosition();
                } else {
                    goto out_peg_7;
                }
            }
                
        }
        goto success_peg_3;
        out_peg_7:
        {
            
            result_peg_2.setValue(Value((void*) 255));
            if ((unsigned char) stream.get(result_peg_2.getPosition()) == (unsigned char) 255){
                result_peg_2.nextPosition();
            } else {
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
    
    RuleTrace trace_peg_30(stream, "comment");
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
                            result_peg_9 = rule_newline(stream, result_peg_9.getPosition());
                            if (result_peg_9.error()){
                                goto not_peg_8;
                            }
                            goto loop_peg_5;
                            not_peg_8:
                            result_peg_6.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_10 = stream.get(result_peg_6.getPosition());
                            if (temp_peg_10 != '\0'){
                                result_peg_6.setValue(Value((void*) (long) temp_peg_10));
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
        Result result_peg_11(myposition);
        
        {
        
            result_peg_11.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_11.getPosition()))){
                        result_peg_11.nextPosition();
                    } else {
                        goto out_peg_13;
                    }
                }
            
            
            
            result_peg_11.reset();
                do{
                    Result result_peg_15(result_peg_11.getPosition());
                    {
                    
                        Result result_peg_18(result_peg_15);
                            result_peg_18 = rule_newline(stream, result_peg_18.getPosition());
                            if (result_peg_18.error()){
                                goto not_peg_17;
                            }
                            goto loop_peg_14;
                            not_peg_17:
                            result_peg_15.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_19 = stream.get(result_peg_15.getPosition());
                            if (temp_peg_19 != '\0'){
                                result_peg_15.setValue(Value((void*) (long) temp_peg_19));
                                result_peg_15.nextPosition();
                            } else {
                                goto loop_peg_14;
                            }
                        
                        
                    }
                    result_peg_11.addResult(result_peg_15);
                } while (true);
                loop_peg_14:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_11;
        stream.update(result_peg_11.getPosition());
        
        
        return result_peg_11;
        out_peg_13:
        Result result_peg_20(myposition);
        
        {
        
            result_peg_20.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_20.getPosition()))){
                        result_peg_20.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            
            
            result_peg_20.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_20.getPosition()))){
                        result_peg_20.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            
            
            result_peg_20.reset();
                do{
                    Result result_peg_25(result_peg_20.getPosition());
                    {
                    
                        Result result_peg_28(result_peg_25);
                            result_peg_28 = rule_newline(stream, result_peg_28.getPosition());
                            if (result_peg_28.error()){
                                goto not_peg_27;
                            }
                            goto loop_peg_24;
                            not_peg_27:
                            result_peg_25.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_29 = stream.get(result_peg_25.getPosition());
                            if (temp_peg_29 != '\0'){
                                result_peg_25.setValue(Value((void*) (long) temp_peg_29));
                                result_peg_25.nextPosition();
                            } else {
                                goto loop_peg_24;
                            }
                        
                        
                    }
                    result_peg_20.addResult(result_peg_25);
                } while (true);
                loop_peg_24:
                ;
            
            
        }
        
        if (column_peg_1.chunk1 == 0){
            column_peg_1.chunk1 = new Chunk1();
        }
        column_peg_1.chunk1->chunk_comment = result_peg_20;
        stream.update(result_peg_20.getPosition());
        
        
        return result_peg_20;
        out_peg_22:
    
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
    
    RuleTrace trace_peg_106(stream, "collision_default");
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_19(result_peg_2.getPosition());
                        {
                            
                            result_peg_19.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                    result_peg_19.nextPosition();
                                } else {
                                    goto out_peg_22;
                                }
                            }
                                
                        }
                        goto success_peg_20;
                        out_peg_22:
                        {
                            
                            result_peg_19.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                    result_peg_19.nextPosition();
                                } else {
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_20;
                        out_peg_24:
                        {
                            
                            result_peg_19.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_19.getPosition()) == (unsigned char) 255){
                                result_peg_19.nextPosition();
                            } else {
                                goto out_peg_26;
                            }
                            
                        }
                        goto success_peg_20;
                        out_peg_26:
                        goto loop_peg_18;
                        success_peg_20:
                        ;
                        result_peg_2.addResult(result_peg_19);
                    } while (true);
                    loop_peg_18:
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
        Result result_peg_28(myposition);
        
        {
        
            result_peg_28.setValue(Value((void*) "Clsn1Default"));
                for (int i = 0; i < 12; i++){
                    if (compareCharCase("Clsn1Default"[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_30;
                    }
                }
            
            Result result_peg_29 = result_peg_28;
            
            result_peg_28.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_30;
                    }
                }
            
            
            
            {
                    
                    result_peg_28.reset();
                    do{
                        Result result_peg_45(result_peg_28.getPosition());
                        {
                            
                            result_peg_45.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_45.getPosition()))){
                                    result_peg_45.nextPosition();
                                } else {
                                    goto out_peg_48;
                                }
                            }
                                
                        }
                        goto success_peg_46;
                        out_peg_48:
                        {
                            
                            result_peg_45.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_45.getPosition()))){
                                    result_peg_45.nextPosition();
                                } else {
                                    goto out_peg_50;
                                }
                            }
                                
                        }
                        goto success_peg_46;
                        out_peg_50:
                        {
                            
                            result_peg_45.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_45.getPosition()) == (unsigned char) 255){
                                result_peg_45.nextPosition();
                            } else {
                                goto out_peg_52;
                            }
                            
                        }
                        goto success_peg_46;
                        out_peg_52:
                        goto loop_peg_44;
                        success_peg_46:
                        ;
                        result_peg_28.addResult(result_peg_45);
                    } while (true);
                    loop_peg_44:
                    ;
                            
                }
                goto success_peg_33;
                goto out_peg_30;
                success_peg_33:
                ;
            
            
            
            result_peg_28 = rule_integer(stream, result_peg_28.getPosition());
                if (result_peg_28.error()){
                    goto out_peg_30;
                }
                num = result_peg_28.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_29.getValues())), num);
                    result_peg_28.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_28;
        stream.update(result_peg_28.getPosition());
        
        
        return result_peg_28;
        out_peg_30:
        Result result_peg_54(myposition);
        
        {
        
            result_peg_54.setValue(Value((void*) "Clsn2"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn2"[i], stream.get(result_peg_54.getPosition()))){
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
                    
                    result_peg_54.reset();
                    do{
                        Result result_peg_71(result_peg_54.getPosition());
                        {
                            
                            result_peg_71.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_71.getPosition()))){
                                    result_peg_71.nextPosition();
                                } else {
                                    goto out_peg_74;
                                }
                            }
                                
                        }
                        goto success_peg_72;
                        out_peg_74:
                        {
                            
                            result_peg_71.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_71.getPosition()))){
                                    result_peg_71.nextPosition();
                                } else {
                                    goto out_peg_76;
                                }
                            }
                                
                        }
                        goto success_peg_72;
                        out_peg_76:
                        {
                            
                            result_peg_71.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_71.getPosition()) == (unsigned char) 255){
                                result_peg_71.nextPosition();
                            } else {
                                goto out_peg_78;
                            }
                            
                        }
                        goto success_peg_72;
                        out_peg_78:
                        goto loop_peg_70;
                        success_peg_72:
                        ;
                        result_peg_54.addResult(result_peg_71);
                    } while (true);
                    loop_peg_70:
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
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_54;
        stream.update(result_peg_54.getPosition());
        
        
        return result_peg_54;
        out_peg_56:
        Result result_peg_80(myposition);
        
        {
        
            result_peg_80.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_80.getPosition()))){
                        result_peg_80.nextPosition();
                    } else {
                        goto out_peg_82;
                    }
                }
            
            Result result_peg_81 = result_peg_80;
            
            result_peg_80.setValue(Value((void*) ":"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_80.getPosition()))){
                        result_peg_80.nextPosition();
                    } else {
                        goto out_peg_82;
                    }
                }
            
            
            
            {
                    
                    result_peg_80.reset();
                    do{
                        Result result_peg_97(result_peg_80.getPosition());
                        {
                            
                            result_peg_97.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_97.getPosition()))){
                                    result_peg_97.nextPosition();
                                } else {
                                    goto out_peg_100;
                                }
                            }
                                
                        }
                        goto success_peg_98;
                        out_peg_100:
                        {
                            
                            result_peg_97.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_97.getPosition()))){
                                    result_peg_97.nextPosition();
                                } else {
                                    goto out_peg_102;
                                }
                            }
                                
                        }
                        goto success_peg_98;
                        out_peg_102:
                        {
                            
                            result_peg_97.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_97.getPosition()) == (unsigned char) 255){
                                result_peg_97.nextPosition();
                            } else {
                                goto out_peg_104;
                            }
                            
                        }
                        goto success_peg_98;
                        out_peg_104:
                        goto loop_peg_96;
                        success_peg_98:
                        ;
                        result_peg_80.addResult(result_peg_97);
                    } while (true);
                    loop_peg_96:
                    ;
                            
                }
                goto success_peg_85;
                goto out_peg_82;
                success_peg_85:
                ;
            
            
            
            result_peg_80 = rule_integer(stream, result_peg_80.getPosition());
                if (result_peg_80.error()){
                    goto out_peg_82;
                }
                num = result_peg_80.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeKeyword(Value(makeKeyword(result_peg_81.getValues())), num);
                    result_peg_80.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision_default = result_peg_80;
        stream.update(result_peg_80.getPosition());
        
        
        return result_peg_80;
        out_peg_82:
    
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
    
    RuleTrace trace_peg_492(stream, "collision");
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_18(result_peg_2.getPosition());
                        {
                            
                            result_peg_18.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_18.getPosition()))){
                                    result_peg_18.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_19;
                        out_peg_21:
                        {
                            
                            result_peg_18.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_18.getPosition()))){
                                    result_peg_18.nextPosition();
                                } else {
                                    goto out_peg_23;
                                }
                            }
                                
                        }
                        goto success_peg_19;
                        out_peg_23:
                        {
                            
                            result_peg_18.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_18.getPosition()) == (unsigned char) 255){
                                result_peg_18.nextPosition();
                            } else {
                                goto out_peg_25;
                            }
                            
                        }
                        goto success_peg_19;
                        out_peg_25:
                        goto loop_peg_17;
                        success_peg_19:
                        ;
                        result_peg_2.addResult(result_peg_18);
                    } while (true);
                    loop_peg_17:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_40(result_peg_2.getPosition());
                        {
                            
                            result_peg_40.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_43;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_43:
                        {
                            
                            result_peg_40.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_45;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_45:
                        {
                            
                            result_peg_40.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_40.getPosition()) == (unsigned char) 255){
                                result_peg_40.nextPosition();
                            } else {
                                goto out_peg_47;
                            }
                            
                        }
                        goto success_peg_41;
                        out_peg_47:
                        goto loop_peg_39;
                        success_peg_41:
                        ;
                        result_peg_2.addResult(result_peg_40);
                    } while (true);
                    loop_peg_39:
                    ;
                            
                }
                goto success_peg_28;
                goto out_peg_4;
                success_peg_28:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                index = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_62(result_peg_2.getPosition());
                        {
                            
                            result_peg_62.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    goto out_peg_65;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_65:
                        {
                            
                            result_peg_62.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    goto out_peg_67;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_67:
                        {
                            
                            result_peg_62.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_62.getPosition()) == (unsigned char) 255){
                                result_peg_62.nextPosition();
                            } else {
                                goto out_peg_69;
                            }
                            
                        }
                        goto success_peg_63;
                        out_peg_69:
                        goto loop_peg_61;
                        success_peg_63:
                        ;
                        result_peg_2.addResult(result_peg_62);
                    } while (true);
                    loop_peg_61:
                    ;
                            
                }
                goto success_peg_50;
                goto out_peg_4;
                success_peg_50:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_84(result_peg_2.getPosition());
                        {
                            
                            result_peg_84.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    goto out_peg_87;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_87:
                        {
                            
                            result_peg_84.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    goto out_peg_89;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_89:
                        {
                            
                            result_peg_84.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_84.getPosition()) == (unsigned char) 255){
                                result_peg_84.nextPosition();
                            } else {
                                goto out_peg_91;
                            }
                            
                        }
                        goto success_peg_85;
                        out_peg_91:
                        goto loop_peg_83;
                        success_peg_85:
                        ;
                        result_peg_2.addResult(result_peg_84);
                    } while (true);
                    loop_peg_83:
                    ;
                            
                }
                goto success_peg_72;
                goto out_peg_4;
                success_peg_72:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_106(result_peg_2.getPosition());
                        {
                            
                            result_peg_106.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_106.getPosition()))){
                                    result_peg_106.nextPosition();
                                } else {
                                    goto out_peg_109;
                                }
                            }
                                
                        }
                        goto success_peg_107;
                        out_peg_109:
                        {
                            
                            result_peg_106.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_106.getPosition()))){
                                    result_peg_106.nextPosition();
                                } else {
                                    goto out_peg_111;
                                }
                            }
                                
                        }
                        goto success_peg_107;
                        out_peg_111:
                        {
                            
                            result_peg_106.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_106.getPosition()) == (unsigned char) 255){
                                result_peg_106.nextPosition();
                            } else {
                                goto out_peg_113;
                            }
                            
                        }
                        goto success_peg_107;
                        out_peg_113:
                        goto loop_peg_105;
                        success_peg_107:
                        ;
                        result_peg_2.addResult(result_peg_106);
                    } while (true);
                    loop_peg_105:
                    ;
                            
                }
                goto success_peg_94;
                goto out_peg_4;
                success_peg_94:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n1 = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_128(result_peg_2.getPosition());
                        {
                            
                            result_peg_128.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_128.getPosition()))){
                                    result_peg_128.nextPosition();
                                } else {
                                    goto out_peg_131;
                                }
                            }
                                
                        }
                        goto success_peg_129;
                        out_peg_131:
                        {
                            
                            result_peg_128.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_128.getPosition()))){
                                    result_peg_128.nextPosition();
                                } else {
                                    goto out_peg_133;
                                }
                            }
                                
                        }
                        goto success_peg_129;
                        out_peg_133:
                        {
                            
                            result_peg_128.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_128.getPosition()) == (unsigned char) 255){
                                result_peg_128.nextPosition();
                            } else {
                                goto out_peg_135;
                            }
                            
                        }
                        goto success_peg_129;
                        out_peg_135:
                        goto loop_peg_127;
                        success_peg_129:
                        ;
                        result_peg_2.addResult(result_peg_128);
                    } while (true);
                    loop_peg_127:
                    ;
                            
                }
                goto success_peg_116;
                goto out_peg_4;
                success_peg_116:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_150(result_peg_2.getPosition());
                        {
                            
                            result_peg_150.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_150.getPosition()))){
                                    result_peg_150.nextPosition();
                                } else {
                                    goto out_peg_153;
                                }
                            }
                                
                        }
                        goto success_peg_151;
                        out_peg_153:
                        {
                            
                            result_peg_150.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_150.getPosition()))){
                                    result_peg_150.nextPosition();
                                } else {
                                    goto out_peg_155;
                                }
                            }
                                
                        }
                        goto success_peg_151;
                        out_peg_155:
                        {
                            
                            result_peg_150.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_150.getPosition()) == (unsigned char) 255){
                                result_peg_150.nextPosition();
                            } else {
                                goto out_peg_157;
                            }
                            
                        }
                        goto success_peg_151;
                        out_peg_157:
                        goto loop_peg_149;
                        success_peg_151:
                        ;
                        result_peg_2.addResult(result_peg_150);
                    } while (true);
                    loop_peg_149:
                    ;
                            
                }
                goto success_peg_138;
                goto out_peg_4;
                success_peg_138:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n2 = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_172(result_peg_2.getPosition());
                        {
                            
                            result_peg_172.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_172.getPosition()))){
                                    result_peg_172.nextPosition();
                                } else {
                                    goto out_peg_175;
                                }
                            }
                                
                        }
                        goto success_peg_173;
                        out_peg_175:
                        {
                            
                            result_peg_172.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_172.getPosition()))){
                                    result_peg_172.nextPosition();
                                } else {
                                    goto out_peg_177;
                                }
                            }
                                
                        }
                        goto success_peg_173;
                        out_peg_177:
                        {
                            
                            result_peg_172.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_172.getPosition()) == (unsigned char) 255){
                                result_peg_172.nextPosition();
                            } else {
                                goto out_peg_179;
                            }
                            
                        }
                        goto success_peg_173;
                        out_peg_179:
                        goto loop_peg_171;
                        success_peg_173:
                        ;
                        result_peg_2.addResult(result_peg_172);
                    } while (true);
                    loop_peg_171:
                    ;
                            
                }
                goto success_peg_160;
                goto out_peg_4;
                success_peg_160:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_194(result_peg_2.getPosition());
                        {
                            
                            result_peg_194.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_194.getPosition()))){
                                    result_peg_194.nextPosition();
                                } else {
                                    goto out_peg_197;
                                }
                            }
                                
                        }
                        goto success_peg_195;
                        out_peg_197:
                        {
                            
                            result_peg_194.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_194.getPosition()))){
                                    result_peg_194.nextPosition();
                                } else {
                                    goto out_peg_199;
                                }
                            }
                                
                        }
                        goto success_peg_195;
                        out_peg_199:
                        {
                            
                            result_peg_194.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_194.getPosition()) == (unsigned char) 255){
                                result_peg_194.nextPosition();
                            } else {
                                goto out_peg_201;
                            }
                            
                        }
                        goto success_peg_195;
                        out_peg_201:
                        goto loop_peg_193;
                        success_peg_195:
                        ;
                        result_peg_2.addResult(result_peg_194);
                    } while (true);
                    loop_peg_193:
                    ;
                            
                }
                goto success_peg_182;
                goto out_peg_4;
                success_peg_182:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                n3 = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_216(result_peg_2.getPosition());
                        {
                            
                            result_peg_216.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_216.getPosition()))){
                                    result_peg_216.nextPosition();
                                } else {
                                    goto out_peg_219;
                                }
                            }
                                
                        }
                        goto success_peg_217;
                        out_peg_219:
                        {
                            
                            result_peg_216.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_216.getPosition()))){
                                    result_peg_216.nextPosition();
                                } else {
                                    goto out_peg_221;
                                }
                            }
                                
                        }
                        goto success_peg_217;
                        out_peg_221:
                        {
                            
                            result_peg_216.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_216.getPosition()) == (unsigned char) 255){
                                result_peg_216.nextPosition();
                            } else {
                                goto out_peg_223;
                            }
                            
                        }
                        goto success_peg_217;
                        out_peg_223:
                        goto loop_peg_215;
                        success_peg_217:
                        ;
                        result_peg_2.addResult(result_peg_216);
                    } while (true);
                    loop_peg_215:
                    ;
                            
                }
                goto success_peg_204;
                goto out_peg_4;
                success_peg_204:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_238(result_peg_2.getPosition());
                        {
                            
                            result_peg_238.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_238.getPosition()))){
                                    result_peg_238.nextPosition();
                                } else {
                                    goto out_peg_241;
                                }
                            }
                                
                        }
                        goto success_peg_239;
                        out_peg_241:
                        {
                            
                            result_peg_238.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_238.getPosition()))){
                                    result_peg_238.nextPosition();
                                } else {
                                    goto out_peg_243;
                                }
                            }
                                
                        }
                        goto success_peg_239;
                        out_peg_243:
                        {
                            
                            result_peg_238.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_238.getPosition()) == (unsigned char) 255){
                                result_peg_238.nextPosition();
                            } else {
                                goto out_peg_245;
                            }
                            
                        }
                        goto success_peg_239;
                        out_peg_245:
                        goto loop_peg_237;
                        success_peg_239:
                        ;
                        result_peg_2.addResult(result_peg_238);
                    } while (true);
                    loop_peg_237:
                    ;
                            
                }
                goto success_peg_226;
                goto out_peg_4;
                success_peg_226:
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
        Result result_peg_247(myposition);
        
        {
        
            result_peg_247.setValue(Value((void*) "Clsn1"));
                for (int i = 0; i < 5; i++){
                    if (compareCharCase("Clsn1"[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
                name = result_peg_247.getValues();
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_263(result_peg_247.getPosition());
                        {
                            
                            result_peg_263.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_263.getPosition()))){
                                    result_peg_263.nextPosition();
                                } else {
                                    goto out_peg_266;
                                }
                            }
                                
                        }
                        goto success_peg_264;
                        out_peg_266:
                        {
                            
                            result_peg_263.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_263.getPosition()))){
                                    result_peg_263.nextPosition();
                                } else {
                                    goto out_peg_268;
                                }
                            }
                                
                        }
                        goto success_peg_264;
                        out_peg_268:
                        {
                            
                            result_peg_263.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_263.getPosition()) == (unsigned char) 255){
                                result_peg_263.nextPosition();
                            } else {
                                goto out_peg_270;
                            }
                            
                        }
                        goto success_peg_264;
                        out_peg_270:
                        goto loop_peg_262;
                        success_peg_264:
                        ;
                        result_peg_247.addResult(result_peg_263);
                    } while (true);
                    loop_peg_262:
                    ;
                            
                }
                goto success_peg_251;
                goto out_peg_249;
                success_peg_251:
                ;
            
            
            
            result_peg_247.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_285(result_peg_247.getPosition());
                        {
                            
                            result_peg_285.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_285.getPosition()))){
                                    result_peg_285.nextPosition();
                                } else {
                                    goto out_peg_288;
                                }
                            }
                                
                        }
                        goto success_peg_286;
                        out_peg_288:
                        {
                            
                            result_peg_285.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_285.getPosition()))){
                                    result_peg_285.nextPosition();
                                } else {
                                    goto out_peg_290;
                                }
                            }
                                
                        }
                        goto success_peg_286;
                        out_peg_290:
                        {
                            
                            result_peg_285.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_285.getPosition()) == (unsigned char) 255){
                                result_peg_285.nextPosition();
                            } else {
                                goto out_peg_292;
                            }
                            
                        }
                        goto success_peg_286;
                        out_peg_292:
                        goto loop_peg_284;
                        success_peg_286:
                        ;
                        result_peg_247.addResult(result_peg_285);
                    } while (true);
                    loop_peg_284:
                    ;
                            
                }
                goto success_peg_273;
                goto out_peg_249;
                success_peg_273:
                ;
            
            
            
            result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
                if (result_peg_247.error()){
                    goto out_peg_249;
                }
                index = result_peg_247.getValues();
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_307(result_peg_247.getPosition());
                        {
                            
                            result_peg_307.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_307.getPosition()))){
                                    result_peg_307.nextPosition();
                                } else {
                                    goto out_peg_310;
                                }
                            }
                                
                        }
                        goto success_peg_308;
                        out_peg_310:
                        {
                            
                            result_peg_307.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_307.getPosition()))){
                                    result_peg_307.nextPosition();
                                } else {
                                    goto out_peg_312;
                                }
                            }
                                
                        }
                        goto success_peg_308;
                        out_peg_312:
                        {
                            
                            result_peg_307.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_307.getPosition()) == (unsigned char) 255){
                                result_peg_307.nextPosition();
                            } else {
                                goto out_peg_314;
                            }
                            
                        }
                        goto success_peg_308;
                        out_peg_314:
                        goto loop_peg_306;
                        success_peg_308:
                        ;
                        result_peg_247.addResult(result_peg_307);
                    } while (true);
                    loop_peg_306:
                    ;
                            
                }
                goto success_peg_295;
                goto out_peg_249;
                success_peg_295:
                ;
            
            
            
            result_peg_247.setValue(Value((void*) "]"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_329(result_peg_247.getPosition());
                        {
                            
                            result_peg_329.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_329.getPosition()))){
                                    result_peg_329.nextPosition();
                                } else {
                                    goto out_peg_332;
                                }
                            }
                                
                        }
                        goto success_peg_330;
                        out_peg_332:
                        {
                            
                            result_peg_329.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_329.getPosition()))){
                                    result_peg_329.nextPosition();
                                } else {
                                    goto out_peg_334;
                                }
                            }
                                
                        }
                        goto success_peg_330;
                        out_peg_334:
                        {
                            
                            result_peg_329.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_329.getPosition()) == (unsigned char) 255){
                                result_peg_329.nextPosition();
                            } else {
                                goto out_peg_336;
                            }
                            
                        }
                        goto success_peg_330;
                        out_peg_336:
                        goto loop_peg_328;
                        success_peg_330:
                        ;
                        result_peg_247.addResult(result_peg_329);
                    } while (true);
                    loop_peg_328:
                    ;
                            
                }
                goto success_peg_317;
                goto out_peg_249;
                success_peg_317:
                ;
            
            
            
            result_peg_247.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_351(result_peg_247.getPosition());
                        {
                            
                            result_peg_351.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_351.getPosition()))){
                                    result_peg_351.nextPosition();
                                } else {
                                    goto out_peg_354;
                                }
                            }
                                
                        }
                        goto success_peg_352;
                        out_peg_354:
                        {
                            
                            result_peg_351.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_351.getPosition()))){
                                    result_peg_351.nextPosition();
                                } else {
                                    goto out_peg_356;
                                }
                            }
                                
                        }
                        goto success_peg_352;
                        out_peg_356:
                        {
                            
                            result_peg_351.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_351.getPosition()) == (unsigned char) 255){
                                result_peg_351.nextPosition();
                            } else {
                                goto out_peg_358;
                            }
                            
                        }
                        goto success_peg_352;
                        out_peg_358:
                        goto loop_peg_350;
                        success_peg_352:
                        ;
                        result_peg_247.addResult(result_peg_351);
                    } while (true);
                    loop_peg_350:
                    ;
                            
                }
                goto success_peg_339;
                goto out_peg_249;
                success_peg_339:
                ;
            
            
            
            result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
                if (result_peg_247.error()){
                    goto out_peg_249;
                }
                n1 = result_peg_247.getValues();
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_373(result_peg_247.getPosition());
                        {
                            
                            result_peg_373.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_373.getPosition()))){
                                    result_peg_373.nextPosition();
                                } else {
                                    goto out_peg_376;
                                }
                            }
                                
                        }
                        goto success_peg_374;
                        out_peg_376:
                        {
                            
                            result_peg_373.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_373.getPosition()))){
                                    result_peg_373.nextPosition();
                                } else {
                                    goto out_peg_378;
                                }
                            }
                                
                        }
                        goto success_peg_374;
                        out_peg_378:
                        {
                            
                            result_peg_373.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_373.getPosition()) == (unsigned char) 255){
                                result_peg_373.nextPosition();
                            } else {
                                goto out_peg_380;
                            }
                            
                        }
                        goto success_peg_374;
                        out_peg_380:
                        goto loop_peg_372;
                        success_peg_374:
                        ;
                        result_peg_247.addResult(result_peg_373);
                    } while (true);
                    loop_peg_372:
                    ;
                            
                }
                goto success_peg_361;
                goto out_peg_249;
                success_peg_361:
                ;
            
            
            
            result_peg_247.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_395(result_peg_247.getPosition());
                        {
                            
                            result_peg_395.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_395.getPosition()))){
                                    result_peg_395.nextPosition();
                                } else {
                                    goto out_peg_398;
                                }
                            }
                                
                        }
                        goto success_peg_396;
                        out_peg_398:
                        {
                            
                            result_peg_395.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_395.getPosition()))){
                                    result_peg_395.nextPosition();
                                } else {
                                    goto out_peg_400;
                                }
                            }
                                
                        }
                        goto success_peg_396;
                        out_peg_400:
                        {
                            
                            result_peg_395.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_395.getPosition()) == (unsigned char) 255){
                                result_peg_395.nextPosition();
                            } else {
                                goto out_peg_402;
                            }
                            
                        }
                        goto success_peg_396;
                        out_peg_402:
                        goto loop_peg_394;
                        success_peg_396:
                        ;
                        result_peg_247.addResult(result_peg_395);
                    } while (true);
                    loop_peg_394:
                    ;
                            
                }
                goto success_peg_383;
                goto out_peg_249;
                success_peg_383:
                ;
            
            
            
            result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
                if (result_peg_247.error()){
                    goto out_peg_249;
                }
                n2 = result_peg_247.getValues();
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_417(result_peg_247.getPosition());
                        {
                            
                            result_peg_417.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_417.getPosition()))){
                                    result_peg_417.nextPosition();
                                } else {
                                    goto out_peg_420;
                                }
                            }
                                
                        }
                        goto success_peg_418;
                        out_peg_420:
                        {
                            
                            result_peg_417.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_417.getPosition()))){
                                    result_peg_417.nextPosition();
                                } else {
                                    goto out_peg_422;
                                }
                            }
                                
                        }
                        goto success_peg_418;
                        out_peg_422:
                        {
                            
                            result_peg_417.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_417.getPosition()) == (unsigned char) 255){
                                result_peg_417.nextPosition();
                            } else {
                                goto out_peg_424;
                            }
                            
                        }
                        goto success_peg_418;
                        out_peg_424:
                        goto loop_peg_416;
                        success_peg_418:
                        ;
                        result_peg_247.addResult(result_peg_417);
                    } while (true);
                    loop_peg_416:
                    ;
                            
                }
                goto success_peg_405;
                goto out_peg_249;
                success_peg_405:
                ;
            
            
            
            result_peg_247.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_439(result_peg_247.getPosition());
                        {
                            
                            result_peg_439.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_439.getPosition()))){
                                    result_peg_439.nextPosition();
                                } else {
                                    goto out_peg_442;
                                }
                            }
                                
                        }
                        goto success_peg_440;
                        out_peg_442:
                        {
                            
                            result_peg_439.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_439.getPosition()))){
                                    result_peg_439.nextPosition();
                                } else {
                                    goto out_peg_444;
                                }
                            }
                                
                        }
                        goto success_peg_440;
                        out_peg_444:
                        {
                            
                            result_peg_439.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_439.getPosition()) == (unsigned char) 255){
                                result_peg_439.nextPosition();
                            } else {
                                goto out_peg_446;
                            }
                            
                        }
                        goto success_peg_440;
                        out_peg_446:
                        goto loop_peg_438;
                        success_peg_440:
                        ;
                        result_peg_247.addResult(result_peg_439);
                    } while (true);
                    loop_peg_438:
                    ;
                            
                }
                goto success_peg_427;
                goto out_peg_249;
                success_peg_427:
                ;
            
            
            
            result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
                if (result_peg_247.error()){
                    goto out_peg_249;
                }
                n3 = result_peg_247.getValues();
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_461(result_peg_247.getPosition());
                        {
                            
                            result_peg_461.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_461.getPosition()))){
                                    result_peg_461.nextPosition();
                                } else {
                                    goto out_peg_464;
                                }
                            }
                                
                        }
                        goto success_peg_462;
                        out_peg_464:
                        {
                            
                            result_peg_461.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_461.getPosition()))){
                                    result_peg_461.nextPosition();
                                } else {
                                    goto out_peg_466;
                                }
                            }
                                
                        }
                        goto success_peg_462;
                        out_peg_466:
                        {
                            
                            result_peg_461.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_461.getPosition()) == (unsigned char) 255){
                                result_peg_461.nextPosition();
                            } else {
                                goto out_peg_468;
                            }
                            
                        }
                        goto success_peg_462;
                        out_peg_468:
                        goto loop_peg_460;
                        success_peg_462:
                        ;
                        result_peg_247.addResult(result_peg_461);
                    } while (true);
                    loop_peg_460:
                    ;
                            
                }
                goto success_peg_449;
                goto out_peg_249;
                success_peg_449:
                ;
            
            
            
            result_peg_247.setValue(Value((void*) ","));
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_247.getPosition()))){
                        result_peg_247.nextPosition();
                    } else {
                        goto out_peg_249;
                    }
                }
            
            
            
            {
                    
                    result_peg_247.reset();
                    do{
                        Result result_peg_483(result_peg_247.getPosition());
                        {
                            
                            result_peg_483.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_483.getPosition()))){
                                    result_peg_483.nextPosition();
                                } else {
                                    goto out_peg_486;
                                }
                            }
                                
                        }
                        goto success_peg_484;
                        out_peg_486:
                        {
                            
                            result_peg_483.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_483.getPosition()))){
                                    result_peg_483.nextPosition();
                                } else {
                                    goto out_peg_488;
                                }
                            }
                                
                        }
                        goto success_peg_484;
                        out_peg_488:
                        {
                            
                            result_peg_483.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_483.getPosition()) == (unsigned char) 255){
                                result_peg_483.nextPosition();
                            } else {
                                goto out_peg_490;
                            }
                            
                        }
                        goto success_peg_484;
                        out_peg_490:
                        goto loop_peg_482;
                        success_peg_484:
                        ;
                        result_peg_247.addResult(result_peg_483);
                    } while (true);
                    loop_peg_482:
                    ;
                            
                }
                goto success_peg_471;
                goto out_peg_249;
                success_peg_471:
                ;
            
            
            
            result_peg_247 = rule_integer(stream, result_peg_247.getPosition());
                if (result_peg_247.error()){
                    goto out_peg_249;
                }
                n4 = result_peg_247.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeArray(makeKeyword(name), as<Ast::Value*>(index), makeValueList(n1, n2, n3, n4));
                    result_peg_247.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk2 == 0){
            column_peg_1.chunk2 = new Chunk2();
        }
        column_peg_1.chunk2->chunk_collision = result_peg_247;
        stream.update(result_peg_247.getPosition());
        
        
        return result_peg_247;
        out_peg_249:
    
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
    
    RuleTrace trace_peg_93(stream, "action_start");
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_18(result_peg_2.getPosition());
                        {
                            
                            result_peg_18.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_18.getPosition()))){
                                    result_peg_18.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_19;
                        out_peg_21:
                        {
                            
                            result_peg_18.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_18.getPosition()))){
                                    result_peg_18.nextPosition();
                                } else {
                                    goto out_peg_23;
                                }
                            }
                                
                        }
                        goto success_peg_19;
                        out_peg_23:
                        {
                            
                            result_peg_18.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_18.getPosition()) == (unsigned char) 255){
                                result_peg_18.nextPosition();
                            } else {
                                goto out_peg_25;
                            }
                            
                        }
                        goto success_peg_19;
                        out_peg_25:
                        goto loop_peg_17;
                        success_peg_19:
                        ;
                        result_peg_2.addResult(result_peg_18);
                    } while (true);
                    loop_peg_17:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_40(result_peg_2.getPosition());
                        {
                            
                            result_peg_40.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_43;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_43:
                        {
                            
                            result_peg_40.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_45;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_45:
                        {
                            
                            result_peg_40.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_40.getPosition()) == (unsigned char) 255){
                                result_peg_40.nextPosition();
                            } else {
                                goto out_peg_47;
                            }
                            
                        }
                        goto success_peg_41;
                        out_peg_47:
                        goto loop_peg_39;
                        success_peg_41:
                        ;
                        result_peg_2.addResult(result_peg_40);
                    } while (true);
                    loop_peg_39:
                    ;
                            
                }
                goto success_peg_28;
                goto out_peg_4;
                success_peg_28:
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_62(result_peg_2.getPosition());
                        {
                            
                            result_peg_62.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    goto out_peg_65;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_65:
                        {
                            
                            result_peg_62.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    goto out_peg_67;
                                }
                            }
                                
                        }
                        goto success_peg_63;
                        out_peg_67:
                        {
                            
                            result_peg_62.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_62.getPosition()) == (unsigned char) 255){
                                result_peg_62.nextPosition();
                            } else {
                                goto out_peg_69;
                            }
                            
                        }
                        goto success_peg_63;
                        out_peg_69:
                        goto loop_peg_61;
                        success_peg_63:
                        ;
                        result_peg_2.addResult(result_peg_62);
                    } while (true);
                    loop_peg_61:
                    ;
                            
                }
                goto success_peg_50;
                goto out_peg_4;
                success_peg_50:
                ;
            
            
            
            result_peg_2 = rule_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                num = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_84(result_peg_2.getPosition());
                        {
                            
                            result_peg_84.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    goto out_peg_87;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_87:
                        {
                            
                            result_peg_84.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    goto out_peg_89;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_89:
                        {
                            
                            result_peg_84.setValue(Value((void*) 255));
                            if ((unsigned char) stream.get(result_peg_84.getPosition()) == (unsigned char) 255){
                                result_peg_84.nextPosition();
                            } else {
                                goto out_peg_91;
                            }
                            
                        }
                        goto success_peg_85;
                        out_peg_91:
                        goto loop_peg_83;
                        success_peg_85:
                        ;
                        result_peg_2.addResult(result_peg_84);
                    } while (true);
                    loop_peg_83:
                    ;
                            
                }
                goto success_peg_72;
                goto out_peg_4;
                success_peg_72:
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
    
    RuleTrace trace_peg_19(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    
                    result_peg_2.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_7;
                        }
                    }
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    
                    result_peg_2.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
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
                        
                        char letter_peg_17 = stream.get(result_peg_12.getPosition());
                        if (letter_peg_17 != '\0' && strchr("0123456789", letter_peg_17) != NULL){
                            result_peg_12.nextPosition();
                            result_peg_12.setValue(Value((void*) (long) letter_peg_17));
                        } else {
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
                    goto out_peg_18;
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
        out_peg_18:
    
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
    
    RuleTrace trace_peg_52(stream, "valuelist");
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
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_21(result_peg_7.getPosition());
                                    {
                                        
                                        result_peg_21.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                                result_peg_21.nextPosition();
                                            } else {
                                                goto out_peg_24;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_22;
                                    out_peg_24:
                                    {
                                        
                                        result_peg_21.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                                result_peg_21.nextPosition();
                                            } else {
                                                goto out_peg_26;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_22;
                                    out_peg_26:
                                    {
                                        
                                        result_peg_21.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_21.getPosition()) == (unsigned char) 255){
                                            result_peg_21.nextPosition();
                                        } else {
                                            goto out_peg_28;
                                        }
                                        
                                    }
                                    goto success_peg_22;
                                    out_peg_28:
                                    goto loop_peg_20;
                                    success_peg_22:
                                    ;
                                    result_peg_7.addResult(result_peg_21);
                                } while (true);
                                loop_peg_20:
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
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_43(result_peg_7.getPosition());
                                    {
                                        
                                        result_peg_43.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_43.getPosition()))){
                                                result_peg_43.nextPosition();
                                            } else {
                                                goto out_peg_46;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_44;
                                    out_peg_46:
                                    {
                                        
                                        result_peg_43.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_43.getPosition()))){
                                                result_peg_43.nextPosition();
                                            } else {
                                                goto out_peg_48;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_44;
                                    out_peg_48:
                                    {
                                        
                                        result_peg_43.setValue(Value((void*) 255));
                                        if ((unsigned char) stream.get(result_peg_43.getPosition()) == (unsigned char) 255){
                                            result_peg_43.nextPosition();
                                        } else {
                                            goto out_peg_50;
                                        }
                                        
                                    }
                                    goto success_peg_44;
                                    out_peg_50:
                                    goto loop_peg_42;
                                    success_peg_44:
                                    ;
                                    result_peg_7.addResult(result_peg_43);
                                } while (true);
                                loop_peg_42:
                                ;
                                        
                            }
                            goto success_peg_31;
                            goto loop_peg_6;
                            success_peg_31:
                            ;
                        
                        
                        
                        int save_peg_51 = result_peg_7.getPosition();
                            
                            result_peg_7 = rule_value(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                
                                result_peg_7 = Result(save_peg_51);
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
    
    RuleTrace trace_peg_32(stream, "value");
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
        Result result_peg_20(myposition);
        
        {
        
            result_peg_20.setValue(Value((void*) "vh"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("vh"[i], stream.get(result_peg_20.getPosition()))){
                        result_peg_20.nextPosition();
                    } else {
                        goto out_peg_22;
                    }
                }
            
            Result result_peg_21 = result_peg_20;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_21.getValues());
                    result_peg_20.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_20;
        stream.update(result_peg_20.getPosition());
        
        
        return result_peg_20;
        out_peg_22:
        Result result_peg_23(myposition);
        
        {
        
            result_peg_23.setValue(Value((void*) "hv"));
                for (int i = 0; i < 2; i++){
                    if (compareCharCase("hv"[i], stream.get(result_peg_23.getPosition()))){
                        result_peg_23.nextPosition();
                    } else {
                        goto out_peg_25;
                    }
                }
            
            Result result_peg_24 = result_peg_23;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_24.getValues());
                    result_peg_23.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_23;
        stream.update(result_peg_23.getPosition());
        
        
        return result_peg_23;
        out_peg_25:
        Result result_peg_26(myposition);
        
        {
        
            result_peg_26.setValue(Value((void*) "v"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("v"[i], stream.get(result_peg_26.getPosition()))){
                        result_peg_26.nextPosition();
                    } else {
                        goto out_peg_28;
                    }
                }
            
            Result result_peg_27 = result_peg_26;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_27.getValues());
                    result_peg_26.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_26;
        stream.update(result_peg_26.getPosition());
        
        
        return result_peg_26;
        out_peg_28:
        Result result_peg_29(myposition);
        
        {
        
            result_peg_29.setValue(Value((void*) "h"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_29.getPosition()))){
                        result_peg_29.nextPosition();
                    } else {
                        goto out_peg_31;
                    }
                }
            
            Result result_peg_30 = result_peg_29;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_30.getValues());
                    result_peg_29.setValue(value);
                }
            
            
        }
        
        if (column_peg_1.chunk3 == 0){
            column_peg_1.chunk3 = new Chunk3();
        }
        column_peg_1.chunk3->chunk_value = result_peg_29;
        stream.update(result_peg_29.getPosition());
        
        
        return result_peg_29;
        out_peg_31:
    
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



    
    } /* Mugen */
    
} /* Air */

        
