

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

    namespace Def{
    
    


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
    Result chunk_line_end;
    Result chunk_section;
    Result chunk_section_line;
};

struct Chunk1{
Result chunk_section_start;
    Result chunk_name;
    Result chunk_line_end_or_comment;
    Result chunk_filename;
    Result chunk_attribute;
};

struct Chunk2{
Result chunk_attribute_simple;
    Result chunk_identifier;
    Result chunk_identifier_list;
    Result chunk_valuelist;
    Result chunk_value;
};

struct Chunk3{
Result chunk_date;
    Result chunk_number;
    Result chunk_float_or_integer;
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
        return 18;
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
Result rule_line_end(Stream &, const int);
Result rule_section(Stream &, const int);
Result rule_section_line(Stream &, const int, Value section);
Result rule_section_start(Stream &, const int);
Result rule_name(Stream &, const int);
Result rule_line_end_or_comment(Stream &, const int);
Result rule_filename(Stream &, const int);
Result rule_attribute(Stream &, const int);
Result rule_attribute_simple(Stream &, const int);
Result rule_identifier(Stream &, const int);
Result rule_identifier_list(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_date(Stream &, const int);
Result rule_number(Stream &, const int);
Result rule_float_or_integer(Stream &, const int);


template<class X>
X as(const Value & value){
    return (X) value.getValue();
}

void addSection(const Value & section_list_value, const Value & section_value){
  SectionList * sections = (SectionList*) section_list_value.getValue();
  Ast::Section * section = (Ast::Section*) section_value.getValue();
  if (section == 0){
    throw ParseException("Cannot add null section");
  }
  sections->push_back(section);
}

Ast::Section * makeSection(const Value & str, int line, int column){
    Ast::Section * object = new Ast::Section(as<std::string*>(str), line, column);
    GC::save(object);
    return object;
}

SectionList * makeSectionList(){
    SectionList * object = new SectionList();
    GC::save(object);
    return object;
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

double * parseDouble(const Value & value){
    std::string * str = toString(value);
    std::istringstream get(*str);
    double * number = new double;
    get >> *number;
    GC::save(number);
    return number;
}

double * parseDouble(const Value & left, const Value & right){
    std::string * str1 = toString(left);
    std::string * str2 = toString(right);
    std::istringstream get(*str1 + "." + *str2);
    double * number = new double;
    get >> *number;
    GC::save(number);
    return number;
}

std::string * toString(char front, const Value & input){
  std::string * str = toString(input);
  str->insert(str->begin(), front);
  return str;
}

Ast::ValueAttribute * makeValueAttribute(const Value & attribute){
    /* FIXME: fix line numbers here */
    Ast::ValueAttribute * value = new Ast::ValueAttribute(-1, -1, as<Ast::Attribute*>(attribute));
    GC::save(value);
    return value;
}

Ast::Attribute * makeAttribute(const Value & line, const Value & id, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(getCurrentLine(line), getCurrentColumn(line), as<Ast::Identifier*>(id), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttribute(const Value & id, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(as<Ast::Identifier*>(id), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributeWithInfo(const Value & line, const Value & id){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(getCurrentLine(line), getCurrentColumn(line), as<Ast::Identifier*>(id));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttribute(const Value & id){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(as<Ast::Identifier*>(id));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributeFilename(const Value & id, const Value & data){
    /* TODO */
    throw ParseException("makeAttributeFilename not implemented");
    // return makeAttribute(id, data);
}

Ast::Attribute * makeIndexedAttribute(const Value & id, const Value & index, const Value & data){
    /* TODO: fix this */
    throw ParseException("makeIndexedAttribute not implemented");
    /*
    Ast::Attribute * object = new Ast::Attribute(Ast::Attribute::None);
    GC::save(object);
    return object;
    */
}

Ast::Keyword * makeKeyword(const Value & value){
    /* FIXME: fix line numbers here */
    Ast::Keyword * object = new Ast::Keyword(-1, -1, as<char*>(value));
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttributes(const Value & id, const Value & data){
    /* TODO */
    throw ParseException("makeAttributes not implemented");
    /*
    Ast::Attribute * object = new Ast::Attribute(Ast::Attribute::None);
    GC::save(object);
    return object;
    */
}

/* TODO */
Ast::Value * makeValue(){
    /* FIXME: fix line numbers here */
    Ast::Number * object = new Ast::Number(-1, -1, 0);
    GC::save(object);
    return object;
}

Ast::Value * makeValueList(const Value & line, const Value & front, const Value & rest){
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

    Ast::ValueList * object = new Ast::ValueList(getCurrentLine(line), getCurrentColumn(line), values);
    GC::save(object);
    return object;
}

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string> ids;
    ids.push_back(*as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        ids.push_back(*as<std::string*>(Value((*it).getValue())));
    }
    Ast::Identifier * object = new Ast::Identifier(ids);
    GC::save(object);
    return object;
}

Ast::Value * makeNumber(const Value & sign, const Value & number){
    double value = *(as<double*>(number));
    if (sign.getValue() != 0 && strcmp(as<const char *>(sign), "-") == 0){
        value = -value;
    }

    /* FIXME: fix line numbers here */
    Ast::Number * object = new Ast::Number(-1, -1, value);
    GC::save(object);
    return object;
}

Ast::String * makeString(const Value & value){
    /* FIXME: fix line numbers here */
    Ast::String * object = new Ast::String(-1, -1, toString(value));
    GC::save(object);
    return object;
}

Ast::Section * asSection(const Value & value){
    return as<Ast::Section*>(value);
}

Ast::Attribute * asAttribute(const Value & value){
    return as<Ast::Attribute*>(value);
}

Ast::Value * asValue(const Value & value){
    return as<Ast::Value*>(value);
}

/* FIXME */
Ast::Value * makeDate(const Value & month, const Value & day, const Value & year){
    /* FIXME: fix line numbers here */
    Ast::Number * object = new Ast::Number(-1, -1, 0);
    GC::save(object);
    return object;
}




Result rule_start(Stream & stream, const int position){
    
    
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
            
            
            
            {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_9(result_peg_3.getPosition());
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
                            
                            {
                                int position_peg_17 = result_peg_9.getPosition();
                                {
                                
                                    result_peg_9.setValue(Value((void*) ";"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_9.getPosition()))){
                                                result_peg_9.nextPosition();
                                            } else {
                                                result_peg_9.setPosition(position_peg_17);
                                                goto out_peg_19;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_9.reset();
                                        do{
                                            Result result_peg_21(result_peg_9.getPosition());
                                            {
                                            
                                                Result result_peg_24(result_peg_21);
                                                    result_peg_24.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_24.getPosition()))){
                                                            result_peg_24.nextPosition();
                                                        } else {
                                                            goto not_peg_23;
                                                        }
                                                    }
                                                    goto loop_peg_20;
                                                    not_peg_23:
                                                    result_peg_21.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_25 = stream.get(result_peg_21.getPosition());
                                                    if (temp_peg_25 != '\0'){
                                                        result_peg_21.setValue(Value((void*) (intptr_t) temp_peg_25));
                                                        result_peg_21.nextPosition();
                                                    } else {
                                                        goto loop_peg_20;
                                                    }
                                                
                                                
                                            }
                                            result_peg_9.addResult(result_peg_21);
                                        } while (true);
                                        loop_peg_20:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_16;
                            out_peg_19:
                            {
                                int position_peg_26 = result_peg_9.getPosition();
                                {
                                
                                    result_peg_9.setValue(Value((void*) "="));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_9.getPosition()))){
                                                result_peg_9.nextPosition();
                                            } else {
                                                result_peg_9.setPosition(position_peg_26);
                                                goto out_peg_28;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_9.reset();
                                        do{
                                            Result result_peg_30(result_peg_9.getPosition());
                                            {
                                            
                                                Result result_peg_33(result_peg_30);
                                                    result_peg_33.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_33.getPosition()))){
                                                            result_peg_33.nextPosition();
                                                        } else {
                                                            goto not_peg_32;
                                                        }
                                                    }
                                                    goto loop_peg_29;
                                                    not_peg_32:
                                                    result_peg_30.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_34 = stream.get(result_peg_30.getPosition());
                                                    if (temp_peg_34 != '\0'){
                                                        result_peg_30.setValue(Value((void*) (intptr_t) temp_peg_34));
                                                        result_peg_30.nextPosition();
                                                    } else {
                                                        goto loop_peg_29;
                                                    }
                                                
                                                
                                            }
                                            result_peg_9.addResult(result_peg_30);
                                        } while (true);
                                        loop_peg_29:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_16;
                            out_peg_28:
                            {
                                int position_peg_35 = result_peg_9.getPosition();
                                {
                                
                                    result_peg_9.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_9.getPosition()))){
                                                result_peg_9.nextPosition();
                                            } else {
                                                result_peg_9.setPosition(position_peg_35);
                                                goto out_peg_37;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_9.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_9.getPosition()))){
                                                result_peg_9.nextPosition();
                                            } else {
                                                result_peg_9.setPosition(position_peg_35);
                                                goto out_peg_37;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_9.reset();
                                        do{
                                            Result result_peg_40(result_peg_9.getPosition());
                                            {
                                            
                                                Result result_peg_43(result_peg_40);
                                                    {
                                                        int position_peg_45 = result_peg_43.getPosition();
                                                        
                                                        result_peg_43.setValue(Value((void*) "\n"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("\n"[i], stream.get(result_peg_43.getPosition()))){
                                                                result_peg_43.nextPosition();
                                                            } else {
                                                                result_peg_43.setPosition(position_peg_45);
                                                                goto out_peg_46;
                                                            }
                                                        }
                                                            
                                                    }
                                                    goto success_peg_44;
                                                    out_peg_46:
                                                    {
                                                        int position_peg_47 = result_peg_43.getPosition();
                                                        
                                                        result_peg_43.setValue(Value((void*) "\r"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("\r"[i], stream.get(result_peg_43.getPosition()))){
                                                                result_peg_43.nextPosition();
                                                            } else {
                                                                result_peg_43.setPosition(position_peg_47);
                                                                goto out_peg_48;
                                                            }
                                                        }
                                                            
                                                    }
                                                    goto success_peg_44;
                                                    out_peg_48:
                                                    goto not_peg_42;
                                                    success_peg_44:
                                                    ;
                                                    goto loop_peg_39;
                                                    not_peg_42:
                                                    result_peg_40.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_49 = stream.get(result_peg_40.getPosition());
                                                    if (temp_peg_49 != '\0'){
                                                        result_peg_40.setValue(Value((void*) (intptr_t) temp_peg_49));
                                                        result_peg_40.nextPosition();
                                                    } else {
                                                        goto loop_peg_39;
                                                    }
                                                
                                                
                                            }
                                            result_peg_9.addResult(result_peg_40);
                                        } while (true);
                                        loop_peg_39:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_16;
                            out_peg_37:
                            result_peg_9.setPosition(position_peg_15);
                            goto out_peg_50;
                            success_peg_16:
                            ;
                            
                        }
                        goto success_peg_10;
                        out_peg_50:
                        goto loop_peg_8;
                        success_peg_10:
                        ;
                        result_peg_3.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_51;
                success_peg_6:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_54(result_peg_3.getPosition());
                    {
                        int position_peg_56 = result_peg_54.getPosition();
                        
                        result_peg_54.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_54.getPosition()))){
                                result_peg_54.nextPosition();
                            } else {
                                result_peg_54.setPosition(position_peg_56);
                                goto out_peg_57;
                            }
                        }
                            
                    }
                    goto success_peg_55;
                    out_peg_57:
                    {
                        int position_peg_58 = result_peg_54.getPosition();
                        
                        result_peg_54.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_54.getPosition()))){
                                result_peg_54.nextPosition();
                            } else {
                                result_peg_54.setPosition(position_peg_58);
                                goto out_peg_59;
                            }
                        }
                            
                    }
                    goto success_peg_55;
                    out_peg_59:
                    goto loop_peg_53;
                    success_peg_55:
                    ;
                    result_peg_3.addResult(result_peg_54);
                } while (true);
                loop_peg_53:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_62(result_peg_3.getPosition());
                    {
                    
                        result_peg_62 = rule_line(stream, result_peg_62.getPosition(), current);
                            if (result_peg_62.error()){
                                goto loop_peg_61;
                            }
                        
                        
                        
                        {
                                int position_peg_66 = result_peg_62.getPosition();
                                
                                result_peg_62.reset();
                                do{
                                    Result result_peg_68(result_peg_62.getPosition());
                                    {
                                        int position_peg_70 = result_peg_68.getPosition();
                                        
                                        result_peg_68.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_68.getPosition()))){
                                                result_peg_68.nextPosition();
                                            } else {
                                                result_peg_68.setPosition(position_peg_70);
                                                goto out_peg_71;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_69;
                                    out_peg_71:
                                    {
                                        int position_peg_72 = result_peg_68.getPosition();
                                        
                                        result_peg_68.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_68.getPosition()))){
                                                result_peg_68.nextPosition();
                                            } else {
                                                result_peg_68.setPosition(position_peg_72);
                                                goto out_peg_73;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_69;
                                    out_peg_73:
                                    {
                                        int position_peg_74 = result_peg_68.getPosition();
                                        
                                        {
                                            int position_peg_76 = result_peg_68.getPosition();
                                            {
                                            
                                                result_peg_68.setValue(Value((void*) ";"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_68.getPosition()))){
                                                            result_peg_68.nextPosition();
                                                        } else {
                                                            result_peg_68.setPosition(position_peg_76);
                                                            goto out_peg_78;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_68.reset();
                                                    do{
                                                        Result result_peg_80(result_peg_68.getPosition());
                                                        {
                                                        
                                                            Result result_peg_83(result_peg_80);
                                                                result_peg_83.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_83.getPosition()))){
                                                                        result_peg_83.nextPosition();
                                                                    } else {
                                                                        goto not_peg_82;
                                                                    }
                                                                }
                                                                goto loop_peg_79;
                                                                not_peg_82:
                                                                result_peg_80.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_84 = stream.get(result_peg_80.getPosition());
                                                                if (temp_peg_84 != '\0'){
                                                                    result_peg_80.setValue(Value((void*) (intptr_t) temp_peg_84));
                                                                    result_peg_80.nextPosition();
                                                                } else {
                                                                    goto loop_peg_79;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_68.addResult(result_peg_80);
                                                    } while (true);
                                                    loop_peg_79:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_75;
                                        out_peg_78:
                                        {
                                            int position_peg_85 = result_peg_68.getPosition();
                                            {
                                            
                                                result_peg_68.setValue(Value((void*) "="));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_68.getPosition()))){
                                                            result_peg_68.nextPosition();
                                                        } else {
                                                            result_peg_68.setPosition(position_peg_85);
                                                            goto out_peg_87;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_68.reset();
                                                    do{
                                                        Result result_peg_89(result_peg_68.getPosition());
                                                        {
                                                        
                                                            Result result_peg_92(result_peg_89);
                                                                result_peg_92.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_92.getPosition()))){
                                                                        result_peg_92.nextPosition();
                                                                    } else {
                                                                        goto not_peg_91;
                                                                    }
                                                                }
                                                                goto loop_peg_88;
                                                                not_peg_91:
                                                                result_peg_89.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_93 = stream.get(result_peg_89.getPosition());
                                                                if (temp_peg_93 != '\0'){
                                                                    result_peg_89.setValue(Value((void*) (intptr_t) temp_peg_93));
                                                                    result_peg_89.nextPosition();
                                                                } else {
                                                                    goto loop_peg_88;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_68.addResult(result_peg_89);
                                                    } while (true);
                                                    loop_peg_88:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_75;
                                        out_peg_87:
                                        {
                                            int position_peg_94 = result_peg_68.getPosition();
                                            {
                                            
                                                result_peg_68.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_68.getPosition()))){
                                                            result_peg_68.nextPosition();
                                                        } else {
                                                            result_peg_68.setPosition(position_peg_94);
                                                            goto out_peg_96;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_68.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_68.getPosition()))){
                                                            result_peg_68.nextPosition();
                                                        } else {
                                                            result_peg_68.setPosition(position_peg_94);
                                                            goto out_peg_96;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_68.reset();
                                                    do{
                                                        Result result_peg_99(result_peg_68.getPosition());
                                                        {
                                                        
                                                            Result result_peg_102(result_peg_99);
                                                                {
                                                                    int position_peg_104 = result_peg_102.getPosition();
                                                                    
                                                                    result_peg_102.setValue(Value((void*) "\n"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\n"[i], stream.get(result_peg_102.getPosition()))){
                                                                            result_peg_102.nextPosition();
                                                                        } else {
                                                                            result_peg_102.setPosition(position_peg_104);
                                                                            goto out_peg_105;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_103;
                                                                out_peg_105:
                                                                {
                                                                    int position_peg_106 = result_peg_102.getPosition();
                                                                    
                                                                    result_peg_102.setValue(Value((void*) "\r"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\r"[i], stream.get(result_peg_102.getPosition()))){
                                                                            result_peg_102.nextPosition();
                                                                        } else {
                                                                            result_peg_102.setPosition(position_peg_106);
                                                                            goto out_peg_107;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_103;
                                                                out_peg_107:
                                                                goto not_peg_101;
                                                                success_peg_103:
                                                                ;
                                                                goto loop_peg_98;
                                                                not_peg_101:
                                                                result_peg_99.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_108 = stream.get(result_peg_99.getPosition());
                                                                if (temp_peg_108 != '\0'){
                                                                    result_peg_99.setValue(Value((void*) (intptr_t) temp_peg_108));
                                                                    result_peg_99.nextPosition();
                                                                } else {
                                                                    goto loop_peg_98;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_68.addResult(result_peg_99);
                                                    } while (true);
                                                    loop_peg_98:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_75;
                                        out_peg_96:
                                        result_peg_68.setPosition(position_peg_74);
                                        goto out_peg_109;
                                        success_peg_75:
                                        ;
                                        
                                    }
                                    goto success_peg_69;
                                    out_peg_109:
                                    goto loop_peg_67;
                                    success_peg_69:
                                    ;
                                    result_peg_62.addResult(result_peg_68);
                                } while (true);
                                loop_peg_67:
                                ;
                                        
                            }
                            goto success_peg_65;
                            goto loop_peg_61;
                            success_peg_65:
                            ;
                        
                        
                        
                        int save_peg_110 = result_peg_62.getPosition();
                            
                            result_peg_62 = rule_line_end(stream, result_peg_62.getPosition());
                            if (result_peg_62.error()){
                                
                                result_peg_62 = Result(save_peg_110);
                                result_peg_62.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_62);
                } while (true);
                loop_peg_61:
                ;
            
            
            
            if ('\0' == stream.get(result_peg_3.getPosition())){
                    result_peg_3.nextPosition();
                    result_peg_3.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_51;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = current; GC::cleanup(as<SectionList*>(value));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_51:
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
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_14;
                success_peg_5:
                ;
            
            
            
            {
                    int position_peg_16 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_16);
                                    goto out_peg_18;
                                }
                            }
                        
                        
                        
                        result_peg_3.reset();
                            do{
                                Result result_peg_20(result_peg_3.getPosition());
                                {
                                
                                    Result result_peg_23(result_peg_20);
                                        result_peg_23.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_23.getPosition()))){
                                                result_peg_23.nextPosition();
                                            } else {
                                                goto not_peg_22;
                                            }
                                        }
                                        goto loop_peg_19;
                                        not_peg_22:
                                        result_peg_20.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_24 = stream.get(result_peg_20.getPosition());
                                        if (temp_peg_24 != '\0'){
                                            result_peg_20.setValue(Value((void*) (intptr_t) temp_peg_24));
                                            result_peg_20.nextPosition();
                                        } else {
                                            goto loop_peg_19;
                                        }
                                    
                                    
                                }
                                result_peg_3.addResult(result_peg_20);
                            } while (true);
                            loop_peg_19:
                            ;
                        
                        
                    }
                }
                goto success_peg_15;
                out_peg_18:
                {
                    int position_peg_25 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_25);
                                    goto out_peg_27;
                                }
                            }
                        
                        
                        
                        result_peg_3.reset();
                            do{
                                Result result_peg_29(result_peg_3.getPosition());
                                {
                                
                                    Result result_peg_32(result_peg_29);
                                        result_peg_32.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_32.getPosition()))){
                                                result_peg_32.nextPosition();
                                            } else {
                                                goto not_peg_31;
                                            }
                                        }
                                        goto loop_peg_28;
                                        not_peg_31:
                                        result_peg_29.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_33 = stream.get(result_peg_29.getPosition());
                                        if (temp_peg_33 != '\0'){
                                            result_peg_29.setValue(Value((void*) (intptr_t) temp_peg_33));
                                            result_peg_29.nextPosition();
                                        } else {
                                            goto loop_peg_28;
                                        }
                                    
                                    
                                }
                                result_peg_3.addResult(result_peg_29);
                            } while (true);
                            loop_peg_28:
                            ;
                        
                        
                    }
                }
                goto success_peg_15;
                out_peg_27:
                {
                    int position_peg_34 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_34);
                                    goto out_peg_36;
                                }
                            }
                        
                        
                        
                        result_peg_3.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_34);
                                    goto out_peg_36;
                                }
                            }
                        
                        
                        
                        result_peg_3.reset();
                            do{
                                Result result_peg_39(result_peg_3.getPosition());
                                {
                                
                                    Result result_peg_42(result_peg_39);
                                        {
                                            int position_peg_44 = result_peg_42.getPosition();
                                            
                                            result_peg_42.setValue(Value((void*) "\n"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
                                                    result_peg_42.nextPosition();
                                                } else {
                                                    result_peg_42.setPosition(position_peg_44);
                                                    goto out_peg_45;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_43;
                                        out_peg_45:
                                        {
                                            int position_peg_46 = result_peg_42.getPosition();
                                            
                                            result_peg_42.setValue(Value((void*) "\r"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_42.getPosition()))){
                                                    result_peg_42.nextPosition();
                                                } else {
                                                    result_peg_42.setPosition(position_peg_46);
                                                    goto out_peg_47;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_43;
                                        out_peg_47:
                                        goto not_peg_41;
                                        success_peg_43:
                                        ;
                                        goto loop_peg_38;
                                        not_peg_41:
                                        result_peg_39.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_48 = stream.get(result_peg_39.getPosition());
                                        if (temp_peg_48 != '\0'){
                                            result_peg_39.setValue(Value((void*) (intptr_t) temp_peg_48));
                                            result_peg_39.nextPosition();
                                        } else {
                                            goto loop_peg_38;
                                        }
                                    
                                    
                                }
                                result_peg_3.addResult(result_peg_39);
                            } while (true);
                            loop_peg_38:
                            ;
                        
                        
                    }
                }
                goto success_peg_15;
                out_peg_36:
                goto out_peg_14;
                success_peg_15:
                ;
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_14:
        Result result_peg_49(myposition);
        
        
        {
        
            {
                    int position_peg_52 = result_peg_49.getPosition();
                    
                    result_peg_49.reset();
                    do{
                        Result result_peg_54(result_peg_49.getPosition());
                        {
                            int position_peg_56 = result_peg_54.getPosition();
                            
                            result_peg_54.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_54.getPosition()))){
                                    result_peg_54.nextPosition();
                                } else {
                                    result_peg_54.setPosition(position_peg_56);
                                    goto out_peg_57;
                                }
                            }
                                
                        }
                        goto success_peg_55;
                        out_peg_57:
                        {
                            int position_peg_58 = result_peg_54.getPosition();
                            
                            result_peg_54.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_54.getPosition()))){
                                    result_peg_54.nextPosition();
                                } else {
                                    result_peg_54.setPosition(position_peg_58);
                                    goto out_peg_59;
                                }
                            }
                                
                        }
                        goto success_peg_55;
                        out_peg_59:
                        goto loop_peg_53;
                        success_peg_55:
                        ;
                        result_peg_49.addResult(result_peg_54);
                    } while (true);
                    loop_peg_53:
                    ;
                            
                }
                goto success_peg_51;
                goto out_peg_60;
                success_peg_51:
                ;
            
            
            
            result_peg_49 = rule_section(stream, result_peg_49.getPosition());
                if (result_peg_49.error()){
                    goto out_peg_60;
                }
            
            Result result_peg_61 = result_peg_49;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_61.getValues());
                    result_peg_49.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_49.getPosition());
        
        return result_peg_49;
        out_peg_60:
        Result result_peg_62(myposition);
        
        
        result_peg_62.reset();
        do{
            Result result_peg_64(result_peg_62.getPosition());
            {
                int position_peg_66 = result_peg_64.getPosition();
                
                result_peg_64.setValue(Value((void*) " "));
                for (int i = 0; i < 1; i++){
                    if (compareChar(" "[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        result_peg_64.setPosition(position_peg_66);
                        goto out_peg_67;
                    }
                }
                    
            }
            goto success_peg_65;
            out_peg_67:
            {
                int position_peg_68 = result_peg_64.getPosition();
                
                result_peg_64.setValue(Value((void*) "\t"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\t"[i], stream.get(result_peg_64.getPosition()))){
                        result_peg_64.nextPosition();
                    } else {
                        result_peg_64.setPosition(position_peg_68);
                        goto out_peg_69;
                    }
                }
                    
            }
            goto success_peg_65;
            out_peg_69:
            goto loop_peg_63;
            success_peg_65:
            ;
            result_peg_62.addResult(result_peg_64);
        } while (true);
        loop_peg_63:
        if (result_peg_62.matches() == 0){
            goto out_peg_70;
        }
        
        stream.update(result_peg_62.getPosition());
        
        return result_peg_62;
        out_peg_70:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    
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
        
        stream.update(result_peg_12.getPosition());
        
        return result_peg_12;
        out_peg_15:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "section");
    int myposition = position;
    
    
    Value line;
        Value name;
        Value ast;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            result_peg_3 = rule_section_start(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
                name = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSection(name, getCurrentLine(line), getCurrentColumn(line));
                    result_peg_3.setValue(value);
                }
                ast = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_11 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_13(result_peg_3.getPosition());
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
                            
                            {
                                int position_peg_21 = result_peg_13.getPosition();
                                {
                                
                                    result_peg_13.setValue(Value((void*) ";"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_21);
                                                goto out_peg_23;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_13.reset();
                                        do{
                                            Result result_peg_25(result_peg_13.getPosition());
                                            {
                                            
                                                Result result_peg_28(result_peg_25);
                                                    result_peg_28.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_28.getPosition()))){
                                                            result_peg_28.nextPosition();
                                                        } else {
                                                            goto not_peg_27;
                                                        }
                                                    }
                                                    goto loop_peg_24;
                                                    not_peg_27:
                                                    result_peg_25.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_29 = stream.get(result_peg_25.getPosition());
                                                    if (temp_peg_29 != '\0'){
                                                        result_peg_25.setValue(Value((void*) (intptr_t) temp_peg_29));
                                                        result_peg_25.nextPosition();
                                                    } else {
                                                        goto loop_peg_24;
                                                    }
                                                
                                                
                                            }
                                            result_peg_13.addResult(result_peg_25);
                                        } while (true);
                                        loop_peg_24:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_20;
                            out_peg_23:
                            {
                                int position_peg_30 = result_peg_13.getPosition();
                                {
                                
                                    result_peg_13.setValue(Value((void*) "="));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_30);
                                                goto out_peg_32;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_13.reset();
                                        do{
                                            Result result_peg_34(result_peg_13.getPosition());
                                            {
                                            
                                                Result result_peg_37(result_peg_34);
                                                    result_peg_37.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_37.getPosition()))){
                                                            result_peg_37.nextPosition();
                                                        } else {
                                                            goto not_peg_36;
                                                        }
                                                    }
                                                    goto loop_peg_33;
                                                    not_peg_36:
                                                    result_peg_34.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_38 = stream.get(result_peg_34.getPosition());
                                                    if (temp_peg_38 != '\0'){
                                                        result_peg_34.setValue(Value((void*) (intptr_t) temp_peg_38));
                                                        result_peg_34.nextPosition();
                                                    } else {
                                                        goto loop_peg_33;
                                                    }
                                                
                                                
                                            }
                                            result_peg_13.addResult(result_peg_34);
                                        } while (true);
                                        loop_peg_33:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_20;
                            out_peg_32:
                            {
                                int position_peg_39 = result_peg_13.getPosition();
                                {
                                
                                    result_peg_13.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_39);
                                                goto out_peg_41;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_13.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_13.getPosition()))){
                                                result_peg_13.nextPosition();
                                            } else {
                                                result_peg_13.setPosition(position_peg_39);
                                                goto out_peg_41;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_13.reset();
                                        do{
                                            Result result_peg_44(result_peg_13.getPosition());
                                            {
                                            
                                                Result result_peg_47(result_peg_44);
                                                    {
                                                        int position_peg_49 = result_peg_47.getPosition();
                                                        
                                                        result_peg_47.setValue(Value((void*) "\n"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("\n"[i], stream.get(result_peg_47.getPosition()))){
                                                                result_peg_47.nextPosition();
                                                            } else {
                                                                result_peg_47.setPosition(position_peg_49);
                                                                goto out_peg_50;
                                                            }
                                                        }
                                                            
                                                    }
                                                    goto success_peg_48;
                                                    out_peg_50:
                                                    {
                                                        int position_peg_51 = result_peg_47.getPosition();
                                                        
                                                        result_peg_47.setValue(Value((void*) "\r"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("\r"[i], stream.get(result_peg_47.getPosition()))){
                                                                result_peg_47.nextPosition();
                                                            } else {
                                                                result_peg_47.setPosition(position_peg_51);
                                                                goto out_peg_52;
                                                            }
                                                        }
                                                            
                                                    }
                                                    goto success_peg_48;
                                                    out_peg_52:
                                                    goto not_peg_46;
                                                    success_peg_48:
                                                    ;
                                                    goto loop_peg_43;
                                                    not_peg_46:
                                                    result_peg_44.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_53 = stream.get(result_peg_44.getPosition());
                                                    if (temp_peg_53 != '\0'){
                                                        result_peg_44.setValue(Value((void*) (intptr_t) temp_peg_53));
                                                        result_peg_44.nextPosition();
                                                    } else {
                                                        goto loop_peg_43;
                                                    }
                                                
                                                
                                            }
                                            result_peg_13.addResult(result_peg_44);
                                        } while (true);
                                        loop_peg_43:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_20;
                            out_peg_41:
                            result_peg_13.setPosition(position_peg_19);
                            goto out_peg_54;
                            success_peg_20:
                            ;
                            
                        }
                        goto success_peg_14;
                        out_peg_54:
                        goto loop_peg_12;
                        success_peg_14:
                        ;
                        result_peg_3.addResult(result_peg_13);
                    } while (true);
                    loop_peg_12:
                    ;
                            
                }
                goto success_peg_10;
                goto out_peg_7;
                success_peg_10:
                ;
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_57(result_peg_3.getPosition());
                    {
                        int position_peg_59 = result_peg_57.getPosition();
                        
                        result_peg_57.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_57.getPosition()))){
                                result_peg_57.nextPosition();
                            } else {
                                result_peg_57.setPosition(position_peg_59);
                                goto out_peg_60;
                            }
                        }
                            
                    }
                    goto success_peg_58;
                    out_peg_60:
                    {
                        int position_peg_61 = result_peg_57.getPosition();
                        
                        result_peg_57.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_57.getPosition()))){
                                result_peg_57.nextPosition();
                            } else {
                                result_peg_57.setPosition(position_peg_61);
                                goto out_peg_62;
                            }
                        }
                            
                    }
                    goto success_peg_58;
                    out_peg_62:
                    goto loop_peg_56;
                    success_peg_58:
                    ;
                    result_peg_3.addResult(result_peg_57);
                } while (true);
                loop_peg_56:
                if (result_peg_3.matches() == 0){
                    goto out_peg_7;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_65(result_peg_3.getPosition());
                    {
                    
                        result_peg_65 = rule_section_line(stream, result_peg_65.getPosition(), ast);
                            if (result_peg_65.error()){
                                goto loop_peg_64;
                            }
                        
                        
                        
                        {
                                int position_peg_69 = result_peg_65.getPosition();
                                
                                result_peg_65.reset();
                                do{
                                    Result result_peg_71(result_peg_65.getPosition());
                                    {
                                        int position_peg_73 = result_peg_71.getPosition();
                                        
                                        result_peg_71.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_71.getPosition()))){
                                                result_peg_71.nextPosition();
                                            } else {
                                                result_peg_71.setPosition(position_peg_73);
                                                goto out_peg_74;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_72;
                                    out_peg_74:
                                    {
                                        int position_peg_75 = result_peg_71.getPosition();
                                        
                                        result_peg_71.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_71.getPosition()))){
                                                result_peg_71.nextPosition();
                                            } else {
                                                result_peg_71.setPosition(position_peg_75);
                                                goto out_peg_76;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_72;
                                    out_peg_76:
                                    {
                                        int position_peg_77 = result_peg_71.getPosition();
                                        
                                        {
                                            int position_peg_79 = result_peg_71.getPosition();
                                            {
                                            
                                                result_peg_71.setValue(Value((void*) ";"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_71.getPosition()))){
                                                            result_peg_71.nextPosition();
                                                        } else {
                                                            result_peg_71.setPosition(position_peg_79);
                                                            goto out_peg_81;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_71.reset();
                                                    do{
                                                        Result result_peg_83(result_peg_71.getPosition());
                                                        {
                                                        
                                                            Result result_peg_86(result_peg_83);
                                                                result_peg_86.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_86.getPosition()))){
                                                                        result_peg_86.nextPosition();
                                                                    } else {
                                                                        goto not_peg_85;
                                                                    }
                                                                }
                                                                goto loop_peg_82;
                                                                not_peg_85:
                                                                result_peg_83.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_87 = stream.get(result_peg_83.getPosition());
                                                                if (temp_peg_87 != '\0'){
                                                                    result_peg_83.setValue(Value((void*) (intptr_t) temp_peg_87));
                                                                    result_peg_83.nextPosition();
                                                                } else {
                                                                    goto loop_peg_82;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_71.addResult(result_peg_83);
                                                    } while (true);
                                                    loop_peg_82:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_78;
                                        out_peg_81:
                                        {
                                            int position_peg_88 = result_peg_71.getPosition();
                                            {
                                            
                                                result_peg_71.setValue(Value((void*) "="));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_71.getPosition()))){
                                                            result_peg_71.nextPosition();
                                                        } else {
                                                            result_peg_71.setPosition(position_peg_88);
                                                            goto out_peg_90;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_71.reset();
                                                    do{
                                                        Result result_peg_92(result_peg_71.getPosition());
                                                        {
                                                        
                                                            Result result_peg_95(result_peg_92);
                                                                result_peg_95.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_95.getPosition()))){
                                                                        result_peg_95.nextPosition();
                                                                    } else {
                                                                        goto not_peg_94;
                                                                    }
                                                                }
                                                                goto loop_peg_91;
                                                                not_peg_94:
                                                                result_peg_92.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_96 = stream.get(result_peg_92.getPosition());
                                                                if (temp_peg_96 != '\0'){
                                                                    result_peg_92.setValue(Value((void*) (intptr_t) temp_peg_96));
                                                                    result_peg_92.nextPosition();
                                                                } else {
                                                                    goto loop_peg_91;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_71.addResult(result_peg_92);
                                                    } while (true);
                                                    loop_peg_91:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_78;
                                        out_peg_90:
                                        {
                                            int position_peg_97 = result_peg_71.getPosition();
                                            {
                                            
                                                result_peg_71.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_71.getPosition()))){
                                                            result_peg_71.nextPosition();
                                                        } else {
                                                            result_peg_71.setPosition(position_peg_97);
                                                            goto out_peg_99;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_71.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_71.getPosition()))){
                                                            result_peg_71.nextPosition();
                                                        } else {
                                                            result_peg_71.setPosition(position_peg_97);
                                                            goto out_peg_99;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_71.reset();
                                                    do{
                                                        Result result_peg_102(result_peg_71.getPosition());
                                                        {
                                                        
                                                            Result result_peg_105(result_peg_102);
                                                                {
                                                                    int position_peg_107 = result_peg_105.getPosition();
                                                                    
                                                                    result_peg_105.setValue(Value((void*) "\n"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\n"[i], stream.get(result_peg_105.getPosition()))){
                                                                            result_peg_105.nextPosition();
                                                                        } else {
                                                                            result_peg_105.setPosition(position_peg_107);
                                                                            goto out_peg_108;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_106;
                                                                out_peg_108:
                                                                {
                                                                    int position_peg_109 = result_peg_105.getPosition();
                                                                    
                                                                    result_peg_105.setValue(Value((void*) "\r"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\r"[i], stream.get(result_peg_105.getPosition()))){
                                                                            result_peg_105.nextPosition();
                                                                        } else {
                                                                            result_peg_105.setPosition(position_peg_109);
                                                                            goto out_peg_110;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_106;
                                                                out_peg_110:
                                                                goto not_peg_104;
                                                                success_peg_106:
                                                                ;
                                                                goto loop_peg_101;
                                                                not_peg_104:
                                                                result_peg_102.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_111 = stream.get(result_peg_102.getPosition());
                                                                if (temp_peg_111 != '\0'){
                                                                    result_peg_102.setValue(Value((void*) (intptr_t) temp_peg_111));
                                                                    result_peg_102.nextPosition();
                                                                } else {
                                                                    goto loop_peg_101;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_71.addResult(result_peg_102);
                                                    } while (true);
                                                    loop_peg_101:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_78;
                                        out_peg_99:
                                        result_peg_71.setPosition(position_peg_77);
                                        goto out_peg_112;
                                        success_peg_78:
                                        ;
                                        
                                    }
                                    goto success_peg_72;
                                    out_peg_112:
                                    goto loop_peg_70;
                                    success_peg_72:
                                    ;
                                    result_peg_65.addResult(result_peg_71);
                                } while (true);
                                loop_peg_70:
                                ;
                                        
                            }
                            goto success_peg_68;
                            goto loop_peg_64;
                            success_peg_68:
                            ;
                        
                        
                        
                        result_peg_65 = rule_line_end(stream, result_peg_65.getPosition());
                            if (result_peg_65.error()){
                                goto loop_peg_64;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_65);
                } while (true);
                loop_peg_64:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_7:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    
    RuleTrace trace_peg_1(stream, "section_line");
    int myposition = position;
    
    
    Value data;
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
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_3.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_14;
                success_peg_5:
                ;
            
            
            
            {
                    int position_peg_16 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_16);
                                    goto out_peg_18;
                                }
                            }
                        
                        
                        
                        result_peg_3.reset();
                            do{
                                Result result_peg_20(result_peg_3.getPosition());
                                {
                                
                                    Result result_peg_23(result_peg_20);
                                        result_peg_23.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_23.getPosition()))){
                                                result_peg_23.nextPosition();
                                            } else {
                                                goto not_peg_22;
                                            }
                                        }
                                        goto loop_peg_19;
                                        not_peg_22:
                                        result_peg_20.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_24 = stream.get(result_peg_20.getPosition());
                                        if (temp_peg_24 != '\0'){
                                            result_peg_20.setValue(Value((void*) (intptr_t) temp_peg_24));
                                            result_peg_20.nextPosition();
                                        } else {
                                            goto loop_peg_19;
                                        }
                                    
                                    
                                }
                                result_peg_3.addResult(result_peg_20);
                            } while (true);
                            loop_peg_19:
                            ;
                        
                        
                    }
                }
                goto success_peg_15;
                out_peg_18:
                {
                    int position_peg_25 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_25);
                                    goto out_peg_27;
                                }
                            }
                        
                        
                        
                        result_peg_3.reset();
                            do{
                                Result result_peg_29(result_peg_3.getPosition());
                                {
                                
                                    Result result_peg_32(result_peg_29);
                                        result_peg_32.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_32.getPosition()))){
                                                result_peg_32.nextPosition();
                                            } else {
                                                goto not_peg_31;
                                            }
                                        }
                                        goto loop_peg_28;
                                        not_peg_31:
                                        result_peg_29.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_33 = stream.get(result_peg_29.getPosition());
                                        if (temp_peg_33 != '\0'){
                                            result_peg_29.setValue(Value((void*) (intptr_t) temp_peg_33));
                                            result_peg_29.nextPosition();
                                        } else {
                                            goto loop_peg_28;
                                        }
                                    
                                    
                                }
                                result_peg_3.addResult(result_peg_29);
                            } while (true);
                            loop_peg_28:
                            ;
                        
                        
                    }
                }
                goto success_peg_15;
                out_peg_27:
                {
                    int position_peg_34 = result_peg_3.getPosition();
                    {
                    
                        result_peg_3.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_34);
                                    goto out_peg_36;
                                }
                            }
                        
                        
                        
                        result_peg_3.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
                                    result_peg_3.nextPosition();
                                } else {
                                    result_peg_3.setPosition(position_peg_34);
                                    goto out_peg_36;
                                }
                            }
                        
                        
                        
                        result_peg_3.reset();
                            do{
                                Result result_peg_39(result_peg_3.getPosition());
                                {
                                
                                    Result result_peg_42(result_peg_39);
                                        {
                                            int position_peg_44 = result_peg_42.getPosition();
                                            
                                            result_peg_42.setValue(Value((void*) "\n"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
                                                    result_peg_42.nextPosition();
                                                } else {
                                                    result_peg_42.setPosition(position_peg_44);
                                                    goto out_peg_45;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_43;
                                        out_peg_45:
                                        {
                                            int position_peg_46 = result_peg_42.getPosition();
                                            
                                            result_peg_42.setValue(Value((void*) "\r"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_42.getPosition()))){
                                                    result_peg_42.nextPosition();
                                                } else {
                                                    result_peg_42.setPosition(position_peg_46);
                                                    goto out_peg_47;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_43;
                                        out_peg_47:
                                        goto not_peg_41;
                                        success_peg_43:
                                        ;
                                        goto loop_peg_38;
                                        not_peg_41:
                                        result_peg_39.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_48 = stream.get(result_peg_39.getPosition());
                                        if (temp_peg_48 != '\0'){
                                            result_peg_39.setValue(Value((void*) (intptr_t) temp_peg_48));
                                            result_peg_39.nextPosition();
                                        } else {
                                            goto loop_peg_38;
                                        }
                                    
                                    
                                }
                                result_peg_3.addResult(result_peg_39);
                            } while (true);
                            loop_peg_38:
                            ;
                        
                        
                    }
                }
                goto success_peg_15;
                out_peg_36:
                goto out_peg_14;
                success_peg_15:
                ;
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_14:
        Result result_peg_49(myposition);
        
        
        {
        
            {
                    int position_peg_52 = result_peg_49.getPosition();
                    
                    result_peg_49.reset();
                    do{
                        Result result_peg_54(result_peg_49.getPosition());
                        {
                            int position_peg_56 = result_peg_54.getPosition();
                            
                            result_peg_54.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_54.getPosition()))){
                                    result_peg_54.nextPosition();
                                } else {
                                    result_peg_54.setPosition(position_peg_56);
                                    goto out_peg_57;
                                }
                            }
                                
                        }
                        goto success_peg_55;
                        out_peg_57:
                        {
                            int position_peg_58 = result_peg_54.getPosition();
                            
                            result_peg_54.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_54.getPosition()))){
                                    result_peg_54.nextPosition();
                                } else {
                                    result_peg_54.setPosition(position_peg_58);
                                    goto out_peg_59;
                                }
                            }
                                
                        }
                        goto success_peg_55;
                        out_peg_59:
                        goto loop_peg_53;
                        success_peg_55:
                        ;
                        result_peg_49.addResult(result_peg_54);
                    } while (true);
                    loop_peg_53:
                    ;
                            
                }
                goto success_peg_51;
                goto out_peg_60;
                success_peg_51:
                ;
            
            
            
            result_peg_49 = rule_attribute(stream, result_peg_49.getPosition());
                if (result_peg_49.error()){
                    goto out_peg_60;
                }
                data = result_peg_49.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addAttribute(asAttribute(data));
                    result_peg_49.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_49.getPosition());
        
        return result_peg_49;
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
                        goto loop_peg_66;
                        success_peg_68:
                        ;
                        result_peg_62.addResult(result_peg_67);
                    } while (true);
                    loop_peg_66:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_73;
                success_peg_64:
                ;
            
            
            
            {
                    int position_peg_76 = result_peg_62.getPosition();
                    {
                    
                        result_peg_62.setValue(Value((void*) "loopstart"));
                            for (int i = 0; i < 9; i++){
                                if (compareCharCase("loopstart"[i], stream.get(result_peg_62.getPosition()))){
                                    result_peg_62.nextPosition();
                                } else {
                                    result_peg_62.setPosition(position_peg_76);
                                    goto out_peg_78;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = makeValue();
                                result_peg_62.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_75;
                out_peg_78:
                goto out_peg_73;
                success_peg_75:
                ;
                data = result_peg_62.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_62.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_62.getPosition());
        
        return result_peg_62;
        out_peg_73:
        Result result_peg_79(myposition);
        
        
        {
        
            {
                    int position_peg_82 = result_peg_79.getPosition();
                    
                    result_peg_79.reset();
                    do{
                        Result result_peg_84(result_peg_79.getPosition());
                        {
                            int position_peg_86 = result_peg_84.getPosition();
                            
                            result_peg_84.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    result_peg_84.setPosition(position_peg_86);
                                    goto out_peg_87;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_87:
                        {
                            int position_peg_88 = result_peg_84.getPosition();
                            
                            result_peg_84.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    result_peg_84.setPosition(position_peg_88);
                                    goto out_peg_89;
                                }
                            }
                                
                        }
                        goto success_peg_85;
                        out_peg_89:
                        goto loop_peg_83;
                        success_peg_85:
                        ;
                        result_peg_79.addResult(result_peg_84);
                    } while (true);
                    loop_peg_83:
                    ;
                            
                }
                goto success_peg_81;
                goto out_peg_90;
                success_peg_81:
                ;
            
            
            
            result_peg_79 = rule_valuelist(stream, result_peg_79.getPosition());
                if (result_peg_79.error()){
                    goto out_peg_90;
                }
                data = result_peg_79.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_79.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_79.getPosition());
        
        return result_peg_79;
        out_peg_90:
        Result result_peg_92(myposition);
        
        
        {
        
            result_peg_92.reset();
                do{
                    Result result_peg_95(result_peg_92.getPosition());
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
                    goto loop_peg_94;
                    success_peg_96:
                    ;
                    result_peg_92.addResult(result_peg_95);
                } while (true);
                loop_peg_94:
                if (result_peg_92.matches() == 0){
                    goto out_peg_101;
                }
            
            
            
            Result result_peg_103(result_peg_92);
                result_peg_103.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_103.getPosition()))){
                        result_peg_103.nextPosition();
                    } else {
                        goto not_peg_102;
                    }
                }
                goto out_peg_101;
                not_peg_102:
                result_peg_92.setValue(Value((void*)0));
            
            
        }
        
        stream.update(result_peg_92.getPosition());
        
        return result_peg_92;
        out_peg_101:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "section_start");
    int myposition = position;
    
    
    Value data;
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
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_18(result_peg_3.getPosition());
                    {
                    
                        Result result_peg_21(result_peg_18);
                            result_peg_21.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    goto not_peg_20;
                                }
                            }
                            goto loop_peg_17;
                            not_peg_20:
                            result_peg_18.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_22 = stream.get(result_peg_18.getPosition());
                            if (temp_peg_22 != '\0'){
                                result_peg_18.setValue(Value((void*) (intptr_t) temp_peg_22));
                                result_peg_18.nextPosition();
                            } else {
                                goto loop_peg_17;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_18);
                } while (true);
                loop_peg_17:
                if (result_peg_3.matches() == 0){
                    goto out_peg_5;
                }
                data = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_25 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_27(result_peg_3.getPosition());
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
                        goto loop_peg_26;
                        success_peg_28:
                        ;
                        result_peg_3.addResult(result_peg_27);
                    } while (true);
                    loop_peg_26:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_5;
                success_peg_24:
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
                    value = toString(data);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_5:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            {
                    int position_peg_6 = result_peg_3.getPosition();
                    
                    char letter_peg_7 = stream.get(result_peg_3.getPosition());
                    if (letter_peg_7 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_7) != NULL){
                        result_peg_3.nextPosition();
                        result_peg_3.setValue(Value((void*) (intptr_t) letter_peg_7));
                    } else {
                        result_peg_3.setPosition(position_peg_6);
                        goto out_peg_8;
                    }
                    
                }
                goto success_peg_5;
                out_peg_8:
                goto out_peg_9;
                success_peg_5:
                ;
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_12(result_peg_3.getPosition());
                    {
                        int position_peg_14 = result_peg_12.getPosition();
                        
                        {
                            int position_peg_16 = result_peg_12.getPosition();
                            
                            char letter_peg_17 = stream.get(result_peg_12.getPosition());
                            if (letter_peg_17 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_17) != NULL){
                                result_peg_12.nextPosition();
                                result_peg_12.setValue(Value((void*) (intptr_t) letter_peg_17));
                            } else {
                                result_peg_12.setPosition(position_peg_16);
                                goto out_peg_18;
                            }
                            
                        }
                        goto success_peg_15;
                        out_peg_18:
                        result_peg_12.setPosition(position_peg_14);
                        goto out_peg_19;
                        success_peg_15:
                        ;
                        
                    }
                    goto success_peg_13;
                    out_peg_19:
                    {
                        int position_peg_20 = result_peg_12.getPosition();
                        
                        {
                            int position_peg_22 = result_peg_12.getPosition();
                            
                            char letter_peg_23 = stream.get(result_peg_12.getPosition());
                            if (letter_peg_23 != '\0' && strchr("0123456789", letter_peg_23) != NULL){
                                result_peg_12.nextPosition();
                                result_peg_12.setValue(Value((void*) (intptr_t) letter_peg_23));
                            } else {
                                result_peg_12.setPosition(position_peg_22);
                                goto out_peg_24;
                            }
                            
                        }
                        goto success_peg_21;
                        out_peg_24:
                        result_peg_12.setPosition(position_peg_20);
                        goto out_peg_25;
                        success_peg_21:
                        ;
                        
                    }
                    goto success_peg_13;
                    out_peg_25:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_3.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                ;
            
            Result result_peg_10 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_4.getValues().getValue(),result_peg_10.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_9:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "line_end_or_comment");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        result_peg_3 = rule_line_end(stream, result_peg_3.getPosition());
        if (result_peg_3.error()){
            goto out_peg_4;
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_4:
        Result result_peg_5(myposition);
        
        
        {
            int position_peg_7 = result_peg_5.getPosition();
            {
            
                result_peg_5.setValue(Value((void*) ";"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(";"[i], stream.get(result_peg_5.getPosition()))){
                            result_peg_5.nextPosition();
                        } else {
                            result_peg_5.setPosition(position_peg_7);
                            goto out_peg_9;
                        }
                    }
                
                
                
                result_peg_5.reset();
                    do{
                        Result result_peg_11(result_peg_5.getPosition());
                        {
                        
                            Result result_peg_14(result_peg_11);
                                result_peg_14.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_14.getPosition()))){
                                        result_peg_14.nextPosition();
                                    } else {
                                        goto not_peg_13;
                                    }
                                }
                                goto loop_peg_10;
                                not_peg_13:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_15 = stream.get(result_peg_11.getPosition());
                                if (temp_peg_15 != '\0'){
                                    result_peg_11.setValue(Value((void*) (intptr_t) temp_peg_15));
                                    result_peg_11.nextPosition();
                                } else {
                                    goto loop_peg_10;
                                }
                            
                            
                        }
                        result_peg_5.addResult(result_peg_11);
                    } while (true);
                    loop_peg_10:
                    ;
                
                
            }
        }
        goto success_peg_6;
        out_peg_9:
        {
            int position_peg_16 = result_peg_5.getPosition();
            {
            
                result_peg_5.setValue(Value((void*) "="));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("="[i], stream.get(result_peg_5.getPosition()))){
                            result_peg_5.nextPosition();
                        } else {
                            result_peg_5.setPosition(position_peg_16);
                            goto out_peg_18;
                        }
                    }
                
                
                
                result_peg_5.reset();
                    do{
                        Result result_peg_20(result_peg_5.getPosition());
                        {
                        
                            Result result_peg_23(result_peg_20);
                                result_peg_23.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_23.getPosition()))){
                                        result_peg_23.nextPosition();
                                    } else {
                                        goto not_peg_22;
                                    }
                                }
                                goto loop_peg_19;
                                not_peg_22:
                                result_peg_20.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_24 = stream.get(result_peg_20.getPosition());
                                if (temp_peg_24 != '\0'){
                                    result_peg_20.setValue(Value((void*) (intptr_t) temp_peg_24));
                                    result_peg_20.nextPosition();
                                } else {
                                    goto loop_peg_19;
                                }
                            
                            
                        }
                        result_peg_5.addResult(result_peg_20);
                    } while (true);
                    loop_peg_19:
                    ;
                
                
            }
        }
        goto success_peg_6;
        out_peg_18:
        {
            int position_peg_25 = result_peg_5.getPosition();
            {
            
                result_peg_5.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_5.getPosition()))){
                            result_peg_5.nextPosition();
                        } else {
                            result_peg_5.setPosition(position_peg_25);
                            goto out_peg_27;
                        }
                    }
                
                
                
                result_peg_5.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_5.getPosition()))){
                            result_peg_5.nextPosition();
                        } else {
                            result_peg_5.setPosition(position_peg_25);
                            goto out_peg_27;
                        }
                    }
                
                
                
                result_peg_5.reset();
                    do{
                        Result result_peg_30(result_peg_5.getPosition());
                        {
                        
                            Result result_peg_33(result_peg_30);
                                {
                                    int position_peg_35 = result_peg_33.getPosition();
                                    
                                    result_peg_33.setValue(Value((void*) "\n"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_33.getPosition()))){
                                            result_peg_33.nextPosition();
                                        } else {
                                            result_peg_33.setPosition(position_peg_35);
                                            goto out_peg_36;
                                        }
                                    }
                                        
                                }
                                goto success_peg_34;
                                out_peg_36:
                                {
                                    int position_peg_37 = result_peg_33.getPosition();
                                    
                                    result_peg_33.setValue(Value((void*) "\r"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\r"[i], stream.get(result_peg_33.getPosition()))){
                                            result_peg_33.nextPosition();
                                        } else {
                                            result_peg_33.setPosition(position_peg_37);
                                            goto out_peg_38;
                                        }
                                    }
                                        
                                }
                                goto success_peg_34;
                                out_peg_38:
                                goto not_peg_32;
                                success_peg_34:
                                ;
                                goto loop_peg_29;
                                not_peg_32:
                                result_peg_30.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_39 = stream.get(result_peg_30.getPosition());
                                if (temp_peg_39 != '\0'){
                                    result_peg_30.setValue(Value((void*) (intptr_t) temp_peg_39));
                                    result_peg_30.nextPosition();
                                } else {
                                    goto loop_peg_29;
                                }
                            
                            
                        }
                        result_peg_5.addResult(result_peg_30);
                    } while (true);
                    loop_peg_29:
                    ;
                
                
            }
        }
        goto success_peg_6;
        out_peg_27:
        goto out_peg_40;
        success_peg_6:
        ;
        
        stream.update(result_peg_5.getPosition());
        
        return result_peg_5;
        out_peg_40:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "filename");
    int myposition = position;
    
    
    Value line;
        Value file;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            Result result_peg_8(result_peg_3);
                result_peg_8.setValue(Value((void*) "\""));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_8.getPosition()))){
                        result_peg_8.nextPosition();
                    } else {
                        goto not_peg_7;
                    }
                }
                goto out_peg_9;
                not_peg_7:
                result_peg_3.setValue(Value((void*)0));
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_12(result_peg_3.getPosition());
                    {
                        int position_peg_14 = result_peg_12.getPosition();
                        {
                        
                            Result result_peg_17(result_peg_12);
                                result_peg_17.setValue(Value((void*) ","));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(","[i], stream.get(result_peg_17.getPosition()))){
                                        result_peg_17.nextPosition();
                                    } else {
                                        goto not_peg_16;
                                    }
                                }
                                result_peg_12.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_16:
                                result_peg_12.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_21(result_peg_12);
                                result_peg_21.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_21.getPosition()))){
                                        result_peg_21.nextPosition();
                                    } else {
                                        goto not_peg_20;
                                    }
                                }
                                result_peg_12.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_20:
                                result_peg_12.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_24(result_peg_12);
                                result_peg_24.setValue(Value((void*) "["));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("["[i], stream.get(result_peg_24.getPosition()))){
                                        result_peg_24.nextPosition();
                                    } else {
                                        goto not_peg_23;
                                    }
                                }
                                result_peg_12.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_23:
                                result_peg_12.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_27(result_peg_12);
                                result_peg_27.setValue(Value((void*) 13));
                                if ((unsigned char) stream.get(result_peg_27.getPosition()) == (unsigned char) 13){
                                    result_peg_27.nextPosition();
                                } else {
                                    goto not_peg_26;
                                }
                                result_peg_12.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_26:
                                result_peg_12.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_30(result_peg_12);
                                result_peg_30.setValue(Value((void*) "="));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("="[i], stream.get(result_peg_30.getPosition()))){
                                        result_peg_30.nextPosition();
                                    } else {
                                        goto not_peg_29;
                                    }
                                }
                                result_peg_12.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_29:
                                result_peg_12.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_33(result_peg_12);
                                result_peg_33.setValue(Value((void*) ";"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(";"[i], stream.get(result_peg_33.getPosition()))){
                                        result_peg_33.nextPosition();
                                    } else {
                                        goto not_peg_32;
                                    }
                                }
                                result_peg_12.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_32:
                                result_peg_12.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_34 = stream.get(result_peg_12.getPosition());
                                if (temp_peg_34 != '\0'){
                                    result_peg_12.setValue(Value((void*) (intptr_t) temp_peg_34));
                                    result_peg_12.nextPosition();
                                } else {
                                    result_peg_12.setPosition(position_peg_14);
                                    goto out_peg_18;
                                }
                            
                            
                        }
                    }
                    goto success_peg_13;
                    out_peg_18:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_3.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                if (result_peg_3.matches() == 0){
                    goto out_peg_9;
                }
                file = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new Ast::Filename(getCurrentLine(line), getCurrentColumn(line), toString(file)); GC::save(as<Ast::Filename*>(value));
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_9:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "attribute");
    int myposition = position;
    
    
    Value line;
        Value id;
        Value data;
        Value index;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            result_peg_3 = rule_identifier(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
                id = result_peg_3.getValues();
            
            
            
            {
                    int position_peg_10 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_12(result_peg_3.getPosition());
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
                        goto loop_peg_11;
                        success_peg_13:
                        ;
                        result_peg_3.addResult(result_peg_12);
                    } while (true);
                    loop_peg_11:
                    ;
                            
                }
                goto success_peg_9;
                goto out_peg_7;
                success_peg_9:
                ;
            
            
            
            result_peg_3.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_7;
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
                        goto loop_peg_22;
                        success_peg_24:
                        ;
                        result_peg_3.addResult(result_peg_23);
                    } while (true);
                    loop_peg_22:
                    ;
                            
                }
                goto success_peg_20;
                goto out_peg_7;
                success_peg_20:
                ;
            
            
            
            Result result_peg_30(result_peg_3.getPosition());
                result_peg_30 = rule_line_end_or_comment(stream, result_peg_30.getPosition());
                if (result_peg_30.error()){
                    goto out_peg_7;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeWithInfo(line, id);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_7:
        Result result_peg_31(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_33 = stream.getLineInfo(result_peg_31.getPosition());
                line = &line_info_peg_33;
            
            
            
            result_peg_31 = rule_identifier(stream, result_peg_31.getPosition());
                if (result_peg_31.error()){
                    goto out_peg_35;
                }
                id = result_peg_31.getValues();
            
            
            
            {
                    int position_peg_38 = result_peg_31.getPosition();
                    
                    result_peg_31.reset();
                    do{
                        Result result_peg_40(result_peg_31.getPosition());
                        {
                            int position_peg_42 = result_peg_40.getPosition();
                            
                            result_peg_40.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    result_peg_40.setPosition(position_peg_42);
                                    goto out_peg_43;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_43:
                        {
                            int position_peg_44 = result_peg_40.getPosition();
                            
                            result_peg_40.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    result_peg_40.setPosition(position_peg_44);
                                    goto out_peg_45;
                                }
                            }
                                
                        }
                        goto success_peg_41;
                        out_peg_45:
                        goto loop_peg_39;
                        success_peg_41:
                        ;
                        result_peg_31.addResult(result_peg_40);
                    } while (true);
                    loop_peg_39:
                    ;
                            
                }
                goto success_peg_37;
                goto out_peg_35;
                success_peg_37:
                ;
            
            
            
            result_peg_31.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
            
            
            
            {
                    int position_peg_49 = result_peg_31.getPosition();
                    
                    result_peg_31.reset();
                    do{
                        Result result_peg_51(result_peg_31.getPosition());
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
                        goto loop_peg_50;
                        success_peg_52:
                        ;
                        result_peg_31.addResult(result_peg_51);
                    } while (true);
                    loop_peg_50:
                    ;
                            
                }
                goto success_peg_48;
                goto out_peg_35;
                success_peg_48:
                ;
            
            
            
            result_peg_31 = rule_valuelist(stream, result_peg_31.getPosition());
                if (result_peg_31.error()){
                    goto out_peg_35;
                }
                data = result_peg_31.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(line, id, data);
                    result_peg_31.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_31.getPosition());
        
        return result_peg_31;
        out_peg_35:
        Result result_peg_58(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_60 = stream.getLineInfo(result_peg_58.getPosition());
                line = &line_info_peg_60;
            
            
            
            result_peg_58 = rule_identifier(stream, result_peg_58.getPosition());
                if (result_peg_58.error()){
                    goto out_peg_62;
                }
                id = result_peg_58.getValues();
            
            
            
            {
                    int position_peg_65 = result_peg_58.getPosition();
                    
                    result_peg_58.reset();
                    do{
                        Result result_peg_67(result_peg_58.getPosition());
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
                        goto loop_peg_66;
                        success_peg_68:
                        ;
                        result_peg_58.addResult(result_peg_67);
                    } while (true);
                    loop_peg_66:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_62;
                success_peg_64:
                ;
            
            
            
            result_peg_58.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_58.getPosition()))){
                        result_peg_58.nextPosition();
                    } else {
                        goto out_peg_62;
                    }
                }
            
            
            
            {
                    int position_peg_76 = result_peg_58.getPosition();
                    
                    result_peg_58.reset();
                    do{
                        Result result_peg_78(result_peg_58.getPosition());
                        {
                            int position_peg_80 = result_peg_78.getPosition();
                            
                            result_peg_78.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_78.getPosition()))){
                                    result_peg_78.nextPosition();
                                } else {
                                    result_peg_78.setPosition(position_peg_80);
                                    goto out_peg_81;
                                }
                            }
                                
                        }
                        goto success_peg_79;
                        out_peg_81:
                        {
                            int position_peg_82 = result_peg_78.getPosition();
                            
                            result_peg_78.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_78.getPosition()))){
                                    result_peg_78.nextPosition();
                                } else {
                                    result_peg_78.setPosition(position_peg_82);
                                    goto out_peg_83;
                                }
                            }
                                
                        }
                        goto success_peg_79;
                        out_peg_83:
                        goto loop_peg_77;
                        success_peg_79:
                        ;
                        result_peg_58.addResult(result_peg_78);
                    } while (true);
                    loop_peg_77:
                    ;
                            
                }
                goto success_peg_75;
                goto out_peg_62;
                success_peg_75:
                ;
            
            
            
            result_peg_58 = rule_filename(stream, result_peg_58.getPosition());
                if (result_peg_58.error()){
                    goto out_peg_62;
                }
                data = result_peg_58.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(line, id, data);
                    result_peg_58.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_58.getPosition());
        
        return result_peg_58;
        out_peg_62:
        Result result_peg_85(myposition);
        
        
        {
        
            result_peg_85 = rule_identifier(stream, result_peg_85.getPosition());
                if (result_peg_85.error()){
                    goto out_peg_87;
                }
                id = result_peg_85.getValues();
            
            
            
            {
                    int position_peg_90 = result_peg_85.getPosition();
                    
                    result_peg_85.reset();
                    do{
                        Result result_peg_92(result_peg_85.getPosition());
                        {
                            int position_peg_94 = result_peg_92.getPosition();
                            
                            result_peg_92.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_92.getPosition()))){
                                    result_peg_92.nextPosition();
                                } else {
                                    result_peg_92.setPosition(position_peg_94);
                                    goto out_peg_95;
                                }
                            }
                                
                        }
                        goto success_peg_93;
                        out_peg_95:
                        {
                            int position_peg_96 = result_peg_92.getPosition();
                            
                            result_peg_92.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_92.getPosition()))){
                                    result_peg_92.nextPosition();
                                } else {
                                    result_peg_92.setPosition(position_peg_96);
                                    goto out_peg_97;
                                }
                            }
                                
                        }
                        goto success_peg_93;
                        out_peg_97:
                        goto loop_peg_91;
                        success_peg_93:
                        ;
                        result_peg_85.addResult(result_peg_92);
                    } while (true);
                    loop_peg_91:
                    ;
                            
                }
                goto success_peg_89;
                goto out_peg_87;
                success_peg_89:
                ;
            
            
            
            result_peg_85.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_85.getPosition()))){
                        result_peg_85.nextPosition();
                    } else {
                        goto out_peg_87;
                    }
                }
            
            
            
            {
                    int position_peg_101 = result_peg_85.getPosition();
                    
                    result_peg_85.reset();
                    do{
                        Result result_peg_103(result_peg_85.getPosition());
                        {
                            int position_peg_105 = result_peg_103.getPosition();
                            
                            result_peg_103.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_103.getPosition()))){
                                    result_peg_103.nextPosition();
                                } else {
                                    result_peg_103.setPosition(position_peg_105);
                                    goto out_peg_106;
                                }
                            }
                                
                        }
                        goto success_peg_104;
                        out_peg_106:
                        {
                            int position_peg_107 = result_peg_103.getPosition();
                            
                            result_peg_103.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_103.getPosition()))){
                                    result_peg_103.nextPosition();
                                } else {
                                    result_peg_103.setPosition(position_peg_107);
                                    goto out_peg_108;
                                }
                            }
                                
                        }
                        goto success_peg_104;
                        out_peg_108:
                        goto loop_peg_102;
                        success_peg_104:
                        ;
                        result_peg_85.addResult(result_peg_103);
                    } while (true);
                    loop_peg_102:
                    ;
                            
                }
                goto success_peg_100;
                goto out_peg_87;
                success_peg_100:
                ;
            
            
            
            result_peg_85 = rule_number(stream, result_peg_85.getPosition());
                if (result_peg_85.error()){
                    goto out_peg_87;
                }
                index = result_peg_85.getValues();
            
            
            
            {
                    int position_peg_112 = result_peg_85.getPosition();
                    
                    result_peg_85.reset();
                    do{
                        Result result_peg_114(result_peg_85.getPosition());
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
                        goto loop_peg_113;
                        success_peg_115:
                        ;
                        result_peg_85.addResult(result_peg_114);
                    } while (true);
                    loop_peg_113:
                    ;
                            
                }
                goto success_peg_111;
                goto out_peg_87;
                success_peg_111:
                ;
            
            
            
            result_peg_85.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_85.getPosition()))){
                        result_peg_85.nextPosition();
                    } else {
                        goto out_peg_87;
                    }
                }
            
            
            
            {
                    int position_peg_123 = result_peg_85.getPosition();
                    
                    result_peg_85.reset();
                    do{
                        Result result_peg_125(result_peg_85.getPosition());
                        {
                            int position_peg_127 = result_peg_125.getPosition();
                            
                            result_peg_125.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_125.getPosition()))){
                                    result_peg_125.nextPosition();
                                } else {
                                    result_peg_125.setPosition(position_peg_127);
                                    goto out_peg_128;
                                }
                            }
                                
                        }
                        goto success_peg_126;
                        out_peg_128:
                        {
                            int position_peg_129 = result_peg_125.getPosition();
                            
                            result_peg_125.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_125.getPosition()))){
                                    result_peg_125.nextPosition();
                                } else {
                                    result_peg_125.setPosition(position_peg_129);
                                    goto out_peg_130;
                                }
                            }
                                
                        }
                        goto success_peg_126;
                        out_peg_130:
                        goto loop_peg_124;
                        success_peg_126:
                        ;
                        result_peg_85.addResult(result_peg_125);
                    } while (true);
                    loop_peg_124:
                    ;
                            
                }
                goto success_peg_122;
                goto out_peg_87;
                success_peg_122:
                ;
            
            
            
            result_peg_85.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_85.getPosition()))){
                        result_peg_85.nextPosition();
                    } else {
                        goto out_peg_87;
                    }
                }
            
            
            
            {
                    int position_peg_134 = result_peg_85.getPosition();
                    
                    result_peg_85.reset();
                    do{
                        Result result_peg_136(result_peg_85.getPosition());
                        {
                            int position_peg_138 = result_peg_136.getPosition();
                            
                            result_peg_136.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_136.getPosition()))){
                                    result_peg_136.nextPosition();
                                } else {
                                    result_peg_136.setPosition(position_peg_138);
                                    goto out_peg_139;
                                }
                            }
                                
                        }
                        goto success_peg_137;
                        out_peg_139:
                        {
                            int position_peg_140 = result_peg_136.getPosition();
                            
                            result_peg_136.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_136.getPosition()))){
                                    result_peg_136.nextPosition();
                                } else {
                                    result_peg_136.setPosition(position_peg_140);
                                    goto out_peg_141;
                                }
                            }
                                
                        }
                        goto success_peg_137;
                        out_peg_141:
                        goto loop_peg_135;
                        success_peg_137:
                        ;
                        result_peg_85.addResult(result_peg_136);
                    } while (true);
                    loop_peg_135:
                    ;
                            
                }
                goto success_peg_133;
                goto out_peg_87;
                success_peg_133:
                ;
            
            
            
            result_peg_85 = rule_valuelist(stream, result_peg_85.getPosition());
                if (result_peg_85.error()){
                    goto out_peg_87;
                }
                data = result_peg_85.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(id, index, data);
                    result_peg_85.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_85.getPosition());
        
        return result_peg_85;
        out_peg_87:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_attribute_simple(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "attribute_simple");
    int myposition = position;
    
    
    Value id;
        Value data;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_identifier(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                id = result_peg_3.getValues();
            
            
            
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
            
            
            
            result_peg_3.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_5;
                    }
                }
            
            
            
            {
                    int position_peg_19 = result_peg_3.getPosition();
                    
                    result_peg_3.reset();
                    do{
                        Result result_peg_21(result_peg_3.getPosition());
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
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_3.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
                    ;
                            
                }
                goto success_peg_18;
                goto out_peg_5;
                success_peg_18:
                ;
            
            
            
            result_peg_3 = rule_value(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
                data = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_5:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "identifier");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_name(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_8(result_peg_3.getPosition());
                    {
                    
                        result_peg_8.setValue(Value((void*) "."));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_8.getPosition()))){
                                    result_peg_8.nextPosition();
                                } else {
                                    goto loop_peg_7;
                                }
                            }
                        
                        
                        
                        result_peg_8 = rule_name(stream, result_peg_8.getPosition());
                            if (result_peg_8.error()){
                                goto loop_peg_7;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_8);
                } while (true);
                loop_peg_7:
                ;
            
            Result result_peg_6 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(result_peg_4.getValues(),result_peg_6.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_5:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "identifier_list");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3 = rule_identifier(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_5;
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_7(result_peg_3.getPosition());
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
                                int position_peg_21 = result_peg_7.getPosition();
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_23(result_peg_7.getPosition());
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
                                    goto loop_peg_22;
                                    success_peg_24:
                                    ;
                                    result_peg_7.addResult(result_peg_23);
                                } while (true);
                                loop_peg_22:
                                ;
                                        
                            }
                            goto success_peg_20;
                            goto loop_peg_6;
                            success_peg_20:
                            ;
                        
                        
                        
                        result_peg_7 = rule_filename(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                if (result_peg_3.matches() == 0){
                    goto out_peg_5;
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_5:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "valuelist");
    int myposition = position;
    
    
    Value line;
    Result result_peg_3(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_5 = stream.getLineInfo(result_peg_3.getPosition());
                line = &line_info_peg_5;
            
            
            
            result_peg_3 = rule_value(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_7;
                }
            
            Result result_peg_6 = result_peg_3;
            
            result_peg_3.reset();
                do{
                    Result result_peg_10(result_peg_3.getPosition());
                    {
                    
                        {
                                int position_peg_13 = result_peg_10.getPosition();
                                
                                result_peg_10.reset();
                                do{
                                    Result result_peg_15(result_peg_10.getPosition());
                                    {
                                        int position_peg_17 = result_peg_15.getPosition();
                                        
                                        result_peg_15.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_15.getPosition()))){
                                                result_peg_15.nextPosition();
                                            } else {
                                                result_peg_15.setPosition(position_peg_17);
                                                goto out_peg_18;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_16;
                                    out_peg_18:
                                    {
                                        int position_peg_19 = result_peg_15.getPosition();
                                        
                                        result_peg_15.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_15.getPosition()))){
                                                result_peg_15.nextPosition();
                                            } else {
                                                result_peg_15.setPosition(position_peg_19);
                                                goto out_peg_20;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_16;
                                    out_peg_20:
                                    goto loop_peg_14;
                                    success_peg_16:
                                    ;
                                    result_peg_10.addResult(result_peg_15);
                                } while (true);
                                loop_peg_14:
                                ;
                                        
                            }
                            goto success_peg_12;
                            goto loop_peg_9;
                            success_peg_12:
                            ;
                        
                        
                        
                        result_peg_10.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    goto loop_peg_9;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_24 = result_peg_10.getPosition();
                                
                                result_peg_10.reset();
                                do{
                                    Result result_peg_26(result_peg_10.getPosition());
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
                                    goto loop_peg_25;
                                    success_peg_27:
                                    ;
                                    result_peg_10.addResult(result_peg_26);
                                } while (true);
                                loop_peg_25:
                                ;
                                        
                            }
                            goto success_peg_23;
                            goto loop_peg_9;
                            success_peg_23:
                            ;
                        
                        
                        
                        int save_peg_32 = result_peg_10.getPosition();
                            
                            result_peg_10 = rule_value(stream, result_peg_10.getPosition());
                            if (result_peg_10.error()){
                                
                                result_peg_10 = Result(save_peg_32);
                                result_peg_10.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_3.addResult(result_peg_10);
                } while (true);
                loop_peg_9:
                ;
            
            Result result_peg_8 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeValueList(line, result_peg_6.getValues(),result_peg_8.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_7:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "value");
    int myposition = position;
    
    
    Value data;
    Result result_peg_3(myposition);
        
        
        {
            int position_peg_5 = result_peg_3.getPosition();
            {
            
                result_peg_3.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_5);
                            goto out_peg_7;
                        }
                    }
                
                
                
                result_peg_3.reset();
                    do{
                        Result result_peg_10(result_peg_3.getPosition());
                        {
                        
                            Result result_peg_13(result_peg_10);
                                result_peg_13.setValue(Value((void*) "\""));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\""[i], stream.get(result_peg_13.getPosition()))){
                                        result_peg_13.nextPosition();
                                    } else {
                                        goto not_peg_12;
                                    }
                                }
                                goto loop_peg_9;
                                not_peg_12:
                                result_peg_10.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_16(result_peg_10);
                                result_peg_16.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_16.getPosition()))){
                                        result_peg_16.nextPosition();
                                    } else {
                                        goto not_peg_15;
                                    }
                                }
                                goto loop_peg_9;
                                not_peg_15:
                                result_peg_10.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_17 = stream.get(result_peg_10.getPosition());
                                if (temp_peg_17 != '\0'){
                                    result_peg_10.setValue(Value((void*) (intptr_t) temp_peg_17));
                                    result_peg_10.nextPosition();
                                } else {
                                    goto loop_peg_9;
                                }
                            
                            
                        }
                        result_peg_3.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                    data = result_peg_3.getValues();
                
                
                
                result_peg_3.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_3.getPosition()))){
                            result_peg_3.nextPosition();
                        } else {
                            result_peg_3.setPosition(position_peg_5);
                            goto out_peg_7;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = makeString(data);
                        result_peg_3.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_4;
        out_peg_7:
        goto out_peg_19;
        success_peg_4:
        ;
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_19:
        Result result_peg_20(myposition);
        
        
        result_peg_20 = rule_date(stream, result_peg_20.getPosition());
        if (result_peg_20.error()){
            goto out_peg_21;
        }
        
        stream.update(result_peg_20.getPosition());
        
        return result_peg_20;
        out_peg_21:
        Result result_peg_22(myposition);
        
        
        {
        
            result_peg_22 = rule_number(stream, result_peg_22.getPosition());
                if (result_peg_22.error()){
                    goto out_peg_24;
                }
            
            Result result_peg_23 = result_peg_22;
            
            Result result_peg_27(result_peg_22);
                {
                    int position_peg_29 = result_peg_27.getPosition();
                    
                    char letter_peg_30 = stream.get(result_peg_27.getPosition());
                    if (letter_peg_30 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_30) != NULL){
                        result_peg_27.nextPosition();
                        result_peg_27.setValue(Value((void*) (intptr_t) letter_peg_30));
                    } else {
                        result_peg_27.setPosition(position_peg_29);
                        goto out_peg_31;
                    }
                    
                }
                goto success_peg_28;
                out_peg_31:
                goto not_peg_26;
                success_peg_28:
                ;
                goto out_peg_24;
                not_peg_26:
                result_peg_22.setValue(Value((void*)0));
            
            
            
            Result result_peg_34(result_peg_22);
                result_peg_34.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_34.getPosition()))){
                        result_peg_34.nextPosition();
                    } else {
                        goto not_peg_33;
                    }
                }
                goto out_peg_24;
                not_peg_33:
                result_peg_22.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_23.getValues();
                    result_peg_22.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_22.getPosition());
        
        return result_peg_22;
        out_peg_24:
        Result result_peg_35(myposition);
        
        
        {
        
            result_peg_35 = rule_attribute_simple(stream, result_peg_35.getPosition());
                if (result_peg_35.error()){
                    goto out_peg_37;
                }
            
            Result result_peg_36 = result_peg_35;
            
            {
                    Value value((void*) 0);
                    value = makeValueAttribute(result_peg_36.getValues());
                    result_peg_35.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_35.getPosition());
        
        return result_peg_35;
        out_peg_37:
        Result result_peg_38(myposition);
        
        
        {
        
            {
                    int position_peg_41 = result_peg_38.getPosition();
                    
                    result_peg_38.setValue(Value((void*) "normal"));
                    for (int i = 0; i < 6; i++){
                        if (compareChar("normal"[i], stream.get(result_peg_38.getPosition()))){
                            result_peg_38.nextPosition();
                        } else {
                            result_peg_38.setPosition(position_peg_41);
                            goto out_peg_42;
                        }
                    }
                        
                }
                goto success_peg_40;
                out_peg_42:
                {
                    int position_peg_43 = result_peg_38.getPosition();
                    
                    result_peg_38.setValue(Value((void*) "parallax"));
                    for (int i = 0; i < 8; i++){
                        if (compareChar("parallax"[i], stream.get(result_peg_38.getPosition()))){
                            result_peg_38.nextPosition();
                        } else {
                            result_peg_38.setPosition(position_peg_43);
                            goto out_peg_44;
                        }
                    }
                        
                }
                goto success_peg_40;
                out_peg_44:
                {
                    int position_peg_45 = result_peg_38.getPosition();
                    
                    result_peg_38.setValue(Value((void*) "addalpha"));
                    for (int i = 0; i < 8; i++){
                        if (compareChar("addalpha"[i], stream.get(result_peg_38.getPosition()))){
                            result_peg_38.nextPosition();
                        } else {
                            result_peg_38.setPosition(position_peg_45);
                            goto out_peg_46;
                        }
                    }
                        
                }
                goto success_peg_40;
                out_peg_46:
                {
                    int position_peg_47 = result_peg_38.getPosition();
                    
                    result_peg_38.setValue(Value((void*) "add1"));
                    for (int i = 0; i < 4; i++){
                        if (compareChar("add1"[i], stream.get(result_peg_38.getPosition()))){
                            result_peg_38.nextPosition();
                        } else {
                            result_peg_38.setPosition(position_peg_47);
                            goto out_peg_48;
                        }
                    }
                        
                }
                goto success_peg_40;
                out_peg_48:
                {
                    int position_peg_49 = result_peg_38.getPosition();
                    
                    result_peg_38.setValue(Value((void*) "add"));
                    for (int i = 0; i < 3; i++){
                        if (compareChar("add"[i], stream.get(result_peg_38.getPosition()))){
                            result_peg_38.nextPosition();
                        } else {
                            result_peg_38.setPosition(position_peg_49);
                            goto out_peg_50;
                        }
                    }
                        
                }
                goto success_peg_40;
                out_peg_50:
                {
                    int position_peg_51 = result_peg_38.getPosition();
                    
                    result_peg_38.setValue(Value((void*) "sub"));
                    for (int i = 0; i < 3; i++){
                        if (compareChar("sub"[i], stream.get(result_peg_38.getPosition()))){
                            result_peg_38.nextPosition();
                        } else {
                            result_peg_38.setPosition(position_peg_51);
                            goto out_peg_52;
                        }
                    }
                        
                }
                goto success_peg_40;
                out_peg_52:
                goto out_peg_53;
                success_peg_40:
                ;
            
            Result result_peg_39 = result_peg_38;
            
            Result result_peg_56(result_peg_38);
                {
                    int position_peg_58 = result_peg_56.getPosition();
                    
                    {
                        int position_peg_60 = result_peg_56.getPosition();
                        
                        char letter_peg_61 = stream.get(result_peg_56.getPosition());
                        if (letter_peg_61 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_61) != NULL){
                            result_peg_56.nextPosition();
                            result_peg_56.setValue(Value((void*) (intptr_t) letter_peg_61));
                        } else {
                            result_peg_56.setPosition(position_peg_60);
                            goto out_peg_62;
                        }
                        
                    }
                    goto success_peg_59;
                    out_peg_62:
                    result_peg_56.setPosition(position_peg_58);
                    goto out_peg_63;
                    success_peg_59:
                    ;
                    
                }
                goto success_peg_57;
                out_peg_63:
                {
                    int position_peg_64 = result_peg_56.getPosition();
                    
                    {
                        int position_peg_66 = result_peg_56.getPosition();
                        
                        char letter_peg_67 = stream.get(result_peg_56.getPosition());
                        if (letter_peg_67 != '\0' && strchr("0123456789", letter_peg_67) != NULL){
                            result_peg_56.nextPosition();
                            result_peg_56.setValue(Value((void*) (intptr_t) letter_peg_67));
                        } else {
                            result_peg_56.setPosition(position_peg_66);
                            goto out_peg_68;
                        }
                        
                    }
                    goto success_peg_65;
                    out_peg_68:
                    result_peg_56.setPosition(position_peg_64);
                    goto out_peg_69;
                    success_peg_65:
                    ;
                    
                }
                goto success_peg_57;
                out_peg_69:
                goto not_peg_55;
                success_peg_57:
                ;
                goto out_peg_53;
                not_peg_55:
                result_peg_38.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_39.getValues());
                    result_peg_38.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_38.getPosition());
        
        return result_peg_38;
        out_peg_53:
        Result result_peg_70(myposition);
        
        
        {
        
            result_peg_70.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_70.getPosition()))){
                        result_peg_70.nextPosition();
                    } else {
                        goto out_peg_72;
                    }
                }
            
            Result result_peg_71 = result_peg_70;
            
            Result result_peg_75(result_peg_70);
                {
                    int position_peg_77 = result_peg_75.getPosition();
                    {
                    
                        Result result_peg_80(result_peg_75);
                            result_peg_80.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_80.getPosition()))){
                                    result_peg_80.nextPosition();
                                } else {
                                    goto not_peg_79;
                                }
                            }
                            result_peg_75.setPosition(position_peg_77);
                            goto out_peg_81;
                            not_peg_79:
                            result_peg_75.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_84(result_peg_75);
                            result_peg_84.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_84.getPosition()))){
                                    result_peg_84.nextPosition();
                                } else {
                                    goto not_peg_83;
                                }
                            }
                            result_peg_75.setPosition(position_peg_77);
                            goto out_peg_81;
                            not_peg_83:
                            result_peg_75.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_87(result_peg_75);
                            result_peg_87.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_87.getPosition()))){
                                    result_peg_87.nextPosition();
                                } else {
                                    goto not_peg_86;
                                }
                            }
                            result_peg_75.setPosition(position_peg_77);
                            goto out_peg_81;
                            not_peg_86:
                            result_peg_75.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_90(result_peg_75);
                            result_peg_90.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_90.getPosition()) == (unsigned char) 13){
                                result_peg_90.nextPosition();
                            } else {
                                goto not_peg_89;
                            }
                            result_peg_75.setPosition(position_peg_77);
                            goto out_peg_81;
                            not_peg_89:
                            result_peg_75.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_93(result_peg_75);
                            result_peg_93.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_93.getPosition()))){
                                    result_peg_93.nextPosition();
                                } else {
                                    goto not_peg_92;
                                }
                            }
                            result_peg_75.setPosition(position_peg_77);
                            goto out_peg_81;
                            not_peg_92:
                            result_peg_75.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_96(result_peg_75);
                            result_peg_96.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    goto not_peg_95;
                                }
                            }
                            result_peg_75.setPosition(position_peg_77);
                            goto out_peg_81;
                            not_peg_95:
                            result_peg_75.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_97 = stream.get(result_peg_75.getPosition());
                            if (temp_peg_97 != '\0'){
                                result_peg_75.setValue(Value((void*) (intptr_t) temp_peg_97));
                                result_peg_75.nextPosition();
                            } else {
                                result_peg_75.setPosition(position_peg_77);
                                goto out_peg_81;
                            }
                        
                        
                    }
                }
                goto success_peg_76;
                out_peg_81:
                goto not_peg_74;
                success_peg_76:
                ;
                goto out_peg_72;
                not_peg_74:
                result_peg_70.setValue(Value((void*)0));
            
            
            
            Result result_peg_100(result_peg_70);
                result_peg_100.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_100.getPosition()))){
                        result_peg_100.nextPosition();
                    } else {
                        goto not_peg_99;
                    }
                }
                goto out_peg_72;
                not_peg_99:
                result_peg_70.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_71.getValues());
                    result_peg_70.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_70.getPosition());
        
        return result_peg_70;
        out_peg_72:
        Result result_peg_101(myposition);
        
        
        {
        
            result_peg_101.setValue(Value((void*) "h"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_101.getPosition()))){
                        result_peg_101.nextPosition();
                    } else {
                        goto out_peg_103;
                    }
                }
            
            Result result_peg_102 = result_peg_101;
            
            Result result_peg_106(result_peg_101);
                {
                    int position_peg_108 = result_peg_106.getPosition();
                    {
                    
                        Result result_peg_111(result_peg_106);
                            result_peg_111.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_111.getPosition()))){
                                    result_peg_111.nextPosition();
                                } else {
                                    goto not_peg_110;
                                }
                            }
                            result_peg_106.setPosition(position_peg_108);
                            goto out_peg_112;
                            not_peg_110:
                            result_peg_106.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_115(result_peg_106);
                            result_peg_115.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_115.getPosition()))){
                                    result_peg_115.nextPosition();
                                } else {
                                    goto not_peg_114;
                                }
                            }
                            result_peg_106.setPosition(position_peg_108);
                            goto out_peg_112;
                            not_peg_114:
                            result_peg_106.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_118(result_peg_106);
                            result_peg_118.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_118.getPosition()))){
                                    result_peg_118.nextPosition();
                                } else {
                                    goto not_peg_117;
                                }
                            }
                            result_peg_106.setPosition(position_peg_108);
                            goto out_peg_112;
                            not_peg_117:
                            result_peg_106.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_121(result_peg_106);
                            result_peg_121.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_121.getPosition()) == (unsigned char) 13){
                                result_peg_121.nextPosition();
                            } else {
                                goto not_peg_120;
                            }
                            result_peg_106.setPosition(position_peg_108);
                            goto out_peg_112;
                            not_peg_120:
                            result_peg_106.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_124(result_peg_106);
                            result_peg_124.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_124.getPosition()))){
                                    result_peg_124.nextPosition();
                                } else {
                                    goto not_peg_123;
                                }
                            }
                            result_peg_106.setPosition(position_peg_108);
                            goto out_peg_112;
                            not_peg_123:
                            result_peg_106.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_127(result_peg_106);
                            result_peg_127.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_127.getPosition()))){
                                    result_peg_127.nextPosition();
                                } else {
                                    goto not_peg_126;
                                }
                            }
                            result_peg_106.setPosition(position_peg_108);
                            goto out_peg_112;
                            not_peg_126:
                            result_peg_106.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_128 = stream.get(result_peg_106.getPosition());
                            if (temp_peg_128 != '\0'){
                                result_peg_106.setValue(Value((void*) (intptr_t) temp_peg_128));
                                result_peg_106.nextPosition();
                            } else {
                                result_peg_106.setPosition(position_peg_108);
                                goto out_peg_112;
                            }
                        
                        
                    }
                }
                goto success_peg_107;
                out_peg_112:
                goto not_peg_105;
                success_peg_107:
                ;
                goto out_peg_103;
                not_peg_105:
                result_peg_101.setValue(Value((void*)0));
            
            
            
            Result result_peg_131(result_peg_101);
                result_peg_131.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_131.getPosition()))){
                        result_peg_131.nextPosition();
                    } else {
                        goto not_peg_130;
                    }
                }
                goto out_peg_103;
                not_peg_130:
                result_peg_101.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_102.getValues());
                    result_peg_101.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_101.getPosition());
        
        return result_peg_101;
        out_peg_103:
        Result result_peg_132(myposition);
        
        
        {
        
            result_peg_132.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_132.getPosition()))){
                        result_peg_132.nextPosition();
                    } else {
                        goto out_peg_134;
                    }
                }
            
            Result result_peg_133 = result_peg_132;
            
            int save_peg_136 = result_peg_132.getPosition();
                
                result_peg_132 = rule_number(stream, result_peg_132.getPosition());
                if (result_peg_132.error()){
                    
                    result_peg_132 = Result(save_peg_136);
                    result_peg_132.setValue(Value((void*) 0));
                    
                }
            
            
            
            Result result_peg_139(result_peg_132);
                {
                    int position_peg_141 = result_peg_139.getPosition();
                    {
                    
                        Result result_peg_144(result_peg_139);
                            result_peg_144.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_144.getPosition()))){
                                    result_peg_144.nextPosition();
                                } else {
                                    goto not_peg_143;
                                }
                            }
                            result_peg_139.setPosition(position_peg_141);
                            goto out_peg_145;
                            not_peg_143:
                            result_peg_139.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_148(result_peg_139);
                            result_peg_148.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_148.getPosition()))){
                                    result_peg_148.nextPosition();
                                } else {
                                    goto not_peg_147;
                                }
                            }
                            result_peg_139.setPosition(position_peg_141);
                            goto out_peg_145;
                            not_peg_147:
                            result_peg_139.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_151(result_peg_139);
                            result_peg_151.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_151.getPosition()))){
                                    result_peg_151.nextPosition();
                                } else {
                                    goto not_peg_150;
                                }
                            }
                            result_peg_139.setPosition(position_peg_141);
                            goto out_peg_145;
                            not_peg_150:
                            result_peg_139.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_154(result_peg_139);
                            result_peg_154.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_154.getPosition()) == (unsigned char) 13){
                                result_peg_154.nextPosition();
                            } else {
                                goto not_peg_153;
                            }
                            result_peg_139.setPosition(position_peg_141);
                            goto out_peg_145;
                            not_peg_153:
                            result_peg_139.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_157(result_peg_139);
                            result_peg_157.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_157.getPosition()))){
                                    result_peg_157.nextPosition();
                                } else {
                                    goto not_peg_156;
                                }
                            }
                            result_peg_139.setPosition(position_peg_141);
                            goto out_peg_145;
                            not_peg_156:
                            result_peg_139.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_160(result_peg_139);
                            result_peg_160.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_160.getPosition()))){
                                    result_peg_160.nextPosition();
                                } else {
                                    goto not_peg_159;
                                }
                            }
                            result_peg_139.setPosition(position_peg_141);
                            goto out_peg_145;
                            not_peg_159:
                            result_peg_139.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_161 = stream.get(result_peg_139.getPosition());
                            if (temp_peg_161 != '\0'){
                                result_peg_139.setValue(Value((void*) (intptr_t) temp_peg_161));
                                result_peg_139.nextPosition();
                            } else {
                                result_peg_139.setPosition(position_peg_141);
                                goto out_peg_145;
                            }
                        
                        
                    }
                }
                goto success_peg_140;
                out_peg_145:
                goto not_peg_138;
                success_peg_140:
                ;
                goto out_peg_134;
                not_peg_138:
                result_peg_132.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_133.getValues());
                    result_peg_132.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_132.getPosition());
        
        return result_peg_132;
        out_peg_134:
        Result result_peg_162(myposition);
        
        
        result_peg_162 = rule_filename(stream, result_peg_162.getPosition());
        if (result_peg_162.error()){
            goto out_peg_163;
        }
        
        stream.update(result_peg_162.getPosition());
        
        return result_peg_162;
        out_peg_163:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "date");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.reset();
                do{
                    Result result_peg_6(result_peg_3.getPosition());
                    {
                        int position_peg_8 = result_peg_6.getPosition();
                        
                        char letter_peg_9 = stream.get(result_peg_6.getPosition());
                        if (letter_peg_9 != '\0' && strchr("0123456789", letter_peg_9) != NULL){
                            result_peg_6.nextPosition();
                            result_peg_6.setValue(Value((void*) (intptr_t) letter_peg_9));
                        } else {
                            result_peg_6.setPosition(position_peg_8);
                            goto out_peg_10;
                        }
                        
                    }
                    goto success_peg_7;
                    out_peg_10:
                    goto loop_peg_5;
                    success_peg_7:
                    ;
                    result_peg_3.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                if (result_peg_3.matches() == 0){
                    goto out_peg_11;
                }
            
            Result result_peg_4 = result_peg_3;
            
            result_peg_3.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_15(result_peg_3.getPosition());
                    {
                        int position_peg_17 = result_peg_15.getPosition();
                        
                        char letter_peg_18 = stream.get(result_peg_15.getPosition());
                        if (letter_peg_18 != '\0' && strchr("0123456789", letter_peg_18) != NULL){
                            result_peg_15.nextPosition();
                            result_peg_15.setValue(Value((void*) (intptr_t) letter_peg_18));
                        } else {
                            result_peg_15.setPosition(position_peg_17);
                            goto out_peg_19;
                        }
                        
                    }
                    goto success_peg_16;
                    out_peg_19:
                    goto loop_peg_14;
                    success_peg_16:
                    ;
                    result_peg_3.addResult(result_peg_15);
                } while (true);
                loop_peg_14:
                if (result_peg_3.matches() == 0){
                    goto out_peg_11;
                }
            
            Result result_peg_13 = result_peg_3;
            
            result_peg_3.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_23(result_peg_3.getPosition());
                    {
                        int position_peg_25 = result_peg_23.getPosition();
                        
                        char letter_peg_26 = stream.get(result_peg_23.getPosition());
                        if (letter_peg_26 != '\0' && strchr("0123456789", letter_peg_26) != NULL){
                            result_peg_23.nextPosition();
                            result_peg_23.setValue(Value((void*) (intptr_t) letter_peg_26));
                        } else {
                            result_peg_23.setPosition(position_peg_25);
                            goto out_peg_27;
                        }
                        
                    }
                    goto success_peg_24;
                    out_peg_27:
                    goto loop_peg_22;
                    success_peg_24:
                    ;
                    result_peg_3.addResult(result_peg_23);
                } while (true);
                loop_peg_22:
                if (result_peg_3.matches() == 0){
                    goto out_peg_11;
                }
            
            Result result_peg_21 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_4.getValues(),result_peg_13.getValues(),result_peg_21.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_11:
        Result result_peg_28(myposition);
        
        
        {
        
            result_peg_28.reset();
                do{
                    Result result_peg_31(result_peg_28.getPosition());
                    {
                        int position_peg_33 = result_peg_31.getPosition();
                        
                        char letter_peg_34 = stream.get(result_peg_31.getPosition());
                        if (letter_peg_34 != '\0' && strchr("0123456789", letter_peg_34) != NULL){
                            result_peg_31.nextPosition();
                            result_peg_31.setValue(Value((void*) (intptr_t) letter_peg_34));
                        } else {
                            result_peg_31.setPosition(position_peg_33);
                            goto out_peg_35;
                        }
                        
                    }
                    goto success_peg_32;
                    out_peg_35:
                    goto loop_peg_30;
                    success_peg_32:
                    ;
                    result_peg_28.addResult(result_peg_31);
                } while (true);
                loop_peg_30:
                if (result_peg_28.matches() == 0){
                    goto out_peg_36;
                }
            
            Result result_peg_29 = result_peg_28;
            
            result_peg_28.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_36;
                    }
                }
            
            
            
            result_peg_28.reset();
                do{
                    Result result_peg_40(result_peg_28.getPosition());
                    {
                        int position_peg_42 = result_peg_40.getPosition();
                        
                        char letter_peg_43 = stream.get(result_peg_40.getPosition());
                        if (letter_peg_43 != '\0' && strchr("0123456789", letter_peg_43) != NULL){
                            result_peg_40.nextPosition();
                            result_peg_40.setValue(Value((void*) (intptr_t) letter_peg_43));
                        } else {
                            result_peg_40.setPosition(position_peg_42);
                            goto out_peg_44;
                        }
                        
                    }
                    goto success_peg_41;
                    out_peg_44:
                    goto loop_peg_39;
                    success_peg_41:
                    ;
                    result_peg_28.addResult(result_peg_40);
                } while (true);
                loop_peg_39:
                if (result_peg_28.matches() == 0){
                    goto out_peg_36;
                }
            
            Result result_peg_38 = result_peg_28;
            
            result_peg_28.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_28.getPosition()))){
                        result_peg_28.nextPosition();
                    } else {
                        goto out_peg_36;
                    }
                }
            
            
            
            result_peg_28.reset();
                do{
                    Result result_peg_48(result_peg_28.getPosition());
                    {
                        int position_peg_50 = result_peg_48.getPosition();
                        
                        char letter_peg_51 = stream.get(result_peg_48.getPosition());
                        if (letter_peg_51 != '\0' && strchr("0123456789", letter_peg_51) != NULL){
                            result_peg_48.nextPosition();
                            result_peg_48.setValue(Value((void*) (intptr_t) letter_peg_51));
                        } else {
                            result_peg_48.setPosition(position_peg_50);
                            goto out_peg_52;
                        }
                        
                    }
                    goto success_peg_49;
                    out_peg_52:
                    goto loop_peg_47;
                    success_peg_49:
                    ;
                    result_peg_28.addResult(result_peg_48);
                } while (true);
                loop_peg_47:
                if (result_peg_28.matches() == 0){
                    goto out_peg_36;
                }
            
            Result result_peg_46 = result_peg_28;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_29.getValues(),result_peg_38.getValues(),result_peg_46.getValues());
                    result_peg_28.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_28.getPosition());
        
        return result_peg_28;
        out_peg_36:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "number");
    int myposition = position;
    
    
    
    Result result_peg_3(myposition);
        
        
        {
        
            int save_peg_5 = result_peg_3.getPosition();
                
                {
                    int position_peg_7 = result_peg_3.getPosition();
                    
                    result_peg_3.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_3.getPosition()))){
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
                    
                    result_peg_3.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_3.getPosition()))){
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
            
            result_peg_3 = rule_float_or_integer(stream, result_peg_3.getPosition());
                if (result_peg_3.error()){
                    goto out_peg_12;
                }
            
            Result result_peg_11 = result_peg_3;
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_4.getValues(),result_peg_11.getValues());
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_12:
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_1(stream, "float_or_integer");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_3(myposition);
        
        
        {
        
            result_peg_3.reset();
                do{
                    Result result_peg_6(result_peg_3.getPosition());
                    {
                        int position_peg_8 = result_peg_6.getPosition();
                        
                        char letter_peg_9 = stream.get(result_peg_6.getPosition());
                        if (letter_peg_9 != '\0' && strchr("0123456789", letter_peg_9) != NULL){
                            result_peg_6.nextPosition();
                            result_peg_6.setValue(Value((void*) (intptr_t) letter_peg_9));
                        } else {
                            result_peg_6.setPosition(position_peg_8);
                            goto out_peg_10;
                        }
                        
                    }
                    goto success_peg_7;
                    out_peg_10:
                    goto loop_peg_5;
                    success_peg_7:
                    ;
                    result_peg_3.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                ;
                left = result_peg_3.getValues();
            
            
            
            result_peg_3.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_3.getPosition()))){
                        result_peg_3.nextPosition();
                    } else {
                        goto out_peg_12;
                    }
                }
            
            
            
            result_peg_3.reset();
                do{
                    Result result_peg_15(result_peg_3.getPosition());
                    {
                        int position_peg_17 = result_peg_15.getPosition();
                        
                        char letter_peg_18 = stream.get(result_peg_15.getPosition());
                        if (letter_peg_18 != '\0' && strchr("0123456789", letter_peg_18) != NULL){
                            result_peg_15.nextPosition();
                            result_peg_15.setValue(Value((void*) (intptr_t) letter_peg_18));
                        } else {
                            result_peg_15.setPosition(position_peg_17);
                            goto out_peg_19;
                        }
                        
                    }
                    goto success_peg_16;
                    out_peg_19:
                    goto loop_peg_14;
                    success_peg_16:
                    ;
                    result_peg_3.addResult(result_peg_15);
                } while (true);
                loop_peg_14:
                if (result_peg_3.matches() == 0){
                    goto out_peg_12;
                }
                right = result_peg_3.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(left,right);
                    result_peg_3.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_3.getPosition());
        
        return result_peg_3;
        out_peg_12:
        Result result_peg_20(myposition);
        
        
        {
        
            result_peg_20.reset();
                do{
                    Result result_peg_23(result_peg_20.getPosition());
                    {
                        int position_peg_25 = result_peg_23.getPosition();
                        
                        char letter_peg_26 = stream.get(result_peg_23.getPosition());
                        if (letter_peg_26 != '\0' && strchr("0123456789", letter_peg_26) != NULL){
                            result_peg_23.nextPosition();
                            result_peg_23.setValue(Value((void*) (intptr_t) letter_peg_26));
                        } else {
                            result_peg_23.setPosition(position_peg_25);
                            goto out_peg_27;
                        }
                        
                    }
                    goto success_peg_24;
                    out_peg_27:
                    goto loop_peg_22;
                    success_peg_24:
                    ;
                    result_peg_20.addResult(result_peg_23);
                } while (true);
                loop_peg_22:
                if (result_peg_20.matches() == 0){
                    goto out_peg_28;
                }
            
            Result result_peg_21 = result_peg_20;
            
            Result result_peg_31(result_peg_20);
                result_peg_31.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_31.getPosition()))){
                        result_peg_31.nextPosition();
                    } else {
                        goto not_peg_30;
                    }
                }
                goto out_peg_28;
                not_peg_30:
                result_peg_20.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_21.getValues());
                    result_peg_20.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_20.getPosition());
        
        return result_peg_20;
        out_peg_28:
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



    
    } /* Def */
    
} /* Mugen */

    
