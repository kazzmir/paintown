

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
    Result chunk_args;
};

struct Chunk10{
    Result chunk_function_name;
    Result chunk_function_rest;
    Result chunk_keys;
    Result chunk_key_value_list;
    Result chunk_key;
};

struct Chunk11{
    Result chunk_key_rest;
    Result chunk_key_real;
    Result chunk_key_modifier;
    Result chunk_key_name;
    Result chunk_value;
};

struct Chunk12{
    Result chunk_helper;
    Result chunk_helper__expression;
    Result chunk_helper__name;
    Result chunk_helper__identifier;
    Result chunk_hitflag;
};

struct Chunk13{
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
(chunk13 != NULL ? ((chunk13->chunk_keyword.calculated() ? 1 : 0)
+ (chunk13->chunk_keyword_real.calculated() ? 1 : 0)) : 0)
+
(chunk12 != NULL ? ((chunk12->chunk_helper.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__expression.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__name.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__identifier.calculated() ? 1 : 0)
+ (chunk12->chunk_hitflag.calculated() ? 1 : 0)) : 0)
+
(chunk11 != NULL ? ((chunk11->chunk_key_rest.calculated() ? 1 : 0)
+ (chunk11->chunk_key_real.calculated() ? 1 : 0)
+ (chunk11->chunk_key_modifier.calculated() ? 1 : 0)
+ (chunk11->chunk_key_name.calculated() ? 1 : 0)
+ (chunk11->chunk_value.calculated() ? 1 : 0)) : 0)
+
(chunk10 != NULL ? ((chunk10->chunk_function_name.calculated() ? 1 : 0)
+ (chunk10->chunk_function_rest.calculated() ? 1 : 0)
+ (chunk10->chunk_keys.calculated() ? 1 : 0)
+ (chunk10->chunk_key_value_list.calculated() ? 1 : 0)
+ (chunk10->chunk_key.calculated() ? 1 : 0)) : 0)
+
(chunk9 != NULL ? ((chunk9->chunk_expr12_rest.calculated() ? 1 : 0)
+ (chunk9->chunk_unary.calculated() ? 1 : 0)
+ (chunk9->chunk_expr13_real.calculated() ? 1 : 0)
+ (chunk9->chunk_function.calculated() ? 1 : 0)
+ (chunk9->chunk_args.calculated() ? 1 : 0)) : 0)
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
        return 67;
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
    
    RuleTrace trace_peg_86(stream, "line");
    int myposition = position;
    
    
    Result result_peg_23(myposition);
    
    {
    
        {
                
                result_peg_23.reset();
                do{
                    Result result_peg_35(result_peg_23.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_35.getPosition()))){
                                result_peg_35.nextPosition();
                            } else {
                                goto out_peg_38;
                            }
                        }
                        result_peg_35.setValue((void*) " ");
                            
                    }
                    goto success_peg_36;
                    out_peg_38:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_35.getPosition()))){
                                result_peg_35.nextPosition();
                            } else {
                                goto out_peg_40;
                            }
                        }
                        result_peg_35.setValue((void*) "\t");
                            
                    }
                    goto success_peg_36;
                    out_peg_40:
                    goto loop_peg_34;
                    success_peg_36:
                    ;
                    result_peg_23.addResult(result_peg_35);
                } while (true);
                loop_peg_34:
                ;
                        
            }
            goto success_peg_25;
            goto out_peg_41;
            success_peg_25:
            ;
        
        
        
        result_peg_23 = rule_comment(stream, result_peg_23.getPosition());
            if (result_peg_23.error()){
                goto out_peg_41;
            }
        
        
    }
    stream.update(result_peg_23.getPosition());
    
    return result_peg_23;
    out_peg_41:
    Result result_peg_42(myposition);
    
    {
    
        {
                
                result_peg_42.reset();
                do{
                    Result result_peg_54(result_peg_42.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_54.getPosition()))){
                                result_peg_54.nextPosition();
                            } else {
                                goto out_peg_57;
                            }
                        }
                        result_peg_54.setValue((void*) " ");
                            
                    }
                    goto success_peg_55;
                    out_peg_57:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_54.getPosition()))){
                                result_peg_54.nextPosition();
                            } else {
                                goto out_peg_59;
                            }
                        }
                        result_peg_54.setValue((void*) "\t");
                            
                    }
                    goto success_peg_55;
                    out_peg_59:
                    goto loop_peg_53;
                    success_peg_55:
                    ;
                    result_peg_42.addResult(result_peg_54);
                } while (true);
                loop_peg_53:
                ;
                        
            }
            goto success_peg_44;
            goto out_peg_60;
            success_peg_44:
            ;
        
        
        
        result_peg_42 = rule_section(stream, result_peg_42.getPosition());
            if (result_peg_42.error()){
                goto out_peg_60;
            }
        
        Result result_peg_61 = result_peg_42;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_61.getValues());
                result_peg_42.setValue(value);
            }
        
        
    }
    stream.update(result_peg_42.getPosition());
    
    return result_peg_42;
    out_peg_60:
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
        
        
        
        {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\n"[i], stream.get(result_peg_62.getPosition()))){
                        result_peg_62.nextPosition();
                    } else {
                        goto out_peg_83;
                    }
                }
                result_peg_62.setValue((void*) "\n");
                    
            }
            goto success_peg_81;
            out_peg_83:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("\r"[i], stream.get(result_peg_62.getPosition()))){
                        result_peg_62.nextPosition();
                    } else {
                        goto out_peg_85;
                    }
                }
                result_peg_62.setValue((void*) "\r");
                    
            }
            goto success_peg_81;
            out_peg_85:
            goto out_peg_80;
            success_peg_81:
            ;
        
        
    }
    stream.update(result_peg_62.getPosition());
    
    return result_peg_62;
    out_peg_80:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_87 = stream.getColumn(position);
    if (column_peg_87.chunk0 != 0 && column_peg_87.chunk0->chunk_line_end.calculated()){
        return column_peg_87.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_101(stream, "line_end");
    int myposition = position;
    
    
    Result result_peg_88(myposition);
    
    result_peg_88.reset();
    do{
        Result result_peg_90(result_peg_88.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_90.getPosition()))){
                    result_peg_90.nextPosition();
                } else {
                    goto out_peg_93;
                }
            }
            result_peg_90.setValue((void*) "\n");
                
        }
        goto success_peg_91;
        out_peg_93:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_90.getPosition()))){
                    result_peg_90.nextPosition();
                } else {
                    goto out_peg_95;
                }
            }
            result_peg_90.setValue((void*) "\r");
                
        }
        goto success_peg_91;
        out_peg_95:
        goto loop_peg_89;
        success_peg_91:
        ;
        result_peg_88.addResult(result_peg_90);
    } while (true);
    loop_peg_89:
    if (result_peg_88.matches() == 0){
        goto out_peg_96;
    }
    
    if (column_peg_87.chunk0 == 0){
        column_peg_87.chunk0 = new Chunk0();
    }
    column_peg_87.chunk0->chunk_line_end = result_peg_88;
    stream.update(result_peg_88.getPosition());
    
    
    return result_peg_88;
    out_peg_96:
    Result result_peg_97(myposition);
    
    {
    
        Result result_peg_99(result_peg_97.getPosition());
            if ('\0' == stream.get(result_peg_99.getPosition())){
                result_peg_99.nextPosition();
                result_peg_99.setValue((void *) '\0');
            } else {
                goto out_peg_100;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_87.chunk0 == 0){
        column_peg_87.chunk0 = new Chunk0();
    }
    column_peg_87.chunk0->chunk_line_end = result_peg_97;
    stream.update(result_peg_97.getPosition());
    
    
    return result_peg_97;
    out_peg_100:
    
    if (column_peg_87.chunk0 == 0){
        column_peg_87.chunk0 = new Chunk0();
    }
    column_peg_87.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_102 = stream.getColumn(position);
    if (column_peg_102.chunk0 != 0 && column_peg_102.chunk0->chunk_whitespace.calculated()){
        return column_peg_102.chunk0->chunk_whitespace;
    }
    
    RuleTrace trace_peg_106(stream, "whitespace");
    int myposition = position;
    
    
    Result result_peg_103(myposition);
    
    result_peg_103.reset();
    do{
        Result result_peg_105(result_peg_103.getPosition());
        result_peg_105 = rule_sw(stream, result_peg_105.getPosition());
        if (result_peg_105.error()){
            goto loop_peg_104;
        }
        result_peg_103.addResult(result_peg_105);
    } while (true);
    loop_peg_104:
    ;
    
    if (column_peg_102.chunk0 == 0){
        column_peg_102.chunk0 = new Chunk0();
    }
    column_peg_102.chunk0->chunk_whitespace = result_peg_103;
    stream.update(result_peg_103.getPosition());
    
    
    return result_peg_103;
    
    if (column_peg_102.chunk0 == 0){
        column_peg_102.chunk0 = new Chunk0();
    }
    column_peg_102.chunk0->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_107 = stream.getColumn(position);
    if (column_peg_107.chunk0 != 0 && column_peg_107.chunk0->chunk_sw.calculated()){
        return column_peg_107.chunk0->chunk_sw;
    }
    
    RuleTrace trace_peg_117(stream, "sw");
    int myposition = position;
    
    
    Result result_peg_108(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_108.getPosition()))){
                result_peg_108.nextPosition();
            } else {
                goto out_peg_111;
            }
        }
        result_peg_108.setValue((void*) " ");
            
    }
    goto success_peg_109;
    out_peg_111:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_108.getPosition()))){
                result_peg_108.nextPosition();
            } else {
                goto out_peg_113;
            }
        }
        result_peg_108.setValue((void*) "\t");
            
    }
    goto success_peg_109;
    out_peg_113:
    goto out_peg_114;
    success_peg_109:
    ;
    
    if (column_peg_107.chunk0 == 0){
        column_peg_107.chunk0 = new Chunk0();
    }
    column_peg_107.chunk0->chunk_sw = result_peg_108;
    stream.update(result_peg_108.getPosition());
    
    
    return result_peg_108;
    out_peg_114:
    Result result_peg_115(myposition);
    
    result_peg_115 = rule_comment(stream, result_peg_115.getPosition());
    if (result_peg_115.error()){
        goto out_peg_116;
    }
    
    if (column_peg_107.chunk0 == 0){
        column_peg_107.chunk0 = new Chunk0();
    }
    column_peg_107.chunk0->chunk_sw = result_peg_115;
    stream.update(result_peg_115.getPosition());
    
    
    return result_peg_115;
    out_peg_116:
    
    if (column_peg_107.chunk0 == 0){
        column_peg_107.chunk0 = new Chunk0();
    }
    column_peg_107.chunk0->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_118 = stream.getColumn(position);
    if (column_peg_118.chunk1 != 0 && column_peg_118.chunk1->chunk_comment.calculated()){
        return column_peg_118.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_147(stream, "comment");
    int myposition = position;
    
    
    Result result_peg_119(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_119.getPosition()))){
                    result_peg_119.nextPosition();
                } else {
                    goto out_peg_121;
                }
            }
            result_peg_119.setValue((void*) ";");
        
        
        
        result_peg_119.reset();
            do{
                Result result_peg_123(result_peg_119.getPosition());
                {
                
                    Result result_peg_126(result_peg_123);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_126.getPosition()))){
                                result_peg_126.nextPosition();
                            } else {
                                goto not_peg_125;
                            }
                        }
                        result_peg_126.setValue((void*) "\n");
                        goto loop_peg_122;
                        not_peg_125:
                        result_peg_123.setValue((void*)0);
                    
                    
                    
                    char temp_peg_127 = stream.get(result_peg_123.getPosition());
                        if (temp_peg_127 != '\0'){
                            result_peg_123.setValue((void*) (long) temp_peg_127);
                            result_peg_123.nextPosition();
                        } else {
                            goto loop_peg_122;
                        }
                    
                    
                }
                result_peg_119.addResult(result_peg_123);
            } while (true);
            loop_peg_122:
            ;
        
        
    }
    
    if (column_peg_118.chunk1 == 0){
        column_peg_118.chunk1 = new Chunk1();
    }
    column_peg_118.chunk1->chunk_comment = result_peg_119;
    stream.update(result_peg_119.getPosition());
    
    
    return result_peg_119;
    out_peg_121:
    Result result_peg_128(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_128.getPosition()))){
                    result_peg_128.nextPosition();
                } else {
                    goto out_peg_130;
                }
            }
            result_peg_128.setValue((void*) "=");
        
        
        
        result_peg_128.reset();
            do{
                Result result_peg_132(result_peg_128.getPosition());
                {
                
                    Result result_peg_135(result_peg_132);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_135.getPosition()))){
                                result_peg_135.nextPosition();
                            } else {
                                goto not_peg_134;
                            }
                        }
                        result_peg_135.setValue((void*) "\n");
                        goto loop_peg_131;
                        not_peg_134:
                        result_peg_132.setValue((void*)0);
                    
                    
                    
                    char temp_peg_136 = stream.get(result_peg_132.getPosition());
                        if (temp_peg_136 != '\0'){
                            result_peg_132.setValue((void*) (long) temp_peg_136);
                            result_peg_132.nextPosition();
                        } else {
                            goto loop_peg_131;
                        }
                    
                    
                }
                result_peg_128.addResult(result_peg_132);
            } while (true);
            loop_peg_131:
            ;
        
        
    }
    
    if (column_peg_118.chunk1 == 0){
        column_peg_118.chunk1 = new Chunk1();
    }
    column_peg_118.chunk1->chunk_comment = result_peg_128;
    stream.update(result_peg_128.getPosition());
    
    
    return result_peg_128;
    out_peg_130:
    Result result_peg_137(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_137.getPosition()))){
                    result_peg_137.nextPosition();
                } else {
                    goto out_peg_139;
                }
            }
            result_peg_137.setValue((void*) "-");
        
        
        
        result_peg_137.reset();
            do{
                Result result_peg_141(result_peg_137.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_141.getPosition()))){
                        result_peg_141.nextPosition();
                    } else {
                        goto loop_peg_140;
                    }
                }
                result_peg_141.setValue((void*) "-");
                result_peg_137.addResult(result_peg_141);
            } while (true);
            loop_peg_140:
            if (result_peg_137.matches() == 0){
                goto out_peg_139;
            }
        
        
    }
    
    if (column_peg_118.chunk1 == 0){
        column_peg_118.chunk1 = new Chunk1();
    }
    column_peg_118.chunk1->chunk_comment = result_peg_137;
    stream.update(result_peg_137.getPosition());
    
    
    return result_peg_137;
    out_peg_139:
    Result result_peg_142(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_142.getPosition()))){
                    result_peg_142.nextPosition();
                } else {
                    goto out_peg_144;
                }
            }
            result_peg_142.setValue((void*) ":");
        
        
        
        result_peg_142.reset();
            do{
                Result result_peg_146(result_peg_142.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_146.getPosition()))){
                        result_peg_146.nextPosition();
                    } else {
                        goto loop_peg_145;
                    }
                }
                result_peg_146.setValue((void*) ":");
                result_peg_142.addResult(result_peg_146);
            } while (true);
            loop_peg_145:
            if (result_peg_142.matches() == 0){
                goto out_peg_144;
            }
        
        
    }
    
    if (column_peg_118.chunk1 == 0){
        column_peg_118.chunk1 = new Chunk1();
    }
    column_peg_118.chunk1->chunk_comment = result_peg_142;
    stream.update(result_peg_142.getPosition());
    
    
    return result_peg_142;
    out_peg_144:
    
    if (column_peg_118.chunk1 == 0){
        column_peg_118.chunk1 = new Chunk1();
    }
    column_peg_118.chunk1->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_148 = stream.getColumn(position);
    if (column_peg_148.chunk1 != 0 && column_peg_148.chunk1->chunk_section.calculated()){
        return column_peg_148.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_167(stream, "section");
    int myposition = position;
    
    Value ast;
    Result result_peg_149(myposition);
    
    {
    
        result_peg_149 = rule_section_title(stream, result_peg_149.getPosition());
            if (result_peg_149.error()){
                goto out_peg_151;
            }
        
        Result result_peg_150 = result_peg_149;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_150.getValues());
                result_peg_149.setValue(value);
            }
            ast = result_peg_149.getValues();
        
        
        
        result_peg_149 = rule_whitespace(stream, result_peg_149.getPosition());
            if (result_peg_149.error()){
                goto out_peg_151;
            }
        
        
        
        result_peg_149.reset();
            do{
                Result result_peg_156(result_peg_149.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_156.getPosition()))){
                            result_peg_156.nextPosition();
                        } else {
                            goto out_peg_159;
                        }
                    }
                    result_peg_156.setValue((void*) "\n");
                        
                }
                goto success_peg_157;
                out_peg_159:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_156.getPosition()))){
                            result_peg_156.nextPosition();
                        } else {
                            goto out_peg_161;
                        }
                    }
                    result_peg_156.setValue((void*) "\r");
                        
                }
                goto success_peg_157;
                out_peg_161:
                goto loop_peg_155;
                success_peg_157:
                ;
                result_peg_149.addResult(result_peg_156);
            } while (true);
            loop_peg_155:
            if (result_peg_149.matches() == 0){
                goto out_peg_151;
            }
        
        
        
        result_peg_149.reset();
            do{
                Result result_peg_164(result_peg_149.getPosition());
                {
                
                    result_peg_164 = rule_section_item(stream, result_peg_164.getPosition(), ast);
                        if (result_peg_164.error()){
                            goto loop_peg_163;
                        }
                    
                    
                    
                    result_peg_164 = rule_whitespace(stream, result_peg_164.getPosition());
                        if (result_peg_164.error()){
                            goto loop_peg_163;
                        }
                    
                    
                    
                    result_peg_164 = rule_line_end(stream, result_peg_164.getPosition());
                        if (result_peg_164.error()){
                            goto loop_peg_163;
                        }
                    
                    
                }
                result_peg_149.addResult(result_peg_164);
            } while (true);
            loop_peg_163:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_149.setValue(value);
            }
        
        
    }
    
    if (column_peg_148.chunk1 == 0){
        column_peg_148.chunk1 = new Chunk1();
    }
    column_peg_148.chunk1->chunk_section = result_peg_149;
    stream.update(result_peg_149.getPosition());
    
    
    return result_peg_149;
    out_peg_151:
    
    if (column_peg_148.chunk1 == 0){
        column_peg_148.chunk1 = new Chunk1();
    }
    column_peg_148.chunk1->chunk_section = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_208(stream, "section_item");
    int myposition = position;
    
    Value data;
    Result result_peg_169(myposition);
    
    {
    
        {
                
                result_peg_169.reset();
                do{
                    Result result_peg_181(result_peg_169.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_181.getPosition()))){
                                result_peg_181.nextPosition();
                            } else {
                                goto out_peg_184;
                            }
                        }
                        result_peg_181.setValue((void*) " ");
                            
                    }
                    goto success_peg_182;
                    out_peg_184:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_181.getPosition()))){
                                result_peg_181.nextPosition();
                            } else {
                                goto out_peg_186;
                            }
                        }
                        result_peg_181.setValue((void*) "\t");
                            
                    }
                    goto success_peg_182;
                    out_peg_186:
                    goto loop_peg_180;
                    success_peg_182:
                    ;
                    result_peg_169.addResult(result_peg_181);
                } while (true);
                loop_peg_180:
                ;
                        
            }
            goto success_peg_171;
            goto out_peg_187;
            success_peg_171:
            ;
        
        
        
        result_peg_169 = rule_comment(stream, result_peg_169.getPosition());
            if (result_peg_169.error()){
                goto out_peg_187;
            }
        
        
    }
    stream.update(result_peg_169.getPosition());
    
    return result_peg_169;
    out_peg_187:
    Result result_peg_188(myposition);
    
    {
    
        {
                
                result_peg_188.reset();
                do{
                    Result result_peg_200(result_peg_188.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_200.getPosition()))){
                                result_peg_200.nextPosition();
                            } else {
                                goto out_peg_203;
                            }
                        }
                        result_peg_200.setValue((void*) " ");
                            
                    }
                    goto success_peg_201;
                    out_peg_203:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_200.getPosition()))){
                                result_peg_200.nextPosition();
                            } else {
                                goto out_peg_205;
                            }
                        }
                        result_peg_200.setValue((void*) "\t");
                            
                    }
                    goto success_peg_201;
                    out_peg_205:
                    goto loop_peg_199;
                    success_peg_201:
                    ;
                    result_peg_188.addResult(result_peg_200);
                } while (true);
                loop_peg_199:
                ;
                        
            }
            goto success_peg_190;
            goto out_peg_206;
            success_peg_190:
            ;
        
        
        
        result_peg_188 = rule_assignment(stream, result_peg_188.getPosition());
            if (result_peg_188.error()){
                goto out_peg_206;
            }
            data = result_peg_188.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_188.setValue(value);
            }
        
        
    }
    stream.update(result_peg_188.getPosition());
    
    return result_peg_188;
    out_peg_206:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_209 = stream.getColumn(position);
    if (column_peg_209.chunk1 != 0 && column_peg_209.chunk1->chunk_section_title.calculated()){
        return column_peg_209.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_238(stream, "section_title");
    int myposition = position;
    
    Value name;
    Result result_peg_210(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_210.getPosition()))){
                    result_peg_210.nextPosition();
                } else {
                    goto out_peg_212;
                }
            }
            result_peg_210.setValue((void*) "[");
        
        
        
        {
                
                result_peg_210.reset();
                do{
                    Result result_peg_224(result_peg_210.getPosition());
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
                    goto loop_peg_223;
                    success_peg_225:
                    ;
                    result_peg_210.addResult(result_peg_224);
                } while (true);
                loop_peg_223:
                ;
                        
            }
            goto success_peg_214;
            goto out_peg_212;
            success_peg_214:
            ;
        
        
        
        result_peg_210.reset();
            do{
                Result result_peg_232(result_peg_210.getPosition());
                {
                
                    Result result_peg_235(result_peg_232);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_235.getPosition()))){
                                result_peg_235.nextPosition();
                            } else {
                                goto not_peg_234;
                            }
                        }
                        result_peg_235.setValue((void*) "]");
                        goto loop_peg_231;
                        not_peg_234:
                        result_peg_232.setValue((void*)0);
                    
                    
                    
                    char temp_peg_236 = stream.get(result_peg_232.getPosition());
                        if (temp_peg_236 != '\0'){
                            result_peg_232.setValue((void*) (long) temp_peg_236);
                            result_peg_232.nextPosition();
                        } else {
                            goto loop_peg_231;
                        }
                    
                    
                }
                result_peg_210.addResult(result_peg_232);
            } while (true);
            loop_peg_231:
            if (result_peg_210.matches() == 0){
                goto out_peg_212;
            }
            name = result_peg_210.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_210.getPosition()))){
                    result_peg_210.nextPosition();
                } else {
                    goto out_peg_212;
                }
            }
            result_peg_210.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(name);
                result_peg_210.setValue(value);
            }
        
        
    }
    
    if (column_peg_209.chunk1 == 0){
        column_peg_209.chunk1 = new Chunk1();
    }
    column_peg_209.chunk1->chunk_section_title = result_peg_210;
    stream.update(result_peg_210.getPosition());
    
    
    return result_peg_210;
    out_peg_212:
    
    if (column_peg_209.chunk1 == 0){
        column_peg_209.chunk1 = new Chunk1();
    }
    column_peg_209.chunk1->chunk_section_title = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_239 = stream.getColumn(position);
    if (column_peg_239.chunk1 != 0 && column_peg_239.chunk1->chunk_assignment.calculated()){
        return column_peg_239.chunk1->chunk_assignment;
    }
    
    RuleTrace trace_peg_538(stream, "assignment");
    int myposition = position;
    
    Value all;
    Value line;
    Value exp;
    Value name;
    Value index;
    Result result_peg_240(myposition);
    
    {
    
        for (int i = 0; i < 7; i++){
                if (compareCharCase("command"[i], stream.get(result_peg_240.getPosition()))){
                    result_peg_240.nextPosition();
                } else {
                    goto out_peg_242;
                }
            }
            result_peg_240.setValue((void*) "command");
        
        
        
        {
                
                result_peg_240.reset();
                do{
                    Result result_peg_254(result_peg_240.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_254.getPosition()))){
                                result_peg_254.nextPosition();
                            } else {
                                goto out_peg_257;
                            }
                        }
                        result_peg_254.setValue((void*) " ");
                            
                    }
                    goto success_peg_255;
                    out_peg_257:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_254.getPosition()))){
                                result_peg_254.nextPosition();
                            } else {
                                goto out_peg_259;
                            }
                        }
                        result_peg_254.setValue((void*) "\t");
                            
                    }
                    goto success_peg_255;
                    out_peg_259:
                    goto loop_peg_253;
                    success_peg_255:
                    ;
                    result_peg_240.addResult(result_peg_254);
                } while (true);
                loop_peg_253:
                ;
                        
            }
            goto success_peg_244;
            goto out_peg_242;
            success_peg_244:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_240.getPosition()))){
                    result_peg_240.nextPosition();
                } else {
                    goto out_peg_242;
                }
            }
            result_peg_240.setValue((void*) "=");
        
        
        
        {
                
                result_peg_240.reset();
                do{
                    Result result_peg_272(result_peg_240.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_272.getPosition()))){
                                result_peg_272.nextPosition();
                            } else {
                                goto out_peg_275;
                            }
                        }
                        result_peg_272.setValue((void*) " ");
                            
                    }
                    goto success_peg_273;
                    out_peg_275:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_272.getPosition()))){
                                result_peg_272.nextPosition();
                            } else {
                                goto out_peg_277;
                            }
                        }
                        result_peg_272.setValue((void*) "\t");
                            
                    }
                    goto success_peg_273;
                    out_peg_277:
                    goto loop_peg_271;
                    success_peg_273:
                    ;
                    result_peg_240.addResult(result_peg_272);
                } while (true);
                loop_peg_271:
                ;
                        
            }
            goto success_peg_262;
            goto out_peg_242;
            success_peg_262:
            ;
        
        
        
        result_peg_240 = rule_keys(stream, result_peg_240.getPosition());
            if (result_peg_240.error()){
                goto out_peg_242;
            }
            all = result_peg_240.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("command", all);
                result_peg_240.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_240;
    stream.update(result_peg_240.getPosition());
    
    
    return result_peg_240;
    out_peg_242:
    Result result_peg_279(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_281 = stream.getLineInfo(result_peg_279.getPosition());
            line = &line_info_peg_281;
        
        
        
        for (int i = 0; i < 7; i++){
                if (compareCharCase("sparkno"[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_283;
                }
            }
            result_peg_279.setValue((void*) "sparkno");
        
        
        
        {
                
                result_peg_279.reset();
                do{
                    Result result_peg_295(result_peg_279.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_295.getPosition()))){
                                result_peg_295.nextPosition();
                            } else {
                                goto out_peg_298;
                            }
                        }
                        result_peg_295.setValue((void*) " ");
                            
                    }
                    goto success_peg_296;
                    out_peg_298:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_295.getPosition()))){
                                result_peg_295.nextPosition();
                            } else {
                                goto out_peg_300;
                            }
                        }
                        result_peg_295.setValue((void*) "\t");
                            
                    }
                    goto success_peg_296;
                    out_peg_300:
                    goto loop_peg_294;
                    success_peg_296:
                    ;
                    result_peg_279.addResult(result_peg_295);
                } while (true);
                loop_peg_294:
                ;
                        
            }
            goto success_peg_285;
            goto out_peg_283;
            success_peg_285:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_283;
                }
            }
            result_peg_279.setValue((void*) "=");
        
        
        
        {
                
                result_peg_279.reset();
                do{
                    Result result_peg_313(result_peg_279.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_313.getPosition()))){
                                result_peg_313.nextPosition();
                            } else {
                                goto out_peg_316;
                            }
                        }
                        result_peg_313.setValue((void*) " ");
                            
                    }
                    goto success_peg_314;
                    out_peg_316:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_313.getPosition()))){
                                result_peg_313.nextPosition();
                            } else {
                                goto out_peg_318;
                            }
                        }
                        result_peg_313.setValue((void*) "\t");
                            
                    }
                    goto success_peg_314;
                    out_peg_318:
                    goto loop_peg_312;
                    success_peg_314:
                    ;
                    result_peg_279.addResult(result_peg_313);
                } while (true);
                loop_peg_312:
                ;
                        
            }
            goto success_peg_303;
            goto out_peg_283;
            success_peg_303:
            ;
        
        
        
        int save_peg_320 = result_peg_279.getPosition();
            
            for (int i = 0; i < 1; i++){
                if (compareCharCase("s"[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    
                    result_peg_279 = Result(save_peg_320);
                    result_peg_279.setValue((void*) 0);
                    
                }
            }
            result_peg_279.setValue((void*) "s");
        
        
        
        result_peg_279 = rule_expr(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_283;
            }
            exp = result_peg_279.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("sparkno", exp);
                result_peg_279.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_279;
    stream.update(result_peg_279.getPosition());
    
    
    return result_peg_279;
    out_peg_283:
    Result result_peg_322(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_324 = stream.getLineInfo(result_peg_322.getPosition());
            line = &line_info_peg_324;
        
        
        
        result_peg_322 = rule_identifier(stream, result_peg_322.getPosition());
            if (result_peg_322.error()){
                goto out_peg_326;
            }
            name = result_peg_322.getValues();
        
        
        
        {
                
                result_peg_322.reset();
                do{
                    Result result_peg_338(result_peg_322.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_338.getPosition()))){
                                result_peg_338.nextPosition();
                            } else {
                                goto out_peg_341;
                            }
                        }
                        result_peg_338.setValue((void*) " ");
                            
                    }
                    goto success_peg_339;
                    out_peg_341:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_338.getPosition()))){
                                result_peg_338.nextPosition();
                            } else {
                                goto out_peg_343;
                            }
                        }
                        result_peg_338.setValue((void*) "\t");
                            
                    }
                    goto success_peg_339;
                    out_peg_343:
                    goto loop_peg_337;
                    success_peg_339:
                    ;
                    result_peg_322.addResult(result_peg_338);
                } while (true);
                loop_peg_337:
                ;
                        
            }
            goto success_peg_328;
            goto out_peg_326;
            success_peg_328:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_322.getPosition()))){
                    result_peg_322.nextPosition();
                } else {
                    goto out_peg_326;
                }
            }
            result_peg_322.setValue((void*) "=");
        
        
        
        {
                
                result_peg_322.reset();
                do{
                    Result result_peg_356(result_peg_322.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_356.getPosition()))){
                                result_peg_356.nextPosition();
                            } else {
                                goto out_peg_359;
                            }
                        }
                        result_peg_356.setValue((void*) " ");
                            
                    }
                    goto success_peg_357;
                    out_peg_359:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_356.getPosition()))){
                                result_peg_356.nextPosition();
                            } else {
                                goto out_peg_361;
                            }
                        }
                        result_peg_356.setValue((void*) "\t");
                            
                    }
                    goto success_peg_357;
                    out_peg_361:
                    goto loop_peg_355;
                    success_peg_357:
                    ;
                    result_peg_322.addResult(result_peg_356);
                } while (true);
                loop_peg_355:
                ;
                        
            }
            goto success_peg_346;
            goto out_peg_326;
            success_peg_346:
            ;
        
        
        
        result_peg_322 = rule_expr(stream, result_peg_322.getPosition());
            if (result_peg_322.error()){
                goto out_peg_326;
            }
            exp = result_peg_322.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(getCurrentLine(line), getCurrentColumn(line), name, exp);
                result_peg_322.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_322;
    stream.update(result_peg_322.getPosition());
    
    
    return result_peg_322;
    out_peg_326:
    Result result_peg_363(myposition);
    
    {
    
        result_peg_363 = rule_identifier(stream, result_peg_363.getPosition());
            if (result_peg_363.error()){
                goto out_peg_365;
            }
            name = result_peg_363.getValues();
        
        
        
        {
                
                result_peg_363.reset();
                do{
                    Result result_peg_377(result_peg_363.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_377.getPosition()))){
                                result_peg_377.nextPosition();
                            } else {
                                goto out_peg_380;
                            }
                        }
                        result_peg_377.setValue((void*) " ");
                            
                    }
                    goto success_peg_378;
                    out_peg_380:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_377.getPosition()))){
                                result_peg_377.nextPosition();
                            } else {
                                goto out_peg_382;
                            }
                        }
                        result_peg_377.setValue((void*) "\t");
                            
                    }
                    goto success_peg_378;
                    out_peg_382:
                    goto loop_peg_376;
                    success_peg_378:
                    ;
                    result_peg_363.addResult(result_peg_377);
                } while (true);
                loop_peg_376:
                ;
                        
            }
            goto success_peg_367;
            goto out_peg_365;
            success_peg_367:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_363.getPosition()))){
                    result_peg_363.nextPosition();
                } else {
                    goto out_peg_365;
                }
            }
            result_peg_363.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_363.reset();
                do{
                    Result result_peg_395(result_peg_363.getPosition());
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
                    result_peg_363.addResult(result_peg_395);
                } while (true);
                loop_peg_394:
                ;
                        
            }
            goto success_peg_385;
            goto out_peg_365;
            success_peg_385:
            ;
        
        
        
        result_peg_363 = rule_expr(stream, result_peg_363.getPosition());
            if (result_peg_363.error()){
                goto out_peg_365;
            }
            exp = result_peg_363.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, negateExpression(exp));
                result_peg_363.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_363;
    stream.update(result_peg_363.getPosition());
    
    
    return result_peg_363;
    out_peg_365:
    Result result_peg_402(myposition);
    
    {
    
        result_peg_402 = rule_identifier(stream, result_peg_402.getPosition());
            if (result_peg_402.error()){
                goto out_peg_404;
            }
            name = result_peg_402.getValues();
        
        
        
        {
                
                result_peg_402.reset();
                do{
                    Result result_peg_416(result_peg_402.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_416.getPosition()))){
                                result_peg_416.nextPosition();
                            } else {
                                goto out_peg_419;
                            }
                        }
                        result_peg_416.setValue((void*) " ");
                            
                    }
                    goto success_peg_417;
                    out_peg_419:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_416.getPosition()))){
                                result_peg_416.nextPosition();
                            } else {
                                goto out_peg_421;
                            }
                        }
                        result_peg_416.setValue((void*) "\t");
                            
                    }
                    goto success_peg_417;
                    out_peg_421:
                    goto loop_peg_415;
                    success_peg_417:
                    ;
                    result_peg_402.addResult(result_peg_416);
                } while (true);
                loop_peg_415:
                ;
                        
            }
            goto success_peg_406;
            goto out_peg_404;
            success_peg_406:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_402.getPosition()))){
                    result_peg_402.nextPosition();
                } else {
                    goto out_peg_404;
                }
            }
            result_peg_402.setValue((void*) "=");
        
        
        
        {
                
                result_peg_402.reset();
                do{
                    Result result_peg_434(result_peg_402.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_434.getPosition()))){
                                result_peg_434.nextPosition();
                            } else {
                                goto out_peg_437;
                            }
                        }
                        result_peg_434.setValue((void*) " ");
                            
                    }
                    goto success_peg_435;
                    out_peg_437:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_434.getPosition()))){
                                result_peg_434.nextPosition();
                            } else {
                                goto out_peg_439;
                            }
                        }
                        result_peg_434.setValue((void*) "\t");
                            
                    }
                    goto success_peg_435;
                    out_peg_439:
                    goto loop_peg_433;
                    success_peg_435:
                    ;
                    result_peg_402.addResult(result_peg_434);
                } while (true);
                loop_peg_433:
                ;
                        
            }
            goto success_peg_424;
            goto out_peg_404;
            success_peg_424:
            ;
        
        
        
        Result result_peg_441(result_peg_402.getPosition());
            result_peg_441 = rule_line_end(stream, result_peg_441.getPosition());
            if (result_peg_441.error()){
                goto out_peg_404;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_402.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_402;
    stream.update(result_peg_402.getPosition());
    
    
    return result_peg_402;
    out_peg_404:
    Result result_peg_442(myposition);
    
    {
    
        result_peg_442 = rule_identifier(stream, result_peg_442.getPosition());
            if (result_peg_442.error()){
                goto out_peg_444;
            }
            name = result_peg_442.getValues();
        
        
        
        {
                
                result_peg_442.reset();
                do{
                    Result result_peg_456(result_peg_442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_456.getPosition()))){
                                result_peg_456.nextPosition();
                            } else {
                                goto out_peg_459;
                            }
                        }
                        result_peg_456.setValue((void*) " ");
                            
                    }
                    goto success_peg_457;
                    out_peg_459:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_456.getPosition()))){
                                result_peg_456.nextPosition();
                            } else {
                                goto out_peg_461;
                            }
                        }
                        result_peg_456.setValue((void*) "\t");
                            
                    }
                    goto success_peg_457;
                    out_peg_461:
                    goto loop_peg_455;
                    success_peg_457:
                    ;
                    result_peg_442.addResult(result_peg_456);
                } while (true);
                loop_peg_455:
                ;
                        
            }
            goto success_peg_446;
            goto out_peg_444;
            success_peg_446:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_442.getPosition()))){
                    result_peg_442.nextPosition();
                } else {
                    goto out_peg_444;
                }
            }
            result_peg_442.setValue((void*) "(");
        
        
        
        {
                
                result_peg_442.reset();
                do{
                    Result result_peg_474(result_peg_442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_474.getPosition()))){
                                result_peg_474.nextPosition();
                            } else {
                                goto out_peg_477;
                            }
                        }
                        result_peg_474.setValue((void*) " ");
                            
                    }
                    goto success_peg_475;
                    out_peg_477:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_474.getPosition()))){
                                result_peg_474.nextPosition();
                            } else {
                                goto out_peg_479;
                            }
                        }
                        result_peg_474.setValue((void*) "\t");
                            
                    }
                    goto success_peg_475;
                    out_peg_479:
                    goto loop_peg_473;
                    success_peg_475:
                    ;
                    result_peg_442.addResult(result_peg_474);
                } while (true);
                loop_peg_473:
                ;
                        
            }
            goto success_peg_464;
            goto out_peg_444;
            success_peg_464:
            ;
        
        
        
        result_peg_442 = rule_integer(stream, result_peg_442.getPosition());
            if (result_peg_442.error()){
                goto out_peg_444;
            }
            index = result_peg_442.getValues();
        
        
        
        {
                
                result_peg_442.reset();
                do{
                    Result result_peg_492(result_peg_442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_492.getPosition()))){
                                result_peg_492.nextPosition();
                            } else {
                                goto out_peg_495;
                            }
                        }
                        result_peg_492.setValue((void*) " ");
                            
                    }
                    goto success_peg_493;
                    out_peg_495:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_492.getPosition()))){
                                result_peg_492.nextPosition();
                            } else {
                                goto out_peg_497;
                            }
                        }
                        result_peg_492.setValue((void*) "\t");
                            
                    }
                    goto success_peg_493;
                    out_peg_497:
                    goto loop_peg_491;
                    success_peg_493:
                    ;
                    result_peg_442.addResult(result_peg_492);
                } while (true);
                loop_peg_491:
                ;
                        
            }
            goto success_peg_482;
            goto out_peg_444;
            success_peg_482:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_442.getPosition()))){
                    result_peg_442.nextPosition();
                } else {
                    goto out_peg_444;
                }
            }
            result_peg_442.setValue((void*) ")");
        
        
        
        {
                
                result_peg_442.reset();
                do{
                    Result result_peg_510(result_peg_442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_510.getPosition()))){
                                result_peg_510.nextPosition();
                            } else {
                                goto out_peg_513;
                            }
                        }
                        result_peg_510.setValue((void*) " ");
                            
                    }
                    goto success_peg_511;
                    out_peg_513:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_510.getPosition()))){
                                result_peg_510.nextPosition();
                            } else {
                                goto out_peg_515;
                            }
                        }
                        result_peg_510.setValue((void*) "\t");
                            
                    }
                    goto success_peg_511;
                    out_peg_515:
                    goto loop_peg_509;
                    success_peg_511:
                    ;
                    result_peg_442.addResult(result_peg_510);
                } while (true);
                loop_peg_509:
                ;
                        
            }
            goto success_peg_500;
            goto out_peg_444;
            success_peg_500:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_442.getPosition()))){
                    result_peg_442.nextPosition();
                } else {
                    goto out_peg_444;
                }
            }
            result_peg_442.setValue((void*) "=");
        
        
        
        {
                
                result_peg_442.reset();
                do{
                    Result result_peg_528(result_peg_442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_528.getPosition()))){
                                result_peg_528.nextPosition();
                            } else {
                                goto out_peg_531;
                            }
                        }
                        result_peg_528.setValue((void*) " ");
                            
                    }
                    goto success_peg_529;
                    out_peg_531:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_528.getPosition()))){
                                result_peg_528.nextPosition();
                            } else {
                                goto out_peg_533;
                            }
                        }
                        result_peg_528.setValue((void*) "\t");
                            
                    }
                    goto success_peg_529;
                    out_peg_533:
                    goto loop_peg_527;
                    success_peg_529:
                    ;
                    result_peg_442.addResult(result_peg_528);
                } while (true);
                loop_peg_527:
                ;
                        
            }
            goto success_peg_518;
            goto out_peg_444;
            success_peg_518:
            ;
        
        
        
        result_peg_442 = rule_expr(stream, result_peg_442.getPosition());
            if (result_peg_442.error()){
                goto out_peg_444;
            }
            exp = result_peg_442.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_442.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_442;
    stream.update(result_peg_442.getPosition());
    
    
    return result_peg_442;
    out_peg_444:
    Result result_peg_535(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareChar("ctrl"[i], stream.get(result_peg_535.getPosition()))){
                    result_peg_535.nextPosition();
                } else {
                    goto out_peg_537;
                }
            }
            result_peg_535.setValue((void*) "ctrl");
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(makeSimpleIdentifier("ctrl"), makeNumber(1));
                result_peg_535.setValue(value);
            }
        
        
    }
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = result_peg_535;
    stream.update(result_peg_535.getPosition());
    
    
    return result_peg_535;
    out_peg_537:
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_539 = stream.getColumn(position);
    if (column_peg_539.chunk2 != 0 && column_peg_539.chunk2->chunk_identifier.calculated()){
        return column_peg_539.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_549(stream, "identifier");
    int myposition = position;
    
    Value line;
    Value first;
    Value rest;
    Result result_peg_540(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_542 = stream.getLineInfo(result_peg_540.getPosition());
            line = &line_info_peg_542;
        
        
        
        result_peg_540 = rule_name(stream, result_peg_540.getPosition());
            if (result_peg_540.error()){
                goto out_peg_544;
            }
            first = result_peg_540.getValues();
        
        
        
        result_peg_540.reset();
            do{
                Result result_peg_547(result_peg_540.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_547.getPosition()))){
                                result_peg_547.nextPosition();
                            } else {
                                goto loop_peg_546;
                            }
                        }
                        result_peg_547.setValue((void*) ".");
                    
                    
                    
                    result_peg_547 = rule_name(stream, result_peg_547.getPosition());
                        if (result_peg_547.error()){
                            goto loop_peg_546;
                        }
                    
                    
                }
                result_peg_540.addResult(result_peg_547);
            } while (true);
            loop_peg_546:
            ;
            rest = result_peg_540.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                result_peg_540.setValue(value);
            }
        
        
    }
    
    if (column_peg_539.chunk2 == 0){
        column_peg_539.chunk2 = new Chunk2();
    }
    column_peg_539.chunk2->chunk_identifier = result_peg_540;
    stream.update(result_peg_540.getPosition());
    
    
    return result_peg_540;
    out_peg_544:
    
    if (column_peg_539.chunk2 == 0){
        column_peg_539.chunk2 = new Chunk2();
    }
    column_peg_539.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_550 = stream.getColumn(position);
    if (column_peg_550.chunk2 != 0 && column_peg_550.chunk2->chunk_integer.calculated()){
        return column_peg_550.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_568(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_551(myposition);
    
    {
    
        int save_peg_553 = result_peg_551.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_551.getPosition()))){
                        result_peg_551.nextPosition();
                    } else {
                        goto out_peg_556;
                    }
                }
                result_peg_551.setValue((void*) "-");
                    
            }
            goto success_peg_554;
            out_peg_556:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_551.getPosition()))){
                        result_peg_551.nextPosition();
                    } else {
                        goto out_peg_558;
                    }
                }
                result_peg_551.setValue((void*) "+");
                    
            }
            goto success_peg_554;
            out_peg_558:
            
            result_peg_551 = Result(save_peg_553);
            result_peg_551.setValue((void*) 0);
            
            success_peg_554:
            ;
        
        Result result_peg_552 = result_peg_551;
        
        result_peg_551.reset();
            do{
                Result result_peg_561(result_peg_551.getPosition());
                {
                    
                    char letter_peg_566 = stream.get(result_peg_561.getPosition());
                    if (letter_peg_566 != '\0' && strchr("0123456789", letter_peg_566) != NULL){
                        result_peg_561.nextPosition();
                        result_peg_561.setValue((void*) (long) letter_peg_566);
                    } else {
                        goto out_peg_565;
                    }
                    
                }
                goto success_peg_562;
                out_peg_565:
                goto loop_peg_560;
                success_peg_562:
                ;
                result_peg_551.addResult(result_peg_561);
            } while (true);
            loop_peg_560:
            if (result_peg_551.matches() == 0){
                goto out_peg_567;
            }
        
        Result result_peg_559 = result_peg_551;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_552.getValues(), result_peg_559.getValues());
                result_peg_551.setValue(value);
            }
        
        
    }
    
    if (column_peg_550.chunk2 == 0){
        column_peg_550.chunk2 = new Chunk2();
    }
    column_peg_550.chunk2->chunk_integer = result_peg_551;
    stream.update(result_peg_551.getPosition());
    
    
    return result_peg_551;
    out_peg_567:
    
    if (column_peg_550.chunk2 == 0){
        column_peg_550.chunk2 = new Chunk2();
    }
    column_peg_550.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_569 = stream.getColumn(position);
    if (column_peg_569.chunk2 != 0 && column_peg_569.chunk2->chunk_float.calculated()){
        return column_peg_569.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_614(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_570(myposition);
    
    {
    
        int save_peg_572 = result_peg_570.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_570.getPosition()))){
                        result_peg_570.nextPosition();
                    } else {
                        goto out_peg_575;
                    }
                }
                result_peg_570.setValue((void*) "-");
                    
            }
            goto success_peg_573;
            out_peg_575:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_570.getPosition()))){
                        result_peg_570.nextPosition();
                    } else {
                        goto out_peg_577;
                    }
                }
                result_peg_570.setValue((void*) "+");
                    
            }
            goto success_peg_573;
            out_peg_577:
            
            result_peg_570 = Result(save_peg_572);
            result_peg_570.setValue((void*) 0);
            
            success_peg_573:
            ;
        
        Result result_peg_571 = result_peg_570;
        
        result_peg_570.reset();
            do{
                Result result_peg_580(result_peg_570.getPosition());
                {
                    
                    char letter_peg_585 = stream.get(result_peg_580.getPosition());
                    if (letter_peg_585 != '\0' && strchr("0123456789", letter_peg_585) != NULL){
                        result_peg_580.nextPosition();
                        result_peg_580.setValue((void*) (long) letter_peg_585);
                    } else {
                        goto out_peg_584;
                    }
                    
                }
                goto success_peg_581;
                out_peg_584:
                goto loop_peg_579;
                success_peg_581:
                ;
                result_peg_570.addResult(result_peg_580);
            } while (true);
            loop_peg_579:
            ;
            left = result_peg_570.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_570.getPosition()))){
                    result_peg_570.nextPosition();
                } else {
                    goto out_peg_587;
                }
            }
            result_peg_570.setValue((void*) ".");
        
        
        
        result_peg_570.reset();
            do{
                Result result_peg_590(result_peg_570.getPosition());
                {
                    
                    char letter_peg_595 = stream.get(result_peg_590.getPosition());
                    if (letter_peg_595 != '\0' && strchr("0123456789", letter_peg_595) != NULL){
                        result_peg_590.nextPosition();
                        result_peg_590.setValue((void*) (long) letter_peg_595);
                    } else {
                        goto out_peg_594;
                    }
                    
                }
                goto success_peg_591;
                out_peg_594:
                goto loop_peg_589;
                success_peg_591:
                ;
                result_peg_570.addResult(result_peg_590);
            } while (true);
            loop_peg_589:
            if (result_peg_570.matches() == 0){
                goto out_peg_587;
            }
            right = result_peg_570.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_571.getValues(), parseDouble(left,right));
                result_peg_570.setValue(value);
            }
        
        
    }
    
    if (column_peg_569.chunk2 == 0){
        column_peg_569.chunk2 = new Chunk2();
    }
    column_peg_569.chunk2->chunk_float = result_peg_570;
    stream.update(result_peg_570.getPosition());
    
    
    return result_peg_570;
    out_peg_587:
    Result result_peg_596(myposition);
    
    {
    
        int save_peg_598 = result_peg_596.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_596.getPosition()))){
                        result_peg_596.nextPosition();
                    } else {
                        goto out_peg_601;
                    }
                }
                result_peg_596.setValue((void*) "-");
                    
            }
            goto success_peg_599;
            out_peg_601:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_596.getPosition()))){
                        result_peg_596.nextPosition();
                    } else {
                        goto out_peg_603;
                    }
                }
                result_peg_596.setValue((void*) "+");
                    
            }
            goto success_peg_599;
            out_peg_603:
            
            result_peg_596 = Result(save_peg_598);
            result_peg_596.setValue((void*) 0);
            
            success_peg_599:
            ;
        
        Result result_peg_597 = result_peg_596;
        
        result_peg_596.reset();
            do{
                Result result_peg_606(result_peg_596.getPosition());
                {
                    
                    char letter_peg_611 = stream.get(result_peg_606.getPosition());
                    if (letter_peg_611 != '\0' && strchr("0123456789", letter_peg_611) != NULL){
                        result_peg_606.nextPosition();
                        result_peg_606.setValue((void*) (long) letter_peg_611);
                    } else {
                        goto out_peg_610;
                    }
                    
                }
                goto success_peg_607;
                out_peg_610:
                goto loop_peg_605;
                success_peg_607:
                ;
                result_peg_596.addResult(result_peg_606);
            } while (true);
            loop_peg_605:
            if (result_peg_596.matches() == 0){
                goto out_peg_612;
            }
            left = result_peg_596.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_596.getPosition()))){
                    result_peg_596.nextPosition();
                } else {
                    goto out_peg_612;
                }
            }
            result_peg_596.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_597.getValues(), parseDouble(left));
                result_peg_596.setValue(value);
            }
        
        
    }
    
    if (column_peg_569.chunk2 == 0){
        column_peg_569.chunk2 = new Chunk2();
    }
    column_peg_569.chunk2->chunk_float = result_peg_596;
    stream.update(result_peg_596.getPosition());
    
    
    return result_peg_596;
    out_peg_612:
    
    if (column_peg_569.chunk2 == 0){
        column_peg_569.chunk2 = new Chunk2();
    }
    column_peg_569.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_615 = stream.getColumn(position);
    if (column_peg_615.chunk2 != 0 && column_peg_615.chunk2->chunk_string.calculated()){
        return column_peg_615.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_630(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_616(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_616.getPosition()))){
                    result_peg_616.nextPosition();
                } else {
                    goto out_peg_618;
                }
            }
            result_peg_616.setValue((void*) "\"");
        
        
        
        result_peg_616.reset();
            do{
                Result result_peg_621(result_peg_616.getPosition());
                {
                
                    Result result_peg_624(result_peg_621);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_624.getPosition()))){
                                result_peg_624.nextPosition();
                            } else {
                                goto not_peg_623;
                            }
                        }
                        result_peg_624.setValue((void*) "\"");
                        goto loop_peg_620;
                        not_peg_623:
                        result_peg_621.setValue((void*)0);
                    
                    
                    
                    Result result_peg_627(result_peg_621);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_627.getPosition()))){
                                result_peg_627.nextPosition();
                            } else {
                                goto not_peg_626;
                            }
                        }
                        result_peg_627.setValue((void*) "\n");
                        goto loop_peg_620;
                        not_peg_626:
                        result_peg_621.setValue((void*)0);
                    
                    
                    
                    char temp_peg_628 = stream.get(result_peg_621.getPosition());
                        if (temp_peg_628 != '\0'){
                            result_peg_621.setValue((void*) (long) temp_peg_628);
                            result_peg_621.nextPosition();
                        } else {
                            goto loop_peg_620;
                        }
                    
                    
                }
                result_peg_616.addResult(result_peg_621);
            } while (true);
            loop_peg_620:
            ;
            contents = result_peg_616.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_616.getPosition()))){
                    result_peg_616.nextPosition();
                } else {
                    goto out_peg_618;
                }
            }
            result_peg_616.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_616.setValue(value);
            }
        
        
    }
    
    if (column_peg_615.chunk2 == 0){
        column_peg_615.chunk2 = new Chunk2();
    }
    column_peg_615.chunk2->chunk_string = result_peg_616;
    stream.update(result_peg_616.getPosition());
    
    
    return result_peg_616;
    out_peg_618:
    
    if (column_peg_615.chunk2 == 0){
        column_peg_615.chunk2 = new Chunk2();
    }
    column_peg_615.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_631 = stream.getColumn(position);
    if (column_peg_631.chunk2 != 0 && column_peg_631.chunk2->chunk_range.calculated()){
        return column_peg_631.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_932(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_632(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_632.getPosition()))){
                    result_peg_632.nextPosition();
                } else {
                    goto out_peg_634;
                }
            }
            result_peg_632.setValue((void*) "[");
        
        
        
        {
                
                result_peg_632.reset();
                do{
                    Result result_peg_646(result_peg_632.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_646.getPosition()))){
                                result_peg_646.nextPosition();
                            } else {
                                goto out_peg_649;
                            }
                        }
                        result_peg_646.setValue((void*) " ");
                            
                    }
                    goto success_peg_647;
                    out_peg_649:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_646.getPosition()))){
                                result_peg_646.nextPosition();
                            } else {
                                goto out_peg_651;
                            }
                        }
                        result_peg_646.setValue((void*) "\t");
                            
                    }
                    goto success_peg_647;
                    out_peg_651:
                    goto loop_peg_645;
                    success_peg_647:
                    ;
                    result_peg_632.addResult(result_peg_646);
                } while (true);
                loop_peg_645:
                ;
                        
            }
            goto success_peg_636;
            goto out_peg_634;
            success_peg_636:
            ;
        
        
        
        result_peg_632 = rule_expr_c(stream, result_peg_632.getPosition());
            if (result_peg_632.error()){
                goto out_peg_634;
            }
            low = result_peg_632.getValues();
        
        
        
        {
                
                result_peg_632.reset();
                do{
                    Result result_peg_664(result_peg_632.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_664.getPosition()))){
                                result_peg_664.nextPosition();
                            } else {
                                goto out_peg_667;
                            }
                        }
                        result_peg_664.setValue((void*) " ");
                            
                    }
                    goto success_peg_665;
                    out_peg_667:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_664.getPosition()))){
                                result_peg_664.nextPosition();
                            } else {
                                goto out_peg_669;
                            }
                        }
                        result_peg_664.setValue((void*) "\t");
                            
                    }
                    goto success_peg_665;
                    out_peg_669:
                    goto loop_peg_663;
                    success_peg_665:
                    ;
                    result_peg_632.addResult(result_peg_664);
                } while (true);
                loop_peg_663:
                ;
                        
            }
            goto success_peg_654;
            goto out_peg_634;
            success_peg_654:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_632.getPosition()))){
                    result_peg_632.nextPosition();
                } else {
                    goto out_peg_634;
                }
            }
            result_peg_632.setValue((void*) ",");
        
        
        
        {
                
                result_peg_632.reset();
                do{
                    Result result_peg_682(result_peg_632.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_682.getPosition()))){
                                result_peg_682.nextPosition();
                            } else {
                                goto out_peg_685;
                            }
                        }
                        result_peg_682.setValue((void*) " ");
                            
                    }
                    goto success_peg_683;
                    out_peg_685:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_682.getPosition()))){
                                result_peg_682.nextPosition();
                            } else {
                                goto out_peg_687;
                            }
                        }
                        result_peg_682.setValue((void*) "\t");
                            
                    }
                    goto success_peg_683;
                    out_peg_687:
                    goto loop_peg_681;
                    success_peg_683:
                    ;
                    result_peg_632.addResult(result_peg_682);
                } while (true);
                loop_peg_681:
                ;
                        
            }
            goto success_peg_672;
            goto out_peg_634;
            success_peg_672:
            ;
        
        
        
        result_peg_632 = rule_expr_c(stream, result_peg_632.getPosition());
            if (result_peg_632.error()){
                goto out_peg_634;
            }
            high = result_peg_632.getValues();
        
        
        
        {
                
                result_peg_632.reset();
                do{
                    Result result_peg_700(result_peg_632.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_700.getPosition()))){
                                result_peg_700.nextPosition();
                            } else {
                                goto out_peg_703;
                            }
                        }
                        result_peg_700.setValue((void*) " ");
                            
                    }
                    goto success_peg_701;
                    out_peg_703:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_700.getPosition()))){
                                result_peg_700.nextPosition();
                            } else {
                                goto out_peg_705;
                            }
                        }
                        result_peg_700.setValue((void*) "\t");
                            
                    }
                    goto success_peg_701;
                    out_peg_705:
                    goto loop_peg_699;
                    success_peg_701:
                    ;
                    result_peg_632.addResult(result_peg_700);
                } while (true);
                loop_peg_699:
                ;
                        
            }
            goto success_peg_690;
            goto out_peg_634;
            success_peg_690:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_632.getPosition()))){
                    result_peg_632.nextPosition();
                } else {
                    goto out_peg_634;
                }
            }
            result_peg_632.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_632.setValue(value);
            }
        
        
    }
    
    if (column_peg_631.chunk2 == 0){
        column_peg_631.chunk2 = new Chunk2();
    }
    column_peg_631.chunk2->chunk_range = result_peg_632;
    stream.update(result_peg_632.getPosition());
    
    
    return result_peg_632;
    out_peg_634:
    Result result_peg_707(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_707.getPosition()))){
                    result_peg_707.nextPosition();
                } else {
                    goto out_peg_709;
                }
            }
            result_peg_707.setValue((void*) "(");
        
        
        
        {
                
                result_peg_707.reset();
                do{
                    Result result_peg_721(result_peg_707.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_721.getPosition()))){
                                result_peg_721.nextPosition();
                            } else {
                                goto out_peg_724;
                            }
                        }
                        result_peg_721.setValue((void*) " ");
                            
                    }
                    goto success_peg_722;
                    out_peg_724:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_721.getPosition()))){
                                result_peg_721.nextPosition();
                            } else {
                                goto out_peg_726;
                            }
                        }
                        result_peg_721.setValue((void*) "\t");
                            
                    }
                    goto success_peg_722;
                    out_peg_726:
                    goto loop_peg_720;
                    success_peg_722:
                    ;
                    result_peg_707.addResult(result_peg_721);
                } while (true);
                loop_peg_720:
                ;
                        
            }
            goto success_peg_711;
            goto out_peg_709;
            success_peg_711:
            ;
        
        
        
        result_peg_707 = rule_expr_c(stream, result_peg_707.getPosition());
            if (result_peg_707.error()){
                goto out_peg_709;
            }
            low = result_peg_707.getValues();
        
        
        
        {
                
                result_peg_707.reset();
                do{
                    Result result_peg_739(result_peg_707.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_739.getPosition()))){
                                result_peg_739.nextPosition();
                            } else {
                                goto out_peg_742;
                            }
                        }
                        result_peg_739.setValue((void*) " ");
                            
                    }
                    goto success_peg_740;
                    out_peg_742:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_739.getPosition()))){
                                result_peg_739.nextPosition();
                            } else {
                                goto out_peg_744;
                            }
                        }
                        result_peg_739.setValue((void*) "\t");
                            
                    }
                    goto success_peg_740;
                    out_peg_744:
                    goto loop_peg_738;
                    success_peg_740:
                    ;
                    result_peg_707.addResult(result_peg_739);
                } while (true);
                loop_peg_738:
                ;
                        
            }
            goto success_peg_729;
            goto out_peg_709;
            success_peg_729:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_707.getPosition()))){
                    result_peg_707.nextPosition();
                } else {
                    goto out_peg_709;
                }
            }
            result_peg_707.setValue((void*) ",");
        
        
        
        {
                
                result_peg_707.reset();
                do{
                    Result result_peg_757(result_peg_707.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_757.getPosition()))){
                                result_peg_757.nextPosition();
                            } else {
                                goto out_peg_760;
                            }
                        }
                        result_peg_757.setValue((void*) " ");
                            
                    }
                    goto success_peg_758;
                    out_peg_760:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_757.getPosition()))){
                                result_peg_757.nextPosition();
                            } else {
                                goto out_peg_762;
                            }
                        }
                        result_peg_757.setValue((void*) "\t");
                            
                    }
                    goto success_peg_758;
                    out_peg_762:
                    goto loop_peg_756;
                    success_peg_758:
                    ;
                    result_peg_707.addResult(result_peg_757);
                } while (true);
                loop_peg_756:
                ;
                        
            }
            goto success_peg_747;
            goto out_peg_709;
            success_peg_747:
            ;
        
        
        
        result_peg_707 = rule_expr_c(stream, result_peg_707.getPosition());
            if (result_peg_707.error()){
                goto out_peg_709;
            }
            high = result_peg_707.getValues();
        
        
        
        {
                
                result_peg_707.reset();
                do{
                    Result result_peg_775(result_peg_707.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_775.getPosition()))){
                                result_peg_775.nextPosition();
                            } else {
                                goto out_peg_778;
                            }
                        }
                        result_peg_775.setValue((void*) " ");
                            
                    }
                    goto success_peg_776;
                    out_peg_778:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_775.getPosition()))){
                                result_peg_775.nextPosition();
                            } else {
                                goto out_peg_780;
                            }
                        }
                        result_peg_775.setValue((void*) "\t");
                            
                    }
                    goto success_peg_776;
                    out_peg_780:
                    goto loop_peg_774;
                    success_peg_776:
                    ;
                    result_peg_707.addResult(result_peg_775);
                } while (true);
                loop_peg_774:
                ;
                        
            }
            goto success_peg_765;
            goto out_peg_709;
            success_peg_765:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_707.getPosition()))){
                    result_peg_707.nextPosition();
                } else {
                    goto out_peg_709;
                }
            }
            result_peg_707.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_707.setValue(value);
            }
        
        
    }
    
    if (column_peg_631.chunk2 == 0){
        column_peg_631.chunk2 = new Chunk2();
    }
    column_peg_631.chunk2->chunk_range = result_peg_707;
    stream.update(result_peg_707.getPosition());
    
    
    return result_peg_707;
    out_peg_709:
    Result result_peg_782(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_782.getPosition()))){
                    result_peg_782.nextPosition();
                } else {
                    goto out_peg_784;
                }
            }
            result_peg_782.setValue((void*) "(");
        
        
        
        {
                
                result_peg_782.reset();
                do{
                    Result result_peg_796(result_peg_782.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_796.getPosition()))){
                                result_peg_796.nextPosition();
                            } else {
                                goto out_peg_799;
                            }
                        }
                        result_peg_796.setValue((void*) " ");
                            
                    }
                    goto success_peg_797;
                    out_peg_799:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_796.getPosition()))){
                                result_peg_796.nextPosition();
                            } else {
                                goto out_peg_801;
                            }
                        }
                        result_peg_796.setValue((void*) "\t");
                            
                    }
                    goto success_peg_797;
                    out_peg_801:
                    goto loop_peg_795;
                    success_peg_797:
                    ;
                    result_peg_782.addResult(result_peg_796);
                } while (true);
                loop_peg_795:
                ;
                        
            }
            goto success_peg_786;
            goto out_peg_784;
            success_peg_786:
            ;
        
        
        
        result_peg_782 = rule_expr_c(stream, result_peg_782.getPosition());
            if (result_peg_782.error()){
                goto out_peg_784;
            }
            low = result_peg_782.getValues();
        
        
        
        {
                
                result_peg_782.reset();
                do{
                    Result result_peg_814(result_peg_782.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_814.getPosition()))){
                                result_peg_814.nextPosition();
                            } else {
                                goto out_peg_817;
                            }
                        }
                        result_peg_814.setValue((void*) " ");
                            
                    }
                    goto success_peg_815;
                    out_peg_817:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_814.getPosition()))){
                                result_peg_814.nextPosition();
                            } else {
                                goto out_peg_819;
                            }
                        }
                        result_peg_814.setValue((void*) "\t");
                            
                    }
                    goto success_peg_815;
                    out_peg_819:
                    goto loop_peg_813;
                    success_peg_815:
                    ;
                    result_peg_782.addResult(result_peg_814);
                } while (true);
                loop_peg_813:
                ;
                        
            }
            goto success_peg_804;
            goto out_peg_784;
            success_peg_804:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_782.getPosition()))){
                    result_peg_782.nextPosition();
                } else {
                    goto out_peg_784;
                }
            }
            result_peg_782.setValue((void*) ",");
        
        
        
        {
                
                result_peg_782.reset();
                do{
                    Result result_peg_832(result_peg_782.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_832.getPosition()))){
                                result_peg_832.nextPosition();
                            } else {
                                goto out_peg_835;
                            }
                        }
                        result_peg_832.setValue((void*) " ");
                            
                    }
                    goto success_peg_833;
                    out_peg_835:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_832.getPosition()))){
                                result_peg_832.nextPosition();
                            } else {
                                goto out_peg_837;
                            }
                        }
                        result_peg_832.setValue((void*) "\t");
                            
                    }
                    goto success_peg_833;
                    out_peg_837:
                    goto loop_peg_831;
                    success_peg_833:
                    ;
                    result_peg_782.addResult(result_peg_832);
                } while (true);
                loop_peg_831:
                ;
                        
            }
            goto success_peg_822;
            goto out_peg_784;
            success_peg_822:
            ;
        
        
        
        result_peg_782 = rule_expr_c(stream, result_peg_782.getPosition());
            if (result_peg_782.error()){
                goto out_peg_784;
            }
            high = result_peg_782.getValues();
        
        
        
        {
                
                result_peg_782.reset();
                do{
                    Result result_peg_850(result_peg_782.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_850.getPosition()))){
                                result_peg_850.nextPosition();
                            } else {
                                goto out_peg_853;
                            }
                        }
                        result_peg_850.setValue((void*) " ");
                            
                    }
                    goto success_peg_851;
                    out_peg_853:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_850.getPosition()))){
                                result_peg_850.nextPosition();
                            } else {
                                goto out_peg_855;
                            }
                        }
                        result_peg_850.setValue((void*) "\t");
                            
                    }
                    goto success_peg_851;
                    out_peg_855:
                    goto loop_peg_849;
                    success_peg_851:
                    ;
                    result_peg_782.addResult(result_peg_850);
                } while (true);
                loop_peg_849:
                ;
                        
            }
            goto success_peg_840;
            goto out_peg_784;
            success_peg_840:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_782.getPosition()))){
                    result_peg_782.nextPosition();
                } else {
                    goto out_peg_784;
                }
            }
            result_peg_782.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_782.setValue(value);
            }
        
        
    }
    
    if (column_peg_631.chunk2 == 0){
        column_peg_631.chunk2 = new Chunk2();
    }
    column_peg_631.chunk2->chunk_range = result_peg_782;
    stream.update(result_peg_782.getPosition());
    
    
    return result_peg_782;
    out_peg_784:
    Result result_peg_857(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_857.getPosition()))){
                    result_peg_857.nextPosition();
                } else {
                    goto out_peg_859;
                }
            }
            result_peg_857.setValue((void*) "[");
        
        
        
        {
                
                result_peg_857.reset();
                do{
                    Result result_peg_871(result_peg_857.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_871.getPosition()))){
                                result_peg_871.nextPosition();
                            } else {
                                goto out_peg_874;
                            }
                        }
                        result_peg_871.setValue((void*) " ");
                            
                    }
                    goto success_peg_872;
                    out_peg_874:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_871.getPosition()))){
                                result_peg_871.nextPosition();
                            } else {
                                goto out_peg_876;
                            }
                        }
                        result_peg_871.setValue((void*) "\t");
                            
                    }
                    goto success_peg_872;
                    out_peg_876:
                    goto loop_peg_870;
                    success_peg_872:
                    ;
                    result_peg_857.addResult(result_peg_871);
                } while (true);
                loop_peg_870:
                ;
                        
            }
            goto success_peg_861;
            goto out_peg_859;
            success_peg_861:
            ;
        
        
        
        result_peg_857 = rule_expr_c(stream, result_peg_857.getPosition());
            if (result_peg_857.error()){
                goto out_peg_859;
            }
            low = result_peg_857.getValues();
        
        
        
        {
                
                result_peg_857.reset();
                do{
                    Result result_peg_889(result_peg_857.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_889.getPosition()))){
                                result_peg_889.nextPosition();
                            } else {
                                goto out_peg_892;
                            }
                        }
                        result_peg_889.setValue((void*) " ");
                            
                    }
                    goto success_peg_890;
                    out_peg_892:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_889.getPosition()))){
                                result_peg_889.nextPosition();
                            } else {
                                goto out_peg_894;
                            }
                        }
                        result_peg_889.setValue((void*) "\t");
                            
                    }
                    goto success_peg_890;
                    out_peg_894:
                    goto loop_peg_888;
                    success_peg_890:
                    ;
                    result_peg_857.addResult(result_peg_889);
                } while (true);
                loop_peg_888:
                ;
                        
            }
            goto success_peg_879;
            goto out_peg_859;
            success_peg_879:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_857.getPosition()))){
                    result_peg_857.nextPosition();
                } else {
                    goto out_peg_859;
                }
            }
            result_peg_857.setValue((void*) ",");
        
        
        
        {
                
                result_peg_857.reset();
                do{
                    Result result_peg_907(result_peg_857.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_907.getPosition()))){
                                result_peg_907.nextPosition();
                            } else {
                                goto out_peg_910;
                            }
                        }
                        result_peg_907.setValue((void*) " ");
                            
                    }
                    goto success_peg_908;
                    out_peg_910:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_907.getPosition()))){
                                result_peg_907.nextPosition();
                            } else {
                                goto out_peg_912;
                            }
                        }
                        result_peg_907.setValue((void*) "\t");
                            
                    }
                    goto success_peg_908;
                    out_peg_912:
                    goto loop_peg_906;
                    success_peg_908:
                    ;
                    result_peg_857.addResult(result_peg_907);
                } while (true);
                loop_peg_906:
                ;
                        
            }
            goto success_peg_897;
            goto out_peg_859;
            success_peg_897:
            ;
        
        
        
        result_peg_857 = rule_expr_c(stream, result_peg_857.getPosition());
            if (result_peg_857.error()){
                goto out_peg_859;
            }
            high = result_peg_857.getValues();
        
        
        
        {
                
                result_peg_857.reset();
                do{
                    Result result_peg_925(result_peg_857.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_925.getPosition()))){
                                result_peg_925.nextPosition();
                            } else {
                                goto out_peg_928;
                            }
                        }
                        result_peg_925.setValue((void*) " ");
                            
                    }
                    goto success_peg_926;
                    out_peg_928:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_925.getPosition()))){
                                result_peg_925.nextPosition();
                            } else {
                                goto out_peg_930;
                            }
                        }
                        result_peg_925.setValue((void*) "\t");
                            
                    }
                    goto success_peg_926;
                    out_peg_930:
                    goto loop_peg_924;
                    success_peg_926:
                    ;
                    result_peg_857.addResult(result_peg_925);
                } while (true);
                loop_peg_924:
                ;
                        
            }
            goto success_peg_915;
            goto out_peg_859;
            success_peg_915:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_857.getPosition()))){
                    result_peg_857.nextPosition();
                } else {
                    goto out_peg_859;
                }
            }
            result_peg_857.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_857.setValue(value);
            }
        
        
    }
    
    if (column_peg_631.chunk2 == 0){
        column_peg_631.chunk2 = new Chunk2();
    }
    column_peg_631.chunk2->chunk_range = result_peg_857;
    stream.update(result_peg_857.getPosition());
    
    
    return result_peg_857;
    out_peg_859:
    
    if (column_peg_631.chunk2 == 0){
        column_peg_631.chunk2 = new Chunk2();
    }
    column_peg_631.chunk2->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_933 = stream.getColumn(position);
    if (column_peg_933.chunk3 != 0 && column_peg_933.chunk3->chunk_name.calculated()){
        return column_peg_933.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_945(stream, "name");
    int myposition = position;
    
    
    Result result_peg_934(myposition);
    
    {
    
        {
                
                char letter_peg_940 = stream.get(result_peg_934.getPosition());
                if (letter_peg_940 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_940) != NULL){
                    result_peg_934.nextPosition();
                    result_peg_934.setValue((void*) (long) letter_peg_940);
                } else {
                    goto out_peg_939;
                }
                
            }
            goto success_peg_936;
            out_peg_939:
            goto out_peg_941;
            success_peg_936:
            ;
        
        Result result_peg_935 = result_peg_934;
        
        result_peg_934.reset();
            do{
                Result result_peg_944(result_peg_934.getPosition());
                result_peg_944 = rule_alpha_digit(stream, result_peg_944.getPosition());
                if (result_peg_944.error()){
                    goto loop_peg_943;
                }
                result_peg_934.addResult(result_peg_944);
            } while (true);
            loop_peg_943:
            ;
        
        Result result_peg_942 = result_peg_934;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_935.getValues().getValue(),result_peg_942.getValues());
                result_peg_934.setValue(value);
            }
        
        
    }
    
    if (column_peg_933.chunk3 == 0){
        column_peg_933.chunk3 = new Chunk3();
    }
    column_peg_933.chunk3->chunk_name = result_peg_934;
    stream.update(result_peg_934.getPosition());
    
    
    return result_peg_934;
    out_peg_941:
    
    if (column_peg_933.chunk3 == 0){
        column_peg_933.chunk3 = new Chunk3();
    }
    column_peg_933.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_946 = stream.getColumn(position);
    if (column_peg_946.chunk3 != 0 && column_peg_946.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_946.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_961(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_947(myposition);
    
    {
        
        char letter_peg_952 = stream.get(result_peg_947.getPosition());
        if (letter_peg_952 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_952) != NULL){
            result_peg_947.nextPosition();
            result_peg_947.setValue((void*) (long) letter_peg_952);
        } else {
            goto out_peg_951;
        }
        
    }
    goto success_peg_948;
    out_peg_951:
    goto out_peg_953;
    success_peg_948:
    ;
    
    if (column_peg_946.chunk3 == 0){
        column_peg_946.chunk3 = new Chunk3();
    }
    column_peg_946.chunk3->chunk_alpha_digit = result_peg_947;
    stream.update(result_peg_947.getPosition());
    
    
    return result_peg_947;
    out_peg_953:
    Result result_peg_954(myposition);
    
    {
        
        char letter_peg_959 = stream.get(result_peg_954.getPosition());
        if (letter_peg_959 != '\0' && strchr("0123456789", letter_peg_959) != NULL){
            result_peg_954.nextPosition();
            result_peg_954.setValue((void*) (long) letter_peg_959);
        } else {
            goto out_peg_958;
        }
        
    }
    goto success_peg_955;
    out_peg_958:
    goto out_peg_960;
    success_peg_955:
    ;
    
    if (column_peg_946.chunk3 == 0){
        column_peg_946.chunk3 = new Chunk3();
    }
    column_peg_946.chunk3->chunk_alpha_digit = result_peg_954;
    stream.update(result_peg_954.getPosition());
    
    
    return result_peg_954;
    out_peg_960:
    
    if (column_peg_946.chunk3 == 0){
        column_peg_946.chunk3 = new Chunk3();
    }
    column_peg_946.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_962 = stream.getColumn(position);
    if (column_peg_962.chunk3 != 0 && column_peg_962.chunk3->chunk_valuelist.calculated()){
        return column_peg_962.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1080(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_963(myposition);
    
    {
    
        result_peg_963 = rule_expr_c(stream, result_peg_963.getPosition());
            if (result_peg_963.error()){
                goto out_peg_965;
            }
            first = result_peg_963.getValues();
        
        
        
        result_peg_963.reset();
            do{
                Result result_peg_968(result_peg_963.getPosition());
                {
                
                    {
                            
                            result_peg_968.reset();
                            do{
                                Result result_peg_980(result_peg_968.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_980.getPosition()))){
                                            result_peg_980.nextPosition();
                                        } else {
                                            goto out_peg_983;
                                        }
                                    }
                                    result_peg_980.setValue((void*) " ");
                                        
                                }
                                goto success_peg_981;
                                out_peg_983:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_980.getPosition()))){
                                            result_peg_980.nextPosition();
                                        } else {
                                            goto out_peg_985;
                                        }
                                    }
                                    result_peg_980.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_981;
                                out_peg_985:
                                goto loop_peg_979;
                                success_peg_981:
                                ;
                                result_peg_968.addResult(result_peg_980);
                            } while (true);
                            loop_peg_979:
                            ;
                                    
                        }
                        goto success_peg_970;
                        goto loop_peg_967;
                        success_peg_970:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_968.getPosition()))){
                                result_peg_968.nextPosition();
                            } else {
                                goto loop_peg_967;
                            }
                        }
                        result_peg_968.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_968.reset();
                            do{
                                Result result_peg_998(result_peg_968.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_998.getPosition()))){
                                            result_peg_998.nextPosition();
                                        } else {
                                            goto out_peg_1001;
                                        }
                                    }
                                    result_peg_998.setValue((void*) " ");
                                        
                                }
                                goto success_peg_999;
                                out_peg_1001:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_998.getPosition()))){
                                            result_peg_998.nextPosition();
                                        } else {
                                            goto out_peg_1003;
                                        }
                                    }
                                    result_peg_998.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_999;
                                out_peg_1003:
                                goto loop_peg_997;
                                success_peg_999:
                                ;
                                result_peg_968.addResult(result_peg_998);
                            } while (true);
                            loop_peg_997:
                            ;
                                    
                        }
                        goto success_peg_988;
                        goto loop_peg_967;
                        success_peg_988:
                        ;
                    
                    
                    
                    result_peg_968 = rule_expr_c(stream, result_peg_968.getPosition());
                        if (result_peg_968.error()){
                            goto loop_peg_967;
                        }
                    
                    
                }
                result_peg_963.addResult(result_peg_968);
            } while (true);
            loop_peg_967:
            if (result_peg_963.matches() == 0){
                goto out_peg_965;
            }
            rest = result_peg_963.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_963.setValue(value);
            }
        
        
    }
    
    if (column_peg_962.chunk3 == 0){
        column_peg_962.chunk3 = new Chunk3();
    }
    column_peg_962.chunk3->chunk_valuelist = result_peg_963;
    stream.update(result_peg_963.getPosition());
    
    
    return result_peg_963;
    out_peg_965:
    Result result_peg_1004(myposition);
    
    {
    
        {
                
                result_peg_1004.reset();
                do{
                    Result result_peg_1016(result_peg_1004.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1016.getPosition()))){
                                result_peg_1016.nextPosition();
                            } else {
                                goto out_peg_1019;
                            }
                        }
                        result_peg_1016.setValue((void*) " ");
                            
                    }
                    goto success_peg_1017;
                    out_peg_1019:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1016.getPosition()))){
                                result_peg_1016.nextPosition();
                            } else {
                                goto out_peg_1021;
                            }
                        }
                        result_peg_1016.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1017;
                    out_peg_1021:
                    goto loop_peg_1015;
                    success_peg_1017:
                    ;
                    result_peg_1004.addResult(result_peg_1016);
                } while (true);
                loop_peg_1015:
                ;
                        
            }
            goto success_peg_1006;
            goto out_peg_1022;
            success_peg_1006:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1004.getPosition()))){
                    result_peg_1004.nextPosition();
                } else {
                    goto out_peg_1022;
                }
            }
            result_peg_1004.setValue((void*) ",");
        
        
        
        {
                
                result_peg_1004.reset();
                do{
                    Result result_peg_1035(result_peg_1004.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1035.getPosition()))){
                                result_peg_1035.nextPosition();
                            } else {
                                goto out_peg_1038;
                            }
                        }
                        result_peg_1035.setValue((void*) " ");
                            
                    }
                    goto success_peg_1036;
                    out_peg_1038:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1035.getPosition()))){
                                result_peg_1035.nextPosition();
                            } else {
                                goto out_peg_1040;
                            }
                        }
                        result_peg_1035.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1036;
                    out_peg_1040:
                    goto loop_peg_1034;
                    success_peg_1036:
                    ;
                    result_peg_1004.addResult(result_peg_1035);
                } while (true);
                loop_peg_1034:
                ;
                        
            }
            goto success_peg_1025;
            goto out_peg_1022;
            success_peg_1025:
            ;
        
        
        
        result_peg_1004 = rule_expr_c(stream, result_peg_1004.getPosition());
            if (result_peg_1004.error()){
                goto out_peg_1022;
            }
            first = result_peg_1004.getValues();
        
        
        
        result_peg_1004.reset();
            do{
                Result result_peg_1044(result_peg_1004.getPosition());
                {
                
                    {
                            
                            result_peg_1044.reset();
                            do{
                                Result result_peg_1056(result_peg_1044.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1056.getPosition()))){
                                            result_peg_1056.nextPosition();
                                        } else {
                                            goto out_peg_1059;
                                        }
                                    }
                                    result_peg_1056.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1057;
                                out_peg_1059:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1056.getPosition()))){
                                            result_peg_1056.nextPosition();
                                        } else {
                                            goto out_peg_1061;
                                        }
                                    }
                                    result_peg_1056.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1057;
                                out_peg_1061:
                                goto loop_peg_1055;
                                success_peg_1057:
                                ;
                                result_peg_1044.addResult(result_peg_1056);
                            } while (true);
                            loop_peg_1055:
                            ;
                                    
                        }
                        goto success_peg_1046;
                        goto loop_peg_1043;
                        success_peg_1046:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1044.getPosition()))){
                                result_peg_1044.nextPosition();
                            } else {
                                goto loop_peg_1043;
                            }
                        }
                        result_peg_1044.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_1044.reset();
                            do{
                                Result result_peg_1074(result_peg_1044.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1074.getPosition()))){
                                            result_peg_1074.nextPosition();
                                        } else {
                                            goto out_peg_1077;
                                        }
                                    }
                                    result_peg_1074.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1075;
                                out_peg_1077:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1074.getPosition()))){
                                            result_peg_1074.nextPosition();
                                        } else {
                                            goto out_peg_1079;
                                        }
                                    }
                                    result_peg_1074.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1075;
                                out_peg_1079:
                                goto loop_peg_1073;
                                success_peg_1075:
                                ;
                                result_peg_1044.addResult(result_peg_1074);
                            } while (true);
                            loop_peg_1073:
                            ;
                                    
                        }
                        goto success_peg_1064;
                        goto loop_peg_1043;
                        success_peg_1064:
                        ;
                    
                    
                    
                    result_peg_1044 = rule_expr_c(stream, result_peg_1044.getPosition());
                        if (result_peg_1044.error()){
                            goto loop_peg_1043;
                        }
                    
                    
                }
                result_peg_1004.addResult(result_peg_1044);
            } while (true);
            loop_peg_1043:
            ;
            rest = result_peg_1004.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_1004.setValue(value);
            }
        
        
    }
    
    if (column_peg_962.chunk3 == 0){
        column_peg_962.chunk3 = new Chunk3();
    }
    column_peg_962.chunk3->chunk_valuelist = result_peg_1004;
    stream.update(result_peg_1004.getPosition());
    
    
    return result_peg_1004;
    out_peg_1022:
    
    if (column_peg_962.chunk3 == 0){
        column_peg_962.chunk3 = new Chunk3();
    }
    column_peg_962.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1081 = stream.getColumn(position);
    if (column_peg_1081.chunk3 != 0 && column_peg_1081.chunk3->chunk_expr.calculated()){
        return column_peg_1081.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_1108(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_1082(myposition);
    
    {
    
        result_peg_1082 = rule_expr_c(stream, result_peg_1082.getPosition());
            if (result_peg_1082.error()){
                goto out_peg_1084;
            }
        
        Result result_peg_1083 = result_peg_1082;
        
        Result result_peg_1087(result_peg_1082);
            {
            
                {
                        
                        result_peg_1087.reset();
                        do{
                            Result result_peg_1099(result_peg_1087.getPosition());
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(" "[i], stream.get(result_peg_1099.getPosition()))){
                                        result_peg_1099.nextPosition();
                                    } else {
                                        goto out_peg_1102;
                                    }
                                }
                                result_peg_1099.setValue((void*) " ");
                                    
                            }
                            goto success_peg_1100;
                            out_peg_1102:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\t"[i], stream.get(result_peg_1099.getPosition()))){
                                        result_peg_1099.nextPosition();
                                    } else {
                                        goto out_peg_1104;
                                    }
                                }
                                result_peg_1099.setValue((void*) "\t");
                                    
                            }
                            goto success_peg_1100;
                            out_peg_1104:
                            goto loop_peg_1098;
                            success_peg_1100:
                            ;
                            result_peg_1087.addResult(result_peg_1099);
                        } while (true);
                        loop_peg_1098:
                        ;
                                
                    }
                    goto success_peg_1089;
                    goto not_peg_1086;
                    success_peg_1089:
                    ;
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_1087.getPosition()))){
                            result_peg_1087.nextPosition();
                        } else {
                            goto not_peg_1086;
                        }
                    }
                    result_peg_1087.setValue((void*) ",");
                
                
            }
            goto out_peg_1084;
            not_peg_1086:
            result_peg_1082.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1083.getValues();
                result_peg_1082.setValue(value);
            }
        
        
    }
    
    if (column_peg_1081.chunk3 == 0){
        column_peg_1081.chunk3 = new Chunk3();
    }
    column_peg_1081.chunk3->chunk_expr = result_peg_1082;
    stream.update(result_peg_1082.getPosition());
    
    
    return result_peg_1082;
    out_peg_1084:
    Result result_peg_1105(myposition);
    
    {
    
        result_peg_1105 = rule_valuelist(stream, result_peg_1105.getPosition());
            if (result_peg_1105.error()){
                goto out_peg_1107;
            }
        
        Result result_peg_1106 = result_peg_1105;
        
        result_peg_1105 = rule_expr2_rest(stream, result_peg_1105.getPosition(), result_peg_1106.getValues());
            if (result_peg_1105.error()){
                goto out_peg_1107;
            }
        
        
    }
    
    if (column_peg_1081.chunk3 == 0){
        column_peg_1081.chunk3 = new Chunk3();
    }
    column_peg_1081.chunk3->chunk_expr = result_peg_1105;
    stream.update(result_peg_1105.getPosition());
    
    
    return result_peg_1105;
    out_peg_1107:
    
    if (column_peg_1081.chunk3 == 0){
        column_peg_1081.chunk3 = new Chunk3();
    }
    column_peg_1081.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1109 = stream.getColumn(position);
    if (column_peg_1109.chunk3 != 0 && column_peg_1109.chunk3->chunk_expr_c.calculated()){
        return column_peg_1109.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_1113(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_1110(myposition);
    
    {
    
        result_peg_1110 = rule_expr2(stream, result_peg_1110.getPosition());
            if (result_peg_1110.error()){
                goto out_peg_1112;
            }
            left = result_peg_1110.getValues();
        
        
        
        result_peg_1110 = rule_expr_rest(stream, result_peg_1110.getPosition(), left);
            if (result_peg_1110.error()){
                goto out_peg_1112;
            }
        
        
    }
    
    if (column_peg_1109.chunk3 == 0){
        column_peg_1109.chunk3 = new Chunk3();
    }
    column_peg_1109.chunk3->chunk_expr_c = result_peg_1110;
    stream.update(result_peg_1110.getPosition());
    
    
    return result_peg_1110;
    out_peg_1112:
    
    if (column_peg_1109.chunk3 == 0){
        column_peg_1109.chunk3 = new Chunk3();
    }
    column_peg_1109.chunk3->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1114 = stream.getColumn(position);
    if (column_peg_1114.chunk4 != 0 && column_peg_1114.chunk4->chunk_expr2.calculated()){
        return column_peg_1114.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_1118(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_1115(myposition);
    
    {
    
        result_peg_1115 = rule_expr3(stream, result_peg_1115.getPosition());
            if (result_peg_1115.error()){
                goto out_peg_1117;
            }
            left = result_peg_1115.getValues();
        
        
        
        result_peg_1115 = rule_expr2_rest(stream, result_peg_1115.getPosition(), left);
            if (result_peg_1115.error()){
                goto out_peg_1117;
            }
        
        
    }
    
    if (column_peg_1114.chunk4 == 0){
        column_peg_1114.chunk4 = new Chunk4();
    }
    column_peg_1114.chunk4->chunk_expr2 = result_peg_1115;
    stream.update(result_peg_1115.getPosition());
    
    
    return result_peg_1115;
    out_peg_1117:
    
    if (column_peg_1114.chunk4 == 0){
        column_peg_1114.chunk4 = new Chunk4();
    }
    column_peg_1114.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1119 = stream.getColumn(position);
    if (column_peg_1119.chunk4 != 0 && column_peg_1119.chunk4->chunk_expr3.calculated()){
        return column_peg_1119.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_1123(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_1120(myposition);
    
    {
    
        result_peg_1120 = rule_expr4(stream, result_peg_1120.getPosition());
            if (result_peg_1120.error()){
                goto out_peg_1122;
            }
            left = result_peg_1120.getValues();
        
        
        
        result_peg_1120 = rule_expr3_rest(stream, result_peg_1120.getPosition(), left);
            if (result_peg_1120.error()){
                goto out_peg_1122;
            }
        
        
    }
    
    if (column_peg_1119.chunk4 == 0){
        column_peg_1119.chunk4 = new Chunk4();
    }
    column_peg_1119.chunk4->chunk_expr3 = result_peg_1120;
    stream.update(result_peg_1120.getPosition());
    
    
    return result_peg_1120;
    out_peg_1122:
    
    if (column_peg_1119.chunk4 == 0){
        column_peg_1119.chunk4 = new Chunk4();
    }
    column_peg_1119.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1124 = stream.getColumn(position);
    if (column_peg_1124.chunk4 != 0 && column_peg_1124.chunk4->chunk_expr4.calculated()){
        return column_peg_1124.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_1128(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_1125(myposition);
    
    {
    
        result_peg_1125 = rule_expr5(stream, result_peg_1125.getPosition());
            if (result_peg_1125.error()){
                goto out_peg_1127;
            }
            left = result_peg_1125.getValues();
        
        
        
        result_peg_1125 = rule_expr4_rest(stream, result_peg_1125.getPosition(), left);
            if (result_peg_1125.error()){
                goto out_peg_1127;
            }
        
        
    }
    
    if (column_peg_1124.chunk4 == 0){
        column_peg_1124.chunk4 = new Chunk4();
    }
    column_peg_1124.chunk4->chunk_expr4 = result_peg_1125;
    stream.update(result_peg_1125.getPosition());
    
    
    return result_peg_1125;
    out_peg_1127:
    
    if (column_peg_1124.chunk4 == 0){
        column_peg_1124.chunk4 = new Chunk4();
    }
    column_peg_1124.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1129 = stream.getColumn(position);
    if (column_peg_1129.chunk4 != 0 && column_peg_1129.chunk4->chunk_expr5.calculated()){
        return column_peg_1129.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_1133(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_1130(myposition);
    
    {
    
        result_peg_1130 = rule_expr6(stream, result_peg_1130.getPosition());
            if (result_peg_1130.error()){
                goto out_peg_1132;
            }
            left = result_peg_1130.getValues();
        
        
        
        result_peg_1130 = rule_expr5_rest(stream, result_peg_1130.getPosition(), left);
            if (result_peg_1130.error()){
                goto out_peg_1132;
            }
        
        
    }
    
    if (column_peg_1129.chunk4 == 0){
        column_peg_1129.chunk4 = new Chunk4();
    }
    column_peg_1129.chunk4->chunk_expr5 = result_peg_1130;
    stream.update(result_peg_1130.getPosition());
    
    
    return result_peg_1130;
    out_peg_1132:
    
    if (column_peg_1129.chunk4 == 0){
        column_peg_1129.chunk4 = new Chunk4();
    }
    column_peg_1129.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1134 = stream.getColumn(position);
    if (column_peg_1134.chunk4 != 0 && column_peg_1134.chunk4->chunk_expr6.calculated()){
        return column_peg_1134.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_1138(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_1135(myposition);
    
    {
    
        result_peg_1135 = rule_expr7(stream, result_peg_1135.getPosition());
            if (result_peg_1135.error()){
                goto out_peg_1137;
            }
            left = result_peg_1135.getValues();
        
        
        
        result_peg_1135 = rule_expr6_rest(stream, result_peg_1135.getPosition(), left);
            if (result_peg_1135.error()){
                goto out_peg_1137;
            }
        
        
    }
    
    if (column_peg_1134.chunk4 == 0){
        column_peg_1134.chunk4 = new Chunk4();
    }
    column_peg_1134.chunk4->chunk_expr6 = result_peg_1135;
    stream.update(result_peg_1135.getPosition());
    
    
    return result_peg_1135;
    out_peg_1137:
    
    if (column_peg_1134.chunk4 == 0){
        column_peg_1134.chunk4 = new Chunk4();
    }
    column_peg_1134.chunk4->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1139 = stream.getColumn(position);
    if (column_peg_1139.chunk5 != 0 && column_peg_1139.chunk5->chunk_expr7.calculated()){
        return column_peg_1139.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_1143(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_1140(myposition);
    
    {
    
        result_peg_1140 = rule_expr8(stream, result_peg_1140.getPosition());
            if (result_peg_1140.error()){
                goto out_peg_1142;
            }
            left = result_peg_1140.getValues();
        
        
        
        result_peg_1140 = rule_expr7_rest(stream, result_peg_1140.getPosition(), left);
            if (result_peg_1140.error()){
                goto out_peg_1142;
            }
        
        
    }
    
    if (column_peg_1139.chunk5 == 0){
        column_peg_1139.chunk5 = new Chunk5();
    }
    column_peg_1139.chunk5->chunk_expr7 = result_peg_1140;
    stream.update(result_peg_1140.getPosition());
    
    
    return result_peg_1140;
    out_peg_1142:
    
    if (column_peg_1139.chunk5 == 0){
        column_peg_1139.chunk5 = new Chunk5();
    }
    column_peg_1139.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1144 = stream.getColumn(position);
    if (column_peg_1144.chunk5 != 0 && column_peg_1144.chunk5->chunk_expr8.calculated()){
        return column_peg_1144.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_1148(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_1145(myposition);
    
    {
    
        result_peg_1145 = rule_expr9(stream, result_peg_1145.getPosition());
            if (result_peg_1145.error()){
                goto out_peg_1147;
            }
            left = result_peg_1145.getValues();
        
        
        
        result_peg_1145 = rule_expr8_rest(stream, result_peg_1145.getPosition(), left);
            if (result_peg_1145.error()){
                goto out_peg_1147;
            }
        
        
    }
    
    if (column_peg_1144.chunk5 == 0){
        column_peg_1144.chunk5 = new Chunk5();
    }
    column_peg_1144.chunk5->chunk_expr8 = result_peg_1145;
    stream.update(result_peg_1145.getPosition());
    
    
    return result_peg_1145;
    out_peg_1147:
    
    if (column_peg_1144.chunk5 == 0){
        column_peg_1144.chunk5 = new Chunk5();
    }
    column_peg_1144.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1149 = stream.getColumn(position);
    if (column_peg_1149.chunk5 != 0 && column_peg_1149.chunk5->chunk_expr9.calculated()){
        return column_peg_1149.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_1153(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_1150(myposition);
    
    {
    
        result_peg_1150 = rule_expr10(stream, result_peg_1150.getPosition());
            if (result_peg_1150.error()){
                goto out_peg_1152;
            }
            left = result_peg_1150.getValues();
        
        
        
        result_peg_1150 = rule_expr9_rest(stream, result_peg_1150.getPosition(), left);
            if (result_peg_1150.error()){
                goto out_peg_1152;
            }
        
        
    }
    
    if (column_peg_1149.chunk5 == 0){
        column_peg_1149.chunk5 = new Chunk5();
    }
    column_peg_1149.chunk5->chunk_expr9 = result_peg_1150;
    stream.update(result_peg_1150.getPosition());
    
    
    return result_peg_1150;
    out_peg_1152:
    
    if (column_peg_1149.chunk5 == 0){
        column_peg_1149.chunk5 = new Chunk5();
    }
    column_peg_1149.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1154 = stream.getColumn(position);
    if (column_peg_1154.chunk5 != 0 && column_peg_1154.chunk5->chunk_expr10.calculated()){
        return column_peg_1154.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_1158(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_1155(myposition);
    
    {
    
        result_peg_1155 = rule_expr11(stream, result_peg_1155.getPosition());
            if (result_peg_1155.error()){
                goto out_peg_1157;
            }
            left = result_peg_1155.getValues();
        
        
        
        result_peg_1155 = rule_expr10_rest(stream, result_peg_1155.getPosition(), left);
            if (result_peg_1155.error()){
                goto out_peg_1157;
            }
        
        
    }
    
    if (column_peg_1154.chunk5 == 0){
        column_peg_1154.chunk5 = new Chunk5();
    }
    column_peg_1154.chunk5->chunk_expr10 = result_peg_1155;
    stream.update(result_peg_1155.getPosition());
    
    
    return result_peg_1155;
    out_peg_1157:
    
    if (column_peg_1154.chunk5 == 0){
        column_peg_1154.chunk5 = new Chunk5();
    }
    column_peg_1154.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1159 = stream.getColumn(position);
    if (column_peg_1159.chunk5 != 0 && column_peg_1159.chunk5->chunk_expr11.calculated()){
        return column_peg_1159.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_1163(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_1160(myposition);
    
    {
    
        result_peg_1160 = rule_expr12(stream, result_peg_1160.getPosition());
            if (result_peg_1160.error()){
                goto out_peg_1162;
            }
            left = result_peg_1160.getValues();
        
        
        
        result_peg_1160 = rule_expr11_rest(stream, result_peg_1160.getPosition(), left);
            if (result_peg_1160.error()){
                goto out_peg_1162;
            }
        
        
    }
    
    if (column_peg_1159.chunk5 == 0){
        column_peg_1159.chunk5 = new Chunk5();
    }
    column_peg_1159.chunk5->chunk_expr11 = result_peg_1160;
    stream.update(result_peg_1160.getPosition());
    
    
    return result_peg_1160;
    out_peg_1162:
    
    if (column_peg_1159.chunk5 == 0){
        column_peg_1159.chunk5 = new Chunk5();
    }
    column_peg_1159.chunk5->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1164 = stream.getColumn(position);
    if (column_peg_1164.chunk6 != 0 && column_peg_1164.chunk6->chunk_expr12.calculated()){
        return column_peg_1164.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_1168(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_1165(myposition);
    
    {
    
        result_peg_1165 = rule_expr13(stream, result_peg_1165.getPosition());
            if (result_peg_1165.error()){
                goto out_peg_1167;
            }
            left = result_peg_1165.getValues();
        
        
        
        result_peg_1165 = rule_expr12_rest(stream, result_peg_1165.getPosition(), left);
            if (result_peg_1165.error()){
                goto out_peg_1167;
            }
        
        
    }
    
    if (column_peg_1164.chunk6 == 0){
        column_peg_1164.chunk6 = new Chunk6();
    }
    column_peg_1164.chunk6->chunk_expr12 = result_peg_1165;
    stream.update(result_peg_1165.getPosition());
    
    
    return result_peg_1165;
    out_peg_1167:
    
    if (column_peg_1164.chunk6 == 0){
        column_peg_1164.chunk6 = new Chunk6();
    }
    column_peg_1164.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1169 = stream.getColumn(position);
    if (column_peg_1169.chunk6 != 0 && column_peg_1169.chunk6->chunk_expr13.calculated()){
        return column_peg_1169.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_1176(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_1170(myposition);
    
    {
    
        result_peg_1170.reset();
            do{
                Result result_peg_1173(result_peg_1170.getPosition());
                result_peg_1173 = rule_unary(stream, result_peg_1173.getPosition());
                if (result_peg_1173.error()){
                    goto loop_peg_1172;
                }
                result_peg_1170.addResult(result_peg_1173);
            } while (true);
            loop_peg_1172:
            ;
        
        Result result_peg_1171 = result_peg_1170;
        
        result_peg_1170 = rule_expr13_real(stream, result_peg_1170.getPosition());
            if (result_peg_1170.error()){
                goto out_peg_1175;
            }
            exp = result_peg_1170.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_1171.getValues(), exp);
                result_peg_1170.setValue(value);
            }
        
        
    }
    
    if (column_peg_1169.chunk6 == 0){
        column_peg_1169.chunk6 = new Chunk6();
    }
    column_peg_1169.chunk6->chunk_expr13 = result_peg_1170;
    stream.update(result_peg_1170.getPosition());
    
    
    return result_peg_1170;
    out_peg_1175:
    
    if (column_peg_1169.chunk6 == 0){
        column_peg_1169.chunk6 = new Chunk6();
    }
    column_peg_1169.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1220(stream, "expr_rest");
    int myposition = position;
    tail_peg_1179:
    Value right;
    Value new_left;
    Result result_peg_1178(myposition);
    {
    
        {
                
                result_peg_1178.reset();
                do{
                    Result result_peg_1191(result_peg_1178.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1191.getPosition()))){
                                result_peg_1191.nextPosition();
                            } else {
                                goto out_peg_1194;
                            }
                        }
                        result_peg_1191.setValue((void*) " ");
                            
                    }
                    goto success_peg_1192;
                    out_peg_1194:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1191.getPosition()))){
                                result_peg_1191.nextPosition();
                            } else {
                                goto out_peg_1196;
                            }
                        }
                        result_peg_1191.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1192;
                    out_peg_1196:
                    goto loop_peg_1190;
                    success_peg_1192:
                    ;
                    result_peg_1178.addResult(result_peg_1191);
                } while (true);
                loop_peg_1190:
                ;
                        
            }
            goto success_peg_1181;
            goto out_peg_1197;
            success_peg_1181:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_1178.getPosition()))){
                    result_peg_1178.nextPosition();
                } else {
                    goto out_peg_1197;
                }
            }
            result_peg_1178.setValue((void*) "||");
        
        
        
        {
                
                result_peg_1178.reset();
                do{
                    Result result_peg_1210(result_peg_1178.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1210.getPosition()))){
                                result_peg_1210.nextPosition();
                            } else {
                                goto out_peg_1213;
                            }
                        }
                        result_peg_1210.setValue((void*) " ");
                            
                    }
                    goto success_peg_1211;
                    out_peg_1213:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1210.getPosition()))){
                                result_peg_1210.nextPosition();
                            } else {
                                goto out_peg_1215;
                            }
                        }
                        result_peg_1210.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1211;
                    out_peg_1215:
                    goto loop_peg_1209;
                    success_peg_1211:
                    ;
                    result_peg_1178.addResult(result_peg_1210);
                } while (true);
                loop_peg_1209:
                ;
                        
            }
            goto success_peg_1200;
            goto out_peg_1197;
            success_peg_1200:
            ;
        
        
        
        result_peg_1178 = rule_expr2(stream, result_peg_1178.getPosition());
            if (result_peg_1178.error()){
                goto out_peg_1197;
            }
            right = result_peg_1178.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_1178.setValue(value);
            }
            new_left = result_peg_1178.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1178.getPosition();
    goto tail_peg_1179;
    out_peg_1197:
    Result result_peg_1218(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1218.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1218.getPosition());
    
    return result_peg_1218;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1264(stream, "expr2_rest");
    int myposition = position;
    tail_peg_1223:
    Value right;
    Value new_left;
    Result result_peg_1222(myposition);
    {
    
        {
                
                result_peg_1222.reset();
                do{
                    Result result_peg_1235(result_peg_1222.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1235.getPosition()))){
                                result_peg_1235.nextPosition();
                            } else {
                                goto out_peg_1238;
                            }
                        }
                        result_peg_1235.setValue((void*) " ");
                            
                    }
                    goto success_peg_1236;
                    out_peg_1238:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1235.getPosition()))){
                                result_peg_1235.nextPosition();
                            } else {
                                goto out_peg_1240;
                            }
                        }
                        result_peg_1235.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1236;
                    out_peg_1240:
                    goto loop_peg_1234;
                    success_peg_1236:
                    ;
                    result_peg_1222.addResult(result_peg_1235);
                } while (true);
                loop_peg_1234:
                ;
                        
            }
            goto success_peg_1225;
            goto out_peg_1241;
            success_peg_1225:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_1222.getPosition()))){
                    result_peg_1222.nextPosition();
                } else {
                    goto out_peg_1241;
                }
            }
            result_peg_1222.setValue((void*) "^^");
        
        
        
        {
                
                result_peg_1222.reset();
                do{
                    Result result_peg_1254(result_peg_1222.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1254.getPosition()))){
                                result_peg_1254.nextPosition();
                            } else {
                                goto out_peg_1257;
                            }
                        }
                        result_peg_1254.setValue((void*) " ");
                            
                    }
                    goto success_peg_1255;
                    out_peg_1257:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1254.getPosition()))){
                                result_peg_1254.nextPosition();
                            } else {
                                goto out_peg_1259;
                            }
                        }
                        result_peg_1254.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1255;
                    out_peg_1259:
                    goto loop_peg_1253;
                    success_peg_1255:
                    ;
                    result_peg_1222.addResult(result_peg_1254);
                } while (true);
                loop_peg_1253:
                ;
                        
            }
            goto success_peg_1244;
            goto out_peg_1241;
            success_peg_1244:
            ;
        
        
        
        result_peg_1222 = rule_expr3(stream, result_peg_1222.getPosition());
            if (result_peg_1222.error()){
                goto out_peg_1241;
            }
            right = result_peg_1222.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_1222.setValue(value);
            }
            new_left = result_peg_1222.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1222.getPosition();
    goto tail_peg_1223;
    out_peg_1241:
    Result result_peg_1262(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1262.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1262.getPosition());
    
    return result_peg_1262;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1308(stream, "expr3_rest");
    int myposition = position;
    tail_peg_1267:
    Value right;
    Value new_left;
    Result result_peg_1266(myposition);
    {
    
        {
                
                result_peg_1266.reset();
                do{
                    Result result_peg_1279(result_peg_1266.getPosition());
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
                    result_peg_1266.addResult(result_peg_1279);
                } while (true);
                loop_peg_1278:
                ;
                        
            }
            goto success_peg_1269;
            goto out_peg_1285;
            success_peg_1269:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_1266.getPosition()))){
                    result_peg_1266.nextPosition();
                } else {
                    goto out_peg_1285;
                }
            }
            result_peg_1266.setValue((void*) "&&");
        
        
        
        {
                
                result_peg_1266.reset();
                do{
                    Result result_peg_1298(result_peg_1266.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1298.getPosition()))){
                                result_peg_1298.nextPosition();
                            } else {
                                goto out_peg_1301;
                            }
                        }
                        result_peg_1298.setValue((void*) " ");
                            
                    }
                    goto success_peg_1299;
                    out_peg_1301:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1298.getPosition()))){
                                result_peg_1298.nextPosition();
                            } else {
                                goto out_peg_1303;
                            }
                        }
                        result_peg_1298.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1299;
                    out_peg_1303:
                    goto loop_peg_1297;
                    success_peg_1299:
                    ;
                    result_peg_1266.addResult(result_peg_1298);
                } while (true);
                loop_peg_1297:
                ;
                        
            }
            goto success_peg_1288;
            goto out_peg_1285;
            success_peg_1288:
            ;
        
        
        
        result_peg_1266 = rule_expr4(stream, result_peg_1266.getPosition());
            if (result_peg_1266.error()){
                goto out_peg_1285;
            }
            right = result_peg_1266.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_1266.setValue(value);
            }
            new_left = result_peg_1266.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1266.getPosition();
    goto tail_peg_1267;
    out_peg_1285:
    Result result_peg_1306(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1306.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1306.getPosition());
    
    return result_peg_1306;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1352(stream, "expr4_rest");
    int myposition = position;
    tail_peg_1311:
    Value right;
    Value new_left;
    Result result_peg_1310(myposition);
    {
    
        {
                
                result_peg_1310.reset();
                do{
                    Result result_peg_1323(result_peg_1310.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1323.getPosition()))){
                                result_peg_1323.nextPosition();
                            } else {
                                goto out_peg_1326;
                            }
                        }
                        result_peg_1323.setValue((void*) " ");
                            
                    }
                    goto success_peg_1324;
                    out_peg_1326:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1323.getPosition()))){
                                result_peg_1323.nextPosition();
                            } else {
                                goto out_peg_1328;
                            }
                        }
                        result_peg_1323.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1324;
                    out_peg_1328:
                    goto loop_peg_1322;
                    success_peg_1324:
                    ;
                    result_peg_1310.addResult(result_peg_1323);
                } while (true);
                loop_peg_1322:
                ;
                        
            }
            goto success_peg_1313;
            goto out_peg_1329;
            success_peg_1313:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_1310.getPosition()))){
                    result_peg_1310.nextPosition();
                } else {
                    goto out_peg_1329;
                }
            }
            result_peg_1310.setValue((void*) "|");
        
        
        
        {
                
                result_peg_1310.reset();
                do{
                    Result result_peg_1342(result_peg_1310.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1342.getPosition()))){
                                result_peg_1342.nextPosition();
                            } else {
                                goto out_peg_1345;
                            }
                        }
                        result_peg_1342.setValue((void*) " ");
                            
                    }
                    goto success_peg_1343;
                    out_peg_1345:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1342.getPosition()))){
                                result_peg_1342.nextPosition();
                            } else {
                                goto out_peg_1347;
                            }
                        }
                        result_peg_1342.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1343;
                    out_peg_1347:
                    goto loop_peg_1341;
                    success_peg_1343:
                    ;
                    result_peg_1310.addResult(result_peg_1342);
                } while (true);
                loop_peg_1341:
                ;
                        
            }
            goto success_peg_1332;
            goto out_peg_1329;
            success_peg_1332:
            ;
        
        
        
        result_peg_1310 = rule_expr5(stream, result_peg_1310.getPosition());
            if (result_peg_1310.error()){
                goto out_peg_1329;
            }
            right = result_peg_1310.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_1310.setValue(value);
            }
            new_left = result_peg_1310.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1310.getPosition();
    goto tail_peg_1311;
    out_peg_1329:
    Result result_peg_1350(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1350.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1350.getPosition());
    
    return result_peg_1350;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1396(stream, "expr5_rest");
    int myposition = position;
    tail_peg_1355:
    Value right;
    Value new_left;
    Result result_peg_1354(myposition);
    {
    
        {
                
                result_peg_1354.reset();
                do{
                    Result result_peg_1367(result_peg_1354.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1367.getPosition()))){
                                result_peg_1367.nextPosition();
                            } else {
                                goto out_peg_1370;
                            }
                        }
                        result_peg_1367.setValue((void*) " ");
                            
                    }
                    goto success_peg_1368;
                    out_peg_1370:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1367.getPosition()))){
                                result_peg_1367.nextPosition();
                            } else {
                                goto out_peg_1372;
                            }
                        }
                        result_peg_1367.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1368;
                    out_peg_1372:
                    goto loop_peg_1366;
                    success_peg_1368:
                    ;
                    result_peg_1354.addResult(result_peg_1367);
                } while (true);
                loop_peg_1366:
                ;
                        
            }
            goto success_peg_1357;
            goto out_peg_1373;
            success_peg_1357:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_1354.getPosition()))){
                    result_peg_1354.nextPosition();
                } else {
                    goto out_peg_1373;
                }
            }
            result_peg_1354.setValue((void*) "^");
        
        
        
        {
                
                result_peg_1354.reset();
                do{
                    Result result_peg_1386(result_peg_1354.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1386.getPosition()))){
                                result_peg_1386.nextPosition();
                            } else {
                                goto out_peg_1389;
                            }
                        }
                        result_peg_1386.setValue((void*) " ");
                            
                    }
                    goto success_peg_1387;
                    out_peg_1389:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1386.getPosition()))){
                                result_peg_1386.nextPosition();
                            } else {
                                goto out_peg_1391;
                            }
                        }
                        result_peg_1386.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1387;
                    out_peg_1391:
                    goto loop_peg_1385;
                    success_peg_1387:
                    ;
                    result_peg_1354.addResult(result_peg_1386);
                } while (true);
                loop_peg_1385:
                ;
                        
            }
            goto success_peg_1376;
            goto out_peg_1373;
            success_peg_1376:
            ;
        
        
        
        result_peg_1354 = rule_expr6(stream, result_peg_1354.getPosition());
            if (result_peg_1354.error()){
                goto out_peg_1373;
            }
            right = result_peg_1354.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_1354.setValue(value);
            }
            new_left = result_peg_1354.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1354.getPosition();
    goto tail_peg_1355;
    out_peg_1373:
    Result result_peg_1394(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1394.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1394.getPosition());
    
    return result_peg_1394;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1440(stream, "expr6_rest");
    int myposition = position;
    tail_peg_1399:
    Value right;
    Value new_left;
    Result result_peg_1398(myposition);
    {
    
        {
                
                result_peg_1398.reset();
                do{
                    Result result_peg_1411(result_peg_1398.getPosition());
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
                    result_peg_1398.addResult(result_peg_1411);
                } while (true);
                loop_peg_1410:
                ;
                        
            }
            goto success_peg_1401;
            goto out_peg_1417;
            success_peg_1401:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_1398.getPosition()))){
                    result_peg_1398.nextPosition();
                } else {
                    goto out_peg_1417;
                }
            }
            result_peg_1398.setValue((void*) "&");
        
        
        
        {
                
                result_peg_1398.reset();
                do{
                    Result result_peg_1430(result_peg_1398.getPosition());
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
                    result_peg_1398.addResult(result_peg_1430);
                } while (true);
                loop_peg_1429:
                ;
                        
            }
            goto success_peg_1420;
            goto out_peg_1417;
            success_peg_1420:
            ;
        
        
        
        result_peg_1398 = rule_expr7(stream, result_peg_1398.getPosition());
            if (result_peg_1398.error()){
                goto out_peg_1417;
            }
            right = result_peg_1398.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_1398.setValue(value);
            }
            new_left = result_peg_1398.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1398.getPosition();
    goto tail_peg_1399;
    out_peg_1417:
    Result result_peg_1438(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1438.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1438.getPosition());
    
    return result_peg_1438;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1484(stream, "expr7_rest");
    int myposition = position;
    tail_peg_1443:
    Value right;
    Value new_left;
    Result result_peg_1442(myposition);
    {
    
        {
                
                result_peg_1442.reset();
                do{
                    Result result_peg_1455(result_peg_1442.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1455.getPosition()))){
                                result_peg_1455.nextPosition();
                            } else {
                                goto out_peg_1458;
                            }
                        }
                        result_peg_1455.setValue((void*) " ");
                            
                    }
                    goto success_peg_1456;
                    out_peg_1458:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1455.getPosition()))){
                                result_peg_1455.nextPosition();
                            } else {
                                goto out_peg_1460;
                            }
                        }
                        result_peg_1455.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1456;
                    out_peg_1460:
                    goto loop_peg_1454;
                    success_peg_1456:
                    ;
                    result_peg_1442.addResult(result_peg_1455);
                } while (true);
                loop_peg_1454:
                ;
                        
            }
            goto success_peg_1445;
            goto out_peg_1461;
            success_peg_1445:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_1442.getPosition()))){
                    result_peg_1442.nextPosition();
                } else {
                    goto out_peg_1461;
                }
            }
            result_peg_1442.setValue((void*) ":=");
        
        
        
        {
                
                result_peg_1442.reset();
                do{
                    Result result_peg_1474(result_peg_1442.getPosition());
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
                    result_peg_1442.addResult(result_peg_1474);
                } while (true);
                loop_peg_1473:
                ;
                        
            }
            goto success_peg_1464;
            goto out_peg_1461;
            success_peg_1464:
            ;
        
        
        
        result_peg_1442 = rule_expr8(stream, result_peg_1442.getPosition());
            if (result_peg_1442.error()){
                goto out_peg_1461;
            }
            right = result_peg_1442.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_1442.setValue(value);
            }
            new_left = result_peg_1442.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1442.getPosition();
    goto tail_peg_1443;
    out_peg_1461:
    Result result_peg_1482(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1482.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1482.getPosition());
    
    return result_peg_1482;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1567(stream, "expr8_rest");
    int myposition = position;
    tail_peg_1487:
    Value right;
    Value new_left;
    Result result_peg_1486(myposition);
    {
    
        {
                
                result_peg_1486.reset();
                do{
                    Result result_peg_1499(result_peg_1486.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1499.getPosition()))){
                                result_peg_1499.nextPosition();
                            } else {
                                goto out_peg_1502;
                            }
                        }
                        result_peg_1499.setValue((void*) " ");
                            
                    }
                    goto success_peg_1500;
                    out_peg_1502:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1499.getPosition()))){
                                result_peg_1499.nextPosition();
                            } else {
                                goto out_peg_1504;
                            }
                        }
                        result_peg_1499.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1500;
                    out_peg_1504:
                    goto loop_peg_1498;
                    success_peg_1500:
                    ;
                    result_peg_1486.addResult(result_peg_1499);
                } while (true);
                loop_peg_1498:
                ;
                        
            }
            goto success_peg_1489;
            goto out_peg_1505;
            success_peg_1489:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1486.getPosition()))){
                    result_peg_1486.nextPosition();
                } else {
                    goto out_peg_1505;
                }
            }
            result_peg_1486.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1486.reset();
                do{
                    Result result_peg_1518(result_peg_1486.getPosition());
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
                    result_peg_1486.addResult(result_peg_1518);
                } while (true);
                loop_peg_1517:
                ;
                        
            }
            goto success_peg_1508;
            goto out_peg_1505;
            success_peg_1508:
            ;
        
        
        
        result_peg_1486 = rule_expr9(stream, result_peg_1486.getPosition());
            if (result_peg_1486.error()){
                goto out_peg_1505;
            }
            right = result_peg_1486.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_1486.setValue(value);
            }
            new_left = result_peg_1486.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1486.getPosition();
    goto tail_peg_1487;
    out_peg_1505:
    Result result_peg_1526(myposition);
    {
    
        {
                
                result_peg_1526.reset();
                do{
                    Result result_peg_1538(result_peg_1526.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1538.getPosition()))){
                                result_peg_1538.nextPosition();
                            } else {
                                goto out_peg_1541;
                            }
                        }
                        result_peg_1538.setValue((void*) " ");
                            
                    }
                    goto success_peg_1539;
                    out_peg_1541:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1538.getPosition()))){
                                result_peg_1538.nextPosition();
                            } else {
                                goto out_peg_1543;
                            }
                        }
                        result_peg_1538.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1539;
                    out_peg_1543:
                    goto loop_peg_1537;
                    success_peg_1539:
                    ;
                    result_peg_1526.addResult(result_peg_1538);
                } while (true);
                loop_peg_1537:
                ;
                        
            }
            goto success_peg_1528;
            goto out_peg_1544;
            success_peg_1528:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_1526.getPosition()))){
                    result_peg_1526.nextPosition();
                } else {
                    goto out_peg_1544;
                }
            }
            result_peg_1526.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_1526.reset();
                do{
                    Result result_peg_1557(result_peg_1526.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1557.getPosition()))){
                                result_peg_1557.nextPosition();
                            } else {
                                goto out_peg_1560;
                            }
                        }
                        result_peg_1557.setValue((void*) " ");
                            
                    }
                    goto success_peg_1558;
                    out_peg_1560:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1557.getPosition()))){
                                result_peg_1557.nextPosition();
                            } else {
                                goto out_peg_1562;
                            }
                        }
                        result_peg_1557.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1558;
                    out_peg_1562:
                    goto loop_peg_1556;
                    success_peg_1558:
                    ;
                    result_peg_1526.addResult(result_peg_1557);
                } while (true);
                loop_peg_1556:
                ;
                        
            }
            goto success_peg_1547;
            goto out_peg_1544;
            success_peg_1547:
            ;
        
        
        
        result_peg_1526 = rule_expr9(stream, result_peg_1526.getPosition());
            if (result_peg_1526.error()){
                goto out_peg_1544;
            }
            right = result_peg_1526.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_1526.setValue(value);
            }
            new_left = result_peg_1526.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1526.getPosition();
    goto tail_peg_1487;
    out_peg_1544:
    Result result_peg_1565(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1565.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1565.getPosition());
    
    return result_peg_1565;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1611(stream, "expr9_rest");
    int myposition = position;
    tail_peg_1570:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_1569(myposition);
    {
    
        {
                
                result_peg_1569.reset();
                do{
                    Result result_peg_1582(result_peg_1569.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1582.getPosition()))){
                                result_peg_1582.nextPosition();
                            } else {
                                goto out_peg_1585;
                            }
                        }
                        result_peg_1582.setValue((void*) " ");
                            
                    }
                    goto success_peg_1583;
                    out_peg_1585:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1582.getPosition()))){
                                result_peg_1582.nextPosition();
                            } else {
                                goto out_peg_1587;
                            }
                        }
                        result_peg_1582.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1583;
                    out_peg_1587:
                    goto loop_peg_1581;
                    success_peg_1583:
                    ;
                    result_peg_1569.addResult(result_peg_1582);
                } while (true);
                loop_peg_1581:
                ;
                        
            }
            goto success_peg_1572;
            goto out_peg_1588;
            success_peg_1572:
            ;
        
        
        
        result_peg_1569 = rule_compare(stream, result_peg_1569.getPosition());
            if (result_peg_1569.error()){
                goto out_peg_1588;
            }
            maker = result_peg_1569.getValues();
        
        
        
        {
                
                result_peg_1569.reset();
                do{
                    Result result_peg_1601(result_peg_1569.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1601.getPosition()))){
                                result_peg_1601.nextPosition();
                            } else {
                                goto out_peg_1604;
                            }
                        }
                        result_peg_1601.setValue((void*) " ");
                            
                    }
                    goto success_peg_1602;
                    out_peg_1604:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1601.getPosition()))){
                                result_peg_1601.nextPosition();
                            } else {
                                goto out_peg_1606;
                            }
                        }
                        result_peg_1601.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1602;
                    out_peg_1606:
                    goto loop_peg_1600;
                    success_peg_1602:
                    ;
                    result_peg_1569.addResult(result_peg_1601);
                } while (true);
                loop_peg_1600:
                ;
                        
            }
            goto success_peg_1591;
            goto out_peg_1588;
            success_peg_1591:
            ;
        
        
        
        result_peg_1569 = rule_expr10(stream, result_peg_1569.getPosition());
            if (result_peg_1569.error()){
                goto out_peg_1588;
            }
            right = result_peg_1569.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_1569.setValue(value);
            }
            new_left = result_peg_1569.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1569.getPosition();
    goto tail_peg_1570;
    out_peg_1588:
    Result result_peg_1609(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1609.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1609.getPosition());
    
    return result_peg_1609;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1612 = stream.getColumn(position);
    if (column_peg_1612.chunk8 != 0 && column_peg_1612.chunk8->chunk_compare.calculated()){
        return column_peg_1612.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1625(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_1613(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_1613.getPosition()))){
                    result_peg_1613.nextPosition();
                } else {
                    goto out_peg_1615;
                }
            }
            result_peg_1613.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_1613.setValue(value);
            }
        
        
    }
    
    if (column_peg_1612.chunk8 == 0){
        column_peg_1612.chunk8 = new Chunk8();
    }
    column_peg_1612.chunk8->chunk_compare = result_peg_1613;
    stream.update(result_peg_1613.getPosition());
    
    
    return result_peg_1613;
    out_peg_1615:
    Result result_peg_1616(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_1616.getPosition()))){
                    result_peg_1616.nextPosition();
                } else {
                    goto out_peg_1618;
                }
            }
            result_peg_1616.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_1616.setValue(value);
            }
        
        
    }
    
    if (column_peg_1612.chunk8 == 0){
        column_peg_1612.chunk8 = new Chunk8();
    }
    column_peg_1612.chunk8->chunk_compare = result_peg_1616;
    stream.update(result_peg_1616.getPosition());
    
    
    return result_peg_1616;
    out_peg_1618:
    Result result_peg_1619(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_1619.getPosition()))){
                    result_peg_1619.nextPosition();
                } else {
                    goto out_peg_1621;
                }
            }
            result_peg_1619.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_1619.setValue(value);
            }
        
        
    }
    
    if (column_peg_1612.chunk8 == 0){
        column_peg_1612.chunk8 = new Chunk8();
    }
    column_peg_1612.chunk8->chunk_compare = result_peg_1619;
    stream.update(result_peg_1619.getPosition());
    
    
    return result_peg_1619;
    out_peg_1621:
    Result result_peg_1622(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1622.getPosition()))){
                    result_peg_1622.nextPosition();
                } else {
                    goto out_peg_1624;
                }
            }
            result_peg_1622.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_1622.setValue(value);
            }
        
        
    }
    
    if (column_peg_1612.chunk8 == 0){
        column_peg_1612.chunk8 = new Chunk8();
    }
    column_peg_1612.chunk8->chunk_compare = result_peg_1622;
    stream.update(result_peg_1622.getPosition());
    
    
    return result_peg_1622;
    out_peg_1624:
    
    if (column_peg_1612.chunk8 == 0){
        column_peg_1612.chunk8 = new Chunk8();
    }
    column_peg_1612.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1626 = stream.getColumn(position);
    if (column_peg_1626.chunk8 != 0 && column_peg_1626.chunk8->chunk_all_compare.calculated()){
        return column_peg_1626.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1633(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_1627(myposition);
    
    result_peg_1627 = rule_compare(stream, result_peg_1627.getPosition());
    if (result_peg_1627.error()){
        goto out_peg_1628;
    }
    
    if (column_peg_1626.chunk8 == 0){
        column_peg_1626.chunk8 = new Chunk8();
    }
    column_peg_1626.chunk8->chunk_all_compare = result_peg_1627;
    stream.update(result_peg_1627.getPosition());
    
    
    return result_peg_1627;
    out_peg_1628:
    Result result_peg_1629(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_1629.getPosition()))){
            result_peg_1629.nextPosition();
        } else {
            goto out_peg_1630;
        }
    }
    result_peg_1629.setValue((void*) "=");
    
    if (column_peg_1626.chunk8 == 0){
        column_peg_1626.chunk8 = new Chunk8();
    }
    column_peg_1626.chunk8->chunk_all_compare = result_peg_1629;
    stream.update(result_peg_1629.getPosition());
    
    
    return result_peg_1629;
    out_peg_1630:
    Result result_peg_1631(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_1631.getPosition()))){
            result_peg_1631.nextPosition();
        } else {
            goto out_peg_1632;
        }
    }
    result_peg_1631.setValue((void*) "!=");
    
    if (column_peg_1626.chunk8 == 0){
        column_peg_1626.chunk8 = new Chunk8();
    }
    column_peg_1626.chunk8->chunk_all_compare = result_peg_1631;
    stream.update(result_peg_1631.getPosition());
    
    
    return result_peg_1631;
    out_peg_1632:
    
    if (column_peg_1626.chunk8 == 0){
        column_peg_1626.chunk8 = new Chunk8();
    }
    column_peg_1626.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1716(stream, "expr10_rest");
    int myposition = position;
    tail_peg_1636:
    Value right;
    Value new_left;
    Result result_peg_1635(myposition);
    {
    
        {
                
                result_peg_1635.reset();
                do{
                    Result result_peg_1648(result_peg_1635.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1648.getPosition()))){
                                result_peg_1648.nextPosition();
                            } else {
                                goto out_peg_1651;
                            }
                        }
                        result_peg_1648.setValue((void*) " ");
                            
                    }
                    goto success_peg_1649;
                    out_peg_1651:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1648.getPosition()))){
                                result_peg_1648.nextPosition();
                            } else {
                                goto out_peg_1653;
                            }
                        }
                        result_peg_1648.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1649;
                    out_peg_1653:
                    goto loop_peg_1647;
                    success_peg_1649:
                    ;
                    result_peg_1635.addResult(result_peg_1648);
                } while (true);
                loop_peg_1647:
                ;
                        
            }
            goto success_peg_1638;
            goto out_peg_1654;
            success_peg_1638:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_1635.getPosition()))){
                    result_peg_1635.nextPosition();
                } else {
                    goto out_peg_1654;
                }
            }
            result_peg_1635.setValue((void*) "+");
        
        
        
        {
                
                result_peg_1635.reset();
                do{
                    Result result_peg_1667(result_peg_1635.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1667.getPosition()))){
                                result_peg_1667.nextPosition();
                            } else {
                                goto out_peg_1670;
                            }
                        }
                        result_peg_1667.setValue((void*) " ");
                            
                    }
                    goto success_peg_1668;
                    out_peg_1670:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1667.getPosition()))){
                                result_peg_1667.nextPosition();
                            } else {
                                goto out_peg_1672;
                            }
                        }
                        result_peg_1667.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1668;
                    out_peg_1672:
                    goto loop_peg_1666;
                    success_peg_1668:
                    ;
                    result_peg_1635.addResult(result_peg_1667);
                } while (true);
                loop_peg_1666:
                ;
                        
            }
            goto success_peg_1657;
            goto out_peg_1654;
            success_peg_1657:
            ;
        
        
        
        result_peg_1635 = rule_expr11(stream, result_peg_1635.getPosition());
            if (result_peg_1635.error()){
                goto out_peg_1654;
            }
            right = result_peg_1635.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_1635.setValue(value);
            }
            new_left = result_peg_1635.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1635.getPosition();
    goto tail_peg_1636;
    out_peg_1654:
    Result result_peg_1675(myposition);
    {
    
        {
                
                result_peg_1675.reset();
                do{
                    Result result_peg_1687(result_peg_1675.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1687.getPosition()))){
                                result_peg_1687.nextPosition();
                            } else {
                                goto out_peg_1690;
                            }
                        }
                        result_peg_1687.setValue((void*) " ");
                            
                    }
                    goto success_peg_1688;
                    out_peg_1690:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1687.getPosition()))){
                                result_peg_1687.nextPosition();
                            } else {
                                goto out_peg_1692;
                            }
                        }
                        result_peg_1687.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1688;
                    out_peg_1692:
                    goto loop_peg_1686;
                    success_peg_1688:
                    ;
                    result_peg_1675.addResult(result_peg_1687);
                } while (true);
                loop_peg_1686:
                ;
                        
            }
            goto success_peg_1677;
            goto out_peg_1693;
            success_peg_1677:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1675.getPosition()))){
                    result_peg_1675.nextPosition();
                } else {
                    goto out_peg_1693;
                }
            }
            result_peg_1675.setValue((void*) "-");
        
        
        
        {
                
                result_peg_1675.reset();
                do{
                    Result result_peg_1706(result_peg_1675.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1706.getPosition()))){
                                result_peg_1706.nextPosition();
                            } else {
                                goto out_peg_1709;
                            }
                        }
                        result_peg_1706.setValue((void*) " ");
                            
                    }
                    goto success_peg_1707;
                    out_peg_1709:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1706.getPosition()))){
                                result_peg_1706.nextPosition();
                            } else {
                                goto out_peg_1711;
                            }
                        }
                        result_peg_1706.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1707;
                    out_peg_1711:
                    goto loop_peg_1705;
                    success_peg_1707:
                    ;
                    result_peg_1675.addResult(result_peg_1706);
                } while (true);
                loop_peg_1705:
                ;
                        
            }
            goto success_peg_1696;
            goto out_peg_1693;
            success_peg_1696:
            ;
        
        
        
        result_peg_1675 = rule_expr11(stream, result_peg_1675.getPosition());
            if (result_peg_1675.error()){
                goto out_peg_1693;
            }
            right = result_peg_1675.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_1675.setValue(value);
            }
            new_left = result_peg_1675.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1675.getPosition();
    goto tail_peg_1636;
    out_peg_1693:
    Result result_peg_1714(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1714.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1714.getPosition());
    
    return result_peg_1714;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1838(stream, "expr11_rest");
    int myposition = position;
    tail_peg_1719:
    Value right;
    Value new_left;
    Result result_peg_1718(myposition);
    {
    
        {
                
                result_peg_1718.reset();
                do{
                    Result result_peg_1731(result_peg_1718.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1731.getPosition()))){
                                result_peg_1731.nextPosition();
                            } else {
                                goto out_peg_1734;
                            }
                        }
                        result_peg_1731.setValue((void*) " ");
                            
                    }
                    goto success_peg_1732;
                    out_peg_1734:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1731.getPosition()))){
                                result_peg_1731.nextPosition();
                            } else {
                                goto out_peg_1736;
                            }
                        }
                        result_peg_1731.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1732;
                    out_peg_1736:
                    goto loop_peg_1730;
                    success_peg_1732:
                    ;
                    result_peg_1718.addResult(result_peg_1731);
                } while (true);
                loop_peg_1730:
                ;
                        
            }
            goto success_peg_1721;
            goto out_peg_1737;
            success_peg_1721:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_1718.getPosition()))){
                    result_peg_1718.nextPosition();
                } else {
                    goto out_peg_1737;
                }
            }
            result_peg_1718.setValue((void*) "*");
        
        
        
        {
                
                result_peg_1718.reset();
                do{
                    Result result_peg_1750(result_peg_1718.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1750.getPosition()))){
                                result_peg_1750.nextPosition();
                            } else {
                                goto out_peg_1753;
                            }
                        }
                        result_peg_1750.setValue((void*) " ");
                            
                    }
                    goto success_peg_1751;
                    out_peg_1753:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1750.getPosition()))){
                                result_peg_1750.nextPosition();
                            } else {
                                goto out_peg_1755;
                            }
                        }
                        result_peg_1750.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1751;
                    out_peg_1755:
                    goto loop_peg_1749;
                    success_peg_1751:
                    ;
                    result_peg_1718.addResult(result_peg_1750);
                } while (true);
                loop_peg_1749:
                ;
                        
            }
            goto success_peg_1740;
            goto out_peg_1737;
            success_peg_1740:
            ;
        
        
        
        result_peg_1718 = rule_expr12(stream, result_peg_1718.getPosition());
            if (result_peg_1718.error()){
                goto out_peg_1737;
            }
            right = result_peg_1718.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_1718.setValue(value);
            }
            new_left = result_peg_1718.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1718.getPosition();
    goto tail_peg_1719;
    out_peg_1737:
    Result result_peg_1758(myposition);
    {
    
        {
                
                result_peg_1758.reset();
                do{
                    Result result_peg_1770(result_peg_1758.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1770.getPosition()))){
                                result_peg_1770.nextPosition();
                            } else {
                                goto out_peg_1773;
                            }
                        }
                        result_peg_1770.setValue((void*) " ");
                            
                    }
                    goto success_peg_1771;
                    out_peg_1773:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1770.getPosition()))){
                                result_peg_1770.nextPosition();
                            } else {
                                goto out_peg_1775;
                            }
                        }
                        result_peg_1770.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1771;
                    out_peg_1775:
                    goto loop_peg_1769;
                    success_peg_1771:
                    ;
                    result_peg_1758.addResult(result_peg_1770);
                } while (true);
                loop_peg_1769:
                ;
                        
            }
            goto success_peg_1760;
            goto out_peg_1776;
            success_peg_1760:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1758.getPosition()))){
                    result_peg_1758.nextPosition();
                } else {
                    goto out_peg_1776;
                }
            }
            result_peg_1758.setValue((void*) "/");
        
        
        
        {
                
                result_peg_1758.reset();
                do{
                    Result result_peg_1789(result_peg_1758.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1789.getPosition()))){
                                result_peg_1789.nextPosition();
                            } else {
                                goto out_peg_1792;
                            }
                        }
                        result_peg_1789.setValue((void*) " ");
                            
                    }
                    goto success_peg_1790;
                    out_peg_1792:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1789.getPosition()))){
                                result_peg_1789.nextPosition();
                            } else {
                                goto out_peg_1794;
                            }
                        }
                        result_peg_1789.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1790;
                    out_peg_1794:
                    goto loop_peg_1788;
                    success_peg_1790:
                    ;
                    result_peg_1758.addResult(result_peg_1789);
                } while (true);
                loop_peg_1788:
                ;
                        
            }
            goto success_peg_1779;
            goto out_peg_1776;
            success_peg_1779:
            ;
        
        
        
        result_peg_1758 = rule_expr12(stream, result_peg_1758.getPosition());
            if (result_peg_1758.error()){
                goto out_peg_1776;
            }
            right = result_peg_1758.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_1758.setValue(value);
            }
            new_left = result_peg_1758.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1758.getPosition();
    goto tail_peg_1719;
    out_peg_1776:
    Result result_peg_1797(myposition);
    {
    
        {
                
                result_peg_1797.reset();
                do{
                    Result result_peg_1809(result_peg_1797.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1809.getPosition()))){
                                result_peg_1809.nextPosition();
                            } else {
                                goto out_peg_1812;
                            }
                        }
                        result_peg_1809.setValue((void*) " ");
                            
                    }
                    goto success_peg_1810;
                    out_peg_1812:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1809.getPosition()))){
                                result_peg_1809.nextPosition();
                            } else {
                                goto out_peg_1814;
                            }
                        }
                        result_peg_1809.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1810;
                    out_peg_1814:
                    goto loop_peg_1808;
                    success_peg_1810:
                    ;
                    result_peg_1797.addResult(result_peg_1809);
                } while (true);
                loop_peg_1808:
                ;
                        
            }
            goto success_peg_1799;
            goto out_peg_1815;
            success_peg_1799:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_1797.getPosition()))){
                    result_peg_1797.nextPosition();
                } else {
                    goto out_peg_1815;
                }
            }
            result_peg_1797.setValue((void*) "%");
        
        
        
        {
                
                result_peg_1797.reset();
                do{
                    Result result_peg_1828(result_peg_1797.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1828.getPosition()))){
                                result_peg_1828.nextPosition();
                            } else {
                                goto out_peg_1831;
                            }
                        }
                        result_peg_1828.setValue((void*) " ");
                            
                    }
                    goto success_peg_1829;
                    out_peg_1831:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1828.getPosition()))){
                                result_peg_1828.nextPosition();
                            } else {
                                goto out_peg_1833;
                            }
                        }
                        result_peg_1828.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1829;
                    out_peg_1833:
                    goto loop_peg_1827;
                    success_peg_1829:
                    ;
                    result_peg_1797.addResult(result_peg_1828);
                } while (true);
                loop_peg_1827:
                ;
                        
            }
            goto success_peg_1818;
            goto out_peg_1815;
            success_peg_1818:
            ;
        
        
        
        result_peg_1797 = rule_expr12(stream, result_peg_1797.getPosition());
            if (result_peg_1797.error()){
                goto out_peg_1815;
            }
            right = result_peg_1797.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_1797.setValue(value);
            }
            new_left = result_peg_1797.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1797.getPosition();
    goto tail_peg_1719;
    out_peg_1815:
    Result result_peg_1836(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1836.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1836.getPosition());
    
    return result_peg_1836;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1882(stream, "expr12_rest");
    int myposition = position;
    tail_peg_1841:
    Value right;
    Value new_left;
    Result result_peg_1840(myposition);
    {
    
        {
                
                result_peg_1840.reset();
                do{
                    Result result_peg_1853(result_peg_1840.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1853.getPosition()))){
                                result_peg_1853.nextPosition();
                            } else {
                                goto out_peg_1856;
                            }
                        }
                        result_peg_1853.setValue((void*) " ");
                            
                    }
                    goto success_peg_1854;
                    out_peg_1856:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1853.getPosition()))){
                                result_peg_1853.nextPosition();
                            } else {
                                goto out_peg_1858;
                            }
                        }
                        result_peg_1853.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1854;
                    out_peg_1858:
                    goto loop_peg_1852;
                    success_peg_1854:
                    ;
                    result_peg_1840.addResult(result_peg_1853);
                } while (true);
                loop_peg_1852:
                ;
                        
            }
            goto success_peg_1843;
            goto out_peg_1859;
            success_peg_1843:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_1840.getPosition()))){
                    result_peg_1840.nextPosition();
                } else {
                    goto out_peg_1859;
                }
            }
            result_peg_1840.setValue((void*) "**");
        
        
        
        {
                
                result_peg_1840.reset();
                do{
                    Result result_peg_1872(result_peg_1840.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1872.getPosition()))){
                                result_peg_1872.nextPosition();
                            } else {
                                goto out_peg_1875;
                            }
                        }
                        result_peg_1872.setValue((void*) " ");
                            
                    }
                    goto success_peg_1873;
                    out_peg_1875:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1872.getPosition()))){
                                result_peg_1872.nextPosition();
                            } else {
                                goto out_peg_1877;
                            }
                        }
                        result_peg_1872.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1873;
                    out_peg_1877:
                    goto loop_peg_1871;
                    success_peg_1873:
                    ;
                    result_peg_1840.addResult(result_peg_1872);
                } while (true);
                loop_peg_1871:
                ;
                        
            }
            goto success_peg_1862;
            goto out_peg_1859;
            success_peg_1862:
            ;
        
        
        
        result_peg_1840 = rule_expr13(stream, result_peg_1840.getPosition());
            if (result_peg_1840.error()){
                goto out_peg_1859;
            }
            right = result_peg_1840.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_1840.setValue(value);
            }
            new_left = result_peg_1840.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1840.getPosition();
    goto tail_peg_1841;
    out_peg_1859:
    Result result_peg_1880(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1880.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1880.getPosition());
    
    return result_peg_1880;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_1883 = stream.getColumn(position);
    if (column_peg_1883.chunk9 != 0 && column_peg_1883.chunk9->chunk_unary.calculated()){
        return column_peg_1883.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_1912(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_1884(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_1884.getPosition()))){
                    result_peg_1884.nextPosition();
                } else {
                    goto out_peg_1886;
                }
            }
            result_peg_1884.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_1884.setValue(value);
            }
        
        
    }
    
    if (column_peg_1883.chunk9 == 0){
        column_peg_1883.chunk9 = new Chunk9();
    }
    column_peg_1883.chunk9->chunk_unary = result_peg_1884;
    stream.update(result_peg_1884.getPosition());
    
    
    return result_peg_1884;
    out_peg_1886:
    Result result_peg_1887(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1887.getPosition()))){
                    result_peg_1887.nextPosition();
                } else {
                    goto out_peg_1889;
                }
            }
            result_peg_1887.setValue((void*) "-");
        
        
        
        Result result_peg_1892(result_peg_1887);
            {
                
                char letter_peg_1897 = stream.get(result_peg_1892.getPosition());
                if (letter_peg_1897 != '\0' && strchr("0123456789", letter_peg_1897) != NULL){
                    result_peg_1892.nextPosition();
                    result_peg_1892.setValue((void*) (long) letter_peg_1897);
                } else {
                    goto out_peg_1896;
                }
                
            }
            goto success_peg_1893;
            out_peg_1896:
            goto not_peg_1891;
            success_peg_1893:
            ;
            goto out_peg_1889;
            not_peg_1891:
            result_peg_1887.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_1887.setValue(value);
            }
        
        
    }
    
    if (column_peg_1883.chunk9 == 0){
        column_peg_1883.chunk9 = new Chunk9();
    }
    column_peg_1883.chunk9->chunk_unary = result_peg_1887;
    stream.update(result_peg_1887.getPosition());
    
    
    return result_peg_1887;
    out_peg_1889:
    Result result_peg_1898(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1898.getPosition()))){
                    result_peg_1898.nextPosition();
                } else {
                    goto out_peg_1900;
                }
            }
            result_peg_1898.setValue((void*) "~");
        
        
        
        Result result_peg_1903(result_peg_1898);
            result_peg_1903 = rule_key(stream, result_peg_1903.getPosition());
            if (result_peg_1903.error()){
                goto not_peg_1902;
            }
            goto out_peg_1900;
            not_peg_1902:
            result_peg_1898.setValue((void*)0);
        
        
        
        Result result_peg_1906(result_peg_1898);
            {
                
                char letter_peg_1911 = stream.get(result_peg_1906.getPosition());
                if (letter_peg_1911 != '\0' && strchr("0123456789", letter_peg_1911) != NULL){
                    result_peg_1906.nextPosition();
                    result_peg_1906.setValue((void*) (long) letter_peg_1911);
                } else {
                    goto out_peg_1910;
                }
                
            }
            goto success_peg_1907;
            out_peg_1910:
            goto not_peg_1905;
            success_peg_1907:
            ;
            goto out_peg_1900;
            not_peg_1905:
            result_peg_1898.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_1898.setValue(value);
            }
        
        
    }
    
    if (column_peg_1883.chunk9 == 0){
        column_peg_1883.chunk9 = new Chunk9();
    }
    column_peg_1883.chunk9->chunk_unary = result_peg_1898;
    stream.update(result_peg_1898.getPosition());
    
    
    return result_peg_1898;
    out_peg_1900:
    
    if (column_peg_1883.chunk9 == 0){
        column_peg_1883.chunk9 = new Chunk9();
    }
    column_peg_1883.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_1913 = stream.getColumn(position);
    if (column_peg_1913.chunk9 != 0 && column_peg_1913.chunk9->chunk_expr13_real.calculated()){
        return column_peg_1913.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_1959(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_1914(myposition);
    
    result_peg_1914 = rule_helper(stream, result_peg_1914.getPosition());
    if (result_peg_1914.error()){
        goto out_peg_1915;
    }
    
    if (column_peg_1913.chunk9 == 0){
        column_peg_1913.chunk9 = new Chunk9();
    }
    column_peg_1913.chunk9->chunk_expr13_real = result_peg_1914;
    stream.update(result_peg_1914.getPosition());
    
    
    return result_peg_1914;
    out_peg_1915:
    Result result_peg_1916(myposition);
    
    result_peg_1916 = rule_function(stream, result_peg_1916.getPosition());
    if (result_peg_1916.error()){
        goto out_peg_1917;
    }
    
    if (column_peg_1913.chunk9 == 0){
        column_peg_1913.chunk9 = new Chunk9();
    }
    column_peg_1913.chunk9->chunk_expr13_real = result_peg_1916;
    stream.update(result_peg_1916.getPosition());
    
    
    return result_peg_1916;
    out_peg_1917:
    Result result_peg_1918(myposition);
    
    result_peg_1918 = rule_value(stream, result_peg_1918.getPosition());
    if (result_peg_1918.error()){
        goto out_peg_1919;
    }
    
    if (column_peg_1913.chunk9 == 0){
        column_peg_1913.chunk9 = new Chunk9();
    }
    column_peg_1913.chunk9->chunk_expr13_real = result_peg_1918;
    stream.update(result_peg_1918.getPosition());
    
    
    return result_peg_1918;
    out_peg_1919:
    Result result_peg_1920(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1920.getPosition()))){
                    result_peg_1920.nextPosition();
                } else {
                    goto out_peg_1922;
                }
            }
            result_peg_1920.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1920.reset();
                do{
                    Result result_peg_1934(result_peg_1920.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1934.getPosition()))){
                                result_peg_1934.nextPosition();
                            } else {
                                goto out_peg_1937;
                            }
                        }
                        result_peg_1934.setValue((void*) " ");
                            
                    }
                    goto success_peg_1935;
                    out_peg_1937:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1934.getPosition()))){
                                result_peg_1934.nextPosition();
                            } else {
                                goto out_peg_1939;
                            }
                        }
                        result_peg_1934.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1935;
                    out_peg_1939:
                    goto loop_peg_1933;
                    success_peg_1935:
                    ;
                    result_peg_1920.addResult(result_peg_1934);
                } while (true);
                loop_peg_1933:
                ;
                        
            }
            goto success_peg_1924;
            goto out_peg_1922;
            success_peg_1924:
            ;
        
        
        
        result_peg_1920 = rule_expr(stream, result_peg_1920.getPosition());
            if (result_peg_1920.error()){
                goto out_peg_1922;
            }
            e = result_peg_1920.getValues();
        
        
        
        {
                
                result_peg_1920.reset();
                do{
                    Result result_peg_1952(result_peg_1920.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1952.getPosition()))){
                                result_peg_1952.nextPosition();
                            } else {
                                goto out_peg_1955;
                            }
                        }
                        result_peg_1952.setValue((void*) " ");
                            
                    }
                    goto success_peg_1953;
                    out_peg_1955:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1952.getPosition()))){
                                result_peg_1952.nextPosition();
                            } else {
                                goto out_peg_1957;
                            }
                        }
                        result_peg_1952.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1953;
                    out_peg_1957:
                    goto loop_peg_1951;
                    success_peg_1953:
                    ;
                    result_peg_1920.addResult(result_peg_1952);
                } while (true);
                loop_peg_1951:
                ;
                        
            }
            goto success_peg_1942;
            goto out_peg_1922;
            success_peg_1942:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1920.getPosition()))){
                    result_peg_1920.nextPosition();
                } else {
                    goto out_peg_1922;
                }
            }
            result_peg_1920.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_1920.setValue(value);
            }
        
        
    }
    
    if (column_peg_1913.chunk9 == 0){
        column_peg_1913.chunk9 = new Chunk9();
    }
    column_peg_1913.chunk9->chunk_expr13_real = result_peg_1920;
    stream.update(result_peg_1920.getPosition());
    
    
    return result_peg_1920;
    out_peg_1922:
    
    if (column_peg_1913.chunk9 == 0){
        column_peg_1913.chunk9 = new Chunk9();
    }
    column_peg_1913.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_1960 = stream.getColumn(position);
    if (column_peg_1960.chunk9 != 0 && column_peg_1960.chunk9->chunk_function.calculated()){
        return column_peg_1960.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2176(stream, "function");
    int myposition = position;
    
    Value name;
    Value args;
    Value arg1;
    Result result_peg_1961(myposition);
    
    {
    
        result_peg_1961 = rule_function_name(stream, result_peg_1961.getPosition());
            if (result_peg_1961.error()){
                goto out_peg_1963;
            }
            name = result_peg_1961.getValues();
        
        
        
        {
                
                result_peg_1961.reset();
                do{
                    Result result_peg_1975(result_peg_1961.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1975.getPosition()))){
                                result_peg_1975.nextPosition();
                            } else {
                                goto out_peg_1978;
                            }
                        }
                        result_peg_1975.setValue((void*) " ");
                            
                    }
                    goto success_peg_1976;
                    out_peg_1978:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1975.getPosition()))){
                                result_peg_1975.nextPosition();
                            } else {
                                goto out_peg_1980;
                            }
                        }
                        result_peg_1975.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1976;
                    out_peg_1980:
                    goto loop_peg_1974;
                    success_peg_1976:
                    ;
                    result_peg_1961.addResult(result_peg_1975);
                } while (true);
                loop_peg_1974:
                ;
                        
            }
            goto success_peg_1965;
            goto out_peg_1963;
            success_peg_1965:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1961.getPosition()))){
                    result_peg_1961.nextPosition();
                } else {
                    goto out_peg_1963;
                }
            }
            result_peg_1961.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1961.reset();
                do{
                    Result result_peg_1993(result_peg_1961.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1993.getPosition()))){
                                result_peg_1993.nextPosition();
                            } else {
                                goto out_peg_1996;
                            }
                        }
                        result_peg_1993.setValue((void*) " ");
                            
                    }
                    goto success_peg_1994;
                    out_peg_1996:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1993.getPosition()))){
                                result_peg_1993.nextPosition();
                            } else {
                                goto out_peg_1998;
                            }
                        }
                        result_peg_1993.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1994;
                    out_peg_1998:
                    goto loop_peg_1992;
                    success_peg_1994:
                    ;
                    result_peg_1961.addResult(result_peg_1993);
                } while (true);
                loop_peg_1992:
                ;
                        
            }
            goto success_peg_1983;
            goto out_peg_1963;
            success_peg_1983:
            ;
        
        
        
        result_peg_1961 = rule_args(stream, result_peg_1961.getPosition());
            if (result_peg_1961.error()){
                goto out_peg_1963;
            }
            args = result_peg_1961.getValues();
        
        
        
        {
                
                result_peg_1961.reset();
                do{
                    Result result_peg_2011(result_peg_1961.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2011.getPosition()))){
                                result_peg_2011.nextPosition();
                            } else {
                                goto out_peg_2014;
                            }
                        }
                        result_peg_2011.setValue((void*) " ");
                            
                    }
                    goto success_peg_2012;
                    out_peg_2014:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2011.getPosition()))){
                                result_peg_2011.nextPosition();
                            } else {
                                goto out_peg_2016;
                            }
                        }
                        result_peg_2011.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2012;
                    out_peg_2016:
                    goto loop_peg_2010;
                    success_peg_2012:
                    ;
                    result_peg_1961.addResult(result_peg_2011);
                } while (true);
                loop_peg_2010:
                ;
                        
            }
            goto success_peg_2001;
            goto out_peg_1963;
            success_peg_2001:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1961.getPosition()))){
                    result_peg_1961.nextPosition();
                } else {
                    goto out_peg_1963;
                }
            }
            result_peg_1961.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, args);
                result_peg_1961.setValue(value);
            }
        
        
    }
    
    if (column_peg_1960.chunk9 == 0){
        column_peg_1960.chunk9 = new Chunk9();
    }
    column_peg_1960.chunk9->chunk_function = result_peg_1961;
    stream.update(result_peg_1961.getPosition());
    
    
    return result_peg_1961;
    out_peg_1963:
    Result result_peg_2018(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("teammode"[i], stream.get(result_peg_2018.getPosition()))){
                    result_peg_2018.nextPosition();
                } else {
                    goto out_peg_2020;
                }
            }
            result_peg_2018.setValue((void*) "teammode");
            name = result_peg_2018.getValues();
        
        
        
        {
                
                result_peg_2018.reset();
                do{
                    Result result_peg_2032(result_peg_2018.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2032.getPosition()))){
                                result_peg_2032.nextPosition();
                            } else {
                                goto out_peg_2035;
                            }
                        }
                        result_peg_2032.setValue((void*) " ");
                            
                    }
                    goto success_peg_2033;
                    out_peg_2035:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2032.getPosition()))){
                                result_peg_2032.nextPosition();
                            } else {
                                goto out_peg_2037;
                            }
                        }
                        result_peg_2032.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2033;
                    out_peg_2037:
                    goto loop_peg_2031;
                    success_peg_2033:
                    ;
                    result_peg_2018.addResult(result_peg_2032);
                } while (true);
                loop_peg_2031:
                ;
                        
            }
            goto success_peg_2022;
            goto out_peg_2020;
            success_peg_2022:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2018.getPosition()))){
                    result_peg_2018.nextPosition();
                } else {
                    goto out_peg_2020;
                }
            }
            result_peg_2018.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2018.reset();
                do{
                    Result result_peg_2050(result_peg_2018.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2050.getPosition()))){
                                result_peg_2050.nextPosition();
                            } else {
                                goto out_peg_2053;
                            }
                        }
                        result_peg_2050.setValue((void*) " ");
                            
                    }
                    goto success_peg_2051;
                    out_peg_2053:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2050.getPosition()))){
                                result_peg_2050.nextPosition();
                            } else {
                                goto out_peg_2055;
                            }
                        }
                        result_peg_2050.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2051;
                    out_peg_2055:
                    goto loop_peg_2049;
                    success_peg_2051:
                    ;
                    result_peg_2018.addResult(result_peg_2050);
                } while (true);
                loop_peg_2049:
                ;
                        
            }
            goto success_peg_2040;
            goto out_peg_2020;
            success_peg_2040:
            ;
        
        
        
        result_peg_2018 = rule_identifier(stream, result_peg_2018.getPosition());
            if (result_peg_2018.error()){
                goto out_peg_2020;
            }
            arg1 = result_peg_2018.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction1(name, arg1);
                result_peg_2018.setValue(value);
            }
        
        
    }
    
    if (column_peg_1960.chunk9 == 0){
        column_peg_1960.chunk9 = new Chunk9();
    }
    column_peg_1960.chunk9->chunk_function = result_peg_2018;
    stream.update(result_peg_2018.getPosition());
    
    
    return result_peg_2018;
    out_peg_2020:
    Result result_peg_2057(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("teammode"[i], stream.get(result_peg_2057.getPosition()))){
                    result_peg_2057.nextPosition();
                } else {
                    goto out_peg_2059;
                }
            }
            result_peg_2057.setValue((void*) "teammode");
            name = result_peg_2057.getValues();
        
        
        
        {
                
                result_peg_2057.reset();
                do{
                    Result result_peg_2071(result_peg_2057.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2071.getPosition()))){
                                result_peg_2071.nextPosition();
                            } else {
                                goto out_peg_2074;
                            }
                        }
                        result_peg_2071.setValue((void*) " ");
                            
                    }
                    goto success_peg_2072;
                    out_peg_2074:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2071.getPosition()))){
                                result_peg_2071.nextPosition();
                            } else {
                                goto out_peg_2076;
                            }
                        }
                        result_peg_2071.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2072;
                    out_peg_2076:
                    goto loop_peg_2070;
                    success_peg_2072:
                    ;
                    result_peg_2057.addResult(result_peg_2071);
                } while (true);
                loop_peg_2070:
                ;
                        
            }
            goto success_peg_2061;
            goto out_peg_2059;
            success_peg_2061:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_2057.getPosition()))){
                    result_peg_2057.nextPosition();
                } else {
                    goto out_peg_2059;
                }
            }
            result_peg_2057.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_2057.reset();
                do{
                    Result result_peg_2089(result_peg_2057.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2089.getPosition()))){
                                result_peg_2089.nextPosition();
                            } else {
                                goto out_peg_2092;
                            }
                        }
                        result_peg_2089.setValue((void*) " ");
                            
                    }
                    goto success_peg_2090;
                    out_peg_2092:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2089.getPosition()))){
                                result_peg_2089.nextPosition();
                            } else {
                                goto out_peg_2094;
                            }
                        }
                        result_peg_2089.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2090;
                    out_peg_2094:
                    goto loop_peg_2088;
                    success_peg_2090:
                    ;
                    result_peg_2057.addResult(result_peg_2089);
                } while (true);
                loop_peg_2088:
                ;
                        
            }
            goto success_peg_2079;
            goto out_peg_2059;
            success_peg_2079:
            ;
        
        
        
        result_peg_2057 = rule_identifier(stream, result_peg_2057.getPosition());
            if (result_peg_2057.error()){
                goto out_peg_2059;
            }
            arg1 = result_peg_2057.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction1("teammode!=", arg1);
                result_peg_2057.setValue(value);
            }
        
        
    }
    
    if (column_peg_1960.chunk9 == 0){
        column_peg_1960.chunk9 = new Chunk9();
    }
    column_peg_1960.chunk9->chunk_function = result_peg_2057;
    stream.update(result_peg_2057.getPosition());
    
    
    return result_peg_2057;
    out_peg_2059:
    Result result_peg_2096(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2096.getPosition()))){
                    result_peg_2096.nextPosition();
                } else {
                    goto out_peg_2098;
                }
            }
            result_peg_2096.setValue((void*) "animelem");
            name = result_peg_2096.getValues();
        
        
        
        {
                
                result_peg_2096.reset();
                do{
                    Result result_peg_2110(result_peg_2096.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2110.getPosition()))){
                                result_peg_2110.nextPosition();
                            } else {
                                goto out_peg_2113;
                            }
                        }
                        result_peg_2110.setValue((void*) " ");
                            
                    }
                    goto success_peg_2111;
                    out_peg_2113:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2110.getPosition()))){
                                result_peg_2110.nextPosition();
                            } else {
                                goto out_peg_2115;
                            }
                        }
                        result_peg_2110.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2111;
                    out_peg_2115:
                    goto loop_peg_2109;
                    success_peg_2111:
                    ;
                    result_peg_2096.addResult(result_peg_2110);
                } while (true);
                loop_peg_2109:
                ;
                        
            }
            goto success_peg_2100;
            goto out_peg_2098;
            success_peg_2100:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2096.getPosition()))){
                    result_peg_2096.nextPosition();
                } else {
                    goto out_peg_2098;
                }
            }
            result_peg_2096.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2096.reset();
                do{
                    Result result_peg_2128(result_peg_2096.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2128.getPosition()))){
                                result_peg_2128.nextPosition();
                            } else {
                                goto out_peg_2131;
                            }
                        }
                        result_peg_2128.setValue((void*) " ");
                            
                    }
                    goto success_peg_2129;
                    out_peg_2131:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2128.getPosition()))){
                                result_peg_2128.nextPosition();
                            } else {
                                goto out_peg_2133;
                            }
                        }
                        result_peg_2128.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2129;
                    out_peg_2133:
                    goto loop_peg_2127;
                    success_peg_2129:
                    ;
                    result_peg_2096.addResult(result_peg_2128);
                } while (true);
                loop_peg_2127:
                ;
                        
            }
            goto success_peg_2118;
            goto out_peg_2098;
            success_peg_2118:
            ;
        
        
        
        result_peg_2096 = rule_integer(stream, result_peg_2096.getPosition());
            if (result_peg_2096.error()){
                goto out_peg_2098;
            }
            arg1 = result_peg_2096.getValues();
        
        
        
        result_peg_2096 = rule_function_rest(stream, result_peg_2096.getPosition());
            if (result_peg_2096.error()){
                goto out_peg_2098;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1(name, arg1);
                result_peg_2096.setValue(value);
            }
        
        
    }
    
    if (column_peg_1960.chunk9 == 0){
        column_peg_1960.chunk9 = new Chunk9();
    }
    column_peg_1960.chunk9->chunk_function = result_peg_2096;
    stream.update(result_peg_2096.getPosition());
    
    
    return result_peg_2096;
    out_peg_2098:
    Result result_peg_2136(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2136.getPosition()))){
                    result_peg_2136.nextPosition();
                } else {
                    goto out_peg_2138;
                }
            }
            result_peg_2136.setValue((void*) "animelem");
            name = result_peg_2136.getValues();
        
        
        
        {
                
                result_peg_2136.reset();
                do{
                    Result result_peg_2150(result_peg_2136.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2150.getPosition()))){
                                result_peg_2150.nextPosition();
                            } else {
                                goto out_peg_2153;
                            }
                        }
                        result_peg_2150.setValue((void*) " ");
                            
                    }
                    goto success_peg_2151;
                    out_peg_2153:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2150.getPosition()))){
                                result_peg_2150.nextPosition();
                            } else {
                                goto out_peg_2155;
                            }
                        }
                        result_peg_2150.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2151;
                    out_peg_2155:
                    goto loop_peg_2149;
                    success_peg_2151:
                    ;
                    result_peg_2136.addResult(result_peg_2150);
                } while (true);
                loop_peg_2149:
                ;
                        
            }
            goto success_peg_2140;
            goto out_peg_2138;
            success_peg_2140:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_2136.getPosition()))){
                    result_peg_2136.nextPosition();
                } else {
                    goto out_peg_2138;
                }
            }
            result_peg_2136.setValue((void*) "<=");
        
        
        
        {
                
                result_peg_2136.reset();
                do{
                    Result result_peg_2168(result_peg_2136.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2168.getPosition()))){
                                result_peg_2168.nextPosition();
                            } else {
                                goto out_peg_2171;
                            }
                        }
                        result_peg_2168.setValue((void*) " ");
                            
                    }
                    goto success_peg_2169;
                    out_peg_2171:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2168.getPosition()))){
                                result_peg_2168.nextPosition();
                            } else {
                                goto out_peg_2173;
                            }
                        }
                        result_peg_2168.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2169;
                    out_peg_2173:
                    goto loop_peg_2167;
                    success_peg_2169:
                    ;
                    result_peg_2136.addResult(result_peg_2168);
                } while (true);
                loop_peg_2167:
                ;
                        
            }
            goto success_peg_2158;
            goto out_peg_2138;
            success_peg_2158:
            ;
        
        
        
        result_peg_2136 = rule_integer(stream, result_peg_2136.getPosition());
            if (result_peg_2136.error()){
                goto out_peg_2138;
            }
            arg1 = result_peg_2136.getValues();
        
        
        
        result_peg_2136 = rule_function_rest(stream, result_peg_2136.getPosition());
            if (result_peg_2136.error()){
                goto out_peg_2138;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1("animelem<=", arg1);
                result_peg_2136.setValue(value);
            }
        
        
    }
    
    if (column_peg_1960.chunk9 == 0){
        column_peg_1960.chunk9 = new Chunk9();
    }
    column_peg_1960.chunk9->chunk_function = result_peg_2136;
    stream.update(result_peg_2136.getPosition());
    
    
    return result_peg_2136;
    out_peg_2138:
    
    if (column_peg_1960.chunk9 == 0){
        column_peg_1960.chunk9 = new Chunk9();
    }
    column_peg_1960.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2177 = stream.getColumn(position);
    if (column_peg_2177.chunk9 != 0 && column_peg_2177.chunk9->chunk_args.calculated()){
        return column_peg_2177.chunk9->chunk_args;
    }
    
    RuleTrace trace_peg_2219(stream, "args");
    int myposition = position;
    
    Value expr1;
    Value expr_rest;
    Result result_peg_2178(myposition);
    
    {
    
        result_peg_2178 = rule_expr_c(stream, result_peg_2178.getPosition());
            if (result_peg_2178.error()){
                goto out_peg_2180;
            }
            expr1 = result_peg_2178.getValues();
        
        
        
        result_peg_2178.reset();
            do{
                Result result_peg_2183(result_peg_2178.getPosition());
                {
                
                    {
                            
                            result_peg_2183.reset();
                            do{
                                Result result_peg_2195(result_peg_2183.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2195.getPosition()))){
                                            result_peg_2195.nextPosition();
                                        } else {
                                            goto out_peg_2198;
                                        }
                                    }
                                    result_peg_2195.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2196;
                                out_peg_2198:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2195.getPosition()))){
                                            result_peg_2195.nextPosition();
                                        } else {
                                            goto out_peg_2200;
                                        }
                                    }
                                    result_peg_2195.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2196;
                                out_peg_2200:
                                goto loop_peg_2194;
                                success_peg_2196:
                                ;
                                result_peg_2183.addResult(result_peg_2195);
                            } while (true);
                            loop_peg_2194:
                            ;
                                    
                        }
                        goto success_peg_2185;
                        goto loop_peg_2182;
                        success_peg_2185:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2183.getPosition()))){
                                result_peg_2183.nextPosition();
                            } else {
                                goto loop_peg_2182;
                            }
                        }
                        result_peg_2183.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2183.reset();
                            do{
                                Result result_peg_2213(result_peg_2183.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2213.getPosition()))){
                                            result_peg_2213.nextPosition();
                                        } else {
                                            goto out_peg_2216;
                                        }
                                    }
                                    result_peg_2213.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2214;
                                out_peg_2216:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2213.getPosition()))){
                                            result_peg_2213.nextPosition();
                                        } else {
                                            goto out_peg_2218;
                                        }
                                    }
                                    result_peg_2213.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2214;
                                out_peg_2218:
                                goto loop_peg_2212;
                                success_peg_2214:
                                ;
                                result_peg_2183.addResult(result_peg_2213);
                            } while (true);
                            loop_peg_2212:
                            ;
                                    
                        }
                        goto success_peg_2203;
                        goto loop_peg_2182;
                        success_peg_2203:
                        ;
                    
                    
                    
                    result_peg_2183 = rule_expr_c(stream, result_peg_2183.getPosition());
                        if (result_peg_2183.error()){
                            goto loop_peg_2182;
                        }
                    
                    
                }
                result_peg_2178.addResult(result_peg_2183);
            } while (true);
            loop_peg_2182:
            ;
            expr_rest = result_peg_2178.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(expr1, expr_rest);
                result_peg_2178.setValue(value);
            }
        
        
    }
    
    if (column_peg_2177.chunk9 == 0){
        column_peg_2177.chunk9 = new Chunk9();
    }
    column_peg_2177.chunk9->chunk_args = result_peg_2178;
    stream.update(result_peg_2178.getPosition());
    
    
    return result_peg_2178;
    out_peg_2180:
    
    if (column_peg_2177.chunk9 == 0){
        column_peg_2177.chunk9 = new Chunk9();
    }
    column_peg_2177.chunk9->chunk_args = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2220 = stream.getColumn(position);
    if (column_peg_2220.chunk10 != 0 && column_peg_2220.chunk10->chunk_function_name.calculated()){
        return column_peg_2220.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_2285(stream, "function_name");
    int myposition = position;
    
    
    Result result_peg_2221(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("abs"[i], stream.get(result_peg_2221.getPosition()))){
            result_peg_2221.nextPosition();
        } else {
            goto out_peg_2222;
        }
    }
    result_peg_2221.setValue((void*) "abs");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2221;
    stream.update(result_peg_2221.getPosition());
    
    
    return result_peg_2221;
    out_peg_2222:
    Result result_peg_2223(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("const"[i], stream.get(result_peg_2223.getPosition()))){
            result_peg_2223.nextPosition();
        } else {
            goto out_peg_2224;
        }
    }
    result_peg_2223.setValue((void*) "const");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2223;
    stream.update(result_peg_2223.getPosition());
    
    
    return result_peg_2223;
    out_peg_2224:
    Result result_peg_2225(myposition);
    
    for (int i = 0; i < 13; i++){
        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2225.getPosition()))){
            result_peg_2225.nextPosition();
        } else {
            goto out_peg_2226;
        }
    }
    result_peg_2225.setValue((void*) "selfanimexist");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2225;
    stream.update(result_peg_2225.getPosition());
    
    
    return result_peg_2225;
    out_peg_2226:
    Result result_peg_2227(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("ifelse"[i], stream.get(result_peg_2227.getPosition()))){
            result_peg_2227.nextPosition();
        } else {
            goto out_peg_2228;
        }
    }
    result_peg_2227.setValue((void*) "ifelse");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2227;
    stream.update(result_peg_2227.getPosition());
    
    
    return result_peg_2227;
    out_peg_2228:
    Result result_peg_2229(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("gethitvar"[i], stream.get(result_peg_2229.getPosition()))){
            result_peg_2229.nextPosition();
        } else {
            goto out_peg_2230;
        }
    }
    result_peg_2229.setValue((void*) "gethitvar");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2229;
    stream.update(result_peg_2229.getPosition());
    
    
    return result_peg_2229;
    out_peg_2230:
    Result result_peg_2231(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("floor"[i], stream.get(result_peg_2231.getPosition()))){
            result_peg_2231.nextPosition();
        } else {
            goto out_peg_2232;
        }
    }
    result_peg_2231.setValue((void*) "floor");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2231;
    stream.update(result_peg_2231.getPosition());
    
    
    return result_peg_2231;
    out_peg_2232:
    Result result_peg_2233(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("ceil"[i], stream.get(result_peg_2233.getPosition()))){
            result_peg_2233.nextPosition();
        } else {
            goto out_peg_2234;
        }
    }
    result_peg_2233.setValue((void*) "ceil");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2233;
    stream.update(result_peg_2233.getPosition());
    
    
    return result_peg_2233;
    out_peg_2234:
    Result result_peg_2235(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("acos"[i], stream.get(result_peg_2235.getPosition()))){
            result_peg_2235.nextPosition();
        } else {
            goto out_peg_2236;
        }
    }
    result_peg_2235.setValue((void*) "acos");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2235;
    stream.update(result_peg_2235.getPosition());
    
    
    return result_peg_2235;
    out_peg_2236:
    Result result_peg_2237(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("asin"[i], stream.get(result_peg_2237.getPosition()))){
            result_peg_2237.nextPosition();
        } else {
            goto out_peg_2238;
        }
    }
    result_peg_2237.setValue((void*) "asin");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2237;
    stream.update(result_peg_2237.getPosition());
    
    
    return result_peg_2237;
    out_peg_2238:
    Result result_peg_2239(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("atan"[i], stream.get(result_peg_2239.getPosition()))){
            result_peg_2239.nextPosition();
        } else {
            goto out_peg_2240;
        }
    }
    result_peg_2239.setValue((void*) "atan");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2239;
    stream.update(result_peg_2239.getPosition());
    
    
    return result_peg_2239;
    out_peg_2240:
    Result result_peg_2241(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("tan"[i], stream.get(result_peg_2241.getPosition()))){
            result_peg_2241.nextPosition();
        } else {
            goto out_peg_2242;
        }
    }
    result_peg_2241.setValue((void*) "tan");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2241;
    stream.update(result_peg_2241.getPosition());
    
    
    return result_peg_2241;
    out_peg_2242:
    Result result_peg_2243(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("cos"[i], stream.get(result_peg_2243.getPosition()))){
            result_peg_2243.nextPosition();
        } else {
            goto out_peg_2244;
        }
    }
    result_peg_2243.setValue((void*) "cos");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2243;
    stream.update(result_peg_2243.getPosition());
    
    
    return result_peg_2243;
    out_peg_2244:
    Result result_peg_2245(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("sin"[i], stream.get(result_peg_2245.getPosition()))){
            result_peg_2245.nextPosition();
        } else {
            goto out_peg_2246;
        }
    }
    result_peg_2245.setValue((void*) "sin");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2245;
    stream.update(result_peg_2245.getPosition());
    
    
    return result_peg_2245;
    out_peg_2246:
    Result result_peg_2247(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("log"[i], stream.get(result_peg_2247.getPosition()))){
            result_peg_2247.nextPosition();
        } else {
            goto out_peg_2248;
        }
    }
    result_peg_2247.setValue((void*) "log");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2247;
    stream.update(result_peg_2247.getPosition());
    
    
    return result_peg_2247;
    out_peg_2248:
    Result result_peg_2249(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("ln"[i], stream.get(result_peg_2249.getPosition()))){
            result_peg_2249.nextPosition();
        } else {
            goto out_peg_2250;
        }
    }
    result_peg_2249.setValue((void*) "ln");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2249;
    stream.update(result_peg_2249.getPosition());
    
    
    return result_peg_2249;
    out_peg_2250:
    Result result_peg_2251(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("sysfvar"[i], stream.get(result_peg_2251.getPosition()))){
            result_peg_2251.nextPosition();
        } else {
            goto out_peg_2252;
        }
    }
    result_peg_2251.setValue((void*) "sysfvar");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2251;
    stream.update(result_peg_2251.getPosition());
    
    
    return result_peg_2251;
    out_peg_2252:
    Result result_peg_2253(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("sysvar"[i], stream.get(result_peg_2253.getPosition()))){
            result_peg_2253.nextPosition();
        } else {
            goto out_peg_2254;
        }
    }
    result_peg_2253.setValue((void*) "sysvar");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2253;
    stream.update(result_peg_2253.getPosition());
    
    
    return result_peg_2253;
    out_peg_2254:
    Result result_peg_2255(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("var"[i], stream.get(result_peg_2255.getPosition()))){
            result_peg_2255.nextPosition();
        } else {
            goto out_peg_2256;
        }
    }
    result_peg_2255.setValue((void*) "var");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2255;
    stream.update(result_peg_2255.getPosition());
    
    
    return result_peg_2255;
    out_peg_2256:
    Result result_peg_2257(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numexplod"[i], stream.get(result_peg_2257.getPosition()))){
            result_peg_2257.nextPosition();
        } else {
            goto out_peg_2258;
        }
    }
    result_peg_2257.setValue((void*) "numexplod");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2257;
    stream.update(result_peg_2257.getPosition());
    
    
    return result_peg_2257;
    out_peg_2258:
    Result result_peg_2259(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2259.getPosition()))){
            result_peg_2259.nextPosition();
        } else {
            goto out_peg_2260;
        }
    }
    result_peg_2259.setValue((void*) "numhelper");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2259;
    stream.update(result_peg_2259.getPosition());
    
    
    return result_peg_2259;
    out_peg_2260:
    Result result_peg_2261(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numprojid"[i], stream.get(result_peg_2261.getPosition()))){
            result_peg_2261.nextPosition();
        } else {
            goto out_peg_2262;
        }
    }
    result_peg_2261.setValue((void*) "numprojid");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2261;
    stream.update(result_peg_2261.getPosition());
    
    
    return result_peg_2261;
    out_peg_2262:
    Result result_peg_2263(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("fvar"[i], stream.get(result_peg_2263.getPosition()))){
            result_peg_2263.nextPosition();
        } else {
            goto out_peg_2264;
        }
    }
    result_peg_2263.setValue((void*) "fvar");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2263;
    stream.update(result_peg_2263.getPosition());
    
    
    return result_peg_2263;
    out_peg_2264:
    Result result_peg_2265(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("ishelper"[i], stream.get(result_peg_2265.getPosition()))){
            result_peg_2265.nextPosition();
        } else {
            goto out_peg_2266;
        }
    }
    result_peg_2265.setValue((void*) "ishelper");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2265;
    stream.update(result_peg_2265.getPosition());
    
    
    return result_peg_2265;
    out_peg_2266:
    Result result_peg_2267(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numtarget"[i], stream.get(result_peg_2267.getPosition()))){
            result_peg_2267.nextPosition();
        } else {
            goto out_peg_2268;
        }
    }
    result_peg_2267.setValue((void*) "numtarget");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2267;
    stream.update(result_peg_2267.getPosition());
    
    
    return result_peg_2267;
    out_peg_2268:
    Result result_peg_2269(myposition);
    
    for (int i = 0; i < 12; i++){
        if (compareCharCase("animelemtime"[i], stream.get(result_peg_2269.getPosition()))){
            result_peg_2269.nextPosition();
        } else {
            goto out_peg_2270;
        }
    }
    result_peg_2269.setValue((void*) "animelemtime");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2269;
    stream.update(result_peg_2269.getPosition());
    
    
    return result_peg_2269;
    out_peg_2270:
    Result result_peg_2271(myposition);
    
    for (int i = 0; i < 10; i++){
        if (compareCharCase("animelemno"[i], stream.get(result_peg_2271.getPosition()))){
            result_peg_2271.nextPosition();
        } else {
            goto out_peg_2272;
        }
    }
    result_peg_2271.setValue((void*) "animelemno");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2271;
    stream.update(result_peg_2271.getPosition());
    
    
    return result_peg_2271;
    out_peg_2272:
    Result result_peg_2273(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("animexist"[i], stream.get(result_peg_2273.getPosition()))){
            result_peg_2273.nextPosition();
        } else {
            goto out_peg_2274;
        }
    }
    result_peg_2273.setValue((void*) "animexist");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2273;
    stream.update(result_peg_2273.getPosition());
    
    
    return result_peg_2273;
    out_peg_2274:
    Result result_peg_2275(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projguarded"[i], stream.get(result_peg_2275.getPosition()))){
            result_peg_2275.nextPosition();
        } else {
            goto out_peg_2276;
        }
    }
    result_peg_2275.setValue((void*) "projguarded");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2275;
    stream.update(result_peg_2275.getPosition());
    
    
    return result_peg_2275;
    out_peg_2276:
    Result result_peg_2277(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projhittime"[i], stream.get(result_peg_2277.getPosition()))){
            result_peg_2277.nextPosition();
        } else {
            goto out_peg_2278;
        }
    }
    result_peg_2277.setValue((void*) "projhittime");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2277;
    stream.update(result_peg_2277.getPosition());
    
    
    return result_peg_2277;
    out_peg_2278:
    Result result_peg_2279(myposition);
    
    for (int i = 0; i < 15; i++){
        if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2279.getPosition()))){
            result_peg_2279.nextPosition();
        } else {
            goto out_peg_2280;
        }
    }
    result_peg_2279.setValue((void*) "projcontacttime");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2279;
    stream.update(result_peg_2279.getPosition());
    
    
    return result_peg_2279;
    out_peg_2280:
    Result result_peg_2281(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projcontact"[i], stream.get(result_peg_2281.getPosition()))){
            result_peg_2281.nextPosition();
        } else {
            goto out_peg_2282;
        }
    }
    result_peg_2281.setValue((void*) "projcontact");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2281;
    stream.update(result_peg_2281.getPosition());
    
    
    return result_peg_2281;
    out_peg_2282:
    Result result_peg_2283(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2283.getPosition()))){
            result_peg_2283.nextPosition();
        } else {
            goto out_peg_2284;
        }
    }
    result_peg_2283.setValue((void*) "numhelper");
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = result_peg_2283;
    stream.update(result_peg_2283.getPosition());
    
    
    return result_peg_2283;
    out_peg_2284:
    
    if (column_peg_2220.chunk10 == 0){
        column_peg_2220.chunk10 = new Chunk10();
    }
    column_peg_2220.chunk10->chunk_function_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2286 = stream.getColumn(position);
    if (column_peg_2286.chunk10 != 0 && column_peg_2286.chunk10->chunk_function_rest.calculated()){
        return column_peg_2286.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2344(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_2287(myposition);
    
    {
    
        {
                
                result_peg_2287.reset();
                do{
                    Result result_peg_2299(result_peg_2287.getPosition());
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
                    result_peg_2287.addResult(result_peg_2299);
                } while (true);
                loop_peg_2298:
                ;
                        
            }
            goto success_peg_2289;
            goto out_peg_2305;
            success_peg_2289:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2287.getPosition()))){
                    result_peg_2287.nextPosition();
                } else {
                    goto out_peg_2305;
                }
            }
            result_peg_2287.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2287.reset();
                do{
                    Result result_peg_2318(result_peg_2287.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2318.getPosition()))){
                                result_peg_2318.nextPosition();
                            } else {
                                goto out_peg_2321;
                            }
                        }
                        result_peg_2318.setValue((void*) " ");
                            
                    }
                    goto success_peg_2319;
                    out_peg_2321:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2318.getPosition()))){
                                result_peg_2318.nextPosition();
                            } else {
                                goto out_peg_2323;
                            }
                        }
                        result_peg_2318.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2319;
                    out_peg_2323:
                    goto loop_peg_2317;
                    success_peg_2319:
                    ;
                    result_peg_2287.addResult(result_peg_2318);
                } while (true);
                loop_peg_2317:
                ;
                        
            }
            goto success_peg_2308;
            goto out_peg_2305;
            success_peg_2308:
            ;
        
        
        
        int save_peg_2325 = result_peg_2287.getPosition();
            
            result_peg_2287 = rule_all_compare(stream, result_peg_2287.getPosition());
            if (result_peg_2287.error()){
                
                result_peg_2287 = Result(save_peg_2325);
                result_peg_2287.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2287.reset();
                do{
                    Result result_peg_2337(result_peg_2287.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2337.getPosition()))){
                                result_peg_2337.nextPosition();
                            } else {
                                goto out_peg_2340;
                            }
                        }
                        result_peg_2337.setValue((void*) " ");
                            
                    }
                    goto success_peg_2338;
                    out_peg_2340:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2337.getPosition()))){
                                result_peg_2337.nextPosition();
                            } else {
                                goto out_peg_2342;
                            }
                        }
                        result_peg_2337.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2338;
                    out_peg_2342:
                    goto loop_peg_2336;
                    success_peg_2338:
                    ;
                    result_peg_2287.addResult(result_peg_2337);
                } while (true);
                loop_peg_2336:
                ;
                        
            }
            goto success_peg_2327;
            goto out_peg_2305;
            success_peg_2327:
            ;
        
        
        
        result_peg_2287 = rule_value(stream, result_peg_2287.getPosition());
            if (result_peg_2287.error()){
                goto out_peg_2305;
            }
        
        
    }
    
    if (column_peg_2286.chunk10 == 0){
        column_peg_2286.chunk10 = new Chunk10();
    }
    column_peg_2286.chunk10->chunk_function_rest = result_peg_2287;
    stream.update(result_peg_2287.getPosition());
    
    
    return result_peg_2287;
    out_peg_2305:
    Result result_peg_2343(myposition);
    
    
    
    if (column_peg_2286.chunk10 == 0){
        column_peg_2286.chunk10 = new Chunk10();
    }
    column_peg_2286.chunk10->chunk_function_rest = result_peg_2343;
    stream.update(result_peg_2343.getPosition());
    
    
    return result_peg_2343;
    
    if (column_peg_2286.chunk10 == 0){
        column_peg_2286.chunk10 = new Chunk10();
    }
    column_peg_2286.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2345 = stream.getColumn(position);
    if (column_peg_2345.chunk10 != 0 && column_peg_2345.chunk10->chunk_keys.calculated()){
        return column_peg_2345.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_2348(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_2346(myposition);
    
    result_peg_2346 = rule_key_value_list(stream, result_peg_2346.getPosition());
    if (result_peg_2346.error()){
        goto out_peg_2347;
    }
    
    if (column_peg_2345.chunk10 == 0){
        column_peg_2345.chunk10 = new Chunk10();
    }
    column_peg_2345.chunk10->chunk_keys = result_peg_2346;
    stream.update(result_peg_2346.getPosition());
    
    
    return result_peg_2346;
    out_peg_2347:
    
    if (column_peg_2345.chunk10 == 0){
        column_peg_2345.chunk10 = new Chunk10();
    }
    column_peg_2345.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2349 = stream.getColumn(position);
    if (column_peg_2349.chunk10 != 0 && column_peg_2349.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2349.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2391(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_2350(myposition);
    
    {
    
        result_peg_2350 = rule_key(stream, result_peg_2350.getPosition());
            if (result_peg_2350.error()){
                goto out_peg_2352;
            }
            first = result_peg_2350.getValues();
        
        
        
        result_peg_2350.reset();
            do{
                Result result_peg_2355(result_peg_2350.getPosition());
                {
                
                    {
                            
                            result_peg_2355.reset();
                            do{
                                Result result_peg_2367(result_peg_2355.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2367.getPosition()))){
                                            result_peg_2367.nextPosition();
                                        } else {
                                            goto out_peg_2370;
                                        }
                                    }
                                    result_peg_2367.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2368;
                                out_peg_2370:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2367.getPosition()))){
                                            result_peg_2367.nextPosition();
                                        } else {
                                            goto out_peg_2372;
                                        }
                                    }
                                    result_peg_2367.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2368;
                                out_peg_2372:
                                goto loop_peg_2366;
                                success_peg_2368:
                                ;
                                result_peg_2355.addResult(result_peg_2367);
                            } while (true);
                            loop_peg_2366:
                            ;
                                    
                        }
                        goto success_peg_2357;
                        goto loop_peg_2354;
                        success_peg_2357:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2355.getPosition()))){
                                result_peg_2355.nextPosition();
                            } else {
                                goto loop_peg_2354;
                            }
                        }
                        result_peg_2355.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2355.reset();
                            do{
                                Result result_peg_2385(result_peg_2355.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2385.getPosition()))){
                                            result_peg_2385.nextPosition();
                                        } else {
                                            goto out_peg_2388;
                                        }
                                    }
                                    result_peg_2385.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2386;
                                out_peg_2388:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2385.getPosition()))){
                                            result_peg_2385.nextPosition();
                                        } else {
                                            goto out_peg_2390;
                                        }
                                    }
                                    result_peg_2385.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2386;
                                out_peg_2390:
                                goto loop_peg_2384;
                                success_peg_2386:
                                ;
                                result_peg_2355.addResult(result_peg_2385);
                            } while (true);
                            loop_peg_2384:
                            ;
                                    
                        }
                        goto success_peg_2375;
                        goto loop_peg_2354;
                        success_peg_2375:
                        ;
                    
                    
                    
                    result_peg_2355 = rule_key(stream, result_peg_2355.getPosition());
                        if (result_peg_2355.error()){
                            goto loop_peg_2354;
                        }
                    
                    
                }
                result_peg_2350.addResult(result_peg_2355);
            } while (true);
            loop_peg_2354:
            ;
            rest = result_peg_2350.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_2350.setValue(value);
            }
        
        
    }
    
    if (column_peg_2349.chunk10 == 0){
        column_peg_2349.chunk10 = new Chunk10();
    }
    column_peg_2349.chunk10->chunk_key_value_list = result_peg_2350;
    stream.update(result_peg_2350.getPosition());
    
    
    return result_peg_2350;
    out_peg_2352:
    
    if (column_peg_2349.chunk10 == 0){
        column_peg_2349.chunk10 = new Chunk10();
    }
    column_peg_2349.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2392 = stream.getColumn(position);
    if (column_peg_2392.chunk10 != 0 && column_peg_2392.chunk10->chunk_key.calculated()){
        return column_peg_2392.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_2400(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_2393(myposition);
    
    {
    
        result_peg_2393 = rule_key_real(stream, result_peg_2393.getPosition());
            if (result_peg_2393.error()){
                goto out_peg_2395;
            }
        
        Result result_peg_2394 = result_peg_2393;
        
        result_peg_2393 = rule_key_rest(stream, result_peg_2393.getPosition(), result_peg_2394.getValues());
            if (result_peg_2393.error()){
                goto out_peg_2395;
            }
            ok = result_peg_2393.getValues();
        
        
        
        Result result_peg_2399(result_peg_2393);
            result_peg_2399 = rule_identifier(stream, result_peg_2399.getPosition());
            if (result_peg_2399.error()){
                goto not_peg_2398;
            }
            goto out_peg_2395;
            not_peg_2398:
            result_peg_2393.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_2393.setValue(value);
            }
        
        
    }
    
    if (column_peg_2392.chunk10 == 0){
        column_peg_2392.chunk10 = new Chunk10();
    }
    column_peg_2392.chunk10->chunk_key = result_peg_2393;
    stream.update(result_peg_2393.getPosition());
    
    
    return result_peg_2393;
    out_peg_2395:
    
    if (column_peg_2392.chunk10 == 0){
        column_peg_2392.chunk10 = new Chunk10();
    }
    column_peg_2392.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2427(stream, "key_rest");
    int myposition = position;
    tail_peg_2403:
    Value another;
    Value new_left;
    Result result_peg_2402(myposition);
    {
    
        {
                
                result_peg_2402.reset();
                do{
                    Result result_peg_2415(result_peg_2402.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2415.getPosition()))){
                                result_peg_2415.nextPosition();
                            } else {
                                goto out_peg_2418;
                            }
                        }
                        result_peg_2415.setValue((void*) " ");
                            
                    }
                    goto success_peg_2416;
                    out_peg_2418:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2415.getPosition()))){
                                result_peg_2415.nextPosition();
                            } else {
                                goto out_peg_2420;
                            }
                        }
                        result_peg_2415.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2416;
                    out_peg_2420:
                    goto loop_peg_2414;
                    success_peg_2416:
                    ;
                    result_peg_2402.addResult(result_peg_2415);
                } while (true);
                loop_peg_2414:
                ;
                        
            }
            goto success_peg_2405;
            goto out_peg_2421;
            success_peg_2405:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_2402.getPosition()))){
                    result_peg_2402.nextPosition();
                } else {
                    goto out_peg_2421;
                }
            }
            result_peg_2402.setValue((void*) "+");
        
        
        
        result_peg_2402 = rule_key_real(stream, result_peg_2402.getPosition());
            if (result_peg_2402.error()){
                goto out_peg_2421;
            }
            another = result_peg_2402.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_2402.setValue(value);
            }
            new_left = result_peg_2402.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_2402.getPosition();
    goto tail_peg_2403;
    out_peg_2421:
    Result result_peg_2425(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_2425.setValue(value);
            }
        
        
    }
    stream.update(result_peg_2425.getPosition());
    
    return result_peg_2425;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2428 = stream.getColumn(position);
    if (column_peg_2428.chunk11 != 0 && column_peg_2428.chunk11->chunk_key_real.calculated()){
        return column_peg_2428.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_2435(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_2429(myposition);
    
    {
    
        result_peg_2429.reset();
            do{
                Result result_peg_2432(result_peg_2429.getPosition());
                result_peg_2432 = rule_key_modifier(stream, result_peg_2432.getPosition());
                if (result_peg_2432.error()){
                    goto loop_peg_2431;
                }
                result_peg_2429.addResult(result_peg_2432);
            } while (true);
            loop_peg_2431:
            ;
            mods = result_peg_2429.getValues();
        
        
        
        result_peg_2429 = rule_key_name(stream, result_peg_2429.getPosition());
            if (result_peg_2429.error()){
                goto out_peg_2434;
            }
            name = result_peg_2429.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_2429.setValue(value);
            }
        
        
    }
    
    if (column_peg_2428.chunk11 == 0){
        column_peg_2428.chunk11 = new Chunk11();
    }
    column_peg_2428.chunk11->chunk_key_real = result_peg_2429;
    stream.update(result_peg_2429.getPosition());
    
    
    return result_peg_2429;
    out_peg_2434:
    
    if (column_peg_2428.chunk11 == 0){
        column_peg_2428.chunk11 = new Chunk11();
    }
    column_peg_2428.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2436 = stream.getColumn(position);
    if (column_peg_2436.chunk11 != 0 && column_peg_2436.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2436.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2457(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_2437(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_2437.getPosition()))){
                    result_peg_2437.nextPosition();
                } else {
                    goto out_peg_2439;
                }
            }
            result_peg_2437.setValue((void*) "~");
        
        
        
        result_peg_2437.reset();
            do{
                Result result_peg_2442(result_peg_2437.getPosition());
                {
                    
                    char letter_peg_2447 = stream.get(result_peg_2442.getPosition());
                    if (letter_peg_2447 != '\0' && strchr("0123456789", letter_peg_2447) != NULL){
                        result_peg_2442.nextPosition();
                        result_peg_2442.setValue((void*) (long) letter_peg_2447);
                    } else {
                        goto out_peg_2446;
                    }
                    
                }
                goto success_peg_2443;
                out_peg_2446:
                goto loop_peg_2441;
                success_peg_2443:
                ;
                result_peg_2437.addResult(result_peg_2442);
            } while (true);
            loop_peg_2441:
            ;
            num = result_peg_2437.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_2437.setValue(value);
            }
        
        
    }
    
    if (column_peg_2436.chunk11 == 0){
        column_peg_2436.chunk11 = new Chunk11();
    }
    column_peg_2436.chunk11->chunk_key_modifier = result_peg_2437;
    stream.update(result_peg_2437.getPosition());
    
    
    return result_peg_2437;
    out_peg_2439:
    Result result_peg_2448(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_2448.getPosition()))){
                    result_peg_2448.nextPosition();
                } else {
                    goto out_peg_2450;
                }
            }
            result_peg_2448.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_2448.setValue(value);
            }
        
        
    }
    
    if (column_peg_2436.chunk11 == 0){
        column_peg_2436.chunk11 = new Chunk11();
    }
    column_peg_2436.chunk11->chunk_key_modifier = result_peg_2448;
    stream.update(result_peg_2448.getPosition());
    
    
    return result_peg_2448;
    out_peg_2450:
    Result result_peg_2451(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_2451.getPosition()))){
                    result_peg_2451.nextPosition();
                } else {
                    goto out_peg_2453;
                }
            }
            result_peg_2451.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_2451.setValue(value);
            }
        
        
    }
    
    if (column_peg_2436.chunk11 == 0){
        column_peg_2436.chunk11 = new Chunk11();
    }
    column_peg_2436.chunk11->chunk_key_modifier = result_peg_2451;
    stream.update(result_peg_2451.getPosition());
    
    
    return result_peg_2451;
    out_peg_2453:
    Result result_peg_2454(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_2454.getPosition()))){
                    result_peg_2454.nextPosition();
                } else {
                    goto out_peg_2456;
                }
            }
            result_peg_2454.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_2454.setValue(value);
            }
        
        
    }
    
    if (column_peg_2436.chunk11 == 0){
        column_peg_2436.chunk11 = new Chunk11();
    }
    column_peg_2436.chunk11->chunk_key_modifier = result_peg_2454;
    stream.update(result_peg_2454.getPosition());
    
    
    return result_peg_2454;
    out_peg_2456:
    
    if (column_peg_2436.chunk11 == 0){
        column_peg_2436.chunk11 = new Chunk11();
    }
    column_peg_2436.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2458 = stream.getColumn(position);
    if (column_peg_2458.chunk11 != 0 && column_peg_2458.chunk11->chunk_key_name.calculated()){
        return column_peg_2458.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_2489(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_2459(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_2459.getPosition()))){
            result_peg_2459.nextPosition();
        } else {
            goto out_peg_2460;
        }
    }
    result_peg_2459.setValue((void*) "DB");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2459;
    stream.update(result_peg_2459.getPosition());
    
    
    return result_peg_2459;
    out_peg_2460:
    Result result_peg_2461(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_2461.getPosition()))){
            result_peg_2461.nextPosition();
        } else {
            goto out_peg_2462;
        }
    }
    result_peg_2461.setValue((void*) "B");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2461;
    stream.update(result_peg_2461.getPosition());
    
    
    return result_peg_2461;
    out_peg_2462:
    Result result_peg_2463(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_2463.getPosition()))){
            result_peg_2463.nextPosition();
        } else {
            goto out_peg_2464;
        }
    }
    result_peg_2463.setValue((void*) "DF");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2463;
    stream.update(result_peg_2463.getPosition());
    
    
    return result_peg_2463;
    out_peg_2464:
    Result result_peg_2465(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_2465.getPosition()))){
            result_peg_2465.nextPosition();
        } else {
            goto out_peg_2466;
        }
    }
    result_peg_2465.setValue((void*) "D");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2465;
    stream.update(result_peg_2465.getPosition());
    
    
    return result_peg_2465;
    out_peg_2466:
    Result result_peg_2467(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_2467.getPosition()))){
            result_peg_2467.nextPosition();
        } else {
            goto out_peg_2468;
        }
    }
    result_peg_2467.setValue((void*) "F");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2467;
    stream.update(result_peg_2467.getPosition());
    
    
    return result_peg_2467;
    out_peg_2468:
    Result result_peg_2469(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_2469.getPosition()))){
            result_peg_2469.nextPosition();
        } else {
            goto out_peg_2470;
        }
    }
    result_peg_2469.setValue((void*) "UF");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2469;
    stream.update(result_peg_2469.getPosition());
    
    
    return result_peg_2469;
    out_peg_2470:
    Result result_peg_2471(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_2471.getPosition()))){
            result_peg_2471.nextPosition();
        } else {
            goto out_peg_2472;
        }
    }
    result_peg_2471.setValue((void*) "UB");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2471;
    stream.update(result_peg_2471.getPosition());
    
    
    return result_peg_2471;
    out_peg_2472:
    Result result_peg_2473(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_2473.getPosition()))){
            result_peg_2473.nextPosition();
        } else {
            goto out_peg_2474;
        }
    }
    result_peg_2473.setValue((void*) "U");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2473;
    stream.update(result_peg_2473.getPosition());
    
    
    return result_peg_2473;
    out_peg_2474:
    Result result_peg_2475(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_2475.getPosition()))){
            result_peg_2475.nextPosition();
        } else {
            goto out_peg_2476;
        }
    }
    result_peg_2475.setValue((void*) "a");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2475;
    stream.update(result_peg_2475.getPosition());
    
    
    return result_peg_2475;
    out_peg_2476:
    Result result_peg_2477(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_2477.getPosition()))){
            result_peg_2477.nextPosition();
        } else {
            goto out_peg_2478;
        }
    }
    result_peg_2477.setValue((void*) "b");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2477;
    stream.update(result_peg_2477.getPosition());
    
    
    return result_peg_2477;
    out_peg_2478:
    Result result_peg_2479(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_2479.getPosition()))){
            result_peg_2479.nextPosition();
        } else {
            goto out_peg_2480;
        }
    }
    result_peg_2479.setValue((void*) "c");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2479;
    stream.update(result_peg_2479.getPosition());
    
    
    return result_peg_2479;
    out_peg_2480:
    Result result_peg_2481(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_2481.getPosition()))){
            result_peg_2481.nextPosition();
        } else {
            goto out_peg_2482;
        }
    }
    result_peg_2481.setValue((void*) "x");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2481;
    stream.update(result_peg_2481.getPosition());
    
    
    return result_peg_2481;
    out_peg_2482:
    Result result_peg_2483(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_2483.getPosition()))){
            result_peg_2483.nextPosition();
        } else {
            goto out_peg_2484;
        }
    }
    result_peg_2483.setValue((void*) "y");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2483;
    stream.update(result_peg_2483.getPosition());
    
    
    return result_peg_2483;
    out_peg_2484:
    Result result_peg_2485(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_2485.getPosition()))){
            result_peg_2485.nextPosition();
        } else {
            goto out_peg_2486;
        }
    }
    result_peg_2485.setValue((void*) "z");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2485;
    stream.update(result_peg_2485.getPosition());
    
    
    return result_peg_2485;
    out_peg_2486:
    Result result_peg_2487(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_2487.getPosition()))){
            result_peg_2487.nextPosition();
        } else {
            goto out_peg_2488;
        }
    }
    result_peg_2487.setValue((void*) "s");
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = result_peg_2487;
    stream.update(result_peg_2487.getPosition());
    
    
    return result_peg_2487;
    out_peg_2488:
    
    if (column_peg_2458.chunk11 == 0){
        column_peg_2458.chunk11 = new Chunk11();
    }
    column_peg_2458.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2490 = stream.getColumn(position);
    if (column_peg_2490.chunk11 != 0 && column_peg_2490.chunk11->chunk_value.calculated()){
        return column_peg_2490.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_2508(stream, "value");
    int myposition = position;
    
    
    Result result_peg_2491(myposition);
    
    result_peg_2491 = rule_float(stream, result_peg_2491.getPosition());
    if (result_peg_2491.error()){
        goto out_peg_2492;
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2491;
    stream.update(result_peg_2491.getPosition());
    
    
    return result_peg_2491;
    out_peg_2492:
    Result result_peg_2493(myposition);
    
    result_peg_2493 = rule_integer(stream, result_peg_2493.getPosition());
    if (result_peg_2493.error()){
        goto out_peg_2494;
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2493;
    stream.update(result_peg_2493.getPosition());
    
    
    return result_peg_2493;
    out_peg_2494:
    Result result_peg_2495(myposition);
    
    result_peg_2495 = rule_keyword(stream, result_peg_2495.getPosition());
    if (result_peg_2495.error()){
        goto out_peg_2496;
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2495;
    stream.update(result_peg_2495.getPosition());
    
    
    return result_peg_2495;
    out_peg_2496:
    Result result_peg_2497(myposition);
    
    {
    
        Result result_peg_2500(result_peg_2497);
            result_peg_2500 = rule_keyword(stream, result_peg_2500.getPosition());
            if (result_peg_2500.error()){
                goto not_peg_2499;
            }
            goto out_peg_2501;
            not_peg_2499:
            result_peg_2497.setValue((void*)0);
        
        
        
        result_peg_2497 = rule_identifier(stream, result_peg_2497.getPosition());
            if (result_peg_2497.error()){
                goto out_peg_2501;
            }
        
        
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2497;
    stream.update(result_peg_2497.getPosition());
    
    
    return result_peg_2497;
    out_peg_2501:
    Result result_peg_2502(myposition);
    
    result_peg_2502 = rule_range(stream, result_peg_2502.getPosition());
    if (result_peg_2502.error()){
        goto out_peg_2503;
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2502;
    stream.update(result_peg_2502.getPosition());
    
    
    return result_peg_2502;
    out_peg_2503:
    Result result_peg_2504(myposition);
    
    result_peg_2504 = rule_string(stream, result_peg_2504.getPosition());
    if (result_peg_2504.error()){
        goto out_peg_2505;
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2504;
    stream.update(result_peg_2504.getPosition());
    
    
    return result_peg_2504;
    out_peg_2505:
    Result result_peg_2506(myposition);
    
    result_peg_2506 = rule_hitflag(stream, result_peg_2506.getPosition());
    if (result_peg_2506.error()){
        goto out_peg_2507;
    }
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = result_peg_2506;
    stream.update(result_peg_2506.getPosition());
    
    
    return result_peg_2506;
    out_peg_2507:
    
    if (column_peg_2490.chunk11 == 0){
        column_peg_2490.chunk11 = new Chunk11();
    }
    column_peg_2490.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2509 = stream.getColumn(position);
    if (column_peg_2509.chunk12 != 0 && column_peg_2509.chunk12->chunk_helper.calculated()){
        return column_peg_2509.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_2568(stream, "helper");
    int myposition = position;
    
    Value name;
    Value id;
    Result result_peg_2510(myposition);
    
    {
    
        result_peg_2510 = rule_helper__name(stream, result_peg_2510.getPosition());
            if (result_peg_2510.error()){
                goto out_peg_2512;
            }
            name = result_peg_2510.getValues();
        
        
        
        {
                
                result_peg_2510.reset();
                do{
                    Result result_peg_2524(result_peg_2510.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2524.getPosition()))){
                                result_peg_2524.nextPosition();
                            } else {
                                goto out_peg_2527;
                            }
                        }
                        result_peg_2524.setValue((void*) " ");
                            
                    }
                    goto success_peg_2525;
                    out_peg_2527:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2524.getPosition()))){
                                result_peg_2524.nextPosition();
                            } else {
                                goto out_peg_2529;
                            }
                        }
                        result_peg_2524.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2525;
                    out_peg_2529:
                    goto loop_peg_2523;
                    success_peg_2525:
                    ;
                    result_peg_2510.addResult(result_peg_2524);
                } while (true);
                loop_peg_2523:
                ;
                        
            }
            goto success_peg_2514;
            goto out_peg_2512;
            success_peg_2514:
            ;
        
        
        
        int save_peg_2531 = result_peg_2510.getPosition();
            
            result_peg_2510 = rule_helper__expression(stream, result_peg_2510.getPosition());
            if (result_peg_2510.error()){
                
                result_peg_2510 = Result(save_peg_2531);
                result_peg_2510.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2510.reset();
                do{
                    Result result_peg_2543(result_peg_2510.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2543.getPosition()))){
                                result_peg_2543.nextPosition();
                            } else {
                                goto out_peg_2546;
                            }
                        }
                        result_peg_2543.setValue((void*) " ");
                            
                    }
                    goto success_peg_2544;
                    out_peg_2546:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2543.getPosition()))){
                                result_peg_2543.nextPosition();
                            } else {
                                goto out_peg_2548;
                            }
                        }
                        result_peg_2543.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2544;
                    out_peg_2548:
                    goto loop_peg_2542;
                    success_peg_2544:
                    ;
                    result_peg_2510.addResult(result_peg_2543);
                } while (true);
                loop_peg_2542:
                ;
                        
            }
            goto success_peg_2533;
            goto out_peg_2512;
            success_peg_2533:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2510.getPosition()))){
                    result_peg_2510.nextPosition();
                } else {
                    goto out_peg_2512;
                }
            }
            result_peg_2510.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2510.reset();
                do{
                    Result result_peg_2561(result_peg_2510.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2561.getPosition()))){
                                result_peg_2561.nextPosition();
                            } else {
                                goto out_peg_2564;
                            }
                        }
                        result_peg_2561.setValue((void*) " ");
                            
                    }
                    goto success_peg_2562;
                    out_peg_2564:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2561.getPosition()))){
                                result_peg_2561.nextPosition();
                            } else {
                                goto out_peg_2566;
                            }
                        }
                        result_peg_2561.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2562;
                    out_peg_2566:
                    goto loop_peg_2560;
                    success_peg_2562:
                    ;
                    result_peg_2510.addResult(result_peg_2561);
                } while (true);
                loop_peg_2560:
                ;
                        
            }
            goto success_peg_2551;
            goto out_peg_2512;
            success_peg_2551:
            ;
        
        
        
        result_peg_2510 = rule_helper__identifier(stream, result_peg_2510.getPosition());
            if (result_peg_2510.error()){
                goto out_peg_2512;
            }
            id = result_peg_2510.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeHelper(name, id);
                result_peg_2510.setValue(value);
            }
        
        
    }
    
    if (column_peg_2509.chunk12 == 0){
        column_peg_2509.chunk12 = new Chunk12();
    }
    column_peg_2509.chunk12->chunk_helper = result_peg_2510;
    stream.update(result_peg_2510.getPosition());
    
    
    return result_peg_2510;
    out_peg_2512:
    
    if (column_peg_2509.chunk12 == 0){
        column_peg_2509.chunk12 = new Chunk12();
    }
    column_peg_2509.chunk12->chunk_helper = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2569 = stream.getColumn(position);
    if (column_peg_2569.chunk12 != 0 && column_peg_2569.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2569.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_2608(stream, "helper__expression");
    int myposition = position;
    
    
    Result result_peg_2570(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_2570.getPosition()))){
                    result_peg_2570.nextPosition();
                } else {
                    goto out_peg_2572;
                }
            }
            result_peg_2570.setValue((void*) "(");
        
        
        
        {
                
                result_peg_2570.reset();
                do{
                    Result result_peg_2584(result_peg_2570.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2584.getPosition()))){
                                result_peg_2584.nextPosition();
                            } else {
                                goto out_peg_2587;
                            }
                        }
                        result_peg_2584.setValue((void*) " ");
                            
                    }
                    goto success_peg_2585;
                    out_peg_2587:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2584.getPosition()))){
                                result_peg_2584.nextPosition();
                            } else {
                                goto out_peg_2589;
                            }
                        }
                        result_peg_2584.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2585;
                    out_peg_2589:
                    goto loop_peg_2583;
                    success_peg_2585:
                    ;
                    result_peg_2570.addResult(result_peg_2584);
                } while (true);
                loop_peg_2583:
                ;
                        
            }
            goto success_peg_2574;
            goto out_peg_2572;
            success_peg_2574:
            ;
        
        
        
        result_peg_2570 = rule_expr_c(stream, result_peg_2570.getPosition());
            if (result_peg_2570.error()){
                goto out_peg_2572;
            }
        
        
        
        {
                
                result_peg_2570.reset();
                do{
                    Result result_peg_2602(result_peg_2570.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2602.getPosition()))){
                                result_peg_2602.nextPosition();
                            } else {
                                goto out_peg_2605;
                            }
                        }
                        result_peg_2602.setValue((void*) " ");
                            
                    }
                    goto success_peg_2603;
                    out_peg_2605:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2602.getPosition()))){
                                result_peg_2602.nextPosition();
                            } else {
                                goto out_peg_2607;
                            }
                        }
                        result_peg_2602.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2603;
                    out_peg_2607:
                    goto loop_peg_2601;
                    success_peg_2603:
                    ;
                    result_peg_2570.addResult(result_peg_2602);
                } while (true);
                loop_peg_2601:
                ;
                        
            }
            goto success_peg_2592;
            goto out_peg_2572;
            success_peg_2592:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_2570.getPosition()))){
                    result_peg_2570.nextPosition();
                } else {
                    goto out_peg_2572;
                }
            }
            result_peg_2570.setValue((void*) ")");
        
        
    }
    
    if (column_peg_2569.chunk12 == 0){
        column_peg_2569.chunk12 = new Chunk12();
    }
    column_peg_2569.chunk12->chunk_helper__expression = result_peg_2570;
    stream.update(result_peg_2570.getPosition());
    
    
    return result_peg_2570;
    out_peg_2572:
    
    if (column_peg_2569.chunk12 == 0){
        column_peg_2569.chunk12 = new Chunk12();
    }
    column_peg_2569.chunk12->chunk_helper__expression = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_2609 = stream.getColumn(position);
    if (column_peg_2609.chunk12 != 0 && column_peg_2609.chunk12->chunk_helper__name.calculated()){
        return column_peg_2609.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_2626(stream, "helper__name");
    int myposition = position;
    
    
    Result result_peg_2610(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("parent"[i], stream.get(result_peg_2610.getPosition()))){
            result_peg_2610.nextPosition();
        } else {
            goto out_peg_2611;
        }
    }
    result_peg_2610.setValue((void*) "parent");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2610;
    stream.update(result_peg_2610.getPosition());
    
    
    return result_peg_2610;
    out_peg_2611:
    Result result_peg_2612(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("root"[i], stream.get(result_peg_2612.getPosition()))){
            result_peg_2612.nextPosition();
        } else {
            goto out_peg_2613;
        }
    }
    result_peg_2612.setValue((void*) "root");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2612;
    stream.update(result_peg_2612.getPosition());
    
    
    return result_peg_2612;
    out_peg_2613:
    Result result_peg_2614(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("helper"[i], stream.get(result_peg_2614.getPosition()))){
            result_peg_2614.nextPosition();
        } else {
            goto out_peg_2615;
        }
    }
    result_peg_2614.setValue((void*) "helper");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2614;
    stream.update(result_peg_2614.getPosition());
    
    
    return result_peg_2614;
    out_peg_2615:
    Result result_peg_2616(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("target"[i], stream.get(result_peg_2616.getPosition()))){
            result_peg_2616.nextPosition();
        } else {
            goto out_peg_2617;
        }
    }
    result_peg_2616.setValue((void*) "target");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2616;
    stream.update(result_peg_2616.getPosition());
    
    
    return result_peg_2616;
    out_peg_2617:
    Result result_peg_2618(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("partner"[i], stream.get(result_peg_2618.getPosition()))){
            result_peg_2618.nextPosition();
        } else {
            goto out_peg_2619;
        }
    }
    result_peg_2618.setValue((void*) "partner");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2618;
    stream.update(result_peg_2618.getPosition());
    
    
    return result_peg_2618;
    out_peg_2619:
    Result result_peg_2620(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemyNear"[i], stream.get(result_peg_2620.getPosition()))){
            result_peg_2620.nextPosition();
        } else {
            goto out_peg_2621;
        }
    }
    result_peg_2620.setValue((void*) "enemyNear");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2620;
    stream.update(result_peg_2620.getPosition());
    
    
    return result_peg_2620;
    out_peg_2621:
    Result result_peg_2622(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("enemy"[i], stream.get(result_peg_2622.getPosition()))){
            result_peg_2622.nextPosition();
        } else {
            goto out_peg_2623;
        }
    }
    result_peg_2622.setValue((void*) "enemy");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2622;
    stream.update(result_peg_2622.getPosition());
    
    
    return result_peg_2622;
    out_peg_2623:
    Result result_peg_2624(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("playerid"[i], stream.get(result_peg_2624.getPosition()))){
            result_peg_2624.nextPosition();
        } else {
            goto out_peg_2625;
        }
    }
    result_peg_2624.setValue((void*) "playerid");
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = result_peg_2624;
    stream.update(result_peg_2624.getPosition());
    
    
    return result_peg_2624;
    out_peg_2625:
    
    if (column_peg_2609.chunk12 == 0){
        column_peg_2609.chunk12 = new Chunk12();
    }
    column_peg_2609.chunk12->chunk_helper__name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2627 = stream.getColumn(position);
    if (column_peg_2627.chunk12 != 0 && column_peg_2627.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2627.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_2637(stream, "helper__identifier");
    int myposition = position;
    
    
    Result result_peg_2628(myposition);
    
    result_peg_2628 = rule_function(stream, result_peg_2628.getPosition());
    if (result_peg_2628.error()){
        goto out_peg_2629;
    }
    
    if (column_peg_2627.chunk12 == 0){
        column_peg_2627.chunk12 = new Chunk12();
    }
    column_peg_2627.chunk12->chunk_helper__identifier = result_peg_2628;
    stream.update(result_peg_2628.getPosition());
    
    
    return result_peg_2628;
    out_peg_2629:
    Result result_peg_2630(myposition);
    
    result_peg_2630 = rule_keyword(stream, result_peg_2630.getPosition());
    if (result_peg_2630.error()){
        goto out_peg_2631;
    }
    
    if (column_peg_2627.chunk12 == 0){
        column_peg_2627.chunk12 = new Chunk12();
    }
    column_peg_2627.chunk12->chunk_helper__identifier = result_peg_2630;
    stream.update(result_peg_2630.getPosition());
    
    
    return result_peg_2630;
    out_peg_2631:
    Result result_peg_2632(myposition);
    
    {
    
        Result result_peg_2635(result_peg_2632);
            result_peg_2635 = rule_keyword(stream, result_peg_2635.getPosition());
            if (result_peg_2635.error()){
                goto not_peg_2634;
            }
            goto out_peg_2636;
            not_peg_2634:
            result_peg_2632.setValue((void*)0);
        
        
        
        result_peg_2632 = rule_identifier(stream, result_peg_2632.getPosition());
            if (result_peg_2632.error()){
                goto out_peg_2636;
            }
        
        
    }
    
    if (column_peg_2627.chunk12 == 0){
        column_peg_2627.chunk12 = new Chunk12();
    }
    column_peg_2627.chunk12->chunk_helper__identifier = result_peg_2632;
    stream.update(result_peg_2632.getPosition());
    
    
    return result_peg_2632;
    out_peg_2636:
    
    if (column_peg_2627.chunk12 == 0){
        column_peg_2627.chunk12 = new Chunk12();
    }
    column_peg_2627.chunk12->chunk_helper__identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2638 = stream.getColumn(position);
    if (column_peg_2638.chunk12 != 0 && column_peg_2638.chunk12->chunk_hitflag.calculated()){
        return column_peg_2638.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_2660(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_2639(myposition);
    
    {
    
        result_peg_2639.reset();
            do{
                Result result_peg_2642(result_peg_2639.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("A"[i], stream.get(result_peg_2642.getPosition()))){
                            result_peg_2642.nextPosition();
                        } else {
                            goto out_peg_2645;
                        }
                    }
                    result_peg_2642.setValue((void*) "A");
                        
                }
                goto success_peg_2643;
                out_peg_2645:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("M"[i], stream.get(result_peg_2642.getPosition()))){
                            result_peg_2642.nextPosition();
                        } else {
                            goto out_peg_2647;
                        }
                    }
                    result_peg_2642.setValue((void*) "M");
                        
                }
                goto success_peg_2643;
                out_peg_2647:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_2642.getPosition()))){
                            result_peg_2642.nextPosition();
                        } else {
                            goto out_peg_2649;
                        }
                    }
                    result_peg_2642.setValue((void*) "F");
                        
                }
                goto success_peg_2643;
                out_peg_2649:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("D"[i], stream.get(result_peg_2642.getPosition()))){
                            result_peg_2642.nextPosition();
                        } else {
                            goto out_peg_2651;
                        }
                    }
                    result_peg_2642.setValue((void*) "D");
                        
                }
                goto success_peg_2643;
                out_peg_2651:
                goto loop_peg_2641;
                success_peg_2643:
                ;
                result_peg_2639.addResult(result_peg_2642);
            } while (true);
            loop_peg_2641:
            if (result_peg_2639.matches() == 0){
                goto out_peg_2652;
            }
        
        
        
        int save_peg_2654 = result_peg_2639.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2639.getPosition()))){
                        result_peg_2639.nextPosition();
                    } else {
                        goto out_peg_2657;
                    }
                }
                result_peg_2639.setValue((void*) "+");
                    
            }
            goto success_peg_2655;
            out_peg_2657:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2639.getPosition()))){
                        result_peg_2639.nextPosition();
                    } else {
                        goto out_peg_2659;
                    }
                }
                result_peg_2639.setValue((void*) "-");
                    
            }
            goto success_peg_2655;
            out_peg_2659:
            
            result_peg_2639 = Result(save_peg_2654);
            result_peg_2639.setValue((void*) 0);
            
            success_peg_2655:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = makeSimpleIdentifier("A");
                result_peg_2639.setValue(value);
            }
        
        
    }
    
    if (column_peg_2638.chunk12 == 0){
        column_peg_2638.chunk12 = new Chunk12();
    }
    column_peg_2638.chunk12->chunk_hitflag = result_peg_2639;
    stream.update(result_peg_2639.getPosition());
    
    
    return result_peg_2639;
    out_peg_2652:
    
    if (column_peg_2638.chunk12 == 0){
        column_peg_2638.chunk12 = new Chunk12();
    }
    column_peg_2638.chunk12->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2661 = stream.getColumn(position);
    if (column_peg_2661.chunk13 != 0 && column_peg_2661.chunk13->chunk_keyword.calculated()){
        return column_peg_2661.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_2668(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_2662(myposition);
    
    {
    
        result_peg_2662 = rule_keyword_real(stream, result_peg_2662.getPosition());
            if (result_peg_2662.error()){
                goto out_peg_2664;
            }
        
        Result result_peg_2663 = result_peg_2662;
        
        Result result_peg_2667(result_peg_2662);
            result_peg_2667 = rule_alpha_digit(stream, result_peg_2667.getPosition());
            if (result_peg_2667.error()){
                goto not_peg_2666;
            }
            goto out_peg_2664;
            not_peg_2666:
            result_peg_2662.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_2663.getValues();
                result_peg_2662.setValue(value);
            }
        
        
    }
    
    if (column_peg_2661.chunk13 == 0){
        column_peg_2661.chunk13 = new Chunk13();
    }
    column_peg_2661.chunk13->chunk_keyword = result_peg_2662;
    stream.update(result_peg_2662.getPosition());
    
    
    return result_peg_2662;
    out_peg_2664:
    
    if (column_peg_2661.chunk13 == 0){
        column_peg_2661.chunk13 = new Chunk13();
    }
    column_peg_2661.chunk13->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2669 = stream.getColumn(position);
    if (column_peg_2669.chunk13 != 0 && column_peg_2669.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2669.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3071(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_2670(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2670.getPosition()))){
                    result_peg_2670.nextPosition();
                } else {
                    goto out_peg_2672;
                }
            }
            result_peg_2670.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2670.reset();
                do{
                    Result result_peg_2684(result_peg_2670.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2684.getPosition()))){
                                result_peg_2684.nextPosition();
                            } else {
                                goto out_peg_2687;
                            }
                        }
                        result_peg_2684.setValue((void*) " ");
                            
                    }
                    goto success_peg_2685;
                    out_peg_2687:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2684.getPosition()))){
                                result_peg_2684.nextPosition();
                            } else {
                                goto out_peg_2689;
                            }
                        }
                        result_peg_2684.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2685;
                    out_peg_2689:
                    goto loop_peg_2683;
                    success_peg_2685:
                    ;
                    result_peg_2670.addResult(result_peg_2684);
                } while (true);
                loop_peg_2683:
                ;
                        
            }
            goto success_peg_2674;
            goto out_peg_2672;
            success_peg_2674:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2670.getPosition()))){
                    result_peg_2670.nextPosition();
                } else {
                    goto out_peg_2672;
                }
            }
            result_peg_2670.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_2670.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2670;
    stream.update(result_peg_2670.getPosition());
    
    
    return result_peg_2670;
    out_peg_2672:
    Result result_peg_2691(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2691.getPosition()))){
                    result_peg_2691.nextPosition();
                } else {
                    goto out_peg_2693;
                }
            }
            result_peg_2691.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2691.reset();
                do{
                    Result result_peg_2705(result_peg_2691.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2705.getPosition()))){
                                result_peg_2705.nextPosition();
                            } else {
                                goto out_peg_2708;
                            }
                        }
                        result_peg_2705.setValue((void*) " ");
                            
                    }
                    goto success_peg_2706;
                    out_peg_2708:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2705.getPosition()))){
                                result_peg_2705.nextPosition();
                            } else {
                                goto out_peg_2710;
                            }
                        }
                        result_peg_2705.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2706;
                    out_peg_2710:
                    goto loop_peg_2704;
                    success_peg_2706:
                    ;
                    result_peg_2691.addResult(result_peg_2705);
                } while (true);
                loop_peg_2704:
                ;
                        
            }
            goto success_peg_2695;
            goto out_peg_2693;
            success_peg_2695:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2691.getPosition()))){
                    result_peg_2691.nextPosition();
                } else {
                    goto out_peg_2693;
                }
            }
            result_peg_2691.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_2691.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2691;
    stream.update(result_peg_2691.getPosition());
    
    
    return result_peg_2691;
    out_peg_2693:
    Result result_peg_2712(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2712.getPosition()))){
                    result_peg_2712.nextPosition();
                } else {
                    goto out_peg_2714;
                }
            }
            result_peg_2712.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2712.reset();
                do{
                    Result result_peg_2726(result_peg_2712.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2726.getPosition()))){
                                result_peg_2726.nextPosition();
                            } else {
                                goto out_peg_2729;
                            }
                        }
                        result_peg_2726.setValue((void*) " ");
                            
                    }
                    goto success_peg_2727;
                    out_peg_2729:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2726.getPosition()))){
                                result_peg_2726.nextPosition();
                            } else {
                                goto out_peg_2731;
                            }
                        }
                        result_peg_2726.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2727;
                    out_peg_2731:
                    goto loop_peg_2725;
                    success_peg_2727:
                    ;
                    result_peg_2712.addResult(result_peg_2726);
                } while (true);
                loop_peg_2725:
                ;
                        
            }
            goto success_peg_2716;
            goto out_peg_2714;
            success_peg_2716:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2712.getPosition()))){
                    result_peg_2712.nextPosition();
                } else {
                    goto out_peg_2714;
                }
            }
            result_peg_2712.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_2712.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2712;
    stream.update(result_peg_2712.getPosition());
    
    
    return result_peg_2712;
    out_peg_2714:
    Result result_peg_2733(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2733.getPosition()))){
                    result_peg_2733.nextPosition();
                } else {
                    goto out_peg_2735;
                }
            }
            result_peg_2733.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2733.reset();
                do{
                    Result result_peg_2747(result_peg_2733.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2747.getPosition()))){
                                result_peg_2747.nextPosition();
                            } else {
                                goto out_peg_2750;
                            }
                        }
                        result_peg_2747.setValue((void*) " ");
                            
                    }
                    goto success_peg_2748;
                    out_peg_2750:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2747.getPosition()))){
                                result_peg_2747.nextPosition();
                            } else {
                                goto out_peg_2752;
                            }
                        }
                        result_peg_2747.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2748;
                    out_peg_2752:
                    goto loop_peg_2746;
                    success_peg_2748:
                    ;
                    result_peg_2733.addResult(result_peg_2747);
                } while (true);
                loop_peg_2746:
                ;
                        
            }
            goto success_peg_2737;
            goto out_peg_2735;
            success_peg_2737:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2733.getPosition()))){
                    result_peg_2733.nextPosition();
                } else {
                    goto out_peg_2735;
                }
            }
            result_peg_2733.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_2733.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2733;
    stream.update(result_peg_2733.getPosition());
    
    
    return result_peg_2733;
    out_peg_2735:
    Result result_peg_2754(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2754.getPosition()))){
                    result_peg_2754.nextPosition();
                } else {
                    goto out_peg_2756;
                }
            }
            result_peg_2754.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2754.reset();
                do{
                    Result result_peg_2768(result_peg_2754.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2768.getPosition()))){
                                result_peg_2768.nextPosition();
                            } else {
                                goto out_peg_2771;
                            }
                        }
                        result_peg_2768.setValue((void*) " ");
                            
                    }
                    goto success_peg_2769;
                    out_peg_2771:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2768.getPosition()))){
                                result_peg_2768.nextPosition();
                            } else {
                                goto out_peg_2773;
                            }
                        }
                        result_peg_2768.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2769;
                    out_peg_2773:
                    goto loop_peg_2767;
                    success_peg_2769:
                    ;
                    result_peg_2754.addResult(result_peg_2768);
                } while (true);
                loop_peg_2767:
                ;
                        
            }
            goto success_peg_2758;
            goto out_peg_2756;
            success_peg_2758:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2754.getPosition()))){
                    result_peg_2754.nextPosition();
                } else {
                    goto out_peg_2756;
                }
            }
            result_peg_2754.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_2754.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2754;
    stream.update(result_peg_2754.getPosition());
    
    
    return result_peg_2754;
    out_peg_2756:
    Result result_peg_2775(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2775.getPosition()))){
                    result_peg_2775.nextPosition();
                } else {
                    goto out_peg_2777;
                }
            }
            result_peg_2775.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2775.reset();
                do{
                    Result result_peg_2789(result_peg_2775.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2789.getPosition()))){
                                result_peg_2789.nextPosition();
                            } else {
                                goto out_peg_2792;
                            }
                        }
                        result_peg_2789.setValue((void*) " ");
                            
                    }
                    goto success_peg_2790;
                    out_peg_2792:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2789.getPosition()))){
                                result_peg_2789.nextPosition();
                            } else {
                                goto out_peg_2794;
                            }
                        }
                        result_peg_2789.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2790;
                    out_peg_2794:
                    goto loop_peg_2788;
                    success_peg_2790:
                    ;
                    result_peg_2775.addResult(result_peg_2789);
                } while (true);
                loop_peg_2788:
                ;
                        
            }
            goto success_peg_2779;
            goto out_peg_2777;
            success_peg_2779:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2775.getPosition()))){
                    result_peg_2775.nextPosition();
                } else {
                    goto out_peg_2777;
                }
            }
            result_peg_2775.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_2775.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2775;
    stream.update(result_peg_2775.getPosition());
    
    
    return result_peg_2775;
    out_peg_2777:
    Result result_peg_2796(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2796.getPosition()))){
                    result_peg_2796.nextPosition();
                } else {
                    goto out_peg_2798;
                }
            }
            result_peg_2796.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2796.reset();
                do{
                    Result result_peg_2810(result_peg_2796.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2810.getPosition()))){
                                result_peg_2810.nextPosition();
                            } else {
                                goto out_peg_2813;
                            }
                        }
                        result_peg_2810.setValue((void*) " ");
                            
                    }
                    goto success_peg_2811;
                    out_peg_2813:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2810.getPosition()))){
                                result_peg_2810.nextPosition();
                            } else {
                                goto out_peg_2815;
                            }
                        }
                        result_peg_2810.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2811;
                    out_peg_2815:
                    goto loop_peg_2809;
                    success_peg_2811:
                    ;
                    result_peg_2796.addResult(result_peg_2810);
                } while (true);
                loop_peg_2809:
                ;
                        
            }
            goto success_peg_2800;
            goto out_peg_2798;
            success_peg_2800:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2796.getPosition()))){
                    result_peg_2796.nextPosition();
                } else {
                    goto out_peg_2798;
                }
            }
            result_peg_2796.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_2796.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2796;
    stream.update(result_peg_2796.getPosition());
    
    
    return result_peg_2796;
    out_peg_2798:
    Result result_peg_2817(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2817.getPosition()))){
                    result_peg_2817.nextPosition();
                } else {
                    goto out_peg_2819;
                }
            }
            result_peg_2817.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2817.reset();
                do{
                    Result result_peg_2831(result_peg_2817.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2831.getPosition()))){
                                result_peg_2831.nextPosition();
                            } else {
                                goto out_peg_2834;
                            }
                        }
                        result_peg_2831.setValue((void*) " ");
                            
                    }
                    goto success_peg_2832;
                    out_peg_2834:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2831.getPosition()))){
                                result_peg_2831.nextPosition();
                            } else {
                                goto out_peg_2836;
                            }
                        }
                        result_peg_2831.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2832;
                    out_peg_2836:
                    goto loop_peg_2830;
                    success_peg_2832:
                    ;
                    result_peg_2817.addResult(result_peg_2831);
                } while (true);
                loop_peg_2830:
                ;
                        
            }
            goto success_peg_2821;
            goto out_peg_2819;
            success_peg_2821:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2817.getPosition()))){
                    result_peg_2817.nextPosition();
                } else {
                    goto out_peg_2819;
                }
            }
            result_peg_2817.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_2817.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2817;
    stream.update(result_peg_2817.getPosition());
    
    
    return result_peg_2817;
    out_peg_2819:
    Result result_peg_2838(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2838.getPosition()))){
                    result_peg_2838.nextPosition();
                } else {
                    goto out_peg_2840;
                }
            }
            result_peg_2838.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2838.reset();
                do{
                    Result result_peg_2852(result_peg_2838.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2852.getPosition()))){
                                result_peg_2852.nextPosition();
                            } else {
                                goto out_peg_2855;
                            }
                        }
                        result_peg_2852.setValue((void*) " ");
                            
                    }
                    goto success_peg_2853;
                    out_peg_2855:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2852.getPosition()))){
                                result_peg_2852.nextPosition();
                            } else {
                                goto out_peg_2857;
                            }
                        }
                        result_peg_2852.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2853;
                    out_peg_2857:
                    goto loop_peg_2851;
                    success_peg_2853:
                    ;
                    result_peg_2838.addResult(result_peg_2852);
                } while (true);
                loop_peg_2851:
                ;
                        
            }
            goto success_peg_2842;
            goto out_peg_2840;
            success_peg_2842:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2838.getPosition()))){
                    result_peg_2838.nextPosition();
                } else {
                    goto out_peg_2840;
                }
            }
            result_peg_2838.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_2838.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2838;
    stream.update(result_peg_2838.getPosition());
    
    
    return result_peg_2838;
    out_peg_2840:
    Result result_peg_2859(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2859.getPosition()))){
                    result_peg_2859.nextPosition();
                } else {
                    goto out_peg_2861;
                }
            }
            result_peg_2859.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2859.reset();
                do{
                    Result result_peg_2873(result_peg_2859.getPosition());
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
                    result_peg_2859.addResult(result_peg_2873);
                } while (true);
                loop_peg_2872:
                ;
                        
            }
            goto success_peg_2863;
            goto out_peg_2861;
            success_peg_2863:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2859.getPosition()))){
                    result_peg_2859.nextPosition();
                } else {
                    goto out_peg_2861;
                }
            }
            result_peg_2859.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_2859.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2859;
    stream.update(result_peg_2859.getPosition());
    
    
    return result_peg_2859;
    out_peg_2861:
    Result result_peg_2880(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2880.getPosition()))){
                    result_peg_2880.nextPosition();
                } else {
                    goto out_peg_2882;
                }
            }
            result_peg_2880.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2880.reset();
                do{
                    Result result_peg_2894(result_peg_2880.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2894.getPosition()))){
                                result_peg_2894.nextPosition();
                            } else {
                                goto out_peg_2897;
                            }
                        }
                        result_peg_2894.setValue((void*) " ");
                            
                    }
                    goto success_peg_2895;
                    out_peg_2897:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2894.getPosition()))){
                                result_peg_2894.nextPosition();
                            } else {
                                goto out_peg_2899;
                            }
                        }
                        result_peg_2894.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2895;
                    out_peg_2899:
                    goto loop_peg_2893;
                    success_peg_2895:
                    ;
                    result_peg_2880.addResult(result_peg_2894);
                } while (true);
                loop_peg_2893:
                ;
                        
            }
            goto success_peg_2884;
            goto out_peg_2882;
            success_peg_2884:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2880.getPosition()))){
                    result_peg_2880.nextPosition();
                } else {
                    goto out_peg_2882;
                }
            }
            result_peg_2880.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_2880.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2880;
    stream.update(result_peg_2880.getPosition());
    
    
    return result_peg_2880;
    out_peg_2882:
    Result result_peg_2901(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2901.getPosition()))){
                    result_peg_2901.nextPosition();
                } else {
                    goto out_peg_2903;
                }
            }
            result_peg_2901.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2901.reset();
                do{
                    Result result_peg_2915(result_peg_2901.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2915.getPosition()))){
                                result_peg_2915.nextPosition();
                            } else {
                                goto out_peg_2918;
                            }
                        }
                        result_peg_2915.setValue((void*) " ");
                            
                    }
                    goto success_peg_2916;
                    out_peg_2918:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2915.getPosition()))){
                                result_peg_2915.nextPosition();
                            } else {
                                goto out_peg_2920;
                            }
                        }
                        result_peg_2915.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2916;
                    out_peg_2920:
                    goto loop_peg_2914;
                    success_peg_2916:
                    ;
                    result_peg_2901.addResult(result_peg_2915);
                } while (true);
                loop_peg_2914:
                ;
                        
            }
            goto success_peg_2905;
            goto out_peg_2903;
            success_peg_2905:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2901.getPosition()))){
                    result_peg_2901.nextPosition();
                } else {
                    goto out_peg_2903;
                }
            }
            result_peg_2901.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_2901.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2901;
    stream.update(result_peg_2901.getPosition());
    
    
    return result_peg_2901;
    out_peg_2903:
    Result result_peg_2922(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2922.getPosition()))){
                    result_peg_2922.nextPosition();
                } else {
                    goto out_peg_2924;
                }
            }
            result_peg_2922.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2922.reset();
                do{
                    Result result_peg_2936(result_peg_2922.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2936.getPosition()))){
                                result_peg_2936.nextPosition();
                            } else {
                                goto out_peg_2939;
                            }
                        }
                        result_peg_2936.setValue((void*) " ");
                            
                    }
                    goto success_peg_2937;
                    out_peg_2939:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2936.getPosition()))){
                                result_peg_2936.nextPosition();
                            } else {
                                goto out_peg_2941;
                            }
                        }
                        result_peg_2936.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2937;
                    out_peg_2941:
                    goto loop_peg_2935;
                    success_peg_2937:
                    ;
                    result_peg_2922.addResult(result_peg_2936);
                } while (true);
                loop_peg_2935:
                ;
                        
            }
            goto success_peg_2926;
            goto out_peg_2924;
            success_peg_2926:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2922.getPosition()))){
                    result_peg_2922.nextPosition();
                } else {
                    goto out_peg_2924;
                }
            }
            result_peg_2922.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_2922.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2922;
    stream.update(result_peg_2922.getPosition());
    
    
    return result_peg_2922;
    out_peg_2924:
    Result result_peg_2943(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_2943.getPosition()))){
                    result_peg_2943.nextPosition();
                } else {
                    goto out_peg_2945;
                }
            }
            result_peg_2943.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_2943.reset();
                do{
                    Result result_peg_2957(result_peg_2943.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2957.getPosition()))){
                                result_peg_2957.nextPosition();
                            } else {
                                goto out_peg_2960;
                            }
                        }
                        result_peg_2957.setValue((void*) " ");
                            
                    }
                    goto success_peg_2958;
                    out_peg_2960:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2957.getPosition()))){
                                result_peg_2957.nextPosition();
                            } else {
                                goto out_peg_2962;
                            }
                        }
                        result_peg_2957.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2958;
                    out_peg_2962:
                    goto loop_peg_2956;
                    success_peg_2958:
                    ;
                    result_peg_2943.addResult(result_peg_2957);
                } while (true);
                loop_peg_2956:
                ;
                        
            }
            goto success_peg_2947;
            goto out_peg_2945;
            success_peg_2947:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2943.getPosition()))){
                    result_peg_2943.nextPosition();
                } else {
                    goto out_peg_2945;
                }
            }
            result_peg_2943.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_2943.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2943;
    stream.update(result_peg_2943.getPosition());
    
    
    return result_peg_2943;
    out_peg_2945:
    Result result_peg_2964(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_2964.getPosition()))){
                    result_peg_2964.nextPosition();
                } else {
                    goto out_peg_2966;
                }
            }
            result_peg_2964.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_2964.reset();
                do{
                    Result result_peg_2978(result_peg_2964.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2978.getPosition()))){
                                result_peg_2978.nextPosition();
                            } else {
                                goto out_peg_2981;
                            }
                        }
                        result_peg_2978.setValue((void*) " ");
                            
                    }
                    goto success_peg_2979;
                    out_peg_2981:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2978.getPosition()))){
                                result_peg_2978.nextPosition();
                            } else {
                                goto out_peg_2983;
                            }
                        }
                        result_peg_2978.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2979;
                    out_peg_2983:
                    goto loop_peg_2977;
                    success_peg_2979:
                    ;
                    result_peg_2964.addResult(result_peg_2978);
                } while (true);
                loop_peg_2977:
                ;
                        
            }
            goto success_peg_2968;
            goto out_peg_2966;
            success_peg_2968:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2964.getPosition()))){
                    result_peg_2964.nextPosition();
                } else {
                    goto out_peg_2966;
                }
            }
            result_peg_2964.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_2964.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2964;
    stream.update(result_peg_2964.getPosition());
    
    
    return result_peg_2964;
    out_peg_2966:
    Result result_peg_2985(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2985.getPosition()))){
                    result_peg_2985.nextPosition();
                } else {
                    goto out_peg_2987;
                }
            }
            result_peg_2985.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2985.reset();
                do{
                    Result result_peg_2999(result_peg_2985.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2999.getPosition()))){
                                result_peg_2999.nextPosition();
                            } else {
                                goto out_peg_3002;
                            }
                        }
                        result_peg_2999.setValue((void*) " ");
                            
                    }
                    goto success_peg_3000;
                    out_peg_3002:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2999.getPosition()))){
                                result_peg_2999.nextPosition();
                            } else {
                                goto out_peg_3004;
                            }
                        }
                        result_peg_2999.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3000;
                    out_peg_3004:
                    goto loop_peg_2998;
                    success_peg_3000:
                    ;
                    result_peg_2985.addResult(result_peg_2999);
                } while (true);
                loop_peg_2998:
                ;
                        
            }
            goto success_peg_2989;
            goto out_peg_2987;
            success_peg_2989:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2985.getPosition()))){
                    result_peg_2985.nextPosition();
                } else {
                    goto out_peg_2987;
                }
            }
            result_peg_2985.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_2985.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_2985;
    stream.update(result_peg_2985.getPosition());
    
    
    return result_peg_2985;
    out_peg_2987:
    Result result_peg_3006(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_3006.getPosition()))){
                    result_peg_3006.nextPosition();
                } else {
                    goto out_peg_3008;
                }
            }
            result_peg_3006.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_3006.reset();
                do{
                    Result result_peg_3020(result_peg_3006.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3020.getPosition()))){
                                result_peg_3020.nextPosition();
                            } else {
                                goto out_peg_3023;
                            }
                        }
                        result_peg_3020.setValue((void*) " ");
                            
                    }
                    goto success_peg_3021;
                    out_peg_3023:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3020.getPosition()))){
                                result_peg_3020.nextPosition();
                            } else {
                                goto out_peg_3025;
                            }
                        }
                        result_peg_3020.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3021;
                    out_peg_3025:
                    goto loop_peg_3019;
                    success_peg_3021:
                    ;
                    result_peg_3006.addResult(result_peg_3020);
                } while (true);
                loop_peg_3019:
                ;
                        
            }
            goto success_peg_3010;
            goto out_peg_3008;
            success_peg_3010:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3006.getPosition()))){
                    result_peg_3006.nextPosition();
                } else {
                    goto out_peg_3008;
                }
            }
            result_peg_3006.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist y");
                result_peg_3006.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_3006;
    stream.update(result_peg_3006.getPosition());
    
    
    return result_peg_3006;
    out_peg_3008:
    Result result_peg_3027(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_3027.getPosition()))){
                    result_peg_3027.nextPosition();
                } else {
                    goto out_peg_3029;
                }
            }
            result_peg_3027.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_3027.reset();
                do{
                    Result result_peg_3041(result_peg_3027.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3041.getPosition()))){
                                result_peg_3041.nextPosition();
                            } else {
                                goto out_peg_3044;
                            }
                        }
                        result_peg_3041.setValue((void*) " ");
                            
                    }
                    goto success_peg_3042;
                    out_peg_3044:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3041.getPosition()))){
                                result_peg_3041.nextPosition();
                            } else {
                                goto out_peg_3046;
                            }
                        }
                        result_peg_3041.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3042;
                    out_peg_3046:
                    goto loop_peg_3040;
                    success_peg_3042:
                    ;
                    result_peg_3027.addResult(result_peg_3041);
                } while (true);
                loop_peg_3040:
                ;
                        
            }
            goto success_peg_3031;
            goto out_peg_3029;
            success_peg_3031:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_3027.getPosition()))){
                    result_peg_3027.nextPosition();
                } else {
                    goto out_peg_3029;
                }
            }
            result_peg_3027.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist x");
                result_peg_3027.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_3027;
    stream.update(result_peg_3027.getPosition());
    
    
    return result_peg_3027;
    out_peg_3029:
    Result result_peg_3048(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_3048.getPosition()))){
                    result_peg_3048.nextPosition();
                } else {
                    goto out_peg_3050;
                }
            }
            result_peg_3048.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_3048.reset();
                do{
                    Result result_peg_3062(result_peg_3048.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3062.getPosition()))){
                                result_peg_3062.nextPosition();
                            } else {
                                goto out_peg_3065;
                            }
                        }
                        result_peg_3062.setValue((void*) " ");
                            
                    }
                    goto success_peg_3063;
                    out_peg_3065:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3062.getPosition()))){
                                result_peg_3062.nextPosition();
                            } else {
                                goto out_peg_3067;
                            }
                        }
                        result_peg_3062.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3063;
                    out_peg_3067:
                    goto loop_peg_3061;
                    success_peg_3063:
                    ;
                    result_peg_3048.addResult(result_peg_3062);
                } while (true);
                loop_peg_3061:
                ;
                        
            }
            goto success_peg_3052;
            goto out_peg_3050;
            success_peg_3052:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3048.getPosition()))){
                    result_peg_3048.nextPosition();
                } else {
                    goto out_peg_3050;
                }
            }
            result_peg_3048.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist y");
                result_peg_3048.setValue(value);
            }
        
        
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_3048;
    stream.update(result_peg_3048.getPosition());
    
    
    return result_peg_3048;
    out_peg_3050:
    Result result_peg_3069(myposition);
    
    result_peg_3069 = rule_hitflag(stream, result_peg_3069.getPosition());
    if (result_peg_3069.error()){
        goto out_peg_3070;
    }
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = result_peg_3069;
    stream.update(result_peg_3069.getPosition());
    
    
    return result_peg_3069;
    out_peg_3070:
    
    if (column_peg_2669.chunk13 == 0){
        column_peg_2669.chunk13 = new Chunk13();
    }
    column_peg_2669.chunk13->chunk_keyword_real = errorResult;
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

        
