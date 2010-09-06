

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

Ast::Value * makeFunction1(const std::string & name, const Value & arg1){
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

Ast::Identifier * makeSimpleIdentifier(int line, int column, const char * name){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(line, column, name);
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

Ast::Value * copyValue(const Value & value){
    Ast::Value * copied = (Ast::Value*) as<Ast::Value*>(value)->copy();
    GC::save(copied);
    return copied;
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

Ast::Value * makeAnimElem(const Value & line, const Value & arg1, const Value & comparison, const Value & arg2){
    /* animelemno == arg1 && animelemtime(arg1) compare arg2 */
    Ast::Value * element;
    Ast::Value * time;
    element = makeExpressionEquals(makeSimpleIdentifier(getCurrentLine(line), getCurrentColumn(line), "anim"), arg1);
    /* make a copy of arg1 because values can only have one parent */
    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
    time = as<compare_func>(comparison)(makeFunction1(std::string("animelemtime"), Value(copyValue(arg1))), arg2);
    return makeExpressionAnd(element, time);
}

Ast::Value * makeAnimElem(const Value & line, const Value & arg1){
    Value comparison = Value((void*) makeExpressionEquals);
    Value arg2 = Value(makeNumber(0));
    return makeAnimElem(line, arg1, comparison, arg2);
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
    
    RuleTrace trace_peg_1870(stream, "all_compare");
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
        
        result_peg_1868 = rule_compare_equal(stream, result_peg_1868.getPosition());
        if (result_peg_1868.error()){
            goto out_peg_1869;
        }
        
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = result_peg_1868;
        stream.update(result_peg_1868.getPosition());
        
        
        return result_peg_1868;
        out_peg_1869:
    
        if (column_peg_1865.chunk8 == 0){
            column_peg_1865.chunk8 = new Chunk8();
        }
        column_peg_1865.chunk8->chunk_all_compare = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1953(stream, "expr10_rest");
    int myposition = position;
    
    tail_peg_1873:
    Value right;
        Value new_left;
    Result result_peg_1872(myposition);
        {
        
            {
                    
                    result_peg_1872.reset();
                    do{
                        Result result_peg_1885(result_peg_1872.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1885.getPosition()))){
                                    result_peg_1885.nextPosition();
                                } else {
                                    goto out_peg_1888;
                                }
                            }
                            result_peg_1885.setValue((void*) " ");
                                
                        }
                        goto success_peg_1886;
                        out_peg_1888:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1885.getPosition()))){
                                    result_peg_1885.nextPosition();
                                } else {
                                    goto out_peg_1890;
                                }
                            }
                            result_peg_1885.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1886;
                        out_peg_1890:
                        goto loop_peg_1884;
                        success_peg_1886:
                        ;
                        result_peg_1872.addResult(result_peg_1885);
                    } while (true);
                    loop_peg_1884:
                    ;
                            
                }
                goto success_peg_1875;
                goto out_peg_1891;
                success_peg_1875:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_1872.getPosition()))){
                        result_peg_1872.nextPosition();
                    } else {
                        goto out_peg_1891;
                    }
                }
                result_peg_1872.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_1872.reset();
                    do{
                        Result result_peg_1904(result_peg_1872.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1904.getPosition()))){
                                    result_peg_1904.nextPosition();
                                } else {
                                    goto out_peg_1907;
                                }
                            }
                            result_peg_1904.setValue((void*) " ");
                                
                        }
                        goto success_peg_1905;
                        out_peg_1907:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1904.getPosition()))){
                                    result_peg_1904.nextPosition();
                                } else {
                                    goto out_peg_1909;
                                }
                            }
                            result_peg_1904.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1905;
                        out_peg_1909:
                        goto loop_peg_1903;
                        success_peg_1905:
                        ;
                        result_peg_1872.addResult(result_peg_1904);
                    } while (true);
                    loop_peg_1903:
                    ;
                            
                }
                goto success_peg_1894;
                goto out_peg_1891;
                success_peg_1894:
                ;
            
            
            
            result_peg_1872 = rule_expr11(stream, result_peg_1872.getPosition());
                if (result_peg_1872.error()){
                    goto out_peg_1891;
                }
                right = result_peg_1872.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionAdd(left, right);
                    result_peg_1872.setValue(value);
                }
                new_left = result_peg_1872.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1872.getPosition();
        goto tail_peg_1873;
        out_peg_1891:
        Result result_peg_1912(myposition);
        {
        
            {
                    
                    result_peg_1912.reset();
                    do{
                        Result result_peg_1924(result_peg_1912.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1924.getPosition()))){
                                    result_peg_1924.nextPosition();
                                } else {
                                    goto out_peg_1927;
                                }
                            }
                            result_peg_1924.setValue((void*) " ");
                                
                        }
                        goto success_peg_1925;
                        out_peg_1927:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1924.getPosition()))){
                                    result_peg_1924.nextPosition();
                                } else {
                                    goto out_peg_1929;
                                }
                            }
                            result_peg_1924.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1925;
                        out_peg_1929:
                        goto loop_peg_1923;
                        success_peg_1925:
                        ;
                        result_peg_1912.addResult(result_peg_1924);
                    } while (true);
                    loop_peg_1923:
                    ;
                            
                }
                goto success_peg_1914;
                goto out_peg_1930;
                success_peg_1914:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_1912.getPosition()))){
                        result_peg_1912.nextPosition();
                    } else {
                        goto out_peg_1930;
                    }
                }
                result_peg_1912.setValue((void*) "-");
            
            
            
            {
                    
                    result_peg_1912.reset();
                    do{
                        Result result_peg_1943(result_peg_1912.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1943.getPosition()))){
                                    result_peg_1943.nextPosition();
                                } else {
                                    goto out_peg_1946;
                                }
                            }
                            result_peg_1943.setValue((void*) " ");
                                
                        }
                        goto success_peg_1944;
                        out_peg_1946:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1943.getPosition()))){
                                    result_peg_1943.nextPosition();
                                } else {
                                    goto out_peg_1948;
                                }
                            }
                            result_peg_1943.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1944;
                        out_peg_1948:
                        goto loop_peg_1942;
                        success_peg_1944:
                        ;
                        result_peg_1912.addResult(result_peg_1943);
                    } while (true);
                    loop_peg_1942:
                    ;
                            
                }
                goto success_peg_1933;
                goto out_peg_1930;
                success_peg_1933:
                ;
            
            
            
            result_peg_1912 = rule_expr11(stream, result_peg_1912.getPosition());
                if (result_peg_1912.error()){
                    goto out_peg_1930;
                }
                right = result_peg_1912.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionSubtract(left, right);
                    result_peg_1912.setValue(value);
                }
                new_left = result_peg_1912.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1912.getPosition();
        goto tail_peg_1873;
        out_peg_1930:
        Result result_peg_1951(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_1951.setValue(value);
                }
            
            
        }
        stream.update(result_peg_1951.getPosition());
        
        return result_peg_1951;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2075(stream, "expr11_rest");
    int myposition = position;
    
    tail_peg_1956:
    Value right;
        Value new_left;
    Result result_peg_1955(myposition);
        {
        
            {
                    
                    result_peg_1955.reset();
                    do{
                        Result result_peg_1968(result_peg_1955.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1968.getPosition()))){
                                    result_peg_1968.nextPosition();
                                } else {
                                    goto out_peg_1971;
                                }
                            }
                            result_peg_1968.setValue((void*) " ");
                                
                        }
                        goto success_peg_1969;
                        out_peg_1971:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1968.getPosition()))){
                                    result_peg_1968.nextPosition();
                                } else {
                                    goto out_peg_1973;
                                }
                            }
                            result_peg_1968.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1969;
                        out_peg_1973:
                        goto loop_peg_1967;
                        success_peg_1969:
                        ;
                        result_peg_1955.addResult(result_peg_1968);
                    } while (true);
                    loop_peg_1967:
                    ;
                            
                }
                goto success_peg_1958;
                goto out_peg_1974;
                success_peg_1958:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("*"[i], stream.get(result_peg_1955.getPosition()))){
                        result_peg_1955.nextPosition();
                    } else {
                        goto out_peg_1974;
                    }
                }
                result_peg_1955.setValue((void*) "*");
            
            
            
            {
                    
                    result_peg_1955.reset();
                    do{
                        Result result_peg_1987(result_peg_1955.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_1987.getPosition()))){
                                    result_peg_1987.nextPosition();
                                } else {
                                    goto out_peg_1990;
                                }
                            }
                            result_peg_1987.setValue((void*) " ");
                                
                        }
                        goto success_peg_1988;
                        out_peg_1990:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_1987.getPosition()))){
                                    result_peg_1987.nextPosition();
                                } else {
                                    goto out_peg_1992;
                                }
                            }
                            result_peg_1987.setValue((void*) "\t");
                                
                        }
                        goto success_peg_1988;
                        out_peg_1992:
                        goto loop_peg_1986;
                        success_peg_1988:
                        ;
                        result_peg_1955.addResult(result_peg_1987);
                    } while (true);
                    loop_peg_1986:
                    ;
                            
                }
                goto success_peg_1977;
                goto out_peg_1974;
                success_peg_1977:
                ;
            
            
            
            result_peg_1955 = rule_expr12(stream, result_peg_1955.getPosition());
                if (result_peg_1955.error()){
                    goto out_peg_1974;
                }
                right = result_peg_1955.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionMultiply(left, right);
                    result_peg_1955.setValue(value);
                }
                new_left = result_peg_1955.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1955.getPosition();
        goto tail_peg_1956;
        out_peg_1974:
        Result result_peg_1995(myposition);
        {
        
            {
                    
                    result_peg_1995.reset();
                    do{
                        Result result_peg_2007(result_peg_1995.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2007.getPosition()))){
                                    result_peg_2007.nextPosition();
                                } else {
                                    goto out_peg_2010;
                                }
                            }
                            result_peg_2007.setValue((void*) " ");
                                
                        }
                        goto success_peg_2008;
                        out_peg_2010:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2007.getPosition()))){
                                    result_peg_2007.nextPosition();
                                } else {
                                    goto out_peg_2012;
                                }
                            }
                            result_peg_2007.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2008;
                        out_peg_2012:
                        goto loop_peg_2006;
                        success_peg_2008:
                        ;
                        result_peg_1995.addResult(result_peg_2007);
                    } while (true);
                    loop_peg_2006:
                    ;
                            
                }
                goto success_peg_1997;
                goto out_peg_2013;
                success_peg_1997:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_1995.getPosition()))){
                        result_peg_1995.nextPosition();
                    } else {
                        goto out_peg_2013;
                    }
                }
                result_peg_1995.setValue((void*) "/");
            
            
            
            {
                    
                    result_peg_1995.reset();
                    do{
                        Result result_peg_2026(result_peg_1995.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2026.getPosition()))){
                                    result_peg_2026.nextPosition();
                                } else {
                                    goto out_peg_2029;
                                }
                            }
                            result_peg_2026.setValue((void*) " ");
                                
                        }
                        goto success_peg_2027;
                        out_peg_2029:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2026.getPosition()))){
                                    result_peg_2026.nextPosition();
                                } else {
                                    goto out_peg_2031;
                                }
                            }
                            result_peg_2026.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2027;
                        out_peg_2031:
                        goto loop_peg_2025;
                        success_peg_2027:
                        ;
                        result_peg_1995.addResult(result_peg_2026);
                    } while (true);
                    loop_peg_2025:
                    ;
                            
                }
                goto success_peg_2016;
                goto out_peg_2013;
                success_peg_2016:
                ;
            
            
            
            result_peg_1995 = rule_expr12(stream, result_peg_1995.getPosition());
                if (result_peg_1995.error()){
                    goto out_peg_2013;
                }
                right = result_peg_1995.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionDivide(left, right);
                    result_peg_1995.setValue(value);
                }
                new_left = result_peg_1995.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_1995.getPosition();
        goto tail_peg_1956;
        out_peg_2013:
        Result result_peg_2034(myposition);
        {
        
            {
                    
                    result_peg_2034.reset();
                    do{
                        Result result_peg_2046(result_peg_2034.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2046.getPosition()))){
                                    result_peg_2046.nextPosition();
                                } else {
                                    goto out_peg_2049;
                                }
                            }
                            result_peg_2046.setValue((void*) " ");
                                
                        }
                        goto success_peg_2047;
                        out_peg_2049:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2046.getPosition()))){
                                    result_peg_2046.nextPosition();
                                } else {
                                    goto out_peg_2051;
                                }
                            }
                            result_peg_2046.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2047;
                        out_peg_2051:
                        goto loop_peg_2045;
                        success_peg_2047:
                        ;
                        result_peg_2034.addResult(result_peg_2046);
                    } while (true);
                    loop_peg_2045:
                    ;
                            
                }
                goto success_peg_2036;
                goto out_peg_2052;
                success_peg_2036:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("%"[i], stream.get(result_peg_2034.getPosition()))){
                        result_peg_2034.nextPosition();
                    } else {
                        goto out_peg_2052;
                    }
                }
                result_peg_2034.setValue((void*) "%");
            
            
            
            {
                    
                    result_peg_2034.reset();
                    do{
                        Result result_peg_2065(result_peg_2034.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2065.getPosition()))){
                                    result_peg_2065.nextPosition();
                                } else {
                                    goto out_peg_2068;
                                }
                            }
                            result_peg_2065.setValue((void*) " ");
                                
                        }
                        goto success_peg_2066;
                        out_peg_2068:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2065.getPosition()))){
                                    result_peg_2065.nextPosition();
                                } else {
                                    goto out_peg_2070;
                                }
                            }
                            result_peg_2065.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2066;
                        out_peg_2070:
                        goto loop_peg_2064;
                        success_peg_2066:
                        ;
                        result_peg_2034.addResult(result_peg_2065);
                    } while (true);
                    loop_peg_2064:
                    ;
                            
                }
                goto success_peg_2055;
                goto out_peg_2052;
                success_peg_2055:
                ;
            
            
            
            result_peg_2034 = rule_expr12(stream, result_peg_2034.getPosition());
                if (result_peg_2034.error()){
                    goto out_peg_2052;
                }
                right = result_peg_2034.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionModulo(left, right);
                    result_peg_2034.setValue(value);
                }
                new_left = result_peg_2034.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2034.getPosition();
        goto tail_peg_1956;
        out_peg_2052:
        Result result_peg_2073(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2073.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2073.getPosition());
        
        return result_peg_2073;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2119(stream, "expr12_rest");
    int myposition = position;
    
    tail_peg_2078:
    Value right;
        Value new_left;
    Result result_peg_2077(myposition);
        {
        
            {
                    
                    result_peg_2077.reset();
                    do{
                        Result result_peg_2090(result_peg_2077.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2090.getPosition()))){
                                    result_peg_2090.nextPosition();
                                } else {
                                    goto out_peg_2093;
                                }
                            }
                            result_peg_2090.setValue((void*) " ");
                                
                        }
                        goto success_peg_2091;
                        out_peg_2093:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2090.getPosition()))){
                                    result_peg_2090.nextPosition();
                                } else {
                                    goto out_peg_2095;
                                }
                            }
                            result_peg_2090.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2091;
                        out_peg_2095:
                        goto loop_peg_2089;
                        success_peg_2091:
                        ;
                        result_peg_2077.addResult(result_peg_2090);
                    } while (true);
                    loop_peg_2089:
                    ;
                            
                }
                goto success_peg_2080;
                goto out_peg_2096;
                success_peg_2080:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("**"[i], stream.get(result_peg_2077.getPosition()))){
                        result_peg_2077.nextPosition();
                    } else {
                        goto out_peg_2096;
                    }
                }
                result_peg_2077.setValue((void*) "**");
            
            
            
            {
                    
                    result_peg_2077.reset();
                    do{
                        Result result_peg_2109(result_peg_2077.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2109.getPosition()))){
                                    result_peg_2109.nextPosition();
                                } else {
                                    goto out_peg_2112;
                                }
                            }
                            result_peg_2109.setValue((void*) " ");
                                
                        }
                        goto success_peg_2110;
                        out_peg_2112:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2109.getPosition()))){
                                    result_peg_2109.nextPosition();
                                } else {
                                    goto out_peg_2114;
                                }
                            }
                            result_peg_2109.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2110;
                        out_peg_2114:
                        goto loop_peg_2108;
                        success_peg_2110:
                        ;
                        result_peg_2077.addResult(result_peg_2109);
                    } while (true);
                    loop_peg_2108:
                    ;
                            
                }
                goto success_peg_2099;
                goto out_peg_2096;
                success_peg_2099:
                ;
            
            
            
            result_peg_2077 = rule_expr13(stream, result_peg_2077.getPosition());
                if (result_peg_2077.error()){
                    goto out_peg_2096;
                }
                right = result_peg_2077.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeExpressionPower(left, right);
                    result_peg_2077.setValue(value);
                }
                new_left = result_peg_2077.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2077.getPosition();
        goto tail_peg_2078;
        out_peg_2096:
        Result result_peg_2117(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2117.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2117.getPosition());
        
        return result_peg_2117;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_2120 = stream.getColumn(position);
    if (column_peg_2120.chunk9 != 0 && column_peg_2120.chunk9->chunk_unary.calculated()){
        return column_peg_2120.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_2138(stream, "unary");
    int myposition = position;
    
    
    
    Result result_peg_2121(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("!"[i], stream.get(result_peg_2121.getPosition()))){
                        result_peg_2121.nextPosition();
                    } else {
                        goto out_peg_2123;
                    }
                }
                result_peg_2121.setValue((void*) "!");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Not;
                    result_peg_2121.setValue(value);
                }
            
            
        }
        
        if (column_peg_2120.chunk9 == 0){
            column_peg_2120.chunk9 = new Chunk9();
        }
        column_peg_2120.chunk9->chunk_unary = result_peg_2121;
        stream.update(result_peg_2121.getPosition());
        
        
        return result_peg_2121;
        out_peg_2123:
        Result result_peg_2124(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2124.getPosition()))){
                        result_peg_2124.nextPosition();
                    } else {
                        goto out_peg_2126;
                    }
                }
                result_peg_2124.setValue((void*) "-");
            
            
            
            Result result_peg_2129(result_peg_2124);
                {
                    
                    char letter_peg_2134 = stream.get(result_peg_2129.getPosition());
                    if (letter_peg_2134 != '\0' && strchr("0123456789", letter_peg_2134) != NULL){
                        result_peg_2129.nextPosition();
                        result_peg_2129.setValue((void*) (long) letter_peg_2134);
                    } else {
                        goto out_peg_2133;
                    }
                    
                }
                goto success_peg_2130;
                out_peg_2133:
                goto not_peg_2128;
                success_peg_2130:
                ;
                goto out_peg_2126;
                not_peg_2128:
                result_peg_2124.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Minus;
                    result_peg_2124.setValue(value);
                }
            
            
        }
        
        if (column_peg_2120.chunk9 == 0){
            column_peg_2120.chunk9 = new Chunk9();
        }
        column_peg_2120.chunk9->chunk_unary = result_peg_2124;
        stream.update(result_peg_2124.getPosition());
        
        
        return result_peg_2124;
        out_peg_2126:
        Result result_peg_2135(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2135.getPosition()))){
                        result_peg_2135.nextPosition();
                    } else {
                        goto out_peg_2137;
                    }
                }
                result_peg_2135.setValue((void*) "~");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) Ast::ExpressionUnary::Negation;
                    result_peg_2135.setValue(value);
                }
            
            
        }
        
        if (column_peg_2120.chunk9 == 0){
            column_peg_2120.chunk9 = new Chunk9();
        }
        column_peg_2120.chunk9->chunk_unary = result_peg_2135;
        stream.update(result_peg_2135.getPosition());
        
        
        return result_peg_2135;
        out_peg_2137:
    
        if (column_peg_2120.chunk9 == 0){
            column_peg_2120.chunk9 = new Chunk9();
        }
        column_peg_2120.chunk9->chunk_unary = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_2139 = stream.getColumn(position);
    if (column_peg_2139.chunk9 != 0 && column_peg_2139.chunk9->chunk_expr13_real.calculated()){
        return column_peg_2139.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_2185(stream, "expr13_real");
    int myposition = position;
    
    
    Value e;
    Result result_peg_2140(myposition);
        
        result_peg_2140 = rule_helper(stream, result_peg_2140.getPosition());
        if (result_peg_2140.error()){
            goto out_peg_2141;
        }
        
        if (column_peg_2139.chunk9 == 0){
            column_peg_2139.chunk9 = new Chunk9();
        }
        column_peg_2139.chunk9->chunk_expr13_real = result_peg_2140;
        stream.update(result_peg_2140.getPosition());
        
        
        return result_peg_2140;
        out_peg_2141:
        Result result_peg_2142(myposition);
        
        result_peg_2142 = rule_function(stream, result_peg_2142.getPosition());
        if (result_peg_2142.error()){
            goto out_peg_2143;
        }
        
        if (column_peg_2139.chunk9 == 0){
            column_peg_2139.chunk9 = new Chunk9();
        }
        column_peg_2139.chunk9->chunk_expr13_real = result_peg_2142;
        stream.update(result_peg_2142.getPosition());
        
        
        return result_peg_2142;
        out_peg_2143:
        Result result_peg_2144(myposition);
        
        result_peg_2144 = rule_value(stream, result_peg_2144.getPosition());
        if (result_peg_2144.error()){
            goto out_peg_2145;
        }
        
        if (column_peg_2139.chunk9 == 0){
            column_peg_2139.chunk9 = new Chunk9();
        }
        column_peg_2139.chunk9->chunk_expr13_real = result_peg_2144;
        stream.update(result_peg_2144.getPosition());
        
        
        return result_peg_2144;
        out_peg_2145:
        Result result_peg_2146(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2146.getPosition()))){
                        result_peg_2146.nextPosition();
                    } else {
                        goto out_peg_2148;
                    }
                }
                result_peg_2146.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2146.reset();
                    do{
                        Result result_peg_2160(result_peg_2146.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2160.getPosition()))){
                                    result_peg_2160.nextPosition();
                                } else {
                                    goto out_peg_2163;
                                }
                            }
                            result_peg_2160.setValue((void*) " ");
                                
                        }
                        goto success_peg_2161;
                        out_peg_2163:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2160.getPosition()))){
                                    result_peg_2160.nextPosition();
                                } else {
                                    goto out_peg_2165;
                                }
                            }
                            result_peg_2160.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2161;
                        out_peg_2165:
                        goto loop_peg_2159;
                        success_peg_2161:
                        ;
                        result_peg_2146.addResult(result_peg_2160);
                    } while (true);
                    loop_peg_2159:
                    ;
                            
                }
                goto success_peg_2150;
                goto out_peg_2148;
                success_peg_2150:
                ;
            
            
            
            result_peg_2146 = rule_expr(stream, result_peg_2146.getPosition());
                if (result_peg_2146.error()){
                    goto out_peg_2148;
                }
                e = result_peg_2146.getValues();
            
            
            
            {
                    
                    result_peg_2146.reset();
                    do{
                        Result result_peg_2178(result_peg_2146.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2178.getPosition()))){
                                    result_peg_2178.nextPosition();
                                } else {
                                    goto out_peg_2181;
                                }
                            }
                            result_peg_2178.setValue((void*) " ");
                                
                        }
                        goto success_peg_2179;
                        out_peg_2181:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2178.getPosition()))){
                                    result_peg_2178.nextPosition();
                                } else {
                                    goto out_peg_2183;
                                }
                            }
                            result_peg_2178.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2179;
                        out_peg_2183:
                        goto loop_peg_2177;
                        success_peg_2179:
                        ;
                        result_peg_2146.addResult(result_peg_2178);
                    } while (true);
                    loop_peg_2177:
                    ;
                            
                }
                goto success_peg_2168;
                goto out_peg_2148;
                success_peg_2168:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2146.getPosition()))){
                        result_peg_2146.nextPosition();
                    } else {
                        goto out_peg_2148;
                    }
                }
                result_peg_2146.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = e;
                    result_peg_2146.setValue(value);
                }
            
            
        }
        
        if (column_peg_2139.chunk9 == 0){
            column_peg_2139.chunk9 = new Chunk9();
        }
        column_peg_2139.chunk9->chunk_expr13_real = result_peg_2146;
        stream.update(result_peg_2146.getPosition());
        
        
        return result_peg_2146;
        out_peg_2148:
    
        if (column_peg_2139.chunk9 == 0){
            column_peg_2139.chunk9 = new Chunk9();
        }
        column_peg_2139.chunk9->chunk_expr13_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_2186 = stream.getColumn(position);
    if (column_peg_2186.chunk9 != 0 && column_peg_2186.chunk9->chunk_function.calculated()){
        return column_peg_2186.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2575(stream, "function");
    int myposition = position;
    
    
    Value name;
        Value args;
        Value arg1;
        Value line;
        Value comparison;
        Value arg2;
        Value id;
        Value state;
        Value attributes;
    Result result_peg_2187(myposition);
        
        {
        
            result_peg_2187 = rule_function_name(stream, result_peg_2187.getPosition());
                if (result_peg_2187.error()){
                    goto out_peg_2189;
                }
                name = result_peg_2187.getValues();
            
            
            
            {
                    
                    result_peg_2187.reset();
                    do{
                        Result result_peg_2201(result_peg_2187.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2201.getPosition()))){
                                    result_peg_2201.nextPosition();
                                } else {
                                    goto out_peg_2204;
                                }
                            }
                            result_peg_2201.setValue((void*) " ");
                                
                        }
                        goto success_peg_2202;
                        out_peg_2204:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2201.getPosition()))){
                                    result_peg_2201.nextPosition();
                                } else {
                                    goto out_peg_2206;
                                }
                            }
                            result_peg_2201.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2202;
                        out_peg_2206:
                        goto loop_peg_2200;
                        success_peg_2202:
                        ;
                        result_peg_2187.addResult(result_peg_2201);
                    } while (true);
                    loop_peg_2200:
                    ;
                            
                }
                goto success_peg_2191;
                goto out_peg_2189;
                success_peg_2191:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_2187.getPosition()))){
                        result_peg_2187.nextPosition();
                    } else {
                        goto out_peg_2189;
                    }
                }
                result_peg_2187.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_2187.reset();
                    do{
                        Result result_peg_2219(result_peg_2187.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2219.getPosition()))){
                                    result_peg_2219.nextPosition();
                                } else {
                                    goto out_peg_2222;
                                }
                            }
                            result_peg_2219.setValue((void*) " ");
                                
                        }
                        goto success_peg_2220;
                        out_peg_2222:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2219.getPosition()))){
                                    result_peg_2219.nextPosition();
                                } else {
                                    goto out_peg_2224;
                                }
                            }
                            result_peg_2219.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2220;
                        out_peg_2224:
                        goto loop_peg_2218;
                        success_peg_2220:
                        ;
                        result_peg_2187.addResult(result_peg_2219);
                    } while (true);
                    loop_peg_2218:
                    ;
                            
                }
                goto success_peg_2209;
                goto out_peg_2189;
                success_peg_2209:
                ;
            
            
            
            result_peg_2187 = rule_args(stream, result_peg_2187.getPosition());
                if (result_peg_2187.error()){
                    goto out_peg_2189;
                }
                args = result_peg_2187.getValues();
            
            
            
            {
                    
                    result_peg_2187.reset();
                    do{
                        Result result_peg_2237(result_peg_2187.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2237.getPosition()))){
                                    result_peg_2237.nextPosition();
                                } else {
                                    goto out_peg_2240;
                                }
                            }
                            result_peg_2237.setValue((void*) " ");
                                
                        }
                        goto success_peg_2238;
                        out_peg_2240:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2237.getPosition()))){
                                    result_peg_2237.nextPosition();
                                } else {
                                    goto out_peg_2242;
                                }
                            }
                            result_peg_2237.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2238;
                        out_peg_2242:
                        goto loop_peg_2236;
                        success_peg_2238:
                        ;
                        result_peg_2187.addResult(result_peg_2237);
                    } while (true);
                    loop_peg_2236:
                    ;
                            
                }
                goto success_peg_2227;
                goto out_peg_2189;
                success_peg_2227:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_2187.getPosition()))){
                        result_peg_2187.nextPosition();
                    } else {
                        goto out_peg_2189;
                    }
                }
                result_peg_2187.setValue((void*) ")");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction(name, args);
                    result_peg_2187.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2187;
        stream.update(result_peg_2187.getPosition());
        
        
        return result_peg_2187;
        out_peg_2189:
        Result result_peg_2244(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2244.getPosition()))){
                        result_peg_2244.nextPosition();
                    } else {
                        goto out_peg_2246;
                    }
                }
                result_peg_2244.setValue((void*) "teammode");
                name = result_peg_2244.getValues();
            
            
            
            {
                    
                    result_peg_2244.reset();
                    do{
                        Result result_peg_2258(result_peg_2244.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2258.getPosition()))){
                                    result_peg_2258.nextPosition();
                                } else {
                                    goto out_peg_2261;
                                }
                            }
                            result_peg_2258.setValue((void*) " ");
                                
                        }
                        goto success_peg_2259;
                        out_peg_2261:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2258.getPosition()))){
                                    result_peg_2258.nextPosition();
                                } else {
                                    goto out_peg_2263;
                                }
                            }
                            result_peg_2258.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2259;
                        out_peg_2263:
                        goto loop_peg_2257;
                        success_peg_2259:
                        ;
                        result_peg_2244.addResult(result_peg_2258);
                    } while (true);
                    loop_peg_2257:
                    ;
                            
                }
                goto success_peg_2248;
                goto out_peg_2246;
                success_peg_2248:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2244.getPosition()))){
                        result_peg_2244.nextPosition();
                    } else {
                        goto out_peg_2246;
                    }
                }
                result_peg_2244.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2244.reset();
                    do{
                        Result result_peg_2276(result_peg_2244.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2276.getPosition()))){
                                    result_peg_2276.nextPosition();
                                } else {
                                    goto out_peg_2279;
                                }
                            }
                            result_peg_2276.setValue((void*) " ");
                                
                        }
                        goto success_peg_2277;
                        out_peg_2279:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2276.getPosition()))){
                                    result_peg_2276.nextPosition();
                                } else {
                                    goto out_peg_2281;
                                }
                            }
                            result_peg_2276.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2277;
                        out_peg_2281:
                        goto loop_peg_2275;
                        success_peg_2277:
                        ;
                        result_peg_2244.addResult(result_peg_2276);
                    } while (true);
                    loop_peg_2275:
                    ;
                            
                }
                goto success_peg_2266;
                goto out_peg_2246;
                success_peg_2266:
                ;
            
            
            
            result_peg_2244 = rule_identifier(stream, result_peg_2244.getPosition());
                if (result_peg_2244.error()){
                    goto out_peg_2246;
                }
                arg1 = result_peg_2244.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(name, arg1);
                    result_peg_2244.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2244;
        stream.update(result_peg_2244.getPosition());
        
        
        return result_peg_2244;
        out_peg_2246:
        Result result_peg_2283(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("teammode"[i], stream.get(result_peg_2283.getPosition()))){
                        result_peg_2283.nextPosition();
                    } else {
                        goto out_peg_2285;
                    }
                }
                result_peg_2283.setValue((void*) "teammode");
                name = result_peg_2283.getValues();
            
            
            
            {
                    
                    result_peg_2283.reset();
                    do{
                        Result result_peg_2297(result_peg_2283.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2297.getPosition()))){
                                    result_peg_2297.nextPosition();
                                } else {
                                    goto out_peg_2300;
                                }
                            }
                            result_peg_2297.setValue((void*) " ");
                                
                        }
                        goto success_peg_2298;
                        out_peg_2300:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2297.getPosition()))){
                                    result_peg_2297.nextPosition();
                                } else {
                                    goto out_peg_2302;
                                }
                            }
                            result_peg_2297.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2298;
                        out_peg_2302:
                        goto loop_peg_2296;
                        success_peg_2298:
                        ;
                        result_peg_2283.addResult(result_peg_2297);
                    } while (true);
                    loop_peg_2296:
                    ;
                            
                }
                goto success_peg_2287;
                goto out_peg_2285;
                success_peg_2287:
                ;
            
            
            
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_2283.getPosition()))){
                        result_peg_2283.nextPosition();
                    } else {
                        goto out_peg_2285;
                    }
                }
                result_peg_2283.setValue((void*) "!=");
            
            
            
            {
                    
                    result_peg_2283.reset();
                    do{
                        Result result_peg_2315(result_peg_2283.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2315.getPosition()))){
                                    result_peg_2315.nextPosition();
                                } else {
                                    goto out_peg_2318;
                                }
                            }
                            result_peg_2315.setValue((void*) " ");
                                
                        }
                        goto success_peg_2316;
                        out_peg_2318:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2315.getPosition()))){
                                    result_peg_2315.nextPosition();
                                } else {
                                    goto out_peg_2320;
                                }
                            }
                            result_peg_2315.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2316;
                        out_peg_2320:
                        goto loop_peg_2314;
                        success_peg_2316:
                        ;
                        result_peg_2283.addResult(result_peg_2315);
                    } while (true);
                    loop_peg_2314:
                    ;
                            
                }
                goto success_peg_2305;
                goto out_peg_2285;
                success_peg_2305:
                ;
            
            
            
            result_peg_2283 = rule_identifier(stream, result_peg_2283.getPosition());
                if (result_peg_2283.error()){
                    goto out_peg_2285;
                }
                arg1 = result_peg_2283.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeFunction1(std::string("teammode!="), arg1);
                    result_peg_2283.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2283;
        stream.update(result_peg_2283.getPosition());
        
        
        return result_peg_2283;
        out_peg_2285:
        Result result_peg_2322(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_2324 = stream.getLineInfo(result_peg_2322.getPosition());
                line = &line_info_peg_2324;
            
            
            
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2322.getPosition()))){
                        result_peg_2322.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2322.setValue((void*) "animelem");
            
            
            
            {
                    
                    result_peg_2322.reset();
                    do{
                        Result result_peg_2338(result_peg_2322.getPosition());
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
                        result_peg_2322.addResult(result_peg_2338);
                    } while (true);
                    loop_peg_2337:
                    ;
                            
                }
                goto success_peg_2328;
                goto out_peg_2326;
                success_peg_2328:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2322.getPosition()))){
                        result_peg_2322.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2322.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2322.reset();
                    do{
                        Result result_peg_2356(result_peg_2322.getPosition());
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
                        result_peg_2322.addResult(result_peg_2356);
                    } while (true);
                    loop_peg_2355:
                    ;
                            
                }
                goto success_peg_2346;
                goto out_peg_2326;
                success_peg_2346:
                ;
            
            
            
            result_peg_2322 = rule_integer(stream, result_peg_2322.getPosition());
                if (result_peg_2322.error()){
                    goto out_peg_2326;
                }
                arg1 = result_peg_2322.getValues();
            
            
            
            {
                    
                    result_peg_2322.reset();
                    do{
                        Result result_peg_2374(result_peg_2322.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2374.getPosition()))){
                                    result_peg_2374.nextPosition();
                                } else {
                                    goto out_peg_2377;
                                }
                            }
                            result_peg_2374.setValue((void*) " ");
                                
                        }
                        goto success_peg_2375;
                        out_peg_2377:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2374.getPosition()))){
                                    result_peg_2374.nextPosition();
                                } else {
                                    goto out_peg_2379;
                                }
                            }
                            result_peg_2374.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2375;
                        out_peg_2379:
                        goto loop_peg_2373;
                        success_peg_2375:
                        ;
                        result_peg_2322.addResult(result_peg_2374);
                    } while (true);
                    loop_peg_2373:
                    ;
                            
                }
                goto success_peg_2364;
                goto out_peg_2326;
                success_peg_2364:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2322.getPosition()))){
                        result_peg_2322.nextPosition();
                    } else {
                        goto out_peg_2326;
                    }
                }
                result_peg_2322.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2322.reset();
                    do{
                        Result result_peg_2392(result_peg_2322.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2392.getPosition()))){
                                    result_peg_2392.nextPosition();
                                } else {
                                    goto out_peg_2395;
                                }
                            }
                            result_peg_2392.setValue((void*) " ");
                                
                        }
                        goto success_peg_2393;
                        out_peg_2395:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2392.getPosition()))){
                                    result_peg_2392.nextPosition();
                                } else {
                                    goto out_peg_2397;
                                }
                            }
                            result_peg_2392.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2393;
                        out_peg_2397:
                        goto loop_peg_2391;
                        success_peg_2393:
                        ;
                        result_peg_2322.addResult(result_peg_2392);
                    } while (true);
                    loop_peg_2391:
                    ;
                            
                }
                goto success_peg_2382;
                goto out_peg_2326;
                success_peg_2382:
                ;
            
            
            
            result_peg_2322 = rule_all_compare(stream, result_peg_2322.getPosition());
                if (result_peg_2322.error()){
                    goto out_peg_2326;
                }
                comparison = result_peg_2322.getValues();
            
            
            
            {
                    
                    result_peg_2322.reset();
                    do{
                        Result result_peg_2410(result_peg_2322.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2410.getPosition()))){
                                    result_peg_2410.nextPosition();
                                } else {
                                    goto out_peg_2413;
                                }
                            }
                            result_peg_2410.setValue((void*) " ");
                                
                        }
                        goto success_peg_2411;
                        out_peg_2413:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2410.getPosition()))){
                                    result_peg_2410.nextPosition();
                                } else {
                                    goto out_peg_2415;
                                }
                            }
                            result_peg_2410.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2411;
                        out_peg_2415:
                        goto loop_peg_2409;
                        success_peg_2411:
                        ;
                        result_peg_2322.addResult(result_peg_2410);
                    } while (true);
                    loop_peg_2409:
                    ;
                            
                }
                goto success_peg_2400;
                goto out_peg_2326;
                success_peg_2400:
                ;
            
            
            
            result_peg_2322 = rule_integer(stream, result_peg_2322.getPosition());
                if (result_peg_2322.error()){
                    goto out_peg_2326;
                }
                arg2 = result_peg_2322.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAnimElem(line, arg1, comparison, arg2);
                    result_peg_2322.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2322;
        stream.update(result_peg_2322.getPosition());
        
        
        return result_peg_2322;
        out_peg_2326:
        Result result_peg_2417(myposition);
        
        {
        
            Stream::LineInfo line_info_peg_2419 = stream.getLineInfo(result_peg_2417.getPosition());
                line = &line_info_peg_2419;
            
            
            
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("animelem"[i], stream.get(result_peg_2417.getPosition()))){
                        result_peg_2417.nextPosition();
                    } else {
                        goto out_peg_2421;
                    }
                }
                result_peg_2417.setValue((void*) "animelem");
            
            
            
            {
                    
                    result_peg_2417.reset();
                    do{
                        Result result_peg_2433(result_peg_2417.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2433.getPosition()))){
                                    result_peg_2433.nextPosition();
                                } else {
                                    goto out_peg_2436;
                                }
                            }
                            result_peg_2433.setValue((void*) " ");
                                
                        }
                        goto success_peg_2434;
                        out_peg_2436:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2433.getPosition()))){
                                    result_peg_2433.nextPosition();
                                } else {
                                    goto out_peg_2438;
                                }
                            }
                            result_peg_2433.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2434;
                        out_peg_2438:
                        goto loop_peg_2432;
                        success_peg_2434:
                        ;
                        result_peg_2417.addResult(result_peg_2433);
                    } while (true);
                    loop_peg_2432:
                    ;
                            
                }
                goto success_peg_2423;
                goto out_peg_2421;
                success_peg_2423:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2417.getPosition()))){
                        result_peg_2417.nextPosition();
                    } else {
                        goto out_peg_2421;
                    }
                }
                result_peg_2417.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2417.reset();
                    do{
                        Result result_peg_2451(result_peg_2417.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2451.getPosition()))){
                                    result_peg_2451.nextPosition();
                                } else {
                                    goto out_peg_2454;
                                }
                            }
                            result_peg_2451.setValue((void*) " ");
                                
                        }
                        goto success_peg_2452;
                        out_peg_2454:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2451.getPosition()))){
                                    result_peg_2451.nextPosition();
                                } else {
                                    goto out_peg_2456;
                                }
                            }
                            result_peg_2451.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2452;
                        out_peg_2456:
                        goto loop_peg_2450;
                        success_peg_2452:
                        ;
                        result_peg_2417.addResult(result_peg_2451);
                    } while (true);
                    loop_peg_2450:
                    ;
                            
                }
                goto success_peg_2441;
                goto out_peg_2421;
                success_peg_2441:
                ;
            
            
            
            result_peg_2417 = rule_integer(stream, result_peg_2417.getPosition());
                if (result_peg_2417.error()){
                    goto out_peg_2421;
                }
                arg1 = result_peg_2417.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeAnimElem(line, arg1);
                    result_peg_2417.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2417;
        stream.update(result_peg_2417.getPosition());
        
        
        return result_peg_2417;
        out_peg_2421:
        Result result_peg_2458(myposition);
        
        {
        
            for (int i = 0; i < 11; i++){
                    if (compareCharCase("projcontact"[i], stream.get(result_peg_2458.getPosition()))){
                        result_peg_2458.nextPosition();
                    } else {
                        goto out_peg_2460;
                    }
                }
                result_peg_2458.setValue((void*) "projcontact");
                name = result_peg_2458.getValues();
            
            
            
            int save_peg_2462 = result_peg_2458.getPosition();
                
                result_peg_2458 = rule_integer(stream, result_peg_2458.getPosition());
                if (result_peg_2458.error()){
                    
                    result_peg_2458 = Result(save_peg_2462);
                    result_peg_2458.setValue((void*) 0);
                    
                }
                id = result_peg_2458.getValues();
            
            
            
            {
                    
                    result_peg_2458.reset();
                    do{
                        Result result_peg_2474(result_peg_2458.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2474.getPosition()))){
                                    result_peg_2474.nextPosition();
                                } else {
                                    goto out_peg_2477;
                                }
                            }
                            result_peg_2474.setValue((void*) " ");
                                
                        }
                        goto success_peg_2475;
                        out_peg_2477:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2474.getPosition()))){
                                    result_peg_2474.nextPosition();
                                } else {
                                    goto out_peg_2479;
                                }
                            }
                            result_peg_2474.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2475;
                        out_peg_2479:
                        goto loop_peg_2473;
                        success_peg_2475:
                        ;
                        result_peg_2458.addResult(result_peg_2474);
                    } while (true);
                    loop_peg_2473:
                    ;
                            
                }
                goto success_peg_2464;
                goto out_peg_2460;
                success_peg_2464:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2458.getPosition()))){
                        result_peg_2458.nextPosition();
                    } else {
                        goto out_peg_2460;
                    }
                }
                result_peg_2458.setValue((void*) "=");
            
            
            
            {
                    
                    result_peg_2458.reset();
                    do{
                        Result result_peg_2492(result_peg_2458.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2492.getPosition()))){
                                    result_peg_2492.nextPosition();
                                } else {
                                    goto out_peg_2495;
                                }
                            }
                            result_peg_2492.setValue((void*) " ");
                                
                        }
                        goto success_peg_2493;
                        out_peg_2495:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2492.getPosition()))){
                                    result_peg_2492.nextPosition();
                                } else {
                                    goto out_peg_2497;
                                }
                            }
                            result_peg_2492.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2493;
                        out_peg_2497:
                        goto loop_peg_2491;
                        success_peg_2493:
                        ;
                        result_peg_2458.addResult(result_peg_2492);
                    } while (true);
                    loop_peg_2491:
                    ;
                            
                }
                goto success_peg_2482;
                goto out_peg_2460;
                success_peg_2482:
                ;
            
            
            
            result_peg_2458 = rule_integer(stream, result_peg_2458.getPosition());
                if (result_peg_2458.error()){
                    goto out_peg_2460;
                }
                arg1 = result_peg_2458.getValues();
            
            
            
            result_peg_2458 = rule_function_rest(stream, result_peg_2458.getPosition());
                if (result_peg_2458.error()){
                    goto out_peg_2460;
                }
            
            
            
            {
                    Value value((void*) 0);
                    /* FIXME: handle function rest */
                                    std::string name = combine("projcontact", id);
                                    value = makeFunction(name, arg1);
                    result_peg_2458.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2458;
        stream.update(result_peg_2458.getPosition());
        
        
        return result_peg_2458;
        out_peg_2460:
        Result result_peg_2500(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("hitdefattr"[i], stream.get(result_peg_2500.getPosition()))){
                        result_peg_2500.nextPosition();
                    } else {
                        goto out_peg_2502;
                    }
                }
                result_peg_2500.setValue((void*) "hitdefattr");
            
            
            
            {
                    
                    result_peg_2500.reset();
                    do{
                        Result result_peg_2514(result_peg_2500.getPosition());
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
                        result_peg_2500.addResult(result_peg_2514);
                    } while (true);
                    loop_peg_2513:
                    ;
                            
                }
                goto success_peg_2504;
                goto out_peg_2502;
                success_peg_2504:
                ;
            
            
            
            result_peg_2500 = rule_compare_equal(stream, result_peg_2500.getPosition());
                if (result_peg_2500.error()){
                    goto out_peg_2502;
                }
                comparison = result_peg_2500.getValues();
            
            
            
            {
                    
                    result_peg_2500.reset();
                    do{
                        Result result_peg_2532(result_peg_2500.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2532.getPosition()))){
                                    result_peg_2532.nextPosition();
                                } else {
                                    goto out_peg_2535;
                                }
                            }
                            result_peg_2532.setValue((void*) " ");
                                
                        }
                        goto success_peg_2533;
                        out_peg_2535:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2532.getPosition()))){
                                    result_peg_2532.nextPosition();
                                } else {
                                    goto out_peg_2537;
                                }
                            }
                            result_peg_2532.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2533;
                        out_peg_2537:
                        goto loop_peg_2531;
                        success_peg_2533:
                        ;
                        result_peg_2500.addResult(result_peg_2532);
                    } while (true);
                    loop_peg_2531:
                    ;
                            
                }
                goto success_peg_2522;
                goto out_peg_2502;
                success_peg_2522:
                ;
            
            
            
            result_peg_2500 = rule_hitdef__attribute(stream, result_peg_2500.getPosition());
                if (result_peg_2500.error()){
                    goto out_peg_2502;
                }
                state = result_peg_2500.getValues();
            
            
            
            {
                    
                    result_peg_2500.reset();
                    do{
                        Result result_peg_2550(result_peg_2500.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2550.getPosition()))){
                                    result_peg_2550.nextPosition();
                                } else {
                                    goto out_peg_2553;
                                }
                            }
                            result_peg_2550.setValue((void*) " ");
                                
                        }
                        goto success_peg_2551;
                        out_peg_2553:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2550.getPosition()))){
                                    result_peg_2550.nextPosition();
                                } else {
                                    goto out_peg_2555;
                                }
                            }
                            result_peg_2550.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2551;
                        out_peg_2555:
                        goto loop_peg_2549;
                        success_peg_2551:
                        ;
                        result_peg_2500.addResult(result_peg_2550);
                    } while (true);
                    loop_peg_2549:
                    ;
                            
                }
                goto success_peg_2540;
                goto out_peg_2502;
                success_peg_2540:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2500.getPosition()))){
                        result_peg_2500.nextPosition();
                    } else {
                        goto out_peg_2502;
                    }
                }
                result_peg_2500.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2500.reset();
                    do{
                        Result result_peg_2568(result_peg_2500.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2568.getPosition()))){
                                    result_peg_2568.nextPosition();
                                } else {
                                    goto out_peg_2571;
                                }
                            }
                            result_peg_2568.setValue((void*) " ");
                                
                        }
                        goto success_peg_2569;
                        out_peg_2571:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2568.getPosition()))){
                                    result_peg_2568.nextPosition();
                                } else {
                                    goto out_peg_2573;
                                }
                            }
                            result_peg_2568.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2569;
                        out_peg_2573:
                        goto loop_peg_2567;
                        success_peg_2569:
                        ;
                        result_peg_2500.addResult(result_peg_2568);
                    } while (true);
                    loop_peg_2567:
                    ;
                            
                }
                goto success_peg_2558;
                goto out_peg_2502;
                success_peg_2558:
                ;
            
            
            
            result_peg_2500 = rule_hitdef__attack__attribute(stream, result_peg_2500.getPosition());
                if (result_peg_2500.error()){
                    goto out_peg_2502;
                }
                attributes = result_peg_2500.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                                     Ast::Value * compare_state = as<compare_func>(comparison)(makeSimpleIdentifier("hitdefattr:state"), state);
                                     Ast::Value * compare_attribute = as<compare_func>(comparison)(makeSimpleIdentifier("hitdefattr:attribute"), attributes);
                                     value = makeExpressionAnd(compare_state, compare_attribute);
                    result_peg_2500.setValue(value);
                }
            
            
        }
        
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = result_peg_2500;
        stream.update(result_peg_2500.getPosition());
        
        
        return result_peg_2500;
        out_peg_2502:
    
        if (column_peg_2186.chunk9 == 0){
            column_peg_2186.chunk9 = new Chunk9();
        }
        column_peg_2186.chunk9->chunk_function = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_compare_equal(Stream & stream, const int position){
    
    Column & column_peg_2576 = stream.getColumn(position);
    if (column_peg_2576.chunk9 != 0 && column_peg_2576.chunk9->chunk_compare_equal.calculated()){
        return column_peg_2576.chunk9->chunk_compare_equal;
    }
    
    RuleTrace trace_peg_2583(stream, "compare_equal");
    int myposition = position;
    
    
    
    Result result_peg_2577(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("="[i], stream.get(result_peg_2577.getPosition()))){
                        result_peg_2577.nextPosition();
                    } else {
                        goto out_peg_2579;
                    }
                }
                result_peg_2577.setValue((void*) "=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) makeExpressionEquals;
                    result_peg_2577.setValue(value);
                }
            
            
        }
        
        if (column_peg_2576.chunk9 == 0){
            column_peg_2576.chunk9 = new Chunk9();
        }
        column_peg_2576.chunk9->chunk_compare_equal = result_peg_2577;
        stream.update(result_peg_2577.getPosition());
        
        
        return result_peg_2577;
        out_peg_2579:
        Result result_peg_2580(myposition);
        
        {
        
            for (int i = 0; i < 2; i++){
                    if (compareChar("!="[i], stream.get(result_peg_2580.getPosition()))){
                        result_peg_2580.nextPosition();
                    } else {
                        goto out_peg_2582;
                    }
                }
                result_peg_2580.setValue((void*) "!=");
            
            
            
            {
                    Value value((void*) 0);
                    value = (void*) makeExpressionUnequals;
                    result_peg_2580.setValue(value);
                }
            
            
        }
        
        if (column_peg_2576.chunk9 == 0){
            column_peg_2576.chunk9 = new Chunk9();
        }
        column_peg_2576.chunk9->chunk_compare_equal = result_peg_2580;
        stream.update(result_peg_2580.getPosition());
        
        
        return result_peg_2580;
        out_peg_2582:
    
        if (column_peg_2576.chunk9 == 0){
            column_peg_2576.chunk9 = new Chunk9();
        }
        column_peg_2576.chunk9->chunk_compare_equal = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute(Stream & stream, const int position){
    
    Column & column_peg_2584 = stream.getColumn(position);
    if (column_peg_2584.chunk10 != 0 && column_peg_2584.chunk10->chunk_hitdef__attack__attribute.calculated()){
        return column_peg_2584.chunk10->chunk_hitdef__attack__attribute;
    }
    
    RuleTrace trace_peg_2627(stream, "hitdef__attack__attribute");
    int myposition = position;
    
    
    Value attribute;
    Result result_peg_2585(myposition);
        
        {
        
            {
                    Value value((void*) 0);
                    value = makeHitDefAttackAttribute();
                    result_peg_2585.setValue(value);
                }
                attribute = result_peg_2585.getValues();
            
            
            
            result_peg_2585 = rule_hitdef__attack__attribute__item(stream, result_peg_2585.getPosition(), attribute);
                if (result_peg_2585.error()){
                    goto out_peg_2588;
                }
            
            
            
            result_peg_2585.reset();
                do{
                    Result result_peg_2591(result_peg_2585.getPosition());
                    {
                    
                        {
                                
                                result_peg_2591.reset();
                                do{
                                    Result result_peg_2603(result_peg_2591.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2603.getPosition()))){
                                                result_peg_2603.nextPosition();
                                            } else {
                                                goto out_peg_2606;
                                            }
                                        }
                                        result_peg_2603.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2604;
                                    out_peg_2606:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2603.getPosition()))){
                                                result_peg_2603.nextPosition();
                                            } else {
                                                goto out_peg_2608;
                                            }
                                        }
                                        result_peg_2603.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2604;
                                    out_peg_2608:
                                    goto loop_peg_2602;
                                    success_peg_2604:
                                    ;
                                    result_peg_2591.addResult(result_peg_2603);
                                } while (true);
                                loop_peg_2602:
                                ;
                                        
                            }
                            goto success_peg_2593;
                            goto loop_peg_2590;
                            success_peg_2593:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2591.getPosition()))){
                                    result_peg_2591.nextPosition();
                                } else {
                                    goto loop_peg_2590;
                                }
                            }
                            result_peg_2591.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2591.reset();
                                do{
                                    Result result_peg_2621(result_peg_2591.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2621.getPosition()))){
                                                result_peg_2621.nextPosition();
                                            } else {
                                                goto out_peg_2624;
                                            }
                                        }
                                        result_peg_2621.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2622;
                                    out_peg_2624:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2621.getPosition()))){
                                                result_peg_2621.nextPosition();
                                            } else {
                                                goto out_peg_2626;
                                            }
                                        }
                                        result_peg_2621.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2622;
                                    out_peg_2626:
                                    goto loop_peg_2620;
                                    success_peg_2622:
                                    ;
                                    result_peg_2591.addResult(result_peg_2621);
                                } while (true);
                                loop_peg_2620:
                                ;
                                        
                            }
                            goto success_peg_2611;
                            goto loop_peg_2590;
                            success_peg_2611:
                            ;
                        
                        
                        
                        result_peg_2591 = rule_hitdef__attack__attribute__item(stream, result_peg_2591.getPosition(), attribute);
                            if (result_peg_2591.error()){
                                goto loop_peg_2590;
                            }
                        
                        
                    }
                    result_peg_2585.addResult(result_peg_2591);
                } while (true);
                loop_peg_2590:
                ;
            
            
            
            {
                    Value value((void*) 0);
                    as<Ast::HitDefAttackAttribute*>(attribute)->reset(); value = attribute;
                    result_peg_2585.setValue(value);
                }
            
            
        }
        
        if (column_peg_2584.chunk10 == 0){
            column_peg_2584.chunk10 = new Chunk10();
        }
        column_peg_2584.chunk10->chunk_hitdef__attack__attribute = result_peg_2585;
        stream.update(result_peg_2585.getPosition());
        
        
        return result_peg_2585;
        out_peg_2588:
    
        if (column_peg_2584.chunk10 == 0){
            column_peg_2584.chunk10 = new Chunk10();
        }
        column_peg_2584.chunk10->chunk_hitdef__attack__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__attribute__item(Stream & stream, const int position, Value attribute){
    
    RuleTrace trace_peg_2633(stream, "hitdef__attack__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_2629(myposition);
        
        {
        
            result_peg_2629 = rule_hitdef__attack__type(stream, result_peg_2629.getPosition());
                if (result_peg_2629.error()){
                    goto out_peg_2631;
                }
            
            Result result_peg_2630 = result_peg_2629;
            
            result_peg_2629 = rule_hitdef__attack__movement(stream, result_peg_2629.getPosition());
                if (result_peg_2629.error()){
                    goto out_peg_2631;
                }
            
            Result result_peg_2632 = result_peg_2629;
            
            {
                    Value value((void*) 0);
                    std::string * result = toString(as<const char *>(result_peg_2630.getValues()));
                            std::string * other = toString(as<const char *>(result_peg_2632.getValues()));
                            as<Ast::HitDefAttackAttribute*>(attribute)->addAttribute(*result + *other);
                    result_peg_2629.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2629.getPosition());
        
        return result_peg_2629;
        out_peg_2631:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_hitdef__attack__type(Stream & stream, const int position){
    
    Column & column_peg_2634 = stream.getColumn(position);
    if (column_peg_2634.chunk10 != 0 && column_peg_2634.chunk10->chunk_hitdef__attack__type.calculated()){
        return column_peg_2634.chunk10->chunk_hitdef__attack__type;
    }
    
    RuleTrace trace_peg_2641(stream, "hitdef__attack__type");
    int myposition = position;
    
    
    
    Result result_peg_2635(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("n"[i], stream.get(result_peg_2635.getPosition()))){
                result_peg_2635.nextPosition();
            } else {
                goto out_peg_2636;
            }
        }
        result_peg_2635.setValue((void*) "n");
        
        if (column_peg_2634.chunk10 == 0){
            column_peg_2634.chunk10 = new Chunk10();
        }
        column_peg_2634.chunk10->chunk_hitdef__attack__type = result_peg_2635;
        stream.update(result_peg_2635.getPosition());
        
        
        return result_peg_2635;
        out_peg_2636:
        Result result_peg_2637(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("s"[i], stream.get(result_peg_2637.getPosition()))){
                result_peg_2637.nextPosition();
            } else {
                goto out_peg_2638;
            }
        }
        result_peg_2637.setValue((void*) "s");
        
        if (column_peg_2634.chunk10 == 0){
            column_peg_2634.chunk10 = new Chunk10();
        }
        column_peg_2634.chunk10->chunk_hitdef__attack__type = result_peg_2637;
        stream.update(result_peg_2637.getPosition());
        
        
        return result_peg_2637;
        out_peg_2638:
        Result result_peg_2639(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("h"[i], stream.get(result_peg_2639.getPosition()))){
                result_peg_2639.nextPosition();
            } else {
                goto out_peg_2640;
            }
        }
        result_peg_2639.setValue((void*) "h");
        
        if (column_peg_2634.chunk10 == 0){
            column_peg_2634.chunk10 = new Chunk10();
        }
        column_peg_2634.chunk10->chunk_hitdef__attack__type = result_peg_2639;
        stream.update(result_peg_2639.getPosition());
        
        
        return result_peg_2639;
        out_peg_2640:
    
        if (column_peg_2634.chunk10 == 0){
            column_peg_2634.chunk10 = new Chunk10();
        }
        column_peg_2634.chunk10->chunk_hitdef__attack__type = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attack__movement(Stream & stream, const int position){
    
    Column & column_peg_2642 = stream.getColumn(position);
    if (column_peg_2642.chunk10 != 0 && column_peg_2642.chunk10->chunk_hitdef__attack__movement.calculated()){
        return column_peg_2642.chunk10->chunk_hitdef__attack__movement;
    }
    
    RuleTrace trace_peg_2649(stream, "hitdef__attack__movement");
    int myposition = position;
    
    
    
    Result result_peg_2643(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("a"[i], stream.get(result_peg_2643.getPosition()))){
                result_peg_2643.nextPosition();
            } else {
                goto out_peg_2644;
            }
        }
        result_peg_2643.setValue((void*) "a");
        
        if (column_peg_2642.chunk10 == 0){
            column_peg_2642.chunk10 = new Chunk10();
        }
        column_peg_2642.chunk10->chunk_hitdef__attack__movement = result_peg_2643;
        stream.update(result_peg_2643.getPosition());
        
        
        return result_peg_2643;
        out_peg_2644:
        Result result_peg_2645(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("t"[i], stream.get(result_peg_2645.getPosition()))){
                result_peg_2645.nextPosition();
            } else {
                goto out_peg_2646;
            }
        }
        result_peg_2645.setValue((void*) "t");
        
        if (column_peg_2642.chunk10 == 0){
            column_peg_2642.chunk10 = new Chunk10();
        }
        column_peg_2642.chunk10->chunk_hitdef__attack__movement = result_peg_2645;
        stream.update(result_peg_2645.getPosition());
        
        
        return result_peg_2645;
        out_peg_2646:
        Result result_peg_2647(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("p"[i], stream.get(result_peg_2647.getPosition()))){
                result_peg_2647.nextPosition();
            } else {
                goto out_peg_2648;
            }
        }
        result_peg_2647.setValue((void*) "p");
        
        if (column_peg_2642.chunk10 == 0){
            column_peg_2642.chunk10 = new Chunk10();
        }
        column_peg_2642.chunk10->chunk_hitdef__attack__movement = result_peg_2647;
        stream.update(result_peg_2647.getPosition());
        
        
        return result_peg_2647;
        out_peg_2648:
    
        if (column_peg_2642.chunk10 == 0){
            column_peg_2642.chunk10 = new Chunk10();
        }
        column_peg_2642.chunk10->chunk_hitdef__attack__movement = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attribute(Stream & stream, const int position){
    
    Column & column_peg_2650 = stream.getColumn(position);
    if (column_peg_2650.chunk10 != 0 && column_peg_2650.chunk10->chunk_hitdef__attribute.calculated()){
        return column_peg_2650.chunk10->chunk_hitdef__attribute;
    }
    
    RuleTrace trace_peg_2656(stream, "hitdef__attribute");
    int myposition = position;
    
    
    
    Result result_peg_2651(myposition);
        
        {
        
            result_peg_2651.reset();
                do{
                    Result result_peg_2654(result_peg_2651.getPosition());
                    result_peg_2654 = rule_hitdef__attribute__item(stream, result_peg_2654.getPosition());
                    if (result_peg_2654.error()){
                        goto loop_peg_2653;
                    }
                    result_peg_2651.addResult(result_peg_2654);
                } while (true);
                loop_peg_2653:
                if (result_peg_2651.matches() == 0){
                    goto out_peg_2655;
                }
            
            Result result_peg_2652 = result_peg_2651;
            
            {
                    Value value((void*) 0);
                    value = makeHitDefAttribute(result_peg_2652.getValues());
                    result_peg_2651.setValue(value);
                }
            
            
        }
        
        if (column_peg_2650.chunk10 == 0){
            column_peg_2650.chunk10 = new Chunk10();
        }
        column_peg_2650.chunk10->chunk_hitdef__attribute = result_peg_2651;
        stream.update(result_peg_2651.getPosition());
        
        
        return result_peg_2651;
        out_peg_2655:
    
        if (column_peg_2650.chunk10 == 0){
            column_peg_2650.chunk10 = new Chunk10();
        }
        column_peg_2650.chunk10->chunk_hitdef__attribute = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitdef__attribute__item(Stream & stream, const int position){
    
    Column & column_peg_2657 = stream.getColumn(position);
    if (column_peg_2657.chunk11 != 0 && column_peg_2657.chunk11->chunk_hitdef__attribute__item.calculated()){
        return column_peg_2657.chunk11->chunk_hitdef__attribute__item;
    }
    
    RuleTrace trace_peg_2664(stream, "hitdef__attribute__item");
    int myposition = position;
    
    
    
    Result result_peg_2658(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("s"[i], stream.get(result_peg_2658.getPosition()))){
                result_peg_2658.nextPosition();
            } else {
                goto out_peg_2659;
            }
        }
        result_peg_2658.setValue((void*) "s");
        
        if (column_peg_2657.chunk11 == 0){
            column_peg_2657.chunk11 = new Chunk11();
        }
        column_peg_2657.chunk11->chunk_hitdef__attribute__item = result_peg_2658;
        stream.update(result_peg_2658.getPosition());
        
        
        return result_peg_2658;
        out_peg_2659:
        Result result_peg_2660(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("c"[i], stream.get(result_peg_2660.getPosition()))){
                result_peg_2660.nextPosition();
            } else {
                goto out_peg_2661;
            }
        }
        result_peg_2660.setValue((void*) "c");
        
        if (column_peg_2657.chunk11 == 0){
            column_peg_2657.chunk11 = new Chunk11();
        }
        column_peg_2657.chunk11->chunk_hitdef__attribute__item = result_peg_2660;
        stream.update(result_peg_2660.getPosition());
        
        
        return result_peg_2660;
        out_peg_2661:
        Result result_peg_2662(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareCharCase("a"[i], stream.get(result_peg_2662.getPosition()))){
                result_peg_2662.nextPosition();
            } else {
                goto out_peg_2663;
            }
        }
        result_peg_2662.setValue((void*) "a");
        
        if (column_peg_2657.chunk11 == 0){
            column_peg_2657.chunk11 = new Chunk11();
        }
        column_peg_2657.chunk11->chunk_hitdef__attribute__item = result_peg_2662;
        stream.update(result_peg_2662.getPosition());
        
        
        return result_peg_2662;
        out_peg_2663:
    
        if (column_peg_2657.chunk11 == 0){
            column_peg_2657.chunk11 = new Chunk11();
        }
        column_peg_2657.chunk11->chunk_hitdef__attribute__item = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2665 = stream.getColumn(position);
    if (column_peg_2665.chunk11 != 0 && column_peg_2665.chunk11->chunk_args.calculated()){
        return column_peg_2665.chunk11->chunk_args;
    }
    
    RuleTrace trace_peg_2707(stream, "args");
    int myposition = position;
    
    
    Value expr1;
        Value expr_rest;
    Result result_peg_2666(myposition);
        
        {
        
            result_peg_2666 = rule_expr_c(stream, result_peg_2666.getPosition());
                if (result_peg_2666.error()){
                    goto out_peg_2668;
                }
                expr1 = result_peg_2666.getValues();
            
            
            
            result_peg_2666.reset();
                do{
                    Result result_peg_2671(result_peg_2666.getPosition());
                    {
                    
                        {
                                
                                result_peg_2671.reset();
                                do{
                                    Result result_peg_2683(result_peg_2671.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2683.getPosition()))){
                                                result_peg_2683.nextPosition();
                                            } else {
                                                goto out_peg_2686;
                                            }
                                        }
                                        result_peg_2683.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2684;
                                    out_peg_2686:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2683.getPosition()))){
                                                result_peg_2683.nextPosition();
                                            } else {
                                                goto out_peg_2688;
                                            }
                                        }
                                        result_peg_2683.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2684;
                                    out_peg_2688:
                                    goto loop_peg_2682;
                                    success_peg_2684:
                                    ;
                                    result_peg_2671.addResult(result_peg_2683);
                                } while (true);
                                loop_peg_2682:
                                ;
                                        
                            }
                            goto success_peg_2673;
                            goto loop_peg_2670;
                            success_peg_2673:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2671.getPosition()))){
                                    result_peg_2671.nextPosition();
                                } else {
                                    goto loop_peg_2670;
                                }
                            }
                            result_peg_2671.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2671.reset();
                                do{
                                    Result result_peg_2701(result_peg_2671.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2701.getPosition()))){
                                                result_peg_2701.nextPosition();
                                            } else {
                                                goto out_peg_2704;
                                            }
                                        }
                                        result_peg_2701.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2702;
                                    out_peg_2704:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2701.getPosition()))){
                                                result_peg_2701.nextPosition();
                                            } else {
                                                goto out_peg_2706;
                                            }
                                        }
                                        result_peg_2701.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2702;
                                    out_peg_2706:
                                    goto loop_peg_2700;
                                    success_peg_2702:
                                    ;
                                    result_peg_2671.addResult(result_peg_2701);
                                } while (true);
                                loop_peg_2700:
                                ;
                                        
                            }
                            goto success_peg_2691;
                            goto loop_peg_2670;
                            success_peg_2691:
                            ;
                        
                        
                        
                        result_peg_2671 = rule_expr_c(stream, result_peg_2671.getPosition());
                            if (result_peg_2671.error()){
                                goto loop_peg_2670;
                            }
                        
                        
                    }
                    result_peg_2666.addResult(result_peg_2671);
                } while (true);
                loop_peg_2670:
                ;
                expr_rest = result_peg_2666.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeValueList(expr1, expr_rest);
                    result_peg_2666.setValue(value);
                }
            
            
        }
        
        if (column_peg_2665.chunk11 == 0){
            column_peg_2665.chunk11 = new Chunk11();
        }
        column_peg_2665.chunk11->chunk_args = result_peg_2666;
        stream.update(result_peg_2666.getPosition());
        
        
        return result_peg_2666;
        out_peg_2668:
    
        if (column_peg_2665.chunk11 == 0){
            column_peg_2665.chunk11 = new Chunk11();
        }
        column_peg_2665.chunk11->chunk_args = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2708 = stream.getColumn(position);
    if (column_peg_2708.chunk11 != 0 && column_peg_2708.chunk11->chunk_function_name.calculated()){
        return column_peg_2708.chunk11->chunk_function_name;
    }
    
    RuleTrace trace_peg_2773(stream, "function_name");
    int myposition = position;
    
    
    
    Result result_peg_2709(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("abs"[i], stream.get(result_peg_2709.getPosition()))){
                result_peg_2709.nextPosition();
            } else {
                goto out_peg_2710;
            }
        }
        result_peg_2709.setValue((void*) "abs");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2709;
        stream.update(result_peg_2709.getPosition());
        
        
        return result_peg_2709;
        out_peg_2710:
        Result result_peg_2711(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("const"[i], stream.get(result_peg_2711.getPosition()))){
                result_peg_2711.nextPosition();
            } else {
                goto out_peg_2712;
            }
        }
        result_peg_2711.setValue((void*) "const");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2711;
        stream.update(result_peg_2711.getPosition());
        
        
        return result_peg_2711;
        out_peg_2712:
        Result result_peg_2713(myposition);
        
        for (int i = 0; i < 13; i++){
            if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2713.getPosition()))){
                result_peg_2713.nextPosition();
            } else {
                goto out_peg_2714;
            }
        }
        result_peg_2713.setValue((void*) "selfanimexist");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2713;
        stream.update(result_peg_2713.getPosition());
        
        
        return result_peg_2713;
        out_peg_2714:
        Result result_peg_2715(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("ifelse"[i], stream.get(result_peg_2715.getPosition()))){
                result_peg_2715.nextPosition();
            } else {
                goto out_peg_2716;
            }
        }
        result_peg_2715.setValue((void*) "ifelse");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2715;
        stream.update(result_peg_2715.getPosition());
        
        
        return result_peg_2715;
        out_peg_2716:
        Result result_peg_2717(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("gethitvar"[i], stream.get(result_peg_2717.getPosition()))){
                result_peg_2717.nextPosition();
            } else {
                goto out_peg_2718;
            }
        }
        result_peg_2717.setValue((void*) "gethitvar");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2717;
        stream.update(result_peg_2717.getPosition());
        
        
        return result_peg_2717;
        out_peg_2718:
        Result result_peg_2719(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("floor"[i], stream.get(result_peg_2719.getPosition()))){
                result_peg_2719.nextPosition();
            } else {
                goto out_peg_2720;
            }
        }
        result_peg_2719.setValue((void*) "floor");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2719;
        stream.update(result_peg_2719.getPosition());
        
        
        return result_peg_2719;
        out_peg_2720:
        Result result_peg_2721(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("ceil"[i], stream.get(result_peg_2721.getPosition()))){
                result_peg_2721.nextPosition();
            } else {
                goto out_peg_2722;
            }
        }
        result_peg_2721.setValue((void*) "ceil");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2721;
        stream.update(result_peg_2721.getPosition());
        
        
        return result_peg_2721;
        out_peg_2722:
        Result result_peg_2723(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("exp"[i], stream.get(result_peg_2723.getPosition()))){
                result_peg_2723.nextPosition();
            } else {
                goto out_peg_2724;
            }
        }
        result_peg_2723.setValue((void*) "exp");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2723;
        stream.update(result_peg_2723.getPosition());
        
        
        return result_peg_2723;
        out_peg_2724:
        Result result_peg_2725(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("acos"[i], stream.get(result_peg_2725.getPosition()))){
                result_peg_2725.nextPosition();
            } else {
                goto out_peg_2726;
            }
        }
        result_peg_2725.setValue((void*) "acos");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2725;
        stream.update(result_peg_2725.getPosition());
        
        
        return result_peg_2725;
        out_peg_2726:
        Result result_peg_2727(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("asin"[i], stream.get(result_peg_2727.getPosition()))){
                result_peg_2727.nextPosition();
            } else {
                goto out_peg_2728;
            }
        }
        result_peg_2727.setValue((void*) "asin");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2727;
        stream.update(result_peg_2727.getPosition());
        
        
        return result_peg_2727;
        out_peg_2728:
        Result result_peg_2729(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("atan"[i], stream.get(result_peg_2729.getPosition()))){
                result_peg_2729.nextPosition();
            } else {
                goto out_peg_2730;
            }
        }
        result_peg_2729.setValue((void*) "atan");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2729;
        stream.update(result_peg_2729.getPosition());
        
        
        return result_peg_2729;
        out_peg_2730:
        Result result_peg_2731(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("tan"[i], stream.get(result_peg_2731.getPosition()))){
                result_peg_2731.nextPosition();
            } else {
                goto out_peg_2732;
            }
        }
        result_peg_2731.setValue((void*) "tan");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2731;
        stream.update(result_peg_2731.getPosition());
        
        
        return result_peg_2731;
        out_peg_2732:
        Result result_peg_2733(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("cos"[i], stream.get(result_peg_2733.getPosition()))){
                result_peg_2733.nextPosition();
            } else {
                goto out_peg_2734;
            }
        }
        result_peg_2733.setValue((void*) "cos");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2733;
        stream.update(result_peg_2733.getPosition());
        
        
        return result_peg_2733;
        out_peg_2734:
        Result result_peg_2735(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("sin"[i], stream.get(result_peg_2735.getPosition()))){
                result_peg_2735.nextPosition();
            } else {
                goto out_peg_2736;
            }
        }
        result_peg_2735.setValue((void*) "sin");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2735;
        stream.update(result_peg_2735.getPosition());
        
        
        return result_peg_2735;
        out_peg_2736:
        Result result_peg_2737(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("log"[i], stream.get(result_peg_2737.getPosition()))){
                result_peg_2737.nextPosition();
            } else {
                goto out_peg_2738;
            }
        }
        result_peg_2737.setValue((void*) "log");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2737;
        stream.update(result_peg_2737.getPosition());
        
        
        return result_peg_2737;
        out_peg_2738:
        Result result_peg_2739(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareCharCase("ln"[i], stream.get(result_peg_2739.getPosition()))){
                result_peg_2739.nextPosition();
            } else {
                goto out_peg_2740;
            }
        }
        result_peg_2739.setValue((void*) "ln");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2739;
        stream.update(result_peg_2739.getPosition());
        
        
        return result_peg_2739;
        out_peg_2740:
        Result result_peg_2741(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("sysfvar"[i], stream.get(result_peg_2741.getPosition()))){
                result_peg_2741.nextPosition();
            } else {
                goto out_peg_2742;
            }
        }
        result_peg_2741.setValue((void*) "sysfvar");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2741;
        stream.update(result_peg_2741.getPosition());
        
        
        return result_peg_2741;
        out_peg_2742:
        Result result_peg_2743(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("sysvar"[i], stream.get(result_peg_2743.getPosition()))){
                result_peg_2743.nextPosition();
            } else {
                goto out_peg_2744;
            }
        }
        result_peg_2743.setValue((void*) "sysvar");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2743;
        stream.update(result_peg_2743.getPosition());
        
        
        return result_peg_2743;
        out_peg_2744:
        Result result_peg_2745(myposition);
        
        for (int i = 0; i < 3; i++){
            if (compareCharCase("var"[i], stream.get(result_peg_2745.getPosition()))){
                result_peg_2745.nextPosition();
            } else {
                goto out_peg_2746;
            }
        }
        result_peg_2745.setValue((void*) "var");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2745;
        stream.update(result_peg_2745.getPosition());
        
        
        return result_peg_2745;
        out_peg_2746:
        Result result_peg_2747(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numexplod"[i], stream.get(result_peg_2747.getPosition()))){
                result_peg_2747.nextPosition();
            } else {
                goto out_peg_2748;
            }
        }
        result_peg_2747.setValue((void*) "numexplod");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2747;
        stream.update(result_peg_2747.getPosition());
        
        
        return result_peg_2747;
        out_peg_2748:
        Result result_peg_2749(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2749.getPosition()))){
                result_peg_2749.nextPosition();
            } else {
                goto out_peg_2750;
            }
        }
        result_peg_2749.setValue((void*) "numhelper");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2749;
        stream.update(result_peg_2749.getPosition());
        
        
        return result_peg_2749;
        out_peg_2750:
        Result result_peg_2751(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numprojid"[i], stream.get(result_peg_2751.getPosition()))){
                result_peg_2751.nextPosition();
            } else {
                goto out_peg_2752;
            }
        }
        result_peg_2751.setValue((void*) "numprojid");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2751;
        stream.update(result_peg_2751.getPosition());
        
        
        return result_peg_2751;
        out_peg_2752:
        Result result_peg_2753(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("fvar"[i], stream.get(result_peg_2753.getPosition()))){
                result_peg_2753.nextPosition();
            } else {
                goto out_peg_2754;
            }
        }
        result_peg_2753.setValue((void*) "fvar");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2753;
        stream.update(result_peg_2753.getPosition());
        
        
        return result_peg_2753;
        out_peg_2754:
        Result result_peg_2755(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("ishelper"[i], stream.get(result_peg_2755.getPosition()))){
                result_peg_2755.nextPosition();
            } else {
                goto out_peg_2756;
            }
        }
        result_peg_2755.setValue((void*) "ishelper");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2755;
        stream.update(result_peg_2755.getPosition());
        
        
        return result_peg_2755;
        out_peg_2756:
        Result result_peg_2757(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numtarget"[i], stream.get(result_peg_2757.getPosition()))){
                result_peg_2757.nextPosition();
            } else {
                goto out_peg_2758;
            }
        }
        result_peg_2757.setValue((void*) "numtarget");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2757;
        stream.update(result_peg_2757.getPosition());
        
        
        return result_peg_2757;
        out_peg_2758:
        Result result_peg_2759(myposition);
        
        for (int i = 0; i < 12; i++){
            if (compareCharCase("animelemtime"[i], stream.get(result_peg_2759.getPosition()))){
                result_peg_2759.nextPosition();
            } else {
                goto out_peg_2760;
            }
        }
        result_peg_2759.setValue((void*) "animelemtime");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2759;
        stream.update(result_peg_2759.getPosition());
        
        
        return result_peg_2759;
        out_peg_2760:
        Result result_peg_2761(myposition);
        
        for (int i = 0; i < 10; i++){
            if (compareCharCase("animelemno"[i], stream.get(result_peg_2761.getPosition()))){
                result_peg_2761.nextPosition();
            } else {
                goto out_peg_2762;
            }
        }
        result_peg_2761.setValue((void*) "animelemno");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2761;
        stream.update(result_peg_2761.getPosition());
        
        
        return result_peg_2761;
        out_peg_2762:
        Result result_peg_2763(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("animexist"[i], stream.get(result_peg_2763.getPosition()))){
                result_peg_2763.nextPosition();
            } else {
                goto out_peg_2764;
            }
        }
        result_peg_2763.setValue((void*) "animexist");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2763;
        stream.update(result_peg_2763.getPosition());
        
        
        return result_peg_2763;
        out_peg_2764:
        Result result_peg_2765(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projguarded"[i], stream.get(result_peg_2765.getPosition()))){
                result_peg_2765.nextPosition();
            } else {
                goto out_peg_2766;
            }
        }
        result_peg_2765.setValue((void*) "projguarded");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2765;
        stream.update(result_peg_2765.getPosition());
        
        
        return result_peg_2765;
        out_peg_2766:
        Result result_peg_2767(myposition);
        
        for (int i = 0; i < 11; i++){
            if (compareCharCase("projhittime"[i], stream.get(result_peg_2767.getPosition()))){
                result_peg_2767.nextPosition();
            } else {
                goto out_peg_2768;
            }
        }
        result_peg_2767.setValue((void*) "projhittime");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2767;
        stream.update(result_peg_2767.getPosition());
        
        
        return result_peg_2767;
        out_peg_2768:
        Result result_peg_2769(myposition);
        
        for (int i = 0; i < 15; i++){
            if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2769.getPosition()))){
                result_peg_2769.nextPosition();
            } else {
                goto out_peg_2770;
            }
        }
        result_peg_2769.setValue((void*) "projcontacttime");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2769;
        stream.update(result_peg_2769.getPosition());
        
        
        return result_peg_2769;
        out_peg_2770:
        Result result_peg_2771(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("numhelper"[i], stream.get(result_peg_2771.getPosition()))){
                result_peg_2771.nextPosition();
            } else {
                goto out_peg_2772;
            }
        }
        result_peg_2771.setValue((void*) "numhelper");
        
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = result_peg_2771;
        stream.update(result_peg_2771.getPosition());
        
        
        return result_peg_2771;
        out_peg_2772:
    
        if (column_peg_2708.chunk11 == 0){
            column_peg_2708.chunk11 = new Chunk11();
        }
        column_peg_2708.chunk11->chunk_function_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2774 = stream.getColumn(position);
    if (column_peg_2774.chunk11 != 0 && column_peg_2774.chunk11->chunk_function_rest.calculated()){
        return column_peg_2774.chunk11->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2832(stream, "function_rest");
    int myposition = position;
    
    
    
    Result result_peg_2775(myposition);
        
        {
        
            {
                    
                    result_peg_2775.reset();
                    do{
                        Result result_peg_2787(result_peg_2775.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2787.getPosition()))){
                                    result_peg_2787.nextPosition();
                                } else {
                                    goto out_peg_2790;
                                }
                            }
                            result_peg_2787.setValue((void*) " ");
                                
                        }
                        goto success_peg_2788;
                        out_peg_2790:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2787.getPosition()))){
                                    result_peg_2787.nextPosition();
                                } else {
                                    goto out_peg_2792;
                                }
                            }
                            result_peg_2787.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2788;
                        out_peg_2792:
                        goto loop_peg_2786;
                        success_peg_2788:
                        ;
                        result_peg_2775.addResult(result_peg_2787);
                    } while (true);
                    loop_peg_2786:
                    ;
                            
                }
                goto success_peg_2777;
                goto out_peg_2793;
                success_peg_2777:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_2775.getPosition()))){
                        result_peg_2775.nextPosition();
                    } else {
                        goto out_peg_2793;
                    }
                }
                result_peg_2775.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_2775.reset();
                    do{
                        Result result_peg_2806(result_peg_2775.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2806.getPosition()))){
                                    result_peg_2806.nextPosition();
                                } else {
                                    goto out_peg_2809;
                                }
                            }
                            result_peg_2806.setValue((void*) " ");
                                
                        }
                        goto success_peg_2807;
                        out_peg_2809:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2806.getPosition()))){
                                    result_peg_2806.nextPosition();
                                } else {
                                    goto out_peg_2811;
                                }
                            }
                            result_peg_2806.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2807;
                        out_peg_2811:
                        goto loop_peg_2805;
                        success_peg_2807:
                        ;
                        result_peg_2775.addResult(result_peg_2806);
                    } while (true);
                    loop_peg_2805:
                    ;
                            
                }
                goto success_peg_2796;
                goto out_peg_2793;
                success_peg_2796:
                ;
            
            
            
            int save_peg_2813 = result_peg_2775.getPosition();
                
                result_peg_2775 = rule_all_compare(stream, result_peg_2775.getPosition());
                if (result_peg_2775.error()){
                    
                    result_peg_2775 = Result(save_peg_2813);
                    result_peg_2775.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_2775.reset();
                    do{
                        Result result_peg_2825(result_peg_2775.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2825.getPosition()))){
                                    result_peg_2825.nextPosition();
                                } else {
                                    goto out_peg_2828;
                                }
                            }
                            result_peg_2825.setValue((void*) " ");
                                
                        }
                        goto success_peg_2826;
                        out_peg_2828:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2825.getPosition()))){
                                    result_peg_2825.nextPosition();
                                } else {
                                    goto out_peg_2830;
                                }
                            }
                            result_peg_2825.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2826;
                        out_peg_2830:
                        goto loop_peg_2824;
                        success_peg_2826:
                        ;
                        result_peg_2775.addResult(result_peg_2825);
                    } while (true);
                    loop_peg_2824:
                    ;
                            
                }
                goto success_peg_2815;
                goto out_peg_2793;
                success_peg_2815:
                ;
            
            
            
            result_peg_2775 = rule_value(stream, result_peg_2775.getPosition());
                if (result_peg_2775.error()){
                    goto out_peg_2793;
                }
            
            
        }
        
        if (column_peg_2774.chunk11 == 0){
            column_peg_2774.chunk11 = new Chunk11();
        }
        column_peg_2774.chunk11->chunk_function_rest = result_peg_2775;
        stream.update(result_peg_2775.getPosition());
        
        
        return result_peg_2775;
        out_peg_2793:
        Result result_peg_2831(myposition);
        
        
        
        if (column_peg_2774.chunk11 == 0){
            column_peg_2774.chunk11 = new Chunk11();
        }
        column_peg_2774.chunk11->chunk_function_rest = result_peg_2831;
        stream.update(result_peg_2831.getPosition());
        
        
        return result_peg_2831;
    
        if (column_peg_2774.chunk11 == 0){
            column_peg_2774.chunk11 = new Chunk11();
        }
        column_peg_2774.chunk11->chunk_function_rest = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2833 = stream.getColumn(position);
    if (column_peg_2833.chunk11 != 0 && column_peg_2833.chunk11->chunk_keys.calculated()){
        return column_peg_2833.chunk11->chunk_keys;
    }
    
    RuleTrace trace_peg_2836(stream, "keys");
    int myposition = position;
    
    
    
    Result result_peg_2834(myposition);
        
        result_peg_2834 = rule_key_value_list(stream, result_peg_2834.getPosition());
        if (result_peg_2834.error()){
            goto out_peg_2835;
        }
        
        if (column_peg_2833.chunk11 == 0){
            column_peg_2833.chunk11 = new Chunk11();
        }
        column_peg_2833.chunk11->chunk_keys = result_peg_2834;
        stream.update(result_peg_2834.getPosition());
        
        
        return result_peg_2834;
        out_peg_2835:
    
        if (column_peg_2833.chunk11 == 0){
            column_peg_2833.chunk11 = new Chunk11();
        }
        column_peg_2833.chunk11->chunk_keys = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2837 = stream.getColumn(position);
    if (column_peg_2837.chunk12 != 0 && column_peg_2837.chunk12->chunk_key_value_list.calculated()){
        return column_peg_2837.chunk12->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2881(stream, "key_value_list");
    int myposition = position;
    
    
    Value first;
        Value rest;
    Result result_peg_2838(myposition);
        
        {
        
            result_peg_2838 = rule_key(stream, result_peg_2838.getPosition());
                if (result_peg_2838.error()){
                    goto out_peg_2840;
                }
                first = result_peg_2838.getValues();
            
            
            
            result_peg_2838.reset();
                do{
                    Result result_peg_2843(result_peg_2838.getPosition());
                    {
                    
                        {
                                
                                result_peg_2843.reset();
                                do{
                                    Result result_peg_2855(result_peg_2843.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2855.getPosition()))){
                                                result_peg_2855.nextPosition();
                                            } else {
                                                goto out_peg_2858;
                                            }
                                        }
                                        result_peg_2855.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2856;
                                    out_peg_2858:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2855.getPosition()))){
                                                result_peg_2855.nextPosition();
                                            } else {
                                                goto out_peg_2860;
                                            }
                                        }
                                        result_peg_2855.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2856;
                                    out_peg_2860:
                                    goto loop_peg_2854;
                                    success_peg_2856:
                                    ;
                                    result_peg_2843.addResult(result_peg_2855);
                                } while (true);
                                loop_peg_2854:
                                ;
                                        
                            }
                            goto success_peg_2845;
                            goto loop_peg_2842;
                            success_peg_2845:
                            ;
                        
                        
                        
                        for (int i = 0; i < 1; i++){
                                if (compareChar(","[i], stream.get(result_peg_2843.getPosition()))){
                                    result_peg_2843.nextPosition();
                                } else {
                                    goto loop_peg_2842;
                                }
                            }
                            result_peg_2843.setValue((void*) ",");
                        
                        
                        
                        {
                                
                                result_peg_2843.reset();
                                do{
                                    Result result_peg_2873(result_peg_2843.getPosition());
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar(" "[i], stream.get(result_peg_2873.getPosition()))){
                                                result_peg_2873.nextPosition();
                                            } else {
                                                goto out_peg_2876;
                                            }
                                        }
                                        result_peg_2873.setValue((void*) " ");
                                            
                                    }
                                    goto success_peg_2874;
                                    out_peg_2876:
                                    {
                                        
                                        for (int i = 0; i < 1; i++){
                                            if (compareChar("\t"[i], stream.get(result_peg_2873.getPosition()))){
                                                result_peg_2873.nextPosition();
                                            } else {
                                                goto out_peg_2878;
                                            }
                                        }
                                        result_peg_2873.setValue((void*) "\t");
                                            
                                    }
                                    goto success_peg_2874;
                                    out_peg_2878:
                                    goto loop_peg_2872;
                                    success_peg_2874:
                                    ;
                                    result_peg_2843.addResult(result_peg_2873);
                                } while (true);
                                loop_peg_2872:
                                ;
                                        
                            }
                            goto success_peg_2863;
                            goto loop_peg_2842;
                            success_peg_2863:
                            ;
                        
                        
                        
                        result_peg_2843 = rule_key(stream, result_peg_2843.getPosition());
                            if (result_peg_2843.error()){
                                goto loop_peg_2842;
                            }
                        
                        
                    }
                    result_peg_2838.addResult(result_peg_2843);
                } while (true);
                loop_peg_2842:
                ;
                rest = result_peg_2838.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyList(first, rest);
                    result_peg_2838.setValue(value);
                }
            
            
        }
        
        if (column_peg_2837.chunk12 == 0){
            column_peg_2837.chunk12 = new Chunk12();
        }
        column_peg_2837.chunk12->chunk_key_value_list = result_peg_2838;
        stream.update(result_peg_2838.getPosition());
        
        
        return result_peg_2838;
        out_peg_2840:
        Result result_peg_2879(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = makeEmptyKeyList();
                    result_peg_2879.setValue(value);
                }
            
            
        }
        
        if (column_peg_2837.chunk12 == 0){
            column_peg_2837.chunk12 = new Chunk12();
        }
        column_peg_2837.chunk12->chunk_key_value_list = result_peg_2879;
        stream.update(result_peg_2879.getPosition());
        
        
        return result_peg_2879;
    
        if (column_peg_2837.chunk12 == 0){
            column_peg_2837.chunk12 = new Chunk12();
        }
        column_peg_2837.chunk12->chunk_key_value_list = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2882 = stream.getColumn(position);
    if (column_peg_2882.chunk12 != 0 && column_peg_2882.chunk12->chunk_key.calculated()){
        return column_peg_2882.chunk12->chunk_key;
    }
    
    RuleTrace trace_peg_2890(stream, "key");
    int myposition = position;
    
    
    Value ok;
    Result result_peg_2883(myposition);
        
        {
        
            result_peg_2883 = rule_key_real(stream, result_peg_2883.getPosition());
                if (result_peg_2883.error()){
                    goto out_peg_2885;
                }
            
            Result result_peg_2884 = result_peg_2883;
            
            result_peg_2883 = rule_key_rest(stream, result_peg_2883.getPosition(), result_peg_2884.getValues());
                if (result_peg_2883.error()){
                    goto out_peg_2885;
                }
                ok = result_peg_2883.getValues();
            
            
            
            Result result_peg_2889(result_peg_2883);
                result_peg_2889 = rule_identifier(stream, result_peg_2889.getPosition());
                if (result_peg_2889.error()){
                    goto not_peg_2888;
                }
                goto out_peg_2885;
                not_peg_2888:
                result_peg_2883.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = ok;
                    result_peg_2883.setValue(value);
                }
            
            
        }
        
        if (column_peg_2882.chunk12 == 0){
            column_peg_2882.chunk12 = new Chunk12();
        }
        column_peg_2882.chunk12->chunk_key = result_peg_2883;
        stream.update(result_peg_2883.getPosition());
        
        
        return result_peg_2883;
        out_peg_2885:
    
        if (column_peg_2882.chunk12 == 0){
            column_peg_2882.chunk12 = new Chunk12();
        }
        column_peg_2882.chunk12->chunk_key = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2934(stream, "key_rest");
    int myposition = position;
    
    tail_peg_2893:
    Value another;
        Value new_left;
    Result result_peg_2892(myposition);
        {
        
            {
                    
                    result_peg_2892.reset();
                    do{
                        Result result_peg_2905(result_peg_2892.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2905.getPosition()))){
                                    result_peg_2905.nextPosition();
                                } else {
                                    goto out_peg_2908;
                                }
                            }
                            result_peg_2905.setValue((void*) " ");
                                
                        }
                        goto success_peg_2906;
                        out_peg_2908:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2905.getPosition()))){
                                    result_peg_2905.nextPosition();
                                } else {
                                    goto out_peg_2910;
                                }
                            }
                            result_peg_2905.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2906;
                        out_peg_2910:
                        goto loop_peg_2904;
                        success_peg_2906:
                        ;
                        result_peg_2892.addResult(result_peg_2905);
                    } while (true);
                    loop_peg_2904:
                    ;
                            
                }
                goto success_peg_2895;
                goto out_peg_2911;
                success_peg_2895:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2892.getPosition()))){
                        result_peg_2892.nextPosition();
                    } else {
                        goto out_peg_2911;
                    }
                }
                result_peg_2892.setValue((void*) "+");
            
            
            
            {
                    
                    result_peg_2892.reset();
                    do{
                        Result result_peg_2924(result_peg_2892.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_2924.getPosition()))){
                                    result_peg_2924.nextPosition();
                                } else {
                                    goto out_peg_2927;
                                }
                            }
                            result_peg_2924.setValue((void*) " ");
                                
                        }
                        goto success_peg_2925;
                        out_peg_2927:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_2924.getPosition()))){
                                    result_peg_2924.nextPosition();
                                } else {
                                    goto out_peg_2929;
                                }
                            }
                            result_peg_2924.setValue((void*) "\t");
                                
                        }
                        goto success_peg_2925;
                        out_peg_2929:
                        goto loop_peg_2923;
                        success_peg_2925:
                        ;
                        result_peg_2892.addResult(result_peg_2924);
                    } while (true);
                    loop_peg_2923:
                    ;
                            
                }
                goto success_peg_2914;
                goto out_peg_2911;
                success_peg_2914:
                ;
            
            
            
            result_peg_2892 = rule_key_real(stream, result_peg_2892.getPosition());
                if (result_peg_2892.error()){
                    goto out_peg_2911;
                }
                another = result_peg_2892.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyCombined(left, another);
                    result_peg_2892.setValue(value);
                }
                new_left = result_peg_2892.getValues();
            
            
            
            left = new_left;
            
            
        }
        myposition = result_peg_2892.getPosition();
        goto tail_peg_2893;
        out_peg_2911:
        Result result_peg_2932(myposition);
        
        {
        
            
            
            
            
            {
                    Value value((void*) 0);
                    value = left;
                    result_peg_2932.setValue(value);
                }
            
            
        }
        stream.update(result_peg_2932.getPosition());
        
        return result_peg_2932;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2935 = stream.getColumn(position);
    if (column_peg_2935.chunk12 != 0 && column_peg_2935.chunk12->chunk_key_real.calculated()){
        return column_peg_2935.chunk12->chunk_key_real;
    }
    
    RuleTrace trace_peg_2942(stream, "key_real");
    int myposition = position;
    
    
    Value mods;
        Value name;
    Result result_peg_2936(myposition);
        
        {
        
            result_peg_2936.reset();
                do{
                    Result result_peg_2939(result_peg_2936.getPosition());
                    result_peg_2939 = rule_key_modifier(stream, result_peg_2939.getPosition());
                    if (result_peg_2939.error()){
                        goto loop_peg_2938;
                    }
                    result_peg_2936.addResult(result_peg_2939);
                } while (true);
                loop_peg_2938:
                ;
                mods = result_peg_2936.getValues();
            
            
            
            result_peg_2936 = rule_key_name(stream, result_peg_2936.getPosition());
                if (result_peg_2936.error()){
                    goto out_peg_2941;
                }
                name = result_peg_2936.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = applyKeyModifiers(mods, makeKey(name));
                    result_peg_2936.setValue(value);
                }
            
            
        }
        
        if (column_peg_2935.chunk12 == 0){
            column_peg_2935.chunk12 = new Chunk12();
        }
        column_peg_2935.chunk12->chunk_key_real = result_peg_2936;
        stream.update(result_peg_2936.getPosition());
        
        
        return result_peg_2936;
        out_peg_2941:
    
        if (column_peg_2935.chunk12 == 0){
            column_peg_2935.chunk12 = new Chunk12();
        }
        column_peg_2935.chunk12->chunk_key_real = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2943 = stream.getColumn(position);
    if (column_peg_2943.chunk12 != 0 && column_peg_2943.chunk12->chunk_key_modifier.calculated()){
        return column_peg_2943.chunk12->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2964(stream, "key_modifier");
    int myposition = position;
    
    
    Value num;
    Result result_peg_2944(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("~"[i], stream.get(result_peg_2944.getPosition()))){
                        result_peg_2944.nextPosition();
                    } else {
                        goto out_peg_2946;
                    }
                }
                result_peg_2944.setValue((void*) "~");
            
            
            
            result_peg_2944.reset();
                do{
                    Result result_peg_2949(result_peg_2944.getPosition());
                    {
                        
                        char letter_peg_2954 = stream.get(result_peg_2949.getPosition());
                        if (letter_peg_2954 != '\0' && strchr("0123456789", letter_peg_2954) != NULL){
                            result_peg_2949.nextPosition();
                            result_peg_2949.setValue((void*) (long) letter_peg_2954);
                        } else {
                            goto out_peg_2953;
                        }
                        
                    }
                    goto success_peg_2950;
                    out_peg_2953:
                    goto loop_peg_2948;
                    success_peg_2950:
                    ;
                    result_peg_2944.addResult(result_peg_2949);
                } while (true);
                loop_peg_2948:
                ;
                num = result_peg_2944.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = new ReleaseKeyModifier((int) *parseDouble(num));
                    result_peg_2944.setValue(value);
                }
            
            
        }
        
        if (column_peg_2943.chunk12 == 0){
            column_peg_2943.chunk12 = new Chunk12();
        }
        column_peg_2943.chunk12->chunk_key_modifier = result_peg_2944;
        stream.update(result_peg_2944.getPosition());
        
        
        return result_peg_2944;
        out_peg_2946:
        Result result_peg_2955(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("$"[i], stream.get(result_peg_2955.getPosition()))){
                        result_peg_2955.nextPosition();
                    } else {
                        goto out_peg_2957;
                    }
                }
                result_peg_2955.setValue((void*) "$");
            
            
            
            {
                    Value value((void*) 0);
                    value = new DirectionKeyModifier();
                    result_peg_2955.setValue(value);
                }
            
            
        }
        
        if (column_peg_2943.chunk12 == 0){
            column_peg_2943.chunk12 = new Chunk12();
        }
        column_peg_2943.chunk12->chunk_key_modifier = result_peg_2955;
        stream.update(result_peg_2955.getPosition());
        
        
        return result_peg_2955;
        out_peg_2957:
        Result result_peg_2958(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("/"[i], stream.get(result_peg_2958.getPosition()))){
                        result_peg_2958.nextPosition();
                    } else {
                        goto out_peg_2960;
                    }
                }
                result_peg_2958.setValue((void*) "/");
            
            
            
            {
                    Value value((void*) 0);
                    value = new HeldDownKeyModifier();
                    result_peg_2958.setValue(value);
                }
            
            
        }
        
        if (column_peg_2943.chunk12 == 0){
            column_peg_2943.chunk12 = new Chunk12();
        }
        column_peg_2943.chunk12->chunk_key_modifier = result_peg_2958;
        stream.update(result_peg_2958.getPosition());
        
        
        return result_peg_2958;
        out_peg_2960:
        Result result_peg_2961(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar(">"[i], stream.get(result_peg_2961.getPosition()))){
                        result_peg_2961.nextPosition();
                    } else {
                        goto out_peg_2963;
                    }
                }
                result_peg_2961.setValue((void*) ">");
            
            
            
            {
                    Value value((void*) 0);
                    value = new OnlyKeyModifier();
                    result_peg_2961.setValue(value);
                }
            
            
        }
        
        if (column_peg_2943.chunk12 == 0){
            column_peg_2943.chunk12 = new Chunk12();
        }
        column_peg_2943.chunk12->chunk_key_modifier = result_peg_2961;
        stream.update(result_peg_2961.getPosition());
        
        
        return result_peg_2961;
        out_peg_2963:
    
        if (column_peg_2943.chunk12 == 0){
            column_peg_2943.chunk12 = new Chunk12();
        }
        column_peg_2943.chunk12->chunk_key_modifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2965 = stream.getColumn(position);
    if (column_peg_2965.chunk13 != 0 && column_peg_2965.chunk13->chunk_key_name.calculated()){
        return column_peg_2965.chunk13->chunk_key_name;
    }
    
    RuleTrace trace_peg_2996(stream, "key_name");
    int myposition = position;
    
    
    
    Result result_peg_2966(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DB"[i], stream.get(result_peg_2966.getPosition()))){
                result_peg_2966.nextPosition();
            } else {
                goto out_peg_2967;
            }
        }
        result_peg_2966.setValue((void*) "DB");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2966;
        stream.update(result_peg_2966.getPosition());
        
        
        return result_peg_2966;
        out_peg_2967:
        Result result_peg_2968(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("B"[i], stream.get(result_peg_2968.getPosition()))){
                result_peg_2968.nextPosition();
            } else {
                goto out_peg_2969;
            }
        }
        result_peg_2968.setValue((void*) "B");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2968;
        stream.update(result_peg_2968.getPosition());
        
        
        return result_peg_2968;
        out_peg_2969:
        Result result_peg_2970(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("DF"[i], stream.get(result_peg_2970.getPosition()))){
                result_peg_2970.nextPosition();
            } else {
                goto out_peg_2971;
            }
        }
        result_peg_2970.setValue((void*) "DF");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2970;
        stream.update(result_peg_2970.getPosition());
        
        
        return result_peg_2970;
        out_peg_2971:
        Result result_peg_2972(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("D"[i], stream.get(result_peg_2972.getPosition()))){
                result_peg_2972.nextPosition();
            } else {
                goto out_peg_2973;
            }
        }
        result_peg_2972.setValue((void*) "D");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2972;
        stream.update(result_peg_2972.getPosition());
        
        
        return result_peg_2972;
        out_peg_2973:
        Result result_peg_2974(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("F"[i], stream.get(result_peg_2974.getPosition()))){
                result_peg_2974.nextPosition();
            } else {
                goto out_peg_2975;
            }
        }
        result_peg_2974.setValue((void*) "F");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2974;
        stream.update(result_peg_2974.getPosition());
        
        
        return result_peg_2974;
        out_peg_2975:
        Result result_peg_2976(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UF"[i], stream.get(result_peg_2976.getPosition()))){
                result_peg_2976.nextPosition();
            } else {
                goto out_peg_2977;
            }
        }
        result_peg_2976.setValue((void*) "UF");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2976;
        stream.update(result_peg_2976.getPosition());
        
        
        return result_peg_2976;
        out_peg_2977:
        Result result_peg_2978(myposition);
        
        for (int i = 0; i < 2; i++){
            if (compareChar("UB"[i], stream.get(result_peg_2978.getPosition()))){
                result_peg_2978.nextPosition();
            } else {
                goto out_peg_2979;
            }
        }
        result_peg_2978.setValue((void*) "UB");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2978;
        stream.update(result_peg_2978.getPosition());
        
        
        return result_peg_2978;
        out_peg_2979:
        Result result_peg_2980(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("U"[i], stream.get(result_peg_2980.getPosition()))){
                result_peg_2980.nextPosition();
            } else {
                goto out_peg_2981;
            }
        }
        result_peg_2980.setValue((void*) "U");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2980;
        stream.update(result_peg_2980.getPosition());
        
        
        return result_peg_2980;
        out_peg_2981:
        Result result_peg_2982(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("a"[i], stream.get(result_peg_2982.getPosition()))){
                result_peg_2982.nextPosition();
            } else {
                goto out_peg_2983;
            }
        }
        result_peg_2982.setValue((void*) "a");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2982;
        stream.update(result_peg_2982.getPosition());
        
        
        return result_peg_2982;
        out_peg_2983:
        Result result_peg_2984(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("b"[i], stream.get(result_peg_2984.getPosition()))){
                result_peg_2984.nextPosition();
            } else {
                goto out_peg_2985;
            }
        }
        result_peg_2984.setValue((void*) "b");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2984;
        stream.update(result_peg_2984.getPosition());
        
        
        return result_peg_2984;
        out_peg_2985:
        Result result_peg_2986(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("c"[i], stream.get(result_peg_2986.getPosition()))){
                result_peg_2986.nextPosition();
            } else {
                goto out_peg_2987;
            }
        }
        result_peg_2986.setValue((void*) "c");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2986;
        stream.update(result_peg_2986.getPosition());
        
        
        return result_peg_2986;
        out_peg_2987:
        Result result_peg_2988(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("x"[i], stream.get(result_peg_2988.getPosition()))){
                result_peg_2988.nextPosition();
            } else {
                goto out_peg_2989;
            }
        }
        result_peg_2988.setValue((void*) "x");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2988;
        stream.update(result_peg_2988.getPosition());
        
        
        return result_peg_2988;
        out_peg_2989:
        Result result_peg_2990(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("y"[i], stream.get(result_peg_2990.getPosition()))){
                result_peg_2990.nextPosition();
            } else {
                goto out_peg_2991;
            }
        }
        result_peg_2990.setValue((void*) "y");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2990;
        stream.update(result_peg_2990.getPosition());
        
        
        return result_peg_2990;
        out_peg_2991:
        Result result_peg_2992(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("z"[i], stream.get(result_peg_2992.getPosition()))){
                result_peg_2992.nextPosition();
            } else {
                goto out_peg_2993;
            }
        }
        result_peg_2992.setValue((void*) "z");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2992;
        stream.update(result_peg_2992.getPosition());
        
        
        return result_peg_2992;
        out_peg_2993:
        Result result_peg_2994(myposition);
        
        for (int i = 0; i < 1; i++){
            if (compareChar("s"[i], stream.get(result_peg_2994.getPosition()))){
                result_peg_2994.nextPosition();
            } else {
                goto out_peg_2995;
            }
        }
        result_peg_2994.setValue((void*) "s");
        
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = result_peg_2994;
        stream.update(result_peg_2994.getPosition());
        
        
        return result_peg_2994;
        out_peg_2995:
    
        if (column_peg_2965.chunk13 == 0){
            column_peg_2965.chunk13 = new Chunk13();
        }
        column_peg_2965.chunk13->chunk_key_name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2997 = stream.getColumn(position);
    if (column_peg_2997.chunk13 != 0 && column_peg_2997.chunk13->chunk_value.calculated()){
        return column_peg_2997.chunk13->chunk_value;
    }
    
    RuleTrace trace_peg_3015(stream, "value");
    int myposition = position;
    
    
    
    Result result_peg_2998(myposition);
        
        result_peg_2998 = rule_float(stream, result_peg_2998.getPosition());
        if (result_peg_2998.error()){
            goto out_peg_2999;
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_2998;
        stream.update(result_peg_2998.getPosition());
        
        
        return result_peg_2998;
        out_peg_2999:
        Result result_peg_3000(myposition);
        
        result_peg_3000 = rule_integer(stream, result_peg_3000.getPosition());
        if (result_peg_3000.error()){
            goto out_peg_3001;
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_3000;
        stream.update(result_peg_3000.getPosition());
        
        
        return result_peg_3000;
        out_peg_3001:
        Result result_peg_3002(myposition);
        
        result_peg_3002 = rule_keyword(stream, result_peg_3002.getPosition());
        if (result_peg_3002.error()){
            goto out_peg_3003;
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_3002;
        stream.update(result_peg_3002.getPosition());
        
        
        return result_peg_3002;
        out_peg_3003:
        Result result_peg_3004(myposition);
        
        {
        
            Result result_peg_3007(result_peg_3004);
                result_peg_3007 = rule_keyword(stream, result_peg_3007.getPosition());
                if (result_peg_3007.error()){
                    goto not_peg_3006;
                }
                goto out_peg_3008;
                not_peg_3006:
                result_peg_3004.setValue((void*)0);
            
            
            
            result_peg_3004 = rule_identifier(stream, result_peg_3004.getPosition());
                if (result_peg_3004.error()){
                    goto out_peg_3008;
                }
            
            
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_3004;
        stream.update(result_peg_3004.getPosition());
        
        
        return result_peg_3004;
        out_peg_3008:
        Result result_peg_3009(myposition);
        
        result_peg_3009 = rule_range(stream, result_peg_3009.getPosition());
        if (result_peg_3009.error()){
            goto out_peg_3010;
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_3009;
        stream.update(result_peg_3009.getPosition());
        
        
        return result_peg_3009;
        out_peg_3010:
        Result result_peg_3011(myposition);
        
        result_peg_3011 = rule_string(stream, result_peg_3011.getPosition());
        if (result_peg_3011.error()){
            goto out_peg_3012;
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_3011;
        stream.update(result_peg_3011.getPosition());
        
        
        return result_peg_3011;
        out_peg_3012:
        Result result_peg_3013(myposition);
        
        result_peg_3013 = rule_hitflag(stream, result_peg_3013.getPosition());
        if (result_peg_3013.error()){
            goto out_peg_3014;
        }
        
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = result_peg_3013;
        stream.update(result_peg_3013.getPosition());
        
        
        return result_peg_3013;
        out_peg_3014:
    
        if (column_peg_2997.chunk13 == 0){
            column_peg_2997.chunk13 = new Chunk13();
        }
        column_peg_2997.chunk13->chunk_value = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_3016 = stream.getColumn(position);
    if (column_peg_3016.chunk13 != 0 && column_peg_3016.chunk13->chunk_helper.calculated()){
        return column_peg_3016.chunk13->chunk_helper;
    }
    
    RuleTrace trace_peg_3075(stream, "helper");
    int myposition = position;
    
    
    Value name;
        Value id;
    Result result_peg_3017(myposition);
        
        {
        
            result_peg_3017 = rule_helper__name(stream, result_peg_3017.getPosition());
                if (result_peg_3017.error()){
                    goto out_peg_3019;
                }
                name = result_peg_3017.getValues();
            
            
            
            {
                    
                    result_peg_3017.reset();
                    do{
                        Result result_peg_3031(result_peg_3017.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3031.getPosition()))){
                                    result_peg_3031.nextPosition();
                                } else {
                                    goto out_peg_3034;
                                }
                            }
                            result_peg_3031.setValue((void*) " ");
                                
                        }
                        goto success_peg_3032;
                        out_peg_3034:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3031.getPosition()))){
                                    result_peg_3031.nextPosition();
                                } else {
                                    goto out_peg_3036;
                                }
                            }
                            result_peg_3031.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3032;
                        out_peg_3036:
                        goto loop_peg_3030;
                        success_peg_3032:
                        ;
                        result_peg_3017.addResult(result_peg_3031);
                    } while (true);
                    loop_peg_3030:
                    ;
                            
                }
                goto success_peg_3021;
                goto out_peg_3019;
                success_peg_3021:
                ;
            
            
            
            int save_peg_3038 = result_peg_3017.getPosition();
                
                result_peg_3017 = rule_helper__expression(stream, result_peg_3017.getPosition());
                if (result_peg_3017.error()){
                    
                    result_peg_3017 = Result(save_peg_3038);
                    result_peg_3017.setValue((void*) 0);
                    
                }
            
            
            
            {
                    
                    result_peg_3017.reset();
                    do{
                        Result result_peg_3050(result_peg_3017.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3050.getPosition()))){
                                    result_peg_3050.nextPosition();
                                } else {
                                    goto out_peg_3053;
                                }
                            }
                            result_peg_3050.setValue((void*) " ");
                                
                        }
                        goto success_peg_3051;
                        out_peg_3053:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3050.getPosition()))){
                                    result_peg_3050.nextPosition();
                                } else {
                                    goto out_peg_3055;
                                }
                            }
                            result_peg_3050.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3051;
                        out_peg_3055:
                        goto loop_peg_3049;
                        success_peg_3051:
                        ;
                        result_peg_3017.addResult(result_peg_3050);
                    } while (true);
                    loop_peg_3049:
                    ;
                            
                }
                goto success_peg_3040;
                goto out_peg_3019;
                success_peg_3040:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(","[i], stream.get(result_peg_3017.getPosition()))){
                        result_peg_3017.nextPosition();
                    } else {
                        goto out_peg_3019;
                    }
                }
                result_peg_3017.setValue((void*) ",");
            
            
            
            {
                    
                    result_peg_3017.reset();
                    do{
                        Result result_peg_3068(result_peg_3017.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3068.getPosition()))){
                                    result_peg_3068.nextPosition();
                                } else {
                                    goto out_peg_3071;
                                }
                            }
                            result_peg_3068.setValue((void*) " ");
                                
                        }
                        goto success_peg_3069;
                        out_peg_3071:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3068.getPosition()))){
                                    result_peg_3068.nextPosition();
                                } else {
                                    goto out_peg_3073;
                                }
                            }
                            result_peg_3068.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3069;
                        out_peg_3073:
                        goto loop_peg_3067;
                        success_peg_3069:
                        ;
                        result_peg_3017.addResult(result_peg_3068);
                    } while (true);
                    loop_peg_3067:
                    ;
                            
                }
                goto success_peg_3058;
                goto out_peg_3019;
                success_peg_3058:
                ;
            
            
            
            result_peg_3017 = rule_helper__identifier(stream, result_peg_3017.getPosition());
                if (result_peg_3017.error()){
                    goto out_peg_3019;
                }
                id = result_peg_3017.getValues();
            
            
            
            {
                    Value value((void*) 0);
                    value = makeHelper(name, id);
                    result_peg_3017.setValue(value);
                }
            
            
        }
        
        if (column_peg_3016.chunk13 == 0){
            column_peg_3016.chunk13 = new Chunk13();
        }
        column_peg_3016.chunk13->chunk_helper = result_peg_3017;
        stream.update(result_peg_3017.getPosition());
        
        
        return result_peg_3017;
        out_peg_3019:
    
        if (column_peg_3016.chunk13 == 0){
            column_peg_3016.chunk13 = new Chunk13();
        }
        column_peg_3016.chunk13->chunk_helper = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_3076 = stream.getColumn(position);
    if (column_peg_3076.chunk13 != 0 && column_peg_3076.chunk13->chunk_helper__expression.calculated()){
        return column_peg_3076.chunk13->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_3115(stream, "helper__expression");
    int myposition = position;
    
    
    
    Result result_peg_3077(myposition);
        
        {
        
            for (int i = 0; i < 1; i++){
                    if (compareChar("("[i], stream.get(result_peg_3077.getPosition()))){
                        result_peg_3077.nextPosition();
                    } else {
                        goto out_peg_3079;
                    }
                }
                result_peg_3077.setValue((void*) "(");
            
            
            
            {
                    
                    result_peg_3077.reset();
                    do{
                        Result result_peg_3091(result_peg_3077.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3091.getPosition()))){
                                    result_peg_3091.nextPosition();
                                } else {
                                    goto out_peg_3094;
                                }
                            }
                            result_peg_3091.setValue((void*) " ");
                                
                        }
                        goto success_peg_3092;
                        out_peg_3094:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3091.getPosition()))){
                                    result_peg_3091.nextPosition();
                                } else {
                                    goto out_peg_3096;
                                }
                            }
                            result_peg_3091.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3092;
                        out_peg_3096:
                        goto loop_peg_3090;
                        success_peg_3092:
                        ;
                        result_peg_3077.addResult(result_peg_3091);
                    } while (true);
                    loop_peg_3090:
                    ;
                            
                }
                goto success_peg_3081;
                goto out_peg_3079;
                success_peg_3081:
                ;
            
            
            
            result_peg_3077 = rule_expr_c(stream, result_peg_3077.getPosition());
                if (result_peg_3077.error()){
                    goto out_peg_3079;
                }
            
            
            
            {
                    
                    result_peg_3077.reset();
                    do{
                        Result result_peg_3109(result_peg_3077.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3109.getPosition()))){
                                    result_peg_3109.nextPosition();
                                } else {
                                    goto out_peg_3112;
                                }
                            }
                            result_peg_3109.setValue((void*) " ");
                                
                        }
                        goto success_peg_3110;
                        out_peg_3112:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3109.getPosition()))){
                                    result_peg_3109.nextPosition();
                                } else {
                                    goto out_peg_3114;
                                }
                            }
                            result_peg_3109.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3110;
                        out_peg_3114:
                        goto loop_peg_3108;
                        success_peg_3110:
                        ;
                        result_peg_3077.addResult(result_peg_3109);
                    } while (true);
                    loop_peg_3108:
                    ;
                            
                }
                goto success_peg_3099;
                goto out_peg_3079;
                success_peg_3099:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareChar(")"[i], stream.get(result_peg_3077.getPosition()))){
                        result_peg_3077.nextPosition();
                    } else {
                        goto out_peg_3079;
                    }
                }
                result_peg_3077.setValue((void*) ")");
            
            
        }
        
        if (column_peg_3076.chunk13 == 0){
            column_peg_3076.chunk13 = new Chunk13();
        }
        column_peg_3076.chunk13->chunk_helper__expression = result_peg_3077;
        stream.update(result_peg_3077.getPosition());
        
        
        return result_peg_3077;
        out_peg_3079:
    
        if (column_peg_3076.chunk13 == 0){
            column_peg_3076.chunk13 = new Chunk13();
        }
        column_peg_3076.chunk13->chunk_helper__expression = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_3116 = stream.getColumn(position);
    if (column_peg_3116.chunk13 != 0 && column_peg_3116.chunk13->chunk_helper__name.calculated()){
        return column_peg_3116.chunk13->chunk_helper__name;
    }
    
    RuleTrace trace_peg_3133(stream, "helper__name");
    int myposition = position;
    
    
    
    Result result_peg_3117(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("parent"[i], stream.get(result_peg_3117.getPosition()))){
                result_peg_3117.nextPosition();
            } else {
                goto out_peg_3118;
            }
        }
        result_peg_3117.setValue((void*) "parent");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3117;
        stream.update(result_peg_3117.getPosition());
        
        
        return result_peg_3117;
        out_peg_3118:
        Result result_peg_3119(myposition);
        
        for (int i = 0; i < 4; i++){
            if (compareCharCase("root"[i], stream.get(result_peg_3119.getPosition()))){
                result_peg_3119.nextPosition();
            } else {
                goto out_peg_3120;
            }
        }
        result_peg_3119.setValue((void*) "root");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3119;
        stream.update(result_peg_3119.getPosition());
        
        
        return result_peg_3119;
        out_peg_3120:
        Result result_peg_3121(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("helper"[i], stream.get(result_peg_3121.getPosition()))){
                result_peg_3121.nextPosition();
            } else {
                goto out_peg_3122;
            }
        }
        result_peg_3121.setValue((void*) "helper");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3121;
        stream.update(result_peg_3121.getPosition());
        
        
        return result_peg_3121;
        out_peg_3122:
        Result result_peg_3123(myposition);
        
        for (int i = 0; i < 6; i++){
            if (compareCharCase("target"[i], stream.get(result_peg_3123.getPosition()))){
                result_peg_3123.nextPosition();
            } else {
                goto out_peg_3124;
            }
        }
        result_peg_3123.setValue((void*) "target");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3123;
        stream.update(result_peg_3123.getPosition());
        
        
        return result_peg_3123;
        out_peg_3124:
        Result result_peg_3125(myposition);
        
        for (int i = 0; i < 7; i++){
            if (compareCharCase("partner"[i], stream.get(result_peg_3125.getPosition()))){
                result_peg_3125.nextPosition();
            } else {
                goto out_peg_3126;
            }
        }
        result_peg_3125.setValue((void*) "partner");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3125;
        stream.update(result_peg_3125.getPosition());
        
        
        return result_peg_3125;
        out_peg_3126:
        Result result_peg_3127(myposition);
        
        for (int i = 0; i < 9; i++){
            if (compareCharCase("enemyNear"[i], stream.get(result_peg_3127.getPosition()))){
                result_peg_3127.nextPosition();
            } else {
                goto out_peg_3128;
            }
        }
        result_peg_3127.setValue((void*) "enemyNear");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3127;
        stream.update(result_peg_3127.getPosition());
        
        
        return result_peg_3127;
        out_peg_3128:
        Result result_peg_3129(myposition);
        
        for (int i = 0; i < 5; i++){
            if (compareCharCase("enemy"[i], stream.get(result_peg_3129.getPosition()))){
                result_peg_3129.nextPosition();
            } else {
                goto out_peg_3130;
            }
        }
        result_peg_3129.setValue((void*) "enemy");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3129;
        stream.update(result_peg_3129.getPosition());
        
        
        return result_peg_3129;
        out_peg_3130:
        Result result_peg_3131(myposition);
        
        for (int i = 0; i < 8; i++){
            if (compareCharCase("playerid"[i], stream.get(result_peg_3131.getPosition()))){
                result_peg_3131.nextPosition();
            } else {
                goto out_peg_3132;
            }
        }
        result_peg_3131.setValue((void*) "playerid");
        
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = result_peg_3131;
        stream.update(result_peg_3131.getPosition());
        
        
        return result_peg_3131;
        out_peg_3132:
    
        if (column_peg_3116.chunk13 == 0){
            column_peg_3116.chunk13 = new Chunk13();
        }
        column_peg_3116.chunk13->chunk_helper__name = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_3134 = stream.getColumn(position);
    if (column_peg_3134.chunk14 != 0 && column_peg_3134.chunk14->chunk_helper__identifier.calculated()){
        return column_peg_3134.chunk14->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_3144(stream, "helper__identifier");
    int myposition = position;
    
    
    
    Result result_peg_3135(myposition);
        
        result_peg_3135 = rule_function(stream, result_peg_3135.getPosition());
        if (result_peg_3135.error()){
            goto out_peg_3136;
        }
        
        if (column_peg_3134.chunk14 == 0){
            column_peg_3134.chunk14 = new Chunk14();
        }
        column_peg_3134.chunk14->chunk_helper__identifier = result_peg_3135;
        stream.update(result_peg_3135.getPosition());
        
        
        return result_peg_3135;
        out_peg_3136:
        Result result_peg_3137(myposition);
        
        result_peg_3137 = rule_keyword(stream, result_peg_3137.getPosition());
        if (result_peg_3137.error()){
            goto out_peg_3138;
        }
        
        if (column_peg_3134.chunk14 == 0){
            column_peg_3134.chunk14 = new Chunk14();
        }
        column_peg_3134.chunk14->chunk_helper__identifier = result_peg_3137;
        stream.update(result_peg_3137.getPosition());
        
        
        return result_peg_3137;
        out_peg_3138:
        Result result_peg_3139(myposition);
        
        {
        
            Result result_peg_3142(result_peg_3139);
                result_peg_3142 = rule_keyword(stream, result_peg_3142.getPosition());
                if (result_peg_3142.error()){
                    goto not_peg_3141;
                }
                goto out_peg_3143;
                not_peg_3141:
                result_peg_3139.setValue((void*)0);
            
            
            
            result_peg_3139 = rule_identifier(stream, result_peg_3139.getPosition());
                if (result_peg_3139.error()){
                    goto out_peg_3143;
                }
            
            
        }
        
        if (column_peg_3134.chunk14 == 0){
            column_peg_3134.chunk14 = new Chunk14();
        }
        column_peg_3134.chunk14->chunk_helper__identifier = result_peg_3139;
        stream.update(result_peg_3139.getPosition());
        
        
        return result_peg_3139;
        out_peg_3143:
    
        if (column_peg_3134.chunk14 == 0){
            column_peg_3134.chunk14 = new Chunk14();
        }
        column_peg_3134.chunk14->chunk_helper__identifier = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_3145 = stream.getColumn(position);
    if (column_peg_3145.chunk14 != 0 && column_peg_3145.chunk14->chunk_hitflag.calculated()){
        return column_peg_3145.chunk14->chunk_hitflag;
    }
    
    RuleTrace trace_peg_3170(stream, "hitflag");
    int myposition = position;
    
    
    
    Result result_peg_3146(myposition);
        
        {
        
            result_peg_3146.reset();
                do{
                    Result result_peg_3149(result_peg_3146.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("A"[i], stream.get(result_peg_3149.getPosition()))){
                                result_peg_3149.nextPosition();
                            } else {
                                goto out_peg_3152;
                            }
                        }
                        result_peg_3149.setValue((void*) "A");
                            
                    }
                    goto success_peg_3150;
                    out_peg_3152:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("M"[i], stream.get(result_peg_3149.getPosition()))){
                                result_peg_3149.nextPosition();
                            } else {
                                goto out_peg_3154;
                            }
                        }
                        result_peg_3149.setValue((void*) "M");
                            
                    }
                    goto success_peg_3150;
                    out_peg_3154:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("F"[i], stream.get(result_peg_3149.getPosition()))){
                                result_peg_3149.nextPosition();
                            } else {
                                goto out_peg_3156;
                            }
                        }
                        result_peg_3149.setValue((void*) "F");
                            
                    }
                    goto success_peg_3150;
                    out_peg_3156:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("D"[i], stream.get(result_peg_3149.getPosition()))){
                                result_peg_3149.nextPosition();
                            } else {
                                goto out_peg_3158;
                            }
                        }
                        result_peg_3149.setValue((void*) "D");
                            
                    }
                    goto success_peg_3150;
                    out_peg_3158:
                    goto loop_peg_3148;
                    success_peg_3150:
                    ;
                    result_peg_3146.addResult(result_peg_3149);
                } while (true);
                loop_peg_3148:
                if (result_peg_3146.matches() == 0){
                    goto out_peg_3159;
                }
            
            
            
            int save_peg_3161 = result_peg_3146.getPosition();
                
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("+"[i], stream.get(result_peg_3146.getPosition()))){
                            result_peg_3146.nextPosition();
                        } else {
                            goto out_peg_3164;
                        }
                    }
                    result_peg_3146.setValue((void*) "+");
                        
                }
                goto success_peg_3162;
                out_peg_3164:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("-"[i], stream.get(result_peg_3146.getPosition()))){
                            result_peg_3146.nextPosition();
                        } else {
                            goto out_peg_3166;
                        }
                    }
                    result_peg_3146.setValue((void*) "-");
                        
                }
                goto success_peg_3162;
                out_peg_3166:
                
                result_peg_3146 = Result(save_peg_3161);
                result_peg_3146.setValue((void*) 0);
                
                success_peg_3162:
                ;
            
            
            
            Result result_peg_3169(result_peg_3146);
                for (int i = 0; i < 1; i++){
                    if (compareChar("."[i], stream.get(result_peg_3169.getPosition()))){
                        result_peg_3169.nextPosition();
                    } else {
                        goto not_peg_3168;
                    }
                }
                result_peg_3169.setValue((void*) ".");
                goto out_peg_3159;
                not_peg_3168:
                result_peg_3146.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = makeSimpleIdentifier("A");
                    result_peg_3146.setValue(value);
                }
            
            
        }
        
        if (column_peg_3145.chunk14 == 0){
            column_peg_3145.chunk14 = new Chunk14();
        }
        column_peg_3145.chunk14->chunk_hitflag = result_peg_3146;
        stream.update(result_peg_3146.getPosition());
        
        
        return result_peg_3146;
        out_peg_3159:
    
        if (column_peg_3145.chunk14 == 0){
            column_peg_3145.chunk14 = new Chunk14();
        }
        column_peg_3145.chunk14->chunk_hitflag = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_3171 = stream.getColumn(position);
    if (column_peg_3171.chunk14 != 0 && column_peg_3171.chunk14->chunk_keyword.calculated()){
        return column_peg_3171.chunk14->chunk_keyword;
    }
    
    RuleTrace trace_peg_3178(stream, "keyword");
    int myposition = position;
    
    
    
    Result result_peg_3172(myposition);
        
        {
        
            result_peg_3172 = rule_keyword_real(stream, result_peg_3172.getPosition());
                if (result_peg_3172.error()){
                    goto out_peg_3174;
                }
            
            Result result_peg_3173 = result_peg_3172;
            
            Result result_peg_3177(result_peg_3172);
                result_peg_3177 = rule_alpha_digit(stream, result_peg_3177.getPosition());
                if (result_peg_3177.error()){
                    goto not_peg_3176;
                }
                goto out_peg_3174;
                not_peg_3176:
                result_peg_3172.setValue((void*)0);
            
            
            
            {
                    Value value((void*) 0);
                    value = result_peg_3173.getValues();
                    result_peg_3172.setValue(value);
                }
            
            
        }
        
        if (column_peg_3171.chunk14 == 0){
            column_peg_3171.chunk14 = new Chunk14();
        }
        column_peg_3171.chunk14->chunk_keyword = result_peg_3172;
        stream.update(result_peg_3172.getPosition());
        
        
        return result_peg_3172;
        out_peg_3174:
    
        if (column_peg_3171.chunk14 == 0){
            column_peg_3171.chunk14 = new Chunk14();
        }
        column_peg_3171.chunk14->chunk_keyword = errorResult;
        stream.update(errorResult.getPosition());
        
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_3179 = stream.getColumn(position);
    if (column_peg_3179.chunk14 != 0 && column_peg_3179.chunk14->chunk_keyword_real.calculated()){
        return column_peg_3179.chunk14->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3600(stream, "keyword_real");
    int myposition = position;
    
    
    
    Result result_peg_3180(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_3180.getPosition()))){
                        result_peg_3180.nextPosition();
                    } else {
                        goto out_peg_3182;
                    }
                }
                result_peg_3180.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_3180.reset();
                    do{
                        Result result_peg_3195(result_peg_3180.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3195.getPosition()))){
                                    result_peg_3195.nextPosition();
                                } else {
                                    goto out_peg_3198;
                                }
                            }
                            result_peg_3195.setValue((void*) " ");
                                
                        }
                        goto success_peg_3196;
                        out_peg_3198:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3195.getPosition()))){
                                    result_peg_3195.nextPosition();
                                } else {
                                    goto out_peg_3200;
                                }
                            }
                            result_peg_3195.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3196;
                        out_peg_3200:
                        goto loop_peg_3194;
                        success_peg_3196:
                        ;
                        result_peg_3180.addResult(result_peg_3195);
                    } while (true);
                    loop_peg_3194:
                    if (result_peg_3180.matches() == 0){
                        goto out_peg_3193;
                    }
                    
                }
                goto success_peg_3184;
                out_peg_3193:
                goto out_peg_3182;
                success_peg_3184:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3180.getPosition()))){
                        result_peg_3180.nextPosition();
                    } else {
                        goto out_peg_3182;
                    }
                }
                result_peg_3180.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel y");
                    result_peg_3180.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3180;
        stream.update(result_peg_3180.getPosition());
        
        
        return result_peg_3180;
        out_peg_3182:
        Result result_peg_3202(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("vel"[i], stream.get(result_peg_3202.getPosition()))){
                        result_peg_3202.nextPosition();
                    } else {
                        goto out_peg_3204;
                    }
                }
                result_peg_3202.setValue((void*) "vel");
            
            
            
            {
                    
                    result_peg_3202.reset();
                    do{
                        Result result_peg_3217(result_peg_3202.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3217.getPosition()))){
                                    result_peg_3217.nextPosition();
                                } else {
                                    goto out_peg_3220;
                                }
                            }
                            result_peg_3217.setValue((void*) " ");
                                
                        }
                        goto success_peg_3218;
                        out_peg_3220:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3217.getPosition()))){
                                    result_peg_3217.nextPosition();
                                } else {
                                    goto out_peg_3222;
                                }
                            }
                            result_peg_3217.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3218;
                        out_peg_3222:
                        goto loop_peg_3216;
                        success_peg_3218:
                        ;
                        result_peg_3202.addResult(result_peg_3217);
                    } while (true);
                    loop_peg_3216:
                    if (result_peg_3202.matches() == 0){
                        goto out_peg_3215;
                    }
                    
                }
                goto success_peg_3206;
                out_peg_3215:
                goto out_peg_3204;
                success_peg_3206:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3202.getPosition()))){
                        result_peg_3202.nextPosition();
                    } else {
                        goto out_peg_3204;
                    }
                }
                result_peg_3202.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("vel x");
                    result_peg_3202.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3202;
        stream.update(result_peg_3202.getPosition());
        
        
        return result_peg_3202;
        out_peg_3204:
        Result result_peg_3224(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_3224.getPosition()))){
                        result_peg_3224.nextPosition();
                    } else {
                        goto out_peg_3226;
                    }
                }
                result_peg_3224.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_3224.reset();
                    do{
                        Result result_peg_3239(result_peg_3224.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3239.getPosition()))){
                                    result_peg_3239.nextPosition();
                                } else {
                                    goto out_peg_3242;
                                }
                            }
                            result_peg_3239.setValue((void*) " ");
                                
                        }
                        goto success_peg_3240;
                        out_peg_3242:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3239.getPosition()))){
                                    result_peg_3239.nextPosition();
                                } else {
                                    goto out_peg_3244;
                                }
                            }
                            result_peg_3239.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3240;
                        out_peg_3244:
                        goto loop_peg_3238;
                        success_peg_3240:
                        ;
                        result_peg_3224.addResult(result_peg_3239);
                    } while (true);
                    loop_peg_3238:
                    if (result_peg_3224.matches() == 0){
                        goto out_peg_3237;
                    }
                    
                }
                goto success_peg_3228;
                out_peg_3237:
                goto out_peg_3226;
                success_peg_3228:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3224.getPosition()))){
                        result_peg_3224.nextPosition();
                    } else {
                        goto out_peg_3226;
                    }
                }
                result_peg_3224.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos y");
                    result_peg_3224.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3224;
        stream.update(result_peg_3224.getPosition());
        
        
        return result_peg_3224;
        out_peg_3226:
        Result result_peg_3246(myposition);
        
        {
        
            for (int i = 0; i < 3; i++){
                    if (compareCharCase("pos"[i], stream.get(result_peg_3246.getPosition()))){
                        result_peg_3246.nextPosition();
                    } else {
                        goto out_peg_3248;
                    }
                }
                result_peg_3246.setValue((void*) "pos");
            
            
            
            {
                    
                    result_peg_3246.reset();
                    do{
                        Result result_peg_3261(result_peg_3246.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3261.getPosition()))){
                                    result_peg_3261.nextPosition();
                                } else {
                                    goto out_peg_3264;
                                }
                            }
                            result_peg_3261.setValue((void*) " ");
                                
                        }
                        goto success_peg_3262;
                        out_peg_3264:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3261.getPosition()))){
                                    result_peg_3261.nextPosition();
                                } else {
                                    goto out_peg_3266;
                                }
                            }
                            result_peg_3261.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3262;
                        out_peg_3266:
                        goto loop_peg_3260;
                        success_peg_3262:
                        ;
                        result_peg_3246.addResult(result_peg_3261);
                    } while (true);
                    loop_peg_3260:
                    if (result_peg_3246.matches() == 0){
                        goto out_peg_3259;
                    }
                    
                }
                goto success_peg_3250;
                out_peg_3259:
                goto out_peg_3248;
                success_peg_3250:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3246.getPosition()))){
                        result_peg_3246.nextPosition();
                    } else {
                        goto out_peg_3248;
                    }
                }
                result_peg_3246.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("pos x");
                    result_peg_3246.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3246;
        stream.update(result_peg_3246.getPosition());
        
        
        return result_peg_3246;
        out_peg_3248:
        Result result_peg_3268(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_3268.getPosition()))){
                        result_peg_3268.nextPosition();
                    } else {
                        goto out_peg_3270;
                    }
                }
                result_peg_3268.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_3268.reset();
                    do{
                        Result result_peg_3283(result_peg_3268.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3283.getPosition()))){
                                    result_peg_3283.nextPosition();
                                } else {
                                    goto out_peg_3286;
                                }
                            }
                            result_peg_3283.setValue((void*) " ");
                                
                        }
                        goto success_peg_3284;
                        out_peg_3286:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3283.getPosition()))){
                                    result_peg_3283.nextPosition();
                                } else {
                                    goto out_peg_3288;
                                }
                            }
                            result_peg_3283.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3284;
                        out_peg_3288:
                        goto loop_peg_3282;
                        success_peg_3284:
                        ;
                        result_peg_3268.addResult(result_peg_3283);
                    } while (true);
                    loop_peg_3282:
                    if (result_peg_3268.matches() == 0){
                        goto out_peg_3281;
                    }
                    
                }
                goto success_peg_3272;
                out_peg_3281:
                goto out_peg_3270;
                success_peg_3272:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3268.getPosition()))){
                        result_peg_3268.nextPosition();
                    } else {
                        goto out_peg_3270;
                    }
                }
                result_peg_3268.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist x");
                    result_peg_3268.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3268;
        stream.update(result_peg_3268.getPosition());
        
        
        return result_peg_3268;
        out_peg_3270:
        Result result_peg_3290(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p2dist"[i], stream.get(result_peg_3290.getPosition()))){
                        result_peg_3290.nextPosition();
                    } else {
                        goto out_peg_3292;
                    }
                }
                result_peg_3290.setValue((void*) "p2dist");
            
            
            
            {
                    
                    result_peg_3290.reset();
                    do{
                        Result result_peg_3305(result_peg_3290.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3305.getPosition()))){
                                    result_peg_3305.nextPosition();
                                } else {
                                    goto out_peg_3308;
                                }
                            }
                            result_peg_3305.setValue((void*) " ");
                                
                        }
                        goto success_peg_3306;
                        out_peg_3308:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3305.getPosition()))){
                                    result_peg_3305.nextPosition();
                                } else {
                                    goto out_peg_3310;
                                }
                            }
                            result_peg_3305.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3306;
                        out_peg_3310:
                        goto loop_peg_3304;
                        success_peg_3306:
                        ;
                        result_peg_3290.addResult(result_peg_3305);
                    } while (true);
                    loop_peg_3304:
                    if (result_peg_3290.matches() == 0){
                        goto out_peg_3303;
                    }
                    
                }
                goto success_peg_3294;
                out_peg_3303:
                goto out_peg_3292;
                success_peg_3294:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3290.getPosition()))){
                        result_peg_3290.nextPosition();
                    } else {
                        goto out_peg_3292;
                    }
                }
                result_peg_3290.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2dist y");
                    result_peg_3290.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3290;
        stream.update(result_peg_3290.getPosition());
        
        
        return result_peg_3290;
        out_peg_3292:
        Result result_peg_3312(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3312.getPosition()))){
                        result_peg_3312.nextPosition();
                    } else {
                        goto out_peg_3314;
                    }
                }
                result_peg_3312.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3312.reset();
                    do{
                        Result result_peg_3327(result_peg_3312.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3327.getPosition()))){
                                    result_peg_3327.nextPosition();
                                } else {
                                    goto out_peg_3330;
                                }
                            }
                            result_peg_3327.setValue((void*) " ");
                                
                        }
                        goto success_peg_3328;
                        out_peg_3330:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3327.getPosition()))){
                                    result_peg_3327.nextPosition();
                                } else {
                                    goto out_peg_3332;
                                }
                            }
                            result_peg_3327.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3328;
                        out_peg_3332:
                        goto loop_peg_3326;
                        success_peg_3328:
                        ;
                        result_peg_3312.addResult(result_peg_3327);
                    } while (true);
                    loop_peg_3326:
                    if (result_peg_3312.matches() == 0){
                        goto out_peg_3325;
                    }
                    
                }
                goto success_peg_3316;
                out_peg_3325:
                goto out_peg_3314;
                success_peg_3316:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3312.getPosition()))){
                        result_peg_3312.nextPosition();
                    } else {
                        goto out_peg_3314;
                    }
                }
                result_peg_3312.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist x");
                    result_peg_3312.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3312;
        stream.update(result_peg_3312.getPosition());
        
        
        return result_peg_3312;
        out_peg_3314:
        Result result_peg_3334(myposition);
        
        {
        
            for (int i = 0; i < 6; i++){
                    if (compareCharCase("p1dist"[i], stream.get(result_peg_3334.getPosition()))){
                        result_peg_3334.nextPosition();
                    } else {
                        goto out_peg_3336;
                    }
                }
                result_peg_3334.setValue((void*) "p1dist");
            
            
            
            {
                    
                    result_peg_3334.reset();
                    do{
                        Result result_peg_3349(result_peg_3334.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3349.getPosition()))){
                                    result_peg_3349.nextPosition();
                                } else {
                                    goto out_peg_3352;
                                }
                            }
                            result_peg_3349.setValue((void*) " ");
                                
                        }
                        goto success_peg_3350;
                        out_peg_3352:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3349.getPosition()))){
                                    result_peg_3349.nextPosition();
                                } else {
                                    goto out_peg_3354;
                                }
                            }
                            result_peg_3349.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3350;
                        out_peg_3354:
                        goto loop_peg_3348;
                        success_peg_3350:
                        ;
                        result_peg_3334.addResult(result_peg_3349);
                    } while (true);
                    loop_peg_3348:
                    if (result_peg_3334.matches() == 0){
                        goto out_peg_3347;
                    }
                    
                }
                goto success_peg_3338;
                out_peg_3347:
                goto out_peg_3336;
                success_peg_3338:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3334.getPosition()))){
                        result_peg_3334.nextPosition();
                    } else {
                        goto out_peg_3336;
                    }
                }
                result_peg_3334.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1dist y");
                    result_peg_3334.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3334;
        stream.update(result_peg_3334.getPosition());
        
        
        return result_peg_3334;
        out_peg_3336:
        Result result_peg_3356(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3356.getPosition()))){
                        result_peg_3356.nextPosition();
                    } else {
                        goto out_peg_3358;
                    }
                }
                result_peg_3356.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3356.reset();
                    do{
                        Result result_peg_3371(result_peg_3356.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3371.getPosition()))){
                                    result_peg_3371.nextPosition();
                                } else {
                                    goto out_peg_3374;
                                }
                            }
                            result_peg_3371.setValue((void*) " ");
                                
                        }
                        goto success_peg_3372;
                        out_peg_3374:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3371.getPosition()))){
                                    result_peg_3371.nextPosition();
                                } else {
                                    goto out_peg_3376;
                                }
                            }
                            result_peg_3371.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3372;
                        out_peg_3376:
                        goto loop_peg_3370;
                        success_peg_3372:
                        ;
                        result_peg_3356.addResult(result_peg_3371);
                    } while (true);
                    loop_peg_3370:
                    if (result_peg_3356.matches() == 0){
                        goto out_peg_3369;
                    }
                    
                }
                goto success_peg_3360;
                out_peg_3369:
                goto out_peg_3358;
                success_peg_3360:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3356.getPosition()))){
                        result_peg_3356.nextPosition();
                    } else {
                        goto out_peg_3358;
                    }
                }
                result_peg_3356.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist x");
                    result_peg_3356.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3356;
        stream.update(result_peg_3356.getPosition());
        
        
        return result_peg_3356;
        out_peg_3358:
        Result result_peg_3378(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p2bodydist"[i], stream.get(result_peg_3378.getPosition()))){
                        result_peg_3378.nextPosition();
                    } else {
                        goto out_peg_3380;
                    }
                }
                result_peg_3378.setValue((void*) "p2bodydist");
            
            
            
            {
                    
                    result_peg_3378.reset();
                    do{
                        Result result_peg_3393(result_peg_3378.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3393.getPosition()))){
                                    result_peg_3393.nextPosition();
                                } else {
                                    goto out_peg_3396;
                                }
                            }
                            result_peg_3393.setValue((void*) " ");
                                
                        }
                        goto success_peg_3394;
                        out_peg_3396:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3393.getPosition()))){
                                    result_peg_3393.nextPosition();
                                } else {
                                    goto out_peg_3398;
                                }
                            }
                            result_peg_3393.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3394;
                        out_peg_3398:
                        goto loop_peg_3392;
                        success_peg_3394:
                        ;
                        result_peg_3378.addResult(result_peg_3393);
                    } while (true);
                    loop_peg_3392:
                    if (result_peg_3378.matches() == 0){
                        goto out_peg_3391;
                    }
                    
                }
                goto success_peg_3382;
                out_peg_3391:
                goto out_peg_3380;
                success_peg_3382:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3378.getPosition()))){
                        result_peg_3378.nextPosition();
                    } else {
                        goto out_peg_3380;
                    }
                }
                result_peg_3378.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p2bodydist y");
                    result_peg_3378.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3378;
        stream.update(result_peg_3378.getPosition());
        
        
        return result_peg_3378;
        out_peg_3380:
        Result result_peg_3400(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3400.getPosition()))){
                        result_peg_3400.nextPosition();
                    } else {
                        goto out_peg_3402;
                    }
                }
                result_peg_3400.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3400.reset();
                    do{
                        Result result_peg_3415(result_peg_3400.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3415.getPosition()))){
                                    result_peg_3415.nextPosition();
                                } else {
                                    goto out_peg_3418;
                                }
                            }
                            result_peg_3415.setValue((void*) " ");
                                
                        }
                        goto success_peg_3416;
                        out_peg_3418:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3415.getPosition()))){
                                    result_peg_3415.nextPosition();
                                } else {
                                    goto out_peg_3420;
                                }
                            }
                            result_peg_3415.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3416;
                        out_peg_3420:
                        goto loop_peg_3414;
                        success_peg_3416:
                        ;
                        result_peg_3400.addResult(result_peg_3415);
                    } while (true);
                    loop_peg_3414:
                    if (result_peg_3400.matches() == 0){
                        goto out_peg_3413;
                    }
                    
                }
                goto success_peg_3404;
                out_peg_3413:
                goto out_peg_3402;
                success_peg_3404:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3400.getPosition()))){
                        result_peg_3400.nextPosition();
                    } else {
                        goto out_peg_3402;
                    }
                }
                result_peg_3400.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist x");
                    result_peg_3400.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3400;
        stream.update(result_peg_3400.getPosition());
        
        
        return result_peg_3400;
        out_peg_3402:
        Result result_peg_3422(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("p1bodydist"[i], stream.get(result_peg_3422.getPosition()))){
                        result_peg_3422.nextPosition();
                    } else {
                        goto out_peg_3424;
                    }
                }
                result_peg_3422.setValue((void*) "p1bodydist");
            
            
            
            {
                    
                    result_peg_3422.reset();
                    do{
                        Result result_peg_3437(result_peg_3422.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3437.getPosition()))){
                                    result_peg_3437.nextPosition();
                                } else {
                                    goto out_peg_3440;
                                }
                            }
                            result_peg_3437.setValue((void*) " ");
                                
                        }
                        goto success_peg_3438;
                        out_peg_3440:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3437.getPosition()))){
                                    result_peg_3437.nextPosition();
                                } else {
                                    goto out_peg_3442;
                                }
                            }
                            result_peg_3437.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3438;
                        out_peg_3442:
                        goto loop_peg_3436;
                        success_peg_3438:
                        ;
                        result_peg_3422.addResult(result_peg_3437);
                    } while (true);
                    loop_peg_3436:
                    if (result_peg_3422.matches() == 0){
                        goto out_peg_3435;
                    }
                    
                }
                goto success_peg_3426;
                out_peg_3435:
                goto out_peg_3424;
                success_peg_3426:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3422.getPosition()))){
                        result_peg_3422.nextPosition();
                    } else {
                        goto out_peg_3424;
                    }
                }
                result_peg_3422.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("p1bodydist y");
                    result_peg_3422.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3422;
        stream.update(result_peg_3422.getPosition());
        
        
        return result_peg_3422;
        out_peg_3424:
        Result result_peg_3444(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3444.getPosition()))){
                        result_peg_3444.nextPosition();
                    } else {
                        goto out_peg_3446;
                    }
                }
                result_peg_3444.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3444.reset();
                    do{
                        Result result_peg_3459(result_peg_3444.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3459.getPosition()))){
                                    result_peg_3459.nextPosition();
                                } else {
                                    goto out_peg_3462;
                                }
                            }
                            result_peg_3459.setValue((void*) " ");
                                
                        }
                        goto success_peg_3460;
                        out_peg_3462:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3459.getPosition()))){
                                    result_peg_3459.nextPosition();
                                } else {
                                    goto out_peg_3464;
                                }
                            }
                            result_peg_3459.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3460;
                        out_peg_3464:
                        goto loop_peg_3458;
                        success_peg_3460:
                        ;
                        result_peg_3444.addResult(result_peg_3459);
                    } while (true);
                    loop_peg_3458:
                    if (result_peg_3444.matches() == 0){
                        goto out_peg_3457;
                    }
                    
                }
                goto success_peg_3448;
                out_peg_3457:
                goto out_peg_3446;
                success_peg_3448:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3444.getPosition()))){
                        result_peg_3444.nextPosition();
                    } else {
                        goto out_peg_3446;
                    }
                }
                result_peg_3444.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3444.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3444;
        stream.update(result_peg_3444.getPosition());
        
        
        return result_peg_3444;
        out_peg_3446:
        Result result_peg_3466(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3466.getPosition()))){
                        result_peg_3466.nextPosition();
                    } else {
                        goto out_peg_3468;
                    }
                }
                result_peg_3466.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3466.reset();
                    do{
                        Result result_peg_3481(result_peg_3466.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3481.getPosition()))){
                                    result_peg_3481.nextPosition();
                                } else {
                                    goto out_peg_3484;
                                }
                            }
                            result_peg_3481.setValue((void*) " ");
                                
                        }
                        goto success_peg_3482;
                        out_peg_3484:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3481.getPosition()))){
                                    result_peg_3481.nextPosition();
                                } else {
                                    goto out_peg_3486;
                                }
                            }
                            result_peg_3481.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3482;
                        out_peg_3486:
                        goto loop_peg_3480;
                        success_peg_3482:
                        ;
                        result_peg_3466.addResult(result_peg_3481);
                    } while (true);
                    loop_peg_3480:
                    if (result_peg_3466.matches() == 0){
                        goto out_peg_3479;
                    }
                    
                }
                goto success_peg_3470;
                out_peg_3479:
                goto out_peg_3468;
                success_peg_3470:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3466.getPosition()))){
                        result_peg_3466.nextPosition();
                    } else {
                        goto out_peg_3468;
                    }
                }
                result_peg_3466.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos x");
                    result_peg_3466.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3466;
        stream.update(result_peg_3466.getPosition());
        
        
        return result_peg_3466;
        out_peg_3468:
        Result result_peg_3488(myposition);
        
        {
        
            for (int i = 0; i < 9; i++){
                    if (compareCharCase("screenpos"[i], stream.get(result_peg_3488.getPosition()))){
                        result_peg_3488.nextPosition();
                    } else {
                        goto out_peg_3490;
                    }
                }
                result_peg_3488.setValue((void*) "screenpos");
            
            
            
            {
                    
                    result_peg_3488.reset();
                    do{
                        Result result_peg_3503(result_peg_3488.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3503.getPosition()))){
                                    result_peg_3503.nextPosition();
                                } else {
                                    goto out_peg_3506;
                                }
                            }
                            result_peg_3503.setValue((void*) " ");
                                
                        }
                        goto success_peg_3504;
                        out_peg_3506:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3503.getPosition()))){
                                    result_peg_3503.nextPosition();
                                } else {
                                    goto out_peg_3508;
                                }
                            }
                            result_peg_3503.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3504;
                        out_peg_3508:
                        goto loop_peg_3502;
                        success_peg_3504:
                        ;
                        result_peg_3488.addResult(result_peg_3503);
                    } while (true);
                    loop_peg_3502:
                    if (result_peg_3488.matches() == 0){
                        goto out_peg_3501;
                    }
                    
                }
                goto success_peg_3492;
                out_peg_3501:
                goto out_peg_3490;
                success_peg_3492:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3488.getPosition()))){
                        result_peg_3488.nextPosition();
                    } else {
                        goto out_peg_3490;
                    }
                }
                result_peg_3488.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("screenpos y");
                    result_peg_3488.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3488;
        stream.update(result_peg_3488.getPosition());
        
        
        return result_peg_3488;
        out_peg_3490:
        Result result_peg_3510(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3510.getPosition()))){
                        result_peg_3510.nextPosition();
                    } else {
                        goto out_peg_3512;
                    }
                }
                result_peg_3510.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3510.reset();
                    do{
                        Result result_peg_3525(result_peg_3510.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3525.getPosition()))){
                                    result_peg_3525.nextPosition();
                                } else {
                                    goto out_peg_3528;
                                }
                            }
                            result_peg_3525.setValue((void*) " ");
                                
                        }
                        goto success_peg_3526;
                        out_peg_3528:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3525.getPosition()))){
                                    result_peg_3525.nextPosition();
                                } else {
                                    goto out_peg_3530;
                                }
                            }
                            result_peg_3525.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3526;
                        out_peg_3530:
                        goto loop_peg_3524;
                        success_peg_3526:
                        ;
                        result_peg_3510.addResult(result_peg_3525);
                    } while (true);
                    loop_peg_3524:
                    if (result_peg_3510.matches() == 0){
                        goto out_peg_3523;
                    }
                    
                }
                goto success_peg_3514;
                out_peg_3523:
                goto out_peg_3512;
                success_peg_3514:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3510.getPosition()))){
                        result_peg_3510.nextPosition();
                    } else {
                        goto out_peg_3512;
                    }
                }
                result_peg_3510.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist x");
                    result_peg_3510.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3510;
        stream.update(result_peg_3510.getPosition());
        
        
        return result_peg_3510;
        out_peg_3512:
        Result result_peg_3532(myposition);
        
        {
        
            for (int i = 0; i < 10; i++){
                    if (compareCharCase("parentdist"[i], stream.get(result_peg_3532.getPosition()))){
                        result_peg_3532.nextPosition();
                    } else {
                        goto out_peg_3534;
                    }
                }
                result_peg_3532.setValue((void*) "parentdist");
            
            
            
            {
                    
                    result_peg_3532.reset();
                    do{
                        Result result_peg_3547(result_peg_3532.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3547.getPosition()))){
                                    result_peg_3547.nextPosition();
                                } else {
                                    goto out_peg_3550;
                                }
                            }
                            result_peg_3547.setValue((void*) " ");
                                
                        }
                        goto success_peg_3548;
                        out_peg_3550:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3547.getPosition()))){
                                    result_peg_3547.nextPosition();
                                } else {
                                    goto out_peg_3552;
                                }
                            }
                            result_peg_3547.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3548;
                        out_peg_3552:
                        goto loop_peg_3546;
                        success_peg_3548:
                        ;
                        result_peg_3532.addResult(result_peg_3547);
                    } while (true);
                    loop_peg_3546:
                    if (result_peg_3532.matches() == 0){
                        goto out_peg_3545;
                    }
                    
                }
                goto success_peg_3536;
                out_peg_3545:
                goto out_peg_3534;
                success_peg_3536:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3532.getPosition()))){
                        result_peg_3532.nextPosition();
                    } else {
                        goto out_peg_3534;
                    }
                }
                result_peg_3532.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("parentdist y");
                    result_peg_3532.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3532;
        stream.update(result_peg_3532.getPosition());
        
        
        return result_peg_3532;
        out_peg_3534:
        Result result_peg_3554(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3554.getPosition()))){
                        result_peg_3554.nextPosition();
                    } else {
                        goto out_peg_3556;
                    }
                }
                result_peg_3554.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3554.reset();
                    do{
                        Result result_peg_3569(result_peg_3554.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3569.getPosition()))){
                                    result_peg_3569.nextPosition();
                                } else {
                                    goto out_peg_3572;
                                }
                            }
                            result_peg_3569.setValue((void*) " ");
                                
                        }
                        goto success_peg_3570;
                        out_peg_3572:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3569.getPosition()))){
                                    result_peg_3569.nextPosition();
                                } else {
                                    goto out_peg_3574;
                                }
                            }
                            result_peg_3569.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3570;
                        out_peg_3574:
                        goto loop_peg_3568;
                        success_peg_3570:
                        ;
                        result_peg_3554.addResult(result_peg_3569);
                    } while (true);
                    loop_peg_3568:
                    if (result_peg_3554.matches() == 0){
                        goto out_peg_3567;
                    }
                    
                }
                goto success_peg_3558;
                out_peg_3567:
                goto out_peg_3556;
                success_peg_3558:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("x"[i], stream.get(result_peg_3554.getPosition()))){
                        result_peg_3554.nextPosition();
                    } else {
                        goto out_peg_3556;
                    }
                }
                result_peg_3554.setValue((void*) "x");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist x");
                    result_peg_3554.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3554;
        stream.update(result_peg_3554.getPosition());
        
        
        return result_peg_3554;
        out_peg_3556:
        Result result_peg_3576(myposition);
        
        {
        
            for (int i = 0; i < 8; i++){
                    if (compareCharCase("rootdist"[i], stream.get(result_peg_3576.getPosition()))){
                        result_peg_3576.nextPosition();
                    } else {
                        goto out_peg_3578;
                    }
                }
                result_peg_3576.setValue((void*) "rootdist");
            
            
            
            {
                    
                    result_peg_3576.reset();
                    do{
                        Result result_peg_3591(result_peg_3576.getPosition());
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar(" "[i], stream.get(result_peg_3591.getPosition()))){
                                    result_peg_3591.nextPosition();
                                } else {
                                    goto out_peg_3594;
                                }
                            }
                            result_peg_3591.setValue((void*) " ");
                                
                        }
                        goto success_peg_3592;
                        out_peg_3594:
                        {
                            
                            for (int i = 0; i < 1; i++){
                                if (compareChar("\t"[i], stream.get(result_peg_3591.getPosition()))){
                                    result_peg_3591.nextPosition();
                                } else {
                                    goto out_peg_3596;
                                }
                            }
                            result_peg_3591.setValue((void*) "\t");
                                
                        }
                        goto success_peg_3592;
                        out_peg_3596:
                        goto loop_peg_3590;
                        success_peg_3592:
                        ;
                        result_peg_3576.addResult(result_peg_3591);
                    } while (true);
                    loop_peg_3590:
                    if (result_peg_3576.matches() == 0){
                        goto out_peg_3589;
                    }
                    
                }
                goto success_peg_3580;
                out_peg_3589:
                goto out_peg_3578;
                success_peg_3580:
                ;
            
            
            
            for (int i = 0; i < 1; i++){
                    if (compareCharCase("y"[i], stream.get(result_peg_3576.getPosition()))){
                        result_peg_3576.nextPosition();
                    } else {
                        goto out_peg_3578;
                    }
                }
                result_peg_3576.setValue((void*) "y");
            
            
            
            {
                    Value value((void*) 0);
                    value = makeKeyword("rootdist y");
                    result_peg_3576.setValue(value);
                }
            
            
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3576;
        stream.update(result_peg_3576.getPosition());
        
        
        return result_peg_3576;
        out_peg_3578:
        Result result_peg_3598(myposition);
        
        result_peg_3598 = rule_hitflag(stream, result_peg_3598.getPosition());
        if (result_peg_3598.error()){
            goto out_peg_3599;
        }
        
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = result_peg_3598;
        stream.update(result_peg_3598.getPosition());
        
        
        return result_peg_3598;
        out_peg_3599:
    
        if (column_peg_3179.chunk14 == 0){
            column_peg_3179.chunk14 = new Chunk14();
        }
        column_peg_3179.chunk14->chunk_keyword_real = errorResult;
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

        
