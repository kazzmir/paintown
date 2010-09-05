

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

    namespace Cmd{
    
    



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
    Result chunk_whitespace;
    Result chunk_sw;
};

struct Chunk1{
    Result chunk_comment;
    Result chunk_section;
    Result chunk_section_item;
    Result chunk_section_title;
    Result chunk_assignment;
};

struct Chunk2{
    Result chunk_identifier;
    Result chunk_integer;
    Result chunk_float;
    Result chunk_string;
    Result chunk_range;
};

struct Chunk3{
    Result chunk_name;
    Result chunk_alpha_digit;
    Result chunk_valuelist;
    Result chunk_expr;
    Result chunk_expr_c;
};

struct Chunk4{
    Result chunk_expr2;
    Result chunk_expr3;
    Result chunk_expr4;
    Result chunk_expr5;
    Result chunk_expr6;
};

struct Chunk5{
    Result chunk_expr7;
    Result chunk_expr8;
    Result chunk_expr9;
    Result chunk_expr10;
    Result chunk_expr11;
};

struct Chunk6{
    Result chunk_expr12;
    Result chunk_expr13;
    Result chunk_expr_rest;
    Result chunk_expr2_rest;
    Result chunk_expr3_rest;
};

struct Chunk7{
    Result chunk_expr4_rest;
    Result chunk_expr5_rest;
    Result chunk_expr6_rest;
    Result chunk_expr7_rest;
    Result chunk_expr8_rest;
};

struct Chunk8{
    Result chunk_expr9_rest;
    Result chunk_compare;
    Result chunk_all_compare;
    Result chunk_expr10_rest;
    Result chunk_expr11_rest;
};

struct Chunk9{
    Result chunk_expr12_rest;
    Result chunk_unary;
    Result chunk_expr13_real;
    Result chunk_function;
    Result chunk_compare_equal;
};

struct Chunk10{
    Result chunk_hitdef__attack__attribute;
    Result chunk_hitdef__attack__attribute__item;
    Result chunk_hitdef__attack__type;
    Result chunk_hitdef__attack__movement;
    Result chunk_hitdef__attribute;
};

struct Chunk11{
    Result chunk_hitdef__attribute__item;
    Result chunk_args;
    Result chunk_function_name;
    Result chunk_function_rest;
    Result chunk_keys;
};

struct Chunk12{
    Result chunk_key_value_list;
    Result chunk_key;
    Result chunk_key_rest;
    Result chunk_key_real;
    Result chunk_key_modifier;
};

struct Chunk13{
    Result chunk_key_name;
    Result chunk_value;
    Result chunk_helper;
    Result chunk_helper__expression;
    Result chunk_helper__name;
};

struct Chunk14{
    Result chunk_helper__identifier;
    Result chunk_hitflag;
    Result chunk_keyword;
    Result chunk_keyword_real;
};

struct Column{
    Column():
        chunk0(0)
        ,chunk1(0)
        ,chunk2(0)
        ,chunk3(0)
        ,chunk4(0)
        ,chunk5(0)
        ,chunk6(0)
        ,chunk7(0)
        ,chunk8(0)
        ,chunk9(0)
        ,chunk10(0)
        ,chunk11(0)
        ,chunk12(0)
        ,chunk13(0)
        ,chunk14(0){
    }

    Chunk0 * chunk0;
    Chunk1 * chunk1;
    Chunk2 * chunk2;
    Chunk3 * chunk3;
    Chunk4 * chunk4;
    Chunk5 * chunk5;
    Chunk6 * chunk6;
    Chunk7 * chunk7;
    Chunk8 * chunk8;
    Chunk9 * chunk9;
    Chunk10 * chunk10;
    Chunk11 * chunk11;
    Chunk12 * chunk12;
    Chunk13 * chunk13;
    Chunk14 * chunk14;

    int hitCount(){
        return 
(chunk13 != NULL ? ((chunk13->chunk_key_name.calculated() ? 1 : 0)
+ (chunk13->chunk_value.calculated() ? 1 : 0)
+ (chunk13->chunk_helper.calculated() ? 1 : 0)
+ (chunk13->chunk_helper__expression.calculated() ? 1 : 0)
+ (chunk13->chunk_helper__name.calculated() ? 1 : 0)) : 0)
+
(chunk12 != NULL ? ((chunk12->chunk_key_value_list.calculated() ? 1 : 0)
+ (chunk12->chunk_key.calculated() ? 1 : 0)
+ (chunk12->chunk_key_rest.calculated() ? 1 : 0)
+ (chunk12->chunk_key_real.calculated() ? 1 : 0)
+ (chunk12->chunk_key_modifier.calculated() ? 1 : 0)) : 0)
+
(chunk11 != NULL ? ((chunk11->chunk_hitdef__attribute__item.calculated() ? 1 : 0)
+ (chunk11->chunk_args.calculated() ? 1 : 0)
+ (chunk11->chunk_function_name.calculated() ? 1 : 0)
+ (chunk11->chunk_function_rest.calculated() ? 1 : 0)
+ (chunk11->chunk_keys.calculated() ? 1 : 0)) : 0)
+
(chunk10 != NULL ? ((chunk10->chunk_hitdef__attack__attribute.calculated() ? 1 : 0)
+ (chunk10->chunk_hitdef__attack__attribute__item.calculated() ? 1 : 0)
+ (chunk10->chunk_hitdef__attack__type.calculated() ? 1 : 0)
+ (chunk10->chunk_hitdef__attack__movement.calculated() ? 1 : 0)
+ (chunk10->chunk_hitdef__attribute.calculated() ? 1 : 0)) : 0)
+
(chunk14 != NULL ? ((chunk14->chunk_helper__identifier.calculated() ? 1 : 0)
+ (chunk14->chunk_hitflag.calculated() ? 1 : 0)
+ (chunk14->chunk_keyword.calculated() ? 1 : 0)
+ (chunk14->chunk_keyword_real.calculated() ? 1 : 0)) : 0)
+
(chunk9 != NULL ? ((chunk9->chunk_expr12_rest.calculated() ? 1 : 0)
+ (chunk9->chunk_unary.calculated() ? 1 : 0)
+ (chunk9->chunk_expr13_real.calculated() ? 1 : 0)
+ (chunk9->chunk_function.calculated() ? 1 : 0)
+ (chunk9->chunk_compare_equal.calculated() ? 1 : 0)) : 0)
+
(chunk8 != NULL ? ((chunk8->chunk_expr9_rest.calculated() ? 1 : 0)
+ (chunk8->chunk_compare.calculated() ? 1 : 0)
+ (chunk8->chunk_all_compare.calculated() ? 1 : 0)
+ (chunk8->chunk_expr10_rest.calculated() ? 1 : 0)
+ (chunk8->chunk_expr11_rest.calculated() ? 1 : 0)) : 0)
+
(chunk3 != NULL ? ((chunk3->chunk_name.calculated() ? 1 : 0)
+ (chunk3->chunk_alpha_digit.calculated() ? 1 : 0)
+ (chunk3->chunk_valuelist.calculated() ? 1 : 0)
+ (chunk3->chunk_expr.calculated() ? 1 : 0)
+ (chunk3->chunk_expr_c.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_identifier.calculated() ? 1 : 0)
+ (chunk2->chunk_integer.calculated() ? 1 : 0)
+ (chunk2->chunk_float.calculated() ? 1 : 0)
+ (chunk2->chunk_string.calculated() ? 1 : 0)
+ (chunk2->chunk_range.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_comment.calculated() ? 1 : 0)
+ (chunk1->chunk_section.calculated() ? 1 : 0)
+ (chunk1->chunk_section_item.calculated() ? 1 : 0)
+ (chunk1->chunk_section_title.calculated() ? 1 : 0)
+ (chunk1->chunk_assignment.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)
+ (chunk0->chunk_whitespace.calculated() ? 1 : 0)
+ (chunk0->chunk_sw.calculated() ? 1 : 0)) : 0)
+
(chunk7 != NULL ? ((chunk7->chunk_expr4_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr5_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr6_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr7_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr8_rest.calculated() ? 1 : 0)) : 0)
+
(chunk6 != NULL ? ((chunk6->chunk_expr12.calculated() ? 1 : 0)
+ (chunk6->chunk_expr13.calculated() ? 1 : 0)
+ (chunk6->chunk_expr_rest.calculated() ? 1 : 0)
+ (chunk6->chunk_expr2_rest.calculated() ? 1 : 0)
+ (chunk6->chunk_expr3_rest.calculated() ? 1 : 0)) : 0)
+
(chunk5 != NULL ? ((chunk5->chunk_expr7.calculated() ? 1 : 0)
+ (chunk5->chunk_expr8.calculated() ? 1 : 0)
+ (chunk5->chunk_expr9.calculated() ? 1 : 0)
+ (chunk5->chunk_expr10.calculated() ? 1 : 0)
+ (chunk5->chunk_expr11.calculated() ? 1 : 0)) : 0)
+
(chunk4 != NULL ? ((chunk4->chunk_expr2.calculated() ? 1 : 0)
+ (chunk4->chunk_expr3.calculated() ? 1 : 0)
+ (chunk4->chunk_expr4.calculated() ? 1 : 0)
+ (chunk4->chunk_expr5.calculated() ? 1 : 0)
+ (chunk4->chunk_expr6.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 74;
    }

    ~Column(){
        delete chunk0;
        delete chunk1;
        delete chunk2;
        delete chunk3;
        delete chunk4;
        delete chunk5;
        delete chunk6;
        delete chunk7;
        delete chunk8;
        delete chunk9;
        delete chunk10;
        delete chunk11;
        delete chunk12;
        delete chunk13;
        delete chunk14;
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
        int context = 20;
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
Result rule_whitespace(Stream &, const int);
Result rule_sw(Stream &, const int);
Result rule_comment(Stream &, const int);
Result rule_section(Stream &, const int);
Result rule_section_item(Stream &, const int, Value ast);
Result rule_section_title(Stream &, const int);
Result rule_assignment(Stream &, const int);
Result rule_identifier(Stream &, const int);
Result rule_integer(Stream &, const int);
Result rule_float(Stream &, const int);
Result rule_string(Stream &, const int);
Result rule_range(Stream &, const int);
Result rule_name(Stream &, const int);
Result rule_alpha_digit(Stream &, const int);
Result rule_valuelist(Stream &, const int);
Result rule_expr(Stream &, const int);
Result rule_expr_c(Stream &, const int);
Result rule_expr2(Stream &, const int);
Result rule_expr3(Stream &, const int);
Result rule_expr4(Stream &, const int);
Result rule_expr5(Stream &, const int);
Result rule_expr6(Stream &, const int);
Result rule_expr7(Stream &, const int);
Result rule_expr8(Stream &, const int);
Result rule_expr9(Stream &, const int);
Result rule_expr10(Stream &, const int);
Result rule_expr11(Stream &, const int);
Result rule_expr12(Stream &, const int);
Result rule_expr13(Stream &, const int);
Result rule_expr_rest(Stream &, const int, Value left);
Result rule_expr2_rest(Stream &, const int, Value left);
Result rule_expr3_rest(Stream &, const int, Value left);
Result rule_expr4_rest(Stream &, const int, Value left);
Result rule_expr5_rest(Stream &, const int, Value left);
Result rule_expr6_rest(Stream &, const int, Value left);
Result rule_expr7_rest(Stream &, const int, Value left);
Result rule_expr8_rest(Stream &, const int, Value left);
Result rule_expr9_rest(Stream &, const int, Value left);
Result rule_compare(Stream &, const int);
Result rule_all_compare(Stream &, const int);
Result rule_expr10_rest(Stream &, const int, Value left);
Result rule_expr11_rest(Stream &, const int, Value left);
Result rule_expr12_rest(Stream &, const int, Value left);
Result rule_unary(Stream &, const int);
Result rule_expr13_real(Stream &, const int);
Result rule_function(Stream &, const int);
Result rule_compare_equal(Stream &, const int);
Result rule_hitdef__attack__attribute(Stream &, const int);
Result rule_hitdef__attack__attribute__item(Stream &, const int, Value attribute);
Result rule_hitdef__attack__type(Stream &, const int);
Result rule_hitdef__attack__movement(Stream &, const int);
Result rule_hitdef__attribute(Stream &, const int);
Result rule_hitdef__attribute__item(Stream &, const int);
Result rule_args(Stream &, const int);
Result rule_function_name(Stream &, const int);
Result rule_function_rest(Stream &, const int);
Result rule_keys(Stream &, const int);
Result rule_key_value_list(Stream &, const int);
Result rule_key(Stream &, const int);
Result rule_key_rest(Stream &, const int, Value left);
Result rule_key_real(Stream &, const int);
Result rule_key_modifier(Stream &, const int);
Result rule_key_name(Stream &, const int);
Result rule_value(Stream &, const int);
Result rule_helper(Stream &, const int);
Result rule_helper__expression(Stream &, const int);
Result rule_helper__name(Stream &, const int);
Result rule_helper__identifier(Stream &, const int);
Result rule_hitflag(Stream &, const int);
Result rule_keyword(Stream &, const int);
Result rule_keyword_real(Stream &, const int);



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

std::string combineStrings(const Value & input){
    std::ostringstream out;
    for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
        std::string * str = (std::string *) (*it).getValue();
        out << *str;
    }
    return out.str();
}

std::string * toString(const char * x){
    std::string * object = new std::string(x);
    GC::save(object);
    return object;
}

Ast::Value * makeInteger(const Value & sign, const Value & digits){
    std::istringstream in(*toString(digits));
    double value = 0;
    in >> value;

    if (sign.getValue() != 0){
        value = -value;
    }
    
    Ast::Number * object = new Ast::Number(value);
    GC::save(object);
    return object;
}

std::string * toString(char front, const Value & input){
  std::string * str = toString(input);
  str->insert(str->begin(), front);
  return str;
}

Ast::String * makeString(const Value & value){
    Ast::String * object = new Ast::String(toString(value));
    GC::save(object);
    return object;
}

SectionList * makeSectionList(){
    SectionList * object = new SectionList();
    GC::save(object);
    return object;
}

Ast::Value * makeExpression(){
    Ast::Value * object = new Ast::Expression();
    GC::save(object);
    return object;
}

Ast::Value * makeHelper(const Value & name, const Value & id){
    Ast::Value * helper = new Ast::Helper(std::string(as<const char*>(name)), as<Ast::Value*>(id));
    GC::save(helper);
    return helper;
}

Ast::Value * makeKeyword(const char * name){
    Ast::Value * keyword = new Ast::Keyword(name);
    GC::save(keyword);
    return keyword;
}

Ast::Value * makeExpressionInfix(Ast::ExpressionInfix::InfixType type, const Value & left, const Value & right){
    Ast::Value * object = new Ast::ExpressionInfix(type, as<Ast::Value*>(left), as<Ast::Value*>(right));
    GC::save(object);
    return object;
}

Ast::Value * makeExpressionOr(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Or, left, right);
}

Ast::Value * makeExpressionBitwiseOr(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::BitwiseOr, left, right);
}

Ast::Value * makeExpressionBitwiseXOr(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::BitwiseXOr, left, right);
}

Ast::Value * makeExpressionBitwiseAnd(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::BitwiseAnd, left, right);
}

Ast::Value * makeExpressionAssignment(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Assignment, left, right);
}

Ast::Value * makeExpressionEquals(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Equals, left, right);
}

Ast::Value * makeExpressionUnequals(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Unequals, left, right);
}

Ast::Value * makeExpressionGreaterThanEquals(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::GreaterThanEquals, left, right);
}

Ast::Value * makeExpressionGreaterThan(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::GreaterThan, left, right);
}

Ast::Value * makeExpressionLessThanEquals(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::LessThanEquals, left, right);
}

Ast::Value * makeExpressionLessThan(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::LessThan, left, right);
}

Ast::Value * makeExpressionXOr(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::XOr, left, right);
}

Ast::Value * makeExpressionAnd(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::And, left, right);
}

Ast::Value * makeExpressionAdd(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Add, left, right);
}

Ast::Value * makeExpressionSubtract(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Subtract, left, right);
}

Ast::Value * makeExpressionMultiply(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Multiply, left, right);
}

Ast::Value * makeExpressionDivide(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Divide, left, right);
}

Ast::Value * makeExpressionModulo(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Modulo, left, right);
}

Ast::Value * makeExpressionPower(const Value & left, const Value & right){
    return makeExpressionInfix(Ast::ExpressionInfix::Power, left, right);
}

Ast::Value * negateExpression(const Value & exp){
    Ast::Value * expression = as<Ast::Value*>(exp);
    Ast::Value * negation = new Ast::ExpressionUnary(Ast::ExpressionUnary::Negation, expression);
    GC::save(negation);
    return negation;
}

Ast::Value * makeUnaryExpression(const Value & unaries, const Value & exp){
    Ast::Value * expression = as<Ast::Value*>(exp);
    for (Value::iterator it = unaries.getValues().begin(); it != unaries.getValues().end(); it++){
        Ast::ExpressionUnary::UnaryType unary = (Ast::ExpressionUnary::UnaryType) (long) (*it).getValue();
        expression = new Ast::ExpressionUnary(unary, expression);
        GC::save(expression);
    }
    return expression;
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

Ast::Value * makeFunction(const Value & name, const Value & arg1){
    Ast::Value * function = new Ast::Function(std::string(as<const char*>(name)), as<Ast::ValueList*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction(const std::string & name, const Value & arg1){
    Ast::Value * function = new Ast::Function(name, as<Ast::ValueList*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction1(const Value & name, const Value & arg1){
    return makeFunction(name, makeValueList(arg1, Value()));
}

std::string combine(const char * name, const Value & what){
    if (what.getValue() != 0){
        std::ostringstream out;
        out << name << as<Ast::Number*>(what)->toString();
        return out.str();
    } else {
        return std::string(name);
    }
}

/*
Ast::Value * makeFunction(const Value & name, const Value & arg1, const Value & arg2, const Value & arg3){
    Ast::Value * function = new Ast::Function(std::string(as<const char*>(name)),
                                              as<Ast::Value*>(arg1),
                                              as<Ast::Value*>(arg2),
                                              as<Ast::Value*>(arg3));
    GC::save(function);
    return function;
}
*/

Ast::Value * makeRange(Ast::Range::RangeType type, const Value & low, const Value & high){
    Ast::Value * range = new Ast::Range(type, as<Ast::Value*>(low), as<Ast::Value*>(high));
    GC::save(range);
    return range;
}

Ast::Identifier * makeIdentifier(int line, int column, const Value & front, const Value & rest){
    std::list<std::string> ids;
    ids.push_back(*as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        /* this works becuase as() will coerce a void* into Value(void*) */
        ids.push_back(*as<std::string*>((*it).getValue()));
    }
    Ast::Identifier * object = new Ast::Identifier(line, column, ids);
    GC::save(object);
    return object;
}

Ast::Attribute * makeAttribute(int line, int column, const Value & id, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(line, column, as<Ast::Identifier*>(id), as<Ast::Value*>(data));
    GC::save(object);
    return object;
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

Ast::Attribute * makeIndexedAttribute(const Value & id, const Value & index, const Value & data){
    Ast::Attribute * object = new Ast::AttributeArray(as<Ast::Identifier*>(id), as<Ast::Value*>(index), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Identifier * makeSimpleIdentifier(const Value & name){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(as<const char *>(name));
    GC::save(identifier);
    return identifier;
}

Ast::Identifier * makeSimpleIdentifier(const char * name){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(name);
    GC::save(identifier);
    return identifier;
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

Ast::Identifier * makeSimpleIdentifier(const std::string & str){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(str);
    GC::save(identifier);
    return identifier;
}

Ast::Attribute * makeAttribute(const char * name, const Value & data){
    Ast::AttributeSimple * object = new Ast::AttributeSimple(makeSimpleIdentifier(std::string(name)), as<Ast::Value*>(data));
    GC::save(object);
    return object;
}

Ast::Value * makeNumber(double value){
    Ast::Number * object = new Ast::Number(value);
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

Ast::Key * makeKeyModifier(Ast::Key * in, Ast::KeyModifier::ModifierType type, int ticks = 0){
    Ast::Key * modded = new Ast::KeyModifier(type, in, ticks);
    GC::save(modded);
    return modded;
}

Ast::Key * makeKeyCombined(const Value & left, const Value & right){
    Ast::Key * key = new Ast::KeyCombined(as<Ast::Key*>(left), as<Ast::Key*>(right));
    GC::save(key);
    return key;
}

/* for commands with no keys listed */
Ast::Key * makeEmptyKeyList(){
    std::vector<Ast::Key*> all;
    Ast::Key * object = new Ast::KeyList(all);
    GC::save(object);
    return object;
}
    
Ast::Key * makeKeyList(const Value & first, const Value & rest){
    std::vector<Ast::Key*> all;
    all.push_back(as<Ast::Key*>(first));

    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        Ast::Key * key = (Ast::Key*) (*it).getValue();
        all.push_back(key);
    }

    Ast::Key * object = new Ast::KeyList(all);
    GC::save(object);
    return object;
}

Ast::Key * makeKey(const Value & value){
    Ast::Key * key = new Ast::KeySingle(as<const char *>(value));
    GC::save(key);
    return key;
}

class KeyModifier{
public:
    virtual Ast::Key * apply(Ast::Key * in) = 0;
    virtual ~KeyModifier(){
    }
};

class ReleaseKeyModifier: public KeyModifier {
public:
    ReleaseKeyModifier(int ticks):
    ticks(ticks){
    }

    virtual ~ReleaseKeyModifier(){
    }

    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Release, ticks);
    }

protected:

    int ticks;
};

class DirectionKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Direction);
    }

    virtual ~DirectionKeyModifier(){
    }
};

class HeldDownKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::MustBeHeldDown);
    }

    virtual ~HeldDownKeyModifier(){
    }
};

class OnlyKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Only);
    }

    virtual ~OnlyKeyModifier(){
    }
};

Ast::Key * applyKeyModifiers(const Value & mods, Ast::Key * key){
    for (Value::iterator it = mods.getValues().begin(); it != mods.getValues().end(); it++){
        KeyModifier * modifier = (KeyModifier*) (*it).getValue();
        key = modifier->apply(key);
        delete modifier;
    }
    return key;
}

Ast::HitDefAttackAttribute * makeHitDefAttackAttribute(){
    Ast::HitDefAttackAttribute * object = new Ast::HitDefAttackAttribute();
    GC::save(object);
    return object;
}

Ast::HitDefAttribute * makeHitDefAttribute(const Value & input){
    std::ostringstream out;
    for (Value::iterator it = input.getValues().begin(); it != input.getValues().end(); it++){
        out << (char*) (*it).getValue();
    }
    Ast::HitDefAttribute * object = new Ast::HitDefAttribute(out.str());
    GC::save(object);
    return object;
}




Result rule_start(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_start.calculated()){
        return column_peg_1.chunk0->chunk_start;
    }
    
    RuleTrace trace_peg_135(stream, "start");
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
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                goto out_peg_11;
                            }
                        }
                        result_peg_8.setValue((void*) "\n");
                            
                    }
                    goto success_peg_9;
                    out_peg_11:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\r"[i], stream.get(result_peg_8.getPosition()))){
                                result_peg_8.nextPosition();
                            } else {
                                goto out_peg_13;
                            }
                        }
                        result_peg_8.setValue((void*) "\r");
                            
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
                    
                        {
                                
                                result_peg_16.reset();
                                do{
                                    Result result_peg_48(result_peg_16.getPosition());
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_59;
                                                }
                                            }
                                            result_peg_48.setValue((void*) " ");
                                                
                                        }
                                        goto success_peg_57;
                                        out_peg_59:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_61;
                                                }
                                            }
                                            result_peg_48.setValue((void*) "\t");
                                                
                                        }
                                        goto success_peg_57;
                                        out_peg_61:
                                        goto out_peg_56;
                                        success_peg_57:
                                        ;
                                        
                                    }
                                    goto success_peg_49;
                                    out_peg_56:
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_71;
                                                }
                                            }
                                            result_peg_48.setValue((void*) "\n");
                                                
                                        }
                                        goto success_peg_69;
                                        out_peg_71:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_48.getPosition()))){
                                                    result_peg_48.nextPosition();
                                                } else {
                                                    goto out_peg_73;
                                                }
                                            }
                                            result_peg_48.setValue((void*) "\r");
                                                
                                        }
                                        goto success_peg_69;
                                        out_peg_73:
                                        goto out_peg_68;
                                        success_peg_69:
                                        ;
                                        
                                    }
                                    goto success_peg_49;
                                    out_peg_68:
                                    goto loop_peg_47;
                                    success_peg_49:
                                    ;
                                    result_peg_16.addResult(result_peg_48);
                                } while (true);
                                loop_peg_47:
                                ;
                                        
                            }
                            goto success_peg_18;
                            goto loop_peg_15;
                            success_peg_18:
                            ;
                        
                        
                        
                        result_peg_16 = rule_line(stream, result_peg_16.getPosition(), current);
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        result_peg_16 = rule_whitespace(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                goto loop_peg_15;
                            }
                        
                        
                        
                        int save_peg_76 = result_peg_16.getPosition();
                            
                            result_peg_16 = rule_line_end(stream, result_peg_16.getPosition());
                            if (result_peg_16.error()){
                                
                                result_peg_16 = Result(save_peg_76);
                                result_peg_16.setValue((void*) 0);
                                
                            }
                        
                        
                    }
                    result_peg_2.addResult(result_peg_16);
                } while (true);
                loop_peg_15:
                ;
            
            
            
            {
                    
                    result_peg_2.reset();
                    do{
                        Result result_peg_108(result_peg_2.getPosition());
                        {
                            
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(" "[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_119;
                                    }
                                }
                                result_peg_108.setValue((void*) " ");
                                    
                            }
                            goto success_peg_117;
                            out_peg_119:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\t"[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_121;
                                    }
                                }
                                result_peg_108.setValue((void*) "\t");
                                    
                            }
                            goto success_peg_117;
                            out_peg_121:
                            goto out_peg_116;
                            success_peg_117:
                            ;
                            
                        }
                        goto success_peg_109;
                        out_peg_116:
                        {
                            
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_131;
                                    }
                                }
                                result_peg_108.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_129;
                            out_peg_131:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_108.getPosition()))){
                                        result_peg_108.nextPosition();
                                    } else {
                                        goto out_peg_133;
                                    }
                                }
                                result_peg_108.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_129;
                            out_peg_133:
                            goto out_peg_128;
                            success_peg_129:
                            ;
                            
                        }
                        goto success_peg_109;
                        out_peg_128:
                        goto loop_peg_107;
                        success_peg_109:
                        ;
                        result_peg_2.addResult(result_peg_108);
                    } while (true);
                    loop_peg_107:
                    ;
                            
                }
                goto success_peg_78;
                goto out_peg_5;
                success_peg_78:
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
        
    
    } catch (...){
         GC::cleanup(0); 
        throw;
    }
    
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    RuleTrace trace_peg_202(stream, "line");
    int myposition = position;
    
    
    
    Result result_peg_137(myposition);
        
        {
        
            {
                    
                    result_peg_137.reset();
                    do{
                        Result result_peg_149(result_peg_137.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_149.getPosition()))){
                                    result_peg_149.nextPosition();
                                } else {
                                    goto out_peg_152;
                                }
                            }
                            result_peg_149.setValue((void*) " ");
                                
                        }
                        goto success_peg_150;
                        out_peg_152:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_149.getPosition()))){
                                    result_peg_149.nextPosition();
                                } else {
                                    goto out_peg_154;
                                }
                            }
                            result_peg_149.setValue((void*) "\t");
                                
                        }
                        goto success_peg_150;
                        out_peg_154:
                        goto loop_peg_148;
                        success_peg_150:
                        ;
                        result_peg_137.addResult(result_peg_149);
                    } while (true);
                    loop_peg_148:
                    ;
                            
                }
                goto success_peg_139;
                goto out_peg_155;
                success_peg_139:
                ;
            
            
            
            result_peg_137 = rule_comment(stream, result_peg_137.getPosition());
                if (result_peg_137.error()){
                    goto out_peg_155;
                }
            
            
            
            Result result_peg_157(result_peg_137.getPosition());
                result_peg_157 = rule_line_end(stream, result_peg_157.getPosition());
                if (result_peg_157.error()){
                    goto out_peg_155;
                }
            
            
        }
        stream.update(result_peg_137.getPosition());
        
        return result_peg_137;
        out_peg_155:
        Result result_peg_158(myposition);
        
        {
        
            {
                    
                    result_peg_158.reset();
                    do{
                        Result result_peg_170(result_peg_158.getPosition());
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
                        result_peg_158.addResult(result_peg_170);
                    } while (true);
                    loop_peg_169:
                    ;
                            
                }
                goto success_peg_160;
                goto out_peg_176;
                success_peg_160:
                ;
            
            
            
            result_peg_158 = rule_section(stream, result_peg_158.getPosition());
                if (result_peg_158.error()){
                    goto out_peg_176;
                }
            
            Result result_peg_177 = result_peg_158;
            
            {
                    Value value((void*) 0);
                    addSection(current, result_peg_177.getValues());
                    result_peg_158.setValue(value);
                }
            
            
        }
        stream.update(result_peg_158.getPosition());
        
        return result_peg_158;
        out_peg_176:
        Result result_peg_178(myposition);
        
        {
        
            {
                    
                    result_peg_178.reset();
                    do{
                        Result result_peg_190(result_peg_178.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    goto out_peg_193;
                                }
                            }
                            result_peg_190.setValue((void*) " ");
                                
                        }
                        goto success_peg_191;
                        out_peg_193:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_190.getPosition()))){
                                    result_peg_190.nextPosition();
                                } else {
                                    goto out_peg_195;
                                }
                            }
                            result_peg_190.setValue((void*) "\t");
                                
                        }
                        goto success_peg_191;
                        out_peg_195:
                        goto loop_peg_189;
                        success_peg_191:
                        ;
                        result_peg_178.addResult(result_peg_190);
                    } while (true);
                    loop_peg_189:
                    ;
                            
                }
                goto success_peg_180;
                goto out_peg_196;
                success_peg_180:
                ;
            
            
            
            {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_178.getPosition()))){
                            result_peg_178.nextPosition();
                        } else {
                            goto out_peg_199;
                        }
                    }
                    result_peg_178.setValue((void*) "\n");
                        
                }
                goto success_peg_197;
                out_peg_199:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_178.getPosition()))){
                            result_peg_178.nextPosition();
                        } else {
                            goto out_peg_201;
                        }
                    }
                    result_peg_178.setValue((void*) "\r");
                        
                }
                goto success_peg_197;
                out_peg_201:
                goto out_peg_196;
                success_peg_197:
                ;
            
            
        }
        stream.update(result_peg_178.getPosition());
        
        return result_peg_178;
        out_peg_196:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_203 = stream.getColumn(position);
    if (column_peg_203.chunk0 != 0 && column_peg_203.chunk0->chunk_line_end.calculated()){
        return column_peg_203.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_217(stream, "line_end");
    int myposition = position;
    
    
    
    Result result_peg_204(myposition);
        
        result_peg_204.reset();
        do{
            Result result_peg_206(result_peg_204.getPosition());
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_206.getPosition()))){
                        result_peg_206.nextPosition();
                    } else {
                        goto out_peg_209;
                    }
                }
                result_peg_206.setValue((void*) "\n");
                    
            }
            goto success_peg_207;
            out_peg_209:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_206.getPosition()))){
                        result_peg_206.nextPosition();
                    } else {
                        goto out_peg_211;
                    }
                }
                result_peg_206.setValue((void*) "\r");
                    
            }
            goto success_peg_207;
            out_peg_211:
            goto loop_peg_205;
            success_peg_207:
            ;
            result_peg_204.addResult(result_peg_206);
        } while (true);
        loop_peg_205:
        if (result_peg_204.matches() == 0){
            goto out_peg_212;
        }
        
        if (column_peg_203.chunk0 == 0){
            column_peg_203.chunk0 = new Chunk0();
        }
        column_peg_203.chunk0->chunk_line_end = result_peg_204;
        stream.update(result_peg_204.getPosition());
        
        
        return result_peg_204;
        out_peg_212:
        Result result_peg_213(myposition);
        
        {
        
            Result result_peg_215(result_peg_213.getPosition());
                if ('\0' == stream.get(result_peg_215.getPosition())){
                    result_peg_215.nextPosition();
                    result_peg_215.setValue((void *) '\0');
                } else {
                    goto out_peg_216;
                }
            
            
            
            
            
            
        }
        
        if (column_peg_203.chunk0 == 0){
            column_peg_203.chunk0 = new Chunk0();
        }
        column_peg_203.chunk0->chunk_line_end = result_peg_213;
        stream.update(result_peg_213.getPosition());
        
        
        return result_peg_213;
        out_peg_216:
    
        if (column_peg_203.chunk0 == 0){
            column_peg_203.chunk0 = new Chunk0();
        }
        column_peg_203.chunk0->chunk_line_end = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_218 = stream.getColumn(position);
    if (column_peg_218.chunk0 != 0 && column_peg_218.chunk0->chunk_whitespace.calculated()){
        return column_peg_218.chunk0->chunk_whitespace;
    }
    
    RuleTrace trace_peg_222(stream, "whitespace");
    int myposition = position;
    
    
    
    Result result_peg_219(myposition);
        
        result_peg_219.reset();
        do{
            Result result_peg_221(result_peg_219.getPosition());
            result_peg_221 = rule_sw(stream, result_peg_221.getPosition());
            if (result_peg_221.error()){
                goto loop_peg_220;
            }
            result_peg_219.addResult(result_peg_221);
        } while (true);
        loop_peg_220:
        ;
        
        if (column_peg_218.chunk0 == 0){
            column_peg_218.chunk0 = new Chunk0();
        }
        column_peg_218.chunk0->chunk_whitespace = result_peg_219;
        stream.update(result_peg_219.getPosition());
        
        
        return result_peg_219;
    
        if (column_peg_218.chunk0 == 0){
            column_peg_218.chunk0 = new Chunk0();
        }
        column_peg_218.chunk0->chunk_whitespace = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_223 = stream.getColumn(position);
    if (column_peg_223.chunk0 != 0 && column_peg_223.chunk0->chunk_sw.calculated()){
        return column_peg_223.chunk0->chunk_sw;
    }
    
    RuleTrace trace_peg_233(stream, "sw");
    int myposition = position;
    
    
    
    Result result_peg_224(myposition);
        
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_224.getPosition()))){
                    result_peg_224.nextPosition();
                } else {
                    goto out_peg_227;
                }
            }
            result_peg_224.setValue((void*) " ");
                
        }
        goto success_peg_225;
        out_peg_227:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_224.getPosition()))){
                    result_peg_224.nextPosition();
                } else {
                    goto out_peg_229;
                }
            }
            result_peg_224.setValue((void*) "\t");
                
        }
        goto success_peg_225;
        out_peg_229:
        goto out_peg_230;
        success_peg_225:
        ;
        
        if (column_peg_223.chunk0 == 0){
            column_peg_223.chunk0 = new Chunk0();
        }
        column_peg_223.chunk0->chunk_sw = result_peg_224;
        stream.update(result_peg_224.getPosition());
        
        
        return result_peg_224;
        out_peg_230:
        Result result_peg_231(myposition);
        
        result_peg_231 = rule_comment(stream, result_peg_231.getPosition());
        if (result_peg_231.error()){
            goto out_peg_232;
        }
        
        if (column_peg_223.chunk0 == 0){
            column_peg_223.chunk0 = new Chunk0();
        }
        column_peg_223.chunk0->chunk_sw = result_peg_231;
        stream.update(result_peg_231.getPosition());
        
        
        return result_peg_231;
        out_peg_232:
    
        if (column_peg_223.chunk0 == 0){
            column_peg_223.chunk0 = new Chunk0();
        }
        column_peg_223.chunk0->chunk_sw = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_234 = stream.getColumn(position);
    if (column_peg_234.chunk1 != 0 && column_peg_234.chunk1->chunk_comment.calculated()){
        return column_peg_234.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_293(stream, "comment");
    int myposition = position;
    
    
    
    Result result_peg_235(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(";"[i], stream.get(result_peg_235.getPosition()))){
                        result_peg_235.nextPosition();
                    } else {
                        goto out_peg_237;
                    }
                }
                result_peg_235.setValue((void*) ";");
            
            
            
            result_peg_235.reset();
                do{
                    Result result_peg_239(result_peg_235.getPosition());
                    {
                    
                        Result result_peg_242(result_peg_239);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_242.getPosition()))){
                                        result_peg_242.nextPosition();
                                    } else {
                                        goto out_peg_245;
                                    }
                                }
                                result_peg_242.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_243;
                            out_peg_245:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_242.getPosition()))){
                                        result_peg_242.nextPosition();
                                    } else {
                                        goto out_peg_247;
                                    }
                                }
                                result_peg_242.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_243;
                            out_peg_247:
                            goto not_peg_241;
                            success_peg_243:
                            ;
                            goto loop_peg_238;
                            not_peg_241:
                            result_peg_239.setValue((void*)0);
                        
                        
                        
                        char temp_peg_248 = stream.get(result_peg_239.getPosition());
                            if (temp_peg_248 != '\0'){
                                result_peg_239.setValue((void*) (long) temp_peg_248);
                                result_peg_239.nextPosition();
                            } else {
                                goto loop_peg_238;
                            }
                        
                        
                    }
                    result_peg_235.addResult(result_peg_239);
                } while (true);
                loop_peg_238:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_235;
        stream.update(result_peg_235.getPosition());
        
        
        return result_peg_235;
        out_peg_237:
        Result result_peg_249(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_249.getPosition()))){
                        result_peg_249.nextPosition();
                    } else {
                        goto out_peg_251;
                    }
                }
                result_peg_249.setValue((void*) "=");
            
            
            
            result_peg_249.reset();
                do{
                    Result result_peg_253(result_peg_249.getPosition());
                    {
                    
                        Result result_peg_256(result_peg_253);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_256.getPosition()))){
                                        result_peg_256.nextPosition();
                                    } else {
                                        goto out_peg_259;
                                    }
                                }
                                result_peg_256.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_257;
                            out_peg_259:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_256.getPosition()))){
                                        result_peg_256.nextPosition();
                                    } else {
                                        goto out_peg_261;
                                    }
                                }
                                result_peg_256.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_257;
                            out_peg_261:
                            goto not_peg_255;
                            success_peg_257:
                            ;
                            goto loop_peg_252;
                            not_peg_255:
                            result_peg_253.setValue((void*)0);
                        
                        
                        
                        char temp_peg_262 = stream.get(result_peg_253.getPosition());
                            if (temp_peg_262 != '\0'){
                                result_peg_253.setValue((void*) (long) temp_peg_262);
                                result_peg_253.nextPosition();
                            } else {
                                goto loop_peg_252;
                            }
                        
                        
                    }
                    result_peg_249.addResult(result_peg_253);
                } while (true);
                loop_peg_252:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_249;
        stream.update(result_peg_249.getPosition());
        
        
        return result_peg_249;
        out_peg_251:
        Result result_peg_263(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_263.getPosition()))){
                        result_peg_263.nextPosition();
                    } else {
                        goto out_peg_265;
                    }
                }
                result_peg_263.setValue((void*) "-");
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_263.getPosition()))){
                        result_peg_263.nextPosition();
                    } else {
                        goto out_peg_265;
                    }
                }
                result_peg_263.setValue((void*) "-");
            
            
            
            result_peg_263.reset();
                do{
                    Result result_peg_268(result_peg_263.getPosition());
                    {
                    
                        Result result_peg_271(result_peg_268);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_271.getPosition()))){
                                        result_peg_271.nextPosition();
                                    } else {
                                        goto out_peg_274;
                                    }
                                }
                                result_peg_271.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_272;
                            out_peg_274:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_271.getPosition()))){
                                        result_peg_271.nextPosition();
                                    } else {
                                        goto out_peg_276;
                                    }
                                }
                                result_peg_271.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_272;
                            out_peg_276:
                            goto not_peg_270;
                            success_peg_272:
                            ;
                            goto loop_peg_267;
                            not_peg_270:
                            result_peg_268.setValue((void*)0);
                        
                        
                        
                        char temp_peg_277 = stream.get(result_peg_268.getPosition());
                            if (temp_peg_277 != '\0'){
                                result_peg_268.setValue((void*) (long) temp_peg_277);
                                result_peg_268.nextPosition();
                            } else {
                                goto loop_peg_267;
                            }
                        
                        
                    }
                    result_peg_263.addResult(result_peg_268);
                } while (true);
                loop_peg_267:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_263;
        stream.update(result_peg_263.getPosition());
        
        
        return result_peg_263;
        out_peg_265:
        Result result_peg_278(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_278.getPosition()))){
                        result_peg_278.nextPosition();
                    } else {
                        goto out_peg_280;
                    }
                }
                result_peg_278.setValue((void*) ":");
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_278.getPosition()))){
                        result_peg_278.nextPosition();
                    } else {
                        goto out_peg_280;
                    }
                }
                result_peg_278.setValue((void*) ":");
            
            
            
            result_peg_278.reset();
                do{
                    Result result_peg_283(result_peg_278.getPosition());
                    {
                    
                        Result result_peg_286(result_peg_283);
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\n"[i], stream.get(result_peg_286.getPosition()))){
                                        result_peg_286.nextPosition();
                                    } else {
                                        goto out_peg_289;
                                    }
                                }
                                result_peg_286.setValue((void*) "\n");
                                    
                            }
                            goto success_peg_287;
                            out_peg_289:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\r"[i], stream.get(result_peg_286.getPosition()))){
                                        result_peg_286.nextPosition();
                                    } else {
                                        goto out_peg_291;
                                    }
                                }
                                result_peg_286.setValue((void*) "\r");
                                    
                            }
                            goto success_peg_287;
                            out_peg_291:
                            goto not_peg_285;
                            success_peg_287:
                            ;
                            goto loop_peg_282;
                            not_peg_285:
                            result_peg_283.setValue((void*)0);
                        
                        
                        
                        char temp_peg_292 = stream.get(result_peg_283.getPosition());
                            if (temp_peg_292 != '\0'){
                                result_peg_283.setValue((void*) (long) temp_peg_292);
                                result_peg_283.nextPosition();
                            } else {
                                goto loop_peg_282;
                            }
                        
                        
                    }
                    result_peg_278.addResult(result_peg_283);
                } while (true);
                loop_peg_282:
                ;
            
            
        }
        
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = result_peg_278;
        stream.update(result_peg_278.getPosition());
        
        
        return result_peg_278;
        out_peg_280:
    
        if (column_peg_234.chunk1 == 0){
            column_peg_234.chunk1 = new Chunk1();
        }
        column_peg_234.chunk1->chunk_comment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_294 = stream.getColumn(position);
    if (column_peg_294.chunk1 != 0 && column_peg_294.chunk1->chunk_section.calculated()){
        return column_peg_294.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_362(stream, "section");
    int myposition = position;
    
    
    Value ast;
    Result result_peg_295(myposition);
        
        {
        
            result_peg_295 = rule_section_title(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
            
            Result result_peg_296 = result_peg_295;
            
            {
                    Value value((void*) 0);
                    value = makeSection(result_peg_296.getValues());
                    result_peg_295.setValue(value);
                }
                ast = result_peg_295.getValues();
            
            
            
            result_peg_295 = rule_whitespace(stream, result_peg_295.getPosition());
                if (result_peg_295.error()){
                    goto out_peg_297;
                }
            
            
            
            result_peg_295.reset();
                do{
                    Result result_peg_302(result_peg_295.getPosition());
                    {
                    
                        {
                                
                                result_peg_302.reset();
                                do{
                                    Result result_peg_334(result_peg_302.getPosition());
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar(" "[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_345;
                                                }
                                            }
                                            result_peg_334.setValue((void*) " ");
                                                
                                        }
                                        goto success_peg_343;
                                        out_peg_345:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\t"[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_347;
                                                }
                                            }
                                            result_peg_334.setValue((void*) "\t");
                                                
                                        }
                                        goto success_peg_343;
                                        out_peg_347:
                                        goto out_peg_342;
                                        success_peg_343:
                                        ;
                                        
                                    }
                                    goto success_peg_335;
                                    out_peg_342:
                                    {
                                        
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\n"[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_357;
                                                }
                                            }
                                            result_peg_334.setValue((void*) "\n");
                                                
                                        }
                                        goto success_peg_355;
                                        out_peg_357:
                                        {
                                            
                                            for (int i = 0; i < 1; i++){
                                                if (compareChar("\r"[i], stream.get(result_peg_334.getPosition()))){
                                                    result_peg_334.nextPosition();
                                                } else {
                                                    goto out_peg_359;
                                                }
                                            }
                                            result_peg_334.setValue((void*) "\r");
                                                
                                        }
                                        goto success_peg_355;
                                        out_peg_359:
                                        goto out_peg_354;
                                        success_peg_355:
                                        ;
                                        
                                    }
                                    goto success_peg_335;
                                    out_peg_354:
                                    goto loop_peg_333;
                                    success_peg_335:
                                    ;
                                    result_peg_302.addResult(result_peg_334);
                                } while (true);
                                loop_peg_333:
                                ;
                                        
                            }
                            goto success_peg_304;
                            goto loop_peg_301;
                            success_peg_304:
                            ;
                        
                        
                        
                        result_peg_302 = rule_section_item(stream, result_peg_302.getPosition(), ast);
                            if (result_peg_302.error()){
                                goto loop_peg_301;
                            }
                        
                        
                        
                        result_peg_302 = rule_whitespace(stream, result_peg_302.getPosition());
                            if (result_peg_302.error()){
                                goto loop_peg_301;
                            }
                        
                        
                        
                        result_peg_302 = rule_line_end(stream, result_peg_302.getPosition());
                            if (result_peg_302.error()){
                                goto loop_peg_301;
                            }
                        
                        
                    }
                    result_peg_295.addResult(result_peg_302);
                } while (true);
                loop_peg_301:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    value = ast;
                    result_peg_295.setValue(value);
                }
            
            
        }
        
        if (column_peg_294.chunk1 == 0){
            column_peg_294.chunk1 = new Chunk1();
        }
        column_peg_294.chunk1->chunk_section = result_peg_295;
        stream.update(result_peg_295.getPosition());
        
        
        return result_peg_295;
        out_peg_297:
    
        if (column_peg_294.chunk1 == 0){
            column_peg_294.chunk1 = new Chunk1();
        }
        column_peg_294.chunk1->chunk_section = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_403(stream, "section_item");
    int myposition = position;
    
    
    Value data;
    Result result_peg_364(myposition);
        
        {
        
            {
                    
                    result_peg_364.reset();
                    do{
                        Result result_peg_376(result_peg_364.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_376.getPosition()))){
                                    result_peg_376.nextPosition();
                                } else {
                                    goto out_peg_379;
                                }
                            }
                            result_peg_376.setValue((void*) " ");
                                
                        }
                        goto success_peg_377;
                        out_peg_379:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_376.getPosition()))){
                                    result_peg_376.nextPosition();
                                } else {
                                    goto out_peg_381;
                                }
                            }
                            result_peg_376.setValue((void*) "\t");
                                
                        }
                        goto success_peg_377;
                        out_peg_381:
                        goto loop_peg_375;
                        success_peg_377:
                        ;
                        result_peg_364.addResult(result_peg_376);
                    } while (true);
                    loop_peg_375:
                    ;
                            
                }
                goto success_peg_366;
                goto out_peg_382;
                success_peg_366:
                ;
            
            
            
            result_peg_364 = rule_comment(stream, result_peg_364.getPosition());
                if (result_peg_364.error()){
                    goto out_peg_382;
                }
            
            
        }
        stream.update(result_peg_364.getPosition());
        
        return result_peg_364;
        out_peg_382:
        Result result_peg_383(myposition);
        
        {
        
            {
                    
                    result_peg_383.reset();
                    do{
                        Result result_peg_395(result_peg_383.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_395.getPosition()))){
                                    result_peg_395.nextPosition();
                                } else {
                                    goto out_peg_398;
                                }
                            }
                            result_peg_395.setValue((void*) " ");
                                
                        }
                        goto success_peg_396;
                        out_peg_398:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_395.getPosition()))){
                                    result_peg_395.nextPosition();
                                } else {
                                    goto out_peg_400;
                                }
                            }
                            result_peg_395.setValue((void*) "\t");
                                
                        }
                        goto success_peg_396;
                        out_peg_400:
                        goto loop_peg_394;
                        success_peg_396:
                        ;
                        result_peg_383.addResult(result_peg_395);
                    } while (true);
                    loop_peg_394:
                    ;
                            
                }
                goto success_peg_385;
                goto out_peg_401;
                success_peg_385:
                ;
            
            
            
            result_peg_383 = rule_assignment(stream, result_peg_383.getPosition());
                if (result_peg_383.error()){
                    goto out_peg_401;
                }
                data = result_peg_383.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                    result_peg_383.setValue(value);
                }
            
            
        }
        stream.update(result_peg_383.getPosition());
        
        return result_peg_383;
        out_peg_401:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_404 = stream.getColumn(position);
    if (column_peg_404.chunk1 != 0 && column_peg_404.chunk1->chunk_section_title.calculated()){
        return column_peg_404.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_433(stream, "section_title");
    int myposition = position;
    
    
    Value name;
    Result result_peg_405(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_405.getPosition()))){
                        result_peg_405.nextPosition();
                    } else {
                        goto out_peg_407;
                    }
                }
                result_peg_405.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_405.reset();
                    do{
                        Result result_peg_419(result_peg_405.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_419.getPosition()))){
                                    result_peg_419.nextPosition();
                                } else {
                                    goto out_peg_422;
                                }
                            }
                            result_peg_419.setValue((void*) " ");
                                
                        }
                        goto success_peg_420;
                        out_peg_422:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_419.getPosition()))){
                                    result_peg_419.nextPosition();
                                } else {
                                    goto out_peg_424;
                                }
                            }
                            result_peg_419.setValue((void*) "\t");
                                
                        }
                        goto success_peg_420;
                        out_peg_424:
                        goto loop_peg_418;
                        success_peg_420:
                        ;
                        result_peg_405.addResult(result_peg_419);
                    } while (true);
                    loop_peg_418:
                    ;
                            
                }
                goto success_peg_409;
                goto out_peg_407;
                success_peg_409:
                ;
            
            
            
            result_peg_405.reset();
                do{
                    Result result_peg_427(result_peg_405.getPosition());
                    {
                    
                        Result result_peg_430(result_peg_427);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("]"[i], stream.get(result_peg_430.getPosition()))){
                                    result_peg_430.nextPosition();
                                } else {
                                    goto not_peg_429;
                                }
                            }
                            result_peg_430.setValue((void*) "]");
                            goto loop_peg_426;
                            not_peg_429:
                            result_peg_427.setValue((void*)0);
                        
                        
                        
                        char temp_peg_431 = stream.get(result_peg_427.getPosition());
                            if (temp_peg_431 != '\0'){
                                result_peg_427.setValue((void*) (long) temp_peg_431);
                                result_peg_427.nextPosition();
                            } else {
                                goto loop_peg_426;
                            }
                        
                        
                    }
                    result_peg_405.addResult(result_peg_427);
                } while (true);
                loop_peg_426:
                if (result_peg_405.matches() == 0){
                    goto out_peg_407;
                }
                name = result_peg_405.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_405.getPosition()))){
                        result_peg_405.nextPosition();
                    } else {
                        goto out_peg_407;
                    }
                }
                result_peg_405.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = toString(name);
                    result_peg_405.setValue(value);
                }
            
            
        }
        
        if (column_peg_404.chunk1 == 0){
            column_peg_404.chunk1 = new Chunk1();
        }
        column_peg_404.chunk1->chunk_section_title = result_peg_405;
        stream.update(result_peg_405.getPosition());
        
        
        return result_peg_405;
        out_peg_407:
    
        if (column_peg_404.chunk1 == 0){
            column_peg_404.chunk1 = new Chunk1();
        }
        column_peg_404.chunk1->chunk_section_title = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_434 = stream.getColumn(position);
    if (column_peg_434.chunk1 != 0 && column_peg_434.chunk1->chunk_assignment.calculated()){
        return column_peg_434.chunk1->chunk_assignment;
    }
    
    RuleTrace trace_peg_760(stream, "assignment");
    int myposition = position;
    
    
    Value all;
        Value line;
        Value own;
        Value exp;
        Value name;
        Value index;
    Result result_peg_435(myposition);
        
        {
        
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("command"[i], stream.get(result_peg_435.getPosition()))){
                        result_peg_435.nextPosition();
                    } else {
                        goto out_peg_437;
                    }
                }
                result_peg_435.setValue((void*) "command");
            
            
            
            {
                    
                    result_peg_435.reset();
                    do{
                        Result result_peg_449(result_peg_435.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_449.getPosition()))){
                                    result_peg_449.nextPosition();
                                } else {
                                    goto out_peg_452;
                                }
                            }
                            result_peg_449.setValue((void*) " ");
                                
                        }
                        goto success_peg_450;
                        out_peg_452:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_449.getPosition()))){
                                    result_peg_449.nextPosition();
                                } else {
                                    goto out_peg_454;
                                }
                            }
                            result_peg_449.setValue((void*) "\t");
                                
                        }
                        goto success_peg_450;
                        out_peg_454:
                        goto loop_peg_448;
                        success_peg_450:
                        ;
                        result_peg_435.addResult(result_peg_449);
                    } while (true);
                    loop_peg_448:
                    ;
                            
                }
                goto success_peg_439;
                goto out_peg_437;
                success_peg_439:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_435.getPosition()))){
                        result_peg_435.nextPosition();
                    } else {
                        goto out_peg_437;
                    }
                }
                result_peg_435.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_435.reset();
                    do{
                        Result result_peg_467(result_peg_435.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_467.getPosition()))){
                                    result_peg_467.nextPosition();
                                } else {
                                    goto out_peg_470;
                                }
                            }
                            result_peg_467.setValue((void*) " ");
                                
                        }
                        goto success_peg_468;
                        out_peg_470:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_467.getPosition()))){
                                    result_peg_467.nextPosition();
                                } else {
                                    goto out_peg_472;
                                }
                            }
                            result_peg_467.setValue((void*) "\t");
                                
                        }
                        goto success_peg_468;
                        out_peg_472:
                        goto loop_peg_466;
                        success_peg_468:
                        ;
                        result_peg_435.addResult(result_peg_467);
                    } while (true);
                    loop_peg_466:
                    ;
                            
                }
                goto success_peg_457;
                goto out_peg_437;
                success_peg_457:
                ;
            
            
            
            result_peg_435 = rule_keys(stream, result_peg_435.getPosition());
                if (result_peg_435.error()){
                    goto out_peg_437;
                }
                all = result_peg_435.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("command", all);
                    result_peg_435.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_435;
        stream.update(result_peg_435.getPosition());
        
        
        return result_peg_435;
        out_peg_437:
        Result result_peg_474(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_476 = stream.getLineInfo(result_peg_474.getPosition());
                line = &line_info_peg_476;
            
            
            
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("guardsound"[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        goto out_peg_478;
                    }
                }
                result_peg_474.setValue((void*) "guardsound");
            
            
            
            {
                    
                    result_peg_474.reset();
                    do{
                        Result result_peg_490(result_peg_474.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_490.getPosition()))){
                                    result_peg_490.nextPosition();
                                } else {
                                    goto out_peg_493;
                                }
                            }
                            result_peg_490.setValue((void*) " ");
                                
                        }
                        goto success_peg_491;
                        out_peg_493:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_490.getPosition()))){
                                    result_peg_490.nextPosition();
                                } else {
                                    goto out_peg_495;
                                }
                            }
                            result_peg_490.setValue((void*) "\t");
                                
                        }
                        goto success_peg_491;
                        out_peg_495:
                        goto loop_peg_489;
                        success_peg_491:
                        ;
                        result_peg_474.addResult(result_peg_490);
                    } while (true);
                    loop_peg_489:
                    ;
                            
                }
                goto success_peg_480;
                goto out_peg_478;
                success_peg_480:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        goto out_peg_478;
                    }
                }
                result_peg_474.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_474.reset();
                    do{
                        Result result_peg_508(result_peg_474.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_508.getPosition()))){
                                    result_peg_508.nextPosition();
                                } else {
                                    goto out_peg_511;
                                }
                            }
                            result_peg_508.setValue((void*) " ");
                                
                        }
                        goto success_peg_509;
                        out_peg_511:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_508.getPosition()))){
                                    result_peg_508.nextPosition();
                                } else {
                                    goto out_peg_513;
                                }
                            }
                            result_peg_508.setValue((void*) "\t");
                                
                        }
                        goto success_peg_509;
                        out_peg_513:
                        goto loop_peg_507;
                        success_peg_509:
                        ;
                        result_peg_474.addResult(result_peg_508);
                    } while (true);
                    loop_peg_507:
                    ;
                            
                }
                goto success_peg_498;
                goto out_peg_478;
                success_peg_498:
                ;
            
            
            
            int save_peg_515 = result_peg_474.getPosition();
                
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_474.getPosition()))){
                        result_peg_474.nextPosition();
                    } else {
                        
                        result_peg_474 = Result(save_peg_515);
                        result_peg_474.setValue((void*) 0);
                        
                    }
                }
                result_peg_474.setValue((void*) "s");
                own = result_peg_474.getValues();
            
            
            
            result_peg_474 = rule_expr(stream, result_peg_474.getPosition());
                if (result_peg_474.error()){
                    goto out_peg_478;
                }
                exp = result_peg_474.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    if (own.getValue() != 0){
                                            value = makeAttribute("guardsound-own", exp);
                                        } else {
                                            value = makeAttribute("guardsound", exp);
                                        }
                    result_peg_474.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_474;
        stream.update(result_peg_474.getPosition());
        
        
        return result_peg_474;
        out_peg_478:
        Result result_peg_517(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_519 = stream.getLineInfo(result_peg_517.getPosition());
                line = &line_info_peg_519;
            
            
            
            for (int i = 0; i < 7; i++){
                    if (compareCharCase("sparkno"[i], stream.get(result_peg_517.getPosition()))){
                        result_peg_517.nextPosition();
                    } else {
                        goto out_peg_521;
                    }
                }
                result_peg_517.setValue((void*) "sparkno");
            
            
            
            {
                    
                    result_peg_517.reset();
                    do{
                        Result result_peg_533(result_peg_517.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_533.getPosition()))){
                                    result_peg_533.nextPosition();
                                } else {
                                    goto out_peg_536;
                                }
                            }
                            result_peg_533.setValue((void*) " ");
                                
                        }
                        goto success_peg_534;
                        out_peg_536:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_533.getPosition()))){
                                    result_peg_533.nextPosition();
                                } else {
                                    goto out_peg_538;
                                }
                            }
                            result_peg_533.setValue((void*) "\t");
                                
                        }
                        goto success_peg_534;
                        out_peg_538:
                        goto loop_peg_532;
                        success_peg_534:
                        ;
                        result_peg_517.addResult(result_peg_533);
                    } while (true);
                    loop_peg_532:
                    ;
                            
                }
                goto success_peg_523;
                goto out_peg_521;
                success_peg_523:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_517.getPosition()))){
                        result_peg_517.nextPosition();
                    } else {
                        goto out_peg_521;
                    }
                }
                result_peg_517.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_517.reset();
                    do{
                        Result result_peg_551(result_peg_517.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_551.getPosition()))){
                                    result_peg_551.nextPosition();
                                } else {
                                    goto out_peg_554;
                                }
                            }
                            result_peg_551.setValue((void*) " ");
                                
                        }
                        goto success_peg_552;
                        out_peg_554:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_551.getPosition()))){
                                    result_peg_551.nextPosition();
                                } else {
                                    goto out_peg_556;
                                }
                            }
                            result_peg_551.setValue((void*) "\t");
                                
                        }
                        goto success_peg_552;
                        out_peg_556:
                        goto loop_peg_550;
                        success_peg_552:
                        ;
                        result_peg_517.addResult(result_peg_551);
                    } while (true);
                    loop_peg_550:
                    ;
                            
                }
                goto success_peg_541;
                goto out_peg_521;
                success_peg_541:
                ;
            
            
            
            int save_peg_558 = result_peg_517.getPosition();
                
                for (int i = 0; i < 1; i++){
                    if (compareCharCase("s"[i], stream.get(result_peg_517.getPosition()))){
                        result_peg_517.nextPosition();
                    } else {
                        
                        result_peg_517 = Result(save_peg_558);
                        result_peg_517.setValue((void*) 0);
                        
                    }
                }
                result_peg_517.setValue((void*) "s");
            
            
            
            result_peg_517 = rule_expr(stream, result_peg_517.getPosition());
                if (result_peg_517.error()){
                    goto out_peg_521;
                }
                exp = result_peg_517.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute("sparkno", exp);
                    result_peg_517.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_517;
        stream.update(result_peg_517.getPosition());
        
        
        return result_peg_517;
        out_peg_521:
        Result result_peg_560(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_562 = stream.getLineInfo(result_peg_560.getPosition());
                line = &line_info_peg_562;
            
            
            
            result_peg_560 = rule_identifier(stream, result_peg_560.getPosition());
                if (result_peg_560.error()){
                    goto out_peg_564;
                }
                name = result_peg_560.getValues();
            
            
            
            {
                    
                    result_peg_560.reset();
                    do{
                        Result result_peg_576(result_peg_560.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_576.getPosition()))){
                                    result_peg_576.nextPosition();
                                } else {
                                    goto out_peg_579;
                                }
                            }
                            result_peg_576.setValue((void*) " ");
                                
                        }
                        goto success_peg_577;
                        out_peg_579:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_576.getPosition()))){
                                    result_peg_576.nextPosition();
                                } else {
                                    goto out_peg_581;
                                }
                            }
                            result_peg_576.setValue((void*) "\t");
                                
                        }
                        goto success_peg_577;
                        out_peg_581:
                        goto loop_peg_575;
                        success_peg_577:
                        ;
                        result_peg_560.addResult(result_peg_576);
                    } while (true);
                    loop_peg_575:
                    ;
                            
                }
                goto success_peg_566;
                goto out_peg_564;
                success_peg_566:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_560.getPosition()))){
                        result_peg_560.nextPosition();
                    } else {
                        goto out_peg_564;
                    }
                }
                result_peg_560.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_560.reset();
                    do{
                        Result result_peg_594(result_peg_560.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_594.getPosition()))){
                                    result_peg_594.nextPosition();
                                } else {
                                    goto out_peg_597;
                                }
                            }
                            result_peg_594.setValue((void*) " ");
                                
                        }
                        goto success_peg_595;
                        out_peg_597:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_594.getPosition()))){
                                    result_peg_594.nextPosition();
                                } else {
                                    goto out_peg_599;
                                }
                            }
                            result_peg_594.setValue((void*) "\t");
                                
                        }
                        goto success_peg_595;
                        out_peg_599:
                        goto loop_peg_593;
                        success_peg_595:
                        ;
                        result_peg_560.addResult(result_peg_594);
                    } while (true);
                    loop_peg_593:
                    ;
                            
                }
                goto success_peg_584;
                goto out_peg_564;
                success_peg_584:
                ;
            
            
            
            result_peg_560 = rule_expr(stream, result_peg_560.getPosition());
                if (result_peg_560.error()){
                    goto out_peg_564;
                }
                exp = result_peg_560.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                    result_peg_560.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_560;
        stream.update(result_peg_560.getPosition());
        
        
        return result_peg_560;
        out_peg_564:
        Result result_peg_601(myposition);
        
        {
        
            result_peg_601 = rule_identifier(stream, result_peg_601.getPosition());
                if (result_peg_601.error()){
                    goto out_peg_603;
                }
                name = result_peg_601.getValues();
            
            
            
            {
                    
                    result_peg_601.reset();
                    do{
                        Result result_peg_615(result_peg_601.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_615.getPosition()))){
                                    result_peg_615.nextPosition();
                                } else {
                                    goto out_peg_618;
                                }
                            }
                            result_peg_615.setValue((void*) " ");
                                
                        }
                        goto success_peg_616;
                        out_peg_618:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_615.getPosition()))){
                                    result_peg_615.nextPosition();
                                } else {
                                    goto out_peg_620;
                                }
                            }
                            result_peg_615.setValue((void*) "\t");
                                
                        }
                        goto success_peg_616;
                        out_peg_620:
                        goto loop_peg_614;
                        success_peg_616:
                        ;
                        result_peg_601.addResult(result_peg_615);
                    } while (true);
                    loop_peg_614:
                    ;
                            
                }
                goto success_peg_605;
                goto out_peg_603;
                success_peg_605:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_601.getPosition()))){
                        result_peg_601.nextPosition();
                    } else {
                        goto out_peg_603;
                    }
                }
                result_peg_601.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_601.reset();
                    do{
                        Result result_peg_633(result_peg_601.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_633.getPosition()))){
                                    result_peg_633.nextPosition();
                                } else {
                                    goto out_peg_636;
                                }
                            }
                            result_peg_633.setValue((void*) " ");
                                
                        }
                        goto success_peg_634;
                        out_peg_636:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_633.getPosition()))){
                                    result_peg_633.nextPosition();
                                } else {
                                    goto out_peg_638;
                                }
                            }
                            result_peg_633.setValue((void*) "\t");
                                
                        }
                        goto success_peg_634;
                        out_peg_638:
                        goto loop_peg_632;
                        success_peg_634:
                        ;
                        result_peg_601.addResult(result_peg_633);
                    } while (true);
                    loop_peg_632:
                    ;
                            
                }
                goto success_peg_623;
                goto out_peg_603;
                success_peg_623:
                ;
            
            
            
            result_peg_601 = rule_expr(stream, result_peg_601.getPosition());
                if (result_peg_601.error()){
                    goto out_peg_603;
                }
                exp = result_peg_601.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name, negateExpression(exp));
                    result_peg_601.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_601;
        stream.update(result_peg_601.getPosition());
        
        
        return result_peg_601;
        out_peg_603:
        Result result_peg_640(myposition);
        
        {
        
            result_peg_640 = rule_identifier(stream, result_peg_640.getPosition());
                if (result_peg_640.error()){
                    goto out_peg_642;
                }
                name = result_peg_640.getValues();
            
            
            
            {
                    
                    result_peg_640.reset();
                    do{
                        Result result_peg_654(result_peg_640.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_654.getPosition()))){
                                    result_peg_654.nextPosition();
                                } else {
                                    goto out_peg_657;
                                }
                            }
                            result_peg_654.setValue((void*) " ");
                                
                        }
                        goto success_peg_655;
                        out_peg_657:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_654.getPosition()))){
                                    result_peg_654.nextPosition();
                                } else {
                                    goto out_peg_659;
                                }
                            }
                            result_peg_654.setValue((void*) "\t");
                                
                        }
                        goto success_peg_655;
                        out_peg_659:
                        goto loop_peg_653;
                        success_peg_655:
                        ;
                        result_peg_640.addResult(result_peg_654);
                    } while (true);
                    loop_peg_653:
                    ;
                            
                }
                goto success_peg_644;
                goto out_peg_642;
                success_peg_644:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_640.getPosition()))){
                        result_peg_640.nextPosition();
                    } else {
                        goto out_peg_642;
                    }
                }
                result_peg_640.setValue((void*) "=");
            
            
            
            result_peg_640 = rule_whitespace(stream, result_peg_640.getPosition());
                if (result_peg_640.error()){
                    goto out_peg_642;
                }
            
            
            
            Result result_peg_663(result_peg_640.getPosition());
                result_peg_663 = rule_line_end(stream, result_peg_663.getPosition());
                if (result_peg_663.error()){
                    goto out_peg_642;
                }
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(name);
                    result_peg_640.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_640;
        stream.update(result_peg_640.getPosition());
        
        
        return result_peg_640;
        out_peg_642:
        Result result_peg_664(myposition);
        
        {
        
            result_peg_664 = rule_identifier(stream, result_peg_664.getPosition());
                if (result_peg_664.error()){
                    goto out_peg_666;
                }
                name = result_peg_664.getValues();
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_678(result_peg_664.getPosition());
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
                        goto loop_peg_677;
                        success_peg_679:
                        ;
                        result_peg_664.addResult(result_peg_678);
                    } while (true);
                    loop_peg_677:
                    ;
                            
                }
                goto success_peg_668;
                goto out_peg_666;
                success_peg_668:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_664.getPosition()))){
                        result_peg_664.nextPosition();
                    } else {
                        goto out_peg_666;
                    }
                }
                result_peg_664.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_696(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_696.getPosition()))){
                                    result_peg_696.nextPosition();
                                } else {
                                    goto out_peg_699;
                                }
                            }
                            result_peg_696.setValue((void*) " ");
                                
                        }
                        goto success_peg_697;
                        out_peg_699:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_696.getPosition()))){
                                    result_peg_696.nextPosition();
                                } else {
                                    goto out_peg_701;
                                }
                            }
                            result_peg_696.setValue((void*) "\t");
                                
                        }
                        goto success_peg_697;
                        out_peg_701:
                        goto loop_peg_695;
                        success_peg_697:
                        ;
                        result_peg_664.addResult(result_peg_696);
                    } while (true);
                    loop_peg_695:
                    ;
                            
                }
                goto success_peg_686;
                goto out_peg_666;
                success_peg_686:
                ;
            
            
            
            result_peg_664 = rule_integer(stream, result_peg_664.getPosition());
                if (result_peg_664.error()){
                    goto out_peg_666;
                }
                index = result_peg_664.getValues();
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_714(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_714.getPosition()))){
                                    result_peg_714.nextPosition();
                                } else {
                                    goto out_peg_717;
                                }
                            }
                            result_peg_714.setValue((void*) " ");
                                
                        }
                        goto success_peg_715;
                        out_peg_717:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_714.getPosition()))){
                                    result_peg_714.nextPosition();
                                } else {
                                    goto out_peg_719;
                                }
                            }
                            result_peg_714.setValue((void*) "\t");
                                
                        }
                        goto success_peg_715;
                        out_peg_719:
                        goto loop_peg_713;
                        success_peg_715:
                        ;
                        result_peg_664.addResult(result_peg_714);
                    } while (true);
                    loop_peg_713:
                    ;
                            
                }
                goto success_peg_704;
                goto out_peg_666;
                success_peg_704:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_664.getPosition()))){
                        result_peg_664.nextPosition();
                    } else {
                        goto out_peg_666;
                    }
                }
                result_peg_664.setValue((void*) ")");
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_732(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_732.getPosition()))){
                                    result_peg_732.nextPosition();
                                } else {
                                    goto out_peg_735;
                                }
                            }
                            result_peg_732.setValue((void*) " ");
                                
                        }
                        goto success_peg_733;
                        out_peg_735:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_732.getPosition()))){
                                    result_peg_732.nextPosition();
                                } else {
                                    goto out_peg_737;
                                }
                            }
                            result_peg_732.setValue((void*) "\t");
                                
                        }
                        goto success_peg_733;
                        out_peg_737:
                        goto loop_peg_731;
                        success_peg_733:
                        ;
                        result_peg_664.addResult(result_peg_732);
                    } while (true);
                    loop_peg_731:
                    ;
                            
                }
                goto success_peg_722;
                goto out_peg_666;
                success_peg_722:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_664.getPosition()))){
                        result_peg_664.nextPosition();
                    } else {
                        goto out_peg_666;
                    }
                }
                result_peg_664.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_664.reset();
                    do{
                        Result result_peg_750(result_peg_664.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_750.getPosition()))){
                                    result_peg_750.nextPosition();
                                } else {
                                    goto out_peg_753;
                                }
                            }
                            result_peg_750.setValue((void*) " ");
                                
                        }
                        goto success_peg_751;
                        out_peg_753:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_750.getPosition()))){
                                    result_peg_750.nextPosition();
                                } else {
                                    goto out_peg_755;
                                }
                            }
                            result_peg_750.setValue((void*) "\t");
                                
                        }
                        goto success_peg_751;
                        out_peg_755:
                        goto loop_peg_749;
                        success_peg_751:
                        ;
                        result_peg_664.addResult(result_peg_750);
                    } while (true);
                    loop_peg_749:
                    ;
                            
                }
                goto success_peg_740;
                goto out_peg_666;
                success_peg_740:
                ;
            
            
            
            result_peg_664 = rule_expr(stream, result_peg_664.getPosition());
                if (result_peg_664.error()){
                    goto out_peg_666;
                }
                exp = result_peg_664.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIndexedAttribute(name, index, exp);
                    result_peg_664.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_664;
        stream.update(result_peg_664.getPosition());
        
        
        return result_peg_664;
        out_peg_666:
        Result result_peg_757(myposition);
        
        {
        
            for (int i = 0; i < 4; i++){
                    if (compareCharCase("ctrl"[i], stream.get(result_peg_757.getPosition()))){
                        result_peg_757.nextPosition();
                    } else {
                        goto out_peg_759;
                    }
                }
                result_peg_757.setValue((void*) "ctrl");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAttribute(makeSimpleIdentifier("ctrl"), makeNumber(1));
                    result_peg_757.setValue(value);
                }
            
            
        }
        
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = result_peg_757;
        stream.update(result_peg_757.getPosition());
        
        
        return result_peg_757;
        out_peg_759:
    
        if (column_peg_434.chunk1 == 0){
            column_peg_434.chunk1 = new Chunk1();
        }
        column_peg_434.chunk1->chunk_assignment = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_761 = stream.getColumn(position);
    if (column_peg_761.chunk2 != 0 && column_peg_761.chunk2->chunk_identifier.calculated()){
        return column_peg_761.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_771(stream, "identifier");
    int myposition = position;
    
    
    Value line;
        Value first;
        Value rest;
    Result result_peg_762(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_764 = stream.getLineInfo(result_peg_762.getPosition());
                line = &line_info_peg_764;
            
            
            
            result_peg_762 = rule_name(stream, result_peg_762.getPosition());
                if (result_peg_762.error()){
                    goto out_peg_766;
                }
                first = result_peg_762.getValues();
            
            
            
            result_peg_762.reset();
                do{
                    Result result_peg_769(result_peg_762.getPosition());
                    {
                    
                        for (int i = 0; i < 1; i++){
                                if (compareChar("."[i], stream.get(result_peg_769.getPosition()))){
                                    result_peg_769.nextPosition();
                                } else {
                                    goto loop_peg_768;
                                }
                            }
                            result_peg_769.setValue((void*) ".");
                        
                        
                        
                        result_peg_769 = rule_name(stream, result_peg_769.getPosition());
                            if (result_peg_769.error()){
                                goto loop_peg_768;
                            }
                        
                        
                    }
                    result_peg_762.addResult(result_peg_769);
                } while (true);
                loop_peg_768:
                ;
                rest = result_peg_762.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                    result_peg_762.setValue(value);
                }
            
            
        }
        
        if (column_peg_761.chunk2 == 0){
            column_peg_761.chunk2 = new Chunk2();
        }
        column_peg_761.chunk2->chunk_identifier = result_peg_762;
        stream.update(result_peg_762.getPosition());
        
        
        return result_peg_762;
        out_peg_766:
    
        if (column_peg_761.chunk2 == 0){
            column_peg_761.chunk2 = new Chunk2();
        }
        column_peg_761.chunk2->chunk_identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_772 = stream.getColumn(position);
    if (column_peg_772.chunk2 != 0 && column_peg_772.chunk2->chunk_integer.calculated()){
        return column_peg_772.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_790(stream, "integer");
    int myposition = position;
    
    
    
    Result result_peg_773(myposition);
        
        {
        
            int save_peg_775 = result_peg_773.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_773.getPosition()))){
                            result_peg_773.nextPosition();
                        } else {
                            goto out_peg_778;
                        }
                    }
                    result_peg_773.setValue((void*) "-");
                        
                }
                goto success_peg_776;
                out_peg_778:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_773.getPosition()))){
                            result_peg_773.nextPosition();
                        } else {
                            goto out_peg_780;
                        }
                    }
                    result_peg_773.setValue((void*) "+");
                        
                }
                goto success_peg_776;
                out_peg_780:
                
                result_peg_773 = Result(save_peg_775);
                result_peg_773.setValue((void*) 0);
                
                success_peg_776:
                ;
            
            Result result_peg_774 = result_peg_773;
            
            result_peg_773.reset();
                do{
                    Result result_peg_783(result_peg_773.getPosition());
                    {
                        
                        char letter_peg_788 = stream.get(result_peg_783.getPosition());
                        if (letter_peg_788 != '\0' && strchr("0123456789", letter_peg_788) != NULL){
                            result_peg_783.nextPosition();
                            result_peg_783.setValue((void*) (long) letter_peg_788);
                        } else {
                            goto out_peg_787;
                        }
                        
                    }
                    goto success_peg_784;
                    out_peg_787:
                    goto loop_peg_782;
                    success_peg_784:
                    ;
                    result_peg_773.addResult(result_peg_783);
                } while (true);
                loop_peg_782:
                if (result_peg_773.matches() == 0){
                    goto out_peg_789;
                }
            
            Result result_peg_781 = result_peg_773;
            
            {
                    Value value((void*) 0);
                    value = makeInteger(result_peg_774.getValues(), result_peg_781.getValues());
                    result_peg_773.setValue(value);
                }
            
            
        }
        
        if (column_peg_772.chunk2 == 0){
            column_peg_772.chunk2 = new Chunk2();
        }
        column_peg_772.chunk2->chunk_integer = result_peg_773;
        stream.update(result_peg_773.getPosition());
        
        
        return result_peg_773;
        out_peg_789:
    
        if (column_peg_772.chunk2 == 0){
            column_peg_772.chunk2 = new Chunk2();
        }
        column_peg_772.chunk2->chunk_integer = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_791 = stream.getColumn(position);
    if (column_peg_791.chunk2 != 0 && column_peg_791.chunk2->chunk_float.calculated()){
        return column_peg_791.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_836(stream, "float");
    int myposition = position;
    
    
    Value left;
        Value right;
    Result result_peg_792(myposition);
        
        {
        
            int save_peg_794 = result_peg_792.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_792.getPosition()))){
                            result_peg_792.nextPosition();
                        } else {
                            goto out_peg_797;
                        }
                    }
                    result_peg_792.setValue((void*) "-");
                        
                }
                goto success_peg_795;
                out_peg_797:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_792.getPosition()))){
                            result_peg_792.nextPosition();
                        } else {
                            goto out_peg_799;
                        }
                    }
                    result_peg_792.setValue((void*) "+");
                        
                }
                goto success_peg_795;
                out_peg_799:
                
                result_peg_792 = Result(save_peg_794);
                result_peg_792.setValue((void*) 0);
                
                success_peg_795:
                ;
            
            Result result_peg_793 = result_peg_792;
            
            result_peg_792.reset();
                do{
                    Result result_peg_802(result_peg_792.getPosition());
                    {
                        
                        char letter_peg_807 = stream.get(result_peg_802.getPosition());
                        if (letter_peg_807 != '\0' && strchr("0123456789", letter_peg_807) != NULL){
                            result_peg_802.nextPosition();
                            result_peg_802.setValue((void*) (long) letter_peg_807);
                        } else {
                            goto out_peg_806;
                        }
                        
                    }
                    goto success_peg_803;
                    out_peg_806:
                    goto loop_peg_801;
                    success_peg_803:
                    ;
                    result_peg_792.addResult(result_peg_802);
                } while (true);
                loop_peg_801:
                ;
                left = result_peg_792.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_792.getPosition()))){
                        result_peg_792.nextPosition();
                    } else {
                        goto out_peg_809;
                    }
                }
                result_peg_792.setValue((void*) ".");
            
            
            
            result_peg_792.reset();
                do{
                    Result result_peg_812(result_peg_792.getPosition());
                    {
                        
                        char letter_peg_817 = stream.get(result_peg_812.getPosition());
                        if (letter_peg_817 != '\0' && strchr("0123456789", letter_peg_817) != NULL){
                            result_peg_812.nextPosition();
                            result_peg_812.setValue((void*) (long) letter_peg_817);
                        } else {
                            goto out_peg_816;
                        }
                        
                    }
                    goto success_peg_813;
                    out_peg_816:
                    goto loop_peg_811;
                    success_peg_813:
                    ;
                    result_peg_792.addResult(result_peg_812);
                } while (true);
                loop_peg_811:
                if (result_peg_792.matches() == 0){
                    goto out_peg_809;
                }
                right = result_peg_792.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_793.getValues(), parseDouble(left,right));
                    result_peg_792.setValue(value);
                }
            
            
        }
        
        if (column_peg_791.chunk2 == 0){
            column_peg_791.chunk2 = new Chunk2();
        }
        column_peg_791.chunk2->chunk_float = result_peg_792;
        stream.update(result_peg_792.getPosition());
        
        
        return result_peg_792;
        out_peg_809:
        Result result_peg_818(myposition);
        
        {
        
            int save_peg_820 = result_peg_818.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_818.getPosition()))){
                            result_peg_818.nextPosition();
                        } else {
                            goto out_peg_823;
                        }
                    }
                    result_peg_818.setValue((void*) "-");
                        
                }
                goto success_peg_821;
                out_peg_823:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_818.getPosition()))){
                            result_peg_818.nextPosition();
                        } else {
                            goto out_peg_825;
                        }
                    }
                    result_peg_818.setValue((void*) "+");
                        
                }
                goto success_peg_821;
                out_peg_825:
                
                result_peg_818 = Result(save_peg_820);
                result_peg_818.setValue((void*) 0);
                
                success_peg_821:
                ;
            
            Result result_peg_819 = result_peg_818;
            
            result_peg_818.reset();
                do{
                    Result result_peg_828(result_peg_818.getPosition());
                    {
                        
                        char letter_peg_833 = stream.get(result_peg_828.getPosition());
                        if (letter_peg_833 != '\0' && strchr("0123456789", letter_peg_833) != NULL){
                            result_peg_828.nextPosition();
                            result_peg_828.setValue((void*) (long) letter_peg_833);
                        } else {
                            goto out_peg_832;
                        }
                        
                    }
                    goto success_peg_829;
                    out_peg_832:
                    goto loop_peg_827;
                    success_peg_829:
                    ;
                    result_peg_818.addResult(result_peg_828);
                } while (true);
                loop_peg_827:
                if (result_peg_818.matches() == 0){
                    goto out_peg_834;
                }
                left = result_peg_818.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_818.getPosition()))){
                        result_peg_818.nextPosition();
                    } else {
                        goto out_peg_834;
                    }
                }
                result_peg_818.setValue((void*) ".");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeNumber(result_peg_819.getValues(), parseDouble(left));
                    result_peg_818.setValue(value);
                }
            
            
        }
        
        if (column_peg_791.chunk2 == 0){
            column_peg_791.chunk2 = new Chunk2();
        }
        column_peg_791.chunk2->chunk_float = result_peg_818;
        stream.update(result_peg_818.getPosition());
        
        
        return result_peg_818;
        out_peg_834:
    
        if (column_peg_791.chunk2 == 0){
            column_peg_791.chunk2 = new Chunk2();
        }
        column_peg_791.chunk2->chunk_float = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_837 = stream.getColumn(position);
    if (column_peg_837.chunk2 != 0 && column_peg_837.chunk2->chunk_string.calculated()){
        return column_peg_837.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_852(stream, "string");
    int myposition = position;
    
    
    Value contents;
    Result result_peg_838(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_838.getPosition()))){
                        result_peg_838.nextPosition();
                    } else {
                        goto out_peg_840;
                    }
                }
                result_peg_838.setValue((void*) "\"");
            
            
            
            result_peg_838.reset();
                do{
                    Result result_peg_843(result_peg_838.getPosition());
                    {
                    
                        Result result_peg_846(result_peg_843);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\""[i], stream.get(result_peg_846.getPosition()))){
                                    result_peg_846.nextPosition();
                                } else {
                                    goto not_peg_845;
                                }
                            }
                            result_peg_846.setValue((void*) "\"");
                            goto loop_peg_842;
                            not_peg_845:
                            result_peg_843.setValue((void*)0);
                        
                        
                        
                        Result result_peg_849(result_peg_843);
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\n"[i], stream.get(result_peg_849.getPosition()))){
                                    result_peg_849.nextPosition();
                                } else {
                                    goto not_peg_848;
                                }
                            }
                            result_peg_849.setValue((void*) "\n");
                            goto loop_peg_842;
                            not_peg_848:
                            result_peg_843.setValue((void*)0);
                        
                        
                        
                        char temp_peg_850 = stream.get(result_peg_843.getPosition());
                            if (temp_peg_850 != '\0'){
                                result_peg_843.setValue((void*) (long) temp_peg_850);
                                result_peg_843.nextPosition();
                            } else {
                                goto loop_peg_842;
                            }
                        
                        
                    }
                    result_peg_838.addResult(result_peg_843);
                } while (true);
                loop_peg_842:
                ;
                contents = result_peg_838.getValues();
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("\""[i], stream.get(result_peg_838.getPosition()))){
                        result_peg_838.nextPosition();
                    } else {
                        goto out_peg_840;
                    }
                }
                result_peg_838.setValue((void*) "\"");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeString(contents);
                    result_peg_838.setValue(value);
                }
            
            
        }
        
        if (column_peg_837.chunk2 == 0){
            column_peg_837.chunk2 = new Chunk2();
        }
        column_peg_837.chunk2->chunk_string = result_peg_838;
        stream.update(result_peg_838.getPosition());
        
        
        return result_peg_838;
        out_peg_840:
    
        if (column_peg_837.chunk2 == 0){
            column_peg_837.chunk2 = new Chunk2();
        }
        column_peg_837.chunk2->chunk_string = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_853 = stream.getColumn(position);
    if (column_peg_853.chunk2 != 0 && column_peg_853.chunk2->chunk_range.calculated()){
        return column_peg_853.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_1154(stream, "range");
    int myposition = position;
    
    
    Value low;
        Value high;
    Result result_peg_854(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_854.getPosition()))){
                        result_peg_854.nextPosition();
                    } else {
                        goto out_peg_856;
                    }
                }
                result_peg_854.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_854.reset();
                    do{
                        Result result_peg_868(result_peg_854.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_868.getPosition()))){
                                    result_peg_868.nextPosition();
                                } else {
                                    goto out_peg_871;
                                }
                            }
                            result_peg_868.setValue((void*) " ");
                                
                        }
                        goto success_peg_869;
                        out_peg_871:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_868.getPosition()))){
                                    result_peg_868.nextPosition();
                                } else {
                                    goto out_peg_873;
                                }
                            }
                            result_peg_868.setValue((void*) "\t");
                                
                        }
                        goto success_peg_869;
                        out_peg_873:
                        goto loop_peg_867;
                        success_peg_869:
                        ;
                        result_peg_854.addResult(result_peg_868);
                    } while (true);
                    loop_peg_867:
                    ;
                            
                }
                goto success_peg_858;
                goto out_peg_856;
                success_peg_858:
                ;
            
            
            
            result_peg_854 = rule_expr_c(stream, result_peg_854.getPosition());
                if (result_peg_854.error()){
                    goto out_peg_856;
                }
                low = result_peg_854.getValues();
            
            
            
            {
                    
                    result_peg_854.reset();
                    do{
                        Result result_peg_886(result_peg_854.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_886.getPosition()))){
                                    result_peg_886.nextPosition();
                                } else {
                                    goto out_peg_889;
                                }
                            }
                            result_peg_886.setValue((void*) " ");
                                
                        }
                        goto success_peg_887;
                        out_peg_889:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_886.getPosition()))){
                                    result_peg_886.nextPosition();
                                } else {
                                    goto out_peg_891;
                                }
                            }
                            result_peg_886.setValue((void*) "\t");
                                
                        }
                        goto success_peg_887;
                        out_peg_891:
                        goto loop_peg_885;
                        success_peg_887:
                        ;
                        result_peg_854.addResult(result_peg_886);
                    } while (true);
                    loop_peg_885:
                    ;
                            
                }
                goto success_peg_876;
                goto out_peg_856;
                success_peg_876:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_854.getPosition()))){
                        result_peg_854.nextPosition();
                    } else {
                        goto out_peg_856;
                    }
                }
                result_peg_854.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_854.reset();
                    do{
                        Result result_peg_904(result_peg_854.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_904.getPosition()))){
                                    result_peg_904.nextPosition();
                                } else {
                                    goto out_peg_907;
                                }
                            }
                            result_peg_904.setValue((void*) " ");
                                
                        }
                        goto success_peg_905;
                        out_peg_907:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_904.getPosition()))){
                                    result_peg_904.nextPosition();
                                } else {
                                    goto out_peg_909;
                                }
                            }
                            result_peg_904.setValue((void*) "\t");
                                
                        }
                        goto success_peg_905;
                        out_peg_909:
                        goto loop_peg_903;
                        success_peg_905:
                        ;
                        result_peg_854.addResult(result_peg_904);
                    } while (true);
                    loop_peg_903:
                    ;
                            
                }
                goto success_peg_894;
                goto out_peg_856;
                success_peg_894:
                ;
            
            
            
            result_peg_854 = rule_expr_c(stream, result_peg_854.getPosition());
                if (result_peg_854.error()){
                    goto out_peg_856;
                }
                high = result_peg_854.getValues();
            
            
            
            {
                    
                    result_peg_854.reset();
                    do{
                        Result result_peg_922(result_peg_854.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_922.getPosition()))){
                                    result_peg_922.nextPosition();
                                } else {
                                    goto out_peg_925;
                                }
                            }
                            result_peg_922.setValue((void*) " ");
                                
                        }
                        goto success_peg_923;
                        out_peg_925:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_922.getPosition()))){
                                    result_peg_922.nextPosition();
                                } else {
                                    goto out_peg_927;
                                }
                            }
                            result_peg_922.setValue((void*) "\t");
                                
                        }
                        goto success_peg_923;
                        out_peg_927:
                        goto loop_peg_921;
                        success_peg_923:
                        ;
                        result_peg_854.addResult(result_peg_922);
                    } while (true);
                    loop_peg_921:
                    ;
                            
                }
                goto success_peg_912;
                goto out_peg_856;
                success_peg_912:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_854.getPosition()))){
                        result_peg_854.nextPosition();
                    } else {
                        goto out_peg_856;
                    }
                }
                result_peg_854.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllInclusive, low, high);
                    result_peg_854.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_854;
        stream.update(result_peg_854.getPosition());
        
        
        return result_peg_854;
        out_peg_856:
        Result result_peg_929(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_929.getPosition()))){
                        result_peg_929.nextPosition();
                    } else {
                        goto out_peg_931;
                    }
                }
                result_peg_929.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_943(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_943.getPosition()))){
                                    result_peg_943.nextPosition();
                                } else {
                                    goto out_peg_946;
                                }
                            }
                            result_peg_943.setValue((void*) " ");
                                
                        }
                        goto success_peg_944;
                        out_peg_946:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_943.getPosition()))){
                                    result_peg_943.nextPosition();
                                } else {
                                    goto out_peg_948;
                                }
                            }
                            result_peg_943.setValue((void*) "\t");
                                
                        }
                        goto success_peg_944;
                        out_peg_948:
                        goto loop_peg_942;
                        success_peg_944:
                        ;
                        result_peg_929.addResult(result_peg_943);
                    } while (true);
                    loop_peg_942:
                    ;
                            
                }
                goto success_peg_933;
                goto out_peg_931;
                success_peg_933:
                ;
            
            
            
            result_peg_929 = rule_expr_c(stream, result_peg_929.getPosition());
                if (result_peg_929.error()){
                    goto out_peg_931;
                }
                low = result_peg_929.getValues();
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_961(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_961.getPosition()))){
                                    result_peg_961.nextPosition();
                                } else {
                                    goto out_peg_964;
                                }
                            }
                            result_peg_961.setValue((void*) " ");
                                
                        }
                        goto success_peg_962;
                        out_peg_964:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_961.getPosition()))){
                                    result_peg_961.nextPosition();
                                } else {
                                    goto out_peg_966;
                                }
                            }
                            result_peg_961.setValue((void*) "\t");
                                
                        }
                        goto success_peg_962;
                        out_peg_966:
                        goto loop_peg_960;
                        success_peg_962:
                        ;
                        result_peg_929.addResult(result_peg_961);
                    } while (true);
                    loop_peg_960:
                    ;
                            
                }
                goto success_peg_951;
                goto out_peg_931;
                success_peg_951:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_929.getPosition()))){
                        result_peg_929.nextPosition();
                    } else {
                        goto out_peg_931;
                    }
                }
                result_peg_929.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_979(result_peg_929.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_979.getPosition()))){
                                    result_peg_979.nextPosition();
                                } else {
                                    goto out_peg_982;
                                }
                            }
                            result_peg_979.setValue((void*) " ");
                                
                        }
                        goto success_peg_980;
                        out_peg_982:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_979.getPosition()))){
                                    result_peg_979.nextPosition();
                                } else {
                                    goto out_peg_984;
                                }
                            }
                            result_peg_979.setValue((void*) "\t");
                                
                        }
                        goto success_peg_980;
                        out_peg_984:
                        goto loop_peg_978;
                        success_peg_980:
                        ;
                        result_peg_929.addResult(result_peg_979);
                    } while (true);
                    loop_peg_978:
                    ;
                            
                }
                goto success_peg_969;
                goto out_peg_931;
                success_peg_969:
                ;
            
            
            
            result_peg_929 = rule_expr_c(stream, result_peg_929.getPosition());
                if (result_peg_929.error()){
                    goto out_peg_931;
                }
                high = result_peg_929.getValues();
            
            
            
            {
                    
                    result_peg_929.reset();
                    do{
                        Result result_peg_997(result_peg_929.getPosition());
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
                        result_peg_929.addResult(result_peg_997);
                    } while (true);
                    loop_peg_996:
                    ;
                            
                }
                goto success_peg_987;
                goto out_peg_931;
                success_peg_987:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_929.getPosition()))){
                        result_peg_929.nextPosition();
                    } else {
                        goto out_peg_931;
                    }
                }
                result_peg_929.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::AllExclusive, low, high);
                    result_peg_929.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_929;
        stream.update(result_peg_929.getPosition());
        
        
        return result_peg_929;
        out_peg_931:
        Result result_peg_1004(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_1004.getPosition()))){
                        result_peg_1004.nextPosition();
                    } else {
                        goto out_peg_1006;
                    }
                }
                result_peg_1004.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1018(result_peg_1004.getPosition());
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
                        goto loop_peg_1017;
                        success_peg_1019:
                        ;
                        result_peg_1004.addResult(result_peg_1018);
                    } while (true);
                    loop_peg_1017:
                    ;
                            
                }
                goto success_peg_1008;
                goto out_peg_1006;
                success_peg_1008:
                ;
            
            
            
            result_peg_1004 = rule_expr_c(stream, result_peg_1004.getPosition());
                if (result_peg_1004.error()){
                    goto out_peg_1006;
                }
                low = result_peg_1004.getValues();
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1036(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1036.getPosition()))){
                                    result_peg_1036.nextPosition();
                                } else {
                                    goto out_peg_1039;
                                }
                            }
                            result_peg_1036.setValue((void*) " ");
                                
                        }
                        goto success_peg_1037;
                        out_peg_1039:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1036.getPosition()))){
                                    result_peg_1036.nextPosition();
                                } else {
                                    goto out_peg_1041;
                                }
                            }
                            result_peg_1036.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1037;
                        out_peg_1041:
                        goto loop_peg_1035;
                        success_peg_1037:
                        ;
                        result_peg_1004.addResult(result_peg_1036);
                    } while (true);
                    loop_peg_1035:
                    ;
                            
                }
                goto success_peg_1026;
                goto out_peg_1006;
                success_peg_1026:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1004.getPosition()))){
                        result_peg_1004.nextPosition();
                    } else {
                        goto out_peg_1006;
                    }
                }
                result_peg_1004.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1054(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1054.getPosition()))){
                                    result_peg_1054.nextPosition();
                                } else {
                                    goto out_peg_1057;
                                }
                            }
                            result_peg_1054.setValue((void*) " ");
                                
                        }
                        goto success_peg_1055;
                        out_peg_1057:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1054.getPosition()))){
                                    result_peg_1054.nextPosition();
                                } else {
                                    goto out_peg_1059;
                                }
                            }
                            result_peg_1054.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1055;
                        out_peg_1059:
                        goto loop_peg_1053;
                        success_peg_1055:
                        ;
                        result_peg_1004.addResult(result_peg_1054);
                    } while (true);
                    loop_peg_1053:
                    ;
                            
                }
                goto success_peg_1044;
                goto out_peg_1006;
                success_peg_1044:
                ;
            
            
            
            result_peg_1004 = rule_expr_c(stream, result_peg_1004.getPosition());
                if (result_peg_1004.error()){
                    goto out_peg_1006;
                }
                high = result_peg_1004.getValues();
            
            
            
            {
                    
                    result_peg_1004.reset();
                    do{
                        Result result_peg_1072(result_peg_1004.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1072.getPosition()))){
                                    result_peg_1072.nextPosition();
                                } else {
                                    goto out_peg_1075;
                                }
                            }
                            result_peg_1072.setValue((void*) " ");
                                
                        }
                        goto success_peg_1073;
                        out_peg_1075:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1072.getPosition()))){
                                    result_peg_1072.nextPosition();
                                } else {
                                    goto out_peg_1077;
                                }
                            }
                            result_peg_1072.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1073;
                        out_peg_1077:
                        goto loop_peg_1071;
                        success_peg_1073:
                        ;
                        result_peg_1004.addResult(result_peg_1072);
                    } while (true);
                    loop_peg_1071:
                    ;
                            
                }
                goto success_peg_1062;
                goto out_peg_1006;
                success_peg_1062:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("]"[i], stream.get(result_peg_1004.getPosition()))){
                        result_peg_1004.nextPosition();
                    } else {
                        goto out_peg_1006;
                    }
                }
                result_peg_1004.setValue((void*) "]");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                    result_peg_1004.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_1004;
        stream.update(result_peg_1004.getPosition());
        
        
        return result_peg_1004;
        out_peg_1006:
        Result result_peg_1079(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("["[i], stream.get(result_peg_1079.getPosition()))){
                        result_peg_1079.nextPosition();
                    } else {
                        goto out_peg_1081;
                    }
                }
                result_peg_1079.setValue((void*) "[");
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1093(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1093.getPosition()))){
                                    result_peg_1093.nextPosition();
                                } else {
                                    goto out_peg_1096;
                                }
                            }
                            result_peg_1093.setValue((void*) " ");
                                
                        }
                        goto success_peg_1094;
                        out_peg_1096:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1093.getPosition()))){
                                    result_peg_1093.nextPosition();
                                } else {
                                    goto out_peg_1098;
                                }
                            }
                            result_peg_1093.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1094;
                        out_peg_1098:
                        goto loop_peg_1092;
                        success_peg_1094:
                        ;
                        result_peg_1079.addResult(result_peg_1093);
                    } while (true);
                    loop_peg_1092:
                    ;
                            
                }
                goto success_peg_1083;
                goto out_peg_1081;
                success_peg_1083:
                ;
            
            
            
            result_peg_1079 = rule_expr_c(stream, result_peg_1079.getPosition());
                if (result_peg_1079.error()){
                    goto out_peg_1081;
                }
                low = result_peg_1079.getValues();
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1111(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1111.getPosition()))){
                                    result_peg_1111.nextPosition();
                                } else {
                                    goto out_peg_1114;
                                }
                            }
                            result_peg_1111.setValue((void*) " ");
                                
                        }
                        goto success_peg_1112;
                        out_peg_1114:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1111.getPosition()))){
                                    result_peg_1111.nextPosition();
                                } else {
                                    goto out_peg_1116;
                                }
                            }
                            result_peg_1111.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1112;
                        out_peg_1116:
                        goto loop_peg_1110;
                        success_peg_1112:
                        ;
                        result_peg_1079.addResult(result_peg_1111);
                    } while (true);
                    loop_peg_1110:
                    ;
                            
                }
                goto success_peg_1101;
                goto out_peg_1081;
                success_peg_1101:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1079.getPosition()))){
                        result_peg_1079.nextPosition();
                    } else {
                        goto out_peg_1081;
                    }
                }
                result_peg_1079.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1129(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1129.getPosition()))){
                                    result_peg_1129.nextPosition();
                                } else {
                                    goto out_peg_1132;
                                }
                            }
                            result_peg_1129.setValue((void*) " ");
                                
                        }
                        goto success_peg_1130;
                        out_peg_1132:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1129.getPosition()))){
                                    result_peg_1129.nextPosition();
                                } else {
                                    goto out_peg_1134;
                                }
                            }
                            result_peg_1129.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1130;
                        out_peg_1134:
                        goto loop_peg_1128;
                        success_peg_1130:
                        ;
                        result_peg_1079.addResult(result_peg_1129);
                    } while (true);
                    loop_peg_1128:
                    ;
                            
                }
                goto success_peg_1119;
                goto out_peg_1081;
                success_peg_1119:
                ;
            
            
            
            result_peg_1079 = rule_expr_c(stream, result_peg_1079.getPosition());
                if (result_peg_1079.error()){
                    goto out_peg_1081;
                }
                high = result_peg_1079.getValues();
            
            
            
            {
                    
                    result_peg_1079.reset();
                    do{
                        Result result_peg_1147(result_peg_1079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1147.getPosition()))){
                                    result_peg_1147.nextPosition();
                                } else {
                                    goto out_peg_1150;
                                }
                            }
                            result_peg_1147.setValue((void*) " ");
                                
                        }
                        goto success_peg_1148;
                        out_peg_1150:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1147.getPosition()))){
                                    result_peg_1147.nextPosition();
                                } else {
                                    goto out_peg_1152;
                                }
                            }
                            result_peg_1147.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1148;
                        out_peg_1152:
                        goto loop_peg_1146;
                        success_peg_1148:
                        ;
                        result_peg_1079.addResult(result_peg_1147);
                    } while (true);
                    loop_peg_1146:
                    ;
                            
                }
                goto success_peg_1137;
                goto out_peg_1081;
                success_peg_1137:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_1079.getPosition()))){
                        result_peg_1079.nextPosition();
                    } else {
                        goto out_peg_1081;
                    }
                }
                result_peg_1079.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                    result_peg_1079.setValue(value);
                }
            
            
        }
        
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = result_peg_1079;
        stream.update(result_peg_1079.getPosition());
        
        
        return result_peg_1079;
        out_peg_1081:
    
        if (column_peg_853.chunk2 == 0){
            column_peg_853.chunk2 = new Chunk2();
        }
        column_peg_853.chunk2->chunk_range = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_1155 = stream.getColumn(position);
    if (column_peg_1155.chunk3 != 0 && column_peg_1155.chunk3->chunk_name.calculated()){
        return column_peg_1155.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_1167(stream, "name");
    int myposition = position;
    
    
    
    Result result_peg_1156(myposition);
        
        {
        
            {
                    
                    char letter_peg_1162 = stream.get(result_peg_1156.getPosition());
                    if (letter_peg_1162 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1162) != NULL){
                        result_peg_1156.nextPosition();
                        result_peg_1156.setValue((void*) (long) letter_peg_1162);
                    } else {
                        goto out_peg_1161;
                    }
                    
                }
                goto success_peg_1158;
                out_peg_1161:
                goto out_peg_1163;
                success_peg_1158:
                ;
            
            Result result_peg_1157 = result_peg_1156;
            
            result_peg_1156.reset();
                do{
                    Result result_peg_1166(result_peg_1156.getPosition());
                    result_peg_1166 = rule_alpha_digit(stream, result_peg_1166.getPosition());
                    if (result_peg_1166.error()){
                        goto loop_peg_1165;
                    }
                    result_peg_1156.addResult(result_peg_1166);
                } while (true);
                loop_peg_1165:
                ;
            
            Result result_peg_1164 = result_peg_1156;
            
            {
                    Value value((void*) 0);
                    value = toString((char)(long)result_peg_1157.getValues().getValue(),result_peg_1164.getValues());
                    result_peg_1156.setValue(value);
                }
            
            
        }
        
        if (column_peg_1155.chunk3 == 0){
            column_peg_1155.chunk3 = new Chunk3();
        }
        column_peg_1155.chunk3->chunk_name = result_peg_1156;
        stream.update(result_peg_1156.getPosition());
        
        
        return result_peg_1156;
        out_peg_1163:
    
        if (column_peg_1155.chunk3 == 0){
            column_peg_1155.chunk3 = new Chunk3();
        }
        column_peg_1155.chunk3->chunk_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_1168 = stream.getColumn(position);
    if (column_peg_1168.chunk3 != 0 && column_peg_1168.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_1168.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_1183(stream, "alpha_digit");
    int myposition = position;
    
    
    
    Result result_peg_1169(myposition);
        
        {
            
            char letter_peg_1174 = stream.get(result_peg_1169.getPosition());
            if (letter_peg_1174 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_1174) != NULL){
                result_peg_1169.nextPosition();
                result_peg_1169.setValue((void*) (long) letter_peg_1174);
            } else {
                goto out_peg_1173;
            }
            
        }
        goto success_peg_1170;
        out_peg_1173:
        goto out_peg_1175;
        success_peg_1170:
        ;
        
        if (column_peg_1168.chunk3 == 0){
            column_peg_1168.chunk3 = new Chunk3();
        }
        column_peg_1168.chunk3->chunk_alpha_digit = result_peg_1169;
        stream.update(result_peg_1169.getPosition());
        
        
        return result_peg_1169;
        out_peg_1175:
        Result result_peg_1176(myposition);
        
        {
            
            char letter_peg_1181 = stream.get(result_peg_1176.getPosition());
            if (letter_peg_1181 != '\0' && strchr("0123456789", letter_peg_1181) != NULL){
                result_peg_1176.nextPosition();
                result_peg_1176.setValue((void*) (long) letter_peg_1181);
            } else {
                goto out_peg_1180;
            }
            
        }
        goto success_peg_1177;
        out_peg_1180:
        goto out_peg_1182;
        success_peg_1177:
        ;
        
        if (column_peg_1168.chunk3 == 0){
            column_peg_1168.chunk3 = new Chunk3();
        }
        column_peg_1168.chunk3->chunk_alpha_digit = result_peg_1176;
        stream.update(result_peg_1176.getPosition());
        
        
        return result_peg_1176;
        out_peg_1182:
    
        if (column_peg_1168.chunk3 == 0){
            column_peg_1168.chunk3 = new Chunk3();
        }
        column_peg_1168.chunk3->chunk_alpha_digit = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_1184 = stream.getColumn(position);
    if (column_peg_1184.chunk3 != 0 && column_peg_1184.chunk3->chunk_valuelist.calculated()){
        return column_peg_1184.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1302(stream, "valuelist");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_1185(myposition);
        
        {
        
            result_peg_1185 = rule_expr_c(stream, result_peg_1185.getPosition());
                if (result_peg_1185.error()){
                    goto out_peg_1187;
                }
                first = result_peg_1185.getValues();
            
            
            
            result_peg_1185.reset();
                do{
                    Result result_peg_1190(result_peg_1185.getPosition());
                    {
                    
                        {
                                
                                result_peg_1190.reset();
                                do{
                                    Result result_peg_1202(result_peg_1190.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1202.getPosition()))){
                                                result_peg_1202.nextPosition();
                                            } else {
                                                goto out_peg_1205;
                                            }
                                        }
                                        result_peg_1202.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1203;
                                    out_peg_1205:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1202.getPosition()))){
                                                result_peg_1202.nextPosition();
                                            } else {
                                                goto out_peg_1207;
                                            }
                                        }
                                        result_peg_1202.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1203;
                                    out_peg_1207:
                                    goto loop_peg_1201;
                                    success_peg_1203:
                                    ;
                                    result_peg_1190.addResult(result_peg_1202);
                                } while (true);
                                loop_peg_1201:
                                ;
                                        
                            }
                            goto success_peg_1192;
                            goto loop_peg_1189;
                            success_peg_1192:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1190.getPosition()))){
                                    result_peg_1190.nextPosition();
                                } else {
                                    goto loop_peg_1189;
                                }
                            }
                            result_peg_1190.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1190.reset();
                                do{
                                    Result result_peg_1220(result_peg_1190.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1220.getPosition()))){
                                                result_peg_1220.nextPosition();
                                            } else {
                                                goto out_peg_1223;
                                            }
                                        }
                                        result_peg_1220.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1221;
                                    out_peg_1223:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1220.getPosition()))){
                                                result_peg_1220.nextPosition();
                                            } else {
                                                goto out_peg_1225;
                                            }
                                        }
                                        result_peg_1220.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1221;
                                    out_peg_1225:
                                    goto loop_peg_1219;
                                    success_peg_1221:
                                    ;
                                    result_peg_1190.addResult(result_peg_1220);
                                } while (true);
                                loop_peg_1219:
                                ;
                                        
                            }
                            goto success_peg_1210;
                            goto loop_peg_1189;
                            success_peg_1210:
                            ;
                        
                        
                        
                        result_peg_1190 = rule_expr_c(stream, result_peg_1190.getPosition());
                            if (result_peg_1190.error()){
                                goto loop_peg_1189;
                            }
                        
                        
                    }
                    result_peg_1185.addResult(result_peg_1190);
                } while (true);
                loop_peg_1189:
                if (result_peg_1185.matches() == 0){
                    goto out_peg_1187;
                }
                rest = result_peg_1185.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_1185.setValue(value);
                }
            
            
        }
        
        if (column_peg_1184.chunk3 == 0){
            column_peg_1184.chunk3 = new Chunk3();
        }
        column_peg_1184.chunk3->chunk_valuelist = result_peg_1185;
        stream.update(result_peg_1185.getPosition());
        
        
        return result_peg_1185;
        out_peg_1187:
        Result result_peg_1226(myposition);
        
        {
        
            {
                    
                    result_peg_1226.reset();
                    do{
                        Result result_peg_1238(result_peg_1226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1238.getPosition()))){
                                    result_peg_1238.nextPosition();
                                } else {
                                    goto out_peg_1241;
                                }
                            }
                            result_peg_1238.setValue((void*) " ");
                                
                        }
                        goto success_peg_1239;
                        out_peg_1241:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1238.getPosition()))){
                                    result_peg_1238.nextPosition();
                                } else {
                                    goto out_peg_1243;
                                }
                            }
                            result_peg_1238.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1239;
                        out_peg_1243:
                        goto loop_peg_1237;
                        success_peg_1239:
                        ;
                        result_peg_1226.addResult(result_peg_1238);
                    } while (true);
                    loop_peg_1237:
                    ;
                            
                }
                goto success_peg_1228;
                goto out_peg_1244;
                success_peg_1228:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_1226.getPosition()))){
                        result_peg_1226.nextPosition();
                    } else {
                        goto out_peg_1244;
                    }
                }
                result_peg_1226.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_1226.reset();
                    do{
                        Result result_peg_1257(result_peg_1226.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1257.getPosition()))){
                                    result_peg_1257.nextPosition();
                                } else {
                                    goto out_peg_1260;
                                }
                            }
                            result_peg_1257.setValue((void*) " ");
                                
                        }
                        goto success_peg_1258;
                        out_peg_1260:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1257.getPosition()))){
                                    result_peg_1257.nextPosition();
                                } else {
                                    goto out_peg_1262;
                                }
                            }
                            result_peg_1257.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1258;
                        out_peg_1262:
                        goto loop_peg_1256;
                        success_peg_1258:
                        ;
                        result_peg_1226.addResult(result_peg_1257);
                    } while (true);
                    loop_peg_1256:
                    ;
                            
                }
                goto success_peg_1247;
                goto out_peg_1244;
                success_peg_1247:
                ;
            
            
            
            result_peg_1226 = rule_expr_c(stream, result_peg_1226.getPosition());
                if (result_peg_1226.error()){
                    goto out_peg_1244;
                }
                first = result_peg_1226.getValues();
            
            
            
            result_peg_1226.reset();
                do{
                    Result result_peg_1266(result_peg_1226.getPosition());
                    {
                    
                        {
                                
                                result_peg_1266.reset();
                                do{
                                    Result result_peg_1278(result_peg_1266.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1278.getPosition()))){
                                                result_peg_1278.nextPosition();
                                            } else {
                                                goto out_peg_1281;
                                            }
                                        }
                                        result_peg_1278.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1279;
                                    out_peg_1281:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1278.getPosition()))){
                                                result_peg_1278.nextPosition();
                                            } else {
                                                goto out_peg_1283;
                                            }
                                        }
                                        result_peg_1278.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1279;
                                    out_peg_1283:
                                    goto loop_peg_1277;
                                    success_peg_1279:
                                    ;
                                    result_peg_1266.addResult(result_peg_1278);
                                } while (true);
                                loop_peg_1277:
                                ;
                                        
                            }
                            goto success_peg_1268;
                            goto loop_peg_1265;
                            success_peg_1268:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_1266.getPosition()))){
                                    result_peg_1266.nextPosition();
                                } else {
                                    goto loop_peg_1265;
                                }
                            }
                            result_peg_1266.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_1266.reset();
                                do{
                                    Result result_peg_1296(result_peg_1266.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_1296.getPosition()))){
                                                result_peg_1296.nextPosition();
                                            } else {
                                                goto out_peg_1299;
                                            }
                                        }
                                        result_peg_1296.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_1297;
                                    out_peg_1299:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_1296.getPosition()))){
                                                result_peg_1296.nextPosition();
                                            } else {
                                                goto out_peg_1301;
                                            }
                                        }
                                        result_peg_1296.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_1297;
                                    out_peg_1301:
                                    goto loop_peg_1295;
                                    success_peg_1297:
                                    ;
                                    result_peg_1266.addResult(result_peg_1296);
                                } while (true);
                                loop_peg_1295:
                                ;
                                        
                            }
                            goto success_peg_1286;
                            goto loop_peg_1265;
                            success_peg_1286:
                            ;
                        
                        
                        
                        result_peg_1266 = rule_expr_c(stream, result_peg_1266.getPosition());
                            if (result_peg_1266.error()){
                                goto loop_peg_1265;
                            }
                        
                        
                    }
                    result_peg_1226.addResult(result_peg_1266);
                } while (true);
                loop_peg_1265:
                ;
                rest = result_peg_1226.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(first, rest);
                    result_peg_1226.setValue(value);
                }
            
            
        }
        
        if (column_peg_1184.chunk3 == 0){
            column_peg_1184.chunk3 = new Chunk3();
        }
        column_peg_1184.chunk3->chunk_valuelist = result_peg_1226;
        stream.update(result_peg_1226.getPosition());
        
        
        return result_peg_1226;
        out_peg_1244:
    
        if (column_peg_1184.chunk3 == 0){
            column_peg_1184.chunk3 = new Chunk3();
        }
        column_peg_1184.chunk3->chunk_valuelist = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1303 = stream.getColumn(position);
    if (column_peg_1303.chunk3 != 0 && column_peg_1303.chunk3->chunk_expr.calculated()){
        return column_peg_1303.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_1330(stream, "expr");
    int myposition = position;
    
    
    
    Result result_peg_1304(myposition);
        
        {
        
            result_peg_1304 = rule_expr_c(stream, result_peg_1304.getPosition());
                if (result_peg_1304.error()){
                    goto out_peg_1306;
                }
            
            Result result_peg_1305 = result_peg_1304;
            
            Result result_peg_1309(result_peg_1304);
                {
                
                    {
                            
                            result_peg_1309.reset();
                            do{
                                Result result_peg_1321(result_peg_1309.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1321.getPosition()))){
                                            result_peg_1321.nextPosition();
                                        } else {
                                            goto out_peg_1324;
                                        }
                                    }
                                    result_peg_1321.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1322;
                                out_peg_1324:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1321.getPosition()))){
                                            result_peg_1321.nextPosition();
                                        } else {
                                            goto out_peg_1326;
                                        }
                                    }
                                    result_peg_1321.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1322;
                                out_peg_1326:
                                goto loop_peg_1320;
                                success_peg_1322:
                                ;
                                result_peg_1309.addResult(result_peg_1321);
                            } while (true);
                            loop_peg_1320:
                            ;
                                    
                        }
                        goto success_peg_1311;
                        goto not_peg_1308;
                        success_peg_1311:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1309.getPosition()))){
                                result_peg_1309.nextPosition();
                            } else {
                                goto not_peg_1308;
                            }
                        }
                        result_peg_1309.setValue((void*) ",");
                    
                    
                }
                goto out_peg_1306;
                not_peg_1308:
                result_peg_1304.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_1305.getValues();
                    result_peg_1304.setValue(value);
                }
            
            
        }
        
        if (column_peg_1303.chunk3 == 0){
            column_peg_1303.chunk3 = new Chunk3();
        }
        column_peg_1303.chunk3->chunk_expr = result_peg_1304;
        stream.update(result_peg_1304.getPosition());
        
        
        return result_peg_1304;
        out_peg_1306:
        Result result_peg_1327(myposition);
        
        {
        
            result_peg_1327 = rule_valuelist(stream, result_peg_1327.getPosition());
                if (result_peg_1327.error()){
                    goto out_peg_1329;
                }
            
            Result result_peg_1328 = result_peg_1327;
            
            result_peg_1327 = rule_expr2_rest(stream, result_peg_1327.getPosition(), result_peg_1328.getValues());
                if (result_peg_1327.error()){
                    goto out_peg_1329;
                }
            
            
        }
        
        if (column_peg_1303.chunk3 == 0){
            column_peg_1303.chunk3 = new Chunk3();
        }
        column_peg_1303.chunk3->chunk_expr = result_peg_1327;
        stream.update(result_peg_1327.getPosition());
        
        
        return result_peg_1327;
        out_peg_1329:
    
        if (column_peg_1303.chunk3 == 0){
            column_peg_1303.chunk3 = new Chunk3();
        }
        column_peg_1303.chunk3->chunk_expr = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1331 = stream.getColumn(position);
    if (column_peg_1331.chunk3 != 0 && column_peg_1331.chunk3->chunk_expr_c.calculated()){
        return column_peg_1331.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_1335(stream, "expr_c");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1332(myposition);
        
        {
        
            result_peg_1332 = rule_expr2(stream, result_peg_1332.getPosition());
                if (result_peg_1332.error()){
                    goto out_peg_1334;
                }
                left = result_peg_1332.getValues();
            
            
            
            result_peg_1332 = rule_expr_rest(stream, result_peg_1332.getPosition(), left);
                if (result_peg_1332.error()){
                    goto out_peg_1334;
                }
            
            
        }
        
        if (column_peg_1331.chunk3 == 0){
            column_peg_1331.chunk3 = new Chunk3();
        }
        column_peg_1331.chunk3->chunk_expr_c = result_peg_1332;
        stream.update(result_peg_1332.getPosition());
        
        
        return result_peg_1332;
        out_peg_1334:
    
        if (column_peg_1331.chunk3 == 0){
            column_peg_1331.chunk3 = new Chunk3();
        }
        column_peg_1331.chunk3->chunk_expr_c = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1336 = stream.getColumn(position);
    if (column_peg_1336.chunk4 != 0 && column_peg_1336.chunk4->chunk_expr2.calculated()){
        return column_peg_1336.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_1340(stream, "expr2");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1337(myposition);
        
        {
        
            result_peg_1337 = rule_expr3(stream, result_peg_1337.getPosition());
                if (result_peg_1337.error()){
                    goto out_peg_1339;
                }
                left = result_peg_1337.getValues();
            
            
            
            result_peg_1337 = rule_expr2_rest(stream, result_peg_1337.getPosition(), left);
                if (result_peg_1337.error()){
                    goto out_peg_1339;
                }
            
            
        }
        
        if (column_peg_1336.chunk4 == 0){
            column_peg_1336.chunk4 = new Chunk4();
        }
        column_peg_1336.chunk4->chunk_expr2 = result_peg_1337;
        stream.update(result_peg_1337.getPosition());
        
        
        return result_peg_1337;
        out_peg_1339:
    
        if (column_peg_1336.chunk4 == 0){
            column_peg_1336.chunk4 = new Chunk4();
        }
        column_peg_1336.chunk4->chunk_expr2 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1341 = stream.getColumn(position);
    if (column_peg_1341.chunk4 != 0 && column_peg_1341.chunk4->chunk_expr3.calculated()){
        return column_peg_1341.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_1345(stream, "expr3");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1342(myposition);
        
        {
        
            result_peg_1342 = rule_expr4(stream, result_peg_1342.getPosition());
                if (result_peg_1342.error()){
                    goto out_peg_1344;
                }
                left = result_peg_1342.getValues();
            
            
            
            result_peg_1342 = rule_expr3_rest(stream, result_peg_1342.getPosition(), left);
                if (result_peg_1342.error()){
                    goto out_peg_1344;
                }
            
            
        }
        
        if (column_peg_1341.chunk4 == 0){
            column_peg_1341.chunk4 = new Chunk4();
        }
        column_peg_1341.chunk4->chunk_expr3 = result_peg_1342;
        stream.update(result_peg_1342.getPosition());
        
        
        return result_peg_1342;
        out_peg_1344:
    
        if (column_peg_1341.chunk4 == 0){
            column_peg_1341.chunk4 = new Chunk4();
        }
        column_peg_1341.chunk4->chunk_expr3 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1346 = stream.getColumn(position);
    if (column_peg_1346.chunk4 != 0 && column_peg_1346.chunk4->chunk_expr4.calculated()){
        return column_peg_1346.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_1350(stream, "expr4");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1347(myposition);
        
        {
        
            result_peg_1347 = rule_expr5(stream, result_peg_1347.getPosition());
                if (result_peg_1347.error()){
                    goto out_peg_1349;
                }
                left = result_peg_1347.getValues();
            
            
            
            result_peg_1347 = rule_expr4_rest(stream, result_peg_1347.getPosition(), left);
                if (result_peg_1347.error()){
                    goto out_peg_1349;
                }
            
            
        }
        
        if (column_peg_1346.chunk4 == 0){
            column_peg_1346.chunk4 = new Chunk4();
        }
        column_peg_1346.chunk4->chunk_expr4 = result_peg_1347;
        stream.update(result_peg_1347.getPosition());
        
        
        return result_peg_1347;
        out_peg_1349:
    
        if (column_peg_1346.chunk4 == 0){
            column_peg_1346.chunk4 = new Chunk4();
        }
        column_peg_1346.chunk4->chunk_expr4 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1351 = stream.getColumn(position);
    if (column_peg_1351.chunk4 != 0 && column_peg_1351.chunk4->chunk_expr5.calculated()){
        return column_peg_1351.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_1355(stream, "expr5");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1352(myposition);
        
        {
        
            result_peg_1352 = rule_expr6(stream, result_peg_1352.getPosition());
                if (result_peg_1352.error()){
                    goto out_peg_1354;
                }
                left = result_peg_1352.getValues();
            
            
            
            result_peg_1352 = rule_expr5_rest(stream, result_peg_1352.getPosition(), left);
                if (result_peg_1352.error()){
                    goto out_peg_1354;
                }
            
            
        }
        
        if (column_peg_1351.chunk4 == 0){
            column_peg_1351.chunk4 = new Chunk4();
        }
        column_peg_1351.chunk4->chunk_expr5 = result_peg_1352;
        stream.update(result_peg_1352.getPosition());
        
        
        return result_peg_1352;
        out_peg_1354:
    
        if (column_peg_1351.chunk4 == 0){
            column_peg_1351.chunk4 = new Chunk4();
        }
        column_peg_1351.chunk4->chunk_expr5 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1356 = stream.getColumn(position);
    if (column_peg_1356.chunk4 != 0 && column_peg_1356.chunk4->chunk_expr6.calculated()){
        return column_peg_1356.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_1360(stream, "expr6");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1357(myposition);
        
        {
        
            result_peg_1357 = rule_expr7(stream, result_peg_1357.getPosition());
                if (result_peg_1357.error()){
                    goto out_peg_1359;
                }
                left = result_peg_1357.getValues();
            
            
            
            result_peg_1357 = rule_expr6_rest(stream, result_peg_1357.getPosition(), left);
                if (result_peg_1357.error()){
                    goto out_peg_1359;
                }
            
            
        }
        
        if (column_peg_1356.chunk4 == 0){
            column_peg_1356.chunk4 = new Chunk4();
        }
        column_peg_1356.chunk4->chunk_expr6 = result_peg_1357;
        stream.update(result_peg_1357.getPosition());
        
        
        return result_peg_1357;
        out_peg_1359:
    
        if (column_peg_1356.chunk4 == 0){
            column_peg_1356.chunk4 = new Chunk4();
        }
        column_peg_1356.chunk4->chunk_expr6 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1361 = stream.getColumn(position);
    if (column_peg_1361.chunk5 != 0 && column_peg_1361.chunk5->chunk_expr7.calculated()){
        return column_peg_1361.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_1365(stream, "expr7");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1362(myposition);
        
        {
        
            result_peg_1362 = rule_expr8(stream, result_peg_1362.getPosition());
                if (result_peg_1362.error()){
                    goto out_peg_1364;
                }
                left = result_peg_1362.getValues();
            
            
            
            result_peg_1362 = rule_expr7_rest(stream, result_peg_1362.getPosition(), left);
                if (result_peg_1362.error()){
                    goto out_peg_1364;
                }
            
            
        }
        
        if (column_peg_1361.chunk5 == 0){
            column_peg_1361.chunk5 = new Chunk5();
        }
        column_peg_1361.chunk5->chunk_expr7 = result_peg_1362;
        stream.update(result_peg_1362.getPosition());
        
        
        return result_peg_1362;
        out_peg_1364:
    
        if (column_peg_1361.chunk5 == 0){
            column_peg_1361.chunk5 = new Chunk5();
        }
        column_peg_1361.chunk5->chunk_expr7 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1366 = stream.getColumn(position);
    if (column_peg_1366.chunk5 != 0 && column_peg_1366.chunk5->chunk_expr8.calculated()){
        return column_peg_1366.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_1370(stream, "expr8");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1367(myposition);
        
        {
        
            result_peg_1367 = rule_expr9(stream, result_peg_1367.getPosition());
                if (result_peg_1367.error()){
                    goto out_peg_1369;
                }
                left = result_peg_1367.getValues();
            
            
            
            result_peg_1367 = rule_expr8_rest(stream, result_peg_1367.getPosition(), left);
                if (result_peg_1367.error()){
                    goto out_peg_1369;
                }
            
            
        }
        
        if (column_peg_1366.chunk5 == 0){
            column_peg_1366.chunk5 = new Chunk5();
        }
        column_peg_1366.chunk5->chunk_expr8 = result_peg_1367;
        stream.update(result_peg_1367.getPosition());
        
        
        return result_peg_1367;
        out_peg_1369:
    
        if (column_peg_1366.chunk5 == 0){
            column_peg_1366.chunk5 = new Chunk5();
        }
        column_peg_1366.chunk5->chunk_expr8 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1371 = stream.getColumn(position);
    if (column_peg_1371.chunk5 != 0 && column_peg_1371.chunk5->chunk_expr9.calculated()){
        return column_peg_1371.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_1375(stream, "expr9");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1372(myposition);
        
        {
        
            result_peg_1372 = rule_expr10(stream, result_peg_1372.getPosition());
                if (result_peg_1372.error()){
                    goto out_peg_1374;
                }
                left = result_peg_1372.getValues();
            
            
            
            result_peg_1372 = rule_expr9_rest(stream, result_peg_1372.getPosition(), left);
                if (result_peg_1372.error()){
                    goto out_peg_1374;
                }
            
            
        }
        
        if (column_peg_1371.chunk5 == 0){
            column_peg_1371.chunk5 = new Chunk5();
        }
        column_peg_1371.chunk5->chunk_expr9 = result_peg_1372;
        stream.update(result_peg_1372.getPosition());
        
        
        return result_peg_1372;
        out_peg_1374:
    
        if (column_peg_1371.chunk5 == 0){
            column_peg_1371.chunk5 = new Chunk5();
        }
        column_peg_1371.chunk5->chunk_expr9 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1376 = stream.getColumn(position);
    if (column_peg_1376.chunk5 != 0 && column_peg_1376.chunk5->chunk_expr10.calculated()){
        return column_peg_1376.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_1380(stream, "expr10");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1377(myposition);
        
        {
        
            result_peg_1377 = rule_expr11(stream, result_peg_1377.getPosition());
                if (result_peg_1377.error()){
                    goto out_peg_1379;
                }
                left = result_peg_1377.getValues();
            
            
            
            result_peg_1377 = rule_expr10_rest(stream, result_peg_1377.getPosition(), left);
                if (result_peg_1377.error()){
                    goto out_peg_1379;
                }
            
            
        }
        
        if (column_peg_1376.chunk5 == 0){
            column_peg_1376.chunk5 = new Chunk5();
        }
        column_peg_1376.chunk5->chunk_expr10 = result_peg_1377;
        stream.update(result_peg_1377.getPosition());
        
        
        return result_peg_1377;
        out_peg_1379:
    
        if (column_peg_1376.chunk5 == 0){
            column_peg_1376.chunk5 = new Chunk5();
        }
        column_peg_1376.chunk5->chunk_expr10 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1381 = stream.getColumn(position);
    if (column_peg_1381.chunk5 != 0 && column_peg_1381.chunk5->chunk_expr11.calculated()){
        return column_peg_1381.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_1385(stream, "expr11");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1382(myposition);
        
        {
        
            result_peg_1382 = rule_expr12(stream, result_peg_1382.getPosition());
                if (result_peg_1382.error()){
                    goto out_peg_1384;
                }
                left = result_peg_1382.getValues();
            
            
            
            result_peg_1382 = rule_expr11_rest(stream, result_peg_1382.getPosition(), left);
                if (result_peg_1382.error()){
                    goto out_peg_1384;
                }
            
            
        }
        
        if (column_peg_1381.chunk5 == 0){
            column_peg_1381.chunk5 = new Chunk5();
        }
        column_peg_1381.chunk5->chunk_expr11 = result_peg_1382;
        stream.update(result_peg_1382.getPosition());
        
        
        return result_peg_1382;
        out_peg_1384:
    
        if (column_peg_1381.chunk5 == 0){
            column_peg_1381.chunk5 = new Chunk5();
        }
        column_peg_1381.chunk5->chunk_expr11 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1386 = stream.getColumn(position);
    if (column_peg_1386.chunk6 != 0 && column_peg_1386.chunk6->chunk_expr12.calculated()){
        return column_peg_1386.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_1390(stream, "expr12");
    int myposition = position;
    
    
    Value left;
    Result result_peg_1387(myposition);
        
        {
        
            result_peg_1387 = rule_expr13(stream, result_peg_1387.getPosition());
                if (result_peg_1387.error()){
                    goto out_peg_1389;
                }
                left = result_peg_1387.getValues();
            
            
            
            result_peg_1387 = rule_expr12_rest(stream, result_peg_1387.getPosition(), left);
                if (result_peg_1387.error()){
                    goto out_peg_1389;
                }
            
            
        }
        
        if (column_peg_1386.chunk6 == 0){
            column_peg_1386.chunk6 = new Chunk6();
        }
        column_peg_1386.chunk6->chunk_expr12 = result_peg_1387;
        stream.update(result_peg_1387.getPosition());
        
        
        return result_peg_1387;
        out_peg_1389:
    
        if (column_peg_1386.chunk6 == 0){
            column_peg_1386.chunk6 = new Chunk6();
        }
        column_peg_1386.chunk6->chunk_expr12 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1391 = stream.getColumn(position);
    if (column_peg_1391.chunk6 != 0 && column_peg_1391.chunk6->chunk_expr13.calculated()){
        return column_peg_1391.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_1415(stream, "expr13");
    int myposition = position;
    
    
    Value exp;
    Result result_peg_1392(myposition);
        
        {
        
            result_peg_1392.reset();
                do{
                    Result result_peg_1395(result_peg_1392.getPosition());
                    result_peg_1395 = rule_unary(stream, result_peg_1395.getPosition());
                    if (result_peg_1395.error()){
                        goto loop_peg_1394;
                    }
                    result_peg_1392.addResult(result_peg_1395);
                } while (true);
                loop_peg_1394:
                ;
            
            Result result_peg_1393 = result_peg_1392;
            
            {
                    
                    result_peg_1392.reset();
                    do{
                        Result result_peg_1407(result_peg_1392.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1407.getPosition()))){
                                    result_peg_1407.nextPosition();
                                } else {
                                    goto out_peg_1410;
                                }
                            }
                            result_peg_1407.setValue((void*) " ");
                                
                        }
                        goto success_peg_1408;
                        out_peg_1410:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1407.getPosition()))){
                                    result_peg_1407.nextPosition();
                                } else {
                                    goto out_peg_1412;
                                }
                            }
                            result_peg_1407.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1408;
                        out_peg_1412:
                        goto loop_peg_1406;
                        success_peg_1408:
                        ;
                        result_peg_1392.addResult(result_peg_1407);
                    } while (true);
                    loop_peg_1406:
                    ;
                            
                }
                goto success_peg_1397;
                goto out_peg_1413;
                success_peg_1397:
                ;
            
            
            
            result_peg_1392 = rule_expr13_real(stream, result_peg_1392.getPosition());
                if (result_peg_1392.error()){
                    goto out_peg_1413;
                }
                exp = result_peg_1392.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeUnaryExpression(result_peg_1393.getValues(), exp);
                    result_peg_1392.setValue(value);
                }
            
            
        }
        
        if (column_peg_1391.chunk6 == 0){
            column_peg_1391.chunk6 = new Chunk6();
        }
        column_peg_1391.chunk6->chunk_expr13 = result_peg_1392;
        stream.update(result_peg_1392.getPosition());
        
        
        return result_peg_1392;
        out_peg_1413:
    
        if (column_peg_1391.chunk6 == 0){
            column_peg_1391.chunk6 = new Chunk6();
        }
        column_peg_1391.chunk6->chunk_expr13 = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1459(stream, "expr_rest");
    int myposition = position;
    
    tail_peg_1418:
    Value right;
        Value new_left;
    Result result_peg_1417(myposition);
        {
        
            {
                    
                    result_peg_1417.reset();
                    do{
                        Result result_peg_1430(result_peg_1417.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1430.getPosition()))){
                                    result_peg_1430.nextPosition();
                                } else {
                                    goto out_peg_1433;
                                }
                            }
                            result_peg_1430.setValue((void*) " ");
                                
                        }
                        goto success_peg_1431;
                        out_peg_1433:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1430.getPosition()))){
                                    result_peg_1430.nextPosition();
                                } else {
                                    goto out_peg_1435;
                                }
                            }
                            result_peg_1430.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1431;
                        out_peg_1435:
                        goto loop_peg_1429;
                        success_peg_1431:
                        ;
                        result_peg_1417.addResult(result_peg_1430);
                    } while (true);
                    loop_peg_1429:
                    ;
                            
                }
                goto success_peg_1420;
                goto out_peg_1436;
                success_peg_1420:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("||"[i], stream.get(result_peg_1417.getPosition()))){
                        result_peg_1417.nextPosition();
                    } else {
                        goto out_peg_1436;
                    }
                }
                result_peg_1417.setValue((void*) "||");
            
            
            
            {
                    
                    result_peg_1417.reset();
                    do{
                        Result result_peg_1449(result_peg_1417.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1449.getPosition()))){
                                    result_peg_1449.nextPosition();
                                } else {
                                    goto out_peg_1452;
                                }
                            }
                            result_peg_1449.setValue((void*) " ");
                                
                        }
                        goto success_peg_1450;
                        out_peg_1452:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1449.getPosition()))){
                                    result_peg_1449.nextPosition();
                                } else {
                                    goto out_peg_1454;
                                }
                            }
                            result_peg_1449.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1450;
                        out_peg_1454:
                        goto loop_peg_1448;
                        success_peg_1450:
                        ;
                        result_peg_1417.addResult(result_peg_1449);
                    } while (true);
                    loop_peg_1448:
                    ;
                            
                }
                goto success_peg_1439;
                goto out_peg_1436;
                success_peg_1439:
                ;
            
            
            
            result_peg_1417 = rule_expr2(stream, result_peg_1417.getPosition());
                if (result_peg_1417.error()){
                    goto out_peg_1436;
                }
                right = result_peg_1417.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionOr(left, right);
                    result_peg_1417.setValue(value);
                }
                new_left = result_peg_1417.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1417.getPosition();
        goto tail_peg_1418;
        out_peg_1436:
        Result result_peg_1457(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1457.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1457.getPosition());
        
        return result_peg_1457;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1503(stream, "expr2_rest");
    int myposition = position;
    
    tail_peg_1462:
    Value right;
        Value new_left;
    Result result_peg_1461(myposition);
        {
        
            {
                    
                    result_peg_1461.reset();
                    do{
                        Result result_peg_1474(result_peg_1461.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1474.getPosition()))){
                                    result_peg_1474.nextPosition();
                                } else {
                                    goto out_peg_1477;
                                }
                            }
                            result_peg_1474.setValue((void*) " ");
                                
                        }
                        goto success_peg_1475;
                        out_peg_1477:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1474.getPosition()))){
                                    result_peg_1474.nextPosition();
                                } else {
                                    goto out_peg_1479;
                                }
                            }
                            result_peg_1474.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1475;
                        out_peg_1479:
                        goto loop_peg_1473;
                        success_peg_1475:
                        ;
                        result_peg_1461.addResult(result_peg_1474);
                    } while (true);
                    loop_peg_1473:
                    ;
                            
                }
                goto success_peg_1464;
                goto out_peg_1480;
                success_peg_1464:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("^^"[i], stream.get(result_peg_1461.getPosition()))){
                        result_peg_1461.nextPosition();
                    } else {
                        goto out_peg_1480;
                    }
                }
                result_peg_1461.setValue((void*) "^^");
            
            
            
            {
                    
                    result_peg_1461.reset();
                    do{
                        Result result_peg_1493(result_peg_1461.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1493.getPosition()))){
                                    result_peg_1493.nextPosition();
                                } else {
                                    goto out_peg_1496;
                                }
                            }
                            result_peg_1493.setValue((void*) " ");
                                
                        }
                        goto success_peg_1494;
                        out_peg_1496:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1493.getPosition()))){
                                    result_peg_1493.nextPosition();
                                } else {
                                    goto out_peg_1498;
                                }
                            }
                            result_peg_1493.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1494;
                        out_peg_1498:
                        goto loop_peg_1492;
                        success_peg_1494:
                        ;
                        result_peg_1461.addResult(result_peg_1493);
                    } while (true);
                    loop_peg_1492:
                    ;
                            
                }
                goto success_peg_1483;
                goto out_peg_1480;
                success_peg_1483:
                ;
            
            
            
            result_peg_1461 = rule_expr3(stream, result_peg_1461.getPosition());
                if (result_peg_1461.error()){
                    goto out_peg_1480;
                }
                right = result_peg_1461.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionXOr(left, right);
                    result_peg_1461.setValue(value);
                }
                new_left = result_peg_1461.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1461.getPosition();
        goto tail_peg_1462;
        out_peg_1480:
        Result result_peg_1501(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1501.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1501.getPosition());
        
        return result_peg_1501;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1547(stream, "expr3_rest");
    int myposition = position;
    
    tail_peg_1506:
    Value right;
        Value new_left;
    Result result_peg_1505(myposition);
        {
        
            {
                    
                    result_peg_1505.reset();
                    do{
                        Result result_peg_1518(result_peg_1505.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1518.getPosition()))){
                                    result_peg_1518.nextPosition();
                                } else {
                                    goto out_peg_1521;
                                }
                            }
                            result_peg_1518.setValue((void*) " ");
                                
                        }
                        goto success_peg_1519;
                        out_peg_1521:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1518.getPosition()))){
                                    result_peg_1518.nextPosition();
                                } else {
                                    goto out_peg_1523;
                                }
                            }
                            result_peg_1518.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1519;
                        out_peg_1523:
                        goto loop_peg_1517;
                        success_peg_1519:
                        ;
                        result_peg_1505.addResult(result_peg_1518);
                    } while (true);
                    loop_peg_1517:
                    ;
                            
                }
                goto success_peg_1508;
                goto out_peg_1524;
                success_peg_1508:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("&&"[i], stream.get(result_peg_1505.getPosition()))){
                        result_peg_1505.nextPosition();
                    } else {
                        goto out_peg_1524;
                    }
                }
                result_peg_1505.setValue((void*) "&&");
            
            
            
            {
                    
                    result_peg_1505.reset();
                    do{
                        Result result_peg_1537(result_peg_1505.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1537.getPosition()))){
                                    result_peg_1537.nextPosition();
                                } else {
                                    goto out_peg_1540;
                                }
                            }
                            result_peg_1537.setValue((void*) " ");
                                
                        }
                        goto success_peg_1538;
                        out_peg_1540:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1537.getPosition()))){
                                    result_peg_1537.nextPosition();
                                } else {
                                    goto out_peg_1542;
                                }
                            }
                            result_peg_1537.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1538;
                        out_peg_1542:
                        goto loop_peg_1536;
                        success_peg_1538:
                        ;
                        result_peg_1505.addResult(result_peg_1537);
                    } while (true);
                    loop_peg_1536:
                    ;
                            
                }
                goto success_peg_1527;
                goto out_peg_1524;
                success_peg_1527:
                ;
            
            
            
            result_peg_1505 = rule_expr4(stream, result_peg_1505.getPosition());
                if (result_peg_1505.error()){
                    goto out_peg_1524;
                }
                right = result_peg_1505.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAnd(left, right);
                    result_peg_1505.setValue(value);
                }
                new_left = result_peg_1505.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1505.getPosition();
        goto tail_peg_1506;
        out_peg_1524:
        Result result_peg_1545(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1545.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1545.getPosition());
        
        return result_peg_1545;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1591(stream, "expr4_rest");
    int myposition = position;
    
    tail_peg_1550:
    Value right;
        Value new_left;
    Result result_peg_1549(myposition);
        {
        
            {
                    
                    result_peg_1549.reset();
                    do{
                        Result result_peg_1562(result_peg_1549.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1562.getPosition()))){
                                    result_peg_1562.nextPosition();
                                } else {
                                    goto out_peg_1565;
                                }
                            }
                            result_peg_1562.setValue((void*) " ");
                                
                        }
                        goto success_peg_1563;
                        out_peg_1565:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1562.getPosition()))){
                                    result_peg_1562.nextPosition();
                                } else {
                                    goto out_peg_1567;
                                }
                            }
                            result_peg_1562.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1563;
                        out_peg_1567:
                        goto loop_peg_1561;
                        success_peg_1563:
                        ;
                        result_peg_1549.addResult(result_peg_1562);
                    } while (true);
                    loop_peg_1561:
                    ;
                            
                }
                goto success_peg_1552;
                goto out_peg_1568;
                success_peg_1552:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("|"[i], stream.get(result_peg_1549.getPosition()))){
                        result_peg_1549.nextPosition();
                    } else {
                        goto out_peg_1568;
                    }
                }
                result_peg_1549.setValue((void*) "|");
            
            
            
            {
                    
                    result_peg_1549.reset();
                    do{
                        Result result_peg_1581(result_peg_1549.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1581.getPosition()))){
                                    result_peg_1581.nextPosition();
                                } else {
                                    goto out_peg_1584;
                                }
                            }
                            result_peg_1581.setValue((void*) " ");
                                
                        }
                        goto success_peg_1582;
                        out_peg_1584:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1581.getPosition()))){
                                    result_peg_1581.nextPosition();
                                } else {
                                    goto out_peg_1586;
                                }
                            }
                            result_peg_1581.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1582;
                        out_peg_1586:
                        goto loop_peg_1580;
                        success_peg_1582:
                        ;
                        result_peg_1549.addResult(result_peg_1581);
                    } while (true);
                    loop_peg_1580:
                    ;
                            
                }
                goto success_peg_1571;
                goto out_peg_1568;
                success_peg_1571:
                ;
            
            
            
            result_peg_1549 = rule_expr5(stream, result_peg_1549.getPosition());
                if (result_peg_1549.error()){
                    goto out_peg_1568;
                }
                right = result_peg_1549.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseOr(left, right);
                    result_peg_1549.setValue(value);
                }
                new_left = result_peg_1549.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1549.getPosition();
        goto tail_peg_1550;
        out_peg_1568:
        Result result_peg_1589(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1589.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1589.getPosition());
        
        return result_peg_1589;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1635(stream, "expr5_rest");
    int myposition = position;
    
    tail_peg_1594:
    Value right;
        Value new_left;
    Result result_peg_1593(myposition);
        {
        
            {
                    
                    result_peg_1593.reset();
                    do{
                        Result result_peg_1606(result_peg_1593.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1606.getPosition()))){
                                    result_peg_1606.nextPosition();
                                } else {
                                    goto out_peg_1609;
                                }
                            }
                            result_peg_1606.setValue((void*) " ");
                                
                        }
                        goto success_peg_1607;
                        out_peg_1609:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1606.getPosition()))){
                                    result_peg_1606.nextPosition();
                                } else {
                                    goto out_peg_1611;
                                }
                            }
                            result_peg_1606.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1607;
                        out_peg_1611:
                        goto loop_peg_1605;
                        success_peg_1607:
                        ;
                        result_peg_1593.addResult(result_peg_1606);
                    } while (true);
                    loop_peg_1605:
                    ;
                            
                }
                goto success_peg_1596;
                goto out_peg_1612;
                success_peg_1596:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("^"[i], stream.get(result_peg_1593.getPosition()))){
                        result_peg_1593.nextPosition();
                    } else {
                        goto out_peg_1612;
                    }
                }
                result_peg_1593.setValue((void*) "^");
            
            
            
            {
                    
                    result_peg_1593.reset();
                    do{
                        Result result_peg_1625(result_peg_1593.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1625.getPosition()))){
                                    result_peg_1625.nextPosition();
                                } else {
                                    goto out_peg_1628;
                                }
                            }
                            result_peg_1625.setValue((void*) " ");
                                
                        }
                        goto success_peg_1626;
                        out_peg_1628:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1625.getPosition()))){
                                    result_peg_1625.nextPosition();
                                } else {
                                    goto out_peg_1630;
                                }
                            }
                            result_peg_1625.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1626;
                        out_peg_1630:
                        goto loop_peg_1624;
                        success_peg_1626:
                        ;
                        result_peg_1593.addResult(result_peg_1625);
                    } while (true);
                    loop_peg_1624:
                    ;
                            
                }
                goto success_peg_1615;
                goto out_peg_1612;
                success_peg_1615:
                ;
            
            
            
            result_peg_1593 = rule_expr6(stream, result_peg_1593.getPosition());
                if (result_peg_1593.error()){
                    goto out_peg_1612;
                }
                right = result_peg_1593.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseXOr(left, right);
                    result_peg_1593.setValue(value);
                }
                new_left = result_peg_1593.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1593.getPosition();
        goto tail_peg_1594;
        out_peg_1612:
        Result result_peg_1633(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1633.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1633.getPosition());
        
        return result_peg_1633;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1679(stream, "expr6_rest");
    int myposition = position;
    
    tail_peg_1638:
    Value right;
        Value new_left;
    Result result_peg_1637(myposition);
        {
        
            {
                    
                    result_peg_1637.reset();
                    do{
                        Result result_peg_1650(result_peg_1637.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1650.getPosition()))){
                                    result_peg_1650.nextPosition();
                                } else {
                                    goto out_peg_1653;
                                }
                            }
                            result_peg_1650.setValue((void*) " ");
                                
                        }
                        goto success_peg_1651;
                        out_peg_1653:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1650.getPosition()))){
                                    result_peg_1650.nextPosition();
                                } else {
                                    goto out_peg_1655;
                                }
                            }
                            result_peg_1650.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1651;
                        out_peg_1655:
                        goto loop_peg_1649;
                        success_peg_1651:
                        ;
                        result_peg_1637.addResult(result_peg_1650);
                    } while (true);
                    loop_peg_1649:
                    ;
                            
                }
                goto success_peg_1640;
                goto out_peg_1656;
                success_peg_1640:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("&"[i], stream.get(result_peg_1637.getPosition()))){
                        result_peg_1637.nextPosition();
                    } else {
                        goto out_peg_1656;
                    }
                }
                result_peg_1637.setValue((void*) "&");
            
            
            
            {
                    
                    result_peg_1637.reset();
                    do{
                        Result result_peg_1669(result_peg_1637.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1669.getPosition()))){
                                    result_peg_1669.nextPosition();
                                } else {
                                    goto out_peg_1672;
                                }
                            }
                            result_peg_1669.setValue((void*) " ");
                                
                        }
                        goto success_peg_1670;
                        out_peg_1672:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1669.getPosition()))){
                                    result_peg_1669.nextPosition();
                                } else {
                                    goto out_peg_1674;
                                }
                            }
                            result_peg_1669.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1670;
                        out_peg_1674:
                        goto loop_peg_1668;
                        success_peg_1670:
                        ;
                        result_peg_1637.addResult(result_peg_1669);
                    } while (true);
                    loop_peg_1668:
                    ;
                            
                }
                goto success_peg_1659;
                goto out_peg_1656;
                success_peg_1659:
                ;
            
            
            
            result_peg_1637 = rule_expr7(stream, result_peg_1637.getPosition());
                if (result_peg_1637.error()){
                    goto out_peg_1656;
                }
                right = result_peg_1637.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionBitwiseAnd(left, right);
                    result_peg_1637.setValue(value);
                }
                new_left = result_peg_1637.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1637.getPosition();
        goto tail_peg_1638;
        out_peg_1656:
        Result result_peg_1677(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1677.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1677.getPosition());
        
        return result_peg_1677;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1723(stream, "expr7_rest");
    int myposition = position;
    
    tail_peg_1682:
    Value right;
        Value new_left;
    Result result_peg_1681(myposition);
        {
        
            {
                    
                    result_peg_1681.reset();
                    do{
                        Result result_peg_1694(result_peg_1681.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1694.getPosition()))){
                                    result_peg_1694.nextPosition();
                                } else {
                                    goto out_peg_1697;
                                }
                            }
                            result_peg_1694.setValue((void*) " ");
                                
                        }
                        goto success_peg_1695;
                        out_peg_1697:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1694.getPosition()))){
                                    result_peg_1694.nextPosition();
                                } else {
                                    goto out_peg_1699;
                                }
                            }
                            result_peg_1694.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1695;
                        out_peg_1699:
                        goto loop_peg_1693;
                        success_peg_1695:
                        ;
                        result_peg_1681.addResult(result_peg_1694);
                    } while (true);
                    loop_peg_1693:
                    ;
                            
                }
                goto success_peg_1684;
                goto out_peg_1700;
                success_peg_1684:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar(":="[i], stream.get(result_peg_1681.getPosition()))){
                        result_peg_1681.nextPosition();
                    } else {
                        goto out_peg_1700;
                    }
                }
                result_peg_1681.setValue((void*) ":=");
            
            
            
            {
                    
                    result_peg_1681.reset();
                    do{
                        Result result_peg_1713(result_peg_1681.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1713.getPosition()))){
                                    result_peg_1713.nextPosition();
                                } else {
                                    goto out_peg_1716;
                                }
                            }
                            result_peg_1713.setValue((void*) " ");
                                
                        }
                        goto success_peg_1714;
                        out_peg_1716:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1713.getPosition()))){
                                    result_peg_1713.nextPosition();
                                } else {
                                    goto out_peg_1718;
                                }
                            }
                            result_peg_1713.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1714;
                        out_peg_1718:
                        goto loop_peg_1712;
                        success_peg_1714:
                        ;
                        result_peg_1681.addResult(result_peg_1713);
                    } while (true);
                    loop_peg_1712:
                    ;
                            
                }
                goto success_peg_1703;
                goto out_peg_1700;
                success_peg_1703:
                ;
            
            
            
            result_peg_1681 = rule_expr8(stream, result_peg_1681.getPosition());
                if (result_peg_1681.error()){
                    goto out_peg_1700;
                }
                right = result_peg_1681.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAssignment(left, right);
                    result_peg_1681.setValue(value);
                }
                new_left = result_peg_1681.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1681.getPosition();
        goto tail_peg_1682;
        out_peg_1700:
        Result result_peg_1721(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1721.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1721.getPosition());
        
        return result_peg_1721;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1806(stream, "expr8_rest");
    int myposition = position;
    
    tail_peg_1726:
    Value right;
        Value new_left;
    Result result_peg_1725(myposition);
        {
        
            {
                    
                    result_peg_1725.reset();
                    do{
                        Result result_peg_1738(result_peg_1725.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1738.getPosition()))){
                                    result_peg_1738.nextPosition();
                                } else {
                                    goto out_peg_1741;
                                }
                            }
                            result_peg_1738.setValue((void*) " ");
                                
                        }
                        goto success_peg_1739;
                        out_peg_1741:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1738.getPosition()))){
                                    result_peg_1738.nextPosition();
                                } else {
                                    goto out_peg_1743;
                                }
                            }
                            result_peg_1738.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1739;
                        out_peg_1743:
                        goto loop_peg_1737;
                        success_peg_1739:
                        ;
                        result_peg_1725.addResult(result_peg_1738);
                    } while (true);
                    loop_peg_1737:
                    ;
                            
                }
                goto success_peg_1728;
                goto out_peg_1744;
                success_peg_1728:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_1725.getPosition()))){
                        result_peg_1725.nextPosition();
                    } else {
                        goto out_peg_1744;
                    }
                }
                result_peg_1725.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_1725.reset();
                    do{
                        Result result_peg_1757(result_peg_1725.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1757.getPosition()))){
                                    result_peg_1757.nextPosition();
                                } else {
                                    goto out_peg_1760;
                                }
                            }
                            result_peg_1757.setValue((void*) " ");
                                
                        }
                        goto success_peg_1758;
                        out_peg_1760:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1757.getPosition()))){
                                    result_peg_1757.nextPosition();
                                } else {
                                    goto out_peg_1762;
                                }
                            }
                            result_peg_1757.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1758;
                        out_peg_1762:
                        goto loop_peg_1756;
                        success_peg_1758:
                        ;
                        result_peg_1725.addResult(result_peg_1757);
                    } while (true);
                    loop_peg_1756:
                    ;
                            
                }
                goto success_peg_1747;
                goto out_peg_1744;
                success_peg_1747:
                ;
            
            
            
            result_peg_1725 = rule_expr9(stream, result_peg_1725.getPosition());
                if (result_peg_1725.error()){
                    goto out_peg_1744;
                }
                right = result_peg_1725.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionEquals(left, right);
                    result_peg_1725.setValue(value);
                }
                new_left = result_peg_1725.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1725.getPosition();
        goto tail_peg_1726;
        out_peg_1744:
        Result result_peg_1765(myposition);
        {
        
            {
                    
                    result_peg_1765.reset();
                    do{
                        Result result_peg_1777(result_peg_1765.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1777.getPosition()))){
                                    result_peg_1777.nextPosition();
                                } else {
                                    goto out_peg_1780;
                                }
                            }
                            result_peg_1777.setValue((void*) " ");
                                
                        }
                        goto success_peg_1778;
                        out_peg_1780:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1777.getPosition()))){
                                    result_peg_1777.nextPosition();
                                } else {
                                    goto out_peg_1782;
                                }
                            }
                            result_peg_1777.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1778;
                        out_peg_1782:
                        goto loop_peg_1776;
                        success_peg_1778:
                        ;
                        result_peg_1765.addResult(result_peg_1777);
                    } while (true);
                    loop_peg_1776:
                    ;
                            
                }
                goto success_peg_1767;
                goto out_peg_1783;
                success_peg_1767:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_1765.getPosition()))){
                        result_peg_1765.nextPosition();
                    } else {
                        goto out_peg_1783;
                    }
                }
                result_peg_1765.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_1765.reset();
                    do{
                        Result result_peg_1796(result_peg_1765.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1796.getPosition()))){
                                    result_peg_1796.nextPosition();
                                } else {
                                    goto out_peg_1799;
                                }
                            }
                            result_peg_1796.setValue((void*) " ");
                                
                        }
                        goto success_peg_1797;
                        out_peg_1799:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1796.getPosition()))){
                                    result_peg_1796.nextPosition();
                                } else {
                                    goto out_peg_1801;
                                }
                            }
                            result_peg_1796.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1797;
                        out_peg_1801:
                        goto loop_peg_1795;
                        success_peg_1797:
                        ;
                        result_peg_1765.addResult(result_peg_1796);
                    } while (true);
                    loop_peg_1795:
                    ;
                            
                }
                goto success_peg_1786;
                goto out_peg_1783;
                success_peg_1786:
                ;
            
            
            
            result_peg_1765 = rule_expr9(stream, result_peg_1765.getPosition());
                if (result_peg_1765.error()){
                    goto out_peg_1783;
                }
                right = result_peg_1765.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionUnequals(left, right);
                    result_peg_1765.setValue(value);
                }
                new_left = result_peg_1765.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1765.getPosition();
        goto tail_peg_1726;
        out_peg_1783:
        Result result_peg_1804(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1804.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1804.getPosition());
        
        return result_peg_1804;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1850(stream, "expr9_rest");
    int myposition = position;
    
    tail_peg_1809:
    Value maker;
        Value right;
        Value new_left;
    Result result_peg_1808(myposition);
        {
        
            {
                    
                    result_peg_1808.reset();
                    do{
                        Result result_peg_1821(result_peg_1808.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1821.getPosition()))){
                                    result_peg_1821.nextPosition();
                                } else {
                                    goto out_peg_1824;
                                }
                            }
                            result_peg_1821.setValue((void*) " ");
                                
                        }
                        goto success_peg_1822;
                        out_peg_1824:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1821.getPosition()))){
                                    result_peg_1821.nextPosition();
                                } else {
                                    goto out_peg_1826;
                                }
                            }
                            result_peg_1821.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1822;
                        out_peg_1826:
                        goto loop_peg_1820;
                        success_peg_1822:
                        ;
                        result_peg_1808.addResult(result_peg_1821);
                    } while (true);
                    loop_peg_1820:
                    ;
                            
                }
                goto success_peg_1811;
                goto out_peg_1827;
                success_peg_1811:
                ;
            
            
            
            result_peg_1808 = rule_compare(stream, result_peg_1808.getPosition());
                if (result_peg_1808.error()){
                    goto out_peg_1827;
                }
                maker = result_peg_1808.getValues();
            
            
            
            {
                    
                    result_peg_1808.reset();
                    do{
                        Result result_peg_1840(result_peg_1808.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1840.getPosition()))){
                                    result_peg_1840.nextPosition();
                                } else {
                                    goto out_peg_1843;
                                }
                            }
                            result_peg_1840.setValue((void*) " ");
                                
                        }
                        goto success_peg_1841;
                        out_peg_1843:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1840.getPosition()))){
                                    result_peg_1840.nextPosition();
                                } else {
                                    goto out_peg_1845;
                                }
                            }
                            result_peg_1840.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1841;
                        out_peg_1845:
                        goto loop_peg_1839;
                        success_peg_1841:
                        ;
                        result_peg_1808.addResult(result_peg_1840);
                    } while (true);
                    loop_peg_1839:
                    ;
                            
                }
                goto success_peg_1830;
                goto out_peg_1827;
                success_peg_1830:
                ;
            
            
            
            result_peg_1808 = rule_expr10(stream, result_peg_1808.getPosition());
                if (result_peg_1808.error()){
                    goto out_peg_1827;
                }
                right = result_peg_1808.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    {
                            typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                            value = as<compare_func>(maker)(left, right);
                            }
                    result_peg_1808.setValue(value);
                }
                new_left = result_peg_1808.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1808.getPosition();
        goto tail_peg_1809;
        out_peg_1827:
        Result result_peg_1848(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1848.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1848.getPosition());
        
        return result_peg_1848;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1851 = stream.getColumn(position);
    if (column_peg_1851.chunk8 != 0 && column_peg_1851.chunk8->chunk_compare.calculated()){
        return column_peg_1851.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1864(stream, "compare");
    int myposition = position;
    
    
    
    Result result_peg_1852(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_1852.getPosition()))){
                        result_peg_1852.nextPosition();
                    } else {
                        goto out_peg_1854;
                    }
                }
                result_peg_1852.setValue((void*) "<=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThanEquals;
                    result_peg_1852.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1852;
        stream.update(result_peg_1852.getPosition());
        
        
        return result_peg_1852;
        out_peg_1854:
        Result result_peg_1855(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar(">="[i], stream.get(result_peg_1855.getPosition()))){
                        result_peg_1855.nextPosition();
                    } else {
                        goto out_peg_1857;
                    }
                }
                result_peg_1855.setValue((void*) ">=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThanEquals;
                    result_peg_1855.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1855;
        stream.update(result_peg_1855.getPosition());
        
        
        return result_peg_1855;
        out_peg_1857:
        Result result_peg_1858(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("<"[i], stream.get(result_peg_1858.getPosition()))){
                        result_peg_1858.nextPosition();
                    } else {
                        goto out_peg_1860;
                    }
                }
                result_peg_1858.setValue((void*) "<");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionLessThan;
                    result_peg_1858.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1858;
        stream.update(result_peg_1858.getPosition());
        
        
        return result_peg_1858;
        out_peg_1860:
        Result result_peg_1861(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_1861.getPosition()))){
                        result_peg_1861.nextPosition();
                    } else {
                        goto out_peg_1863;
                    }
                }
                result_peg_1861.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void *) makeExpressionGreaterThan;
                    result_peg_1861.setValue(value);
                }
            
            
        }
        
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = result_peg_1861;
        stream.update(result_peg_1861.getPosition());
        
        
        return result_peg_1861;
        out_peg_1863:
    
        if (column_peg_1851.chunk8 == 0){
            column_peg_1851.chunk8 = new Chunk8();
        }
        column_peg_1851.chunk8->chunk_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1865 = stream.getColumn(position);
    if (column_peg_1865.chunk8 != 0 && column_peg_1865.chunk8->chunk_all_compare.calculated()){
        return column_peg_1865.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1872(stream, "all_compare");
    int myposition = position;
    
    
    
    Result result_peg_1866(myposition);
        
        result_peg_1866 = rule_compare(stream, result_peg_1866.getPosition());
        if (result_peg_1866.error()){
            goto out_peg_1867;
        }
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1866;
        stream.update(result_peg_1866.getPosition());
        
        
        return result_peg_1866;
        out_peg_1867:
        Result result_peg_1868(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("="[i], stream.get(result_peg_1868.getPosition()))){
                result_peg_1868.nextPosition();
            } else {
                goto out_peg_1869;
            }
        }
        result_peg_1868.setValue((void*) "=");
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1868;
        stream.update(result_peg_1868.getPosition());
        
        
        return result_peg_1868;
        out_peg_1869:
        Result result_peg_1870(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("!="[i], stream.get(result_peg_1870.getPosition()))){
                result_peg_1870.nextPosition();
            } else {
                goto out_peg_1871;
            }
        }
        result_peg_1870.setValue((void*) "!=");
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1870;
        stream.update(result_peg_1870.getPosition());
        
        
        return result_peg_1870;
        out_peg_1871:
    
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1955(stream, "expr10_rest");
    int myposition = position;
    
    tail_peg_1875:
    Value right;
        Value new_left;
    Result result_peg_1874(myposition);
        {
        
            {
                    
                    result_peg_1874.reset();
                    do{
                        Result result_peg_1887(result_peg_1874.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1887.getPosition()))){
                                    result_peg_1887.nextPosition();
                                } else {
                                    goto out_peg_1890;
                                }
                            }
                            result_peg_1887.setValue((void*) " ");
                                
                        }
                        goto success_peg_1888;
                        out_peg_1890:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1887.getPosition()))){
                                    result_peg_1887.nextPosition();
                                } else {
                                    goto out_peg_1892;
                                }
                            }
                            result_peg_1887.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1888;
                        out_peg_1892:
                        goto loop_peg_1886;
                        success_peg_1888:
                        ;
                        result_peg_1874.addResult(result_peg_1887);
                    } while (true);
                    loop_peg_1886:
                    ;
                            
                }
                goto success_peg_1877;
                goto out_peg_1893;
                success_peg_1877:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1874.getPosition()))){
                        result_peg_1874.nextPosition();
                    } else {
                        goto out_peg_1893;
                    }
                }
                result_peg_1874.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_1874.reset();
                    do{
                        Result result_peg_1906(result_peg_1874.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1906.getPosition()))){
                                    result_peg_1906.nextPosition();
                                } else {
                                    goto out_peg_1909;
                                }
                            }
                            result_peg_1906.setValue((void*) " ");
                                
                        }
                        goto success_peg_1907;
                        out_peg_1909:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1906.getPosition()))){
                                    result_peg_1906.nextPosition();
                                } else {
                                    goto out_peg_1911;
                                }
                            }
                            result_peg_1906.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1907;
                        out_peg_1911:
                        goto loop_peg_1905;
                        success_peg_1907:
                        ;
                        result_peg_1874.addResult(result_peg_1906);
                    } while (true);
                    loop_peg_1905:
                    ;
                            
                }
                goto success_peg_1896;
                goto out_peg_1893;
                success_peg_1896:
                ;
            
            
            
            result_peg_1874 = rule_expr11(stream, result_peg_1874.getPosition());
                if (result_peg_1874.error()){
                    goto out_peg_1893;
                }
                right = result_peg_1874.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAdd(left, right);
                    result_peg_1874.setValue(value);
                }
                new_left = result_peg_1874.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1874.getPosition();
        goto tail_peg_1875;
        out_peg_1893:
        Result result_peg_1914(myposition);
        {
        
            {
                    
                    result_peg_1914.reset();
                    do{
                        Result result_peg_1926(result_peg_1914.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1926.getPosition()))){
                                    result_peg_1926.nextPosition();
                                } else {
                                    goto out_peg_1929;
                                }
                            }
                            result_peg_1926.setValue((void*) " ");
                                
                        }
                        goto success_peg_1927;
                        out_peg_1929:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1926.getPosition()))){
                                    result_peg_1926.nextPosition();
                                } else {
                                    goto out_peg_1931;
                                }
                            }
                            result_peg_1926.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1927;
                        out_peg_1931:
                        goto loop_peg_1925;
                        success_peg_1927:
                        ;
                        result_peg_1914.addResult(result_peg_1926);
                    } while (true);
                    loop_peg_1925:
                    ;
                            
                }
                goto success_peg_1916;
                goto out_peg_1932;
                success_peg_1916:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1914.getPosition()))){
                        result_peg_1914.nextPosition();
                    } else {
                        goto out_peg_1932;
                    }
                }
                result_peg_1914.setValue((void*) "-");
            
            
            
            {
                    
                    result_peg_1914.reset();
                    do{
                        Result result_peg_1945(result_peg_1914.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1945.getPosition()))){
                                    result_peg_1945.nextPosition();
                                } else {
                                    goto out_peg_1948;
                                }
                            }
                            result_peg_1945.setValue((void*) " ");
                                
                        }
                        goto success_peg_1946;
                        out_peg_1948:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1945.getPosition()))){
                                    result_peg_1945.nextPosition();
                                } else {
                                    goto out_peg_1950;
                                }
                            }
                            result_peg_1945.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1946;
                        out_peg_1950:
                        goto loop_peg_1944;
                        success_peg_1946:
                        ;
                        result_peg_1914.addResult(result_peg_1945);
                    } while (true);
                    loop_peg_1944:
                    ;
                            
                }
                goto success_peg_1935;
                goto out_peg_1932;
                success_peg_1935:
                ;
            
            
            
            result_peg_1914 = rule_expr11(stream, result_peg_1914.getPosition());
                if (result_peg_1914.error()){
                    goto out_peg_1932;
                }
                right = result_peg_1914.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionSubtract(left, right);
                    result_peg_1914.setValue(value);
                }
                new_left = result_peg_1914.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1914.getPosition();
        goto tail_peg_1875;
        out_peg_1932:
        Result result_peg_1953(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1953.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1953.getPosition());
        
        return result_peg_1953;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2077(stream, "expr11_rest");
    int myposition = position;
    
    tail_peg_1958:
    Value right;
        Value new_left;
    Result result_peg_1957(myposition);
        {
        
            {
                    
                    result_peg_1957.reset();
                    do{
                        Result result_peg_1970(result_peg_1957.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1970.getPosition()))){
                                    result_peg_1970.nextPosition();
                                } else {
                                    goto out_peg_1973;
                                }
                            }
                            result_peg_1970.setValue((void*) " ");
                                
                        }
                        goto success_peg_1971;
                        out_peg_1973:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1970.getPosition()))){
                                    result_peg_1970.nextPosition();
                                } else {
                                    goto out_peg_1975;
                                }
                            }
                            result_peg_1970.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1971;
                        out_peg_1975:
                        goto loop_peg_1969;
                        success_peg_1971:
                        ;
                        result_peg_1957.addResult(result_peg_1970);
                    } while (true);
                    loop_peg_1969:
                    ;
                            
                }
                goto success_peg_1960;
                goto out_peg_1976;
                success_peg_1960:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("*"[i], stream.get(result_peg_1957.getPosition()))){
                        result_peg_1957.nextPosition();
                    } else {
                        goto out_peg_1976;
                    }
                }
                result_peg_1957.setValue((void*) "*");
            
            
            
            {
                    
                    result_peg_1957.reset();
                    do{
                        Result result_peg_1989(result_peg_1957.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1989.getPosition()))){
                                    result_peg_1989.nextPosition();
                                } else {
                                    goto out_peg_1992;
                                }
                            }
                            result_peg_1989.setValue((void*) " ");
                                
                        }
                        goto success_peg_1990;
                        out_peg_1992:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1989.getPosition()))){
                                    result_peg_1989.nextPosition();
                                } else {
                                    goto out_peg_1994;
                                }
                            }
                            result_peg_1989.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1990;
                        out_peg_1994:
                        goto loop_peg_1988;
                        success_peg_1990:
                        ;
                        result_peg_1957.addResult(result_peg_1989);
                    } while (true);
                    loop_peg_1988:
                    ;
                            
                }
                goto success_peg_1979;
                goto out_peg_1976;
                success_peg_1979:
                ;
            
            
            
            result_peg_1957 = rule_expr12(stream, result_peg_1957.getPosition());
                if (result_peg_1957.error()){
                    goto out_peg_1976;
                }
                right = result_peg_1957.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionMultiply(left, right);
                    result_peg_1957.setValue(value);
                }
                new_left = result_peg_1957.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1957.getPosition();
        goto tail_peg_1958;
        out_peg_1976:
        Result result_peg_1997(myposition);
        {
        
            {
                    
                    result_peg_1997.reset();
                    do{
                        Result result_peg_2009(result_peg_1997.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2009.getPosition()))){
                                    result_peg_2009.nextPosition();
                                } else {
                                    goto out_peg_2012;
                                }
                            }
                            result_peg_2009.setValue((void*) " ");
                                
                        }
                        goto success_peg_2010;
                        out_peg_2012:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2009.getPosition()))){
                                    result_peg_2009.nextPosition();
                                } else {
                                    goto out_peg_2014;
                                }
                            }
                            result_peg_2009.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2010;
                        out_peg_2014:
                        goto loop_peg_2008;
                        success_peg_2010:
                        ;
                        result_peg_1997.addResult(result_peg_2009);
                    } while (true);
                    loop_peg_2008:
                    ;
                            
                }
                goto success_peg_1999;
                goto out_peg_2015;
                success_peg_1999:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_1997.getPosition()))){
                        result_peg_1997.nextPosition();
                    } else {
                        goto out_peg_2015;
                    }
                }
                result_peg_1997.setValue((void*) "/");
            
            
            
            {
                    
                    result_peg_1997.reset();
                    do{
                        Result result_peg_2028(result_peg_1997.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2028.getPosition()))){
                                    result_peg_2028.nextPosition();
                                } else {
                                    goto out_peg_2031;
                                }
                            }
                            result_peg_2028.setValue((void*) " ");
                                
                        }
                        goto success_peg_2029;
                        out_peg_2031:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2028.getPosition()))){
                                    result_peg_2028.nextPosition();
                                } else {
                                    goto out_peg_2033;
                                }
                            }
                            result_peg_2028.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2029;
                        out_peg_2033:
                        goto loop_peg_2027;
                        success_peg_2029:
                        ;
                        result_peg_1997.addResult(result_peg_2028);
                    } while (true);
                    loop_peg_2027:
                    ;
                            
                }
                goto success_peg_2018;
                goto out_peg_2015;
                success_peg_2018:
                ;
            
            
            
            result_peg_1997 = rule_expr12(stream, result_peg_1997.getPosition());
                if (result_peg_1997.error()){
                    goto out_peg_2015;
                }
                right = result_peg_1997.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionDivide(left, right);
                    result_peg_1997.setValue(value);
                }
                new_left = result_peg_1997.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1997.getPosition();
        goto tail_peg_1958;
        out_peg_2015:
        Result result_peg_2036(myposition);
        {
        
            {
                    
                    result_peg_2036.reset();
                    do{
                        Result result_peg_2048(result_peg_2036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2048.getPosition()))){
                                    result_peg_2048.nextPosition();
                                } else {
                                    goto out_peg_2051;
                                }
                            }
                            result_peg_2048.setValue((void*) " ");
                                
                        }
                        goto success_peg_2049;
                        out_peg_2051:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2048.getPosition()))){
                                    result_peg_2048.nextPosition();
                                } else {
                                    goto out_peg_2053;
                                }
                            }
                            result_peg_2048.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2049;
                        out_peg_2053:
                        goto loop_peg_2047;
                        success_peg_2049:
                        ;
                        result_peg_2036.addResult(result_peg_2048);
                    } while (true);
                    loop_peg_2047:
                    ;
                            
                }
                goto success_peg_2038;
                goto out_peg_2054;
                success_peg_2038:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("%"[i], stream.get(result_peg_2036.getPosition()))){
                        result_peg_2036.nextPosition();
                    } else {
                        goto out_peg_2054;
                    }
                }
                result_peg_2036.setValue((void*) "%");
            
            
            
            {
                    
                    result_peg_2036.reset();
                    do{
                        Result result_peg_2067(result_peg_2036.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2067.getPosition()))){
                                    result_peg_2067.nextPosition();
                                } else {
                                    goto out_peg_2070;
                                }
                            }
                            result_peg_2067.setValue((void*) " ");
                                
                        }
                        goto success_peg_2068;
                        out_peg_2070:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2067.getPosition()))){
                                    result_peg_2067.nextPosition();
                                } else {
                                    goto out_peg_2072;
                                }
                            }
                            result_peg_2067.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2068;
                        out_peg_2072:
                        goto loop_peg_2066;
                        success_peg_2068:
                        ;
                        result_peg_2036.addResult(result_peg_2067);
                    } while (true);
                    loop_peg_2066:
                    ;
                            
                }
                goto success_peg_2057;
                goto out_peg_2054;
                success_peg_2057:
                ;
            
            
            
            result_peg_2036 = rule_expr12(stream, result_peg_2036.getPosition());
                if (result_peg_2036.error()){
                    goto out_peg_2054;
                }
                right = result_peg_2036.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionModulo(left, right);
                    result_peg_2036.setValue(value);
                }
                new_left = result_peg_2036.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2036.getPosition();
        goto tail_peg_1958;
        out_peg_2054:
        Result result_peg_2075(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2075.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2075.getPosition());
        
        return result_peg_2075;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2121(stream, "expr12_rest");
    int myposition = position;
    
    tail_peg_2080:
    Value right;
        Value new_left;
    Result result_peg_2079(myposition);
        {
        
            {
                    
                    result_peg_2079.reset();
                    do{
                        Result result_peg_2092(result_peg_2079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2092.getPosition()))){
                                    result_peg_2092.nextPosition();
                                } else {
                                    goto out_peg_2095;
                                }
                            }
                            result_peg_2092.setValue((void*) " ");
                                
                        }
                        goto success_peg_2093;
                        out_peg_2095:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2092.getPosition()))){
                                    result_peg_2092.nextPosition();
                                } else {
                                    goto out_peg_2097;
                                }
                            }
                            result_peg_2092.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2093;
                        out_peg_2097:
                        goto loop_peg_2091;
                        success_peg_2093:
                        ;
                        result_peg_2079.addResult(result_peg_2092);
                    } while (true);
                    loop_peg_2091:
                    ;
                            
                }
                goto success_peg_2082;
                goto out_peg_2098;
                success_peg_2082:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("**"[i], stream.get(result_peg_2079.getPosition()))){
                        result_peg_2079.nextPosition();
                    } else {
                        goto out_peg_2098;
                    }
                }
                result_peg_2079.setValue((void*) "**");
            
            
            
            {
                    
                    result_peg_2079.reset();
                    do{
                        Result result_peg_2111(result_peg_2079.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2111.getPosition()))){
                                    result_peg_2111.nextPosition();
                                } else {
                                    goto out_peg_2114;
                                }
                            }
                            result_peg_2111.setValue((void*) " ");
                                
                        }
                        goto success_peg_2112;
                        out_peg_2114:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2111.getPosition()))){
                                    result_peg_2111.nextPosition();
                                } else {
                                    goto out_peg_2116;
                                }
                            }
                            result_peg_2111.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2112;
                        out_peg_2116:
                        goto loop_peg_2110;
                        success_peg_2112:
                        ;
                        result_peg_2079.addResult(result_peg_2111);
                    } while (true);
                    loop_peg_2110:
                    ;
                            
                }
                goto success_peg_2101;
                goto out_peg_2098;
                success_peg_2101:
                ;
            
            
            
            result_peg_2079 = rule_expr13(stream, result_peg_2079.getPosition());
                if (result_peg_2079.error()){
                    goto out_peg_2098;
                }
                right = result_peg_2079.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionPower(left, right);
                    result_peg_2079.setValue(value);
                }
                new_left = result_peg_2079.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2079.getPosition();
        goto tail_peg_2080;
        out_peg_2098:
        Result result_peg_2119(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2119.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2119.getPosition());
        
        return result_peg_2119;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_2122 = stream.getColumn(position);
    if (column_peg_2122.chunk9 != 0 && column_peg_2122.chunk9->chunk_unary.calculated()){
        return column_peg_2122.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_2140(stream, "unary");
    int myposition = position;
    
    
    
    Result result_peg_2123(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("!"[i], stream.get(result_peg_2123.getPosition()))){
                        result_peg_2123.nextPosition();
                    } else {
                        goto out_peg_2125;
                    }
                }
                result_peg_2123.setValue((void*) "!");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Not;
                    result_peg_2123.setValue(value);
                }
            
            
        }
        
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = result_peg_2123;
        stream.update(result_peg_2123.getPosition());
        
        
        return result_peg_2123;
        out_peg_2125:
        Result result_peg_2126(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2126.getPosition()))){
                        result_peg_2126.nextPosition();
                    } else {
                        goto out_peg_2128;
                    }
                }
                result_peg_2126.setValue((void*) "-");
            
            
            
            Result result_peg_2131(result_peg_2126);
                {
                    
                    char letter_peg_2136 = stream.get(result_peg_2131.getPosition());
                    if (letter_peg_2136 != '\0' && strchr("0123456789", letter_peg_2136) != NULL){
                        result_peg_2131.nextPosition();
                        result_peg_2131.setValue((void*) (long) letter_peg_2136);
                    } else {
                        goto out_peg_2135;
                    }
                    
                }
                goto success_peg_2132;
                out_peg_2135:
                goto not_peg_2130;
                success_peg_2132:
                ;
                goto out_peg_2128;
                not_peg_2130:
                result_peg_2126.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Minus;
                    result_peg_2126.setValue(value);
                }
            
            
        }
        
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = result_peg_2126;
        stream.update(result_peg_2126.getPosition());
        
        
        return result_peg_2126;
        out_peg_2128:
        Result result_peg_2137(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2137.getPosition()))){
                        result_peg_2137.nextPosition();
                    } else {
                        goto out_peg_2139;
                    }
                }
                result_peg_2137.setValue((void*) "~");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Negation;
                    result_peg_2137.setValue(value);
                }
            
            
        }
        
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = result_peg_2137;
        stream.update(result_peg_2137.getPosition());
        
        
        return result_peg_2137;
        out_peg_2139:
    
        if (column_peg_2122.chunk9 == 0){
            column_peg_2122.chunk9 = new Chunk9();
        }
        column_peg_2122.chunk9->chunk_unary = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_2141 = stream.getColumn(position);
    if (column_peg_2141.chunk9 != 0 && column_peg_2141.chunk9->chunk_expr13_real.calculated()){
        return column_peg_2141.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_2187(stream, "expr13_real");
    int myposition = position;
    
    
    Value e;
    Result result_peg_2142(myposition);
        
        result_peg_2142 = rule_helper(stream, result_peg_2142.getPosition());
        if (result_peg_2142.error()){
            goto out_peg_2143;
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2142;
        stream.update(result_peg_2142.getPosition());
        
        
        return result_peg_2142;
        out_peg_2143:
        Result result_peg_2144(myposition);
        
        result_peg_2144 = rule_function(stream, result_peg_2144.getPosition());
        if (result_peg_2144.error()){
            goto out_peg_2145;
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2144;
        stream.update(result_peg_2144.getPosition());
        
        
        return result_peg_2144;
        out_peg_2145:
        Result result_peg_2146(myposition);
        
        result_peg_2146 = rule_value(stream, result_peg_2146.getPosition());
        if (result_peg_2146.error()){
            goto out_peg_2147;
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2146;
        stream.update(result_peg_2146.getPosition());
        
        
        return result_peg_2146;
        out_peg_2147:
        Result result_peg_2148(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2148.getPosition()))){
                        result_peg_2148.nextPosition();
                    } else {
                        goto out_peg_2150;
                    }
                }
                result_peg_2148.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2148.reset();
                    do{
                        Result result_peg_2162(result_peg_2148.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2162.getPosition()))){
                                    result_peg_2162.nextPosition();
                                } else {
                                    goto out_peg_2165;
                                }
                            }
                            result_peg_2162.setValue((void*) " ");
                                
                        }
                        goto success_peg_2163;
                        out_peg_2165:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2162.getPosition()))){
                                    result_peg_2162.nextPosition();
                                } else {
                                    goto out_peg_2167;
                                }
                            }
                            result_peg_2162.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2163;
                        out_peg_2167:
                        goto loop_peg_2161;
                        success_peg_2163:
                        ;
                        result_peg_2148.addResult(result_peg_2162);
                    } while (true);
                    loop_peg_2161:
                    ;
                            
                }
                goto success_peg_2152;
                goto out_peg_2150;
                success_peg_2152:
                ;
            
            
            
            result_peg_2148 = rule_expr(stream, result_peg_2148.getPosition());
                if (result_peg_2148.error()){
                    goto out_peg_2150;
                }
                e = result_peg_2148.getValues();
            
            
            
            {
                    
                    result_peg_2148.reset();
                    do{
                        Result result_peg_2180(result_peg_2148.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2180.getPosition()))){
                                    result_peg_2180.nextPosition();
                                } else {
                                    goto out_peg_2183;
                                }
                            }
                            result_peg_2180.setValue((void*) " ");
                                
                        }
                        goto success_peg_2181;
                        out_peg_2183:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2180.getPosition()))){
                                    result_peg_2180.nextPosition();
                                } else {
                                    goto out_peg_2185;
                                }
                            }
                            result_peg_2180.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2181;
                        out_peg_2185:
                        goto loop_peg_2179;
                        success_peg_2181:
                        ;
                        result_peg_2148.addResult(result_peg_2180);
                    } while (true);
                    loop_peg_2179:
                    ;
                            
                }
                goto success_peg_2170;
                goto out_peg_2150;
                success_peg_2170:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2148.getPosition()))){
                        result_peg_2148.nextPosition();
                    } else {
                        goto out_peg_2150;
                    }
                }
                result_peg_2148.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = e;
                    result_peg_2148.setValue(value);
                }
            
            
        }
        
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = result_peg_2148;
        stream.update(result_peg_2148.getPosition());
        
        
        return result_peg_2148;
        out_peg_2150:
    
        if (column_peg_2141.chunk9 == 0){
            column_peg_2141.chunk9 = new Chunk9();
        }
        column_peg_2141.chunk9->chunk_expr13_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_2188 = stream.getColumn(position);
    if (column_peg_2188.chunk9 != 0 && column_peg_2188.chunk9->chunk_function.calculated()){
        return column_peg_2188.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2521(stream, "function");
    int myposition = position;
    
    
    Value name;
        Value args;
        Value arg1;
        Value id;
        Value comparison;
        Value state;
        Value attributes;
    Result result_peg_2189(myposition);
        
        {
        
            result_peg_2189 = rule_function_name(stream, result_peg_2189.getPosition());
                if (result_peg_2189.error()){
                    goto out_peg_2191;
                }
                name = result_peg_2189.getValues();
            
            
            
            {
                    
                    result_peg_2189.reset();
                    do{
                        Result result_peg_2203(result_peg_2189.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2203.getPosition()))){
                                    result_peg_2203.nextPosition();
                                } else {
                                    goto out_peg_2206;
                                }
                            }
                            result_peg_2203.setValue((void*) " ");
                                
                        }
                        goto success_peg_2204;
                        out_peg_2206:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2203.getPosition()))){
                                    result_peg_2203.nextPosition();
                                } else {
                                    goto out_peg_2208;
                                }
                            }
                            result_peg_2203.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2204;
                        out_peg_2208:
                        goto loop_peg_2202;
                        success_peg_2204:
                        ;
                        result_peg_2189.addResult(result_peg_2203);
                    } while (true);
                    loop_peg_2202:
                    ;
                            
                }
                goto success_peg_2193;
                goto out_peg_2191;
                success_peg_2193:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2189.getPosition()))){
                        result_peg_2189.nextPosition();
                    } else {
                        goto out_peg_2191;
                    }
                }
                result_peg_2189.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2189.reset();
                    do{
                        Result result_peg_2221(result_peg_2189.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2221.getPosition()))){
                                    result_peg_2221.nextPosition();
                                } else {
                                    goto out_peg_2224;
                                }
                            }
                            result_peg_2221.setValue((void*) " ");
                                
                        }
                        goto success_peg_2222;
                        out_peg_2224:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2221.getPosition()))){
                                    result_peg_2221.nextPosition();
                                } else {
                                    goto out_peg_2226;
                                }
                            }
                            result_peg_2221.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2222;
                        out_peg_2226:
                        goto loop_peg_2220;
                        success_peg_2222:
                        ;
                        result_peg_2189.addResult(result_peg_2221);
                    } while (true);
                    loop_peg_2220:
                    ;
                            
                }
                goto success_peg_2211;
                goto out_peg_2191;
                success_peg_2211:
                ;
            
            
            
            result_peg_2189 = rule_args(stream, result_peg_2189.getPosition());
                if (result_peg_2189.error()){
                    goto out_peg_2191;
                }
                args = result_peg_2189.getValues();
            
            
            
            {
                    
                    result_peg_2189.reset();
                    do{
                        Result result_peg_2239(result_peg_2189.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2239.getPosition()))){
                                    result_peg_2239.nextPosition();
                                } else {
                                    goto out_peg_2242;
                                }
                            }
                            result_peg_2239.setValue((void*) " ");
                                
                        }
                        goto success_peg_2240;
                        out_peg_2242:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2239.getPosition()))){
                                    result_peg_2239.nextPosition();
                                } else {
                                    goto out_peg_2244;
                                }
                            }
                            result_peg_2239.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2240;
                        out_peg_2244:
                        goto loop_peg_2238;
                        success_peg_2240:
                        ;
                        result_peg_2189.addResult(result_peg_2239);
                    } while (true);
                    loop_peg_2238:
                    ;
                            
                }
                goto success_peg_2229;
                goto out_peg_2191;
                success_peg_2229:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2189.getPosition()))){
                        result_peg_2189.nextPosition();
                    } else {
                        goto out_peg_2191;
                    }
                }
                result_peg_2189.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction(name, args);
                    result_peg_2189.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2189;
        stream.update(result_peg_2189.getPosition());
        
        
        return result_peg_2189;
        out_peg_2191:
        Result result_peg_2246(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2246.getPosition()))){
                        result_peg_2246.nextPosition();
                    } else {
                        goto out_peg_2248;
                    }
                }
                result_peg_2246.setValue((void*) "teammode");
                name = result_peg_2246.getValues();
            
            
            
            {
                    
                    result_peg_2246.reset();
                    do{
                        Result result_peg_2260(result_peg_2246.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2260.getPosition()))){
                                    result_peg_2260.nextPosition();
                                } else {
                                    goto out_peg_2263;
                                }
                            }
                            result_peg_2260.setValue((void*) " ");
                                
                        }
                        goto success_peg_2261;
                        out_peg_2263:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2260.getPosition()))){
                                    result_peg_2260.nextPosition();
                                } else {
                                    goto out_peg_2265;
                                }
                            }
                            result_peg_2260.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2261;
                        out_peg_2265:
                        goto loop_peg_2259;
                        success_peg_2261:
                        ;
                        result_peg_2246.addResult(result_peg_2260);
                    } while (true);
                    loop_peg_2259:
                    ;
                            
                }
                goto success_peg_2250;
                goto out_peg_2248;
                success_peg_2250:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2246.getPosition()))){
                        result_peg_2246.nextPosition();
                    } else {
                        goto out_peg_2248;
                    }
                }
                result_peg_2246.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2246.reset();
                    do{
                        Result result_peg_2278(result_peg_2246.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2278.getPosition()))){
                                    result_peg_2278.nextPosition();
                                } else {
                                    goto out_peg_2281;
                                }
                            }
                            result_peg_2278.setValue((void*) " ");
                                
                        }
                        goto success_peg_2279;
                        out_peg_2281:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2278.getPosition()))){
                                    result_peg_2278.nextPosition();
                                } else {
                                    goto out_peg_2283;
                                }
                            }
                            result_peg_2278.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2279;
                        out_peg_2283:
                        goto loop_peg_2277;
                        success_peg_2279:
                        ;
                        result_peg_2246.addResult(result_peg_2278);
                    } while (true);
                    loop_peg_2277:
                    ;
                            
                }
                goto success_peg_2268;
                goto out_peg_2248;
                success_peg_2268:
                ;
            
            
            
            result_peg_2246 = rule_identifier(stream, result_peg_2246.getPosition());
                if (result_peg_2246.error()){
                    goto out_peg_2248;
                }
                arg1 = result_peg_2246.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(name, arg1);
                    result_peg_2246.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2246;
        stream.update(result_peg_2246.getPosition());
        
        
        return result_peg_2246;
        out_peg_2248:
        Result result_peg_2285(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2285.getPosition()))){
                        result_peg_2285.nextPosition();
                    } else {
                        goto out_peg_2287;
                    }
                }
                result_peg_2285.setValue((void*) "teammode");
                name = result_peg_2285.getValues();
            
            
            
            {
                    
                    result_peg_2285.reset();
                    do{
                        Result result_peg_2299(result_peg_2285.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2299.getPosition()))){
                                    result_peg_2299.nextPosition();
                                } else {
                                    goto out_peg_2302;
                                }
                            }
                            result_peg_2299.setValue((void*) " ");
                                
                        }
                        goto success_peg_2300;
                        out_peg_2302:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2299.getPosition()))){
                                    result_peg_2299.nextPosition();
                                } else {
                                    goto out_peg_2304;
                                }
                            }
                            result_peg_2299.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2300;
                        out_peg_2304:
                        goto loop_peg_2298;
                        success_peg_2300:
                        ;
                        result_peg_2285.addResult(result_peg_2299);
                    } while (true);
                    loop_peg_2298:
                    ;
                            
                }
                goto success_peg_2289;
                goto out_peg_2287;
                success_peg_2289:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_2285.getPosition()))){
                        result_peg_2285.nextPosition();
                    } else {
                        goto out_peg_2287;
                    }
                }
                result_peg_2285.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_2285.reset();
                    do{
                        Result result_peg_2317(result_peg_2285.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2317.getPosition()))){
                                    result_peg_2317.nextPosition();
                                } else {
                                    goto out_peg_2320;
                                }
                            }
                            result_peg_2317.setValue((void*) " ");
                                
                        }
                        goto success_peg_2318;
                        out_peg_2320:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2317.getPosition()))){
                                    result_peg_2317.nextPosition();
                                } else {
                                    goto out_peg_2322;
                                }
                            }
                            result_peg_2317.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2318;
                        out_peg_2322:
                        goto loop_peg_2316;
                        success_peg_2318:
                        ;
                        result_peg_2285.addResult(result_peg_2317);
                    } while (true);
                    loop_peg_2316:
                    ;
                            
                }
                goto success_peg_2307;
                goto out_peg_2287;
                success_peg_2307:
                ;
            
            
            
            result_peg_2285 = rule_identifier(stream, result_peg_2285.getPosition());
                if (result_peg_2285.error()){
                    goto out_peg_2287;
                }
                arg1 = result_peg_2285.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1("teammode!=", arg1);
                    result_peg_2285.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2285;
        stream.update(result_peg_2285.getPosition());
        
        
        return result_peg_2285;
        out_peg_2287:
        Result result_peg_2324(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2324.getPosition()))){
                        result_peg_2324.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2324.setValue((void*) "animelem");
                name = result_peg_2324.getValues();
            
            
            
            {
                    
                    result_peg_2324.reset();
                    do{
                        Result result_peg_2338(result_peg_2324.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2338.getPosition()))){
                                    result_peg_2338.nextPosition();
                                } else {
                                    goto out_peg_2341;
                                }
                            }
                            result_peg_2338.setValue((void*) " ");
                                
                        }
                        goto success_peg_2339;
                        out_peg_2341:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2338.getPosition()))){
                                    result_peg_2338.nextPosition();
                                } else {
                                    goto out_peg_2343;
                                }
                            }
                            result_peg_2338.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2339;
                        out_peg_2343:
                        goto loop_peg_2337;
                        success_peg_2339:
                        ;
                        result_peg_2324.addResult(result_peg_2338);
                    } while (true);
                    loop_peg_2337:
                    ;
                            
                }
                goto success_peg_2328;
                goto out_peg_2326;
                success_peg_2328:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2324.getPosition()))){
                        result_peg_2324.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2324.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2324.reset();
                    do{
                        Result result_peg_2356(result_peg_2324.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2356.getPosition()))){
                                    result_peg_2356.nextPosition();
                                } else {
                                    goto out_peg_2359;
                                }
                            }
                            result_peg_2356.setValue((void*) " ");
                                
                        }
                        goto success_peg_2357;
                        out_peg_2359:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2356.getPosition()))){
                                    result_peg_2356.nextPosition();
                                } else {
                                    goto out_peg_2361;
                                }
                            }
                            result_peg_2356.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2357;
                        out_peg_2361:
                        goto loop_peg_2355;
                        success_peg_2357:
                        ;
                        result_peg_2324.addResult(result_peg_2356);
                    } while (true);
                    loop_peg_2355:
                    ;
                            
                }
                goto success_peg_2346;
                goto out_peg_2326;
                success_peg_2346:
                ;
            
            
            
            result_peg_2324 = rule_integer(stream, result_peg_2324.getPosition());
                if (result_peg_2324.error()){
                    goto out_peg_2326;
                }
                arg1 = result_peg_2324.getValues();
            
            
            
            result_peg_2324 = rule_function_rest(stream, result_peg_2324.getPosition());
                if (result_peg_2324.error()){
                    goto out_peg_2326;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    value = makeFunction1(name, arg1);
                    result_peg_2324.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2324;
        stream.update(result_peg_2324.getPosition());
        
        
        return result_peg_2324;
        out_peg_2326:
        Result result_peg_2364(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2364.getPosition()))){
                        result_peg_2364.nextPosition();
                    } else {
                        goto out_peg_2366;
                    }
                }
                result_peg_2364.setValue((void*) "animelem");
                name = result_peg_2364.getValues();
            
            
            
            {
                    
                    result_peg_2364.reset();
                    do{
                        Result result_peg_2378(result_peg_2364.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2378.getPosition()))){
                                    result_peg_2378.nextPosition();
                                } else {
                                    goto out_peg_2381;
                                }
                            }
                            result_peg_2378.setValue((void*) " ");
                                
                        }
                        goto success_peg_2379;
                        out_peg_2381:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2378.getPosition()))){
                                    result_peg_2378.nextPosition();
                                } else {
                                    goto out_peg_2383;
                                }
                            }
                            result_peg_2378.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2379;
                        out_peg_2383:
                        goto loop_peg_2377;
                        success_peg_2379:
                        ;
                        result_peg_2364.addResult(result_peg_2378);
                    } while (true);
                    loop_peg_2377:
                    ;
                            
                }
                goto success_peg_2368;
                goto out_peg_2366;
                success_peg_2368:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("<="[i], stream.get(result_peg_2364.getPosition()))){
                        result_peg_2364.nextPosition();
                    } else {
                        goto out_peg_2366;
                    }
                }
                result_peg_2364.setValue((void*) "<=");
            
            
            
            {
                    
                    result_peg_2364.reset();
                    do{
                        Result result_peg_2396(result_peg_2364.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2396.getPosition()))){
                                    result_peg_2396.nextPosition();
                                } else {
                                    goto out_peg_2399;
                                }
                            }
                            result_peg_2396.setValue((void*) " ");
                                
                        }
                        goto success_peg_2397;
                        out_peg_2399:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2396.getPosition()))){
                                    result_peg_2396.nextPosition();
                                } else {
                                    goto out_peg_2401;
                                }
                            }
                            result_peg_2396.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2397;
                        out_peg_2401:
                        goto loop_peg_2395;
                        success_peg_2397:
                        ;
                        result_peg_2364.addResult(result_peg_2396);
                    } while (true);
                    loop_peg_2395:
                    ;
                            
                }
                goto success_peg_2386;
                goto out_peg_2366;
                success_peg_2386:
                ;
            
            
            
            result_peg_2364 = rule_integer(stream, result_peg_2364.getPosition());
                if (result_peg_2364.error()){
                    goto out_peg_2366;
                }
                arg1 = result_peg_2364.getValues();
            
            
            
            result_peg_2364 = rule_function_rest(stream, result_peg_2364.getPosition());
                if (result_peg_2364.error()){
                    goto out_peg_2366;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    value = makeFunction1("animelem<=", arg1);
                    result_peg_2364.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2364;
        stream.update(result_peg_2364.getPosition());
        
        
        return result_peg_2364;
        out_peg_2366:
        Result result_peg_2404(myposition);
        
        {
        
            for (int i = 0; i < 11; i++){
                    if (compareCharCase("projcontact"[i], stream.get(result_peg_2404.getPosition()))){
                        result_peg_2404.nextPosition();
                    } else {
                        goto out_peg_2406;
                    }
                }
                result_peg_2404.setValue((void*) "projcontact");
                name = result_peg_2404.getValues();
            
            
            
            int save_peg_2408 = result_peg_2404.getPosition();
                
                result_peg_2404 = rule_integer(stream, result_peg_2404.getPosition());
                if (result_peg_2404.error()){
                    
                    result_peg_2404 = Result(save_peg_2408);
                    result_peg_2404.setValue((void*) 0);
                    
                }
                id = result_peg_2404.getValues();
            
            
            
            {
                    
                    result_peg_2404.reset();
                    do{
                        Result result_peg_2420(result_peg_2404.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2420.getPosition()))){
                                    result_peg_2420.nextPosition();
                                } else {
                                    goto out_peg_2423;
                                }
                            }
                            result_peg_2420.setValue((void*) " ");
                                
                        }
                        goto success_peg_2421;
                        out_peg_2423:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2420.getPosition()))){
                                    result_peg_2420.nextPosition();
                                } else {
                                    goto out_peg_2425;
                                }
                            }
                            result_peg_2420.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2421;
                        out_peg_2425:
                        goto loop_peg_2419;
                        success_peg_2421:
                        ;
                        result_peg_2404.addResult(result_peg_2420);
                    } while (true);
                    loop_peg_2419:
                    ;
                            
                }
                goto success_peg_2410;
                goto out_peg_2406;
                success_peg_2410:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2404.getPosition()))){
                        result_peg_2404.nextPosition();
                    } else {
                        goto out_peg_2406;
                    }
                }
                result_peg_2404.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2404.reset();
                    do{
                        Result result_peg_2438(result_peg_2404.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2438.getPosition()))){
                                    result_peg_2438.nextPosition();
                                } else {
                                    goto out_peg_2441;
                                }
                            }
                            result_peg_2438.setValue((void*) " ");
                                
                        }
                        goto success_peg_2439;
                        out_peg_2441:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2438.getPosition()))){
                                    result_peg_2438.nextPosition();
                                } else {
                                    goto out_peg_2443;
                                }
                            }
                            result_peg_2438.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2439;
                        out_peg_2443:
                        goto loop_peg_2437;
                        success_peg_2439:
                        ;
                        result_peg_2404.addResult(result_peg_2438);
                    } while (true);
                    loop_peg_2437:
                    ;
                            
                }
                goto success_peg_2428;
                goto out_peg_2406;
                success_peg_2428:
                ;
            
            
            
            result_peg_2404 = rule_integer(stream, result_peg_2404.getPosition());
                if (result_peg_2404.error()){
                    goto out_peg_2406;
                }
                arg1 = result_peg_2404.getValues();
            
            
            
            result_peg_2404 = rule_function_rest(stream, result_peg_2404.getPosition());
                if (result_peg_2404.error()){
                    goto out_peg_2406;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    std::string name = combine("projcontact", id);
                                    value = makeFunction(name, arg1);
                    result_peg_2404.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2404;
        stream.update(result_peg_2404.getPosition());
        
        
        return result_peg_2404;
        out_peg_2406:
        Result result_peg_2446(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("hitdefattr"[i], stream.get(result_peg_2446.getPosition()))){
                        result_peg_2446.nextPosition();
                    } else {
                        goto out_peg_2448;
                    }
                }
                result_peg_2446.setValue((void*) "hitdefattr");
                name = result_peg_2446.getValues();
            
            
            
            {
                    
                    result_peg_2446.reset();
                    do{
                        Result result_peg_2460(result_peg_2446.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2460.getPosition()))){
                                    result_peg_2460.nextPosition();
                                } else {
                                    goto out_peg_2463;
                                }
                            }
                            result_peg_2460.setValue((void*) " ");
                                
                        }
                        goto success_peg_2461;
                        out_peg_2463:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2460.getPosition()))){
                                    result_peg_2460.nextPosition();
                                } else {
                                    goto out_peg_2465;
                                }
                            }
                            result_peg_2460.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2461;
                        out_peg_2465:
                        goto loop_peg_2459;
                        success_peg_2461:
                        ;
                        result_peg_2446.addResult(result_peg_2460);
                    } while (true);
                    loop_peg_2459:
                    ;
                            
                }
                goto success_peg_2450;
                goto out_peg_2448;
                success_peg_2450:
                ;
            
            
            
            result_peg_2446 = rule_compare_equal(stream, result_peg_2446.getPosition());
                if (result_peg_2446.error()){
                    goto out_peg_2448;
                }
                comparison = result_peg_2446.getValues();
            
            
            
            {
                    
                    result_peg_2446.reset();
                    do{
                        Result result_peg_2478(result_peg_2446.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2478.getPosition()))){
                                    result_peg_2478.nextPosition();
                                } else {
                                    goto out_peg_2481;
                                }
                            }
                            result_peg_2478.setValue((void*) " ");
                                
                        }
                        goto success_peg_2479;
                        out_peg_2481:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2478.getPosition()))){
                                    result_peg_2478.nextPosition();
                                } else {
                                    goto out_peg_2483;
                                }
                            }
                            result_peg_2478.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2479;
                        out_peg_2483:
                        goto loop_peg_2477;
                        success_peg_2479:
                        ;
                        result_peg_2446.addResult(result_peg_2478);
                    } while (true);
                    loop_peg_2477:
                    ;
                            
                }
                goto success_peg_2468;
                goto out_peg_2448;
                success_peg_2468:
                ;
            
            
            
            result_peg_2446 = rule_hitdef__attribute(stream, result_peg_2446.getPosition());
                if (result_peg_2446.error()){
                    goto out_peg_2448;
                }
                state = result_peg_2446.getValues();
            
            
            
            {
                    
                    result_peg_2446.reset();
                    do{
                        Result result_peg_2496(result_peg_2446.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2496.getPosition()))){
                                    result_peg_2496.nextPosition();
                                } else {
                                    goto out_peg_2499;
                                }
                            }
                            result_peg_2496.setValue((void*) " ");
                                
                        }
                        goto success_peg_2497;
                        out_peg_2499:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2496.getPosition()))){
                                    result_peg_2496.nextPosition();
                                } else {
                                    goto out_peg_2501;
                                }
                            }
                            result_peg_2496.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2497;
                        out_peg_2501:
                        goto loop_peg_2495;
                        success_peg_2497:
                        ;
                        result_peg_2446.addResult(result_peg_2496);
                    } while (true);
                    loop_peg_2495:
                    ;
                            
                }
                goto success_peg_2486;
                goto out_peg_2448;
                success_peg_2486:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2446.getPosition()))){
                        result_peg_2446.nextPosition();
                    } else {
                        goto out_peg_2448;
                    }
                }
                result_peg_2446.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2446.reset();
                    do{
                        Result result_peg_2514(result_peg_2446.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2514.getPosition()))){
                                    result_peg_2514.nextPosition();
                                } else {
                                    goto out_peg_2517;
                                }
                            }
                            result_peg_2514.setValue((void*) " ");
                                
                        }
                        goto success_peg_2515;
                        out_peg_2517:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2514.getPosition()))){
                                    result_peg_2514.nextPosition();
                                } else {
                                    goto out_peg_2519;
                                }
                            }
                            result_peg_2514.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2515;
                        out_peg_2519:
                        goto loop_peg_2513;
                        success_peg_2515:
                        ;
                        result_peg_2446.addResult(result_peg_2514);
                    } while (true);
                    loop_peg_2513:
                    ;
                            
                }
                goto success_peg_2504;
                goto out_peg_2448;
                success_peg_2504:
                ;
            
            
            
            result_peg_2446 = rule_hitdef__attack__attribute(stream, result_peg_2446.getPosition());
                if (result_peg_2446.error()){
                    goto out_peg_2448;
                }
                attributes = result_peg_2446.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                                     Ast::Value * compare_state = as<compare_func>(comparison)(makeSimpleIdentifier("hitdefattr:state"), state);
                                     Ast::Value * compare_attribute = as<compare_func>(comparison)(makeSimpleIdentifier("hitdefattr:attribute"), attributes);
                                     value = makeExpressionAnd(compare_state, compare_attribute);
                    result_peg_2446.setValue(value);
                }
            
            
        }
        
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = result_peg_2446;
        stream.update(result_peg_2446.getPosition());
        
        
        return result_peg_2446;
        out_peg_2448:
    
        if (column_peg_2188.chunk9 == 0){
            column_peg_2188.chunk9 = new Chunk9();
        }
        column_peg_2188.chunk9->chunk_function = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_compare_equal(Stream & stream, const int position){
    
    Column & column_peg_2522 = stream.getColumn(position);
    if (column_peg_2522.chunk9 != 0 && column_peg_2522.chunk9->chunk_compare_equal.calculated()){
        return column_peg_2522.chunk9->chunk_compare_equal;
    }
    
    RuleTrace trace_peg_2529(stream, "compare_equal");
    int myposition = position;
    
    
    
    Result result_peg_2523(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2523.getPosition()))){
                        result_peg_2523.nextPosition();
                    } else {
                        goto out_peg_2525;
                    }
                }
                result_peg_2523.setValue((void*) "=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) makeExpressionEquals;
                    result_peg_2523.setValue(value);
                }
            
            
        }
        
        if (column_peg_2522.chunk9 == 0){
            column_peg_2522.chunk9 = new Chunk9();
        }
        column_peg_2522.chunk9->chunk_compare_equal = result_peg_2523;
        stream.update(result_peg_2523.getPosition());
        
        
        return result_peg_2523;
        out_peg_2525:
        Result result_peg_2526(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_2526.getPosition()))){
                        result_peg_2526.nextPosition();
                    } else {
                        goto out_peg_2528;
                    }
                }
                result_peg_2526.setValue((void*) "!=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) makeExpressionUnequals;
                    result_peg_2526.setValue(value);
                }
            
            
        }
        
        if (column_peg_2522.chunk9 == 0){
            column_peg_2522.chunk9 = new Chunk9();
        }
        column_peg_2522.chunk9->chunk_compare_equal = result_peg_2526;
        stream.update(result_peg_2526.getPosition());
        
        
        return result_peg_2526;
        out_peg_2528:
    
        if (column_peg_2522.chunk9 == 0){
            column_peg_2522.chunk9 = new Chunk9();
        }
        column_peg_2522.chunk9->chunk_compare_equal = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute(Stream & stream, const int position){
    
    Column & column_peg_2530 = stream.getColumn(position);
    if (column_peg_2530.chunk10 != 0 && column_peg_2530.chunk10->chunk_hitdef__attack__attribute.calculated()){
        return column_peg_2530.chunk10->chunk_hitdef__attack__attribute;
    }
    
    RuleTrace trace_peg_2573(stream, "hitdef__attack__attribute");
    int myposition = position;
    
    
    Value attribute;
    Result result_peg_2531(myposition);
        
        {
        
            {
                    Value value((void*) 0);
                    value = makeHitDefAttackAttribute();
                    result_peg_2531.setValue(value);
                }
                attribute = result_peg_2531.getValues();
            
            
            
            result_peg_2531 = rule_hitdef__attack__attribute__item(stream, result_peg_2531.getPosition(), attribute);
                if (result_peg_2531.error()){
                    goto out_peg_2534;
                }
            
            
            
            result_peg_2531.reset();
                do{
                    Result result_peg_2537(result_peg_2531.getPosition());
                    {
                    
                        {
                                
                                result_peg_2537.reset();
                                do{
                                    Result result_peg_2549(result_peg_2537.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2549.getPosition()))){
                                                result_peg_2549.nextPosition();
                                            } else {
                                                goto out_peg_2552;
                                            }
                                        }
                                        result_peg_2549.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2550;
                                    out_peg_2552:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2549.getPosition()))){
                                                result_peg_2549.nextPosition();
                                            } else {
                                                goto out_peg_2554;
                                            }
                                        }
                                        result_peg_2549.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2550;
                                    out_peg_2554:
                                    goto loop_peg_2548;
                                    success_peg_2550:
                                    ;
                                    result_peg_2537.addResult(result_peg_2549);
                                } while (true);
                                loop_peg_2548:
                                ;
                                        
                            }
                            goto success_peg_2539;
                            goto loop_peg_2536;
                            success_peg_2539:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2537.getPosition()))){
                                    result_peg_2537.nextPosition();
                                } else {
                                    goto loop_peg_2536;
                                }
                            }
                            result_peg_2537.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2537.reset();
                                do{
                                    Result result_peg_2567(result_peg_2537.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2567.getPosition()))){
                                                result_peg_2567.nextPosition();
                                            } else {
                                                goto out_peg_2570;
                                            }
                                        }
                                        result_peg_2567.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2568;
                                    out_peg_2570:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2567.getPosition()))){
                                                result_peg_2567.nextPosition();
                                            } else {
                                                goto out_peg_2572;
                                            }
                                        }
                                        result_peg_2567.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2568;
                                    out_peg_2572:
                                    goto loop_peg_2566;
                                    success_peg_2568:
                                    ;
                                    result_peg_2537.addResult(result_peg_2567);
                                } while (true);
                                loop_peg_2566:
                                ;
                                        
                            }
                            goto success_peg_2557;
                            goto loop_peg_2536;
                            success_peg_2557:
                            ;
                        
                        
                        
                        result_peg_2537 = rule_hitdef__attack__attribute__item(stream, result_peg_2537.getPosition(), attribute);
                            if (result_peg_2537.error()){
                                goto loop_peg_2536;
                            }
                        
                        
                    }
                    result_peg_2531.addResult(result_peg_2537);
                } while (true);
                loop_peg_2536:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::HitDefAttackAttribute*>(attribute)->reset(); value = attribute;
                    result_peg_2531.setValue(value);
                }
            
            
        }
        
        if (column_peg_2530.chunk10 == 0){
            column_peg_2530.chunk10 = new Chunk10();
        }
        column_peg_2530.chunk10->chunk_hitdef__attack__attribute = result_peg_2531;
        stream.update(result_peg_2531.getPosition());
        
        
        return result_peg_2531;
        out_peg_2534:
    
        if (column_peg_2530.chunk10 == 0){
            column_peg_2530.chunk10 = new Chunk10();
        }
        column_peg_2530.chunk10->chunk_hitdef__attack__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute__item(Stream & stream, const int position, Value attribute){
    
    RuleTrace trace_peg_2579(stream, "hitdef__attack__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_2575(myposition);
        
        {
        
            result_peg_2575 = rule_hitdef__attack__type(stream, result_peg_2575.getPosition());
                if (result_peg_2575.error()){
                    goto out_peg_2577;
                }
            
            Result result_peg_2576 = result_peg_2575;
            
            result_peg_2575 = rule_hitdef__attack__movement(stream, result_peg_2575.getPosition());
                if (result_peg_2575.error()){
                    goto out_peg_2577;
                }
            
            Result result_peg_2578 = result_peg_2575;
            
            {
                    Value value((void*) 0);
                    std::string * result = toString(as<const char *>(result_peg_2576.getValues()));
                            std::string * other = toString(as<const char *>(result_peg_2578.getValues()));
                            as<Ast::HitDefAttackAttribute*>(attribute)->addAttribute(*result + *other);
                    result_peg_2575.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2575.getPosition());
        
        return result_peg_2575;
        out_peg_2577:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_hitdef__attack__type(Stream & stream, const int position){
    
    Column & column_peg_2580 = stream.getColumn(position);
    if (column_peg_2580.chunk10 != 0 && column_peg_2580.chunk10->chunk_hitdef__attack__type.calculated()){
        return column_peg_2580.chunk10->chunk_hitdef__attack__type;
    }
    
    RuleTrace trace_peg_2587(stream, "hitdef__attack__type");
    int myposition = position;
    
    
    
    Result result_peg_2581(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("n"[i], stream.get(result_peg_2581.getPosition()))){
                result_peg_2581.nextPosition();
            } else {
                goto out_peg_2582;
            }
        }
        result_peg_2581.setValue((void*) "n");
        
        if (column_peg_2580.chunk10 == 0){
            column_peg_2580.chunk10 = new Chunk10();
        }
        column_peg_2580.chunk10->chunk_hitdef__attack__type = result_peg_2581;
        stream.update(result_peg_2581.getPosition());
        
        
        return result_peg_2581;
        out_peg_2582:
        Result result_peg_2583(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("s"[i], stream.get(result_peg_2583.getPosition()))){
                result_peg_2583.nextPosition();
            } else {
                goto out_peg_2584;
            }
        }
        result_peg_2583.setValue((void*) "s");
        
        if (column_peg_2580.chunk10 == 0){
            column_peg_2580.chunk10 = new Chunk10();
        }
        column_peg_2580.chunk10->chunk_hitdef__attack__type = result_peg_2583;
        stream.update(result_peg_2583.getPosition());
        
        
        return result_peg_2583;
        out_peg_2584:
        Result result_peg_2585(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("h"[i], stream.get(result_peg_2585.getPosition()))){
                result_peg_2585.nextPosition();
            } else {
                goto out_peg_2586;
            }
        }
        result_peg_2585.setValue((void*) "h");
        
        if (column_peg_2580.chunk10 == 0){
            column_peg_2580.chunk10 = new Chunk10();
        }
        column_peg_2580.chunk10->chunk_hitdef__attack__type = result_peg_2585;
        stream.update(result_peg_2585.getPosition());
        
        
        return result_peg_2585;
        out_peg_2586:
    
        if (column_peg_2580.chunk10 == 0){
            column_peg_2580.chunk10 = new Chunk10();
        }
        column_peg_2580.chunk10->chunk_hitdef__attack__type = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__movement(Stream & stream, const int position){
    
    Column & column_peg_2588 = stream.getColumn(position);
    if (column_peg_2588.chunk10 != 0 && column_peg_2588.chunk10->chunk_hitdef__attack__movement.calculated()){
        return column_peg_2588.chunk10->chunk_hitdef__attack__movement;
    }
    
    RuleTrace trace_peg_2593(stream, "hitdef__attack__movement");
    int myposition = position;
    
    
    
    Result result_peg_2589(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("a"[i], stream.get(result_peg_2589.getPosition()))){
                result_peg_2589.nextPosition();
            } else {
                goto out_peg_2590;
            }
        }
        result_peg_2589.setValue((void*) "a");
        
        if (column_peg_2588.chunk10 == 0){
            column_peg_2588.chunk10 = new Chunk10();
        }
        column_peg_2588.chunk10->chunk_hitdef__attack__movement = result_peg_2589;
        stream.update(result_peg_2589.getPosition());
        
        
        return result_peg_2589;
        out_peg_2590:
        Result result_peg_2591(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("t"[i], stream.get(result_peg_2591.getPosition()))){
                result_peg_2591.nextPosition();
            } else {
                goto out_peg_2592;
            }
        }
        result_peg_2591.setValue((void*) "t");
        
        if (column_peg_2588.chunk10 == 0){
            column_peg_2588.chunk10 = new Chunk10();
        }
        column_peg_2588.chunk10->chunk_hitdef__attack__movement = result_peg_2591;
        stream.update(result_peg_2591.getPosition());
        
        
        return result_peg_2591;
        out_peg_2592:
    
        if (column_peg_2588.chunk10 == 0){
            column_peg_2588.chunk10 = new Chunk10();
        }
        column_peg_2588.chunk10->chunk_hitdef__attack__movement = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attribute(Stream & stream, const int position){
    
    Column & column_peg_2594 = stream.getColumn(position);
    if (column_peg_2594.chunk10 != 0 && column_peg_2594.chunk10->chunk_hitdef__attribute.calculated()){
        return column_peg_2594.chunk10->chunk_hitdef__attribute;
    }
    
    RuleTrace trace_peg_2600(stream, "hitdef__attribute");
    int myposition = position;
    
    
    
    Result result_peg_2595(myposition);
        
        {
        
            result_peg_2595.reset();
                do{
                    Result result_peg_2598(result_peg_2595.getPosition());
                    result_peg_2598 = rule_hitdef__attribute__item(stream, result_peg_2598.getPosition());
                    if (result_peg_2598.error()){
                        goto loop_peg_2597;
                    }
                    result_peg_2595.addResult(result_peg_2598);
                } while (true);
                loop_peg_2597:
                if (result_peg_2595.matches() == 0){
                    goto out_peg_2599;
                }
            
            Result result_peg_2596 = result_peg_2595;
            
            {
                    Value value((void*) 0);
                    value = makeHitDefAttribute(result_peg_2596.getValues());
                    result_peg_2595.setValue(value);
                }
            
            
        }
        
        if (column_peg_2594.chunk10 == 0){
            column_peg_2594.chunk10 = new Chunk10();
        }
        column_peg_2594.chunk10->chunk_hitdef__attribute = result_peg_2595;
        stream.update(result_peg_2595.getPosition());
        
        
        return result_peg_2595;
        out_peg_2599:
    
        if (column_peg_2594.chunk10 == 0){
            column_peg_2594.chunk10 = new Chunk10();
        }
        column_peg_2594.chunk10->chunk_hitdef__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attribute__item(Stream & stream, const int position){
    
    Column & column_peg_2601 = stream.getColumn(position);
    if (column_peg_2601.chunk11 != 0 && column_peg_2601.chunk11->chunk_hitdef__attribute__item.calculated()){
        return column_peg_2601.chunk11->chunk_hitdef__attribute__item;
    }
    
    RuleTrace trace_peg_2608(stream, "hitdef__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_2602(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("s"[i], stream.get(result_peg_2602.getPosition()))){
                result_peg_2602.nextPosition();
            } else {
                goto out_peg_2603;
            }
        }
        result_peg_2602.setValue((void*) "s");
        
        if (column_peg_2601.chunk11 == 0){
            column_peg_2601.chunk11 = new Chunk11();
        }
        column_peg_2601.chunk11->chunk_hitdef__attribute__item = result_peg_2602;
        stream.update(result_peg_2602.getPosition());
        
        
        return result_peg_2602;
        out_peg_2603:
        Result result_peg_2604(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("c"[i], stream.get(result_peg_2604.getPosition()))){
                result_peg_2604.nextPosition();
            } else {
                goto out_peg_2605;
            }
        }
        result_peg_2604.setValue((void*) "c");
        
        if (column_peg_2601.chunk11 == 0){
            column_peg_2601.chunk11 = new Chunk11();
        }
        column_peg_2601.chunk11->chunk_hitdef__attribute__item = result_peg_2604;
        stream.update(result_peg_2604.getPosition());
        
        
        return result_peg_2604;
        out_peg_2605:
        Result result_peg_2606(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("a"[i], stream.get(result_peg_2606.getPosition()))){
                result_peg_2606.nextPosition();
            } else {
                goto out_peg_2607;
            }
        }
        result_peg_2606.setValue((void*) "a");
        
        if (column_peg_2601.chunk11 == 0){
            column_peg_2601.chunk11 = new Chunk11();
        }
        column_peg_2601.chunk11->chunk_hitdef__attribute__item = result_peg_2606;
        stream.update(result_peg_2606.getPosition());
        
        
        return result_peg_2606;
        out_peg_2607:
    
        if (column_peg_2601.chunk11 == 0){
            column_peg_2601.chunk11 = new Chunk11();
        }
        column_peg_2601.chunk11->chunk_hitdef__attribute__item = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2609 = stream.getColumn(position);
    if (column_peg_2609.chunk11 != 0 && column_peg_2609.chunk11->chunk_args.calculated()){
        return column_peg_2609.chunk11->chunk_args;
    }
    
    RuleTrace trace_peg_2651(stream, "args");
    int myposition = position;
    
    
    Value expr1;
        Value expr_rest;
    Result result_peg_2610(myposition);
        
        {
        
            result_peg_2610 = rule_expr_c(stream, result_peg_2610.getPosition());
                if (result_peg_2610.error()){
                    goto out_peg_2612;
                }
                expr1 = result_peg_2610.getValues();
            
            
            
            result_peg_2610.reset();
                do{
                    Result result_peg_2615(result_peg_2610.getPosition());
                    {
                    
                        {
                                
                                result_peg_2615.reset();
                                do{
                                    Result result_peg_2627(result_peg_2615.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2627.getPosition()))){
                                                result_peg_2627.nextPosition();
                                            } else {
                                                goto out_peg_2630;
                                            }
                                        }
                                        result_peg_2627.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2628;
                                    out_peg_2630:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2627.getPosition()))){
                                                result_peg_2627.nextPosition();
                                            } else {
                                                goto out_peg_2632;
                                            }
                                        }
                                        result_peg_2627.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2628;
                                    out_peg_2632:
                                    goto loop_peg_2626;
                                    success_peg_2628:
                                    ;
                                    result_peg_2615.addResult(result_peg_2627);
                                } while (true);
                                loop_peg_2626:
                                ;
                                        
                            }
                            goto success_peg_2617;
                            goto loop_peg_2614;
                            success_peg_2617:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2615.getPosition()))){
                                    result_peg_2615.nextPosition();
                                } else {
                                    goto loop_peg_2614;
                                }
                            }
                            result_peg_2615.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2615.reset();
                                do{
                                    Result result_peg_2645(result_peg_2615.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2645.getPosition()))){
                                                result_peg_2645.nextPosition();
                                            } else {
                                                goto out_peg_2648;
                                            }
                                        }
                                        result_peg_2645.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2646;
                                    out_peg_2648:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2645.getPosition()))){
                                                result_peg_2645.nextPosition();
                                            } else {
                                                goto out_peg_2650;
                                            }
                                        }
                                        result_peg_2645.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2646;
                                    out_peg_2650:
                                    goto loop_peg_2644;
                                    success_peg_2646:
                                    ;
                                    result_peg_2615.addResult(result_peg_2645);
                                } while (true);
                                loop_peg_2644:
                                ;
                                        
                            }
                            goto success_peg_2635;
                            goto loop_peg_2614;
                            success_peg_2635:
                            ;
                        
                        
                        
                        result_peg_2615 = rule_expr_c(stream, result_peg_2615.getPosition());
                            if (result_peg_2615.error()){
                                goto loop_peg_2614;
                            }
                        
                        
                    }
                    result_peg_2610.addResult(result_peg_2615);
                } while (true);
                loop_peg_2614:
                ;
                expr_rest = result_peg_2610.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(expr1, expr_rest);
                    result_peg_2610.setValue(value);
                }
            
            
        }
        
        if (column_peg_2609.chunk11 == 0){
            column_peg_2609.chunk11 = new Chunk11();
        }
        column_peg_2609.chunk11->chunk_args = result_peg_2610;
        stream.update(result_peg_2610.getPosition());
        
        
        return result_peg_2610;
        out_peg_2612:
    
        if (column_peg_2609.chunk11 == 0){
            column_peg_2609.chunk11 = new Chunk11();
        }
        column_peg_2609.chunk11->chunk_args = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2652 = stream.getColumn(position);
    if (column_peg_2652.chunk11 != 0 && column_peg_2652.chunk11->chunk_function_name.calculated()){
        return column_peg_2652.chunk11->chunk_function_name;
    }
    
    RuleTrace trace_peg_2717(stream, "function_name");
    int myposition = position;
    
    
    
    Result result_peg_2653(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("abs"[i], stream.get(result_peg_2653.getPosition()))){
                result_peg_2653.nextPosition();
            } else {
                goto out_peg_2654;
            }
        }
        result_peg_2653.setValue((void*) "abs");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2653;
        stream.update(result_peg_2653.getPosition());
        
        
        return result_peg_2653;
        out_peg_2654:
        Result result_peg_2655(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("const"[i], stream.get(result_peg_2655.getPosition()))){
                result_peg_2655.nextPosition();
            } else {
                goto out_peg_2656;
            }
        }
        result_peg_2655.setValue((void*) "const");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2655;
        stream.update(result_peg_2655.getPosition());
        
        
        return result_peg_2655;
        out_peg_2656:
        Result result_peg_2657(myposition);
        
        for (int i = 0; i < 13; i++){
            if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2657.getPosition()))){
                result_peg_2657.nextPosition();
            } else {
                goto out_peg_2658;
            }
        }
        result_peg_2657.setValue((void*) "selfanimexist");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2657;
        stream.update(result_peg_2657.getPosition());
        
        
        return result_peg_2657;
        out_peg_2658:
        Result result_peg_2659(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("ifelse"[i], stream.get(result_peg_2659.getPosition()))){
                result_peg_2659.nextPosition();
            } else {
                goto out_peg_2660;
            }
        }
        result_peg_2659.setValue((void*) "ifelse");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2659;
        stream.update(result_peg_2659.getPosition());
        
        
        return result_peg_2659;
        out_peg_2660:
        Result result_peg_2661(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("gethitvar"[i], stream.get(result_peg_2661.getPosition()))){
                result_peg_2661.nextPosition();
            } else {
                goto out_peg_2662;
            }
        }
        result_peg_2661.setValue((void*) "gethitvar");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2661;
        stream.update(result_peg_2661.getPosition());
        
        
        return result_peg_2661;
        out_peg_2662:
        Result result_peg_2663(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("floor"[i], stream.get(result_peg_2663.getPosition()))){
                result_peg_2663.nextPosition();
            } else {
                goto out_peg_2664;
            }
        }
        result_peg_2663.setValue((void*) "floor");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2663;
        stream.update(result_peg_2663.getPosition());
        
        
        return result_peg_2663;
        out_peg_2664:
        Result result_peg_2665(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("ceil"[i], stream.get(result_peg_2665.getPosition()))){
                result_peg_2665.nextPosition();
            } else {
                goto out_peg_2666;
            }
        }
        result_peg_2665.setValue((void*) "ceil");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2665;
        stream.update(result_peg_2665.getPosition());
        
        
        return result_peg_2665;
        out_peg_2666:
        Result result_peg_2667(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("exp"[i], stream.get(result_peg_2667.getPosition()))){
                result_peg_2667.nextPosition();
            } else {
                goto out_peg_2668;
            }
        }
        result_peg_2667.setValue((void*) "exp");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2667;
        stream.update(result_peg_2667.getPosition());
        
        
        return result_peg_2667;
        out_peg_2668:
        Result result_peg_2669(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("acos"[i], stream.get(result_peg_2669.getPosition()))){
                result_peg_2669.nextPosition();
            } else {
                goto out_peg_2670;
            }
        }
        result_peg_2669.setValue((void*) "acos");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2669;
        stream.update(result_peg_2669.getPosition());
        
        
        return result_peg_2669;
        out_peg_2670:
        Result result_peg_2671(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("asin"[i], stream.get(result_peg_2671.getPosition()))){
                result_peg_2671.nextPosition();
            } else {
                goto out_peg_2672;
            }
        }
        result_peg_2671.setValue((void*) "asin");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2671;
        stream.update(result_peg_2671.getPosition());
        
        
        return result_peg_2671;
        out_peg_2672:
        Result result_peg_2673(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("atan"[i], stream.get(result_peg_2673.getPosition()))){
                result_peg_2673.nextPosition();
            } else {
                goto out_peg_2674;
            }
        }
        result_peg_2673.setValue((void*) "atan");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2673;
        stream.update(result_peg_2673.getPosition());
        
        
        return result_peg_2673;
        out_peg_2674:
        Result result_peg_2675(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("tan"[i], stream.get(result_peg_2675.getPosition()))){
                result_peg_2675.nextPosition();
            } else {
                goto out_peg_2676;
            }
        }
        result_peg_2675.setValue((void*) "tan");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2675;
        stream.update(result_peg_2675.getPosition());
        
        
        return result_peg_2675;
        out_peg_2676:
        Result result_peg_2677(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("cos"[i], stream.get(result_peg_2677.getPosition()))){
                result_peg_2677.nextPosition();
            } else {
                goto out_peg_2678;
            }
        }
        result_peg_2677.setValue((void*) "cos");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2677;
        stream.update(result_peg_2677.getPosition());
        
        
        return result_peg_2677;
        out_peg_2678:
        Result result_peg_2679(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("sin"[i], stream.get(result_peg_2679.getPosition()))){
                result_peg_2679.nextPosition();
            } else {
                goto out_peg_2680;
            }
        }
        result_peg_2679.setValue((void*) "sin");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2679;
        stream.update(result_peg_2679.getPosition());
        
        
        return result_peg_2679;
        out_peg_2680:
        Result result_peg_2681(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("log"[i], stream.get(result_peg_2681.getPosition()))){
                result_peg_2681.nextPosition();
            } else {
                goto out_peg_2682;
            }
        }
        result_peg_2681.setValue((void*) "log");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2681;
        stream.update(result_peg_2681.getPosition());
        
        
        return result_peg_2681;
        out_peg_2682:
        Result result_peg_2683(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareCharCase("ln"[i], stream.get(result_peg_2683.getPosition()))){
                result_peg_2683.nextPosition();
            } else {
                goto out_peg_2684;
            }
        }
        result_peg_2683.setValue((void*) "ln");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2683;
        stream.update(result_peg_2683.getPosition());
        
        
        return result_peg_2683;
        out_peg_2684:
        Result result_peg_2685(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("sysfvar"[i], stream.get(result_peg_2685.getPosition()))){
                result_peg_2685.nextPosition();
            } else {
                goto out_peg_2686;
            }
        }
        result_peg_2685.setValue((void*) "sysfvar");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2685;
        stream.update(result_peg_2685.getPosition());
        
        
        return result_peg_2685;
        out_peg_2686:
        Result result_peg_2687(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("sysvar"[i], stream.get(result_peg_2687.getPosition()))){
                result_peg_2687.nextPosition();
            } else {
                goto out_peg_2688;
            }
        }
        result_peg_2687.setValue((void*) "sysvar");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2687;
        stream.update(result_peg_2687.getPosition());
        
        
        return result_peg_2687;
        out_peg_2688:
        Result result_peg_2689(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("var"[i], stream.get(result_peg_2689.getPosition()))){
                result_peg_2689.nextPosition();
            } else {
                goto out_peg_2690;
            }
        }
        result_peg_2689.setValue((void*) "var");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2689;
        stream.update(result_peg_2689.getPosition());
        
        
        return result_peg_2689;
        out_peg_2690:
        Result result_peg_2691(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numexplod"[i], stream.get(result_peg_2691.getPosition()))){
                result_peg_2691.nextPosition();
            } else {
                goto out_peg_2692;
            }
        }
        result_peg_2691.setValue((void*) "numexplod");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2691;
        stream.update(result_peg_2691.getPosition());
        
        
        return result_peg_2691;
        out_peg_2692:
        Result result_peg_2693(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2693.getPosition()))){
                result_peg_2693.nextPosition();
            } else {
                goto out_peg_2694;
            }
        }
        result_peg_2693.setValue((void*) "numhelper");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2693;
        stream.update(result_peg_2693.getPosition());
        
        
        return result_peg_2693;
        out_peg_2694:
        Result result_peg_2695(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numprojid"[i], stream.get(result_peg_2695.getPosition()))){
                result_peg_2695.nextPosition();
            } else {
                goto out_peg_2696;
            }
        }
        result_peg_2695.setValue((void*) "numprojid");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2695;
        stream.update(result_peg_2695.getPosition());
        
        
        return result_peg_2695;
        out_peg_2696:
        Result result_peg_2697(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("fvar"[i], stream.get(result_peg_2697.getPosition()))){
                result_peg_2697.nextPosition();
            } else {
                goto out_peg_2698;
            }
        }
        result_peg_2697.setValue((void*) "fvar");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2697;
        stream.update(result_peg_2697.getPosition());
        
        
        return result_peg_2697;
        out_peg_2698:
        Result result_peg_2699(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("ishelper"[i], stream.get(result_peg_2699.getPosition()))){
                result_peg_2699.nextPosition();
            } else {
                goto out_peg_2700;
            }
        }
        result_peg_2699.setValue((void*) "ishelper");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2699;
        stream.update(result_peg_2699.getPosition());
        
        
        return result_peg_2699;
        out_peg_2700:
        Result result_peg_2701(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numtarget"[i], stream.get(result_peg_2701.getPosition()))){
                result_peg_2701.nextPosition();
            } else {
                goto out_peg_2702;
            }
        }
        result_peg_2701.setValue((void*) "numtarget");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2701;
        stream.update(result_peg_2701.getPosition());
        
        
        return result_peg_2701;
        out_peg_2702:
        Result result_peg_2703(myposition);
        
        for (int i = 0; i < 12; i++){
            if (compareCharCase("animelemtime"[i], stream.get(result_peg_2703.getPosition()))){
                result_peg_2703.nextPosition();
            } else {
                goto out_peg_2704;
            }
        }
        result_peg_2703.setValue((void*) "animelemtime");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2703;
        stream.update(result_peg_2703.getPosition());
        
        
        return result_peg_2703;
        out_peg_2704:
        Result result_peg_2705(myposition);
        
        for (int i = 0; i < 10; i++){
            if (compareCharCase("animelemno"[i], stream.get(result_peg_2705.getPosition()))){
                result_peg_2705.nextPosition();
            } else {
                goto out_peg_2706;
            }
        }
        result_peg_2705.setValue((void*) "animelemno");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2705;
        stream.update(result_peg_2705.getPosition());
        
        
        return result_peg_2705;
        out_peg_2706:
        Result result_peg_2707(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("animexist"[i], stream.get(result_peg_2707.getPosition()))){
                result_peg_2707.nextPosition();
            } else {
                goto out_peg_2708;
            }
        }
        result_peg_2707.setValue((void*) "animexist");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2707;
        stream.update(result_peg_2707.getPosition());
        
        
        return result_peg_2707;
        out_peg_2708:
        Result result_peg_2709(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projguarded"[i], stream.get(result_peg_2709.getPosition()))){
                result_peg_2709.nextPosition();
            } else {
                goto out_peg_2710;
            }
        }
        result_peg_2709.setValue((void*) "projguarded");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2709;
        stream.update(result_peg_2709.getPosition());
        
        
        return result_peg_2709;
        out_peg_2710:
        Result result_peg_2711(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projhittime"[i], stream.get(result_peg_2711.getPosition()))){
                result_peg_2711.nextPosition();
            } else {
                goto out_peg_2712;
            }
        }
        result_peg_2711.setValue((void*) "projhittime");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2711;
        stream.update(result_peg_2711.getPosition());
        
        
        return result_peg_2711;
        out_peg_2712:
        Result result_peg_2713(myposition);
        
        for (int i = 0; i < 15; i++){
            if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2713.getPosition()))){
                result_peg_2713.nextPosition();
            } else {
                goto out_peg_2714;
            }
        }
        result_peg_2713.setValue((void*) "projcontacttime");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2713;
        stream.update(result_peg_2713.getPosition());
        
        
        return result_peg_2713;
        out_peg_2714:
        Result result_peg_2715(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2715.getPosition()))){
                result_peg_2715.nextPosition();
            } else {
                goto out_peg_2716;
            }
        }
        result_peg_2715.setValue((void*) "numhelper");
        
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = result_peg_2715;
        stream.update(result_peg_2715.getPosition());
        
        
        return result_peg_2715;
        out_peg_2716:
    
        if (column_peg_2652.chunk11 == 0){
            column_peg_2652.chunk11 = new Chunk11();
        }
        column_peg_2652.chunk11->chunk_function_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2718 = stream.getColumn(position);
    if (column_peg_2718.chunk11 != 0 && column_peg_2718.chunk11->chunk_function_rest.calculated()){
        return column_peg_2718.chunk11->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2776(stream, "function_rest");
    int myposition = position;
    
    
    
    Result result_peg_2719(myposition);
        
        {
        
            {
                    
                    result_peg_2719.reset();
                    do{
                        Result result_peg_2731(result_peg_2719.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2731.getPosition()))){
                                    result_peg_2731.nextPosition();
                                } else {
                                    goto out_peg_2734;
                                }
                            }
                            result_peg_2731.setValue((void*) " ");
                                
                        }
                        goto success_peg_2732;
                        out_peg_2734:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2731.getPosition()))){
                                    result_peg_2731.nextPosition();
                                } else {
                                    goto out_peg_2736;
                                }
                            }
                            result_peg_2731.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2732;
                        out_peg_2736:
                        goto loop_peg_2730;
                        success_peg_2732:
                        ;
                        result_peg_2719.addResult(result_peg_2731);
                    } while (true);
                    loop_peg_2730:
                    ;
                            
                }
                goto success_peg_2721;
                goto out_peg_2737;
                success_peg_2721:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2719.getPosition()))){
                        result_peg_2719.nextPosition();
                    } else {
                        goto out_peg_2737;
                    }
                }
                result_peg_2719.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2719.reset();
                    do{
                        Result result_peg_2750(result_peg_2719.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2750.getPosition()))){
                                    result_peg_2750.nextPosition();
                                } else {
                                    goto out_peg_2753;
                                }
                            }
                            result_peg_2750.setValue((void*) " ");
                                
                        }
                        goto success_peg_2751;
                        out_peg_2753:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2750.getPosition()))){
                                    result_peg_2750.nextPosition();
                                } else {
                                    goto out_peg_2755;
                                }
                            }
                            result_peg_2750.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2751;
                        out_peg_2755:
                        goto loop_peg_2749;
                        success_peg_2751:
                        ;
                        result_peg_2719.addResult(result_peg_2750);
                    } while (true);
                    loop_peg_2749:
                    ;
                            
                }
                goto success_peg_2740;
                goto out_peg_2737;
                success_peg_2740:
                ;
            
            
            
            int save_peg_2757 = result_peg_2719.getPosition();
                
                result_peg_2719 = rule_all_compare(stream, result_peg_2719.getPosition());
                if (result_peg_2719.error()){
                    
                    result_peg_2719 = Result(save_peg_2757);
                    result_peg_2719.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2719.reset();
                    do{
                        Result result_peg_2769(result_peg_2719.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2769.getPosition()))){
                                    result_peg_2769.nextPosition();
                                } else {
                                    goto out_peg_2772;
                                }
                            }
                            result_peg_2769.setValue((void*) " ");
                                
                        }
                        goto success_peg_2770;
                        out_peg_2772:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2769.getPosition()))){
                                    result_peg_2769.nextPosition();
                                } else {
                                    goto out_peg_2774;
                                }
                            }
                            result_peg_2769.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2770;
                        out_peg_2774:
                        goto loop_peg_2768;
                        success_peg_2770:
                        ;
                        result_peg_2719.addResult(result_peg_2769);
                    } while (true);
                    loop_peg_2768:
                    ;
                            
                }
                goto success_peg_2759;
                goto out_peg_2737;
                success_peg_2759:
                ;
            
            
            
            result_peg_2719 = rule_value(stream, result_peg_2719.getPosition());
                if (result_peg_2719.error()){
                    goto out_peg_2737;
                }
            
            
        }
        
        if (column_peg_2718.chunk11 == 0){
            column_peg_2718.chunk11 = new Chunk11();
        }
        column_peg_2718.chunk11->chunk_function_rest = result_peg_2719;
        stream.update(result_peg_2719.getPosition());
        
        
        return result_peg_2719;
        out_peg_2737:
        Result result_peg_2775(myposition);
        
        
        
        if (column_peg_2718.chunk11 == 0){
            column_peg_2718.chunk11 = new Chunk11();
        }
        column_peg_2718.chunk11->chunk_function_rest = result_peg_2775;
        stream.update(result_peg_2775.getPosition());
        
        
        return result_peg_2775;
    
        if (column_peg_2718.chunk11 == 0){
            column_peg_2718.chunk11 = new Chunk11();
        }
        column_peg_2718.chunk11->chunk_function_rest = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2777 = stream.getColumn(position);
    if (column_peg_2777.chunk11 != 0 && column_peg_2777.chunk11->chunk_keys.calculated()){
        return column_peg_2777.chunk11->chunk_keys;
    }
    
    RuleTrace trace_peg_2780(stream, "keys");
    int myposition = position;
    
    
    
    Result result_peg_2778(myposition);
        
        result_peg_2778 = rule_key_value_list(stream, result_peg_2778.getPosition());
        if (result_peg_2778.error()){
            goto out_peg_2779;
        }
        
        if (column_peg_2777.chunk11 == 0){
            column_peg_2777.chunk11 = new Chunk11();
        }
        column_peg_2777.chunk11->chunk_keys = result_peg_2778;
        stream.update(result_peg_2778.getPosition());
        
        
        return result_peg_2778;
        out_peg_2779:
    
        if (column_peg_2777.chunk11 == 0){
            column_peg_2777.chunk11 = new Chunk11();
        }
        column_peg_2777.chunk11->chunk_keys = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2781 = stream.getColumn(position);
    if (column_peg_2781.chunk12 != 0 && column_peg_2781.chunk12->chunk_key_value_list.calculated()){
        return column_peg_2781.chunk12->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2825(stream, "key_value_list");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_2782(myposition);
        
        {
        
            result_peg_2782 = rule_key(stream, result_peg_2782.getPosition());
                if (result_peg_2782.error()){
                    goto out_peg_2784;
                }
                first = result_peg_2782.getValues();
            
            
            
            result_peg_2782.reset();
                do{
                    Result result_peg_2787(result_peg_2782.getPosition());
                    {
                    
                        {
                                
                                result_peg_2787.reset();
                                do{
                                    Result result_peg_2799(result_peg_2787.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2799.getPosition()))){
                                                result_peg_2799.nextPosition();
                                            } else {
                                                goto out_peg_2802;
                                            }
                                        }
                                        result_peg_2799.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2800;
                                    out_peg_2802:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2799.getPosition()))){
                                                result_peg_2799.nextPosition();
                                            } else {
                                                goto out_peg_2804;
                                            }
                                        }
                                        result_peg_2799.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2800;
                                    out_peg_2804:
                                    goto loop_peg_2798;
                                    success_peg_2800:
                                    ;
                                    result_peg_2787.addResult(result_peg_2799);
                                } while (true);
                                loop_peg_2798:
                                ;
                                        
                            }
                            goto success_peg_2789;
                            goto loop_peg_2786;
                            success_peg_2789:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2787.getPosition()))){
                                    result_peg_2787.nextPosition();
                                } else {
                                    goto loop_peg_2786;
                                }
                            }
                            result_peg_2787.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2787.reset();
                                do{
                                    Result result_peg_2817(result_peg_2787.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2817.getPosition()))){
                                                result_peg_2817.nextPosition();
                                            } else {
                                                goto out_peg_2820;
                                            }
                                        }
                                        result_peg_2817.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2818;
                                    out_peg_2820:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2817.getPosition()))){
                                                result_peg_2817.nextPosition();
                                            } else {
                                                goto out_peg_2822;
                                            }
                                        }
                                        result_peg_2817.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2818;
                                    out_peg_2822:
                                    goto loop_peg_2816;
                                    success_peg_2818:
                                    ;
                                    result_peg_2787.addResult(result_peg_2817);
                                } while (true);
                                loop_peg_2816:
                                ;
                                        
                            }
                            goto success_peg_2807;
                            goto loop_peg_2786;
                            success_peg_2807:
                            ;
                        
                        
                        
                        result_peg_2787 = rule_key(stream, result_peg_2787.getPosition());
                            if (result_peg_2787.error()){
                                goto loop_peg_2786;
                            }
                        
                        
                    }
                    result_peg_2782.addResult(result_peg_2787);
                } while (true);
                loop_peg_2786:
                ;
                rest = result_peg_2782.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyList(first, rest);
                    result_peg_2782.setValue(value);
                }
            
            
        }
        
        if (column_peg_2781.chunk12 == 0){
            column_peg_2781.chunk12 = new Chunk12();
        }
        column_peg_2781.chunk12->chunk_key_value_list = result_peg_2782;
        stream.update(result_peg_2782.getPosition());
        
        
        return result_peg_2782;
        out_peg_2784:
        Result result_peg_2823(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = makeEmptyKeyList();
                    result_peg_2823.setValue(value);
                }
            
            
        }
        
        if (column_peg_2781.chunk12 == 0){
            column_peg_2781.chunk12 = new Chunk12();
        }
        column_peg_2781.chunk12->chunk_key_value_list = result_peg_2823;
        stream.update(result_peg_2823.getPosition());
        
        
        return result_peg_2823;
    
        if (column_peg_2781.chunk12 == 0){
            column_peg_2781.chunk12 = new Chunk12();
        }
        column_peg_2781.chunk12->chunk_key_value_list = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2826 = stream.getColumn(position);
    if (column_peg_2826.chunk12 != 0 && column_peg_2826.chunk12->chunk_key.calculated()){
        return column_peg_2826.chunk12->chunk_key;
    }
    
    RuleTrace trace_peg_2834(stream, "key");
    int myposition = position;
    
    
    Value ok;
    Result result_peg_2827(myposition);
        
        {
        
            result_peg_2827 = rule_key_real(stream, result_peg_2827.getPosition());
                if (result_peg_2827.error()){
                    goto out_peg_2829;
                }
            
            Result result_peg_2828 = result_peg_2827;
            
            result_peg_2827 = rule_key_rest(stream, result_peg_2827.getPosition(), result_peg_2828.getValues());
                if (result_peg_2827.error()){
                    goto out_peg_2829;
                }
                ok = result_peg_2827.getValues();
            
            
            
            Result result_peg_2833(result_peg_2827);
                result_peg_2833 = rule_identifier(stream, result_peg_2833.getPosition());
                if (result_peg_2833.error()){
                    goto not_peg_2832;
                }
                goto out_peg_2829;
                not_peg_2832:
                result_peg_2827.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = ok;
                    result_peg_2827.setValue(value);
                }
            
            
        }
        
        if (column_peg_2826.chunk12 == 0){
            column_peg_2826.chunk12 = new Chunk12();
        }
        column_peg_2826.chunk12->chunk_key = result_peg_2827;
        stream.update(result_peg_2827.getPosition());
        
        
        return result_peg_2827;
        out_peg_2829:
    
        if (column_peg_2826.chunk12 == 0){
            column_peg_2826.chunk12 = new Chunk12();
        }
        column_peg_2826.chunk12->chunk_key = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2878(stream, "key_rest");
    int myposition = position;
    
    tail_peg_2837:
    Value another;
        Value new_left;
    Result result_peg_2836(myposition);
        {
        
            {
                    
                    result_peg_2836.reset();
                    do{
                        Result result_peg_2849(result_peg_2836.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2849.getPosition()))){
                                    result_peg_2849.nextPosition();
                                } else {
                                    goto out_peg_2852;
                                }
                            }
                            result_peg_2849.setValue((void*) " ");
                                
                        }
                        goto success_peg_2850;
                        out_peg_2852:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2849.getPosition()))){
                                    result_peg_2849.nextPosition();
                                } else {
                                    goto out_peg_2854;
                                }
                            }
                            result_peg_2849.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2850;
                        out_peg_2854:
                        goto loop_peg_2848;
                        success_peg_2850:
                        ;
                        result_peg_2836.addResult(result_peg_2849);
                    } while (true);
                    loop_peg_2848:
                    ;
                            
                }
                goto success_peg_2839;
                goto out_peg_2855;
                success_peg_2839:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2836.getPosition()))){
                        result_peg_2836.nextPosition();
                    } else {
                        goto out_peg_2855;
                    }
                }
                result_peg_2836.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_2836.reset();
                    do{
                        Result result_peg_2868(result_peg_2836.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2868.getPosition()))){
                                    result_peg_2868.nextPosition();
                                } else {
                                    goto out_peg_2871;
                                }
                            }
                            result_peg_2868.setValue((void*) " ");
                                
                        }
                        goto success_peg_2869;
                        out_peg_2871:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2868.getPosition()))){
                                    result_peg_2868.nextPosition();
                                } else {
                                    goto out_peg_2873;
                                }
                            }
                            result_peg_2868.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2869;
                        out_peg_2873:
                        goto loop_peg_2867;
                        success_peg_2869:
                        ;
                        result_peg_2836.addResult(result_peg_2868);
                    } while (true);
                    loop_peg_2867:
                    ;
                            
                }
                goto success_peg_2858;
                goto out_peg_2855;
                success_peg_2858:
                ;
            
            
            
            result_peg_2836 = rule_key_real(stream, result_peg_2836.getPosition());
                if (result_peg_2836.error()){
                    goto out_peg_2855;
                }
                another = result_peg_2836.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyCombined(left, another);
                    result_peg_2836.setValue(value);
                }
                new_left = result_peg_2836.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2836.getPosition();
        goto tail_peg_2837;
        out_peg_2855:
        Result result_peg_2876(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2876.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2876.getPosition());
        
        return result_peg_2876;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2879 = stream.getColumn(position);
    if (column_peg_2879.chunk12 != 0 && column_peg_2879.chunk12->chunk_key_real.calculated()){
        return column_peg_2879.chunk12->chunk_key_real;
    }
    
    RuleTrace trace_peg_2886(stream, "key_real");
    int myposition = position;
    
    
    Value mods;
        Value name;
    Result result_peg_2880(myposition);
        
        {
        
            result_peg_2880.reset();
                do{
                    Result result_peg_2883(result_peg_2880.getPosition());
                    result_peg_2883 = rule_key_modifier(stream, result_peg_2883.getPosition());
                    if (result_peg_2883.error()){
                        goto loop_peg_2882;
                    }
                    result_peg_2880.addResult(result_peg_2883);
                } while (true);
                loop_peg_2882:
                ;
                mods = result_peg_2880.getValues();
            
            
            
            result_peg_2880 = rule_key_name(stream, result_peg_2880.getPosition());
                if (result_peg_2880.error()){
                    goto out_peg_2885;
                }
                name = result_peg_2880.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = applyKeyModifiers(mods, makeKey(name));
                    result_peg_2880.setValue(value);
                }
            
            
        }
        
        if (column_peg_2879.chunk12 == 0){
            column_peg_2879.chunk12 = new Chunk12();
        }
        column_peg_2879.chunk12->chunk_key_real = result_peg_2880;
        stream.update(result_peg_2880.getPosition());
        
        
        return result_peg_2880;
        out_peg_2885:
    
        if (column_peg_2879.chunk12 == 0){
            column_peg_2879.chunk12 = new Chunk12();
        }
        column_peg_2879.chunk12->chunk_key_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2887 = stream.getColumn(position);
    if (column_peg_2887.chunk12 != 0 && column_peg_2887.chunk12->chunk_key_modifier.calculated()){
        return column_peg_2887.chunk12->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2908(stream, "key_modifier");
    int myposition = position;
    
    
    Value num;
    Result result_peg_2888(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2888.getPosition()))){
                        result_peg_2888.nextPosition();
                    } else {
                        goto out_peg_2890;
                    }
                }
                result_peg_2888.setValue((void*) "~");
            
            
            
            result_peg_2888.reset();
                do{
                    Result result_peg_2893(result_peg_2888.getPosition());
                    {
                        
                        char letter_peg_2898 = stream.get(result_peg_2893.getPosition());
                        if (letter_peg_2898 != '\0' && strchr("0123456789", letter_peg_2898) != NULL){
                            result_peg_2893.nextPosition();
                            result_peg_2893.setValue((void*) (long) letter_peg_2898);
                        } else {
                            goto out_peg_2897;
                        }
                        
                    }
                    goto success_peg_2894;
                    out_peg_2897:
                    goto loop_peg_2892;
                    success_peg_2894:
                    ;
                    result_peg_2888.addResult(result_peg_2893);
                } while (true);
                loop_peg_2892:
                ;
                num = result_peg_2888.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new ReleaseKeyModifier((int) *parseDouble(num));
                    result_peg_2888.setValue(value);
                }
            
            
        }
        
        if (column_peg_2887.chunk12 == 0){
            column_peg_2887.chunk12 = new Chunk12();
        }
        column_peg_2887.chunk12->chunk_key_modifier = result_peg_2888;
        stream.update(result_peg_2888.getPosition());
        
        
        return result_peg_2888;
        out_peg_2890:
        Result result_peg_2899(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("$"[i], stream.get(result_peg_2899.getPosition()))){
                        result_peg_2899.nextPosition();
                    } else {
                        goto out_peg_2901;
                    }
                }
                result_peg_2899.setValue((void*) "$");
            
            
            
            {
                    Value value((void*) 0);
                    value = new DirectionKeyModifier();
                    result_peg_2899.setValue(value);
                }
            
            
        }
        
        if (column_peg_2887.chunk12 == 0){
            column_peg_2887.chunk12 = new Chunk12();
        }
        column_peg_2887.chunk12->chunk_key_modifier = result_peg_2899;
        stream.update(result_peg_2899.getPosition());
        
        
        return result_peg_2899;
        out_peg_2901:
        Result result_peg_2902(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_2902.getPosition()))){
                        result_peg_2902.nextPosition();
                    } else {
                        goto out_peg_2904;
                    }
                }
                result_peg_2902.setValue((void*) "/");
            
            
            
            {
                    Value value((void*) 0);
                    value = new HeldDownKeyModifier();
                    result_peg_2902.setValue(value);
                }
            
            
        }
        
        if (column_peg_2887.chunk12 == 0){
            column_peg_2887.chunk12 = new Chunk12();
        }
        column_peg_2887.chunk12->chunk_key_modifier = result_peg_2902;
        stream.update(result_peg_2902.getPosition());
        
        
        return result_peg_2902;
        out_peg_2904:
        Result result_peg_2905(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_2905.getPosition()))){
                        result_peg_2905.nextPosition();
                    } else {
                        goto out_peg_2907;
                    }
                }
                result_peg_2905.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = new OnlyKeyModifier();
                    result_peg_2905.setValue(value);
                }
            
            
        }
        
        if (column_peg_2887.chunk12 == 0){
            column_peg_2887.chunk12 = new Chunk12();
        }
        column_peg_2887.chunk12->chunk_key_modifier = result_peg_2905;
        stream.update(result_peg_2905.getPosition());
        
        
        return result_peg_2905;
        out_peg_2907:
    
        if (column_peg_2887.chunk12 == 0){
            column_peg_2887.chunk12 = new Chunk12();
        }
        column_peg_2887.chunk12->chunk_key_modifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2909 = stream.getColumn(position);
    if (column_peg_2909.chunk13 != 0 && column_peg_2909.chunk13->chunk_key_name.calculated()){
        return column_peg_2909.chunk13->chunk_key_name;
    }
    
    RuleTrace trace_peg_2940(stream, "key_name");
    int myposition = position;
    
    
    
    Result result_peg_2910(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DB"[i], stream.get(result_peg_2910.getPosition()))){
                result_peg_2910.nextPosition();
            } else {
                goto out_peg_2911;
            }
        }
        result_peg_2910.setValue((void*) "DB");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2910;
        stream.update(result_peg_2910.getPosition());
        
        
        return result_peg_2910;
        out_peg_2911:
        Result result_peg_2912(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("B"[i], stream.get(result_peg_2912.getPosition()))){
                result_peg_2912.nextPosition();
            } else {
                goto out_peg_2913;
            }
        }
        result_peg_2912.setValue((void*) "B");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2912;
        stream.update(result_peg_2912.getPosition());
        
        
        return result_peg_2912;
        out_peg_2913:
        Result result_peg_2914(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DF"[i], stream.get(result_peg_2914.getPosition()))){
                result_peg_2914.nextPosition();
            } else {
                goto out_peg_2915;
            }
        }
        result_peg_2914.setValue((void*) "DF");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2914;
        stream.update(result_peg_2914.getPosition());
        
        
        return result_peg_2914;
        out_peg_2915:
        Result result_peg_2916(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("D"[i], stream.get(result_peg_2916.getPosition()))){
                result_peg_2916.nextPosition();
            } else {
                goto out_peg_2917;
            }
        }
        result_peg_2916.setValue((void*) "D");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2916;
        stream.update(result_peg_2916.getPosition());
        
        
        return result_peg_2916;
        out_peg_2917:
        Result result_peg_2918(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("F"[i], stream.get(result_peg_2918.getPosition()))){
                result_peg_2918.nextPosition();
            } else {
                goto out_peg_2919;
            }
        }
        result_peg_2918.setValue((void*) "F");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2918;
        stream.update(result_peg_2918.getPosition());
        
        
        return result_peg_2918;
        out_peg_2919:
        Result result_peg_2920(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UF"[i], stream.get(result_peg_2920.getPosition()))){
                result_peg_2920.nextPosition();
            } else {
                goto out_peg_2921;
            }
        }
        result_peg_2920.setValue((void*) "UF");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2920;
        stream.update(result_peg_2920.getPosition());
        
        
        return result_peg_2920;
        out_peg_2921:
        Result result_peg_2922(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UB"[i], stream.get(result_peg_2922.getPosition()))){
                result_peg_2922.nextPosition();
            } else {
                goto out_peg_2923;
            }
        }
        result_peg_2922.setValue((void*) "UB");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2922;
        stream.update(result_peg_2922.getPosition());
        
        
        return result_peg_2922;
        out_peg_2923:
        Result result_peg_2924(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("U"[i], stream.get(result_peg_2924.getPosition()))){
                result_peg_2924.nextPosition();
            } else {
                goto out_peg_2925;
            }
        }
        result_peg_2924.setValue((void*) "U");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2924;
        stream.update(result_peg_2924.getPosition());
        
        
        return result_peg_2924;
        out_peg_2925:
        Result result_peg_2926(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("a"[i], stream.get(result_peg_2926.getPosition()))){
                result_peg_2926.nextPosition();
            } else {
                goto out_peg_2927;
            }
        }
        result_peg_2926.setValue((void*) "a");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2926;
        stream.update(result_peg_2926.getPosition());
        
        
        return result_peg_2926;
        out_peg_2927:
        Result result_peg_2928(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("b"[i], stream.get(result_peg_2928.getPosition()))){
                result_peg_2928.nextPosition();
            } else {
                goto out_peg_2929;
            }
        }
        result_peg_2928.setValue((void*) "b");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2928;
        stream.update(result_peg_2928.getPosition());
        
        
        return result_peg_2928;
        out_peg_2929:
        Result result_peg_2930(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("c"[i], stream.get(result_peg_2930.getPosition()))){
                result_peg_2930.nextPosition();
            } else {
                goto out_peg_2931;
            }
        }
        result_peg_2930.setValue((void*) "c");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2930;
        stream.update(result_peg_2930.getPosition());
        
        
        return result_peg_2930;
        out_peg_2931:
        Result result_peg_2932(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("x"[i], stream.get(result_peg_2932.getPosition()))){
                result_peg_2932.nextPosition();
            } else {
                goto out_peg_2933;
            }
        }
        result_peg_2932.setValue((void*) "x");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2932;
        stream.update(result_peg_2932.getPosition());
        
        
        return result_peg_2932;
        out_peg_2933:
        Result result_peg_2934(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("y"[i], stream.get(result_peg_2934.getPosition()))){
                result_peg_2934.nextPosition();
            } else {
                goto out_peg_2935;
            }
        }
        result_peg_2934.setValue((void*) "y");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2934;
        stream.update(result_peg_2934.getPosition());
        
        
        return result_peg_2934;
        out_peg_2935:
        Result result_peg_2936(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("z"[i], stream.get(result_peg_2936.getPosition()))){
                result_peg_2936.nextPosition();
            } else {
                goto out_peg_2937;
            }
        }
        result_peg_2936.setValue((void*) "z");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2936;
        stream.update(result_peg_2936.getPosition());
        
        
        return result_peg_2936;
        out_peg_2937:
        Result result_peg_2938(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("s"[i], stream.get(result_peg_2938.getPosition()))){
                result_peg_2938.nextPosition();
            } else {
                goto out_peg_2939;
            }
        }
        result_peg_2938.setValue((void*) "s");
        
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = result_peg_2938;
        stream.update(result_peg_2938.getPosition());
        
        
        return result_peg_2938;
        out_peg_2939:
    
        if (column_peg_2909.chunk13 == 0){
            column_peg_2909.chunk13 = new Chunk13();
        }
        column_peg_2909.chunk13->chunk_key_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2941 = stream.getColumn(position);
    if (column_peg_2941.chunk13 != 0 && column_peg_2941.chunk13->chunk_value.calculated()){
        return column_peg_2941.chunk13->chunk_value;
    }
    
    RuleTrace trace_peg_2959(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_2942(myposition);
        
        result_peg_2942 = rule_float(stream, result_peg_2942.getPosition());
        if (result_peg_2942.error()){
            goto out_peg_2943;
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2942;
        stream.update(result_peg_2942.getPosition());
        
        
        return result_peg_2942;
        out_peg_2943:
        Result result_peg_2944(myposition);
        
        result_peg_2944 = rule_integer(stream, result_peg_2944.getPosition());
        if (result_peg_2944.error()){
            goto out_peg_2945;
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2944;
        stream.update(result_peg_2944.getPosition());
        
        
        return result_peg_2944;
        out_peg_2945:
        Result result_peg_2946(myposition);
        
        result_peg_2946 = rule_keyword(stream, result_peg_2946.getPosition());
        if (result_peg_2946.error()){
            goto out_peg_2947;
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2946;
        stream.update(result_peg_2946.getPosition());
        
        
        return result_peg_2946;
        out_peg_2947:
        Result result_peg_2948(myposition);
        
        {
        
            Result result_peg_2951(result_peg_2948);
                result_peg_2951 = rule_keyword(stream, result_peg_2951.getPosition());
                if (result_peg_2951.error()){
                    goto not_peg_2950;
                }
                goto out_peg_2952;
                not_peg_2950:
                result_peg_2948.setValue((void*)0);
            
            
            
            result_peg_2948 = rule_identifier(stream, result_peg_2948.getPosition());
                if (result_peg_2948.error()){
                    goto out_peg_2952;
                }
            
            
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2948;
        stream.update(result_peg_2948.getPosition());
        
        
        return result_peg_2948;
        out_peg_2952:
        Result result_peg_2953(myposition);
        
        result_peg_2953 = rule_range(stream, result_peg_2953.getPosition());
        if (result_peg_2953.error()){
            goto out_peg_2954;
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2953;
        stream.update(result_peg_2953.getPosition());
        
        
        return result_peg_2953;
        out_peg_2954:
        Result result_peg_2955(myposition);
        
        result_peg_2955 = rule_string(stream, result_peg_2955.getPosition());
        if (result_peg_2955.error()){
            goto out_peg_2956;
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2955;
        stream.update(result_peg_2955.getPosition());
        
        
        return result_peg_2955;
        out_peg_2956:
        Result result_peg_2957(myposition);
        
        result_peg_2957 = rule_hitflag(stream, result_peg_2957.getPosition());
        if (result_peg_2957.error()){
            goto out_peg_2958;
        }
        
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = result_peg_2957;
        stream.update(result_peg_2957.getPosition());
        
        
        return result_peg_2957;
        out_peg_2958:
    
        if (column_peg_2941.chunk13 == 0){
            column_peg_2941.chunk13 = new Chunk13();
        }
        column_peg_2941.chunk13->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2960 = stream.getColumn(position);
    if (column_peg_2960.chunk13 != 0 && column_peg_2960.chunk13->chunk_helper.calculated()){
        return column_peg_2960.chunk13->chunk_helper;
    }
    
    RuleTrace trace_peg_3019(stream, "helper");
    int myposition = position;
    
    
    Value name;
        Value id;
    Result result_peg_2961(myposition);
        
        {
        
            result_peg_2961 = rule_helper__name(stream, result_peg_2961.getPosition());
                if (result_peg_2961.error()){
                    goto out_peg_2963;
                }
                name = result_peg_2961.getValues();
            
            
            
            {
                    
                    result_peg_2961.reset();
                    do{
                        Result result_peg_2975(result_peg_2961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2975.getPosition()))){
                                    result_peg_2975.nextPosition();
                                } else {
                                    goto out_peg_2978;
                                }
                            }
                            result_peg_2975.setValue((void*) " ");
                                
                        }
                        goto success_peg_2976;
                        out_peg_2978:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2975.getPosition()))){
                                    result_peg_2975.nextPosition();
                                } else {
                                    goto out_peg_2980;
                                }
                            }
                            result_peg_2975.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2976;
                        out_peg_2980:
                        goto loop_peg_2974;
                        success_peg_2976:
                        ;
                        result_peg_2961.addResult(result_peg_2975);
                    } while (true);
                    loop_peg_2974:
                    ;
                            
                }
                goto success_peg_2965;
                goto out_peg_2963;
                success_peg_2965:
                ;
            
            
            
            int save_peg_2982 = result_peg_2961.getPosition();
                
                result_peg_2961 = rule_helper__expression(stream, result_peg_2961.getPosition());
                if (result_peg_2961.error()){
                    
                    result_peg_2961 = Result(save_peg_2982);
                    result_peg_2961.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2961.reset();
                    do{
                        Result result_peg_2994(result_peg_2961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2994.getPosition()))){
                                    result_peg_2994.nextPosition();
                                } else {
                                    goto out_peg_2997;
                                }
                            }
                            result_peg_2994.setValue((void*) " ");
                                
                        }
                        goto success_peg_2995;
                        out_peg_2997:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2994.getPosition()))){
                                    result_peg_2994.nextPosition();
                                } else {
                                    goto out_peg_2999;
                                }
                            }
                            result_peg_2994.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2995;
                        out_peg_2999:
                        goto loop_peg_2993;
                        success_peg_2995:
                        ;
                        result_peg_2961.addResult(result_peg_2994);
                    } while (true);
                    loop_peg_2993:
                    ;
                            
                }
                goto success_peg_2984;
                goto out_peg_2963;
                success_peg_2984:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2961.getPosition()))){
                        result_peg_2961.nextPosition();
                    } else {
                        goto out_peg_2963;
                    }
                }
                result_peg_2961.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2961.reset();
                    do{
                        Result result_peg_3012(result_peg_2961.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3012.getPosition()))){
                                    result_peg_3012.nextPosition();
                                } else {
                                    goto out_peg_3015;
                                }
                            }
                            result_peg_3012.setValue((void*) " ");
                                
                        }
                        goto success_peg_3013;
                        out_peg_3015:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3012.getPosition()))){
                                    result_peg_3012.nextPosition();
                                } else {
                                    goto out_peg_3017;
                                }
                            }
                            result_peg_3012.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3013;
                        out_peg_3017:
                        goto loop_peg_3011;
                        success_peg_3013:
                        ;
                        result_peg_2961.addResult(result_peg_3012);
                    } while (true);
                    loop_peg_3011:
                    ;
                            
                }
                goto success_peg_3002;
                goto out_peg_2963;
                success_peg_3002:
                ;
            
            
            
            result_peg_2961 = rule_helper__identifier(stream, result_peg_2961.getPosition());
                if (result_peg_2961.error()){
                    goto out_peg_2963;
                }
                id = result_peg_2961.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHelper(name, id);
                    result_peg_2961.setValue(value);
                }
            
            
        }
        
        if (column_peg_2960.chunk13 == 0){
            column_peg_2960.chunk13 = new Chunk13();
        }
        column_peg_2960.chunk13->chunk_helper = result_peg_2961;
        stream.update(result_peg_2961.getPosition());
        
        
        return result_peg_2961;
        out_peg_2963:
    
        if (column_peg_2960.chunk13 == 0){
            column_peg_2960.chunk13 = new Chunk13();
        }
        column_peg_2960.chunk13->chunk_helper = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_3020 = stream.getColumn(position);
    if (column_peg_3020.chunk13 != 0 && column_peg_3020.chunk13->chunk_helper__expression.calculated()){
        return column_peg_3020.chunk13->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_3059(stream, "helper__expression");
    int myposition = position;
    
    
    
    Result result_peg_3021(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_3021.getPosition()))){
                        result_peg_3021.nextPosition();
                    } else {
                        goto out_peg_3023;
                    }
                }
                result_peg_3021.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_3021.reset();
                    do{
                        Result result_peg_3035(result_peg_3021.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3035.getPosition()))){
                                    result_peg_3035.nextPosition();
                                } else {
                                    goto out_peg_3038;
                                }
                            }
                            result_peg_3035.setValue((void*) " ");
                                
                        }
                        goto success_peg_3036;
                        out_peg_3038:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3035.getPosition()))){
                                    result_peg_3035.nextPosition();
                                } else {
                                    goto out_peg_3040;
                                }
                            }
                            result_peg_3035.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3036;
                        out_peg_3040:
                        goto loop_peg_3034;
                        success_peg_3036:
                        ;
                        result_peg_3021.addResult(result_peg_3035);
                    } while (true);
                    loop_peg_3034:
                    ;
                            
                }
                goto success_peg_3025;
                goto out_peg_3023;
                success_peg_3025:
                ;
            
            
            
            result_peg_3021 = rule_expr_c(stream, result_peg_3021.getPosition());
                if (result_peg_3021.error()){
                    goto out_peg_3023;
                }
            
            
            
            {
                    
                    result_peg_3021.reset();
                    do{
                        Result result_peg_3053(result_peg_3021.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3053.getPosition()))){
                                    result_peg_3053.nextPosition();
                                } else {
                                    goto out_peg_3056;
                                }
                            }
                            result_peg_3053.setValue((void*) " ");
                                
                        }
                        goto success_peg_3054;
                        out_peg_3056:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3053.getPosition()))){
                                    result_peg_3053.nextPosition();
                                } else {
                                    goto out_peg_3058;
                                }
                            }
                            result_peg_3053.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3054;
                        out_peg_3058:
                        goto loop_peg_3052;
                        success_peg_3054:
                        ;
                        result_peg_3021.addResult(result_peg_3053);
                    } while (true);
                    loop_peg_3052:
                    ;
                            
                }
                goto success_peg_3043;
                goto out_peg_3023;
                success_peg_3043:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_3021.getPosition()))){
                        result_peg_3021.nextPosition();
                    } else {
                        goto out_peg_3023;
                    }
                }
                result_peg_3021.setValue((void*) ")");
            
            
        }
        
        if (column_peg_3020.chunk13 == 0){
            column_peg_3020.chunk13 = new Chunk13();
        }
        column_peg_3020.chunk13->chunk_helper__expression = result_peg_3021;
        stream.update(result_peg_3021.getPosition());
        
        
        return result_peg_3021;
        out_peg_3023:
    
        if (column_peg_3020.chunk13 == 0){
            column_peg_3020.chunk13 = new Chunk13();
        }
        column_peg_3020.chunk13->chunk_helper__expression = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_3060 = stream.getColumn(position);
    if (column_peg_3060.chunk13 != 0 && column_peg_3060.chunk13->chunk_helper__name.calculated()){
        return column_peg_3060.chunk13->chunk_helper__name;
    }
    
    RuleTrace trace_peg_3077(stream, "helper__name");
    int myposition = position;
    
    
    
    Result result_peg_3061(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("parent"[i], stream.get(result_peg_3061.getPosition()))){
                result_peg_3061.nextPosition();
            } else {
                goto out_peg_3062;
            }
        }
        result_peg_3061.setValue((void*) "parent");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3061;
        stream.update(result_peg_3061.getPosition());
        
        
        return result_peg_3061;
        out_peg_3062:
        Result result_peg_3063(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("root"[i], stream.get(result_peg_3063.getPosition()))){
                result_peg_3063.nextPosition();
            } else {
                goto out_peg_3064;
            }
        }
        result_peg_3063.setValue((void*) "root");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3063;
        stream.update(result_peg_3063.getPosition());
        
        
        return result_peg_3063;
        out_peg_3064:
        Result result_peg_3065(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("helper"[i], stream.get(result_peg_3065.getPosition()))){
                result_peg_3065.nextPosition();
            } else {
                goto out_peg_3066;
            }
        }
        result_peg_3065.setValue((void*) "helper");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3065;
        stream.update(result_peg_3065.getPosition());
        
        
        return result_peg_3065;
        out_peg_3066:
        Result result_peg_3067(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("target"[i], stream.get(result_peg_3067.getPosition()))){
                result_peg_3067.nextPosition();
            } else {
                goto out_peg_3068;
            }
        }
        result_peg_3067.setValue((void*) "target");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3067;
        stream.update(result_peg_3067.getPosition());
        
        
        return result_peg_3067;
        out_peg_3068:
        Result result_peg_3069(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("partner"[i], stream.get(result_peg_3069.getPosition()))){
                result_peg_3069.nextPosition();
            } else {
                goto out_peg_3070;
            }
        }
        result_peg_3069.setValue((void*) "partner");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3069;
        stream.update(result_peg_3069.getPosition());
        
        
        return result_peg_3069;
        out_peg_3070:
        Result result_peg_3071(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("enemyNear"[i], stream.get(result_peg_3071.getPosition()))){
                result_peg_3071.nextPosition();
            } else {
                goto out_peg_3072;
            }
        }
        result_peg_3071.setValue((void*) "enemyNear");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3071;
        stream.update(result_peg_3071.getPosition());
        
        
        return result_peg_3071;
        out_peg_3072:
        Result result_peg_3073(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("enemy"[i], stream.get(result_peg_3073.getPosition()))){
                result_peg_3073.nextPosition();
            } else {
                goto out_peg_3074;
            }
        }
        result_peg_3073.setValue((void*) "enemy");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3073;
        stream.update(result_peg_3073.getPosition());
        
        
        return result_peg_3073;
        out_peg_3074:
        Result result_peg_3075(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("playerid"[i], stream.get(result_peg_3075.getPosition()))){
                result_peg_3075.nextPosition();
            } else {
                goto out_peg_3076;
            }
        }
        result_peg_3075.setValue((void*) "playerid");
        
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = result_peg_3075;
        stream.update(result_peg_3075.getPosition());
        
        
        return result_peg_3075;
        out_peg_3076:
    
        if (column_peg_3060.chunk13 == 0){
            column_peg_3060.chunk13 = new Chunk13();
        }
        column_peg_3060.chunk13->chunk_helper__name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_3078 = stream.getColumn(position);
    if (column_peg_3078.chunk14 != 0 && column_peg_3078.chunk14->chunk_helper__identifier.calculated()){
        return column_peg_3078.chunk14->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_3088(stream, "helper__identifier");
    int myposition = position;
    
    
    
    Result result_peg_3079(myposition);
        
        result_peg_3079 = rule_function(stream, result_peg_3079.getPosition());
        if (result_peg_3079.error()){
            goto out_peg_3080;
        }
        
        if (column_peg_3078.chunk14 == 0){
            column_peg_3078.chunk14 = new Chunk14();
        }
        column_peg_3078.chunk14->chunk_helper__identifier = result_peg_3079;
        stream.update(result_peg_3079.getPosition());
        
        
        return result_peg_3079;
        out_peg_3080:
        Result result_peg_3081(myposition);
        
        result_peg_3081 = rule_keyword(stream, result_peg_3081.getPosition());
        if (result_peg_3081.error()){
            goto out_peg_3082;
        }
        
        if (column_peg_3078.chunk14 == 0){
            column_peg_3078.chunk14 = new Chunk14();
        }
        column_peg_3078.chunk14->chunk_helper__identifier = result_peg_3081;
        stream.update(result_peg_3081.getPosition());
        
        
        return result_peg_3081;
        out_peg_3082:
        Result result_peg_3083(myposition);
        
        {
        
            Result result_peg_3086(result_peg_3083);
                result_peg_3086 = rule_keyword(stream, result_peg_3086.getPosition());
                if (result_peg_3086.error()){
                    goto not_peg_3085;
                }
                goto out_peg_3087;
                not_peg_3085:
                result_peg_3083.setValue((void*)0);
            
            
            
            result_peg_3083 = rule_identifier(stream, result_peg_3083.getPosition());
                if (result_peg_3083.error()){
                    goto out_peg_3087;
                }
            
            
        }
        
        if (column_peg_3078.chunk14 == 0){
            column_peg_3078.chunk14 = new Chunk14();
        }
        column_peg_3078.chunk14->chunk_helper__identifier = result_peg_3083;
        stream.update(result_peg_3083.getPosition());
        
        
        return result_peg_3083;
        out_peg_3087:
    
        if (column_peg_3078.chunk14 == 0){
            column_peg_3078.chunk14 = new Chunk14();
        }
        column_peg_3078.chunk14->chunk_helper__identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_3089 = stream.getColumn(position);
    if (column_peg_3089.chunk14 != 0 && column_peg_3089.chunk14->chunk_hitflag.calculated()){
        return column_peg_3089.chunk14->chunk_hitflag;
    }
    
    RuleTrace trace_peg_3114(stream, "hitflag");
    int myposition = position;
    
    
    
    Result result_peg_3090(myposition);
        
        {
        
            result_peg_3090.reset();
                do{
                    Result result_peg_3093(result_peg_3090.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("A"[i], stream.get(result_peg_3093.getPosition()))){
                                result_peg_3093.nextPosition();
                            } else {
                                goto out_peg_3096;
                            }
                        }
                        result_peg_3093.setValue((void*) "A");
                            
                    }
                    goto success_peg_3094;
                    out_peg_3096:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("M"[i], stream.get(result_peg_3093.getPosition()))){
                                result_peg_3093.nextPosition();
                            } else {
                                goto out_peg_3098;
                            }
                        }
                        result_peg_3093.setValue((void*) "M");
                            
                    }
                    goto success_peg_3094;
                    out_peg_3098:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("F"[i], stream.get(result_peg_3093.getPosition()))){
                                result_peg_3093.nextPosition();
                            } else {
                                goto out_peg_3100;
                            }
                        }
                        result_peg_3093.setValue((void*) "F");
                            
                    }
                    goto success_peg_3094;
                    out_peg_3100:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("D"[i], stream.get(result_peg_3093.getPosition()))){
                                result_peg_3093.nextPosition();
                            } else {
                                goto out_peg_3102;
                            }
                        }
                        result_peg_3093.setValue((void*) "D");
                            
                    }
                    goto success_peg_3094;
                    out_peg_3102:
                    goto loop_peg_3092;
                    success_peg_3094:
                    ;
                    result_peg_3090.addResult(result_peg_3093);
                } while (true);
                loop_peg_3092:
                if (result_peg_3090.matches() == 0){
                    goto out_peg_3103;
                }
            
            
            
            int save_peg_3105 = result_peg_3090.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_3090.getPosition()))){
                            result_peg_3090.nextPosition();
                        } else {
                            goto out_peg_3108;
                        }
                    }
                    result_peg_3090.setValue((void*) "+");
                        
                }
                goto success_peg_3106;
                out_peg_3108:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_3090.getPosition()))){
                            result_peg_3090.nextPosition();
                        } else {
                            goto out_peg_3110;
                        }
                    }
                    result_peg_3090.setValue((void*) "-");
                        
                }
                goto success_peg_3106;
                out_peg_3110:
                
                result_peg_3090 = Result(save_peg_3105);
                result_peg_3090.setValue((void*) 0);
                
                success_peg_3106:
                ;
            
            
            
            Result result_peg_3113(result_peg_3090);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_3113.getPosition()))){
                        result_peg_3113.nextPosition();
                    } else {
                        goto not_peg_3112;
                    }
                }
                result_peg_3113.setValue((void*) ".");
                goto out_peg_3103;
                not_peg_3112:
                result_peg_3090.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSimpleIdentifier("A");
                    result_peg_3090.setValue(value);
                }
            
            
        }
        
        if (column_peg_3089.chunk14 == 0){
            column_peg_3089.chunk14 = new Chunk14();
        }
        column_peg_3089.chunk14->chunk_hitflag = result_peg_3090;
        stream.update(result_peg_3090.getPosition());
        
        
        return result_peg_3090;
        out_peg_3103:
    
        if (column_peg_3089.chunk14 == 0){
            column_peg_3089.chunk14 = new Chunk14();
        }
        column_peg_3089.chunk14->chunk_hitflag = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_3115 = stream.getColumn(position);
    if (column_peg_3115.chunk14 != 0 && column_peg_3115.chunk14->chunk_keyword.calculated()){
        return column_peg_3115.chunk14->chunk_keyword;
    }
    
    RuleTrace trace_peg_3122(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_3116(myposition);
        
        {
        
            result_peg_3116 = rule_keyword_real(stream, result_peg_3116.getPosition());
                if (result_peg_3116.error()){
                    goto out_peg_3118;
                }
            
            Result result_peg_3117 = result_peg_3116;
            
            Result result_peg_3121(result_peg_3116);
                result_peg_3121 = rule_alpha_digit(stream, result_peg_3121.getPosition());
                if (result_peg_3121.error()){
                    goto not_peg_3120;
                }
                goto out_peg_3118;
                not_peg_3120:
                result_peg_3116.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_3117.getValues();
                    result_peg_3116.setValue(value);
                }
            
            
        }
        
        if (column_peg_3115.chunk14 == 0){
            column_peg_3115.chunk14 = new Chunk14();
        }
        column_peg_3115.chunk14->chunk_keyword = result_peg_3116;
        stream.update(result_peg_3116.getPosition());
        
        
        return result_peg_3116;
        out_peg_3118:
    
        if (column_peg_3115.chunk14 == 0){
            column_peg_3115.chunk14 = new Chunk14();
        }
        column_peg_3115.chunk14->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_3123 = stream.getColumn(position);
    if (column_peg_3123.chunk14 != 0 && column_peg_3123.chunk14->chunk_keyword_real.calculated()){
        return column_peg_3123.chunk14->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3544(stream, "keyword_real");
    int myposition = position;
    
    
    
    Result result_peg_3124(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_3124.getPosition()))){
                        result_peg_3124.nextPosition();
                    } else {
                        goto out_peg_3126;
                    }
                }
                result_peg_3124.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_3124.reset();
                    do{
                        Result result_peg_3139(result_peg_3124.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3139.getPosition()))){
                                    result_peg_3139.nextPosition();
                                } else {
                                    goto out_peg_3142;
                                }
                            }
                            result_peg_3139.setValue((void*) " ");
                                
                        }
                        goto success_peg_3140;
                        out_peg_3142:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3139.getPosition()))){
                                    result_peg_3139.nextPosition();
                                } else {
                                    goto out_peg_3144;
                                }
                            }
                            result_peg_3139.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3140;
                        out_peg_3144:
                        goto loop_peg_3138;
                        success_peg_3140:
                        ;
                        result_peg_3124.addResult(result_peg_3139);
                    } while (true);
                    loop_peg_3138:
                    if (result_peg_3124.matches() == 0){
                        goto out_peg_3137;
                    }
                    
                }
                goto success_peg_3128;
                out_peg_3137:
                goto out_peg_3126;
                success_peg_3128:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3124.getPosition()))){
                        result_peg_3124.nextPosition();
                    } else {
                        goto out_peg_3126;
                    }
                }
                result_peg_3124.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel y");
                    result_peg_3124.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3124;
        stream.update(result_peg_3124.getPosition());
        
        
        return result_peg_3124;
        out_peg_3126:
        Result result_peg_3146(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_3146.getPosition()))){
                        result_peg_3146.nextPosition();
                    } else {
                        goto out_peg_3148;
                    }
                }
                result_peg_3146.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_3146.reset();
                    do{
                        Result result_peg_3161(result_peg_3146.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3161.getPosition()))){
                                    result_peg_3161.nextPosition();
                                } else {
                                    goto out_peg_3164;
                                }
                            }
                            result_peg_3161.setValue((void*) " ");
                                
                        }
                        goto success_peg_3162;
                        out_peg_3164:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3161.getPosition()))){
                                    result_peg_3161.nextPosition();
                                } else {
                                    goto out_peg_3166;
                                }
                            }
                            result_peg_3161.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3162;
                        out_peg_3166:
                        goto loop_peg_3160;
                        success_peg_3162:
                        ;
                        result_peg_3146.addResult(result_peg_3161);
                    } while (true);
                    loop_peg_3160:
                    if (result_peg_3146.matches() == 0){
                        goto out_peg_3159;
                    }
                    
                }
                goto success_peg_3150;
                out_peg_3159:
                goto out_peg_3148;
                success_peg_3150:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3146.getPosition()))){
                        result_peg_3146.nextPosition();
                    } else {
                        goto out_peg_3148;
                    }
                }
                result_peg_3146.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel x");
                    result_peg_3146.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3146;
        stream.update(result_peg_3146.getPosition());
        
        
        return result_peg_3146;
        out_peg_3148:
        Result result_peg_3168(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_3168.getPosition()))){
                        result_peg_3168.nextPosition();
                    } else {
                        goto out_peg_3170;
                    }
                }
                result_peg_3168.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_3168.reset();
                    do{
                        Result result_peg_3183(result_peg_3168.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3183.getPosition()))){
                                    result_peg_3183.nextPosition();
                                } else {
                                    goto out_peg_3186;
                                }
                            }
                            result_peg_3183.setValue((void*) " ");
                                
                        }
                        goto success_peg_3184;
                        out_peg_3186:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3183.getPosition()))){
                                    result_peg_3183.nextPosition();
                                } else {
                                    goto out_peg_3188;
                                }
                            }
                            result_peg_3183.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3184;
                        out_peg_3188:
                        goto loop_peg_3182;
                        success_peg_3184:
                        ;
                        result_peg_3168.addResult(result_peg_3183);
                    } while (true);
                    loop_peg_3182:
                    if (result_peg_3168.matches() == 0){
                        goto out_peg_3181;
                    }
                    
                }
                goto success_peg_3172;
                out_peg_3181:
                goto out_peg_3170;
                success_peg_3172:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3168.getPosition()))){
                        result_peg_3168.nextPosition();
                    } else {
                        goto out_peg_3170;
                    }
                }
                result_peg_3168.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos y");
                    result_peg_3168.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3168;
        stream.update(result_peg_3168.getPosition());
        
        
        return result_peg_3168;
        out_peg_3170:
        Result result_peg_3190(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_3190.getPosition()))){
                        result_peg_3190.nextPosition();
                    } else {
                        goto out_peg_3192;
                    }
                }
                result_peg_3190.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_3190.reset();
                    do{
                        Result result_peg_3205(result_peg_3190.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3205.getPosition()))){
                                    result_peg_3205.nextPosition();
                                } else {
                                    goto out_peg_3208;
                                }
                            }
                            result_peg_3205.setValue((void*) " ");
                                
                        }
                        goto success_peg_3206;
                        out_peg_3208:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3205.getPosition()))){
                                    result_peg_3205.nextPosition();
                                } else {
                                    goto out_peg_3210;
                                }
                            }
                            result_peg_3205.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3206;
                        out_peg_3210:
                        goto loop_peg_3204;
                        success_peg_3206:
                        ;
                        result_peg_3190.addResult(result_peg_3205);
                    } while (true);
                    loop_peg_3204:
                    if (result_peg_3190.matches() == 0){
                        goto out_peg_3203;
                    }
                    
                }
                goto success_peg_3194;
                out_peg_3203:
                goto out_peg_3192;
                success_peg_3194:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3190.getPosition()))){
                        result_peg_3190.nextPosition();
                    } else {
                        goto out_peg_3192;
                    }
                }
                result_peg_3190.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos x");
                    result_peg_3190.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3190;
        stream.update(result_peg_3190.getPosition());
        
        
        return result_peg_3190;
        out_peg_3192:
        Result result_peg_3212(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_3212.getPosition()))){
                        result_peg_3212.nextPosition();
                    } else {
                        goto out_peg_3214;
                    }
                }
                result_peg_3212.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_3212.reset();
                    do{
                        Result result_peg_3227(result_peg_3212.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3227.getPosition()))){
                                    result_peg_3227.nextPosition();
                                } else {
                                    goto out_peg_3230;
                                }
                            }
                            result_peg_3227.setValue((void*) " ");
                                
                        }
                        goto success_peg_3228;
                        out_peg_3230:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3227.getPosition()))){
                                    result_peg_3227.nextPosition();
                                } else {
                                    goto out_peg_3232;
                                }
                            }
                            result_peg_3227.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3228;
                        out_peg_3232:
                        goto loop_peg_3226;
                        success_peg_3228:
                        ;
                        result_peg_3212.addResult(result_peg_3227);
                    } while (true);
                    loop_peg_3226:
                    if (result_peg_3212.matches() == 0){
                        goto out_peg_3225;
                    }
                    
                }
                goto success_peg_3216;
                out_peg_3225:
                goto out_peg_3214;
                success_peg_3216:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3212.getPosition()))){
                        result_peg_3212.nextPosition();
                    } else {
                        goto out_peg_3214;
                    }
                }
                result_peg_3212.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist x");
                    result_peg_3212.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3212;
        stream.update(result_peg_3212.getPosition());
        
        
        return result_peg_3212;
        out_peg_3214:
        Result result_peg_3234(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_3234.getPosition()))){
                        result_peg_3234.nextPosition();
                    } else {
                        goto out_peg_3236;
                    }
                }
                result_peg_3234.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_3234.reset();
                    do{
                        Result result_peg_3249(result_peg_3234.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3249.getPosition()))){
                                    result_peg_3249.nextPosition();
                                } else {
                                    goto out_peg_3252;
                                }
                            }
                            result_peg_3249.setValue((void*) " ");
                                
                        }
                        goto success_peg_3250;
                        out_peg_3252:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3249.getPosition()))){
                                    result_peg_3249.nextPosition();
                                } else {
                                    goto out_peg_3254;
                                }
                            }
                            result_peg_3249.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3250;
                        out_peg_3254:
                        goto loop_peg_3248;
                        success_peg_3250:
                        ;
                        result_peg_3234.addResult(result_peg_3249);
                    } while (true);
                    loop_peg_3248:
                    if (result_peg_3234.matches() == 0){
                        goto out_peg_3247;
                    }
                    
                }
                goto success_peg_3238;
                out_peg_3247:
                goto out_peg_3236;
                success_peg_3238:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3234.getPosition()))){
                        result_peg_3234.nextPosition();
                    } else {
                        goto out_peg_3236;
                    }
                }
                result_peg_3234.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist y");
                    result_peg_3234.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3234;
        stream.update(result_peg_3234.getPosition());
        
        
        return result_peg_3234;
        out_peg_3236:
        Result result_peg_3256(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3256.getPosition()))){
                        result_peg_3256.nextPosition();
                    } else {
                        goto out_peg_3258;
                    }
                }
                result_peg_3256.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3256.reset();
                    do{
                        Result result_peg_3271(result_peg_3256.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3271.getPosition()))){
                                    result_peg_3271.nextPosition();
                                } else {
                                    goto out_peg_3274;
                                }
                            }
                            result_peg_3271.setValue((void*) " ");
                                
                        }
                        goto success_peg_3272;
                        out_peg_3274:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3271.getPosition()))){
                                    result_peg_3271.nextPosition();
                                } else {
                                    goto out_peg_3276;
                                }
                            }
                            result_peg_3271.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3272;
                        out_peg_3276:
                        goto loop_peg_3270;
                        success_peg_3272:
                        ;
                        result_peg_3256.addResult(result_peg_3271);
                    } while (true);
                    loop_peg_3270:
                    if (result_peg_3256.matches() == 0){
                        goto out_peg_3269;
                    }
                    
                }
                goto success_peg_3260;
                out_peg_3269:
                goto out_peg_3258;
                success_peg_3260:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3256.getPosition()))){
                        result_peg_3256.nextPosition();
                    } else {
                        goto out_peg_3258;
                    }
                }
                result_peg_3256.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist x");
                    result_peg_3256.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3256;
        stream.update(result_peg_3256.getPosition());
        
        
        return result_peg_3256;
        out_peg_3258:
        Result result_peg_3278(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3278.getPosition()))){
                        result_peg_3278.nextPosition();
                    } else {
                        goto out_peg_3280;
                    }
                }
                result_peg_3278.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3278.reset();
                    do{
                        Result result_peg_3293(result_peg_3278.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3293.getPosition()))){
                                    result_peg_3293.nextPosition();
                                } else {
                                    goto out_peg_3296;
                                }
                            }
                            result_peg_3293.setValue((void*) " ");
                                
                        }
                        goto success_peg_3294;
                        out_peg_3296:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3293.getPosition()))){
                                    result_peg_3293.nextPosition();
                                } else {
                                    goto out_peg_3298;
                                }
                            }
                            result_peg_3293.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3294;
                        out_peg_3298:
                        goto loop_peg_3292;
                        success_peg_3294:
                        ;
                        result_peg_3278.addResult(result_peg_3293);
                    } while (true);
                    loop_peg_3292:
                    if (result_peg_3278.matches() == 0){
                        goto out_peg_3291;
                    }
                    
                }
                goto success_peg_3282;
                out_peg_3291:
                goto out_peg_3280;
                success_peg_3282:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3278.getPosition()))){
                        result_peg_3278.nextPosition();
                    } else {
                        goto out_peg_3280;
                    }
                }
                result_peg_3278.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist y");
                    result_peg_3278.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3278;
        stream.update(result_peg_3278.getPosition());
        
        
        return result_peg_3278;
        out_peg_3280:
        Result result_peg_3300(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3300.getPosition()))){
                        result_peg_3300.nextPosition();
                    } else {
                        goto out_peg_3302;
                    }
                }
                result_peg_3300.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3300.reset();
                    do{
                        Result result_peg_3315(result_peg_3300.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3315.getPosition()))){
                                    result_peg_3315.nextPosition();
                                } else {
                                    goto out_peg_3318;
                                }
                            }
                            result_peg_3315.setValue((void*) " ");
                                
                        }
                        goto success_peg_3316;
                        out_peg_3318:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3315.getPosition()))){
                                    result_peg_3315.nextPosition();
                                } else {
                                    goto out_peg_3320;
                                }
                            }
                            result_peg_3315.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3316;
                        out_peg_3320:
                        goto loop_peg_3314;
                        success_peg_3316:
                        ;
                        result_peg_3300.addResult(result_peg_3315);
                    } while (true);
                    loop_peg_3314:
                    if (result_peg_3300.matches() == 0){
                        goto out_peg_3313;
                    }
                    
                }
                goto success_peg_3304;
                out_peg_3313:
                goto out_peg_3302;
                success_peg_3304:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3300.getPosition()))){
                        result_peg_3300.nextPosition();
                    } else {
                        goto out_peg_3302;
                    }
                }
                result_peg_3300.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist x");
                    result_peg_3300.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3300;
        stream.update(result_peg_3300.getPosition());
        
        
        return result_peg_3300;
        out_peg_3302:
        Result result_peg_3322(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3322.getPosition()))){
                        result_peg_3322.nextPosition();
                    } else {
                        goto out_peg_3324;
                    }
                }
                result_peg_3322.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3322.reset();
                    do{
                        Result result_peg_3337(result_peg_3322.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3337.getPosition()))){
                                    result_peg_3337.nextPosition();
                                } else {
                                    goto out_peg_3340;
                                }
                            }
                            result_peg_3337.setValue((void*) " ");
                                
                        }
                        goto success_peg_3338;
                        out_peg_3340:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3337.getPosition()))){
                                    result_peg_3337.nextPosition();
                                } else {
                                    goto out_peg_3342;
                                }
                            }
                            result_peg_3337.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3338;
                        out_peg_3342:
                        goto loop_peg_3336;
                        success_peg_3338:
                        ;
                        result_peg_3322.addResult(result_peg_3337);
                    } while (true);
                    loop_peg_3336:
                    if (result_peg_3322.matches() == 0){
                        goto out_peg_3335;
                    }
                    
                }
                goto success_peg_3326;
                out_peg_3335:
                goto out_peg_3324;
                success_peg_3326:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3322.getPosition()))){
                        result_peg_3322.nextPosition();
                    } else {
                        goto out_peg_3324;
                    }
                }
                result_peg_3322.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist y");
                    result_peg_3322.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3322;
        stream.update(result_peg_3322.getPosition());
        
        
        return result_peg_3322;
        out_peg_3324:
        Result result_peg_3344(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3344.getPosition()))){
                        result_peg_3344.nextPosition();
                    } else {
                        goto out_peg_3346;
                    }
                }
                result_peg_3344.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3344.reset();
                    do{
                        Result result_peg_3359(result_peg_3344.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3359.getPosition()))){
                                    result_peg_3359.nextPosition();
                                } else {
                                    goto out_peg_3362;
                                }
                            }
                            result_peg_3359.setValue((void*) " ");
                                
                        }
                        goto success_peg_3360;
                        out_peg_3362:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3359.getPosition()))){
                                    result_peg_3359.nextPosition();
                                } else {
                                    goto out_peg_3364;
                                }
                            }
                            result_peg_3359.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3360;
                        out_peg_3364:
                        goto loop_peg_3358;
                        success_peg_3360:
                        ;
                        result_peg_3344.addResult(result_peg_3359);
                    } while (true);
                    loop_peg_3358:
                    if (result_peg_3344.matches() == 0){
                        goto out_peg_3357;
                    }
                    
                }
                goto success_peg_3348;
                out_peg_3357:
                goto out_peg_3346;
                success_peg_3348:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3344.getPosition()))){
                        result_peg_3344.nextPosition();
                    } else {
                        goto out_peg_3346;
                    }
                }
                result_peg_3344.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist x");
                    result_peg_3344.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3344;
        stream.update(result_peg_3344.getPosition());
        
        
        return result_peg_3344;
        out_peg_3346:
        Result result_peg_3366(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3366.getPosition()))){
                        result_peg_3366.nextPosition();
                    } else {
                        goto out_peg_3368;
                    }
                }
                result_peg_3366.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3366.reset();
                    do{
                        Result result_peg_3381(result_peg_3366.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3381.getPosition()))){
                                    result_peg_3381.nextPosition();
                                } else {
                                    goto out_peg_3384;
                                }
                            }
                            result_peg_3381.setValue((void*) " ");
                                
                        }
                        goto success_peg_3382;
                        out_peg_3384:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3381.getPosition()))){
                                    result_peg_3381.nextPosition();
                                } else {
                                    goto out_peg_3386;
                                }
                            }
                            result_peg_3381.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3382;
                        out_peg_3386:
                        goto loop_peg_3380;
                        success_peg_3382:
                        ;
                        result_peg_3366.addResult(result_peg_3381);
                    } while (true);
                    loop_peg_3380:
                    if (result_peg_3366.matches() == 0){
                        goto out_peg_3379;
                    }
                    
                }
                goto success_peg_3370;
                out_peg_3379:
                goto out_peg_3368;
                success_peg_3370:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3366.getPosition()))){
                        result_peg_3366.nextPosition();
                    } else {
                        goto out_peg_3368;
                    }
                }
                result_peg_3366.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist y");
                    result_peg_3366.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3366;
        stream.update(result_peg_3366.getPosition());
        
        
        return result_peg_3366;
        out_peg_3368:
        Result result_peg_3388(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3388.getPosition()))){
                        result_peg_3388.nextPosition();
                    } else {
                        goto out_peg_3390;
                    }
                }
                result_peg_3388.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3388.reset();
                    do{
                        Result result_peg_3403(result_peg_3388.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3403.getPosition()))){
                                    result_peg_3403.nextPosition();
                                } else {
                                    goto out_peg_3406;
                                }
                            }
                            result_peg_3403.setValue((void*) " ");
                                
                        }
                        goto success_peg_3404;
                        out_peg_3406:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3403.getPosition()))){
                                    result_peg_3403.nextPosition();
                                } else {
                                    goto out_peg_3408;
                                }
                            }
                            result_peg_3403.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3404;
                        out_peg_3408:
                        goto loop_peg_3402;
                        success_peg_3404:
                        ;
                        result_peg_3388.addResult(result_peg_3403);
                    } while (true);
                    loop_peg_3402:
                    if (result_peg_3388.matches() == 0){
                        goto out_peg_3401;
                    }
                    
                }
                goto success_peg_3392;
                out_peg_3401:
                goto out_peg_3390;
                success_peg_3392:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3388.getPosition()))){
                        result_peg_3388.nextPosition();
                    } else {
                        goto out_peg_3390;
                    }
                }
                result_peg_3388.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3388.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3388;
        stream.update(result_peg_3388.getPosition());
        
        
        return result_peg_3388;
        out_peg_3390:
        Result result_peg_3410(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3410.getPosition()))){
                        result_peg_3410.nextPosition();
                    } else {
                        goto out_peg_3412;
                    }
                }
                result_peg_3410.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3410.reset();
                    do{
                        Result result_peg_3425(result_peg_3410.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3425.getPosition()))){
                                    result_peg_3425.nextPosition();
                                } else {
                                    goto out_peg_3428;
                                }
                            }
                            result_peg_3425.setValue((void*) " ");
                                
                        }
                        goto success_peg_3426;
                        out_peg_3428:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3425.getPosition()))){
                                    result_peg_3425.nextPosition();
                                } else {
                                    goto out_peg_3430;
                                }
                            }
                            result_peg_3425.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3426;
                        out_peg_3430:
                        goto loop_peg_3424;
                        success_peg_3426:
                        ;
                        result_peg_3410.addResult(result_peg_3425);
                    } while (true);
                    loop_peg_3424:
                    if (result_peg_3410.matches() == 0){
                        goto out_peg_3423;
                    }
                    
                }
                goto success_peg_3414;
                out_peg_3423:
                goto out_peg_3412;
                success_peg_3414:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3410.getPosition()))){
                        result_peg_3410.nextPosition();
                    } else {
                        goto out_peg_3412;
                    }
                }
                result_peg_3410.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos x");
                    result_peg_3410.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3410;
        stream.update(result_peg_3410.getPosition());
        
        
        return result_peg_3410;
        out_peg_3412:
        Result result_peg_3432(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3432.getPosition()))){
                        result_peg_3432.nextPosition();
                    } else {
                        goto out_peg_3434;
                    }
                }
                result_peg_3432.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3432.reset();
                    do{
                        Result result_peg_3447(result_peg_3432.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3447.getPosition()))){
                                    result_peg_3447.nextPosition();
                                } else {
                                    goto out_peg_3450;
                                }
                            }
                            result_peg_3447.setValue((void*) " ");
                                
                        }
                        goto success_peg_3448;
                        out_peg_3450:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3447.getPosition()))){
                                    result_peg_3447.nextPosition();
                                } else {
                                    goto out_peg_3452;
                                }
                            }
                            result_peg_3447.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3448;
                        out_peg_3452:
                        goto loop_peg_3446;
                        success_peg_3448:
                        ;
                        result_peg_3432.addResult(result_peg_3447);
                    } while (true);
                    loop_peg_3446:
                    if (result_peg_3432.matches() == 0){
                        goto out_peg_3445;
                    }
                    
                }
                goto success_peg_3436;
                out_peg_3445:
                goto out_peg_3434;
                success_peg_3436:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3432.getPosition()))){
                        result_peg_3432.nextPosition();
                    } else {
                        goto out_peg_3434;
                    }
                }
                result_peg_3432.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos y");
                    result_peg_3432.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3432;
        stream.update(result_peg_3432.getPosition());
        
        
        return result_peg_3432;
        out_peg_3434:
        Result result_peg_3454(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3454.getPosition()))){
                        result_peg_3454.nextPosition();
                    } else {
                        goto out_peg_3456;
                    }
                }
                result_peg_3454.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3454.reset();
                    do{
                        Result result_peg_3469(result_peg_3454.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3469.getPosition()))){
                                    result_peg_3469.nextPosition();
                                } else {
                                    goto out_peg_3472;
                                }
                            }
                            result_peg_3469.setValue((void*) " ");
                                
                        }
                        goto success_peg_3470;
                        out_peg_3472:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3469.getPosition()))){
                                    result_peg_3469.nextPosition();
                                } else {
                                    goto out_peg_3474;
                                }
                            }
                            result_peg_3469.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3470;
                        out_peg_3474:
                        goto loop_peg_3468;
                        success_peg_3470:
                        ;
                        result_peg_3454.addResult(result_peg_3469);
                    } while (true);
                    loop_peg_3468:
                    if (result_peg_3454.matches() == 0){
                        goto out_peg_3467;
                    }
                    
                }
                goto success_peg_3458;
                out_peg_3467:
                goto out_peg_3456;
                success_peg_3458:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3454.getPosition()))){
                        result_peg_3454.nextPosition();
                    } else {
                        goto out_peg_3456;
                    }
                }
                result_peg_3454.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3454.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3454;
        stream.update(result_peg_3454.getPosition());
        
        
        return result_peg_3454;
        out_peg_3456:
        Result result_peg_3476(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3476.getPosition()))){
                        result_peg_3476.nextPosition();
                    } else {
                        goto out_peg_3478;
                    }
                }
                result_peg_3476.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3476.reset();
                    do{
                        Result result_peg_3491(result_peg_3476.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3491.getPosition()))){
                                    result_peg_3491.nextPosition();
                                } else {
                                    goto out_peg_3494;
                                }
                            }
                            result_peg_3491.setValue((void*) " ");
                                
                        }
                        goto success_peg_3492;
                        out_peg_3494:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3491.getPosition()))){
                                    result_peg_3491.nextPosition();
                                } else {
                                    goto out_peg_3496;
                                }
                            }
                            result_peg_3491.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3492;
                        out_peg_3496:
                        goto loop_peg_3490;
                        success_peg_3492:
                        ;
                        result_peg_3476.addResult(result_peg_3491);
                    } while (true);
                    loop_peg_3490:
                    if (result_peg_3476.matches() == 0){
                        goto out_peg_3489;
                    }
                    
                }
                goto success_peg_3480;
                out_peg_3489:
                goto out_peg_3478;
                success_peg_3480:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3476.getPosition()))){
                        result_peg_3476.nextPosition();
                    } else {
                        goto out_peg_3478;
                    }
                }
                result_peg_3476.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist y");
                    result_peg_3476.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3476;
        stream.update(result_peg_3476.getPosition());
        
        
        return result_peg_3476;
        out_peg_3478:
        Result result_peg_3498(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3498.getPosition()))){
                        result_peg_3498.nextPosition();
                    } else {
                        goto out_peg_3500;
                    }
                }
                result_peg_3498.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3498.reset();
                    do{
                        Result result_peg_3513(result_peg_3498.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3513.getPosition()))){
                                    result_peg_3513.nextPosition();
                                } else {
                                    goto out_peg_3516;
                                }
                            }
                            result_peg_3513.setValue((void*) " ");
                                
                        }
                        goto success_peg_3514;
                        out_peg_3516:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3513.getPosition()))){
                                    result_peg_3513.nextPosition();
                                } else {
                                    goto out_peg_3518;
                                }
                            }
                            result_peg_3513.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3514;
                        out_peg_3518:
                        goto loop_peg_3512;
                        success_peg_3514:
                        ;
                        result_peg_3498.addResult(result_peg_3513);
                    } while (true);
                    loop_peg_3512:
                    if (result_peg_3498.matches() == 0){
                        goto out_peg_3511;
                    }
                    
                }
                goto success_peg_3502;
                out_peg_3511:
                goto out_peg_3500;
                success_peg_3502:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3498.getPosition()))){
                        result_peg_3498.nextPosition();
                    } else {
                        goto out_peg_3500;
                    }
                }
                result_peg_3498.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist x");
                    result_peg_3498.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3498;
        stream.update(result_peg_3498.getPosition());
        
        
        return result_peg_3498;
        out_peg_3500:
        Result result_peg_3520(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3520.getPosition()))){
                        result_peg_3520.nextPosition();
                    } else {
                        goto out_peg_3522;
                    }
                }
                result_peg_3520.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3520.reset();
                    do{
                        Result result_peg_3535(result_peg_3520.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3535.getPosition()))){
                                    result_peg_3535.nextPosition();
                                } else {
                                    goto out_peg_3538;
                                }
                            }
                            result_peg_3535.setValue((void*) " ");
                                
                        }
                        goto success_peg_3536;
                        out_peg_3538:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3535.getPosition()))){
                                    result_peg_3535.nextPosition();
                                } else {
                                    goto out_peg_3540;
                                }
                            }
                            result_peg_3535.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3536;
                        out_peg_3540:
                        goto loop_peg_3534;
                        success_peg_3536:
                        ;
                        result_peg_3520.addResult(result_peg_3535);
                    } while (true);
                    loop_peg_3534:
                    if (result_peg_3520.matches() == 0){
                        goto out_peg_3533;
                    }
                    
                }
                goto success_peg_3524;
                out_peg_3533:
                goto out_peg_3522;
                success_peg_3524:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3520.getPosition()))){
                        result_peg_3520.nextPosition();
                    } else {
                        goto out_peg_3522;
                    }
                }
                result_peg_3520.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist y");
                    result_peg_3520.setValue(value);
                }
            
            
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3520;
        stream.update(result_peg_3520.getPosition());
        
        
        return result_peg_3520;
        out_peg_3522:
        Result result_peg_3542(myposition);
        
        result_peg_3542 = rule_hitflag(stream, result_peg_3542.getPosition());
        if (result_peg_3542.error()){
            goto out_peg_3543;
        }
        
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = result_peg_3542;
        stream.update(result_peg_3542.getPosition());
        
        
        return result_peg_3542;
        out_peg_3543:
    
        if (column_peg_3123.chunk14 == 0){
            column_peg_3123.chunk14 = new Chunk14();
        }
        column_peg_3123.chunk14->chunk_keyword_real = errorResult;
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



    
    } /* Cmd */
    
} /* Mugen */

        
