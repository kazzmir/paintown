

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
    
    RuleTrace trace_peg_1193(stream, "expr13");
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
        
        {
                
                result_peg_1170.reset();
                do{
                    Result result_peg_1185(result_peg_1170.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1185.getPosition()))){
                                result_peg_1185.nextPosition();
                            } else {
                                goto out_peg_1188;
                            }
                        }
                        result_peg_1185.setValue((void*) " ");
                            
                    }
                    goto success_peg_1186;
                    out_peg_1188:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1185.getPosition()))){
                                result_peg_1185.nextPosition();
                            } else {
                                goto out_peg_1190;
                            }
                        }
                        result_peg_1185.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1186;
                    out_peg_1190:
                    goto loop_peg_1184;
                    success_peg_1186:
                    ;
                    result_peg_1170.addResult(result_peg_1185);
                } while (true);
                loop_peg_1184:
                ;
                        
            }
            goto success_peg_1175;
            goto out_peg_1191;
            success_peg_1175:
            ;
        
        
        
        result_peg_1170 = rule_expr13_real(stream, result_peg_1170.getPosition());
            if (result_peg_1170.error()){
                goto out_peg_1191;
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
    out_peg_1191:
    
    if (column_peg_1169.chunk6 == 0){
        column_peg_1169.chunk6 = new Chunk6();
    }
    column_peg_1169.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1237(stream, "expr_rest");
    int myposition = position;
    tail_peg_1196:
    Value right;
    Value new_left;
    Result result_peg_1195(myposition);
    {
    
        {
                
                result_peg_1195.reset();
                do{
                    Result result_peg_1208(result_peg_1195.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1208.getPosition()))){
                                result_peg_1208.nextPosition();
                            } else {
                                goto out_peg_1211;
                            }
                        }
                        result_peg_1208.setValue((void*) " ");
                            
                    }
                    goto success_peg_1209;
                    out_peg_1211:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1208.getPosition()))){
                                result_peg_1208.nextPosition();
                            } else {
                                goto out_peg_1213;
                            }
                        }
                        result_peg_1208.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1209;
                    out_peg_1213:
                    goto loop_peg_1207;
                    success_peg_1209:
                    ;
                    result_peg_1195.addResult(result_peg_1208);
                } while (true);
                loop_peg_1207:
                ;
                        
            }
            goto success_peg_1198;
            goto out_peg_1214;
            success_peg_1198:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_1195.getPosition()))){
                    result_peg_1195.nextPosition();
                } else {
                    goto out_peg_1214;
                }
            }
            result_peg_1195.setValue((void*) "||");
        
        
        
        {
                
                result_peg_1195.reset();
                do{
                    Result result_peg_1227(result_peg_1195.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1227.getPosition()))){
                                result_peg_1227.nextPosition();
                            } else {
                                goto out_peg_1230;
                            }
                        }
                        result_peg_1227.setValue((void*) " ");
                            
                    }
                    goto success_peg_1228;
                    out_peg_1230:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1227.getPosition()))){
                                result_peg_1227.nextPosition();
                            } else {
                                goto out_peg_1232;
                            }
                        }
                        result_peg_1227.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1228;
                    out_peg_1232:
                    goto loop_peg_1226;
                    success_peg_1228:
                    ;
                    result_peg_1195.addResult(result_peg_1227);
                } while (true);
                loop_peg_1226:
                ;
                        
            }
            goto success_peg_1217;
            goto out_peg_1214;
            success_peg_1217:
            ;
        
        
        
        result_peg_1195 = rule_expr2(stream, result_peg_1195.getPosition());
            if (result_peg_1195.error()){
                goto out_peg_1214;
            }
            right = result_peg_1195.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_1195.setValue(value);
            }
            new_left = result_peg_1195.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1195.getPosition();
    goto tail_peg_1196;
    out_peg_1214:
    Result result_peg_1235(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1235.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1235.getPosition());
    
    return result_peg_1235;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1281(stream, "expr2_rest");
    int myposition = position;
    tail_peg_1240:
    Value right;
    Value new_left;
    Result result_peg_1239(myposition);
    {
    
        {
                
                result_peg_1239.reset();
                do{
                    Result result_peg_1252(result_peg_1239.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1252.getPosition()))){
                                result_peg_1252.nextPosition();
                            } else {
                                goto out_peg_1255;
                            }
                        }
                        result_peg_1252.setValue((void*) " ");
                            
                    }
                    goto success_peg_1253;
                    out_peg_1255:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1252.getPosition()))){
                                result_peg_1252.nextPosition();
                            } else {
                                goto out_peg_1257;
                            }
                        }
                        result_peg_1252.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1253;
                    out_peg_1257:
                    goto loop_peg_1251;
                    success_peg_1253:
                    ;
                    result_peg_1239.addResult(result_peg_1252);
                } while (true);
                loop_peg_1251:
                ;
                        
            }
            goto success_peg_1242;
            goto out_peg_1258;
            success_peg_1242:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_1239.getPosition()))){
                    result_peg_1239.nextPosition();
                } else {
                    goto out_peg_1258;
                }
            }
            result_peg_1239.setValue((void*) "^^");
        
        
        
        {
                
                result_peg_1239.reset();
                do{
                    Result result_peg_1271(result_peg_1239.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1271.getPosition()))){
                                result_peg_1271.nextPosition();
                            } else {
                                goto out_peg_1274;
                            }
                        }
                        result_peg_1271.setValue((void*) " ");
                            
                    }
                    goto success_peg_1272;
                    out_peg_1274:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1271.getPosition()))){
                                result_peg_1271.nextPosition();
                            } else {
                                goto out_peg_1276;
                            }
                        }
                        result_peg_1271.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1272;
                    out_peg_1276:
                    goto loop_peg_1270;
                    success_peg_1272:
                    ;
                    result_peg_1239.addResult(result_peg_1271);
                } while (true);
                loop_peg_1270:
                ;
                        
            }
            goto success_peg_1261;
            goto out_peg_1258;
            success_peg_1261:
            ;
        
        
        
        result_peg_1239 = rule_expr3(stream, result_peg_1239.getPosition());
            if (result_peg_1239.error()){
                goto out_peg_1258;
            }
            right = result_peg_1239.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_1239.setValue(value);
            }
            new_left = result_peg_1239.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1239.getPosition();
    goto tail_peg_1240;
    out_peg_1258:
    Result result_peg_1279(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1279.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1279.getPosition());
    
    return result_peg_1279;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1325(stream, "expr3_rest");
    int myposition = position;
    tail_peg_1284:
    Value right;
    Value new_left;
    Result result_peg_1283(myposition);
    {
    
        {
                
                result_peg_1283.reset();
                do{
                    Result result_peg_1296(result_peg_1283.getPosition());
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
                    result_peg_1283.addResult(result_peg_1296);
                } while (true);
                loop_peg_1295:
                ;
                        
            }
            goto success_peg_1286;
            goto out_peg_1302;
            success_peg_1286:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_1283.getPosition()))){
                    result_peg_1283.nextPosition();
                } else {
                    goto out_peg_1302;
                }
            }
            result_peg_1283.setValue((void*) "&&");
        
        
        
        {
                
                result_peg_1283.reset();
                do{
                    Result result_peg_1315(result_peg_1283.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1315.getPosition()))){
                                result_peg_1315.nextPosition();
                            } else {
                                goto out_peg_1318;
                            }
                        }
                        result_peg_1315.setValue((void*) " ");
                            
                    }
                    goto success_peg_1316;
                    out_peg_1318:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1315.getPosition()))){
                                result_peg_1315.nextPosition();
                            } else {
                                goto out_peg_1320;
                            }
                        }
                        result_peg_1315.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1316;
                    out_peg_1320:
                    goto loop_peg_1314;
                    success_peg_1316:
                    ;
                    result_peg_1283.addResult(result_peg_1315);
                } while (true);
                loop_peg_1314:
                ;
                        
            }
            goto success_peg_1305;
            goto out_peg_1302;
            success_peg_1305:
            ;
        
        
        
        result_peg_1283 = rule_expr4(stream, result_peg_1283.getPosition());
            if (result_peg_1283.error()){
                goto out_peg_1302;
            }
            right = result_peg_1283.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_1283.setValue(value);
            }
            new_left = result_peg_1283.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1283.getPosition();
    goto tail_peg_1284;
    out_peg_1302:
    Result result_peg_1323(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1323.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1323.getPosition());
    
    return result_peg_1323;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1369(stream, "expr4_rest");
    int myposition = position;
    tail_peg_1328:
    Value right;
    Value new_left;
    Result result_peg_1327(myposition);
    {
    
        {
                
                result_peg_1327.reset();
                do{
                    Result result_peg_1340(result_peg_1327.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1340.getPosition()))){
                                result_peg_1340.nextPosition();
                            } else {
                                goto out_peg_1343;
                            }
                        }
                        result_peg_1340.setValue((void*) " ");
                            
                    }
                    goto success_peg_1341;
                    out_peg_1343:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1340.getPosition()))){
                                result_peg_1340.nextPosition();
                            } else {
                                goto out_peg_1345;
                            }
                        }
                        result_peg_1340.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1341;
                    out_peg_1345:
                    goto loop_peg_1339;
                    success_peg_1341:
                    ;
                    result_peg_1327.addResult(result_peg_1340);
                } while (true);
                loop_peg_1339:
                ;
                        
            }
            goto success_peg_1330;
            goto out_peg_1346;
            success_peg_1330:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_1327.getPosition()))){
                    result_peg_1327.nextPosition();
                } else {
                    goto out_peg_1346;
                }
            }
            result_peg_1327.setValue((void*) "|");
        
        
        
        {
                
                result_peg_1327.reset();
                do{
                    Result result_peg_1359(result_peg_1327.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1359.getPosition()))){
                                result_peg_1359.nextPosition();
                            } else {
                                goto out_peg_1362;
                            }
                        }
                        result_peg_1359.setValue((void*) " ");
                            
                    }
                    goto success_peg_1360;
                    out_peg_1362:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1359.getPosition()))){
                                result_peg_1359.nextPosition();
                            } else {
                                goto out_peg_1364;
                            }
                        }
                        result_peg_1359.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1360;
                    out_peg_1364:
                    goto loop_peg_1358;
                    success_peg_1360:
                    ;
                    result_peg_1327.addResult(result_peg_1359);
                } while (true);
                loop_peg_1358:
                ;
                        
            }
            goto success_peg_1349;
            goto out_peg_1346;
            success_peg_1349:
            ;
        
        
        
        result_peg_1327 = rule_expr5(stream, result_peg_1327.getPosition());
            if (result_peg_1327.error()){
                goto out_peg_1346;
            }
            right = result_peg_1327.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_1327.setValue(value);
            }
            new_left = result_peg_1327.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1327.getPosition();
    goto tail_peg_1328;
    out_peg_1346:
    Result result_peg_1367(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1367.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1367.getPosition());
    
    return result_peg_1367;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1413(stream, "expr5_rest");
    int myposition = position;
    tail_peg_1372:
    Value right;
    Value new_left;
    Result result_peg_1371(myposition);
    {
    
        {
                
                result_peg_1371.reset();
                do{
                    Result result_peg_1384(result_peg_1371.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1384.getPosition()))){
                                result_peg_1384.nextPosition();
                            } else {
                                goto out_peg_1387;
                            }
                        }
                        result_peg_1384.setValue((void*) " ");
                            
                    }
                    goto success_peg_1385;
                    out_peg_1387:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1384.getPosition()))){
                                result_peg_1384.nextPosition();
                            } else {
                                goto out_peg_1389;
                            }
                        }
                        result_peg_1384.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1385;
                    out_peg_1389:
                    goto loop_peg_1383;
                    success_peg_1385:
                    ;
                    result_peg_1371.addResult(result_peg_1384);
                } while (true);
                loop_peg_1383:
                ;
                        
            }
            goto success_peg_1374;
            goto out_peg_1390;
            success_peg_1374:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_1371.getPosition()))){
                    result_peg_1371.nextPosition();
                } else {
                    goto out_peg_1390;
                }
            }
            result_peg_1371.setValue((void*) "^");
        
        
        
        {
                
                result_peg_1371.reset();
                do{
                    Result result_peg_1403(result_peg_1371.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1403.getPosition()))){
                                result_peg_1403.nextPosition();
                            } else {
                                goto out_peg_1406;
                            }
                        }
                        result_peg_1403.setValue((void*) " ");
                            
                    }
                    goto success_peg_1404;
                    out_peg_1406:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1403.getPosition()))){
                                result_peg_1403.nextPosition();
                            } else {
                                goto out_peg_1408;
                            }
                        }
                        result_peg_1403.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1404;
                    out_peg_1408:
                    goto loop_peg_1402;
                    success_peg_1404:
                    ;
                    result_peg_1371.addResult(result_peg_1403);
                } while (true);
                loop_peg_1402:
                ;
                        
            }
            goto success_peg_1393;
            goto out_peg_1390;
            success_peg_1393:
            ;
        
        
        
        result_peg_1371 = rule_expr6(stream, result_peg_1371.getPosition());
            if (result_peg_1371.error()){
                goto out_peg_1390;
            }
            right = result_peg_1371.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_1371.setValue(value);
            }
            new_left = result_peg_1371.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1371.getPosition();
    goto tail_peg_1372;
    out_peg_1390:
    Result result_peg_1411(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1411.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1411.getPosition());
    
    return result_peg_1411;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1457(stream, "expr6_rest");
    int myposition = position;
    tail_peg_1416:
    Value right;
    Value new_left;
    Result result_peg_1415(myposition);
    {
    
        {
                
                result_peg_1415.reset();
                do{
                    Result result_peg_1428(result_peg_1415.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1428.getPosition()))){
                                result_peg_1428.nextPosition();
                            } else {
                                goto out_peg_1431;
                            }
                        }
                        result_peg_1428.setValue((void*) " ");
                            
                    }
                    goto success_peg_1429;
                    out_peg_1431:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1428.getPosition()))){
                                result_peg_1428.nextPosition();
                            } else {
                                goto out_peg_1433;
                            }
                        }
                        result_peg_1428.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1429;
                    out_peg_1433:
                    goto loop_peg_1427;
                    success_peg_1429:
                    ;
                    result_peg_1415.addResult(result_peg_1428);
                } while (true);
                loop_peg_1427:
                ;
                        
            }
            goto success_peg_1418;
            goto out_peg_1434;
            success_peg_1418:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_1415.getPosition()))){
                    result_peg_1415.nextPosition();
                } else {
                    goto out_peg_1434;
                }
            }
            result_peg_1415.setValue((void*) "&");
        
        
        
        {
                
                result_peg_1415.reset();
                do{
                    Result result_peg_1447(result_peg_1415.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1447.getPosition()))){
                                result_peg_1447.nextPosition();
                            } else {
                                goto out_peg_1450;
                            }
                        }
                        result_peg_1447.setValue((void*) " ");
                            
                    }
                    goto success_peg_1448;
                    out_peg_1450:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1447.getPosition()))){
                                result_peg_1447.nextPosition();
                            } else {
                                goto out_peg_1452;
                            }
                        }
                        result_peg_1447.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1448;
                    out_peg_1452:
                    goto loop_peg_1446;
                    success_peg_1448:
                    ;
                    result_peg_1415.addResult(result_peg_1447);
                } while (true);
                loop_peg_1446:
                ;
                        
            }
            goto success_peg_1437;
            goto out_peg_1434;
            success_peg_1437:
            ;
        
        
        
        result_peg_1415 = rule_expr7(stream, result_peg_1415.getPosition());
            if (result_peg_1415.error()){
                goto out_peg_1434;
            }
            right = result_peg_1415.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_1415.setValue(value);
            }
            new_left = result_peg_1415.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1415.getPosition();
    goto tail_peg_1416;
    out_peg_1434:
    Result result_peg_1455(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1455.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1455.getPosition());
    
    return result_peg_1455;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1501(stream, "expr7_rest");
    int myposition = position;
    tail_peg_1460:
    Value right;
    Value new_left;
    Result result_peg_1459(myposition);
    {
    
        {
                
                result_peg_1459.reset();
                do{
                    Result result_peg_1472(result_peg_1459.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1472.getPosition()))){
                                result_peg_1472.nextPosition();
                            } else {
                                goto out_peg_1475;
                            }
                        }
                        result_peg_1472.setValue((void*) " ");
                            
                    }
                    goto success_peg_1473;
                    out_peg_1475:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1472.getPosition()))){
                                result_peg_1472.nextPosition();
                            } else {
                                goto out_peg_1477;
                            }
                        }
                        result_peg_1472.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1473;
                    out_peg_1477:
                    goto loop_peg_1471;
                    success_peg_1473:
                    ;
                    result_peg_1459.addResult(result_peg_1472);
                } while (true);
                loop_peg_1471:
                ;
                        
            }
            goto success_peg_1462;
            goto out_peg_1478;
            success_peg_1462:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_1459.getPosition()))){
                    result_peg_1459.nextPosition();
                } else {
                    goto out_peg_1478;
                }
            }
            result_peg_1459.setValue((void*) ":=");
        
        
        
        {
                
                result_peg_1459.reset();
                do{
                    Result result_peg_1491(result_peg_1459.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1491.getPosition()))){
                                result_peg_1491.nextPosition();
                            } else {
                                goto out_peg_1494;
                            }
                        }
                        result_peg_1491.setValue((void*) " ");
                            
                    }
                    goto success_peg_1492;
                    out_peg_1494:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1491.getPosition()))){
                                result_peg_1491.nextPosition();
                            } else {
                                goto out_peg_1496;
                            }
                        }
                        result_peg_1491.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1492;
                    out_peg_1496:
                    goto loop_peg_1490;
                    success_peg_1492:
                    ;
                    result_peg_1459.addResult(result_peg_1491);
                } while (true);
                loop_peg_1490:
                ;
                        
            }
            goto success_peg_1481;
            goto out_peg_1478;
            success_peg_1481:
            ;
        
        
        
        result_peg_1459 = rule_expr8(stream, result_peg_1459.getPosition());
            if (result_peg_1459.error()){
                goto out_peg_1478;
            }
            right = result_peg_1459.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_1459.setValue(value);
            }
            new_left = result_peg_1459.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1459.getPosition();
    goto tail_peg_1460;
    out_peg_1478:
    Result result_peg_1499(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1499.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1499.getPosition());
    
    return result_peg_1499;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1584(stream, "expr8_rest");
    int myposition = position;
    tail_peg_1504:
    Value right;
    Value new_left;
    Result result_peg_1503(myposition);
    {
    
        {
                
                result_peg_1503.reset();
                do{
                    Result result_peg_1516(result_peg_1503.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1516.getPosition()))){
                                result_peg_1516.nextPosition();
                            } else {
                                goto out_peg_1519;
                            }
                        }
                        result_peg_1516.setValue((void*) " ");
                            
                    }
                    goto success_peg_1517;
                    out_peg_1519:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1516.getPosition()))){
                                result_peg_1516.nextPosition();
                            } else {
                                goto out_peg_1521;
                            }
                        }
                        result_peg_1516.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1517;
                    out_peg_1521:
                    goto loop_peg_1515;
                    success_peg_1517:
                    ;
                    result_peg_1503.addResult(result_peg_1516);
                } while (true);
                loop_peg_1515:
                ;
                        
            }
            goto success_peg_1506;
            goto out_peg_1522;
            success_peg_1506:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1503.getPosition()))){
                    result_peg_1503.nextPosition();
                } else {
                    goto out_peg_1522;
                }
            }
            result_peg_1503.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1503.reset();
                do{
                    Result result_peg_1535(result_peg_1503.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1535.getPosition()))){
                                result_peg_1535.nextPosition();
                            } else {
                                goto out_peg_1538;
                            }
                        }
                        result_peg_1535.setValue((void*) " ");
                            
                    }
                    goto success_peg_1536;
                    out_peg_1538:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1535.getPosition()))){
                                result_peg_1535.nextPosition();
                            } else {
                                goto out_peg_1540;
                            }
                        }
                        result_peg_1535.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1536;
                    out_peg_1540:
                    goto loop_peg_1534;
                    success_peg_1536:
                    ;
                    result_peg_1503.addResult(result_peg_1535);
                } while (true);
                loop_peg_1534:
                ;
                        
            }
            goto success_peg_1525;
            goto out_peg_1522;
            success_peg_1525:
            ;
        
        
        
        result_peg_1503 = rule_expr9(stream, result_peg_1503.getPosition());
            if (result_peg_1503.error()){
                goto out_peg_1522;
            }
            right = result_peg_1503.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_1503.setValue(value);
            }
            new_left = result_peg_1503.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1503.getPosition();
    goto tail_peg_1504;
    out_peg_1522:
    Result result_peg_1543(myposition);
    {
    
        {
                
                result_peg_1543.reset();
                do{
                    Result result_peg_1555(result_peg_1543.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1555.getPosition()))){
                                result_peg_1555.nextPosition();
                            } else {
                                goto out_peg_1558;
                            }
                        }
                        result_peg_1555.setValue((void*) " ");
                            
                    }
                    goto success_peg_1556;
                    out_peg_1558:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1555.getPosition()))){
                                result_peg_1555.nextPosition();
                            } else {
                                goto out_peg_1560;
                            }
                        }
                        result_peg_1555.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1556;
                    out_peg_1560:
                    goto loop_peg_1554;
                    success_peg_1556:
                    ;
                    result_peg_1543.addResult(result_peg_1555);
                } while (true);
                loop_peg_1554:
                ;
                        
            }
            goto success_peg_1545;
            goto out_peg_1561;
            success_peg_1545:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_1543.getPosition()))){
                    result_peg_1543.nextPosition();
                } else {
                    goto out_peg_1561;
                }
            }
            result_peg_1543.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_1543.reset();
                do{
                    Result result_peg_1574(result_peg_1543.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1574.getPosition()))){
                                result_peg_1574.nextPosition();
                            } else {
                                goto out_peg_1577;
                            }
                        }
                        result_peg_1574.setValue((void*) " ");
                            
                    }
                    goto success_peg_1575;
                    out_peg_1577:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1574.getPosition()))){
                                result_peg_1574.nextPosition();
                            } else {
                                goto out_peg_1579;
                            }
                        }
                        result_peg_1574.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1575;
                    out_peg_1579:
                    goto loop_peg_1573;
                    success_peg_1575:
                    ;
                    result_peg_1543.addResult(result_peg_1574);
                } while (true);
                loop_peg_1573:
                ;
                        
            }
            goto success_peg_1564;
            goto out_peg_1561;
            success_peg_1564:
            ;
        
        
        
        result_peg_1543 = rule_expr9(stream, result_peg_1543.getPosition());
            if (result_peg_1543.error()){
                goto out_peg_1561;
            }
            right = result_peg_1543.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_1543.setValue(value);
            }
            new_left = result_peg_1543.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1543.getPosition();
    goto tail_peg_1504;
    out_peg_1561:
    Result result_peg_1582(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1582.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1582.getPosition());
    
    return result_peg_1582;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1628(stream, "expr9_rest");
    int myposition = position;
    tail_peg_1587:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_1586(myposition);
    {
    
        {
                
                result_peg_1586.reset();
                do{
                    Result result_peg_1599(result_peg_1586.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1599.getPosition()))){
                                result_peg_1599.nextPosition();
                            } else {
                                goto out_peg_1602;
                            }
                        }
                        result_peg_1599.setValue((void*) " ");
                            
                    }
                    goto success_peg_1600;
                    out_peg_1602:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1599.getPosition()))){
                                result_peg_1599.nextPosition();
                            } else {
                                goto out_peg_1604;
                            }
                        }
                        result_peg_1599.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1600;
                    out_peg_1604:
                    goto loop_peg_1598;
                    success_peg_1600:
                    ;
                    result_peg_1586.addResult(result_peg_1599);
                } while (true);
                loop_peg_1598:
                ;
                        
            }
            goto success_peg_1589;
            goto out_peg_1605;
            success_peg_1589:
            ;
        
        
        
        result_peg_1586 = rule_compare(stream, result_peg_1586.getPosition());
            if (result_peg_1586.error()){
                goto out_peg_1605;
            }
            maker = result_peg_1586.getValues();
        
        
        
        {
                
                result_peg_1586.reset();
                do{
                    Result result_peg_1618(result_peg_1586.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1618.getPosition()))){
                                result_peg_1618.nextPosition();
                            } else {
                                goto out_peg_1621;
                            }
                        }
                        result_peg_1618.setValue((void*) " ");
                            
                    }
                    goto success_peg_1619;
                    out_peg_1621:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1618.getPosition()))){
                                result_peg_1618.nextPosition();
                            } else {
                                goto out_peg_1623;
                            }
                        }
                        result_peg_1618.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1619;
                    out_peg_1623:
                    goto loop_peg_1617;
                    success_peg_1619:
                    ;
                    result_peg_1586.addResult(result_peg_1618);
                } while (true);
                loop_peg_1617:
                ;
                        
            }
            goto success_peg_1608;
            goto out_peg_1605;
            success_peg_1608:
            ;
        
        
        
        result_peg_1586 = rule_expr10(stream, result_peg_1586.getPosition());
            if (result_peg_1586.error()){
                goto out_peg_1605;
            }
            right = result_peg_1586.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_1586.setValue(value);
            }
            new_left = result_peg_1586.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1586.getPosition();
    goto tail_peg_1587;
    out_peg_1605:
    Result result_peg_1626(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1626.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1626.getPosition());
    
    return result_peg_1626;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1629 = stream.getColumn(position);
    if (column_peg_1629.chunk8 != 0 && column_peg_1629.chunk8->chunk_compare.calculated()){
        return column_peg_1629.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1642(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_1630(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_1630.getPosition()))){
                    result_peg_1630.nextPosition();
                } else {
                    goto out_peg_1632;
                }
            }
            result_peg_1630.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_1630.setValue(value);
            }
        
        
    }
    
    if (column_peg_1629.chunk8 == 0){
        column_peg_1629.chunk8 = new Chunk8();
    }
    column_peg_1629.chunk8->chunk_compare = result_peg_1630;
    stream.update(result_peg_1630.getPosition());
    
    
    return result_peg_1630;
    out_peg_1632:
    Result result_peg_1633(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_1633.getPosition()))){
                    result_peg_1633.nextPosition();
                } else {
                    goto out_peg_1635;
                }
            }
            result_peg_1633.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_1633.setValue(value);
            }
        
        
    }
    
    if (column_peg_1629.chunk8 == 0){
        column_peg_1629.chunk8 = new Chunk8();
    }
    column_peg_1629.chunk8->chunk_compare = result_peg_1633;
    stream.update(result_peg_1633.getPosition());
    
    
    return result_peg_1633;
    out_peg_1635:
    Result result_peg_1636(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_1636.getPosition()))){
                    result_peg_1636.nextPosition();
                } else {
                    goto out_peg_1638;
                }
            }
            result_peg_1636.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_1636.setValue(value);
            }
        
        
    }
    
    if (column_peg_1629.chunk8 == 0){
        column_peg_1629.chunk8 = new Chunk8();
    }
    column_peg_1629.chunk8->chunk_compare = result_peg_1636;
    stream.update(result_peg_1636.getPosition());
    
    
    return result_peg_1636;
    out_peg_1638:
    Result result_peg_1639(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1639.getPosition()))){
                    result_peg_1639.nextPosition();
                } else {
                    goto out_peg_1641;
                }
            }
            result_peg_1639.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_1639.setValue(value);
            }
        
        
    }
    
    if (column_peg_1629.chunk8 == 0){
        column_peg_1629.chunk8 = new Chunk8();
    }
    column_peg_1629.chunk8->chunk_compare = result_peg_1639;
    stream.update(result_peg_1639.getPosition());
    
    
    return result_peg_1639;
    out_peg_1641:
    
    if (column_peg_1629.chunk8 == 0){
        column_peg_1629.chunk8 = new Chunk8();
    }
    column_peg_1629.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1643 = stream.getColumn(position);
    if (column_peg_1643.chunk8 != 0 && column_peg_1643.chunk8->chunk_all_compare.calculated()){
        return column_peg_1643.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1650(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_1644(myposition);
    
    result_peg_1644 = rule_compare(stream, result_peg_1644.getPosition());
    if (result_peg_1644.error()){
        goto out_peg_1645;
    }
    
    if (column_peg_1643.chunk8 == 0){
        column_peg_1643.chunk8 = new Chunk8();
    }
    column_peg_1643.chunk8->chunk_all_compare = result_peg_1644;
    stream.update(result_peg_1644.getPosition());
    
    
    return result_peg_1644;
    out_peg_1645:
    Result result_peg_1646(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_1646.getPosition()))){
            result_peg_1646.nextPosition();
        } else {
            goto out_peg_1647;
        }
    }
    result_peg_1646.setValue((void*) "=");
    
    if (column_peg_1643.chunk8 == 0){
        column_peg_1643.chunk8 = new Chunk8();
    }
    column_peg_1643.chunk8->chunk_all_compare = result_peg_1646;
    stream.update(result_peg_1646.getPosition());
    
    
    return result_peg_1646;
    out_peg_1647:
    Result result_peg_1648(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_1648.getPosition()))){
            result_peg_1648.nextPosition();
        } else {
            goto out_peg_1649;
        }
    }
    result_peg_1648.setValue((void*) "!=");
    
    if (column_peg_1643.chunk8 == 0){
        column_peg_1643.chunk8 = new Chunk8();
    }
    column_peg_1643.chunk8->chunk_all_compare = result_peg_1648;
    stream.update(result_peg_1648.getPosition());
    
    
    return result_peg_1648;
    out_peg_1649:
    
    if (column_peg_1643.chunk8 == 0){
        column_peg_1643.chunk8 = new Chunk8();
    }
    column_peg_1643.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1733(stream, "expr10_rest");
    int myposition = position;
    tail_peg_1653:
    Value right;
    Value new_left;
    Result result_peg_1652(myposition);
    {
    
        {
                
                result_peg_1652.reset();
                do{
                    Result result_peg_1665(result_peg_1652.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1665.getPosition()))){
                                result_peg_1665.nextPosition();
                            } else {
                                goto out_peg_1668;
                            }
                        }
                        result_peg_1665.setValue((void*) " ");
                            
                    }
                    goto success_peg_1666;
                    out_peg_1668:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1665.getPosition()))){
                                result_peg_1665.nextPosition();
                            } else {
                                goto out_peg_1670;
                            }
                        }
                        result_peg_1665.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1666;
                    out_peg_1670:
                    goto loop_peg_1664;
                    success_peg_1666:
                    ;
                    result_peg_1652.addResult(result_peg_1665);
                } while (true);
                loop_peg_1664:
                ;
                        
            }
            goto success_peg_1655;
            goto out_peg_1671;
            success_peg_1655:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_1652.getPosition()))){
                    result_peg_1652.nextPosition();
                } else {
                    goto out_peg_1671;
                }
            }
            result_peg_1652.setValue((void*) "+");
        
        
        
        {
                
                result_peg_1652.reset();
                do{
                    Result result_peg_1684(result_peg_1652.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1684.getPosition()))){
                                result_peg_1684.nextPosition();
                            } else {
                                goto out_peg_1687;
                            }
                        }
                        result_peg_1684.setValue((void*) " ");
                            
                    }
                    goto success_peg_1685;
                    out_peg_1687:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1684.getPosition()))){
                                result_peg_1684.nextPosition();
                            } else {
                                goto out_peg_1689;
                            }
                        }
                        result_peg_1684.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1685;
                    out_peg_1689:
                    goto loop_peg_1683;
                    success_peg_1685:
                    ;
                    result_peg_1652.addResult(result_peg_1684);
                } while (true);
                loop_peg_1683:
                ;
                        
            }
            goto success_peg_1674;
            goto out_peg_1671;
            success_peg_1674:
            ;
        
        
        
        result_peg_1652 = rule_expr11(stream, result_peg_1652.getPosition());
            if (result_peg_1652.error()){
                goto out_peg_1671;
            }
            right = result_peg_1652.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_1652.setValue(value);
            }
            new_left = result_peg_1652.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1652.getPosition();
    goto tail_peg_1653;
    out_peg_1671:
    Result result_peg_1692(myposition);
    {
    
        {
                
                result_peg_1692.reset();
                do{
                    Result result_peg_1704(result_peg_1692.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1704.getPosition()))){
                                result_peg_1704.nextPosition();
                            } else {
                                goto out_peg_1707;
                            }
                        }
                        result_peg_1704.setValue((void*) " ");
                            
                    }
                    goto success_peg_1705;
                    out_peg_1707:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1704.getPosition()))){
                                result_peg_1704.nextPosition();
                            } else {
                                goto out_peg_1709;
                            }
                        }
                        result_peg_1704.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1705;
                    out_peg_1709:
                    goto loop_peg_1703;
                    success_peg_1705:
                    ;
                    result_peg_1692.addResult(result_peg_1704);
                } while (true);
                loop_peg_1703:
                ;
                        
            }
            goto success_peg_1694;
            goto out_peg_1710;
            success_peg_1694:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1692.getPosition()))){
                    result_peg_1692.nextPosition();
                } else {
                    goto out_peg_1710;
                }
            }
            result_peg_1692.setValue((void*) "-");
        
        
        
        {
                
                result_peg_1692.reset();
                do{
                    Result result_peg_1723(result_peg_1692.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1723.getPosition()))){
                                result_peg_1723.nextPosition();
                            } else {
                                goto out_peg_1726;
                            }
                        }
                        result_peg_1723.setValue((void*) " ");
                            
                    }
                    goto success_peg_1724;
                    out_peg_1726:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1723.getPosition()))){
                                result_peg_1723.nextPosition();
                            } else {
                                goto out_peg_1728;
                            }
                        }
                        result_peg_1723.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1724;
                    out_peg_1728:
                    goto loop_peg_1722;
                    success_peg_1724:
                    ;
                    result_peg_1692.addResult(result_peg_1723);
                } while (true);
                loop_peg_1722:
                ;
                        
            }
            goto success_peg_1713;
            goto out_peg_1710;
            success_peg_1713:
            ;
        
        
        
        result_peg_1692 = rule_expr11(stream, result_peg_1692.getPosition());
            if (result_peg_1692.error()){
                goto out_peg_1710;
            }
            right = result_peg_1692.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_1692.setValue(value);
            }
            new_left = result_peg_1692.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1692.getPosition();
    goto tail_peg_1653;
    out_peg_1710:
    Result result_peg_1731(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1731.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1731.getPosition());
    
    return result_peg_1731;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1855(stream, "expr11_rest");
    int myposition = position;
    tail_peg_1736:
    Value right;
    Value new_left;
    Result result_peg_1735(myposition);
    {
    
        {
                
                result_peg_1735.reset();
                do{
                    Result result_peg_1748(result_peg_1735.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1748.getPosition()))){
                                result_peg_1748.nextPosition();
                            } else {
                                goto out_peg_1751;
                            }
                        }
                        result_peg_1748.setValue((void*) " ");
                            
                    }
                    goto success_peg_1749;
                    out_peg_1751:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1748.getPosition()))){
                                result_peg_1748.nextPosition();
                            } else {
                                goto out_peg_1753;
                            }
                        }
                        result_peg_1748.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1749;
                    out_peg_1753:
                    goto loop_peg_1747;
                    success_peg_1749:
                    ;
                    result_peg_1735.addResult(result_peg_1748);
                } while (true);
                loop_peg_1747:
                ;
                        
            }
            goto success_peg_1738;
            goto out_peg_1754;
            success_peg_1738:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_1735.getPosition()))){
                    result_peg_1735.nextPosition();
                } else {
                    goto out_peg_1754;
                }
            }
            result_peg_1735.setValue((void*) "*");
        
        
        
        {
                
                result_peg_1735.reset();
                do{
                    Result result_peg_1767(result_peg_1735.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1767.getPosition()))){
                                result_peg_1767.nextPosition();
                            } else {
                                goto out_peg_1770;
                            }
                        }
                        result_peg_1767.setValue((void*) " ");
                            
                    }
                    goto success_peg_1768;
                    out_peg_1770:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1767.getPosition()))){
                                result_peg_1767.nextPosition();
                            } else {
                                goto out_peg_1772;
                            }
                        }
                        result_peg_1767.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1768;
                    out_peg_1772:
                    goto loop_peg_1766;
                    success_peg_1768:
                    ;
                    result_peg_1735.addResult(result_peg_1767);
                } while (true);
                loop_peg_1766:
                ;
                        
            }
            goto success_peg_1757;
            goto out_peg_1754;
            success_peg_1757:
            ;
        
        
        
        result_peg_1735 = rule_expr12(stream, result_peg_1735.getPosition());
            if (result_peg_1735.error()){
                goto out_peg_1754;
            }
            right = result_peg_1735.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_1735.setValue(value);
            }
            new_left = result_peg_1735.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1735.getPosition();
    goto tail_peg_1736;
    out_peg_1754:
    Result result_peg_1775(myposition);
    {
    
        {
                
                result_peg_1775.reset();
                do{
                    Result result_peg_1787(result_peg_1775.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1787.getPosition()))){
                                result_peg_1787.nextPosition();
                            } else {
                                goto out_peg_1790;
                            }
                        }
                        result_peg_1787.setValue((void*) " ");
                            
                    }
                    goto success_peg_1788;
                    out_peg_1790:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1787.getPosition()))){
                                result_peg_1787.nextPosition();
                            } else {
                                goto out_peg_1792;
                            }
                        }
                        result_peg_1787.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1788;
                    out_peg_1792:
                    goto loop_peg_1786;
                    success_peg_1788:
                    ;
                    result_peg_1775.addResult(result_peg_1787);
                } while (true);
                loop_peg_1786:
                ;
                        
            }
            goto success_peg_1777;
            goto out_peg_1793;
            success_peg_1777:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1775.getPosition()))){
                    result_peg_1775.nextPosition();
                } else {
                    goto out_peg_1793;
                }
            }
            result_peg_1775.setValue((void*) "/");
        
        
        
        {
                
                result_peg_1775.reset();
                do{
                    Result result_peg_1806(result_peg_1775.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1806.getPosition()))){
                                result_peg_1806.nextPosition();
                            } else {
                                goto out_peg_1809;
                            }
                        }
                        result_peg_1806.setValue((void*) " ");
                            
                    }
                    goto success_peg_1807;
                    out_peg_1809:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1806.getPosition()))){
                                result_peg_1806.nextPosition();
                            } else {
                                goto out_peg_1811;
                            }
                        }
                        result_peg_1806.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1807;
                    out_peg_1811:
                    goto loop_peg_1805;
                    success_peg_1807:
                    ;
                    result_peg_1775.addResult(result_peg_1806);
                } while (true);
                loop_peg_1805:
                ;
                        
            }
            goto success_peg_1796;
            goto out_peg_1793;
            success_peg_1796:
            ;
        
        
        
        result_peg_1775 = rule_expr12(stream, result_peg_1775.getPosition());
            if (result_peg_1775.error()){
                goto out_peg_1793;
            }
            right = result_peg_1775.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_1775.setValue(value);
            }
            new_left = result_peg_1775.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1775.getPosition();
    goto tail_peg_1736;
    out_peg_1793:
    Result result_peg_1814(myposition);
    {
    
        {
                
                result_peg_1814.reset();
                do{
                    Result result_peg_1826(result_peg_1814.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1826.getPosition()))){
                                result_peg_1826.nextPosition();
                            } else {
                                goto out_peg_1829;
                            }
                        }
                        result_peg_1826.setValue((void*) " ");
                            
                    }
                    goto success_peg_1827;
                    out_peg_1829:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1826.getPosition()))){
                                result_peg_1826.nextPosition();
                            } else {
                                goto out_peg_1831;
                            }
                        }
                        result_peg_1826.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1827;
                    out_peg_1831:
                    goto loop_peg_1825;
                    success_peg_1827:
                    ;
                    result_peg_1814.addResult(result_peg_1826);
                } while (true);
                loop_peg_1825:
                ;
                        
            }
            goto success_peg_1816;
            goto out_peg_1832;
            success_peg_1816:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_1814.getPosition()))){
                    result_peg_1814.nextPosition();
                } else {
                    goto out_peg_1832;
                }
            }
            result_peg_1814.setValue((void*) "%");
        
        
        
        {
                
                result_peg_1814.reset();
                do{
                    Result result_peg_1845(result_peg_1814.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1845.getPosition()))){
                                result_peg_1845.nextPosition();
                            } else {
                                goto out_peg_1848;
                            }
                        }
                        result_peg_1845.setValue((void*) " ");
                            
                    }
                    goto success_peg_1846;
                    out_peg_1848:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1845.getPosition()))){
                                result_peg_1845.nextPosition();
                            } else {
                                goto out_peg_1850;
                            }
                        }
                        result_peg_1845.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1846;
                    out_peg_1850:
                    goto loop_peg_1844;
                    success_peg_1846:
                    ;
                    result_peg_1814.addResult(result_peg_1845);
                } while (true);
                loop_peg_1844:
                ;
                        
            }
            goto success_peg_1835;
            goto out_peg_1832;
            success_peg_1835:
            ;
        
        
        
        result_peg_1814 = rule_expr12(stream, result_peg_1814.getPosition());
            if (result_peg_1814.error()){
                goto out_peg_1832;
            }
            right = result_peg_1814.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_1814.setValue(value);
            }
            new_left = result_peg_1814.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1814.getPosition();
    goto tail_peg_1736;
    out_peg_1832:
    Result result_peg_1853(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1853.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1853.getPosition());
    
    return result_peg_1853;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1899(stream, "expr12_rest");
    int myposition = position;
    tail_peg_1858:
    Value right;
    Value new_left;
    Result result_peg_1857(myposition);
    {
    
        {
                
                result_peg_1857.reset();
                do{
                    Result result_peg_1870(result_peg_1857.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1870.getPosition()))){
                                result_peg_1870.nextPosition();
                            } else {
                                goto out_peg_1873;
                            }
                        }
                        result_peg_1870.setValue((void*) " ");
                            
                    }
                    goto success_peg_1871;
                    out_peg_1873:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1870.getPosition()))){
                                result_peg_1870.nextPosition();
                            } else {
                                goto out_peg_1875;
                            }
                        }
                        result_peg_1870.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1871;
                    out_peg_1875:
                    goto loop_peg_1869;
                    success_peg_1871:
                    ;
                    result_peg_1857.addResult(result_peg_1870);
                } while (true);
                loop_peg_1869:
                ;
                        
            }
            goto success_peg_1860;
            goto out_peg_1876;
            success_peg_1860:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_1857.getPosition()))){
                    result_peg_1857.nextPosition();
                } else {
                    goto out_peg_1876;
                }
            }
            result_peg_1857.setValue((void*) "**");
        
        
        
        {
                
                result_peg_1857.reset();
                do{
                    Result result_peg_1889(result_peg_1857.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1889.getPosition()))){
                                result_peg_1889.nextPosition();
                            } else {
                                goto out_peg_1892;
                            }
                        }
                        result_peg_1889.setValue((void*) " ");
                            
                    }
                    goto success_peg_1890;
                    out_peg_1892:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1889.getPosition()))){
                                result_peg_1889.nextPosition();
                            } else {
                                goto out_peg_1894;
                            }
                        }
                        result_peg_1889.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1890;
                    out_peg_1894:
                    goto loop_peg_1888;
                    success_peg_1890:
                    ;
                    result_peg_1857.addResult(result_peg_1889);
                } while (true);
                loop_peg_1888:
                ;
                        
            }
            goto success_peg_1879;
            goto out_peg_1876;
            success_peg_1879:
            ;
        
        
        
        result_peg_1857 = rule_expr13(stream, result_peg_1857.getPosition());
            if (result_peg_1857.error()){
                goto out_peg_1876;
            }
            right = result_peg_1857.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_1857.setValue(value);
            }
            new_left = result_peg_1857.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1857.getPosition();
    goto tail_peg_1858;
    out_peg_1876:
    Result result_peg_1897(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1897.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1897.getPosition());
    
    return result_peg_1897;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_1900 = stream.getColumn(position);
    if (column_peg_1900.chunk9 != 0 && column_peg_1900.chunk9->chunk_unary.calculated()){
        return column_peg_1900.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_1929(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_1901(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_1901.getPosition()))){
                    result_peg_1901.nextPosition();
                } else {
                    goto out_peg_1903;
                }
            }
            result_peg_1901.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_1901.setValue(value);
            }
        
        
    }
    
    if (column_peg_1900.chunk9 == 0){
        column_peg_1900.chunk9 = new Chunk9();
    }
    column_peg_1900.chunk9->chunk_unary = result_peg_1901;
    stream.update(result_peg_1901.getPosition());
    
    
    return result_peg_1901;
    out_peg_1903:
    Result result_peg_1904(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1904.getPosition()))){
                    result_peg_1904.nextPosition();
                } else {
                    goto out_peg_1906;
                }
            }
            result_peg_1904.setValue((void*) "-");
        
        
        
        Result result_peg_1909(result_peg_1904);
            {
                
                char letter_peg_1914 = stream.get(result_peg_1909.getPosition());
                if (letter_peg_1914 != '\0' && strchr("0123456789", letter_peg_1914) != NULL){
                    result_peg_1909.nextPosition();
                    result_peg_1909.setValue((void*) (long) letter_peg_1914);
                } else {
                    goto out_peg_1913;
                }
                
            }
            goto success_peg_1910;
            out_peg_1913:
            goto not_peg_1908;
            success_peg_1910:
            ;
            goto out_peg_1906;
            not_peg_1908:
            result_peg_1904.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_1904.setValue(value);
            }
        
        
    }
    
    if (column_peg_1900.chunk9 == 0){
        column_peg_1900.chunk9 = new Chunk9();
    }
    column_peg_1900.chunk9->chunk_unary = result_peg_1904;
    stream.update(result_peg_1904.getPosition());
    
    
    return result_peg_1904;
    out_peg_1906:
    Result result_peg_1915(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1915.getPosition()))){
                    result_peg_1915.nextPosition();
                } else {
                    goto out_peg_1917;
                }
            }
            result_peg_1915.setValue((void*) "~");
        
        
        
        Result result_peg_1920(result_peg_1915);
            result_peg_1920 = rule_key(stream, result_peg_1920.getPosition());
            if (result_peg_1920.error()){
                goto not_peg_1919;
            }
            goto out_peg_1917;
            not_peg_1919:
            result_peg_1915.setValue((void*)0);
        
        
        
        Result result_peg_1923(result_peg_1915);
            {
                
                char letter_peg_1928 = stream.get(result_peg_1923.getPosition());
                if (letter_peg_1928 != '\0' && strchr("0123456789", letter_peg_1928) != NULL){
                    result_peg_1923.nextPosition();
                    result_peg_1923.setValue((void*) (long) letter_peg_1928);
                } else {
                    goto out_peg_1927;
                }
                
            }
            goto success_peg_1924;
            out_peg_1927:
            goto not_peg_1922;
            success_peg_1924:
            ;
            goto out_peg_1917;
            not_peg_1922:
            result_peg_1915.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_1915.setValue(value);
            }
        
        
    }
    
    if (column_peg_1900.chunk9 == 0){
        column_peg_1900.chunk9 = new Chunk9();
    }
    column_peg_1900.chunk9->chunk_unary = result_peg_1915;
    stream.update(result_peg_1915.getPosition());
    
    
    return result_peg_1915;
    out_peg_1917:
    
    if (column_peg_1900.chunk9 == 0){
        column_peg_1900.chunk9 = new Chunk9();
    }
    column_peg_1900.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_1930 = stream.getColumn(position);
    if (column_peg_1930.chunk9 != 0 && column_peg_1930.chunk9->chunk_expr13_real.calculated()){
        return column_peg_1930.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_1976(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_1931(myposition);
    
    result_peg_1931 = rule_helper(stream, result_peg_1931.getPosition());
    if (result_peg_1931.error()){
        goto out_peg_1932;
    }
    
    if (column_peg_1930.chunk9 == 0){
        column_peg_1930.chunk9 = new Chunk9();
    }
    column_peg_1930.chunk9->chunk_expr13_real = result_peg_1931;
    stream.update(result_peg_1931.getPosition());
    
    
    return result_peg_1931;
    out_peg_1932:
    Result result_peg_1933(myposition);
    
    result_peg_1933 = rule_function(stream, result_peg_1933.getPosition());
    if (result_peg_1933.error()){
        goto out_peg_1934;
    }
    
    if (column_peg_1930.chunk9 == 0){
        column_peg_1930.chunk9 = new Chunk9();
    }
    column_peg_1930.chunk9->chunk_expr13_real = result_peg_1933;
    stream.update(result_peg_1933.getPosition());
    
    
    return result_peg_1933;
    out_peg_1934:
    Result result_peg_1935(myposition);
    
    result_peg_1935 = rule_value(stream, result_peg_1935.getPosition());
    if (result_peg_1935.error()){
        goto out_peg_1936;
    }
    
    if (column_peg_1930.chunk9 == 0){
        column_peg_1930.chunk9 = new Chunk9();
    }
    column_peg_1930.chunk9->chunk_expr13_real = result_peg_1935;
    stream.update(result_peg_1935.getPosition());
    
    
    return result_peg_1935;
    out_peg_1936:
    Result result_peg_1937(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1937.getPosition()))){
                    result_peg_1937.nextPosition();
                } else {
                    goto out_peg_1939;
                }
            }
            result_peg_1937.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1937.reset();
                do{
                    Result result_peg_1951(result_peg_1937.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1951.getPosition()))){
                                result_peg_1951.nextPosition();
                            } else {
                                goto out_peg_1954;
                            }
                        }
                        result_peg_1951.setValue((void*) " ");
                            
                    }
                    goto success_peg_1952;
                    out_peg_1954:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1951.getPosition()))){
                                result_peg_1951.nextPosition();
                            } else {
                                goto out_peg_1956;
                            }
                        }
                        result_peg_1951.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1952;
                    out_peg_1956:
                    goto loop_peg_1950;
                    success_peg_1952:
                    ;
                    result_peg_1937.addResult(result_peg_1951);
                } while (true);
                loop_peg_1950:
                ;
                        
            }
            goto success_peg_1941;
            goto out_peg_1939;
            success_peg_1941:
            ;
        
        
        
        result_peg_1937 = rule_expr(stream, result_peg_1937.getPosition());
            if (result_peg_1937.error()){
                goto out_peg_1939;
            }
            e = result_peg_1937.getValues();
        
        
        
        {
                
                result_peg_1937.reset();
                do{
                    Result result_peg_1969(result_peg_1937.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1969.getPosition()))){
                                result_peg_1969.nextPosition();
                            } else {
                                goto out_peg_1972;
                            }
                        }
                        result_peg_1969.setValue((void*) " ");
                            
                    }
                    goto success_peg_1970;
                    out_peg_1972:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1969.getPosition()))){
                                result_peg_1969.nextPosition();
                            } else {
                                goto out_peg_1974;
                            }
                        }
                        result_peg_1969.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1970;
                    out_peg_1974:
                    goto loop_peg_1968;
                    success_peg_1970:
                    ;
                    result_peg_1937.addResult(result_peg_1969);
                } while (true);
                loop_peg_1968:
                ;
                        
            }
            goto success_peg_1959;
            goto out_peg_1939;
            success_peg_1959:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1937.getPosition()))){
                    result_peg_1937.nextPosition();
                } else {
                    goto out_peg_1939;
                }
            }
            result_peg_1937.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_1937.setValue(value);
            }
        
        
    }
    
    if (column_peg_1930.chunk9 == 0){
        column_peg_1930.chunk9 = new Chunk9();
    }
    column_peg_1930.chunk9->chunk_expr13_real = result_peg_1937;
    stream.update(result_peg_1937.getPosition());
    
    
    return result_peg_1937;
    out_peg_1939:
    
    if (column_peg_1930.chunk9 == 0){
        column_peg_1930.chunk9 = new Chunk9();
    }
    column_peg_1930.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_1977 = stream.getColumn(position);
    if (column_peg_1977.chunk9 != 0 && column_peg_1977.chunk9->chunk_function.calculated()){
        return column_peg_1977.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2193(stream, "function");
    int myposition = position;
    
    Value name;
    Value args;
    Value arg1;
    Result result_peg_1978(myposition);
    
    {
    
        result_peg_1978 = rule_function_name(stream, result_peg_1978.getPosition());
            if (result_peg_1978.error()){
                goto out_peg_1980;
            }
            name = result_peg_1978.getValues();
        
        
        
        {
                
                result_peg_1978.reset();
                do{
                    Result result_peg_1992(result_peg_1978.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1992.getPosition()))){
                                result_peg_1992.nextPosition();
                            } else {
                                goto out_peg_1995;
                            }
                        }
                        result_peg_1992.setValue((void*) " ");
                            
                    }
                    goto success_peg_1993;
                    out_peg_1995:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1992.getPosition()))){
                                result_peg_1992.nextPosition();
                            } else {
                                goto out_peg_1997;
                            }
                        }
                        result_peg_1992.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1993;
                    out_peg_1997:
                    goto loop_peg_1991;
                    success_peg_1993:
                    ;
                    result_peg_1978.addResult(result_peg_1992);
                } while (true);
                loop_peg_1991:
                ;
                        
            }
            goto success_peg_1982;
            goto out_peg_1980;
            success_peg_1982:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1978.getPosition()))){
                    result_peg_1978.nextPosition();
                } else {
                    goto out_peg_1980;
                }
            }
            result_peg_1978.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1978.reset();
                do{
                    Result result_peg_2010(result_peg_1978.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2010.getPosition()))){
                                result_peg_2010.nextPosition();
                            } else {
                                goto out_peg_2013;
                            }
                        }
                        result_peg_2010.setValue((void*) " ");
                            
                    }
                    goto success_peg_2011;
                    out_peg_2013:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2010.getPosition()))){
                                result_peg_2010.nextPosition();
                            } else {
                                goto out_peg_2015;
                            }
                        }
                        result_peg_2010.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2011;
                    out_peg_2015:
                    goto loop_peg_2009;
                    success_peg_2011:
                    ;
                    result_peg_1978.addResult(result_peg_2010);
                } while (true);
                loop_peg_2009:
                ;
                        
            }
            goto success_peg_2000;
            goto out_peg_1980;
            success_peg_2000:
            ;
        
        
        
        result_peg_1978 = rule_args(stream, result_peg_1978.getPosition());
            if (result_peg_1978.error()){
                goto out_peg_1980;
            }
            args = result_peg_1978.getValues();
        
        
        
        {
                
                result_peg_1978.reset();
                do{
                    Result result_peg_2028(result_peg_1978.getPosition());
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
                    result_peg_1978.addResult(result_peg_2028);
                } while (true);
                loop_peg_2027:
                ;
                        
            }
            goto success_peg_2018;
            goto out_peg_1980;
            success_peg_2018:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1978.getPosition()))){
                    result_peg_1978.nextPosition();
                } else {
                    goto out_peg_1980;
                }
            }
            result_peg_1978.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, args);
                result_peg_1978.setValue(value);
            }
        
        
    }
    
    if (column_peg_1977.chunk9 == 0){
        column_peg_1977.chunk9 = new Chunk9();
    }
    column_peg_1977.chunk9->chunk_function = result_peg_1978;
    stream.update(result_peg_1978.getPosition());
    
    
    return result_peg_1978;
    out_peg_1980:
    Result result_peg_2035(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("teammode"[i], stream.get(result_peg_2035.getPosition()))){
                    result_peg_2035.nextPosition();
                } else {
                    goto out_peg_2037;
                }
            }
            result_peg_2035.setValue((void*) "teammode");
            name = result_peg_2035.getValues();
        
        
        
        {
                
                result_peg_2035.reset();
                do{
                    Result result_peg_2049(result_peg_2035.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2049.getPosition()))){
                                result_peg_2049.nextPosition();
                            } else {
                                goto out_peg_2052;
                            }
                        }
                        result_peg_2049.setValue((void*) " ");
                            
                    }
                    goto success_peg_2050;
                    out_peg_2052:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2049.getPosition()))){
                                result_peg_2049.nextPosition();
                            } else {
                                goto out_peg_2054;
                            }
                        }
                        result_peg_2049.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2050;
                    out_peg_2054:
                    goto loop_peg_2048;
                    success_peg_2050:
                    ;
                    result_peg_2035.addResult(result_peg_2049);
                } while (true);
                loop_peg_2048:
                ;
                        
            }
            goto success_peg_2039;
            goto out_peg_2037;
            success_peg_2039:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2035.getPosition()))){
                    result_peg_2035.nextPosition();
                } else {
                    goto out_peg_2037;
                }
            }
            result_peg_2035.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2035.reset();
                do{
                    Result result_peg_2067(result_peg_2035.getPosition());
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
                    result_peg_2035.addResult(result_peg_2067);
                } while (true);
                loop_peg_2066:
                ;
                        
            }
            goto success_peg_2057;
            goto out_peg_2037;
            success_peg_2057:
            ;
        
        
        
        result_peg_2035 = rule_identifier(stream, result_peg_2035.getPosition());
            if (result_peg_2035.error()){
                goto out_peg_2037;
            }
            arg1 = result_peg_2035.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction1(name, arg1);
                result_peg_2035.setValue(value);
            }
        
        
    }
    
    if (column_peg_1977.chunk9 == 0){
        column_peg_1977.chunk9 = new Chunk9();
    }
    column_peg_1977.chunk9->chunk_function = result_peg_2035;
    stream.update(result_peg_2035.getPosition());
    
    
    return result_peg_2035;
    out_peg_2037:
    Result result_peg_2074(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("teammode"[i], stream.get(result_peg_2074.getPosition()))){
                    result_peg_2074.nextPosition();
                } else {
                    goto out_peg_2076;
                }
            }
            result_peg_2074.setValue((void*) "teammode");
            name = result_peg_2074.getValues();
        
        
        
        {
                
                result_peg_2074.reset();
                do{
                    Result result_peg_2088(result_peg_2074.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2088.getPosition()))){
                                result_peg_2088.nextPosition();
                            } else {
                                goto out_peg_2091;
                            }
                        }
                        result_peg_2088.setValue((void*) " ");
                            
                    }
                    goto success_peg_2089;
                    out_peg_2091:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2088.getPosition()))){
                                result_peg_2088.nextPosition();
                            } else {
                                goto out_peg_2093;
                            }
                        }
                        result_peg_2088.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2089;
                    out_peg_2093:
                    goto loop_peg_2087;
                    success_peg_2089:
                    ;
                    result_peg_2074.addResult(result_peg_2088);
                } while (true);
                loop_peg_2087:
                ;
                        
            }
            goto success_peg_2078;
            goto out_peg_2076;
            success_peg_2078:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_2074.getPosition()))){
                    result_peg_2074.nextPosition();
                } else {
                    goto out_peg_2076;
                }
            }
            result_peg_2074.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_2074.reset();
                do{
                    Result result_peg_2106(result_peg_2074.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2106.getPosition()))){
                                result_peg_2106.nextPosition();
                            } else {
                                goto out_peg_2109;
                            }
                        }
                        result_peg_2106.setValue((void*) " ");
                            
                    }
                    goto success_peg_2107;
                    out_peg_2109:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2106.getPosition()))){
                                result_peg_2106.nextPosition();
                            } else {
                                goto out_peg_2111;
                            }
                        }
                        result_peg_2106.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2107;
                    out_peg_2111:
                    goto loop_peg_2105;
                    success_peg_2107:
                    ;
                    result_peg_2074.addResult(result_peg_2106);
                } while (true);
                loop_peg_2105:
                ;
                        
            }
            goto success_peg_2096;
            goto out_peg_2076;
            success_peg_2096:
            ;
        
        
        
        result_peg_2074 = rule_identifier(stream, result_peg_2074.getPosition());
            if (result_peg_2074.error()){
                goto out_peg_2076;
            }
            arg1 = result_peg_2074.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction1("teammode!=", arg1);
                result_peg_2074.setValue(value);
            }
        
        
    }
    
    if (column_peg_1977.chunk9 == 0){
        column_peg_1977.chunk9 = new Chunk9();
    }
    column_peg_1977.chunk9->chunk_function = result_peg_2074;
    stream.update(result_peg_2074.getPosition());
    
    
    return result_peg_2074;
    out_peg_2076:
    Result result_peg_2113(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2113.getPosition()))){
                    result_peg_2113.nextPosition();
                } else {
                    goto out_peg_2115;
                }
            }
            result_peg_2113.setValue((void*) "animelem");
            name = result_peg_2113.getValues();
        
        
        
        {
                
                result_peg_2113.reset();
                do{
                    Result result_peg_2127(result_peg_2113.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2127.getPosition()))){
                                result_peg_2127.nextPosition();
                            } else {
                                goto out_peg_2130;
                            }
                        }
                        result_peg_2127.setValue((void*) " ");
                            
                    }
                    goto success_peg_2128;
                    out_peg_2130:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2127.getPosition()))){
                                result_peg_2127.nextPosition();
                            } else {
                                goto out_peg_2132;
                            }
                        }
                        result_peg_2127.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2128;
                    out_peg_2132:
                    goto loop_peg_2126;
                    success_peg_2128:
                    ;
                    result_peg_2113.addResult(result_peg_2127);
                } while (true);
                loop_peg_2126:
                ;
                        
            }
            goto success_peg_2117;
            goto out_peg_2115;
            success_peg_2117:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2113.getPosition()))){
                    result_peg_2113.nextPosition();
                } else {
                    goto out_peg_2115;
                }
            }
            result_peg_2113.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2113.reset();
                do{
                    Result result_peg_2145(result_peg_2113.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2145.getPosition()))){
                                result_peg_2145.nextPosition();
                            } else {
                                goto out_peg_2148;
                            }
                        }
                        result_peg_2145.setValue((void*) " ");
                            
                    }
                    goto success_peg_2146;
                    out_peg_2148:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2145.getPosition()))){
                                result_peg_2145.nextPosition();
                            } else {
                                goto out_peg_2150;
                            }
                        }
                        result_peg_2145.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2146;
                    out_peg_2150:
                    goto loop_peg_2144;
                    success_peg_2146:
                    ;
                    result_peg_2113.addResult(result_peg_2145);
                } while (true);
                loop_peg_2144:
                ;
                        
            }
            goto success_peg_2135;
            goto out_peg_2115;
            success_peg_2135:
            ;
        
        
        
        result_peg_2113 = rule_integer(stream, result_peg_2113.getPosition());
            if (result_peg_2113.error()){
                goto out_peg_2115;
            }
            arg1 = result_peg_2113.getValues();
        
        
        
        result_peg_2113 = rule_function_rest(stream, result_peg_2113.getPosition());
            if (result_peg_2113.error()){
                goto out_peg_2115;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1(name, arg1);
                result_peg_2113.setValue(value);
            }
        
        
    }
    
    if (column_peg_1977.chunk9 == 0){
        column_peg_1977.chunk9 = new Chunk9();
    }
    column_peg_1977.chunk9->chunk_function = result_peg_2113;
    stream.update(result_peg_2113.getPosition());
    
    
    return result_peg_2113;
    out_peg_2115:
    Result result_peg_2153(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2153.getPosition()))){
                    result_peg_2153.nextPosition();
                } else {
                    goto out_peg_2155;
                }
            }
            result_peg_2153.setValue((void*) "animelem");
            name = result_peg_2153.getValues();
        
        
        
        {
                
                result_peg_2153.reset();
                do{
                    Result result_peg_2167(result_peg_2153.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2167.getPosition()))){
                                result_peg_2167.nextPosition();
                            } else {
                                goto out_peg_2170;
                            }
                        }
                        result_peg_2167.setValue((void*) " ");
                            
                    }
                    goto success_peg_2168;
                    out_peg_2170:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2167.getPosition()))){
                                result_peg_2167.nextPosition();
                            } else {
                                goto out_peg_2172;
                            }
                        }
                        result_peg_2167.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2168;
                    out_peg_2172:
                    goto loop_peg_2166;
                    success_peg_2168:
                    ;
                    result_peg_2153.addResult(result_peg_2167);
                } while (true);
                loop_peg_2166:
                ;
                        
            }
            goto success_peg_2157;
            goto out_peg_2155;
            success_peg_2157:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_2153.getPosition()))){
                    result_peg_2153.nextPosition();
                } else {
                    goto out_peg_2155;
                }
            }
            result_peg_2153.setValue((void*) "<=");
        
        
        
        {
                
                result_peg_2153.reset();
                do{
                    Result result_peg_2185(result_peg_2153.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2185.getPosition()))){
                                result_peg_2185.nextPosition();
                            } else {
                                goto out_peg_2188;
                            }
                        }
                        result_peg_2185.setValue((void*) " ");
                            
                    }
                    goto success_peg_2186;
                    out_peg_2188:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2185.getPosition()))){
                                result_peg_2185.nextPosition();
                            } else {
                                goto out_peg_2190;
                            }
                        }
                        result_peg_2185.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2186;
                    out_peg_2190:
                    goto loop_peg_2184;
                    success_peg_2186:
                    ;
                    result_peg_2153.addResult(result_peg_2185);
                } while (true);
                loop_peg_2184:
                ;
                        
            }
            goto success_peg_2175;
            goto out_peg_2155;
            success_peg_2175:
            ;
        
        
        
        result_peg_2153 = rule_integer(stream, result_peg_2153.getPosition());
            if (result_peg_2153.error()){
                goto out_peg_2155;
            }
            arg1 = result_peg_2153.getValues();
        
        
        
        result_peg_2153 = rule_function_rest(stream, result_peg_2153.getPosition());
            if (result_peg_2153.error()){
                goto out_peg_2155;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1("animelem<=", arg1);
                result_peg_2153.setValue(value);
            }
        
        
    }
    
    if (column_peg_1977.chunk9 == 0){
        column_peg_1977.chunk9 = new Chunk9();
    }
    column_peg_1977.chunk9->chunk_function = result_peg_2153;
    stream.update(result_peg_2153.getPosition());
    
    
    return result_peg_2153;
    out_peg_2155:
    
    if (column_peg_1977.chunk9 == 0){
        column_peg_1977.chunk9 = new Chunk9();
    }
    column_peg_1977.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2194 = stream.getColumn(position);
    if (column_peg_2194.chunk9 != 0 && column_peg_2194.chunk9->chunk_args.calculated()){
        return column_peg_2194.chunk9->chunk_args;
    }
    
    RuleTrace trace_peg_2236(stream, "args");
    int myposition = position;
    
    Value expr1;
    Value expr_rest;
    Result result_peg_2195(myposition);
    
    {
    
        result_peg_2195 = rule_expr_c(stream, result_peg_2195.getPosition());
            if (result_peg_2195.error()){
                goto out_peg_2197;
            }
            expr1 = result_peg_2195.getValues();
        
        
        
        result_peg_2195.reset();
            do{
                Result result_peg_2200(result_peg_2195.getPosition());
                {
                
                    {
                            
                            result_peg_2200.reset();
                            do{
                                Result result_peg_2212(result_peg_2200.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2212.getPosition()))){
                                            result_peg_2212.nextPosition();
                                        } else {
                                            goto out_peg_2215;
                                        }
                                    }
                                    result_peg_2212.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2213;
                                out_peg_2215:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2212.getPosition()))){
                                            result_peg_2212.nextPosition();
                                        } else {
                                            goto out_peg_2217;
                                        }
                                    }
                                    result_peg_2212.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2213;
                                out_peg_2217:
                                goto loop_peg_2211;
                                success_peg_2213:
                                ;
                                result_peg_2200.addResult(result_peg_2212);
                            } while (true);
                            loop_peg_2211:
                            ;
                                    
                        }
                        goto success_peg_2202;
                        goto loop_peg_2199;
                        success_peg_2202:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2200.getPosition()))){
                                result_peg_2200.nextPosition();
                            } else {
                                goto loop_peg_2199;
                            }
                        }
                        result_peg_2200.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2200.reset();
                            do{
                                Result result_peg_2230(result_peg_2200.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2230.getPosition()))){
                                            result_peg_2230.nextPosition();
                                        } else {
                                            goto out_peg_2233;
                                        }
                                    }
                                    result_peg_2230.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2231;
                                out_peg_2233:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2230.getPosition()))){
                                            result_peg_2230.nextPosition();
                                        } else {
                                            goto out_peg_2235;
                                        }
                                    }
                                    result_peg_2230.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2231;
                                out_peg_2235:
                                goto loop_peg_2229;
                                success_peg_2231:
                                ;
                                result_peg_2200.addResult(result_peg_2230);
                            } while (true);
                            loop_peg_2229:
                            ;
                                    
                        }
                        goto success_peg_2220;
                        goto loop_peg_2199;
                        success_peg_2220:
                        ;
                    
                    
                    
                    result_peg_2200 = rule_expr_c(stream, result_peg_2200.getPosition());
                        if (result_peg_2200.error()){
                            goto loop_peg_2199;
                        }
                    
                    
                }
                result_peg_2195.addResult(result_peg_2200);
            } while (true);
            loop_peg_2199:
            ;
            expr_rest = result_peg_2195.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(expr1, expr_rest);
                result_peg_2195.setValue(value);
            }
        
        
    }
    
    if (column_peg_2194.chunk9 == 0){
        column_peg_2194.chunk9 = new Chunk9();
    }
    column_peg_2194.chunk9->chunk_args = result_peg_2195;
    stream.update(result_peg_2195.getPosition());
    
    
    return result_peg_2195;
    out_peg_2197:
    
    if (column_peg_2194.chunk9 == 0){
        column_peg_2194.chunk9 = new Chunk9();
    }
    column_peg_2194.chunk9->chunk_args = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2237 = stream.getColumn(position);
    if (column_peg_2237.chunk10 != 0 && column_peg_2237.chunk10->chunk_function_name.calculated()){
        return column_peg_2237.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_2302(stream, "function_name");
    int myposition = position;
    
    
    Result result_peg_2238(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("abs"[i], stream.get(result_peg_2238.getPosition()))){
            result_peg_2238.nextPosition();
        } else {
            goto out_peg_2239;
        }
    }
    result_peg_2238.setValue((void*) "abs");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2238;
    stream.update(result_peg_2238.getPosition());
    
    
    return result_peg_2238;
    out_peg_2239:
    Result result_peg_2240(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("const"[i], stream.get(result_peg_2240.getPosition()))){
            result_peg_2240.nextPosition();
        } else {
            goto out_peg_2241;
        }
    }
    result_peg_2240.setValue((void*) "const");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2240;
    stream.update(result_peg_2240.getPosition());
    
    
    return result_peg_2240;
    out_peg_2241:
    Result result_peg_2242(myposition);
    
    for (int i = 0; i < 13; i++){
        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2242.getPosition()))){
            result_peg_2242.nextPosition();
        } else {
            goto out_peg_2243;
        }
    }
    result_peg_2242.setValue((void*) "selfanimexist");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2242;
    stream.update(result_peg_2242.getPosition());
    
    
    return result_peg_2242;
    out_peg_2243:
    Result result_peg_2244(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("ifelse"[i], stream.get(result_peg_2244.getPosition()))){
            result_peg_2244.nextPosition();
        } else {
            goto out_peg_2245;
        }
    }
    result_peg_2244.setValue((void*) "ifelse");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2244;
    stream.update(result_peg_2244.getPosition());
    
    
    return result_peg_2244;
    out_peg_2245:
    Result result_peg_2246(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("gethitvar"[i], stream.get(result_peg_2246.getPosition()))){
            result_peg_2246.nextPosition();
        } else {
            goto out_peg_2247;
        }
    }
    result_peg_2246.setValue((void*) "gethitvar");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2246;
    stream.update(result_peg_2246.getPosition());
    
    
    return result_peg_2246;
    out_peg_2247:
    Result result_peg_2248(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("floor"[i], stream.get(result_peg_2248.getPosition()))){
            result_peg_2248.nextPosition();
        } else {
            goto out_peg_2249;
        }
    }
    result_peg_2248.setValue((void*) "floor");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2248;
    stream.update(result_peg_2248.getPosition());
    
    
    return result_peg_2248;
    out_peg_2249:
    Result result_peg_2250(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("ceil"[i], stream.get(result_peg_2250.getPosition()))){
            result_peg_2250.nextPosition();
        } else {
            goto out_peg_2251;
        }
    }
    result_peg_2250.setValue((void*) "ceil");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2250;
    stream.update(result_peg_2250.getPosition());
    
    
    return result_peg_2250;
    out_peg_2251:
    Result result_peg_2252(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("acos"[i], stream.get(result_peg_2252.getPosition()))){
            result_peg_2252.nextPosition();
        } else {
            goto out_peg_2253;
        }
    }
    result_peg_2252.setValue((void*) "acos");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2252;
    stream.update(result_peg_2252.getPosition());
    
    
    return result_peg_2252;
    out_peg_2253:
    Result result_peg_2254(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("asin"[i], stream.get(result_peg_2254.getPosition()))){
            result_peg_2254.nextPosition();
        } else {
            goto out_peg_2255;
        }
    }
    result_peg_2254.setValue((void*) "asin");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2254;
    stream.update(result_peg_2254.getPosition());
    
    
    return result_peg_2254;
    out_peg_2255:
    Result result_peg_2256(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("atan"[i], stream.get(result_peg_2256.getPosition()))){
            result_peg_2256.nextPosition();
        } else {
            goto out_peg_2257;
        }
    }
    result_peg_2256.setValue((void*) "atan");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2256;
    stream.update(result_peg_2256.getPosition());
    
    
    return result_peg_2256;
    out_peg_2257:
    Result result_peg_2258(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("tan"[i], stream.get(result_peg_2258.getPosition()))){
            result_peg_2258.nextPosition();
        } else {
            goto out_peg_2259;
        }
    }
    result_peg_2258.setValue((void*) "tan");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2258;
    stream.update(result_peg_2258.getPosition());
    
    
    return result_peg_2258;
    out_peg_2259:
    Result result_peg_2260(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("cos"[i], stream.get(result_peg_2260.getPosition()))){
            result_peg_2260.nextPosition();
        } else {
            goto out_peg_2261;
        }
    }
    result_peg_2260.setValue((void*) "cos");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2260;
    stream.update(result_peg_2260.getPosition());
    
    
    return result_peg_2260;
    out_peg_2261:
    Result result_peg_2262(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("sin"[i], stream.get(result_peg_2262.getPosition()))){
            result_peg_2262.nextPosition();
        } else {
            goto out_peg_2263;
        }
    }
    result_peg_2262.setValue((void*) "sin");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2262;
    stream.update(result_peg_2262.getPosition());
    
    
    return result_peg_2262;
    out_peg_2263:
    Result result_peg_2264(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("log"[i], stream.get(result_peg_2264.getPosition()))){
            result_peg_2264.nextPosition();
        } else {
            goto out_peg_2265;
        }
    }
    result_peg_2264.setValue((void*) "log");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2264;
    stream.update(result_peg_2264.getPosition());
    
    
    return result_peg_2264;
    out_peg_2265:
    Result result_peg_2266(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareCharCase("ln"[i], stream.get(result_peg_2266.getPosition()))){
            result_peg_2266.nextPosition();
        } else {
            goto out_peg_2267;
        }
    }
    result_peg_2266.setValue((void*) "ln");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2266;
    stream.update(result_peg_2266.getPosition());
    
    
    return result_peg_2266;
    out_peg_2267:
    Result result_peg_2268(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("sysfvar"[i], stream.get(result_peg_2268.getPosition()))){
            result_peg_2268.nextPosition();
        } else {
            goto out_peg_2269;
        }
    }
    result_peg_2268.setValue((void*) "sysfvar");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2268;
    stream.update(result_peg_2268.getPosition());
    
    
    return result_peg_2268;
    out_peg_2269:
    Result result_peg_2270(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("sysvar"[i], stream.get(result_peg_2270.getPosition()))){
            result_peg_2270.nextPosition();
        } else {
            goto out_peg_2271;
        }
    }
    result_peg_2270.setValue((void*) "sysvar");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2270;
    stream.update(result_peg_2270.getPosition());
    
    
    return result_peg_2270;
    out_peg_2271:
    Result result_peg_2272(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("var"[i], stream.get(result_peg_2272.getPosition()))){
            result_peg_2272.nextPosition();
        } else {
            goto out_peg_2273;
        }
    }
    result_peg_2272.setValue((void*) "var");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2272;
    stream.update(result_peg_2272.getPosition());
    
    
    return result_peg_2272;
    out_peg_2273:
    Result result_peg_2274(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numexplod"[i], stream.get(result_peg_2274.getPosition()))){
            result_peg_2274.nextPosition();
        } else {
            goto out_peg_2275;
        }
    }
    result_peg_2274.setValue((void*) "numexplod");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2274;
    stream.update(result_peg_2274.getPosition());
    
    
    return result_peg_2274;
    out_peg_2275:
    Result result_peg_2276(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2276.getPosition()))){
            result_peg_2276.nextPosition();
        } else {
            goto out_peg_2277;
        }
    }
    result_peg_2276.setValue((void*) "numhelper");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2276;
    stream.update(result_peg_2276.getPosition());
    
    
    return result_peg_2276;
    out_peg_2277:
    Result result_peg_2278(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numprojid"[i], stream.get(result_peg_2278.getPosition()))){
            result_peg_2278.nextPosition();
        } else {
            goto out_peg_2279;
        }
    }
    result_peg_2278.setValue((void*) "numprojid");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2278;
    stream.update(result_peg_2278.getPosition());
    
    
    return result_peg_2278;
    out_peg_2279:
    Result result_peg_2280(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("fvar"[i], stream.get(result_peg_2280.getPosition()))){
            result_peg_2280.nextPosition();
        } else {
            goto out_peg_2281;
        }
    }
    result_peg_2280.setValue((void*) "fvar");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2280;
    stream.update(result_peg_2280.getPosition());
    
    
    return result_peg_2280;
    out_peg_2281:
    Result result_peg_2282(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("ishelper"[i], stream.get(result_peg_2282.getPosition()))){
            result_peg_2282.nextPosition();
        } else {
            goto out_peg_2283;
        }
    }
    result_peg_2282.setValue((void*) "ishelper");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2282;
    stream.update(result_peg_2282.getPosition());
    
    
    return result_peg_2282;
    out_peg_2283:
    Result result_peg_2284(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numtarget"[i], stream.get(result_peg_2284.getPosition()))){
            result_peg_2284.nextPosition();
        } else {
            goto out_peg_2285;
        }
    }
    result_peg_2284.setValue((void*) "numtarget");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2284;
    stream.update(result_peg_2284.getPosition());
    
    
    return result_peg_2284;
    out_peg_2285:
    Result result_peg_2286(myposition);
    
    for (int i = 0; i < 12; i++){
        if (compareCharCase("animelemtime"[i], stream.get(result_peg_2286.getPosition()))){
            result_peg_2286.nextPosition();
        } else {
            goto out_peg_2287;
        }
    }
    result_peg_2286.setValue((void*) "animelemtime");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2286;
    stream.update(result_peg_2286.getPosition());
    
    
    return result_peg_2286;
    out_peg_2287:
    Result result_peg_2288(myposition);
    
    for (int i = 0; i < 10; i++){
        if (compareCharCase("animelemno"[i], stream.get(result_peg_2288.getPosition()))){
            result_peg_2288.nextPosition();
        } else {
            goto out_peg_2289;
        }
    }
    result_peg_2288.setValue((void*) "animelemno");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2288;
    stream.update(result_peg_2288.getPosition());
    
    
    return result_peg_2288;
    out_peg_2289:
    Result result_peg_2290(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("animexist"[i], stream.get(result_peg_2290.getPosition()))){
            result_peg_2290.nextPosition();
        } else {
            goto out_peg_2291;
        }
    }
    result_peg_2290.setValue((void*) "animexist");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2290;
    stream.update(result_peg_2290.getPosition());
    
    
    return result_peg_2290;
    out_peg_2291:
    Result result_peg_2292(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projguarded"[i], stream.get(result_peg_2292.getPosition()))){
            result_peg_2292.nextPosition();
        } else {
            goto out_peg_2293;
        }
    }
    result_peg_2292.setValue((void*) "projguarded");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2292;
    stream.update(result_peg_2292.getPosition());
    
    
    return result_peg_2292;
    out_peg_2293:
    Result result_peg_2294(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projhittime"[i], stream.get(result_peg_2294.getPosition()))){
            result_peg_2294.nextPosition();
        } else {
            goto out_peg_2295;
        }
    }
    result_peg_2294.setValue((void*) "projhittime");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2294;
    stream.update(result_peg_2294.getPosition());
    
    
    return result_peg_2294;
    out_peg_2295:
    Result result_peg_2296(myposition);
    
    for (int i = 0; i < 15; i++){
        if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2296.getPosition()))){
            result_peg_2296.nextPosition();
        } else {
            goto out_peg_2297;
        }
    }
    result_peg_2296.setValue((void*) "projcontacttime");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2296;
    stream.update(result_peg_2296.getPosition());
    
    
    return result_peg_2296;
    out_peg_2297:
    Result result_peg_2298(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projcontact"[i], stream.get(result_peg_2298.getPosition()))){
            result_peg_2298.nextPosition();
        } else {
            goto out_peg_2299;
        }
    }
    result_peg_2298.setValue((void*) "projcontact");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2298;
    stream.update(result_peg_2298.getPosition());
    
    
    return result_peg_2298;
    out_peg_2299:
    Result result_peg_2300(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2300.getPosition()))){
            result_peg_2300.nextPosition();
        } else {
            goto out_peg_2301;
        }
    }
    result_peg_2300.setValue((void*) "numhelper");
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = result_peg_2300;
    stream.update(result_peg_2300.getPosition());
    
    
    return result_peg_2300;
    out_peg_2301:
    
    if (column_peg_2237.chunk10 == 0){
        column_peg_2237.chunk10 = new Chunk10();
    }
    column_peg_2237.chunk10->chunk_function_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2303 = stream.getColumn(position);
    if (column_peg_2303.chunk10 != 0 && column_peg_2303.chunk10->chunk_function_rest.calculated()){
        return column_peg_2303.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2361(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_2304(myposition);
    
    {
    
        {
                
                result_peg_2304.reset();
                do{
                    Result result_peg_2316(result_peg_2304.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2316.getPosition()))){
                                result_peg_2316.nextPosition();
                            } else {
                                goto out_peg_2319;
                            }
                        }
                        result_peg_2316.setValue((void*) " ");
                            
                    }
                    goto success_peg_2317;
                    out_peg_2319:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2316.getPosition()))){
                                result_peg_2316.nextPosition();
                            } else {
                                goto out_peg_2321;
                            }
                        }
                        result_peg_2316.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2317;
                    out_peg_2321:
                    goto loop_peg_2315;
                    success_peg_2317:
                    ;
                    result_peg_2304.addResult(result_peg_2316);
                } while (true);
                loop_peg_2315:
                ;
                        
            }
            goto success_peg_2306;
            goto out_peg_2322;
            success_peg_2306:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2304.getPosition()))){
                    result_peg_2304.nextPosition();
                } else {
                    goto out_peg_2322;
                }
            }
            result_peg_2304.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2304.reset();
                do{
                    Result result_peg_2335(result_peg_2304.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2335.getPosition()))){
                                result_peg_2335.nextPosition();
                            } else {
                                goto out_peg_2338;
                            }
                        }
                        result_peg_2335.setValue((void*) " ");
                            
                    }
                    goto success_peg_2336;
                    out_peg_2338:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2335.getPosition()))){
                                result_peg_2335.nextPosition();
                            } else {
                                goto out_peg_2340;
                            }
                        }
                        result_peg_2335.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2336;
                    out_peg_2340:
                    goto loop_peg_2334;
                    success_peg_2336:
                    ;
                    result_peg_2304.addResult(result_peg_2335);
                } while (true);
                loop_peg_2334:
                ;
                        
            }
            goto success_peg_2325;
            goto out_peg_2322;
            success_peg_2325:
            ;
        
        
        
        int save_peg_2342 = result_peg_2304.getPosition();
            
            result_peg_2304 = rule_all_compare(stream, result_peg_2304.getPosition());
            if (result_peg_2304.error()){
                
                result_peg_2304 = Result(save_peg_2342);
                result_peg_2304.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2304.reset();
                do{
                    Result result_peg_2354(result_peg_2304.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2354.getPosition()))){
                                result_peg_2354.nextPosition();
                            } else {
                                goto out_peg_2357;
                            }
                        }
                        result_peg_2354.setValue((void*) " ");
                            
                    }
                    goto success_peg_2355;
                    out_peg_2357:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2354.getPosition()))){
                                result_peg_2354.nextPosition();
                            } else {
                                goto out_peg_2359;
                            }
                        }
                        result_peg_2354.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2355;
                    out_peg_2359:
                    goto loop_peg_2353;
                    success_peg_2355:
                    ;
                    result_peg_2304.addResult(result_peg_2354);
                } while (true);
                loop_peg_2353:
                ;
                        
            }
            goto success_peg_2344;
            goto out_peg_2322;
            success_peg_2344:
            ;
        
        
        
        result_peg_2304 = rule_value(stream, result_peg_2304.getPosition());
            if (result_peg_2304.error()){
                goto out_peg_2322;
            }
        
        
    }
    
    if (column_peg_2303.chunk10 == 0){
        column_peg_2303.chunk10 = new Chunk10();
    }
    column_peg_2303.chunk10->chunk_function_rest = result_peg_2304;
    stream.update(result_peg_2304.getPosition());
    
    
    return result_peg_2304;
    out_peg_2322:
    Result result_peg_2360(myposition);
    
    
    
    if (column_peg_2303.chunk10 == 0){
        column_peg_2303.chunk10 = new Chunk10();
    }
    column_peg_2303.chunk10->chunk_function_rest = result_peg_2360;
    stream.update(result_peg_2360.getPosition());
    
    
    return result_peg_2360;
    
    if (column_peg_2303.chunk10 == 0){
        column_peg_2303.chunk10 = new Chunk10();
    }
    column_peg_2303.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2362 = stream.getColumn(position);
    if (column_peg_2362.chunk10 != 0 && column_peg_2362.chunk10->chunk_keys.calculated()){
        return column_peg_2362.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_2365(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_2363(myposition);
    
    result_peg_2363 = rule_key_value_list(stream, result_peg_2363.getPosition());
    if (result_peg_2363.error()){
        goto out_peg_2364;
    }
    
    if (column_peg_2362.chunk10 == 0){
        column_peg_2362.chunk10 = new Chunk10();
    }
    column_peg_2362.chunk10->chunk_keys = result_peg_2363;
    stream.update(result_peg_2363.getPosition());
    
    
    return result_peg_2363;
    out_peg_2364:
    
    if (column_peg_2362.chunk10 == 0){
        column_peg_2362.chunk10 = new Chunk10();
    }
    column_peg_2362.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2366 = stream.getColumn(position);
    if (column_peg_2366.chunk10 != 0 && column_peg_2366.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2366.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2408(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_2367(myposition);
    
    {
    
        result_peg_2367 = rule_key(stream, result_peg_2367.getPosition());
            if (result_peg_2367.error()){
                goto out_peg_2369;
            }
            first = result_peg_2367.getValues();
        
        
        
        result_peg_2367.reset();
            do{
                Result result_peg_2372(result_peg_2367.getPosition());
                {
                
                    {
                            
                            result_peg_2372.reset();
                            do{
                                Result result_peg_2384(result_peg_2372.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2384.getPosition()))){
                                            result_peg_2384.nextPosition();
                                        } else {
                                            goto out_peg_2387;
                                        }
                                    }
                                    result_peg_2384.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2385;
                                out_peg_2387:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2384.getPosition()))){
                                            result_peg_2384.nextPosition();
                                        } else {
                                            goto out_peg_2389;
                                        }
                                    }
                                    result_peg_2384.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2385;
                                out_peg_2389:
                                goto loop_peg_2383;
                                success_peg_2385:
                                ;
                                result_peg_2372.addResult(result_peg_2384);
                            } while (true);
                            loop_peg_2383:
                            ;
                                    
                        }
                        goto success_peg_2374;
                        goto loop_peg_2371;
                        success_peg_2374:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2372.getPosition()))){
                                result_peg_2372.nextPosition();
                            } else {
                                goto loop_peg_2371;
                            }
                        }
                        result_peg_2372.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2372.reset();
                            do{
                                Result result_peg_2402(result_peg_2372.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2402.getPosition()))){
                                            result_peg_2402.nextPosition();
                                        } else {
                                            goto out_peg_2405;
                                        }
                                    }
                                    result_peg_2402.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2403;
                                out_peg_2405:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2402.getPosition()))){
                                            result_peg_2402.nextPosition();
                                        } else {
                                            goto out_peg_2407;
                                        }
                                    }
                                    result_peg_2402.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2403;
                                out_peg_2407:
                                goto loop_peg_2401;
                                success_peg_2403:
                                ;
                                result_peg_2372.addResult(result_peg_2402);
                            } while (true);
                            loop_peg_2401:
                            ;
                                    
                        }
                        goto success_peg_2392;
                        goto loop_peg_2371;
                        success_peg_2392:
                        ;
                    
                    
                    
                    result_peg_2372 = rule_key(stream, result_peg_2372.getPosition());
                        if (result_peg_2372.error()){
                            goto loop_peg_2371;
                        }
                    
                    
                }
                result_peg_2367.addResult(result_peg_2372);
            } while (true);
            loop_peg_2371:
            ;
            rest = result_peg_2367.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_2367.setValue(value);
            }
        
        
    }
    
    if (column_peg_2366.chunk10 == 0){
        column_peg_2366.chunk10 = new Chunk10();
    }
    column_peg_2366.chunk10->chunk_key_value_list = result_peg_2367;
    stream.update(result_peg_2367.getPosition());
    
    
    return result_peg_2367;
    out_peg_2369:
    
    if (column_peg_2366.chunk10 == 0){
        column_peg_2366.chunk10 = new Chunk10();
    }
    column_peg_2366.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2409 = stream.getColumn(position);
    if (column_peg_2409.chunk10 != 0 && column_peg_2409.chunk10->chunk_key.calculated()){
        return column_peg_2409.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_2417(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_2410(myposition);
    
    {
    
        result_peg_2410 = rule_key_real(stream, result_peg_2410.getPosition());
            if (result_peg_2410.error()){
                goto out_peg_2412;
            }
        
        Result result_peg_2411 = result_peg_2410;
        
        result_peg_2410 = rule_key_rest(stream, result_peg_2410.getPosition(), result_peg_2411.getValues());
            if (result_peg_2410.error()){
                goto out_peg_2412;
            }
            ok = result_peg_2410.getValues();
        
        
        
        Result result_peg_2416(result_peg_2410);
            result_peg_2416 = rule_identifier(stream, result_peg_2416.getPosition());
            if (result_peg_2416.error()){
                goto not_peg_2415;
            }
            goto out_peg_2412;
            not_peg_2415:
            result_peg_2410.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_2410.setValue(value);
            }
        
        
    }
    
    if (column_peg_2409.chunk10 == 0){
        column_peg_2409.chunk10 = new Chunk10();
    }
    column_peg_2409.chunk10->chunk_key = result_peg_2410;
    stream.update(result_peg_2410.getPosition());
    
    
    return result_peg_2410;
    out_peg_2412:
    
    if (column_peg_2409.chunk10 == 0){
        column_peg_2409.chunk10 = new Chunk10();
    }
    column_peg_2409.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2461(stream, "key_rest");
    int myposition = position;
    tail_peg_2420:
    Value another;
    Value new_left;
    Result result_peg_2419(myposition);
    {
    
        {
                
                result_peg_2419.reset();
                do{
                    Result result_peg_2432(result_peg_2419.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2432.getPosition()))){
                                result_peg_2432.nextPosition();
                            } else {
                                goto out_peg_2435;
                            }
                        }
                        result_peg_2432.setValue((void*) " ");
                            
                    }
                    goto success_peg_2433;
                    out_peg_2435:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2432.getPosition()))){
                                result_peg_2432.nextPosition();
                            } else {
                                goto out_peg_2437;
                            }
                        }
                        result_peg_2432.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2433;
                    out_peg_2437:
                    goto loop_peg_2431;
                    success_peg_2433:
                    ;
                    result_peg_2419.addResult(result_peg_2432);
                } while (true);
                loop_peg_2431:
                ;
                        
            }
            goto success_peg_2422;
            goto out_peg_2438;
            success_peg_2422:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_2419.getPosition()))){
                    result_peg_2419.nextPosition();
                } else {
                    goto out_peg_2438;
                }
            }
            result_peg_2419.setValue((void*) "+");
        
        
        
        {
                
                result_peg_2419.reset();
                do{
                    Result result_peg_2451(result_peg_2419.getPosition());
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
                    result_peg_2419.addResult(result_peg_2451);
                } while (true);
                loop_peg_2450:
                ;
                        
            }
            goto success_peg_2441;
            goto out_peg_2438;
            success_peg_2441:
            ;
        
        
        
        result_peg_2419 = rule_key_real(stream, result_peg_2419.getPosition());
            if (result_peg_2419.error()){
                goto out_peg_2438;
            }
            another = result_peg_2419.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_2419.setValue(value);
            }
            new_left = result_peg_2419.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_2419.getPosition();
    goto tail_peg_2420;
    out_peg_2438:
    Result result_peg_2459(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_2459.setValue(value);
            }
        
        
    }
    stream.update(result_peg_2459.getPosition());
    
    return result_peg_2459;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2462 = stream.getColumn(position);
    if (column_peg_2462.chunk11 != 0 && column_peg_2462.chunk11->chunk_key_real.calculated()){
        return column_peg_2462.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_2469(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_2463(myposition);
    
    {
    
        result_peg_2463.reset();
            do{
                Result result_peg_2466(result_peg_2463.getPosition());
                result_peg_2466 = rule_key_modifier(stream, result_peg_2466.getPosition());
                if (result_peg_2466.error()){
                    goto loop_peg_2465;
                }
                result_peg_2463.addResult(result_peg_2466);
            } while (true);
            loop_peg_2465:
            ;
            mods = result_peg_2463.getValues();
        
        
        
        result_peg_2463 = rule_key_name(stream, result_peg_2463.getPosition());
            if (result_peg_2463.error()){
                goto out_peg_2468;
            }
            name = result_peg_2463.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_2463.setValue(value);
            }
        
        
    }
    
    if (column_peg_2462.chunk11 == 0){
        column_peg_2462.chunk11 = new Chunk11();
    }
    column_peg_2462.chunk11->chunk_key_real = result_peg_2463;
    stream.update(result_peg_2463.getPosition());
    
    
    return result_peg_2463;
    out_peg_2468:
    
    if (column_peg_2462.chunk11 == 0){
        column_peg_2462.chunk11 = new Chunk11();
    }
    column_peg_2462.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2470 = stream.getColumn(position);
    if (column_peg_2470.chunk11 != 0 && column_peg_2470.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2470.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2491(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_2471(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_2471.getPosition()))){
                    result_peg_2471.nextPosition();
                } else {
                    goto out_peg_2473;
                }
            }
            result_peg_2471.setValue((void*) "~");
        
        
        
        result_peg_2471.reset();
            do{
                Result result_peg_2476(result_peg_2471.getPosition());
                {
                    
                    char letter_peg_2481 = stream.get(result_peg_2476.getPosition());
                    if (letter_peg_2481 != '\0' && strchr("0123456789", letter_peg_2481) != NULL){
                        result_peg_2476.nextPosition();
                        result_peg_2476.setValue((void*) (long) letter_peg_2481);
                    } else {
                        goto out_peg_2480;
                    }
                    
                }
                goto success_peg_2477;
                out_peg_2480:
                goto loop_peg_2475;
                success_peg_2477:
                ;
                result_peg_2471.addResult(result_peg_2476);
            } while (true);
            loop_peg_2475:
            ;
            num = result_peg_2471.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_2471.setValue(value);
            }
        
        
    }
    
    if (column_peg_2470.chunk11 == 0){
        column_peg_2470.chunk11 = new Chunk11();
    }
    column_peg_2470.chunk11->chunk_key_modifier = result_peg_2471;
    stream.update(result_peg_2471.getPosition());
    
    
    return result_peg_2471;
    out_peg_2473:
    Result result_peg_2482(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_2482.getPosition()))){
                    result_peg_2482.nextPosition();
                } else {
                    goto out_peg_2484;
                }
            }
            result_peg_2482.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_2482.setValue(value);
            }
        
        
    }
    
    if (column_peg_2470.chunk11 == 0){
        column_peg_2470.chunk11 = new Chunk11();
    }
    column_peg_2470.chunk11->chunk_key_modifier = result_peg_2482;
    stream.update(result_peg_2482.getPosition());
    
    
    return result_peg_2482;
    out_peg_2484:
    Result result_peg_2485(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_2485.getPosition()))){
                    result_peg_2485.nextPosition();
                } else {
                    goto out_peg_2487;
                }
            }
            result_peg_2485.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_2485.setValue(value);
            }
        
        
    }
    
    if (column_peg_2470.chunk11 == 0){
        column_peg_2470.chunk11 = new Chunk11();
    }
    column_peg_2470.chunk11->chunk_key_modifier = result_peg_2485;
    stream.update(result_peg_2485.getPosition());
    
    
    return result_peg_2485;
    out_peg_2487:
    Result result_peg_2488(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_2488.getPosition()))){
                    result_peg_2488.nextPosition();
                } else {
                    goto out_peg_2490;
                }
            }
            result_peg_2488.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_2488.setValue(value);
            }
        
        
    }
    
    if (column_peg_2470.chunk11 == 0){
        column_peg_2470.chunk11 = new Chunk11();
    }
    column_peg_2470.chunk11->chunk_key_modifier = result_peg_2488;
    stream.update(result_peg_2488.getPosition());
    
    
    return result_peg_2488;
    out_peg_2490:
    
    if (column_peg_2470.chunk11 == 0){
        column_peg_2470.chunk11 = new Chunk11();
    }
    column_peg_2470.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2492 = stream.getColumn(position);
    if (column_peg_2492.chunk11 != 0 && column_peg_2492.chunk11->chunk_key_name.calculated()){
        return column_peg_2492.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_2523(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_2493(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_2493.getPosition()))){
            result_peg_2493.nextPosition();
        } else {
            goto out_peg_2494;
        }
    }
    result_peg_2493.setValue((void*) "DB");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2493;
    stream.update(result_peg_2493.getPosition());
    
    
    return result_peg_2493;
    out_peg_2494:
    Result result_peg_2495(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_2495.getPosition()))){
            result_peg_2495.nextPosition();
        } else {
            goto out_peg_2496;
        }
    }
    result_peg_2495.setValue((void*) "B");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2495;
    stream.update(result_peg_2495.getPosition());
    
    
    return result_peg_2495;
    out_peg_2496:
    Result result_peg_2497(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_2497.getPosition()))){
            result_peg_2497.nextPosition();
        } else {
            goto out_peg_2498;
        }
    }
    result_peg_2497.setValue((void*) "DF");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2497;
    stream.update(result_peg_2497.getPosition());
    
    
    return result_peg_2497;
    out_peg_2498:
    Result result_peg_2499(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_2499.getPosition()))){
            result_peg_2499.nextPosition();
        } else {
            goto out_peg_2500;
        }
    }
    result_peg_2499.setValue((void*) "D");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2499;
    stream.update(result_peg_2499.getPosition());
    
    
    return result_peg_2499;
    out_peg_2500:
    Result result_peg_2501(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_2501.getPosition()))){
            result_peg_2501.nextPosition();
        } else {
            goto out_peg_2502;
        }
    }
    result_peg_2501.setValue((void*) "F");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2501;
    stream.update(result_peg_2501.getPosition());
    
    
    return result_peg_2501;
    out_peg_2502:
    Result result_peg_2503(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_2503.getPosition()))){
            result_peg_2503.nextPosition();
        } else {
            goto out_peg_2504;
        }
    }
    result_peg_2503.setValue((void*) "UF");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2503;
    stream.update(result_peg_2503.getPosition());
    
    
    return result_peg_2503;
    out_peg_2504:
    Result result_peg_2505(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_2505.getPosition()))){
            result_peg_2505.nextPosition();
        } else {
            goto out_peg_2506;
        }
    }
    result_peg_2505.setValue((void*) "UB");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2505;
    stream.update(result_peg_2505.getPosition());
    
    
    return result_peg_2505;
    out_peg_2506:
    Result result_peg_2507(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_2507.getPosition()))){
            result_peg_2507.nextPosition();
        } else {
            goto out_peg_2508;
        }
    }
    result_peg_2507.setValue((void*) "U");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2507;
    stream.update(result_peg_2507.getPosition());
    
    
    return result_peg_2507;
    out_peg_2508:
    Result result_peg_2509(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_2509.getPosition()))){
            result_peg_2509.nextPosition();
        } else {
            goto out_peg_2510;
        }
    }
    result_peg_2509.setValue((void*) "a");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2509;
    stream.update(result_peg_2509.getPosition());
    
    
    return result_peg_2509;
    out_peg_2510:
    Result result_peg_2511(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_2511.getPosition()))){
            result_peg_2511.nextPosition();
        } else {
            goto out_peg_2512;
        }
    }
    result_peg_2511.setValue((void*) "b");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2511;
    stream.update(result_peg_2511.getPosition());
    
    
    return result_peg_2511;
    out_peg_2512:
    Result result_peg_2513(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_2513.getPosition()))){
            result_peg_2513.nextPosition();
        } else {
            goto out_peg_2514;
        }
    }
    result_peg_2513.setValue((void*) "c");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2513;
    stream.update(result_peg_2513.getPosition());
    
    
    return result_peg_2513;
    out_peg_2514:
    Result result_peg_2515(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_2515.getPosition()))){
            result_peg_2515.nextPosition();
        } else {
            goto out_peg_2516;
        }
    }
    result_peg_2515.setValue((void*) "x");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2515;
    stream.update(result_peg_2515.getPosition());
    
    
    return result_peg_2515;
    out_peg_2516:
    Result result_peg_2517(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_2517.getPosition()))){
            result_peg_2517.nextPosition();
        } else {
            goto out_peg_2518;
        }
    }
    result_peg_2517.setValue((void*) "y");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2517;
    stream.update(result_peg_2517.getPosition());
    
    
    return result_peg_2517;
    out_peg_2518:
    Result result_peg_2519(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_2519.getPosition()))){
            result_peg_2519.nextPosition();
        } else {
            goto out_peg_2520;
        }
    }
    result_peg_2519.setValue((void*) "z");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2519;
    stream.update(result_peg_2519.getPosition());
    
    
    return result_peg_2519;
    out_peg_2520:
    Result result_peg_2521(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_2521.getPosition()))){
            result_peg_2521.nextPosition();
        } else {
            goto out_peg_2522;
        }
    }
    result_peg_2521.setValue((void*) "s");
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = result_peg_2521;
    stream.update(result_peg_2521.getPosition());
    
    
    return result_peg_2521;
    out_peg_2522:
    
    if (column_peg_2492.chunk11 == 0){
        column_peg_2492.chunk11 = new Chunk11();
    }
    column_peg_2492.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2524 = stream.getColumn(position);
    if (column_peg_2524.chunk11 != 0 && column_peg_2524.chunk11->chunk_value.calculated()){
        return column_peg_2524.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_2542(stream, "value");
    int myposition = position;
    
    
    Result result_peg_2525(myposition);
    
    result_peg_2525 = rule_float(stream, result_peg_2525.getPosition());
    if (result_peg_2525.error()){
        goto out_peg_2526;
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2525;
    stream.update(result_peg_2525.getPosition());
    
    
    return result_peg_2525;
    out_peg_2526:
    Result result_peg_2527(myposition);
    
    result_peg_2527 = rule_integer(stream, result_peg_2527.getPosition());
    if (result_peg_2527.error()){
        goto out_peg_2528;
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2527;
    stream.update(result_peg_2527.getPosition());
    
    
    return result_peg_2527;
    out_peg_2528:
    Result result_peg_2529(myposition);
    
    result_peg_2529 = rule_keyword(stream, result_peg_2529.getPosition());
    if (result_peg_2529.error()){
        goto out_peg_2530;
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2529;
    stream.update(result_peg_2529.getPosition());
    
    
    return result_peg_2529;
    out_peg_2530:
    Result result_peg_2531(myposition);
    
    {
    
        Result result_peg_2534(result_peg_2531);
            result_peg_2534 = rule_keyword(stream, result_peg_2534.getPosition());
            if (result_peg_2534.error()){
                goto not_peg_2533;
            }
            goto out_peg_2535;
            not_peg_2533:
            result_peg_2531.setValue((void*)0);
        
        
        
        result_peg_2531 = rule_identifier(stream, result_peg_2531.getPosition());
            if (result_peg_2531.error()){
                goto out_peg_2535;
            }
        
        
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2531;
    stream.update(result_peg_2531.getPosition());
    
    
    return result_peg_2531;
    out_peg_2535:
    Result result_peg_2536(myposition);
    
    result_peg_2536 = rule_range(stream, result_peg_2536.getPosition());
    if (result_peg_2536.error()){
        goto out_peg_2537;
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2536;
    stream.update(result_peg_2536.getPosition());
    
    
    return result_peg_2536;
    out_peg_2537:
    Result result_peg_2538(myposition);
    
    result_peg_2538 = rule_string(stream, result_peg_2538.getPosition());
    if (result_peg_2538.error()){
        goto out_peg_2539;
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2538;
    stream.update(result_peg_2538.getPosition());
    
    
    return result_peg_2538;
    out_peg_2539:
    Result result_peg_2540(myposition);
    
    result_peg_2540 = rule_hitflag(stream, result_peg_2540.getPosition());
    if (result_peg_2540.error()){
        goto out_peg_2541;
    }
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = result_peg_2540;
    stream.update(result_peg_2540.getPosition());
    
    
    return result_peg_2540;
    out_peg_2541:
    
    if (column_peg_2524.chunk11 == 0){
        column_peg_2524.chunk11 = new Chunk11();
    }
    column_peg_2524.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2543 = stream.getColumn(position);
    if (column_peg_2543.chunk12 != 0 && column_peg_2543.chunk12->chunk_helper.calculated()){
        return column_peg_2543.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_2602(stream, "helper");
    int myposition = position;
    
    Value name;
    Value id;
    Result result_peg_2544(myposition);
    
    {
    
        result_peg_2544 = rule_helper__name(stream, result_peg_2544.getPosition());
            if (result_peg_2544.error()){
                goto out_peg_2546;
            }
            name = result_peg_2544.getValues();
        
        
        
        {
                
                result_peg_2544.reset();
                do{
                    Result result_peg_2558(result_peg_2544.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2558.getPosition()))){
                                result_peg_2558.nextPosition();
                            } else {
                                goto out_peg_2561;
                            }
                        }
                        result_peg_2558.setValue((void*) " ");
                            
                    }
                    goto success_peg_2559;
                    out_peg_2561:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2558.getPosition()))){
                                result_peg_2558.nextPosition();
                            } else {
                                goto out_peg_2563;
                            }
                        }
                        result_peg_2558.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2559;
                    out_peg_2563:
                    goto loop_peg_2557;
                    success_peg_2559:
                    ;
                    result_peg_2544.addResult(result_peg_2558);
                } while (true);
                loop_peg_2557:
                ;
                        
            }
            goto success_peg_2548;
            goto out_peg_2546;
            success_peg_2548:
            ;
        
        
        
        int save_peg_2565 = result_peg_2544.getPosition();
            
            result_peg_2544 = rule_helper__expression(stream, result_peg_2544.getPosition());
            if (result_peg_2544.error()){
                
                result_peg_2544 = Result(save_peg_2565);
                result_peg_2544.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2544.reset();
                do{
                    Result result_peg_2577(result_peg_2544.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2577.getPosition()))){
                                result_peg_2577.nextPosition();
                            } else {
                                goto out_peg_2580;
                            }
                        }
                        result_peg_2577.setValue((void*) " ");
                            
                    }
                    goto success_peg_2578;
                    out_peg_2580:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2577.getPosition()))){
                                result_peg_2577.nextPosition();
                            } else {
                                goto out_peg_2582;
                            }
                        }
                        result_peg_2577.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2578;
                    out_peg_2582:
                    goto loop_peg_2576;
                    success_peg_2578:
                    ;
                    result_peg_2544.addResult(result_peg_2577);
                } while (true);
                loop_peg_2576:
                ;
                        
            }
            goto success_peg_2567;
            goto out_peg_2546;
            success_peg_2567:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2544.getPosition()))){
                    result_peg_2544.nextPosition();
                } else {
                    goto out_peg_2546;
                }
            }
            result_peg_2544.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2544.reset();
                do{
                    Result result_peg_2595(result_peg_2544.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2595.getPosition()))){
                                result_peg_2595.nextPosition();
                            } else {
                                goto out_peg_2598;
                            }
                        }
                        result_peg_2595.setValue((void*) " ");
                            
                    }
                    goto success_peg_2596;
                    out_peg_2598:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2595.getPosition()))){
                                result_peg_2595.nextPosition();
                            } else {
                                goto out_peg_2600;
                            }
                        }
                        result_peg_2595.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2596;
                    out_peg_2600:
                    goto loop_peg_2594;
                    success_peg_2596:
                    ;
                    result_peg_2544.addResult(result_peg_2595);
                } while (true);
                loop_peg_2594:
                ;
                        
            }
            goto success_peg_2585;
            goto out_peg_2546;
            success_peg_2585:
            ;
        
        
        
        result_peg_2544 = rule_helper__identifier(stream, result_peg_2544.getPosition());
            if (result_peg_2544.error()){
                goto out_peg_2546;
            }
            id = result_peg_2544.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeHelper(name, id);
                result_peg_2544.setValue(value);
            }
        
        
    }
    
    if (column_peg_2543.chunk12 == 0){
        column_peg_2543.chunk12 = new Chunk12();
    }
    column_peg_2543.chunk12->chunk_helper = result_peg_2544;
    stream.update(result_peg_2544.getPosition());
    
    
    return result_peg_2544;
    out_peg_2546:
    
    if (column_peg_2543.chunk12 == 0){
        column_peg_2543.chunk12 = new Chunk12();
    }
    column_peg_2543.chunk12->chunk_helper = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2603 = stream.getColumn(position);
    if (column_peg_2603.chunk12 != 0 && column_peg_2603.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2603.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_2642(stream, "helper__expression");
    int myposition = position;
    
    
    Result result_peg_2604(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_2604.getPosition()))){
                    result_peg_2604.nextPosition();
                } else {
                    goto out_peg_2606;
                }
            }
            result_peg_2604.setValue((void*) "(");
        
        
        
        {
                
                result_peg_2604.reset();
                do{
                    Result result_peg_2618(result_peg_2604.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2618.getPosition()))){
                                result_peg_2618.nextPosition();
                            } else {
                                goto out_peg_2621;
                            }
                        }
                        result_peg_2618.setValue((void*) " ");
                            
                    }
                    goto success_peg_2619;
                    out_peg_2621:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2618.getPosition()))){
                                result_peg_2618.nextPosition();
                            } else {
                                goto out_peg_2623;
                            }
                        }
                        result_peg_2618.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2619;
                    out_peg_2623:
                    goto loop_peg_2617;
                    success_peg_2619:
                    ;
                    result_peg_2604.addResult(result_peg_2618);
                } while (true);
                loop_peg_2617:
                ;
                        
            }
            goto success_peg_2608;
            goto out_peg_2606;
            success_peg_2608:
            ;
        
        
        
        result_peg_2604 = rule_expr_c(stream, result_peg_2604.getPosition());
            if (result_peg_2604.error()){
                goto out_peg_2606;
            }
        
        
        
        {
                
                result_peg_2604.reset();
                do{
                    Result result_peg_2636(result_peg_2604.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2636.getPosition()))){
                                result_peg_2636.nextPosition();
                            } else {
                                goto out_peg_2639;
                            }
                        }
                        result_peg_2636.setValue((void*) " ");
                            
                    }
                    goto success_peg_2637;
                    out_peg_2639:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2636.getPosition()))){
                                result_peg_2636.nextPosition();
                            } else {
                                goto out_peg_2641;
                            }
                        }
                        result_peg_2636.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2637;
                    out_peg_2641:
                    goto loop_peg_2635;
                    success_peg_2637:
                    ;
                    result_peg_2604.addResult(result_peg_2636);
                } while (true);
                loop_peg_2635:
                ;
                        
            }
            goto success_peg_2626;
            goto out_peg_2606;
            success_peg_2626:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_2604.getPosition()))){
                    result_peg_2604.nextPosition();
                } else {
                    goto out_peg_2606;
                }
            }
            result_peg_2604.setValue((void*) ")");
        
        
    }
    
    if (column_peg_2603.chunk12 == 0){
        column_peg_2603.chunk12 = new Chunk12();
    }
    column_peg_2603.chunk12->chunk_helper__expression = result_peg_2604;
    stream.update(result_peg_2604.getPosition());
    
    
    return result_peg_2604;
    out_peg_2606:
    
    if (column_peg_2603.chunk12 == 0){
        column_peg_2603.chunk12 = new Chunk12();
    }
    column_peg_2603.chunk12->chunk_helper__expression = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_2643 = stream.getColumn(position);
    if (column_peg_2643.chunk12 != 0 && column_peg_2643.chunk12->chunk_helper__name.calculated()){
        return column_peg_2643.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_2660(stream, "helper__name");
    int myposition = position;
    
    
    Result result_peg_2644(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("parent"[i], stream.get(result_peg_2644.getPosition()))){
            result_peg_2644.nextPosition();
        } else {
            goto out_peg_2645;
        }
    }
    result_peg_2644.setValue((void*) "parent");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2644;
    stream.update(result_peg_2644.getPosition());
    
    
    return result_peg_2644;
    out_peg_2645:
    Result result_peg_2646(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("root"[i], stream.get(result_peg_2646.getPosition()))){
            result_peg_2646.nextPosition();
        } else {
            goto out_peg_2647;
        }
    }
    result_peg_2646.setValue((void*) "root");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2646;
    stream.update(result_peg_2646.getPosition());
    
    
    return result_peg_2646;
    out_peg_2647:
    Result result_peg_2648(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("helper"[i], stream.get(result_peg_2648.getPosition()))){
            result_peg_2648.nextPosition();
        } else {
            goto out_peg_2649;
        }
    }
    result_peg_2648.setValue((void*) "helper");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2648;
    stream.update(result_peg_2648.getPosition());
    
    
    return result_peg_2648;
    out_peg_2649:
    Result result_peg_2650(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("target"[i], stream.get(result_peg_2650.getPosition()))){
            result_peg_2650.nextPosition();
        } else {
            goto out_peg_2651;
        }
    }
    result_peg_2650.setValue((void*) "target");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2650;
    stream.update(result_peg_2650.getPosition());
    
    
    return result_peg_2650;
    out_peg_2651:
    Result result_peg_2652(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("partner"[i], stream.get(result_peg_2652.getPosition()))){
            result_peg_2652.nextPosition();
        } else {
            goto out_peg_2653;
        }
    }
    result_peg_2652.setValue((void*) "partner");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2652;
    stream.update(result_peg_2652.getPosition());
    
    
    return result_peg_2652;
    out_peg_2653:
    Result result_peg_2654(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemyNear"[i], stream.get(result_peg_2654.getPosition()))){
            result_peg_2654.nextPosition();
        } else {
            goto out_peg_2655;
        }
    }
    result_peg_2654.setValue((void*) "enemyNear");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2654;
    stream.update(result_peg_2654.getPosition());
    
    
    return result_peg_2654;
    out_peg_2655:
    Result result_peg_2656(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("enemy"[i], stream.get(result_peg_2656.getPosition()))){
            result_peg_2656.nextPosition();
        } else {
            goto out_peg_2657;
        }
    }
    result_peg_2656.setValue((void*) "enemy");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2656;
    stream.update(result_peg_2656.getPosition());
    
    
    return result_peg_2656;
    out_peg_2657:
    Result result_peg_2658(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("playerid"[i], stream.get(result_peg_2658.getPosition()))){
            result_peg_2658.nextPosition();
        } else {
            goto out_peg_2659;
        }
    }
    result_peg_2658.setValue((void*) "playerid");
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = result_peg_2658;
    stream.update(result_peg_2658.getPosition());
    
    
    return result_peg_2658;
    out_peg_2659:
    
    if (column_peg_2643.chunk12 == 0){
        column_peg_2643.chunk12 = new Chunk12();
    }
    column_peg_2643.chunk12->chunk_helper__name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2661 = stream.getColumn(position);
    if (column_peg_2661.chunk12 != 0 && column_peg_2661.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2661.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_2671(stream, "helper__identifier");
    int myposition = position;
    
    
    Result result_peg_2662(myposition);
    
    result_peg_2662 = rule_function(stream, result_peg_2662.getPosition());
    if (result_peg_2662.error()){
        goto out_peg_2663;
    }
    
    if (column_peg_2661.chunk12 == 0){
        column_peg_2661.chunk12 = new Chunk12();
    }
    column_peg_2661.chunk12->chunk_helper__identifier = result_peg_2662;
    stream.update(result_peg_2662.getPosition());
    
    
    return result_peg_2662;
    out_peg_2663:
    Result result_peg_2664(myposition);
    
    result_peg_2664 = rule_keyword(stream, result_peg_2664.getPosition());
    if (result_peg_2664.error()){
        goto out_peg_2665;
    }
    
    if (column_peg_2661.chunk12 == 0){
        column_peg_2661.chunk12 = new Chunk12();
    }
    column_peg_2661.chunk12->chunk_helper__identifier = result_peg_2664;
    stream.update(result_peg_2664.getPosition());
    
    
    return result_peg_2664;
    out_peg_2665:
    Result result_peg_2666(myposition);
    
    {
    
        Result result_peg_2669(result_peg_2666);
            result_peg_2669 = rule_keyword(stream, result_peg_2669.getPosition());
            if (result_peg_2669.error()){
                goto not_peg_2668;
            }
            goto out_peg_2670;
            not_peg_2668:
            result_peg_2666.setValue((void*)0);
        
        
        
        result_peg_2666 = rule_identifier(stream, result_peg_2666.getPosition());
            if (result_peg_2666.error()){
                goto out_peg_2670;
            }
        
        
    }
    
    if (column_peg_2661.chunk12 == 0){
        column_peg_2661.chunk12 = new Chunk12();
    }
    column_peg_2661.chunk12->chunk_helper__identifier = result_peg_2666;
    stream.update(result_peg_2666.getPosition());
    
    
    return result_peg_2666;
    out_peg_2670:
    
    if (column_peg_2661.chunk12 == 0){
        column_peg_2661.chunk12 = new Chunk12();
    }
    column_peg_2661.chunk12->chunk_helper__identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2672 = stream.getColumn(position);
    if (column_peg_2672.chunk12 != 0 && column_peg_2672.chunk12->chunk_hitflag.calculated()){
        return column_peg_2672.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_2694(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_2673(myposition);
    
    {
    
        result_peg_2673.reset();
            do{
                Result result_peg_2676(result_peg_2673.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("A"[i], stream.get(result_peg_2676.getPosition()))){
                            result_peg_2676.nextPosition();
                        } else {
                            goto out_peg_2679;
                        }
                    }
                    result_peg_2676.setValue((void*) "A");
                        
                }
                goto success_peg_2677;
                out_peg_2679:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("M"[i], stream.get(result_peg_2676.getPosition()))){
                            result_peg_2676.nextPosition();
                        } else {
                            goto out_peg_2681;
                        }
                    }
                    result_peg_2676.setValue((void*) "M");
                        
                }
                goto success_peg_2677;
                out_peg_2681:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_2676.getPosition()))){
                            result_peg_2676.nextPosition();
                        } else {
                            goto out_peg_2683;
                        }
                    }
                    result_peg_2676.setValue((void*) "F");
                        
                }
                goto success_peg_2677;
                out_peg_2683:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("D"[i], stream.get(result_peg_2676.getPosition()))){
                            result_peg_2676.nextPosition();
                        } else {
                            goto out_peg_2685;
                        }
                    }
                    result_peg_2676.setValue((void*) "D");
                        
                }
                goto success_peg_2677;
                out_peg_2685:
                goto loop_peg_2675;
                success_peg_2677:
                ;
                result_peg_2673.addResult(result_peg_2676);
            } while (true);
            loop_peg_2675:
            if (result_peg_2673.matches() == 0){
                goto out_peg_2686;
            }
        
        
        
        int save_peg_2688 = result_peg_2673.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2673.getPosition()))){
                        result_peg_2673.nextPosition();
                    } else {
                        goto out_peg_2691;
                    }
                }
                result_peg_2673.setValue((void*) "+");
                    
            }
            goto success_peg_2689;
            out_peg_2691:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2673.getPosition()))){
                        result_peg_2673.nextPosition();
                    } else {
                        goto out_peg_2693;
                    }
                }
                result_peg_2673.setValue((void*) "-");
                    
            }
            goto success_peg_2689;
            out_peg_2693:
            
            result_peg_2673 = Result(save_peg_2688);
            result_peg_2673.setValue((void*) 0);
            
            success_peg_2689:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = makeSimpleIdentifier("A");
                result_peg_2673.setValue(value);
            }
        
        
    }
    
    if (column_peg_2672.chunk12 == 0){
        column_peg_2672.chunk12 = new Chunk12();
    }
    column_peg_2672.chunk12->chunk_hitflag = result_peg_2673;
    stream.update(result_peg_2673.getPosition());
    
    
    return result_peg_2673;
    out_peg_2686:
    
    if (column_peg_2672.chunk12 == 0){
        column_peg_2672.chunk12 = new Chunk12();
    }
    column_peg_2672.chunk12->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2695 = stream.getColumn(position);
    if (column_peg_2695.chunk13 != 0 && column_peg_2695.chunk13->chunk_keyword.calculated()){
        return column_peg_2695.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_2702(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_2696(myposition);
    
    {
    
        result_peg_2696 = rule_keyword_real(stream, result_peg_2696.getPosition());
            if (result_peg_2696.error()){
                goto out_peg_2698;
            }
        
        Result result_peg_2697 = result_peg_2696;
        
        Result result_peg_2701(result_peg_2696);
            result_peg_2701 = rule_alpha_digit(stream, result_peg_2701.getPosition());
            if (result_peg_2701.error()){
                goto not_peg_2700;
            }
            goto out_peg_2698;
            not_peg_2700:
            result_peg_2696.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_2697.getValues();
                result_peg_2696.setValue(value);
            }
        
        
    }
    
    if (column_peg_2695.chunk13 == 0){
        column_peg_2695.chunk13 = new Chunk13();
    }
    column_peg_2695.chunk13->chunk_keyword = result_peg_2696;
    stream.update(result_peg_2696.getPosition());
    
    
    return result_peg_2696;
    out_peg_2698:
    
    if (column_peg_2695.chunk13 == 0){
        column_peg_2695.chunk13 = new Chunk13();
    }
    column_peg_2695.chunk13->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2703 = stream.getColumn(position);
    if (column_peg_2703.chunk13 != 0 && column_peg_2703.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2703.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_3105(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_2704(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2704.getPosition()))){
                    result_peg_2704.nextPosition();
                } else {
                    goto out_peg_2706;
                }
            }
            result_peg_2704.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2704.reset();
                do{
                    Result result_peg_2718(result_peg_2704.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2718.getPosition()))){
                                result_peg_2718.nextPosition();
                            } else {
                                goto out_peg_2721;
                            }
                        }
                        result_peg_2718.setValue((void*) " ");
                            
                    }
                    goto success_peg_2719;
                    out_peg_2721:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2718.getPosition()))){
                                result_peg_2718.nextPosition();
                            } else {
                                goto out_peg_2723;
                            }
                        }
                        result_peg_2718.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2719;
                    out_peg_2723:
                    goto loop_peg_2717;
                    success_peg_2719:
                    ;
                    result_peg_2704.addResult(result_peg_2718);
                } while (true);
                loop_peg_2717:
                ;
                        
            }
            goto success_peg_2708;
            goto out_peg_2706;
            success_peg_2708:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2704.getPosition()))){
                    result_peg_2704.nextPosition();
                } else {
                    goto out_peg_2706;
                }
            }
            result_peg_2704.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_2704.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2704;
    stream.update(result_peg_2704.getPosition());
    
    
    return result_peg_2704;
    out_peg_2706:
    Result result_peg_2725(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2725.getPosition()))){
                    result_peg_2725.nextPosition();
                } else {
                    goto out_peg_2727;
                }
            }
            result_peg_2725.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2725.reset();
                do{
                    Result result_peg_2739(result_peg_2725.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2739.getPosition()))){
                                result_peg_2739.nextPosition();
                            } else {
                                goto out_peg_2742;
                            }
                        }
                        result_peg_2739.setValue((void*) " ");
                            
                    }
                    goto success_peg_2740;
                    out_peg_2742:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2739.getPosition()))){
                                result_peg_2739.nextPosition();
                            } else {
                                goto out_peg_2744;
                            }
                        }
                        result_peg_2739.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2740;
                    out_peg_2744:
                    goto loop_peg_2738;
                    success_peg_2740:
                    ;
                    result_peg_2725.addResult(result_peg_2739);
                } while (true);
                loop_peg_2738:
                ;
                        
            }
            goto success_peg_2729;
            goto out_peg_2727;
            success_peg_2729:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2725.getPosition()))){
                    result_peg_2725.nextPosition();
                } else {
                    goto out_peg_2727;
                }
            }
            result_peg_2725.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_2725.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2725;
    stream.update(result_peg_2725.getPosition());
    
    
    return result_peg_2725;
    out_peg_2727:
    Result result_peg_2746(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2746.getPosition()))){
                    result_peg_2746.nextPosition();
                } else {
                    goto out_peg_2748;
                }
            }
            result_peg_2746.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2746.reset();
                do{
                    Result result_peg_2760(result_peg_2746.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2760.getPosition()))){
                                result_peg_2760.nextPosition();
                            } else {
                                goto out_peg_2763;
                            }
                        }
                        result_peg_2760.setValue((void*) " ");
                            
                    }
                    goto success_peg_2761;
                    out_peg_2763:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2760.getPosition()))){
                                result_peg_2760.nextPosition();
                            } else {
                                goto out_peg_2765;
                            }
                        }
                        result_peg_2760.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2761;
                    out_peg_2765:
                    goto loop_peg_2759;
                    success_peg_2761:
                    ;
                    result_peg_2746.addResult(result_peg_2760);
                } while (true);
                loop_peg_2759:
                ;
                        
            }
            goto success_peg_2750;
            goto out_peg_2748;
            success_peg_2750:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2746.getPosition()))){
                    result_peg_2746.nextPosition();
                } else {
                    goto out_peg_2748;
                }
            }
            result_peg_2746.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_2746.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2746;
    stream.update(result_peg_2746.getPosition());
    
    
    return result_peg_2746;
    out_peg_2748:
    Result result_peg_2767(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2767.getPosition()))){
                    result_peg_2767.nextPosition();
                } else {
                    goto out_peg_2769;
                }
            }
            result_peg_2767.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2767.reset();
                do{
                    Result result_peg_2781(result_peg_2767.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2781.getPosition()))){
                                result_peg_2781.nextPosition();
                            } else {
                                goto out_peg_2784;
                            }
                        }
                        result_peg_2781.setValue((void*) " ");
                            
                    }
                    goto success_peg_2782;
                    out_peg_2784:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2781.getPosition()))){
                                result_peg_2781.nextPosition();
                            } else {
                                goto out_peg_2786;
                            }
                        }
                        result_peg_2781.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2782;
                    out_peg_2786:
                    goto loop_peg_2780;
                    success_peg_2782:
                    ;
                    result_peg_2767.addResult(result_peg_2781);
                } while (true);
                loop_peg_2780:
                ;
                        
            }
            goto success_peg_2771;
            goto out_peg_2769;
            success_peg_2771:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2767.getPosition()))){
                    result_peg_2767.nextPosition();
                } else {
                    goto out_peg_2769;
                }
            }
            result_peg_2767.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_2767.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2767;
    stream.update(result_peg_2767.getPosition());
    
    
    return result_peg_2767;
    out_peg_2769:
    Result result_peg_2788(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2788.getPosition()))){
                    result_peg_2788.nextPosition();
                } else {
                    goto out_peg_2790;
                }
            }
            result_peg_2788.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2788.reset();
                do{
                    Result result_peg_2802(result_peg_2788.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2802.getPosition()))){
                                result_peg_2802.nextPosition();
                            } else {
                                goto out_peg_2805;
                            }
                        }
                        result_peg_2802.setValue((void*) " ");
                            
                    }
                    goto success_peg_2803;
                    out_peg_2805:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2802.getPosition()))){
                                result_peg_2802.nextPosition();
                            } else {
                                goto out_peg_2807;
                            }
                        }
                        result_peg_2802.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2803;
                    out_peg_2807:
                    goto loop_peg_2801;
                    success_peg_2803:
                    ;
                    result_peg_2788.addResult(result_peg_2802);
                } while (true);
                loop_peg_2801:
                ;
                        
            }
            goto success_peg_2792;
            goto out_peg_2790;
            success_peg_2792:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2788.getPosition()))){
                    result_peg_2788.nextPosition();
                } else {
                    goto out_peg_2790;
                }
            }
            result_peg_2788.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_2788.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2788;
    stream.update(result_peg_2788.getPosition());
    
    
    return result_peg_2788;
    out_peg_2790:
    Result result_peg_2809(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2809.getPosition()))){
                    result_peg_2809.nextPosition();
                } else {
                    goto out_peg_2811;
                }
            }
            result_peg_2809.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2809.reset();
                do{
                    Result result_peg_2823(result_peg_2809.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2823.getPosition()))){
                                result_peg_2823.nextPosition();
                            } else {
                                goto out_peg_2826;
                            }
                        }
                        result_peg_2823.setValue((void*) " ");
                            
                    }
                    goto success_peg_2824;
                    out_peg_2826:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2823.getPosition()))){
                                result_peg_2823.nextPosition();
                            } else {
                                goto out_peg_2828;
                            }
                        }
                        result_peg_2823.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2824;
                    out_peg_2828:
                    goto loop_peg_2822;
                    success_peg_2824:
                    ;
                    result_peg_2809.addResult(result_peg_2823);
                } while (true);
                loop_peg_2822:
                ;
                        
            }
            goto success_peg_2813;
            goto out_peg_2811;
            success_peg_2813:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2809.getPosition()))){
                    result_peg_2809.nextPosition();
                } else {
                    goto out_peg_2811;
                }
            }
            result_peg_2809.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_2809.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2809;
    stream.update(result_peg_2809.getPosition());
    
    
    return result_peg_2809;
    out_peg_2811:
    Result result_peg_2830(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2830.getPosition()))){
                    result_peg_2830.nextPosition();
                } else {
                    goto out_peg_2832;
                }
            }
            result_peg_2830.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2830.reset();
                do{
                    Result result_peg_2844(result_peg_2830.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2844.getPosition()))){
                                result_peg_2844.nextPosition();
                            } else {
                                goto out_peg_2847;
                            }
                        }
                        result_peg_2844.setValue((void*) " ");
                            
                    }
                    goto success_peg_2845;
                    out_peg_2847:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2844.getPosition()))){
                                result_peg_2844.nextPosition();
                            } else {
                                goto out_peg_2849;
                            }
                        }
                        result_peg_2844.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2845;
                    out_peg_2849:
                    goto loop_peg_2843;
                    success_peg_2845:
                    ;
                    result_peg_2830.addResult(result_peg_2844);
                } while (true);
                loop_peg_2843:
                ;
                        
            }
            goto success_peg_2834;
            goto out_peg_2832;
            success_peg_2834:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2830.getPosition()))){
                    result_peg_2830.nextPosition();
                } else {
                    goto out_peg_2832;
                }
            }
            result_peg_2830.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_2830.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2830;
    stream.update(result_peg_2830.getPosition());
    
    
    return result_peg_2830;
    out_peg_2832:
    Result result_peg_2851(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2851.getPosition()))){
                    result_peg_2851.nextPosition();
                } else {
                    goto out_peg_2853;
                }
            }
            result_peg_2851.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2851.reset();
                do{
                    Result result_peg_2865(result_peg_2851.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2865.getPosition()))){
                                result_peg_2865.nextPosition();
                            } else {
                                goto out_peg_2868;
                            }
                        }
                        result_peg_2865.setValue((void*) " ");
                            
                    }
                    goto success_peg_2866;
                    out_peg_2868:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2865.getPosition()))){
                                result_peg_2865.nextPosition();
                            } else {
                                goto out_peg_2870;
                            }
                        }
                        result_peg_2865.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2866;
                    out_peg_2870:
                    goto loop_peg_2864;
                    success_peg_2866:
                    ;
                    result_peg_2851.addResult(result_peg_2865);
                } while (true);
                loop_peg_2864:
                ;
                        
            }
            goto success_peg_2855;
            goto out_peg_2853;
            success_peg_2855:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2851.getPosition()))){
                    result_peg_2851.nextPosition();
                } else {
                    goto out_peg_2853;
                }
            }
            result_peg_2851.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_2851.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2851;
    stream.update(result_peg_2851.getPosition());
    
    
    return result_peg_2851;
    out_peg_2853:
    Result result_peg_2872(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2872.getPosition()))){
                    result_peg_2872.nextPosition();
                } else {
                    goto out_peg_2874;
                }
            }
            result_peg_2872.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2872.reset();
                do{
                    Result result_peg_2886(result_peg_2872.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2886.getPosition()))){
                                result_peg_2886.nextPosition();
                            } else {
                                goto out_peg_2889;
                            }
                        }
                        result_peg_2886.setValue((void*) " ");
                            
                    }
                    goto success_peg_2887;
                    out_peg_2889:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2886.getPosition()))){
                                result_peg_2886.nextPosition();
                            } else {
                                goto out_peg_2891;
                            }
                        }
                        result_peg_2886.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2887;
                    out_peg_2891:
                    goto loop_peg_2885;
                    success_peg_2887:
                    ;
                    result_peg_2872.addResult(result_peg_2886);
                } while (true);
                loop_peg_2885:
                ;
                        
            }
            goto success_peg_2876;
            goto out_peg_2874;
            success_peg_2876:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2872.getPosition()))){
                    result_peg_2872.nextPosition();
                } else {
                    goto out_peg_2874;
                }
            }
            result_peg_2872.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_2872.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2872;
    stream.update(result_peg_2872.getPosition());
    
    
    return result_peg_2872;
    out_peg_2874:
    Result result_peg_2893(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2893.getPosition()))){
                    result_peg_2893.nextPosition();
                } else {
                    goto out_peg_2895;
                }
            }
            result_peg_2893.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2893.reset();
                do{
                    Result result_peg_2907(result_peg_2893.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2907.getPosition()))){
                                result_peg_2907.nextPosition();
                            } else {
                                goto out_peg_2910;
                            }
                        }
                        result_peg_2907.setValue((void*) " ");
                            
                    }
                    goto success_peg_2908;
                    out_peg_2910:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2907.getPosition()))){
                                result_peg_2907.nextPosition();
                            } else {
                                goto out_peg_2912;
                            }
                        }
                        result_peg_2907.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2908;
                    out_peg_2912:
                    goto loop_peg_2906;
                    success_peg_2908:
                    ;
                    result_peg_2893.addResult(result_peg_2907);
                } while (true);
                loop_peg_2906:
                ;
                        
            }
            goto success_peg_2897;
            goto out_peg_2895;
            success_peg_2897:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2893.getPosition()))){
                    result_peg_2893.nextPosition();
                } else {
                    goto out_peg_2895;
                }
            }
            result_peg_2893.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_2893.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2893;
    stream.update(result_peg_2893.getPosition());
    
    
    return result_peg_2893;
    out_peg_2895:
    Result result_peg_2914(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2914.getPosition()))){
                    result_peg_2914.nextPosition();
                } else {
                    goto out_peg_2916;
                }
            }
            result_peg_2914.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2914.reset();
                do{
                    Result result_peg_2928(result_peg_2914.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2928.getPosition()))){
                                result_peg_2928.nextPosition();
                            } else {
                                goto out_peg_2931;
                            }
                        }
                        result_peg_2928.setValue((void*) " ");
                            
                    }
                    goto success_peg_2929;
                    out_peg_2931:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2928.getPosition()))){
                                result_peg_2928.nextPosition();
                            } else {
                                goto out_peg_2933;
                            }
                        }
                        result_peg_2928.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2929;
                    out_peg_2933:
                    goto loop_peg_2927;
                    success_peg_2929:
                    ;
                    result_peg_2914.addResult(result_peg_2928);
                } while (true);
                loop_peg_2927:
                ;
                        
            }
            goto success_peg_2918;
            goto out_peg_2916;
            success_peg_2918:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2914.getPosition()))){
                    result_peg_2914.nextPosition();
                } else {
                    goto out_peg_2916;
                }
            }
            result_peg_2914.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_2914.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2914;
    stream.update(result_peg_2914.getPosition());
    
    
    return result_peg_2914;
    out_peg_2916:
    Result result_peg_2935(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2935.getPosition()))){
                    result_peg_2935.nextPosition();
                } else {
                    goto out_peg_2937;
                }
            }
            result_peg_2935.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2935.reset();
                do{
                    Result result_peg_2949(result_peg_2935.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2949.getPosition()))){
                                result_peg_2949.nextPosition();
                            } else {
                                goto out_peg_2952;
                            }
                        }
                        result_peg_2949.setValue((void*) " ");
                            
                    }
                    goto success_peg_2950;
                    out_peg_2952:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2949.getPosition()))){
                                result_peg_2949.nextPosition();
                            } else {
                                goto out_peg_2954;
                            }
                        }
                        result_peg_2949.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2950;
                    out_peg_2954:
                    goto loop_peg_2948;
                    success_peg_2950:
                    ;
                    result_peg_2935.addResult(result_peg_2949);
                } while (true);
                loop_peg_2948:
                ;
                        
            }
            goto success_peg_2939;
            goto out_peg_2937;
            success_peg_2939:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2935.getPosition()))){
                    result_peg_2935.nextPosition();
                } else {
                    goto out_peg_2937;
                }
            }
            result_peg_2935.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_2935.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2935;
    stream.update(result_peg_2935.getPosition());
    
    
    return result_peg_2935;
    out_peg_2937:
    Result result_peg_2956(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2956.getPosition()))){
                    result_peg_2956.nextPosition();
                } else {
                    goto out_peg_2958;
                }
            }
            result_peg_2956.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2956.reset();
                do{
                    Result result_peg_2970(result_peg_2956.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2970.getPosition()))){
                                result_peg_2970.nextPosition();
                            } else {
                                goto out_peg_2973;
                            }
                        }
                        result_peg_2970.setValue((void*) " ");
                            
                    }
                    goto success_peg_2971;
                    out_peg_2973:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2970.getPosition()))){
                                result_peg_2970.nextPosition();
                            } else {
                                goto out_peg_2975;
                            }
                        }
                        result_peg_2970.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2971;
                    out_peg_2975:
                    goto loop_peg_2969;
                    success_peg_2971:
                    ;
                    result_peg_2956.addResult(result_peg_2970);
                } while (true);
                loop_peg_2969:
                ;
                        
            }
            goto success_peg_2960;
            goto out_peg_2958;
            success_peg_2960:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2956.getPosition()))){
                    result_peg_2956.nextPosition();
                } else {
                    goto out_peg_2958;
                }
            }
            result_peg_2956.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_2956.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2956;
    stream.update(result_peg_2956.getPosition());
    
    
    return result_peg_2956;
    out_peg_2958:
    Result result_peg_2977(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_2977.getPosition()))){
                    result_peg_2977.nextPosition();
                } else {
                    goto out_peg_2979;
                }
            }
            result_peg_2977.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_2977.reset();
                do{
                    Result result_peg_2991(result_peg_2977.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2991.getPosition()))){
                                result_peg_2991.nextPosition();
                            } else {
                                goto out_peg_2994;
                            }
                        }
                        result_peg_2991.setValue((void*) " ");
                            
                    }
                    goto success_peg_2992;
                    out_peg_2994:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2991.getPosition()))){
                                result_peg_2991.nextPosition();
                            } else {
                                goto out_peg_2996;
                            }
                        }
                        result_peg_2991.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2992;
                    out_peg_2996:
                    goto loop_peg_2990;
                    success_peg_2992:
                    ;
                    result_peg_2977.addResult(result_peg_2991);
                } while (true);
                loop_peg_2990:
                ;
                        
            }
            goto success_peg_2981;
            goto out_peg_2979;
            success_peg_2981:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2977.getPosition()))){
                    result_peg_2977.nextPosition();
                } else {
                    goto out_peg_2979;
                }
            }
            result_peg_2977.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_2977.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2977;
    stream.update(result_peg_2977.getPosition());
    
    
    return result_peg_2977;
    out_peg_2979:
    Result result_peg_2998(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_2998.getPosition()))){
                    result_peg_2998.nextPosition();
                } else {
                    goto out_peg_3000;
                }
            }
            result_peg_2998.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_2998.reset();
                do{
                    Result result_peg_3012(result_peg_2998.getPosition());
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
                    result_peg_2998.addResult(result_peg_3012);
                } while (true);
                loop_peg_3011:
                ;
                        
            }
            goto success_peg_3002;
            goto out_peg_3000;
            success_peg_3002:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2998.getPosition()))){
                    result_peg_2998.nextPosition();
                } else {
                    goto out_peg_3000;
                }
            }
            result_peg_2998.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_2998.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_2998;
    stream.update(result_peg_2998.getPosition());
    
    
    return result_peg_2998;
    out_peg_3000:
    Result result_peg_3019(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_3019.getPosition()))){
                    result_peg_3019.nextPosition();
                } else {
                    goto out_peg_3021;
                }
            }
            result_peg_3019.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_3019.reset();
                do{
                    Result result_peg_3033(result_peg_3019.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3033.getPosition()))){
                                result_peg_3033.nextPosition();
                            } else {
                                goto out_peg_3036;
                            }
                        }
                        result_peg_3033.setValue((void*) " ");
                            
                    }
                    goto success_peg_3034;
                    out_peg_3036:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3033.getPosition()))){
                                result_peg_3033.nextPosition();
                            } else {
                                goto out_peg_3038;
                            }
                        }
                        result_peg_3033.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3034;
                    out_peg_3038:
                    goto loop_peg_3032;
                    success_peg_3034:
                    ;
                    result_peg_3019.addResult(result_peg_3033);
                } while (true);
                loop_peg_3032:
                ;
                        
            }
            goto success_peg_3023;
            goto out_peg_3021;
            success_peg_3023:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_3019.getPosition()))){
                    result_peg_3019.nextPosition();
                } else {
                    goto out_peg_3021;
                }
            }
            result_peg_3019.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_3019.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_3019;
    stream.update(result_peg_3019.getPosition());
    
    
    return result_peg_3019;
    out_peg_3021:
    Result result_peg_3040(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_3040.getPosition()))){
                    result_peg_3040.nextPosition();
                } else {
                    goto out_peg_3042;
                }
            }
            result_peg_3040.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_3040.reset();
                do{
                    Result result_peg_3054(result_peg_3040.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3054.getPosition()))){
                                result_peg_3054.nextPosition();
                            } else {
                                goto out_peg_3057;
                            }
                        }
                        result_peg_3054.setValue((void*) " ");
                            
                    }
                    goto success_peg_3055;
                    out_peg_3057:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3054.getPosition()))){
                                result_peg_3054.nextPosition();
                            } else {
                                goto out_peg_3059;
                            }
                        }
                        result_peg_3054.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3055;
                    out_peg_3059:
                    goto loop_peg_3053;
                    success_peg_3055:
                    ;
                    result_peg_3040.addResult(result_peg_3054);
                } while (true);
                loop_peg_3053:
                ;
                        
            }
            goto success_peg_3044;
            goto out_peg_3042;
            success_peg_3044:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3040.getPosition()))){
                    result_peg_3040.nextPosition();
                } else {
                    goto out_peg_3042;
                }
            }
            result_peg_3040.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist y");
                result_peg_3040.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_3040;
    stream.update(result_peg_3040.getPosition());
    
    
    return result_peg_3040;
    out_peg_3042:
    Result result_peg_3061(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_3061.getPosition()))){
                    result_peg_3061.nextPosition();
                } else {
                    goto out_peg_3063;
                }
            }
            result_peg_3061.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_3061.reset();
                do{
                    Result result_peg_3075(result_peg_3061.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3075.getPosition()))){
                                result_peg_3075.nextPosition();
                            } else {
                                goto out_peg_3078;
                            }
                        }
                        result_peg_3075.setValue((void*) " ");
                            
                    }
                    goto success_peg_3076;
                    out_peg_3078:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3075.getPosition()))){
                                result_peg_3075.nextPosition();
                            } else {
                                goto out_peg_3080;
                            }
                        }
                        result_peg_3075.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3076;
                    out_peg_3080:
                    goto loop_peg_3074;
                    success_peg_3076:
                    ;
                    result_peg_3061.addResult(result_peg_3075);
                } while (true);
                loop_peg_3074:
                ;
                        
            }
            goto success_peg_3065;
            goto out_peg_3063;
            success_peg_3065:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_3061.getPosition()))){
                    result_peg_3061.nextPosition();
                } else {
                    goto out_peg_3063;
                }
            }
            result_peg_3061.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist x");
                result_peg_3061.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_3061;
    stream.update(result_peg_3061.getPosition());
    
    
    return result_peg_3061;
    out_peg_3063:
    Result result_peg_3082(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_3082.getPosition()))){
                    result_peg_3082.nextPosition();
                } else {
                    goto out_peg_3084;
                }
            }
            result_peg_3082.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_3082.reset();
                do{
                    Result result_peg_3096(result_peg_3082.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_3096.getPosition()))){
                                result_peg_3096.nextPosition();
                            } else {
                                goto out_peg_3099;
                            }
                        }
                        result_peg_3096.setValue((void*) " ");
                            
                    }
                    goto success_peg_3097;
                    out_peg_3099:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_3096.getPosition()))){
                                result_peg_3096.nextPosition();
                            } else {
                                goto out_peg_3101;
                            }
                        }
                        result_peg_3096.setValue((void*) "\t");
                            
                    }
                    goto success_peg_3097;
                    out_peg_3101:
                    goto loop_peg_3095;
                    success_peg_3097:
                    ;
                    result_peg_3082.addResult(result_peg_3096);
                } while (true);
                loop_peg_3095:
                ;
                        
            }
            goto success_peg_3086;
            goto out_peg_3084;
            success_peg_3086:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_3082.getPosition()))){
                    result_peg_3082.nextPosition();
                } else {
                    goto out_peg_3084;
                }
            }
            result_peg_3082.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist y");
                result_peg_3082.setValue(value);
            }
        
        
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_3082;
    stream.update(result_peg_3082.getPosition());
    
    
    return result_peg_3082;
    out_peg_3084:
    Result result_peg_3103(myposition);
    
    result_peg_3103 = rule_hitflag(stream, result_peg_3103.getPosition());
    if (result_peg_3103.error()){
        goto out_peg_3104;
    }
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = result_peg_3103;
    stream.update(result_peg_3103.getPosition());
    
    
    return result_peg_3103;
    out_peg_3104:
    
    if (column_peg_2703.chunk13 == 0){
        column_peg_2703.chunk13 = new Chunk13();
    }
    column_peg_2703.chunk13->chunk_keyword_real = errorResult;
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

        
