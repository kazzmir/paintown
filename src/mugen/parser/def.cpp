

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
    Result chunk_line_end;
    Result chunk_section;
    Result chunk_section_line;
};

struct Chunk1{
    Result chunk_section_start;
    Result chunk_loopstart;
    Result chunk_name;
    Result chunk_line_end_or_comment;
    Result chunk_filename;
};

struct Chunk2{
    Result chunk_filename_char;
    Result chunk_attribute;
    Result chunk_attribute_simple;
    Result chunk_identifier;
    Result chunk_identifier_list;
};

struct Chunk3{
    Result chunk_valuelist;
    Result chunk_value;
    Result chunk_keyword;
    Result chunk_date;
    Result chunk_string;
};

struct Chunk4{
    Result chunk_number;
    Result chunk_float_or_integer;
};

struct Column{
    Column():
        chunk0(0)
        ,chunk1(0)
        ,chunk2(0)
        ,chunk3(0)
        ,chunk4(0){
    }

    Chunk0 * chunk0;
    Chunk1 * chunk1;
    Chunk2 * chunk2;
    Chunk3 * chunk3;
    Chunk4 * chunk4;

    int hitCount(){
        return 
(chunk3 != NULL ? ((chunk3->chunk_valuelist.calculated() ? 1 : 0)
+ (chunk3->chunk_value.calculated() ? 1 : 0)
+ (chunk3->chunk_keyword.calculated() ? 1 : 0)
+ (chunk3->chunk_date.calculated() ? 1 : 0)
+ (chunk3->chunk_string.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_filename_char.calculated() ? 1 : 0)
+ (chunk2->chunk_attribute.calculated() ? 1 : 0)
+ (chunk2->chunk_attribute_simple.calculated() ? 1 : 0)
+ (chunk2->chunk_identifier.calculated() ? 1 : 0)
+ (chunk2->chunk_identifier_list.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_section_start.calculated() ? 1 : 0)
+ (chunk1->chunk_loopstart.calculated() ? 1 : 0)
+ (chunk1->chunk_name.calculated() ? 1 : 0)
+ (chunk1->chunk_line_end_or_comment.calculated() ? 1 : 0)
+ (chunk1->chunk_filename.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)
+ (chunk0->chunk_section.calculated() ? 1 : 0)
+ (chunk0->chunk_section_line.calculated() ? 1 : 0)) : 0)
+
(chunk4 != NULL ? ((chunk4->chunk_number.calculated() ? 1 : 0)
+ (chunk4->chunk_float_or_integer.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 22;
    }

    ~Column(){
        delete chunk0;
        delete chunk1;
        delete chunk2;
        delete chunk3;
        delete chunk4;
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
Result rule_line_end(Stream &, const int);
Result rule_section(Stream &, const int);
Result rule_section_line(Stream &, const int, Value section);
Result rule_section_start(Stream &, const int);
Result rule_loopstart(Stream &, const int);
Result rule_name(Stream &, const int);
Result rule_line_end_or_comment(Stream &, const int);
Result rule_filename(Stream &, const int);
Result rule_filename_char(Stream &, const int);
Result rule_attribute(Stream &, const int);
Result rule_attribute_simple(Stream &, const int);
Result rule_identifier(Stream &, const int);
Result rule_identifier_list(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_keyword(Stream &, const int);
Result rule_date(Stream &, const int);
Result rule_string(Stream &, const int);
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

Ast::Section * makeSection(const Value & str){
    Ast::Section * object = new Ast::Section(as<std::string*>(str));
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

Ast::Attribute * makeAttribute(const Value & id, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(as<Ast::Identifier*>(id), as<Ast::Value*>(data));
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
    Ast::Keyword * object = new Ast::Keyword(as<char*>(value));
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
    Ast::Number * object = new Ast::Number(0);
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

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string> ids;
    ids.push_back(*as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        ids.push_back(*as<std::string*>((*it).getValue()));
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

    Ast::Number * object = new Ast::Number(value);
    GC::save(object);
    return object;
}

Ast::String * makeString(const Value & value){
    Ast::String * object = new Ast::String(toString(value));
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
    Ast::Number * object = new Ast::Number(0);
    GC::save(object);
    return object;
}




Result rule_start(Stream & stream, const int position){
    
    RuleTrace trace_peg_389(stream, "start");
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_99(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_99.getPosition()))){
                                    result_peg_99.nextPosition();
                                } else {
                                    goto out_peg_102;
                                }
                            }
                            result_peg_99.setValue((void*) " ");
                                
                        }
                        goto success_peg_100;
                        out_peg_102:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_99.getPosition()))){
                                    result_peg_99.nextPosition();
                                } else {
                                    goto out_peg_104;
                                }
                            }
                            result_peg_99.setValue((void*) "\t");
                                
                        }
                        goto success_peg_100;
                        out_peg_104:
                        {
                            
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_99.getPosition()))){
                                                result_peg_99.nextPosition();
                                            } else {
                                                goto out_peg_151;
                                            }
                                        }
                                        result_peg_99.setValue((void*) ";");
                                    
                                    
                                    
                                    result_peg_99.reset();
                                        do{
                                            Result result_peg_160(result_peg_99.getPosition());
                                            {
                                            
                                                Result result_peg_163(result_peg_160);
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_163.getPosition()))){
                                                            result_peg_163.nextPosition();
                                                        } else {
                                                            goto not_peg_162;
                                                        }
                                                    }
                                                    result_peg_163.setValue((void*) "\n");
                                                    goto loop_peg_159;
                                                    not_peg_162:
                                                    result_peg_160.setValue((void*)0);
                                                
                                                
                                                
                                                char temp_peg_164 = stream.get(result_peg_160.getPosition());
                                                    if (temp_peg_164 != '\0'){
                                                        result_peg_160.setValue((void*) (long) temp_peg_164);
                                                        result_peg_160.nextPosition();
                                                    } else {
                                                        goto loop_peg_159;
                                                    }
                                                
                                                
                                            }
                                            result_peg_99.addResult(result_peg_160);
                                        } while (true);
                                        loop_peg_159:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_148;
                            out_peg_151:
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_99.getPosition()))){
                                                result_peg_99.nextPosition();
                                            } else {
                                                goto out_peg_167;
                                            }
                                        }
                                        result_peg_99.setValue((void*) "=");
                                    
                                    
                                    
                                    result_peg_99.reset();
                                        do{
                                            Result result_peg_176(result_peg_99.getPosition());
                                            {
                                            
                                                Result result_peg_179(result_peg_176);
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_179.getPosition()))){
                                                            result_peg_179.nextPosition();
                                                        } else {
                                                            goto not_peg_178;
                                                        }
                                                    }
                                                    result_peg_179.setValue((void*) "\n");
                                                    goto loop_peg_175;
                                                    not_peg_178:
                                                    result_peg_176.setValue((void*)0);
                                                
                                                
                                                
                                                char temp_peg_180 = stream.get(result_peg_176.getPosition());
                                                    if (temp_peg_180 != '\0'){
                                                        result_peg_176.setValue((void*) (long) temp_peg_180);
                                                        result_peg_176.nextPosition();
                                                    } else {
                                                        goto loop_peg_175;
                                                    }
                                                
                                                
                                            }
                                            result_peg_99.addResult(result_peg_176);
                                        } while (true);
                                        loop_peg_175:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_148;
                            out_peg_167:
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_99.getPosition()))){
                                                result_peg_99.nextPosition();
                                            } else {
                                                goto out_peg_183;
                                            }
                                        }
                                        result_peg_99.setValue((void*) "-");
                                    
                                    
                                    
                                    result_peg_99.reset();
                                        do{
                                            Result result_peg_188(result_peg_99.getPosition());
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("-"[i], stream.get(result_peg_188.getPosition()))){
                                                    result_peg_188.nextPosition();
                                                } else {
                                                    goto loop_peg_187;
                                                }
                                            }
                                            result_peg_188.setValue((void*) "-");
                                            result_peg_99.addResult(result_peg_188);
                                        } while (true);
                                        loop_peg_187:
                                        if (result_peg_99.matches() == 0){
                                            goto out_peg_183;
                                        }
                                    
                                    
                                }
                            }
                            goto success_peg_148;
                            out_peg_183:
                            goto out_peg_147;
                            success_peg_148:
                            ;
                            
                        }
                        goto success_peg_100;
                        out_peg_147:
                        goto loop_peg_98;
                        success_peg_100:
                        ;
                        result_peg_2.addResult(result_peg_99);
                    } while (true);
                    loop_peg_98:
                    ;
                            
                }
                goto success_peg_5;
                goto out_peg_189;
                success_peg_5:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_192(result_peg_2.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_192.getPosition()))){
                                result_peg_192.nextPosition();
                            } else {
                                goto out_peg_195;
                            }
                        }
                        result_peg_192.setValue((void*) "\n");
                            
                    }
                    goto success_peg_193;
                    out_peg_195:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_192.getPosition()))){
                                result_peg_192.nextPosition();
                            } else {
                                goto out_peg_197;
                            }
                        }
                        result_peg_192.setValue((void*) "\r");
                            
                    }
                    goto success_peg_193;
                    out_peg_197:
                    goto loop_peg_191;
                    success_peg_193:
                    ;
                    result_peg_2.addResult(result_peg_192);
                } while (true);
                loop_peg_191:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_200(result_peg_2.getPosition());
                    {
                    
                        result_peg_200 = rule_line(stream, result_peg_200.getPosition(), current);
                            if (result_peg_200.error()){
                                goto loop_peg_199;
                            }
                        
                        
                        
                        {
                                
                                result_peg_200.reset();
                                do{
                                    Result result_peg_297(result_peg_200.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_297.getPosition()))){
                                                result_peg_297.nextPosition();
                                            } else {
                                                goto out_peg_300;
                                            }
                                        }
                                        result_peg_297.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_298;
                                    out_peg_300:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_297.getPosition()))){
                                                result_peg_297.nextPosition();
                                            } else {
                                                goto out_peg_302;
                                            }
                                        }
                                        result_peg_297.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_298;
                                    out_peg_302:
                                    {
                                        
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_297.getPosition()))){
                                                            result_peg_297.nextPosition();
                                                        } else {
                                                            goto out_peg_349;
                                                        }
                                                    }
                                                    result_peg_297.setValue((void*) ";");
                                                
                                                
                                                
                                                result_peg_297.reset();
                                                    do{
                                                        Result result_peg_358(result_peg_297.getPosition());
                                                        {
                                                        
                                                            Result result_peg_361(result_peg_358);
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_361.getPosition()))){
                                                                        result_peg_361.nextPosition();
                                                                    } else {
                                                                        goto not_peg_360;
                                                                    }
                                                                }
                                                                result_peg_361.setValue((void*) "\n");
                                                                goto loop_peg_357;
                                                                not_peg_360:
                                                                result_peg_358.setValue((void*)0);
                                                            
                                                            
                                                            
                                                            char temp_peg_362 = stream.get(result_peg_358.getPosition());
                                                                if (temp_peg_362 != '\0'){
                                                                    result_peg_358.setValue((void*) (long) temp_peg_362);
                                                                    result_peg_358.nextPosition();
                                                                } else {
                                                                    goto loop_peg_357;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_297.addResult(result_peg_358);
                                                    } while (true);
                                                    loop_peg_357:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_346;
                                        out_peg_349:
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_297.getPosition()))){
                                                            result_peg_297.nextPosition();
                                                        } else {
                                                            goto out_peg_365;
                                                        }
                                                    }
                                                    result_peg_297.setValue((void*) "=");
                                                
                                                
                                                
                                                result_peg_297.reset();
                                                    do{
                                                        Result result_peg_374(result_peg_297.getPosition());
                                                        {
                                                        
                                                            Result result_peg_377(result_peg_374);
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_377.getPosition()))){
                                                                        result_peg_377.nextPosition();
                                                                    } else {
                                                                        goto not_peg_376;
                                                                    }
                                                                }
                                                                result_peg_377.setValue((void*) "\n");
                                                                goto loop_peg_373;
                                                                not_peg_376:
                                                                result_peg_374.setValue((void*)0);
                                                            
                                                            
                                                            
                                                            char temp_peg_378 = stream.get(result_peg_374.getPosition());
                                                                if (temp_peg_378 != '\0'){
                                                                    result_peg_374.setValue((void*) (long) temp_peg_378);
                                                                    result_peg_374.nextPosition();
                                                                } else {
                                                                    goto loop_peg_373;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_297.addResult(result_peg_374);
                                                    } while (true);
                                                    loop_peg_373:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_346;
                                        out_peg_365:
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_297.getPosition()))){
                                                            result_peg_297.nextPosition();
                                                        } else {
                                                            goto out_peg_381;
                                                        }
                                                    }
                                                    result_peg_297.setValue((void*) "-");
                                                
                                                
                                                
                                                result_peg_297.reset();
                                                    do{
                                                        Result result_peg_386(result_peg_297.getPosition());
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("-"[i], stream.get(result_peg_386.getPosition()))){
                                                                result_peg_386.nextPosition();
                                                            } else {
                                                                goto loop_peg_385;
                                                            }
                                                        }
                                                        result_peg_386.setValue((void*) "-");
                                                        result_peg_297.addResult(result_peg_386);
                                                    } while (true);
                                                    loop_peg_385:
                                                    if (result_peg_297.matches() == 0){
                                                        goto out_peg_381;
                                                    }
                                                
                                                
                                            }
                                        }
                                        goto success_peg_346;
                                        out_peg_381:
                                        goto out_peg_345;
                                        success_peg_346:
                                        ;
                                        
                                    }
                                    goto success_peg_298;
                                    out_peg_345:
                                    goto loop_peg_296;
                                    success_peg_298:
                                    ;
                                    result_peg_200.addResult(result_peg_297);
                                } while (true);
                                loop_peg_296:
                                ;
                                        
                            }
                            goto success_peg_203;
                            goto loop_peg_199;
                            success_peg_203:
                            ;
                        
                        
                        
                        int save_peg_387 = result_peg_200.getPosition();
                            
                            result_peg_200 = rule_line_end(stream, result_peg_200.getPosition());
                            if (result_peg_200.error()){
                                
                                result_peg_200 = Result(save_peg_387);
                                result_peg_200.setValue((void*) 0);
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_200);
                } while (true);
                loop_peg_199:
                ;
            
            
            
            if ('\0' == stream.get(result_peg_2.getPosition())){
                    result_peg_2.nextPosition();
                    result_peg_2.setValue((void *) '\0');
                } else {
                    goto out_peg_189;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = current; GC::cleanup(as<SectionList*>(value));
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_189:
    stream.update(errorResult.getPosition());
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    RuleTrace trace_peg_91(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_14(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_17;
                                }
                            }
                            result_peg_14.setValue((void*) " ");
                                
                        }
                        goto success_peg_15;
                        out_peg_17:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_14.setValue((void*) "\t");
                                
                        }
                        goto success_peg_15;
                        out_peg_19:
                        goto loop_peg_13;
                        success_peg_15:
                        ;
                        result_peg_2.addResult(result_peg_14);
                    } while (true);
                    loop_peg_13:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_20;
                success_peg_4:
                ;
            
            
            
            {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    goto out_peg_24;
                                }
                            }
                            result_peg_2.setValue((void*) ";");
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_33(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_36(result_peg_33);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_36.getPosition()))){
                                                result_peg_36.nextPosition();
                                            } else {
                                                goto not_peg_35;
                                            }
                                        }
                                        result_peg_36.setValue((void*) "\n");
                                        goto loop_peg_32;
                                        not_peg_35:
                                        result_peg_33.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_37 = stream.get(result_peg_33.getPosition());
                                        if (temp_peg_37 != '\0'){
                                            result_peg_33.setValue((void*) (long) temp_peg_37);
                                            result_peg_33.nextPosition();
                                        } else {
                                            goto loop_peg_32;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_33);
                            } while (true);
                            loop_peg_32:
                            ;
                        
                        
                    }
                }
                goto success_peg_21;
                out_peg_24:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    goto out_peg_40;
                                }
                            }
                            result_peg_2.setValue((void*) "=");
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_49(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_52(result_peg_49);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_52.getPosition()))){
                                                result_peg_52.nextPosition();
                                            } else {
                                                goto not_peg_51;
                                            }
                                        }
                                        result_peg_52.setValue((void*) "\n");
                                        goto loop_peg_48;
                                        not_peg_51:
                                        result_peg_49.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_53 = stream.get(result_peg_49.getPosition());
                                        if (temp_peg_53 != '\0'){
                                            result_peg_49.setValue((void*) (long) temp_peg_53);
                                            result_peg_49.nextPosition();
                                        } else {
                                            goto loop_peg_48;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_49);
                            } while (true);
                            loop_peg_48:
                            ;
                        
                        
                    }
                }
                goto success_peg_21;
                out_peg_40:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    goto out_peg_56;
                                }
                            }
                            result_peg_2.setValue((void*) "-");
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_61(result_peg_2.getPosition());
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_61.getPosition()))){
                                        result_peg_61.nextPosition();
                                    } else {
                                        goto loop_peg_60;
                                    }
                                }
                                result_peg_61.setValue((void*) "-");
                                result_peg_2.addResult(result_peg_61);
                            } while (true);
                            loop_peg_60:
                            if (result_peg_2.matches() == 0){
                                goto out_peg_56;
                            }
                        
                        
                    }
                }
                goto success_peg_21;
                out_peg_56:
                goto out_peg_20;
                success_peg_21:
                ;
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_20:
        Result result_peg_62(myposition);
        
        {
        
            {
                    
                    result_peg_62.reset();
                    do{
                        Result result_peg_74(result_peg_62.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_77;
                                }
                            }
                            result_peg_74.setValue((void*) " ");
                                
                        }
                        goto success_peg_75;
                        out_peg_77:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_79;
                                }
                            }
                            result_peg_74.setValue((void*) "\t");
                                
                        }
                        goto success_peg_75;
                        out_peg_79:
                        goto loop_peg_73;
                        success_peg_75:
                        ;
                        result_peg_62.addResult(result_peg_74);
                    } while (true);
                    loop_peg_73:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_80;
                success_peg_64:
                ;
            
            
            
            result_peg_62 = rule_section(stream, result_peg_62.getPosition());
                if (result_peg_62.error()){
                    goto out_peg_80;
                }
            
            Result result_peg_81 = result_peg_62;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_81.getValues());
                    result_peg_62.setValue(value);
                }
            
            
        }
        stream.update(result_peg_62.getPosition());
        
        return result_peg_62;
        out_peg_80:
        Result result_peg_82(myposition);
        
        result_peg_82.reset();
        do{
            Result result_peg_84(result_peg_82.getPosition());
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar(" "[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_87;
                    }
                }
                result_peg_84.setValue((void*) " ");
                    
            }
            goto success_peg_85;
            out_peg_87:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\t"[i], stream.get(result_peg_84.getPosition()))){
                        result_peg_84.nextPosition();
                    } else {
                        goto out_peg_89;
                    }
                }
                result_peg_84.setValue((void*) "\t");
                    
            }
            goto success_peg_85;
            out_peg_89:
            goto loop_peg_83;
            success_peg_85:
            ;
            result_peg_82.addResult(result_peg_84);
        } while (true);
        loop_peg_83:
        if (result_peg_82.matches() == 0){
            goto out_peg_90;
        }
        stream.update(result_peg_82.getPosition());
        
        return result_peg_82;
        out_peg_90:
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
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        goto out_peg_7;
                    }
                }
                result_peg_4.setValue((void*) "\n");
                    
            }
            goto success_peg_5;
            out_peg_7:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_4.getPosition()))){
                        result_peg_4.nextPosition();
                    } else {
                        goto out_peg_9;
                    }
                }
                result_peg_4.setValue((void*) "\r");
                    
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
                    result_peg_13.setValue((void *) '\0');
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
    
    RuleTrace trace_peg_388(stream, "section");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_section_start(stream, result_peg_2.getPosition());
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
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_101(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_101.getPosition()))){
                                    result_peg_101.nextPosition();
                                } else {
                                    goto out_peg_104;
                                }
                            }
                            result_peg_101.setValue((void*) " ");
                                
                        }
                        goto success_peg_102;
                        out_peg_104:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_101.getPosition()))){
                                    result_peg_101.nextPosition();
                                } else {
                                    goto out_peg_106;
                                }
                            }
                            result_peg_101.setValue((void*) "\t");
                                
                        }
                        goto success_peg_102;
                        out_peg_106:
                        {
                            
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_101.getPosition()))){
                                                result_peg_101.nextPosition();
                                            } else {
                                                goto out_peg_153;
                                            }
                                        }
                                        result_peg_101.setValue((void*) ";");
                                    
                                    
                                    
                                    result_peg_101.reset();
                                        do{
                                            Result result_peg_162(result_peg_101.getPosition());
                                            {
                                            
                                                Result result_peg_165(result_peg_162);
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_165.getPosition()))){
                                                            result_peg_165.nextPosition();
                                                        } else {
                                                            goto not_peg_164;
                                                        }
                                                    }
                                                    result_peg_165.setValue((void*) "\n");
                                                    goto loop_peg_161;
                                                    not_peg_164:
                                                    result_peg_162.setValue((void*)0);
                                                
                                                
                                                
                                                char temp_peg_166 = stream.get(result_peg_162.getPosition());
                                                    if (temp_peg_166 != '\0'){
                                                        result_peg_162.setValue((void*) (long) temp_peg_166);
                                                        result_peg_162.nextPosition();
                                                    } else {
                                                        goto loop_peg_161;
                                                    }
                                                
                                                
                                            }
                                            result_peg_101.addResult(result_peg_162);
                                        } while (true);
                                        loop_peg_161:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_150;
                            out_peg_153:
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_101.getPosition()))){
                                                result_peg_101.nextPosition();
                                            } else {
                                                goto out_peg_169;
                                            }
                                        }
                                        result_peg_101.setValue((void*) "=");
                                    
                                    
                                    
                                    result_peg_101.reset();
                                        do{
                                            Result result_peg_178(result_peg_101.getPosition());
                                            {
                                            
                                                Result result_peg_181(result_peg_178);
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_181.getPosition()))){
                                                            result_peg_181.nextPosition();
                                                        } else {
                                                            goto not_peg_180;
                                                        }
                                                    }
                                                    result_peg_181.setValue((void*) "\n");
                                                    goto loop_peg_177;
                                                    not_peg_180:
                                                    result_peg_178.setValue((void*)0);
                                                
                                                
                                                
                                                char temp_peg_182 = stream.get(result_peg_178.getPosition());
                                                    if (temp_peg_182 != '\0'){
                                                        result_peg_178.setValue((void*) (long) temp_peg_182);
                                                        result_peg_178.nextPosition();
                                                    } else {
                                                        goto loop_peg_177;
                                                    }
                                                
                                                
                                            }
                                            result_peg_101.addResult(result_peg_178);
                                        } while (true);
                                        loop_peg_177:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_150;
                            out_peg_169:
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_101.getPosition()))){
                                                result_peg_101.nextPosition();
                                            } else {
                                                goto out_peg_185;
                                            }
                                        }
                                        result_peg_101.setValue((void*) "-");
                                    
                                    
                                    
                                    result_peg_101.reset();
                                        do{
                                            Result result_peg_190(result_peg_101.getPosition());
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("-"[i], stream.get(result_peg_190.getPosition()))){
                                                    result_peg_190.nextPosition();
                                                } else {
                                                    goto loop_peg_189;
                                                }
                                            }
                                            result_peg_190.setValue((void*) "-");
                                            result_peg_101.addResult(result_peg_190);
                                        } while (true);
                                        loop_peg_189:
                                        if (result_peg_101.matches() == 0){
                                            goto out_peg_185;
                                        }
                                    
                                    
                                }
                            }
                            goto success_peg_150;
                            out_peg_185:
                            goto out_peg_149;
                            success_peg_150:
                            ;
                            
                        }
                        goto success_peg_102;
                        out_peg_149:
                        goto loop_peg_100;
                        success_peg_102:
                        ;
                        result_peg_2.addResult(result_peg_101);
                    } while (true);
                    loop_peg_100:
                    ;
                            
                }
                goto success_peg_7;
                goto out_peg_4;
                success_peg_7:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_193(result_peg_2.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_193.getPosition()))){
                                result_peg_193.nextPosition();
                            } else {
                                goto out_peg_196;
                            }
                        }
                        result_peg_193.setValue((void*) "\n");
                            
                    }
                    goto success_peg_194;
                    out_peg_196:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_193.getPosition()))){
                                result_peg_193.nextPosition();
                            } else {
                                goto out_peg_198;
                            }
                        }
                        result_peg_193.setValue((void*) "\r");
                            
                    }
                    goto success_peg_194;
                    out_peg_198:
                    goto loop_peg_192;
                    success_peg_194:
                    ;
                    result_peg_2.addResult(result_peg_193);
                } while (true);
                loop_peg_192:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_201(result_peg_2.getPosition());
                    {
                    
                        result_peg_201 = rule_section_line(stream, result_peg_201.getPosition(), ast);
                            if (result_peg_201.error()){
                                goto loop_peg_200;
                            }
                        
                        
                        
                        {
                                
                                result_peg_201.reset();
                                do{
                                    Result result_peg_298(result_peg_201.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_298.getPosition()))){
                                                result_peg_298.nextPosition();
                                            } else {
                                                goto out_peg_301;
                                            }
                                        }
                                        result_peg_298.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_299;
                                    out_peg_301:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_298.getPosition()))){
                                                result_peg_298.nextPosition();
                                            } else {
                                                goto out_peg_303;
                                            }
                                        }
                                        result_peg_298.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_299;
                                    out_peg_303:
                                    {
                                        
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_298.getPosition()))){
                                                            result_peg_298.nextPosition();
                                                        } else {
                                                            goto out_peg_350;
                                                        }
                                                    }
                                                    result_peg_298.setValue((void*) ";");
                                                
                                                
                                                
                                                result_peg_298.reset();
                                                    do{
                                                        Result result_peg_359(result_peg_298.getPosition());
                                                        {
                                                        
                                                            Result result_peg_362(result_peg_359);
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_362.getPosition()))){
                                                                        result_peg_362.nextPosition();
                                                                    } else {
                                                                        goto not_peg_361;
                                                                    }
                                                                }
                                                                result_peg_362.setValue((void*) "\n");
                                                                goto loop_peg_358;
                                                                not_peg_361:
                                                                result_peg_359.setValue((void*)0);
                                                            
                                                            
                                                            
                                                            char temp_peg_363 = stream.get(result_peg_359.getPosition());
                                                                if (temp_peg_363 != '\0'){
                                                                    result_peg_359.setValue((void*) (long) temp_peg_363);
                                                                    result_peg_359.nextPosition();
                                                                } else {
                                                                    goto loop_peg_358;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_298.addResult(result_peg_359);
                                                    } while (true);
                                                    loop_peg_358:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_347;
                                        out_peg_350:
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_298.getPosition()))){
                                                            result_peg_298.nextPosition();
                                                        } else {
                                                            goto out_peg_366;
                                                        }
                                                    }
                                                    result_peg_298.setValue((void*) "=");
                                                
                                                
                                                
                                                result_peg_298.reset();
                                                    do{
                                                        Result result_peg_375(result_peg_298.getPosition());
                                                        {
                                                        
                                                            Result result_peg_378(result_peg_375);
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_378.getPosition()))){
                                                                        result_peg_378.nextPosition();
                                                                    } else {
                                                                        goto not_peg_377;
                                                                    }
                                                                }
                                                                result_peg_378.setValue((void*) "\n");
                                                                goto loop_peg_374;
                                                                not_peg_377:
                                                                result_peg_375.setValue((void*)0);
                                                            
                                                            
                                                            
                                                            char temp_peg_379 = stream.get(result_peg_375.getPosition());
                                                                if (temp_peg_379 != '\0'){
                                                                    result_peg_375.setValue((void*) (long) temp_peg_379);
                                                                    result_peg_375.nextPosition();
                                                                } else {
                                                                    goto loop_peg_374;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_298.addResult(result_peg_375);
                                                    } while (true);
                                                    loop_peg_374:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_347;
                                        out_peg_366:
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_298.getPosition()))){
                                                            result_peg_298.nextPosition();
                                                        } else {
                                                            goto out_peg_382;
                                                        }
                                                    }
                                                    result_peg_298.setValue((void*) "-");
                                                
                                                
                                                
                                                result_peg_298.reset();
                                                    do{
                                                        Result result_peg_387(result_peg_298.getPosition());
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("-"[i], stream.get(result_peg_387.getPosition()))){
                                                                result_peg_387.nextPosition();
                                                            } else {
                                                                goto loop_peg_386;
                                                            }
                                                        }
                                                        result_peg_387.setValue((void*) "-");
                                                        result_peg_298.addResult(result_peg_387);
                                                    } while (true);
                                                    loop_peg_386:
                                                    if (result_peg_298.matches() == 0){
                                                        goto out_peg_382;
                                                    }
                                                
                                                
                                            }
                                        }
                                        goto success_peg_347;
                                        out_peg_382:
                                        goto out_peg_346;
                                        success_peg_347:
                                        ;
                                        
                                    }
                                    goto success_peg_299;
                                    out_peg_346:
                                    goto loop_peg_297;
                                    success_peg_299:
                                    ;
                                    result_peg_201.addResult(result_peg_298);
                                } while (true);
                                loop_peg_297:
                                ;
                                        
                            }
                            goto success_peg_204;
                            goto loop_peg_200;
                            success_peg_204:
                            ;
                        
                        
                        
                        result_peg_201 = rule_line_end(stream, result_peg_201.getPosition());
                            if (result_peg_201.error()){
                                goto loop_peg_200;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_201);
                } while (true);
                loop_peg_200:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    RuleTrace trace_peg_134(stream, "section_line");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_14(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_17;
                                }
                            }
                            result_peg_14.setValue((void*) " ");
                                
                        }
                        goto success_peg_15;
                        out_peg_17:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_14.getPosition()))){
                                    result_peg_14.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_14.setValue((void*) "\t");
                                
                        }
                        goto success_peg_15;
                        out_peg_19:
                        goto loop_peg_13;
                        success_peg_15:
                        ;
                        result_peg_2.addResult(result_peg_14);
                    } while (true);
                    loop_peg_13:
                    ;
                            
                }
                goto success_peg_4;
                goto out_peg_20;
                success_peg_4:
                ;
            
            
            
            {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    goto out_peg_24;
                                }
                            }
                            result_peg_2.setValue((void*) ";");
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_33(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_36(result_peg_33);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_36.getPosition()))){
                                                result_peg_36.nextPosition();
                                            } else {
                                                goto not_peg_35;
                                            }
                                        }
                                        result_peg_36.setValue((void*) "\n");
                                        goto loop_peg_32;
                                        not_peg_35:
                                        result_peg_33.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_37 = stream.get(result_peg_33.getPosition());
                                        if (temp_peg_37 != '\0'){
                                            result_peg_33.setValue((void*) (long) temp_peg_37);
                                            result_peg_33.nextPosition();
                                        } else {
                                            goto loop_peg_32;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_33);
                            } while (true);
                            loop_peg_32:
                            ;
                        
                        
                    }
                }
                goto success_peg_21;
                out_peg_24:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    goto out_peg_40;
                                }
                            }
                            result_peg_2.setValue((void*) "=");
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_49(result_peg_2.getPosition());
                                {
                                
                                    Result result_peg_52(result_peg_49);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_52.getPosition()))){
                                                result_peg_52.nextPosition();
                                            } else {
                                                goto not_peg_51;
                                            }
                                        }
                                        result_peg_52.setValue((void*) "\n");
                                        goto loop_peg_48;
                                        not_peg_51:
                                        result_peg_49.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_53 = stream.get(result_peg_49.getPosition());
                                        if (temp_peg_53 != '\0'){
                                            result_peg_49.setValue((void*) (long) temp_peg_53);
                                            result_peg_49.nextPosition();
                                        } else {
                                            goto loop_peg_48;
                                        }
                                    
                                    
                                }
                                result_peg_2.addResult(result_peg_49);
                            } while (true);
                            loop_peg_48:
                            ;
                        
                        
                    }
                }
                goto success_peg_21;
                out_peg_40:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                                    result_peg_2.nextPosition();
                                } else {
                                    goto out_peg_56;
                                }
                            }
                            result_peg_2.setValue((void*) "-");
                        
                        
                        
                        result_peg_2.reset();
                            do{
                                Result result_peg_61(result_peg_2.getPosition());
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_61.getPosition()))){
                                        result_peg_61.nextPosition();
                                    } else {
                                        goto loop_peg_60;
                                    }
                                }
                                result_peg_61.setValue((void*) "-");
                                result_peg_2.addResult(result_peg_61);
                            } while (true);
                            loop_peg_60:
                            if (result_peg_2.matches() == 0){
                                goto out_peg_56;
                            }
                        
                        
                    }
                }
                goto success_peg_21;
                out_peg_56:
                goto out_peg_20;
                success_peg_21:
                ;
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_20:
        Result result_peg_62(myposition);
        
        {
        
            {
                    
                    result_peg_62.reset();
                    do{
                        Result result_peg_74(result_peg_62.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_77;
                                }
                            }
                            result_peg_74.setValue((void*) " ");
                                
                        }
                        goto success_peg_75;
                        out_peg_77:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_79;
                                }
                            }
                            result_peg_74.setValue((void*) "\t");
                                
                        }
                        goto success_peg_75;
                        out_peg_79:
                        goto loop_peg_73;
                        success_peg_75:
                        ;
                        result_peg_62.addResult(result_peg_74);
                    } while (true);
                    loop_peg_73:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_80;
                success_peg_64:
                ;
            
            
            
            result_peg_62 = rule_attribute(stream, result_peg_62.getPosition());
                if (result_peg_62.error()){
                    goto out_peg_80;
                }
                data = result_peg_62.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addAttribute(asAttribute(data));
                    result_peg_62.setValue(value);
                }
            
            
        }
        stream.update(result_peg_62.getPosition());
        
        return result_peg_62;
        out_peg_80:
        Result result_peg_82(myposition);
        
        {
        
            {
                    
                    result_peg_82.reset();
                    do{
                        Result result_peg_94(result_peg_82.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_94.getPosition()))){
                                    result_peg_94.nextPosition();
                                } else {
                                    goto out_peg_97;
                                }
                            }
                            result_peg_94.setValue((void*) " ");
                                
                        }
                        goto success_peg_95;
                        out_peg_97:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_94.getPosition()))){
                                    result_peg_94.nextPosition();
                                } else {
                                    goto out_peg_99;
                                }
                            }
                            result_peg_94.setValue((void*) "\t");
                                
                        }
                        goto success_peg_95;
                        out_peg_99:
                        goto loop_peg_93;
                        success_peg_95:
                        ;
                        result_peg_82.addResult(result_peg_94);
                    } while (true);
                    loop_peg_93:
                    ;
                            
                }
                goto success_peg_84;
                goto out_peg_100;
                success_peg_84:
                ;
            
            
            
            result_peg_82 = rule_loopstart(stream, result_peg_82.getPosition());
                if (result_peg_82.error()){
                    goto out_peg_100;
                }
                data = result_peg_82.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_82.setValue(value);
                }
            
            
        }
        stream.update(result_peg_82.getPosition());
        
        return result_peg_82;
        out_peg_100:
        Result result_peg_102(myposition);
        
        {
        
            {
                    
                    result_peg_102.reset();
                    do{
                        Result result_peg_114(result_peg_102.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_114.getPosition()))){
                                    result_peg_114.nextPosition();
                                } else {
                                    goto out_peg_117;
                                }
                            }
                            result_peg_114.setValue((void*) " ");
                                
                        }
                        goto success_peg_115;
                        out_peg_117:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_114.getPosition()))){
                                    result_peg_114.nextPosition();
                                } else {
                                    goto out_peg_119;
                                }
                            }
                            result_peg_114.setValue((void*) "\t");
                                
                        }
                        goto success_peg_115;
                        out_peg_119:
                        goto loop_peg_113;
                        success_peg_115:
                        ;
                        result_peg_102.addResult(result_peg_114);
                    } while (true);
                    loop_peg_113:
                    ;
                            
                }
                goto success_peg_104;
                goto out_peg_120;
                success_peg_104:
                ;
            
            
            
            result_peg_102 = rule_valuelist(stream, result_peg_102.getPosition());
                if (result_peg_102.error()){
                    goto out_peg_120;
                }
                data = result_peg_102.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_102.setValue(value);
                }
            
            
        }
        stream.update(result_peg_102.getPosition());
        
        return result_peg_102;
        out_peg_120:
        Result result_peg_122(myposition);
        
        {
        
            result_peg_122.reset();
                do{
                    Result result_peg_125(result_peg_122.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_125.getPosition()))){
                                result_peg_125.nextPosition();
                            } else {
                                goto out_peg_128;
                            }
                        }
                        result_peg_125.setValue((void*) " ");
                            
                    }
                    goto success_peg_126;
                    out_peg_128:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_125.getPosition()))){
                                result_peg_125.nextPosition();
                            } else {
                                goto out_peg_130;
                            }
                        }
                        result_peg_125.setValue((void*) "\t");
                            
                    }
                    goto success_peg_126;
                    out_peg_130:
                    goto loop_peg_124;
                    success_peg_126:
                    ;
                    result_peg_122.addResult(result_peg_125);
                } while (true);
                loop_peg_124:
                if (result_peg_122.matches() == 0){
                    goto out_peg_131;
                }
            
            
            
            Result result_peg_133(result_peg_122);
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_133.getPosition()))){
                        result_peg_133.nextPosition();
                    } else {
                        goto not_peg_132;
                    }
                }
                result_peg_133.setValue((void*) "[");
                goto out_peg_131;
                not_peg_132:
                result_peg_122.setValue((void*)0);
            
            
        }
        stream.update(result_peg_122.getPosition());
        
        return result_peg_122;
        out_peg_131:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    RuleTrace trace_peg_47(stream, "section_start");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue((void*) " ");
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue((void*) "\t");
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_24(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_27(result_peg_24);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_27.getPosition()))){
                                    result_peg_27.nextPosition();
                                } else {
                                    goto not_peg_26;
                                }
                            }
                            result_peg_27.setValue((void*) "]");
                            goto loop_peg_23;
                            not_peg_26:
                            result_peg_24.setValue((void*)0);
                        
                        
                        
                        char temp_peg_28 = stream.get(result_peg_24.getPosition());
                            if (temp_peg_28 != '\0'){
                                result_peg_24.setValue((void*) (long) temp_peg_28);
                                result_peg_24.nextPosition();
                            } else {
                                goto loop_peg_23;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_24);
                } while (true);
                loop_peg_23:
                if (result_peg_2.matches() == 0){
                    goto out_peg_4;
                }
                data = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_40(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_43;
                                }
                            }
                            result_peg_40.setValue((void*) " ");
                                
                        }
                        goto success_peg_41;
                        out_peg_43:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_40.getPosition()))){
                                    result_peg_40.nextPosition();
                                } else {
                                    goto out_peg_45;
                                }
                            }
                            result_peg_40.setValue((void*) "\t");
                                
                        }
                        goto success_peg_41;
                        out_peg_45:
                        goto loop_peg_39;
                        success_peg_41:
                        ;
                        result_peg_2.addResult(result_peg_40);
                    } while (true);
                    loop_peg_39:
                    ;
                            
                }
                goto success_peg_30;
                goto out_peg_4;
                success_peg_30:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "]");
            
            
            
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
        

Result rule_loopstart(Stream & stream, const int position){
    
    RuleTrace trace_peg_5(stream, "loopstart");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("loopstart"[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "loopstart");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValue();
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
    
    RuleTrace trace_peg_38(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            {
                    
                    char letter_peg_8 = stream.get(result_peg_2.getPosition());
                    if (letter_peg_8 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_8) != NULL){
                        result_peg_2.nextPosition();
                        result_peg_2.setValue((void*) (long) letter_peg_8);
                    } else {
                        goto out_peg_7;
                    }
                    
                }
                goto success_peg_4;
                out_peg_7:
                goto out_peg_9;
                success_peg_4:
                ;
            
            Result result_peg_3 = result_peg_2;
            
            result_peg_2.reset();
                do{
                    Result result_peg_12(result_peg_2.getPosition());
                    {
                        
                        {
                            
                            char letter_peg_25 = stream.get(result_peg_12.getPosition());
                            if (letter_peg_25 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_25) != NULL){
                                result_peg_12.nextPosition();
                                result_peg_12.setValue((void*) (long) letter_peg_25);
                            } else {
                                goto out_peg_24;
                            }
                            
                        }
                        goto success_peg_21;
                        out_peg_24:
                        goto out_peg_20;
                        success_peg_21:
                        ;
                        
                    }
                    goto success_peg_13;
                    out_peg_20:
                    {
                        
                        {
                            
                            char letter_peg_37 = stream.get(result_peg_12.getPosition());
                            if (letter_peg_37 != '\0' && strchr("0123456789", letter_peg_37) != NULL){
                                result_peg_12.nextPosition();
                                result_peg_12.setValue((void*) (long) letter_peg_37);
                            } else {
                                goto out_peg_36;
                            }
                            
                        }
                        goto success_peg_33;
                        out_peg_36:
                        goto out_peg_32;
                        success_peg_33:
                        ;
                        
                    }
                    goto success_peg_13;
                    out_peg_32:
                    goto loop_peg_11;
                    success_peg_13:
                    ;
                    result_peg_2.addResult(result_peg_12);
                } while (true);
                loop_peg_11:
                ;
            
            Result result_peg_10 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_3.getValues().getValue(),result_peg_10.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_9:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    RuleTrace trace_peg_47(stream, "line_end_or_comment");
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
            {
            
                for (int i = 0; i < 1; i++){
                        if (compareChar(";"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            goto out_peg_8;
                        }
                    }
                    result_peg_4.setValue((void*) ";");
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_17(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_20(result_peg_17);
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_20.getPosition()))){
                                        result_peg_20.nextPosition();
                                    } else {
                                        goto not_peg_19;
                                    }
                                }
                                result_peg_20.setValue((void*) "\n");
                                goto loop_peg_16;
                                not_peg_19:
                                result_peg_17.setValue((void*)0);
                            
                            
                            
                            char temp_peg_21 = stream.get(result_peg_17.getPosition());
                                if (temp_peg_21 != '\0'){
                                    result_peg_17.setValue((void*) (long) temp_peg_21);
                                    result_peg_17.nextPosition();
                                } else {
                                    goto loop_peg_16;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_17);
                    } while (true);
                    loop_peg_16:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_8:
        {
            {
            
                for (int i = 0; i < 1; i++){
                        if (compareChar("="[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            goto out_peg_24;
                        }
                    }
                    result_peg_4.setValue((void*) "=");
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_33(result_peg_4.getPosition());
                        {
                        
                            Result result_peg_36(result_peg_33);
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_36.getPosition()))){
                                        result_peg_36.nextPosition();
                                    } else {
                                        goto not_peg_35;
                                    }
                                }
                                result_peg_36.setValue((void*) "\n");
                                goto loop_peg_32;
                                not_peg_35:
                                result_peg_33.setValue((void*)0);
                            
                            
                            
                            char temp_peg_37 = stream.get(result_peg_33.getPosition());
                                if (temp_peg_37 != '\0'){
                                    result_peg_33.setValue((void*) (long) temp_peg_37);
                                    result_peg_33.nextPosition();
                                } else {
                                    goto loop_peg_32;
                                }
                            
                            
                        }
                        result_peg_4.addResult(result_peg_33);
                    } while (true);
                    loop_peg_32:
                    ;
                
                
            }
        }
        goto success_peg_5;
        out_peg_24:
        {
            {
            
                for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_4.getPosition()))){
                            result_peg_4.nextPosition();
                        } else {
                            goto out_peg_40;
                        }
                    }
                    result_peg_4.setValue((void*) "-");
                
                
                
                result_peg_4.reset();
                    do{
                        Result result_peg_45(result_peg_4.getPosition());
                        for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_45.getPosition()))){
                                result_peg_45.nextPosition();
                            } else {
                                goto loop_peg_44;
                            }
                        }
                        result_peg_45.setValue((void*) "-");
                        result_peg_4.addResult(result_peg_45);
                    } while (true);
                    loop_peg_44:
                    if (result_peg_4.matches() == 0){
                        goto out_peg_40;
                    }
                
                
            }
        }
        goto success_peg_5;
        out_peg_40:
        goto out_peg_46;
        success_peg_5:
        ;
        stream.update(result_peg_4.getPosition());
        
        return result_peg_4;
        out_peg_46:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    RuleTrace trace_peg_10(stream, "filename");
    int myposition = position;
    
    
    Value file;
    Result result_peg_2(myposition);
        
        {
        
            Result result_peg_5(result_peg_2);
                for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto not_peg_4;
                    }
                }
                result_peg_5.setValue((void*) "\"");
                goto out_peg_6;
                not_peg_4:
                result_peg_2.setValue((void*)0);
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_9(result_peg_2.getPosition());
                    result_peg_9 = rule_filename_char(stream, result_peg_9.getPosition());
                    if (result_peg_9.error()){
                        goto loop_peg_8;
                    }
                    result_peg_2.addResult(result_peg_9);
                } while (true);
                loop_peg_8:
                if (result_peg_2.matches() == 0){
                    goto out_peg_6;
                }
                file = result_peg_2.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new Ast::Filename(toString(file)); GC::save(as<Ast::Filename*>(value));
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_6:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_filename_char(Stream & stream, const int position){
    
    RuleTrace trace_peg_20(stream, "filename_char");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            Result result_peg_5(result_peg_2);
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_5.getPosition()))){
                        result_peg_5.nextPosition();
                    } else {
                        goto not_peg_4;
                    }
                }
                result_peg_5.setValue((void*) ",");
                goto out_peg_6;
                not_peg_4:
                result_peg_2.setValue((void*)0);
            
            
            
            Result result_peg_9(result_peg_2);
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_9.getPosition()))){
                        result_peg_9.nextPosition();
                    } else {
                        goto not_peg_8;
                    }
                }
                result_peg_9.setValue((void*) "\n");
                goto out_peg_6;
                not_peg_8:
                result_peg_2.setValue((void*)0);
            
            
            
            Result result_peg_12(result_peg_2);
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_12.getPosition()))){
                        result_peg_12.nextPosition();
                    } else {
                        goto not_peg_11;
                    }
                }
                result_peg_12.setValue((void*) "[");
                goto out_peg_6;
                not_peg_11:
                result_peg_2.setValue((void*)0);
            
            
            
            Result result_peg_15(result_peg_2);
                if ((unsigned char) stream.get(result_peg_15.getPosition()) == (unsigned char) 13){
                    result_peg_15.nextPosition();
                } else {
                    goto not_peg_14;
                }
                result_peg_15.setValue((void*) 13);
                goto out_peg_6;
                not_peg_14:
                result_peg_2.setValue((void*)0);
            
            
            
            Result result_peg_18(result_peg_2);
                for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_18.getPosition()))){
                        result_peg_18.nextPosition();
                    } else {
                        goto not_peg_17;
                    }
                }
                result_peg_18.setValue((void*) ";");
                goto out_peg_6;
                not_peg_17:
                result_peg_2.setValue((void*)0);
            
            
            
            char temp_peg_19 = stream.get(result_peg_2.getPosition());
                if (temp_peg_19 != '\0'){
                    result_peg_2.setValue((void*) (long) temp_peg_19);
                    result_peg_2.nextPosition();
                } else {
                    goto out_peg_6;
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_6:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    RuleTrace trace_peg_252(stream, "attribute");
    int myposition = position;
    
    
    Value id;
        Value data;
        Value index;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2 = rule_identifier(stream, result_peg_2.getPosition());
                if (result_peg_2.error()){
                    goto out_peg_4;
                }
                id = result_peg_2.getValues();
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue((void*) " ");
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue((void*) "\t");
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue((void*) " ");
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue((void*) "\t");
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_4;
                success_peg_24:
                ;
            
            
            
            Result result_peg_41(result_peg_2.getPosition());
                result_peg_41 = rule_line_end_or_comment(stream, result_peg_41.getPosition());
                if (result_peg_41.error()){
                    goto out_peg_4;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id);
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
        Result result_peg_42(myposition);
        
        {
        
            result_peg_42 = rule_identifier(stream, result_peg_42.getPosition());
                if (result_peg_42.error()){
                    goto out_peg_44;
                }
                id = result_peg_42.getValues();
            
            
            
            {
                    
                    result_peg_42.reset();
                    do{
                        Result result_peg_56(result_peg_42.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_56.getPosition()))){
                                    result_peg_56.nextPosition();
                                } else {
                                    goto out_peg_59;
                                }
                            }
                            result_peg_56.setValue((void*) " ");
                                
                        }
                        goto success_peg_57;
                        out_peg_59:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_56.getPosition()))){
                                    result_peg_56.nextPosition();
                                } else {
                                    goto out_peg_61;
                                }
                            }
                            result_peg_56.setValue((void*) "\t");
                                
                        }
                        goto success_peg_57;
                        out_peg_61:
                        goto loop_peg_55;
                        success_peg_57:
                        ;
                        result_peg_42.addResult(result_peg_56);
                    } while (true);
                    loop_peg_55:
                    ;
                            
                }
                goto success_peg_46;
                goto out_peg_44;
                success_peg_46:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_42.getPosition()))){
                        result_peg_42.nextPosition();
                    } else {
                        goto out_peg_44;
                    }
                }
                result_peg_42.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_42.reset();
                    do{
                        Result result_peg_74(result_peg_42.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_77;
                                }
                            }
                            result_peg_74.setValue((void*) " ");
                                
                        }
                        goto success_peg_75;
                        out_peg_77:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_74.getPosition()))){
                                    result_peg_74.nextPosition();
                                } else {
                                    goto out_peg_79;
                                }
                            }
                            result_peg_74.setValue((void*) "\t");
                                
                        }
                        goto success_peg_75;
                        out_peg_79:
                        goto loop_peg_73;
                        success_peg_75:
                        ;
                        result_peg_42.addResult(result_peg_74);
                    } while (true);
                    loop_peg_73:
                    ;
                            
                }
                goto success_peg_64;
                goto out_peg_44;
                success_peg_64:
                ;
            
            
            
            result_peg_42 = rule_valuelist(stream, result_peg_42.getPosition());
                if (result_peg_42.error()){
                    goto out_peg_44;
                }
                data = result_peg_42.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_42.setValue(value);
                }
            
            
        }
        stream.update(result_peg_42.getPosition());
        
        return result_peg_42;
        out_peg_44:
        Result result_peg_81(myposition);
        
        {
        
            result_peg_81 = rule_identifier(stream, result_peg_81.getPosition());
                if (result_peg_81.error()){
                    goto out_peg_83;
                }
                id = result_peg_81.getValues();
            
            
            
            {
                    
                    result_peg_81.reset();
                    do{
                        Result result_peg_95(result_peg_81.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    goto out_peg_98;
                                }
                            }
                            result_peg_95.setValue((void*) " ");
                                
                        }
                        goto success_peg_96;
                        out_peg_98:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_95.getPosition()))){
                                    result_peg_95.nextPosition();
                                } else {
                                    goto out_peg_100;
                                }
                            }
                            result_peg_95.setValue((void*) "\t");
                                
                        }
                        goto success_peg_96;
                        out_peg_100:
                        goto loop_peg_94;
                        success_peg_96:
                        ;
                        result_peg_81.addResult(result_peg_95);
                    } while (true);
                    loop_peg_94:
                    ;
                            
                }
                goto success_peg_85;
                goto out_peg_83;
                success_peg_85:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_81.getPosition()))){
                        result_peg_81.nextPosition();
                    } else {
                        goto out_peg_83;
                    }
                }
                result_peg_81.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_81.reset();
                    do{
                        Result result_peg_113(result_peg_81.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_113.getPosition()))){
                                    result_peg_113.nextPosition();
                                } else {
                                    goto out_peg_116;
                                }
                            }
                            result_peg_113.setValue((void*) " ");
                                
                        }
                        goto success_peg_114;
                        out_peg_116:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_113.getPosition()))){
                                    result_peg_113.nextPosition();
                                } else {
                                    goto out_peg_118;
                                }
                            }
                            result_peg_113.setValue((void*) "\t");
                                
                        }
                        goto success_peg_114;
                        out_peg_118:
                        goto loop_peg_112;
                        success_peg_114:
                        ;
                        result_peg_81.addResult(result_peg_113);
                    } while (true);
                    loop_peg_112:
                    ;
                            
                }
                goto success_peg_103;
                goto out_peg_83;
                success_peg_103:
                ;
            
            
            
            result_peg_81 = rule_filename(stream, result_peg_81.getPosition());
                if (result_peg_81.error()){
                    goto out_peg_83;
                }
                data = result_peg_81.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_81.setValue(value);
                }
            
            
        }
        stream.update(result_peg_81.getPosition());
        
        return result_peg_81;
        out_peg_83:
        Result result_peg_120(myposition);
        
        {
        
            result_peg_120 = rule_identifier(stream, result_peg_120.getPosition());
                if (result_peg_120.error()){
                    goto out_peg_122;
                }
                id = result_peg_120.getValues();
            
            
            
            {
                    
                    result_peg_120.reset();
                    do{
                        Result result_peg_134(result_peg_120.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_134.getPosition()))){
                                    result_peg_134.nextPosition();
                                } else {
                                    goto out_peg_137;
                                }
                            }
                            result_peg_134.setValue((void*) " ");
                                
                        }
                        goto success_peg_135;
                        out_peg_137:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_134.getPosition()))){
                                    result_peg_134.nextPosition();
                                } else {
                                    goto out_peg_139;
                                }
                            }
                            result_peg_134.setValue((void*) "\t");
                                
                        }
                        goto success_peg_135;
                        out_peg_139:
                        goto loop_peg_133;
                        success_peg_135:
                        ;
                        result_peg_120.addResult(result_peg_134);
                    } while (true);
                    loop_peg_133:
                    ;
                            
                }
                goto success_peg_124;
                goto out_peg_122;
                success_peg_124:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_120.getPosition()))){
                        result_peg_120.nextPosition();
                    } else {
                        goto out_peg_122;
                    }
                }
                result_peg_120.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_120.reset();
                    do{
                        Result result_peg_152(result_peg_120.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_152.getPosition()))){
                                    result_peg_152.nextPosition();
                                } else {
                                    goto out_peg_155;
                                }
                            }
                            result_peg_152.setValue((void*) " ");
                                
                        }
                        goto success_peg_153;
                        out_peg_155:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_152.getPosition()))){
                                    result_peg_152.nextPosition();
                                } else {
                                    goto out_peg_157;
                                }
                            }
                            result_peg_152.setValue((void*) "\t");
                                
                        }
                        goto success_peg_153;
                        out_peg_157:
                        goto loop_peg_151;
                        success_peg_153:
                        ;
                        result_peg_120.addResult(result_peg_152);
                    } while (true);
                    loop_peg_151:
                    ;
                            
                }
                goto success_peg_142;
                goto out_peg_122;
                success_peg_142:
                ;
            
            
            
            result_peg_120 = rule_number(stream, result_peg_120.getPosition());
                if (result_peg_120.error()){
                    goto out_peg_122;
                }
                index = result_peg_120.getValues();
            
            
            
            {
                    
                    result_peg_120.reset();
                    do{
                        Result result_peg_170(result_peg_120.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_170.getPosition()))){
                                    result_peg_170.nextPosition();
                                } else {
                                    goto out_peg_173;
                                }
                            }
                            result_peg_170.setValue((void*) " ");
                                
                        }
                        goto success_peg_171;
                        out_peg_173:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_170.getPosition()))){
                                    result_peg_170.nextPosition();
                                } else {
                                    goto out_peg_175;
                                }
                            }
                            result_peg_170.setValue((void*) "\t");
                                
                        }
                        goto success_peg_171;
                        out_peg_175:
                        goto loop_peg_169;
                        success_peg_171:
                        ;
                        result_peg_120.addResult(result_peg_170);
                    } while (true);
                    loop_peg_169:
                    ;
                            
                }
                goto success_peg_160;
                goto out_peg_122;
                success_peg_160:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_120.getPosition()))){
                        result_peg_120.nextPosition();
                    } else {
                        goto out_peg_122;
                    }
                }
                result_peg_120.setValue((void*) ")");
            
            
            
            {
                    
                    result_peg_120.reset();
                    do{
                        Result result_peg_188(result_peg_120.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_188.getPosition()))){
                                    result_peg_188.nextPosition();
                                } else {
                                    goto out_peg_191;
                                }
                            }
                            result_peg_188.setValue((void*) " ");
                                
                        }
                        goto success_peg_189;
                        out_peg_191:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_188.getPosition()))){
                                    result_peg_188.nextPosition();
                                } else {
                                    goto out_peg_193;
                                }
                            }
                            result_peg_188.setValue((void*) "\t");
                                
                        }
                        goto success_peg_189;
                        out_peg_193:
                        goto loop_peg_187;
                        success_peg_189:
                        ;
                        result_peg_120.addResult(result_peg_188);
                    } while (true);
                    loop_peg_187:
                    ;
                            
                }
                goto success_peg_178;
                goto out_peg_122;
                success_peg_178:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_120.getPosition()))){
                        result_peg_120.nextPosition();
                    } else {
                        goto out_peg_122;
                    }
                }
                result_peg_120.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_120.reset();
                    do{
                        Result result_peg_206(result_peg_120.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_206.getPosition()))){
                                    result_peg_206.nextPosition();
                                } else {
                                    goto out_peg_209;
                                }
                            }
                            result_peg_206.setValue((void*) " ");
                                
                        }
                        goto success_peg_207;
                        out_peg_209:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_206.getPosition()))){
                                    result_peg_206.nextPosition();
                                } else {
                                    goto out_peg_211;
                                }
                            }
                            result_peg_206.setValue((void*) "\t");
                                
                        }
                        goto success_peg_207;
                        out_peg_211:
                        goto loop_peg_205;
                        success_peg_207:
                        ;
                        result_peg_120.addResult(result_peg_206);
                    } while (true);
                    loop_peg_205:
                    ;
                            
                }
                goto success_peg_196;
                goto out_peg_122;
                success_peg_196:
                ;
            
            
            
            result_peg_120 = rule_valuelist(stream, result_peg_120.getPosition());
                if (result_peg_120.error()){
                    goto out_peg_122;
                }
                data = result_peg_120.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(id, index, data);
                    result_peg_120.setValue(value);
                }
            
            
        }
        stream.update(result_peg_120.getPosition());
        
        return result_peg_120;
        out_peg_122:
        Result result_peg_213(myposition);
        
        {
        
            result_peg_213 = rule_identifier_list(stream, result_peg_213.getPosition());
                if (result_peg_213.error()){
                    goto out_peg_215;
                }
                id = result_peg_213.getValues();
            
            
            
            {
                    
                    result_peg_213.reset();
                    do{
                        Result result_peg_227(result_peg_213.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_227.getPosition()))){
                                    result_peg_227.nextPosition();
                                } else {
                                    goto out_peg_230;
                                }
                            }
                            result_peg_227.setValue((void*) " ");
                                
                        }
                        goto success_peg_228;
                        out_peg_230:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_227.getPosition()))){
                                    result_peg_227.nextPosition();
                                } else {
                                    goto out_peg_232;
                                }
                            }
                            result_peg_227.setValue((void*) "\t");
                                
                        }
                        goto success_peg_228;
                        out_peg_232:
                        goto loop_peg_226;
                        success_peg_228:
                        ;
                        result_peg_213.addResult(result_peg_227);
                    } while (true);
                    loop_peg_226:
                    ;
                            
                }
                goto success_peg_217;
                goto out_peg_215;
                success_peg_217:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_213.getPosition()))){
                        result_peg_213.nextPosition();
                    } else {
                        goto out_peg_215;
                    }
                }
                result_peg_213.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_213.reset();
                    do{
                        Result result_peg_245(result_peg_213.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_245.getPosition()))){
                                    result_peg_245.nextPosition();
                                } else {
                                    goto out_peg_248;
                                }
                            }
                            result_peg_245.setValue((void*) " ");
                                
                        }
                        goto success_peg_246;
                        out_peg_248:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_245.getPosition()))){
                                    result_peg_245.nextPosition();
                                } else {
                                    goto out_peg_250;
                                }
                            }
                            result_peg_245.setValue((void*) "\t");
                                
                        }
                        goto success_peg_246;
                        out_peg_250:
                        goto loop_peg_244;
                        success_peg_246:
                        ;
                        result_peg_213.addResult(result_peg_245);
                    } while (true);
                    loop_peg_244:
                    ;
                            
                }
                goto success_peg_235;
                goto out_peg_215;
                success_peg_235:
                ;
            
            
            
            result_peg_213 = rule_valuelist(stream, result_peg_213.getPosition());
                if (result_peg_213.error()){
                    goto out_peg_215;
                }
                data = result_peg_213.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributes(id, data);
                    result_peg_213.setValue(value);
                }
            
            
        }
        stream.update(result_peg_213.getPosition());
        
        return result_peg_213;
        out_peg_215:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute_simple(Stream & stream, const int position){
    
    RuleTrace trace_peg_41(stream, "attribute_simple");
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
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_16(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_19;
                                }
                            }
                            result_peg_16.setValue((void*) " ");
                                
                        }
                        goto success_peg_17;
                        out_peg_19:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_16.getPosition()))){
                                    result_peg_16.nextPosition();
                                } else {
                                    goto out_peg_21;
                                }
                            }
                            result_peg_16.setValue((void*) "\t");
                                
                        }
                        goto success_peg_17;
                        out_peg_21:
                        goto loop_peg_15;
                        success_peg_17:
                        ;
                        result_peg_2.addResult(result_peg_16);
                    } while (true);
                    loop_peg_15:
                    ;
                            
                }
                goto success_peg_6;
                goto out_peg_4;
                success_peg_6:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_34(result_peg_2.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_37;
                                }
                            }
                            result_peg_34.setValue((void*) " ");
                                
                        }
                        goto success_peg_35;
                        out_peg_37:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_34.getPosition()))){
                                    result_peg_34.nextPosition();
                                } else {
                                    goto out_peg_39;
                                }
                            }
                            result_peg_34.setValue((void*) "\t");
                                
                        }
                        goto success_peg_35;
                        out_peg_39:
                        goto loop_peg_33;
                        success_peg_35:
                        ;
                        result_peg_2.addResult(result_peg_34);
                    } while (true);
                    loop_peg_33:
                    ;
                            
                }
                goto success_peg_24;
                goto out_peg_4;
                success_peg_24:
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
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                            result_peg_7.setValue((void*) ".");
                        
                        
                        
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
    
    RuleTrace trace_peg_42(stream, "identifier_list");
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
                                
                                result_peg_6.reset();
                                do{
                                    Result result_peg_18(result_peg_6.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_18.getPosition()))){
                                                result_peg_18.nextPosition();
                                            } else {
                                                goto out_peg_21;
                                            }
                                        }
                                        result_peg_18.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_19;
                                    out_peg_21:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_18.getPosition()))){
                                                result_peg_18.nextPosition();
                                            } else {
                                                goto out_peg_23;
                                            }
                                        }
                                        result_peg_18.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_19;
                                    out_peg_23:
                                    goto loop_peg_17;
                                    success_peg_19:
                                    ;
                                    result_peg_6.addResult(result_peg_18);
                                } while (true);
                                loop_peg_17:
                                ;
                                        
                            }
                            goto success_peg_8;
                            goto loop_peg_5;
                            success_peg_8:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_6.getPosition()))){
                                    result_peg_6.nextPosition();
                                } else {
                                    goto loop_peg_5;
                                }
                            }
                            result_peg_6.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_6.reset();
                                do{
                                    Result result_peg_36(result_peg_6.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_36.getPosition()))){
                                                result_peg_36.nextPosition();
                                            } else {
                                                goto out_peg_39;
                                            }
                                        }
                                        result_peg_36.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_37;
                                    out_peg_39:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_36.getPosition()))){
                                                result_peg_36.nextPosition();
                                            } else {
                                                goto out_peg_41;
                                            }
                                        }
                                        result_peg_36.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_37;
                                    out_peg_41:
                                    goto loop_peg_35;
                                    success_peg_37:
                                    ;
                                    result_peg_6.addResult(result_peg_36);
                                } while (true);
                                loop_peg_35:
                                ;
                                        
                            }
                            goto success_peg_26;
                            goto loop_peg_5;
                            success_peg_26:
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
    
    RuleTrace trace_peg_44(stream, "valuelist");
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
                                    Result result_peg_19(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_22;
                                            }
                                        }
                                        result_peg_19.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_22:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_19.getPosition()))){
                                                result_peg_19.nextPosition();
                                            } else {
                                                goto out_peg_24;
                                            }
                                        }
                                        result_peg_19.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_20;
                                    out_peg_24:
                                    goto loop_peg_18;
                                    success_peg_20:
                                    ;
                                    result_peg_7.addResult(result_peg_19);
                                } while (true);
                                loop_peg_18:
                                ;
                                        
                            }
                            goto success_peg_9;
                            goto loop_peg_6;
                            success_peg_9:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_7.getPosition()))){
                                    result_peg_7.nextPosition();
                                } else {
                                    goto loop_peg_6;
                                }
                            }
                            result_peg_7.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_7.reset();
                                do{
                                    Result result_peg_37(result_peg_7.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_40;
                                            }
                                        }
                                        result_peg_37.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_40:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_37.getPosition()))){
                                                result_peg_37.nextPosition();
                                            } else {
                                                goto out_peg_42;
                                            }
                                        }
                                        result_peg_37.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_38;
                                    out_peg_42:
                                    goto loop_peg_36;
                                    success_peg_38:
                                    ;
                                    result_peg_7.addResult(result_peg_37);
                                } while (true);
                                loop_peg_36:
                                ;
                                        
                            }
                            goto success_peg_27;
                            goto loop_peg_6;
                            success_peg_27:
                            ;
                        
                        
                        
                        int save_peg_43 = result_peg_7.getPosition();
                            
                            result_peg_7 = rule_value(stream, result_peg_7.getPosition());
                            if (result_peg_7.error()){
                                
                                result_peg_7 = Result(save_peg_43);
                                result_peg_7.setValue((void*) 0);
                                
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
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    RuleTrace trace_peg_53(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        result_peg_2 = rule_string(stream, result_peg_2.getPosition());
        if (result_peg_2.error()){
            goto out_peg_3;
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        result_peg_4 = rule_date(stream, result_peg_4.getPosition());
        if (result_peg_4.error()){
            goto out_peg_5;
        }
        stream.update(result_peg_4.getPosition());
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        {
        
            result_peg_6 = rule_number(stream, result_peg_6.getPosition());
                if (result_peg_6.error()){
                    goto out_peg_8;
                }
            
            Result result_peg_7 = result_peg_6;
            
            Result result_peg_11(result_peg_6);
                {
                    
                    char letter_peg_16 = stream.get(result_peg_11.getPosition());
                    if (letter_peg_16 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_16) != NULL){
                        result_peg_11.nextPosition();
                        result_peg_11.setValue((void*) (long) letter_peg_16);
                    } else {
                        goto out_peg_15;
                    }
                    
                }
                goto success_peg_12;
                out_peg_15:
                goto not_peg_10;
                success_peg_12:
                ;
                goto out_peg_8;
                not_peg_10:
                result_peg_6.setValue((void*)0);
            
            
            
            Result result_peg_19(result_peg_6);
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_19.getPosition()))){
                        result_peg_19.nextPosition();
                    } else {
                        goto not_peg_18;
                    }
                }
                result_peg_19.setValue((void*) "-");
                goto out_peg_8;
                not_peg_18:
                result_peg_6.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_7.getValues();
                    result_peg_6.setValue(value);
                }
            
            
        }
        stream.update(result_peg_6.getPosition());
        
        return result_peg_6;
        out_peg_8:
        Result result_peg_20(myposition);
        
        result_peg_20 = rule_attribute_simple(stream, result_peg_20.getPosition());
        if (result_peg_20.error()){
            goto out_peg_21;
        }
        stream.update(result_peg_20.getPosition());
        
        return result_peg_20;
        out_peg_21:
        Result result_peg_22(myposition);
        
        {
        
            result_peg_22 = rule_keyword(stream, result_peg_22.getPosition());
                if (result_peg_22.error()){
                    goto out_peg_24;
                }
            
            Result result_peg_23 = result_peg_22;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_23.getValues());
                    result_peg_22.setValue(value);
                }
            
            
        }
        stream.update(result_peg_22.getPosition());
        
        return result_peg_22;
        out_peg_24:
        Result result_peg_25(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_25.getPosition()))){
                        result_peg_25.nextPosition();
                    } else {
                        goto out_peg_27;
                    }
                }
                result_peg_25.setValue((void*) "s");
            
            Result result_peg_26 = result_peg_25;
            
            Result result_peg_30(result_peg_25);
                result_peg_30 = rule_filename_char(stream, result_peg_30.getPosition());
                if (result_peg_30.error()){
                    goto not_peg_29;
                }
                goto out_peg_27;
                not_peg_29:
                result_peg_25.setValue((void*)0);
            
            
            
            Result result_peg_33(result_peg_25);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto not_peg_32;
                    }
                }
                result_peg_33.setValue((void*) ".");
                goto out_peg_27;
                not_peg_32:
                result_peg_25.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_26.getValues());
                    result_peg_25.setValue(value);
                }
            
            
        }
        stream.update(result_peg_25.getPosition());
        
        return result_peg_25;
        out_peg_27:
        Result result_peg_34(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_34.getPosition()))){
                        result_peg_34.nextPosition();
                    } else {
                        goto out_peg_36;
                    }
                }
                result_peg_34.setValue((void*) "h");
            
            Result result_peg_35 = result_peg_34;
            
            Result result_peg_39(result_peg_34);
                result_peg_39 = rule_filename_char(stream, result_peg_39.getPosition());
                if (result_peg_39.error()){
                    goto not_peg_38;
                }
                goto out_peg_36;
                not_peg_38:
                result_peg_34.setValue((void*)0);
            
            
            
            Result result_peg_42(result_peg_34);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_42.getPosition()))){
                        result_peg_42.nextPosition();
                    } else {
                        goto not_peg_41;
                    }
                }
                result_peg_42.setValue((void*) ".");
                goto out_peg_36;
                not_peg_41:
                result_peg_34.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_35.getValues());
                    result_peg_34.setValue(value);
                }
            
            
        }
        stream.update(result_peg_34.getPosition());
        
        return result_peg_34;
        out_peg_36:
        Result result_peg_43(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_43.getPosition()))){
                        result_peg_43.nextPosition();
                    } else {
                        goto out_peg_45;
                    }
                }
                result_peg_43.setValue((void*) "a");
            
            Result result_peg_44 = result_peg_43;
            
            int save_peg_47 = result_peg_43.getPosition();
                
                result_peg_43 = rule_number(stream, result_peg_43.getPosition());
                if (result_peg_43.error()){
                    
                    result_peg_43 = Result(save_peg_47);
                    result_peg_43.setValue((void*) 0);
                    
                }
            
            
            
            Result result_peg_50(result_peg_43);
                result_peg_50 = rule_filename_char(stream, result_peg_50.getPosition());
                if (result_peg_50.error()){
                    goto not_peg_49;
                }
                goto out_peg_45;
                not_peg_49:
                result_peg_43.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_44.getValues());
                    result_peg_43.setValue(value);
                }
            
            
        }
        stream.update(result_peg_43.getPosition());
        
        return result_peg_43;
        out_peg_45:
        Result result_peg_51(myposition);
        
        result_peg_51 = rule_filename(stream, result_peg_51.getPosition());
        if (result_peg_51.error()){
            goto out_peg_52;
        }
        stream.update(result_peg_51.getPosition());
        
        return result_peg_51;
        out_peg_52:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    RuleTrace trace_peg_14(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareChar("normal"[i], stream.get(result_peg_2.getPosition()))){
                result_peg_2.nextPosition();
            } else {
                goto out_peg_3;
            }
        }
        result_peg_2.setValue((void*) "normal");
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_3:
        Result result_peg_4(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareChar("parallax"[i], stream.get(result_peg_4.getPosition()))){
                result_peg_4.nextPosition();
            } else {
                goto out_peg_5;
            }
        }
        result_peg_4.setValue((void*) "parallax");
        stream.update(result_peg_4.getPosition());
        
        return result_peg_4;
        out_peg_5:
        Result result_peg_6(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareChar("addalpha"[i], stream.get(result_peg_6.getPosition()))){
                result_peg_6.nextPosition();
            } else {
                goto out_peg_7;
            }
        }
        result_peg_6.setValue((void*) "addalpha");
        stream.update(result_peg_6.getPosition());
        
        return result_peg_6;
        out_peg_7:
        Result result_peg_8(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareChar("add1"[i], stream.get(result_peg_8.getPosition()))){
                result_peg_8.nextPosition();
            } else {
                goto out_peg_9;
            }
        }
        result_peg_8.setValue((void*) "add1");
        stream.update(result_peg_8.getPosition());
        
        return result_peg_8;
        out_peg_9:
        Result result_peg_10(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareChar("add"[i], stream.get(result_peg_10.getPosition()))){
                result_peg_10.nextPosition();
            } else {
                goto out_peg_11;
            }
        }
        result_peg_10.setValue((void*) "add");
        stream.update(result_peg_10.getPosition());
        
        return result_peg_10;
        out_peg_11:
        Result result_peg_12(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareChar("sub"[i], stream.get(result_peg_12.getPosition()))){
                result_peg_12.nextPosition();
            } else {
                goto out_peg_13;
            }
        }
        result_peg_12.setValue((void*) "sub");
        stream.update(result_peg_12.getPosition());
        
        return result_peg_12;
        out_peg_13:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    RuleTrace trace_peg_58(stream, "date");
    int myposition = position;
    
    
    
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_10 = stream.get(result_peg_5.getPosition());
                        if (letter_peg_10 != '\0' && strchr("0123456789", letter_peg_10) != NULL){
                            result_peg_5.nextPosition();
                            result_peg_5.setValue((void*) (long) letter_peg_10);
                        } else {
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
                    goto out_peg_11;
                }
            
            Result result_peg_3 = result_peg_2;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
                result_peg_2.setValue((void*) ".");
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_15(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_20 = stream.get(result_peg_15.getPosition());
                        if (letter_peg_20 != '\0' && strchr("0123456789", letter_peg_20) != NULL){
                            result_peg_15.nextPosition();
                            result_peg_15.setValue((void*) (long) letter_peg_20);
                        } else {
                            goto out_peg_19;
                        }
                        
                    }
                    goto success_peg_16;
                    out_peg_19:
                    goto loop_peg_14;
                    success_peg_16:
                    ;
                    result_peg_2.addResult(result_peg_15);
                } while (true);
                loop_peg_14:
                if (result_peg_2.matches() == 0){
                    goto out_peg_11;
                }
            
            Result result_peg_13 = result_peg_2;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_11;
                    }
                }
                result_peg_2.setValue((void*) ".");
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_24(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_29 = stream.get(result_peg_24.getPosition());
                        if (letter_peg_29 != '\0' && strchr("0123456789", letter_peg_29) != NULL){
                            result_peg_24.nextPosition();
                            result_peg_24.setValue((void*) (long) letter_peg_29);
                        } else {
                            goto out_peg_28;
                        }
                        
                    }
                    goto success_peg_25;
                    out_peg_28:
                    goto loop_peg_23;
                    success_peg_25:
                    ;
                    result_peg_2.addResult(result_peg_24);
                } while (true);
                loop_peg_23:
                if (result_peg_2.matches() == 0){
                    goto out_peg_11;
                }
            
            Result result_peg_22 = result_peg_2;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_3.getValues(),result_peg_13.getValues(),result_peg_22.getValues());
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_11:
        Result result_peg_30(myposition);
        
        {
        
            result_peg_30.reset();
                do{
                    Result result_peg_33(result_peg_30.getPosition());
                    {
                        
                        char letter_peg_38 = stream.get(result_peg_33.getPosition());
                        if (letter_peg_38 != '\0' && strchr("0123456789", letter_peg_38) != NULL){
                            result_peg_33.nextPosition();
                            result_peg_33.setValue((void*) (long) letter_peg_38);
                        } else {
                            goto out_peg_37;
                        }
                        
                    }
                    goto success_peg_34;
                    out_peg_37:
                    goto loop_peg_32;
                    success_peg_34:
                    ;
                    result_peg_30.addResult(result_peg_33);
                } while (true);
                loop_peg_32:
                if (result_peg_30.matches() == 0){
                    goto out_peg_39;
                }
            
            Result result_peg_31 = result_peg_30;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_39;
                    }
                }
                result_peg_30.setValue((void*) "/");
            
            
            
            result_peg_30.reset();
                do{
                    Result result_peg_43(result_peg_30.getPosition());
                    {
                        
                        char letter_peg_48 = stream.get(result_peg_43.getPosition());
                        if (letter_peg_48 != '\0' && strchr("0123456789", letter_peg_48) != NULL){
                            result_peg_43.nextPosition();
                            result_peg_43.setValue((void*) (long) letter_peg_48);
                        } else {
                            goto out_peg_47;
                        }
                        
                    }
                    goto success_peg_44;
                    out_peg_47:
                    goto loop_peg_42;
                    success_peg_44:
                    ;
                    result_peg_30.addResult(result_peg_43);
                } while (true);
                loop_peg_42:
                if (result_peg_30.matches() == 0){
                    goto out_peg_39;
                }
            
            Result result_peg_41 = result_peg_30;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_39;
                    }
                }
                result_peg_30.setValue((void*) "/");
            
            
            
            result_peg_30.reset();
                do{
                    Result result_peg_52(result_peg_30.getPosition());
                    {
                        
                        char letter_peg_57 = stream.get(result_peg_52.getPosition());
                        if (letter_peg_57 != '\0' && strchr("0123456789", letter_peg_57) != NULL){
                            result_peg_52.nextPosition();
                            result_peg_52.setValue((void*) (long) letter_peg_57);
                        } else {
                            goto out_peg_56;
                        }
                        
                    }
                    goto success_peg_53;
                    out_peg_56:
                    goto loop_peg_51;
                    success_peg_53:
                    ;
                    result_peg_30.addResult(result_peg_52);
                } while (true);
                loop_peg_51:
                if (result_peg_30.matches() == 0){
                    goto out_peg_39;
                }
            
            Result result_peg_50 = result_peg_30;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_31.getValues(),result_peg_41.getValues(),result_peg_50.getValues());
                    result_peg_30.setValue(value);
                }
            
            
        }
        stream.update(result_peg_30.getPosition());
        
        return result_peg_30;
        out_peg_39:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    RuleTrace trace_peg_16(stream, "string");
    int myposition = position;
    
    
    Value data;
    Result result_peg_2(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "\"");
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_7(result_peg_2.getPosition());
                    {
                    
                        Result result_peg_10(result_peg_7);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\""[i], stream.get(result_peg_10.getPosition()))){
                                    result_peg_10.nextPosition();
                                } else {
                                    goto not_peg_9;
                                }
                            }
                            result_peg_10.setValue((void*) "\"");
                            goto loop_peg_6;
                            not_peg_9:
                            result_peg_7.setValue((void*)0);
                        
                        
                        
                        Result result_peg_13(result_peg_7);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_13.getPosition()))){
                                    result_peg_13.nextPosition();
                                } else {
                                    goto not_peg_12;
                                }
                            }
                            result_peg_13.setValue((void*) "\n");
                            goto loop_peg_6;
                            not_peg_12:
                            result_peg_7.setValue((void*)0);
                        
                        
                        
                        char temp_peg_14 = stream.get(result_peg_7.getPosition());
                            if (temp_peg_14 != '\0'){
                                result_peg_7.setValue((void*) (long) temp_peg_14);
                                result_peg_7.nextPosition();
                            } else {
                                goto loop_peg_6;
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_7);
                } while (true);
                loop_peg_6:
                ;
                data = result_peg_2.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_4;
                    }
                }
                result_peg_2.setValue((void*) "\"");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeString(data);
                    result_peg_2.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2.getPosition());
        
        return result_peg_2;
        out_peg_4:
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
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_7;
                        }
                    }
                    result_peg_2.setValue((void*) "+");
                        
                }
                goto success_peg_5;
                out_peg_7:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_2.getPosition()))){
                            result_peg_2.nextPosition();
                        } else {
                            goto out_peg_9;
                        }
                    }
                    result_peg_2.setValue((void*) "-");
                        
                }
                goto success_peg_5;
                out_peg_9:
                
                result_peg_2 = Result(save_peg_4);
                result_peg_2.setValue((void*) 0);
                
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
    
    RuleTrace trace_peg_34(stream, "float_or_integer");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_2(myposition);
        
        {
        
            result_peg_2.reset();
                do{
                    Result result_peg_5(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_10 = stream.get(result_peg_5.getPosition());
                        if (letter_peg_10 != '\0' && strchr("0123456789", letter_peg_10) != NULL){
                            result_peg_5.nextPosition();
                            result_peg_5.setValue((void*) (long) letter_peg_10);
                        } else {
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
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_2.getPosition()))){
                        result_peg_2.nextPosition();
                    } else {
                        goto out_peg_12;
                    }
                }
                result_peg_2.setValue((void*) ".");
            
            
            
            result_peg_2.reset();
                do{
                    Result result_peg_15(result_peg_2.getPosition());
                    {
                        
                        char letter_peg_20 = stream.get(result_peg_15.getPosition());
                        if (letter_peg_20 != '\0' && strchr("0123456789", letter_peg_20) != NULL){
                            result_peg_15.nextPosition();
                            result_peg_15.setValue((void*) (long) letter_peg_20);
                        } else {
                            goto out_peg_19;
                        }
                        
                    }
                    goto success_peg_16;
                    out_peg_19:
                    goto loop_peg_14;
                    success_peg_16:
                    ;
                    result_peg_2.addResult(result_peg_15);
                } while (true);
                loop_peg_14:
                if (result_peg_2.matches() == 0){
                    goto out_peg_12;
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
        out_peg_12:
        Result result_peg_21(myposition);
        
        {
        
            result_peg_21.reset();
                do{
                    Result result_peg_24(result_peg_21.getPosition());
                    {
                        
                        char letter_peg_29 = stream.get(result_peg_24.getPosition());
                        if (letter_peg_29 != '\0' && strchr("0123456789", letter_peg_29) != NULL){
                            result_peg_24.nextPosition();
                            result_peg_24.setValue((void*) (long) letter_peg_29);
                        } else {
                            goto out_peg_28;
                        }
                        
                    }
                    goto success_peg_25;
                    out_peg_28:
                    goto loop_peg_23;
                    success_peg_25:
                    ;
                    result_peg_21.addResult(result_peg_24);
                } while (true);
                loop_peg_23:
                if (result_peg_21.matches() == 0){
                    goto out_peg_30;
                }
            
            Result result_peg_22 = result_peg_21;
            
            Result result_peg_33(result_peg_21);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_33.getPosition()))){
                        result_peg_33.nextPosition();
                    } else {
                        goto not_peg_32;
                    }
                }
                result_peg_33.setValue((void*) ".");
                goto out_peg_30;
                not_peg_32:
                result_peg_21.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_22.getValues());
                    result_peg_21.setValue(value);
                }
            
            
        }
        stream.update(result_peg_21.getPosition());
        
        return result_peg_21;
        out_peg_30:
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



    
    } /* Def */
    
} /* Mugen */

        
