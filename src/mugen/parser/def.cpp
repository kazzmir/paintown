

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
    
    RuleTrace trace_peg_480(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_391(myposition);
        
        {
        
            {
                    
                    result_peg_391.reset();
                    do{
                        Result result_peg_403(result_peg_391.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_403.getPosition()))){
                                    result_peg_403.nextPosition();
                                } else {
                                    goto out_peg_406;
                                }
                            }
                            result_peg_403.setValue((void*) " ");
                                
                        }
                        goto success_peg_404;
                        out_peg_406:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_403.getPosition()))){
                                    result_peg_403.nextPosition();
                                } else {
                                    goto out_peg_408;
                                }
                            }
                            result_peg_403.setValue((void*) "\t");
                                
                        }
                        goto success_peg_404;
                        out_peg_408:
                        goto loop_peg_402;
                        success_peg_404:
                        ;
                        result_peg_391.addResult(result_peg_403);
                    } while (true);
                    loop_peg_402:
                    ;
                            
                }
                goto success_peg_393;
                goto out_peg_409;
                success_peg_393:
                ;
            
            
            
            {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_391.getPosition()))){
                                    result_peg_391.nextPosition();
                                } else {
                                    goto out_peg_413;
                                }
                            }
                            result_peg_391.setValue((void*) ";");
                        
                        
                        
                        result_peg_391.reset();
                            do{
                                Result result_peg_422(result_peg_391.getPosition());
                                {
                                
                                    Result result_peg_425(result_peg_422);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_425.getPosition()))){
                                                result_peg_425.nextPosition();
                                            } else {
                                                goto not_peg_424;
                                            }
                                        }
                                        result_peg_425.setValue((void*) "\n");
                                        goto loop_peg_421;
                                        not_peg_424:
                                        result_peg_422.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_426 = stream.get(result_peg_422.getPosition());
                                        if (temp_peg_426 != '\0'){
                                            result_peg_422.setValue((void*) (long) temp_peg_426);
                                            result_peg_422.nextPosition();
                                        } else {
                                            goto loop_peg_421;
                                        }
                                    
                                    
                                }
                                result_peg_391.addResult(result_peg_422);
                            } while (true);
                            loop_peg_421:
                            ;
                        
                        
                    }
                }
                goto success_peg_410;
                out_peg_413:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_391.getPosition()))){
                                    result_peg_391.nextPosition();
                                } else {
                                    goto out_peg_429;
                                }
                            }
                            result_peg_391.setValue((void*) "=");
                        
                        
                        
                        result_peg_391.reset();
                            do{
                                Result result_peg_438(result_peg_391.getPosition());
                                {
                                
                                    Result result_peg_441(result_peg_438);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_441.getPosition()))){
                                                result_peg_441.nextPosition();
                                            } else {
                                                goto not_peg_440;
                                            }
                                        }
                                        result_peg_441.setValue((void*) "\n");
                                        goto loop_peg_437;
                                        not_peg_440:
                                        result_peg_438.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_442 = stream.get(result_peg_438.getPosition());
                                        if (temp_peg_442 != '\0'){
                                            result_peg_438.setValue((void*) (long) temp_peg_442);
                                            result_peg_438.nextPosition();
                                        } else {
                                            goto loop_peg_437;
                                        }
                                    
                                    
                                }
                                result_peg_391.addResult(result_peg_438);
                            } while (true);
                            loop_peg_437:
                            ;
                        
                        
                    }
                }
                goto success_peg_410;
                out_peg_429:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_391.getPosition()))){
                                    result_peg_391.nextPosition();
                                } else {
                                    goto out_peg_445;
                                }
                            }
                            result_peg_391.setValue((void*) "-");
                        
                        
                        
                        result_peg_391.reset();
                            do{
                                Result result_peg_450(result_peg_391.getPosition());
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_450.getPosition()))){
                                        result_peg_450.nextPosition();
                                    } else {
                                        goto loop_peg_449;
                                    }
                                }
                                result_peg_450.setValue((void*) "-");
                                result_peg_391.addResult(result_peg_450);
                            } while (true);
                            loop_peg_449:
                            if (result_peg_391.matches() == 0){
                                goto out_peg_445;
                            }
                        
                        
                    }
                }
                goto success_peg_410;
                out_peg_445:
                goto out_peg_409;
                success_peg_410:
                ;
            
            
        }
        stream.update(result_peg_391.getPosition());
        
        return result_peg_391;
        out_peg_409:
        Result result_peg_451(myposition);
        
        {
        
            {
                    
                    result_peg_451.reset();
                    do{
                        Result result_peg_463(result_peg_451.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_463.getPosition()))){
                                    result_peg_463.nextPosition();
                                } else {
                                    goto out_peg_466;
                                }
                            }
                            result_peg_463.setValue((void*) " ");
                                
                        }
                        goto success_peg_464;
                        out_peg_466:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_463.getPosition()))){
                                    result_peg_463.nextPosition();
                                } else {
                                    goto out_peg_468;
                                }
                            }
                            result_peg_463.setValue((void*) "\t");
                                
                        }
                        goto success_peg_464;
                        out_peg_468:
                        goto loop_peg_462;
                        success_peg_464:
                        ;
                        result_peg_451.addResult(result_peg_463);
                    } while (true);
                    loop_peg_462:
                    ;
                            
                }
                goto success_peg_453;
                goto out_peg_469;
                success_peg_453:
                ;
            
            
            
            result_peg_451 = rule_section(stream, result_peg_451.getPosition());
                if (result_peg_451.error()){
                    goto out_peg_469;
                }
            
            Result result_peg_470 = result_peg_451;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_470.getValues());
                    result_peg_451.setValue(value);
                }
            
            
        }
        stream.update(result_peg_451.getPosition());
        
        return result_peg_451;
        out_peg_469:
        Result result_peg_471(myposition);
        
        result_peg_471.reset();
        do{
            Result result_peg_473(result_peg_471.getPosition());
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar(" "[i], stream.get(result_peg_473.getPosition()))){
                        result_peg_473.nextPosition();
                    } else {
                        goto out_peg_476;
                    }
                }
                result_peg_473.setValue((void*) " ");
                    
            }
            goto success_peg_474;
            out_peg_476:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\t"[i], stream.get(result_peg_473.getPosition()))){
                        result_peg_473.nextPosition();
                    } else {
                        goto out_peg_478;
                    }
                }
                result_peg_473.setValue((void*) "\t");
                    
            }
            goto success_peg_474;
            out_peg_478:
            goto loop_peg_472;
            success_peg_474:
            ;
            result_peg_471.addResult(result_peg_473);
        } while (true);
        loop_peg_472:
        if (result_peg_471.matches() == 0){
            goto out_peg_479;
        }
        stream.update(result_peg_471.getPosition());
        
        return result_peg_471;
        out_peg_479:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    RuleTrace trace_peg_495(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_482(myposition);
        
        result_peg_482.reset();
        do{
            Result result_peg_484(result_peg_482.getPosition());
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_484.getPosition()))){
                        result_peg_484.nextPosition();
                    } else {
                        goto out_peg_487;
                    }
                }
                result_peg_484.setValue((void*) "\n");
                    
            }
            goto success_peg_485;
            out_peg_487:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_484.getPosition()))){
                        result_peg_484.nextPosition();
                    } else {
                        goto out_peg_489;
                    }
                }
                result_peg_484.setValue((void*) "\r");
                    
            }
            goto success_peg_485;
            out_peg_489:
            goto loop_peg_483;
            success_peg_485:
            ;
            result_peg_482.addResult(result_peg_484);
        } while (true);
        loop_peg_483:
        if (result_peg_482.matches() == 0){
            goto out_peg_490;
        }
        stream.update(result_peg_482.getPosition());
        
        return result_peg_482;
        out_peg_490:
        Result result_peg_491(myposition);
        
        {
        
            Result result_peg_493(result_peg_491.getPosition());
                if ('\0' == stream.get(result_peg_493.getPosition())){
                    result_peg_493.nextPosition();
                    result_peg_493.setValue((void *) '\0');
                } else {
                    goto out_peg_494;
                }
            
            
            
            
            
            
        }
        stream.update(result_peg_491.getPosition());
        
        return result_peg_491;
        out_peg_494:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    RuleTrace trace_peg_883(stream, "section");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_497(myposition);
        
        {
        
            result_peg_497 = rule_section_start(stream, result_peg_497.getPosition());
                if (result_peg_497.error()){
                    goto out_peg_499;
                }
            
            Result result_peg_498 = result_peg_497;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_498.getValues());
                    result_peg_497.setValue(value);
                }
                ast = result_peg_497.getValues();
            
            
            
            {
                    
                    result_peg_497.reset();
                    do{
                        Result result_peg_596(result_peg_497.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_596.getPosition()))){
                                    result_peg_596.nextPosition();
                                } else {
                                    goto out_peg_599;
                                }
                            }
                            result_peg_596.setValue((void*) " ");
                                
                        }
                        goto success_peg_597;
                        out_peg_599:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_596.getPosition()))){
                                    result_peg_596.nextPosition();
                                } else {
                                    goto out_peg_601;
                                }
                            }
                            result_peg_596.setValue((void*) "\t");
                                
                        }
                        goto success_peg_597;
                        out_peg_601:
                        {
                            
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar(";"[i], stream.get(result_peg_596.getPosition()))){
                                                result_peg_596.nextPosition();
                                            } else {
                                                goto out_peg_648;
                                            }
                                        }
                                        result_peg_596.setValue((void*) ";");
                                    
                                    
                                    
                                    result_peg_596.reset();
                                        do{
                                            Result result_peg_657(result_peg_596.getPosition());
                                            {
                                            
                                                Result result_peg_660(result_peg_657);
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_660.getPosition()))){
                                                            result_peg_660.nextPosition();
                                                        } else {
                                                            goto not_peg_659;
                                                        }
                                                    }
                                                    result_peg_660.setValue((void*) "\n");
                                                    goto loop_peg_656;
                                                    not_peg_659:
                                                    result_peg_657.setValue((void*)0);
                                                
                                                
                                                
                                                char temp_peg_661 = stream.get(result_peg_657.getPosition());
                                                    if (temp_peg_661 != '\0'){
                                                        result_peg_657.setValue((void*) (long) temp_peg_661);
                                                        result_peg_657.nextPosition();
                                                    } else {
                                                        goto loop_peg_656;
                                                    }
                                                
                                                
                                            }
                                            result_peg_596.addResult(result_peg_657);
                                        } while (true);
                                        loop_peg_656:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_645;
                            out_peg_648:
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar("="[i], stream.get(result_peg_596.getPosition()))){
                                                result_peg_596.nextPosition();
                                            } else {
                                                goto out_peg_664;
                                            }
                                        }
                                        result_peg_596.setValue((void*) "=");
                                    
                                    
                                    
                                    result_peg_596.reset();
                                        do{
                                            Result result_peg_673(result_peg_596.getPosition());
                                            {
                                            
                                                Result result_peg_676(result_peg_673);
                                                    for (int i = 0; i < 1; i++){
                                                        if (compareChar("\n"[i], stream.get(result_peg_676.getPosition()))){
                                                            result_peg_676.nextPosition();
                                                        } else {
                                                            goto not_peg_675;
                                                        }
                                                    }
                                                    result_peg_676.setValue((void*) "\n");
                                                    goto loop_peg_672;
                                                    not_peg_675:
                                                    result_peg_673.setValue((void*)0);
                                                
                                                
                                                
                                                char temp_peg_677 = stream.get(result_peg_673.getPosition());
                                                    if (temp_peg_677 != '\0'){
                                                        result_peg_673.setValue((void*) (long) temp_peg_677);
                                                        result_peg_673.nextPosition();
                                                    } else {
                                                        goto loop_peg_672;
                                                    }
                                                
                                                
                                            }
                                            result_peg_596.addResult(result_peg_673);
                                        } while (true);
                                        loop_peg_672:
                                        ;
                                    
                                    
                                }
                            }
                            goto success_peg_645;
                            out_peg_664:
                            {
                                {
                                
                                    for (int i = 0; i < 1; i++){
                                            if (compareChar("-"[i], stream.get(result_peg_596.getPosition()))){
                                                result_peg_596.nextPosition();
                                            } else {
                                                goto out_peg_680;
                                            }
                                        }
                                        result_peg_596.setValue((void*) "-");
                                    
                                    
                                    
                                    result_peg_596.reset();
                                        do{
                                            Result result_peg_685(result_peg_596.getPosition());
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("-"[i], stream.get(result_peg_685.getPosition()))){
                                                    result_peg_685.nextPosition();
                                                } else {
                                                    goto loop_peg_684;
                                                }
                                            }
                                            result_peg_685.setValue((void*) "-");
                                            result_peg_596.addResult(result_peg_685);
                                        } while (true);
                                        loop_peg_684:
                                        if (result_peg_596.matches() == 0){
                                            goto out_peg_680;
                                        }
                                    
                                    
                                }
                            }
                            goto success_peg_645;
                            out_peg_680:
                            goto out_peg_644;
                            success_peg_645:
                            ;
                            
                        }
                        goto success_peg_597;
                        out_peg_644:
                        goto loop_peg_595;
                        success_peg_597:
                        ;
                        result_peg_497.addResult(result_peg_596);
                    } while (true);
                    loop_peg_595:
                    ;
                            
                }
                goto success_peg_502;
                goto out_peg_499;
                success_peg_502:
                ;
            
            
            
            result_peg_497.reset();
                do{
                    Result result_peg_688(result_peg_497.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_688.getPosition()))){
                                result_peg_688.nextPosition();
                            } else {
                                goto out_peg_691;
                            }
                        }
                        result_peg_688.setValue((void*) "\n");
                            
                    }
                    goto success_peg_689;
                    out_peg_691:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_688.getPosition()))){
                                result_peg_688.nextPosition();
                            } else {
                                goto out_peg_693;
                            }
                        }
                        result_peg_688.setValue((void*) "\r");
                            
                    }
                    goto success_peg_689;
                    out_peg_693:
                    goto loop_peg_687;
                    success_peg_689:
                    ;
                    result_peg_497.addResult(result_peg_688);
                } while (true);
                loop_peg_687:
                if (result_peg_497.matches() == 0){
                    goto out_peg_499;
                }
            
            
            
            result_peg_497.reset();
                do{
                    Result result_peg_696(result_peg_497.getPosition());
                    {
                    
                        result_peg_696 = rule_section_line(stream, result_peg_696.getPosition(), ast);
                            if (result_peg_696.error()){
                                goto loop_peg_695;
                            }
                        
                        
                        
                        {
                                
                                result_peg_696.reset();
                                do{
                                    Result result_peg_793(result_peg_696.getPosition());
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
                                        
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar(";"[i], stream.get(result_peg_793.getPosition()))){
                                                            result_peg_793.nextPosition();
                                                        } else {
                                                            goto out_peg_845;
                                                        }
                                                    }
                                                    result_peg_793.setValue((void*) ";");
                                                
                                                
                                                
                                                result_peg_793.reset();
                                                    do{
                                                        Result result_peg_854(result_peg_793.getPosition());
                                                        {
                                                        
                                                            Result result_peg_857(result_peg_854);
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_857.getPosition()))){
                                                                        result_peg_857.nextPosition();
                                                                    } else {
                                                                        goto not_peg_856;
                                                                    }
                                                                }
                                                                result_peg_857.setValue((void*) "\n");
                                                                goto loop_peg_853;
                                                                not_peg_856:
                                                                result_peg_854.setValue((void*)0);
                                                            
                                                            
                                                            
                                                            char temp_peg_858 = stream.get(result_peg_854.getPosition());
                                                                if (temp_peg_858 != '\0'){
                                                                    result_peg_854.setValue((void*) (long) temp_peg_858);
                                                                    result_peg_854.nextPosition();
                                                                } else {
                                                                    goto loop_peg_853;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_793.addResult(result_peg_854);
                                                    } while (true);
                                                    loop_peg_853:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_842;
                                        out_peg_845:
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar("="[i], stream.get(result_peg_793.getPosition()))){
                                                            result_peg_793.nextPosition();
                                                        } else {
                                                            goto out_peg_861;
                                                        }
                                                    }
                                                    result_peg_793.setValue((void*) "=");
                                                
                                                
                                                
                                                result_peg_793.reset();
                                                    do{
                                                        Result result_peg_870(result_peg_793.getPosition());
                                                        {
                                                        
                                                            Result result_peg_873(result_peg_870);
                                                                for (int i = 0; i < 1; i++){
                                                                    if (compareChar("\n"[i], stream.get(result_peg_873.getPosition()))){
                                                                        result_peg_873.nextPosition();
                                                                    } else {
                                                                        goto not_peg_872;
                                                                    }
                                                                }
                                                                result_peg_873.setValue((void*) "\n");
                                                                goto loop_peg_869;
                                                                not_peg_872:
                                                                result_peg_870.setValue((void*)0);
                                                            
                                                            
                                                            
                                                            char temp_peg_874 = stream.get(result_peg_870.getPosition());
                                                                if (temp_peg_874 != '\0'){
                                                                    result_peg_870.setValue((void*) (long) temp_peg_874);
                                                                    result_peg_870.nextPosition();
                                                                } else {
                                                                    goto loop_peg_869;
                                                                }
                                                            
                                                            
                                                        }
                                                        result_peg_793.addResult(result_peg_870);
                                                    } while (true);
                                                    loop_peg_869:
                                                    ;
                                                
                                                
                                            }
                                        }
                                        goto success_peg_842;
                                        out_peg_861:
                                        {
                                            {
                                            
                                                for (int i = 0; i < 1; i++){
                                                        if (compareChar("-"[i], stream.get(result_peg_793.getPosition()))){
                                                            result_peg_793.nextPosition();
                                                        } else {
                                                            goto out_peg_877;
                                                        }
                                                    }
                                                    result_peg_793.setValue((void*) "-");
                                                
                                                
                                                
                                                result_peg_793.reset();
                                                    do{
                                                        Result result_peg_882(result_peg_793.getPosition());
                                                        for (int i = 0; i < 1; i++){
                                                            if (compareChar("-"[i], stream.get(result_peg_882.getPosition()))){
                                                                result_peg_882.nextPosition();
                                                            } else {
                                                                goto loop_peg_881;
                                                            }
                                                        }
                                                        result_peg_882.setValue((void*) "-");
                                                        result_peg_793.addResult(result_peg_882);
                                                    } while (true);
                                                    loop_peg_881:
                                                    if (result_peg_793.matches() == 0){
                                                        goto out_peg_877;
                                                    }
                                                
                                                
                                            }
                                        }
                                        goto success_peg_842;
                                        out_peg_877:
                                        goto out_peg_841;
                                        success_peg_842:
                                        ;
                                        
                                    }
                                    goto success_peg_794;
                                    out_peg_841:
                                    goto loop_peg_792;
                                    success_peg_794:
                                    ;
                                    result_peg_696.addResult(result_peg_793);
                                } while (true);
                                loop_peg_792:
                                ;
                                        
                            }
                            goto success_peg_699;
                            goto loop_peg_695;
                            success_peg_699:
                            ;
                        
                        
                        
                        result_peg_696 = rule_line_end(stream, result_peg_696.getPosition());
                            if (result_peg_696.error()){
                                goto loop_peg_695;
                            }
                        
                        
                    }
                    result_peg_497.addResult(result_peg_696);
                } while (true);
                loop_peg_695:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_497.setValue(value);
                }
            
            
        }
        stream.update(result_peg_497.getPosition());
        
        return result_peg_497;
        out_peg_499:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_line(Stream & stream, const int position, Value section){
    
    RuleTrace trace_peg_1017(stream, "section_line");
    int myposition = position;
    
    
    Value data;
    Result result_peg_885(myposition);
        
        {
        
            {
                    
                    result_peg_885.reset();
                    do{
                        Result result_peg_897(result_peg_885.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_897.getPosition()))){
                                    result_peg_897.nextPosition();
                                } else {
                                    goto out_peg_900;
                                }
                            }
                            result_peg_897.setValue((void*) " ");
                                
                        }
                        goto success_peg_898;
                        out_peg_900:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_897.getPosition()))){
                                    result_peg_897.nextPosition();
                                } else {
                                    goto out_peg_902;
                                }
                            }
                            result_peg_897.setValue((void*) "\t");
                                
                        }
                        goto success_peg_898;
                        out_peg_902:
                        goto loop_peg_896;
                        success_peg_898:
                        ;
                        result_peg_885.addResult(result_peg_897);
                    } while (true);
                    loop_peg_896:
                    ;
                            
                }
                goto success_peg_887;
                goto out_peg_903;
                success_peg_887:
                ;
            
            
            
            {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar(";"[i], stream.get(result_peg_885.getPosition()))){
                                    result_peg_885.nextPosition();
                                } else {
                                    goto out_peg_907;
                                }
                            }
                            result_peg_885.setValue((void*) ";");
                        
                        
                        
                        result_peg_885.reset();
                            do{
                                Result result_peg_916(result_peg_885.getPosition());
                                {
                                
                                    Result result_peg_919(result_peg_916);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_919.getPosition()))){
                                                result_peg_919.nextPosition();
                                            } else {
                                                goto not_peg_918;
                                            }
                                        }
                                        result_peg_919.setValue((void*) "\n");
                                        goto loop_peg_915;
                                        not_peg_918:
                                        result_peg_916.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_920 = stream.get(result_peg_916.getPosition());
                                        if (temp_peg_920 != '\0'){
                                            result_peg_916.setValue((void*) (long) temp_peg_920);
                                            result_peg_916.nextPosition();
                                        } else {
                                            goto loop_peg_915;
                                        }
                                    
                                    
                                }
                                result_peg_885.addResult(result_peg_916);
                            } while (true);
                            loop_peg_915:
                            ;
                        
                        
                    }
                }
                goto success_peg_904;
                out_peg_907:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("="[i], stream.get(result_peg_885.getPosition()))){
                                    result_peg_885.nextPosition();
                                } else {
                                    goto out_peg_923;
                                }
                            }
                            result_peg_885.setValue((void*) "=");
                        
                        
                        
                        result_peg_885.reset();
                            do{
                                Result result_peg_932(result_peg_885.getPosition());
                                {
                                
                                    Result result_peg_935(result_peg_932);
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\n"[i], stream.get(result_peg_935.getPosition()))){
                                                result_peg_935.nextPosition();
                                            } else {
                                                goto not_peg_934;
                                            }
                                        }
                                        result_peg_935.setValue((void*) "\n");
                                        goto loop_peg_931;
                                        not_peg_934:
                                        result_peg_932.setValue((void*)0);
                                    
                                    
                                    
                                    char temp_peg_936 = stream.get(result_peg_932.getPosition());
                                        if (temp_peg_936 != '\0'){
                                            result_peg_932.setValue((void*) (long) temp_peg_936);
                                            result_peg_932.nextPosition();
                                        } else {
                                            goto loop_peg_931;
                                        }
                                    
                                    
                                }
                                result_peg_885.addResult(result_peg_932);
                            } while (true);
                            loop_peg_931:
                            ;
                        
                        
                    }
                }
                goto success_peg_904;
                out_peg_923:
                {
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("-"[i], stream.get(result_peg_885.getPosition()))){
                                    result_peg_885.nextPosition();
                                } else {
                                    goto out_peg_939;
                                }
                            }
                            result_peg_885.setValue((void*) "-");
                        
                        
                        
                        result_peg_885.reset();
                            do{
                                Result result_peg_944(result_peg_885.getPosition());
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("-"[i], stream.get(result_peg_944.getPosition()))){
                                        result_peg_944.nextPosition();
                                    } else {
                                        goto loop_peg_943;
                                    }
                                }
                                result_peg_944.setValue((void*) "-");
                                result_peg_885.addResult(result_peg_944);
                            } while (true);
                            loop_peg_943:
                            if (result_peg_885.matches() == 0){
                                goto out_peg_939;
                            }
                        
                        
                    }
                }
                goto success_peg_904;
                out_peg_939:
                goto out_peg_903;
                success_peg_904:
                ;
            
            
        }
        stream.update(result_peg_885.getPosition());
        
        return result_peg_885;
        out_peg_903:
        Result result_peg_945(myposition);
        
        {
        
            {
                    
                    result_peg_945.reset();
                    do{
                        Result result_peg_957(result_peg_945.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_957.getPosition()))){
                                    result_peg_957.nextPosition();
                                } else {
                                    goto out_peg_960;
                                }
                            }
                            result_peg_957.setValue((void*) " ");
                                
                        }
                        goto success_peg_958;
                        out_peg_960:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_957.getPosition()))){
                                    result_peg_957.nextPosition();
                                } else {
                                    goto out_peg_962;
                                }
                            }
                            result_peg_957.setValue((void*) "\t");
                                
                        }
                        goto success_peg_958;
                        out_peg_962:
                        goto loop_peg_956;
                        success_peg_958:
                        ;
                        result_peg_945.addResult(result_peg_957);
                    } while (true);
                    loop_peg_956:
                    ;
                            
                }
                goto success_peg_947;
                goto out_peg_963;
                success_peg_947:
                ;
            
            
            
            result_peg_945 = rule_attribute(stream, result_peg_945.getPosition());
                if (result_peg_945.error()){
                    goto out_peg_963;
                }
                data = result_peg_945.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addAttribute(asAttribute(data));
                    result_peg_945.setValue(value);
                }
            
            
        }
        stream.update(result_peg_945.getPosition());
        
        return result_peg_945;
        out_peg_963:
        Result result_peg_965(myposition);
        
        {
        
            {
                    
                    result_peg_965.reset();
                    do{
                        Result result_peg_977(result_peg_965.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_977.getPosition()))){
                                    result_peg_977.nextPosition();
                                } else {
                                    goto out_peg_980;
                                }
                            }
                            result_peg_977.setValue((void*) " ");
                                
                        }
                        goto success_peg_978;
                        out_peg_980:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_977.getPosition()))){
                                    result_peg_977.nextPosition();
                                } else {
                                    goto out_peg_982;
                                }
                            }
                            result_peg_977.setValue((void*) "\t");
                                
                        }
                        goto success_peg_978;
                        out_peg_982:
                        goto loop_peg_976;
                        success_peg_978:
                        ;
                        result_peg_965.addResult(result_peg_977);
                    } while (true);
                    loop_peg_976:
                    ;
                            
                }
                goto success_peg_967;
                goto out_peg_983;
                success_peg_967:
                ;
            
            
            
            result_peg_965 = rule_loopstart(stream, result_peg_965.getPosition());
                if (result_peg_965.error()){
                    goto out_peg_983;
                }
                data = result_peg_965.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_965.setValue(value);
                }
            
            
        }
        stream.update(result_peg_965.getPosition());
        
        return result_peg_965;
        out_peg_983:
        Result result_peg_985(myposition);
        
        {
        
            {
                    
                    result_peg_985.reset();
                    do{
                        Result result_peg_997(result_peg_985.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_997.getPosition()))){
                                    result_peg_997.nextPosition();
                                } else {
                                    goto out_peg_1000;
                                }
                            }
                            result_peg_997.setValue((void*) " ");
                                
                        }
                        goto success_peg_998;
                        out_peg_1000:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_997.getPosition()))){
                                    result_peg_997.nextPosition();
                                } else {
                                    goto out_peg_1002;
                                }
                            }
                            result_peg_997.setValue((void*) "\t");
                                
                        }
                        goto success_peg_998;
                        out_peg_1002:
                        goto loop_peg_996;
                        success_peg_998:
                        ;
                        result_peg_985.addResult(result_peg_997);
                    } while (true);
                    loop_peg_996:
                    ;
                            
                }
                goto success_peg_987;
                goto out_peg_1003;
                success_peg_987:
                ;
            
            
            
            result_peg_985 = rule_valuelist(stream, result_peg_985.getPosition());
                if (result_peg_985.error()){
                    goto out_peg_1003;
                }
                data = result_peg_985.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    asSection(section)->addValue(asValue(data));
                    result_peg_985.setValue(value);
                }
            
            
        }
        stream.update(result_peg_985.getPosition());
        
        return result_peg_985;
        out_peg_1003:
        Result result_peg_1005(myposition);
        
        {
        
            result_peg_1005.reset();
                do{
                    Result result_peg_1008(result_peg_1005.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1008.getPosition()))){
                                result_peg_1008.nextPosition();
                            } else {
                                goto out_peg_1011;
                            }
                        }
                        result_peg_1008.setValue((void*) " ");
                            
                    }
                    goto success_peg_1009;
                    out_peg_1011:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1008.getPosition()))){
                                result_peg_1008.nextPosition();
                            } else {
                                goto out_peg_1013;
                            }
                        }
                        result_peg_1008.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1009;
                    out_peg_1013:
                    goto loop_peg_1007;
                    success_peg_1009:
                    ;
                    result_peg_1005.addResult(result_peg_1008);
                } while (true);
                loop_peg_1007:
                if (result_peg_1005.matches() == 0){
                    goto out_peg_1014;
                }
            
            
            
            Result result_peg_1016(result_peg_1005);
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_1016.getPosition()))){
                        result_peg_1016.nextPosition();
                    } else {
                        goto not_peg_1015;
                    }
                }
                result_peg_1016.setValue((void*) "[");
                goto out_peg_1014;
                not_peg_1015:
                result_peg_1005.setValue((void*)0);
            
            
        }
        stream.update(result_peg_1005.getPosition());
        
        return result_peg_1005;
        out_peg_1014:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_start(Stream & stream, const int position){
    
    RuleTrace trace_peg_1064(stream, "section_start");
    int myposition = position;
    
    
    Value data;
    Result result_peg_1019(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_1019.getPosition()))){
                        result_peg_1019.nextPosition();
                    } else {
                        goto out_peg_1021;
                    }
                }
                result_peg_1019.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_1019.reset();
                    do{
                        Result result_peg_1033(result_peg_1019.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1033.getPosition()))){
                                    result_peg_1033.nextPosition();
                                } else {
                                    goto out_peg_1036;
                                }
                            }
                            result_peg_1033.setValue((void*) " ");
                                
                        }
                        goto success_peg_1034;
                        out_peg_1036:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1033.getPosition()))){
                                    result_peg_1033.nextPosition();
                                } else {
                                    goto out_peg_1038;
                                }
                            }
                            result_peg_1033.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1034;
                        out_peg_1038:
                        goto loop_peg_1032;
                        success_peg_1034:
                        ;
                        result_peg_1019.addResult(result_peg_1033);
                    } while (true);
                    loop_peg_1032:
                    ;
                            
                }
                goto success_peg_1023;
                goto out_peg_1021;
                success_peg_1023:
                ;
            
            
            
            result_peg_1019.reset();
                do{
                    Result result_peg_1041(result_peg_1019.getPosition());
                    {
                    
                        Result result_peg_1044(result_peg_1041);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_1044.getPosition()))){
                                    result_peg_1044.nextPosition();
                                } else {
                                    goto not_peg_1043;
                                }
                            }
                            result_peg_1044.setValue((void*) "]");
                            goto loop_peg_1040;
                            not_peg_1043:
                            result_peg_1041.setValue((void*)0);
                        
                        
                        
                        char temp_peg_1045 = stream.get(result_peg_1041.getPosition());
                            if (temp_peg_1045 != '\0'){
                                result_peg_1041.setValue((void*) (long) temp_peg_1045);
                                result_peg_1041.nextPosition();
                            } else {
                                goto loop_peg_1040;
                            }
                        
                        
                    }
                    result_peg_1019.addResult(result_peg_1041);
                } while (true);
                loop_peg_1040:
                if (result_peg_1019.matches() == 0){
                    goto out_peg_1021;
                }
                data = result_peg_1019.getValues();
            
            
            
            {
                    
                    result_peg_1019.reset();
                    do{
                        Result result_peg_1057(result_peg_1019.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1057.getPosition()))){
                                    result_peg_1057.nextPosition();
                                } else {
                                    goto out_peg_1060;
                                }
                            }
                            result_peg_1057.setValue((void*) " ");
                                
                        }
                        goto success_peg_1058;
                        out_peg_1060:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1057.getPosition()))){
                                    result_peg_1057.nextPosition();
                                } else {
                                    goto out_peg_1062;
                                }
                            }
                            result_peg_1057.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1058;
                        out_peg_1062:
                        goto loop_peg_1056;
                        success_peg_1058:
                        ;
                        result_peg_1019.addResult(result_peg_1057);
                    } while (true);
                    loop_peg_1056:
                    ;
                            
                }
                goto success_peg_1047;
                goto out_peg_1021;
                success_peg_1047:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_1019.getPosition()))){
                        result_peg_1019.nextPosition();
                    } else {
                        goto out_peg_1021;
                    }
                }
                result_peg_1019.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = toString(data);
                    result_peg_1019.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1019.getPosition());
        
        return result_peg_1019;
        out_peg_1021:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_loopstart(Stream & stream, const int position){
    
    RuleTrace trace_peg_1069(stream, "loopstart");
    int myposition = position;
    
    
    
    Result result_peg_1066(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("loopstart"[i], stream.get(result_peg_1066.getPosition()))){
                        result_peg_1066.nextPosition();
                    } else {
                        goto out_peg_1068;
                    }
                }
                result_peg_1066.setValue((void*) "loopstart");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValue();
                    result_peg_1066.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1066.getPosition());
        
        return result_peg_1066;
        out_peg_1068:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    RuleTrace trace_peg_1107(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_1071(myposition);
        
        {
        
            {
                    
                    char letter_peg_1077 = stream.get(result_peg_1071.getPosition());
                    if (letter_peg_1077 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1077) != NULL){
                        result_peg_1071.nextPosition();
                        result_peg_1071.setValue((void*) (long) letter_peg_1077);
                    } else {
                        goto out_peg_1076;
                    }
                    
                }
                goto success_peg_1073;
                out_peg_1076:
                goto out_peg_1078;
                success_peg_1073:
                ;
            
            Result result_peg_1072 = result_peg_1071;
            
            result_peg_1071.reset();
                do{
                    Result result_peg_1081(result_peg_1071.getPosition());
                    {
                        
                        {
                            
                            char letter_peg_1094 = stream.get(result_peg_1081.getPosition());
                            if (letter_peg_1094 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1094) != NULL){
                                result_peg_1081.nextPosition();
                                result_peg_1081.setValue((void*) (long) letter_peg_1094);
                            } else {
                                goto out_peg_1093;
                            }
                            
                        }
                        goto success_peg_1090;
                        out_peg_1093:
                        goto out_peg_1089;
                        success_peg_1090:
                        ;
                        
                    }
                    goto success_peg_1082;
                    out_peg_1089:
                    {
                        
                        {
                            
                            char letter_peg_1106 = stream.get(result_peg_1081.getPosition());
                            if (letter_peg_1106 != '\0' && strchr("0123456789", letter_peg_1106) != NULL){
                                result_peg_1081.nextPosition();
                                result_peg_1081.setValue((void*) (long) letter_peg_1106);
                            } else {
                                goto out_peg_1105;
                            }
                            
                        }
                        goto success_peg_1102;
                        out_peg_1105:
                        goto out_peg_1101;
                        success_peg_1102:
                        ;
                        
                    }
                    goto success_peg_1082;
                    out_peg_1101:
                    goto loop_peg_1080;
                    success_peg_1082:
                    ;
                    result_peg_1071.addResult(result_peg_1081);
                } while (true);
                loop_peg_1080:
                ;
            
            Result result_peg_1079 = result_peg_1071;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_1072.getValues().getValue(),result_peg_1079.getValues());
                    result_peg_1071.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1071.getPosition());
        
        return result_peg_1071;
        out_peg_1078:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end_or_comment(Stream & stream, const int position){
    
    RuleTrace trace_peg_1154(stream, "line_end_or_comment");
    int myposition = position;
    
    
    
    Result result_peg_1109(myposition);
        
        result_peg_1109 = rule_line_end(stream, result_peg_1109.getPosition());
        if (result_peg_1109.error()){
            goto out_peg_1110;
        }
        stream.update(result_peg_1109.getPosition());
        
        return result_peg_1109;
        out_peg_1110:
        Result result_peg_1111(myposition);
        
        {
            {
            
                for (int i = 0; i < 1; i++){
                        if (compareChar(";"[i], stream.get(result_peg_1111.getPosition()))){
                            result_peg_1111.nextPosition();
                        } else {
                            goto out_peg_1115;
                        }
                    }
                    result_peg_1111.setValue((void*) ";");
                
                
                
                result_peg_1111.reset();
                    do{
                        Result result_peg_1124(result_peg_1111.getPosition());
                        {
                        
                            Result result_peg_1127(result_peg_1124);
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_1127.getPosition()))){
                                        result_peg_1127.nextPosition();
                                    } else {
                                        goto not_peg_1126;
                                    }
                                }
                                result_peg_1127.setValue((void*) "\n");
                                goto loop_peg_1123;
                                not_peg_1126:
                                result_peg_1124.setValue((void*)0);
                            
                            
                            
                            char temp_peg_1128 = stream.get(result_peg_1124.getPosition());
                                if (temp_peg_1128 != '\0'){
                                    result_peg_1124.setValue((void*) (long) temp_peg_1128);
                                    result_peg_1124.nextPosition();
                                } else {
                                    goto loop_peg_1123;
                                }
                            
                            
                        }
                        result_peg_1111.addResult(result_peg_1124);
                    } while (true);
                    loop_peg_1123:
                    ;
                
                
            }
        }
        goto success_peg_1112;
        out_peg_1115:
        {
            {
            
                for (int i = 0; i < 1; i++){
                        if (compareChar("="[i], stream.get(result_peg_1111.getPosition()))){
                            result_peg_1111.nextPosition();
                        } else {
                            goto out_peg_1131;
                        }
                    }
                    result_peg_1111.setValue((void*) "=");
                
                
                
                result_peg_1111.reset();
                    do{
                        Result result_peg_1140(result_peg_1111.getPosition());
                        {
                        
                            Result result_peg_1143(result_peg_1140);
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_1143.getPosition()))){
                                        result_peg_1143.nextPosition();
                                    } else {
                                        goto not_peg_1142;
                                    }
                                }
                                result_peg_1143.setValue((void*) "\n");
                                goto loop_peg_1139;
                                not_peg_1142:
                                result_peg_1140.setValue((void*)0);
                            
                            
                            
                            char temp_peg_1144 = stream.get(result_peg_1140.getPosition());
                                if (temp_peg_1144 != '\0'){
                                    result_peg_1140.setValue((void*) (long) temp_peg_1144);
                                    result_peg_1140.nextPosition();
                                } else {
                                    goto loop_peg_1139;
                                }
                            
                            
                        }
                        result_peg_1111.addResult(result_peg_1140);
                    } while (true);
                    loop_peg_1139:
                    ;
                
                
            }
        }
        goto success_peg_1112;
        out_peg_1131:
        {
            {
            
                for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_1111.getPosition()))){
                            result_peg_1111.nextPosition();
                        } else {
                            goto out_peg_1147;
                        }
                    }
                    result_peg_1111.setValue((void*) "-");
                
                
                
                result_peg_1111.reset();
                    do{
                        Result result_peg_1152(result_peg_1111.getPosition());
                        for (int i = 0; i < 1; i++){
                            if (compareChar("-"[i], stream.get(result_peg_1152.getPosition()))){
                                result_peg_1152.nextPosition();
                            } else {
                                goto loop_peg_1151;
                            }
                        }
                        result_peg_1152.setValue((void*) "-");
                        result_peg_1111.addResult(result_peg_1152);
                    } while (true);
                    loop_peg_1151:
                    if (result_peg_1111.matches() == 0){
                        goto out_peg_1147;
                    }
                
                
            }
        }
        goto success_peg_1112;
        out_peg_1147:
        goto out_peg_1153;
        success_peg_1112:
        ;
        stream.update(result_peg_1111.getPosition());
        
        return result_peg_1111;
        out_peg_1153:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_filename(Stream & stream, const int position){
    
    RuleTrace trace_peg_1164(stream, "filename");
    int myposition = position;
    
    
    Value file;
    Result result_peg_1156(myposition);
        
        {
        
            Result result_peg_1159(result_peg_1156);
                for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_1159.getPosition()))){
                        result_peg_1159.nextPosition();
                    } else {
                        goto not_peg_1158;
                    }
                }
                result_peg_1159.setValue((void*) "\"");
                goto out_peg_1160;
                not_peg_1158:
                result_peg_1156.setValue((void*)0);
            
            
            
            result_peg_1156.reset();
                do{
                    Result result_peg_1163(result_peg_1156.getPosition());
                    result_peg_1163 = rule_filename_char(stream, result_peg_1163.getPosition());
                    if (result_peg_1163.error()){
                        goto loop_peg_1162;
                    }
                    result_peg_1156.addResult(result_peg_1163);
                } while (true);
                loop_peg_1162:
                if (result_peg_1156.matches() == 0){
                    goto out_peg_1160;
                }
                file = result_peg_1156.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new Ast::Filename(toString(file)); GC::save(as<Ast::Filename*>(value));
                    result_peg_1156.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1156.getPosition());
        
        return result_peg_1156;
        out_peg_1160:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_filename_char(Stream & stream, const int position){
    
    RuleTrace trace_peg_1184(stream, "filename_char");
    int myposition = position;
    
    
    
    Result result_peg_1166(myposition);
        
        {
        
            Result result_peg_1169(result_peg_1166);
                for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1169.getPosition()))){
                        result_peg_1169.nextPosition();
                    } else {
                        goto not_peg_1168;
                    }
                }
                result_peg_1169.setValue((void*) ",");
                goto out_peg_1170;
                not_peg_1168:
                result_peg_1166.setValue((void*)0);
            
            
            
            Result result_peg_1173(result_peg_1166);
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_1173.getPosition()))){
                        result_peg_1173.nextPosition();
                    } else {
                        goto not_peg_1172;
                    }
                }
                result_peg_1173.setValue((void*) "\n");
                goto out_peg_1170;
                not_peg_1172:
                result_peg_1166.setValue((void*)0);
            
            
            
            Result result_peg_1176(result_peg_1166);
                for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_1176.getPosition()))){
                        result_peg_1176.nextPosition();
                    } else {
                        goto not_peg_1175;
                    }
                }
                result_peg_1176.setValue((void*) "[");
                goto out_peg_1170;
                not_peg_1175:
                result_peg_1166.setValue((void*)0);
            
            
            
            Result result_peg_1179(result_peg_1166);
                if ((unsigned char) stream.get(result_peg_1179.getPosition()) == (unsigned char) 13){
                    result_peg_1179.nextPosition();
                } else {
                    goto not_peg_1178;
                }
                result_peg_1179.setValue((void*) 13);
                goto out_peg_1170;
                not_peg_1178:
                result_peg_1166.setValue((void*)0);
            
            
            
            Result result_peg_1182(result_peg_1166);
                for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_1182.getPosition()))){
                        result_peg_1182.nextPosition();
                    } else {
                        goto not_peg_1181;
                    }
                }
                result_peg_1182.setValue((void*) ";");
                goto out_peg_1170;
                not_peg_1181:
                result_peg_1166.setValue((void*)0);
            
            
            
            char temp_peg_1183 = stream.get(result_peg_1166.getPosition());
                if (temp_peg_1183 != '\0'){
                    result_peg_1166.setValue((void*) (long) temp_peg_1183);
                    result_peg_1166.nextPosition();
                } else {
                    goto out_peg_1170;
                }
            
            
        }
        stream.update(result_peg_1166.getPosition());
        
        return result_peg_1166;
        out_peg_1170:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute(Stream & stream, const int position){
    
    RuleTrace trace_peg_1436(stream, "attribute");
    int myposition = position;
    
    
    Value id;
        Value data;
        Value index;
    Result result_peg_1186(myposition);
        
        {
        
            result_peg_1186 = rule_identifier(stream, result_peg_1186.getPosition());
                if (result_peg_1186.error()){
                    goto out_peg_1188;
                }
                id = result_peg_1186.getValues();
            
            
            
            {
                    
                    result_peg_1186.reset();
                    do{
                        Result result_peg_1200(result_peg_1186.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1200.getPosition()))){
                                    result_peg_1200.nextPosition();
                                } else {
                                    goto out_peg_1203;
                                }
                            }
                            result_peg_1200.setValue((void*) " ");
                                
                        }
                        goto success_peg_1201;
                        out_peg_1203:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1200.getPosition()))){
                                    result_peg_1200.nextPosition();
                                } else {
                                    goto out_peg_1205;
                                }
                            }
                            result_peg_1200.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1201;
                        out_peg_1205:
                        goto loop_peg_1199;
                        success_peg_1201:
                        ;
                        result_peg_1186.addResult(result_peg_1200);
                    } while (true);
                    loop_peg_1199:
                    ;
                            
                }
                goto success_peg_1190;
                goto out_peg_1188;
                success_peg_1190:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1186.getPosition()))){
                        result_peg_1186.nextPosition();
                    } else {
                        goto out_peg_1188;
                    }
                }
                result_peg_1186.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1186.reset();
                    do{
                        Result result_peg_1218(result_peg_1186.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1218.getPosition()))){
                                    result_peg_1218.nextPosition();
                                } else {
                                    goto out_peg_1221;
                                }
                            }
                            result_peg_1218.setValue((void*) " ");
                                
                        }
                        goto success_peg_1219;
                        out_peg_1221:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1218.getPosition()))){
                                    result_peg_1218.nextPosition();
                                } else {
                                    goto out_peg_1223;
                                }
                            }
                            result_peg_1218.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1219;
                        out_peg_1223:
                        goto loop_peg_1217;
                        success_peg_1219:
                        ;
                        result_peg_1186.addResult(result_peg_1218);
                    } while (true);
                    loop_peg_1217:
                    ;
                            
                }
                goto success_peg_1208;
                goto out_peg_1188;
                success_peg_1208:
                ;
            
            
            
            Result result_peg_1225(result_peg_1186.getPosition());
                result_peg_1225 = rule_line_end_or_comment(stream, result_peg_1225.getPosition());
                if (result_peg_1225.error()){
                    goto out_peg_1188;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id);
                    result_peg_1186.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1186.getPosition());
        
        return result_peg_1186;
        out_peg_1188:
        Result result_peg_1226(myposition);
        
        {
        
            result_peg_1226 = rule_identifier(stream, result_peg_1226.getPosition());
                if (result_peg_1226.error()){
                    goto out_peg_1228;
                }
                id = result_peg_1226.getValues();
            
            
            
            {
                    
                    result_peg_1226.reset();
                    do{
                        Result result_peg_1240(result_peg_1226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1240.getPosition()))){
                                    result_peg_1240.nextPosition();
                                } else {
                                    goto out_peg_1243;
                                }
                            }
                            result_peg_1240.setValue((void*) " ");
                                
                        }
                        goto success_peg_1241;
                        out_peg_1243:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1240.getPosition()))){
                                    result_peg_1240.nextPosition();
                                } else {
                                    goto out_peg_1245;
                                }
                            }
                            result_peg_1240.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1241;
                        out_peg_1245:
                        goto loop_peg_1239;
                        success_peg_1241:
                        ;
                        result_peg_1226.addResult(result_peg_1240);
                    } while (true);
                    loop_peg_1239:
                    ;
                            
                }
                goto success_peg_1230;
                goto out_peg_1228;
                success_peg_1230:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1226.getPosition()))){
                        result_peg_1226.nextPosition();
                    } else {
                        goto out_peg_1228;
                    }
                }
                result_peg_1226.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1226.reset();
                    do{
                        Result result_peg_1258(result_peg_1226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1258.getPosition()))){
                                    result_peg_1258.nextPosition();
                                } else {
                                    goto out_peg_1261;
                                }
                            }
                            result_peg_1258.setValue((void*) " ");
                                
                        }
                        goto success_peg_1259;
                        out_peg_1261:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1258.getPosition()))){
                                    result_peg_1258.nextPosition();
                                } else {
                                    goto out_peg_1263;
                                }
                            }
                            result_peg_1258.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1259;
                        out_peg_1263:
                        goto loop_peg_1257;
                        success_peg_1259:
                        ;
                        result_peg_1226.addResult(result_peg_1258);
                    } while (true);
                    loop_peg_1257:
                    ;
                            
                }
                goto success_peg_1248;
                goto out_peg_1228;
                success_peg_1248:
                ;
            
            
            
            result_peg_1226 = rule_valuelist(stream, result_peg_1226.getPosition());
                if (result_peg_1226.error()){
                    goto out_peg_1228;
                }
                data = result_peg_1226.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_1226.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1226.getPosition());
        
        return result_peg_1226;
        out_peg_1228:
        Result result_peg_1265(myposition);
        
        {
        
            result_peg_1265 = rule_identifier(stream, result_peg_1265.getPosition());
                if (result_peg_1265.error()){
                    goto out_peg_1267;
                }
                id = result_peg_1265.getValues();
            
            
            
            {
                    
                    result_peg_1265.reset();
                    do{
                        Result result_peg_1279(result_peg_1265.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1279.getPosition()))){
                                    result_peg_1279.nextPosition();
                                } else {
                                    goto out_peg_1282;
                                }
                            }
                            result_peg_1279.setValue((void*) " ");
                                
                        }
                        goto success_peg_1280;
                        out_peg_1282:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1279.getPosition()))){
                                    result_peg_1279.nextPosition();
                                } else {
                                    goto out_peg_1284;
                                }
                            }
                            result_peg_1279.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1280;
                        out_peg_1284:
                        goto loop_peg_1278;
                        success_peg_1280:
                        ;
                        result_peg_1265.addResult(result_peg_1279);
                    } while (true);
                    loop_peg_1278:
                    ;
                            
                }
                goto success_peg_1269;
                goto out_peg_1267;
                success_peg_1269:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1265.getPosition()))){
                        result_peg_1265.nextPosition();
                    } else {
                        goto out_peg_1267;
                    }
                }
                result_peg_1265.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1265.reset();
                    do{
                        Result result_peg_1297(result_peg_1265.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1297.getPosition()))){
                                    result_peg_1297.nextPosition();
                                } else {
                                    goto out_peg_1300;
                                }
                            }
                            result_peg_1297.setValue((void*) " ");
                                
                        }
                        goto success_peg_1298;
                        out_peg_1300:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1297.getPosition()))){
                                    result_peg_1297.nextPosition();
                                } else {
                                    goto out_peg_1302;
                                }
                            }
                            result_peg_1297.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1298;
                        out_peg_1302:
                        goto loop_peg_1296;
                        success_peg_1298:
                        ;
                        result_peg_1265.addResult(result_peg_1297);
                    } while (true);
                    loop_peg_1296:
                    ;
                            
                }
                goto success_peg_1287;
                goto out_peg_1267;
                success_peg_1287:
                ;
            
            
            
            result_peg_1265 = rule_filename(stream, result_peg_1265.getPosition());
                if (result_peg_1265.error()){
                    goto out_peg_1267;
                }
                data = result_peg_1265.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_1265.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1265.getPosition());
        
        return result_peg_1265;
        out_peg_1267:
        Result result_peg_1304(myposition);
        
        {
        
            result_peg_1304 = rule_identifier(stream, result_peg_1304.getPosition());
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
                id = result_peg_1304.getValues();
            
            
            
            {
                    
                    result_peg_1304.reset();
                    do{
                        Result result_peg_1318(result_peg_1304.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1318.getPosition()))){
                                    result_peg_1318.nextPosition();
                                } else {
                                    goto out_peg_1321;
                                }
                            }
                            result_peg_1318.setValue((void*) " ");
                                
                        }
                        goto success_peg_1319;
                        out_peg_1321:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1318.getPosition()))){
                                    result_peg_1318.nextPosition();
                                } else {
                                    goto out_peg_1323;
                                }
                            }
                            result_peg_1318.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1319;
                        out_peg_1323:
                        goto loop_peg_1317;
                        success_peg_1319:
                        ;
                        result_peg_1304.addResult(result_peg_1318);
                    } while (true);
                    loop_peg_1317:
                    ;
                            
                }
                goto success_peg_1308;
                goto out_peg_1306;
                success_peg_1308:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_1304.getPosition()))){
                        result_peg_1304.nextPosition();
                    } else {
                        goto out_peg_1306;
                    }
                }
                result_peg_1304.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_1304.reset();
                    do{
                        Result result_peg_1336(result_peg_1304.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1336.getPosition()))){
                                    result_peg_1336.nextPosition();
                                } else {
                                    goto out_peg_1339;
                                }
                            }
                            result_peg_1336.setValue((void*) " ");
                                
                        }
                        goto success_peg_1337;
                        out_peg_1339:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1336.getPosition()))){
                                    result_peg_1336.nextPosition();
                                } else {
                                    goto out_peg_1341;
                                }
                            }
                            result_peg_1336.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1337;
                        out_peg_1341:
                        goto loop_peg_1335;
                        success_peg_1337:
                        ;
                        result_peg_1304.addResult(result_peg_1336);
                    } while (true);
                    loop_peg_1335:
                    ;
                            
                }
                goto success_peg_1326;
                goto out_peg_1306;
                success_peg_1326:
                ;
            
            
            
            result_peg_1304 = rule_number(stream, result_peg_1304.getPosition());
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
                index = result_peg_1304.getValues();
            
            
            
            {
                    
                    result_peg_1304.reset();
                    do{
                        Result result_peg_1354(result_peg_1304.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1354.getPosition()))){
                                    result_peg_1354.nextPosition();
                                } else {
                                    goto out_peg_1357;
                                }
                            }
                            result_peg_1354.setValue((void*) " ");
                                
                        }
                        goto success_peg_1355;
                        out_peg_1357:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1354.getPosition()))){
                                    result_peg_1354.nextPosition();
                                } else {
                                    goto out_peg_1359;
                                }
                            }
                            result_peg_1354.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1355;
                        out_peg_1359:
                        goto loop_peg_1353;
                        success_peg_1355:
                        ;
                        result_peg_1304.addResult(result_peg_1354);
                    } while (true);
                    loop_peg_1353:
                    ;
                            
                }
                goto success_peg_1344;
                goto out_peg_1306;
                success_peg_1344:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_1304.getPosition()))){
                        result_peg_1304.nextPosition();
                    } else {
                        goto out_peg_1306;
                    }
                }
                result_peg_1304.setValue((void*) ")");
            
            
            
            {
                    
                    result_peg_1304.reset();
                    do{
                        Result result_peg_1372(result_peg_1304.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1372.getPosition()))){
                                    result_peg_1372.nextPosition();
                                } else {
                                    goto out_peg_1375;
                                }
                            }
                            result_peg_1372.setValue((void*) " ");
                                
                        }
                        goto success_peg_1373;
                        out_peg_1375:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1372.getPosition()))){
                                    result_peg_1372.nextPosition();
                                } else {
                                    goto out_peg_1377;
                                }
                            }
                            result_peg_1372.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1373;
                        out_peg_1377:
                        goto loop_peg_1371;
                        success_peg_1373:
                        ;
                        result_peg_1304.addResult(result_peg_1372);
                    } while (true);
                    loop_peg_1371:
                    ;
                            
                }
                goto success_peg_1362;
                goto out_peg_1306;
                success_peg_1362:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1304.getPosition()))){
                        result_peg_1304.nextPosition();
                    } else {
                        goto out_peg_1306;
                    }
                }
                result_peg_1304.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1304.reset();
                    do{
                        Result result_peg_1390(result_peg_1304.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1390.getPosition()))){
                                    result_peg_1390.nextPosition();
                                } else {
                                    goto out_peg_1393;
                                }
                            }
                            result_peg_1390.setValue((void*) " ");
                                
                        }
                        goto success_peg_1391;
                        out_peg_1393:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1390.getPosition()))){
                                    result_peg_1390.nextPosition();
                                } else {
                                    goto out_peg_1395;
                                }
                            }
                            result_peg_1390.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1391;
                        out_peg_1395:
                        goto loop_peg_1389;
                        success_peg_1391:
                        ;
                        result_peg_1304.addResult(result_peg_1390);
                    } while (true);
                    loop_peg_1389:
                    ;
                            
                }
                goto success_peg_1380;
                goto out_peg_1306;
                success_peg_1380:
                ;
            
            
            
            result_peg_1304 = rule_valuelist(stream, result_peg_1304.getPosition());
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
                data = result_peg_1304.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(id, index, data);
                    result_peg_1304.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1304.getPosition());
        
        return result_peg_1304;
        out_peg_1306:
        Result result_peg_1397(myposition);
        
        {
        
            result_peg_1397 = rule_identifier_list(stream, result_peg_1397.getPosition());
                if (result_peg_1397.error()){
                    goto out_peg_1399;
                }
                id = result_peg_1397.getValues();
            
            
            
            {
                    
                    result_peg_1397.reset();
                    do{
                        Result result_peg_1411(result_peg_1397.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1411.getPosition()))){
                                    result_peg_1411.nextPosition();
                                } else {
                                    goto out_peg_1414;
                                }
                            }
                            result_peg_1411.setValue((void*) " ");
                                
                        }
                        goto success_peg_1412;
                        out_peg_1414:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1411.getPosition()))){
                                    result_peg_1411.nextPosition();
                                } else {
                                    goto out_peg_1416;
                                }
                            }
                            result_peg_1411.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1412;
                        out_peg_1416:
                        goto loop_peg_1410;
                        success_peg_1412:
                        ;
                        result_peg_1397.addResult(result_peg_1411);
                    } while (true);
                    loop_peg_1410:
                    ;
                            
                }
                goto success_peg_1401;
                goto out_peg_1399;
                success_peg_1401:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1397.getPosition()))){
                        result_peg_1397.nextPosition();
                    } else {
                        goto out_peg_1399;
                    }
                }
                result_peg_1397.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1397.reset();
                    do{
                        Result result_peg_1429(result_peg_1397.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1429.getPosition()))){
                                    result_peg_1429.nextPosition();
                                } else {
                                    goto out_peg_1432;
                                }
                            }
                            result_peg_1429.setValue((void*) " ");
                                
                        }
                        goto success_peg_1430;
                        out_peg_1432:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1429.getPosition()))){
                                    result_peg_1429.nextPosition();
                                } else {
                                    goto out_peg_1434;
                                }
                            }
                            result_peg_1429.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1430;
                        out_peg_1434:
                        goto loop_peg_1428;
                        success_peg_1430:
                        ;
                        result_peg_1397.addResult(result_peg_1429);
                    } while (true);
                    loop_peg_1428:
                    ;
                            
                }
                goto success_peg_1419;
                goto out_peg_1399;
                success_peg_1419:
                ;
            
            
            
            result_peg_1397 = rule_valuelist(stream, result_peg_1397.getPosition());
                if (result_peg_1397.error()){
                    goto out_peg_1399;
                }
                data = result_peg_1397.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttributes(id, data);
                    result_peg_1397.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1397.getPosition());
        
        return result_peg_1397;
        out_peg_1399:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_attribute_simple(Stream & stream, const int position){
    
    RuleTrace trace_peg_1477(stream, "attribute_simple");
    int myposition = position;
    
    
    Value id;
        Value data;
    Result result_peg_1438(myposition);
        
        {
        
            result_peg_1438 = rule_identifier(stream, result_peg_1438.getPosition());
                if (result_peg_1438.error()){
                    goto out_peg_1440;
                }
                id = result_peg_1438.getValues();
            
            
            
            {
                    
                    result_peg_1438.reset();
                    do{
                        Result result_peg_1452(result_peg_1438.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1452.getPosition()))){
                                    result_peg_1452.nextPosition();
                                } else {
                                    goto out_peg_1455;
                                }
                            }
                            result_peg_1452.setValue((void*) " ");
                                
                        }
                        goto success_peg_1453;
                        out_peg_1455:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1452.getPosition()))){
                                    result_peg_1452.nextPosition();
                                } else {
                                    goto out_peg_1457;
                                }
                            }
                            result_peg_1452.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1453;
                        out_peg_1457:
                        goto loop_peg_1451;
                        success_peg_1453:
                        ;
                        result_peg_1438.addResult(result_peg_1452);
                    } while (true);
                    loop_peg_1451:
                    ;
                            
                }
                goto success_peg_1442;
                goto out_peg_1440;
                success_peg_1442:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1438.getPosition()))){
                        result_peg_1438.nextPosition();
                    } else {
                        goto out_peg_1440;
                    }
                }
                result_peg_1438.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1438.reset();
                    do{
                        Result result_peg_1470(result_peg_1438.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1470.getPosition()))){
                                    result_peg_1470.nextPosition();
                                } else {
                                    goto out_peg_1473;
                                }
                            }
                            result_peg_1470.setValue((void*) " ");
                                
                        }
                        goto success_peg_1471;
                        out_peg_1473:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1470.getPosition()))){
                                    result_peg_1470.nextPosition();
                                } else {
                                    goto out_peg_1475;
                                }
                            }
                            result_peg_1470.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1471;
                        out_peg_1475:
                        goto loop_peg_1469;
                        success_peg_1471:
                        ;
                        result_peg_1438.addResult(result_peg_1470);
                    } while (true);
                    loop_peg_1469:
                    ;
                            
                }
                goto success_peg_1460;
                goto out_peg_1440;
                success_peg_1460:
                ;
            
            
            
            result_peg_1438 = rule_value(stream, result_peg_1438.getPosition());
                if (result_peg_1438.error()){
                    goto out_peg_1440;
                }
                data = result_peg_1438.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(id, data);
                    result_peg_1438.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1438.getPosition());
        
        return result_peg_1438;
        out_peg_1440:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    RuleTrace trace_peg_1486(stream, "identifier");
    int myposition = position;
    
    
    
    Result result_peg_1479(myposition);
        
        {
        
            result_peg_1479 = rule_name(stream, result_peg_1479.getPosition());
                if (result_peg_1479.error()){
                    goto out_peg_1481;
                }
            
            Result result_peg_1480 = result_peg_1479;
            
            result_peg_1479.reset();
                do{
                    Result result_peg_1484(result_peg_1479.getPosition());
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_1484.getPosition()))){
                                    result_peg_1484.nextPosition();
                                } else {
                                    goto loop_peg_1483;
                                }
                            }
                            result_peg_1484.setValue((void*) ".");
                        
                        
                        
                        result_peg_1484 = rule_name(stream, result_peg_1484.getPosition());
                            if (result_peg_1484.error()){
                                goto loop_peg_1483;
                            }
                        
                        
                    }
                    result_peg_1479.addResult(result_peg_1484);
                } while (true);
                loop_peg_1483:
                ;
            
            Result result_peg_1482 = result_peg_1479;
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(result_peg_1480.getValues(),result_peg_1482.getValues());
                    result_peg_1479.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1479.getPosition());
        
        return result_peg_1479;
        out_peg_1481:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_identifier_list(Stream & stream, const int position){
    
    RuleTrace trace_peg_1528(stream, "identifier_list");
    int myposition = position;
    
    
    
    Result result_peg_1488(myposition);
        
        {
        
            result_peg_1488 = rule_identifier(stream, result_peg_1488.getPosition());
                if (result_peg_1488.error()){
                    goto out_peg_1490;
                }
            
            
            
            result_peg_1488.reset();
                do{
                    Result result_peg_1492(result_peg_1488.getPosition());
                    {
                    
                        {
                                
                                result_peg_1492.reset();
                                do{
                                    Result result_peg_1504(result_peg_1492.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1504.getPosition()))){
                                                result_peg_1504.nextPosition();
                                            } else {
                                                goto out_peg_1507;
                                            }
                                        }
                                        result_peg_1504.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1505;
                                    out_peg_1507:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1504.getPosition()))){
                                                result_peg_1504.nextPosition();
                                            } else {
                                                goto out_peg_1509;
                                            }
                                        }
                                        result_peg_1504.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1505;
                                    out_peg_1509:
                                    goto loop_peg_1503;
                                    success_peg_1505:
                                    ;
                                    result_peg_1492.addResult(result_peg_1504);
                                } while (true);
                                loop_peg_1503:
                                ;
                                        
                            }
                            goto success_peg_1494;
                            goto loop_peg_1491;
                            success_peg_1494:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1492.getPosition()))){
                                    result_peg_1492.nextPosition();
                                } else {
                                    goto loop_peg_1491;
                                }
                            }
                            result_peg_1492.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1492.reset();
                                do{
                                    Result result_peg_1522(result_peg_1492.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1522.getPosition()))){
                                                result_peg_1522.nextPosition();
                                            } else {
                                                goto out_peg_1525;
                                            }
                                        }
                                        result_peg_1522.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1523;
                                    out_peg_1525:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1522.getPosition()))){
                                                result_peg_1522.nextPosition();
                                            } else {
                                                goto out_peg_1527;
                                            }
                                        }
                                        result_peg_1522.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1523;
                                    out_peg_1527:
                                    goto loop_peg_1521;
                                    success_peg_1523:
                                    ;
                                    result_peg_1492.addResult(result_peg_1522);
                                } while (true);
                                loop_peg_1521:
                                ;
                                        
                            }
                            goto success_peg_1512;
                            goto loop_peg_1491;
                            success_peg_1512:
                            ;
                        
                        
                        
                        result_peg_1492 = rule_filename(stream, result_peg_1492.getPosition());
                            if (result_peg_1492.error()){
                                goto loop_peg_1491;
                            }
                        
                        
                    }
                    result_peg_1488.addResult(result_peg_1492);
                } while (true);
                loop_peg_1491:
                if (result_peg_1488.matches() == 0){
                    goto out_peg_1490;
                }
            
            
        }
        stream.update(result_peg_1488.getPosition());
        
        return result_peg_1488;
        out_peg_1490:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    RuleTrace trace_peg_1572(stream, "valuelist");
    int myposition = position;
    
    
    
    Result result_peg_1530(myposition);
        
        {
        
            result_peg_1530 = rule_value(stream, result_peg_1530.getPosition());
                if (result_peg_1530.error()){
                    goto out_peg_1532;
                }
            
            Result result_peg_1531 = result_peg_1530;
            
            result_peg_1530.reset();
                do{
                    Result result_peg_1535(result_peg_1530.getPosition());
                    {
                    
                        {
                                
                                result_peg_1535.reset();
                                do{
                                    Result result_peg_1547(result_peg_1535.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1547.getPosition()))){
                                                result_peg_1547.nextPosition();
                                            } else {
                                                goto out_peg_1550;
                                            }
                                        }
                                        result_peg_1547.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1548;
                                    out_peg_1550:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1547.getPosition()))){
                                                result_peg_1547.nextPosition();
                                            } else {
                                                goto out_peg_1552;
                                            }
                                        }
                                        result_peg_1547.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1548;
                                    out_peg_1552:
                                    goto loop_peg_1546;
                                    success_peg_1548:
                                    ;
                                    result_peg_1535.addResult(result_peg_1547);
                                } while (true);
                                loop_peg_1546:
                                ;
                                        
                            }
                            goto success_peg_1537;
                            goto loop_peg_1534;
                            success_peg_1537:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1535.getPosition()))){
                                    result_peg_1535.nextPosition();
                                } else {
                                    goto loop_peg_1534;
                                }
                            }
                            result_peg_1535.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1535.reset();
                                do{
                                    Result result_peg_1565(result_peg_1535.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1565.getPosition()))){
                                                result_peg_1565.nextPosition();
                                            } else {
                                                goto out_peg_1568;
                                            }
                                        }
                                        result_peg_1565.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1566;
                                    out_peg_1568:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1565.getPosition()))){
                                                result_peg_1565.nextPosition();
                                            } else {
                                                goto out_peg_1570;
                                            }
                                        }
                                        result_peg_1565.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1566;
                                    out_peg_1570:
                                    goto loop_peg_1564;
                                    success_peg_1566:
                                    ;
                                    result_peg_1535.addResult(result_peg_1565);
                                } while (true);
                                loop_peg_1564:
                                ;
                                        
                            }
                            goto success_peg_1555;
                            goto loop_peg_1534;
                            success_peg_1555:
                            ;
                        
                        
                        
                        int save_peg_1571 = result_peg_1535.getPosition();
                            
                            result_peg_1535 = rule_value(stream, result_peg_1535.getPosition());
                            if (result_peg_1535.error()){
                                
                                result_peg_1535 = Result(save_peg_1571);
                                result_peg_1535.setValue((void*) 0);
                                
                            }
                        
                        
                    }
                    result_peg_1530.addResult(result_peg_1535);
                } while (true);
                loop_peg_1534:
                ;
            
            Result result_peg_1533 = result_peg_1530;
            
            {
                    Value value((void*) 0);
                    value = makeValueList(result_peg_1531.getValues(),result_peg_1533.getValues());
                    result_peg_1530.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1530.getPosition());
        
        return result_peg_1530;
        out_peg_1532:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    RuleTrace trace_peg_1625(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_1574(myposition);
        
        result_peg_1574 = rule_string(stream, result_peg_1574.getPosition());
        if (result_peg_1574.error()){
            goto out_peg_1575;
        }
        stream.update(result_peg_1574.getPosition());
        
        return result_peg_1574;
        out_peg_1575:
        Result result_peg_1576(myposition);
        
        result_peg_1576 = rule_date(stream, result_peg_1576.getPosition());
        if (result_peg_1576.error()){
            goto out_peg_1577;
        }
        stream.update(result_peg_1576.getPosition());
        
        return result_peg_1576;
        out_peg_1577:
        Result result_peg_1578(myposition);
        
        {
        
            result_peg_1578 = rule_number(stream, result_peg_1578.getPosition());
                if (result_peg_1578.error()){
                    goto out_peg_1580;
                }
            
            Result result_peg_1579 = result_peg_1578;
            
            Result result_peg_1583(result_peg_1578);
                {
                    
                    char letter_peg_1588 = stream.get(result_peg_1583.getPosition());
                    if (letter_peg_1588 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1588) != NULL){
                        result_peg_1583.nextPosition();
                        result_peg_1583.setValue((void*) (long) letter_peg_1588);
                    } else {
                        goto out_peg_1587;
                    }
                    
                }
                goto success_peg_1584;
                out_peg_1587:
                goto not_peg_1582;
                success_peg_1584:
                ;
                goto out_peg_1580;
                not_peg_1582:
                result_peg_1578.setValue((void*)0);
            
            
            
            Result result_peg_1591(result_peg_1578);
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1591.getPosition()))){
                        result_peg_1591.nextPosition();
                    } else {
                        goto not_peg_1590;
                    }
                }
                result_peg_1591.setValue((void*) "-");
                goto out_peg_1580;
                not_peg_1590:
                result_peg_1578.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_1579.getValues();
                    result_peg_1578.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1578.getPosition());
        
        return result_peg_1578;
        out_peg_1580:
        Result result_peg_1592(myposition);
        
        result_peg_1592 = rule_attribute_simple(stream, result_peg_1592.getPosition());
        if (result_peg_1592.error()){
            goto out_peg_1593;
        }
        stream.update(result_peg_1592.getPosition());
        
        return result_peg_1592;
        out_peg_1593:
        Result result_peg_1594(myposition);
        
        {
        
            result_peg_1594 = rule_keyword(stream, result_peg_1594.getPosition());
                if (result_peg_1594.error()){
                    goto out_peg_1596;
                }
            
            Result result_peg_1595 = result_peg_1594;
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1595.getValues());
                    result_peg_1594.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1594.getPosition());
        
        return result_peg_1594;
        out_peg_1596:
        Result result_peg_1597(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_1597.getPosition()))){
                        result_peg_1597.nextPosition();
                    } else {
                        goto out_peg_1599;
                    }
                }
                result_peg_1597.setValue((void*) "s");
            
            Result result_peg_1598 = result_peg_1597;
            
            Result result_peg_1602(result_peg_1597);
                result_peg_1602 = rule_filename_char(stream, result_peg_1602.getPosition());
                if (result_peg_1602.error()){
                    goto not_peg_1601;
                }
                goto out_peg_1599;
                not_peg_1601:
                result_peg_1597.setValue((void*)0);
            
            
            
            Result result_peg_1605(result_peg_1597);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_1605.getPosition()))){
                        result_peg_1605.nextPosition();
                    } else {
                        goto not_peg_1604;
                    }
                }
                result_peg_1605.setValue((void*) ".");
                goto out_peg_1599;
                not_peg_1604:
                result_peg_1597.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1598.getValues());
                    result_peg_1597.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1597.getPosition());
        
        return result_peg_1597;
        out_peg_1599:
        Result result_peg_1606(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("h"[i], stream.get(result_peg_1606.getPosition()))){
                        result_peg_1606.nextPosition();
                    } else {
                        goto out_peg_1608;
                    }
                }
                result_peg_1606.setValue((void*) "h");
            
            Result result_peg_1607 = result_peg_1606;
            
            Result result_peg_1611(result_peg_1606);
                result_peg_1611 = rule_filename_char(stream, result_peg_1611.getPosition());
                if (result_peg_1611.error()){
                    goto not_peg_1610;
                }
                goto out_peg_1608;
                not_peg_1610:
                result_peg_1606.setValue((void*)0);
            
            
            
            Result result_peg_1614(result_peg_1606);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_1614.getPosition()))){
                        result_peg_1614.nextPosition();
                    } else {
                        goto not_peg_1613;
                    }
                }
                result_peg_1614.setValue((void*) ".");
                goto out_peg_1608;
                not_peg_1613:
                result_peg_1606.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1607.getValues());
                    result_peg_1606.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1606.getPosition());
        
        return result_peg_1606;
        out_peg_1608:
        Result result_peg_1615(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("a"[i], stream.get(result_peg_1615.getPosition()))){
                        result_peg_1615.nextPosition();
                    } else {
                        goto out_peg_1617;
                    }
                }
                result_peg_1615.setValue((void*) "a");
            
            Result result_peg_1616 = result_peg_1615;
            
            int save_peg_1619 = result_peg_1615.getPosition();
                
                result_peg_1615 = rule_number(stream, result_peg_1615.getPosition());
                if (result_peg_1615.error()){
                    
                    result_peg_1615 = Result(save_peg_1619);
                    result_peg_1615.setValue((void*) 0);
                    
                }
            
            
            
            Result result_peg_1622(result_peg_1615);
                result_peg_1622 = rule_filename_char(stream, result_peg_1622.getPosition());
                if (result_peg_1622.error()){
                    goto not_peg_1621;
                }
                goto out_peg_1617;
                not_peg_1621:
                result_peg_1615.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword(result_peg_1616.getValues());
                    result_peg_1615.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1615.getPosition());
        
        return result_peg_1615;
        out_peg_1617:
        Result result_peg_1623(myposition);
        
        result_peg_1623 = rule_filename(stream, result_peg_1623.getPosition());
        if (result_peg_1623.error()){
            goto out_peg_1624;
        }
        stream.update(result_peg_1623.getPosition());
        
        return result_peg_1623;
        out_peg_1624:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    RuleTrace trace_peg_1639(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_1627(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareChar("normal"[i], stream.get(result_peg_1627.getPosition()))){
                result_peg_1627.nextPosition();
            } else {
                goto out_peg_1628;
            }
        }
        result_peg_1627.setValue((void*) "normal");
        stream.update(result_peg_1627.getPosition());
        
        return result_peg_1627;
        out_peg_1628:
        Result result_peg_1629(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareChar("parallax"[i], stream.get(result_peg_1629.getPosition()))){
                result_peg_1629.nextPosition();
            } else {
                goto out_peg_1630;
            }
        }
        result_peg_1629.setValue((void*) "parallax");
        stream.update(result_peg_1629.getPosition());
        
        return result_peg_1629;
        out_peg_1630:
        Result result_peg_1631(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareChar("addalpha"[i], stream.get(result_peg_1631.getPosition()))){
                result_peg_1631.nextPosition();
            } else {
                goto out_peg_1632;
            }
        }
        result_peg_1631.setValue((void*) "addalpha");
        stream.update(result_peg_1631.getPosition());
        
        return result_peg_1631;
        out_peg_1632:
        Result result_peg_1633(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareChar("add1"[i], stream.get(result_peg_1633.getPosition()))){
                result_peg_1633.nextPosition();
            } else {
                goto out_peg_1634;
            }
        }
        result_peg_1633.setValue((void*) "add1");
        stream.update(result_peg_1633.getPosition());
        
        return result_peg_1633;
        out_peg_1634:
        Result result_peg_1635(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareChar("add"[i], stream.get(result_peg_1635.getPosition()))){
                result_peg_1635.nextPosition();
            } else {
                goto out_peg_1636;
            }
        }
        result_peg_1635.setValue((void*) "add");
        stream.update(result_peg_1635.getPosition());
        
        return result_peg_1635;
        out_peg_1636:
        Result result_peg_1637(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareChar("sub"[i], stream.get(result_peg_1637.getPosition()))){
                result_peg_1637.nextPosition();
            } else {
                goto out_peg_1638;
            }
        }
        result_peg_1637.setValue((void*) "sub");
        stream.update(result_peg_1637.getPosition());
        
        return result_peg_1637;
        out_peg_1638:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_date(Stream & stream, const int position){
    
    RuleTrace trace_peg_1697(stream, "date");
    int myposition = position;
    
    
    
    Result result_peg_1641(myposition);
        
        {
        
            result_peg_1641.reset();
                do{
                    Result result_peg_1644(result_peg_1641.getPosition());
                    {
                        
                        char letter_peg_1649 = stream.get(result_peg_1644.getPosition());
                        if (letter_peg_1649 != '\0' && strchr("0123456789", letter_peg_1649) != NULL){
                            result_peg_1644.nextPosition();
                            result_peg_1644.setValue((void*) (long) letter_peg_1649);
                        } else {
                            goto out_peg_1648;
                        }
                        
                    }
                    goto success_peg_1645;
                    out_peg_1648:
                    goto loop_peg_1643;
                    success_peg_1645:
                    ;
                    result_peg_1641.addResult(result_peg_1644);
                } while (true);
                loop_peg_1643:
                if (result_peg_1641.matches() == 0){
                    goto out_peg_1650;
                }
            
            Result result_peg_1642 = result_peg_1641;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_1641.getPosition()))){
                        result_peg_1641.nextPosition();
                    } else {
                        goto out_peg_1650;
                    }
                }
                result_peg_1641.setValue((void*) ".");
            
            
            
            result_peg_1641.reset();
                do{
                    Result result_peg_1654(result_peg_1641.getPosition());
                    {
                        
                        char letter_peg_1659 = stream.get(result_peg_1654.getPosition());
                        if (letter_peg_1659 != '\0' && strchr("0123456789", letter_peg_1659) != NULL){
                            result_peg_1654.nextPosition();
                            result_peg_1654.setValue((void*) (long) letter_peg_1659);
                        } else {
                            goto out_peg_1658;
                        }
                        
                    }
                    goto success_peg_1655;
                    out_peg_1658:
                    goto loop_peg_1653;
                    success_peg_1655:
                    ;
                    result_peg_1641.addResult(result_peg_1654);
                } while (true);
                loop_peg_1653:
                if (result_peg_1641.matches() == 0){
                    goto out_peg_1650;
                }
            
            Result result_peg_1652 = result_peg_1641;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_1641.getPosition()))){
                        result_peg_1641.nextPosition();
                    } else {
                        goto out_peg_1650;
                    }
                }
                result_peg_1641.setValue((void*) ".");
            
            
            
            result_peg_1641.reset();
                do{
                    Result result_peg_1663(result_peg_1641.getPosition());
                    {
                        
                        char letter_peg_1668 = stream.get(result_peg_1663.getPosition());
                        if (letter_peg_1668 != '\0' && strchr("0123456789", letter_peg_1668) != NULL){
                            result_peg_1663.nextPosition();
                            result_peg_1663.setValue((void*) (long) letter_peg_1668);
                        } else {
                            goto out_peg_1667;
                        }
                        
                    }
                    goto success_peg_1664;
                    out_peg_1667:
                    goto loop_peg_1662;
                    success_peg_1664:
                    ;
                    result_peg_1641.addResult(result_peg_1663);
                } while (true);
                loop_peg_1662:
                if (result_peg_1641.matches() == 0){
                    goto out_peg_1650;
                }
            
            Result result_peg_1661 = result_peg_1641;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_1642.getValues(),result_peg_1652.getValues(),result_peg_1661.getValues());
                    result_peg_1641.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1641.getPosition());
        
        return result_peg_1641;
        out_peg_1650:
        Result result_peg_1669(myposition);
        
        {
        
            result_peg_1669.reset();
                do{
                    Result result_peg_1672(result_peg_1669.getPosition());
                    {
                        
                        char letter_peg_1677 = stream.get(result_peg_1672.getPosition());
                        if (letter_peg_1677 != '\0' && strchr("0123456789", letter_peg_1677) != NULL){
                            result_peg_1672.nextPosition();
                            result_peg_1672.setValue((void*) (long) letter_peg_1677);
                        } else {
                            goto out_peg_1676;
                        }
                        
                    }
                    goto success_peg_1673;
                    out_peg_1676:
                    goto loop_peg_1671;
                    success_peg_1673:
                    ;
                    result_peg_1669.addResult(result_peg_1672);
                } while (true);
                loop_peg_1671:
                if (result_peg_1669.matches() == 0){
                    goto out_peg_1678;
                }
            
            Result result_peg_1670 = result_peg_1669;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_1669.getPosition()))){
                        result_peg_1669.nextPosition();
                    } else {
                        goto out_peg_1678;
                    }
                }
                result_peg_1669.setValue((void*) "/");
            
            
            
            result_peg_1669.reset();
                do{
                    Result result_peg_1682(result_peg_1669.getPosition());
                    {
                        
                        char letter_peg_1687 = stream.get(result_peg_1682.getPosition());
                        if (letter_peg_1687 != '\0' && strchr("0123456789", letter_peg_1687) != NULL){
                            result_peg_1682.nextPosition();
                            result_peg_1682.setValue((void*) (long) letter_peg_1687);
                        } else {
                            goto out_peg_1686;
                        }
                        
                    }
                    goto success_peg_1683;
                    out_peg_1686:
                    goto loop_peg_1681;
                    success_peg_1683:
                    ;
                    result_peg_1669.addResult(result_peg_1682);
                } while (true);
                loop_peg_1681:
                if (result_peg_1669.matches() == 0){
                    goto out_peg_1678;
                }
            
            Result result_peg_1680 = result_peg_1669;
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_1669.getPosition()))){
                        result_peg_1669.nextPosition();
                    } else {
                        goto out_peg_1678;
                    }
                }
                result_peg_1669.setValue((void*) "/");
            
            
            
            result_peg_1669.reset();
                do{
                    Result result_peg_1691(result_peg_1669.getPosition());
                    {
                        
                        char letter_peg_1696 = stream.get(result_peg_1691.getPosition());
                        if (letter_peg_1696 != '\0' && strchr("0123456789", letter_peg_1696) != NULL){
                            result_peg_1691.nextPosition();
                            result_peg_1691.setValue((void*) (long) letter_peg_1696);
                        } else {
                            goto out_peg_1695;
                        }
                        
                    }
                    goto success_peg_1692;
                    out_peg_1695:
                    goto loop_peg_1690;
                    success_peg_1692:
                    ;
                    result_peg_1669.addResult(result_peg_1691);
                } while (true);
                loop_peg_1690:
                if (result_peg_1669.matches() == 0){
                    goto out_peg_1678;
                }
            
            Result result_peg_1689 = result_peg_1669;
            
            {
                    Value value((void*) 0);
                    value = makeDate(result_peg_1670.getValues(),result_peg_1680.getValues(),result_peg_1689.getValues());
                    result_peg_1669.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1669.getPosition());
        
        return result_peg_1669;
        out_peg_1678:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    RuleTrace trace_peg_1713(stream, "string");
    int myposition = position;
    
    
    Value data;
    Result result_peg_1699(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_1699.getPosition()))){
                        result_peg_1699.nextPosition();
                    } else {
                        goto out_peg_1701;
                    }
                }
                result_peg_1699.setValue((void*) "\"");
            
            
            
            result_peg_1699.reset();
                do{
                    Result result_peg_1704(result_peg_1699.getPosition());
                    {
                    
                        Result result_peg_1707(result_peg_1704);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\""[i], stream.get(result_peg_1707.getPosition()))){
                                    result_peg_1707.nextPosition();
                                } else {
                                    goto not_peg_1706;
                                }
                            }
                            result_peg_1707.setValue((void*) "\"");
                            goto loop_peg_1703;
                            not_peg_1706:
                            result_peg_1704.setValue((void*)0);
                        
                        
                        
                        Result result_peg_1710(result_peg_1704);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_1710.getPosition()))){
                                    result_peg_1710.nextPosition();
                                } else {
                                    goto not_peg_1709;
                                }
                            }
                            result_peg_1710.setValue((void*) "\n");
                            goto loop_peg_1703;
                            not_peg_1709:
                            result_peg_1704.setValue((void*)0);
                        
                        
                        
                        char temp_peg_1711 = stream.get(result_peg_1704.getPosition());
                            if (temp_peg_1711 != '\0'){
                                result_peg_1704.setValue((void*) (long) temp_peg_1711);
                                result_peg_1704.nextPosition();
                            } else {
                                goto loop_peg_1703;
                            }
                        
                        
                    }
                    result_peg_1699.addResult(result_peg_1704);
                } while (true);
                loop_peg_1703:
                ;
                data = result_peg_1699.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_1699.getPosition()))){
                        result_peg_1699.nextPosition();
                    } else {
                        goto out_peg_1701;
                    }
                }
                result_peg_1699.setValue((void*) "\"");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeString(data);
                    result_peg_1699.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1699.getPosition());
        
        return result_peg_1699;
        out_peg_1701:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_number(Stream & stream, const int position){
    
    RuleTrace trace_peg_1725(stream, "number");
    int myposition = position;
    
    
    
    Result result_peg_1715(myposition);
        
        {
        
            int save_peg_1717 = result_peg_1715.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_1715.getPosition()))){
                            result_peg_1715.nextPosition();
                        } else {
                            goto out_peg_1720;
                        }
                    }
                    result_peg_1715.setValue((void*) "+");
                        
                }
                goto success_peg_1718;
                out_peg_1720:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_1715.getPosition()))){
                            result_peg_1715.nextPosition();
                        } else {
                            goto out_peg_1722;
                        }
                    }
                    result_peg_1715.setValue((void*) "-");
                        
                }
                goto success_peg_1718;
                out_peg_1722:
                
                result_peg_1715 = Result(save_peg_1717);
                result_peg_1715.setValue((void*) 0);
                
                success_peg_1718:
                ;
            
            Result result_peg_1716 = result_peg_1715;
            
            result_peg_1715 = rule_float_or_integer(stream, result_peg_1715.getPosition());
                if (result_peg_1715.error()){
                    goto out_peg_1724;
                }
            
            Result result_peg_1723 = result_peg_1715;
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_1716.getValues(),result_peg_1723.getValues());
                    result_peg_1715.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1715.getPosition());
        
        return result_peg_1715;
        out_peg_1724:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_float_or_integer(Stream & stream, const int position){
    
    RuleTrace trace_peg_1759(stream, "float_or_integer");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_1727(myposition);
        
        {
        
            result_peg_1727.reset();
                do{
                    Result result_peg_1730(result_peg_1727.getPosition());
                    {
                        
                        char letter_peg_1735 = stream.get(result_peg_1730.getPosition());
                        if (letter_peg_1735 != '\0' && strchr("0123456789", letter_peg_1735) != NULL){
                            result_peg_1730.nextPosition();
                            result_peg_1730.setValue((void*) (long) letter_peg_1735);
                        } else {
                            goto out_peg_1734;
                        }
                        
                    }
                    goto success_peg_1731;
                    out_peg_1734:
                    goto loop_peg_1729;
                    success_peg_1731:
                    ;
                    result_peg_1727.addResult(result_peg_1730);
                } while (true);
                loop_peg_1729:
                ;
                left = result_peg_1727.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_1727.getPosition()))){
                        result_peg_1727.nextPosition();
                    } else {
                        goto out_peg_1737;
                    }
                }
                result_peg_1727.setValue((void*) ".");
            
            
            
            result_peg_1727.reset();
                do{
                    Result result_peg_1740(result_peg_1727.getPosition());
                    {
                        
                        char letter_peg_1745 = stream.get(result_peg_1740.getPosition());
                        if (letter_peg_1745 != '\0' && strchr("0123456789", letter_peg_1745) != NULL){
                            result_peg_1740.nextPosition();
                            result_peg_1740.setValue((void*) (long) letter_peg_1745);
                        } else {
                            goto out_peg_1744;
                        }
                        
                    }
                    goto success_peg_1741;
                    out_peg_1744:
                    goto loop_peg_1739;
                    success_peg_1741:
                    ;
                    result_peg_1727.addResult(result_peg_1740);
                } while (true);
                loop_peg_1739:
                if (result_peg_1727.matches() == 0){
                    goto out_peg_1737;
                }
                right = result_peg_1727.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(left,right);
                    result_peg_1727.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1727.getPosition());
        
        return result_peg_1727;
        out_peg_1737:
        Result result_peg_1746(myposition);
        
        {
        
            result_peg_1746.reset();
                do{
                    Result result_peg_1749(result_peg_1746.getPosition());
                    {
                        
                        char letter_peg_1754 = stream.get(result_peg_1749.getPosition());
                        if (letter_peg_1754 != '\0' && strchr("0123456789", letter_peg_1754) != NULL){
                            result_peg_1749.nextPosition();
                            result_peg_1749.setValue((void*) (long) letter_peg_1754);
                        } else {
                            goto out_peg_1753;
                        }
                        
                    }
                    goto success_peg_1750;
                    out_peg_1753:
                    goto loop_peg_1748;
                    success_peg_1750:
                    ;
                    result_peg_1746.addResult(result_peg_1749);
                } while (true);
                loop_peg_1748:
                if (result_peg_1746.matches() == 0){
                    goto out_peg_1755;
                }
            
            Result result_peg_1747 = result_peg_1746;
            
            Result result_peg_1758(result_peg_1746);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_1758.getPosition()))){
                        result_peg_1758.nextPosition();
                    } else {
                        goto not_peg_1757;
                    }
                }
                result_peg_1758.setValue((void*) ".");
                goto out_peg_1755;
                not_peg_1757:
                result_peg_1746.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = parseDouble(result_peg_1747.getValues());
                    result_peg_1746.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1746.getPosition());
        
        return result_peg_1746;
        out_peg_1755:
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

        
