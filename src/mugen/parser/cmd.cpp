

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
    Result chunk_s;
    Result chunk_comment;
    Result chunk_section;
    Result chunk_section_item;
    Result chunk_section_title;
};

struct Chunk2{
    Result chunk_assignment;
    Result chunk_identifier;
    Result chunk_integer;
    Result chunk_float;
    Result chunk_string;
};

struct Chunk3{
    Result chunk_range;
    Result chunk_name;
    Result chunk_alpha_digit;
    Result chunk_valuelist;
    Result chunk_expr;
};

struct Chunk4{
    Result chunk_expr_c;
    Result chunk_expr2;
    Result chunk_expr3;
    Result chunk_expr4;
    Result chunk_expr5;
};

struct Chunk5{
    Result chunk_expr6;
    Result chunk_expr7;
    Result chunk_expr8;
    Result chunk_expr9;
    Result chunk_expr10;
};

struct Chunk6{
    Result chunk_expr11;
    Result chunk_expr12;
    Result chunk_expr13;
    Result chunk_expr_rest;
    Result chunk_expr2_rest;
};

struct Chunk7{
    Result chunk_expr3_rest;
    Result chunk_expr4_rest;
    Result chunk_expr5_rest;
    Result chunk_expr6_rest;
    Result chunk_expr7_rest;
};

struct Chunk8{
    Result chunk_expr8_rest;
    Result chunk_expr9_rest;
    Result chunk_compare;
    Result chunk_all_compare;
    Result chunk_expr10_rest;
};

struct Chunk9{
    Result chunk_expr11_rest;
    Result chunk_expr12_rest;
    Result chunk_unary;
    Result chunk_expr13_real;
    Result chunk_function;
};

struct Chunk10{
    Result chunk_args;
    Result chunk_function_name;
    Result chunk_function_rest;
    Result chunk_keys;
    Result chunk_key_value_list;
};

struct Chunk11{
    Result chunk_key;
    Result chunk_key_rest;
    Result chunk_key_real;
    Result chunk_key_modifier;
    Result chunk_key_name;
};

struct Chunk12{
    Result chunk_value;
    Result chunk_helper;
    Result chunk_helper__expression;
    Result chunk_helper__name;
    Result chunk_helper__identifier;
};

struct Chunk13{
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
        ,chunk13(0){
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

    int hitCount(){
        return 
(chunk13 != NULL ? ((chunk13->chunk_hitflag.calculated() ? 1 : 0)
+ (chunk13->chunk_keyword.calculated() ? 1 : 0)
+ (chunk13->chunk_keyword_real.calculated() ? 1 : 0)) : 0)
+
(chunk12 != NULL ? ((chunk12->chunk_value.calculated() ? 1 : 0)
+ (chunk12->chunk_helper.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__expression.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__name.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__identifier.calculated() ? 1 : 0)) : 0)
+
(chunk11 != NULL ? ((chunk11->chunk_key.calculated() ? 1 : 0)
+ (chunk11->chunk_key_rest.calculated() ? 1 : 0)
+ (chunk11->chunk_key_real.calculated() ? 1 : 0)
+ (chunk11->chunk_key_modifier.calculated() ? 1 : 0)
+ (chunk11->chunk_key_name.calculated() ? 1 : 0)) : 0)
+
(chunk10 != NULL ? ((chunk10->chunk_args.calculated() ? 1 : 0)
+ (chunk10->chunk_function_name.calculated() ? 1 : 0)
+ (chunk10->chunk_function_rest.calculated() ? 1 : 0)
+ (chunk10->chunk_keys.calculated() ? 1 : 0)
+ (chunk10->chunk_key_value_list.calculated() ? 1 : 0)) : 0)
+
(chunk9 != NULL ? ((chunk9->chunk_expr11_rest.calculated() ? 1 : 0)
+ (chunk9->chunk_expr12_rest.calculated() ? 1 : 0)
+ (chunk9->chunk_unary.calculated() ? 1 : 0)
+ (chunk9->chunk_expr13_real.calculated() ? 1 : 0)
+ (chunk9->chunk_function.calculated() ? 1 : 0)) : 0)
+
(chunk8 != NULL ? ((chunk8->chunk_expr8_rest.calculated() ? 1 : 0)
+ (chunk8->chunk_expr9_rest.calculated() ? 1 : 0)
+ (chunk8->chunk_compare.calculated() ? 1 : 0)
+ (chunk8->chunk_all_compare.calculated() ? 1 : 0)
+ (chunk8->chunk_expr10_rest.calculated() ? 1 : 0)) : 0)
+
(chunk3 != NULL ? ((chunk3->chunk_range.calculated() ? 1 : 0)
+ (chunk3->chunk_name.calculated() ? 1 : 0)
+ (chunk3->chunk_alpha_digit.calculated() ? 1 : 0)
+ (chunk3->chunk_valuelist.calculated() ? 1 : 0)
+ (chunk3->chunk_expr.calculated() ? 1 : 0)) : 0)
+
(chunk2 != NULL ? ((chunk2->chunk_assignment.calculated() ? 1 : 0)
+ (chunk2->chunk_identifier.calculated() ? 1 : 0)
+ (chunk2->chunk_integer.calculated() ? 1 : 0)
+ (chunk2->chunk_float.calculated() ? 1 : 0)
+ (chunk2->chunk_string.calculated() ? 1 : 0)) : 0)
+
(chunk1 != NULL ? ((chunk1->chunk_s.calculated() ? 1 : 0)
+ (chunk1->chunk_comment.calculated() ? 1 : 0)
+ (chunk1->chunk_section.calculated() ? 1 : 0)
+ (chunk1->chunk_section_item.calculated() ? 1 : 0)
+ (chunk1->chunk_section_title.calculated() ? 1 : 0)) : 0)
+
(chunk0 != NULL ? ((chunk0->chunk_start.calculated() ? 1 : 0)
+ (chunk0->chunk_line.calculated() ? 1 : 0)
+ (chunk0->chunk_line_end.calculated() ? 1 : 0)
+ (chunk0->chunk_whitespace.calculated() ? 1 : 0)
+ (chunk0->chunk_sw.calculated() ? 1 : 0)) : 0)
+
(chunk7 != NULL ? ((chunk7->chunk_expr3_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr4_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr5_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr6_rest.calculated() ? 1 : 0)
+ (chunk7->chunk_expr7_rest.calculated() ? 1 : 0)) : 0)
+
(chunk6 != NULL ? ((chunk6->chunk_expr11.calculated() ? 1 : 0)
+ (chunk6->chunk_expr12.calculated() ? 1 : 0)
+ (chunk6->chunk_expr13.calculated() ? 1 : 0)
+ (chunk6->chunk_expr_rest.calculated() ? 1 : 0)
+ (chunk6->chunk_expr2_rest.calculated() ? 1 : 0)) : 0)
+
(chunk5 != NULL ? ((chunk5->chunk_expr6.calculated() ? 1 : 0)
+ (chunk5->chunk_expr7.calculated() ? 1 : 0)
+ (chunk5->chunk_expr8.calculated() ? 1 : 0)
+ (chunk5->chunk_expr9.calculated() ? 1 : 0)
+ (chunk5->chunk_expr10.calculated() ? 1 : 0)) : 0)
+
(chunk4 != NULL ? ((chunk4->chunk_expr_c.calculated() ? 1 : 0)
+ (chunk4->chunk_expr2.calculated() ? 1 : 0)
+ (chunk4->chunk_expr3.calculated() ? 1 : 0)
+ (chunk4->chunk_expr4.calculated() ? 1 : 0)
+ (chunk4->chunk_expr5.calculated() ? 1 : 0)) : 0)
;
    }

    int maxHits(){
        return 68;
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
Result rule_whitespace(Stream &, const int);
Result rule_sw(Stream &, const int);
Result rule_s(Stream &, const int);
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

Ast::Value * makeFunction1(const Value & name, const Value & arg1){
    return makeFunction(name, makeValueList(arg1, Value()));
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




Result rule_start(Stream & stream, const int position){
    
    Column & column_peg_1 = stream.getColumn(position);
    if (column_peg_1.chunk0 != 0 && column_peg_1.chunk0->chunk_start.calculated()){
        return column_peg_1.chunk0->chunk_start;
    }
    
    RuleTrace trace_peg_21(stream, "start");
    int myposition = position;
    
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
                
                    result_peg_16 = rule_line(stream, result_peg_16.getPosition(), current);
                        if (result_peg_16.error()){
                            goto loop_peg_15;
                        }
                    
                    
                    
                    result_peg_16 = rule_whitespace(stream, result_peg_16.getPosition());
                        if (result_peg_16.error()){
                            goto loop_peg_15;
                        }
                    
                    
                    
                    int save_peg_19 = result_peg_16.getPosition();
                        
                        result_peg_16 = rule_line_end(stream, result_peg_16.getPosition());
                        if (result_peg_16.error()){
                            
                            result_peg_16 = Result(save_peg_19);
                            result_peg_16.setValue((void*) 0);
                            
                        }
                    
                    
                }
                result_peg_2.addResult(result_peg_16);
            } while (true);
            loop_peg_15:
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
    
     GC::cleanup(0); 
    return errorResult;
}
        

Result rule_line(Stream & stream, const int position, Value current){
    
    RuleTrace trace_peg_38(stream, "line");
    int myposition = position;
    
    
    Result result_peg_23(myposition);
    
    {
    
        result_peg_23 = rule_s(stream, result_peg_23.getPosition());
            if (result_peg_23.error()){
                goto out_peg_25;
            }
        
        
        
        result_peg_23 = rule_comment(stream, result_peg_23.getPosition());
            if (result_peg_23.error()){
                goto out_peg_25;
            }
        
        
    }
    stream.update(result_peg_23.getPosition());
    
    return result_peg_23;
    out_peg_25:
    Result result_peg_26(myposition);
    
    {
    
        result_peg_26 = rule_s(stream, result_peg_26.getPosition());
            if (result_peg_26.error()){
                goto out_peg_28;
            }
        
        
        
        result_peg_26 = rule_section(stream, result_peg_26.getPosition());
            if (result_peg_26.error()){
                goto out_peg_28;
            }
        
        Result result_peg_29 = result_peg_26;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_29.getValues());
                result_peg_26.setValue(value);
            }
        
        
    }
    stream.update(result_peg_26.getPosition());
    
    return result_peg_26;
    out_peg_28:
    Result result_peg_30(myposition);
    
    {
    
        result_peg_30 = rule_s(stream, result_peg_30.getPosition());
            if (result_peg_30.error()){
                goto out_peg_32;
            }
        
        
        
        {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_35;
                    }
                }
                result_peg_30.setValue((void*) "\n");
                    
            }
            goto success_peg_33;
            out_peg_35:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_30.getPosition()))){
                        result_peg_30.nextPosition();
                    } else {
                        goto out_peg_37;
                    }
                }
                result_peg_30.setValue((void*) "\r");
                    
            }
            goto success_peg_33;
            out_peg_37:
            goto out_peg_32;
            success_peg_33:
            ;
        
        
    }
    stream.update(result_peg_30.getPosition());
    
    return result_peg_30;
    out_peg_32:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_39 = stream.getColumn(position);
    if (column_peg_39.chunk0 != 0 && column_peg_39.chunk0->chunk_line_end.calculated()){
        return column_peg_39.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_53(stream, "line_end");
    int myposition = position;
    
    
    Result result_peg_40(myposition);
    
    result_peg_40.reset();
    do{
        Result result_peg_42(result_peg_40.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_42.getPosition()))){
                    result_peg_42.nextPosition();
                } else {
                    goto out_peg_45;
                }
            }
            result_peg_42.setValue((void*) "\n");
                
        }
        goto success_peg_43;
        out_peg_45:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_42.getPosition()))){
                    result_peg_42.nextPosition();
                } else {
                    goto out_peg_47;
                }
            }
            result_peg_42.setValue((void*) "\r");
                
        }
        goto success_peg_43;
        out_peg_47:
        goto loop_peg_41;
        success_peg_43:
        ;
        result_peg_40.addResult(result_peg_42);
    } while (true);
    loop_peg_41:
    if (result_peg_40.matches() == 0){
        goto out_peg_48;
    }
    
    if (column_peg_39.chunk0 == 0){
        column_peg_39.chunk0 = new Chunk0();
    }
    column_peg_39.chunk0->chunk_line_end = result_peg_40;
    stream.update(result_peg_40.getPosition());
    
    
    return result_peg_40;
    out_peg_48:
    Result result_peg_49(myposition);
    
    {
    
        Result result_peg_51(result_peg_49.getPosition());
            if ('\0' == stream.get(result_peg_51.getPosition())){
                result_peg_51.nextPosition();
                result_peg_51.setValue((void *) '\0');
            } else {
                goto out_peg_52;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_39.chunk0 == 0){
        column_peg_39.chunk0 = new Chunk0();
    }
    column_peg_39.chunk0->chunk_line_end = result_peg_49;
    stream.update(result_peg_49.getPosition());
    
    
    return result_peg_49;
    out_peg_52:
    
    if (column_peg_39.chunk0 == 0){
        column_peg_39.chunk0 = new Chunk0();
    }
    column_peg_39.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_54 = stream.getColumn(position);
    if (column_peg_54.chunk0 != 0 && column_peg_54.chunk0->chunk_whitespace.calculated()){
        return column_peg_54.chunk0->chunk_whitespace;
    }
    
    RuleTrace trace_peg_58(stream, "whitespace");
    int myposition = position;
    
    
    Result result_peg_55(myposition);
    
    result_peg_55.reset();
    do{
        Result result_peg_57(result_peg_55.getPosition());
        result_peg_57 = rule_sw(stream, result_peg_57.getPosition());
        if (result_peg_57.error()){
            goto loop_peg_56;
        }
        result_peg_55.addResult(result_peg_57);
    } while (true);
    loop_peg_56:
    ;
    
    if (column_peg_54.chunk0 == 0){
        column_peg_54.chunk0 = new Chunk0();
    }
    column_peg_54.chunk0->chunk_whitespace = result_peg_55;
    stream.update(result_peg_55.getPosition());
    
    
    return result_peg_55;
    
    if (column_peg_54.chunk0 == 0){
        column_peg_54.chunk0 = new Chunk0();
    }
    column_peg_54.chunk0->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_59 = stream.getColumn(position);
    if (column_peg_59.chunk0 != 0 && column_peg_59.chunk0->chunk_sw.calculated()){
        return column_peg_59.chunk0->chunk_sw;
    }
    
    RuleTrace trace_peg_69(stream, "sw");
    int myposition = position;
    
    
    Result result_peg_60(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_60.getPosition()))){
                result_peg_60.nextPosition();
            } else {
                goto out_peg_63;
            }
        }
        result_peg_60.setValue((void*) " ");
            
    }
    goto success_peg_61;
    out_peg_63:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_60.getPosition()))){
                result_peg_60.nextPosition();
            } else {
                goto out_peg_65;
            }
        }
        result_peg_60.setValue((void*) "\t");
            
    }
    goto success_peg_61;
    out_peg_65:
    goto out_peg_66;
    success_peg_61:
    ;
    
    if (column_peg_59.chunk0 == 0){
        column_peg_59.chunk0 = new Chunk0();
    }
    column_peg_59.chunk0->chunk_sw = result_peg_60;
    stream.update(result_peg_60.getPosition());
    
    
    return result_peg_60;
    out_peg_66:
    Result result_peg_67(myposition);
    
    result_peg_67 = rule_comment(stream, result_peg_67.getPosition());
    if (result_peg_67.error()){
        goto out_peg_68;
    }
    
    if (column_peg_59.chunk0 == 0){
        column_peg_59.chunk0 = new Chunk0();
    }
    column_peg_59.chunk0->chunk_sw = result_peg_67;
    stream.update(result_peg_67.getPosition());
    
    
    return result_peg_67;
    out_peg_68:
    
    if (column_peg_59.chunk0 == 0){
        column_peg_59.chunk0 = new Chunk0();
    }
    column_peg_59.chunk0->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_70 = stream.getColumn(position);
    if (column_peg_70.chunk1 != 0 && column_peg_70.chunk1->chunk_s.calculated()){
        return column_peg_70.chunk1->chunk_s;
    }
    
    RuleTrace trace_peg_79(stream, "s");
    int myposition = position;
    
    
    Result result_peg_71(myposition);
    
    result_peg_71.reset();
    do{
        Result result_peg_73(result_peg_71.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_73.getPosition()))){
                    result_peg_73.nextPosition();
                } else {
                    goto out_peg_76;
                }
            }
            result_peg_73.setValue((void*) " ");
                
        }
        goto success_peg_74;
        out_peg_76:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_73.getPosition()))){
                    result_peg_73.nextPosition();
                } else {
                    goto out_peg_78;
                }
            }
            result_peg_73.setValue((void*) "\t");
                
        }
        goto success_peg_74;
        out_peg_78:
        goto loop_peg_72;
        success_peg_74:
        ;
        result_peg_71.addResult(result_peg_73);
    } while (true);
    loop_peg_72:
    ;
    
    if (column_peg_70.chunk1 == 0){
        column_peg_70.chunk1 = new Chunk1();
    }
    column_peg_70.chunk1->chunk_s = result_peg_71;
    stream.update(result_peg_71.getPosition());
    
    
    return result_peg_71;
    
    if (column_peg_70.chunk1 == 0){
        column_peg_70.chunk1 = new Chunk1();
    }
    column_peg_70.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_80 = stream.getColumn(position);
    if (column_peg_80.chunk1 != 0 && column_peg_80.chunk1->chunk_comment.calculated()){
        return column_peg_80.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_109(stream, "comment");
    int myposition = position;
    
    
    Result result_peg_81(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_81.getPosition()))){
                    result_peg_81.nextPosition();
                } else {
                    goto out_peg_83;
                }
            }
            result_peg_81.setValue((void*) ";");
        
        
        
        result_peg_81.reset();
            do{
                Result result_peg_85(result_peg_81.getPosition());
                {
                
                    Result result_peg_88(result_peg_85);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_88.getPosition()))){
                                result_peg_88.nextPosition();
                            } else {
                                goto not_peg_87;
                            }
                        }
                        result_peg_88.setValue((void*) "\n");
                        goto loop_peg_84;
                        not_peg_87:
                        result_peg_85.setValue((void*)0);
                    
                    
                    
                    char temp_peg_89 = stream.get(result_peg_85.getPosition());
                        if (temp_peg_89 != '\0'){
                            result_peg_85.setValue((void*) (long) temp_peg_89);
                            result_peg_85.nextPosition();
                        } else {
                            goto loop_peg_84;
                        }
                    
                    
                }
                result_peg_81.addResult(result_peg_85);
            } while (true);
            loop_peg_84:
            ;
        
        
    }
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_comment = result_peg_81;
    stream.update(result_peg_81.getPosition());
    
    
    return result_peg_81;
    out_peg_83:
    Result result_peg_90(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_90.getPosition()))){
                    result_peg_90.nextPosition();
                } else {
                    goto out_peg_92;
                }
            }
            result_peg_90.setValue((void*) "=");
        
        
        
        result_peg_90.reset();
            do{
                Result result_peg_94(result_peg_90.getPosition());
                {
                
                    Result result_peg_97(result_peg_94);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_97.getPosition()))){
                                result_peg_97.nextPosition();
                            } else {
                                goto not_peg_96;
                            }
                        }
                        result_peg_97.setValue((void*) "\n");
                        goto loop_peg_93;
                        not_peg_96:
                        result_peg_94.setValue((void*)0);
                    
                    
                    
                    char temp_peg_98 = stream.get(result_peg_94.getPosition());
                        if (temp_peg_98 != '\0'){
                            result_peg_94.setValue((void*) (long) temp_peg_98);
                            result_peg_94.nextPosition();
                        } else {
                            goto loop_peg_93;
                        }
                    
                    
                }
                result_peg_90.addResult(result_peg_94);
            } while (true);
            loop_peg_93:
            ;
        
        
    }
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_comment = result_peg_90;
    stream.update(result_peg_90.getPosition());
    
    
    return result_peg_90;
    out_peg_92:
    Result result_peg_99(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_99.getPosition()))){
                    result_peg_99.nextPosition();
                } else {
                    goto out_peg_101;
                }
            }
            result_peg_99.setValue((void*) "-");
        
        
        
        result_peg_99.reset();
            do{
                Result result_peg_103(result_peg_99.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_103.getPosition()))){
                        result_peg_103.nextPosition();
                    } else {
                        goto loop_peg_102;
                    }
                }
                result_peg_103.setValue((void*) "-");
                result_peg_99.addResult(result_peg_103);
            } while (true);
            loop_peg_102:
            if (result_peg_99.matches() == 0){
                goto out_peg_101;
            }
        
        
    }
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_comment = result_peg_99;
    stream.update(result_peg_99.getPosition());
    
    
    return result_peg_99;
    out_peg_101:
    Result result_peg_104(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_104.getPosition()))){
                    result_peg_104.nextPosition();
                } else {
                    goto out_peg_106;
                }
            }
            result_peg_104.setValue((void*) ":");
        
        
        
        result_peg_104.reset();
            do{
                Result result_peg_108(result_peg_104.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_108.getPosition()))){
                        result_peg_108.nextPosition();
                    } else {
                        goto loop_peg_107;
                    }
                }
                result_peg_108.setValue((void*) ":");
                result_peg_104.addResult(result_peg_108);
            } while (true);
            loop_peg_107:
            if (result_peg_104.matches() == 0){
                goto out_peg_106;
            }
        
        
    }
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_comment = result_peg_104;
    stream.update(result_peg_104.getPosition());
    
    
    return result_peg_104;
    out_peg_106:
    
    if (column_peg_80.chunk1 == 0){
        column_peg_80.chunk1 = new Chunk1();
    }
    column_peg_80.chunk1->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_110 = stream.getColumn(position);
    if (column_peg_110.chunk1 != 0 && column_peg_110.chunk1->chunk_section.calculated()){
        return column_peg_110.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_129(stream, "section");
    int myposition = position;
    
    Value ast;
    Result result_peg_111(myposition);
    
    {
    
        result_peg_111 = rule_section_title(stream, result_peg_111.getPosition());
            if (result_peg_111.error()){
                goto out_peg_113;
            }
        
        Result result_peg_112 = result_peg_111;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_112.getValues());
                result_peg_111.setValue(value);
            }
            ast = result_peg_111.getValues();
        
        
        
        result_peg_111 = rule_whitespace(stream, result_peg_111.getPosition());
            if (result_peg_111.error()){
                goto out_peg_113;
            }
        
        
        
        result_peg_111.reset();
            do{
                Result result_peg_118(result_peg_111.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_118.getPosition()))){
                            result_peg_118.nextPosition();
                        } else {
                            goto out_peg_121;
                        }
                    }
                    result_peg_118.setValue((void*) "\n");
                        
                }
                goto success_peg_119;
                out_peg_121:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_118.getPosition()))){
                            result_peg_118.nextPosition();
                        } else {
                            goto out_peg_123;
                        }
                    }
                    result_peg_118.setValue((void*) "\r");
                        
                }
                goto success_peg_119;
                out_peg_123:
                goto loop_peg_117;
                success_peg_119:
                ;
                result_peg_111.addResult(result_peg_118);
            } while (true);
            loop_peg_117:
            if (result_peg_111.matches() == 0){
                goto out_peg_113;
            }
        
        
        
        result_peg_111.reset();
            do{
                Result result_peg_126(result_peg_111.getPosition());
                {
                
                    result_peg_126 = rule_section_item(stream, result_peg_126.getPosition(), ast);
                        if (result_peg_126.error()){
                            goto loop_peg_125;
                        }
                    
                    
                    
                    result_peg_126 = rule_whitespace(stream, result_peg_126.getPosition());
                        if (result_peg_126.error()){
                            goto loop_peg_125;
                        }
                    
                    
                    
                    result_peg_126 = rule_line_end(stream, result_peg_126.getPosition());
                        if (result_peg_126.error()){
                            goto loop_peg_125;
                        }
                    
                    
                }
                result_peg_111.addResult(result_peg_126);
            } while (true);
            loop_peg_125:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_111.setValue(value);
            }
        
        
    }
    
    if (column_peg_110.chunk1 == 0){
        column_peg_110.chunk1 = new Chunk1();
    }
    column_peg_110.chunk1->chunk_section = result_peg_111;
    stream.update(result_peg_111.getPosition());
    
    
    return result_peg_111;
    out_peg_113:
    
    if (column_peg_110.chunk1 == 0){
        column_peg_110.chunk1 = new Chunk1();
    }
    column_peg_110.chunk1->chunk_section = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_138(stream, "section_item");
    int myposition = position;
    
    Value data;
    Result result_peg_131(myposition);
    
    {
    
        result_peg_131 = rule_s(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
        
        
        
        result_peg_131 = rule_comment(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
        
        
    }
    stream.update(result_peg_131.getPosition());
    
    return result_peg_131;
    out_peg_133:
    Result result_peg_134(myposition);
    
    {
    
        result_peg_134 = rule_s(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
        
        
        
        result_peg_134 = rule_assignment(stream, result_peg_134.getPosition());
            if (result_peg_134.error()){
                goto out_peg_136;
            }
            data = result_peg_134.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_134.setValue(value);
            }
        
        
    }
    stream.update(result_peg_134.getPosition());
    
    return result_peg_134;
    out_peg_136:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_139 = stream.getColumn(position);
    if (column_peg_139.chunk1 != 0 && column_peg_139.chunk1->chunk_section_title.calculated()){
        return column_peg_139.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_152(stream, "section_title");
    int myposition = position;
    
    Value name;
    Result result_peg_140(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_140.getPosition()))){
                    result_peg_140.nextPosition();
                } else {
                    goto out_peg_142;
                }
            }
            result_peg_140.setValue((void*) "[");
        
        
        
        result_peg_140 = rule_s(stream, result_peg_140.getPosition());
            if (result_peg_140.error()){
                goto out_peg_142;
            }
        
        
        
        result_peg_140.reset();
            do{
                Result result_peg_146(result_peg_140.getPosition());
                {
                
                    Result result_peg_149(result_peg_146);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_149.getPosition()))){
                                result_peg_149.nextPosition();
                            } else {
                                goto not_peg_148;
                            }
                        }
                        result_peg_149.setValue((void*) "]");
                        goto loop_peg_145;
                        not_peg_148:
                        result_peg_146.setValue((void*)0);
                    
                    
                    
                    char temp_peg_150 = stream.get(result_peg_146.getPosition());
                        if (temp_peg_150 != '\0'){
                            result_peg_146.setValue((void*) (long) temp_peg_150);
                            result_peg_146.nextPosition();
                        } else {
                            goto loop_peg_145;
                        }
                    
                    
                }
                result_peg_140.addResult(result_peg_146);
            } while (true);
            loop_peg_145:
            if (result_peg_140.matches() == 0){
                goto out_peg_142;
            }
            name = result_peg_140.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_140.getPosition()))){
                    result_peg_140.nextPosition();
                } else {
                    goto out_peg_142;
                }
            }
            result_peg_140.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(name);
                result_peg_140.setValue(value);
            }
        
        
    }
    
    if (column_peg_139.chunk1 == 0){
        column_peg_139.chunk1 = new Chunk1();
    }
    column_peg_139.chunk1->chunk_section_title = result_peg_140;
    stream.update(result_peg_140.getPosition());
    
    
    return result_peg_140;
    out_peg_142:
    
    if (column_peg_139.chunk1 == 0){
        column_peg_139.chunk1 = new Chunk1();
    }
    column_peg_139.chunk1->chunk_section_title = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_153 = stream.getColumn(position);
    if (column_peg_153.chunk2 != 0 && column_peg_153.chunk2->chunk_assignment.calculated()){
        return column_peg_153.chunk2->chunk_assignment;
    }
    
    RuleTrace trace_peg_198(stream, "assignment");
    int myposition = position;
    
    Value all;
    Value line;
    Value name;
    Value exp;
    Value index;
    Result result_peg_154(myposition);
    
    {
    
        for (int i = 0; i < 7; i++){
                if (compareCharCase("command"[i], stream.get(result_peg_154.getPosition()))){
                    result_peg_154.nextPosition();
                } else {
                    goto out_peg_156;
                }
            }
            result_peg_154.setValue((void*) "command");
        
        
        
        result_peg_154 = rule_s(stream, result_peg_154.getPosition());
            if (result_peg_154.error()){
                goto out_peg_156;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_154.getPosition()))){
                    result_peg_154.nextPosition();
                } else {
                    goto out_peg_156;
                }
            }
            result_peg_154.setValue((void*) "=");
        
        
        
        result_peg_154 = rule_s(stream, result_peg_154.getPosition());
            if (result_peg_154.error()){
                goto out_peg_156;
            }
        
        
        
        result_peg_154 = rule_keys(stream, result_peg_154.getPosition());
            if (result_peg_154.error()){
                goto out_peg_156;
            }
            all = result_peg_154.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("command", all);
                result_peg_154.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_154;
    stream.update(result_peg_154.getPosition());
    
    
    return result_peg_154;
    out_peg_156:
    Result result_peg_161(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_163 = stream.getLineInfo(result_peg_161.getPosition());
            line = &line_info_peg_163;
        
        
        
        result_peg_161 = rule_identifier(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_165;
            }
            name = result_peg_161.getValues();
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_165;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_165;
                }
            }
            result_peg_161.setValue((void*) "=");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_165;
            }
        
        
        
        result_peg_161 = rule_expr(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_165;
            }
            exp = result_peg_161.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                result_peg_161.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_161;
    stream.update(result_peg_161.getPosition());
    
    
    return result_peg_161;
    out_peg_165:
    Result result_peg_170(myposition);
    
    {
    
        result_peg_170 = rule_identifier(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
            name = result_peg_170.getValues();
        
        
        
        result_peg_170 = rule_s(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_170.getPosition()))){
                    result_peg_170.nextPosition();
                } else {
                    goto out_peg_172;
                }
            }
            result_peg_170.setValue((void*) "!=");
        
        
        
        result_peg_170 = rule_s(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
        
        
        
        result_peg_170 = rule_expr(stream, result_peg_170.getPosition());
            if (result_peg_170.error()){
                goto out_peg_172;
            }
            exp = result_peg_170.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, negateExpression(exp));
                result_peg_170.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_170;
    stream.update(result_peg_170.getPosition());
    
    
    return result_peg_170;
    out_peg_172:
    Result result_peg_177(myposition);
    
    {
    
        result_peg_177 = rule_identifier(stream, result_peg_177.getPosition());
            if (result_peg_177.error()){
                goto out_peg_179;
            }
            name = result_peg_177.getValues();
        
        
        
        result_peg_177 = rule_s(stream, result_peg_177.getPosition());
            if (result_peg_177.error()){
                goto out_peg_179;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_177.getPosition()))){
                    result_peg_177.nextPosition();
                } else {
                    goto out_peg_179;
                }
            }
            result_peg_177.setValue((void*) "=");
        
        
        
        result_peg_177 = rule_s(stream, result_peg_177.getPosition());
            if (result_peg_177.error()){
                goto out_peg_179;
            }
        
        
        
        Result result_peg_184(result_peg_177.getPosition());
            result_peg_184 = rule_line_end(stream, result_peg_184.getPosition());
            if (result_peg_184.error()){
                goto out_peg_179;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_177.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_177;
    stream.update(result_peg_177.getPosition());
    
    
    return result_peg_177;
    out_peg_179:
    Result result_peg_185(myposition);
    
    {
    
        result_peg_185 = rule_identifier(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
            name = result_peg_185.getValues();
        
        
        
        result_peg_185 = rule_s(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_185.getPosition()))){
                    result_peg_185.nextPosition();
                } else {
                    goto out_peg_187;
                }
            }
            result_peg_185.setValue((void*) "(");
        
        
        
        result_peg_185 = rule_s(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
        
        
        
        result_peg_185 = rule_integer(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
            index = result_peg_185.getValues();
        
        
        
        result_peg_185 = rule_s(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_185.getPosition()))){
                    result_peg_185.nextPosition();
                } else {
                    goto out_peg_187;
                }
            }
            result_peg_185.setValue((void*) ")");
        
        
        
        result_peg_185 = rule_s(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_185.getPosition()))){
                    result_peg_185.nextPosition();
                } else {
                    goto out_peg_187;
                }
            }
            result_peg_185.setValue((void*) "=");
        
        
        
        result_peg_185 = rule_s(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
        
        
        
        result_peg_185 = rule_expr(stream, result_peg_185.getPosition());
            if (result_peg_185.error()){
                goto out_peg_187;
            }
            exp = result_peg_185.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_185.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_185;
    stream.update(result_peg_185.getPosition());
    
    
    return result_peg_185;
    out_peg_187:
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_199 = stream.getColumn(position);
    if (column_peg_199.chunk2 != 0 && column_peg_199.chunk2->chunk_identifier.calculated()){
        return column_peg_199.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_209(stream, "identifier");
    int myposition = position;
    
    Value line;
    Value first;
    Value rest;
    Result result_peg_200(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_202 = stream.getLineInfo(result_peg_200.getPosition());
            line = &line_info_peg_202;
        
        
        
        result_peg_200 = rule_name(stream, result_peg_200.getPosition());
            if (result_peg_200.error()){
                goto out_peg_204;
            }
            first = result_peg_200.getValues();
        
        
        
        result_peg_200.reset();
            do{
                Result result_peg_207(result_peg_200.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_207.getPosition()))){
                                result_peg_207.nextPosition();
                            } else {
                                goto loop_peg_206;
                            }
                        }
                        result_peg_207.setValue((void*) ".");
                    
                    
                    
                    result_peg_207 = rule_name(stream, result_peg_207.getPosition());
                        if (result_peg_207.error()){
                            goto loop_peg_206;
                        }
                    
                    
                }
                result_peg_200.addResult(result_peg_207);
            } while (true);
            loop_peg_206:
            ;
            rest = result_peg_200.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                result_peg_200.setValue(value);
            }
        
        
    }
    
    if (column_peg_199.chunk2 == 0){
        column_peg_199.chunk2 = new Chunk2();
    }
    column_peg_199.chunk2->chunk_identifier = result_peg_200;
    stream.update(result_peg_200.getPosition());
    
    
    return result_peg_200;
    out_peg_204:
    
    if (column_peg_199.chunk2 == 0){
        column_peg_199.chunk2 = new Chunk2();
    }
    column_peg_199.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_210 = stream.getColumn(position);
    if (column_peg_210.chunk2 != 0 && column_peg_210.chunk2->chunk_integer.calculated()){
        return column_peg_210.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_228(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_211(myposition);
    
    {
    
        int save_peg_213 = result_peg_211.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_211.getPosition()))){
                        result_peg_211.nextPosition();
                    } else {
                        goto out_peg_216;
                    }
                }
                result_peg_211.setValue((void*) "-");
                    
            }
            goto success_peg_214;
            out_peg_216:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_211.getPosition()))){
                        result_peg_211.nextPosition();
                    } else {
                        goto out_peg_218;
                    }
                }
                result_peg_211.setValue((void*) "+");
                    
            }
            goto success_peg_214;
            out_peg_218:
            
            result_peg_211 = Result(save_peg_213);
            result_peg_211.setValue((void*) 0);
            
            success_peg_214:
            ;
        
        Result result_peg_212 = result_peg_211;
        
        result_peg_211.reset();
            do{
                Result result_peg_221(result_peg_211.getPosition());
                {
                    
                    char letter_peg_226 = stream.get(result_peg_221.getPosition());
                    if (letter_peg_226 != '\0' && strchr("0123456789", letter_peg_226) != NULL){
                        result_peg_221.nextPosition();
                        result_peg_221.setValue((void*) (long) letter_peg_226);
                    } else {
                        goto out_peg_225;
                    }
                    
                }
                goto success_peg_222;
                out_peg_225:
                goto loop_peg_220;
                success_peg_222:
                ;
                result_peg_211.addResult(result_peg_221);
            } while (true);
            loop_peg_220:
            if (result_peg_211.matches() == 0){
                goto out_peg_227;
            }
        
        Result result_peg_219 = result_peg_211;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_212.getValues(), result_peg_219.getValues());
                result_peg_211.setValue(value);
            }
        
        
    }
    
    if (column_peg_210.chunk2 == 0){
        column_peg_210.chunk2 = new Chunk2();
    }
    column_peg_210.chunk2->chunk_integer = result_peg_211;
    stream.update(result_peg_211.getPosition());
    
    
    return result_peg_211;
    out_peg_227:
    
    if (column_peg_210.chunk2 == 0){
        column_peg_210.chunk2 = new Chunk2();
    }
    column_peg_210.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_229 = stream.getColumn(position);
    if (column_peg_229.chunk2 != 0 && column_peg_229.chunk2->chunk_float.calculated()){
        return column_peg_229.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_274(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_230(myposition);
    
    {
    
        int save_peg_232 = result_peg_230.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_230.getPosition()))){
                        result_peg_230.nextPosition();
                    } else {
                        goto out_peg_235;
                    }
                }
                result_peg_230.setValue((void*) "-");
                    
            }
            goto success_peg_233;
            out_peg_235:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_230.getPosition()))){
                        result_peg_230.nextPosition();
                    } else {
                        goto out_peg_237;
                    }
                }
                result_peg_230.setValue((void*) "+");
                    
            }
            goto success_peg_233;
            out_peg_237:
            
            result_peg_230 = Result(save_peg_232);
            result_peg_230.setValue((void*) 0);
            
            success_peg_233:
            ;
        
        Result result_peg_231 = result_peg_230;
        
        result_peg_230.reset();
            do{
                Result result_peg_240(result_peg_230.getPosition());
                {
                    
                    char letter_peg_245 = stream.get(result_peg_240.getPosition());
                    if (letter_peg_245 != '\0' && strchr("0123456789", letter_peg_245) != NULL){
                        result_peg_240.nextPosition();
                        result_peg_240.setValue((void*) (long) letter_peg_245);
                    } else {
                        goto out_peg_244;
                    }
                    
                }
                goto success_peg_241;
                out_peg_244:
                goto loop_peg_239;
                success_peg_241:
                ;
                result_peg_230.addResult(result_peg_240);
            } while (true);
            loop_peg_239:
            ;
            left = result_peg_230.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_230.getPosition()))){
                    result_peg_230.nextPosition();
                } else {
                    goto out_peg_247;
                }
            }
            result_peg_230.setValue((void*) ".");
        
        
        
        result_peg_230.reset();
            do{
                Result result_peg_250(result_peg_230.getPosition());
                {
                    
                    char letter_peg_255 = stream.get(result_peg_250.getPosition());
                    if (letter_peg_255 != '\0' && strchr("0123456789", letter_peg_255) != NULL){
                        result_peg_250.nextPosition();
                        result_peg_250.setValue((void*) (long) letter_peg_255);
                    } else {
                        goto out_peg_254;
                    }
                    
                }
                goto success_peg_251;
                out_peg_254:
                goto loop_peg_249;
                success_peg_251:
                ;
                result_peg_230.addResult(result_peg_250);
            } while (true);
            loop_peg_249:
            if (result_peg_230.matches() == 0){
                goto out_peg_247;
            }
            right = result_peg_230.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_231.getValues(), parseDouble(left,right));
                result_peg_230.setValue(value);
            }
        
        
    }
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_float = result_peg_230;
    stream.update(result_peg_230.getPosition());
    
    
    return result_peg_230;
    out_peg_247:
    Result result_peg_256(myposition);
    
    {
    
        int save_peg_258 = result_peg_256.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_256.getPosition()))){
                        result_peg_256.nextPosition();
                    } else {
                        goto out_peg_261;
                    }
                }
                result_peg_256.setValue((void*) "-");
                    
            }
            goto success_peg_259;
            out_peg_261:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_256.getPosition()))){
                        result_peg_256.nextPosition();
                    } else {
                        goto out_peg_263;
                    }
                }
                result_peg_256.setValue((void*) "+");
                    
            }
            goto success_peg_259;
            out_peg_263:
            
            result_peg_256 = Result(save_peg_258);
            result_peg_256.setValue((void*) 0);
            
            success_peg_259:
            ;
        
        Result result_peg_257 = result_peg_256;
        
        result_peg_256.reset();
            do{
                Result result_peg_266(result_peg_256.getPosition());
                {
                    
                    char letter_peg_271 = stream.get(result_peg_266.getPosition());
                    if (letter_peg_271 != '\0' && strchr("0123456789", letter_peg_271) != NULL){
                        result_peg_266.nextPosition();
                        result_peg_266.setValue((void*) (long) letter_peg_271);
                    } else {
                        goto out_peg_270;
                    }
                    
                }
                goto success_peg_267;
                out_peg_270:
                goto loop_peg_265;
                success_peg_267:
                ;
                result_peg_256.addResult(result_peg_266);
            } while (true);
            loop_peg_265:
            if (result_peg_256.matches() == 0){
                goto out_peg_272;
            }
            left = result_peg_256.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_256.getPosition()))){
                    result_peg_256.nextPosition();
                } else {
                    goto out_peg_272;
                }
            }
            result_peg_256.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_257.getValues(), parseDouble(left));
                result_peg_256.setValue(value);
            }
        
        
    }
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_float = result_peg_256;
    stream.update(result_peg_256.getPosition());
    
    
    return result_peg_256;
    out_peg_272:
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_275 = stream.getColumn(position);
    if (column_peg_275.chunk2 != 0 && column_peg_275.chunk2->chunk_string.calculated()){
        return column_peg_275.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_290(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_276(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_276.getPosition()))){
                    result_peg_276.nextPosition();
                } else {
                    goto out_peg_278;
                }
            }
            result_peg_276.setValue((void*) "\"");
        
        
        
        result_peg_276.reset();
            do{
                Result result_peg_281(result_peg_276.getPosition());
                {
                
                    Result result_peg_284(result_peg_281);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_284.getPosition()))){
                                result_peg_284.nextPosition();
                            } else {
                                goto not_peg_283;
                            }
                        }
                        result_peg_284.setValue((void*) "\"");
                        goto loop_peg_280;
                        not_peg_283:
                        result_peg_281.setValue((void*)0);
                    
                    
                    
                    Result result_peg_287(result_peg_281);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_287.getPosition()))){
                                result_peg_287.nextPosition();
                            } else {
                                goto not_peg_286;
                            }
                        }
                        result_peg_287.setValue((void*) "\n");
                        goto loop_peg_280;
                        not_peg_286:
                        result_peg_281.setValue((void*)0);
                    
                    
                    
                    char temp_peg_288 = stream.get(result_peg_281.getPosition());
                        if (temp_peg_288 != '\0'){
                            result_peg_281.setValue((void*) (long) temp_peg_288);
                            result_peg_281.nextPosition();
                        } else {
                            goto loop_peg_280;
                        }
                    
                    
                }
                result_peg_276.addResult(result_peg_281);
            } while (true);
            loop_peg_280:
            ;
            contents = result_peg_276.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_276.getPosition()))){
                    result_peg_276.nextPosition();
                } else {
                    goto out_peg_278;
                }
            }
            result_peg_276.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_276.setValue(value);
            }
        
        
    }
    
    if (column_peg_275.chunk2 == 0){
        column_peg_275.chunk2 = new Chunk2();
    }
    column_peg_275.chunk2->chunk_string = result_peg_276;
    stream.update(result_peg_276.getPosition());
    
    
    return result_peg_276;
    out_peg_278:
    
    if (column_peg_275.chunk2 == 0){
        column_peg_275.chunk2 = new Chunk2();
    }
    column_peg_275.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_291 = stream.getColumn(position);
    if (column_peg_291.chunk3 != 0 && column_peg_291.chunk3->chunk_range.calculated()){
        return column_peg_291.chunk3->chunk_range;
    }
    
    RuleTrace trace_peg_336(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_292(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_292.getPosition()))){
                    result_peg_292.nextPosition();
                } else {
                    goto out_peg_294;
                }
            }
            result_peg_292.setValue((void*) "[");
        
        
        
        result_peg_292 = rule_s(stream, result_peg_292.getPosition());
            if (result_peg_292.error()){
                goto out_peg_294;
            }
        
        
        
        result_peg_292 = rule_expr_c(stream, result_peg_292.getPosition());
            if (result_peg_292.error()){
                goto out_peg_294;
            }
            low = result_peg_292.getValues();
        
        
        
        result_peg_292 = rule_s(stream, result_peg_292.getPosition());
            if (result_peg_292.error()){
                goto out_peg_294;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_292.getPosition()))){
                    result_peg_292.nextPosition();
                } else {
                    goto out_peg_294;
                }
            }
            result_peg_292.setValue((void*) ",");
        
        
        
        result_peg_292 = rule_s(stream, result_peg_292.getPosition());
            if (result_peg_292.error()){
                goto out_peg_294;
            }
        
        
        
        result_peg_292 = rule_expr_c(stream, result_peg_292.getPosition());
            if (result_peg_292.error()){
                goto out_peg_294;
            }
            high = result_peg_292.getValues();
        
        
        
        result_peg_292 = rule_s(stream, result_peg_292.getPosition());
            if (result_peg_292.error()){
                goto out_peg_294;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_292.getPosition()))){
                    result_peg_292.nextPosition();
                } else {
                    goto out_peg_294;
                }
            }
            result_peg_292.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_292.setValue(value);
            }
        
        
    }
    
    if (column_peg_291.chunk3 == 0){
        column_peg_291.chunk3 = new Chunk3();
    }
    column_peg_291.chunk3->chunk_range = result_peg_292;
    stream.update(result_peg_292.getPosition());
    
    
    return result_peg_292;
    out_peg_294:
    Result result_peg_303(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_303.getPosition()))){
                    result_peg_303.nextPosition();
                } else {
                    goto out_peg_305;
                }
            }
            result_peg_303.setValue((void*) "(");
        
        
        
        result_peg_303 = rule_s(stream, result_peg_303.getPosition());
            if (result_peg_303.error()){
                goto out_peg_305;
            }
        
        
        
        result_peg_303 = rule_expr_c(stream, result_peg_303.getPosition());
            if (result_peg_303.error()){
                goto out_peg_305;
            }
            low = result_peg_303.getValues();
        
        
        
        result_peg_303 = rule_s(stream, result_peg_303.getPosition());
            if (result_peg_303.error()){
                goto out_peg_305;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_303.getPosition()))){
                    result_peg_303.nextPosition();
                } else {
                    goto out_peg_305;
                }
            }
            result_peg_303.setValue((void*) ",");
        
        
        
        result_peg_303 = rule_s(stream, result_peg_303.getPosition());
            if (result_peg_303.error()){
                goto out_peg_305;
            }
        
        
        
        result_peg_303 = rule_expr_c(stream, result_peg_303.getPosition());
            if (result_peg_303.error()){
                goto out_peg_305;
            }
            high = result_peg_303.getValues();
        
        
        
        result_peg_303 = rule_s(stream, result_peg_303.getPosition());
            if (result_peg_303.error()){
                goto out_peg_305;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_303.getPosition()))){
                    result_peg_303.nextPosition();
                } else {
                    goto out_peg_305;
                }
            }
            result_peg_303.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_303.setValue(value);
            }
        
        
    }
    
    if (column_peg_291.chunk3 == 0){
        column_peg_291.chunk3 = new Chunk3();
    }
    column_peg_291.chunk3->chunk_range = result_peg_303;
    stream.update(result_peg_303.getPosition());
    
    
    return result_peg_303;
    out_peg_305:
    Result result_peg_314(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_314.getPosition()))){
                    result_peg_314.nextPosition();
                } else {
                    goto out_peg_316;
                }
            }
            result_peg_314.setValue((void*) "(");
        
        
        
        result_peg_314 = rule_s(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
        
        
        
        result_peg_314 = rule_expr_c(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
            low = result_peg_314.getValues();
        
        
        
        result_peg_314 = rule_s(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_314.getPosition()))){
                    result_peg_314.nextPosition();
                } else {
                    goto out_peg_316;
                }
            }
            result_peg_314.setValue((void*) ",");
        
        
        
        result_peg_314 = rule_s(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
        
        
        
        result_peg_314 = rule_expr_c(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
            high = result_peg_314.getValues();
        
        
        
        result_peg_314 = rule_s(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_314.getPosition()))){
                    result_peg_314.nextPosition();
                } else {
                    goto out_peg_316;
                }
            }
            result_peg_314.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_314.setValue(value);
            }
        
        
    }
    
    if (column_peg_291.chunk3 == 0){
        column_peg_291.chunk3 = new Chunk3();
    }
    column_peg_291.chunk3->chunk_range = result_peg_314;
    stream.update(result_peg_314.getPosition());
    
    
    return result_peg_314;
    out_peg_316:
    Result result_peg_325(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_325.getPosition()))){
                    result_peg_325.nextPosition();
                } else {
                    goto out_peg_327;
                }
            }
            result_peg_325.setValue((void*) "[");
        
        
        
        result_peg_325 = rule_s(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
        
        
        
        result_peg_325 = rule_expr_c(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
            low = result_peg_325.getValues();
        
        
        
        result_peg_325 = rule_s(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_325.getPosition()))){
                    result_peg_325.nextPosition();
                } else {
                    goto out_peg_327;
                }
            }
            result_peg_325.setValue((void*) ",");
        
        
        
        result_peg_325 = rule_s(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
        
        
        
        result_peg_325 = rule_expr_c(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
            high = result_peg_325.getValues();
        
        
        
        result_peg_325 = rule_s(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_325.getPosition()))){
                    result_peg_325.nextPosition();
                } else {
                    goto out_peg_327;
                }
            }
            result_peg_325.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_325.setValue(value);
            }
        
        
    }
    
    if (column_peg_291.chunk3 == 0){
        column_peg_291.chunk3 = new Chunk3();
    }
    column_peg_291.chunk3->chunk_range = result_peg_325;
    stream.update(result_peg_325.getPosition());
    
    
    return result_peg_325;
    out_peg_327:
    
    if (column_peg_291.chunk3 == 0){
        column_peg_291.chunk3 = new Chunk3();
    }
    column_peg_291.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_337 = stream.getColumn(position);
    if (column_peg_337.chunk3 != 0 && column_peg_337.chunk3->chunk_name.calculated()){
        return column_peg_337.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_349(stream, "name");
    int myposition = position;
    
    
    Result result_peg_338(myposition);
    
    {
    
        {
                
                char letter_peg_344 = stream.get(result_peg_338.getPosition());
                if (letter_peg_344 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_344) != NULL){
                    result_peg_338.nextPosition();
                    result_peg_338.setValue((void*) (long) letter_peg_344);
                } else {
                    goto out_peg_343;
                }
                
            }
            goto success_peg_340;
            out_peg_343:
            goto out_peg_345;
            success_peg_340:
            ;
        
        Result result_peg_339 = result_peg_338;
        
        result_peg_338.reset();
            do{
                Result result_peg_348(result_peg_338.getPosition());
                result_peg_348 = rule_alpha_digit(stream, result_peg_348.getPosition());
                if (result_peg_348.error()){
                    goto loop_peg_347;
                }
                result_peg_338.addResult(result_peg_348);
            } while (true);
            loop_peg_347:
            ;
        
        Result result_peg_346 = result_peg_338;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_339.getValues().getValue(),result_peg_346.getValues());
                result_peg_338.setValue(value);
            }
        
        
    }
    
    if (column_peg_337.chunk3 == 0){
        column_peg_337.chunk3 = new Chunk3();
    }
    column_peg_337.chunk3->chunk_name = result_peg_338;
    stream.update(result_peg_338.getPosition());
    
    
    return result_peg_338;
    out_peg_345:
    
    if (column_peg_337.chunk3 == 0){
        column_peg_337.chunk3 = new Chunk3();
    }
    column_peg_337.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_350 = stream.getColumn(position);
    if (column_peg_350.chunk3 != 0 && column_peg_350.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_350.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_365(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_351(myposition);
    
    {
        
        char letter_peg_356 = stream.get(result_peg_351.getPosition());
        if (letter_peg_356 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_356) != NULL){
            result_peg_351.nextPosition();
            result_peg_351.setValue((void*) (long) letter_peg_356);
        } else {
            goto out_peg_355;
        }
        
    }
    goto success_peg_352;
    out_peg_355:
    goto out_peg_357;
    success_peg_352:
    ;
    
    if (column_peg_350.chunk3 == 0){
        column_peg_350.chunk3 = new Chunk3();
    }
    column_peg_350.chunk3->chunk_alpha_digit = result_peg_351;
    stream.update(result_peg_351.getPosition());
    
    
    return result_peg_351;
    out_peg_357:
    Result result_peg_358(myposition);
    
    {
        
        char letter_peg_363 = stream.get(result_peg_358.getPosition());
        if (letter_peg_363 != '\0' && strchr("0123456789", letter_peg_363) != NULL){
            result_peg_358.nextPosition();
            result_peg_358.setValue((void*) (long) letter_peg_363);
        } else {
            goto out_peg_362;
        }
        
    }
    goto success_peg_359;
    out_peg_362:
    goto out_peg_364;
    success_peg_359:
    ;
    
    if (column_peg_350.chunk3 == 0){
        column_peg_350.chunk3 = new Chunk3();
    }
    column_peg_350.chunk3->chunk_alpha_digit = result_peg_358;
    stream.update(result_peg_358.getPosition());
    
    
    return result_peg_358;
    out_peg_364:
    
    if (column_peg_350.chunk3 == 0){
        column_peg_350.chunk3 = new Chunk3();
    }
    column_peg_350.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_366 = stream.getColumn(position);
    if (column_peg_366.chunk3 != 0 && column_peg_366.chunk3->chunk_valuelist.calculated()){
        return column_peg_366.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_388(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_367(myposition);
    
    {
    
        result_peg_367 = rule_expr_c(stream, result_peg_367.getPosition());
            if (result_peg_367.error()){
                goto out_peg_369;
            }
            first = result_peg_367.getValues();
        
        
        
        result_peg_367.reset();
            do{
                Result result_peg_372(result_peg_367.getPosition());
                {
                
                    result_peg_372 = rule_s(stream, result_peg_372.getPosition());
                        if (result_peg_372.error()){
                            goto loop_peg_371;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_372.getPosition()))){
                                result_peg_372.nextPosition();
                            } else {
                                goto loop_peg_371;
                            }
                        }
                        result_peg_372.setValue((void*) ",");
                    
                    
                    
                    result_peg_372 = rule_s(stream, result_peg_372.getPosition());
                        if (result_peg_372.error()){
                            goto loop_peg_371;
                        }
                    
                    
                    
                    result_peg_372 = rule_expr_c(stream, result_peg_372.getPosition());
                        if (result_peg_372.error()){
                            goto loop_peg_371;
                        }
                    
                    
                }
                result_peg_367.addResult(result_peg_372);
            } while (true);
            loop_peg_371:
            if (result_peg_367.matches() == 0){
                goto out_peg_369;
            }
            rest = result_peg_367.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_367.setValue(value);
            }
        
        
    }
    
    if (column_peg_366.chunk3 == 0){
        column_peg_366.chunk3 = new Chunk3();
    }
    column_peg_366.chunk3->chunk_valuelist = result_peg_367;
    stream.update(result_peg_367.getPosition());
    
    
    return result_peg_367;
    out_peg_369:
    Result result_peg_376(myposition);
    
    {
    
        result_peg_376 = rule_s(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_378;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_376.getPosition()))){
                    result_peg_376.nextPosition();
                } else {
                    goto out_peg_378;
                }
            }
            result_peg_376.setValue((void*) ",");
        
        
        
        result_peg_376 = rule_s(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_378;
            }
        
        
        
        result_peg_376 = rule_expr_c(stream, result_peg_376.getPosition());
            if (result_peg_376.error()){
                goto out_peg_378;
            }
            first = result_peg_376.getValues();
        
        
        
        result_peg_376.reset();
            do{
                Result result_peg_384(result_peg_376.getPosition());
                {
                
                    result_peg_384 = rule_s(stream, result_peg_384.getPosition());
                        if (result_peg_384.error()){
                            goto loop_peg_383;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_384.getPosition()))){
                                result_peg_384.nextPosition();
                            } else {
                                goto loop_peg_383;
                            }
                        }
                        result_peg_384.setValue((void*) ",");
                    
                    
                    
                    result_peg_384 = rule_s(stream, result_peg_384.getPosition());
                        if (result_peg_384.error()){
                            goto loop_peg_383;
                        }
                    
                    
                    
                    result_peg_384 = rule_expr_c(stream, result_peg_384.getPosition());
                        if (result_peg_384.error()){
                            goto loop_peg_383;
                        }
                    
                    
                }
                result_peg_376.addResult(result_peg_384);
            } while (true);
            loop_peg_383:
            ;
            rest = result_peg_376.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_376.setValue(value);
            }
        
        
    }
    
    if (column_peg_366.chunk3 == 0){
        column_peg_366.chunk3 = new Chunk3();
    }
    column_peg_366.chunk3->chunk_valuelist = result_peg_376;
    stream.update(result_peg_376.getPosition());
    
    
    return result_peg_376;
    out_peg_378:
    
    if (column_peg_366.chunk3 == 0){
        column_peg_366.chunk3 = new Chunk3();
    }
    column_peg_366.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_389 = stream.getColumn(position);
    if (column_peg_389.chunk3 != 0 && column_peg_389.chunk3->chunk_expr.calculated()){
        return column_peg_389.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_400(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_390(myposition);
    
    {
    
        result_peg_390 = rule_expr_c(stream, result_peg_390.getPosition());
            if (result_peg_390.error()){
                goto out_peg_392;
            }
        
        Result result_peg_391 = result_peg_390;
        
        Result result_peg_395(result_peg_390);
            {
            
                result_peg_395 = rule_s(stream, result_peg_395.getPosition());
                    if (result_peg_395.error()){
                        goto not_peg_394;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_395.getPosition()))){
                            result_peg_395.nextPosition();
                        } else {
                            goto not_peg_394;
                        }
                    }
                    result_peg_395.setValue((void*) ",");
                
                
            }
            goto out_peg_392;
            not_peg_394:
            result_peg_390.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_391.getValues();
                result_peg_390.setValue(value);
            }
        
        
    }
    
    if (column_peg_389.chunk3 == 0){
        column_peg_389.chunk3 = new Chunk3();
    }
    column_peg_389.chunk3->chunk_expr = result_peg_390;
    stream.update(result_peg_390.getPosition());
    
    
    return result_peg_390;
    out_peg_392:
    Result result_peg_397(myposition);
    
    {
    
        result_peg_397 = rule_valuelist(stream, result_peg_397.getPosition());
            if (result_peg_397.error()){
                goto out_peg_399;
            }
        
        Result result_peg_398 = result_peg_397;
        
        result_peg_397 = rule_expr2_rest(stream, result_peg_397.getPosition(), result_peg_398.getValues());
            if (result_peg_397.error()){
                goto out_peg_399;
            }
        
        
    }
    
    if (column_peg_389.chunk3 == 0){
        column_peg_389.chunk3 = new Chunk3();
    }
    column_peg_389.chunk3->chunk_expr = result_peg_397;
    stream.update(result_peg_397.getPosition());
    
    
    return result_peg_397;
    out_peg_399:
    
    if (column_peg_389.chunk3 == 0){
        column_peg_389.chunk3 = new Chunk3();
    }
    column_peg_389.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_401 = stream.getColumn(position);
    if (column_peg_401.chunk4 != 0 && column_peg_401.chunk4->chunk_expr_c.calculated()){
        return column_peg_401.chunk4->chunk_expr_c;
    }
    
    RuleTrace trace_peg_405(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_402(myposition);
    
    {
    
        result_peg_402 = rule_expr2(stream, result_peg_402.getPosition());
            if (result_peg_402.error()){
                goto out_peg_404;
            }
            left = result_peg_402.getValues();
        
        
        
        result_peg_402 = rule_expr_rest(stream, result_peg_402.getPosition(), left);
            if (result_peg_402.error()){
                goto out_peg_404;
            }
        
        
    }
    
    if (column_peg_401.chunk4 == 0){
        column_peg_401.chunk4 = new Chunk4();
    }
    column_peg_401.chunk4->chunk_expr_c = result_peg_402;
    stream.update(result_peg_402.getPosition());
    
    
    return result_peg_402;
    out_peg_404:
    
    if (column_peg_401.chunk4 == 0){
        column_peg_401.chunk4 = new Chunk4();
    }
    column_peg_401.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_406 = stream.getColumn(position);
    if (column_peg_406.chunk4 != 0 && column_peg_406.chunk4->chunk_expr2.calculated()){
        return column_peg_406.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_410(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_407(myposition);
    
    {
    
        result_peg_407 = rule_expr3(stream, result_peg_407.getPosition());
            if (result_peg_407.error()){
                goto out_peg_409;
            }
            left = result_peg_407.getValues();
        
        
        
        result_peg_407 = rule_expr2_rest(stream, result_peg_407.getPosition(), left);
            if (result_peg_407.error()){
                goto out_peg_409;
            }
        
        
    }
    
    if (column_peg_406.chunk4 == 0){
        column_peg_406.chunk4 = new Chunk4();
    }
    column_peg_406.chunk4->chunk_expr2 = result_peg_407;
    stream.update(result_peg_407.getPosition());
    
    
    return result_peg_407;
    out_peg_409:
    
    if (column_peg_406.chunk4 == 0){
        column_peg_406.chunk4 = new Chunk4();
    }
    column_peg_406.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_411 = stream.getColumn(position);
    if (column_peg_411.chunk4 != 0 && column_peg_411.chunk4->chunk_expr3.calculated()){
        return column_peg_411.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_415(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_412(myposition);
    
    {
    
        result_peg_412 = rule_expr4(stream, result_peg_412.getPosition());
            if (result_peg_412.error()){
                goto out_peg_414;
            }
            left = result_peg_412.getValues();
        
        
        
        result_peg_412 = rule_expr3_rest(stream, result_peg_412.getPosition(), left);
            if (result_peg_412.error()){
                goto out_peg_414;
            }
        
        
    }
    
    if (column_peg_411.chunk4 == 0){
        column_peg_411.chunk4 = new Chunk4();
    }
    column_peg_411.chunk4->chunk_expr3 = result_peg_412;
    stream.update(result_peg_412.getPosition());
    
    
    return result_peg_412;
    out_peg_414:
    
    if (column_peg_411.chunk4 == 0){
        column_peg_411.chunk4 = new Chunk4();
    }
    column_peg_411.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_416 = stream.getColumn(position);
    if (column_peg_416.chunk4 != 0 && column_peg_416.chunk4->chunk_expr4.calculated()){
        return column_peg_416.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_420(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_417(myposition);
    
    {
    
        result_peg_417 = rule_expr5(stream, result_peg_417.getPosition());
            if (result_peg_417.error()){
                goto out_peg_419;
            }
            left = result_peg_417.getValues();
        
        
        
        result_peg_417 = rule_expr4_rest(stream, result_peg_417.getPosition(), left);
            if (result_peg_417.error()){
                goto out_peg_419;
            }
        
        
    }
    
    if (column_peg_416.chunk4 == 0){
        column_peg_416.chunk4 = new Chunk4();
    }
    column_peg_416.chunk4->chunk_expr4 = result_peg_417;
    stream.update(result_peg_417.getPosition());
    
    
    return result_peg_417;
    out_peg_419:
    
    if (column_peg_416.chunk4 == 0){
        column_peg_416.chunk4 = new Chunk4();
    }
    column_peg_416.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_421 = stream.getColumn(position);
    if (column_peg_421.chunk4 != 0 && column_peg_421.chunk4->chunk_expr5.calculated()){
        return column_peg_421.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_425(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_422(myposition);
    
    {
    
        result_peg_422 = rule_expr6(stream, result_peg_422.getPosition());
            if (result_peg_422.error()){
                goto out_peg_424;
            }
            left = result_peg_422.getValues();
        
        
        
        result_peg_422 = rule_expr5_rest(stream, result_peg_422.getPosition(), left);
            if (result_peg_422.error()){
                goto out_peg_424;
            }
        
        
    }
    
    if (column_peg_421.chunk4 == 0){
        column_peg_421.chunk4 = new Chunk4();
    }
    column_peg_421.chunk4->chunk_expr5 = result_peg_422;
    stream.update(result_peg_422.getPosition());
    
    
    return result_peg_422;
    out_peg_424:
    
    if (column_peg_421.chunk4 == 0){
        column_peg_421.chunk4 = new Chunk4();
    }
    column_peg_421.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_426 = stream.getColumn(position);
    if (column_peg_426.chunk5 != 0 && column_peg_426.chunk5->chunk_expr6.calculated()){
        return column_peg_426.chunk5->chunk_expr6;
    }
    
    RuleTrace trace_peg_430(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_427(myposition);
    
    {
    
        result_peg_427 = rule_expr7(stream, result_peg_427.getPosition());
            if (result_peg_427.error()){
                goto out_peg_429;
            }
            left = result_peg_427.getValues();
        
        
        
        result_peg_427 = rule_expr6_rest(stream, result_peg_427.getPosition(), left);
            if (result_peg_427.error()){
                goto out_peg_429;
            }
        
        
    }
    
    if (column_peg_426.chunk5 == 0){
        column_peg_426.chunk5 = new Chunk5();
    }
    column_peg_426.chunk5->chunk_expr6 = result_peg_427;
    stream.update(result_peg_427.getPosition());
    
    
    return result_peg_427;
    out_peg_429:
    
    if (column_peg_426.chunk5 == 0){
        column_peg_426.chunk5 = new Chunk5();
    }
    column_peg_426.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_431 = stream.getColumn(position);
    if (column_peg_431.chunk5 != 0 && column_peg_431.chunk5->chunk_expr7.calculated()){
        return column_peg_431.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_435(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_432(myposition);
    
    {
    
        result_peg_432 = rule_expr8(stream, result_peg_432.getPosition());
            if (result_peg_432.error()){
                goto out_peg_434;
            }
            left = result_peg_432.getValues();
        
        
        
        result_peg_432 = rule_expr7_rest(stream, result_peg_432.getPosition(), left);
            if (result_peg_432.error()){
                goto out_peg_434;
            }
        
        
    }
    
    if (column_peg_431.chunk5 == 0){
        column_peg_431.chunk5 = new Chunk5();
    }
    column_peg_431.chunk5->chunk_expr7 = result_peg_432;
    stream.update(result_peg_432.getPosition());
    
    
    return result_peg_432;
    out_peg_434:
    
    if (column_peg_431.chunk5 == 0){
        column_peg_431.chunk5 = new Chunk5();
    }
    column_peg_431.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_436 = stream.getColumn(position);
    if (column_peg_436.chunk5 != 0 && column_peg_436.chunk5->chunk_expr8.calculated()){
        return column_peg_436.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_440(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_437(myposition);
    
    {
    
        result_peg_437 = rule_expr9(stream, result_peg_437.getPosition());
            if (result_peg_437.error()){
                goto out_peg_439;
            }
            left = result_peg_437.getValues();
        
        
        
        result_peg_437 = rule_expr8_rest(stream, result_peg_437.getPosition(), left);
            if (result_peg_437.error()){
                goto out_peg_439;
            }
        
        
    }
    
    if (column_peg_436.chunk5 == 0){
        column_peg_436.chunk5 = new Chunk5();
    }
    column_peg_436.chunk5->chunk_expr8 = result_peg_437;
    stream.update(result_peg_437.getPosition());
    
    
    return result_peg_437;
    out_peg_439:
    
    if (column_peg_436.chunk5 == 0){
        column_peg_436.chunk5 = new Chunk5();
    }
    column_peg_436.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_441 = stream.getColumn(position);
    if (column_peg_441.chunk5 != 0 && column_peg_441.chunk5->chunk_expr9.calculated()){
        return column_peg_441.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_445(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_442(myposition);
    
    {
    
        result_peg_442 = rule_expr10(stream, result_peg_442.getPosition());
            if (result_peg_442.error()){
                goto out_peg_444;
            }
            left = result_peg_442.getValues();
        
        
        
        result_peg_442 = rule_expr9_rest(stream, result_peg_442.getPosition(), left);
            if (result_peg_442.error()){
                goto out_peg_444;
            }
        
        
    }
    
    if (column_peg_441.chunk5 == 0){
        column_peg_441.chunk5 = new Chunk5();
    }
    column_peg_441.chunk5->chunk_expr9 = result_peg_442;
    stream.update(result_peg_442.getPosition());
    
    
    return result_peg_442;
    out_peg_444:
    
    if (column_peg_441.chunk5 == 0){
        column_peg_441.chunk5 = new Chunk5();
    }
    column_peg_441.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_446 = stream.getColumn(position);
    if (column_peg_446.chunk5 != 0 && column_peg_446.chunk5->chunk_expr10.calculated()){
        return column_peg_446.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_450(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_447(myposition);
    
    {
    
        result_peg_447 = rule_expr11(stream, result_peg_447.getPosition());
            if (result_peg_447.error()){
                goto out_peg_449;
            }
            left = result_peg_447.getValues();
        
        
        
        result_peg_447 = rule_expr10_rest(stream, result_peg_447.getPosition(), left);
            if (result_peg_447.error()){
                goto out_peg_449;
            }
        
        
    }
    
    if (column_peg_446.chunk5 == 0){
        column_peg_446.chunk5 = new Chunk5();
    }
    column_peg_446.chunk5->chunk_expr10 = result_peg_447;
    stream.update(result_peg_447.getPosition());
    
    
    return result_peg_447;
    out_peg_449:
    
    if (column_peg_446.chunk5 == 0){
        column_peg_446.chunk5 = new Chunk5();
    }
    column_peg_446.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_451 = stream.getColumn(position);
    if (column_peg_451.chunk6 != 0 && column_peg_451.chunk6->chunk_expr11.calculated()){
        return column_peg_451.chunk6->chunk_expr11;
    }
    
    RuleTrace trace_peg_455(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_452(myposition);
    
    {
    
        result_peg_452 = rule_expr12(stream, result_peg_452.getPosition());
            if (result_peg_452.error()){
                goto out_peg_454;
            }
            left = result_peg_452.getValues();
        
        
        
        result_peg_452 = rule_expr11_rest(stream, result_peg_452.getPosition(), left);
            if (result_peg_452.error()){
                goto out_peg_454;
            }
        
        
    }
    
    if (column_peg_451.chunk6 == 0){
        column_peg_451.chunk6 = new Chunk6();
    }
    column_peg_451.chunk6->chunk_expr11 = result_peg_452;
    stream.update(result_peg_452.getPosition());
    
    
    return result_peg_452;
    out_peg_454:
    
    if (column_peg_451.chunk6 == 0){
        column_peg_451.chunk6 = new Chunk6();
    }
    column_peg_451.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_456 = stream.getColumn(position);
    if (column_peg_456.chunk6 != 0 && column_peg_456.chunk6->chunk_expr12.calculated()){
        return column_peg_456.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_460(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_457(myposition);
    
    {
    
        result_peg_457 = rule_expr13(stream, result_peg_457.getPosition());
            if (result_peg_457.error()){
                goto out_peg_459;
            }
            left = result_peg_457.getValues();
        
        
        
        result_peg_457 = rule_expr12_rest(stream, result_peg_457.getPosition(), left);
            if (result_peg_457.error()){
                goto out_peg_459;
            }
        
        
    }
    
    if (column_peg_456.chunk6 == 0){
        column_peg_456.chunk6 = new Chunk6();
    }
    column_peg_456.chunk6->chunk_expr12 = result_peg_457;
    stream.update(result_peg_457.getPosition());
    
    
    return result_peg_457;
    out_peg_459:
    
    if (column_peg_456.chunk6 == 0){
        column_peg_456.chunk6 = new Chunk6();
    }
    column_peg_456.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_461 = stream.getColumn(position);
    if (column_peg_461.chunk6 != 0 && column_peg_461.chunk6->chunk_expr13.calculated()){
        return column_peg_461.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_468(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_462(myposition);
    
    {
    
        result_peg_462.reset();
            do{
                Result result_peg_465(result_peg_462.getPosition());
                result_peg_465 = rule_unary(stream, result_peg_465.getPosition());
                if (result_peg_465.error()){
                    goto loop_peg_464;
                }
                result_peg_462.addResult(result_peg_465);
            } while (true);
            loop_peg_464:
            ;
        
        Result result_peg_463 = result_peg_462;
        
        result_peg_462 = rule_expr13_real(stream, result_peg_462.getPosition());
            if (result_peg_462.error()){
                goto out_peg_467;
            }
            exp = result_peg_462.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_463.getValues(), exp);
                result_peg_462.setValue(value);
            }
        
        
    }
    
    if (column_peg_461.chunk6 == 0){
        column_peg_461.chunk6 = new Chunk6();
    }
    column_peg_461.chunk6->chunk_expr13 = result_peg_462;
    stream.update(result_peg_462.getPosition());
    
    
    return result_peg_462;
    out_peg_467:
    
    if (column_peg_461.chunk6 == 0){
        column_peg_461.chunk6 = new Chunk6();
    }
    column_peg_461.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_480(stream, "expr_rest");
    int myposition = position;
    tail_peg_471:
    Value right;
    Value new_left;
    Result result_peg_470(myposition);
    {
    
        result_peg_470 = rule_s(stream, result_peg_470.getPosition());
            if (result_peg_470.error()){
                goto out_peg_473;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_470.getPosition()))){
                    result_peg_470.nextPosition();
                } else {
                    goto out_peg_473;
                }
            }
            result_peg_470.setValue((void*) "||");
        
        
        
        result_peg_470 = rule_s(stream, result_peg_470.getPosition());
            if (result_peg_470.error()){
                goto out_peg_473;
            }
        
        
        
        result_peg_470 = rule_expr2(stream, result_peg_470.getPosition());
            if (result_peg_470.error()){
                goto out_peg_473;
            }
            right = result_peg_470.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_470.setValue(value);
            }
            new_left = result_peg_470.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_470.getPosition();
    goto tail_peg_471;
    out_peg_473:
    Result result_peg_478(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_478.setValue(value);
            }
        
        
    }
    stream.update(result_peg_478.getPosition());
    
    return result_peg_478;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_492(stream, "expr2_rest");
    int myposition = position;
    tail_peg_483:
    Value right;
    Value new_left;
    Result result_peg_482(myposition);
    {
    
        result_peg_482 = rule_s(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_482.getPosition()))){
                    result_peg_482.nextPosition();
                } else {
                    goto out_peg_485;
                }
            }
            result_peg_482.setValue((void*) "^^");
        
        
        
        result_peg_482 = rule_s(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
        
        
        
        result_peg_482 = rule_expr3(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
            right = result_peg_482.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_482.setValue(value);
            }
            new_left = result_peg_482.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_482.getPosition();
    goto tail_peg_483;
    out_peg_485:
    Result result_peg_490(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_490.setValue(value);
            }
        
        
    }
    stream.update(result_peg_490.getPosition());
    
    return result_peg_490;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_504(stream, "expr3_rest");
    int myposition = position;
    tail_peg_495:
    Value right;
    Value new_left;
    Result result_peg_494(myposition);
    {
    
        result_peg_494 = rule_s(stream, result_peg_494.getPosition());
            if (result_peg_494.error()){
                goto out_peg_497;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_494.getPosition()))){
                    result_peg_494.nextPosition();
                } else {
                    goto out_peg_497;
                }
            }
            result_peg_494.setValue((void*) "&&");
        
        
        
        result_peg_494 = rule_s(stream, result_peg_494.getPosition());
            if (result_peg_494.error()){
                goto out_peg_497;
            }
        
        
        
        result_peg_494 = rule_expr4(stream, result_peg_494.getPosition());
            if (result_peg_494.error()){
                goto out_peg_497;
            }
            right = result_peg_494.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_494.setValue(value);
            }
            new_left = result_peg_494.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_494.getPosition();
    goto tail_peg_495;
    out_peg_497:
    Result result_peg_502(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_502.setValue(value);
            }
        
        
    }
    stream.update(result_peg_502.getPosition());
    
    return result_peg_502;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_516(stream, "expr4_rest");
    int myposition = position;
    tail_peg_507:
    Value right;
    Value new_left;
    Result result_peg_506(myposition);
    {
    
        result_peg_506 = rule_s(stream, result_peg_506.getPosition());
            if (result_peg_506.error()){
                goto out_peg_509;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_506.getPosition()))){
                    result_peg_506.nextPosition();
                } else {
                    goto out_peg_509;
                }
            }
            result_peg_506.setValue((void*) "|");
        
        
        
        result_peg_506 = rule_s(stream, result_peg_506.getPosition());
            if (result_peg_506.error()){
                goto out_peg_509;
            }
        
        
        
        result_peg_506 = rule_expr5(stream, result_peg_506.getPosition());
            if (result_peg_506.error()){
                goto out_peg_509;
            }
            right = result_peg_506.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_506.setValue(value);
            }
            new_left = result_peg_506.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_506.getPosition();
    goto tail_peg_507;
    out_peg_509:
    Result result_peg_514(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_514.setValue(value);
            }
        
        
    }
    stream.update(result_peg_514.getPosition());
    
    return result_peg_514;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_528(stream, "expr5_rest");
    int myposition = position;
    tail_peg_519:
    Value right;
    Value new_left;
    Result result_peg_518(myposition);
    {
    
        result_peg_518 = rule_s(stream, result_peg_518.getPosition());
            if (result_peg_518.error()){
                goto out_peg_521;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_518.getPosition()))){
                    result_peg_518.nextPosition();
                } else {
                    goto out_peg_521;
                }
            }
            result_peg_518.setValue((void*) "^");
        
        
        
        result_peg_518 = rule_s(stream, result_peg_518.getPosition());
            if (result_peg_518.error()){
                goto out_peg_521;
            }
        
        
        
        result_peg_518 = rule_expr6(stream, result_peg_518.getPosition());
            if (result_peg_518.error()){
                goto out_peg_521;
            }
            right = result_peg_518.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_518.setValue(value);
            }
            new_left = result_peg_518.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_518.getPosition();
    goto tail_peg_519;
    out_peg_521:
    Result result_peg_526(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_526.setValue(value);
            }
        
        
    }
    stream.update(result_peg_526.getPosition());
    
    return result_peg_526;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_540(stream, "expr6_rest");
    int myposition = position;
    tail_peg_531:
    Value right;
    Value new_left;
    Result result_peg_530(myposition);
    {
    
        result_peg_530 = rule_s(stream, result_peg_530.getPosition());
            if (result_peg_530.error()){
                goto out_peg_533;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_530.getPosition()))){
                    result_peg_530.nextPosition();
                } else {
                    goto out_peg_533;
                }
            }
            result_peg_530.setValue((void*) "&");
        
        
        
        result_peg_530 = rule_s(stream, result_peg_530.getPosition());
            if (result_peg_530.error()){
                goto out_peg_533;
            }
        
        
        
        result_peg_530 = rule_expr7(stream, result_peg_530.getPosition());
            if (result_peg_530.error()){
                goto out_peg_533;
            }
            right = result_peg_530.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_530.setValue(value);
            }
            new_left = result_peg_530.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_530.getPosition();
    goto tail_peg_531;
    out_peg_533:
    Result result_peg_538(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_538.setValue(value);
            }
        
        
    }
    stream.update(result_peg_538.getPosition());
    
    return result_peg_538;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_552(stream, "expr7_rest");
    int myposition = position;
    tail_peg_543:
    Value right;
    Value new_left;
    Result result_peg_542(myposition);
    {
    
        result_peg_542 = rule_s(stream, result_peg_542.getPosition());
            if (result_peg_542.error()){
                goto out_peg_545;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_542.getPosition()))){
                    result_peg_542.nextPosition();
                } else {
                    goto out_peg_545;
                }
            }
            result_peg_542.setValue((void*) ":=");
        
        
        
        result_peg_542 = rule_s(stream, result_peg_542.getPosition());
            if (result_peg_542.error()){
                goto out_peg_545;
            }
        
        
        
        result_peg_542 = rule_expr8(stream, result_peg_542.getPosition());
            if (result_peg_542.error()){
                goto out_peg_545;
            }
            right = result_peg_542.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_542.setValue(value);
            }
            new_left = result_peg_542.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_542.getPosition();
    goto tail_peg_543;
    out_peg_545:
    Result result_peg_550(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_550.setValue(value);
            }
        
        
    }
    stream.update(result_peg_550.getPosition());
    
    return result_peg_550;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_571(stream, "expr8_rest");
    int myposition = position;
    tail_peg_555:
    Value right;
    Value new_left;
    Result result_peg_554(myposition);
    {
    
        result_peg_554 = rule_s(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_554.getPosition()))){
                    result_peg_554.nextPosition();
                } else {
                    goto out_peg_557;
                }
            }
            result_peg_554.setValue((void*) "=");
        
        
        
        result_peg_554 = rule_s(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
        
        
        
        result_peg_554 = rule_expr9(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
            right = result_peg_554.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_554.setValue(value);
            }
            new_left = result_peg_554.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_554.getPosition();
    goto tail_peg_555;
    out_peg_557:
    Result result_peg_562(myposition);
    {
    
        result_peg_562 = rule_s(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_564;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_562.getPosition()))){
                    result_peg_562.nextPosition();
                } else {
                    goto out_peg_564;
                }
            }
            result_peg_562.setValue((void*) "!=");
        
        
        
        result_peg_562 = rule_s(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_564;
            }
        
        
        
        result_peg_562 = rule_expr9(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_564;
            }
            right = result_peg_562.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_562.setValue(value);
            }
            new_left = result_peg_562.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_562.getPosition();
    goto tail_peg_555;
    out_peg_564:
    Result result_peg_569(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_569.setValue(value);
            }
        
        
    }
    stream.update(result_peg_569.getPosition());
    
    return result_peg_569;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_583(stream, "expr9_rest");
    int myposition = position;
    tail_peg_574:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_573(myposition);
    {
    
        result_peg_573 = rule_s(stream, result_peg_573.getPosition());
            if (result_peg_573.error()){
                goto out_peg_576;
            }
        
        
        
        result_peg_573 = rule_compare(stream, result_peg_573.getPosition());
            if (result_peg_573.error()){
                goto out_peg_576;
            }
            maker = result_peg_573.getValues();
        
        
        
        result_peg_573 = rule_s(stream, result_peg_573.getPosition());
            if (result_peg_573.error()){
                goto out_peg_576;
            }
        
        
        
        result_peg_573 = rule_expr10(stream, result_peg_573.getPosition());
            if (result_peg_573.error()){
                goto out_peg_576;
            }
            right = result_peg_573.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_573.setValue(value);
            }
            new_left = result_peg_573.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_573.getPosition();
    goto tail_peg_574;
    out_peg_576:
    Result result_peg_581(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_581.setValue(value);
            }
        
        
    }
    stream.update(result_peg_581.getPosition());
    
    return result_peg_581;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_584 = stream.getColumn(position);
    if (column_peg_584.chunk8 != 0 && column_peg_584.chunk8->chunk_compare.calculated()){
        return column_peg_584.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_597(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_585(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_585.getPosition()))){
                    result_peg_585.nextPosition();
                } else {
                    goto out_peg_587;
                }
            }
            result_peg_585.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_585.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk8 == 0){
        column_peg_584.chunk8 = new Chunk8();
    }
    column_peg_584.chunk8->chunk_compare = result_peg_585;
    stream.update(result_peg_585.getPosition());
    
    
    return result_peg_585;
    out_peg_587:
    Result result_peg_588(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_588.getPosition()))){
                    result_peg_588.nextPosition();
                } else {
                    goto out_peg_590;
                }
            }
            result_peg_588.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_588.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk8 == 0){
        column_peg_584.chunk8 = new Chunk8();
    }
    column_peg_584.chunk8->chunk_compare = result_peg_588;
    stream.update(result_peg_588.getPosition());
    
    
    return result_peg_588;
    out_peg_590:
    Result result_peg_591(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_591.getPosition()))){
                    result_peg_591.nextPosition();
                } else {
                    goto out_peg_593;
                }
            }
            result_peg_591.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_591.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk8 == 0){
        column_peg_584.chunk8 = new Chunk8();
    }
    column_peg_584.chunk8->chunk_compare = result_peg_591;
    stream.update(result_peg_591.getPosition());
    
    
    return result_peg_591;
    out_peg_593:
    Result result_peg_594(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_594.getPosition()))){
                    result_peg_594.nextPosition();
                } else {
                    goto out_peg_596;
                }
            }
            result_peg_594.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_594.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk8 == 0){
        column_peg_584.chunk8 = new Chunk8();
    }
    column_peg_584.chunk8->chunk_compare = result_peg_594;
    stream.update(result_peg_594.getPosition());
    
    
    return result_peg_594;
    out_peg_596:
    
    if (column_peg_584.chunk8 == 0){
        column_peg_584.chunk8 = new Chunk8();
    }
    column_peg_584.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_598 = stream.getColumn(position);
    if (column_peg_598.chunk8 != 0 && column_peg_598.chunk8->chunk_all_compare.calculated()){
        return column_peg_598.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_605(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_599(myposition);
    
    result_peg_599 = rule_compare(stream, result_peg_599.getPosition());
    if (result_peg_599.error()){
        goto out_peg_600;
    }
    
    if (column_peg_598.chunk8 == 0){
        column_peg_598.chunk8 = new Chunk8();
    }
    column_peg_598.chunk8->chunk_all_compare = result_peg_599;
    stream.update(result_peg_599.getPosition());
    
    
    return result_peg_599;
    out_peg_600:
    Result result_peg_601(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_601.getPosition()))){
            result_peg_601.nextPosition();
        } else {
            goto out_peg_602;
        }
    }
    result_peg_601.setValue((void*) "=");
    
    if (column_peg_598.chunk8 == 0){
        column_peg_598.chunk8 = new Chunk8();
    }
    column_peg_598.chunk8->chunk_all_compare = result_peg_601;
    stream.update(result_peg_601.getPosition());
    
    
    return result_peg_601;
    out_peg_602:
    Result result_peg_603(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_603.getPosition()))){
            result_peg_603.nextPosition();
        } else {
            goto out_peg_604;
        }
    }
    result_peg_603.setValue((void*) "!=");
    
    if (column_peg_598.chunk8 == 0){
        column_peg_598.chunk8 = new Chunk8();
    }
    column_peg_598.chunk8->chunk_all_compare = result_peg_603;
    stream.update(result_peg_603.getPosition());
    
    
    return result_peg_603;
    out_peg_604:
    
    if (column_peg_598.chunk8 == 0){
        column_peg_598.chunk8 = new Chunk8();
    }
    column_peg_598.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_624(stream, "expr10_rest");
    int myposition = position;
    tail_peg_608:
    Value right;
    Value new_left;
    Result result_peg_607(myposition);
    {
    
        result_peg_607 = rule_s(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_610;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_607.getPosition()))){
                    result_peg_607.nextPosition();
                } else {
                    goto out_peg_610;
                }
            }
            result_peg_607.setValue((void*) "+");
        
        
        
        result_peg_607 = rule_s(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_610;
            }
        
        
        
        result_peg_607 = rule_expr11(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_610;
            }
            right = result_peg_607.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_607.setValue(value);
            }
            new_left = result_peg_607.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_607.getPosition();
    goto tail_peg_608;
    out_peg_610:
    Result result_peg_615(myposition);
    {
    
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_615.getPosition()))){
                    result_peg_615.nextPosition();
                } else {
                    goto out_peg_617;
                }
            }
            result_peg_615.setValue((void*) "-");
        
        
        
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
        
        
        
        result_peg_615 = rule_expr11(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
            right = result_peg_615.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_615.setValue(value);
            }
            new_left = result_peg_615.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_615.getPosition();
    goto tail_peg_608;
    out_peg_617:
    Result result_peg_622(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_622.setValue(value);
            }
        
        
    }
    stream.update(result_peg_622.getPosition());
    
    return result_peg_622;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_650(stream, "expr11_rest");
    int myposition = position;
    tail_peg_627:
    Value right;
    Value new_left;
    Result result_peg_626(myposition);
    {
    
        result_peg_626 = rule_s(stream, result_peg_626.getPosition());
            if (result_peg_626.error()){
                goto out_peg_629;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_626.getPosition()))){
                    result_peg_626.nextPosition();
                } else {
                    goto out_peg_629;
                }
            }
            result_peg_626.setValue((void*) "*");
        
        
        
        result_peg_626 = rule_s(stream, result_peg_626.getPosition());
            if (result_peg_626.error()){
                goto out_peg_629;
            }
        
        
        
        result_peg_626 = rule_expr12(stream, result_peg_626.getPosition());
            if (result_peg_626.error()){
                goto out_peg_629;
            }
            right = result_peg_626.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_626.setValue(value);
            }
            new_left = result_peg_626.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_626.getPosition();
    goto tail_peg_627;
    out_peg_629:
    Result result_peg_634(myposition);
    {
    
        result_peg_634 = rule_s(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_634.getPosition()))){
                    result_peg_634.nextPosition();
                } else {
                    goto out_peg_636;
                }
            }
            result_peg_634.setValue((void*) "/");
        
        
        
        result_peg_634 = rule_s(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
        
        
        
        result_peg_634 = rule_expr12(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
            right = result_peg_634.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_634.setValue(value);
            }
            new_left = result_peg_634.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_634.getPosition();
    goto tail_peg_627;
    out_peg_636:
    Result result_peg_641(myposition);
    {
    
        result_peg_641 = rule_s(stream, result_peg_641.getPosition());
            if (result_peg_641.error()){
                goto out_peg_643;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_641.getPosition()))){
                    result_peg_641.nextPosition();
                } else {
                    goto out_peg_643;
                }
            }
            result_peg_641.setValue((void*) "%");
        
        
        
        result_peg_641 = rule_s(stream, result_peg_641.getPosition());
            if (result_peg_641.error()){
                goto out_peg_643;
            }
        
        
        
        result_peg_641 = rule_expr12(stream, result_peg_641.getPosition());
            if (result_peg_641.error()){
                goto out_peg_643;
            }
            right = result_peg_641.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_641.setValue(value);
            }
            new_left = result_peg_641.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_641.getPosition();
    goto tail_peg_627;
    out_peg_643:
    Result result_peg_648(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_648.setValue(value);
            }
        
        
    }
    stream.update(result_peg_648.getPosition());
    
    return result_peg_648;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_662(stream, "expr12_rest");
    int myposition = position;
    tail_peg_653:
    Value right;
    Value new_left;
    Result result_peg_652(myposition);
    {
    
        result_peg_652 = rule_s(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_655;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_652.getPosition()))){
                    result_peg_652.nextPosition();
                } else {
                    goto out_peg_655;
                }
            }
            result_peg_652.setValue((void*) "**");
        
        
        
        result_peg_652 = rule_s(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_655;
            }
        
        
        
        result_peg_652 = rule_expr13(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_655;
            }
            right = result_peg_652.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_652.setValue(value);
            }
            new_left = result_peg_652.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_652.getPosition();
    goto tail_peg_653;
    out_peg_655:
    Result result_peg_660(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_660.setValue(value);
            }
        
        
    }
    stream.update(result_peg_660.getPosition());
    
    return result_peg_660;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_663 = stream.getColumn(position);
    if (column_peg_663.chunk9 != 0 && column_peg_663.chunk9->chunk_unary.calculated()){
        return column_peg_663.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_692(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_664(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_664.getPosition()))){
                    result_peg_664.nextPosition();
                } else {
                    goto out_peg_666;
                }
            }
            result_peg_664.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_664.setValue(value);
            }
        
        
    }
    
    if (column_peg_663.chunk9 == 0){
        column_peg_663.chunk9 = new Chunk9();
    }
    column_peg_663.chunk9->chunk_unary = result_peg_664;
    stream.update(result_peg_664.getPosition());
    
    
    return result_peg_664;
    out_peg_666:
    Result result_peg_667(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_667.getPosition()))){
                    result_peg_667.nextPosition();
                } else {
                    goto out_peg_669;
                }
            }
            result_peg_667.setValue((void*) "-");
        
        
        
        Result result_peg_672(result_peg_667);
            {
                
                char letter_peg_677 = stream.get(result_peg_672.getPosition());
                if (letter_peg_677 != '\0' && strchr("0123456789", letter_peg_677) != NULL){
                    result_peg_672.nextPosition();
                    result_peg_672.setValue((void*) (long) letter_peg_677);
                } else {
                    goto out_peg_676;
                }
                
            }
            goto success_peg_673;
            out_peg_676:
            goto not_peg_671;
            success_peg_673:
            ;
            goto out_peg_669;
            not_peg_671:
            result_peg_667.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_667.setValue(value);
            }
        
        
    }
    
    if (column_peg_663.chunk9 == 0){
        column_peg_663.chunk9 = new Chunk9();
    }
    column_peg_663.chunk9->chunk_unary = result_peg_667;
    stream.update(result_peg_667.getPosition());
    
    
    return result_peg_667;
    out_peg_669:
    Result result_peg_678(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_678.getPosition()))){
                    result_peg_678.nextPosition();
                } else {
                    goto out_peg_680;
                }
            }
            result_peg_678.setValue((void*) "~");
        
        
        
        Result result_peg_683(result_peg_678);
            result_peg_683 = rule_key(stream, result_peg_683.getPosition());
            if (result_peg_683.error()){
                goto not_peg_682;
            }
            goto out_peg_680;
            not_peg_682:
            result_peg_678.setValue((void*)0);
        
        
        
        Result result_peg_686(result_peg_678);
            {
                
                char letter_peg_691 = stream.get(result_peg_686.getPosition());
                if (letter_peg_691 != '\0' && strchr("0123456789", letter_peg_691) != NULL){
                    result_peg_686.nextPosition();
                    result_peg_686.setValue((void*) (long) letter_peg_691);
                } else {
                    goto out_peg_690;
                }
                
            }
            goto success_peg_687;
            out_peg_690:
            goto not_peg_685;
            success_peg_687:
            ;
            goto out_peg_680;
            not_peg_685:
            result_peg_678.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_678.setValue(value);
            }
        
        
    }
    
    if (column_peg_663.chunk9 == 0){
        column_peg_663.chunk9 = new Chunk9();
    }
    column_peg_663.chunk9->chunk_unary = result_peg_678;
    stream.update(result_peg_678.getPosition());
    
    
    return result_peg_678;
    out_peg_680:
    
    if (column_peg_663.chunk9 == 0){
        column_peg_663.chunk9 = new Chunk9();
    }
    column_peg_663.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_693 = stream.getColumn(position);
    if (column_peg_693.chunk9 != 0 && column_peg_693.chunk9->chunk_expr13_real.calculated()){
        return column_peg_693.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_707(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_694(myposition);
    
    result_peg_694 = rule_helper(stream, result_peg_694.getPosition());
    if (result_peg_694.error()){
        goto out_peg_695;
    }
    
    if (column_peg_693.chunk9 == 0){
        column_peg_693.chunk9 = new Chunk9();
    }
    column_peg_693.chunk9->chunk_expr13_real = result_peg_694;
    stream.update(result_peg_694.getPosition());
    
    
    return result_peg_694;
    out_peg_695:
    Result result_peg_696(myposition);
    
    result_peg_696 = rule_function(stream, result_peg_696.getPosition());
    if (result_peg_696.error()){
        goto out_peg_697;
    }
    
    if (column_peg_693.chunk9 == 0){
        column_peg_693.chunk9 = new Chunk9();
    }
    column_peg_693.chunk9->chunk_expr13_real = result_peg_696;
    stream.update(result_peg_696.getPosition());
    
    
    return result_peg_696;
    out_peg_697:
    Result result_peg_698(myposition);
    
    result_peg_698 = rule_value(stream, result_peg_698.getPosition());
    if (result_peg_698.error()){
        goto out_peg_699;
    }
    
    if (column_peg_693.chunk9 == 0){
        column_peg_693.chunk9 = new Chunk9();
    }
    column_peg_693.chunk9->chunk_expr13_real = result_peg_698;
    stream.update(result_peg_698.getPosition());
    
    
    return result_peg_698;
    out_peg_699:
    Result result_peg_700(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_700.getPosition()))){
                    result_peg_700.nextPosition();
                } else {
                    goto out_peg_702;
                }
            }
            result_peg_700.setValue((void*) "(");
        
        
        
        result_peg_700 = rule_s(stream, result_peg_700.getPosition());
            if (result_peg_700.error()){
                goto out_peg_702;
            }
        
        
        
        result_peg_700 = rule_expr(stream, result_peg_700.getPosition());
            if (result_peg_700.error()){
                goto out_peg_702;
            }
            e = result_peg_700.getValues();
        
        
        
        result_peg_700 = rule_s(stream, result_peg_700.getPosition());
            if (result_peg_700.error()){
                goto out_peg_702;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_700.getPosition()))){
                    result_peg_700.nextPosition();
                } else {
                    goto out_peg_702;
                }
            }
            result_peg_700.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_700.setValue(value);
            }
        
        
    }
    
    if (column_peg_693.chunk9 == 0){
        column_peg_693.chunk9 = new Chunk9();
    }
    column_peg_693.chunk9->chunk_expr13_real = result_peg_700;
    stream.update(result_peg_700.getPosition());
    
    
    return result_peg_700;
    out_peg_702:
    
    if (column_peg_693.chunk9 == 0){
        column_peg_693.chunk9 = new Chunk9();
    }
    column_peg_693.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_708 = stream.getColumn(position);
    if (column_peg_708.chunk9 != 0 && column_peg_708.chunk9->chunk_function.calculated()){
        return column_peg_708.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_734(stream, "function");
    int myposition = position;
    
    Value name;
    Value args;
    Value arg1;
    Result result_peg_709(myposition);
    
    {
    
        result_peg_709 = rule_function_name(stream, result_peg_709.getPosition());
            if (result_peg_709.error()){
                goto out_peg_711;
            }
            name = result_peg_709.getValues();
        
        
        
        result_peg_709 = rule_s(stream, result_peg_709.getPosition());
            if (result_peg_709.error()){
                goto out_peg_711;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_709.getPosition()))){
                    result_peg_709.nextPosition();
                } else {
                    goto out_peg_711;
                }
            }
            result_peg_709.setValue((void*) "(");
        
        
        
        result_peg_709 = rule_s(stream, result_peg_709.getPosition());
            if (result_peg_709.error()){
                goto out_peg_711;
            }
        
        
        
        result_peg_709 = rule_args(stream, result_peg_709.getPosition());
            if (result_peg_709.error()){
                goto out_peg_711;
            }
            args = result_peg_709.getValues();
        
        
        
        result_peg_709 = rule_s(stream, result_peg_709.getPosition());
            if (result_peg_709.error()){
                goto out_peg_711;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_709.getPosition()))){
                    result_peg_709.nextPosition();
                } else {
                    goto out_peg_711;
                }
            }
            result_peg_709.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, args);
                result_peg_709.setValue(value);
            }
        
        
    }
    
    if (column_peg_708.chunk9 == 0){
        column_peg_708.chunk9 = new Chunk9();
    }
    column_peg_708.chunk9->chunk_function = result_peg_709;
    stream.update(result_peg_709.getPosition());
    
    
    return result_peg_709;
    out_peg_711:
    Result result_peg_718(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_718.getPosition()))){
                    result_peg_718.nextPosition();
                } else {
                    goto out_peg_720;
                }
            }
            result_peg_718.setValue((void*) "animelem");
            name = result_peg_718.getValues();
        
        
        
        result_peg_718 = rule_s(stream, result_peg_718.getPosition());
            if (result_peg_718.error()){
                goto out_peg_720;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_718.getPosition()))){
                    result_peg_718.nextPosition();
                } else {
                    goto out_peg_720;
                }
            }
            result_peg_718.setValue((void*) "=");
        
        
        
        result_peg_718 = rule_s(stream, result_peg_718.getPosition());
            if (result_peg_718.error()){
                goto out_peg_720;
            }
        
        
        
        result_peg_718 = rule_integer(stream, result_peg_718.getPosition());
            if (result_peg_718.error()){
                goto out_peg_720;
            }
            arg1 = result_peg_718.getValues();
        
        
        
        result_peg_718 = rule_function_rest(stream, result_peg_718.getPosition());
            if (result_peg_718.error()){
                goto out_peg_720;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1(name, arg1);
                result_peg_718.setValue(value);
            }
        
        
    }
    
    if (column_peg_708.chunk9 == 0){
        column_peg_708.chunk9 = new Chunk9();
    }
    column_peg_708.chunk9->chunk_function = result_peg_718;
    stream.update(result_peg_718.getPosition());
    
    
    return result_peg_718;
    out_peg_720:
    Result result_peg_726(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_726.getPosition()))){
                    result_peg_726.nextPosition();
                } else {
                    goto out_peg_728;
                }
            }
            result_peg_726.setValue((void*) "animelem");
            name = result_peg_726.getValues();
        
        
        
        result_peg_726 = rule_s(stream, result_peg_726.getPosition());
            if (result_peg_726.error()){
                goto out_peg_728;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_726.getPosition()))){
                    result_peg_726.nextPosition();
                } else {
                    goto out_peg_728;
                }
            }
            result_peg_726.setValue((void*) "<=");
        
        
        
        result_peg_726 = rule_s(stream, result_peg_726.getPosition());
            if (result_peg_726.error()){
                goto out_peg_728;
            }
        
        
        
        result_peg_726 = rule_integer(stream, result_peg_726.getPosition());
            if (result_peg_726.error()){
                goto out_peg_728;
            }
            arg1 = result_peg_726.getValues();
        
        
        
        result_peg_726 = rule_function_rest(stream, result_peg_726.getPosition());
            if (result_peg_726.error()){
                goto out_peg_728;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1("animelem<=", arg1);
                result_peg_726.setValue(value);
            }
        
        
    }
    
    if (column_peg_708.chunk9 == 0){
        column_peg_708.chunk9 = new Chunk9();
    }
    column_peg_708.chunk9->chunk_function = result_peg_726;
    stream.update(result_peg_726.getPosition());
    
    
    return result_peg_726;
    out_peg_728:
    
    if (column_peg_708.chunk9 == 0){
        column_peg_708.chunk9 = new Chunk9();
    }
    column_peg_708.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_735 = stream.getColumn(position);
    if (column_peg_735.chunk10 != 0 && column_peg_735.chunk10->chunk_args.calculated()){
        return column_peg_735.chunk10->chunk_args;
    }
    
    RuleTrace trace_peg_745(stream, "args");
    int myposition = position;
    
    Value expr1;
    Value expr_rest;
    Result result_peg_736(myposition);
    
    {
    
        result_peg_736 = rule_expr_c(stream, result_peg_736.getPosition());
            if (result_peg_736.error()){
                goto out_peg_738;
            }
            expr1 = result_peg_736.getValues();
        
        
        
        result_peg_736.reset();
            do{
                Result result_peg_741(result_peg_736.getPosition());
                {
                
                    result_peg_741 = rule_s(stream, result_peg_741.getPosition());
                        if (result_peg_741.error()){
                            goto loop_peg_740;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_741.getPosition()))){
                                result_peg_741.nextPosition();
                            } else {
                                goto loop_peg_740;
                            }
                        }
                        result_peg_741.setValue((void*) ",");
                    
                    
                    
                    result_peg_741 = rule_s(stream, result_peg_741.getPosition());
                        if (result_peg_741.error()){
                            goto loop_peg_740;
                        }
                    
                    
                    
                    result_peg_741 = rule_expr_c(stream, result_peg_741.getPosition());
                        if (result_peg_741.error()){
                            goto loop_peg_740;
                        }
                    
                    
                }
                result_peg_736.addResult(result_peg_741);
            } while (true);
            loop_peg_740:
            ;
            expr_rest = result_peg_736.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(expr1, expr_rest);
                result_peg_736.setValue(value);
            }
        
        
    }
    
    if (column_peg_735.chunk10 == 0){
        column_peg_735.chunk10 = new Chunk10();
    }
    column_peg_735.chunk10->chunk_args = result_peg_736;
    stream.update(result_peg_736.getPosition());
    
    
    return result_peg_736;
    out_peg_738:
    
    if (column_peg_735.chunk10 == 0){
        column_peg_735.chunk10 = new Chunk10();
    }
    column_peg_735.chunk10->chunk_args = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_746 = stream.getColumn(position);
    if (column_peg_746.chunk10 != 0 && column_peg_746.chunk10->chunk_function_name.calculated()){
        return column_peg_746.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_795(stream, "function_name");
    int myposition = position;
    
    
    Result result_peg_747(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("abs"[i], stream.get(result_peg_747.getPosition()))){
            result_peg_747.nextPosition();
        } else {
            goto out_peg_748;
        }
    }
    result_peg_747.setValue((void*) "abs");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_747;
    stream.update(result_peg_747.getPosition());
    
    
    return result_peg_747;
    out_peg_748:
    Result result_peg_749(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("const"[i], stream.get(result_peg_749.getPosition()))){
            result_peg_749.nextPosition();
        } else {
            goto out_peg_750;
        }
    }
    result_peg_749.setValue((void*) "const");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_749;
    stream.update(result_peg_749.getPosition());
    
    
    return result_peg_749;
    out_peg_750:
    Result result_peg_751(myposition);
    
    for (int i = 0; i < 13; i++){
        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_751.getPosition()))){
            result_peg_751.nextPosition();
        } else {
            goto out_peg_752;
        }
    }
    result_peg_751.setValue((void*) "selfanimexist");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_751;
    stream.update(result_peg_751.getPosition());
    
    
    return result_peg_751;
    out_peg_752:
    Result result_peg_753(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("ifelse"[i], stream.get(result_peg_753.getPosition()))){
            result_peg_753.nextPosition();
        } else {
            goto out_peg_754;
        }
    }
    result_peg_753.setValue((void*) "ifelse");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_753;
    stream.update(result_peg_753.getPosition());
    
    
    return result_peg_753;
    out_peg_754:
    Result result_peg_755(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("gethitvar"[i], stream.get(result_peg_755.getPosition()))){
            result_peg_755.nextPosition();
        } else {
            goto out_peg_756;
        }
    }
    result_peg_755.setValue((void*) "gethitvar");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_755;
    stream.update(result_peg_755.getPosition());
    
    
    return result_peg_755;
    out_peg_756:
    Result result_peg_757(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("floor"[i], stream.get(result_peg_757.getPosition()))){
            result_peg_757.nextPosition();
        } else {
            goto out_peg_758;
        }
    }
    result_peg_757.setValue((void*) "floor");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_757;
    stream.update(result_peg_757.getPosition());
    
    
    return result_peg_757;
    out_peg_758:
    Result result_peg_759(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("ceil"[i], stream.get(result_peg_759.getPosition()))){
            result_peg_759.nextPosition();
        } else {
            goto out_peg_760;
        }
    }
    result_peg_759.setValue((void*) "ceil");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_759;
    stream.update(result_peg_759.getPosition());
    
    
    return result_peg_759;
    out_peg_760:
    Result result_peg_761(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("sin"[i], stream.get(result_peg_761.getPosition()))){
            result_peg_761.nextPosition();
        } else {
            goto out_peg_762;
        }
    }
    result_peg_761.setValue((void*) "sin");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_761;
    stream.update(result_peg_761.getPosition());
    
    
    return result_peg_761;
    out_peg_762:
    Result result_peg_763(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("sysvar"[i], stream.get(result_peg_763.getPosition()))){
            result_peg_763.nextPosition();
        } else {
            goto out_peg_764;
        }
    }
    result_peg_763.setValue((void*) "sysvar");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_763;
    stream.update(result_peg_763.getPosition());
    
    
    return result_peg_763;
    out_peg_764:
    Result result_peg_765(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("var"[i], stream.get(result_peg_765.getPosition()))){
            result_peg_765.nextPosition();
        } else {
            goto out_peg_766;
        }
    }
    result_peg_765.setValue((void*) "var");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_765;
    stream.update(result_peg_765.getPosition());
    
    
    return result_peg_765;
    out_peg_766:
    Result result_peg_767(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numexplod"[i], stream.get(result_peg_767.getPosition()))){
            result_peg_767.nextPosition();
        } else {
            goto out_peg_768;
        }
    }
    result_peg_767.setValue((void*) "numexplod");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_767;
    stream.update(result_peg_767.getPosition());
    
    
    return result_peg_767;
    out_peg_768:
    Result result_peg_769(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_769.getPosition()))){
            result_peg_769.nextPosition();
        } else {
            goto out_peg_770;
        }
    }
    result_peg_769.setValue((void*) "numhelper");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_769;
    stream.update(result_peg_769.getPosition());
    
    
    return result_peg_769;
    out_peg_770:
    Result result_peg_771(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numprojid"[i], stream.get(result_peg_771.getPosition()))){
            result_peg_771.nextPosition();
        } else {
            goto out_peg_772;
        }
    }
    result_peg_771.setValue((void*) "numprojid");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_771;
    stream.update(result_peg_771.getPosition());
    
    
    return result_peg_771;
    out_peg_772:
    Result result_peg_773(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("fvar"[i], stream.get(result_peg_773.getPosition()))){
            result_peg_773.nextPosition();
        } else {
            goto out_peg_774;
        }
    }
    result_peg_773.setValue((void*) "fvar");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_773;
    stream.update(result_peg_773.getPosition());
    
    
    return result_peg_773;
    out_peg_774:
    Result result_peg_775(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("ishelper"[i], stream.get(result_peg_775.getPosition()))){
            result_peg_775.nextPosition();
        } else {
            goto out_peg_776;
        }
    }
    result_peg_775.setValue((void*) "ishelper");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_775;
    stream.update(result_peg_775.getPosition());
    
    
    return result_peg_775;
    out_peg_776:
    Result result_peg_777(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numtarget"[i], stream.get(result_peg_777.getPosition()))){
            result_peg_777.nextPosition();
        } else {
            goto out_peg_778;
        }
    }
    result_peg_777.setValue((void*) "numtarget");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_777;
    stream.update(result_peg_777.getPosition());
    
    
    return result_peg_777;
    out_peg_778:
    Result result_peg_779(myposition);
    
    for (int i = 0; i < 12; i++){
        if (compareCharCase("animelemtime"[i], stream.get(result_peg_779.getPosition()))){
            result_peg_779.nextPosition();
        } else {
            goto out_peg_780;
        }
    }
    result_peg_779.setValue((void*) "animelemtime");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_779;
    stream.update(result_peg_779.getPosition());
    
    
    return result_peg_779;
    out_peg_780:
    Result result_peg_781(myposition);
    
    for (int i = 0; i < 10; i++){
        if (compareCharCase("animelemno"[i], stream.get(result_peg_781.getPosition()))){
            result_peg_781.nextPosition();
        } else {
            goto out_peg_782;
        }
    }
    result_peg_781.setValue((void*) "animelemno");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_781;
    stream.update(result_peg_781.getPosition());
    
    
    return result_peg_781;
    out_peg_782:
    Result result_peg_783(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("animexist"[i], stream.get(result_peg_783.getPosition()))){
            result_peg_783.nextPosition();
        } else {
            goto out_peg_784;
        }
    }
    result_peg_783.setValue((void*) "animexist");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_783;
    stream.update(result_peg_783.getPosition());
    
    
    return result_peg_783;
    out_peg_784:
    Result result_peg_785(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projguarded"[i], stream.get(result_peg_785.getPosition()))){
            result_peg_785.nextPosition();
        } else {
            goto out_peg_786;
        }
    }
    result_peg_785.setValue((void*) "projguarded");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_785;
    stream.update(result_peg_785.getPosition());
    
    
    return result_peg_785;
    out_peg_786:
    Result result_peg_787(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projhittime"[i], stream.get(result_peg_787.getPosition()))){
            result_peg_787.nextPosition();
        } else {
            goto out_peg_788;
        }
    }
    result_peg_787.setValue((void*) "projhittime");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_787;
    stream.update(result_peg_787.getPosition());
    
    
    return result_peg_787;
    out_peg_788:
    Result result_peg_789(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projcontact"[i], stream.get(result_peg_789.getPosition()))){
            result_peg_789.nextPosition();
        } else {
            goto out_peg_790;
        }
    }
    result_peg_789.setValue((void*) "projcontact");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_789;
    stream.update(result_peg_789.getPosition());
    
    
    return result_peg_789;
    out_peg_790:
    Result result_peg_791(myposition);
    
    for (int i = 0; i < 15; i++){
        if (compareCharCase("projcontacttime"[i], stream.get(result_peg_791.getPosition()))){
            result_peg_791.nextPosition();
        } else {
            goto out_peg_792;
        }
    }
    result_peg_791.setValue((void*) "projcontacttime");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_791;
    stream.update(result_peg_791.getPosition());
    
    
    return result_peg_791;
    out_peg_792:
    Result result_peg_793(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_793.getPosition()))){
            result_peg_793.nextPosition();
        } else {
            goto out_peg_794;
        }
    }
    result_peg_793.setValue((void*) "numhelper");
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = result_peg_793;
    stream.update(result_peg_793.getPosition());
    
    
    return result_peg_793;
    out_peg_794:
    
    if (column_peg_746.chunk10 == 0){
        column_peg_746.chunk10 = new Chunk10();
    }
    column_peg_746.chunk10->chunk_function_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_796 = stream.getColumn(position);
    if (column_peg_796.chunk10 != 0 && column_peg_796.chunk10->chunk_function_rest.calculated()){
        return column_peg_796.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_806(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_797(myposition);
    
    {
    
        result_peg_797 = rule_s(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_797.getPosition()))){
                    result_peg_797.nextPosition();
                } else {
                    goto out_peg_799;
                }
            }
            result_peg_797.setValue((void*) ",");
        
        
        
        result_peg_797 = rule_s(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
        
        int save_peg_803 = result_peg_797.getPosition();
            
            result_peg_797 = rule_all_compare(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                
                result_peg_797 = Result(save_peg_803);
                result_peg_797.setValue((void*) 0);
                
            }
        
        
        
        result_peg_797 = rule_s(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
        
        result_peg_797 = rule_value(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
    }
    
    if (column_peg_796.chunk10 == 0){
        column_peg_796.chunk10 = new Chunk10();
    }
    column_peg_796.chunk10->chunk_function_rest = result_peg_797;
    stream.update(result_peg_797.getPosition());
    
    
    return result_peg_797;
    out_peg_799:
    Result result_peg_805(myposition);
    
    
    
    if (column_peg_796.chunk10 == 0){
        column_peg_796.chunk10 = new Chunk10();
    }
    column_peg_796.chunk10->chunk_function_rest = result_peg_805;
    stream.update(result_peg_805.getPosition());
    
    
    return result_peg_805;
    
    if (column_peg_796.chunk10 == 0){
        column_peg_796.chunk10 = new Chunk10();
    }
    column_peg_796.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_807 = stream.getColumn(position);
    if (column_peg_807.chunk10 != 0 && column_peg_807.chunk10->chunk_keys.calculated()){
        return column_peg_807.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_810(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_808(myposition);
    
    result_peg_808 = rule_key_value_list(stream, result_peg_808.getPosition());
    if (result_peg_808.error()){
        goto out_peg_809;
    }
    
    if (column_peg_807.chunk10 == 0){
        column_peg_807.chunk10 = new Chunk10();
    }
    column_peg_807.chunk10->chunk_keys = result_peg_808;
    stream.update(result_peg_808.getPosition());
    
    
    return result_peg_808;
    out_peg_809:
    
    if (column_peg_807.chunk10 == 0){
        column_peg_807.chunk10 = new Chunk10();
    }
    column_peg_807.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_811 = stream.getColumn(position);
    if (column_peg_811.chunk10 != 0 && column_peg_811.chunk10->chunk_key_value_list.calculated()){
        return column_peg_811.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_821(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_812(myposition);
    
    {
    
        result_peg_812 = rule_key(stream, result_peg_812.getPosition());
            if (result_peg_812.error()){
                goto out_peg_814;
            }
            first = result_peg_812.getValues();
        
        
        
        result_peg_812.reset();
            do{
                Result result_peg_817(result_peg_812.getPosition());
                {
                
                    result_peg_817 = rule_s(stream, result_peg_817.getPosition());
                        if (result_peg_817.error()){
                            goto loop_peg_816;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_817.getPosition()))){
                                result_peg_817.nextPosition();
                            } else {
                                goto loop_peg_816;
                            }
                        }
                        result_peg_817.setValue((void*) ",");
                    
                    
                    
                    result_peg_817 = rule_s(stream, result_peg_817.getPosition());
                        if (result_peg_817.error()){
                            goto loop_peg_816;
                        }
                    
                    
                    
                    result_peg_817 = rule_key(stream, result_peg_817.getPosition());
                        if (result_peg_817.error()){
                            goto loop_peg_816;
                        }
                    
                    
                }
                result_peg_812.addResult(result_peg_817);
            } while (true);
            loop_peg_816:
            ;
            rest = result_peg_812.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_812.setValue(value);
            }
        
        
    }
    
    if (column_peg_811.chunk10 == 0){
        column_peg_811.chunk10 = new Chunk10();
    }
    column_peg_811.chunk10->chunk_key_value_list = result_peg_812;
    stream.update(result_peg_812.getPosition());
    
    
    return result_peg_812;
    out_peg_814:
    
    if (column_peg_811.chunk10 == 0){
        column_peg_811.chunk10 = new Chunk10();
    }
    column_peg_811.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_822 = stream.getColumn(position);
    if (column_peg_822.chunk11 != 0 && column_peg_822.chunk11->chunk_key.calculated()){
        return column_peg_822.chunk11->chunk_key;
    }
    
    RuleTrace trace_peg_830(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_823(myposition);
    
    {
    
        result_peg_823 = rule_key_real(stream, result_peg_823.getPosition());
            if (result_peg_823.error()){
                goto out_peg_825;
            }
        
        Result result_peg_824 = result_peg_823;
        
        result_peg_823 = rule_key_rest(stream, result_peg_823.getPosition(), result_peg_824.getValues());
            if (result_peg_823.error()){
                goto out_peg_825;
            }
            ok = result_peg_823.getValues();
        
        
        
        Result result_peg_829(result_peg_823);
            result_peg_829 = rule_identifier(stream, result_peg_829.getPosition());
            if (result_peg_829.error()){
                goto not_peg_828;
            }
            goto out_peg_825;
            not_peg_828:
            result_peg_823.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_823.setValue(value);
            }
        
        
    }
    
    if (column_peg_822.chunk11 == 0){
        column_peg_822.chunk11 = new Chunk11();
    }
    column_peg_822.chunk11->chunk_key = result_peg_823;
    stream.update(result_peg_823.getPosition());
    
    
    return result_peg_823;
    out_peg_825:
    
    if (column_peg_822.chunk11 == 0){
        column_peg_822.chunk11 = new Chunk11();
    }
    column_peg_822.chunk11->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_841(stream, "key_rest");
    int myposition = position;
    tail_peg_833:
    Value another;
    Value new_left;
    Result result_peg_832(myposition);
    {
    
        result_peg_832 = rule_s(stream, result_peg_832.getPosition());
            if (result_peg_832.error()){
                goto out_peg_835;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_832.getPosition()))){
                    result_peg_832.nextPosition();
                } else {
                    goto out_peg_835;
                }
            }
            result_peg_832.setValue((void*) "+");
        
        
        
        result_peg_832 = rule_key_real(stream, result_peg_832.getPosition());
            if (result_peg_832.error()){
                goto out_peg_835;
            }
            another = result_peg_832.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_832.setValue(value);
            }
            new_left = result_peg_832.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_832.getPosition();
    goto tail_peg_833;
    out_peg_835:
    Result result_peg_839(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_839.setValue(value);
            }
        
        
    }
    stream.update(result_peg_839.getPosition());
    
    return result_peg_839;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_842 = stream.getColumn(position);
    if (column_peg_842.chunk11 != 0 && column_peg_842.chunk11->chunk_key_real.calculated()){
        return column_peg_842.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_849(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_843(myposition);
    
    {
    
        result_peg_843.reset();
            do{
                Result result_peg_846(result_peg_843.getPosition());
                result_peg_846 = rule_key_modifier(stream, result_peg_846.getPosition());
                if (result_peg_846.error()){
                    goto loop_peg_845;
                }
                result_peg_843.addResult(result_peg_846);
            } while (true);
            loop_peg_845:
            ;
            mods = result_peg_843.getValues();
        
        
        
        result_peg_843 = rule_key_name(stream, result_peg_843.getPosition());
            if (result_peg_843.error()){
                goto out_peg_848;
            }
            name = result_peg_843.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_843.setValue(value);
            }
        
        
    }
    
    if (column_peg_842.chunk11 == 0){
        column_peg_842.chunk11 = new Chunk11();
    }
    column_peg_842.chunk11->chunk_key_real = result_peg_843;
    stream.update(result_peg_843.getPosition());
    
    
    return result_peg_843;
    out_peg_848:
    
    if (column_peg_842.chunk11 == 0){
        column_peg_842.chunk11 = new Chunk11();
    }
    column_peg_842.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_850 = stream.getColumn(position);
    if (column_peg_850.chunk11 != 0 && column_peg_850.chunk11->chunk_key_modifier.calculated()){
        return column_peg_850.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_871(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_851(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_851.getPosition()))){
                    result_peg_851.nextPosition();
                } else {
                    goto out_peg_853;
                }
            }
            result_peg_851.setValue((void*) "~");
        
        
        
        result_peg_851.reset();
            do{
                Result result_peg_856(result_peg_851.getPosition());
                {
                    
                    char letter_peg_861 = stream.get(result_peg_856.getPosition());
                    if (letter_peg_861 != '\0' && strchr("0123456789", letter_peg_861) != NULL){
                        result_peg_856.nextPosition();
                        result_peg_856.setValue((void*) (long) letter_peg_861);
                    } else {
                        goto out_peg_860;
                    }
                    
                }
                goto success_peg_857;
                out_peg_860:
                goto loop_peg_855;
                success_peg_857:
                ;
                result_peg_851.addResult(result_peg_856);
            } while (true);
            loop_peg_855:
            ;
            num = result_peg_851.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_851.setValue(value);
            }
        
        
    }
    
    if (column_peg_850.chunk11 == 0){
        column_peg_850.chunk11 = new Chunk11();
    }
    column_peg_850.chunk11->chunk_key_modifier = result_peg_851;
    stream.update(result_peg_851.getPosition());
    
    
    return result_peg_851;
    out_peg_853:
    Result result_peg_862(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_862.getPosition()))){
                    result_peg_862.nextPosition();
                } else {
                    goto out_peg_864;
                }
            }
            result_peg_862.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_862.setValue(value);
            }
        
        
    }
    
    if (column_peg_850.chunk11 == 0){
        column_peg_850.chunk11 = new Chunk11();
    }
    column_peg_850.chunk11->chunk_key_modifier = result_peg_862;
    stream.update(result_peg_862.getPosition());
    
    
    return result_peg_862;
    out_peg_864:
    Result result_peg_865(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_865.getPosition()))){
                    result_peg_865.nextPosition();
                } else {
                    goto out_peg_867;
                }
            }
            result_peg_865.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_865.setValue(value);
            }
        
        
    }
    
    if (column_peg_850.chunk11 == 0){
        column_peg_850.chunk11 = new Chunk11();
    }
    column_peg_850.chunk11->chunk_key_modifier = result_peg_865;
    stream.update(result_peg_865.getPosition());
    
    
    return result_peg_865;
    out_peg_867:
    Result result_peg_868(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_868.getPosition()))){
                    result_peg_868.nextPosition();
                } else {
                    goto out_peg_870;
                }
            }
            result_peg_868.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_868.setValue(value);
            }
        
        
    }
    
    if (column_peg_850.chunk11 == 0){
        column_peg_850.chunk11 = new Chunk11();
    }
    column_peg_850.chunk11->chunk_key_modifier = result_peg_868;
    stream.update(result_peg_868.getPosition());
    
    
    return result_peg_868;
    out_peg_870:
    
    if (column_peg_850.chunk11 == 0){
        column_peg_850.chunk11 = new Chunk11();
    }
    column_peg_850.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_872 = stream.getColumn(position);
    if (column_peg_872.chunk11 != 0 && column_peg_872.chunk11->chunk_key_name.calculated()){
        return column_peg_872.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_903(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_873(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_873.getPosition()))){
            result_peg_873.nextPosition();
        } else {
            goto out_peg_874;
        }
    }
    result_peg_873.setValue((void*) "DB");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_873;
    stream.update(result_peg_873.getPosition());
    
    
    return result_peg_873;
    out_peg_874:
    Result result_peg_875(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_875.getPosition()))){
            result_peg_875.nextPosition();
        } else {
            goto out_peg_876;
        }
    }
    result_peg_875.setValue((void*) "B");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_875;
    stream.update(result_peg_875.getPosition());
    
    
    return result_peg_875;
    out_peg_876:
    Result result_peg_877(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_877.getPosition()))){
            result_peg_877.nextPosition();
        } else {
            goto out_peg_878;
        }
    }
    result_peg_877.setValue((void*) "DF");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_877;
    stream.update(result_peg_877.getPosition());
    
    
    return result_peg_877;
    out_peg_878:
    Result result_peg_879(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_879.getPosition()))){
            result_peg_879.nextPosition();
        } else {
            goto out_peg_880;
        }
    }
    result_peg_879.setValue((void*) "D");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_879;
    stream.update(result_peg_879.getPosition());
    
    
    return result_peg_879;
    out_peg_880:
    Result result_peg_881(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_881.getPosition()))){
            result_peg_881.nextPosition();
        } else {
            goto out_peg_882;
        }
    }
    result_peg_881.setValue((void*) "F");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_881;
    stream.update(result_peg_881.getPosition());
    
    
    return result_peg_881;
    out_peg_882:
    Result result_peg_883(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_883.getPosition()))){
            result_peg_883.nextPosition();
        } else {
            goto out_peg_884;
        }
    }
    result_peg_883.setValue((void*) "UF");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_883;
    stream.update(result_peg_883.getPosition());
    
    
    return result_peg_883;
    out_peg_884:
    Result result_peg_885(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_885.getPosition()))){
            result_peg_885.nextPosition();
        } else {
            goto out_peg_886;
        }
    }
    result_peg_885.setValue((void*) "UB");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_885;
    stream.update(result_peg_885.getPosition());
    
    
    return result_peg_885;
    out_peg_886:
    Result result_peg_887(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_887.getPosition()))){
            result_peg_887.nextPosition();
        } else {
            goto out_peg_888;
        }
    }
    result_peg_887.setValue((void*) "U");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_887;
    stream.update(result_peg_887.getPosition());
    
    
    return result_peg_887;
    out_peg_888:
    Result result_peg_889(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_889.getPosition()))){
            result_peg_889.nextPosition();
        } else {
            goto out_peg_890;
        }
    }
    result_peg_889.setValue((void*) "a");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_889;
    stream.update(result_peg_889.getPosition());
    
    
    return result_peg_889;
    out_peg_890:
    Result result_peg_891(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_891.getPosition()))){
            result_peg_891.nextPosition();
        } else {
            goto out_peg_892;
        }
    }
    result_peg_891.setValue((void*) "b");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_891;
    stream.update(result_peg_891.getPosition());
    
    
    return result_peg_891;
    out_peg_892:
    Result result_peg_893(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_893.getPosition()))){
            result_peg_893.nextPosition();
        } else {
            goto out_peg_894;
        }
    }
    result_peg_893.setValue((void*) "c");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_893;
    stream.update(result_peg_893.getPosition());
    
    
    return result_peg_893;
    out_peg_894:
    Result result_peg_895(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_895.getPosition()))){
            result_peg_895.nextPosition();
        } else {
            goto out_peg_896;
        }
    }
    result_peg_895.setValue((void*) "x");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_895;
    stream.update(result_peg_895.getPosition());
    
    
    return result_peg_895;
    out_peg_896:
    Result result_peg_897(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_897.getPosition()))){
            result_peg_897.nextPosition();
        } else {
            goto out_peg_898;
        }
    }
    result_peg_897.setValue((void*) "y");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_897;
    stream.update(result_peg_897.getPosition());
    
    
    return result_peg_897;
    out_peg_898:
    Result result_peg_899(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_899.getPosition()))){
            result_peg_899.nextPosition();
        } else {
            goto out_peg_900;
        }
    }
    result_peg_899.setValue((void*) "z");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_899;
    stream.update(result_peg_899.getPosition());
    
    
    return result_peg_899;
    out_peg_900:
    Result result_peg_901(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_901.getPosition()))){
            result_peg_901.nextPosition();
        } else {
            goto out_peg_902;
        }
    }
    result_peg_901.setValue((void*) "s");
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = result_peg_901;
    stream.update(result_peg_901.getPosition());
    
    
    return result_peg_901;
    out_peg_902:
    
    if (column_peg_872.chunk11 == 0){
        column_peg_872.chunk11 = new Chunk11();
    }
    column_peg_872.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_904 = stream.getColumn(position);
    if (column_peg_904.chunk12 != 0 && column_peg_904.chunk12->chunk_value.calculated()){
        return column_peg_904.chunk12->chunk_value;
    }
    
    RuleTrace trace_peg_922(stream, "value");
    int myposition = position;
    
    
    Result result_peg_905(myposition);
    
    result_peg_905 = rule_float(stream, result_peg_905.getPosition());
    if (result_peg_905.error()){
        goto out_peg_906;
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_905;
    stream.update(result_peg_905.getPosition());
    
    
    return result_peg_905;
    out_peg_906:
    Result result_peg_907(myposition);
    
    result_peg_907 = rule_integer(stream, result_peg_907.getPosition());
    if (result_peg_907.error()){
        goto out_peg_908;
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_907;
    stream.update(result_peg_907.getPosition());
    
    
    return result_peg_907;
    out_peg_908:
    Result result_peg_909(myposition);
    
    result_peg_909 = rule_keyword(stream, result_peg_909.getPosition());
    if (result_peg_909.error()){
        goto out_peg_910;
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_909;
    stream.update(result_peg_909.getPosition());
    
    
    return result_peg_909;
    out_peg_910:
    Result result_peg_911(myposition);
    
    {
    
        Result result_peg_914(result_peg_911);
            result_peg_914 = rule_keyword(stream, result_peg_914.getPosition());
            if (result_peg_914.error()){
                goto not_peg_913;
            }
            goto out_peg_915;
            not_peg_913:
            result_peg_911.setValue((void*)0);
        
        
        
        result_peg_911 = rule_identifier(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_915;
            }
        
        
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_911;
    stream.update(result_peg_911.getPosition());
    
    
    return result_peg_911;
    out_peg_915:
    Result result_peg_916(myposition);
    
    result_peg_916 = rule_range(stream, result_peg_916.getPosition());
    if (result_peg_916.error()){
        goto out_peg_917;
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_916;
    stream.update(result_peg_916.getPosition());
    
    
    return result_peg_916;
    out_peg_917:
    Result result_peg_918(myposition);
    
    result_peg_918 = rule_string(stream, result_peg_918.getPosition());
    if (result_peg_918.error()){
        goto out_peg_919;
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_918;
    stream.update(result_peg_918.getPosition());
    
    
    return result_peg_918;
    out_peg_919:
    Result result_peg_920(myposition);
    
    result_peg_920 = rule_hitflag(stream, result_peg_920.getPosition());
    if (result_peg_920.error()){
        goto out_peg_921;
    }
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = result_peg_920;
    stream.update(result_peg_920.getPosition());
    
    
    return result_peg_920;
    out_peg_921:
    
    if (column_peg_904.chunk12 == 0){
        column_peg_904.chunk12 = new Chunk12();
    }
    column_peg_904.chunk12->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_923 = stream.getColumn(position);
    if (column_peg_923.chunk12 != 0 && column_peg_923.chunk12->chunk_helper.calculated()){
        return column_peg_923.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_934(stream, "helper");
    int myposition = position;
    
    Value name;
    Value id;
    Result result_peg_924(myposition);
    
    {
    
        result_peg_924 = rule_helper__name(stream, result_peg_924.getPosition());
            if (result_peg_924.error()){
                goto out_peg_926;
            }
            name = result_peg_924.getValues();
        
        
        
        result_peg_924 = rule_s(stream, result_peg_924.getPosition());
            if (result_peg_924.error()){
                goto out_peg_926;
            }
        
        
        
        int save_peg_929 = result_peg_924.getPosition();
            
            result_peg_924 = rule_helper__expression(stream, result_peg_924.getPosition());
            if (result_peg_924.error()){
                
                result_peg_924 = Result(save_peg_929);
                result_peg_924.setValue((void*) 0);
                
            }
        
        
        
        result_peg_924 = rule_s(stream, result_peg_924.getPosition());
            if (result_peg_924.error()){
                goto out_peg_926;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_924.getPosition()))){
                    result_peg_924.nextPosition();
                } else {
                    goto out_peg_926;
                }
            }
            result_peg_924.setValue((void*) ",");
        
        
        
        result_peg_924 = rule_s(stream, result_peg_924.getPosition());
            if (result_peg_924.error()){
                goto out_peg_926;
            }
        
        
        
        result_peg_924 = rule_helper__identifier(stream, result_peg_924.getPosition());
            if (result_peg_924.error()){
                goto out_peg_926;
            }
            id = result_peg_924.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeHelper(name, id);
                result_peg_924.setValue(value);
            }
        
        
    }
    
    if (column_peg_923.chunk12 == 0){
        column_peg_923.chunk12 = new Chunk12();
    }
    column_peg_923.chunk12->chunk_helper = result_peg_924;
    stream.update(result_peg_924.getPosition());
    
    
    return result_peg_924;
    out_peg_926:
    
    if (column_peg_923.chunk12 == 0){
        column_peg_923.chunk12 = new Chunk12();
    }
    column_peg_923.chunk12->chunk_helper = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_935 = stream.getColumn(position);
    if (column_peg_935.chunk12 != 0 && column_peg_935.chunk12->chunk_helper__expression.calculated()){
        return column_peg_935.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_942(stream, "helper__expression");
    int myposition = position;
    
    
    Result result_peg_936(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_936.getPosition()))){
                    result_peg_936.nextPosition();
                } else {
                    goto out_peg_938;
                }
            }
            result_peg_936.setValue((void*) "(");
        
        
        
        result_peg_936 = rule_s(stream, result_peg_936.getPosition());
            if (result_peg_936.error()){
                goto out_peg_938;
            }
        
        
        
        result_peg_936 = rule_expr_c(stream, result_peg_936.getPosition());
            if (result_peg_936.error()){
                goto out_peg_938;
            }
        
        
        
        result_peg_936 = rule_s(stream, result_peg_936.getPosition());
            if (result_peg_936.error()){
                goto out_peg_938;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_936.getPosition()))){
                    result_peg_936.nextPosition();
                } else {
                    goto out_peg_938;
                }
            }
            result_peg_936.setValue((void*) ")");
        
        
    }
    
    if (column_peg_935.chunk12 == 0){
        column_peg_935.chunk12 = new Chunk12();
    }
    column_peg_935.chunk12->chunk_helper__expression = result_peg_936;
    stream.update(result_peg_936.getPosition());
    
    
    return result_peg_936;
    out_peg_938:
    
    if (column_peg_935.chunk12 == 0){
        column_peg_935.chunk12 = new Chunk12();
    }
    column_peg_935.chunk12->chunk_helper__expression = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_943 = stream.getColumn(position);
    if (column_peg_943.chunk12 != 0 && column_peg_943.chunk12->chunk_helper__name.calculated()){
        return column_peg_943.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_952(stream, "helper__name");
    int myposition = position;
    
    
    Result result_peg_944(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemynear"[i], stream.get(result_peg_944.getPosition()))){
            result_peg_944.nextPosition();
        } else {
            goto out_peg_945;
        }
    }
    result_peg_944.setValue((void*) "enemynear");
    
    if (column_peg_943.chunk12 == 0){
        column_peg_943.chunk12 = new Chunk12();
    }
    column_peg_943.chunk12->chunk_helper__name = result_peg_944;
    stream.update(result_peg_944.getPosition());
    
    
    return result_peg_944;
    out_peg_945:
    Result result_peg_946(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("target"[i], stream.get(result_peg_946.getPosition()))){
            result_peg_946.nextPosition();
        } else {
            goto out_peg_947;
        }
    }
    result_peg_946.setValue((void*) "target");
    
    if (column_peg_943.chunk12 == 0){
        column_peg_943.chunk12 = new Chunk12();
    }
    column_peg_943.chunk12->chunk_helper__name = result_peg_946;
    stream.update(result_peg_946.getPosition());
    
    
    return result_peg_946;
    out_peg_947:
    Result result_peg_948(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("parent"[i], stream.get(result_peg_948.getPosition()))){
            result_peg_948.nextPosition();
        } else {
            goto out_peg_949;
        }
    }
    result_peg_948.setValue((void*) "parent");
    
    if (column_peg_943.chunk12 == 0){
        column_peg_943.chunk12 = new Chunk12();
    }
    column_peg_943.chunk12->chunk_helper__name = result_peg_948;
    stream.update(result_peg_948.getPosition());
    
    
    return result_peg_948;
    out_peg_949:
    Result result_peg_950(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("helper"[i], stream.get(result_peg_950.getPosition()))){
            result_peg_950.nextPosition();
        } else {
            goto out_peg_951;
        }
    }
    result_peg_950.setValue((void*) "helper");
    
    if (column_peg_943.chunk12 == 0){
        column_peg_943.chunk12 = new Chunk12();
    }
    column_peg_943.chunk12->chunk_helper__name = result_peg_950;
    stream.update(result_peg_950.getPosition());
    
    
    return result_peg_950;
    out_peg_951:
    
    if (column_peg_943.chunk12 == 0){
        column_peg_943.chunk12 = new Chunk12();
    }
    column_peg_943.chunk12->chunk_helper__name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_953 = stream.getColumn(position);
    if (column_peg_953.chunk12 != 0 && column_peg_953.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_953.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_963(stream, "helper__identifier");
    int myposition = position;
    
    
    Result result_peg_954(myposition);
    
    result_peg_954 = rule_function(stream, result_peg_954.getPosition());
    if (result_peg_954.error()){
        goto out_peg_955;
    }
    
    if (column_peg_953.chunk12 == 0){
        column_peg_953.chunk12 = new Chunk12();
    }
    column_peg_953.chunk12->chunk_helper__identifier = result_peg_954;
    stream.update(result_peg_954.getPosition());
    
    
    return result_peg_954;
    out_peg_955:
    Result result_peg_956(myposition);
    
    result_peg_956 = rule_keyword(stream, result_peg_956.getPosition());
    if (result_peg_956.error()){
        goto out_peg_957;
    }
    
    if (column_peg_953.chunk12 == 0){
        column_peg_953.chunk12 = new Chunk12();
    }
    column_peg_953.chunk12->chunk_helper__identifier = result_peg_956;
    stream.update(result_peg_956.getPosition());
    
    
    return result_peg_956;
    out_peg_957:
    Result result_peg_958(myposition);
    
    {
    
        Result result_peg_961(result_peg_958);
            result_peg_961 = rule_keyword(stream, result_peg_961.getPosition());
            if (result_peg_961.error()){
                goto not_peg_960;
            }
            goto out_peg_962;
            not_peg_960:
            result_peg_958.setValue((void*)0);
        
        
        
        result_peg_958 = rule_identifier(stream, result_peg_958.getPosition());
            if (result_peg_958.error()){
                goto out_peg_962;
            }
        
        
    }
    
    if (column_peg_953.chunk12 == 0){
        column_peg_953.chunk12 = new Chunk12();
    }
    column_peg_953.chunk12->chunk_helper__identifier = result_peg_958;
    stream.update(result_peg_958.getPosition());
    
    
    return result_peg_958;
    out_peg_962:
    
    if (column_peg_953.chunk12 == 0){
        column_peg_953.chunk12 = new Chunk12();
    }
    column_peg_953.chunk12->chunk_helper__identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_964 = stream.getColumn(position);
    if (column_peg_964.chunk13 != 0 && column_peg_964.chunk13->chunk_hitflag.calculated()){
        return column_peg_964.chunk13->chunk_hitflag;
    }
    
    RuleTrace trace_peg_984(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_965(myposition);
    
    {
    
        result_peg_965.reset();
            do{
                Result result_peg_968(result_peg_965.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("A"[i], stream.get(result_peg_968.getPosition()))){
                            result_peg_968.nextPosition();
                        } else {
                            goto out_peg_971;
                        }
                    }
                    result_peg_968.setValue((void*) "A");
                        
                }
                goto success_peg_969;
                out_peg_971:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("M"[i], stream.get(result_peg_968.getPosition()))){
                            result_peg_968.nextPosition();
                        } else {
                            goto out_peg_973;
                        }
                    }
                    result_peg_968.setValue((void*) "M");
                        
                }
                goto success_peg_969;
                out_peg_973:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_968.getPosition()))){
                            result_peg_968.nextPosition();
                        } else {
                            goto out_peg_975;
                        }
                    }
                    result_peg_968.setValue((void*) "F");
                        
                }
                goto success_peg_969;
                out_peg_975:
                goto loop_peg_967;
                success_peg_969:
                ;
                result_peg_965.addResult(result_peg_968);
            } while (true);
            loop_peg_967:
            if (result_peg_965.matches() == 0){
                goto out_peg_976;
            }
        
        
        
        int save_peg_978 = result_peg_965.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_965.getPosition()))){
                        result_peg_965.nextPosition();
                    } else {
                        goto out_peg_981;
                    }
                }
                result_peg_965.setValue((void*) "+");
                    
            }
            goto success_peg_979;
            out_peg_981:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_965.getPosition()))){
                        result_peg_965.nextPosition();
                    } else {
                        goto out_peg_983;
                    }
                }
                result_peg_965.setValue((void*) "-");
                    
            }
            goto success_peg_979;
            out_peg_983:
            
            result_peg_965 = Result(save_peg_978);
            result_peg_965.setValue((void*) 0);
            
            success_peg_979:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = makeSimpleIdentifier("A");
                result_peg_965.setValue(value);
            }
        
        
    }
    
    if (column_peg_964.chunk13 == 0){
        column_peg_964.chunk13 = new Chunk13();
    }
    column_peg_964.chunk13->chunk_hitflag = result_peg_965;
    stream.update(result_peg_965.getPosition());
    
    
    return result_peg_965;
    out_peg_976:
    
    if (column_peg_964.chunk13 == 0){
        column_peg_964.chunk13 = new Chunk13();
    }
    column_peg_964.chunk13->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_985 = stream.getColumn(position);
    if (column_peg_985.chunk13 != 0 && column_peg_985.chunk13->chunk_keyword.calculated()){
        return column_peg_985.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_992(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_986(myposition);
    
    {
    
        result_peg_986 = rule_keyword_real(stream, result_peg_986.getPosition());
            if (result_peg_986.error()){
                goto out_peg_988;
            }
        
        Result result_peg_987 = result_peg_986;
        
        Result result_peg_991(result_peg_986);
            result_peg_991 = rule_alpha_digit(stream, result_peg_991.getPosition());
            if (result_peg_991.error()){
                goto not_peg_990;
            }
            goto out_peg_988;
            not_peg_990:
            result_peg_986.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_987.getValues();
                result_peg_986.setValue(value);
            }
        
        
    }
    
    if (column_peg_985.chunk13 == 0){
        column_peg_985.chunk13 = new Chunk13();
    }
    column_peg_985.chunk13->chunk_keyword = result_peg_986;
    stream.update(result_peg_986.getPosition());
    
    
    return result_peg_986;
    out_peg_988:
    
    if (column_peg_985.chunk13 == 0){
        column_peg_985.chunk13 = new Chunk13();
    }
    column_peg_985.chunk13->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_993 = stream.getColumn(position);
    if (column_peg_993.chunk13 != 0 && column_peg_993.chunk13->chunk_keyword_real.calculated()){
        return column_peg_993.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_1091(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_994(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_994.getPosition()))){
                    result_peg_994.nextPosition();
                } else {
                    goto out_peg_996;
                }
            }
            result_peg_994.setValue((void*) "vel");
        
        
        
        result_peg_994 = rule_s(stream, result_peg_994.getPosition());
            if (result_peg_994.error()){
                goto out_peg_996;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_994.getPosition()))){
                    result_peg_994.nextPosition();
                } else {
                    goto out_peg_996;
                }
            }
            result_peg_994.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_994.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_994;
    stream.update(result_peg_994.getPosition());
    
    
    return result_peg_994;
    out_peg_996:
    Result result_peg_999(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_999.getPosition()))){
                    result_peg_999.nextPosition();
                } else {
                    goto out_peg_1001;
                }
            }
            result_peg_999.setValue((void*) "vel");
        
        
        
        result_peg_999 = rule_s(stream, result_peg_999.getPosition());
            if (result_peg_999.error()){
                goto out_peg_1001;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_999.getPosition()))){
                    result_peg_999.nextPosition();
                } else {
                    goto out_peg_1001;
                }
            }
            result_peg_999.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_999.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_999;
    stream.update(result_peg_999.getPosition());
    
    
    return result_peg_999;
    out_peg_1001:
    Result result_peg_1004(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1004.getPosition()))){
                    result_peg_1004.nextPosition();
                } else {
                    goto out_peg_1006;
                }
            }
            result_peg_1004.setValue((void*) "pos");
        
        
        
        result_peg_1004 = rule_s(stream, result_peg_1004.getPosition());
            if (result_peg_1004.error()){
                goto out_peg_1006;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1004.getPosition()))){
                    result_peg_1004.nextPosition();
                } else {
                    goto out_peg_1006;
                }
            }
            result_peg_1004.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_1004.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1004;
    stream.update(result_peg_1004.getPosition());
    
    
    return result_peg_1004;
    out_peg_1006:
    Result result_peg_1009(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1009.getPosition()))){
                    result_peg_1009.nextPosition();
                } else {
                    goto out_peg_1011;
                }
            }
            result_peg_1009.setValue((void*) "pos");
        
        
        
        result_peg_1009 = rule_s(stream, result_peg_1009.getPosition());
            if (result_peg_1009.error()){
                goto out_peg_1011;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1009.getPosition()))){
                    result_peg_1009.nextPosition();
                } else {
                    goto out_peg_1011;
                }
            }
            result_peg_1009.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_1009.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1009;
    stream.update(result_peg_1009.getPosition());
    
    
    return result_peg_1009;
    out_peg_1011:
    Result result_peg_1014(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "p2dist");
        
        
        
        result_peg_1014 = rule_s(stream, result_peg_1014.getPosition());
            if (result_peg_1014.error()){
                goto out_peg_1016;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_1014.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1014;
    stream.update(result_peg_1014.getPosition());
    
    
    return result_peg_1014;
    out_peg_1016:
    Result result_peg_1019(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1019.getPosition()))){
                    result_peg_1019.nextPosition();
                } else {
                    goto out_peg_1021;
                }
            }
            result_peg_1019.setValue((void*) "p2dist");
        
        
        
        result_peg_1019 = rule_s(stream, result_peg_1019.getPosition());
            if (result_peg_1019.error()){
                goto out_peg_1021;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1019.getPosition()))){
                    result_peg_1019.nextPosition();
                } else {
                    goto out_peg_1021;
                }
            }
            result_peg_1019.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_1019.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1019;
    stream.update(result_peg_1019.getPosition());
    
    
    return result_peg_1019;
    out_peg_1021:
    Result result_peg_1024(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1024.getPosition()))){
                    result_peg_1024.nextPosition();
                } else {
                    goto out_peg_1026;
                }
            }
            result_peg_1024.setValue((void*) "p1dist");
        
        
        
        result_peg_1024 = rule_s(stream, result_peg_1024.getPosition());
            if (result_peg_1024.error()){
                goto out_peg_1026;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1024.getPosition()))){
                    result_peg_1024.nextPosition();
                } else {
                    goto out_peg_1026;
                }
            }
            result_peg_1024.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_1024.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1024;
    stream.update(result_peg_1024.getPosition());
    
    
    return result_peg_1024;
    out_peg_1026:
    Result result_peg_1029(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1029.getPosition()))){
                    result_peg_1029.nextPosition();
                } else {
                    goto out_peg_1031;
                }
            }
            result_peg_1029.setValue((void*) "p1dist");
        
        
        
        result_peg_1029 = rule_s(stream, result_peg_1029.getPosition());
            if (result_peg_1029.error()){
                goto out_peg_1031;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1029.getPosition()))){
                    result_peg_1029.nextPosition();
                } else {
                    goto out_peg_1031;
                }
            }
            result_peg_1029.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_1029.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1029;
    stream.update(result_peg_1029.getPosition());
    
    
    return result_peg_1029;
    out_peg_1031:
    Result result_peg_1034(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1034.getPosition()))){
                    result_peg_1034.nextPosition();
                } else {
                    goto out_peg_1036;
                }
            }
            result_peg_1034.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1034 = rule_s(stream, result_peg_1034.getPosition());
            if (result_peg_1034.error()){
                goto out_peg_1036;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1034.getPosition()))){
                    result_peg_1034.nextPosition();
                } else {
                    goto out_peg_1036;
                }
            }
            result_peg_1034.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1034.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1034;
    stream.update(result_peg_1034.getPosition());
    
    
    return result_peg_1034;
    out_peg_1036:
    Result result_peg_1039(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1039.getPosition()))){
                    result_peg_1039.nextPosition();
                } else {
                    goto out_peg_1041;
                }
            }
            result_peg_1039.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1039 = rule_s(stream, result_peg_1039.getPosition());
            if (result_peg_1039.error()){
                goto out_peg_1041;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1039.getPosition()))){
                    result_peg_1039.nextPosition();
                } else {
                    goto out_peg_1041;
                }
            }
            result_peg_1039.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1039.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1039;
    stream.update(result_peg_1039.getPosition());
    
    
    return result_peg_1039;
    out_peg_1041:
    Result result_peg_1044(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1044.getPosition()))){
                    result_peg_1044.nextPosition();
                } else {
                    goto out_peg_1046;
                }
            }
            result_peg_1044.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1044 = rule_s(stream, result_peg_1044.getPosition());
            if (result_peg_1044.error()){
                goto out_peg_1046;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1044.getPosition()))){
                    result_peg_1044.nextPosition();
                } else {
                    goto out_peg_1046;
                }
            }
            result_peg_1044.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1044.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1044;
    stream.update(result_peg_1044.getPosition());
    
    
    return result_peg_1044;
    out_peg_1046:
    Result result_peg_1049(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1049.getPosition()))){
                    result_peg_1049.nextPosition();
                } else {
                    goto out_peg_1051;
                }
            }
            result_peg_1049.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1049 = rule_s(stream, result_peg_1049.getPosition());
            if (result_peg_1049.error()){
                goto out_peg_1051;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1049.getPosition()))){
                    result_peg_1049.nextPosition();
                } else {
                    goto out_peg_1051;
                }
            }
            result_peg_1049.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1049.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1049;
    stream.update(result_peg_1049.getPosition());
    
    
    return result_peg_1049;
    out_peg_1051:
    Result result_peg_1054(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1054.getPosition()))){
                    result_peg_1054.nextPosition();
                } else {
                    goto out_peg_1056;
                }
            }
            result_peg_1054.setValue((void*) "parentdist");
        
        
        
        result_peg_1054 = rule_s(stream, result_peg_1054.getPosition());
            if (result_peg_1054.error()){
                goto out_peg_1056;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1054.getPosition()))){
                    result_peg_1054.nextPosition();
                } else {
                    goto out_peg_1056;
                }
            }
            result_peg_1054.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1054.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1054;
    stream.update(result_peg_1054.getPosition());
    
    
    return result_peg_1054;
    out_peg_1056:
    Result result_peg_1059(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1059.getPosition()))){
                    result_peg_1059.nextPosition();
                } else {
                    goto out_peg_1061;
                }
            }
            result_peg_1059.setValue((void*) "screenpos");
        
        
        
        result_peg_1059 = rule_s(stream, result_peg_1059.getPosition());
            if (result_peg_1059.error()){
                goto out_peg_1061;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1059.getPosition()))){
                    result_peg_1059.nextPosition();
                } else {
                    goto out_peg_1061;
                }
            }
            result_peg_1059.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_1059.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1059;
    stream.update(result_peg_1059.getPosition());
    
    
    return result_peg_1059;
    out_peg_1061:
    Result result_peg_1064(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1064.getPosition()))){
                    result_peg_1064.nextPosition();
                } else {
                    goto out_peg_1066;
                }
            }
            result_peg_1064.setValue((void*) "screenpos");
        
        
        
        result_peg_1064 = rule_s(stream, result_peg_1064.getPosition());
            if (result_peg_1064.error()){
                goto out_peg_1066;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1064.getPosition()))){
                    result_peg_1064.nextPosition();
                } else {
                    goto out_peg_1066;
                }
            }
            result_peg_1064.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1064.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1064;
    stream.update(result_peg_1064.getPosition());
    
    
    return result_peg_1064;
    out_peg_1066:
    Result result_peg_1069(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1069.getPosition()))){
                    result_peg_1069.nextPosition();
                } else {
                    goto out_peg_1071;
                }
            }
            result_peg_1069.setValue((void*) "parentdist");
        
        
        
        result_peg_1069 = rule_s(stream, result_peg_1069.getPosition());
            if (result_peg_1069.error()){
                goto out_peg_1071;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1069.getPosition()))){
                    result_peg_1069.nextPosition();
                } else {
                    goto out_peg_1071;
                }
            }
            result_peg_1069.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1069.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1069;
    stream.update(result_peg_1069.getPosition());
    
    
    return result_peg_1069;
    out_peg_1071:
    Result result_peg_1074(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1074.getPosition()))){
                    result_peg_1074.nextPosition();
                } else {
                    goto out_peg_1076;
                }
            }
            result_peg_1074.setValue((void*) "parentdist");
        
        
        
        result_peg_1074 = rule_s(stream, result_peg_1074.getPosition());
            if (result_peg_1074.error()){
                goto out_peg_1076;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1074.getPosition()))){
                    result_peg_1074.nextPosition();
                } else {
                    goto out_peg_1076;
                }
            }
            result_peg_1074.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist y");
                result_peg_1074.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1074;
    stream.update(result_peg_1074.getPosition());
    
    
    return result_peg_1074;
    out_peg_1076:
    Result result_peg_1079(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_1079.getPosition()))){
                    result_peg_1079.nextPosition();
                } else {
                    goto out_peg_1081;
                }
            }
            result_peg_1079.setValue((void*) "rootdist");
        
        
        
        result_peg_1079 = rule_s(stream, result_peg_1079.getPosition());
            if (result_peg_1079.error()){
                goto out_peg_1081;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1079.getPosition()))){
                    result_peg_1079.nextPosition();
                } else {
                    goto out_peg_1081;
                }
            }
            result_peg_1079.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist x");
                result_peg_1079.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1079;
    stream.update(result_peg_1079.getPosition());
    
    
    return result_peg_1079;
    out_peg_1081:
    Result result_peg_1084(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_1084.getPosition()))){
                    result_peg_1084.nextPosition();
                } else {
                    goto out_peg_1086;
                }
            }
            result_peg_1084.setValue((void*) "rootdist");
        
        
        
        result_peg_1084 = rule_s(stream, result_peg_1084.getPosition());
            if (result_peg_1084.error()){
                goto out_peg_1086;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1084.getPosition()))){
                    result_peg_1084.nextPosition();
                } else {
                    goto out_peg_1086;
                }
            }
            result_peg_1084.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist y");
                result_peg_1084.setValue(value);
            }
        
        
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1084;
    stream.update(result_peg_1084.getPosition());
    
    
    return result_peg_1084;
    out_peg_1086:
    Result result_peg_1089(myposition);
    
    result_peg_1089 = rule_hitflag(stream, result_peg_1089.getPosition());
    if (result_peg_1089.error()){
        goto out_peg_1090;
    }
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = result_peg_1089;
    stream.update(result_peg_1089.getPosition());
    
    
    return result_peg_1089;
    out_peg_1090:
    
    if (column_peg_993.chunk13 == 0){
        column_peg_993.chunk13 = new Chunk13();
    }
    column_peg_993.chunk13->chunk_keyword_real = errorResult;
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

        
