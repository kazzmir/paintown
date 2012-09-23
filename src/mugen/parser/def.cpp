

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
    
    
    RuleTrace trace_peg_111(stream, "start");
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
            
            
            
            {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_8(result_peg_2.getPosition());
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
                            
                            {
                                int position_peg_16 = result_peg_8.getPosition();
                                {
                                
                                    result_peg_8.setValue(Value((void*) ";"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_8.getPosition()))){
                                                result_peg_8.nextPosition();
                                            } else {
                                                result_peg_8.setPosition(position_peg_16);
                                                goto out_peg_18;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_8.reset();
                                        do{
                                            Result result_peg_20(result_peg_8.getPosition());
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
                                                        result_peg_20.setValue(Value((void*) (long) temp_peg_24));
                                                        result_peg_20.nextPosition();
                                                    } else {
                                                        goto loop_peg_19;
                                                    }
                                                
                                                
                                            }
                                            result_peg_8.addResult(result_peg_20);
                                        } while (true);
                                        loop_peg_19:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_15;
                            out_peg_18:
                            {
                                int position_peg_25 = result_peg_8.getPosition();
                                {
                                
                                    result_peg_8.setValue(Value((void*) "="));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_8.getPosition()))){
                                                result_peg_8.nextPosition();
                                            } else {
                                                result_peg_8.setPosition(position_peg_25);
                                                goto out_peg_27;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_8.reset();
                                        do{
                                            Result result_peg_29(result_peg_8.getPosition());
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
                                                        result_peg_29.setValue(Value((void*) (long) temp_peg_33));
                                                        result_peg_29.nextPosition();
                                                    } else {
                                                        goto loop_peg_28;
                                                    }
                                                
                                                
                                            }
                                            result_peg_8.addResult(result_peg_29);
                                        } while (true);
                                        loop_peg_28:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_15;
                            out_peg_27:
                            {
                                int position_peg_34 = result_peg_8.getPosition();
                                {
                                
                                    result_peg_8.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_8.getPosition()))){
                                                result_peg_8.nextPosition();
                                            } else {
                                                result_peg_8.setPosition(position_peg_34);
                                                goto out_peg_36;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_8.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_8.getPosition()))){
                                                result_peg_8.nextPosition();
                                            } else {
                                                result_peg_8.setPosition(position_peg_34);
                                                goto out_peg_36;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_8.reset();
                                        do{
                                            Result result_peg_39(result_peg_8.getPosition());
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
                                                        result_peg_39.setValue(Value((void*) (long) temp_peg_48));
                                                        result_peg_39.nextPosition();
                                                    } else {
                                                        goto loop_peg_38;
                                                    }
                                                
                                                
                                            }
                                            result_peg_8.addResult(result_peg_39);
                                        } while (true);
                                        loop_peg_38:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_15;
                            out_peg_36:
                            result_peg_8.setPosition(position_peg_14);
                            goto out_peg_49;
                            success_peg_15:
                            ;
                            
                        }
                        goto success_peg_9;
                        out_peg_49:
                        goto loop_peg_7;
                        success_peg_9:
                        ;
                        result_peg_2.addResult(result_peg_8);
                    } while (true);
                    loop_peg_7:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_50;
                success_peg_5:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_53(result_peg_2.getPosition());
                    {
                        int position_peg_55 = result_peg_53.getPosition();
                        
                        result_peg_53.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_53.getPosition()))){
                                result_peg_53.nextPosition();
                            } else {
                                result_peg_53.setPosition(position_peg_55);
                                goto out_peg_56;
                            }
                        }
                            
                    }
                    goto success_peg_54;
                    out_peg_56:
                    {
                        int position_peg_57 = result_peg_53.getPosition();
                        
                        result_peg_53.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_53.getPosition()))){
                                result_peg_53.nextPosition();
                            } else {
                                result_peg_53.setPosition(position_peg_57);
                                goto out_peg_58;
                            }
                        }
                            
                    }
                    goto success_peg_54;
                    out_peg_58:
                    goto loop_peg_52;
                    success_peg_54:
                    ;
                    result_peg_2.addResult(result_peg_53);
                } while (true);
                loop_peg_52:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_61(result_peg_2.getPosition());
                    {
                    
                        result_peg_61 = rule_line(stream, result_peg_61.getPosition(), current);
                            if (result_peg_61.error()){
                                goto loop_peg_60;
                            }
                        
                        
                        
                        {
                                int position_peg_65 = result_peg_61.getPosition();
                                
                                result_peg_61.reset();
                                do{
                                    Result result_peg_67(result_peg_61.getPosition());
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
                                        
                                        {
                                            int position_peg_75 = result_peg_67.getPosition();
                                            {
                                            
                                                result_peg_67.setValue(Value((void*) ";"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_67.getPosition()))){
                                                            result_peg_67.nextPosition();
                                                        } else {
                                                            result_peg_67.setPosition(position_peg_75);
                                                            goto out_peg_77;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_67.reset();
                                                    do{
                                                        Result result_peg_79(result_peg_67.getPosition());
                                                        {
                                                        
                                                            Result result_peg_82(result_peg_79);
                                                                result_peg_82.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_82.getPosition()))){
                                                                        result_peg_82.nextPosition();
                                                                    } else {
                                                                        goto not_peg_81;
                                                                    }
                                                                }
                                                                goto loop_peg_78;
                                                                not_peg_81:
                                                                result_peg_79.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_83 = stream.get(result_peg_79.getPosition());
                                                                if (temp_peg_83 != '\0'){
                                                                    result_peg_79.setValue(Value((void*) (long) temp_peg_83));
                                                                    result_peg_79.nextPosition();
                                                                } else {
                                                                    goto loop_peg_78;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_67.addResult(result_peg_79);
                                                    } while (true);
                                                    loop_peg_78:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_74;
                                        out_peg_77:
                                        {
                                            int position_peg_84 = result_peg_67.getPosition();
                                            {
                                            
                                                result_peg_67.setValue(Value((void*) "="));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_67.getPosition()))){
                                                            result_peg_67.nextPosition();
                                                        } else {
                                                            result_peg_67.setPosition(position_peg_84);
                                                            goto out_peg_86;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_67.reset();
                                                    do{
                                                        Result result_peg_88(result_peg_67.getPosition());
                                                        {
                                                        
                                                            Result result_peg_91(result_peg_88);
                                                                result_peg_91.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_91.getPosition()))){
                                                                        result_peg_91.nextPosition();
                                                                    } else {
                                                                        goto not_peg_90;
                                                                    }
                                                                }
                                                                goto loop_peg_87;
                                                                not_peg_90:
                                                                result_peg_88.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_92 = stream.get(result_peg_88.getPosition());
                                                                if (temp_peg_92 != '\0'){
                                                                    result_peg_88.setValue(Value((void*) (long) temp_peg_92));
                                                                    result_peg_88.nextPosition();
                                                                } else {
                                                                    goto loop_peg_87;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_67.addResult(result_peg_88);
                                                    } while (true);
                                                    loop_peg_87:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_74;
                                        out_peg_86:
                                        {
                                            int position_peg_93 = result_peg_67.getPosition();
                                            {
                                            
                                                result_peg_67.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_67.getPosition()))){
                                                            result_peg_67.nextPosition();
                                                        } else {
                                                            result_peg_67.setPosition(position_peg_93);
                                                            goto out_peg_95;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_67.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_67.getPosition()))){
                                                            result_peg_67.nextPosition();
                                                        } else {
                                                            result_peg_67.setPosition(position_peg_93);
                                                            goto out_peg_95;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_67.reset();
                                                    do{
                                                        Result result_peg_98(result_peg_67.getPosition());
                                                        {
                                                        
                                                            Result result_peg_101(result_peg_98);
                                                                {
                                                                    int position_peg_103 = result_peg_101.getPosition();
                                                                    
                                                                    result_peg_101.setValue(Value((void*) "\n"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\n"[i], stream.get(result_peg_101.getPosition()))){
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
                                                                    
                                                                    result_peg_101.setValue(Value((void*) "\r"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\r"[i], stream.get(result_peg_101.getPosition()))){
                                                                            result_peg_101.nextPosition();
                                                                        } else {
                                                                            result_peg_101.setPosition(position_peg_105);
                                                                            goto out_peg_106;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_102;
                                                                out_peg_106:
                                                                goto not_peg_100;
                                                                success_peg_102:
                                                                ;
                                                                goto loop_peg_97;
                                                                not_peg_100:
                                                                result_peg_98.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_107 = stream.get(result_peg_98.getPosition());
                                                                if (temp_peg_107 != '\0'){
                                                                    result_peg_98.setValue(Value((void*) (long) temp_peg_107));
                                                                    result_peg_98.nextPosition();
                                                                } else {
                                                                    goto loop_peg_97;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_67.addResult(result_peg_98);
                                                    } while (true);
                                                    loop_peg_97:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_74;
                                        out_peg_95:
                                        result_peg_67.setPosition(position_peg_73);
                                        goto out_peg_108;
                                        success_peg_74:
                                        ;
                                        
                                    }
                                    goto success_peg_68;
                                    out_peg_108:
                                    goto loop_peg_66;
                                    success_peg_68:
                                    ;
                                    result_peg_61.addResult(result_peg_67);
                                } while (true);
                                loop_peg_66:
                                ;
                                        
                            }
                            goto success_peg_64;
                            goto loop_peg_60;
                            success_peg_64:
                            ;
                        
                        
                        
                        int save_peg_109 = result_peg_61.getPosition();
                            
                            result_peg_61 = rule_line_end(stream, result_peg_61.getPosition());
                            if (result_peg_61.error()){
                                
                                result_peg_61 = Result(save_peg_109);
                                result_peg_61.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_61);
                } while (true);
                loop_peg_60:
                ;
            
            
            
            if ('\0' == stream.get(result_peg_2.getPosition())){
                    result_peg_2.nextPosition();
                    result_peg_2.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_50;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = current; GC::cleanup(as<SectionList*>(value));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_50:
    stream.update(errorResult.getPosition());
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    
    RuleTrace trace_peg_70(stream, "line");
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
                        goto loop_peg_6;
                        success_peg_8:
                        ;
                        result_peg_2.addResult(result_peg_7);
                    } while (true);
                    loop_peg_6:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_13;
                success_peg_4:
                ;
            
            
            
            {
                    int position_peg_15 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_15);
                                    goto out_peg_17;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_19(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_22(result_peg_19);
                                        result_peg_22.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_22.getPosition()))){
                                                result_peg_22.nextPosition();
                                            } else {
                                                goto not_peg_21;
                                            }
                                        }
                                        goto loop_peg_18;
                                        not_peg_21:
                                        result_peg_19.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_23 = stream.get(result_peg_19.getPosition());
                                        if (temp_peg_23 != '\0'){
                                            result_peg_19.setValue(Value((void*) (long) temp_peg_23));
                                            result_peg_19.nextPosition();
                                        } else {
                                            goto loop_peg_18;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_19);
                            } while (true);
                            loop_peg_18:
                            ;
                        
                        
                    }
                }
                goto success_peg_14;
                out_peg_17:
                {
                    int position_peg_24 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_24);
                                    goto out_peg_26;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_28(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_31(result_peg_28);
                                        result_peg_31.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_31.getPosition()))){
                                                result_peg_31.nextPosition();
                                            } else {
                                                goto not_peg_30;
                                            }
                                        }
                                        goto loop_peg_27;
                                        not_peg_30:
                                        result_peg_28.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_32 = stream.get(result_peg_28.getPosition());
                                        if (temp_peg_32 != '\0'){
                                            result_peg_28.setValue(Value((void*) (long) temp_peg_32));
                                            result_peg_28.nextPosition();
                                        } else {
                                            goto loop_peg_27;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_28);
                            } while (true);
                            loop_peg_27:
                            ;
                        
                        
                    }
                }
                goto success_peg_14;
                out_peg_26:
                {
                    int position_peg_33 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_33);
                                    goto out_peg_35;
                                }
                            }
                        
                        
                        
                        result_peg_2.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_33);
                                    goto out_peg_35;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_38(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_41(result_peg_38);
                                        {
                                            int position_peg_43 = result_peg_41.getPosition();
                                            
                                            result_peg_41.setValue(Value((void*) "\n"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    result_peg_41.setPosition(position_peg_43);
                                                    goto out_peg_44;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_42;
                                        out_peg_44:
                                        {
                                            int position_peg_45 = result_peg_41.getPosition();
                                            
                                            result_peg_41.setValue(Value((void*) "\r"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    result_peg_41.setPosition(position_peg_45);
                                                    goto out_peg_46;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_42;
                                        out_peg_46:
                                        goto not_peg_40;
                                        success_peg_42:
                                        ;
                                        goto loop_peg_37;
                                        not_peg_40:
                                        result_peg_38.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_47 = stream.get(result_peg_38.getPosition());
                                        if (temp_peg_47 != '\0'){
                                            result_peg_38.setValue(Value((void*) (long) temp_peg_47));
                                            result_peg_38.nextPosition();
                                        } else {
                                            goto loop_peg_37;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_38);
                            } while (true);
                            loop_peg_37:
                            ;
                        
                        
                    }
                }
                goto success_peg_14;
                out_peg_35:
                goto out_peg_13;
                success_peg_14:
                ;
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_13:
        Result result_peg_48(myposition);
        
        
        {
        
            {
                    int position_peg_51 = result_peg_48.getPosition();
                    
                    result_peg_48.reset();
                    do{
                        Result result_peg_53(result_peg_48.getPosition());
                        {
                            int position_peg_55 = result_peg_53.getPosition();
                            
                            result_peg_53.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_53.getPosition()))){
                                    result_peg_53.nextPosition();
                                } else {
                                    result_peg_53.setPosition(position_peg_55);
                                    goto out_peg_56;
                                }
                            }
                                
                        }
                        goto success_peg_54;
                        out_peg_56:
                        {
                            int position_peg_57 = result_peg_53.getPosition();
                            
                            result_peg_53.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_53.getPosition()))){
                                    result_peg_53.nextPosition();
                                } else {
                                    result_peg_53.setPosition(position_peg_57);
                                    goto out_peg_58;
                                }
                            }
                                
                        }
                        goto success_peg_54;
                        out_peg_58:
                        goto loop_peg_52;
                        success_peg_54:
                        ;
                        result_peg_48.addResult(result_peg_53);
                    } while (true);
                    loop_peg_52:
                    ;
                            
                }
                goto success_peg_50;
                goto out_peg_59;
                success_peg_50:
                ;
            
            
            
            result_peg_48 = rule_section(stream, result_peg_48.getPosition());
                if (result_peg_48.error()){
                    goto out_peg_59;
                }
            
            Result result_peg_60 = result_peg_48;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_60.getValues());
                    result_peg_48.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_48.getPosition());
        
        return result_peg_48;
        out_peg_59:
        Result result_peg_61(myposition);
        
        
        result_peg_61.reset();
        do{
            Result result_peg_63(result_peg_61.getPosition());
            {
                int position_peg_65 = result_peg_63.getPosition();
                
                result_peg_63.setValue(Value((void*) " "));
                for (int i = 0; i < 1; i++){
                    if (compareChar(" "[i], stream.get(result_peg_63.getPosition()))){
                        result_peg_63.nextPosition();
                    } else {
                        result_peg_63.setPosition(position_peg_65);
                        goto out_peg_66;
                    }
                }
                    
            }
            goto success_peg_64;
            out_peg_66:
            {
                int position_peg_67 = result_peg_63.getPosition();
                
                result_peg_63.setValue(Value((void*) "\t"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\t"[i], stream.get(result_peg_63.getPosition()))){
                        result_peg_63.nextPosition();
                    } else {
                        result_peg_63.setPosition(position_peg_67);
                        goto out_peg_68;
                    }
                }
                    
            }
            goto success_peg_64;
            out_peg_68:
            goto loop_peg_62;
            success_peg_64:
            ;
            result_peg_61.addResult(result_peg_63);
        } while (true);
        loop_peg_62:
        if (result_peg_61.matches() == 0){
            goto out_peg_69;
        }
        
        stream.update(result_peg_61.getPosition());
        
        return result_peg_61;
        out_peg_69:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    
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
        
        stream.update(result_peg_11.getPosition());
        
        return result_peg_11;
        out_peg_14:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_112(stream, "section");
    int myposition = position;
    
    
    Value line;
        Value name;
        Value ast;
    Result result_peg_2(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_4 = stream.getLineInfo(result_peg_2.getPosition());
                line = &line_info_peg_4;
            
            
            
            result_peg_2 = rule_section_start(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_6;
                }
                name = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSection(name, getCurrentLine(line), getCurrentColumn(line));
                    result_peg_2.setValue(value);
                }
                ast = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_10 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_12(result_peg_2.getPosition());
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
                            
                            {
                                int position_peg_20 = result_peg_12.getPosition();
                                {
                                
                                    result_peg_12.setValue(Value((void*) ";"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_12.getPosition()))){
                                                result_peg_12.nextPosition();
                                            } else {
                                                result_peg_12.setPosition(position_peg_20);
                                                goto out_peg_22;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_12.reset();
                                        do{
                                            Result result_peg_24(result_peg_12.getPosition());
                                            {
                                            
                                                Result result_peg_27(result_peg_24);
                                                    result_peg_27.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_27.getPosition()))){
                                                            result_peg_27.nextPosition();
                                                        } else {
                                                            goto not_peg_26;
                                                        }
                                                    }
                                                    goto loop_peg_23;
                                                    not_peg_26:
                                                    result_peg_24.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_28 = stream.get(result_peg_24.getPosition());
                                                    if (temp_peg_28 != '\0'){
                                                        result_peg_24.setValue(Value((void*) (long) temp_peg_28));
                                                        result_peg_24.nextPosition();
                                                    } else {
                                                        goto loop_peg_23;
                                                    }
                                                
                                                
                                            }
                                            result_peg_12.addResult(result_peg_24);
                                        } while (true);
                                        loop_peg_23:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_19;
                            out_peg_22:
                            {
                                int position_peg_29 = result_peg_12.getPosition();
                                {
                                
                                    result_peg_12.setValue(Value((void*) "="));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_12.getPosition()))){
                                                result_peg_12.nextPosition();
                                            } else {
                                                result_peg_12.setPosition(position_peg_29);
                                                goto out_peg_31;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_12.reset();
                                        do{
                                            Result result_peg_33(result_peg_12.getPosition());
                                            {
                                            
                                                Result result_peg_36(result_peg_33);
                                                    result_peg_36.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_36.getPosition()))){
                                                            result_peg_36.nextPosition();
                                                        } else {
                                                            goto not_peg_35;
                                                        }
                                                    }
                                                    goto loop_peg_32;
                                                    not_peg_35:
                                                    result_peg_33.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_37 = stream.get(result_peg_33.getPosition());
                                                    if (temp_peg_37 != '\0'){
                                                        result_peg_33.setValue(Value((void*) (long) temp_peg_37));
                                                        result_peg_33.nextPosition();
                                                    } else {
                                                        goto loop_peg_32;
                                                    }
                                                
                                                
                                            }
                                            result_peg_12.addResult(result_peg_33);
                                        } while (true);
                                        loop_peg_32:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_19;
                            out_peg_31:
                            {
                                int position_peg_38 = result_peg_12.getPosition();
                                {
                                
                                    result_peg_12.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_12.getPosition()))){
                                                result_peg_12.nextPosition();
                                            } else {
                                                result_peg_12.setPosition(position_peg_38);
                                                goto out_peg_40;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_12.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_12.getPosition()))){
                                                result_peg_12.nextPosition();
                                            } else {
                                                result_peg_12.setPosition(position_peg_38);
                                                goto out_peg_40;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_12.reset();
                                        do{
                                            Result result_peg_43(result_peg_12.getPosition());
                                            {
                                            
                                                Result result_peg_46(result_peg_43);
                                                    {
                                                        int position_peg_48 = result_peg_46.getPosition();
                                                        
                                                        result_peg_46.setValue(Value((void*) "\n"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("\n"[i], stream.get(result_peg_46.getPosition()))){
                                                                result_peg_46.nextPosition();
                                                            } else {
                                                                result_peg_46.setPosition(position_peg_48);
                                                                goto out_peg_49;
                                                            }
                                                        }
                                                            
                                                    }
                                                    goto success_peg_47;
                                                    out_peg_49:
                                                    {
                                                        int position_peg_50 = result_peg_46.getPosition();
                                                        
                                                        result_peg_46.setValue(Value((void*) "\r"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("\r"[i], stream.get(result_peg_46.getPosition()))){
                                                                result_peg_46.nextPosition();
                                                            } else {
                                                                result_peg_46.setPosition(position_peg_50);
                                                                goto out_peg_51;
                                                            }
                                                        }
                                                            
                                                    }
                                                    goto success_peg_47;
                                                    out_peg_51:
                                                    goto not_peg_45;
                                                    success_peg_47:
                                                    ;
                                                    goto loop_peg_42;
                                                    not_peg_45:
                                                    result_peg_43.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_52 = stream.get(result_peg_43.getPosition());
                                                    if (temp_peg_52 != '\0'){
                                                        result_peg_43.setValue(Value((void*) (long) temp_peg_52));
                                                        result_peg_43.nextPosition();
                                                    } else {
                                                        goto loop_peg_42;
                                                    }
                                                
                                                
                                            }
                                            result_peg_12.addResult(result_peg_43);
                                        } while (true);
                                        loop_peg_42:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_19;
                            out_peg_40:
                            result_peg_12.setPosition(position_peg_18);
                            goto out_peg_53;
                            success_peg_19:
                            ;
                            
                        }
                        goto success_peg_13;
                        out_peg_53:
                        goto loop_peg_11;
                        success_peg_13:
                        ;
                        result_peg_2.addResult(result_peg_12);
                    } while (true);
                    loop_peg_11:
                    ;
                            
                }
                goto success_peg_9;
                goto out_peg_6;
                success_peg_9:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_56(result_peg_2.getPosition());
                    {
                        int position_peg_58 = result_peg_56.getPosition();
                        
                        result_peg_56.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_56.getPosition()))){
                                result_peg_56.nextPosition();
                            } else {
                                result_peg_56.setPosition(position_peg_58);
                                goto out_peg_59;
                            }
                        }
                            
                    }
                    goto success_peg_57;
                    out_peg_59:
                    {
                        int position_peg_60 = result_peg_56.getPosition();
                        
                        result_peg_56.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_56.getPosition()))){
                                result_peg_56.nextPosition();
                            } else {
                                result_peg_56.setPosition(position_peg_60);
                                goto out_peg_61;
                            }
                        }
                            
                    }
                    goto success_peg_57;
                    out_peg_61:
                    goto loop_peg_55;
                    success_peg_57:
                    ;
                    result_peg_2.addResult(result_peg_56);
                } while (true);
                loop_peg_55:
                if (result_peg_2.matches() == 0){
                    goto out_peg_6;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_64(result_peg_2.getPosition());
                    {
                    
                        result_peg_64 = rule_section_line(stream, result_peg_64.getPosition(), ast);
                            if (result_peg_64.error()){
                                goto loop_peg_63;
                            }
                        
                        
                        
                        {
                                int position_peg_68 = result_peg_64.getPosition();
                                
                                result_peg_64.reset();
                                do{
                                    Result result_peg_70(result_peg_64.getPosition());
                                    {
                                        int position_peg_72 = result_peg_70.getPosition();
                                        
                                        result_peg_70.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_70.getPosition()))){
                                                result_peg_70.nextPosition();
                                            } else {
                                                result_peg_70.setPosition(position_peg_72);
                                                goto out_peg_73;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_71;
                                    out_peg_73:
                                    {
                                        int position_peg_74 = result_peg_70.getPosition();
                                        
                                        result_peg_70.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_70.getPosition()))){
                                                result_peg_70.nextPosition();
                                            } else {
                                                result_peg_70.setPosition(position_peg_74);
                                                goto out_peg_75;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_71;
                                    out_peg_75:
                                    {
                                        int position_peg_76 = result_peg_70.getPosition();
                                        
                                        {
                                            int position_peg_78 = result_peg_70.getPosition();
                                            {
                                            
                                                result_peg_70.setValue(Value((void*) ";"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_70.getPosition()))){
                                                            result_peg_70.nextPosition();
                                                        } else {
                                                            result_peg_70.setPosition(position_peg_78);
                                                            goto out_peg_80;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_70.reset();
                                                    do{
                                                        Result result_peg_82(result_peg_70.getPosition());
                                                        {
                                                        
                                                            Result result_peg_85(result_peg_82);
                                                                result_peg_85.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_85.getPosition()))){
                                                                        result_peg_85.nextPosition();
                                                                    } else {
                                                                        goto not_peg_84;
                                                                    }
                                                                }
                                                                goto loop_peg_81;
                                                                not_peg_84:
                                                                result_peg_82.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_86 = stream.get(result_peg_82.getPosition());
                                                                if (temp_peg_86 != '\0'){
                                                                    result_peg_82.setValue(Value((void*) (long) temp_peg_86));
                                                                    result_peg_82.nextPosition();
                                                                } else {
                                                                    goto loop_peg_81;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_70.addResult(result_peg_82);
                                                    } while (true);
                                                    loop_peg_81:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_77;
                                        out_peg_80:
                                        {
                                            int position_peg_87 = result_peg_70.getPosition();
                                            {
                                            
                                                result_peg_70.setValue(Value((void*) "="));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_70.getPosition()))){
                                                            result_peg_70.nextPosition();
                                                        } else {
                                                            result_peg_70.setPosition(position_peg_87);
                                                            goto out_peg_89;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_70.reset();
                                                    do{
                                                        Result result_peg_91(result_peg_70.getPosition());
                                                        {
                                                        
                                                            Result result_peg_94(result_peg_91);
                                                                result_peg_94.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_94.getPosition()))){
                                                                        result_peg_94.nextPosition();
                                                                    } else {
                                                                        goto not_peg_93;
                                                                    }
                                                                }
                                                                goto loop_peg_90;
                                                                not_peg_93:
                                                                result_peg_91.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_95 = stream.get(result_peg_91.getPosition());
                                                                if (temp_peg_95 != '\0'){
                                                                    result_peg_91.setValue(Value((void*) (long) temp_peg_95));
                                                                    result_peg_91.nextPosition();
                                                                } else {
                                                                    goto loop_peg_90;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_70.addResult(result_peg_91);
                                                    } while (true);
                                                    loop_peg_90:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_77;
                                        out_peg_89:
                                        {
                                            int position_peg_96 = result_peg_70.getPosition();
                                            {
                                            
                                                result_peg_70.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_70.getPosition()))){
                                                            result_peg_70.nextPosition();
                                                        } else {
                                                            result_peg_70.setPosition(position_peg_96);
                                                            goto out_peg_98;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_70.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_70.getPosition()))){
                                                            result_peg_70.nextPosition();
                                                        } else {
                                                            result_peg_70.setPosition(position_peg_96);
                                                            goto out_peg_98;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_70.reset();
                                                    do{
                                                        Result result_peg_101(result_peg_70.getPosition());
                                                        {
                                                        
                                                            Result result_peg_104(result_peg_101);
                                                                {
                                                                    int position_peg_106 = result_peg_104.getPosition();
                                                                    
                                                                    result_peg_104.setValue(Value((void*) "\n"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\n"[i], stream.get(result_peg_104.getPosition()))){
                                                                            result_peg_104.nextPosition();
                                                                        } else {
                                                                            result_peg_104.setPosition(position_peg_106);
                                                                            goto out_peg_107;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_105;
                                                                out_peg_107:
                                                                {
                                                                    int position_peg_108 = result_peg_104.getPosition();
                                                                    
                                                                    result_peg_104.setValue(Value((void*) "\r"));
                                                                    for (int i = 0; i < 1; i++){
                                                                        if (compareChar("\r"[i], stream.get(result_peg_104.getPosition()))){
                                                                            result_peg_104.nextPosition();
                                                                        } else {
                                                                            result_peg_104.setPosition(position_peg_108);
                                                                            goto out_peg_109;
                                                                        }
                                                                    }
                                                                        
                                                                }
                                                                goto success_peg_105;
                                                                out_peg_109:
                                                                goto not_peg_103;
                                                                success_peg_105:
                                                                ;
                                                                goto loop_peg_100;
                                                                not_peg_103:
                                                                result_peg_101.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_110 = stream.get(result_peg_101.getPosition());
                                                                if (temp_peg_110 != '\0'){
                                                                    result_peg_101.setValue(Value((void*) (long) temp_peg_110));
                                                                    result_peg_101.nextPosition();
                                                                } else {
                                                                    goto loop_peg_100;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_70.addResult(result_peg_101);
                                                    } while (true);
                                                    loop_peg_100:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_77;
                                        out_peg_98:
                                        result_peg_70.setPosition(position_peg_76);
                                        goto out_peg_111;
                                        success_peg_77:
                                        ;
                                        
                                    }
                                    goto success_peg_71;
                                    out_peg_111:
                                    goto loop_peg_69;
                                    success_peg_71:
                                    ;
                                    result_peg_64.addResult(result_peg_70);
                                } while (true);
                                loop_peg_69:
                                ;
                                        
                            }
                            goto success_peg_67;
                            goto loop_peg_63;
                            success_peg_67:
                            ;
                        
                        
                        
                        result_peg_64 = rule_line_end(stream, result_peg_64.getPosition());
                            if (result_peg_64.error()){
                                goto loop_peg_63;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_64);
                } while (true);
                loop_peg_63:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_6:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    
    RuleTrace trace_peg_103(stream, "section_line");
    int myposition = position;
    
    
    Value data;
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
                        goto loop_peg_6;
                        success_peg_8:
                        ;
                        result_peg_2.addResult(result_peg_7);
                    } while (true);
                    loop_peg_6:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_13;
                success_peg_4:
                ;
            
            
            
            {
                    int position_peg_15 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_15);
                                    goto out_peg_17;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_19(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_22(result_peg_19);
                                        result_peg_22.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_22.getPosition()))){
                                                result_peg_22.nextPosition();
                                            } else {
                                                goto not_peg_21;
                                            }
                                        }
                                        goto loop_peg_18;
                                        not_peg_21:
                                        result_peg_19.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_23 = stream.get(result_peg_19.getPosition());
                                        if (temp_peg_23 != '\0'){
                                            result_peg_19.setValue(Value((void*) (long) temp_peg_23));
                                            result_peg_19.nextPosition();
                                        } else {
                                            goto loop_peg_18;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_19);
                            } while (true);
                            loop_peg_18:
                            ;
                        
                        
                    }
                }
                goto success_peg_14;
                out_peg_17:
                {
                    int position_peg_24 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_24);
                                    goto out_peg_26;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_28(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_31(result_peg_28);
                                        result_peg_31.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_31.getPosition()))){
                                                result_peg_31.nextPosition();
                                            } else {
                                                goto not_peg_30;
                                            }
                                        }
                                        goto loop_peg_27;
                                        not_peg_30:
                                        result_peg_28.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_32 = stream.get(result_peg_28.getPosition());
                                        if (temp_peg_32 != '\0'){
                                            result_peg_28.setValue(Value((void*) (long) temp_peg_32));
                                            result_peg_28.nextPosition();
                                        } else {
                                            goto loop_peg_27;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_28);
                            } while (true);
                            loop_peg_27:
                            ;
                        
                        
                    }
                }
                goto success_peg_14;
                out_peg_26:
                {
                    int position_peg_33 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_33);
                                    goto out_peg_35;
                                }
                            }
                        
                        
                        
                        result_peg_2.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_33);
                                    goto out_peg_35;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_38(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_41(result_peg_38);
                                        {
                                            int position_peg_43 = result_peg_41.getPosition();
                                            
                                            result_peg_41.setValue(Value((void*) "\n"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    result_peg_41.setPosition(position_peg_43);
                                                    goto out_peg_44;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_42;
                                        out_peg_44:
                                        {
                                            int position_peg_45 = result_peg_41.getPosition();
                                            
                                            result_peg_41.setValue(Value((void*) "\r"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_41.getPosition()))){
                                                    result_peg_41.nextPosition();
                                                } else {
                                                    result_peg_41.setPosition(position_peg_45);
                                                    goto out_peg_46;
                                                }
                                            }
                                                
                                        }
                                        goto success_peg_42;
                                        out_peg_46:
                                        goto not_peg_40;
                                        success_peg_42:
                                        ;
                                        goto loop_peg_37;
                                        not_peg_40:
                                        result_peg_38.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_47 = stream.get(result_peg_38.getPosition());
                                        if (temp_peg_47 != '\0'){
                                            result_peg_38.setValue(Value((void*) (long) temp_peg_47));
                                            result_peg_38.nextPosition();
                                        } else {
                                            goto loop_peg_37;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_38);
                            } while (true);
                            loop_peg_37:
                            ;
                        
                        
                    }
                }
                goto success_peg_14;
                out_peg_35:
                goto out_peg_13;
                success_peg_14:
                ;
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_13:
        Result result_peg_48(myposition);
        
        
        {
        
            {
                    int position_peg_51 = result_peg_48.getPosition();
                    
                    result_peg_48.reset();
                    do{
                        Result result_peg_53(result_peg_48.getPosition());
                        {
                            int position_peg_55 = result_peg_53.getPosition();
                            
                            result_peg_53.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_53.getPosition()))){
                                    result_peg_53.nextPosition();
                                } else {
                                    result_peg_53.setPosition(position_peg_55);
                                    goto out_peg_56;
                                }
                            }
                                
                        }
                        goto success_peg_54;
                        out_peg_56:
                        {
                            int position_peg_57 = result_peg_53.getPosition();
                            
                            result_peg_53.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_53.getPosition()))){
                                    result_peg_53.nextPosition();
                                } else {
                                    result_peg_53.setPosition(position_peg_57);
                                    goto out_peg_58;
                                }
                            }
                                
                        }
                        goto success_peg_54;
                        out_peg_58:
                        goto loop_peg_52;
                        success_peg_54:
                        ;
                        result_peg_48.addResult(result_peg_53);
                    } while (true);
                    loop_peg_52:
                    ;
                            
                }
                goto success_peg_50;
                goto out_peg_59;
                success_peg_50:
                ;
            
            
            
            result_peg_48 = rule_attribute(stream, result_peg_48.getPosition());
                if (result_peg_48.error()){
                    goto out_peg_59;
                }
                data = result_peg_48.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addAttribute(asAttribute(data));
                    result_peg_48.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_48.getPosition());
        
        return result_peg_48;
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
                        goto loop_peg_65;
                        success_peg_67:
                        ;
                        result_peg_61.addResult(result_peg_66);
                    } while (true);
                    loop_peg_65:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_72;
                success_peg_63:
                ;
            
            
            
            {
                    int position_peg_75 = result_peg_61.getPosition();
                    {
                    
                        result_peg_61.setValue(Value((void*) "loopstart"));
                            for (int i = 0; i < 9; i++){
                                if (compareCharCase("loopstart"[i], stream.get(result_peg_61.getPosition()))){
                                    result_peg_61.nextPosition();
                                } else {
                                    result_peg_61.setPosition(position_peg_75);
                                    goto out_peg_77;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = makeValue();
                                result_peg_61.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_74;
                out_peg_77:
                goto out_peg_72;
                success_peg_74:
                ;
                data = result_peg_61.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_61.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_61.getPosition());
        
        return result_peg_61;
        out_peg_72:
        Result result_peg_78(myposition);
        
        
        {
        
            {
                    int position_peg_81 = result_peg_78.getPosition();
                    
                    result_peg_78.reset();
                    do{
                        Result result_peg_83(result_peg_78.getPosition());
                        {
                            int position_peg_85 = result_peg_83.getPosition();
                            
                            result_peg_83.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_83.getPosition()))){
                                    result_peg_83.nextPosition();
                                } else {
                                    result_peg_83.setPosition(position_peg_85);
                                    goto out_peg_86;
                                }
                            }
                                
                        }
                        goto success_peg_84;
                        out_peg_86:
                        {
                            int position_peg_87 = result_peg_83.getPosition();
                            
                            result_peg_83.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_83.getPosition()))){
                                    result_peg_83.nextPosition();
                                } else {
                                    result_peg_83.setPosition(position_peg_87);
                                    goto out_peg_88;
                                }
                            }
                                
                        }
                        goto success_peg_84;
                        out_peg_88:
                        goto loop_peg_82;
                        success_peg_84:
                        ;
                        result_peg_78.addResult(result_peg_83);
                    } while (true);
                    loop_peg_82:
                    ;
                            
                }
                goto success_peg_80;
                goto out_peg_89;
                success_peg_80:
                ;
            
            
            
            result_peg_78 = rule_valuelist(stream, result_peg_78.getPosition());
                if (result_peg_78.error()){
                    goto out_peg_89;
                }
                data = result_peg_78.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_78.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_78.getPosition());
        
        return result_peg_78;
        out_peg_89:
        Result result_peg_91(myposition);
        
        
        {
        
            result_peg_91.reset();
                do{
                    Result result_peg_94(result_peg_91.getPosition());
                    {
                        int position_peg_96 = result_peg_94.getPosition();
                        
                        result_peg_94.setValue(Value((void*) " "));
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_94.getPosition()))){
                                result_peg_94.nextPosition();
                            } else {
                                result_peg_94.setPosition(position_peg_96);
                                goto out_peg_97;
                            }
                        }
                            
                    }
                    goto success_peg_95;
                    out_peg_97:
                    {
                        int position_peg_98 = result_peg_94.getPosition();
                        
                        result_peg_94.setValue(Value((void*) "\t"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_94.getPosition()))){
                                result_peg_94.nextPosition();
                            } else {
                                result_peg_94.setPosition(position_peg_98);
                                goto out_peg_99;
                            }
                        }
                            
                    }
                    goto success_peg_95;
                    out_peg_99:
                    goto loop_peg_93;
                    success_peg_95:
                    ;
                    result_peg_91.addResult(result_peg_94);
                } while (true);
                loop_peg_93:
                if (result_peg_91.matches() == 0){
                    goto out_peg_100;
                }
            
            
            
            Result result_peg_102(result_peg_91);
                result_peg_102.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_102.getPosition()))){
                        result_peg_102.nextPosition();
                    } else {
                        goto not_peg_101;
                    }
                }
                goto out_peg_100;
                not_peg_101:
                result_peg_91.setValue(Value((void*)0));
            
            
        }
        
        stream.update(result_peg_91.getPosition());
        
        return result_peg_91;
        out_peg_100:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_33(stream, "section_start");
    int myposition = position;
    
    
    Value data;
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
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_17(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_20(result_peg_17);
                            result_peg_20.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    goto not_peg_19;
                                }
                            }
                            goto loop_peg_16;
                            not_peg_19:
                            result_peg_17.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_21 = stream.get(result_peg_17.getPosition());
                            if (temp_peg_21 != '\0'){
                                result_peg_17.setValue(Value((void*) (long) temp_peg_21));
                                result_peg_17.nextPosition();
                            } else {
                                goto loop_peg_16;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_17);
                } while (true);
                loop_peg_16:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
                data = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_24 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_26(result_peg_2.getPosition());
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
                        result_peg_2.addResult(result_peg_26);
                    } while (true);
                    loop_peg_25:
                    ;
                            
                }
                goto success_peg_23;
                goto out_peg_4;
                success_peg_23:
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
                    value = toString(data);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_25(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            {
                    int position_peg_5 = result_peg_2.getPosition();
                    
                    char letter_peg_6 = stream.get(result_peg_2.getPosition());
                    if (letter_peg_6 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_6) != NULL){
                        result_peg_2.nextPosition();
                        result_peg_2.setValue(Value((void*) (long) letter_peg_6));
                    } else {
                        result_peg_2.setPosition(position_peg_5);
                        goto out_peg_7;
                    }
                    
                }
                goto success_peg_4;
                out_peg_7:
                goto out_peg_8;
                success_peg_4:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_11(result_peg_2.getPosition());
                    {
                        int position_peg_13 = result_peg_11.getPosition();
                        
                        {
                            int position_peg_15 = result_peg_11.getPosition();
                            
                            char letter_peg_16 = stream.get(result_peg_11.getPosition());
                            if (letter_peg_16 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_16) != NULL){
                                result_peg_11.nextPosition();
                                result_peg_11.setValue(Value((void*) (long) letter_peg_16));
                            } else {
                                result_peg_11.setPosition(position_peg_15);
                                goto out_peg_17;
                            }
                            
                        }
                        goto success_peg_14;
                        out_peg_17:
                        result_peg_11.setPosition(position_peg_13);
                        goto out_peg_18;
                        success_peg_14:
                        ;
                        
                    }
                    goto success_peg_12;
                    out_peg_18:
                    {
                        int position_peg_19 = result_peg_11.getPosition();
                        
                        {
                            int position_peg_21 = result_peg_11.getPosition();
                            
                            char letter_peg_22 = stream.get(result_peg_11.getPosition());
                            if (letter_peg_22 != '\0' && strchr("0123456789", letter_peg_22) != NULL){
                                result_peg_11.nextPosition();
                                result_peg_11.setValue(Value((void*) (long) letter_peg_22));
                            } else {
                                result_peg_11.setPosition(position_peg_21);
                                goto out_peg_23;
                            }
                            
                        }
                        goto success_peg_20;
                        out_peg_23:
                        result_peg_11.setPosition(position_peg_19);
                        goto out_peg_24;
                        success_peg_20:
                        ;
                        
                    }
                    goto success_peg_12;
                    out_peg_24:
                    goto loop_peg_10;
                    success_peg_12:
                    ;
                    result_peg_2.addResult(result_peg_11);
                } while (true);
                loop_peg_10:
                ;
            
            Result result_peg_9 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_3.getValues().getValue(),result_peg_9.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_8:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_40(stream, "line_end_or_comment");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        result_peg_2 = rule_line_end(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        
        {
            int position_peg_6 = result_peg_4.getPosition();
            {
            
                result_peg_4.setValue(Value((void*) ";"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(";"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_6);
                            goto out_peg_8;
                        }
                    }
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_10(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_13(result_peg_10);
                                result_peg_13.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_13.getPosition()))){
                                        result_peg_13.nextPosition();
                                    } else {
                                        goto not_peg_12;
                                    }
                                }
                                goto loop_peg_9;
                                not_peg_12:
                                result_peg_10.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_14 = stream.get(result_peg_10.getPosition());
                                if (temp_peg_14 != '\0'){
                                    result_peg_10.setValue(Value((void*) (long) temp_peg_14));
                                    result_peg_10.nextPosition();
                                } else {
                                    goto loop_peg_9;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_10);
                    } while (true);
                    loop_peg_9:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_8:
        {
            int position_peg_15 = result_peg_4.getPosition();
            {
            
                result_peg_4.setValue(Value((void*) "="));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("="[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_15);
                            goto out_peg_17;
                        }
                    }
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_19(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_22(result_peg_19);
                                result_peg_22.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_22.getPosition()))){
                                        result_peg_22.nextPosition();
                                    } else {
                                        goto not_peg_21;
                                    }
                                }
                                goto loop_peg_18;
                                not_peg_21:
                                result_peg_19.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_23 = stream.get(result_peg_19.getPosition());
                                if (temp_peg_23 != '\0'){
                                    result_peg_19.setValue(Value((void*) (long) temp_peg_23));
                                    result_peg_19.nextPosition();
                                } else {
                                    goto loop_peg_18;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_19);
                    } while (true);
                    loop_peg_18:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_17:
        {
            int position_peg_24 = result_peg_4.getPosition();
            {
            
                result_peg_4.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_24);
                            goto out_peg_26;
                        }
                    }
                
                
                
                result_peg_4.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_24);
                            goto out_peg_26;
                        }
                    }
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_29(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_32(result_peg_29);
                                {
                                    int position_peg_34 = result_peg_32.getPosition();
                                    
                                    result_peg_32.setValue(Value((void*) "\n"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\n"[i], stream.get(result_peg_32.getPosition()))){
                                            result_peg_32.nextPosition();
                                        } else {
                                            result_peg_32.setPosition(position_peg_34);
                                            goto out_peg_35;
                                        }
                                    }
                                        
                                }
                                goto success_peg_33;
                                out_peg_35:
                                {
                                    int position_peg_36 = result_peg_32.getPosition();
                                    
                                    result_peg_32.setValue(Value((void*) "\r"));
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\r"[i], stream.get(result_peg_32.getPosition()))){
                                            result_peg_32.nextPosition();
                                        } else {
                                            result_peg_32.setPosition(position_peg_36);
                                            goto out_peg_37;
                                        }
                                    }
                                        
                                }
                                goto success_peg_33;
                                out_peg_37:
                                goto not_peg_31;
                                success_peg_33:
                                ;
                                goto loop_peg_28;
                                not_peg_31:
                                result_peg_29.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_38 = stream.get(result_peg_29.getPosition());
                                if (temp_peg_38 != '\0'){
                                    result_peg_29.setValue(Value((void*) (long) temp_peg_38));
                                    result_peg_29.nextPosition();
                                } else {
                                    goto loop_peg_28;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_29);
                    } while (true);
                    loop_peg_28:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_26:
        goto out_peg_39;
        success_peg_5:
        ;
        
        stream.update(result_peg_4.getPosition());
        
        return result_peg_4;
        out_peg_39:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_34(stream, "filename");
    int myposition = position;
    
    
    Value line;
        Value file;
    Result result_peg_2(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_4 = stream.getLineInfo(result_peg_2.getPosition());
                line = &line_info_peg_4;
            
            
            
            Result result_peg_7(result_peg_2);
                result_peg_7.setValue(Value((void*) "\""));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_7.getPosition()))){
                        result_peg_7.nextPosition();
                    } else {
                        goto not_peg_6;
                    }
                }
                goto out_peg_8;
                not_peg_6:
                result_peg_2.setValue(Value((void*)0));
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_11(result_peg_2.getPosition());
                    {
                        int position_peg_13 = result_peg_11.getPosition();
                        {
                        
                            Result result_peg_16(result_peg_11);
                                result_peg_16.setValue(Value((void*) ","));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(","[i], stream.get(result_peg_16.getPosition()))){
                                        result_peg_16.nextPosition();
                                    } else {
                                        goto not_peg_15;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_13);
                                goto out_peg_17;
                                not_peg_15:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_20(result_peg_11);
                                result_peg_20.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_20.getPosition()))){
                                        result_peg_20.nextPosition();
                                    } else {
                                        goto not_peg_19;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_13);
                                goto out_peg_17;
                                not_peg_19:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_23(result_peg_11);
                                result_peg_23.setValue(Value((void*) "["));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("["[i], stream.get(result_peg_23.getPosition()))){
                                        result_peg_23.nextPosition();
                                    } else {
                                        goto not_peg_22;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_13);
                                goto out_peg_17;
                                not_peg_22:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_26(result_peg_11);
                                result_peg_26.setValue(Value((void*) 13));
                                if ((unsigned char) stream.get(result_peg_26.getPosition()) == (unsigned char) 13){
                                    result_peg_26.nextPosition();
                                } else {
                                    goto not_peg_25;
                                }
                                result_peg_11.setPosition(position_peg_13);
                                goto out_peg_17;
                                not_peg_25:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_29(result_peg_11);
                                result_peg_29.setValue(Value((void*) "="));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("="[i], stream.get(result_peg_29.getPosition()))){
                                        result_peg_29.nextPosition();
                                    } else {
                                        goto not_peg_28;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_13);
                                goto out_peg_17;
                                not_peg_28:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_32(result_peg_11);
                                result_peg_32.setValue(Value((void*) ";"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(";"[i], stream.get(result_peg_32.getPosition()))){
                                        result_peg_32.nextPosition();
                                    } else {
                                        goto not_peg_31;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_13);
                                goto out_peg_17;
                                not_peg_31:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_33 = stream.get(result_peg_11.getPosition());
                                if (temp_peg_33 != '\0'){
                                    result_peg_11.setValue(Value((void*) (long) temp_peg_33));
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_13);
                                    goto out_peg_17;
                                }
                            
                            
                        }
                    }
                    goto success_peg_12;
                    out_peg_17:
                    goto loop_peg_10;
                    success_peg_12:
                    ;
                    result_peg_2.addResult(result_peg_11);
                } while (true);
                loop_peg_10:
                if (result_peg_2.matches() == 0){
                    goto out_peg_8;
                }
                file = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new Ast::Filename(getCurrentLine(line), getCurrentColumn(line), toString(file)); GC::save(as<Ast::Filename*>(value));
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_8:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_142(stream, "attribute");
    int myposition = position;
    
    
    Value line;
        Value id;
        Value data;
        Value index;
    Result result_peg_2(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_4 = stream.getLineInfo(result_peg_2.getPosition());
                line = &line_info_peg_4;
            
            
            
            result_peg_2 = rule_identifier(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_6;
                }
                id = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_9 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_11(result_peg_2.getPosition());
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
                        goto loop_peg_10;
                        success_peg_12:
                        ;
                        result_peg_2.addResult(result_peg_11);
                    } while (true);
                    loop_peg_10:
                    ;
                            
                }
                goto success_peg_8;
                goto out_peg_6;
                success_peg_8:
                ;
            
            
            
            result_peg_2.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_6;
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
                        goto loop_peg_21;
                        success_peg_23:
                        ;
                        result_peg_2.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                            
                }
                goto success_peg_19;
                goto out_peg_6;
                success_peg_19:
                ;
            
            
            
            Result result_peg_29(result_peg_2.getPosition());
                result_peg_29 = rule_line_end_or_comment(stream, result_peg_29.getPosition());
                if (result_peg_29.error()){
                    goto out_peg_6;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributeWithInfo(line, id);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_6:
        Result result_peg_30(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_32 = stream.getLineInfo(result_peg_30.getPosition());
                line = &line_info_peg_32;
            
            
            
            result_peg_30 = rule_identifier(stream, result_peg_30.getPosition());
                if (result_peg_30.error()){
                    goto out_peg_34;
                }
                id = result_peg_30.getValues();
            
            
            
            {
                    int position_peg_37 = result_peg_30.getPosition();
                    
                    result_peg_30.reset();
                    do{
                        Result result_peg_39(result_peg_30.getPosition());
                        {
                            int position_peg_41 = result_peg_39.getPosition();
                            
                            result_peg_39.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_39.getPosition()))){
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
                            
                            result_peg_39.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_39.getPosition()))){
                                    result_peg_39.nextPosition();
                                } else {
                                    result_peg_39.setPosition(position_peg_43);
                                    goto out_peg_44;
                                }
                            }
                                
                        }
                        goto success_peg_40;
                        out_peg_44:
                        goto loop_peg_38;
                        success_peg_40:
                        ;
                        result_peg_30.addResult(result_peg_39);
                    } while (true);
                    loop_peg_38:
                    ;
                            
                }
                goto success_peg_36;
                goto out_peg_34;
                success_peg_36:
                ;
            
            
            
            result_peg_30.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_34;
                    }
                }
            
            
            
            {
                    int position_peg_48 = result_peg_30.getPosition();
                    
                    result_peg_30.reset();
                    do{
                        Result result_peg_50(result_peg_30.getPosition());
                        {
                            int position_peg_52 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_52);
                                    goto out_peg_53;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_53:
                        {
                            int position_peg_54 = result_peg_50.getPosition();
                            
                            result_peg_50.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_50.getPosition()))){
                                    result_peg_50.nextPosition();
                                } else {
                                    result_peg_50.setPosition(position_peg_54);
                                    goto out_peg_55;
                                }
                            }
                                
                        }
                        goto success_peg_51;
                        out_peg_55:
                        goto loop_peg_49;
                        success_peg_51:
                        ;
                        result_peg_30.addResult(result_peg_50);
                    } while (true);
                    loop_peg_49:
                    ;
                            
                }
                goto success_peg_47;
                goto out_peg_34;
                success_peg_47:
                ;
            
            
            
            result_peg_30 = rule_valuelist(stream, result_peg_30.getPosition());
                if (result_peg_30.error()){
                    goto out_peg_34;
                }
                data = result_peg_30.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(line, id, data);
                    result_peg_30.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_30.getPosition());
        
        return result_peg_30;
        out_peg_34:
        Result result_peg_57(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_59 = stream.getLineInfo(result_peg_57.getPosition());
                line = &line_info_peg_59;
            
            
            
            result_peg_57 = rule_identifier(stream, result_peg_57.getPosition());
                if (result_peg_57.error()){
                    goto out_peg_61;
                }
                id = result_peg_57.getValues();
            
            
            
            {
                    int position_peg_64 = result_peg_57.getPosition();
                    
                    result_peg_57.reset();
                    do{
                        Result result_peg_66(result_peg_57.getPosition());
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
                        goto loop_peg_65;
                        success_peg_67:
                        ;
                        result_peg_57.addResult(result_peg_66);
                    } while (true);
                    loop_peg_65:
                    ;
                            
                }
                goto success_peg_63;
                goto out_peg_61;
                success_peg_63:
                ;
            
            
            
            result_peg_57.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_57.getPosition()))){
                        result_peg_57.nextPosition();
                    } else {
                        goto out_peg_61;
                    }
                }
            
            
            
            {
                    int position_peg_75 = result_peg_57.getPosition();
                    
                    result_peg_57.reset();
                    do{
                        Result result_peg_77(result_peg_57.getPosition());
                        {
                            int position_peg_79 = result_peg_77.getPosition();
                            
                            result_peg_77.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_77.getPosition()))){
                                    result_peg_77.nextPosition();
                                } else {
                                    result_peg_77.setPosition(position_peg_79);
                                    goto out_peg_80;
                                }
                            }
                                
                        }
                        goto success_peg_78;
                        out_peg_80:
                        {
                            int position_peg_81 = result_peg_77.getPosition();
                            
                            result_peg_77.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_77.getPosition()))){
                                    result_peg_77.nextPosition();
                                } else {
                                    result_peg_77.setPosition(position_peg_81);
                                    goto out_peg_82;
                                }
                            }
                                
                        }
                        goto success_peg_78;
                        out_peg_82:
                        goto loop_peg_76;
                        success_peg_78:
                        ;
                        result_peg_57.addResult(result_peg_77);
                    } while (true);
                    loop_peg_76:
                    ;
                            
                }
                goto success_peg_74;
                goto out_peg_61;
                success_peg_74:
                ;
            
            
            
            result_peg_57 = rule_filename(stream, result_peg_57.getPosition());
                if (result_peg_57.error()){
                    goto out_peg_61;
                }
                data = result_peg_57.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(line, id, data);
                    result_peg_57.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_57.getPosition());
        
        return result_peg_57;
        out_peg_61:
        Result result_peg_84(myposition);
        
        
        {
        
            result_peg_84 = rule_identifier(stream, result_peg_84.getPosition());
                if (result_peg_84.error()){
                    goto out_peg_86;
                }
                id = result_peg_84.getValues();
            
            
            
            {
                    int position_peg_89 = result_peg_84.getPosition();
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_91(result_peg_84.getPosition());
                        {
                            int position_peg_93 = result_peg_91.getPosition();
                            
                            result_peg_91.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_91.getPosition()))){
                                    result_peg_91.nextPosition();
                                } else {
                                    result_peg_91.setPosition(position_peg_93);
                                    goto out_peg_94;
                                }
                            }
                                
                        }
                        goto success_peg_92;
                        out_peg_94:
                        {
                            int position_peg_95 = result_peg_91.getPosition();
                            
                            result_peg_91.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_91.getPosition()))){
                                    result_peg_91.nextPosition();
                                } else {
                                    result_peg_91.setPosition(position_peg_95);
                                    goto out_peg_96;
                                }
                            }
                                
                        }
                        goto success_peg_92;
                        out_peg_96:
                        goto loop_peg_90;
                        success_peg_92:
                        ;
                        result_peg_84.addResult(result_peg_91);
                    } while (true);
                    loop_peg_90:
                    ;
                            
                }
                goto success_peg_88;
                goto out_peg_86;
                success_peg_88:
                ;
            
            
            
            result_peg_84.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_86;
                    }
                }
            
            
            
            {
                    int position_peg_100 = result_peg_84.getPosition();
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_102(result_peg_84.getPosition());
                        {
                            int position_peg_104 = result_peg_102.getPosition();
                            
                            result_peg_102.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_102.getPosition()))){
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
                            
                            result_peg_102.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_102.getPosition()))){
                                    result_peg_102.nextPosition();
                                } else {
                                    result_peg_102.setPosition(position_peg_106);
                                    goto out_peg_107;
                                }
                            }
                                
                        }
                        goto success_peg_103;
                        out_peg_107:
                        goto loop_peg_101;
                        success_peg_103:
                        ;
                        result_peg_84.addResult(result_peg_102);
                    } while (true);
                    loop_peg_101:
                    ;
                            
                }
                goto success_peg_99;
                goto out_peg_86;
                success_peg_99:
                ;
            
            
            
            result_peg_84 = rule_number(stream, result_peg_84.getPosition());
                if (result_peg_84.error()){
                    goto out_peg_86;
                }
                index = result_peg_84.getValues();
            
            
            
            {
                    int position_peg_111 = result_peg_84.getPosition();
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_113(result_peg_84.getPosition());
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
                        goto loop_peg_112;
                        success_peg_114:
                        ;
                        result_peg_84.addResult(result_peg_113);
                    } while (true);
                    loop_peg_112:
                    ;
                            
                }
                goto success_peg_110;
                goto out_peg_86;
                success_peg_110:
                ;
            
            
            
            result_peg_84.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_86;
                    }
                }
            
            
            
            {
                    int position_peg_122 = result_peg_84.getPosition();
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_124(result_peg_84.getPosition());
                        {
                            int position_peg_126 = result_peg_124.getPosition();
                            
                            result_peg_124.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_124.getPosition()))){
                                    result_peg_124.nextPosition();
                                } else {
                                    result_peg_124.setPosition(position_peg_126);
                                    goto out_peg_127;
                                }
                            }
                                
                        }
                        goto success_peg_125;
                        out_peg_127:
                        {
                            int position_peg_128 = result_peg_124.getPosition();
                            
                            result_peg_124.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_124.getPosition()))){
                                    result_peg_124.nextPosition();
                                } else {
                                    result_peg_124.setPosition(position_peg_128);
                                    goto out_peg_129;
                                }
                            }
                                
                        }
                        goto success_peg_125;
                        out_peg_129:
                        goto loop_peg_123;
                        success_peg_125:
                        ;
                        result_peg_84.addResult(result_peg_124);
                    } while (true);
                    loop_peg_123:
                    ;
                            
                }
                goto success_peg_121;
                goto out_peg_86;
                success_peg_121:
                ;
            
            
            
            result_peg_84.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_86;
                    }
                }
            
            
            
            {
                    int position_peg_133 = result_peg_84.getPosition();
                    
                    result_peg_84.reset();
                    do{
                        Result result_peg_135(result_peg_84.getPosition());
                        {
                            int position_peg_137 = result_peg_135.getPosition();
                            
                            result_peg_135.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_135.getPosition()))){
                                    result_peg_135.nextPosition();
                                } else {
                                    result_peg_135.setPosition(position_peg_137);
                                    goto out_peg_138;
                                }
                            }
                                
                        }
                        goto success_peg_136;
                        out_peg_138:
                        {
                            int position_peg_139 = result_peg_135.getPosition();
                            
                            result_peg_135.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_135.getPosition()))){
                                    result_peg_135.nextPosition();
                                } else {
                                    result_peg_135.setPosition(position_peg_139);
                                    goto out_peg_140;
                                }
                            }
                                
                        }
                        goto success_peg_136;
                        out_peg_140:
                        goto loop_peg_134;
                        success_peg_136:
                        ;
                        result_peg_84.addResult(result_peg_135);
                    } while (true);
                    loop_peg_134:
                    ;
                            
                }
                goto success_peg_132;
                goto out_peg_86;
                success_peg_132:
                ;
            
            
            
            result_peg_84 = rule_valuelist(stream, result_peg_84.getPosition());
                if (result_peg_84.error()){
                    goto out_peg_86;
                }
                data = result_peg_84.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(id, index, data);
                    result_peg_84.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_84.getPosition());
        
        return result_peg_84;
        out_peg_86:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute_simple(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_27(stream, "attribute_simple");
    int myposition = position;
    
    
    Value id;
        Value data;
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2 = rule_identifier(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                id = result_peg_2.getValues();
            
            
            
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
            
            
            
            result_peg_2.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
            
            
            
            {
                    int position_peg_18 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_20(result_peg_2.getPosition());
                        {
                            int position_peg_22 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_22);
                                    goto out_peg_23;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_23:
                        {
                            int position_peg_24 = result_peg_20.getPosition();
                            
                            result_peg_20.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_20.getPosition()))){
                                    result_peg_20.nextPosition();
                                } else {
                                    result_peg_20.setPosition(position_peg_24);
                                    goto out_peg_25;
                                }
                            }
                                
                        }
                        goto success_peg_21;
                        out_peg_25:
                        goto loop_peg_19;
                        success_peg_21:
                        ;
                        result_peg_2.addResult(result_peg_20);
                    } while (true);
                    loop_peg_19:
                    ;
                            
                }
                goto success_peg_17;
                goto out_peg_4;
                success_peg_17:
                ;
            
            
            
            result_peg_2 = rule_value(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                data = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_9(stream, "identifier");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2 = rule_name(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        result_peg_7.setValue(Value((void*) "."));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                        
                        
                        
                        result_peg_7 = rule_name(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
            
            Result result_peg_5 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(result_peg_3.getValues(),result_peg_5.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_28(stream, "identifier_list");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2 = rule_identifier(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_6(result_peg_2.getPosition());
                    {
                    
                        {
                                int position_peg_9 = result_peg_6.getPosition();
                                
                                result_peg_6.reset();
                                do{
                                    Result result_peg_11(result_peg_6.getPosition());
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
                                    goto loop_peg_10;
                                    success_peg_12:
                                    ;
                                    result_peg_6.addResult(result_peg_11);
                                } while (true);
                                loop_peg_10:
                                ;
                                        
                            }
                            goto success_peg_8;
                            goto loop_peg_5;
                            success_peg_8:
                            ;
                        
                        
                        
                        result_peg_6.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_6.getPosition()))){
                                    result_peg_6.nextPosition();
                                } else {
                                    goto loop_peg_5;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_20 = result_peg_6.getPosition();
                                
                                result_peg_6.reset();
                                do{
                                    Result result_peg_22(result_peg_6.getPosition());
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
                                    goto loop_peg_21;
                                    success_peg_23:
                                    ;
                                    result_peg_6.addResult(result_peg_22);
                                } while (true);
                                loop_peg_21:
                                ;
                                        
                            }
                            goto success_peg_19;
                            goto loop_peg_5;
                            success_peg_19:
                            ;
                        
                        
                        
                        result_peg_6 = rule_filename(stream, result_peg_6.getPosition());
                            if (result_peg_6.error()){
                                goto loop_peg_5;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_6);
                } while (true);
                loop_peg_5:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_32(stream, "valuelist");
    int myposition = position;
    
    
    Value line;
    Result result_peg_2(myposition);
        
        
        {
        
            Stream::LineInfo line_info_peg_4 = stream.getLineInfo(result_peg_2.getPosition());
                line = &line_info_peg_4;
            
            
            
            result_peg_2 = rule_value(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_6;
                }
            
            Result result_peg_5 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_9(result_peg_2.getPosition());
                    {
                    
                        {
                                int position_peg_12 = result_peg_9.getPosition();
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_14(result_peg_9.getPosition());
                                    {
                                        int position_peg_16 = result_peg_14.getPosition();
                                        
                                        result_peg_14.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                                result_peg_14.nextPosition();
                                            } else {
                                                result_peg_14.setPosition(position_peg_16);
                                                goto out_peg_17;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_15;
                                    out_peg_17:
                                    {
                                        int position_peg_18 = result_peg_14.getPosition();
                                        
                                        result_peg_14.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                                result_peg_14.nextPosition();
                                            } else {
                                                result_peg_14.setPosition(position_peg_18);
                                                goto out_peg_19;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_15;
                                    out_peg_19:
                                    goto loop_peg_13;
                                    success_peg_15:
                                    ;
                                    result_peg_9.addResult(result_peg_14);
                                } while (true);
                                loop_peg_13:
                                ;
                                        
                            }
                            goto success_peg_11;
                            goto loop_peg_8;
                            success_peg_11:
                            ;
                        
                        
                        
                        result_peg_9.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_9.getPosition()))){
                                    result_peg_9.nextPosition();
                                } else {
                                    goto loop_peg_8;
                                }
                            }
                        
                        
                        
                        {
                                int position_peg_23 = result_peg_9.getPosition();
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_25(result_peg_9.getPosition());
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
                                    goto loop_peg_24;
                                    success_peg_26:
                                    ;
                                    result_peg_9.addResult(result_peg_25);
                                } while (true);
                                loop_peg_24:
                                ;
                                        
                            }
                            goto success_peg_22;
                            goto loop_peg_8;
                            success_peg_22:
                            ;
                        
                        
                        
                        int save_peg_31 = result_peg_9.getPosition();
                            
                            result_peg_9 = rule_value(stream, result_peg_9.getPosition());
                            if (result_peg_9.error()){
                                
                                result_peg_9 = Result(save_peg_31);
                                result_peg_9.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                ;
            
            Result result_peg_7 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeValueList(line, result_peg_5.getValues(),result_peg_7.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_6:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_163(stream, "value");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        
        {
            int position_peg_4 = result_peg_2.getPosition();
            {
            
                result_peg_2.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_4);
                            goto out_peg_6;
                        }
                    }
                
                
                
                result_peg_2.reset();
                    do{
                        Result result_peg_9(result_peg_2.getPosition());
                        {
                        
                            Result result_peg_12(result_peg_9);
                                result_peg_12.setValue(Value((void*) "\""));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\""[i], stream.get(result_peg_12.getPosition()))){
                                        result_peg_12.nextPosition();
                                    } else {
                                        goto not_peg_11;
                                    }
                                }
                                goto loop_peg_8;
                                not_peg_11:
                                result_peg_9.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_15(result_peg_9);
                                result_peg_15.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_15.getPosition()))){
                                        result_peg_15.nextPosition();
                                    } else {
                                        goto not_peg_14;
                                    }
                                }
                                goto loop_peg_8;
                                not_peg_14:
                                result_peg_9.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_16 = stream.get(result_peg_9.getPosition());
                                if (temp_peg_16 != '\0'){
                                    result_peg_9.setValue(Value((void*) (long) temp_peg_16));
                                    result_peg_9.nextPosition();
                                } else {
                                    goto loop_peg_8;
                                }
                            
                            
                        }
                        result_peg_2.addResult(result_peg_9);
                    } while (true);
                    loop_peg_8:
                    ;
                    data = result_peg_2.getValues();
                
                
                
                result_peg_2.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_4);
                            goto out_peg_6;
                        }
                    }
                
                
                
                {
                        Value value((void*) 0);
                        value = makeString(data);
                        result_peg_2.setValue(value);
                    }
                
                
            }
        }
        goto success_peg_3;
        out_peg_6:
        goto out_peg_18;
        success_peg_3:
        ;
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_18:
        Result result_peg_19(myposition);
        
        
        result_peg_19 = rule_date(stream, result_peg_19.getPosition());
        if (result_peg_19.error()){
            goto out_peg_20;
        }
        
        stream.update(result_peg_19.getPosition());
        
        return result_peg_19;
        out_peg_20:
        Result result_peg_21(myposition);
        
        
        {
        
            result_peg_21 = rule_number(stream, result_peg_21.getPosition());
                if (result_peg_21.error()){
                    goto out_peg_23;
                }
            
            Result result_peg_22 = result_peg_21;
            
            Result result_peg_26(result_peg_21);
                {
                    int position_peg_28 = result_peg_26.getPosition();
                    
                    char letter_peg_29 = stream.get(result_peg_26.getPosition());
                    if (letter_peg_29 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_29) != NULL){
                        result_peg_26.nextPosition();
                        result_peg_26.setValue(Value((void*) (long) letter_peg_29));
                    } else {
                        result_peg_26.setPosition(position_peg_28);
                        goto out_peg_30;
                    }
                    
                }
                goto success_peg_27;
                out_peg_30:
                goto not_peg_25;
                success_peg_27:
                ;
                goto out_peg_23;
                not_peg_25:
                result_peg_21.setValue(Value((void*)0));
            
            
            
            Result result_peg_33(result_peg_21);
                result_peg_33.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto not_peg_32;
                    }
                }
                goto out_peg_23;
                not_peg_32:
                result_peg_21.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_22.getValues();
                    result_peg_21.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_21.getPosition());
        
        return result_peg_21;
        out_peg_23:
        Result result_peg_34(myposition);
        
        
        {
        
            result_peg_34 = rule_attribute_simple(stream, result_peg_34.getPosition());
                if (result_peg_34.error()){
                    goto out_peg_36;
                }
            
            Result result_peg_35 = result_peg_34;
            
            {
                    Value value((void*) 0);
                    value = makeValueAttribute(result_peg_35.getValues());
                    result_peg_34.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_34.getPosition());
        
        return result_peg_34;
        out_peg_36:
        Result result_peg_37(myposition);
        
        
        {
        
            {
                    int position_peg_40 = result_peg_37.getPosition();
                    
                    result_peg_37.setValue(Value((void*) "normal"));
                    for (int i = 0; i < 6; i++){
                        if (compareChar("normal"[i], stream.get(result_peg_37.getPosition()))){
                            result_peg_37.nextPosition();
                        } else {
                            result_peg_37.setPosition(position_peg_40);
                            goto out_peg_41;
                        }
                    }
                        
                }
                goto success_peg_39;
                out_peg_41:
                {
                    int position_peg_42 = result_peg_37.getPosition();
                    
                    result_peg_37.setValue(Value((void*) "parallax"));
                    for (int i = 0; i < 8; i++){
                        if (compareChar("parallax"[i], stream.get(result_peg_37.getPosition()))){
                            result_peg_37.nextPosition();
                        } else {
                            result_peg_37.setPosition(position_peg_42);
                            goto out_peg_43;
                        }
                    }
                        
                }
                goto success_peg_39;
                out_peg_43:
                {
                    int position_peg_44 = result_peg_37.getPosition();
                    
                    result_peg_37.setValue(Value((void*) "addalpha"));
                    for (int i = 0; i < 8; i++){
                        if (compareChar("addalpha"[i], stream.get(result_peg_37.getPosition()))){
                            result_peg_37.nextPosition();
                        } else {
                            result_peg_37.setPosition(position_peg_44);
                            goto out_peg_45;
                        }
                    }
                        
                }
                goto success_peg_39;
                out_peg_45:
                {
                    int position_peg_46 = result_peg_37.getPosition();
                    
                    result_peg_37.setValue(Value((void*) "add1"));
                    for (int i = 0; i < 4; i++){
                        if (compareChar("add1"[i], stream.get(result_peg_37.getPosition()))){
                            result_peg_37.nextPosition();
                        } else {
                            result_peg_37.setPosition(position_peg_46);
                            goto out_peg_47;
                        }
                    }
                        
                }
                goto success_peg_39;
                out_peg_47:
                {
                    int position_peg_48 = result_peg_37.getPosition();
                    
                    result_peg_37.setValue(Value((void*) "add"));
                    for (int i = 0; i < 3; i++){
                        if (compareChar("add"[i], stream.get(result_peg_37.getPosition()))){
                            result_peg_37.nextPosition();
                        } else {
                            result_peg_37.setPosition(position_peg_48);
                            goto out_peg_49;
                        }
                    }
                        
                }
                goto success_peg_39;
                out_peg_49:
                {
                    int position_peg_50 = result_peg_37.getPosition();
                    
                    result_peg_37.setValue(Value((void*) "sub"));
                    for (int i = 0; i < 3; i++){
                        if (compareChar("sub"[i], stream.get(result_peg_37.getPosition()))){
                            result_peg_37.nextPosition();
                        } else {
                            result_peg_37.setPosition(position_peg_50);
                            goto out_peg_51;
                        }
                    }
                        
                }
                goto success_peg_39;
                out_peg_51:
                goto out_peg_52;
                success_peg_39:
                ;
            
            Result result_peg_38 = result_peg_37;
            
            Result result_peg_55(result_peg_37);
                {
                    int position_peg_57 = result_peg_55.getPosition();
                    
                    {
                        int position_peg_59 = result_peg_55.getPosition();
                        
                        char letter_peg_60 = stream.get(result_peg_55.getPosition());
                        if (letter_peg_60 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_60) != NULL){
                            result_peg_55.nextPosition();
                            result_peg_55.setValue(Value((void*) (long) letter_peg_60));
                        } else {
                            result_peg_55.setPosition(position_peg_59);
                            goto out_peg_61;
                        }
                        
                    }
                    goto success_peg_58;
                    out_peg_61:
                    result_peg_55.setPosition(position_peg_57);
                    goto out_peg_62;
                    success_peg_58:
                    ;
                    
                }
                goto success_peg_56;
                out_peg_62:
                {
                    int position_peg_63 = result_peg_55.getPosition();
                    
                    {
                        int position_peg_65 = result_peg_55.getPosition();
                        
                        char letter_peg_66 = stream.get(result_peg_55.getPosition());
                        if (letter_peg_66 != '\0' && strchr("0123456789", letter_peg_66) != NULL){
                            result_peg_55.nextPosition();
                            result_peg_55.setValue(Value((void*) (long) letter_peg_66));
                        } else {
                            result_peg_55.setPosition(position_peg_65);
                            goto out_peg_67;
                        }
                        
                    }
                    goto success_peg_64;
                    out_peg_67:
                    result_peg_55.setPosition(position_peg_63);
                    goto out_peg_68;
                    success_peg_64:
                    ;
                    
                }
                goto success_peg_56;
                out_peg_68:
                goto not_peg_54;
                success_peg_56:
                ;
                goto out_peg_52;
                not_peg_54:
                result_peg_37.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_38.getValues());
                    result_peg_37.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_37.getPosition());
        
        return result_peg_37;
        out_peg_52:
        Result result_peg_69(myposition);
        
        
        {
        
            result_peg_69.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_69.getPosition()))){
                        result_peg_69.nextPosition();
                    } else {
                        goto out_peg_71;
                    }
                }
            
            Result result_peg_70 = result_peg_69;
            
            Result result_peg_74(result_peg_69);
                {
                    int position_peg_76 = result_peg_74.getPosition();
                    {
                    
                        Result result_peg_79(result_peg_74);
                            result_peg_79.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_79.getPosition()))){
                                    result_peg_79.nextPosition();
                                } else {
                                    goto not_peg_78;
                                }
                            }
                            result_peg_74.setPosition(position_peg_76);
                            goto out_peg_80;
                            not_peg_78:
                            result_peg_74.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_83(result_peg_74);
                            result_peg_83.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_83.getPosition()))){
                                    result_peg_83.nextPosition();
                                } else {
                                    goto not_peg_82;
                                }
                            }
                            result_peg_74.setPosition(position_peg_76);
                            goto out_peg_80;
                            not_peg_82:
                            result_peg_74.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_86(result_peg_74);
                            result_peg_86.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_86.getPosition()))){
                                    result_peg_86.nextPosition();
                                } else {
                                    goto not_peg_85;
                                }
                            }
                            result_peg_74.setPosition(position_peg_76);
                            goto out_peg_80;
                            not_peg_85:
                            result_peg_74.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_89(result_peg_74);
                            result_peg_89.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_89.getPosition()) == (unsigned char) 13){
                                result_peg_89.nextPosition();
                            } else {
                                goto not_peg_88;
                            }
                            result_peg_74.setPosition(position_peg_76);
                            goto out_peg_80;
                            not_peg_88:
                            result_peg_74.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_92(result_peg_74);
                            result_peg_92.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_92.getPosition()))){
                                    result_peg_92.nextPosition();
                                } else {
                                    goto not_peg_91;
                                }
                            }
                            result_peg_74.setPosition(position_peg_76);
                            goto out_peg_80;
                            not_peg_91:
                            result_peg_74.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_95(result_peg_74);
                            result_peg_95.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    goto not_peg_94;
                                }
                            }
                            result_peg_74.setPosition(position_peg_76);
                            goto out_peg_80;
                            not_peg_94:
                            result_peg_74.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_96 = stream.get(result_peg_74.getPosition());
                            if (temp_peg_96 != '\0'){
                                result_peg_74.setValue(Value((void*) (long) temp_peg_96));
                                result_peg_74.nextPosition();
                            } else {
                                result_peg_74.setPosition(position_peg_76);
                                goto out_peg_80;
                            }
                        
                        
                    }
                }
                goto success_peg_75;
                out_peg_80:
                goto not_peg_73;
                success_peg_75:
                ;
                goto out_peg_71;
                not_peg_73:
                result_peg_69.setValue(Value((void*)0));
            
            
            
            Result result_peg_99(result_peg_69);
                result_peg_99.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_99.getPosition()))){
                        result_peg_99.nextPosition();
                    } else {
                        goto not_peg_98;
                    }
                }
                goto out_peg_71;
                not_peg_98:
                result_peg_69.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_70.getValues());
                    result_peg_69.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_69.getPosition());
        
        return result_peg_69;
        out_peg_71:
        Result result_peg_100(myposition);
        
        
        {
        
            result_peg_100.setValue(Value((void*) "h"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_100.getPosition()))){
                        result_peg_100.nextPosition();
                    } else {
                        goto out_peg_102;
                    }
                }
            
            Result result_peg_101 = result_peg_100;
            
            Result result_peg_105(result_peg_100);
                {
                    int position_peg_107 = result_peg_105.getPosition();
                    {
                    
                        Result result_peg_110(result_peg_105);
                            result_peg_110.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_110.getPosition()))){
                                    result_peg_110.nextPosition();
                                } else {
                                    goto not_peg_109;
                                }
                            }
                            result_peg_105.setPosition(position_peg_107);
                            goto out_peg_111;
                            not_peg_109:
                            result_peg_105.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_114(result_peg_105);
                            result_peg_114.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_114.getPosition()))){
                                    result_peg_114.nextPosition();
                                } else {
                                    goto not_peg_113;
                                }
                            }
                            result_peg_105.setPosition(position_peg_107);
                            goto out_peg_111;
                            not_peg_113:
                            result_peg_105.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_117(result_peg_105);
                            result_peg_117.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_117.getPosition()))){
                                    result_peg_117.nextPosition();
                                } else {
                                    goto not_peg_116;
                                }
                            }
                            result_peg_105.setPosition(position_peg_107);
                            goto out_peg_111;
                            not_peg_116:
                            result_peg_105.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_120(result_peg_105);
                            result_peg_120.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_120.getPosition()) == (unsigned char) 13){
                                result_peg_120.nextPosition();
                            } else {
                                goto not_peg_119;
                            }
                            result_peg_105.setPosition(position_peg_107);
                            goto out_peg_111;
                            not_peg_119:
                            result_peg_105.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_123(result_peg_105);
                            result_peg_123.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_123.getPosition()))){
                                    result_peg_123.nextPosition();
                                } else {
                                    goto not_peg_122;
                                }
                            }
                            result_peg_105.setPosition(position_peg_107);
                            goto out_peg_111;
                            not_peg_122:
                            result_peg_105.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_126(result_peg_105);
                            result_peg_126.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_126.getPosition()))){
                                    result_peg_126.nextPosition();
                                } else {
                                    goto not_peg_125;
                                }
                            }
                            result_peg_105.setPosition(position_peg_107);
                            goto out_peg_111;
                            not_peg_125:
                            result_peg_105.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_127 = stream.get(result_peg_105.getPosition());
                            if (temp_peg_127 != '\0'){
                                result_peg_105.setValue(Value((void*) (long) temp_peg_127));
                                result_peg_105.nextPosition();
                            } else {
                                result_peg_105.setPosition(position_peg_107);
                                goto out_peg_111;
                            }
                        
                        
                    }
                }
                goto success_peg_106;
                out_peg_111:
                goto not_peg_104;
                success_peg_106:
                ;
                goto out_peg_102;
                not_peg_104:
                result_peg_100.setValue(Value((void*)0));
            
            
            
            Result result_peg_130(result_peg_100);
                result_peg_130.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_130.getPosition()))){
                        result_peg_130.nextPosition();
                    } else {
                        goto not_peg_129;
                    }
                }
                goto out_peg_102;
                not_peg_129:
                result_peg_100.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_101.getValues());
                    result_peg_100.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_100.getPosition());
        
        return result_peg_100;
        out_peg_102:
        Result result_peg_131(myposition);
        
        
        {
        
            result_peg_131.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_131.getPosition()))){
                        result_peg_131.nextPosition();
                    } else {
                        goto out_peg_133;
                    }
                }
            
            Result result_peg_132 = result_peg_131;
            
            int save_peg_135 = result_peg_131.getPosition();
                
                result_peg_131 = rule_number(stream, result_peg_131.getPosition());
                if (result_peg_131.error()){
                    
                    result_peg_131 = Result(save_peg_135);
                    result_peg_131.setValue(Value((void*) 0));
                    
                }
            
            
            
            Result result_peg_138(result_peg_131);
                {
                    int position_peg_140 = result_peg_138.getPosition();
                    {
                    
                        Result result_peg_143(result_peg_138);
                            result_peg_143.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_143.getPosition()))){
                                    result_peg_143.nextPosition();
                                } else {
                                    goto not_peg_142;
                                }
                            }
                            result_peg_138.setPosition(position_peg_140);
                            goto out_peg_144;
                            not_peg_142:
                            result_peg_138.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_147(result_peg_138);
                            result_peg_147.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_147.getPosition()))){
                                    result_peg_147.nextPosition();
                                } else {
                                    goto not_peg_146;
                                }
                            }
                            result_peg_138.setPosition(position_peg_140);
                            goto out_peg_144;
                            not_peg_146:
                            result_peg_138.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_150(result_peg_138);
                            result_peg_150.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_150.getPosition()))){
                                    result_peg_150.nextPosition();
                                } else {
                                    goto not_peg_149;
                                }
                            }
                            result_peg_138.setPosition(position_peg_140);
                            goto out_peg_144;
                            not_peg_149:
                            result_peg_138.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_153(result_peg_138);
                            result_peg_153.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_153.getPosition()) == (unsigned char) 13){
                                result_peg_153.nextPosition();
                            } else {
                                goto not_peg_152;
                            }
                            result_peg_138.setPosition(position_peg_140);
                            goto out_peg_144;
                            not_peg_152:
                            result_peg_138.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_156(result_peg_138);
                            result_peg_156.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_156.getPosition()))){
                                    result_peg_156.nextPosition();
                                } else {
                                    goto not_peg_155;
                                }
                            }
                            result_peg_138.setPosition(position_peg_140);
                            goto out_peg_144;
                            not_peg_155:
                            result_peg_138.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_159(result_peg_138);
                            result_peg_159.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_159.getPosition()))){
                                    result_peg_159.nextPosition();
                                } else {
                                    goto not_peg_158;
                                }
                            }
                            result_peg_138.setPosition(position_peg_140);
                            goto out_peg_144;
                            not_peg_158:
                            result_peg_138.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_160 = stream.get(result_peg_138.getPosition());
                            if (temp_peg_160 != '\0'){
                                result_peg_138.setValue(Value((void*) (long) temp_peg_160));
                                result_peg_138.nextPosition();
                            } else {
                                result_peg_138.setPosition(position_peg_140);
                                goto out_peg_144;
                            }
                        
                        
                    }
                }
                goto success_peg_139;
                out_peg_144:
                goto not_peg_137;
                success_peg_139:
                ;
                goto out_peg_133;
                not_peg_137:
                result_peg_131.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_132.getValues());
                    result_peg_131.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_131.getPosition());
        
        return result_peg_131;
        out_peg_133:
        Result result_peg_161(myposition);
        
        
        result_peg_161 = rule_filename(stream, result_peg_161.getPosition());
        if (result_peg_161.error()){
            goto out_peg_162;
        }
        
        stream.update(result_peg_161.getPosition());
        
        return result_peg_161;
        out_peg_162:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_52(stream, "date");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        int position_peg_7 = result_peg_5.getPosition();
                        
                        char letter_peg_8 = stream.get(result_peg_5.getPosition());
                        if (letter_peg_8 != '\0' && strchr("0123456789", letter_peg_8) != NULL){
                            result_peg_5.nextPosition();
                            result_peg_5.setValue(Value((void*) (long) letter_peg_8));
                        } else {
                            result_peg_5.setPosition(position_peg_7);
                            goto out_peg_9;
                        }
                        
                    }
                    goto success_peg_6;
                    out_peg_9:
                    goto loop_peg_4;
                    success_peg_6:
                    ;
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                if (result_peg_2.matches() == 0){
                    goto out_peg_10;
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_10;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_14(result_peg_2.getPosition());
                    {
                        int position_peg_16 = result_peg_14.getPosition();
                        
                        char letter_peg_17 = stream.get(result_peg_14.getPosition());
                        if (letter_peg_17 != '\0' && strchr("0123456789", letter_peg_17) != NULL){
                            result_peg_14.nextPosition();
                            result_peg_14.setValue(Value((void*) (long) letter_peg_17));
                        } else {
                            result_peg_14.setPosition(position_peg_16);
                            goto out_peg_18;
                        }
                        
                    }
                    goto success_peg_15;
                    out_peg_18:
                    goto loop_peg_13;
                    success_peg_15:
                    ;
                    result_peg_2.addResult(result_peg_14);
                } while (true);
                loop_peg_13:
                if (result_peg_2.matches() == 0){
                    goto out_peg_10;
                }
            
            Result result_peg_12 = result_peg_2;
            
            result_peg_2.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_10;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_22(result_peg_2.getPosition());
                    {
                        int position_peg_24 = result_peg_22.getPosition();
                        
                        char letter_peg_25 = stream.get(result_peg_22.getPosition());
                        if (letter_peg_25 != '\0' && strchr("0123456789", letter_peg_25) != NULL){
                            result_peg_22.nextPosition();
                            result_peg_22.setValue(Value((void*) (long) letter_peg_25));
                        } else {
                            result_peg_22.setPosition(position_peg_24);
                            goto out_peg_26;
                        }
                        
                    }
                    goto success_peg_23;
                    out_peg_26:
                    goto loop_peg_21;
                    success_peg_23:
                    ;
                    result_peg_2.addResult(result_peg_22);
                } while (true);
                loop_peg_21:
                if (result_peg_2.matches() == 0){
                    goto out_peg_10;
                }
            
            Result result_peg_20 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_3.getValues(),result_peg_12.getValues(),result_peg_20.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_10:
        Result result_peg_27(myposition);
        
        
        {
        
            result_peg_27.reset();
                do{
                    Result result_peg_30(result_peg_27.getPosition());
                    {
                        int position_peg_32 = result_peg_30.getPosition();
                        
                        char letter_peg_33 = stream.get(result_peg_30.getPosition());
                        if (letter_peg_33 != '\0' && strchr("0123456789", letter_peg_33) != NULL){
                            result_peg_30.nextPosition();
                            result_peg_30.setValue(Value((void*) (long) letter_peg_33));
                        } else {
                            result_peg_30.setPosition(position_peg_32);
                            goto out_peg_34;
                        }
                        
                    }
                    goto success_peg_31;
                    out_peg_34:
                    goto loop_peg_29;
                    success_peg_31:
                    ;
                    result_peg_27.addResult(result_peg_30);
                } while (true);
                loop_peg_29:
                if (result_peg_27.matches() == 0){
                    goto out_peg_35;
                }
            
            Result result_peg_28 = result_peg_27;
            
            result_peg_27.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_27.getPosition()))){
                        result_peg_27.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
            
            
            
            result_peg_27.reset();
                do{
                    Result result_peg_39(result_peg_27.getPosition());
                    {
                        int position_peg_41 = result_peg_39.getPosition();
                        
                        char letter_peg_42 = stream.get(result_peg_39.getPosition());
                        if (letter_peg_42 != '\0' && strchr("0123456789", letter_peg_42) != NULL){
                            result_peg_39.nextPosition();
                            result_peg_39.setValue(Value((void*) (long) letter_peg_42));
                        } else {
                            result_peg_39.setPosition(position_peg_41);
                            goto out_peg_43;
                        }
                        
                    }
                    goto success_peg_40;
                    out_peg_43:
                    goto loop_peg_38;
                    success_peg_40:
                    ;
                    result_peg_27.addResult(result_peg_39);
                } while (true);
                loop_peg_38:
                if (result_peg_27.matches() == 0){
                    goto out_peg_35;
                }
            
            Result result_peg_37 = result_peg_27;
            
            result_peg_27.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_27.getPosition()))){
                        result_peg_27.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
            
            
            
            result_peg_27.reset();
                do{
                    Result result_peg_47(result_peg_27.getPosition());
                    {
                        int position_peg_49 = result_peg_47.getPosition();
                        
                        char letter_peg_50 = stream.get(result_peg_47.getPosition());
                        if (letter_peg_50 != '\0' && strchr("0123456789", letter_peg_50) != NULL){
                            result_peg_47.nextPosition();
                            result_peg_47.setValue(Value((void*) (long) letter_peg_50));
                        } else {
                            result_peg_47.setPosition(position_peg_49);
                            goto out_peg_51;
                        }
                        
                    }
                    goto success_peg_48;
                    out_peg_51:
                    goto loop_peg_46;
                    success_peg_48:
                    ;
                    result_peg_27.addResult(result_peg_47);
                } while (true);
                loop_peg_46:
                if (result_peg_27.matches() == 0){
                    goto out_peg_35;
                }
            
            Result result_peg_45 = result_peg_27;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_28.getValues(),result_peg_37.getValues(),result_peg_45.getValues());
                    result_peg_27.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_27.getPosition());
        
        return result_peg_27;
        out_peg_35:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_12(stream, "number");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
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
                    
                    result_peg_2.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
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
            
            result_peg_2 = rule_float_or_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_11;
                }
            
            Result result_peg_10 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_3.getValues(),result_peg_10.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_11:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    
    RuleTrace trace_peg_31(stream, "float_or_integer");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_2(myposition);
        
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        int position_peg_7 = result_peg_5.getPosition();
                        
                        char letter_peg_8 = stream.get(result_peg_5.getPosition());
                        if (letter_peg_8 != '\0' && strchr("0123456789", letter_peg_8) != NULL){
                            result_peg_5.nextPosition();
                            result_peg_5.setValue(Value((void*) (long) letter_peg_8));
                        } else {
                            result_peg_5.setPosition(position_peg_7);
                            goto out_peg_9;
                        }
                        
                    }
                    goto success_peg_6;
                    out_peg_9:
                    goto loop_peg_4;
                    success_peg_6:
                    ;
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                ;
                left = result_peg_2.getValues();
            
            
            
            result_peg_2.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_14(result_peg_2.getPosition());
                    {
                        int position_peg_16 = result_peg_14.getPosition();
                        
                        char letter_peg_17 = stream.get(result_peg_14.getPosition());
                        if (letter_peg_17 != '\0' && strchr("0123456789", letter_peg_17) != NULL){
                            result_peg_14.nextPosition();
                            result_peg_14.setValue(Value((void*) (long) letter_peg_17));
                        } else {
                            result_peg_14.setPosition(position_peg_16);
                            goto out_peg_18;
                        }
                        
                    }
                    goto success_peg_15;
                    out_peg_18:
                    goto loop_peg_13;
                    success_peg_15:
                    ;
                    result_peg_2.addResult(result_peg_14);
                } while (true);
                loop_peg_13:
                if (result_peg_2.matches() == 0){
                    goto out_peg_11;
                }
                right = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(left,right);
                    result_peg_2.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_11:
        Result result_peg_19(myposition);
        
        
        {
        
            result_peg_19.reset();
                do{
                    Result result_peg_22(result_peg_19.getPosition());
                    {
                        int position_peg_24 = result_peg_22.getPosition();
                        
                        char letter_peg_25 = stream.get(result_peg_22.getPosition());
                        if (letter_peg_25 != '\0' && strchr("0123456789", letter_peg_25) != NULL){
                            result_peg_22.nextPosition();
                            result_peg_22.setValue(Value((void*) (long) letter_peg_25));
                        } else {
                            result_peg_22.setPosition(position_peg_24);
                            goto out_peg_26;
                        }
                        
                    }
                    goto success_peg_23;
                    out_peg_26:
                    goto loop_peg_21;
                    success_peg_23:
                    ;
                    result_peg_19.addResult(result_peg_22);
                } while (true);
                loop_peg_21:
                if (result_peg_19.matches() == 0){
                    goto out_peg_27;
                }
            
            Result result_peg_20 = result_peg_19;
            
            Result result_peg_30(result_peg_19);
                result_peg_30.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto not_peg_29;
                    }
                }
                goto out_peg_27;
                not_peg_29:
                result_peg_19.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_20.getValues());
                    result_peg_19.setValue(value);
                }
            
            
        }
        
        stream.update(result_peg_19.getPosition());
        
        return result_peg_19;
        out_peg_27:
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

    
