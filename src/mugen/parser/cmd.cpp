

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
    
    RuleTrace trace_peg_535(stream, "assignment");
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
    
    if (column_peg_239.chunk1 == 0){
        column_peg_239.chunk1 = new Chunk1();
    }
    column_peg_239.chunk1->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_536 = stream.getColumn(position);
    if (column_peg_536.chunk2 != 0 && column_peg_536.chunk2->chunk_identifier.calculated()){
        return column_peg_536.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_546(stream, "identifier");
    int myposition = position;
    
    Value line;
    Value first;
    Value rest;
    Result result_peg_537(myposition);
    
    {
    
        Stream::LineInfo line_info_peg_539 = stream.getLineInfo(result_peg_537.getPosition());
            line = &line_info_peg_539;
        
        
        
        result_peg_537 = rule_name(stream, result_peg_537.getPosition());
            if (result_peg_537.error()){
                goto out_peg_541;
            }
            first = result_peg_537.getValues();
        
        
        
        result_peg_537.reset();
            do{
                Result result_peg_544(result_peg_537.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_544.getPosition()))){
                                result_peg_544.nextPosition();
                            } else {
                                goto loop_peg_543;
                            }
                        }
                        result_peg_544.setValue((void*) ".");
                    
                    
                    
                    result_peg_544 = rule_name(stream, result_peg_544.getPosition());
                        if (result_peg_544.error()){
                            goto loop_peg_543;
                        }
                    
                    
                }
                result_peg_537.addResult(result_peg_544);
            } while (true);
            loop_peg_543:
            ;
            rest = result_peg_537.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(getCurrentLine(line), getCurrentColumn(line), first, rest);
                result_peg_537.setValue(value);
            }
        
        
    }
    
    if (column_peg_536.chunk2 == 0){
        column_peg_536.chunk2 = new Chunk2();
    }
    column_peg_536.chunk2->chunk_identifier = result_peg_537;
    stream.update(result_peg_537.getPosition());
    
    
    return result_peg_537;
    out_peg_541:
    
    if (column_peg_536.chunk2 == 0){
        column_peg_536.chunk2 = new Chunk2();
    }
    column_peg_536.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_547 = stream.getColumn(position);
    if (column_peg_547.chunk2 != 0 && column_peg_547.chunk2->chunk_integer.calculated()){
        return column_peg_547.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_565(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_548(myposition);
    
    {
    
        int save_peg_550 = result_peg_548.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_548.getPosition()))){
                        result_peg_548.nextPosition();
                    } else {
                        goto out_peg_553;
                    }
                }
                result_peg_548.setValue((void*) "-");
                    
            }
            goto success_peg_551;
            out_peg_553:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_548.getPosition()))){
                        result_peg_548.nextPosition();
                    } else {
                        goto out_peg_555;
                    }
                }
                result_peg_548.setValue((void*) "+");
                    
            }
            goto success_peg_551;
            out_peg_555:
            
            result_peg_548 = Result(save_peg_550);
            result_peg_548.setValue((void*) 0);
            
            success_peg_551:
            ;
        
        Result result_peg_549 = result_peg_548;
        
        result_peg_548.reset();
            do{
                Result result_peg_558(result_peg_548.getPosition());
                {
                    
                    char letter_peg_563 = stream.get(result_peg_558.getPosition());
                    if (letter_peg_563 != '\0' && strchr("0123456789", letter_peg_563) != NULL){
                        result_peg_558.nextPosition();
                        result_peg_558.setValue((void*) (long) letter_peg_563);
                    } else {
                        goto out_peg_562;
                    }
                    
                }
                goto success_peg_559;
                out_peg_562:
                goto loop_peg_557;
                success_peg_559:
                ;
                result_peg_548.addResult(result_peg_558);
            } while (true);
            loop_peg_557:
            if (result_peg_548.matches() == 0){
                goto out_peg_564;
            }
        
        Result result_peg_556 = result_peg_548;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_549.getValues(), result_peg_556.getValues());
                result_peg_548.setValue(value);
            }
        
        
    }
    
    if (column_peg_547.chunk2 == 0){
        column_peg_547.chunk2 = new Chunk2();
    }
    column_peg_547.chunk2->chunk_integer = result_peg_548;
    stream.update(result_peg_548.getPosition());
    
    
    return result_peg_548;
    out_peg_564:
    
    if (column_peg_547.chunk2 == 0){
        column_peg_547.chunk2 = new Chunk2();
    }
    column_peg_547.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_566 = stream.getColumn(position);
    if (column_peg_566.chunk2 != 0 && column_peg_566.chunk2->chunk_float.calculated()){
        return column_peg_566.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_611(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_567(myposition);
    
    {
    
        int save_peg_569 = result_peg_567.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_567.getPosition()))){
                        result_peg_567.nextPosition();
                    } else {
                        goto out_peg_572;
                    }
                }
                result_peg_567.setValue((void*) "-");
                    
            }
            goto success_peg_570;
            out_peg_572:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_567.getPosition()))){
                        result_peg_567.nextPosition();
                    } else {
                        goto out_peg_574;
                    }
                }
                result_peg_567.setValue((void*) "+");
                    
            }
            goto success_peg_570;
            out_peg_574:
            
            result_peg_567 = Result(save_peg_569);
            result_peg_567.setValue((void*) 0);
            
            success_peg_570:
            ;
        
        Result result_peg_568 = result_peg_567;
        
        result_peg_567.reset();
            do{
                Result result_peg_577(result_peg_567.getPosition());
                {
                    
                    char letter_peg_582 = stream.get(result_peg_577.getPosition());
                    if (letter_peg_582 != '\0' && strchr("0123456789", letter_peg_582) != NULL){
                        result_peg_577.nextPosition();
                        result_peg_577.setValue((void*) (long) letter_peg_582);
                    } else {
                        goto out_peg_581;
                    }
                    
                }
                goto success_peg_578;
                out_peg_581:
                goto loop_peg_576;
                success_peg_578:
                ;
                result_peg_567.addResult(result_peg_577);
            } while (true);
            loop_peg_576:
            ;
            left = result_peg_567.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_567.getPosition()))){
                    result_peg_567.nextPosition();
                } else {
                    goto out_peg_584;
                }
            }
            result_peg_567.setValue((void*) ".");
        
        
        
        result_peg_567.reset();
            do{
                Result result_peg_587(result_peg_567.getPosition());
                {
                    
                    char letter_peg_592 = stream.get(result_peg_587.getPosition());
                    if (letter_peg_592 != '\0' && strchr("0123456789", letter_peg_592) != NULL){
                        result_peg_587.nextPosition();
                        result_peg_587.setValue((void*) (long) letter_peg_592);
                    } else {
                        goto out_peg_591;
                    }
                    
                }
                goto success_peg_588;
                out_peg_591:
                goto loop_peg_586;
                success_peg_588:
                ;
                result_peg_567.addResult(result_peg_587);
            } while (true);
            loop_peg_586:
            if (result_peg_567.matches() == 0){
                goto out_peg_584;
            }
            right = result_peg_567.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_568.getValues(), parseDouble(left,right));
                result_peg_567.setValue(value);
            }
        
        
    }
    
    if (column_peg_566.chunk2 == 0){
        column_peg_566.chunk2 = new Chunk2();
    }
    column_peg_566.chunk2->chunk_float = result_peg_567;
    stream.update(result_peg_567.getPosition());
    
    
    return result_peg_567;
    out_peg_584:
    Result result_peg_593(myposition);
    
    {
    
        int save_peg_595 = result_peg_593.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_593.getPosition()))){
                        result_peg_593.nextPosition();
                    } else {
                        goto out_peg_598;
                    }
                }
                result_peg_593.setValue((void*) "-");
                    
            }
            goto success_peg_596;
            out_peg_598:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_593.getPosition()))){
                        result_peg_593.nextPosition();
                    } else {
                        goto out_peg_600;
                    }
                }
                result_peg_593.setValue((void*) "+");
                    
            }
            goto success_peg_596;
            out_peg_600:
            
            result_peg_593 = Result(save_peg_595);
            result_peg_593.setValue((void*) 0);
            
            success_peg_596:
            ;
        
        Result result_peg_594 = result_peg_593;
        
        result_peg_593.reset();
            do{
                Result result_peg_603(result_peg_593.getPosition());
                {
                    
                    char letter_peg_608 = stream.get(result_peg_603.getPosition());
                    if (letter_peg_608 != '\0' && strchr("0123456789", letter_peg_608) != NULL){
                        result_peg_603.nextPosition();
                        result_peg_603.setValue((void*) (long) letter_peg_608);
                    } else {
                        goto out_peg_607;
                    }
                    
                }
                goto success_peg_604;
                out_peg_607:
                goto loop_peg_602;
                success_peg_604:
                ;
                result_peg_593.addResult(result_peg_603);
            } while (true);
            loop_peg_602:
            if (result_peg_593.matches() == 0){
                goto out_peg_609;
            }
            left = result_peg_593.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_593.getPosition()))){
                    result_peg_593.nextPosition();
                } else {
                    goto out_peg_609;
                }
            }
            result_peg_593.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_594.getValues(), parseDouble(left));
                result_peg_593.setValue(value);
            }
        
        
    }
    
    if (column_peg_566.chunk2 == 0){
        column_peg_566.chunk2 = new Chunk2();
    }
    column_peg_566.chunk2->chunk_float = result_peg_593;
    stream.update(result_peg_593.getPosition());
    
    
    return result_peg_593;
    out_peg_609:
    
    if (column_peg_566.chunk2 == 0){
        column_peg_566.chunk2 = new Chunk2();
    }
    column_peg_566.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_612 = stream.getColumn(position);
    if (column_peg_612.chunk2 != 0 && column_peg_612.chunk2->chunk_string.calculated()){
        return column_peg_612.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_627(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_613(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_613.getPosition()))){
                    result_peg_613.nextPosition();
                } else {
                    goto out_peg_615;
                }
            }
            result_peg_613.setValue((void*) "\"");
        
        
        
        result_peg_613.reset();
            do{
                Result result_peg_618(result_peg_613.getPosition());
                {
                
                    Result result_peg_621(result_peg_618);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_621.getPosition()))){
                                result_peg_621.nextPosition();
                            } else {
                                goto not_peg_620;
                            }
                        }
                        result_peg_621.setValue((void*) "\"");
                        goto loop_peg_617;
                        not_peg_620:
                        result_peg_618.setValue((void*)0);
                    
                    
                    
                    Result result_peg_624(result_peg_618);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_624.getPosition()))){
                                result_peg_624.nextPosition();
                            } else {
                                goto not_peg_623;
                            }
                        }
                        result_peg_624.setValue((void*) "\n");
                        goto loop_peg_617;
                        not_peg_623:
                        result_peg_618.setValue((void*)0);
                    
                    
                    
                    char temp_peg_625 = stream.get(result_peg_618.getPosition());
                        if (temp_peg_625 != '\0'){
                            result_peg_618.setValue((void*) (long) temp_peg_625);
                            result_peg_618.nextPosition();
                        } else {
                            goto loop_peg_617;
                        }
                    
                    
                }
                result_peg_613.addResult(result_peg_618);
            } while (true);
            loop_peg_617:
            ;
            contents = result_peg_613.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_613.getPosition()))){
                    result_peg_613.nextPosition();
                } else {
                    goto out_peg_615;
                }
            }
            result_peg_613.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_613.setValue(value);
            }
        
        
    }
    
    if (column_peg_612.chunk2 == 0){
        column_peg_612.chunk2 = new Chunk2();
    }
    column_peg_612.chunk2->chunk_string = result_peg_613;
    stream.update(result_peg_613.getPosition());
    
    
    return result_peg_613;
    out_peg_615:
    
    if (column_peg_612.chunk2 == 0){
        column_peg_612.chunk2 = new Chunk2();
    }
    column_peg_612.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_628 = stream.getColumn(position);
    if (column_peg_628.chunk2 != 0 && column_peg_628.chunk2->chunk_range.calculated()){
        return column_peg_628.chunk2->chunk_range;
    }
    
    RuleTrace trace_peg_929(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_629(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_629.getPosition()))){
                    result_peg_629.nextPosition();
                } else {
                    goto out_peg_631;
                }
            }
            result_peg_629.setValue((void*) "[");
        
        
        
        {
                
                result_peg_629.reset();
                do{
                    Result result_peg_643(result_peg_629.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_643.getPosition()))){
                                result_peg_643.nextPosition();
                            } else {
                                goto out_peg_646;
                            }
                        }
                        result_peg_643.setValue((void*) " ");
                            
                    }
                    goto success_peg_644;
                    out_peg_646:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_643.getPosition()))){
                                result_peg_643.nextPosition();
                            } else {
                                goto out_peg_648;
                            }
                        }
                        result_peg_643.setValue((void*) "\t");
                            
                    }
                    goto success_peg_644;
                    out_peg_648:
                    goto loop_peg_642;
                    success_peg_644:
                    ;
                    result_peg_629.addResult(result_peg_643);
                } while (true);
                loop_peg_642:
                ;
                        
            }
            goto success_peg_633;
            goto out_peg_631;
            success_peg_633:
            ;
        
        
        
        result_peg_629 = rule_expr_c(stream, result_peg_629.getPosition());
            if (result_peg_629.error()){
                goto out_peg_631;
            }
            low = result_peg_629.getValues();
        
        
        
        {
                
                result_peg_629.reset();
                do{
                    Result result_peg_661(result_peg_629.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_661.getPosition()))){
                                result_peg_661.nextPosition();
                            } else {
                                goto out_peg_664;
                            }
                        }
                        result_peg_661.setValue((void*) " ");
                            
                    }
                    goto success_peg_662;
                    out_peg_664:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_661.getPosition()))){
                                result_peg_661.nextPosition();
                            } else {
                                goto out_peg_666;
                            }
                        }
                        result_peg_661.setValue((void*) "\t");
                            
                    }
                    goto success_peg_662;
                    out_peg_666:
                    goto loop_peg_660;
                    success_peg_662:
                    ;
                    result_peg_629.addResult(result_peg_661);
                } while (true);
                loop_peg_660:
                ;
                        
            }
            goto success_peg_651;
            goto out_peg_631;
            success_peg_651:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_629.getPosition()))){
                    result_peg_629.nextPosition();
                } else {
                    goto out_peg_631;
                }
            }
            result_peg_629.setValue((void*) ",");
        
        
        
        {
                
                result_peg_629.reset();
                do{
                    Result result_peg_679(result_peg_629.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_679.getPosition()))){
                                result_peg_679.nextPosition();
                            } else {
                                goto out_peg_682;
                            }
                        }
                        result_peg_679.setValue((void*) " ");
                            
                    }
                    goto success_peg_680;
                    out_peg_682:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_679.getPosition()))){
                                result_peg_679.nextPosition();
                            } else {
                                goto out_peg_684;
                            }
                        }
                        result_peg_679.setValue((void*) "\t");
                            
                    }
                    goto success_peg_680;
                    out_peg_684:
                    goto loop_peg_678;
                    success_peg_680:
                    ;
                    result_peg_629.addResult(result_peg_679);
                } while (true);
                loop_peg_678:
                ;
                        
            }
            goto success_peg_669;
            goto out_peg_631;
            success_peg_669:
            ;
        
        
        
        result_peg_629 = rule_expr_c(stream, result_peg_629.getPosition());
            if (result_peg_629.error()){
                goto out_peg_631;
            }
            high = result_peg_629.getValues();
        
        
        
        {
                
                result_peg_629.reset();
                do{
                    Result result_peg_697(result_peg_629.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_697.getPosition()))){
                                result_peg_697.nextPosition();
                            } else {
                                goto out_peg_700;
                            }
                        }
                        result_peg_697.setValue((void*) " ");
                            
                    }
                    goto success_peg_698;
                    out_peg_700:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_697.getPosition()))){
                                result_peg_697.nextPosition();
                            } else {
                                goto out_peg_702;
                            }
                        }
                        result_peg_697.setValue((void*) "\t");
                            
                    }
                    goto success_peg_698;
                    out_peg_702:
                    goto loop_peg_696;
                    success_peg_698:
                    ;
                    result_peg_629.addResult(result_peg_697);
                } while (true);
                loop_peg_696:
                ;
                        
            }
            goto success_peg_687;
            goto out_peg_631;
            success_peg_687:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_629.getPosition()))){
                    result_peg_629.nextPosition();
                } else {
                    goto out_peg_631;
                }
            }
            result_peg_629.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_629.setValue(value);
            }
        
        
    }
    
    if (column_peg_628.chunk2 == 0){
        column_peg_628.chunk2 = new Chunk2();
    }
    column_peg_628.chunk2->chunk_range = result_peg_629;
    stream.update(result_peg_629.getPosition());
    
    
    return result_peg_629;
    out_peg_631:
    Result result_peg_704(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_704.getPosition()))){
                    result_peg_704.nextPosition();
                } else {
                    goto out_peg_706;
                }
            }
            result_peg_704.setValue((void*) "(");
        
        
        
        {
                
                result_peg_704.reset();
                do{
                    Result result_peg_718(result_peg_704.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_718.getPosition()))){
                                result_peg_718.nextPosition();
                            } else {
                                goto out_peg_721;
                            }
                        }
                        result_peg_718.setValue((void*) " ");
                            
                    }
                    goto success_peg_719;
                    out_peg_721:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_718.getPosition()))){
                                result_peg_718.nextPosition();
                            } else {
                                goto out_peg_723;
                            }
                        }
                        result_peg_718.setValue((void*) "\t");
                            
                    }
                    goto success_peg_719;
                    out_peg_723:
                    goto loop_peg_717;
                    success_peg_719:
                    ;
                    result_peg_704.addResult(result_peg_718);
                } while (true);
                loop_peg_717:
                ;
                        
            }
            goto success_peg_708;
            goto out_peg_706;
            success_peg_708:
            ;
        
        
        
        result_peg_704 = rule_expr_c(stream, result_peg_704.getPosition());
            if (result_peg_704.error()){
                goto out_peg_706;
            }
            low = result_peg_704.getValues();
        
        
        
        {
                
                result_peg_704.reset();
                do{
                    Result result_peg_736(result_peg_704.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_736.getPosition()))){
                                result_peg_736.nextPosition();
                            } else {
                                goto out_peg_739;
                            }
                        }
                        result_peg_736.setValue((void*) " ");
                            
                    }
                    goto success_peg_737;
                    out_peg_739:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_736.getPosition()))){
                                result_peg_736.nextPosition();
                            } else {
                                goto out_peg_741;
                            }
                        }
                        result_peg_736.setValue((void*) "\t");
                            
                    }
                    goto success_peg_737;
                    out_peg_741:
                    goto loop_peg_735;
                    success_peg_737:
                    ;
                    result_peg_704.addResult(result_peg_736);
                } while (true);
                loop_peg_735:
                ;
                        
            }
            goto success_peg_726;
            goto out_peg_706;
            success_peg_726:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_704.getPosition()))){
                    result_peg_704.nextPosition();
                } else {
                    goto out_peg_706;
                }
            }
            result_peg_704.setValue((void*) ",");
        
        
        
        {
                
                result_peg_704.reset();
                do{
                    Result result_peg_754(result_peg_704.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_754.getPosition()))){
                                result_peg_754.nextPosition();
                            } else {
                                goto out_peg_757;
                            }
                        }
                        result_peg_754.setValue((void*) " ");
                            
                    }
                    goto success_peg_755;
                    out_peg_757:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_754.getPosition()))){
                                result_peg_754.nextPosition();
                            } else {
                                goto out_peg_759;
                            }
                        }
                        result_peg_754.setValue((void*) "\t");
                            
                    }
                    goto success_peg_755;
                    out_peg_759:
                    goto loop_peg_753;
                    success_peg_755:
                    ;
                    result_peg_704.addResult(result_peg_754);
                } while (true);
                loop_peg_753:
                ;
                        
            }
            goto success_peg_744;
            goto out_peg_706;
            success_peg_744:
            ;
        
        
        
        result_peg_704 = rule_expr_c(stream, result_peg_704.getPosition());
            if (result_peg_704.error()){
                goto out_peg_706;
            }
            high = result_peg_704.getValues();
        
        
        
        {
                
                result_peg_704.reset();
                do{
                    Result result_peg_772(result_peg_704.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_772.getPosition()))){
                                result_peg_772.nextPosition();
                            } else {
                                goto out_peg_775;
                            }
                        }
                        result_peg_772.setValue((void*) " ");
                            
                    }
                    goto success_peg_773;
                    out_peg_775:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_772.getPosition()))){
                                result_peg_772.nextPosition();
                            } else {
                                goto out_peg_777;
                            }
                        }
                        result_peg_772.setValue((void*) "\t");
                            
                    }
                    goto success_peg_773;
                    out_peg_777:
                    goto loop_peg_771;
                    success_peg_773:
                    ;
                    result_peg_704.addResult(result_peg_772);
                } while (true);
                loop_peg_771:
                ;
                        
            }
            goto success_peg_762;
            goto out_peg_706;
            success_peg_762:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_704.getPosition()))){
                    result_peg_704.nextPosition();
                } else {
                    goto out_peg_706;
                }
            }
            result_peg_704.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_704.setValue(value);
            }
        
        
    }
    
    if (column_peg_628.chunk2 == 0){
        column_peg_628.chunk2 = new Chunk2();
    }
    column_peg_628.chunk2->chunk_range = result_peg_704;
    stream.update(result_peg_704.getPosition());
    
    
    return result_peg_704;
    out_peg_706:
    Result result_peg_779(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_779.getPosition()))){
                    result_peg_779.nextPosition();
                } else {
                    goto out_peg_781;
                }
            }
            result_peg_779.setValue((void*) "(");
        
        
        
        {
                
                result_peg_779.reset();
                do{
                    Result result_peg_793(result_peg_779.getPosition());
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
                    goto loop_peg_792;
                    success_peg_794:
                    ;
                    result_peg_779.addResult(result_peg_793);
                } while (true);
                loop_peg_792:
                ;
                        
            }
            goto success_peg_783;
            goto out_peg_781;
            success_peg_783:
            ;
        
        
        
        result_peg_779 = rule_expr_c(stream, result_peg_779.getPosition());
            if (result_peg_779.error()){
                goto out_peg_781;
            }
            low = result_peg_779.getValues();
        
        
        
        {
                
                result_peg_779.reset();
                do{
                    Result result_peg_811(result_peg_779.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_811.getPosition()))){
                                result_peg_811.nextPosition();
                            } else {
                                goto out_peg_814;
                            }
                        }
                        result_peg_811.setValue((void*) " ");
                            
                    }
                    goto success_peg_812;
                    out_peg_814:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_811.getPosition()))){
                                result_peg_811.nextPosition();
                            } else {
                                goto out_peg_816;
                            }
                        }
                        result_peg_811.setValue((void*) "\t");
                            
                    }
                    goto success_peg_812;
                    out_peg_816:
                    goto loop_peg_810;
                    success_peg_812:
                    ;
                    result_peg_779.addResult(result_peg_811);
                } while (true);
                loop_peg_810:
                ;
                        
            }
            goto success_peg_801;
            goto out_peg_781;
            success_peg_801:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_779.getPosition()))){
                    result_peg_779.nextPosition();
                } else {
                    goto out_peg_781;
                }
            }
            result_peg_779.setValue((void*) ",");
        
        
        
        {
                
                result_peg_779.reset();
                do{
                    Result result_peg_829(result_peg_779.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_829.getPosition()))){
                                result_peg_829.nextPosition();
                            } else {
                                goto out_peg_832;
                            }
                        }
                        result_peg_829.setValue((void*) " ");
                            
                    }
                    goto success_peg_830;
                    out_peg_832:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_829.getPosition()))){
                                result_peg_829.nextPosition();
                            } else {
                                goto out_peg_834;
                            }
                        }
                        result_peg_829.setValue((void*) "\t");
                            
                    }
                    goto success_peg_830;
                    out_peg_834:
                    goto loop_peg_828;
                    success_peg_830:
                    ;
                    result_peg_779.addResult(result_peg_829);
                } while (true);
                loop_peg_828:
                ;
                        
            }
            goto success_peg_819;
            goto out_peg_781;
            success_peg_819:
            ;
        
        
        
        result_peg_779 = rule_expr_c(stream, result_peg_779.getPosition());
            if (result_peg_779.error()){
                goto out_peg_781;
            }
            high = result_peg_779.getValues();
        
        
        
        {
                
                result_peg_779.reset();
                do{
                    Result result_peg_847(result_peg_779.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_847.getPosition()))){
                                result_peg_847.nextPosition();
                            } else {
                                goto out_peg_850;
                            }
                        }
                        result_peg_847.setValue((void*) " ");
                            
                    }
                    goto success_peg_848;
                    out_peg_850:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_847.getPosition()))){
                                result_peg_847.nextPosition();
                            } else {
                                goto out_peg_852;
                            }
                        }
                        result_peg_847.setValue((void*) "\t");
                            
                    }
                    goto success_peg_848;
                    out_peg_852:
                    goto loop_peg_846;
                    success_peg_848:
                    ;
                    result_peg_779.addResult(result_peg_847);
                } while (true);
                loop_peg_846:
                ;
                        
            }
            goto success_peg_837;
            goto out_peg_781;
            success_peg_837:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_779.getPosition()))){
                    result_peg_779.nextPosition();
                } else {
                    goto out_peg_781;
                }
            }
            result_peg_779.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_779.setValue(value);
            }
        
        
    }
    
    if (column_peg_628.chunk2 == 0){
        column_peg_628.chunk2 = new Chunk2();
    }
    column_peg_628.chunk2->chunk_range = result_peg_779;
    stream.update(result_peg_779.getPosition());
    
    
    return result_peg_779;
    out_peg_781:
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
                if (compareChar(")"[i], stream.get(result_peg_854.getPosition()))){
                    result_peg_854.nextPosition();
                } else {
                    goto out_peg_856;
                }
            }
            result_peg_854.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_854.setValue(value);
            }
        
        
    }
    
    if (column_peg_628.chunk2 == 0){
        column_peg_628.chunk2 = new Chunk2();
    }
    column_peg_628.chunk2->chunk_range = result_peg_854;
    stream.update(result_peg_854.getPosition());
    
    
    return result_peg_854;
    out_peg_856:
    
    if (column_peg_628.chunk2 == 0){
        column_peg_628.chunk2 = new Chunk2();
    }
    column_peg_628.chunk2->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_930 = stream.getColumn(position);
    if (column_peg_930.chunk3 != 0 && column_peg_930.chunk3->chunk_name.calculated()){
        return column_peg_930.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_942(stream, "name");
    int myposition = position;
    
    
    Result result_peg_931(myposition);
    
    {
    
        {
                
                char letter_peg_937 = stream.get(result_peg_931.getPosition());
                if (letter_peg_937 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_937) != NULL){
                    result_peg_931.nextPosition();
                    result_peg_931.setValue((void*) (long) letter_peg_937);
                } else {
                    goto out_peg_936;
                }
                
            }
            goto success_peg_933;
            out_peg_936:
            goto out_peg_938;
            success_peg_933:
            ;
        
        Result result_peg_932 = result_peg_931;
        
        result_peg_931.reset();
            do{
                Result result_peg_941(result_peg_931.getPosition());
                result_peg_941 = rule_alpha_digit(stream, result_peg_941.getPosition());
                if (result_peg_941.error()){
                    goto loop_peg_940;
                }
                result_peg_931.addResult(result_peg_941);
            } while (true);
            loop_peg_940:
            ;
        
        Result result_peg_939 = result_peg_931;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_932.getValues().getValue(),result_peg_939.getValues());
                result_peg_931.setValue(value);
            }
        
        
    }
    
    if (column_peg_930.chunk3 == 0){
        column_peg_930.chunk3 = new Chunk3();
    }
    column_peg_930.chunk3->chunk_name = result_peg_931;
    stream.update(result_peg_931.getPosition());
    
    
    return result_peg_931;
    out_peg_938:
    
    if (column_peg_930.chunk3 == 0){
        column_peg_930.chunk3 = new Chunk3();
    }
    column_peg_930.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_943 = stream.getColumn(position);
    if (column_peg_943.chunk3 != 0 && column_peg_943.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_943.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_958(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_944(myposition);
    
    {
        
        char letter_peg_949 = stream.get(result_peg_944.getPosition());
        if (letter_peg_949 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_949) != NULL){
            result_peg_944.nextPosition();
            result_peg_944.setValue((void*) (long) letter_peg_949);
        } else {
            goto out_peg_948;
        }
        
    }
    goto success_peg_945;
    out_peg_948:
    goto out_peg_950;
    success_peg_945:
    ;
    
    if (column_peg_943.chunk3 == 0){
        column_peg_943.chunk3 = new Chunk3();
    }
    column_peg_943.chunk3->chunk_alpha_digit = result_peg_944;
    stream.update(result_peg_944.getPosition());
    
    
    return result_peg_944;
    out_peg_950:
    Result result_peg_951(myposition);
    
    {
        
        char letter_peg_956 = stream.get(result_peg_951.getPosition());
        if (letter_peg_956 != '\0' && strchr("0123456789", letter_peg_956) != NULL){
            result_peg_951.nextPosition();
            result_peg_951.setValue((void*) (long) letter_peg_956);
        } else {
            goto out_peg_955;
        }
        
    }
    goto success_peg_952;
    out_peg_955:
    goto out_peg_957;
    success_peg_952:
    ;
    
    if (column_peg_943.chunk3 == 0){
        column_peg_943.chunk3 = new Chunk3();
    }
    column_peg_943.chunk3->chunk_alpha_digit = result_peg_951;
    stream.update(result_peg_951.getPosition());
    
    
    return result_peg_951;
    out_peg_957:
    
    if (column_peg_943.chunk3 == 0){
        column_peg_943.chunk3 = new Chunk3();
    }
    column_peg_943.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_959 = stream.getColumn(position);
    if (column_peg_959.chunk3 != 0 && column_peg_959.chunk3->chunk_valuelist.calculated()){
        return column_peg_959.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_1077(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_960(myposition);
    
    {
    
        result_peg_960 = rule_expr_c(stream, result_peg_960.getPosition());
            if (result_peg_960.error()){
                goto out_peg_962;
            }
            first = result_peg_960.getValues();
        
        
        
        result_peg_960.reset();
            do{
                Result result_peg_965(result_peg_960.getPosition());
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
                        goto loop_peg_964;
                        success_peg_967:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_965.getPosition()))){
                                result_peg_965.nextPosition();
                            } else {
                                goto loop_peg_964;
                            }
                        }
                        result_peg_965.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_965.reset();
                            do{
                                Result result_peg_995(result_peg_965.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_995.getPosition()))){
                                            result_peg_995.nextPosition();
                                        } else {
                                            goto out_peg_998;
                                        }
                                    }
                                    result_peg_995.setValue((void*) " ");
                                        
                                }
                                goto success_peg_996;
                                out_peg_998:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_995.getPosition()))){
                                            result_peg_995.nextPosition();
                                        } else {
                                            goto out_peg_1000;
                                        }
                                    }
                                    result_peg_995.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_996;
                                out_peg_1000:
                                goto loop_peg_994;
                                success_peg_996:
                                ;
                                result_peg_965.addResult(result_peg_995);
                            } while (true);
                            loop_peg_994:
                            ;
                                    
                        }
                        goto success_peg_985;
                        goto loop_peg_964;
                        success_peg_985:
                        ;
                    
                    
                    
                    result_peg_965 = rule_expr_c(stream, result_peg_965.getPosition());
                        if (result_peg_965.error()){
                            goto loop_peg_964;
                        }
                    
                    
                }
                result_peg_960.addResult(result_peg_965);
            } while (true);
            loop_peg_964:
            if (result_peg_960.matches() == 0){
                goto out_peg_962;
            }
            rest = result_peg_960.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_960.setValue(value);
            }
        
        
    }
    
    if (column_peg_959.chunk3 == 0){
        column_peg_959.chunk3 = new Chunk3();
    }
    column_peg_959.chunk3->chunk_valuelist = result_peg_960;
    stream.update(result_peg_960.getPosition());
    
    
    return result_peg_960;
    out_peg_962:
    Result result_peg_1001(myposition);
    
    {
    
        {
                
                result_peg_1001.reset();
                do{
                    Result result_peg_1013(result_peg_1001.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1013.getPosition()))){
                                result_peg_1013.nextPosition();
                            } else {
                                goto out_peg_1016;
                            }
                        }
                        result_peg_1013.setValue((void*) " ");
                            
                    }
                    goto success_peg_1014;
                    out_peg_1016:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1013.getPosition()))){
                                result_peg_1013.nextPosition();
                            } else {
                                goto out_peg_1018;
                            }
                        }
                        result_peg_1013.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1014;
                    out_peg_1018:
                    goto loop_peg_1012;
                    success_peg_1014:
                    ;
                    result_peg_1001.addResult(result_peg_1013);
                } while (true);
                loop_peg_1012:
                ;
                        
            }
            goto success_peg_1003;
            goto out_peg_1019;
            success_peg_1003:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_1001.getPosition()))){
                    result_peg_1001.nextPosition();
                } else {
                    goto out_peg_1019;
                }
            }
            result_peg_1001.setValue((void*) ",");
        
        
        
        {
                
                result_peg_1001.reset();
                do{
                    Result result_peg_1032(result_peg_1001.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1032.getPosition()))){
                                result_peg_1032.nextPosition();
                            } else {
                                goto out_peg_1035;
                            }
                        }
                        result_peg_1032.setValue((void*) " ");
                            
                    }
                    goto success_peg_1033;
                    out_peg_1035:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1032.getPosition()))){
                                result_peg_1032.nextPosition();
                            } else {
                                goto out_peg_1037;
                            }
                        }
                        result_peg_1032.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1033;
                    out_peg_1037:
                    goto loop_peg_1031;
                    success_peg_1033:
                    ;
                    result_peg_1001.addResult(result_peg_1032);
                } while (true);
                loop_peg_1031:
                ;
                        
            }
            goto success_peg_1022;
            goto out_peg_1019;
            success_peg_1022:
            ;
        
        
        
        result_peg_1001 = rule_expr_c(stream, result_peg_1001.getPosition());
            if (result_peg_1001.error()){
                goto out_peg_1019;
            }
            first = result_peg_1001.getValues();
        
        
        
        result_peg_1001.reset();
            do{
                Result result_peg_1041(result_peg_1001.getPosition());
                {
                
                    {
                            
                            result_peg_1041.reset();
                            do{
                                Result result_peg_1053(result_peg_1041.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1053.getPosition()))){
                                            result_peg_1053.nextPosition();
                                        } else {
                                            goto out_peg_1056;
                                        }
                                    }
                                    result_peg_1053.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1054;
                                out_peg_1056:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1053.getPosition()))){
                                            result_peg_1053.nextPosition();
                                        } else {
                                            goto out_peg_1058;
                                        }
                                    }
                                    result_peg_1053.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1054;
                                out_peg_1058:
                                goto loop_peg_1052;
                                success_peg_1054:
                                ;
                                result_peg_1041.addResult(result_peg_1053);
                            } while (true);
                            loop_peg_1052:
                            ;
                                    
                        }
                        goto success_peg_1043;
                        goto loop_peg_1040;
                        success_peg_1043:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_1041.getPosition()))){
                                result_peg_1041.nextPosition();
                            } else {
                                goto loop_peg_1040;
                            }
                        }
                        result_peg_1041.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_1041.reset();
                            do{
                                Result result_peg_1071(result_peg_1041.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_1071.getPosition()))){
                                            result_peg_1071.nextPosition();
                                        } else {
                                            goto out_peg_1074;
                                        }
                                    }
                                    result_peg_1071.setValue((void*) " ");
                                        
                                }
                                goto success_peg_1072;
                                out_peg_1074:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_1071.getPosition()))){
                                            result_peg_1071.nextPosition();
                                        } else {
                                            goto out_peg_1076;
                                        }
                                    }
                                    result_peg_1071.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_1072;
                                out_peg_1076:
                                goto loop_peg_1070;
                                success_peg_1072:
                                ;
                                result_peg_1041.addResult(result_peg_1071);
                            } while (true);
                            loop_peg_1070:
                            ;
                                    
                        }
                        goto success_peg_1061;
                        goto loop_peg_1040;
                        success_peg_1061:
                        ;
                    
                    
                    
                    result_peg_1041 = rule_expr_c(stream, result_peg_1041.getPosition());
                        if (result_peg_1041.error()){
                            goto loop_peg_1040;
                        }
                    
                    
                }
                result_peg_1001.addResult(result_peg_1041);
            } while (true);
            loop_peg_1040:
            ;
            rest = result_peg_1001.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_1001.setValue(value);
            }
        
        
    }
    
    if (column_peg_959.chunk3 == 0){
        column_peg_959.chunk3 = new Chunk3();
    }
    column_peg_959.chunk3->chunk_valuelist = result_peg_1001;
    stream.update(result_peg_1001.getPosition());
    
    
    return result_peg_1001;
    out_peg_1019:
    
    if (column_peg_959.chunk3 == 0){
        column_peg_959.chunk3 = new Chunk3();
    }
    column_peg_959.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_1078 = stream.getColumn(position);
    if (column_peg_1078.chunk3 != 0 && column_peg_1078.chunk3->chunk_expr.calculated()){
        return column_peg_1078.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_1105(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_1079(myposition);
    
    {
    
        result_peg_1079 = rule_expr_c(stream, result_peg_1079.getPosition());
            if (result_peg_1079.error()){
                goto out_peg_1081;
            }
        
        Result result_peg_1080 = result_peg_1079;
        
        Result result_peg_1084(result_peg_1079);
            {
            
                {
                        
                        result_peg_1084.reset();
                        do{
                            Result result_peg_1096(result_peg_1084.getPosition());
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar(" "[i], stream.get(result_peg_1096.getPosition()))){
                                        result_peg_1096.nextPosition();
                                    } else {
                                        goto out_peg_1099;
                                    }
                                }
                                result_peg_1096.setValue((void*) " ");
                                    
                            }
                            goto success_peg_1097;
                            out_peg_1099:
                            {
                                
                                for (int i = 0; i < 1; i++){
                                    if (compareChar("\t"[i], stream.get(result_peg_1096.getPosition()))){
                                        result_peg_1096.nextPosition();
                                    } else {
                                        goto out_peg_1101;
                                    }
                                }
                                result_peg_1096.setValue((void*) "\t");
                                    
                            }
                            goto success_peg_1097;
                            out_peg_1101:
                            goto loop_peg_1095;
                            success_peg_1097:
                            ;
                            result_peg_1084.addResult(result_peg_1096);
                        } while (true);
                        loop_peg_1095:
                        ;
                                
                    }
                    goto success_peg_1086;
                    goto not_peg_1083;
                    success_peg_1086:
                    ;
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_1084.getPosition()))){
                            result_peg_1084.nextPosition();
                        } else {
                            goto not_peg_1083;
                        }
                    }
                    result_peg_1084.setValue((void*) ",");
                
                
            }
            goto out_peg_1081;
            not_peg_1083:
            result_peg_1079.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1080.getValues();
                result_peg_1079.setValue(value);
            }
        
        
    }
    
    if (column_peg_1078.chunk3 == 0){
        column_peg_1078.chunk3 = new Chunk3();
    }
    column_peg_1078.chunk3->chunk_expr = result_peg_1079;
    stream.update(result_peg_1079.getPosition());
    
    
    return result_peg_1079;
    out_peg_1081:
    Result result_peg_1102(myposition);
    
    {
    
        result_peg_1102 = rule_valuelist(stream, result_peg_1102.getPosition());
            if (result_peg_1102.error()){
                goto out_peg_1104;
            }
        
        Result result_peg_1103 = result_peg_1102;
        
        result_peg_1102 = rule_expr2_rest(stream, result_peg_1102.getPosition(), result_peg_1103.getValues());
            if (result_peg_1102.error()){
                goto out_peg_1104;
            }
        
        
    }
    
    if (column_peg_1078.chunk3 == 0){
        column_peg_1078.chunk3 = new Chunk3();
    }
    column_peg_1078.chunk3->chunk_expr = result_peg_1102;
    stream.update(result_peg_1102.getPosition());
    
    
    return result_peg_1102;
    out_peg_1104:
    
    if (column_peg_1078.chunk3 == 0){
        column_peg_1078.chunk3 = new Chunk3();
    }
    column_peg_1078.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_1106 = stream.getColumn(position);
    if (column_peg_1106.chunk3 != 0 && column_peg_1106.chunk3->chunk_expr_c.calculated()){
        return column_peg_1106.chunk3->chunk_expr_c;
    }
    
    RuleTrace trace_peg_1110(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_1107(myposition);
    
    {
    
        result_peg_1107 = rule_expr2(stream, result_peg_1107.getPosition());
            if (result_peg_1107.error()){
                goto out_peg_1109;
            }
            left = result_peg_1107.getValues();
        
        
        
        result_peg_1107 = rule_expr_rest(stream, result_peg_1107.getPosition(), left);
            if (result_peg_1107.error()){
                goto out_peg_1109;
            }
        
        
    }
    
    if (column_peg_1106.chunk3 == 0){
        column_peg_1106.chunk3 = new Chunk3();
    }
    column_peg_1106.chunk3->chunk_expr_c = result_peg_1107;
    stream.update(result_peg_1107.getPosition());
    
    
    return result_peg_1107;
    out_peg_1109:
    
    if (column_peg_1106.chunk3 == 0){
        column_peg_1106.chunk3 = new Chunk3();
    }
    column_peg_1106.chunk3->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_1111 = stream.getColumn(position);
    if (column_peg_1111.chunk4 != 0 && column_peg_1111.chunk4->chunk_expr2.calculated()){
        return column_peg_1111.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_1115(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_1112(myposition);
    
    {
    
        result_peg_1112 = rule_expr3(stream, result_peg_1112.getPosition());
            if (result_peg_1112.error()){
                goto out_peg_1114;
            }
            left = result_peg_1112.getValues();
        
        
        
        result_peg_1112 = rule_expr2_rest(stream, result_peg_1112.getPosition(), left);
            if (result_peg_1112.error()){
                goto out_peg_1114;
            }
        
        
    }
    
    if (column_peg_1111.chunk4 == 0){
        column_peg_1111.chunk4 = new Chunk4();
    }
    column_peg_1111.chunk4->chunk_expr2 = result_peg_1112;
    stream.update(result_peg_1112.getPosition());
    
    
    return result_peg_1112;
    out_peg_1114:
    
    if (column_peg_1111.chunk4 == 0){
        column_peg_1111.chunk4 = new Chunk4();
    }
    column_peg_1111.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_1116 = stream.getColumn(position);
    if (column_peg_1116.chunk4 != 0 && column_peg_1116.chunk4->chunk_expr3.calculated()){
        return column_peg_1116.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_1120(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_1117(myposition);
    
    {
    
        result_peg_1117 = rule_expr4(stream, result_peg_1117.getPosition());
            if (result_peg_1117.error()){
                goto out_peg_1119;
            }
            left = result_peg_1117.getValues();
        
        
        
        result_peg_1117 = rule_expr3_rest(stream, result_peg_1117.getPosition(), left);
            if (result_peg_1117.error()){
                goto out_peg_1119;
            }
        
        
    }
    
    if (column_peg_1116.chunk4 == 0){
        column_peg_1116.chunk4 = new Chunk4();
    }
    column_peg_1116.chunk4->chunk_expr3 = result_peg_1117;
    stream.update(result_peg_1117.getPosition());
    
    
    return result_peg_1117;
    out_peg_1119:
    
    if (column_peg_1116.chunk4 == 0){
        column_peg_1116.chunk4 = new Chunk4();
    }
    column_peg_1116.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_1121 = stream.getColumn(position);
    if (column_peg_1121.chunk4 != 0 && column_peg_1121.chunk4->chunk_expr4.calculated()){
        return column_peg_1121.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_1125(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_1122(myposition);
    
    {
    
        result_peg_1122 = rule_expr5(stream, result_peg_1122.getPosition());
            if (result_peg_1122.error()){
                goto out_peg_1124;
            }
            left = result_peg_1122.getValues();
        
        
        
        result_peg_1122 = rule_expr4_rest(stream, result_peg_1122.getPosition(), left);
            if (result_peg_1122.error()){
                goto out_peg_1124;
            }
        
        
    }
    
    if (column_peg_1121.chunk4 == 0){
        column_peg_1121.chunk4 = new Chunk4();
    }
    column_peg_1121.chunk4->chunk_expr4 = result_peg_1122;
    stream.update(result_peg_1122.getPosition());
    
    
    return result_peg_1122;
    out_peg_1124:
    
    if (column_peg_1121.chunk4 == 0){
        column_peg_1121.chunk4 = new Chunk4();
    }
    column_peg_1121.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_1126 = stream.getColumn(position);
    if (column_peg_1126.chunk4 != 0 && column_peg_1126.chunk4->chunk_expr5.calculated()){
        return column_peg_1126.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_1130(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_1127(myposition);
    
    {
    
        result_peg_1127 = rule_expr6(stream, result_peg_1127.getPosition());
            if (result_peg_1127.error()){
                goto out_peg_1129;
            }
            left = result_peg_1127.getValues();
        
        
        
        result_peg_1127 = rule_expr5_rest(stream, result_peg_1127.getPosition(), left);
            if (result_peg_1127.error()){
                goto out_peg_1129;
            }
        
        
    }
    
    if (column_peg_1126.chunk4 == 0){
        column_peg_1126.chunk4 = new Chunk4();
    }
    column_peg_1126.chunk4->chunk_expr5 = result_peg_1127;
    stream.update(result_peg_1127.getPosition());
    
    
    return result_peg_1127;
    out_peg_1129:
    
    if (column_peg_1126.chunk4 == 0){
        column_peg_1126.chunk4 = new Chunk4();
    }
    column_peg_1126.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_1131 = stream.getColumn(position);
    if (column_peg_1131.chunk4 != 0 && column_peg_1131.chunk4->chunk_expr6.calculated()){
        return column_peg_1131.chunk4->chunk_expr6;
    }
    
    RuleTrace trace_peg_1135(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_1132(myposition);
    
    {
    
        result_peg_1132 = rule_expr7(stream, result_peg_1132.getPosition());
            if (result_peg_1132.error()){
                goto out_peg_1134;
            }
            left = result_peg_1132.getValues();
        
        
        
        result_peg_1132 = rule_expr6_rest(stream, result_peg_1132.getPosition(), left);
            if (result_peg_1132.error()){
                goto out_peg_1134;
            }
        
        
    }
    
    if (column_peg_1131.chunk4 == 0){
        column_peg_1131.chunk4 = new Chunk4();
    }
    column_peg_1131.chunk4->chunk_expr6 = result_peg_1132;
    stream.update(result_peg_1132.getPosition());
    
    
    return result_peg_1132;
    out_peg_1134:
    
    if (column_peg_1131.chunk4 == 0){
        column_peg_1131.chunk4 = new Chunk4();
    }
    column_peg_1131.chunk4->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_1136 = stream.getColumn(position);
    if (column_peg_1136.chunk5 != 0 && column_peg_1136.chunk5->chunk_expr7.calculated()){
        return column_peg_1136.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_1140(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_1137(myposition);
    
    {
    
        result_peg_1137 = rule_expr8(stream, result_peg_1137.getPosition());
            if (result_peg_1137.error()){
                goto out_peg_1139;
            }
            left = result_peg_1137.getValues();
        
        
        
        result_peg_1137 = rule_expr7_rest(stream, result_peg_1137.getPosition(), left);
            if (result_peg_1137.error()){
                goto out_peg_1139;
            }
        
        
    }
    
    if (column_peg_1136.chunk5 == 0){
        column_peg_1136.chunk5 = new Chunk5();
    }
    column_peg_1136.chunk5->chunk_expr7 = result_peg_1137;
    stream.update(result_peg_1137.getPosition());
    
    
    return result_peg_1137;
    out_peg_1139:
    
    if (column_peg_1136.chunk5 == 0){
        column_peg_1136.chunk5 = new Chunk5();
    }
    column_peg_1136.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_1141 = stream.getColumn(position);
    if (column_peg_1141.chunk5 != 0 && column_peg_1141.chunk5->chunk_expr8.calculated()){
        return column_peg_1141.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_1145(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_1142(myposition);
    
    {
    
        result_peg_1142 = rule_expr9(stream, result_peg_1142.getPosition());
            if (result_peg_1142.error()){
                goto out_peg_1144;
            }
            left = result_peg_1142.getValues();
        
        
        
        result_peg_1142 = rule_expr8_rest(stream, result_peg_1142.getPosition(), left);
            if (result_peg_1142.error()){
                goto out_peg_1144;
            }
        
        
    }
    
    if (column_peg_1141.chunk5 == 0){
        column_peg_1141.chunk5 = new Chunk5();
    }
    column_peg_1141.chunk5->chunk_expr8 = result_peg_1142;
    stream.update(result_peg_1142.getPosition());
    
    
    return result_peg_1142;
    out_peg_1144:
    
    if (column_peg_1141.chunk5 == 0){
        column_peg_1141.chunk5 = new Chunk5();
    }
    column_peg_1141.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_1146 = stream.getColumn(position);
    if (column_peg_1146.chunk5 != 0 && column_peg_1146.chunk5->chunk_expr9.calculated()){
        return column_peg_1146.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_1150(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_1147(myposition);
    
    {
    
        result_peg_1147 = rule_expr10(stream, result_peg_1147.getPosition());
            if (result_peg_1147.error()){
                goto out_peg_1149;
            }
            left = result_peg_1147.getValues();
        
        
        
        result_peg_1147 = rule_expr9_rest(stream, result_peg_1147.getPosition(), left);
            if (result_peg_1147.error()){
                goto out_peg_1149;
            }
        
        
    }
    
    if (column_peg_1146.chunk5 == 0){
        column_peg_1146.chunk5 = new Chunk5();
    }
    column_peg_1146.chunk5->chunk_expr9 = result_peg_1147;
    stream.update(result_peg_1147.getPosition());
    
    
    return result_peg_1147;
    out_peg_1149:
    
    if (column_peg_1146.chunk5 == 0){
        column_peg_1146.chunk5 = new Chunk5();
    }
    column_peg_1146.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_1151 = stream.getColumn(position);
    if (column_peg_1151.chunk5 != 0 && column_peg_1151.chunk5->chunk_expr10.calculated()){
        return column_peg_1151.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_1155(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_1152(myposition);
    
    {
    
        result_peg_1152 = rule_expr11(stream, result_peg_1152.getPosition());
            if (result_peg_1152.error()){
                goto out_peg_1154;
            }
            left = result_peg_1152.getValues();
        
        
        
        result_peg_1152 = rule_expr10_rest(stream, result_peg_1152.getPosition(), left);
            if (result_peg_1152.error()){
                goto out_peg_1154;
            }
        
        
    }
    
    if (column_peg_1151.chunk5 == 0){
        column_peg_1151.chunk5 = new Chunk5();
    }
    column_peg_1151.chunk5->chunk_expr10 = result_peg_1152;
    stream.update(result_peg_1152.getPosition());
    
    
    return result_peg_1152;
    out_peg_1154:
    
    if (column_peg_1151.chunk5 == 0){
        column_peg_1151.chunk5 = new Chunk5();
    }
    column_peg_1151.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_1156 = stream.getColumn(position);
    if (column_peg_1156.chunk5 != 0 && column_peg_1156.chunk5->chunk_expr11.calculated()){
        return column_peg_1156.chunk5->chunk_expr11;
    }
    
    RuleTrace trace_peg_1160(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_1157(myposition);
    
    {
    
        result_peg_1157 = rule_expr12(stream, result_peg_1157.getPosition());
            if (result_peg_1157.error()){
                goto out_peg_1159;
            }
            left = result_peg_1157.getValues();
        
        
        
        result_peg_1157 = rule_expr11_rest(stream, result_peg_1157.getPosition(), left);
            if (result_peg_1157.error()){
                goto out_peg_1159;
            }
        
        
    }
    
    if (column_peg_1156.chunk5 == 0){
        column_peg_1156.chunk5 = new Chunk5();
    }
    column_peg_1156.chunk5->chunk_expr11 = result_peg_1157;
    stream.update(result_peg_1157.getPosition());
    
    
    return result_peg_1157;
    out_peg_1159:
    
    if (column_peg_1156.chunk5 == 0){
        column_peg_1156.chunk5 = new Chunk5();
    }
    column_peg_1156.chunk5->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_1161 = stream.getColumn(position);
    if (column_peg_1161.chunk6 != 0 && column_peg_1161.chunk6->chunk_expr12.calculated()){
        return column_peg_1161.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_1165(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_1162(myposition);
    
    {
    
        result_peg_1162 = rule_expr13(stream, result_peg_1162.getPosition());
            if (result_peg_1162.error()){
                goto out_peg_1164;
            }
            left = result_peg_1162.getValues();
        
        
        
        result_peg_1162 = rule_expr12_rest(stream, result_peg_1162.getPosition(), left);
            if (result_peg_1162.error()){
                goto out_peg_1164;
            }
        
        
    }
    
    if (column_peg_1161.chunk6 == 0){
        column_peg_1161.chunk6 = new Chunk6();
    }
    column_peg_1161.chunk6->chunk_expr12 = result_peg_1162;
    stream.update(result_peg_1162.getPosition());
    
    
    return result_peg_1162;
    out_peg_1164:
    
    if (column_peg_1161.chunk6 == 0){
        column_peg_1161.chunk6 = new Chunk6();
    }
    column_peg_1161.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_1166 = stream.getColumn(position);
    if (column_peg_1166.chunk6 != 0 && column_peg_1166.chunk6->chunk_expr13.calculated()){
        return column_peg_1166.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_1173(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_1167(myposition);
    
    {
    
        result_peg_1167.reset();
            do{
                Result result_peg_1170(result_peg_1167.getPosition());
                result_peg_1170 = rule_unary(stream, result_peg_1170.getPosition());
                if (result_peg_1170.error()){
                    goto loop_peg_1169;
                }
                result_peg_1167.addResult(result_peg_1170);
            } while (true);
            loop_peg_1169:
            ;
        
        Result result_peg_1168 = result_peg_1167;
        
        result_peg_1167 = rule_expr13_real(stream, result_peg_1167.getPosition());
            if (result_peg_1167.error()){
                goto out_peg_1172;
            }
            exp = result_peg_1167.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_1168.getValues(), exp);
                result_peg_1167.setValue(value);
            }
        
        
    }
    
    if (column_peg_1166.chunk6 == 0){
        column_peg_1166.chunk6 = new Chunk6();
    }
    column_peg_1166.chunk6->chunk_expr13 = result_peg_1167;
    stream.update(result_peg_1167.getPosition());
    
    
    return result_peg_1167;
    out_peg_1172:
    
    if (column_peg_1166.chunk6 == 0){
        column_peg_1166.chunk6 = new Chunk6();
    }
    column_peg_1166.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1217(stream, "expr_rest");
    int myposition = position;
    tail_peg_1176:
    Value right;
    Value new_left;
    Result result_peg_1175(myposition);
    {
    
        {
                
                result_peg_1175.reset();
                do{
                    Result result_peg_1188(result_peg_1175.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1188.getPosition()))){
                                result_peg_1188.nextPosition();
                            } else {
                                goto out_peg_1191;
                            }
                        }
                        result_peg_1188.setValue((void*) " ");
                            
                    }
                    goto success_peg_1189;
                    out_peg_1191:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1188.getPosition()))){
                                result_peg_1188.nextPosition();
                            } else {
                                goto out_peg_1193;
                            }
                        }
                        result_peg_1188.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1189;
                    out_peg_1193:
                    goto loop_peg_1187;
                    success_peg_1189:
                    ;
                    result_peg_1175.addResult(result_peg_1188);
                } while (true);
                loop_peg_1187:
                ;
                        
            }
            goto success_peg_1178;
            goto out_peg_1194;
            success_peg_1178:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_1175.getPosition()))){
                    result_peg_1175.nextPosition();
                } else {
                    goto out_peg_1194;
                }
            }
            result_peg_1175.setValue((void*) "||");
        
        
        
        {
                
                result_peg_1175.reset();
                do{
                    Result result_peg_1207(result_peg_1175.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1207.getPosition()))){
                                result_peg_1207.nextPosition();
                            } else {
                                goto out_peg_1210;
                            }
                        }
                        result_peg_1207.setValue((void*) " ");
                            
                    }
                    goto success_peg_1208;
                    out_peg_1210:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1207.getPosition()))){
                                result_peg_1207.nextPosition();
                            } else {
                                goto out_peg_1212;
                            }
                        }
                        result_peg_1207.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1208;
                    out_peg_1212:
                    goto loop_peg_1206;
                    success_peg_1208:
                    ;
                    result_peg_1175.addResult(result_peg_1207);
                } while (true);
                loop_peg_1206:
                ;
                        
            }
            goto success_peg_1197;
            goto out_peg_1194;
            success_peg_1197:
            ;
        
        
        
        result_peg_1175 = rule_expr2(stream, result_peg_1175.getPosition());
            if (result_peg_1175.error()){
                goto out_peg_1194;
            }
            right = result_peg_1175.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_1175.setValue(value);
            }
            new_left = result_peg_1175.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1175.getPosition();
    goto tail_peg_1176;
    out_peg_1194:
    Result result_peg_1215(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1215.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1215.getPosition());
    
    return result_peg_1215;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1261(stream, "expr2_rest");
    int myposition = position;
    tail_peg_1220:
    Value right;
    Value new_left;
    Result result_peg_1219(myposition);
    {
    
        {
                
                result_peg_1219.reset();
                do{
                    Result result_peg_1232(result_peg_1219.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1232.getPosition()))){
                                result_peg_1232.nextPosition();
                            } else {
                                goto out_peg_1235;
                            }
                        }
                        result_peg_1232.setValue((void*) " ");
                            
                    }
                    goto success_peg_1233;
                    out_peg_1235:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1232.getPosition()))){
                                result_peg_1232.nextPosition();
                            } else {
                                goto out_peg_1237;
                            }
                        }
                        result_peg_1232.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1233;
                    out_peg_1237:
                    goto loop_peg_1231;
                    success_peg_1233:
                    ;
                    result_peg_1219.addResult(result_peg_1232);
                } while (true);
                loop_peg_1231:
                ;
                        
            }
            goto success_peg_1222;
            goto out_peg_1238;
            success_peg_1222:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_1219.getPosition()))){
                    result_peg_1219.nextPosition();
                } else {
                    goto out_peg_1238;
                }
            }
            result_peg_1219.setValue((void*) "^^");
        
        
        
        {
                
                result_peg_1219.reset();
                do{
                    Result result_peg_1251(result_peg_1219.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1251.getPosition()))){
                                result_peg_1251.nextPosition();
                            } else {
                                goto out_peg_1254;
                            }
                        }
                        result_peg_1251.setValue((void*) " ");
                            
                    }
                    goto success_peg_1252;
                    out_peg_1254:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1251.getPosition()))){
                                result_peg_1251.nextPosition();
                            } else {
                                goto out_peg_1256;
                            }
                        }
                        result_peg_1251.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1252;
                    out_peg_1256:
                    goto loop_peg_1250;
                    success_peg_1252:
                    ;
                    result_peg_1219.addResult(result_peg_1251);
                } while (true);
                loop_peg_1250:
                ;
                        
            }
            goto success_peg_1241;
            goto out_peg_1238;
            success_peg_1241:
            ;
        
        
        
        result_peg_1219 = rule_expr3(stream, result_peg_1219.getPosition());
            if (result_peg_1219.error()){
                goto out_peg_1238;
            }
            right = result_peg_1219.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_1219.setValue(value);
            }
            new_left = result_peg_1219.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1219.getPosition();
    goto tail_peg_1220;
    out_peg_1238:
    Result result_peg_1259(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1259.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1259.getPosition());
    
    return result_peg_1259;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1305(stream, "expr3_rest");
    int myposition = position;
    tail_peg_1264:
    Value right;
    Value new_left;
    Result result_peg_1263(myposition);
    {
    
        {
                
                result_peg_1263.reset();
                do{
                    Result result_peg_1276(result_peg_1263.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1276.getPosition()))){
                                result_peg_1276.nextPosition();
                            } else {
                                goto out_peg_1279;
                            }
                        }
                        result_peg_1276.setValue((void*) " ");
                            
                    }
                    goto success_peg_1277;
                    out_peg_1279:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1276.getPosition()))){
                                result_peg_1276.nextPosition();
                            } else {
                                goto out_peg_1281;
                            }
                        }
                        result_peg_1276.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1277;
                    out_peg_1281:
                    goto loop_peg_1275;
                    success_peg_1277:
                    ;
                    result_peg_1263.addResult(result_peg_1276);
                } while (true);
                loop_peg_1275:
                ;
                        
            }
            goto success_peg_1266;
            goto out_peg_1282;
            success_peg_1266:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_1263.getPosition()))){
                    result_peg_1263.nextPosition();
                } else {
                    goto out_peg_1282;
                }
            }
            result_peg_1263.setValue((void*) "&&");
        
        
        
        {
                
                result_peg_1263.reset();
                do{
                    Result result_peg_1295(result_peg_1263.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1295.getPosition()))){
                                result_peg_1295.nextPosition();
                            } else {
                                goto out_peg_1298;
                            }
                        }
                        result_peg_1295.setValue((void*) " ");
                            
                    }
                    goto success_peg_1296;
                    out_peg_1298:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1295.getPosition()))){
                                result_peg_1295.nextPosition();
                            } else {
                                goto out_peg_1300;
                            }
                        }
                        result_peg_1295.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1296;
                    out_peg_1300:
                    goto loop_peg_1294;
                    success_peg_1296:
                    ;
                    result_peg_1263.addResult(result_peg_1295);
                } while (true);
                loop_peg_1294:
                ;
                        
            }
            goto success_peg_1285;
            goto out_peg_1282;
            success_peg_1285:
            ;
        
        
        
        result_peg_1263 = rule_expr4(stream, result_peg_1263.getPosition());
            if (result_peg_1263.error()){
                goto out_peg_1282;
            }
            right = result_peg_1263.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_1263.setValue(value);
            }
            new_left = result_peg_1263.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1263.getPosition();
    goto tail_peg_1264;
    out_peg_1282:
    Result result_peg_1303(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1303.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1303.getPosition());
    
    return result_peg_1303;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1349(stream, "expr4_rest");
    int myposition = position;
    tail_peg_1308:
    Value right;
    Value new_left;
    Result result_peg_1307(myposition);
    {
    
        {
                
                result_peg_1307.reset();
                do{
                    Result result_peg_1320(result_peg_1307.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1320.getPosition()))){
                                result_peg_1320.nextPosition();
                            } else {
                                goto out_peg_1323;
                            }
                        }
                        result_peg_1320.setValue((void*) " ");
                            
                    }
                    goto success_peg_1321;
                    out_peg_1323:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1320.getPosition()))){
                                result_peg_1320.nextPosition();
                            } else {
                                goto out_peg_1325;
                            }
                        }
                        result_peg_1320.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1321;
                    out_peg_1325:
                    goto loop_peg_1319;
                    success_peg_1321:
                    ;
                    result_peg_1307.addResult(result_peg_1320);
                } while (true);
                loop_peg_1319:
                ;
                        
            }
            goto success_peg_1310;
            goto out_peg_1326;
            success_peg_1310:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_1307.getPosition()))){
                    result_peg_1307.nextPosition();
                } else {
                    goto out_peg_1326;
                }
            }
            result_peg_1307.setValue((void*) "|");
        
        
        
        {
                
                result_peg_1307.reset();
                do{
                    Result result_peg_1339(result_peg_1307.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1339.getPosition()))){
                                result_peg_1339.nextPosition();
                            } else {
                                goto out_peg_1342;
                            }
                        }
                        result_peg_1339.setValue((void*) " ");
                            
                    }
                    goto success_peg_1340;
                    out_peg_1342:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1339.getPosition()))){
                                result_peg_1339.nextPosition();
                            } else {
                                goto out_peg_1344;
                            }
                        }
                        result_peg_1339.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1340;
                    out_peg_1344:
                    goto loop_peg_1338;
                    success_peg_1340:
                    ;
                    result_peg_1307.addResult(result_peg_1339);
                } while (true);
                loop_peg_1338:
                ;
                        
            }
            goto success_peg_1329;
            goto out_peg_1326;
            success_peg_1329:
            ;
        
        
        
        result_peg_1307 = rule_expr5(stream, result_peg_1307.getPosition());
            if (result_peg_1307.error()){
                goto out_peg_1326;
            }
            right = result_peg_1307.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_1307.setValue(value);
            }
            new_left = result_peg_1307.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1307.getPosition();
    goto tail_peg_1308;
    out_peg_1326:
    Result result_peg_1347(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1347.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1347.getPosition());
    
    return result_peg_1347;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1393(stream, "expr5_rest");
    int myposition = position;
    tail_peg_1352:
    Value right;
    Value new_left;
    Result result_peg_1351(myposition);
    {
    
        {
                
                result_peg_1351.reset();
                do{
                    Result result_peg_1364(result_peg_1351.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1364.getPosition()))){
                                result_peg_1364.nextPosition();
                            } else {
                                goto out_peg_1367;
                            }
                        }
                        result_peg_1364.setValue((void*) " ");
                            
                    }
                    goto success_peg_1365;
                    out_peg_1367:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1364.getPosition()))){
                                result_peg_1364.nextPosition();
                            } else {
                                goto out_peg_1369;
                            }
                        }
                        result_peg_1364.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1365;
                    out_peg_1369:
                    goto loop_peg_1363;
                    success_peg_1365:
                    ;
                    result_peg_1351.addResult(result_peg_1364);
                } while (true);
                loop_peg_1363:
                ;
                        
            }
            goto success_peg_1354;
            goto out_peg_1370;
            success_peg_1354:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_1351.getPosition()))){
                    result_peg_1351.nextPosition();
                } else {
                    goto out_peg_1370;
                }
            }
            result_peg_1351.setValue((void*) "^");
        
        
        
        {
                
                result_peg_1351.reset();
                do{
                    Result result_peg_1383(result_peg_1351.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1383.getPosition()))){
                                result_peg_1383.nextPosition();
                            } else {
                                goto out_peg_1386;
                            }
                        }
                        result_peg_1383.setValue((void*) " ");
                            
                    }
                    goto success_peg_1384;
                    out_peg_1386:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1383.getPosition()))){
                                result_peg_1383.nextPosition();
                            } else {
                                goto out_peg_1388;
                            }
                        }
                        result_peg_1383.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1384;
                    out_peg_1388:
                    goto loop_peg_1382;
                    success_peg_1384:
                    ;
                    result_peg_1351.addResult(result_peg_1383);
                } while (true);
                loop_peg_1382:
                ;
                        
            }
            goto success_peg_1373;
            goto out_peg_1370;
            success_peg_1373:
            ;
        
        
        
        result_peg_1351 = rule_expr6(stream, result_peg_1351.getPosition());
            if (result_peg_1351.error()){
                goto out_peg_1370;
            }
            right = result_peg_1351.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_1351.setValue(value);
            }
            new_left = result_peg_1351.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1351.getPosition();
    goto tail_peg_1352;
    out_peg_1370:
    Result result_peg_1391(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1391.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1391.getPosition());
    
    return result_peg_1391;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1437(stream, "expr6_rest");
    int myposition = position;
    tail_peg_1396:
    Value right;
    Value new_left;
    Result result_peg_1395(myposition);
    {
    
        {
                
                result_peg_1395.reset();
                do{
                    Result result_peg_1408(result_peg_1395.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1408.getPosition()))){
                                result_peg_1408.nextPosition();
                            } else {
                                goto out_peg_1411;
                            }
                        }
                        result_peg_1408.setValue((void*) " ");
                            
                    }
                    goto success_peg_1409;
                    out_peg_1411:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1408.getPosition()))){
                                result_peg_1408.nextPosition();
                            } else {
                                goto out_peg_1413;
                            }
                        }
                        result_peg_1408.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1409;
                    out_peg_1413:
                    goto loop_peg_1407;
                    success_peg_1409:
                    ;
                    result_peg_1395.addResult(result_peg_1408);
                } while (true);
                loop_peg_1407:
                ;
                        
            }
            goto success_peg_1398;
            goto out_peg_1414;
            success_peg_1398:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_1395.getPosition()))){
                    result_peg_1395.nextPosition();
                } else {
                    goto out_peg_1414;
                }
            }
            result_peg_1395.setValue((void*) "&");
        
        
        
        {
                
                result_peg_1395.reset();
                do{
                    Result result_peg_1427(result_peg_1395.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1427.getPosition()))){
                                result_peg_1427.nextPosition();
                            } else {
                                goto out_peg_1430;
                            }
                        }
                        result_peg_1427.setValue((void*) " ");
                            
                    }
                    goto success_peg_1428;
                    out_peg_1430:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1427.getPosition()))){
                                result_peg_1427.nextPosition();
                            } else {
                                goto out_peg_1432;
                            }
                        }
                        result_peg_1427.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1428;
                    out_peg_1432:
                    goto loop_peg_1426;
                    success_peg_1428:
                    ;
                    result_peg_1395.addResult(result_peg_1427);
                } while (true);
                loop_peg_1426:
                ;
                        
            }
            goto success_peg_1417;
            goto out_peg_1414;
            success_peg_1417:
            ;
        
        
        
        result_peg_1395 = rule_expr7(stream, result_peg_1395.getPosition());
            if (result_peg_1395.error()){
                goto out_peg_1414;
            }
            right = result_peg_1395.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_1395.setValue(value);
            }
            new_left = result_peg_1395.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1395.getPosition();
    goto tail_peg_1396;
    out_peg_1414:
    Result result_peg_1435(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1435.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1435.getPosition());
    
    return result_peg_1435;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1481(stream, "expr7_rest");
    int myposition = position;
    tail_peg_1440:
    Value right;
    Value new_left;
    Result result_peg_1439(myposition);
    {
    
        {
                
                result_peg_1439.reset();
                do{
                    Result result_peg_1452(result_peg_1439.getPosition());
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
                    result_peg_1439.addResult(result_peg_1452);
                } while (true);
                loop_peg_1451:
                ;
                        
            }
            goto success_peg_1442;
            goto out_peg_1458;
            success_peg_1442:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_1439.getPosition()))){
                    result_peg_1439.nextPosition();
                } else {
                    goto out_peg_1458;
                }
            }
            result_peg_1439.setValue((void*) ":=");
        
        
        
        {
                
                result_peg_1439.reset();
                do{
                    Result result_peg_1471(result_peg_1439.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1471.getPosition()))){
                                result_peg_1471.nextPosition();
                            } else {
                                goto out_peg_1474;
                            }
                        }
                        result_peg_1471.setValue((void*) " ");
                            
                    }
                    goto success_peg_1472;
                    out_peg_1474:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1471.getPosition()))){
                                result_peg_1471.nextPosition();
                            } else {
                                goto out_peg_1476;
                            }
                        }
                        result_peg_1471.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1472;
                    out_peg_1476:
                    goto loop_peg_1470;
                    success_peg_1472:
                    ;
                    result_peg_1439.addResult(result_peg_1471);
                } while (true);
                loop_peg_1470:
                ;
                        
            }
            goto success_peg_1461;
            goto out_peg_1458;
            success_peg_1461:
            ;
        
        
        
        result_peg_1439 = rule_expr8(stream, result_peg_1439.getPosition());
            if (result_peg_1439.error()){
                goto out_peg_1458;
            }
            right = result_peg_1439.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_1439.setValue(value);
            }
            new_left = result_peg_1439.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1439.getPosition();
    goto tail_peg_1440;
    out_peg_1458:
    Result result_peg_1479(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1479.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1479.getPosition());
    
    return result_peg_1479;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1564(stream, "expr8_rest");
    int myposition = position;
    tail_peg_1484:
    Value right;
    Value new_left;
    Result result_peg_1483(myposition);
    {
    
        {
                
                result_peg_1483.reset();
                do{
                    Result result_peg_1496(result_peg_1483.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1496.getPosition()))){
                                result_peg_1496.nextPosition();
                            } else {
                                goto out_peg_1499;
                            }
                        }
                        result_peg_1496.setValue((void*) " ");
                            
                    }
                    goto success_peg_1497;
                    out_peg_1499:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1496.getPosition()))){
                                result_peg_1496.nextPosition();
                            } else {
                                goto out_peg_1501;
                            }
                        }
                        result_peg_1496.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1497;
                    out_peg_1501:
                    goto loop_peg_1495;
                    success_peg_1497:
                    ;
                    result_peg_1483.addResult(result_peg_1496);
                } while (true);
                loop_peg_1495:
                ;
                        
            }
            goto success_peg_1486;
            goto out_peg_1502;
            success_peg_1486:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_1483.getPosition()))){
                    result_peg_1483.nextPosition();
                } else {
                    goto out_peg_1502;
                }
            }
            result_peg_1483.setValue((void*) "=");
        
        
        
        {
                
                result_peg_1483.reset();
                do{
                    Result result_peg_1515(result_peg_1483.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1515.getPosition()))){
                                result_peg_1515.nextPosition();
                            } else {
                                goto out_peg_1518;
                            }
                        }
                        result_peg_1515.setValue((void*) " ");
                            
                    }
                    goto success_peg_1516;
                    out_peg_1518:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1515.getPosition()))){
                                result_peg_1515.nextPosition();
                            } else {
                                goto out_peg_1520;
                            }
                        }
                        result_peg_1515.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1516;
                    out_peg_1520:
                    goto loop_peg_1514;
                    success_peg_1516:
                    ;
                    result_peg_1483.addResult(result_peg_1515);
                } while (true);
                loop_peg_1514:
                ;
                        
            }
            goto success_peg_1505;
            goto out_peg_1502;
            success_peg_1505:
            ;
        
        
        
        result_peg_1483 = rule_expr9(stream, result_peg_1483.getPosition());
            if (result_peg_1483.error()){
                goto out_peg_1502;
            }
            right = result_peg_1483.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_1483.setValue(value);
            }
            new_left = result_peg_1483.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1483.getPosition();
    goto tail_peg_1484;
    out_peg_1502:
    Result result_peg_1523(myposition);
    {
    
        {
                
                result_peg_1523.reset();
                do{
                    Result result_peg_1535(result_peg_1523.getPosition());
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
                    result_peg_1523.addResult(result_peg_1535);
                } while (true);
                loop_peg_1534:
                ;
                        
            }
            goto success_peg_1525;
            goto out_peg_1541;
            success_peg_1525:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_1523.getPosition()))){
                    result_peg_1523.nextPosition();
                } else {
                    goto out_peg_1541;
                }
            }
            result_peg_1523.setValue((void*) "!=");
        
        
        
        {
                
                result_peg_1523.reset();
                do{
                    Result result_peg_1554(result_peg_1523.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1554.getPosition()))){
                                result_peg_1554.nextPosition();
                            } else {
                                goto out_peg_1557;
                            }
                        }
                        result_peg_1554.setValue((void*) " ");
                            
                    }
                    goto success_peg_1555;
                    out_peg_1557:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1554.getPosition()))){
                                result_peg_1554.nextPosition();
                            } else {
                                goto out_peg_1559;
                            }
                        }
                        result_peg_1554.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1555;
                    out_peg_1559:
                    goto loop_peg_1553;
                    success_peg_1555:
                    ;
                    result_peg_1523.addResult(result_peg_1554);
                } while (true);
                loop_peg_1553:
                ;
                        
            }
            goto success_peg_1544;
            goto out_peg_1541;
            success_peg_1544:
            ;
        
        
        
        result_peg_1523 = rule_expr9(stream, result_peg_1523.getPosition());
            if (result_peg_1523.error()){
                goto out_peg_1541;
            }
            right = result_peg_1523.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_1523.setValue(value);
            }
            new_left = result_peg_1523.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1523.getPosition();
    goto tail_peg_1484;
    out_peg_1541:
    Result result_peg_1562(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1562.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1562.getPosition());
    
    return result_peg_1562;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1608(stream, "expr9_rest");
    int myposition = position;
    tail_peg_1567:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_1566(myposition);
    {
    
        {
                
                result_peg_1566.reset();
                do{
                    Result result_peg_1579(result_peg_1566.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1579.getPosition()))){
                                result_peg_1579.nextPosition();
                            } else {
                                goto out_peg_1582;
                            }
                        }
                        result_peg_1579.setValue((void*) " ");
                            
                    }
                    goto success_peg_1580;
                    out_peg_1582:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1579.getPosition()))){
                                result_peg_1579.nextPosition();
                            } else {
                                goto out_peg_1584;
                            }
                        }
                        result_peg_1579.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1580;
                    out_peg_1584:
                    goto loop_peg_1578;
                    success_peg_1580:
                    ;
                    result_peg_1566.addResult(result_peg_1579);
                } while (true);
                loop_peg_1578:
                ;
                        
            }
            goto success_peg_1569;
            goto out_peg_1585;
            success_peg_1569:
            ;
        
        
        
        result_peg_1566 = rule_compare(stream, result_peg_1566.getPosition());
            if (result_peg_1566.error()){
                goto out_peg_1585;
            }
            maker = result_peg_1566.getValues();
        
        
        
        {
                
                result_peg_1566.reset();
                do{
                    Result result_peg_1598(result_peg_1566.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1598.getPosition()))){
                                result_peg_1598.nextPosition();
                            } else {
                                goto out_peg_1601;
                            }
                        }
                        result_peg_1598.setValue((void*) " ");
                            
                    }
                    goto success_peg_1599;
                    out_peg_1601:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1598.getPosition()))){
                                result_peg_1598.nextPosition();
                            } else {
                                goto out_peg_1603;
                            }
                        }
                        result_peg_1598.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1599;
                    out_peg_1603:
                    goto loop_peg_1597;
                    success_peg_1599:
                    ;
                    result_peg_1566.addResult(result_peg_1598);
                } while (true);
                loop_peg_1597:
                ;
                        
            }
            goto success_peg_1588;
            goto out_peg_1585;
            success_peg_1588:
            ;
        
        
        
        result_peg_1566 = rule_expr10(stream, result_peg_1566.getPosition());
            if (result_peg_1566.error()){
                goto out_peg_1585;
            }
            right = result_peg_1566.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_1566.setValue(value);
            }
            new_left = result_peg_1566.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1566.getPosition();
    goto tail_peg_1567;
    out_peg_1585:
    Result result_peg_1606(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1606.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1606.getPosition());
    
    return result_peg_1606;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_1609 = stream.getColumn(position);
    if (column_peg_1609.chunk8 != 0 && column_peg_1609.chunk8->chunk_compare.calculated()){
        return column_peg_1609.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_1622(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_1610(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_1610.getPosition()))){
                    result_peg_1610.nextPosition();
                } else {
                    goto out_peg_1612;
                }
            }
            result_peg_1610.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_1610.setValue(value);
            }
        
        
    }
    
    if (column_peg_1609.chunk8 == 0){
        column_peg_1609.chunk8 = new Chunk8();
    }
    column_peg_1609.chunk8->chunk_compare = result_peg_1610;
    stream.update(result_peg_1610.getPosition());
    
    
    return result_peg_1610;
    out_peg_1612:
    Result result_peg_1613(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_1613.getPosition()))){
                    result_peg_1613.nextPosition();
                } else {
                    goto out_peg_1615;
                }
            }
            result_peg_1613.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_1613.setValue(value);
            }
        
        
    }
    
    if (column_peg_1609.chunk8 == 0){
        column_peg_1609.chunk8 = new Chunk8();
    }
    column_peg_1609.chunk8->chunk_compare = result_peg_1613;
    stream.update(result_peg_1613.getPosition());
    
    
    return result_peg_1613;
    out_peg_1615:
    Result result_peg_1616(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_1616.getPosition()))){
                    result_peg_1616.nextPosition();
                } else {
                    goto out_peg_1618;
                }
            }
            result_peg_1616.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_1616.setValue(value);
            }
        
        
    }
    
    if (column_peg_1609.chunk8 == 0){
        column_peg_1609.chunk8 = new Chunk8();
    }
    column_peg_1609.chunk8->chunk_compare = result_peg_1616;
    stream.update(result_peg_1616.getPosition());
    
    
    return result_peg_1616;
    out_peg_1618:
    Result result_peg_1619(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1619.getPosition()))){
                    result_peg_1619.nextPosition();
                } else {
                    goto out_peg_1621;
                }
            }
            result_peg_1619.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_1619.setValue(value);
            }
        
        
    }
    
    if (column_peg_1609.chunk8 == 0){
        column_peg_1609.chunk8 = new Chunk8();
    }
    column_peg_1609.chunk8->chunk_compare = result_peg_1619;
    stream.update(result_peg_1619.getPosition());
    
    
    return result_peg_1619;
    out_peg_1621:
    
    if (column_peg_1609.chunk8 == 0){
        column_peg_1609.chunk8 = new Chunk8();
    }
    column_peg_1609.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_1623 = stream.getColumn(position);
    if (column_peg_1623.chunk8 != 0 && column_peg_1623.chunk8->chunk_all_compare.calculated()){
        return column_peg_1623.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_1630(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_1624(myposition);
    
    result_peg_1624 = rule_compare(stream, result_peg_1624.getPosition());
    if (result_peg_1624.error()){
        goto out_peg_1625;
    }
    
    if (column_peg_1623.chunk8 == 0){
        column_peg_1623.chunk8 = new Chunk8();
    }
    column_peg_1623.chunk8->chunk_all_compare = result_peg_1624;
    stream.update(result_peg_1624.getPosition());
    
    
    return result_peg_1624;
    out_peg_1625:
    Result result_peg_1626(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_1626.getPosition()))){
            result_peg_1626.nextPosition();
        } else {
            goto out_peg_1627;
        }
    }
    result_peg_1626.setValue((void*) "=");
    
    if (column_peg_1623.chunk8 == 0){
        column_peg_1623.chunk8 = new Chunk8();
    }
    column_peg_1623.chunk8->chunk_all_compare = result_peg_1626;
    stream.update(result_peg_1626.getPosition());
    
    
    return result_peg_1626;
    out_peg_1627:
    Result result_peg_1628(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_1628.getPosition()))){
            result_peg_1628.nextPosition();
        } else {
            goto out_peg_1629;
        }
    }
    result_peg_1628.setValue((void*) "!=");
    
    if (column_peg_1623.chunk8 == 0){
        column_peg_1623.chunk8 = new Chunk8();
    }
    column_peg_1623.chunk8->chunk_all_compare = result_peg_1628;
    stream.update(result_peg_1628.getPosition());
    
    
    return result_peg_1628;
    out_peg_1629:
    
    if (column_peg_1623.chunk8 == 0){
        column_peg_1623.chunk8 = new Chunk8();
    }
    column_peg_1623.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1713(stream, "expr10_rest");
    int myposition = position;
    tail_peg_1633:
    Value right;
    Value new_left;
    Result result_peg_1632(myposition);
    {
    
        {
                
                result_peg_1632.reset();
                do{
                    Result result_peg_1645(result_peg_1632.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1645.getPosition()))){
                                result_peg_1645.nextPosition();
                            } else {
                                goto out_peg_1648;
                            }
                        }
                        result_peg_1645.setValue((void*) " ");
                            
                    }
                    goto success_peg_1646;
                    out_peg_1648:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1645.getPosition()))){
                                result_peg_1645.nextPosition();
                            } else {
                                goto out_peg_1650;
                            }
                        }
                        result_peg_1645.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1646;
                    out_peg_1650:
                    goto loop_peg_1644;
                    success_peg_1646:
                    ;
                    result_peg_1632.addResult(result_peg_1645);
                } while (true);
                loop_peg_1644:
                ;
                        
            }
            goto success_peg_1635;
            goto out_peg_1651;
            success_peg_1635:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_1632.getPosition()))){
                    result_peg_1632.nextPosition();
                } else {
                    goto out_peg_1651;
                }
            }
            result_peg_1632.setValue((void*) "+");
        
        
        
        {
                
                result_peg_1632.reset();
                do{
                    Result result_peg_1664(result_peg_1632.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1664.getPosition()))){
                                result_peg_1664.nextPosition();
                            } else {
                                goto out_peg_1667;
                            }
                        }
                        result_peg_1664.setValue((void*) " ");
                            
                    }
                    goto success_peg_1665;
                    out_peg_1667:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1664.getPosition()))){
                                result_peg_1664.nextPosition();
                            } else {
                                goto out_peg_1669;
                            }
                        }
                        result_peg_1664.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1665;
                    out_peg_1669:
                    goto loop_peg_1663;
                    success_peg_1665:
                    ;
                    result_peg_1632.addResult(result_peg_1664);
                } while (true);
                loop_peg_1663:
                ;
                        
            }
            goto success_peg_1654;
            goto out_peg_1651;
            success_peg_1654:
            ;
        
        
        
        result_peg_1632 = rule_expr11(stream, result_peg_1632.getPosition());
            if (result_peg_1632.error()){
                goto out_peg_1651;
            }
            right = result_peg_1632.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_1632.setValue(value);
            }
            new_left = result_peg_1632.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1632.getPosition();
    goto tail_peg_1633;
    out_peg_1651:
    Result result_peg_1672(myposition);
    {
    
        {
                
                result_peg_1672.reset();
                do{
                    Result result_peg_1684(result_peg_1672.getPosition());
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
                    result_peg_1672.addResult(result_peg_1684);
                } while (true);
                loop_peg_1683:
                ;
                        
            }
            goto success_peg_1674;
            goto out_peg_1690;
            success_peg_1674:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1672.getPosition()))){
                    result_peg_1672.nextPosition();
                } else {
                    goto out_peg_1690;
                }
            }
            result_peg_1672.setValue((void*) "-");
        
        
        
        {
                
                result_peg_1672.reset();
                do{
                    Result result_peg_1703(result_peg_1672.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1703.getPosition()))){
                                result_peg_1703.nextPosition();
                            } else {
                                goto out_peg_1706;
                            }
                        }
                        result_peg_1703.setValue((void*) " ");
                            
                    }
                    goto success_peg_1704;
                    out_peg_1706:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1703.getPosition()))){
                                result_peg_1703.nextPosition();
                            } else {
                                goto out_peg_1708;
                            }
                        }
                        result_peg_1703.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1704;
                    out_peg_1708:
                    goto loop_peg_1702;
                    success_peg_1704:
                    ;
                    result_peg_1672.addResult(result_peg_1703);
                } while (true);
                loop_peg_1702:
                ;
                        
            }
            goto success_peg_1693;
            goto out_peg_1690;
            success_peg_1693:
            ;
        
        
        
        result_peg_1672 = rule_expr11(stream, result_peg_1672.getPosition());
            if (result_peg_1672.error()){
                goto out_peg_1690;
            }
            right = result_peg_1672.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_1672.setValue(value);
            }
            new_left = result_peg_1672.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1672.getPosition();
    goto tail_peg_1633;
    out_peg_1690:
    Result result_peg_1711(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1711.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1711.getPosition());
    
    return result_peg_1711;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1835(stream, "expr11_rest");
    int myposition = position;
    tail_peg_1716:
    Value right;
    Value new_left;
    Result result_peg_1715(myposition);
    {
    
        {
                
                result_peg_1715.reset();
                do{
                    Result result_peg_1728(result_peg_1715.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1728.getPosition()))){
                                result_peg_1728.nextPosition();
                            } else {
                                goto out_peg_1731;
                            }
                        }
                        result_peg_1728.setValue((void*) " ");
                            
                    }
                    goto success_peg_1729;
                    out_peg_1731:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1728.getPosition()))){
                                result_peg_1728.nextPosition();
                            } else {
                                goto out_peg_1733;
                            }
                        }
                        result_peg_1728.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1729;
                    out_peg_1733:
                    goto loop_peg_1727;
                    success_peg_1729:
                    ;
                    result_peg_1715.addResult(result_peg_1728);
                } while (true);
                loop_peg_1727:
                ;
                        
            }
            goto success_peg_1718;
            goto out_peg_1734;
            success_peg_1718:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_1715.getPosition()))){
                    result_peg_1715.nextPosition();
                } else {
                    goto out_peg_1734;
                }
            }
            result_peg_1715.setValue((void*) "*");
        
        
        
        {
                
                result_peg_1715.reset();
                do{
                    Result result_peg_1747(result_peg_1715.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1747.getPosition()))){
                                result_peg_1747.nextPosition();
                            } else {
                                goto out_peg_1750;
                            }
                        }
                        result_peg_1747.setValue((void*) " ");
                            
                    }
                    goto success_peg_1748;
                    out_peg_1750:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1747.getPosition()))){
                                result_peg_1747.nextPosition();
                            } else {
                                goto out_peg_1752;
                            }
                        }
                        result_peg_1747.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1748;
                    out_peg_1752:
                    goto loop_peg_1746;
                    success_peg_1748:
                    ;
                    result_peg_1715.addResult(result_peg_1747);
                } while (true);
                loop_peg_1746:
                ;
                        
            }
            goto success_peg_1737;
            goto out_peg_1734;
            success_peg_1737:
            ;
        
        
        
        result_peg_1715 = rule_expr12(stream, result_peg_1715.getPosition());
            if (result_peg_1715.error()){
                goto out_peg_1734;
            }
            right = result_peg_1715.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_1715.setValue(value);
            }
            new_left = result_peg_1715.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1715.getPosition();
    goto tail_peg_1716;
    out_peg_1734:
    Result result_peg_1755(myposition);
    {
    
        {
                
                result_peg_1755.reset();
                do{
                    Result result_peg_1767(result_peg_1755.getPosition());
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
                    result_peg_1755.addResult(result_peg_1767);
                } while (true);
                loop_peg_1766:
                ;
                        
            }
            goto success_peg_1757;
            goto out_peg_1773;
            success_peg_1757:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1755.getPosition()))){
                    result_peg_1755.nextPosition();
                } else {
                    goto out_peg_1773;
                }
            }
            result_peg_1755.setValue((void*) "/");
        
        
        
        {
                
                result_peg_1755.reset();
                do{
                    Result result_peg_1786(result_peg_1755.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1786.getPosition()))){
                                result_peg_1786.nextPosition();
                            } else {
                                goto out_peg_1789;
                            }
                        }
                        result_peg_1786.setValue((void*) " ");
                            
                    }
                    goto success_peg_1787;
                    out_peg_1789:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1786.getPosition()))){
                                result_peg_1786.nextPosition();
                            } else {
                                goto out_peg_1791;
                            }
                        }
                        result_peg_1786.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1787;
                    out_peg_1791:
                    goto loop_peg_1785;
                    success_peg_1787:
                    ;
                    result_peg_1755.addResult(result_peg_1786);
                } while (true);
                loop_peg_1785:
                ;
                        
            }
            goto success_peg_1776;
            goto out_peg_1773;
            success_peg_1776:
            ;
        
        
        
        result_peg_1755 = rule_expr12(stream, result_peg_1755.getPosition());
            if (result_peg_1755.error()){
                goto out_peg_1773;
            }
            right = result_peg_1755.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_1755.setValue(value);
            }
            new_left = result_peg_1755.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1755.getPosition();
    goto tail_peg_1716;
    out_peg_1773:
    Result result_peg_1794(myposition);
    {
    
        {
                
                result_peg_1794.reset();
                do{
                    Result result_peg_1806(result_peg_1794.getPosition());
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
                    result_peg_1794.addResult(result_peg_1806);
                } while (true);
                loop_peg_1805:
                ;
                        
            }
            goto success_peg_1796;
            goto out_peg_1812;
            success_peg_1796:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_1794.getPosition()))){
                    result_peg_1794.nextPosition();
                } else {
                    goto out_peg_1812;
                }
            }
            result_peg_1794.setValue((void*) "%");
        
        
        
        {
                
                result_peg_1794.reset();
                do{
                    Result result_peg_1825(result_peg_1794.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1825.getPosition()))){
                                result_peg_1825.nextPosition();
                            } else {
                                goto out_peg_1828;
                            }
                        }
                        result_peg_1825.setValue((void*) " ");
                            
                    }
                    goto success_peg_1826;
                    out_peg_1828:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1825.getPosition()))){
                                result_peg_1825.nextPosition();
                            } else {
                                goto out_peg_1830;
                            }
                        }
                        result_peg_1825.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1826;
                    out_peg_1830:
                    goto loop_peg_1824;
                    success_peg_1826:
                    ;
                    result_peg_1794.addResult(result_peg_1825);
                } while (true);
                loop_peg_1824:
                ;
                        
            }
            goto success_peg_1815;
            goto out_peg_1812;
            success_peg_1815:
            ;
        
        
        
        result_peg_1794 = rule_expr12(stream, result_peg_1794.getPosition());
            if (result_peg_1794.error()){
                goto out_peg_1812;
            }
            right = result_peg_1794.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_1794.setValue(value);
            }
            new_left = result_peg_1794.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1794.getPosition();
    goto tail_peg_1716;
    out_peg_1812:
    Result result_peg_1833(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1833.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1833.getPosition());
    
    return result_peg_1833;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1879(stream, "expr12_rest");
    int myposition = position;
    tail_peg_1838:
    Value right;
    Value new_left;
    Result result_peg_1837(myposition);
    {
    
        {
                
                result_peg_1837.reset();
                do{
                    Result result_peg_1850(result_peg_1837.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1850.getPosition()))){
                                result_peg_1850.nextPosition();
                            } else {
                                goto out_peg_1853;
                            }
                        }
                        result_peg_1850.setValue((void*) " ");
                            
                    }
                    goto success_peg_1851;
                    out_peg_1853:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1850.getPosition()))){
                                result_peg_1850.nextPosition();
                            } else {
                                goto out_peg_1855;
                            }
                        }
                        result_peg_1850.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1851;
                    out_peg_1855:
                    goto loop_peg_1849;
                    success_peg_1851:
                    ;
                    result_peg_1837.addResult(result_peg_1850);
                } while (true);
                loop_peg_1849:
                ;
                        
            }
            goto success_peg_1840;
            goto out_peg_1856;
            success_peg_1840:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_1837.getPosition()))){
                    result_peg_1837.nextPosition();
                } else {
                    goto out_peg_1856;
                }
            }
            result_peg_1837.setValue((void*) "**");
        
        
        
        {
                
                result_peg_1837.reset();
                do{
                    Result result_peg_1869(result_peg_1837.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1869.getPosition()))){
                                result_peg_1869.nextPosition();
                            } else {
                                goto out_peg_1872;
                            }
                        }
                        result_peg_1869.setValue((void*) " ");
                            
                    }
                    goto success_peg_1870;
                    out_peg_1872:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1869.getPosition()))){
                                result_peg_1869.nextPosition();
                            } else {
                                goto out_peg_1874;
                            }
                        }
                        result_peg_1869.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1870;
                    out_peg_1874:
                    goto loop_peg_1868;
                    success_peg_1870:
                    ;
                    result_peg_1837.addResult(result_peg_1869);
                } while (true);
                loop_peg_1868:
                ;
                        
            }
            goto success_peg_1859;
            goto out_peg_1856;
            success_peg_1859:
            ;
        
        
        
        result_peg_1837 = rule_expr13(stream, result_peg_1837.getPosition());
            if (result_peg_1837.error()){
                goto out_peg_1856;
            }
            right = result_peg_1837.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_1837.setValue(value);
            }
            new_left = result_peg_1837.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_1837.getPosition();
    goto tail_peg_1838;
    out_peg_1856:
    Result result_peg_1877(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1877.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1877.getPosition());
    
    return result_peg_1877;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_1880 = stream.getColumn(position);
    if (column_peg_1880.chunk9 != 0 && column_peg_1880.chunk9->chunk_unary.calculated()){
        return column_peg_1880.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_1909(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_1881(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_1881.getPosition()))){
                    result_peg_1881.nextPosition();
                } else {
                    goto out_peg_1883;
                }
            }
            result_peg_1881.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_1881.setValue(value);
            }
        
        
    }
    
    if (column_peg_1880.chunk9 == 0){
        column_peg_1880.chunk9 = new Chunk9();
    }
    column_peg_1880.chunk9->chunk_unary = result_peg_1881;
    stream.update(result_peg_1881.getPosition());
    
    
    return result_peg_1881;
    out_peg_1883:
    Result result_peg_1884(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_1884.getPosition()))){
                    result_peg_1884.nextPosition();
                } else {
                    goto out_peg_1886;
                }
            }
            result_peg_1884.setValue((void*) "-");
        
        
        
        Result result_peg_1889(result_peg_1884);
            {
                
                char letter_peg_1894 = stream.get(result_peg_1889.getPosition());
                if (letter_peg_1894 != '\0' && strchr("0123456789", letter_peg_1894) != NULL){
                    result_peg_1889.nextPosition();
                    result_peg_1889.setValue((void*) (long) letter_peg_1894);
                } else {
                    goto out_peg_1893;
                }
                
            }
            goto success_peg_1890;
            out_peg_1893:
            goto not_peg_1888;
            success_peg_1890:
            ;
            goto out_peg_1886;
            not_peg_1888:
            result_peg_1884.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_1884.setValue(value);
            }
        
        
    }
    
    if (column_peg_1880.chunk9 == 0){
        column_peg_1880.chunk9 = new Chunk9();
    }
    column_peg_1880.chunk9->chunk_unary = result_peg_1884;
    stream.update(result_peg_1884.getPosition());
    
    
    return result_peg_1884;
    out_peg_1886:
    Result result_peg_1895(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1895.getPosition()))){
                    result_peg_1895.nextPosition();
                } else {
                    goto out_peg_1897;
                }
            }
            result_peg_1895.setValue((void*) "~");
        
        
        
        Result result_peg_1900(result_peg_1895);
            result_peg_1900 = rule_key(stream, result_peg_1900.getPosition());
            if (result_peg_1900.error()){
                goto not_peg_1899;
            }
            goto out_peg_1897;
            not_peg_1899:
            result_peg_1895.setValue((void*)0);
        
        
        
        Result result_peg_1903(result_peg_1895);
            {
                
                char letter_peg_1908 = stream.get(result_peg_1903.getPosition());
                if (letter_peg_1908 != '\0' && strchr("0123456789", letter_peg_1908) != NULL){
                    result_peg_1903.nextPosition();
                    result_peg_1903.setValue((void*) (long) letter_peg_1908);
                } else {
                    goto out_peg_1907;
                }
                
            }
            goto success_peg_1904;
            out_peg_1907:
            goto not_peg_1902;
            success_peg_1904:
            ;
            goto out_peg_1897;
            not_peg_1902:
            result_peg_1895.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_1895.setValue(value);
            }
        
        
    }
    
    if (column_peg_1880.chunk9 == 0){
        column_peg_1880.chunk9 = new Chunk9();
    }
    column_peg_1880.chunk9->chunk_unary = result_peg_1895;
    stream.update(result_peg_1895.getPosition());
    
    
    return result_peg_1895;
    out_peg_1897:
    
    if (column_peg_1880.chunk9 == 0){
        column_peg_1880.chunk9 = new Chunk9();
    }
    column_peg_1880.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_1910 = stream.getColumn(position);
    if (column_peg_1910.chunk9 != 0 && column_peg_1910.chunk9->chunk_expr13_real.calculated()){
        return column_peg_1910.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_1956(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_1911(myposition);
    
    result_peg_1911 = rule_helper(stream, result_peg_1911.getPosition());
    if (result_peg_1911.error()){
        goto out_peg_1912;
    }
    
    if (column_peg_1910.chunk9 == 0){
        column_peg_1910.chunk9 = new Chunk9();
    }
    column_peg_1910.chunk9->chunk_expr13_real = result_peg_1911;
    stream.update(result_peg_1911.getPosition());
    
    
    return result_peg_1911;
    out_peg_1912:
    Result result_peg_1913(myposition);
    
    result_peg_1913 = rule_function(stream, result_peg_1913.getPosition());
    if (result_peg_1913.error()){
        goto out_peg_1914;
    }
    
    if (column_peg_1910.chunk9 == 0){
        column_peg_1910.chunk9 = new Chunk9();
    }
    column_peg_1910.chunk9->chunk_expr13_real = result_peg_1913;
    stream.update(result_peg_1913.getPosition());
    
    
    return result_peg_1913;
    out_peg_1914:
    Result result_peg_1915(myposition);
    
    result_peg_1915 = rule_value(stream, result_peg_1915.getPosition());
    if (result_peg_1915.error()){
        goto out_peg_1916;
    }
    
    if (column_peg_1910.chunk9 == 0){
        column_peg_1910.chunk9 = new Chunk9();
    }
    column_peg_1910.chunk9->chunk_expr13_real = result_peg_1915;
    stream.update(result_peg_1915.getPosition());
    
    
    return result_peg_1915;
    out_peg_1916:
    Result result_peg_1917(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1917.getPosition()))){
                    result_peg_1917.nextPosition();
                } else {
                    goto out_peg_1919;
                }
            }
            result_peg_1917.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1917.reset();
                do{
                    Result result_peg_1931(result_peg_1917.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1931.getPosition()))){
                                result_peg_1931.nextPosition();
                            } else {
                                goto out_peg_1934;
                            }
                        }
                        result_peg_1931.setValue((void*) " ");
                            
                    }
                    goto success_peg_1932;
                    out_peg_1934:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1931.getPosition()))){
                                result_peg_1931.nextPosition();
                            } else {
                                goto out_peg_1936;
                            }
                        }
                        result_peg_1931.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1932;
                    out_peg_1936:
                    goto loop_peg_1930;
                    success_peg_1932:
                    ;
                    result_peg_1917.addResult(result_peg_1931);
                } while (true);
                loop_peg_1930:
                ;
                        
            }
            goto success_peg_1921;
            goto out_peg_1919;
            success_peg_1921:
            ;
        
        
        
        result_peg_1917 = rule_expr(stream, result_peg_1917.getPosition());
            if (result_peg_1917.error()){
                goto out_peg_1919;
            }
            e = result_peg_1917.getValues();
        
        
        
        {
                
                result_peg_1917.reset();
                do{
                    Result result_peg_1949(result_peg_1917.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1949.getPosition()))){
                                result_peg_1949.nextPosition();
                            } else {
                                goto out_peg_1952;
                            }
                        }
                        result_peg_1949.setValue((void*) " ");
                            
                    }
                    goto success_peg_1950;
                    out_peg_1952:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1949.getPosition()))){
                                result_peg_1949.nextPosition();
                            } else {
                                goto out_peg_1954;
                            }
                        }
                        result_peg_1949.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1950;
                    out_peg_1954:
                    goto loop_peg_1948;
                    success_peg_1950:
                    ;
                    result_peg_1917.addResult(result_peg_1949);
                } while (true);
                loop_peg_1948:
                ;
                        
            }
            goto success_peg_1939;
            goto out_peg_1919;
            success_peg_1939:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1917.getPosition()))){
                    result_peg_1917.nextPosition();
                } else {
                    goto out_peg_1919;
                }
            }
            result_peg_1917.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_1917.setValue(value);
            }
        
        
    }
    
    if (column_peg_1910.chunk9 == 0){
        column_peg_1910.chunk9 = new Chunk9();
    }
    column_peg_1910.chunk9->chunk_expr13_real = result_peg_1917;
    stream.update(result_peg_1917.getPosition());
    
    
    return result_peg_1917;
    out_peg_1919:
    
    if (column_peg_1910.chunk9 == 0){
        column_peg_1910.chunk9 = new Chunk9();
    }
    column_peg_1910.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_1957 = stream.getColumn(position);
    if (column_peg_1957.chunk9 != 0 && column_peg_1957.chunk9->chunk_function.calculated()){
        return column_peg_1957.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_2095(stream, "function");
    int myposition = position;
    
    Value name;
    Value args;
    Value arg1;
    Result result_peg_1958(myposition);
    
    {
    
        result_peg_1958 = rule_function_name(stream, result_peg_1958.getPosition());
            if (result_peg_1958.error()){
                goto out_peg_1960;
            }
            name = result_peg_1958.getValues();
        
        
        
        {
                
                result_peg_1958.reset();
                do{
                    Result result_peg_1972(result_peg_1958.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1972.getPosition()))){
                                result_peg_1972.nextPosition();
                            } else {
                                goto out_peg_1975;
                            }
                        }
                        result_peg_1972.setValue((void*) " ");
                            
                    }
                    goto success_peg_1973;
                    out_peg_1975:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1972.getPosition()))){
                                result_peg_1972.nextPosition();
                            } else {
                                goto out_peg_1977;
                            }
                        }
                        result_peg_1972.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1973;
                    out_peg_1977:
                    goto loop_peg_1971;
                    success_peg_1973:
                    ;
                    result_peg_1958.addResult(result_peg_1972);
                } while (true);
                loop_peg_1971:
                ;
                        
            }
            goto success_peg_1962;
            goto out_peg_1960;
            success_peg_1962:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_1958.getPosition()))){
                    result_peg_1958.nextPosition();
                } else {
                    goto out_peg_1960;
                }
            }
            result_peg_1958.setValue((void*) "(");
        
        
        
        {
                
                result_peg_1958.reset();
                do{
                    Result result_peg_1990(result_peg_1958.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_1990.getPosition()))){
                                result_peg_1990.nextPosition();
                            } else {
                                goto out_peg_1993;
                            }
                        }
                        result_peg_1990.setValue((void*) " ");
                            
                    }
                    goto success_peg_1991;
                    out_peg_1993:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_1990.getPosition()))){
                                result_peg_1990.nextPosition();
                            } else {
                                goto out_peg_1995;
                            }
                        }
                        result_peg_1990.setValue((void*) "\t");
                            
                    }
                    goto success_peg_1991;
                    out_peg_1995:
                    goto loop_peg_1989;
                    success_peg_1991:
                    ;
                    result_peg_1958.addResult(result_peg_1990);
                } while (true);
                loop_peg_1989:
                ;
                        
            }
            goto success_peg_1980;
            goto out_peg_1960;
            success_peg_1980:
            ;
        
        
        
        result_peg_1958 = rule_args(stream, result_peg_1958.getPosition());
            if (result_peg_1958.error()){
                goto out_peg_1960;
            }
            args = result_peg_1958.getValues();
        
        
        
        {
                
                result_peg_1958.reset();
                do{
                    Result result_peg_2008(result_peg_1958.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2008.getPosition()))){
                                result_peg_2008.nextPosition();
                            } else {
                                goto out_peg_2011;
                            }
                        }
                        result_peg_2008.setValue((void*) " ");
                            
                    }
                    goto success_peg_2009;
                    out_peg_2011:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2008.getPosition()))){
                                result_peg_2008.nextPosition();
                            } else {
                                goto out_peg_2013;
                            }
                        }
                        result_peg_2008.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2009;
                    out_peg_2013:
                    goto loop_peg_2007;
                    success_peg_2009:
                    ;
                    result_peg_1958.addResult(result_peg_2008);
                } while (true);
                loop_peg_2007:
                ;
                        
            }
            goto success_peg_1998;
            goto out_peg_1960;
            success_peg_1998:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_1958.getPosition()))){
                    result_peg_1958.nextPosition();
                } else {
                    goto out_peg_1960;
                }
            }
            result_peg_1958.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, args);
                result_peg_1958.setValue(value);
            }
        
        
    }
    
    if (column_peg_1957.chunk9 == 0){
        column_peg_1957.chunk9 = new Chunk9();
    }
    column_peg_1957.chunk9->chunk_function = result_peg_1958;
    stream.update(result_peg_1958.getPosition());
    
    
    return result_peg_1958;
    out_peg_1960:
    Result result_peg_2015(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2015.getPosition()))){
                    result_peg_2015.nextPosition();
                } else {
                    goto out_peg_2017;
                }
            }
            result_peg_2015.setValue((void*) "animelem");
            name = result_peg_2015.getValues();
        
        
        
        {
                
                result_peg_2015.reset();
                do{
                    Result result_peg_2029(result_peg_2015.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2029.getPosition()))){
                                result_peg_2029.nextPosition();
                            } else {
                                goto out_peg_2032;
                            }
                        }
                        result_peg_2029.setValue((void*) " ");
                            
                    }
                    goto success_peg_2030;
                    out_peg_2032:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2029.getPosition()))){
                                result_peg_2029.nextPosition();
                            } else {
                                goto out_peg_2034;
                            }
                        }
                        result_peg_2029.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2030;
                    out_peg_2034:
                    goto loop_peg_2028;
                    success_peg_2030:
                    ;
                    result_peg_2015.addResult(result_peg_2029);
                } while (true);
                loop_peg_2028:
                ;
                        
            }
            goto success_peg_2019;
            goto out_peg_2017;
            success_peg_2019:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_2015.getPosition()))){
                    result_peg_2015.nextPosition();
                } else {
                    goto out_peg_2017;
                }
            }
            result_peg_2015.setValue((void*) "=");
        
        
        
        {
                
                result_peg_2015.reset();
                do{
                    Result result_peg_2047(result_peg_2015.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2047.getPosition()))){
                                result_peg_2047.nextPosition();
                            } else {
                                goto out_peg_2050;
                            }
                        }
                        result_peg_2047.setValue((void*) " ");
                            
                    }
                    goto success_peg_2048;
                    out_peg_2050:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2047.getPosition()))){
                                result_peg_2047.nextPosition();
                            } else {
                                goto out_peg_2052;
                            }
                        }
                        result_peg_2047.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2048;
                    out_peg_2052:
                    goto loop_peg_2046;
                    success_peg_2048:
                    ;
                    result_peg_2015.addResult(result_peg_2047);
                } while (true);
                loop_peg_2046:
                ;
                        
            }
            goto success_peg_2037;
            goto out_peg_2017;
            success_peg_2037:
            ;
        
        
        
        result_peg_2015 = rule_integer(stream, result_peg_2015.getPosition());
            if (result_peg_2015.error()){
                goto out_peg_2017;
            }
            arg1 = result_peg_2015.getValues();
        
        
        
        result_peg_2015 = rule_function_rest(stream, result_peg_2015.getPosition());
            if (result_peg_2015.error()){
                goto out_peg_2017;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1(name, arg1);
                result_peg_2015.setValue(value);
            }
        
        
    }
    
    if (column_peg_1957.chunk9 == 0){
        column_peg_1957.chunk9 = new Chunk9();
    }
    column_peg_1957.chunk9->chunk_function = result_peg_2015;
    stream.update(result_peg_2015.getPosition());
    
    
    return result_peg_2015;
    out_peg_2017:
    Result result_peg_2055(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_2055.getPosition()))){
                    result_peg_2055.nextPosition();
                } else {
                    goto out_peg_2057;
                }
            }
            result_peg_2055.setValue((void*) "animelem");
            name = result_peg_2055.getValues();
        
        
        
        {
                
                result_peg_2055.reset();
                do{
                    Result result_peg_2069(result_peg_2055.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2069.getPosition()))){
                                result_peg_2069.nextPosition();
                            } else {
                                goto out_peg_2072;
                            }
                        }
                        result_peg_2069.setValue((void*) " ");
                            
                    }
                    goto success_peg_2070;
                    out_peg_2072:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2069.getPosition()))){
                                result_peg_2069.nextPosition();
                            } else {
                                goto out_peg_2074;
                            }
                        }
                        result_peg_2069.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2070;
                    out_peg_2074:
                    goto loop_peg_2068;
                    success_peg_2070:
                    ;
                    result_peg_2055.addResult(result_peg_2069);
                } while (true);
                loop_peg_2068:
                ;
                        
            }
            goto success_peg_2059;
            goto out_peg_2057;
            success_peg_2059:
            ;
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_2055.getPosition()))){
                    result_peg_2055.nextPosition();
                } else {
                    goto out_peg_2057;
                }
            }
            result_peg_2055.setValue((void*) "<=");
        
        
        
        {
                
                result_peg_2055.reset();
                do{
                    Result result_peg_2087(result_peg_2055.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2087.getPosition()))){
                                result_peg_2087.nextPosition();
                            } else {
                                goto out_peg_2090;
                            }
                        }
                        result_peg_2087.setValue((void*) " ");
                            
                    }
                    goto success_peg_2088;
                    out_peg_2090:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2087.getPosition()))){
                                result_peg_2087.nextPosition();
                            } else {
                                goto out_peg_2092;
                            }
                        }
                        result_peg_2087.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2088;
                    out_peg_2092:
                    goto loop_peg_2086;
                    success_peg_2088:
                    ;
                    result_peg_2055.addResult(result_peg_2087);
                } while (true);
                loop_peg_2086:
                ;
                        
            }
            goto success_peg_2077;
            goto out_peg_2057;
            success_peg_2077:
            ;
        
        
        
        result_peg_2055 = rule_integer(stream, result_peg_2055.getPosition());
            if (result_peg_2055.error()){
                goto out_peg_2057;
            }
            arg1 = result_peg_2055.getValues();
        
        
        
        result_peg_2055 = rule_function_rest(stream, result_peg_2055.getPosition());
            if (result_peg_2055.error()){
                goto out_peg_2057;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1("animelem<=", arg1);
                result_peg_2055.setValue(value);
            }
        
        
    }
    
    if (column_peg_1957.chunk9 == 0){
        column_peg_1957.chunk9 = new Chunk9();
    }
    column_peg_1957.chunk9->chunk_function = result_peg_2055;
    stream.update(result_peg_2055.getPosition());
    
    
    return result_peg_2055;
    out_peg_2057:
    
    if (column_peg_1957.chunk9 == 0){
        column_peg_1957.chunk9 = new Chunk9();
    }
    column_peg_1957.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_2096 = stream.getColumn(position);
    if (column_peg_2096.chunk9 != 0 && column_peg_2096.chunk9->chunk_args.calculated()){
        return column_peg_2096.chunk9->chunk_args;
    }
    
    RuleTrace trace_peg_2138(stream, "args");
    int myposition = position;
    
    Value expr1;
    Value expr_rest;
    Result result_peg_2097(myposition);
    
    {
    
        result_peg_2097 = rule_expr_c(stream, result_peg_2097.getPosition());
            if (result_peg_2097.error()){
                goto out_peg_2099;
            }
            expr1 = result_peg_2097.getValues();
        
        
        
        result_peg_2097.reset();
            do{
                Result result_peg_2102(result_peg_2097.getPosition());
                {
                
                    {
                            
                            result_peg_2102.reset();
                            do{
                                Result result_peg_2114(result_peg_2102.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2114.getPosition()))){
                                            result_peg_2114.nextPosition();
                                        } else {
                                            goto out_peg_2117;
                                        }
                                    }
                                    result_peg_2114.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2115;
                                out_peg_2117:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2114.getPosition()))){
                                            result_peg_2114.nextPosition();
                                        } else {
                                            goto out_peg_2119;
                                        }
                                    }
                                    result_peg_2114.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2115;
                                out_peg_2119:
                                goto loop_peg_2113;
                                success_peg_2115:
                                ;
                                result_peg_2102.addResult(result_peg_2114);
                            } while (true);
                            loop_peg_2113:
                            ;
                                    
                        }
                        goto success_peg_2104;
                        goto loop_peg_2101;
                        success_peg_2104:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2102.getPosition()))){
                                result_peg_2102.nextPosition();
                            } else {
                                goto loop_peg_2101;
                            }
                        }
                        result_peg_2102.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2102.reset();
                            do{
                                Result result_peg_2132(result_peg_2102.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2132.getPosition()))){
                                            result_peg_2132.nextPosition();
                                        } else {
                                            goto out_peg_2135;
                                        }
                                    }
                                    result_peg_2132.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2133;
                                out_peg_2135:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2132.getPosition()))){
                                            result_peg_2132.nextPosition();
                                        } else {
                                            goto out_peg_2137;
                                        }
                                    }
                                    result_peg_2132.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2133;
                                out_peg_2137:
                                goto loop_peg_2131;
                                success_peg_2133:
                                ;
                                result_peg_2102.addResult(result_peg_2132);
                            } while (true);
                            loop_peg_2131:
                            ;
                                    
                        }
                        goto success_peg_2122;
                        goto loop_peg_2101;
                        success_peg_2122:
                        ;
                    
                    
                    
                    result_peg_2102 = rule_expr_c(stream, result_peg_2102.getPosition());
                        if (result_peg_2102.error()){
                            goto loop_peg_2101;
                        }
                    
                    
                }
                result_peg_2097.addResult(result_peg_2102);
            } while (true);
            loop_peg_2101:
            ;
            expr_rest = result_peg_2097.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(expr1, expr_rest);
                result_peg_2097.setValue(value);
            }
        
        
    }
    
    if (column_peg_2096.chunk9 == 0){
        column_peg_2096.chunk9 = new Chunk9();
    }
    column_peg_2096.chunk9->chunk_args = result_peg_2097;
    stream.update(result_peg_2097.getPosition());
    
    
    return result_peg_2097;
    out_peg_2099:
    
    if (column_peg_2096.chunk9 == 0){
        column_peg_2096.chunk9 = new Chunk9();
    }
    column_peg_2096.chunk9->chunk_args = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_2139 = stream.getColumn(position);
    if (column_peg_2139.chunk10 != 0 && column_peg_2139.chunk10->chunk_function_name.calculated()){
        return column_peg_2139.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_2190(stream, "function_name");
    int myposition = position;
    
    
    Result result_peg_2140(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("abs"[i], stream.get(result_peg_2140.getPosition()))){
            result_peg_2140.nextPosition();
        } else {
            goto out_peg_2141;
        }
    }
    result_peg_2140.setValue((void*) "abs");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2140;
    stream.update(result_peg_2140.getPosition());
    
    
    return result_peg_2140;
    out_peg_2141:
    Result result_peg_2142(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("const"[i], stream.get(result_peg_2142.getPosition()))){
            result_peg_2142.nextPosition();
        } else {
            goto out_peg_2143;
        }
    }
    result_peg_2142.setValue((void*) "const");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2142;
    stream.update(result_peg_2142.getPosition());
    
    
    return result_peg_2142;
    out_peg_2143:
    Result result_peg_2144(myposition);
    
    for (int i = 0; i < 13; i++){
        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_2144.getPosition()))){
            result_peg_2144.nextPosition();
        } else {
            goto out_peg_2145;
        }
    }
    result_peg_2144.setValue((void*) "selfanimexist");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2144;
    stream.update(result_peg_2144.getPosition());
    
    
    return result_peg_2144;
    out_peg_2145:
    Result result_peg_2146(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("ifelse"[i], stream.get(result_peg_2146.getPosition()))){
            result_peg_2146.nextPosition();
        } else {
            goto out_peg_2147;
        }
    }
    result_peg_2146.setValue((void*) "ifelse");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2146;
    stream.update(result_peg_2146.getPosition());
    
    
    return result_peg_2146;
    out_peg_2147:
    Result result_peg_2148(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("gethitvar"[i], stream.get(result_peg_2148.getPosition()))){
            result_peg_2148.nextPosition();
        } else {
            goto out_peg_2149;
        }
    }
    result_peg_2148.setValue((void*) "gethitvar");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2148;
    stream.update(result_peg_2148.getPosition());
    
    
    return result_peg_2148;
    out_peg_2149:
    Result result_peg_2150(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("floor"[i], stream.get(result_peg_2150.getPosition()))){
            result_peg_2150.nextPosition();
        } else {
            goto out_peg_2151;
        }
    }
    result_peg_2150.setValue((void*) "floor");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2150;
    stream.update(result_peg_2150.getPosition());
    
    
    return result_peg_2150;
    out_peg_2151:
    Result result_peg_2152(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("ceil"[i], stream.get(result_peg_2152.getPosition()))){
            result_peg_2152.nextPosition();
        } else {
            goto out_peg_2153;
        }
    }
    result_peg_2152.setValue((void*) "ceil");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2152;
    stream.update(result_peg_2152.getPosition());
    
    
    return result_peg_2152;
    out_peg_2153:
    Result result_peg_2154(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("sin"[i], stream.get(result_peg_2154.getPosition()))){
            result_peg_2154.nextPosition();
        } else {
            goto out_peg_2155;
        }
    }
    result_peg_2154.setValue((void*) "sin");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2154;
    stream.update(result_peg_2154.getPosition());
    
    
    return result_peg_2154;
    out_peg_2155:
    Result result_peg_2156(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("sysfvar"[i], stream.get(result_peg_2156.getPosition()))){
            result_peg_2156.nextPosition();
        } else {
            goto out_peg_2157;
        }
    }
    result_peg_2156.setValue((void*) "sysfvar");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2156;
    stream.update(result_peg_2156.getPosition());
    
    
    return result_peg_2156;
    out_peg_2157:
    Result result_peg_2158(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("sysvar"[i], stream.get(result_peg_2158.getPosition()))){
            result_peg_2158.nextPosition();
        } else {
            goto out_peg_2159;
        }
    }
    result_peg_2158.setValue((void*) "sysvar");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2158;
    stream.update(result_peg_2158.getPosition());
    
    
    return result_peg_2158;
    out_peg_2159:
    Result result_peg_2160(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("var"[i], stream.get(result_peg_2160.getPosition()))){
            result_peg_2160.nextPosition();
        } else {
            goto out_peg_2161;
        }
    }
    result_peg_2160.setValue((void*) "var");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2160;
    stream.update(result_peg_2160.getPosition());
    
    
    return result_peg_2160;
    out_peg_2161:
    Result result_peg_2162(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numexplod"[i], stream.get(result_peg_2162.getPosition()))){
            result_peg_2162.nextPosition();
        } else {
            goto out_peg_2163;
        }
    }
    result_peg_2162.setValue((void*) "numexplod");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2162;
    stream.update(result_peg_2162.getPosition());
    
    
    return result_peg_2162;
    out_peg_2163:
    Result result_peg_2164(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2164.getPosition()))){
            result_peg_2164.nextPosition();
        } else {
            goto out_peg_2165;
        }
    }
    result_peg_2164.setValue((void*) "numhelper");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2164;
    stream.update(result_peg_2164.getPosition());
    
    
    return result_peg_2164;
    out_peg_2165:
    Result result_peg_2166(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numprojid"[i], stream.get(result_peg_2166.getPosition()))){
            result_peg_2166.nextPosition();
        } else {
            goto out_peg_2167;
        }
    }
    result_peg_2166.setValue((void*) "numprojid");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2166;
    stream.update(result_peg_2166.getPosition());
    
    
    return result_peg_2166;
    out_peg_2167:
    Result result_peg_2168(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("fvar"[i], stream.get(result_peg_2168.getPosition()))){
            result_peg_2168.nextPosition();
        } else {
            goto out_peg_2169;
        }
    }
    result_peg_2168.setValue((void*) "fvar");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2168;
    stream.update(result_peg_2168.getPosition());
    
    
    return result_peg_2168;
    out_peg_2169:
    Result result_peg_2170(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("ishelper"[i], stream.get(result_peg_2170.getPosition()))){
            result_peg_2170.nextPosition();
        } else {
            goto out_peg_2171;
        }
    }
    result_peg_2170.setValue((void*) "ishelper");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2170;
    stream.update(result_peg_2170.getPosition());
    
    
    return result_peg_2170;
    out_peg_2171:
    Result result_peg_2172(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numtarget"[i], stream.get(result_peg_2172.getPosition()))){
            result_peg_2172.nextPosition();
        } else {
            goto out_peg_2173;
        }
    }
    result_peg_2172.setValue((void*) "numtarget");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2172;
    stream.update(result_peg_2172.getPosition());
    
    
    return result_peg_2172;
    out_peg_2173:
    Result result_peg_2174(myposition);
    
    for (int i = 0; i < 12; i++){
        if (compareCharCase("animelemtime"[i], stream.get(result_peg_2174.getPosition()))){
            result_peg_2174.nextPosition();
        } else {
            goto out_peg_2175;
        }
    }
    result_peg_2174.setValue((void*) "animelemtime");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2174;
    stream.update(result_peg_2174.getPosition());
    
    
    return result_peg_2174;
    out_peg_2175:
    Result result_peg_2176(myposition);
    
    for (int i = 0; i < 10; i++){
        if (compareCharCase("animelemno"[i], stream.get(result_peg_2176.getPosition()))){
            result_peg_2176.nextPosition();
        } else {
            goto out_peg_2177;
        }
    }
    result_peg_2176.setValue((void*) "animelemno");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2176;
    stream.update(result_peg_2176.getPosition());
    
    
    return result_peg_2176;
    out_peg_2177:
    Result result_peg_2178(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("animexist"[i], stream.get(result_peg_2178.getPosition()))){
            result_peg_2178.nextPosition();
        } else {
            goto out_peg_2179;
        }
    }
    result_peg_2178.setValue((void*) "animexist");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2178;
    stream.update(result_peg_2178.getPosition());
    
    
    return result_peg_2178;
    out_peg_2179:
    Result result_peg_2180(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projguarded"[i], stream.get(result_peg_2180.getPosition()))){
            result_peg_2180.nextPosition();
        } else {
            goto out_peg_2181;
        }
    }
    result_peg_2180.setValue((void*) "projguarded");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2180;
    stream.update(result_peg_2180.getPosition());
    
    
    return result_peg_2180;
    out_peg_2181:
    Result result_peg_2182(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projhittime"[i], stream.get(result_peg_2182.getPosition()))){
            result_peg_2182.nextPosition();
        } else {
            goto out_peg_2183;
        }
    }
    result_peg_2182.setValue((void*) "projhittime");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2182;
    stream.update(result_peg_2182.getPosition());
    
    
    return result_peg_2182;
    out_peg_2183:
    Result result_peg_2184(myposition);
    
    for (int i = 0; i < 15; i++){
        if (compareCharCase("projcontacttime"[i], stream.get(result_peg_2184.getPosition()))){
            result_peg_2184.nextPosition();
        } else {
            goto out_peg_2185;
        }
    }
    result_peg_2184.setValue((void*) "projcontacttime");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2184;
    stream.update(result_peg_2184.getPosition());
    
    
    return result_peg_2184;
    out_peg_2185:
    Result result_peg_2186(myposition);
    
    for (int i = 0; i < 11; i++){
        if (compareCharCase("projcontact"[i], stream.get(result_peg_2186.getPosition()))){
            result_peg_2186.nextPosition();
        } else {
            goto out_peg_2187;
        }
    }
    result_peg_2186.setValue((void*) "projcontact");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2186;
    stream.update(result_peg_2186.getPosition());
    
    
    return result_peg_2186;
    out_peg_2187:
    Result result_peg_2188(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_2188.getPosition()))){
            result_peg_2188.nextPosition();
        } else {
            goto out_peg_2189;
        }
    }
    result_peg_2188.setValue((void*) "numhelper");
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = result_peg_2188;
    stream.update(result_peg_2188.getPosition());
    
    
    return result_peg_2188;
    out_peg_2189:
    
    if (column_peg_2139.chunk10 == 0){
        column_peg_2139.chunk10 = new Chunk10();
    }
    column_peg_2139.chunk10->chunk_function_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_2191 = stream.getColumn(position);
    if (column_peg_2191.chunk10 != 0 && column_peg_2191.chunk10->chunk_function_rest.calculated()){
        return column_peg_2191.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_2249(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_2192(myposition);
    
    {
    
        {
                
                result_peg_2192.reset();
                do{
                    Result result_peg_2204(result_peg_2192.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2204.getPosition()))){
                                result_peg_2204.nextPosition();
                            } else {
                                goto out_peg_2207;
                            }
                        }
                        result_peg_2204.setValue((void*) " ");
                            
                    }
                    goto success_peg_2205;
                    out_peg_2207:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2204.getPosition()))){
                                result_peg_2204.nextPosition();
                            } else {
                                goto out_peg_2209;
                            }
                        }
                        result_peg_2204.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2205;
                    out_peg_2209:
                    goto loop_peg_2203;
                    success_peg_2205:
                    ;
                    result_peg_2192.addResult(result_peg_2204);
                } while (true);
                loop_peg_2203:
                ;
                        
            }
            goto success_peg_2194;
            goto out_peg_2210;
            success_peg_2194:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2192.getPosition()))){
                    result_peg_2192.nextPosition();
                } else {
                    goto out_peg_2210;
                }
            }
            result_peg_2192.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2192.reset();
                do{
                    Result result_peg_2223(result_peg_2192.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2223.getPosition()))){
                                result_peg_2223.nextPosition();
                            } else {
                                goto out_peg_2226;
                            }
                        }
                        result_peg_2223.setValue((void*) " ");
                            
                    }
                    goto success_peg_2224;
                    out_peg_2226:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2223.getPosition()))){
                                result_peg_2223.nextPosition();
                            } else {
                                goto out_peg_2228;
                            }
                        }
                        result_peg_2223.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2224;
                    out_peg_2228:
                    goto loop_peg_2222;
                    success_peg_2224:
                    ;
                    result_peg_2192.addResult(result_peg_2223);
                } while (true);
                loop_peg_2222:
                ;
                        
            }
            goto success_peg_2213;
            goto out_peg_2210;
            success_peg_2213:
            ;
        
        
        
        int save_peg_2230 = result_peg_2192.getPosition();
            
            result_peg_2192 = rule_all_compare(stream, result_peg_2192.getPosition());
            if (result_peg_2192.error()){
                
                result_peg_2192 = Result(save_peg_2230);
                result_peg_2192.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2192.reset();
                do{
                    Result result_peg_2242(result_peg_2192.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2242.getPosition()))){
                                result_peg_2242.nextPosition();
                            } else {
                                goto out_peg_2245;
                            }
                        }
                        result_peg_2242.setValue((void*) " ");
                            
                    }
                    goto success_peg_2243;
                    out_peg_2245:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2242.getPosition()))){
                                result_peg_2242.nextPosition();
                            } else {
                                goto out_peg_2247;
                            }
                        }
                        result_peg_2242.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2243;
                    out_peg_2247:
                    goto loop_peg_2241;
                    success_peg_2243:
                    ;
                    result_peg_2192.addResult(result_peg_2242);
                } while (true);
                loop_peg_2241:
                ;
                        
            }
            goto success_peg_2232;
            goto out_peg_2210;
            success_peg_2232:
            ;
        
        
        
        result_peg_2192 = rule_value(stream, result_peg_2192.getPosition());
            if (result_peg_2192.error()){
                goto out_peg_2210;
            }
        
        
    }
    
    if (column_peg_2191.chunk10 == 0){
        column_peg_2191.chunk10 = new Chunk10();
    }
    column_peg_2191.chunk10->chunk_function_rest = result_peg_2192;
    stream.update(result_peg_2192.getPosition());
    
    
    return result_peg_2192;
    out_peg_2210:
    Result result_peg_2248(myposition);
    
    
    
    if (column_peg_2191.chunk10 == 0){
        column_peg_2191.chunk10 = new Chunk10();
    }
    column_peg_2191.chunk10->chunk_function_rest = result_peg_2248;
    stream.update(result_peg_2248.getPosition());
    
    
    return result_peg_2248;
    
    if (column_peg_2191.chunk10 == 0){
        column_peg_2191.chunk10 = new Chunk10();
    }
    column_peg_2191.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_2250 = stream.getColumn(position);
    if (column_peg_2250.chunk10 != 0 && column_peg_2250.chunk10->chunk_keys.calculated()){
        return column_peg_2250.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_2253(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_2251(myposition);
    
    result_peg_2251 = rule_key_value_list(stream, result_peg_2251.getPosition());
    if (result_peg_2251.error()){
        goto out_peg_2252;
    }
    
    if (column_peg_2250.chunk10 == 0){
        column_peg_2250.chunk10 = new Chunk10();
    }
    column_peg_2250.chunk10->chunk_keys = result_peg_2251;
    stream.update(result_peg_2251.getPosition());
    
    
    return result_peg_2251;
    out_peg_2252:
    
    if (column_peg_2250.chunk10 == 0){
        column_peg_2250.chunk10 = new Chunk10();
    }
    column_peg_2250.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_2254 = stream.getColumn(position);
    if (column_peg_2254.chunk10 != 0 && column_peg_2254.chunk10->chunk_key_value_list.calculated()){
        return column_peg_2254.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_2296(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_2255(myposition);
    
    {
    
        result_peg_2255 = rule_key(stream, result_peg_2255.getPosition());
            if (result_peg_2255.error()){
                goto out_peg_2257;
            }
            first = result_peg_2255.getValues();
        
        
        
        result_peg_2255.reset();
            do{
                Result result_peg_2260(result_peg_2255.getPosition());
                {
                
                    {
                            
                            result_peg_2260.reset();
                            do{
                                Result result_peg_2272(result_peg_2260.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2272.getPosition()))){
                                            result_peg_2272.nextPosition();
                                        } else {
                                            goto out_peg_2275;
                                        }
                                    }
                                    result_peg_2272.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2273;
                                out_peg_2275:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2272.getPosition()))){
                                            result_peg_2272.nextPosition();
                                        } else {
                                            goto out_peg_2277;
                                        }
                                    }
                                    result_peg_2272.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2273;
                                out_peg_2277:
                                goto loop_peg_2271;
                                success_peg_2273:
                                ;
                                result_peg_2260.addResult(result_peg_2272);
                            } while (true);
                            loop_peg_2271:
                            ;
                                    
                        }
                        goto success_peg_2262;
                        goto loop_peg_2259;
                        success_peg_2262:
                        ;
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_2260.getPosition()))){
                                result_peg_2260.nextPosition();
                            } else {
                                goto loop_peg_2259;
                            }
                        }
                        result_peg_2260.setValue((void*) ",");
                    
                    
                    
                    {
                            
                            result_peg_2260.reset();
                            do{
                                Result result_peg_2290(result_peg_2260.getPosition());
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar(" "[i], stream.get(result_peg_2290.getPosition()))){
                                            result_peg_2290.nextPosition();
                                        } else {
                                            goto out_peg_2293;
                                        }
                                    }
                                    result_peg_2290.setValue((void*) " ");
                                        
                                }
                                goto success_peg_2291;
                                out_peg_2293:
                                {
                                    
                                    for (int i = 0; i < 1; i++){
                                        if (compareChar("\t"[i], stream.get(result_peg_2290.getPosition()))){
                                            result_peg_2290.nextPosition();
                                        } else {
                                            goto out_peg_2295;
                                        }
                                    }
                                    result_peg_2290.setValue((void*) "\t");
                                        
                                }
                                goto success_peg_2291;
                                out_peg_2295:
                                goto loop_peg_2289;
                                success_peg_2291:
                                ;
                                result_peg_2260.addResult(result_peg_2290);
                            } while (true);
                            loop_peg_2289:
                            ;
                                    
                        }
                        goto success_peg_2280;
                        goto loop_peg_2259;
                        success_peg_2280:
                        ;
                    
                    
                    
                    result_peg_2260 = rule_key(stream, result_peg_2260.getPosition());
                        if (result_peg_2260.error()){
                            goto loop_peg_2259;
                        }
                    
                    
                }
                result_peg_2255.addResult(result_peg_2260);
            } while (true);
            loop_peg_2259:
            ;
            rest = result_peg_2255.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_2255.setValue(value);
            }
        
        
    }
    
    if (column_peg_2254.chunk10 == 0){
        column_peg_2254.chunk10 = new Chunk10();
    }
    column_peg_2254.chunk10->chunk_key_value_list = result_peg_2255;
    stream.update(result_peg_2255.getPosition());
    
    
    return result_peg_2255;
    out_peg_2257:
    
    if (column_peg_2254.chunk10 == 0){
        column_peg_2254.chunk10 = new Chunk10();
    }
    column_peg_2254.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_2297 = stream.getColumn(position);
    if (column_peg_2297.chunk10 != 0 && column_peg_2297.chunk10->chunk_key.calculated()){
        return column_peg_2297.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_2305(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_2298(myposition);
    
    {
    
        result_peg_2298 = rule_key_real(stream, result_peg_2298.getPosition());
            if (result_peg_2298.error()){
                goto out_peg_2300;
            }
        
        Result result_peg_2299 = result_peg_2298;
        
        result_peg_2298 = rule_key_rest(stream, result_peg_2298.getPosition(), result_peg_2299.getValues());
            if (result_peg_2298.error()){
                goto out_peg_2300;
            }
            ok = result_peg_2298.getValues();
        
        
        
        Result result_peg_2304(result_peg_2298);
            result_peg_2304 = rule_identifier(stream, result_peg_2304.getPosition());
            if (result_peg_2304.error()){
                goto not_peg_2303;
            }
            goto out_peg_2300;
            not_peg_2303:
            result_peg_2298.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_2298.setValue(value);
            }
        
        
    }
    
    if (column_peg_2297.chunk10 == 0){
        column_peg_2297.chunk10 = new Chunk10();
    }
    column_peg_2297.chunk10->chunk_key = result_peg_2298;
    stream.update(result_peg_2298.getPosition());
    
    
    return result_peg_2298;
    out_peg_2300:
    
    if (column_peg_2297.chunk10 == 0){
        column_peg_2297.chunk10 = new Chunk10();
    }
    column_peg_2297.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_2332(stream, "key_rest");
    int myposition = position;
    tail_peg_2308:
    Value another;
    Value new_left;
    Result result_peg_2307(myposition);
    {
    
        {
                
                result_peg_2307.reset();
                do{
                    Result result_peg_2320(result_peg_2307.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2320.getPosition()))){
                                result_peg_2320.nextPosition();
                            } else {
                                goto out_peg_2323;
                            }
                        }
                        result_peg_2320.setValue((void*) " ");
                            
                    }
                    goto success_peg_2321;
                    out_peg_2323:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2320.getPosition()))){
                                result_peg_2320.nextPosition();
                            } else {
                                goto out_peg_2325;
                            }
                        }
                        result_peg_2320.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2321;
                    out_peg_2325:
                    goto loop_peg_2319;
                    success_peg_2321:
                    ;
                    result_peg_2307.addResult(result_peg_2320);
                } while (true);
                loop_peg_2319:
                ;
                        
            }
            goto success_peg_2310;
            goto out_peg_2326;
            success_peg_2310:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_2307.getPosition()))){
                    result_peg_2307.nextPosition();
                } else {
                    goto out_peg_2326;
                }
            }
            result_peg_2307.setValue((void*) "+");
        
        
        
        result_peg_2307 = rule_key_real(stream, result_peg_2307.getPosition());
            if (result_peg_2307.error()){
                goto out_peg_2326;
            }
            another = result_peg_2307.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_2307.setValue(value);
            }
            new_left = result_peg_2307.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_2307.getPosition();
    goto tail_peg_2308;
    out_peg_2326:
    Result result_peg_2330(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_2330.setValue(value);
            }
        
        
    }
    stream.update(result_peg_2330.getPosition());
    
    return result_peg_2330;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_2333 = stream.getColumn(position);
    if (column_peg_2333.chunk11 != 0 && column_peg_2333.chunk11->chunk_key_real.calculated()){
        return column_peg_2333.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_2340(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_2334(myposition);
    
    {
    
        result_peg_2334.reset();
            do{
                Result result_peg_2337(result_peg_2334.getPosition());
                result_peg_2337 = rule_key_modifier(stream, result_peg_2337.getPosition());
                if (result_peg_2337.error()){
                    goto loop_peg_2336;
                }
                result_peg_2334.addResult(result_peg_2337);
            } while (true);
            loop_peg_2336:
            ;
            mods = result_peg_2334.getValues();
        
        
        
        result_peg_2334 = rule_key_name(stream, result_peg_2334.getPosition());
            if (result_peg_2334.error()){
                goto out_peg_2339;
            }
            name = result_peg_2334.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_2334.setValue(value);
            }
        
        
    }
    
    if (column_peg_2333.chunk11 == 0){
        column_peg_2333.chunk11 = new Chunk11();
    }
    column_peg_2333.chunk11->chunk_key_real = result_peg_2334;
    stream.update(result_peg_2334.getPosition());
    
    
    return result_peg_2334;
    out_peg_2339:
    
    if (column_peg_2333.chunk11 == 0){
        column_peg_2333.chunk11 = new Chunk11();
    }
    column_peg_2333.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_2341 = stream.getColumn(position);
    if (column_peg_2341.chunk11 != 0 && column_peg_2341.chunk11->chunk_key_modifier.calculated()){
        return column_peg_2341.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_2362(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_2342(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_2342.getPosition()))){
                    result_peg_2342.nextPosition();
                } else {
                    goto out_peg_2344;
                }
            }
            result_peg_2342.setValue((void*) "~");
        
        
        
        result_peg_2342.reset();
            do{
                Result result_peg_2347(result_peg_2342.getPosition());
                {
                    
                    char letter_peg_2352 = stream.get(result_peg_2347.getPosition());
                    if (letter_peg_2352 != '\0' && strchr("0123456789", letter_peg_2352) != NULL){
                        result_peg_2347.nextPosition();
                        result_peg_2347.setValue((void*) (long) letter_peg_2352);
                    } else {
                        goto out_peg_2351;
                    }
                    
                }
                goto success_peg_2348;
                out_peg_2351:
                goto loop_peg_2346;
                success_peg_2348:
                ;
                result_peg_2342.addResult(result_peg_2347);
            } while (true);
            loop_peg_2346:
            ;
            num = result_peg_2342.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_2342.setValue(value);
            }
        
        
    }
    
    if (column_peg_2341.chunk11 == 0){
        column_peg_2341.chunk11 = new Chunk11();
    }
    column_peg_2341.chunk11->chunk_key_modifier = result_peg_2342;
    stream.update(result_peg_2342.getPosition());
    
    
    return result_peg_2342;
    out_peg_2344:
    Result result_peg_2353(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_2353.getPosition()))){
                    result_peg_2353.nextPosition();
                } else {
                    goto out_peg_2355;
                }
            }
            result_peg_2353.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_2353.setValue(value);
            }
        
        
    }
    
    if (column_peg_2341.chunk11 == 0){
        column_peg_2341.chunk11 = new Chunk11();
    }
    column_peg_2341.chunk11->chunk_key_modifier = result_peg_2353;
    stream.update(result_peg_2353.getPosition());
    
    
    return result_peg_2353;
    out_peg_2355:
    Result result_peg_2356(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_2356.getPosition()))){
                    result_peg_2356.nextPosition();
                } else {
                    goto out_peg_2358;
                }
            }
            result_peg_2356.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_2356.setValue(value);
            }
        
        
    }
    
    if (column_peg_2341.chunk11 == 0){
        column_peg_2341.chunk11 = new Chunk11();
    }
    column_peg_2341.chunk11->chunk_key_modifier = result_peg_2356;
    stream.update(result_peg_2356.getPosition());
    
    
    return result_peg_2356;
    out_peg_2358:
    Result result_peg_2359(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_2359.getPosition()))){
                    result_peg_2359.nextPosition();
                } else {
                    goto out_peg_2361;
                }
            }
            result_peg_2359.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_2359.setValue(value);
            }
        
        
    }
    
    if (column_peg_2341.chunk11 == 0){
        column_peg_2341.chunk11 = new Chunk11();
    }
    column_peg_2341.chunk11->chunk_key_modifier = result_peg_2359;
    stream.update(result_peg_2359.getPosition());
    
    
    return result_peg_2359;
    out_peg_2361:
    
    if (column_peg_2341.chunk11 == 0){
        column_peg_2341.chunk11 = new Chunk11();
    }
    column_peg_2341.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_2363 = stream.getColumn(position);
    if (column_peg_2363.chunk11 != 0 && column_peg_2363.chunk11->chunk_key_name.calculated()){
        return column_peg_2363.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_2394(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_2364(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_2364.getPosition()))){
            result_peg_2364.nextPosition();
        } else {
            goto out_peg_2365;
        }
    }
    result_peg_2364.setValue((void*) "DB");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2364;
    stream.update(result_peg_2364.getPosition());
    
    
    return result_peg_2364;
    out_peg_2365:
    Result result_peg_2366(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_2366.getPosition()))){
            result_peg_2366.nextPosition();
        } else {
            goto out_peg_2367;
        }
    }
    result_peg_2366.setValue((void*) "B");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2366;
    stream.update(result_peg_2366.getPosition());
    
    
    return result_peg_2366;
    out_peg_2367:
    Result result_peg_2368(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_2368.getPosition()))){
            result_peg_2368.nextPosition();
        } else {
            goto out_peg_2369;
        }
    }
    result_peg_2368.setValue((void*) "DF");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2368;
    stream.update(result_peg_2368.getPosition());
    
    
    return result_peg_2368;
    out_peg_2369:
    Result result_peg_2370(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_2370.getPosition()))){
            result_peg_2370.nextPosition();
        } else {
            goto out_peg_2371;
        }
    }
    result_peg_2370.setValue((void*) "D");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2370;
    stream.update(result_peg_2370.getPosition());
    
    
    return result_peg_2370;
    out_peg_2371:
    Result result_peg_2372(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_2372.getPosition()))){
            result_peg_2372.nextPosition();
        } else {
            goto out_peg_2373;
        }
    }
    result_peg_2372.setValue((void*) "F");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2372;
    stream.update(result_peg_2372.getPosition());
    
    
    return result_peg_2372;
    out_peg_2373:
    Result result_peg_2374(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_2374.getPosition()))){
            result_peg_2374.nextPosition();
        } else {
            goto out_peg_2375;
        }
    }
    result_peg_2374.setValue((void*) "UF");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2374;
    stream.update(result_peg_2374.getPosition());
    
    
    return result_peg_2374;
    out_peg_2375:
    Result result_peg_2376(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_2376.getPosition()))){
            result_peg_2376.nextPosition();
        } else {
            goto out_peg_2377;
        }
    }
    result_peg_2376.setValue((void*) "UB");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2376;
    stream.update(result_peg_2376.getPosition());
    
    
    return result_peg_2376;
    out_peg_2377:
    Result result_peg_2378(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_2378.getPosition()))){
            result_peg_2378.nextPosition();
        } else {
            goto out_peg_2379;
        }
    }
    result_peg_2378.setValue((void*) "U");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2378;
    stream.update(result_peg_2378.getPosition());
    
    
    return result_peg_2378;
    out_peg_2379:
    Result result_peg_2380(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_2380.getPosition()))){
            result_peg_2380.nextPosition();
        } else {
            goto out_peg_2381;
        }
    }
    result_peg_2380.setValue((void*) "a");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2380;
    stream.update(result_peg_2380.getPosition());
    
    
    return result_peg_2380;
    out_peg_2381:
    Result result_peg_2382(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_2382.getPosition()))){
            result_peg_2382.nextPosition();
        } else {
            goto out_peg_2383;
        }
    }
    result_peg_2382.setValue((void*) "b");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2382;
    stream.update(result_peg_2382.getPosition());
    
    
    return result_peg_2382;
    out_peg_2383:
    Result result_peg_2384(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_2384.getPosition()))){
            result_peg_2384.nextPosition();
        } else {
            goto out_peg_2385;
        }
    }
    result_peg_2384.setValue((void*) "c");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2384;
    stream.update(result_peg_2384.getPosition());
    
    
    return result_peg_2384;
    out_peg_2385:
    Result result_peg_2386(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_2386.getPosition()))){
            result_peg_2386.nextPosition();
        } else {
            goto out_peg_2387;
        }
    }
    result_peg_2386.setValue((void*) "x");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2386;
    stream.update(result_peg_2386.getPosition());
    
    
    return result_peg_2386;
    out_peg_2387:
    Result result_peg_2388(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_2388.getPosition()))){
            result_peg_2388.nextPosition();
        } else {
            goto out_peg_2389;
        }
    }
    result_peg_2388.setValue((void*) "y");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2388;
    stream.update(result_peg_2388.getPosition());
    
    
    return result_peg_2388;
    out_peg_2389:
    Result result_peg_2390(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_2390.getPosition()))){
            result_peg_2390.nextPosition();
        } else {
            goto out_peg_2391;
        }
    }
    result_peg_2390.setValue((void*) "z");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2390;
    stream.update(result_peg_2390.getPosition());
    
    
    return result_peg_2390;
    out_peg_2391:
    Result result_peg_2392(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_2392.getPosition()))){
            result_peg_2392.nextPosition();
        } else {
            goto out_peg_2393;
        }
    }
    result_peg_2392.setValue((void*) "s");
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = result_peg_2392;
    stream.update(result_peg_2392.getPosition());
    
    
    return result_peg_2392;
    out_peg_2393:
    
    if (column_peg_2363.chunk11 == 0){
        column_peg_2363.chunk11 = new Chunk11();
    }
    column_peg_2363.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_2395 = stream.getColumn(position);
    if (column_peg_2395.chunk11 != 0 && column_peg_2395.chunk11->chunk_value.calculated()){
        return column_peg_2395.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_2413(stream, "value");
    int myposition = position;
    
    
    Result result_peg_2396(myposition);
    
    result_peg_2396 = rule_float(stream, result_peg_2396.getPosition());
    if (result_peg_2396.error()){
        goto out_peg_2397;
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2396;
    stream.update(result_peg_2396.getPosition());
    
    
    return result_peg_2396;
    out_peg_2397:
    Result result_peg_2398(myposition);
    
    result_peg_2398 = rule_integer(stream, result_peg_2398.getPosition());
    if (result_peg_2398.error()){
        goto out_peg_2399;
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2398;
    stream.update(result_peg_2398.getPosition());
    
    
    return result_peg_2398;
    out_peg_2399:
    Result result_peg_2400(myposition);
    
    result_peg_2400 = rule_keyword(stream, result_peg_2400.getPosition());
    if (result_peg_2400.error()){
        goto out_peg_2401;
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2400;
    stream.update(result_peg_2400.getPosition());
    
    
    return result_peg_2400;
    out_peg_2401:
    Result result_peg_2402(myposition);
    
    {
    
        Result result_peg_2405(result_peg_2402);
            result_peg_2405 = rule_keyword(stream, result_peg_2405.getPosition());
            if (result_peg_2405.error()){
                goto not_peg_2404;
            }
            goto out_peg_2406;
            not_peg_2404:
            result_peg_2402.setValue((void*)0);
        
        
        
        result_peg_2402 = rule_identifier(stream, result_peg_2402.getPosition());
            if (result_peg_2402.error()){
                goto out_peg_2406;
            }
        
        
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2402;
    stream.update(result_peg_2402.getPosition());
    
    
    return result_peg_2402;
    out_peg_2406:
    Result result_peg_2407(myposition);
    
    result_peg_2407 = rule_range(stream, result_peg_2407.getPosition());
    if (result_peg_2407.error()){
        goto out_peg_2408;
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2407;
    stream.update(result_peg_2407.getPosition());
    
    
    return result_peg_2407;
    out_peg_2408:
    Result result_peg_2409(myposition);
    
    result_peg_2409 = rule_string(stream, result_peg_2409.getPosition());
    if (result_peg_2409.error()){
        goto out_peg_2410;
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2409;
    stream.update(result_peg_2409.getPosition());
    
    
    return result_peg_2409;
    out_peg_2410:
    Result result_peg_2411(myposition);
    
    result_peg_2411 = rule_hitflag(stream, result_peg_2411.getPosition());
    if (result_peg_2411.error()){
        goto out_peg_2412;
    }
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = result_peg_2411;
    stream.update(result_peg_2411.getPosition());
    
    
    return result_peg_2411;
    out_peg_2412:
    
    if (column_peg_2395.chunk11 == 0){
        column_peg_2395.chunk11 = new Chunk11();
    }
    column_peg_2395.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_2414 = stream.getColumn(position);
    if (column_peg_2414.chunk12 != 0 && column_peg_2414.chunk12->chunk_helper.calculated()){
        return column_peg_2414.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_2473(stream, "helper");
    int myposition = position;
    
    Value name;
    Value id;
    Result result_peg_2415(myposition);
    
    {
    
        result_peg_2415 = rule_helper__name(stream, result_peg_2415.getPosition());
            if (result_peg_2415.error()){
                goto out_peg_2417;
            }
            name = result_peg_2415.getValues();
        
        
        
        {
                
                result_peg_2415.reset();
                do{
                    Result result_peg_2429(result_peg_2415.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2429.getPosition()))){
                                result_peg_2429.nextPosition();
                            } else {
                                goto out_peg_2432;
                            }
                        }
                        result_peg_2429.setValue((void*) " ");
                            
                    }
                    goto success_peg_2430;
                    out_peg_2432:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2429.getPosition()))){
                                result_peg_2429.nextPosition();
                            } else {
                                goto out_peg_2434;
                            }
                        }
                        result_peg_2429.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2430;
                    out_peg_2434:
                    goto loop_peg_2428;
                    success_peg_2430:
                    ;
                    result_peg_2415.addResult(result_peg_2429);
                } while (true);
                loop_peg_2428:
                ;
                        
            }
            goto success_peg_2419;
            goto out_peg_2417;
            success_peg_2419:
            ;
        
        
        
        int save_peg_2436 = result_peg_2415.getPosition();
            
            result_peg_2415 = rule_helper__expression(stream, result_peg_2415.getPosition());
            if (result_peg_2415.error()){
                
                result_peg_2415 = Result(save_peg_2436);
                result_peg_2415.setValue((void*) 0);
                
            }
        
        
        
        {
                
                result_peg_2415.reset();
                do{
                    Result result_peg_2448(result_peg_2415.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2448.getPosition()))){
                                result_peg_2448.nextPosition();
                            } else {
                                goto out_peg_2451;
                            }
                        }
                        result_peg_2448.setValue((void*) " ");
                            
                    }
                    goto success_peg_2449;
                    out_peg_2451:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2448.getPosition()))){
                                result_peg_2448.nextPosition();
                            } else {
                                goto out_peg_2453;
                            }
                        }
                        result_peg_2448.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2449;
                    out_peg_2453:
                    goto loop_peg_2447;
                    success_peg_2449:
                    ;
                    result_peg_2415.addResult(result_peg_2448);
                } while (true);
                loop_peg_2447:
                ;
                        
            }
            goto success_peg_2438;
            goto out_peg_2417;
            success_peg_2438:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_2415.getPosition()))){
                    result_peg_2415.nextPosition();
                } else {
                    goto out_peg_2417;
                }
            }
            result_peg_2415.setValue((void*) ",");
        
        
        
        {
                
                result_peg_2415.reset();
                do{
                    Result result_peg_2466(result_peg_2415.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2466.getPosition()))){
                                result_peg_2466.nextPosition();
                            } else {
                                goto out_peg_2469;
                            }
                        }
                        result_peg_2466.setValue((void*) " ");
                            
                    }
                    goto success_peg_2467;
                    out_peg_2469:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2466.getPosition()))){
                                result_peg_2466.nextPosition();
                            } else {
                                goto out_peg_2471;
                            }
                        }
                        result_peg_2466.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2467;
                    out_peg_2471:
                    goto loop_peg_2465;
                    success_peg_2467:
                    ;
                    result_peg_2415.addResult(result_peg_2466);
                } while (true);
                loop_peg_2465:
                ;
                        
            }
            goto success_peg_2456;
            goto out_peg_2417;
            success_peg_2456:
            ;
        
        
        
        result_peg_2415 = rule_helper__identifier(stream, result_peg_2415.getPosition());
            if (result_peg_2415.error()){
                goto out_peg_2417;
            }
            id = result_peg_2415.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeHelper(name, id);
                result_peg_2415.setValue(value);
            }
        
        
    }
    
    if (column_peg_2414.chunk12 == 0){
        column_peg_2414.chunk12 = new Chunk12();
    }
    column_peg_2414.chunk12->chunk_helper = result_peg_2415;
    stream.update(result_peg_2415.getPosition());
    
    
    return result_peg_2415;
    out_peg_2417:
    
    if (column_peg_2414.chunk12 == 0){
        column_peg_2414.chunk12 = new Chunk12();
    }
    column_peg_2414.chunk12->chunk_helper = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__expression(Stream & stream, const int position){
    
    Column & column_peg_2474 = stream.getColumn(position);
    if (column_peg_2474.chunk12 != 0 && column_peg_2474.chunk12->chunk_helper__expression.calculated()){
        return column_peg_2474.chunk12->chunk_helper__expression;
    }
    
    RuleTrace trace_peg_2513(stream, "helper__expression");
    int myposition = position;
    
    
    Result result_peg_2475(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_2475.getPosition()))){
                    result_peg_2475.nextPosition();
                } else {
                    goto out_peg_2477;
                }
            }
            result_peg_2475.setValue((void*) "(");
        
        
        
        {
                
                result_peg_2475.reset();
                do{
                    Result result_peg_2489(result_peg_2475.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2489.getPosition()))){
                                result_peg_2489.nextPosition();
                            } else {
                                goto out_peg_2492;
                            }
                        }
                        result_peg_2489.setValue((void*) " ");
                            
                    }
                    goto success_peg_2490;
                    out_peg_2492:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2489.getPosition()))){
                                result_peg_2489.nextPosition();
                            } else {
                                goto out_peg_2494;
                            }
                        }
                        result_peg_2489.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2490;
                    out_peg_2494:
                    goto loop_peg_2488;
                    success_peg_2490:
                    ;
                    result_peg_2475.addResult(result_peg_2489);
                } while (true);
                loop_peg_2488:
                ;
                        
            }
            goto success_peg_2479;
            goto out_peg_2477;
            success_peg_2479:
            ;
        
        
        
        result_peg_2475 = rule_expr_c(stream, result_peg_2475.getPosition());
            if (result_peg_2475.error()){
                goto out_peg_2477;
            }
        
        
        
        {
                
                result_peg_2475.reset();
                do{
                    Result result_peg_2507(result_peg_2475.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2507.getPosition()))){
                                result_peg_2507.nextPosition();
                            } else {
                                goto out_peg_2510;
                            }
                        }
                        result_peg_2507.setValue((void*) " ");
                            
                    }
                    goto success_peg_2508;
                    out_peg_2510:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2507.getPosition()))){
                                result_peg_2507.nextPosition();
                            } else {
                                goto out_peg_2512;
                            }
                        }
                        result_peg_2507.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2508;
                    out_peg_2512:
                    goto loop_peg_2506;
                    success_peg_2508:
                    ;
                    result_peg_2475.addResult(result_peg_2507);
                } while (true);
                loop_peg_2506:
                ;
                        
            }
            goto success_peg_2497;
            goto out_peg_2477;
            success_peg_2497:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_2475.getPosition()))){
                    result_peg_2475.nextPosition();
                } else {
                    goto out_peg_2477;
                }
            }
            result_peg_2475.setValue((void*) ")");
        
        
    }
    
    if (column_peg_2474.chunk12 == 0){
        column_peg_2474.chunk12 = new Chunk12();
    }
    column_peg_2474.chunk12->chunk_helper__expression = result_peg_2475;
    stream.update(result_peg_2475.getPosition());
    
    
    return result_peg_2475;
    out_peg_2477:
    
    if (column_peg_2474.chunk12 == 0){
        column_peg_2474.chunk12 = new Chunk12();
    }
    column_peg_2474.chunk12->chunk_helper__expression = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_2514 = stream.getColumn(position);
    if (column_peg_2514.chunk12 != 0 && column_peg_2514.chunk12->chunk_helper__name.calculated()){
        return column_peg_2514.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_2531(stream, "helper__name");
    int myposition = position;
    
    
    Result result_peg_2515(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("parent"[i], stream.get(result_peg_2515.getPosition()))){
            result_peg_2515.nextPosition();
        } else {
            goto out_peg_2516;
        }
    }
    result_peg_2515.setValue((void*) "parent");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2515;
    stream.update(result_peg_2515.getPosition());
    
    
    return result_peg_2515;
    out_peg_2516:
    Result result_peg_2517(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("root"[i], stream.get(result_peg_2517.getPosition()))){
            result_peg_2517.nextPosition();
        } else {
            goto out_peg_2518;
        }
    }
    result_peg_2517.setValue((void*) "root");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2517;
    stream.update(result_peg_2517.getPosition());
    
    
    return result_peg_2517;
    out_peg_2518:
    Result result_peg_2519(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("helper"[i], stream.get(result_peg_2519.getPosition()))){
            result_peg_2519.nextPosition();
        } else {
            goto out_peg_2520;
        }
    }
    result_peg_2519.setValue((void*) "helper");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2519;
    stream.update(result_peg_2519.getPosition());
    
    
    return result_peg_2519;
    out_peg_2520:
    Result result_peg_2521(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("target"[i], stream.get(result_peg_2521.getPosition()))){
            result_peg_2521.nextPosition();
        } else {
            goto out_peg_2522;
        }
    }
    result_peg_2521.setValue((void*) "target");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2521;
    stream.update(result_peg_2521.getPosition());
    
    
    return result_peg_2521;
    out_peg_2522:
    Result result_peg_2523(myposition);
    
    for (int i = 0; i < 7; i++){
        if (compareCharCase("partner"[i], stream.get(result_peg_2523.getPosition()))){
            result_peg_2523.nextPosition();
        } else {
            goto out_peg_2524;
        }
    }
    result_peg_2523.setValue((void*) "partner");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2523;
    stream.update(result_peg_2523.getPosition());
    
    
    return result_peg_2523;
    out_peg_2524:
    Result result_peg_2525(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemyNear"[i], stream.get(result_peg_2525.getPosition()))){
            result_peg_2525.nextPosition();
        } else {
            goto out_peg_2526;
        }
    }
    result_peg_2525.setValue((void*) "enemyNear");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2525;
    stream.update(result_peg_2525.getPosition());
    
    
    return result_peg_2525;
    out_peg_2526:
    Result result_peg_2527(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("enemy"[i], stream.get(result_peg_2527.getPosition()))){
            result_peg_2527.nextPosition();
        } else {
            goto out_peg_2528;
        }
    }
    result_peg_2527.setValue((void*) "enemy");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2527;
    stream.update(result_peg_2527.getPosition());
    
    
    return result_peg_2527;
    out_peg_2528:
    Result result_peg_2529(myposition);
    
    for (int i = 0; i < 8; i++){
        if (compareCharCase("playerid"[i], stream.get(result_peg_2529.getPosition()))){
            result_peg_2529.nextPosition();
        } else {
            goto out_peg_2530;
        }
    }
    result_peg_2529.setValue((void*) "playerid");
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = result_peg_2529;
    stream.update(result_peg_2529.getPosition());
    
    
    return result_peg_2529;
    out_peg_2530:
    
    if (column_peg_2514.chunk12 == 0){
        column_peg_2514.chunk12 = new Chunk12();
    }
    column_peg_2514.chunk12->chunk_helper__name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_2532 = stream.getColumn(position);
    if (column_peg_2532.chunk12 != 0 && column_peg_2532.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_2532.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_2542(stream, "helper__identifier");
    int myposition = position;
    
    
    Result result_peg_2533(myposition);
    
    result_peg_2533 = rule_function(stream, result_peg_2533.getPosition());
    if (result_peg_2533.error()){
        goto out_peg_2534;
    }
    
    if (column_peg_2532.chunk12 == 0){
        column_peg_2532.chunk12 = new Chunk12();
    }
    column_peg_2532.chunk12->chunk_helper__identifier = result_peg_2533;
    stream.update(result_peg_2533.getPosition());
    
    
    return result_peg_2533;
    out_peg_2534:
    Result result_peg_2535(myposition);
    
    result_peg_2535 = rule_keyword(stream, result_peg_2535.getPosition());
    if (result_peg_2535.error()){
        goto out_peg_2536;
    }
    
    if (column_peg_2532.chunk12 == 0){
        column_peg_2532.chunk12 = new Chunk12();
    }
    column_peg_2532.chunk12->chunk_helper__identifier = result_peg_2535;
    stream.update(result_peg_2535.getPosition());
    
    
    return result_peg_2535;
    out_peg_2536:
    Result result_peg_2537(myposition);
    
    {
    
        Result result_peg_2540(result_peg_2537);
            result_peg_2540 = rule_keyword(stream, result_peg_2540.getPosition());
            if (result_peg_2540.error()){
                goto not_peg_2539;
            }
            goto out_peg_2541;
            not_peg_2539:
            result_peg_2537.setValue((void*)0);
        
        
        
        result_peg_2537 = rule_identifier(stream, result_peg_2537.getPosition());
            if (result_peg_2537.error()){
                goto out_peg_2541;
            }
        
        
    }
    
    if (column_peg_2532.chunk12 == 0){
        column_peg_2532.chunk12 = new Chunk12();
    }
    column_peg_2532.chunk12->chunk_helper__identifier = result_peg_2537;
    stream.update(result_peg_2537.getPosition());
    
    
    return result_peg_2537;
    out_peg_2541:
    
    if (column_peg_2532.chunk12 == 0){
        column_peg_2532.chunk12 = new Chunk12();
    }
    column_peg_2532.chunk12->chunk_helper__identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_2543 = stream.getColumn(position);
    if (column_peg_2543.chunk12 != 0 && column_peg_2543.chunk12->chunk_hitflag.calculated()){
        return column_peg_2543.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_2565(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_2544(myposition);
    
    {
    
        result_peg_2544.reset();
            do{
                Result result_peg_2547(result_peg_2544.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("A"[i], stream.get(result_peg_2547.getPosition()))){
                            result_peg_2547.nextPosition();
                        } else {
                            goto out_peg_2550;
                        }
                    }
                    result_peg_2547.setValue((void*) "A");
                        
                }
                goto success_peg_2548;
                out_peg_2550:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("M"[i], stream.get(result_peg_2547.getPosition()))){
                            result_peg_2547.nextPosition();
                        } else {
                            goto out_peg_2552;
                        }
                    }
                    result_peg_2547.setValue((void*) "M");
                        
                }
                goto success_peg_2548;
                out_peg_2552:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_2547.getPosition()))){
                            result_peg_2547.nextPosition();
                        } else {
                            goto out_peg_2554;
                        }
                    }
                    result_peg_2547.setValue((void*) "F");
                        
                }
                goto success_peg_2548;
                out_peg_2554:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("D"[i], stream.get(result_peg_2547.getPosition()))){
                            result_peg_2547.nextPosition();
                        } else {
                            goto out_peg_2556;
                        }
                    }
                    result_peg_2547.setValue((void*) "D");
                        
                }
                goto success_peg_2548;
                out_peg_2556:
                goto loop_peg_2546;
                success_peg_2548:
                ;
                result_peg_2544.addResult(result_peg_2547);
            } while (true);
            loop_peg_2546:
            if (result_peg_2544.matches() == 0){
                goto out_peg_2557;
            }
        
        
        
        int save_peg_2559 = result_peg_2544.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_2544.getPosition()))){
                        result_peg_2544.nextPosition();
                    } else {
                        goto out_peg_2562;
                    }
                }
                result_peg_2544.setValue((void*) "+");
                    
            }
            goto success_peg_2560;
            out_peg_2562:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_2544.getPosition()))){
                        result_peg_2544.nextPosition();
                    } else {
                        goto out_peg_2564;
                    }
                }
                result_peg_2544.setValue((void*) "-");
                    
            }
            goto success_peg_2560;
            out_peg_2564:
            
            result_peg_2544 = Result(save_peg_2559);
            result_peg_2544.setValue((void*) 0);
            
            success_peg_2560:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = makeSimpleIdentifier("A");
                result_peg_2544.setValue(value);
            }
        
        
    }
    
    if (column_peg_2543.chunk12 == 0){
        column_peg_2543.chunk12 = new Chunk12();
    }
    column_peg_2543.chunk12->chunk_hitflag = result_peg_2544;
    stream.update(result_peg_2544.getPosition());
    
    
    return result_peg_2544;
    out_peg_2557:
    
    if (column_peg_2543.chunk12 == 0){
        column_peg_2543.chunk12 = new Chunk12();
    }
    column_peg_2543.chunk12->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_2566 = stream.getColumn(position);
    if (column_peg_2566.chunk13 != 0 && column_peg_2566.chunk13->chunk_keyword.calculated()){
        return column_peg_2566.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_2573(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_2567(myposition);
    
    {
    
        result_peg_2567 = rule_keyword_real(stream, result_peg_2567.getPosition());
            if (result_peg_2567.error()){
                goto out_peg_2569;
            }
        
        Result result_peg_2568 = result_peg_2567;
        
        Result result_peg_2572(result_peg_2567);
            result_peg_2572 = rule_alpha_digit(stream, result_peg_2572.getPosition());
            if (result_peg_2572.error()){
                goto not_peg_2571;
            }
            goto out_peg_2569;
            not_peg_2571:
            result_peg_2567.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_2568.getValues();
                result_peg_2567.setValue(value);
            }
        
        
    }
    
    if (column_peg_2566.chunk13 == 0){
        column_peg_2566.chunk13 = new Chunk13();
    }
    column_peg_2566.chunk13->chunk_keyword = result_peg_2567;
    stream.update(result_peg_2567.getPosition());
    
    
    return result_peg_2567;
    out_peg_2569:
    
    if (column_peg_2566.chunk13 == 0){
        column_peg_2566.chunk13 = new Chunk13();
    }
    column_peg_2566.chunk13->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_2574 = stream.getColumn(position);
    if (column_peg_2574.chunk13 != 0 && column_peg_2574.chunk13->chunk_keyword_real.calculated()){
        return column_peg_2574.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_2976(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_2575(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2575.getPosition()))){
                    result_peg_2575.nextPosition();
                } else {
                    goto out_peg_2577;
                }
            }
            result_peg_2575.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2575.reset();
                do{
                    Result result_peg_2589(result_peg_2575.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2589.getPosition()))){
                                result_peg_2589.nextPosition();
                            } else {
                                goto out_peg_2592;
                            }
                        }
                        result_peg_2589.setValue((void*) " ");
                            
                    }
                    goto success_peg_2590;
                    out_peg_2592:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2589.getPosition()))){
                                result_peg_2589.nextPosition();
                            } else {
                                goto out_peg_2594;
                            }
                        }
                        result_peg_2589.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2590;
                    out_peg_2594:
                    goto loop_peg_2588;
                    success_peg_2590:
                    ;
                    result_peg_2575.addResult(result_peg_2589);
                } while (true);
                loop_peg_2588:
                ;
                        
            }
            goto success_peg_2579;
            goto out_peg_2577;
            success_peg_2579:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2575.getPosition()))){
                    result_peg_2575.nextPosition();
                } else {
                    goto out_peg_2577;
                }
            }
            result_peg_2575.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_2575.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2575;
    stream.update(result_peg_2575.getPosition());
    
    
    return result_peg_2575;
    out_peg_2577:
    Result result_peg_2596(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_2596.getPosition()))){
                    result_peg_2596.nextPosition();
                } else {
                    goto out_peg_2598;
                }
            }
            result_peg_2596.setValue((void*) "vel");
        
        
        
        {
                
                result_peg_2596.reset();
                do{
                    Result result_peg_2610(result_peg_2596.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2610.getPosition()))){
                                result_peg_2610.nextPosition();
                            } else {
                                goto out_peg_2613;
                            }
                        }
                        result_peg_2610.setValue((void*) " ");
                            
                    }
                    goto success_peg_2611;
                    out_peg_2613:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2610.getPosition()))){
                                result_peg_2610.nextPosition();
                            } else {
                                goto out_peg_2615;
                            }
                        }
                        result_peg_2610.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2611;
                    out_peg_2615:
                    goto loop_peg_2609;
                    success_peg_2611:
                    ;
                    result_peg_2596.addResult(result_peg_2610);
                } while (true);
                loop_peg_2609:
                ;
                        
            }
            goto success_peg_2600;
            goto out_peg_2598;
            success_peg_2600:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2596.getPosition()))){
                    result_peg_2596.nextPosition();
                } else {
                    goto out_peg_2598;
                }
            }
            result_peg_2596.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_2596.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2596;
    stream.update(result_peg_2596.getPosition());
    
    
    return result_peg_2596;
    out_peg_2598:
    Result result_peg_2617(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2617.getPosition()))){
                    result_peg_2617.nextPosition();
                } else {
                    goto out_peg_2619;
                }
            }
            result_peg_2617.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2617.reset();
                do{
                    Result result_peg_2631(result_peg_2617.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2631.getPosition()))){
                                result_peg_2631.nextPosition();
                            } else {
                                goto out_peg_2634;
                            }
                        }
                        result_peg_2631.setValue((void*) " ");
                            
                    }
                    goto success_peg_2632;
                    out_peg_2634:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2631.getPosition()))){
                                result_peg_2631.nextPosition();
                            } else {
                                goto out_peg_2636;
                            }
                        }
                        result_peg_2631.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2632;
                    out_peg_2636:
                    goto loop_peg_2630;
                    success_peg_2632:
                    ;
                    result_peg_2617.addResult(result_peg_2631);
                } while (true);
                loop_peg_2630:
                ;
                        
            }
            goto success_peg_2621;
            goto out_peg_2619;
            success_peg_2621:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2617.getPosition()))){
                    result_peg_2617.nextPosition();
                } else {
                    goto out_peg_2619;
                }
            }
            result_peg_2617.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_2617.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2617;
    stream.update(result_peg_2617.getPosition());
    
    
    return result_peg_2617;
    out_peg_2619:
    Result result_peg_2638(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_2638.getPosition()))){
                    result_peg_2638.nextPosition();
                } else {
                    goto out_peg_2640;
                }
            }
            result_peg_2638.setValue((void*) "pos");
        
        
        
        {
                
                result_peg_2638.reset();
                do{
                    Result result_peg_2652(result_peg_2638.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2652.getPosition()))){
                                result_peg_2652.nextPosition();
                            } else {
                                goto out_peg_2655;
                            }
                        }
                        result_peg_2652.setValue((void*) " ");
                            
                    }
                    goto success_peg_2653;
                    out_peg_2655:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2652.getPosition()))){
                                result_peg_2652.nextPosition();
                            } else {
                                goto out_peg_2657;
                            }
                        }
                        result_peg_2652.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2653;
                    out_peg_2657:
                    goto loop_peg_2651;
                    success_peg_2653:
                    ;
                    result_peg_2638.addResult(result_peg_2652);
                } while (true);
                loop_peg_2651:
                ;
                        
            }
            goto success_peg_2642;
            goto out_peg_2640;
            success_peg_2642:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2638.getPosition()))){
                    result_peg_2638.nextPosition();
                } else {
                    goto out_peg_2640;
                }
            }
            result_peg_2638.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_2638.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2638;
    stream.update(result_peg_2638.getPosition());
    
    
    return result_peg_2638;
    out_peg_2640:
    Result result_peg_2659(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2659.getPosition()))){
                    result_peg_2659.nextPosition();
                } else {
                    goto out_peg_2661;
                }
            }
            result_peg_2659.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2659.reset();
                do{
                    Result result_peg_2673(result_peg_2659.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2673.getPosition()))){
                                result_peg_2673.nextPosition();
                            } else {
                                goto out_peg_2676;
                            }
                        }
                        result_peg_2673.setValue((void*) " ");
                            
                    }
                    goto success_peg_2674;
                    out_peg_2676:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2673.getPosition()))){
                                result_peg_2673.nextPosition();
                            } else {
                                goto out_peg_2678;
                            }
                        }
                        result_peg_2673.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2674;
                    out_peg_2678:
                    goto loop_peg_2672;
                    success_peg_2674:
                    ;
                    result_peg_2659.addResult(result_peg_2673);
                } while (true);
                loop_peg_2672:
                ;
                        
            }
            goto success_peg_2663;
            goto out_peg_2661;
            success_peg_2663:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2659.getPosition()))){
                    result_peg_2659.nextPosition();
                } else {
                    goto out_peg_2661;
                }
            }
            result_peg_2659.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_2659.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2659;
    stream.update(result_peg_2659.getPosition());
    
    
    return result_peg_2659;
    out_peg_2661:
    Result result_peg_2680(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_2680.getPosition()))){
                    result_peg_2680.nextPosition();
                } else {
                    goto out_peg_2682;
                }
            }
            result_peg_2680.setValue((void*) "p2dist");
        
        
        
        {
                
                result_peg_2680.reset();
                do{
                    Result result_peg_2694(result_peg_2680.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2694.getPosition()))){
                                result_peg_2694.nextPosition();
                            } else {
                                goto out_peg_2697;
                            }
                        }
                        result_peg_2694.setValue((void*) " ");
                            
                    }
                    goto success_peg_2695;
                    out_peg_2697:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2694.getPosition()))){
                                result_peg_2694.nextPosition();
                            } else {
                                goto out_peg_2699;
                            }
                        }
                        result_peg_2694.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2695;
                    out_peg_2699:
                    goto loop_peg_2693;
                    success_peg_2695:
                    ;
                    result_peg_2680.addResult(result_peg_2694);
                } while (true);
                loop_peg_2693:
                ;
                        
            }
            goto success_peg_2684;
            goto out_peg_2682;
            success_peg_2684:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2680.getPosition()))){
                    result_peg_2680.nextPosition();
                } else {
                    goto out_peg_2682;
                }
            }
            result_peg_2680.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_2680.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2680;
    stream.update(result_peg_2680.getPosition());
    
    
    return result_peg_2680;
    out_peg_2682:
    Result result_peg_2701(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2701.getPosition()))){
                    result_peg_2701.nextPosition();
                } else {
                    goto out_peg_2703;
                }
            }
            result_peg_2701.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2701.reset();
                do{
                    Result result_peg_2715(result_peg_2701.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2715.getPosition()))){
                                result_peg_2715.nextPosition();
                            } else {
                                goto out_peg_2718;
                            }
                        }
                        result_peg_2715.setValue((void*) " ");
                            
                    }
                    goto success_peg_2716;
                    out_peg_2718:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2715.getPosition()))){
                                result_peg_2715.nextPosition();
                            } else {
                                goto out_peg_2720;
                            }
                        }
                        result_peg_2715.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2716;
                    out_peg_2720:
                    goto loop_peg_2714;
                    success_peg_2716:
                    ;
                    result_peg_2701.addResult(result_peg_2715);
                } while (true);
                loop_peg_2714:
                ;
                        
            }
            goto success_peg_2705;
            goto out_peg_2703;
            success_peg_2705:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2701.getPosition()))){
                    result_peg_2701.nextPosition();
                } else {
                    goto out_peg_2703;
                }
            }
            result_peg_2701.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_2701.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2701;
    stream.update(result_peg_2701.getPosition());
    
    
    return result_peg_2701;
    out_peg_2703:
    Result result_peg_2722(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_2722.getPosition()))){
                    result_peg_2722.nextPosition();
                } else {
                    goto out_peg_2724;
                }
            }
            result_peg_2722.setValue((void*) "p1dist");
        
        
        
        {
                
                result_peg_2722.reset();
                do{
                    Result result_peg_2736(result_peg_2722.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2736.getPosition()))){
                                result_peg_2736.nextPosition();
                            } else {
                                goto out_peg_2739;
                            }
                        }
                        result_peg_2736.setValue((void*) " ");
                            
                    }
                    goto success_peg_2737;
                    out_peg_2739:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2736.getPosition()))){
                                result_peg_2736.nextPosition();
                            } else {
                                goto out_peg_2741;
                            }
                        }
                        result_peg_2736.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2737;
                    out_peg_2741:
                    goto loop_peg_2735;
                    success_peg_2737:
                    ;
                    result_peg_2722.addResult(result_peg_2736);
                } while (true);
                loop_peg_2735:
                ;
                        
            }
            goto success_peg_2726;
            goto out_peg_2724;
            success_peg_2726:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2722.getPosition()))){
                    result_peg_2722.nextPosition();
                } else {
                    goto out_peg_2724;
                }
            }
            result_peg_2722.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_2722.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2722;
    stream.update(result_peg_2722.getPosition());
    
    
    return result_peg_2722;
    out_peg_2724:
    Result result_peg_2743(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2743.getPosition()))){
                    result_peg_2743.nextPosition();
                } else {
                    goto out_peg_2745;
                }
            }
            result_peg_2743.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2743.reset();
                do{
                    Result result_peg_2757(result_peg_2743.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2757.getPosition()))){
                                result_peg_2757.nextPosition();
                            } else {
                                goto out_peg_2760;
                            }
                        }
                        result_peg_2757.setValue((void*) " ");
                            
                    }
                    goto success_peg_2758;
                    out_peg_2760:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2757.getPosition()))){
                                result_peg_2757.nextPosition();
                            } else {
                                goto out_peg_2762;
                            }
                        }
                        result_peg_2757.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2758;
                    out_peg_2762:
                    goto loop_peg_2756;
                    success_peg_2758:
                    ;
                    result_peg_2743.addResult(result_peg_2757);
                } while (true);
                loop_peg_2756:
                ;
                        
            }
            goto success_peg_2747;
            goto out_peg_2745;
            success_peg_2747:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2743.getPosition()))){
                    result_peg_2743.nextPosition();
                } else {
                    goto out_peg_2745;
                }
            }
            result_peg_2743.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_2743.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2743;
    stream.update(result_peg_2743.getPosition());
    
    
    return result_peg_2743;
    out_peg_2745:
    Result result_peg_2764(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_2764.getPosition()))){
                    result_peg_2764.nextPosition();
                } else {
                    goto out_peg_2766;
                }
            }
            result_peg_2764.setValue((void*) "p2bodydist");
        
        
        
        {
                
                result_peg_2764.reset();
                do{
                    Result result_peg_2778(result_peg_2764.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2778.getPosition()))){
                                result_peg_2778.nextPosition();
                            } else {
                                goto out_peg_2781;
                            }
                        }
                        result_peg_2778.setValue((void*) " ");
                            
                    }
                    goto success_peg_2779;
                    out_peg_2781:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2778.getPosition()))){
                                result_peg_2778.nextPosition();
                            } else {
                                goto out_peg_2783;
                            }
                        }
                        result_peg_2778.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2779;
                    out_peg_2783:
                    goto loop_peg_2777;
                    success_peg_2779:
                    ;
                    result_peg_2764.addResult(result_peg_2778);
                } while (true);
                loop_peg_2777:
                ;
                        
            }
            goto success_peg_2768;
            goto out_peg_2766;
            success_peg_2768:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2764.getPosition()))){
                    result_peg_2764.nextPosition();
                } else {
                    goto out_peg_2766;
                }
            }
            result_peg_2764.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_2764.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2764;
    stream.update(result_peg_2764.getPosition());
    
    
    return result_peg_2764;
    out_peg_2766:
    Result result_peg_2785(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2785.getPosition()))){
                    result_peg_2785.nextPosition();
                } else {
                    goto out_peg_2787;
                }
            }
            result_peg_2785.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2785.reset();
                do{
                    Result result_peg_2799(result_peg_2785.getPosition());
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
                    result_peg_2785.addResult(result_peg_2799);
                } while (true);
                loop_peg_2798:
                ;
                        
            }
            goto success_peg_2789;
            goto out_peg_2787;
            success_peg_2789:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2785.getPosition()))){
                    result_peg_2785.nextPosition();
                } else {
                    goto out_peg_2787;
                }
            }
            result_peg_2785.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_2785.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2785;
    stream.update(result_peg_2785.getPosition());
    
    
    return result_peg_2785;
    out_peg_2787:
    Result result_peg_2806(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_2806.getPosition()))){
                    result_peg_2806.nextPosition();
                } else {
                    goto out_peg_2808;
                }
            }
            result_peg_2806.setValue((void*) "p1bodydist");
        
        
        
        {
                
                result_peg_2806.reset();
                do{
                    Result result_peg_2820(result_peg_2806.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2820.getPosition()))){
                                result_peg_2820.nextPosition();
                            } else {
                                goto out_peg_2823;
                            }
                        }
                        result_peg_2820.setValue((void*) " ");
                            
                    }
                    goto success_peg_2821;
                    out_peg_2823:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2820.getPosition()))){
                                result_peg_2820.nextPosition();
                            } else {
                                goto out_peg_2825;
                            }
                        }
                        result_peg_2820.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2821;
                    out_peg_2825:
                    goto loop_peg_2819;
                    success_peg_2821:
                    ;
                    result_peg_2806.addResult(result_peg_2820);
                } while (true);
                loop_peg_2819:
                ;
                        
            }
            goto success_peg_2810;
            goto out_peg_2808;
            success_peg_2810:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2806.getPosition()))){
                    result_peg_2806.nextPosition();
                } else {
                    goto out_peg_2808;
                }
            }
            result_peg_2806.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_2806.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2806;
    stream.update(result_peg_2806.getPosition());
    
    
    return result_peg_2806;
    out_peg_2808:
    Result result_peg_2827(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2827.getPosition()))){
                    result_peg_2827.nextPosition();
                } else {
                    goto out_peg_2829;
                }
            }
            result_peg_2827.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2827.reset();
                do{
                    Result result_peg_2841(result_peg_2827.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2841.getPosition()))){
                                result_peg_2841.nextPosition();
                            } else {
                                goto out_peg_2844;
                            }
                        }
                        result_peg_2841.setValue((void*) " ");
                            
                    }
                    goto success_peg_2842;
                    out_peg_2844:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2841.getPosition()))){
                                result_peg_2841.nextPosition();
                            } else {
                                goto out_peg_2846;
                            }
                        }
                        result_peg_2841.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2842;
                    out_peg_2846:
                    goto loop_peg_2840;
                    success_peg_2842:
                    ;
                    result_peg_2827.addResult(result_peg_2841);
                } while (true);
                loop_peg_2840:
                ;
                        
            }
            goto success_peg_2831;
            goto out_peg_2829;
            success_peg_2831:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2827.getPosition()))){
                    result_peg_2827.nextPosition();
                } else {
                    goto out_peg_2829;
                }
            }
            result_peg_2827.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_2827.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2827;
    stream.update(result_peg_2827.getPosition());
    
    
    return result_peg_2827;
    out_peg_2829:
    Result result_peg_2848(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_2848.getPosition()))){
                    result_peg_2848.nextPosition();
                } else {
                    goto out_peg_2850;
                }
            }
            result_peg_2848.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_2848.reset();
                do{
                    Result result_peg_2862(result_peg_2848.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2862.getPosition()))){
                                result_peg_2862.nextPosition();
                            } else {
                                goto out_peg_2865;
                            }
                        }
                        result_peg_2862.setValue((void*) " ");
                            
                    }
                    goto success_peg_2863;
                    out_peg_2865:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2862.getPosition()))){
                                result_peg_2862.nextPosition();
                            } else {
                                goto out_peg_2867;
                            }
                        }
                        result_peg_2862.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2863;
                    out_peg_2867:
                    goto loop_peg_2861;
                    success_peg_2863:
                    ;
                    result_peg_2848.addResult(result_peg_2862);
                } while (true);
                loop_peg_2861:
                ;
                        
            }
            goto success_peg_2852;
            goto out_peg_2850;
            success_peg_2852:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2848.getPosition()))){
                    result_peg_2848.nextPosition();
                } else {
                    goto out_peg_2850;
                }
            }
            result_peg_2848.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_2848.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2848;
    stream.update(result_peg_2848.getPosition());
    
    
    return result_peg_2848;
    out_peg_2850:
    Result result_peg_2869(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_2869.getPosition()))){
                    result_peg_2869.nextPosition();
                } else {
                    goto out_peg_2871;
                }
            }
            result_peg_2869.setValue((void*) "screenpos");
        
        
        
        {
                
                result_peg_2869.reset();
                do{
                    Result result_peg_2883(result_peg_2869.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2883.getPosition()))){
                                result_peg_2883.nextPosition();
                            } else {
                                goto out_peg_2886;
                            }
                        }
                        result_peg_2883.setValue((void*) " ");
                            
                    }
                    goto success_peg_2884;
                    out_peg_2886:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2883.getPosition()))){
                                result_peg_2883.nextPosition();
                            } else {
                                goto out_peg_2888;
                            }
                        }
                        result_peg_2883.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2884;
                    out_peg_2888:
                    goto loop_peg_2882;
                    success_peg_2884:
                    ;
                    result_peg_2869.addResult(result_peg_2883);
                } while (true);
                loop_peg_2882:
                ;
                        
            }
            goto success_peg_2873;
            goto out_peg_2871;
            success_peg_2873:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2869.getPosition()))){
                    result_peg_2869.nextPosition();
                } else {
                    goto out_peg_2871;
                }
            }
            result_peg_2869.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_2869.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2869;
    stream.update(result_peg_2869.getPosition());
    
    
    return result_peg_2869;
    out_peg_2871:
    Result result_peg_2890(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2890.getPosition()))){
                    result_peg_2890.nextPosition();
                } else {
                    goto out_peg_2892;
                }
            }
            result_peg_2890.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2890.reset();
                do{
                    Result result_peg_2904(result_peg_2890.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2904.getPosition()))){
                                result_peg_2904.nextPosition();
                            } else {
                                goto out_peg_2907;
                            }
                        }
                        result_peg_2904.setValue((void*) " ");
                            
                    }
                    goto success_peg_2905;
                    out_peg_2907:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2904.getPosition()))){
                                result_peg_2904.nextPosition();
                            } else {
                                goto out_peg_2909;
                            }
                        }
                        result_peg_2904.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2905;
                    out_peg_2909:
                    goto loop_peg_2903;
                    success_peg_2905:
                    ;
                    result_peg_2890.addResult(result_peg_2904);
                } while (true);
                loop_peg_2903:
                ;
                        
            }
            goto success_peg_2894;
            goto out_peg_2892;
            success_peg_2894:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2890.getPosition()))){
                    result_peg_2890.nextPosition();
                } else {
                    goto out_peg_2892;
                }
            }
            result_peg_2890.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_2890.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2890;
    stream.update(result_peg_2890.getPosition());
    
    
    return result_peg_2890;
    out_peg_2892:
    Result result_peg_2911(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_2911.getPosition()))){
                    result_peg_2911.nextPosition();
                } else {
                    goto out_peg_2913;
                }
            }
            result_peg_2911.setValue((void*) "parentdist");
        
        
        
        {
                
                result_peg_2911.reset();
                do{
                    Result result_peg_2925(result_peg_2911.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2925.getPosition()))){
                                result_peg_2925.nextPosition();
                            } else {
                                goto out_peg_2928;
                            }
                        }
                        result_peg_2925.setValue((void*) " ");
                            
                    }
                    goto success_peg_2926;
                    out_peg_2928:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2925.getPosition()))){
                                result_peg_2925.nextPosition();
                            } else {
                                goto out_peg_2930;
                            }
                        }
                        result_peg_2925.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2926;
                    out_peg_2930:
                    goto loop_peg_2924;
                    success_peg_2926:
                    ;
                    result_peg_2911.addResult(result_peg_2925);
                } while (true);
                loop_peg_2924:
                ;
                        
            }
            goto success_peg_2915;
            goto out_peg_2913;
            success_peg_2915:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2911.getPosition()))){
                    result_peg_2911.nextPosition();
                } else {
                    goto out_peg_2913;
                }
            }
            result_peg_2911.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist y");
                result_peg_2911.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2911;
    stream.update(result_peg_2911.getPosition());
    
    
    return result_peg_2911;
    out_peg_2913:
    Result result_peg_2932(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_2932.getPosition()))){
                    result_peg_2932.nextPosition();
                } else {
                    goto out_peg_2934;
                }
            }
            result_peg_2932.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_2932.reset();
                do{
                    Result result_peg_2946(result_peg_2932.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2946.getPosition()))){
                                result_peg_2946.nextPosition();
                            } else {
                                goto out_peg_2949;
                            }
                        }
                        result_peg_2946.setValue((void*) " ");
                            
                    }
                    goto success_peg_2947;
                    out_peg_2949:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2946.getPosition()))){
                                result_peg_2946.nextPosition();
                            } else {
                                goto out_peg_2951;
                            }
                        }
                        result_peg_2946.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2947;
                    out_peg_2951:
                    goto loop_peg_2945;
                    success_peg_2947:
                    ;
                    result_peg_2932.addResult(result_peg_2946);
                } while (true);
                loop_peg_2945:
                ;
                        
            }
            goto success_peg_2936;
            goto out_peg_2934;
            success_peg_2936:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_2932.getPosition()))){
                    result_peg_2932.nextPosition();
                } else {
                    goto out_peg_2934;
                }
            }
            result_peg_2932.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist x");
                result_peg_2932.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2932;
    stream.update(result_peg_2932.getPosition());
    
    
    return result_peg_2932;
    out_peg_2934:
    Result result_peg_2953(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("rootdist"[i], stream.get(result_peg_2953.getPosition()))){
                    result_peg_2953.nextPosition();
                } else {
                    goto out_peg_2955;
                }
            }
            result_peg_2953.setValue((void*) "rootdist");
        
        
        
        {
                
                result_peg_2953.reset();
                do{
                    Result result_peg_2967(result_peg_2953.getPosition());
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar(" "[i], stream.get(result_peg_2967.getPosition()))){
                                result_peg_2967.nextPosition();
                            } else {
                                goto out_peg_2970;
                            }
                        }
                        result_peg_2967.setValue((void*) " ");
                            
                    }
                    goto success_peg_2968;
                    out_peg_2970:
                    {
                        
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\t"[i], stream.get(result_peg_2967.getPosition()))){
                                result_peg_2967.nextPosition();
                            } else {
                                goto out_peg_2972;
                            }
                        }
                        result_peg_2967.setValue((void*) "\t");
                            
                    }
                    goto success_peg_2968;
                    out_peg_2972:
                    goto loop_peg_2966;
                    success_peg_2968:
                    ;
                    result_peg_2953.addResult(result_peg_2967);
                } while (true);
                loop_peg_2966:
                ;
                        
            }
            goto success_peg_2957;
            goto out_peg_2955;
            success_peg_2957:
            ;
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_2953.getPosition()))){
                    result_peg_2953.nextPosition();
                } else {
                    goto out_peg_2955;
                }
            }
            result_peg_2953.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("rootdist y");
                result_peg_2953.setValue(value);
            }
        
        
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2953;
    stream.update(result_peg_2953.getPosition());
    
    
    return result_peg_2953;
    out_peg_2955:
    Result result_peg_2974(myposition);
    
    result_peg_2974 = rule_hitflag(stream, result_peg_2974.getPosition());
    if (result_peg_2974.error()){
        goto out_peg_2975;
    }
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = result_peg_2974;
    stream.update(result_peg_2974.getPosition());
    
    
    return result_peg_2974;
    out_peg_2975:
    
    if (column_peg_2574.chunk13 == 0){
        column_peg_2574.chunk13 = new Chunk13();
    }
    column_peg_2574.chunk13->chunk_keyword_real = errorResult;
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

        
