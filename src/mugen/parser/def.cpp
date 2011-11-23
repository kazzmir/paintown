

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
    
    RuleTrace trace_peg_429(stream, "start");
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
                    int position_peg_7 = result_peg_2.getPosition();
                    
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
                            
                            {
                                int position_peg_166 = result_peg_109.getPosition();
                                {
                                
                                    result_peg_109.setValue(Value((void*) ";"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_109.getPosition()))){
                                                result_peg_109.nextPosition();
                                            } else {
                                                result_peg_109.setPosition(position_peg_166);
                                                goto out_peg_168;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_109.reset();
                                        do{
                                            Result result_peg_177(result_peg_109.getPosition());
                                            {
                                            
                                                Result result_peg_180(result_peg_177);
                                                    result_peg_180.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_180.getPosition()))){
                                                            result_peg_180.nextPosition();
                                                        } else {
                                                            goto not_peg_179;
                                                        }
                                                    }
                                                    goto loop_peg_176;
                                                    not_peg_179:
                                                    result_peg_177.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_181 = stream.get(result_peg_177.getPosition());
                                                    if (temp_peg_181 != '\0'){
                                                        result_peg_177.setValue(Value((void*) (long) temp_peg_181));
                                                        result_peg_177.nextPosition();
                                                    } else {
                                                        goto loop_peg_176;
                                                    }
                                                
                                                
                                            }
                                            result_peg_109.addResult(result_peg_177);
                                        } while (true);
                                        loop_peg_176:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_164;
                            out_peg_168:
                            {
                                int position_peg_183 = result_peg_109.getPosition();
                                {
                                
                                    result_peg_109.setValue(Value((void*) "="));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_109.getPosition()))){
                                                result_peg_109.nextPosition();
                                            } else {
                                                result_peg_109.setPosition(position_peg_183);
                                                goto out_peg_185;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_109.reset();
                                        do{
                                            Result result_peg_194(result_peg_109.getPosition());
                                            {
                                            
                                                Result result_peg_197(result_peg_194);
                                                    result_peg_197.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_197.getPosition()))){
                                                            result_peg_197.nextPosition();
                                                        } else {
                                                            goto not_peg_196;
                                                        }
                                                    }
                                                    goto loop_peg_193;
                                                    not_peg_196:
                                                    result_peg_194.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_198 = stream.get(result_peg_194.getPosition());
                                                    if (temp_peg_198 != '\0'){
                                                        result_peg_194.setValue(Value((void*) (long) temp_peg_198));
                                                        result_peg_194.nextPosition();
                                                    } else {
                                                        goto loop_peg_193;
                                                    }
                                                
                                                
                                            }
                                            result_peg_109.addResult(result_peg_194);
                                        } while (true);
                                        loop_peg_193:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_164;
                            out_peg_185:
                            {
                                int position_peg_200 = result_peg_109.getPosition();
                                {
                                
                                    result_peg_109.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_109.getPosition()))){
                                                result_peg_109.nextPosition();
                                            } else {
                                                result_peg_109.setPosition(position_peg_200);
                                                goto out_peg_202;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_109.reset();
                                        do{
                                            Result result_peg_207(result_peg_109.getPosition());
                                            result_peg_207.setValue(Value((void*) "-"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("-"[i], stream.get(result_peg_207.getPosition()))){
                                                    result_peg_207.nextPosition();
                                                } else {
                                                    goto loop_peg_206;
                                                }
                                            }
                                            result_peg_109.addResult(result_peg_207);
                                        } while (true);
                                        loop_peg_206:
                                        if (result_peg_109.matches() == 0){
                                            result_peg_109.setPosition(position_peg_200);
                                            goto out_peg_202;
                                        }
                                    
                                    
                                }
                            }
                            goto success_peg_164;
                            out_peg_202:
                            result_peg_109.setPosition(position_peg_118);
                            goto out_peg_163;
                            success_peg_164:
                            ;
                            
                        }
                        goto success_peg_110;
                        out_peg_163:
                        goto loop_peg_108;
                        success_peg_110:
                        ;
                        result_peg_2.addResult(result_peg_109);
                    } while (true);
                    loop_peg_108:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_208;
                success_peg_5:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_211(result_peg_2.getPosition());
                    {
                        int position_peg_214 = result_peg_211.getPosition();
                        
                        result_peg_211.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_211.getPosition()))){
                                result_peg_211.nextPosition();
                            } else {
                                result_peg_211.setPosition(position_peg_214);
                                goto out_peg_215;
                            }
                        }
                            
                    }
                    goto success_peg_212;
                    out_peg_215:
                    {
                        int position_peg_217 = result_peg_211.getPosition();
                        
                        result_peg_211.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_211.getPosition()))){
                                result_peg_211.nextPosition();
                            } else {
                                result_peg_211.setPosition(position_peg_217);
                                goto out_peg_218;
                            }
                        }
                            
                    }
                    goto success_peg_212;
                    out_peg_218:
                    goto loop_peg_210;
                    success_peg_212:
                    ;
                    result_peg_2.addResult(result_peg_211);
                } while (true);
                loop_peg_210:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_221(result_peg_2.getPosition());
                    {
                    
                        result_peg_221 = rule_line(stream, result_peg_221.getPosition(), current);
                            if (result_peg_221.error()){
                                goto loop_peg_220;
                            }
                        
                        
                        
                        {
                                int position_peg_226 = result_peg_221.getPosition();
                                
                                result_peg_221.reset();
                                do{
                                    Result result_peg_328(result_peg_221.getPosition());
                                    {
                                        int position_peg_331 = result_peg_328.getPosition();
                                        
                                        result_peg_328.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_328.getPosition()))){
                                                result_peg_328.nextPosition();
                                            } else {
                                                result_peg_328.setPosition(position_peg_331);
                                                goto out_peg_332;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_329;
                                    out_peg_332:
                                    {
                                        int position_peg_334 = result_peg_328.getPosition();
                                        
                                        result_peg_328.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_328.getPosition()))){
                                                result_peg_328.nextPosition();
                                            } else {
                                                result_peg_328.setPosition(position_peg_334);
                                                goto out_peg_335;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_329;
                                    out_peg_335:
                                    {
                                        int position_peg_337 = result_peg_328.getPosition();
                                        
                                        {
                                            int position_peg_385 = result_peg_328.getPosition();
                                            {
                                            
                                                result_peg_328.setValue(Value((void*) ";"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_328.getPosition()))){
                                                            result_peg_328.nextPosition();
                                                        } else {
                                                            result_peg_328.setPosition(position_peg_385);
                                                            goto out_peg_387;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_328.reset();
                                                    do{
                                                        Result result_peg_396(result_peg_328.getPosition());
                                                        {
                                                        
                                                            Result result_peg_399(result_peg_396);
                                                                result_peg_399.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_399.getPosition()))){
                                                                        result_peg_399.nextPosition();
                                                                    } else {
                                                                        goto not_peg_398;
                                                                    }
                                                                }
                                                                goto loop_peg_395;
                                                                not_peg_398:
                                                                result_peg_396.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_400 = stream.get(result_peg_396.getPosition());
                                                                if (temp_peg_400 != '\0'){
                                                                    result_peg_396.setValue(Value((void*) (long) temp_peg_400));
                                                                    result_peg_396.nextPosition();
                                                                } else {
                                                                    goto loop_peg_395;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_328.addResult(result_peg_396);
                                                    } while (true);
                                                    loop_peg_395:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_383;
                                        out_peg_387:
                                        {
                                            int position_peg_402 = result_peg_328.getPosition();
                                            {
                                            
                                                result_peg_328.setValue(Value((void*) "="));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_328.getPosition()))){
                                                            result_peg_328.nextPosition();
                                                        } else {
                                                            result_peg_328.setPosition(position_peg_402);
                                                            goto out_peg_404;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_328.reset();
                                                    do{
                                                        Result result_peg_413(result_peg_328.getPosition());
                                                        {
                                                        
                                                            Result result_peg_416(result_peg_413);
                                                                result_peg_416.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_416.getPosition()))){
                                                                        result_peg_416.nextPosition();
                                                                    } else {
                                                                        goto not_peg_415;
                                                                    }
                                                                }
                                                                goto loop_peg_412;
                                                                not_peg_415:
                                                                result_peg_413.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_417 = stream.get(result_peg_413.getPosition());
                                                                if (temp_peg_417 != '\0'){
                                                                    result_peg_413.setValue(Value((void*) (long) temp_peg_417));
                                                                    result_peg_413.nextPosition();
                                                                } else {
                                                                    goto loop_peg_412;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_328.addResult(result_peg_413);
                                                    } while (true);
                                                    loop_peg_412:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_383;
                                        out_peg_404:
                                        {
                                            int position_peg_419 = result_peg_328.getPosition();
                                            {
                                            
                                                result_peg_328.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_328.getPosition()))){
                                                            result_peg_328.nextPosition();
                                                        } else {
                                                            result_peg_328.setPosition(position_peg_419);
                                                            goto out_peg_421;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_328.reset();
                                                    do{
                                                        Result result_peg_426(result_peg_328.getPosition());
                                                        result_peg_426.setValue(Value((void*) "-"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("-"[i], stream.get(result_peg_426.getPosition()))){
                                                                result_peg_426.nextPosition();
                                                            } else {
                                                                goto loop_peg_425;
                                                            }
                                                        }
                                                        result_peg_328.addResult(result_peg_426);
                                                    } while (true);
                                                    loop_peg_425:
                                                    if (result_peg_328.matches() == 0){
                                                        result_peg_328.setPosition(position_peg_419);
                                                        goto out_peg_421;
                                                    }
                                                
                                                
                                            }
                                        }
                                        goto success_peg_383;
                                        out_peg_421:
                                        result_peg_328.setPosition(position_peg_337);
                                        goto out_peg_382;
                                        success_peg_383:
                                        ;
                                        
                                    }
                                    goto success_peg_329;
                                    out_peg_382:
                                    goto loop_peg_327;
                                    success_peg_329:
                                    ;
                                    result_peg_221.addResult(result_peg_328);
                                } while (true);
                                loop_peg_327:
                                ;
                                        
                            }
                            goto success_peg_224;
                            goto loop_peg_220;
                            success_peg_224:
                            ;
                        
                        
                        
                        int save_peg_427 = result_peg_221.getPosition();
                            
                            result_peg_221 = rule_line_end(stream, result_peg_221.getPosition());
                            if (result_peg_221.error()){
                                
                                result_peg_221 = Result(save_peg_427);
                                result_peg_221.setValue(Value((void*) 0));
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_221);
                } while (true);
                loop_peg_220:
                ;
            
            
            
            if ('\0' == stream.get(result_peg_2.getPosition())){
                    result_peg_2.nextPosition();
                    result_peg_2.setValue(Value((void *) '\0'));
                } else {
                    goto out_peg_208;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = current; GC::cleanup(as<SectionList*>(value));
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_208:
    stream.update(errorResult.getPosition());
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    RuleTrace trace_peg_106(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_17(result_peg_2.getPosition());
                        {
                            int position_peg_20 = result_peg_17.getPosition();
                            
                            result_peg_17.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    result_peg_17.setPosition(position_peg_20);
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_18;
                        out_peg_21:
                        {
                            int position_peg_23 = result_peg_17.getPosition();
                            
                            result_peg_17.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    result_peg_17.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_18;
                        out_peg_24:
                        goto loop_peg_16;
                        success_peg_18:
                        ;
                        result_peg_2.addResult(result_peg_17);
                    } while (true);
                    loop_peg_16:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_25;
                success_peg_4:
                ;
            
            
            
            {
                    int position_peg_28 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_28);
                                    goto out_peg_30;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_39(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_42(result_peg_39);
                                        result_peg_42.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
                                                result_peg_42.nextPosition();
                                            } else {
                                                goto not_peg_41;
                                            }
                                        }
                                        goto loop_peg_38;
                                        not_peg_41:
                                        result_peg_39.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_43 = stream.get(result_peg_39.getPosition());
                                        if (temp_peg_43 != '\0'){
                                            result_peg_39.setValue(Value((void*) (long) temp_peg_43));
                                            result_peg_39.nextPosition();
                                        } else {
                                            goto loop_peg_38;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_39);
                            } while (true);
                            loop_peg_38:
                            ;
                        
                        
                    }
                }
                goto success_peg_26;
                out_peg_30:
                {
                    int position_peg_45 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_45);
                                    goto out_peg_47;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_56(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_59(result_peg_56);
                                        result_peg_59.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_59.getPosition()))){
                                                result_peg_59.nextPosition();
                                            } else {
                                                goto not_peg_58;
                                            }
                                        }
                                        goto loop_peg_55;
                                        not_peg_58:
                                        result_peg_56.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_60 = stream.get(result_peg_56.getPosition());
                                        if (temp_peg_60 != '\0'){
                                            result_peg_56.setValue(Value((void*) (long) temp_peg_60));
                                            result_peg_56.nextPosition();
                                        } else {
                                            goto loop_peg_55;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_56);
                            } while (true);
                            loop_peg_55:
                            ;
                        
                        
                    }
                }
                goto success_peg_26;
                out_peg_47:
                {
                    int position_peg_62 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_62);
                                    goto out_peg_64;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_69(result_peg_2.getPosition());
                                result_peg_69.setValue(Value((void*) "-"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_69.getPosition()))){
                                        result_peg_69.nextPosition();
                                    } else {
                                        goto loop_peg_68;
                                    }
                                }
                                result_peg_2.addResult(result_peg_69);
                            } while (true);
                            loop_peg_68:
                            if (result_peg_2.matches() == 0){
                                result_peg_2.setPosition(position_peg_62);
                                goto out_peg_64;
                            }
                        
                        
                    }
                }
                goto success_peg_26;
                out_peg_64:
                goto out_peg_25;
                success_peg_26:
                ;
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_25:
        Result result_peg_70(myposition);
        
        {
        
            {
                    int position_peg_74 = result_peg_70.getPosition();
                    
                    result_peg_70.reset();
                    do{
                        Result result_peg_85(result_peg_70.getPosition());
                        {
                            int position_peg_88 = result_peg_85.getPosition();
                            
                            result_peg_85.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_85.getPosition()))){
                                    result_peg_85.nextPosition();
                                } else {
                                    result_peg_85.setPosition(position_peg_88);
                                    goto out_peg_89;
                                }
                            }
                                
                        }
                        goto success_peg_86;
                        out_peg_89:
                        {
                            int position_peg_91 = result_peg_85.getPosition();
                            
                            result_peg_85.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_85.getPosition()))){
                                    result_peg_85.nextPosition();
                                } else {
                                    result_peg_85.setPosition(position_peg_91);
                                    goto out_peg_92;
                                }
                            }
                                
                        }
                        goto success_peg_86;
                        out_peg_92:
                        goto loop_peg_84;
                        success_peg_86:
                        ;
                        result_peg_70.addResult(result_peg_85);
                    } while (true);
                    loop_peg_84:
                    ;
                            
                }
                goto success_peg_72;
                goto out_peg_93;
                success_peg_72:
                ;
            
            
            
            result_peg_70 = rule_section(stream, result_peg_70.getPosition());
                if (result_peg_70.error()){
                    goto out_peg_93;
                }
            
            Result result_peg_94 = result_peg_70;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_94.getValues());
                    result_peg_70.setValue(value);
                }
            
            
        }
        stream.update(result_peg_70.getPosition());
        
        return result_peg_70;
        out_peg_93:
        Result result_peg_95(myposition);
        
        result_peg_95.reset();
        do{
            Result result_peg_97(result_peg_95.getPosition());
            {
                int position_peg_100 = result_peg_97.getPosition();
                
                result_peg_97.setValue(Value((void*) " "));
                for (int i = 0; i < 1; i++){
                    if (compareChar(" "[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        result_peg_97.setPosition(position_peg_100);
                        goto out_peg_101;
                    }
                }
                    
            }
            goto success_peg_98;
            out_peg_101:
            {
                int position_peg_103 = result_peg_97.getPosition();
                
                result_peg_97.setValue(Value((void*) "\t"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("\t"[i], stream.get(result_peg_97.getPosition()))){
                        result_peg_97.nextPosition();
                    } else {
                        result_peg_97.setPosition(position_peg_103);
                        goto out_peg_104;
                    }
                }
                    
            }
            goto success_peg_98;
            out_peg_104:
            goto loop_peg_96;
            success_peg_98:
            ;
            result_peg_95.addResult(result_peg_97);
        } while (true);
        loop_peg_96:
        if (result_peg_95.matches() == 0){
            goto out_peg_105;
        }
        stream.update(result_peg_95.getPosition());
        
        return result_peg_95;
        out_peg_105:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
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
        stream.update(result_peg_13.getPosition());
        
        return result_peg_13;
        out_peg_16:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    RuleTrace trace_peg_430(stream, "section");
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
                    int position_peg_11 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_113(result_peg_2.getPosition());
                        {
                            int position_peg_116 = result_peg_113.getPosition();
                            
                            result_peg_113.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_113.getPosition()))){
                                    result_peg_113.nextPosition();
                                } else {
                                    result_peg_113.setPosition(position_peg_116);
                                    goto out_peg_117;
                                }
                            }
                                
                        }
                        goto success_peg_114;
                        out_peg_117:
                        {
                            int position_peg_119 = result_peg_113.getPosition();
                            
                            result_peg_113.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_113.getPosition()))){
                                    result_peg_113.nextPosition();
                                } else {
                                    result_peg_113.setPosition(position_peg_119);
                                    goto out_peg_120;
                                }
                            }
                                
                        }
                        goto success_peg_114;
                        out_peg_120:
                        {
                            int position_peg_122 = result_peg_113.getPosition();
                            
                            {
                                int position_peg_170 = result_peg_113.getPosition();
                                {
                                
                                    result_peg_113.setValue(Value((void*) ";"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_113.getPosition()))){
                                                result_peg_113.nextPosition();
                                            } else {
                                                result_peg_113.setPosition(position_peg_170);
                                                goto out_peg_172;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_113.reset();
                                        do{
                                            Result result_peg_181(result_peg_113.getPosition());
                                            {
                                            
                                                Result result_peg_184(result_peg_181);
                                                    result_peg_184.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_184.getPosition()))){
                                                            result_peg_184.nextPosition();
                                                        } else {
                                                            goto not_peg_183;
                                                        }
                                                    }
                                                    goto loop_peg_180;
                                                    not_peg_183:
                                                    result_peg_181.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_185 = stream.get(result_peg_181.getPosition());
                                                    if (temp_peg_185 != '\0'){
                                                        result_peg_181.setValue(Value((void*) (long) temp_peg_185));
                                                        result_peg_181.nextPosition();
                                                    } else {
                                                        goto loop_peg_180;
                                                    }
                                                
                                                
                                            }
                                            result_peg_113.addResult(result_peg_181);
                                        } while (true);
                                        loop_peg_180:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_168;
                            out_peg_172:
                            {
                                int position_peg_187 = result_peg_113.getPosition();
                                {
                                
                                    result_peg_113.setValue(Value((void*) "="));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_113.getPosition()))){
                                                result_peg_113.nextPosition();
                                            } else {
                                                result_peg_113.setPosition(position_peg_187);
                                                goto out_peg_189;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_113.reset();
                                        do{
                                            Result result_peg_198(result_peg_113.getPosition());
                                            {
                                            
                                                Result result_peg_201(result_peg_198);
                                                    result_peg_201.setValue(Value((void*) "\n"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_201.getPosition()))){
                                                            result_peg_201.nextPosition();
                                                        } else {
                                                            goto not_peg_200;
                                                        }
                                                    }
                                                    goto loop_peg_197;
                                                    not_peg_200:
                                                    result_peg_198.setValue(Value((void*)0));
                                                
                                                
                                                
                                                char temp_peg_202 = stream.get(result_peg_198.getPosition());
                                                    if (temp_peg_202 != '\0'){
                                                        result_peg_198.setValue(Value((void*) (long) temp_peg_202));
                                                        result_peg_198.nextPosition();
                                                    } else {
                                                        goto loop_peg_197;
                                                    }
                                                
                                                
                                            }
                                            result_peg_113.addResult(result_peg_198);
                                        } while (true);
                                        loop_peg_197:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_168;
                            out_peg_189:
                            {
                                int position_peg_204 = result_peg_113.getPosition();
                                {
                                
                                    result_peg_113.setValue(Value((void*) "-"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_113.getPosition()))){
                                                result_peg_113.nextPosition();
                                            } else {
                                                result_peg_113.setPosition(position_peg_204);
                                                goto out_peg_206;
                                            }
                                        }
                                    
                                    
                                    
                                    result_peg_113.reset();
                                        do{
                                            Result result_peg_211(result_peg_113.getPosition());
                                            result_peg_211.setValue(Value((void*) "-"));
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("-"[i], stream.get(result_peg_211.getPosition()))){
                                                    result_peg_211.nextPosition();
                                                } else {
                                                    goto loop_peg_210;
                                                }
                                            }
                                            result_peg_113.addResult(result_peg_211);
                                        } while (true);
                                        loop_peg_210:
                                        if (result_peg_113.matches() == 0){
                                            result_peg_113.setPosition(position_peg_204);
                                            goto out_peg_206;
                                        }
                                    
                                    
                                }
                            }
                            goto success_peg_168;
                            out_peg_206:
                            result_peg_113.setPosition(position_peg_122);
                            goto out_peg_167;
                            success_peg_168:
                            ;
                            
                        }
                        goto success_peg_114;
                        out_peg_167:
                        goto loop_peg_112;
                        success_peg_114:
                        ;
                        result_peg_2.addResult(result_peg_113);
                    } while (true);
                    loop_peg_112:
                    ;
                            
                }
                goto success_peg_9;
                goto out_peg_6;
                success_peg_9:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_214(result_peg_2.getPosition());
                    {
                        int position_peg_217 = result_peg_214.getPosition();
                        
                        result_peg_214.setValue(Value((void*) "\n"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_214.getPosition()))){
                                result_peg_214.nextPosition();
                            } else {
                                result_peg_214.setPosition(position_peg_217);
                                goto out_peg_218;
                            }
                        }
                            
                    }
                    goto success_peg_215;
                    out_peg_218:
                    {
                        int position_peg_220 = result_peg_214.getPosition();
                        
                        result_peg_214.setValue(Value((void*) "\r"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_214.getPosition()))){
                                result_peg_214.nextPosition();
                            } else {
                                result_peg_214.setPosition(position_peg_220);
                                goto out_peg_221;
                            }
                        }
                            
                    }
                    goto success_peg_215;
                    out_peg_221:
                    goto loop_peg_213;
                    success_peg_215:
                    ;
                    result_peg_2.addResult(result_peg_214);
                } while (true);
                loop_peg_213:
                if (result_peg_2.matches() == 0){
                    goto out_peg_6;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_224(result_peg_2.getPosition());
                    {
                    
                        result_peg_224 = rule_section_line(stream, result_peg_224.getPosition(), ast);
                            if (result_peg_224.error()){
                                goto loop_peg_223;
                            }
                        
                        
                        
                        {
                                int position_peg_229 = result_peg_224.getPosition();
                                
                                result_peg_224.reset();
                                do{
                                    Result result_peg_331(result_peg_224.getPosition());
                                    {
                                        int position_peg_334 = result_peg_331.getPosition();
                                        
                                        result_peg_331.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_331.getPosition()))){
                                                result_peg_331.nextPosition();
                                            } else {
                                                result_peg_331.setPosition(position_peg_334);
                                                goto out_peg_335;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_332;
                                    out_peg_335:
                                    {
                                        int position_peg_337 = result_peg_331.getPosition();
                                        
                                        result_peg_331.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_331.getPosition()))){
                                                result_peg_331.nextPosition();
                                            } else {
                                                result_peg_331.setPosition(position_peg_337);
                                                goto out_peg_338;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_332;
                                    out_peg_338:
                                    {
                                        int position_peg_340 = result_peg_331.getPosition();
                                        
                                        {
                                            int position_peg_388 = result_peg_331.getPosition();
                                            {
                                            
                                                result_peg_331.setValue(Value((void*) ";"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_331.getPosition()))){
                                                            result_peg_331.nextPosition();
                                                        } else {
                                                            result_peg_331.setPosition(position_peg_388);
                                                            goto out_peg_390;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_331.reset();
                                                    do{
                                                        Result result_peg_399(result_peg_331.getPosition());
                                                        {
                                                        
                                                            Result result_peg_402(result_peg_399);
                                                                result_peg_402.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_402.getPosition()))){
                                                                        result_peg_402.nextPosition();
                                                                    } else {
                                                                        goto not_peg_401;
                                                                    }
                                                                }
                                                                goto loop_peg_398;
                                                                not_peg_401:
                                                                result_peg_399.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_403 = stream.get(result_peg_399.getPosition());
                                                                if (temp_peg_403 != '\0'){
                                                                    result_peg_399.setValue(Value((void*) (long) temp_peg_403));
                                                                    result_peg_399.nextPosition();
                                                                } else {
                                                                    goto loop_peg_398;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_331.addResult(result_peg_399);
                                                    } while (true);
                                                    loop_peg_398:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_386;
                                        out_peg_390:
                                        {
                                            int position_peg_405 = result_peg_331.getPosition();
                                            {
                                            
                                                result_peg_331.setValue(Value((void*) "="));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_331.getPosition()))){
                                                            result_peg_331.nextPosition();
                                                        } else {
                                                            result_peg_331.setPosition(position_peg_405);
                                                            goto out_peg_407;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_331.reset();
                                                    do{
                                                        Result result_peg_416(result_peg_331.getPosition());
                                                        {
                                                        
                                                            Result result_peg_419(result_peg_416);
                                                                result_peg_419.setValue(Value((void*) "\n"));
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_419.getPosition()))){
                                                                        result_peg_419.nextPosition();
                                                                    } else {
                                                                        goto not_peg_418;
                                                                    }
                                                                }
                                                                goto loop_peg_415;
                                                                not_peg_418:
                                                                result_peg_416.setValue(Value((void*)0));
                                                            
                                                            
                                                            
                                                            char temp_peg_420 = stream.get(result_peg_416.getPosition());
                                                                if (temp_peg_420 != '\0'){
                                                                    result_peg_416.setValue(Value((void*) (long) temp_peg_420));
                                                                    result_peg_416.nextPosition();
                                                                } else {
                                                                    goto loop_peg_415;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_331.addResult(result_peg_416);
                                                    } while (true);
                                                    loop_peg_415:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_386;
                                        out_peg_407:
                                        {
                                            int position_peg_422 = result_peg_331.getPosition();
                                            {
                                            
                                                result_peg_331.setValue(Value((void*) "-"));
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_331.getPosition()))){
                                                            result_peg_331.nextPosition();
                                                        } else {
                                                            result_peg_331.setPosition(position_peg_422);
                                                            goto out_peg_424;
                                                        }
                                                    }
                                                
                                                
                                                
                                                result_peg_331.reset();
                                                    do{
                                                        Result result_peg_429(result_peg_331.getPosition());
                                                        result_peg_429.setValue(Value((void*) "-"));
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("-"[i], stream.get(result_peg_429.getPosition()))){
                                                                result_peg_429.nextPosition();
                                                            } else {
                                                                goto loop_peg_428;
                                                            }
                                                        }
                                                        result_peg_331.addResult(result_peg_429);
                                                    } while (true);
                                                    loop_peg_428:
                                                    if (result_peg_331.matches() == 0){
                                                        result_peg_331.setPosition(position_peg_422);
                                                        goto out_peg_424;
                                                    }
                                                
                                                
                                            }
                                        }
                                        goto success_peg_386;
                                        out_peg_424:
                                        result_peg_331.setPosition(position_peg_340);
                                        goto out_peg_385;
                                        success_peg_386:
                                        ;
                                        
                                    }
                                    goto success_peg_332;
                                    out_peg_385:
                                    goto loop_peg_330;
                                    success_peg_332:
                                    ;
                                    result_peg_224.addResult(result_peg_331);
                                } while (true);
                                loop_peg_330:
                                ;
                                        
                            }
                            goto success_peg_227;
                            goto loop_peg_223;
                            success_peg_227:
                            ;
                        
                        
                        
                        result_peg_224 = rule_line_end(stream, result_peg_224.getPosition());
                            if (result_peg_224.error()){
                                goto loop_peg_223;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_224);
                } while (true);
                loop_peg_223:
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
    
    RuleTrace trace_peg_165(stream, "section_line");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
        
            {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_17(result_peg_2.getPosition());
                        {
                            int position_peg_20 = result_peg_17.getPosition();
                            
                            result_peg_17.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    result_peg_17.setPosition(position_peg_20);
                                    goto out_peg_21;
                                }
                            }
                                
                        }
                        goto success_peg_18;
                        out_peg_21:
                        {
                            int position_peg_23 = result_peg_17.getPosition();
                            
                            result_peg_17.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_17.getPosition()))){
                                    result_peg_17.nextPosition();
                                } else {
                                    result_peg_17.setPosition(position_peg_23);
                                    goto out_peg_24;
                                }
                            }
                                
                        }
                        goto success_peg_18;
                        out_peg_24:
                        goto loop_peg_16;
                        success_peg_18:
                        ;
                        result_peg_2.addResult(result_peg_17);
                    } while (true);
                    loop_peg_16:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_25;
                success_peg_4:
                ;
            
            
            
            {
                    int position_peg_28 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_28);
                                    goto out_peg_30;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_39(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_42(result_peg_39);
                                        result_peg_42.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
                                                result_peg_42.nextPosition();
                                            } else {
                                                goto not_peg_41;
                                            }
                                        }
                                        goto loop_peg_38;
                                        not_peg_41:
                                        result_peg_39.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_43 = stream.get(result_peg_39.getPosition());
                                        if (temp_peg_43 != '\0'){
                                            result_peg_39.setValue(Value((void*) (long) temp_peg_43));
                                            result_peg_39.nextPosition();
                                        } else {
                                            goto loop_peg_38;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_39);
                            } while (true);
                            loop_peg_38:
                            ;
                        
                        
                    }
                }
                goto success_peg_26;
                out_peg_30:
                {
                    int position_peg_45 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_45);
                                    goto out_peg_47;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_56(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_59(result_peg_56);
                                        result_peg_59.setValue(Value((void*) "\n"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_59.getPosition()))){
                                                result_peg_59.nextPosition();
                                            } else {
                                                goto not_peg_58;
                                            }
                                        }
                                        goto loop_peg_55;
                                        not_peg_58:
                                        result_peg_56.setValue(Value((void*)0));
                                    
                                    
                                    
                                    char temp_peg_60 = stream.get(result_peg_56.getPosition());
                                        if (temp_peg_60 != '\0'){
                                            result_peg_56.setValue(Value((void*) (long) temp_peg_60));
                                            result_peg_56.nextPosition();
                                        } else {
                                            goto loop_peg_55;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_56);
                            } while (true);
                            loop_peg_55:
                            ;
                        
                        
                    }
                }
                goto success_peg_26;
                out_peg_47:
                {
                    int position_peg_62 = result_peg_2.getPosition();
                    {
                    
                        result_peg_2.setValue(Value((void*) "-"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    result_peg_2.setPosition(position_peg_62);
                                    goto out_peg_64;
                                }
                            }
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_69(result_peg_2.getPosition());
                                result_peg_69.setValue(Value((void*) "-"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_69.getPosition()))){
                                        result_peg_69.nextPosition();
                                    } else {
                                        goto loop_peg_68;
                                    }
                                }
                                result_peg_2.addResult(result_peg_69);
                            } while (true);
                            loop_peg_68:
                            if (result_peg_2.matches() == 0){
                                result_peg_2.setPosition(position_peg_62);
                                goto out_peg_64;
                            }
                        
                        
                    }
                }
                goto success_peg_26;
                out_peg_64:
                goto out_peg_25;
                success_peg_26:
                ;
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_25:
        Result result_peg_70(myposition);
        
        {
        
            {
                    int position_peg_74 = result_peg_70.getPosition();
                    
                    result_peg_70.reset();
                    do{
                        Result result_peg_85(result_peg_70.getPosition());
                        {
                            int position_peg_88 = result_peg_85.getPosition();
                            
                            result_peg_85.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_85.getPosition()))){
                                    result_peg_85.nextPosition();
                                } else {
                                    result_peg_85.setPosition(position_peg_88);
                                    goto out_peg_89;
                                }
                            }
                                
                        }
                        goto success_peg_86;
                        out_peg_89:
                        {
                            int position_peg_91 = result_peg_85.getPosition();
                            
                            result_peg_85.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_85.getPosition()))){
                                    result_peg_85.nextPosition();
                                } else {
                                    result_peg_85.setPosition(position_peg_91);
                                    goto out_peg_92;
                                }
                            }
                                
                        }
                        goto success_peg_86;
                        out_peg_92:
                        goto loop_peg_84;
                        success_peg_86:
                        ;
                        result_peg_70.addResult(result_peg_85);
                    } while (true);
                    loop_peg_84:
                    ;
                            
                }
                goto success_peg_72;
                goto out_peg_93;
                success_peg_72:
                ;
            
            
            
            result_peg_70 = rule_attribute(stream, result_peg_70.getPosition());
                if (result_peg_70.error()){
                    goto out_peg_93;
                }
                data = result_peg_70.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addAttribute(asAttribute(data));
                    result_peg_70.setValue(value);
                }
            
            
        }
        stream.update(result_peg_70.getPosition());
        
        return result_peg_70;
        out_peg_93:
        Result result_peg_95(myposition);
        
        {
        
            {
                    int position_peg_99 = result_peg_95.getPosition();
                    
                    result_peg_95.reset();
                    do{
                        Result result_peg_110(result_peg_95.getPosition());
                        {
                            int position_peg_113 = result_peg_110.getPosition();
                            
                            result_peg_110.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_110.getPosition()))){
                                    result_peg_110.nextPosition();
                                } else {
                                    result_peg_110.setPosition(position_peg_113);
                                    goto out_peg_114;
                                }
                            }
                                
                        }
                        goto success_peg_111;
                        out_peg_114:
                        {
                            int position_peg_116 = result_peg_110.getPosition();
                            
                            result_peg_110.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_110.getPosition()))){
                                    result_peg_110.nextPosition();
                                } else {
                                    result_peg_110.setPosition(position_peg_116);
                                    goto out_peg_117;
                                }
                            }
                                
                        }
                        goto success_peg_111;
                        out_peg_117:
                        goto loop_peg_109;
                        success_peg_111:
                        ;
                        result_peg_95.addResult(result_peg_110);
                    } while (true);
                    loop_peg_109:
                    ;
                            
                }
                goto success_peg_97;
                goto out_peg_118;
                success_peg_97:
                ;
            
            
            
            {
                    int position_peg_122 = result_peg_95.getPosition();
                    {
                    
                        result_peg_95.setValue(Value((void*) "loopstart"));
                            for (int i = 0; i < 9; i++){
                                if (compareCharCase("loopstart"[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    result_peg_95.setPosition(position_peg_122);
                                    goto out_peg_124;
                                }
                            }
                        
                        
                        
                        {
                                Value value((void*) 0);
                                value = makeValue();
                                result_peg_95.setValue(value);
                            }
                        
                        
                    }
                }
                goto success_peg_120;
                out_peg_124:
                goto out_peg_118;
                success_peg_120:
                ;
                data = result_peg_95.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_95.setValue(value);
                }
            
            
        }
        stream.update(result_peg_95.getPosition());
        
        return result_peg_95;
        out_peg_118:
        Result result_peg_126(myposition);
        
        {
        
            {
                    int position_peg_130 = result_peg_126.getPosition();
                    
                    result_peg_126.reset();
                    do{
                        Result result_peg_141(result_peg_126.getPosition());
                        {
                            int position_peg_144 = result_peg_141.getPosition();
                            
                            result_peg_141.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_141.getPosition()))){
                                    result_peg_141.nextPosition();
                                } else {
                                    result_peg_141.setPosition(position_peg_144);
                                    goto out_peg_145;
                                }
                            }
                                
                        }
                        goto success_peg_142;
                        out_peg_145:
                        {
                            int position_peg_147 = result_peg_141.getPosition();
                            
                            result_peg_141.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_141.getPosition()))){
                                    result_peg_141.nextPosition();
                                } else {
                                    result_peg_141.setPosition(position_peg_147);
                                    goto out_peg_148;
                                }
                            }
                                
                        }
                        goto success_peg_142;
                        out_peg_148:
                        goto loop_peg_140;
                        success_peg_142:
                        ;
                        result_peg_126.addResult(result_peg_141);
                    } while (true);
                    loop_peg_140:
                    ;
                            
                }
                goto success_peg_128;
                goto out_peg_149;
                success_peg_128:
                ;
            
            
            
            result_peg_126 = rule_valuelist(stream, result_peg_126.getPosition());
                if (result_peg_126.error()){
                    goto out_peg_149;
                }
                data = result_peg_126.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_126.setValue(value);
                }
            
            
        }
        stream.update(result_peg_126.getPosition());
        
        return result_peg_126;
        out_peg_149:
        Result result_peg_151(myposition);
        
        {
        
            result_peg_151.reset();
                do{
                    Result result_peg_154(result_peg_151.getPosition());
                    {
                        int position_peg_157 = result_peg_154.getPosition();
                        
                        result_peg_154.setValue(Value((void*) " "));
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_154.getPosition()))){
                                result_peg_154.nextPosition();
                            } else {
                                result_peg_154.setPosition(position_peg_157);
                                goto out_peg_158;
                            }
                        }
                            
                    }
                    goto success_peg_155;
                    out_peg_158:
                    {
                        int position_peg_160 = result_peg_154.getPosition();
                        
                        result_peg_154.setValue(Value((void*) "\t"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_154.getPosition()))){
                                result_peg_154.nextPosition();
                            } else {
                                result_peg_154.setPosition(position_peg_160);
                                goto out_peg_161;
                            }
                        }
                            
                    }
                    goto success_peg_155;
                    out_peg_161:
                    goto loop_peg_153;
                    success_peg_155:
                    ;
                    result_peg_151.addResult(result_peg_154);
                } while (true);
                loop_peg_153:
                if (result_peg_151.matches() == 0){
                    goto out_peg_162;
                }
            
            
            
            Result result_peg_164(result_peg_151);
                result_peg_164.setValue(Value((void*) "["));
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_164.getPosition()))){
                        result_peg_164.nextPosition();
                    } else {
                        goto not_peg_163;
                    }
                }
                goto out_peg_162;
                not_peg_163:
                result_peg_151.setValue(Value((void*)0));
            
            
        }
        stream.update(result_peg_151.getPosition());
        
        return result_peg_151;
        out_peg_162:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    RuleTrace trace_peg_57(stream, "section_start");
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
                    int position_peg_8 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_19(result_peg_2.getPosition());
                        {
                            int position_peg_22 = result_peg_19.getPosition();
                            
                            result_peg_19.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                    result_peg_19.nextPosition();
                                } else {
                                    result_peg_19.setPosition(position_peg_22);
                                    goto out_peg_23;
                                }
                            }
                                
                        }
                        goto success_peg_20;
                        out_peg_23:
                        {
                            int position_peg_25 = result_peg_19.getPosition();
                            
                            result_peg_19.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                    result_peg_19.nextPosition();
                                } else {
                                    result_peg_19.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
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
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_29(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_32(result_peg_29);
                            result_peg_32.setValue(Value((void*) "]"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_32.getPosition()))){
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
                    result_peg_2.addResult(result_peg_29);
                } while (true);
                loop_peg_28:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
                data = result_peg_2.getValues();
            
            
            
            {
                    int position_peg_37 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_48(result_peg_2.getPosition());
                        {
                            int position_peg_51 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_51);
                                    goto out_peg_52;
                                }
                            }
                                
                        }
                        goto success_peg_49;
                        out_peg_52:
                        {
                            int position_peg_54 = result_peg_48.getPosition();
                            
                            result_peg_48.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                                    result_peg_48.nextPosition();
                                } else {
                                    result_peg_48.setPosition(position_peg_54);
                                    goto out_peg_55;
                                }
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
                goto success_peg_35;
                goto out_peg_4;
                success_peg_35:
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
    
    RuleTrace trace_peg_45(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    int position_peg_6 = result_peg_2.getPosition();
                    
                    char letter_peg_9 = stream.get(result_peg_2.getPosition());
                    if (letter_peg_9 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_9) != NULL){
                        result_peg_2.nextPosition();
                        result_peg_2.setValue(Value((void*) (long) letter_peg_9));
                    } else {
                        result_peg_2.setPosition(position_peg_6);
                        goto out_peg_8;
                    }
                    
                }
                goto success_peg_4;
                out_peg_8:
                goto out_peg_10;
                success_peg_4:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_13(result_peg_2.getPosition());
                    {
                        int position_peg_16 = result_peg_13.getPosition();
                        
                        {
                            int position_peg_26 = result_peg_13.getPosition();
                            
                            char letter_peg_29 = stream.get(result_peg_13.getPosition());
                            if (letter_peg_29 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_29) != NULL){
                                result_peg_13.nextPosition();
                                result_peg_13.setValue(Value((void*) (long) letter_peg_29));
                            } else {
                                result_peg_13.setPosition(position_peg_26);
                                goto out_peg_28;
                            }
                            
                        }
                        goto success_peg_24;
                        out_peg_28:
                        result_peg_13.setPosition(position_peg_16);
                        goto out_peg_23;
                        success_peg_24:
                        ;
                        
                    }
                    goto success_peg_14;
                    out_peg_23:
                    {
                        int position_peg_31 = result_peg_13.getPosition();
                        
                        {
                            int position_peg_41 = result_peg_13.getPosition();
                            
                            char letter_peg_44 = stream.get(result_peg_13.getPosition());
                            if (letter_peg_44 != '\0' && strchr("0123456789", letter_peg_44) != NULL){
                                result_peg_13.nextPosition();
                                result_peg_13.setValue(Value((void*) (long) letter_peg_44));
                            } else {
                                result_peg_13.setPosition(position_peg_41);
                                goto out_peg_43;
                            }
                            
                        }
                        goto success_peg_39;
                        out_peg_43:
                        result_peg_13.setPosition(position_peg_31);
                        goto out_peg_38;
                        success_peg_39:
                        ;
                        
                    }
                    goto success_peg_14;
                    out_peg_38:
                    goto loop_peg_12;
                    success_peg_14:
                    ;
                    result_peg_2.addResult(result_peg_13);
                } while (true);
                loop_peg_12:
                ;
            
            Result result_peg_11 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_3.getValues().getValue(),result_peg_11.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_10:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    RuleTrace trace_peg_50(stream, "line_end_or_comment");
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
            int position_peg_7 = result_peg_4.getPosition();
            {
            
                result_peg_4.setValue(Value((void*) ";"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar(";"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_7);
                            goto out_peg_9;
                        }
                    }
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_18(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_21(result_peg_18);
                                result_peg_21.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_21.getPosition()))){
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
                                    result_peg_18.setValue(Value((void*) (long) temp_peg_22));
                                    result_peg_18.nextPosition();
                                } else {
                                    goto loop_peg_17;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_18);
                    } while (true);
                    loop_peg_17:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_9:
        {
            int position_peg_24 = result_peg_4.getPosition();
            {
            
                result_peg_4.setValue(Value((void*) "="));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("="[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_24);
                            goto out_peg_26;
                        }
                    }
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_35(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_38(result_peg_35);
                                result_peg_38.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_38.getPosition()))){
                                        result_peg_38.nextPosition();
                                    } else {
                                        goto not_peg_37;
                                    }
                                }
                                goto loop_peg_34;
                                not_peg_37:
                                result_peg_35.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_39 = stream.get(result_peg_35.getPosition());
                                if (temp_peg_39 != '\0'){
                                    result_peg_35.setValue(Value((void*) (long) temp_peg_39));
                                    result_peg_35.nextPosition();
                                } else {
                                    goto loop_peg_34;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_35);
                    } while (true);
                    loop_peg_34:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_26:
        {
            int position_peg_41 = result_peg_4.getPosition();
            {
            
                result_peg_4.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            result_peg_4.setPosition(position_peg_41);
                            goto out_peg_43;
                        }
                    }
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_48(result_peg_4.getPosition());
                        result_peg_48.setValue(Value((void*) "-"));
                        for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_48.getPosition()))){
                                result_peg_48.nextPosition();
                            } else {
                                goto loop_peg_47;
                            }
                        }
                        result_peg_4.addResult(result_peg_48);
                    } while (true);
                    loop_peg_47:
                    if (result_peg_4.matches() == 0){
                        result_peg_4.setPosition(position_peg_41);
                        goto out_peg_43;
                    }
                
                
            }
        }
        goto success_peg_5;
        out_peg_43:
        goto out_peg_49;
        success_peg_5:
        ;
        stream.update(result_peg_4.getPosition());
        
        return result_peg_4;
        out_peg_49:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    RuleTrace trace_peg_54(stream, "filename");
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
                        int position_peg_14 = result_peg_11.getPosition();
                        {
                        
                            Result result_peg_37(result_peg_11);
                                result_peg_37.setValue(Value((void*) ","));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(","[i], stream.get(result_peg_37.getPosition()))){
                                        result_peg_37.nextPosition();
                                    } else {
                                        goto not_peg_36;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_36:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_40(result_peg_11);
                                result_peg_40.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_40.getPosition()))){
                                        result_peg_40.nextPosition();
                                    } else {
                                        goto not_peg_39;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_39:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_43(result_peg_11);
                                result_peg_43.setValue(Value((void*) "["));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("["[i], stream.get(result_peg_43.getPosition()))){
                                        result_peg_43.nextPosition();
                                    } else {
                                        goto not_peg_42;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_42:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_46(result_peg_11);
                                result_peg_46.setValue(Value((void*) 13));
                                if ((unsigned char) stream.get(result_peg_46.getPosition()) == (unsigned char) 13){
                                    result_peg_46.nextPosition();
                                } else {
                                    goto not_peg_45;
                                }
                                result_peg_11.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_45:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_49(result_peg_11);
                                result_peg_49.setValue(Value((void*) "="));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("="[i], stream.get(result_peg_49.getPosition()))){
                                        result_peg_49.nextPosition();
                                    } else {
                                        goto not_peg_48;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_48:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_52(result_peg_11);
                                result_peg_52.setValue(Value((void*) ";"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(";"[i], stream.get(result_peg_52.getPosition()))){
                                        result_peg_52.nextPosition();
                                    } else {
                                        goto not_peg_51;
                                    }
                                }
                                result_peg_11.setPosition(position_peg_14);
                                goto out_peg_18;
                                not_peg_51:
                                result_peg_11.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_53 = stream.get(result_peg_11.getPosition());
                                if (temp_peg_53 != '\0'){
                                    result_peg_11.setValue(Value((void*) (long) temp_peg_53));
                                    result_peg_11.nextPosition();
                                } else {
                                    result_peg_11.setPosition(position_peg_14);
                                    goto out_peg_18;
                                }
                            
                            
                        }
                    }
                    goto success_peg_12;
                    out_peg_18:
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
    
    RuleTrace trace_peg_274(stream, "attribute");
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
                    int position_peg_10 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_21(result_peg_2.getPosition());
                        {
                            int position_peg_24 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_24);
                                    goto out_peg_25;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_25:
                        {
                            int position_peg_27 = result_peg_21.getPosition();
                            
                            result_peg_21.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                    result_peg_21.nextPosition();
                                } else {
                                    result_peg_21.setPosition(position_peg_27);
                                    goto out_peg_28;
                                }
                            }
                                
                        }
                        goto success_peg_22;
                        out_peg_28:
                        goto loop_peg_20;
                        success_peg_22:
                        ;
                        result_peg_2.addResult(result_peg_21);
                    } while (true);
                    loop_peg_20:
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
                    int position_peg_33 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_44(result_peg_2.getPosition());
                        {
                            int position_peg_47 = result_peg_44.getPosition();
                            
                            result_peg_44.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_44.getPosition()))){
                                    result_peg_44.nextPosition();
                                } else {
                                    result_peg_44.setPosition(position_peg_47);
                                    goto out_peg_48;
                                }
                            }
                                
                        }
                        goto success_peg_45;
                        out_peg_48:
                        {
                            int position_peg_50 = result_peg_44.getPosition();
                            
                            result_peg_44.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_44.getPosition()))){
                                    result_peg_44.nextPosition();
                                } else {
                                    result_peg_44.setPosition(position_peg_50);
                                    goto out_peg_51;
                                }
                            }
                                
                        }
                        goto success_peg_45;
                        out_peg_51:
                        goto loop_peg_43;
                        success_peg_45:
                        ;
                        result_peg_2.addResult(result_peg_44);
                    } while (true);
                    loop_peg_43:
                    ;
                            
                }
                goto success_peg_31;
                goto out_peg_6;
                success_peg_31:
                ;
            
            
            
            Result result_peg_53(result_peg_2.getPosition());
                result_peg_53 = rule_line_end_or_comment(stream, result_peg_53.getPosition());
                if (result_peg_53.error()){
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
        Result result_peg_54(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_56 = stream.getLineInfo(result_peg_54.getPosition());
                line = &line_info_peg_56;
            
            
            
            result_peg_54 = rule_identifier(stream, result_peg_54.getPosition());
                if (result_peg_54.error()){
                    goto out_peg_58;
                }
                id = result_peg_54.getValues();
            
            
            
            {
                    int position_peg_62 = result_peg_54.getPosition();
                    
                    result_peg_54.reset();
                    do{
                        Result result_peg_73(result_peg_54.getPosition());
                        {
                            int position_peg_76 = result_peg_73.getPosition();
                            
                            result_peg_73.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    result_peg_73.setPosition(position_peg_76);
                                    goto out_peg_77;
                                }
                            }
                                
                        }
                        goto success_peg_74;
                        out_peg_77:
                        {
                            int position_peg_79 = result_peg_73.getPosition();
                            
                            result_peg_73.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_73.getPosition()))){
                                    result_peg_73.nextPosition();
                                } else {
                                    result_peg_73.setPosition(position_peg_79);
                                    goto out_peg_80;
                                }
                            }
                                
                        }
                        goto success_peg_74;
                        out_peg_80:
                        goto loop_peg_72;
                        success_peg_74:
                        ;
                        result_peg_54.addResult(result_peg_73);
                    } while (true);
                    loop_peg_72:
                    ;
                            
                }
                goto success_peg_60;
                goto out_peg_58;
                success_peg_60:
                ;
            
            
            
            result_peg_54.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_54.getPosition()))){
                        result_peg_54.nextPosition();
                    } else {
                        goto out_peg_58;
                    }
                }
            
            
            
            {
                    int position_peg_85 = result_peg_54.getPosition();
                    
                    result_peg_54.reset();
                    do{
                        Result result_peg_96(result_peg_54.getPosition());
                        {
                            int position_peg_99 = result_peg_96.getPosition();
                            
                            result_peg_96.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    result_peg_96.setPosition(position_peg_99);
                                    goto out_peg_100;
                                }
                            }
                                
                        }
                        goto success_peg_97;
                        out_peg_100:
                        {
                            int position_peg_102 = result_peg_96.getPosition();
                            
                            result_peg_96.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_96.getPosition()))){
                                    result_peg_96.nextPosition();
                                } else {
                                    result_peg_96.setPosition(position_peg_102);
                                    goto out_peg_103;
                                }
                            }
                                
                        }
                        goto success_peg_97;
                        out_peg_103:
                        goto loop_peg_95;
                        success_peg_97:
                        ;
                        result_peg_54.addResult(result_peg_96);
                    } while (true);
                    loop_peg_95:
                    ;
                            
                }
                goto success_peg_83;
                goto out_peg_58;
                success_peg_83:
                ;
            
            
            
            result_peg_54 = rule_valuelist(stream, result_peg_54.getPosition());
                if (result_peg_54.error()){
                    goto out_peg_58;
                }
                data = result_peg_54.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(line, id, data);
                    result_peg_54.setValue(value);
                }
            
            
        }
        stream.update(result_peg_54.getPosition());
        
        return result_peg_54;
        out_peg_58:
        Result result_peg_105(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_107 = stream.getLineInfo(result_peg_105.getPosition());
                line = &line_info_peg_107;
            
            
            
            result_peg_105 = rule_identifier(stream, result_peg_105.getPosition());
                if (result_peg_105.error()){
                    goto out_peg_109;
                }
                id = result_peg_105.getValues();
            
            
            
            {
                    int position_peg_113 = result_peg_105.getPosition();
                    
                    result_peg_105.reset();
                    do{
                        Result result_peg_124(result_peg_105.getPosition());
                        {
                            int position_peg_127 = result_peg_124.getPosition();
                            
                            result_peg_124.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_124.getPosition()))){
                                    result_peg_124.nextPosition();
                                } else {
                                    result_peg_124.setPosition(position_peg_127);
                                    goto out_peg_128;
                                }
                            }
                                
                        }
                        goto success_peg_125;
                        out_peg_128:
                        {
                            int position_peg_130 = result_peg_124.getPosition();
                            
                            result_peg_124.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_124.getPosition()))){
                                    result_peg_124.nextPosition();
                                } else {
                                    result_peg_124.setPosition(position_peg_130);
                                    goto out_peg_131;
                                }
                            }
                                
                        }
                        goto success_peg_125;
                        out_peg_131:
                        goto loop_peg_123;
                        success_peg_125:
                        ;
                        result_peg_105.addResult(result_peg_124);
                    } while (true);
                    loop_peg_123:
                    ;
                            
                }
                goto success_peg_111;
                goto out_peg_109;
                success_peg_111:
                ;
            
            
            
            result_peg_105.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_105.getPosition()))){
                        result_peg_105.nextPosition();
                    } else {
                        goto out_peg_109;
                    }
                }
            
            
            
            {
                    int position_peg_136 = result_peg_105.getPosition();
                    
                    result_peg_105.reset();
                    do{
                        Result result_peg_147(result_peg_105.getPosition());
                        {
                            int position_peg_150 = result_peg_147.getPosition();
                            
                            result_peg_147.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_147.getPosition()))){
                                    result_peg_147.nextPosition();
                                } else {
                                    result_peg_147.setPosition(position_peg_150);
                                    goto out_peg_151;
                                }
                            }
                                
                        }
                        goto success_peg_148;
                        out_peg_151:
                        {
                            int position_peg_153 = result_peg_147.getPosition();
                            
                            result_peg_147.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_147.getPosition()))){
                                    result_peg_147.nextPosition();
                                } else {
                                    result_peg_147.setPosition(position_peg_153);
                                    goto out_peg_154;
                                }
                            }
                                
                        }
                        goto success_peg_148;
                        out_peg_154:
                        goto loop_peg_146;
                        success_peg_148:
                        ;
                        result_peg_105.addResult(result_peg_147);
                    } while (true);
                    loop_peg_146:
                    ;
                            
                }
                goto success_peg_134;
                goto out_peg_109;
                success_peg_134:
                ;
            
            
            
            result_peg_105 = rule_filename(stream, result_peg_105.getPosition());
                if (result_peg_105.error()){
                    goto out_peg_109;
                }
                data = result_peg_105.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(line, id, data);
                    result_peg_105.setValue(value);
                }
            
            
        }
        stream.update(result_peg_105.getPosition());
        
        return result_peg_105;
        out_peg_109:
        Result result_peg_156(myposition);
        
        {
        
            result_peg_156 = rule_identifier(stream, result_peg_156.getPosition());
                if (result_peg_156.error()){
                    goto out_peg_158;
                }
                id = result_peg_156.getValues();
            
            
            
            {
                    int position_peg_162 = result_peg_156.getPosition();
                    
                    result_peg_156.reset();
                    do{
                        Result result_peg_173(result_peg_156.getPosition());
                        {
                            int position_peg_176 = result_peg_173.getPosition();
                            
                            result_peg_173.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_173.getPosition()))){
                                    result_peg_173.nextPosition();
                                } else {
                                    result_peg_173.setPosition(position_peg_176);
                                    goto out_peg_177;
                                }
                            }
                                
                        }
                        goto success_peg_174;
                        out_peg_177:
                        {
                            int position_peg_179 = result_peg_173.getPosition();
                            
                            result_peg_173.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_173.getPosition()))){
                                    result_peg_173.nextPosition();
                                } else {
                                    result_peg_173.setPosition(position_peg_179);
                                    goto out_peg_180;
                                }
                            }
                                
                        }
                        goto success_peg_174;
                        out_peg_180:
                        goto loop_peg_172;
                        success_peg_174:
                        ;
                        result_peg_156.addResult(result_peg_173);
                    } while (true);
                    loop_peg_172:
                    ;
                            
                }
                goto success_peg_160;
                goto out_peg_158;
                success_peg_160:
                ;
            
            
            
            result_peg_156.setValue(Value((void*) "("));
                for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_156.getPosition()))){
                        result_peg_156.nextPosition();
                    } else {
                        goto out_peg_158;
                    }
                }
            
            
            
            {
                    int position_peg_185 = result_peg_156.getPosition();
                    
                    result_peg_156.reset();
                    do{
                        Result result_peg_196(result_peg_156.getPosition());
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
                        goto loop_peg_195;
                        success_peg_197:
                        ;
                        result_peg_156.addResult(result_peg_196);
                    } while (true);
                    loop_peg_195:
                    ;
                            
                }
                goto success_peg_183;
                goto out_peg_158;
                success_peg_183:
                ;
            
            
            
            result_peg_156 = rule_number(stream, result_peg_156.getPosition());
                if (result_peg_156.error()){
                    goto out_peg_158;
                }
                index = result_peg_156.getValues();
            
            
            
            {
                    int position_peg_208 = result_peg_156.getPosition();
                    
                    result_peg_156.reset();
                    do{
                        Result result_peg_219(result_peg_156.getPosition());
                        {
                            int position_peg_222 = result_peg_219.getPosition();
                            
                            result_peg_219.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_219.getPosition()))){
                                    result_peg_219.nextPosition();
                                } else {
                                    result_peg_219.setPosition(position_peg_222);
                                    goto out_peg_223;
                                }
                            }
                                
                        }
                        goto success_peg_220;
                        out_peg_223:
                        {
                            int position_peg_225 = result_peg_219.getPosition();
                            
                            result_peg_219.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_219.getPosition()))){
                                    result_peg_219.nextPosition();
                                } else {
                                    result_peg_219.setPosition(position_peg_225);
                                    goto out_peg_226;
                                }
                            }
                                
                        }
                        goto success_peg_220;
                        out_peg_226:
                        goto loop_peg_218;
                        success_peg_220:
                        ;
                        result_peg_156.addResult(result_peg_219);
                    } while (true);
                    loop_peg_218:
                    ;
                            
                }
                goto success_peg_206;
                goto out_peg_158;
                success_peg_206:
                ;
            
            
            
            result_peg_156.setValue(Value((void*) ")"));
                for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_156.getPosition()))){
                        result_peg_156.nextPosition();
                    } else {
                        goto out_peg_158;
                    }
                }
            
            
            
            {
                    int position_peg_231 = result_peg_156.getPosition();
                    
                    result_peg_156.reset();
                    do{
                        Result result_peg_242(result_peg_156.getPosition());
                        {
                            int position_peg_245 = result_peg_242.getPosition();
                            
                            result_peg_242.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_242.getPosition()))){
                                    result_peg_242.nextPosition();
                                } else {
                                    result_peg_242.setPosition(position_peg_245);
                                    goto out_peg_246;
                                }
                            }
                                
                        }
                        goto success_peg_243;
                        out_peg_246:
                        {
                            int position_peg_248 = result_peg_242.getPosition();
                            
                            result_peg_242.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_242.getPosition()))){
                                    result_peg_242.nextPosition();
                                } else {
                                    result_peg_242.setPosition(position_peg_248);
                                    goto out_peg_249;
                                }
                            }
                                
                        }
                        goto success_peg_243;
                        out_peg_249:
                        goto loop_peg_241;
                        success_peg_243:
                        ;
                        result_peg_156.addResult(result_peg_242);
                    } while (true);
                    loop_peg_241:
                    ;
                            
                }
                goto success_peg_229;
                goto out_peg_158;
                success_peg_229:
                ;
            
            
            
            result_peg_156.setValue(Value((void*) "="));
                for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_156.getPosition()))){
                        result_peg_156.nextPosition();
                    } else {
                        goto out_peg_158;
                    }
                }
            
            
            
            {
                    int position_peg_254 = result_peg_156.getPosition();
                    
                    result_peg_156.reset();
                    do{
                        Result result_peg_265(result_peg_156.getPosition());
                        {
                            int position_peg_268 = result_peg_265.getPosition();
                            
                            result_peg_265.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_265.getPosition()))){
                                    result_peg_265.nextPosition();
                                } else {
                                    result_peg_265.setPosition(position_peg_268);
                                    goto out_peg_269;
                                }
                            }
                                
                        }
                        goto success_peg_266;
                        out_peg_269:
                        {
                            int position_peg_271 = result_peg_265.getPosition();
                            
                            result_peg_265.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_265.getPosition()))){
                                    result_peg_265.nextPosition();
                                } else {
                                    result_peg_265.setPosition(position_peg_271);
                                    goto out_peg_272;
                                }
                            }
                                
                        }
                        goto success_peg_266;
                        out_peg_272:
                        goto loop_peg_264;
                        success_peg_266:
                        ;
                        result_peg_156.addResult(result_peg_265);
                    } while (true);
                    loop_peg_264:
                    ;
                            
                }
                goto success_peg_252;
                goto out_peg_158;
                success_peg_252:
                ;
            
            
            
            result_peg_156 = rule_valuelist(stream, result_peg_156.getPosition());
                if (result_peg_156.error()){
                    goto out_peg_158;
                }
                data = result_peg_156.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(id, index, data);
                    result_peg_156.setValue(value);
                }
            
            
        }
        stream.update(result_peg_156.getPosition());
        
        return result_peg_156;
        out_peg_158:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute_simple(Stream & stream, const int position){
    
    RuleTrace trace_peg_51(stream, "attribute_simple");
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
                    int position_peg_8 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_19(result_peg_2.getPosition());
                        {
                            int position_peg_22 = result_peg_19.getPosition();
                            
                            result_peg_19.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                    result_peg_19.nextPosition();
                                } else {
                                    result_peg_19.setPosition(position_peg_22);
                                    goto out_peg_23;
                                }
                            }
                                
                        }
                        goto success_peg_20;
                        out_peg_23:
                        {
                            int position_peg_25 = result_peg_19.getPosition();
                            
                            result_peg_19.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                    result_peg_19.nextPosition();
                                } else {
                                    result_peg_19.setPosition(position_peg_25);
                                    goto out_peg_26;
                                }
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
                    int position_peg_31 = result_peg_2.getPosition();
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_42(result_peg_2.getPosition());
                        {
                            int position_peg_45 = result_peg_42.getPosition();
                            
                            result_peg_42.setValue(Value((void*) " "));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_42.getPosition()))){
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
                            
                            result_peg_42.setValue(Value((void*) "\t"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_42.getPosition()))){
                                    result_peg_42.nextPosition();
                                } else {
                                    result_peg_42.setPosition(position_peg_48);
                                    goto out_peg_49;
                                }
                            }
                                
                        }
                        goto success_peg_43;
                        out_peg_49:
                        goto loop_peg_41;
                        success_peg_43:
                        ;
                        result_peg_2.addResult(result_peg_42);
                    } while (true);
                    loop_peg_41:
                    ;
                            
                }
                goto success_peg_29;
                goto out_peg_4;
                success_peg_29:
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
    
    RuleTrace trace_peg_52(stream, "identifier_list");
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
                                int position_peg_10 = result_peg_6.getPosition();
                                
                                result_peg_6.reset();
                                do{
                                    Result result_peg_21(result_peg_6.getPosition());
                                    {
                                        int position_peg_24 = result_peg_21.getPosition();
                                        
                                        result_peg_21.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_21.getPosition()))){
                                                result_peg_21.nextPosition();
                                            } else {
                                                result_peg_21.setPosition(position_peg_24);
                                                goto out_peg_25;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_22;
                                    out_peg_25:
                                    {
                                        int position_peg_27 = result_peg_21.getPosition();
                                        
                                        result_peg_21.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_21.getPosition()))){
                                                result_peg_21.nextPosition();
                                            } else {
                                                result_peg_21.setPosition(position_peg_27);
                                                goto out_peg_28;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_22;
                                    out_peg_28:
                                    goto loop_peg_20;
                                    success_peg_22:
                                    ;
                                    result_peg_6.addResult(result_peg_21);
                                } while (true);
                                loop_peg_20:
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
                                int position_peg_33 = result_peg_6.getPosition();
                                
                                result_peg_6.reset();
                                do{
                                    Result result_peg_44(result_peg_6.getPosition());
                                    {
                                        int position_peg_47 = result_peg_44.getPosition();
                                        
                                        result_peg_44.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_44.getPosition()))){
                                                result_peg_44.nextPosition();
                                            } else {
                                                result_peg_44.setPosition(position_peg_47);
                                                goto out_peg_48;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_45;
                                    out_peg_48:
                                    {
                                        int position_peg_50 = result_peg_44.getPosition();
                                        
                                        result_peg_44.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_44.getPosition()))){
                                                result_peg_44.nextPosition();
                                            } else {
                                                result_peg_44.setPosition(position_peg_50);
                                                goto out_peg_51;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_45;
                                    out_peg_51:
                                    goto loop_peg_43;
                                    success_peg_45:
                                    ;
                                    result_peg_6.addResult(result_peg_44);
                                } while (true);
                                loop_peg_43:
                                ;
                                        
                            }
                            goto success_peg_31;
                            goto loop_peg_5;
                            success_peg_31:
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
    
    RuleTrace trace_peg_56(stream, "valuelist");
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
                                int position_peg_13 = result_peg_9.getPosition();
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_24(result_peg_9.getPosition());
                                    {
                                        int position_peg_27 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_27);
                                                goto out_peg_28;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_28:
                                    {
                                        int position_peg_30 = result_peg_24.getPosition();
                                        
                                        result_peg_24.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_24.getPosition()))){
                                                result_peg_24.nextPosition();
                                            } else {
                                                result_peg_24.setPosition(position_peg_30);
                                                goto out_peg_31;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_25;
                                    out_peg_31:
                                    goto loop_peg_23;
                                    success_peg_25:
                                    ;
                                    result_peg_9.addResult(result_peg_24);
                                } while (true);
                                loop_peg_23:
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
                                int position_peg_36 = result_peg_9.getPosition();
                                
                                result_peg_9.reset();
                                do{
                                    Result result_peg_47(result_peg_9.getPosition());
                                    {
                                        int position_peg_50 = result_peg_47.getPosition();
                                        
                                        result_peg_47.setValue(Value((void*) " "));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_47.getPosition()))){
                                                result_peg_47.nextPosition();
                                            } else {
                                                result_peg_47.setPosition(position_peg_50);
                                                goto out_peg_51;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_48;
                                    out_peg_51:
                                    {
                                        int position_peg_53 = result_peg_47.getPosition();
                                        
                                        result_peg_47.setValue(Value((void*) "\t"));
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_47.getPosition()))){
                                                result_peg_47.nextPosition();
                                            } else {
                                                result_peg_47.setPosition(position_peg_53);
                                                goto out_peg_54;
                                            }
                                        }
                                            
                                    }
                                    goto success_peg_48;
                                    out_peg_54:
                                    goto loop_peg_46;
                                    success_peg_48:
                                    ;
                                    result_peg_9.addResult(result_peg_47);
                                } while (true);
                                loop_peg_46:
                                ;
                                        
                            }
                            goto success_peg_34;
                            goto loop_peg_8;
                            success_peg_34:
                            ;
                        
                        
                        
                        int save_peg_55 = result_peg_9.getPosition();
                            
                            result_peg_9 = rule_value(stream, result_peg_9.getPosition());
                            if (result_peg_9.error()){
                                
                                result_peg_9 = Result(save_peg_55);
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
    
    RuleTrace trace_peg_262(stream, "value");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
            int position_peg_5 = result_peg_2.getPosition();
            {
            
                result_peg_2.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_5);
                            goto out_peg_7;
                        }
                    }
                
                
                
                result_peg_2.reset();
                    do{
                        Result result_peg_22(result_peg_2.getPosition());
                        {
                        
                            Result result_peg_25(result_peg_22);
                                result_peg_25.setValue(Value((void*) "\""));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\""[i], stream.get(result_peg_25.getPosition()))){
                                        result_peg_25.nextPosition();
                                    } else {
                                        goto not_peg_24;
                                    }
                                }
                                goto loop_peg_21;
                                not_peg_24:
                                result_peg_22.setValue(Value((void*)0));
                            
                            
                            
                            Result result_peg_28(result_peg_22);
                                result_peg_28.setValue(Value((void*) "\n"));
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_28.getPosition()))){
                                        result_peg_28.nextPosition();
                                    } else {
                                        goto not_peg_27;
                                    }
                                }
                                goto loop_peg_21;
                                not_peg_27:
                                result_peg_22.setValue(Value((void*)0));
                            
                            
                            
                            char temp_peg_29 = stream.get(result_peg_22.getPosition());
                                if (temp_peg_29 != '\0'){
                                    result_peg_22.setValue(Value((void*) (long) temp_peg_29));
                                    result_peg_22.nextPosition();
                                } else {
                                    goto loop_peg_21;
                                }
                            
                            
                        }
                        result_peg_2.addResult(result_peg_22);
                    } while (true);
                    loop_peg_21:
                    ;
                    data = result_peg_2.getValues();
                
                
                
                result_peg_2.setValue(Value((void*) "\""));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            result_peg_2.setPosition(position_peg_5);
                            goto out_peg_7;
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
        out_peg_7:
        goto out_peg_31;
        success_peg_3:
        ;
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_31:
        Result result_peg_32(myposition);
        
        result_peg_32 = rule_date(stream, result_peg_32.getPosition());
        if (result_peg_32.error()){
            goto out_peg_33;
        }
        stream.update(result_peg_32.getPosition());
        
        return result_peg_32;
        out_peg_33:
        Result result_peg_34(myposition);
        
        {
        
            result_peg_34 = rule_number(stream, result_peg_34.getPosition());
                if (result_peg_34.error()){
                    goto out_peg_36;
                }
            
            Result result_peg_35 = result_peg_34;
            
            Result result_peg_39(result_peg_34);
                {
                    int position_peg_42 = result_peg_39.getPosition();
                    
                    char letter_peg_45 = stream.get(result_peg_39.getPosition());
                    if (letter_peg_45 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_45) != NULL){
                        result_peg_39.nextPosition();
                        result_peg_39.setValue(Value((void*) (long) letter_peg_45));
                    } else {
                        result_peg_39.setPosition(position_peg_42);
                        goto out_peg_44;
                    }
                    
                }
                goto success_peg_40;
                out_peg_44:
                goto not_peg_38;
                success_peg_40:
                ;
                goto out_peg_36;
                not_peg_38:
                result_peg_34.setValue(Value((void*)0));
            
            
            
            Result result_peg_48(result_peg_34);
                result_peg_48.setValue(Value((void*) "-"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_48.getPosition()))){
                        result_peg_48.nextPosition();
                    } else {
                        goto not_peg_47;
                    }
                }
                goto out_peg_36;
                not_peg_47:
                result_peg_34.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_35.getValues();
                    result_peg_34.setValue(value);
                }
            
            
        }
        stream.update(result_peg_34.getPosition());
        
        return result_peg_34;
        out_peg_36:
        Result result_peg_49(myposition);
        
        {
        
            result_peg_49 = rule_attribute_simple(stream, result_peg_49.getPosition());
                if (result_peg_49.error()){
                    goto out_peg_51;
                }
            
            Result result_peg_50 = result_peg_49;
            
            {
                    Value value((void*) 0);
                    value = makeValueAttribute(result_peg_50.getValues());
                    result_peg_49.setValue(value);
                }
            
            
        }
        stream.update(result_peg_49.getPosition());
        
        return result_peg_49;
        out_peg_51:
        Result result_peg_52(myposition);
        
        {
        
            {
                    int position_peg_56 = result_peg_52.getPosition();
                    
                    result_peg_52.setValue(Value((void*) "normal"));
                    for (int i = 0; i < 6; i++){
                        if (compareChar("normal"[i], stream.get(result_peg_52.getPosition()))){
                            result_peg_52.nextPosition();
                        } else {
                            result_peg_52.setPosition(position_peg_56);
                            goto out_peg_57;
                        }
                    }
                        
                }
                goto success_peg_54;
                out_peg_57:
                {
                    int position_peg_59 = result_peg_52.getPosition();
                    
                    result_peg_52.setValue(Value((void*) "parallax"));
                    for (int i = 0; i < 8; i++){
                        if (compareChar("parallax"[i], stream.get(result_peg_52.getPosition()))){
                            result_peg_52.nextPosition();
                        } else {
                            result_peg_52.setPosition(position_peg_59);
                            goto out_peg_60;
                        }
                    }
                        
                }
                goto success_peg_54;
                out_peg_60:
                {
                    int position_peg_62 = result_peg_52.getPosition();
                    
                    result_peg_52.setValue(Value((void*) "addalpha"));
                    for (int i = 0; i < 8; i++){
                        if (compareChar("addalpha"[i], stream.get(result_peg_52.getPosition()))){
                            result_peg_52.nextPosition();
                        } else {
                            result_peg_52.setPosition(position_peg_62);
                            goto out_peg_63;
                        }
                    }
                        
                }
                goto success_peg_54;
                out_peg_63:
                {
                    int position_peg_65 = result_peg_52.getPosition();
                    
                    result_peg_52.setValue(Value((void*) "add1"));
                    for (int i = 0; i < 4; i++){
                        if (compareChar("add1"[i], stream.get(result_peg_52.getPosition()))){
                            result_peg_52.nextPosition();
                        } else {
                            result_peg_52.setPosition(position_peg_65);
                            goto out_peg_66;
                        }
                    }
                        
                }
                goto success_peg_54;
                out_peg_66:
                {
                    int position_peg_68 = result_peg_52.getPosition();
                    
                    result_peg_52.setValue(Value((void*) "add"));
                    for (int i = 0; i < 3; i++){
                        if (compareChar("add"[i], stream.get(result_peg_52.getPosition()))){
                            result_peg_52.nextPosition();
                        } else {
                            result_peg_52.setPosition(position_peg_68);
                            goto out_peg_69;
                        }
                    }
                        
                }
                goto success_peg_54;
                out_peg_69:
                {
                    int position_peg_71 = result_peg_52.getPosition();
                    
                    result_peg_52.setValue(Value((void*) "sub"));
                    for (int i = 0; i < 3; i++){
                        if (compareChar("sub"[i], stream.get(result_peg_52.getPosition()))){
                            result_peg_52.nextPosition();
                        } else {
                            result_peg_52.setPosition(position_peg_71);
                            goto out_peg_72;
                        }
                    }
                        
                }
                goto success_peg_54;
                out_peg_72:
                goto out_peg_73;
                success_peg_54:
                ;
            
            Result result_peg_53 = result_peg_52;
            
            Result result_peg_76(result_peg_52);
                {
                    int position_peg_79 = result_peg_76.getPosition();
                    
                    {
                        int position_peg_89 = result_peg_76.getPosition();
                        
                        char letter_peg_92 = stream.get(result_peg_76.getPosition());
                        if (letter_peg_92 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_92) != NULL){
                            result_peg_76.nextPosition();
                            result_peg_76.setValue(Value((void*) (long) letter_peg_92));
                        } else {
                            result_peg_76.setPosition(position_peg_89);
                            goto out_peg_91;
                        }
                        
                    }
                    goto success_peg_87;
                    out_peg_91:
                    result_peg_76.setPosition(position_peg_79);
                    goto out_peg_86;
                    success_peg_87:
                    ;
                    
                }
                goto success_peg_77;
                out_peg_86:
                {
                    int position_peg_94 = result_peg_76.getPosition();
                    
                    {
                        int position_peg_104 = result_peg_76.getPosition();
                        
                        char letter_peg_107 = stream.get(result_peg_76.getPosition());
                        if (letter_peg_107 != '\0' && strchr("0123456789", letter_peg_107) != NULL){
                            result_peg_76.nextPosition();
                            result_peg_76.setValue(Value((void*) (long) letter_peg_107));
                        } else {
                            result_peg_76.setPosition(position_peg_104);
                            goto out_peg_106;
                        }
                        
                    }
                    goto success_peg_102;
                    out_peg_106:
                    result_peg_76.setPosition(position_peg_94);
                    goto out_peg_101;
                    success_peg_102:
                    ;
                    
                }
                goto success_peg_77;
                out_peg_101:
                goto not_peg_75;
                success_peg_77:
                ;
                goto out_peg_73;
                not_peg_75:
                result_peg_52.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_53.getValues());
                    result_peg_52.setValue(value);
                }
            
            
        }
        stream.update(result_peg_52.getPosition());
        
        return result_peg_52;
        out_peg_73:
        Result result_peg_108(myposition);
        
        {
        
            result_peg_108.setValue(Value((void*) "s"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_108.getPosition()))){
                        result_peg_108.nextPosition();
                    } else {
                        goto out_peg_110;
                    }
                }
            
            Result result_peg_109 = result_peg_108;
            
            Result result_peg_113(result_peg_108);
                {
                    int position_peg_116 = result_peg_113.getPosition();
                    {
                    
                        Result result_peg_139(result_peg_113);
                            result_peg_139.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_139.getPosition()))){
                                    result_peg_139.nextPosition();
                                } else {
                                    goto not_peg_138;
                                }
                            }
                            result_peg_113.setPosition(position_peg_116);
                            goto out_peg_120;
                            not_peg_138:
                            result_peg_113.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_142(result_peg_113);
                            result_peg_142.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_142.getPosition()))){
                                    result_peg_142.nextPosition();
                                } else {
                                    goto not_peg_141;
                                }
                            }
                            result_peg_113.setPosition(position_peg_116);
                            goto out_peg_120;
                            not_peg_141:
                            result_peg_113.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_145(result_peg_113);
                            result_peg_145.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_145.getPosition()))){
                                    result_peg_145.nextPosition();
                                } else {
                                    goto not_peg_144;
                                }
                            }
                            result_peg_113.setPosition(position_peg_116);
                            goto out_peg_120;
                            not_peg_144:
                            result_peg_113.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_148(result_peg_113);
                            result_peg_148.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_148.getPosition()) == (unsigned char) 13){
                                result_peg_148.nextPosition();
                            } else {
                                goto not_peg_147;
                            }
                            result_peg_113.setPosition(position_peg_116);
                            goto out_peg_120;
                            not_peg_147:
                            result_peg_113.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_151(result_peg_113);
                            result_peg_151.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_151.getPosition()))){
                                    result_peg_151.nextPosition();
                                } else {
                                    goto not_peg_150;
                                }
                            }
                            result_peg_113.setPosition(position_peg_116);
                            goto out_peg_120;
                            not_peg_150:
                            result_peg_113.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_154(result_peg_113);
                            result_peg_154.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_154.getPosition()))){
                                    result_peg_154.nextPosition();
                                } else {
                                    goto not_peg_153;
                                }
                            }
                            result_peg_113.setPosition(position_peg_116);
                            goto out_peg_120;
                            not_peg_153:
                            result_peg_113.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_155 = stream.get(result_peg_113.getPosition());
                            if (temp_peg_155 != '\0'){
                                result_peg_113.setValue(Value((void*) (long) temp_peg_155));
                                result_peg_113.nextPosition();
                            } else {
                                result_peg_113.setPosition(position_peg_116);
                                goto out_peg_120;
                            }
                        
                        
                    }
                }
                goto success_peg_114;
                out_peg_120:
                goto not_peg_112;
                success_peg_114:
                ;
                goto out_peg_110;
                not_peg_112:
                result_peg_108.setValue(Value((void*)0));
            
            
            
            Result result_peg_158(result_peg_108);
                result_peg_158.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_158.getPosition()))){
                        result_peg_158.nextPosition();
                    } else {
                        goto not_peg_157;
                    }
                }
                goto out_peg_110;
                not_peg_157:
                result_peg_108.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_109.getValues());
                    result_peg_108.setValue(value);
                }
            
            
        }
        stream.update(result_peg_108.getPosition());
        
        return result_peg_108;
        out_peg_110:
        Result result_peg_159(myposition);
        
        {
        
            result_peg_159.setValue(Value((void*) "h"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_159.getPosition()))){
                        result_peg_159.nextPosition();
                    } else {
                        goto out_peg_161;
                    }
                }
            
            Result result_peg_160 = result_peg_159;
            
            Result result_peg_164(result_peg_159);
                {
                    int position_peg_167 = result_peg_164.getPosition();
                    {
                    
                        Result result_peg_190(result_peg_164);
                            result_peg_190.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    goto not_peg_189;
                                }
                            }
                            result_peg_164.setPosition(position_peg_167);
                            goto out_peg_171;
                            not_peg_189:
                            result_peg_164.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_193(result_peg_164);
                            result_peg_193.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_193.getPosition()))){
                                    result_peg_193.nextPosition();
                                } else {
                                    goto not_peg_192;
                                }
                            }
                            result_peg_164.setPosition(position_peg_167);
                            goto out_peg_171;
                            not_peg_192:
                            result_peg_164.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_196(result_peg_164);
                            result_peg_196.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_196.getPosition()))){
                                    result_peg_196.nextPosition();
                                } else {
                                    goto not_peg_195;
                                }
                            }
                            result_peg_164.setPosition(position_peg_167);
                            goto out_peg_171;
                            not_peg_195:
                            result_peg_164.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_199(result_peg_164);
                            result_peg_199.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_199.getPosition()) == (unsigned char) 13){
                                result_peg_199.nextPosition();
                            } else {
                                goto not_peg_198;
                            }
                            result_peg_164.setPosition(position_peg_167);
                            goto out_peg_171;
                            not_peg_198:
                            result_peg_164.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_202(result_peg_164);
                            result_peg_202.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_202.getPosition()))){
                                    result_peg_202.nextPosition();
                                } else {
                                    goto not_peg_201;
                                }
                            }
                            result_peg_164.setPosition(position_peg_167);
                            goto out_peg_171;
                            not_peg_201:
                            result_peg_164.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_205(result_peg_164);
                            result_peg_205.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_205.getPosition()))){
                                    result_peg_205.nextPosition();
                                } else {
                                    goto not_peg_204;
                                }
                            }
                            result_peg_164.setPosition(position_peg_167);
                            goto out_peg_171;
                            not_peg_204:
                            result_peg_164.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_206 = stream.get(result_peg_164.getPosition());
                            if (temp_peg_206 != '\0'){
                                result_peg_164.setValue(Value((void*) (long) temp_peg_206));
                                result_peg_164.nextPosition();
                            } else {
                                result_peg_164.setPosition(position_peg_167);
                                goto out_peg_171;
                            }
                        
                        
                    }
                }
                goto success_peg_165;
                out_peg_171:
                goto not_peg_163;
                success_peg_165:
                ;
                goto out_peg_161;
                not_peg_163:
                result_peg_159.setValue(Value((void*)0));
            
            
            
            Result result_peg_209(result_peg_159);
                result_peg_209.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_209.getPosition()))){
                        result_peg_209.nextPosition();
                    } else {
                        goto not_peg_208;
                    }
                }
                goto out_peg_161;
                not_peg_208:
                result_peg_159.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_160.getValues());
                    result_peg_159.setValue(value);
                }
            
            
        }
        stream.update(result_peg_159.getPosition());
        
        return result_peg_159;
        out_peg_161:
        Result result_peg_210(myposition);
        
        {
        
            result_peg_210.setValue(Value((void*) "a"));
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_210.getPosition()))){
                        result_peg_210.nextPosition();
                    } else {
                        goto out_peg_212;
                    }
                }
            
            Result result_peg_211 = result_peg_210;
            
            int save_peg_214 = result_peg_210.getPosition();
                
                result_peg_210 = rule_number(stream, result_peg_210.getPosition());
                if (result_peg_210.error()){
                    
                    result_peg_210 = Result(save_peg_214);
                    result_peg_210.setValue(Value((void*) 0));
                    
                }
            
            
            
            Result result_peg_217(result_peg_210);
                {
                    int position_peg_220 = result_peg_217.getPosition();
                    {
                    
                        Result result_peg_243(result_peg_217);
                            result_peg_243.setValue(Value((void*) ","));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_243.getPosition()))){
                                    result_peg_243.nextPosition();
                                } else {
                                    goto not_peg_242;
                                }
                            }
                            result_peg_217.setPosition(position_peg_220);
                            goto out_peg_224;
                            not_peg_242:
                            result_peg_217.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_246(result_peg_217);
                            result_peg_246.setValue(Value((void*) "\n"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_246.getPosition()))){
                                    result_peg_246.nextPosition();
                                } else {
                                    goto not_peg_245;
                                }
                            }
                            result_peg_217.setPosition(position_peg_220);
                            goto out_peg_224;
                            not_peg_245:
                            result_peg_217.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_249(result_peg_217);
                            result_peg_249.setValue(Value((void*) "["));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("["[i], stream.get(result_peg_249.getPosition()))){
                                    result_peg_249.nextPosition();
                                } else {
                                    goto not_peg_248;
                                }
                            }
                            result_peg_217.setPosition(position_peg_220);
                            goto out_peg_224;
                            not_peg_248:
                            result_peg_217.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_252(result_peg_217);
                            result_peg_252.setValue(Value((void*) 13));
                            if ((unsigned char) stream.get(result_peg_252.getPosition()) == (unsigned char) 13){
                                result_peg_252.nextPosition();
                            } else {
                                goto not_peg_251;
                            }
                            result_peg_217.setPosition(position_peg_220);
                            goto out_peg_224;
                            not_peg_251:
                            result_peg_217.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_255(result_peg_217);
                            result_peg_255.setValue(Value((void*) "="));
                            for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_255.getPosition()))){
                                    result_peg_255.nextPosition();
                                } else {
                                    goto not_peg_254;
                                }
                            }
                            result_peg_217.setPosition(position_peg_220);
                            goto out_peg_224;
                            not_peg_254:
                            result_peg_217.setValue(Value((void*)0));
                        
                        
                        
                        Result result_peg_258(result_peg_217);
                            result_peg_258.setValue(Value((void*) ";"));
                            for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_258.getPosition()))){
                                    result_peg_258.nextPosition();
                                } else {
                                    goto not_peg_257;
                                }
                            }
                            result_peg_217.setPosition(position_peg_220);
                            goto out_peg_224;
                            not_peg_257:
                            result_peg_217.setValue(Value((void*)0));
                        
                        
                        
                        char temp_peg_259 = stream.get(result_peg_217.getPosition());
                            if (temp_peg_259 != '\0'){
                                result_peg_217.setValue(Value((void*) (long) temp_peg_259));
                                result_peg_217.nextPosition();
                            } else {
                                result_peg_217.setPosition(position_peg_220);
                                goto out_peg_224;
                            }
                        
                        
                    }
                }
                goto success_peg_218;
                out_peg_224:
                goto not_peg_216;
                success_peg_218:
                ;
                goto out_peg_212;
                not_peg_216:
                result_peg_210.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_211.getValues());
                    result_peg_210.setValue(value);
                }
            
            
        }
        stream.update(result_peg_210.getPosition());
        
        return result_peg_210;
        out_peg_212:
        Result result_peg_260(myposition);
        
        result_peg_260 = rule_filename(stream, result_peg_260.getPosition());
        if (result_peg_260.error()){
            goto out_peg_261;
        }
        stream.update(result_peg_260.getPosition());
        
        return result_peg_260;
        out_peg_261:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    RuleTrace trace_peg_64(stream, "date");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        int position_peg_8 = result_peg_5.getPosition();
                        
                        char letter_peg_11 = stream.get(result_peg_5.getPosition());
                        if (letter_peg_11 != '\0' && strchr("0123456789", letter_peg_11) != NULL){
                            result_peg_5.nextPosition();
                            result_peg_5.setValue(Value((void*) (long) letter_peg_11));
                        } else {
                            result_peg_5.setPosition(position_peg_8);
                            goto out_peg_10;
                        }
                        
                    }
                    goto success_peg_6;
                    out_peg_10:
                    goto loop_peg_4;
                    success_peg_6:
                    ;
                    result_peg_2.addResult(result_peg_5);
                } while (true);
                loop_peg_4:
                if (result_peg_2.matches() == 0){
                    goto out_peg_12;
                }
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_12;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_16(result_peg_2.getPosition());
                    {
                        int position_peg_19 = result_peg_16.getPosition();
                        
                        char letter_peg_22 = stream.get(result_peg_16.getPosition());
                        if (letter_peg_22 != '\0' && strchr("0123456789", letter_peg_22) != NULL){
                            result_peg_16.nextPosition();
                            result_peg_16.setValue(Value((void*) (long) letter_peg_22));
                        } else {
                            result_peg_16.setPosition(position_peg_19);
                            goto out_peg_21;
                        }
                        
                    }
                    goto success_peg_17;
                    out_peg_21:
                    goto loop_peg_15;
                    success_peg_17:
                    ;
                    result_peg_2.addResult(result_peg_16);
                } while (true);
                loop_peg_15:
                if (result_peg_2.matches() == 0){
                    goto out_peg_12;
                }
            
            Result result_peg_14 = result_peg_2;
            
            result_peg_2.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_12;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_26(result_peg_2.getPosition());
                    {
                        int position_peg_29 = result_peg_26.getPosition();
                        
                        char letter_peg_32 = stream.get(result_peg_26.getPosition());
                        if (letter_peg_32 != '\0' && strchr("0123456789", letter_peg_32) != NULL){
                            result_peg_26.nextPosition();
                            result_peg_26.setValue(Value((void*) (long) letter_peg_32));
                        } else {
                            result_peg_26.setPosition(position_peg_29);
                            goto out_peg_31;
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
                if (result_peg_2.matches() == 0){
                    goto out_peg_12;
                }
            
            Result result_peg_24 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_3.getValues(),result_peg_14.getValues(),result_peg_24.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_12:
        Result result_peg_33(myposition);
        
        {
        
            result_peg_33.reset();
                do{
                    Result result_peg_36(result_peg_33.getPosition());
                    {
                        int position_peg_39 = result_peg_36.getPosition();
                        
                        char letter_peg_42 = stream.get(result_peg_36.getPosition());
                        if (letter_peg_42 != '\0' && strchr("0123456789", letter_peg_42) != NULL){
                            result_peg_36.nextPosition();
                            result_peg_36.setValue(Value((void*) (long) letter_peg_42));
                        } else {
                            result_peg_36.setPosition(position_peg_39);
                            goto out_peg_41;
                        }
                        
                    }
                    goto success_peg_37;
                    out_peg_41:
                    goto loop_peg_35;
                    success_peg_37:
                    ;
                    result_peg_33.addResult(result_peg_36);
                } while (true);
                loop_peg_35:
                if (result_peg_33.matches() == 0){
                    goto out_peg_43;
                }
            
            Result result_peg_34 = result_peg_33;
            
            result_peg_33.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto out_peg_43;
                    }
                }
            
            
            
            result_peg_33.reset();
                do{
                    Result result_peg_47(result_peg_33.getPosition());
                    {
                        int position_peg_50 = result_peg_47.getPosition();
                        
                        char letter_peg_53 = stream.get(result_peg_47.getPosition());
                        if (letter_peg_53 != '\0' && strchr("0123456789", letter_peg_53) != NULL){
                            result_peg_47.nextPosition();
                            result_peg_47.setValue(Value((void*) (long) letter_peg_53));
                        } else {
                            result_peg_47.setPosition(position_peg_50);
                            goto out_peg_52;
                        }
                        
                    }
                    goto success_peg_48;
                    out_peg_52:
                    goto loop_peg_46;
                    success_peg_48:
                    ;
                    result_peg_33.addResult(result_peg_47);
                } while (true);
                loop_peg_46:
                if (result_peg_33.matches() == 0){
                    goto out_peg_43;
                }
            
            Result result_peg_45 = result_peg_33;
            
            result_peg_33.setValue(Value((void*) "/"));
                for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto out_peg_43;
                    }
                }
            
            
            
            result_peg_33.reset();
                do{
                    Result result_peg_57(result_peg_33.getPosition());
                    {
                        int position_peg_60 = result_peg_57.getPosition();
                        
                        char letter_peg_63 = stream.get(result_peg_57.getPosition());
                        if (letter_peg_63 != '\0' && strchr("0123456789", letter_peg_63) != NULL){
                            result_peg_57.nextPosition();
                            result_peg_57.setValue(Value((void*) (long) letter_peg_63));
                        } else {
                            result_peg_57.setPosition(position_peg_60);
                            goto out_peg_62;
                        }
                        
                    }
                    goto success_peg_58;
                    out_peg_62:
                    goto loop_peg_56;
                    success_peg_58:
                    ;
                    result_peg_33.addResult(result_peg_57);
                } while (true);
                loop_peg_56:
                if (result_peg_33.matches() == 0){
                    goto out_peg_43;
                }
            
            Result result_peg_55 = result_peg_33;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_34.getValues(),result_peg_45.getValues(),result_peg_55.getValues());
                    result_peg_33.setValue(value);
                }
            
            
        }
        stream.update(result_peg_33.getPosition());
        
        return result_peg_33;
        out_peg_43:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    RuleTrace trace_peg_14(stream, "number");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            int save_peg_4 = result_peg_2.getPosition();
                
                {
                    int position_peg_7 = result_peg_2.getPosition();
                    
                    result_peg_2.setValue(Value((void*) "+"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
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
                    
                    result_peg_2.setValue(Value((void*) "-"));
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
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
            
            result_peg_2 = rule_float_or_integer(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_13;
                }
            
            Result result_peg_12 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_3.getValues(),result_peg_12.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_13:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    RuleTrace trace_peg_37(stream, "float_or_integer");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        int position_peg_8 = result_peg_5.getPosition();
                        
                        char letter_peg_11 = stream.get(result_peg_5.getPosition());
                        if (letter_peg_11 != '\0' && strchr("0123456789", letter_peg_11) != NULL){
                            result_peg_5.nextPosition();
                            result_peg_5.setValue(Value((void*) (long) letter_peg_11));
                        } else {
                            result_peg_5.setPosition(position_peg_8);
                            goto out_peg_10;
                        }
                        
                    }
                    goto success_peg_6;
                    out_peg_10:
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
                        goto out_peg_13;
                    }
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_16(result_peg_2.getPosition());
                    {
                        int position_peg_19 = result_peg_16.getPosition();
                        
                        char letter_peg_22 = stream.get(result_peg_16.getPosition());
                        if (letter_peg_22 != '\0' && strchr("0123456789", letter_peg_22) != NULL){
                            result_peg_16.nextPosition();
                            result_peg_16.setValue(Value((void*) (long) letter_peg_22));
                        } else {
                            result_peg_16.setPosition(position_peg_19);
                            goto out_peg_21;
                        }
                        
                    }
                    goto success_peg_17;
                    out_peg_21:
                    goto loop_peg_15;
                    success_peg_17:
                    ;
                    result_peg_2.addResult(result_peg_16);
                } while (true);
                loop_peg_15:
                if (result_peg_2.matches() == 0){
                    goto out_peg_13;
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
        out_peg_13:
        Result result_peg_23(myposition);
        
        {
        
            result_peg_23.reset();
                do{
                    Result result_peg_26(result_peg_23.getPosition());
                    {
                        int position_peg_29 = result_peg_26.getPosition();
                        
                        char letter_peg_32 = stream.get(result_peg_26.getPosition());
                        if (letter_peg_32 != '\0' && strchr("0123456789", letter_peg_32) != NULL){
                            result_peg_26.nextPosition();
                            result_peg_26.setValue(Value((void*) (long) letter_peg_32));
                        } else {
                            result_peg_26.setPosition(position_peg_29);
                            goto out_peg_31;
                        }
                        
                    }
                    goto success_peg_27;
                    out_peg_31:
                    goto loop_peg_25;
                    success_peg_27:
                    ;
                    result_peg_23.addResult(result_peg_26);
                } while (true);
                loop_peg_25:
                if (result_peg_23.matches() == 0){
                    goto out_peg_33;
                }
            
            Result result_peg_24 = result_peg_23;
            
            Result result_peg_36(result_peg_23);
                result_peg_36.setValue(Value((void*) "."));
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_36.getPosition()))){
                        result_peg_36.nextPosition();
                    } else {
                        goto not_peg_35;
                    }
                }
                goto out_peg_33;
                not_peg_35:
                result_peg_23.setValue(Value((void*)0));
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_24.getValues());
                    result_peg_23.setValue(value);
                }
            
            
        }
        stream.update(result_peg_23.getPosition());
        
        return result_peg_23;
        out_peg_33:
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

    
