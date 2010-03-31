

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

    inline const bool isList() const {
        return which == 1;
    }

    inline const bool isData() const {
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

    inline const int getPosition() const {
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

    inline const int matches() const {
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
    Result chunk_function_rest;
    Result chunk_keys;
    Result chunk_key_value_list;
    Result chunk_key;
    Result chunk_key_rest;
};

struct Chunk11{
    Result chunk_key_real;
    Result chunk_key_modifier;
    Result chunk_key_name;
    Result chunk_value;
    Result chunk_hitflag;
};

struct Chunk12{
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
        ,chunk12(0){
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

    int hitCount(){
        return 
(chunk12 != NULL ? ((chunk12->chunk_keyword.calculated() ? 1 : 0)
+ (chunk12->chunk_keyword_real.calculated() ? 1 : 0)) : 0)
+
(chunk11 != NULL ? ((chunk11->chunk_key_real.calculated() ? 1 : 0)
+ (chunk11->chunk_key_modifier.calculated() ? 1 : 0)
+ (chunk11->chunk_key_name.calculated() ? 1 : 0)
+ (chunk11->chunk_value.calculated() ? 1 : 0)
+ (chunk11->chunk_hitflag.calculated() ? 1 : 0)) : 0)
+
(chunk10 != NULL ? ((chunk10->chunk_function_rest.calculated() ? 1 : 0)
+ (chunk10->chunk_keys.calculated() ? 1 : 0)
+ (chunk10->chunk_key_value_list.calculated() ? 1 : 0)
+ (chunk10->chunk_key.calculated() ? 1 : 0)
+ (chunk10->chunk_key_rest.calculated() ? 1 : 0)) : 0)
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
        return 62;
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
    Stream(const std::string & filename):
    temp(0),
    buffer(0),
    farthest(0){
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

        createMemo();
    }

    Stream(const char * in):
    temp(0),
    buffer(in),
    farthest(0){
        max = strlen(buffer);
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
};

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
Result rule_function_rest(Stream &, const int);
Result rule_keys(Stream &, const int);
Result rule_key_value_list(Stream &, const int);
Result rule_key(Stream &, const int);
Result rule_key_rest(Stream &, const int, Value left);
Result rule_key_real(Stream &, const int);
Result rule_key_modifier(Stream &, const int);
Result rule_key_name(Stream &, const int);
Result rule_value(Stream &, const int);
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

Ast::Value * makeUnaryExpression(const Value & unaries, const Value & exp){
    Ast::Value * expression = as<Ast::Value*>(exp);
    for (Value::iterator it = unaries.getValues().begin(); it != unaries.getValues().end(); it++){
        Ast::ExpressionUnary::UnaryType unary = (Ast::ExpressionUnary::UnaryType) (long) (*it).getValue();
        expression = new Ast::ExpressionUnary(unary, expression);
        GC::save(expression);
    }
    return expression;
}

Ast::Value * makeFunction(const Value & name, const Value & arg1){
    Ast::Value * function = new Ast::Function(std::string(as<const char*>(name)), as<Ast::Value*>(arg1));
    GC::save(function);
    return function;
}

Ast::Value * makeFunction(const Value & name, const Value & arg1, const Value & arg2, const Value & arg3){
    Ast::Value * function = new Ast::Function(std::string(as<const char*>(name)),
                                              as<Ast::Value*>(arg1),
                                              as<Ast::Value*>(arg2),
                                              as<Ast::Value*>(arg3));
    GC::save(function);
    return function;
}

Ast::Value * makeRange(Ast::Range::RangeType type, const Value & low, const Value & high){
    Ast::Value * range = new Ast::Range(type, as<Ast::Value*>(low), as<Ast::Value*>(high));
    GC::save(range);
    return range;
}

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string> ids;
    ids.push_back(*as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        /* this works becuase as() will coerce a void* into Value(void*) */
        ids.push_back(*as<std::string*>((*it).getValue()));
    }
    Ast::Identifier * object = new Ast::Identifier(ids);
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

Ast::Identifier * makeSimpleIdentifier(const Value & name){
    Ast::Identifier * identifier = new Ast::SimpleIdentifier(as<const char *>(name));
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
    
    RuleTrace trace_peg_30(stream, "line");
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
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_31 = stream.getColumn(position);
    if (column_peg_31.chunk0 != 0 && column_peg_31.chunk0->chunk_line_end.calculated()){
        return column_peg_31.chunk0->chunk_line_end;
    }
    
    RuleTrace trace_peg_45(stream, "line_end");
    int myposition = position;
    
    
    Result result_peg_32(myposition);
    
    result_peg_32.reset();
    do{
        Result result_peg_34(result_peg_32.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_34.getPosition()))){
                    result_peg_34.nextPosition();
                } else {
                    goto out_peg_37;
                }
            }
            result_peg_34.setValue((void*) "\n");
                
        }
        goto success_peg_35;
        out_peg_37:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_34.getPosition()))){
                    result_peg_34.nextPosition();
                } else {
                    goto out_peg_39;
                }
            }
            result_peg_34.setValue((void*) "\r");
                
        }
        goto success_peg_35;
        out_peg_39:
        goto loop_peg_33;
        success_peg_35:
        ;
        result_peg_32.addResult(result_peg_34);
    } while (true);
    loop_peg_33:
    if (result_peg_32.matches() == 0){
        goto out_peg_40;
    }
    
    if (column_peg_31.chunk0 == 0){
        column_peg_31.chunk0 = new Chunk0();
    }
    column_peg_31.chunk0->chunk_line_end = result_peg_32;
    stream.update(result_peg_32.getPosition());
    
    
    return result_peg_32;
    out_peg_40:
    Result result_peg_41(myposition);
    
    {
    
        Result result_peg_43(result_peg_41.getPosition());
            if ('\0' == stream.get(result_peg_43.getPosition())){
                result_peg_43.nextPosition();
                result_peg_43.setValue((void *) '\0');
            } else {
                goto out_peg_44;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_31.chunk0 == 0){
        column_peg_31.chunk0 = new Chunk0();
    }
    column_peg_31.chunk0->chunk_line_end = result_peg_41;
    stream.update(result_peg_41.getPosition());
    
    
    return result_peg_41;
    out_peg_44:
    
    if (column_peg_31.chunk0 == 0){
        column_peg_31.chunk0 = new Chunk0();
    }
    column_peg_31.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_46 = stream.getColumn(position);
    if (column_peg_46.chunk0 != 0 && column_peg_46.chunk0->chunk_whitespace.calculated()){
        return column_peg_46.chunk0->chunk_whitespace;
    }
    
    RuleTrace trace_peg_50(stream, "whitespace");
    int myposition = position;
    
    
    Result result_peg_47(myposition);
    
    result_peg_47.reset();
    do{
        Result result_peg_49(result_peg_47.getPosition());
        result_peg_49 = rule_sw(stream, result_peg_49.getPosition());
        if (result_peg_49.error()){
            goto loop_peg_48;
        }
        result_peg_47.addResult(result_peg_49);
    } while (true);
    loop_peg_48:
    ;
    
    if (column_peg_46.chunk0 == 0){
        column_peg_46.chunk0 = new Chunk0();
    }
    column_peg_46.chunk0->chunk_whitespace = result_peg_47;
    stream.update(result_peg_47.getPosition());
    
    
    return result_peg_47;
    
    if (column_peg_46.chunk0 == 0){
        column_peg_46.chunk0 = new Chunk0();
    }
    column_peg_46.chunk0->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_51 = stream.getColumn(position);
    if (column_peg_51.chunk0 != 0 && column_peg_51.chunk0->chunk_sw.calculated()){
        return column_peg_51.chunk0->chunk_sw;
    }
    
    RuleTrace trace_peg_61(stream, "sw");
    int myposition = position;
    
    
    Result result_peg_52(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_52.getPosition()))){
                result_peg_52.nextPosition();
            } else {
                goto out_peg_55;
            }
        }
        result_peg_52.setValue((void*) " ");
            
    }
    goto success_peg_53;
    out_peg_55:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_52.getPosition()))){
                result_peg_52.nextPosition();
            } else {
                goto out_peg_57;
            }
        }
        result_peg_52.setValue((void*) "\t");
            
    }
    goto success_peg_53;
    out_peg_57:
    goto out_peg_58;
    success_peg_53:
    ;
    
    if (column_peg_51.chunk0 == 0){
        column_peg_51.chunk0 = new Chunk0();
    }
    column_peg_51.chunk0->chunk_sw = result_peg_52;
    stream.update(result_peg_52.getPosition());
    
    
    return result_peg_52;
    out_peg_58:
    Result result_peg_59(myposition);
    
    result_peg_59 = rule_comment(stream, result_peg_59.getPosition());
    if (result_peg_59.error()){
        goto out_peg_60;
    }
    
    if (column_peg_51.chunk0 == 0){
        column_peg_51.chunk0 = new Chunk0();
    }
    column_peg_51.chunk0->chunk_sw = result_peg_59;
    stream.update(result_peg_59.getPosition());
    
    
    return result_peg_59;
    out_peg_60:
    
    if (column_peg_51.chunk0 == 0){
        column_peg_51.chunk0 = new Chunk0();
    }
    column_peg_51.chunk0->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_62 = stream.getColumn(position);
    if (column_peg_62.chunk1 != 0 && column_peg_62.chunk1->chunk_s.calculated()){
        return column_peg_62.chunk1->chunk_s;
    }
    
    RuleTrace trace_peg_71(stream, "s");
    int myposition = position;
    
    
    Result result_peg_63(myposition);
    
    result_peg_63.reset();
    do{
        Result result_peg_65(result_peg_63.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar(" "[i], stream.get(result_peg_65.getPosition()))){
                    result_peg_65.nextPosition();
                } else {
                    goto out_peg_68;
                }
            }
            result_peg_65.setValue((void*) " ");
                
        }
        goto success_peg_66;
        out_peg_68:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\t"[i], stream.get(result_peg_65.getPosition()))){
                    result_peg_65.nextPosition();
                } else {
                    goto out_peg_70;
                }
            }
            result_peg_65.setValue((void*) "\t");
                
        }
        goto success_peg_66;
        out_peg_70:
        goto loop_peg_64;
        success_peg_66:
        ;
        result_peg_63.addResult(result_peg_65);
    } while (true);
    loop_peg_64:
    ;
    
    if (column_peg_62.chunk1 == 0){
        column_peg_62.chunk1 = new Chunk1();
    }
    column_peg_62.chunk1->chunk_s = result_peg_63;
    stream.update(result_peg_63.getPosition());
    
    
    return result_peg_63;
    
    if (column_peg_62.chunk1 == 0){
        column_peg_62.chunk1 = new Chunk1();
    }
    column_peg_62.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_72 = stream.getColumn(position);
    if (column_peg_72.chunk1 != 0 && column_peg_72.chunk1->chunk_comment.calculated()){
        return column_peg_72.chunk1->chunk_comment;
    }
    
    RuleTrace trace_peg_101(stream, "comment");
    int myposition = position;
    
    
    Result result_peg_73(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_73.getPosition()))){
                    result_peg_73.nextPosition();
                } else {
                    goto out_peg_75;
                }
            }
            result_peg_73.setValue((void*) ";");
        
        
        
        result_peg_73.reset();
            do{
                Result result_peg_77(result_peg_73.getPosition());
                {
                
                    Result result_peg_80(result_peg_77);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_80.getPosition()))){
                                result_peg_80.nextPosition();
                            } else {
                                goto not_peg_79;
                            }
                        }
                        result_peg_80.setValue((void*) "\n");
                        goto loop_peg_76;
                        not_peg_79:
                        result_peg_77.setValue((void*)0);
                    
                    
                    
                    char temp_peg_81 = stream.get(result_peg_77.getPosition());
                        if (temp_peg_81 != '\0'){
                            result_peg_77.setValue((void*) (long) temp_peg_81);
                            result_peg_77.nextPosition();
                        } else {
                            goto loop_peg_76;
                        }
                    
                    
                }
                result_peg_73.addResult(result_peg_77);
            } while (true);
            loop_peg_76:
            ;
        
        
    }
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_comment = result_peg_73;
    stream.update(result_peg_73.getPosition());
    
    
    return result_peg_73;
    out_peg_75:
    Result result_peg_82(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_82.getPosition()))){
                    result_peg_82.nextPosition();
                } else {
                    goto out_peg_84;
                }
            }
            result_peg_82.setValue((void*) "=");
        
        
        
        result_peg_82.reset();
            do{
                Result result_peg_86(result_peg_82.getPosition());
                {
                
                    Result result_peg_89(result_peg_86);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_89.getPosition()))){
                                result_peg_89.nextPosition();
                            } else {
                                goto not_peg_88;
                            }
                        }
                        result_peg_89.setValue((void*) "\n");
                        goto loop_peg_85;
                        not_peg_88:
                        result_peg_86.setValue((void*)0);
                    
                    
                    
                    char temp_peg_90 = stream.get(result_peg_86.getPosition());
                        if (temp_peg_90 != '\0'){
                            result_peg_86.setValue((void*) (long) temp_peg_90);
                            result_peg_86.nextPosition();
                        } else {
                            goto loop_peg_85;
                        }
                    
                    
                }
                result_peg_82.addResult(result_peg_86);
            } while (true);
            loop_peg_85:
            ;
        
        
    }
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_comment = result_peg_82;
    stream.update(result_peg_82.getPosition());
    
    
    return result_peg_82;
    out_peg_84:
    Result result_peg_91(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_91.getPosition()))){
                    result_peg_91.nextPosition();
                } else {
                    goto out_peg_93;
                }
            }
            result_peg_91.setValue((void*) "-");
        
        
        
        result_peg_91.reset();
            do{
                Result result_peg_95(result_peg_91.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_95.getPosition()))){
                        result_peg_95.nextPosition();
                    } else {
                        goto loop_peg_94;
                    }
                }
                result_peg_95.setValue((void*) "-");
                result_peg_91.addResult(result_peg_95);
            } while (true);
            loop_peg_94:
            if (result_peg_91.matches() == 0){
                goto out_peg_93;
            }
        
        
    }
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_comment = result_peg_91;
    stream.update(result_peg_91.getPosition());
    
    
    return result_peg_91;
    out_peg_93:
    Result result_peg_96(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(":"[i], stream.get(result_peg_96.getPosition()))){
                    result_peg_96.nextPosition();
                } else {
                    goto out_peg_98;
                }
            }
            result_peg_96.setValue((void*) ":");
        
        
        
        result_peg_96.reset();
            do{
                Result result_peg_100(result_peg_96.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar(":"[i], stream.get(result_peg_100.getPosition()))){
                        result_peg_100.nextPosition();
                    } else {
                        goto loop_peg_99;
                    }
                }
                result_peg_100.setValue((void*) ":");
                result_peg_96.addResult(result_peg_100);
            } while (true);
            loop_peg_99:
            if (result_peg_96.matches() == 0){
                goto out_peg_98;
            }
        
        
    }
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_comment = result_peg_96;
    stream.update(result_peg_96.getPosition());
    
    
    return result_peg_96;
    out_peg_98:
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_102 = stream.getColumn(position);
    if (column_peg_102.chunk1 != 0 && column_peg_102.chunk1->chunk_section.calculated()){
        return column_peg_102.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_121(stream, "section");
    int myposition = position;
    
    Value ast;
    Result result_peg_103(myposition);
    
    {
    
        result_peg_103 = rule_section_title(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        Result result_peg_104 = result_peg_103;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_104.getValues());
                result_peg_103.setValue(value);
            }
            ast = result_peg_103.getValues();
        
        
        
        result_peg_103 = rule_whitespace(stream, result_peg_103.getPosition());
            if (result_peg_103.error()){
                goto out_peg_105;
            }
        
        
        
        result_peg_103.reset();
            do{
                Result result_peg_110(result_peg_103.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_110.getPosition()))){
                            result_peg_110.nextPosition();
                        } else {
                            goto out_peg_113;
                        }
                    }
                    result_peg_110.setValue((void*) "\n");
                        
                }
                goto success_peg_111;
                out_peg_113:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_110.getPosition()))){
                            result_peg_110.nextPosition();
                        } else {
                            goto out_peg_115;
                        }
                    }
                    result_peg_110.setValue((void*) "\r");
                        
                }
                goto success_peg_111;
                out_peg_115:
                goto loop_peg_109;
                success_peg_111:
                ;
                result_peg_103.addResult(result_peg_110);
            } while (true);
            loop_peg_109:
            if (result_peg_103.matches() == 0){
                goto out_peg_105;
            }
        
        
        
        result_peg_103.reset();
            do{
                Result result_peg_118(result_peg_103.getPosition());
                {
                
                    result_peg_118 = rule_section_item(stream, result_peg_118.getPosition(), ast);
                        if (result_peg_118.error()){
                            goto loop_peg_117;
                        }
                    
                    
                    
                    result_peg_118 = rule_whitespace(stream, result_peg_118.getPosition());
                        if (result_peg_118.error()){
                            goto loop_peg_117;
                        }
                    
                    
                    
                    result_peg_118 = rule_line_end(stream, result_peg_118.getPosition());
                        if (result_peg_118.error()){
                            goto loop_peg_117;
                        }
                    
                    
                }
                result_peg_103.addResult(result_peg_118);
            } while (true);
            loop_peg_117:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_103.setValue(value);
            }
        
        
    }
    
    if (column_peg_102.chunk1 == 0){
        column_peg_102.chunk1 = new Chunk1();
    }
    column_peg_102.chunk1->chunk_section = result_peg_103;
    stream.update(result_peg_103.getPosition());
    
    
    return result_peg_103;
    out_peg_105:
    
    if (column_peg_102.chunk1 == 0){
        column_peg_102.chunk1 = new Chunk1();
    }
    column_peg_102.chunk1->chunk_section = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_130(stream, "section_item");
    int myposition = position;
    
    Value data;
    Result result_peg_123(myposition);
    
    {
    
        result_peg_123 = rule_s(stream, result_peg_123.getPosition());
            if (result_peg_123.error()){
                goto out_peg_125;
            }
        
        
        
        result_peg_123 = rule_comment(stream, result_peg_123.getPosition());
            if (result_peg_123.error()){
                goto out_peg_125;
            }
        
        
    }
    stream.update(result_peg_123.getPosition());
    
    return result_peg_123;
    out_peg_125:
    Result result_peg_126(myposition);
    
    {
    
        result_peg_126 = rule_s(stream, result_peg_126.getPosition());
            if (result_peg_126.error()){
                goto out_peg_128;
            }
        
        
        
        result_peg_126 = rule_assignment(stream, result_peg_126.getPosition());
            if (result_peg_126.error()){
                goto out_peg_128;
            }
            data = result_peg_126.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_126.setValue(value);
            }
        
        
    }
    stream.update(result_peg_126.getPosition());
    
    return result_peg_126;
    out_peg_128:
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_131 = stream.getColumn(position);
    if (column_peg_131.chunk1 != 0 && column_peg_131.chunk1->chunk_section_title.calculated()){
        return column_peg_131.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_144(stream, "section_title");
    int myposition = position;
    
    Value name;
    Result result_peg_132(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_132.getPosition()))){
                    result_peg_132.nextPosition();
                } else {
                    goto out_peg_134;
                }
            }
            result_peg_132.setValue((void*) "[");
        
        
        
        result_peg_132 = rule_s(stream, result_peg_132.getPosition());
            if (result_peg_132.error()){
                goto out_peg_134;
            }
        
        
        
        result_peg_132.reset();
            do{
                Result result_peg_138(result_peg_132.getPosition());
                {
                
                    Result result_peg_141(result_peg_138);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_141.getPosition()))){
                                result_peg_141.nextPosition();
                            } else {
                                goto not_peg_140;
                            }
                        }
                        result_peg_141.setValue((void*) "]");
                        goto loop_peg_137;
                        not_peg_140:
                        result_peg_138.setValue((void*)0);
                    
                    
                    
                    char temp_peg_142 = stream.get(result_peg_138.getPosition());
                        if (temp_peg_142 != '\0'){
                            result_peg_138.setValue((void*) (long) temp_peg_142);
                            result_peg_138.nextPosition();
                        } else {
                            goto loop_peg_137;
                        }
                    
                    
                }
                result_peg_132.addResult(result_peg_138);
            } while (true);
            loop_peg_137:
            if (result_peg_132.matches() == 0){
                goto out_peg_134;
            }
            name = result_peg_132.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_132.getPosition()))){
                    result_peg_132.nextPosition();
                } else {
                    goto out_peg_134;
                }
            }
            result_peg_132.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(name);
                result_peg_132.setValue(value);
            }
        
        
    }
    
    if (column_peg_131.chunk1 == 0){
        column_peg_131.chunk1 = new Chunk1();
    }
    column_peg_131.chunk1->chunk_section_title = result_peg_132;
    stream.update(result_peg_132.getPosition());
    
    
    return result_peg_132;
    out_peg_134:
    
    if (column_peg_131.chunk1 == 0){
        column_peg_131.chunk1 = new Chunk1();
    }
    column_peg_131.chunk1->chunk_section_title = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_145 = stream.getColumn(position);
    if (column_peg_145.chunk2 != 0 && column_peg_145.chunk2->chunk_assignment.calculated()){
        return column_peg_145.chunk2->chunk_assignment;
    }
    
    RuleTrace trace_peg_181(stream, "assignment");
    int myposition = position;
    
    Value all;
    Value name;
    Value exp;
    Value index;
    Result result_peg_146(myposition);
    
    {
    
        for (int i = 0; i < 7; i++){
                if (compareCharCase("command"[i], stream.get(result_peg_146.getPosition()))){
                    result_peg_146.nextPosition();
                } else {
                    goto out_peg_148;
                }
            }
            result_peg_146.setValue((void*) "command");
        
        
        
        result_peg_146 = rule_s(stream, result_peg_146.getPosition());
            if (result_peg_146.error()){
                goto out_peg_148;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_146.getPosition()))){
                    result_peg_146.nextPosition();
                } else {
                    goto out_peg_148;
                }
            }
            result_peg_146.setValue((void*) "=");
        
        
        
        result_peg_146 = rule_s(stream, result_peg_146.getPosition());
            if (result_peg_146.error()){
                goto out_peg_148;
            }
        
        
        
        result_peg_146 = rule_keys(stream, result_peg_146.getPosition());
            if (result_peg_146.error()){
                goto out_peg_148;
            }
            all = result_peg_146.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("command", all);
                result_peg_146.setValue(value);
            }
        
        
    }
    
    if (column_peg_145.chunk2 == 0){
        column_peg_145.chunk2 = new Chunk2();
    }
    column_peg_145.chunk2->chunk_assignment = result_peg_146;
    stream.update(result_peg_146.getPosition());
    
    
    return result_peg_146;
    out_peg_148:
    Result result_peg_153(myposition);
    
    {
    
        result_peg_153 = rule_identifier(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            name = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) "=");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_expr(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            exp = result_peg_153.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
                result_peg_153.setValue(value);
            }
        
        
    }
    
    if (column_peg_145.chunk2 == 0){
        column_peg_145.chunk2 = new Chunk2();
    }
    column_peg_145.chunk2->chunk_assignment = result_peg_153;
    stream.update(result_peg_153.getPosition());
    
    
    return result_peg_153;
    out_peg_155:
    Result result_peg_160(myposition);
    
    {
    
        result_peg_160 = rule_identifier(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
            name = result_peg_160.getValues();
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_160.getPosition()))){
                    result_peg_160.nextPosition();
                } else {
                    goto out_peg_162;
                }
            }
            result_peg_160.setValue((void*) "=");
        
        
        
        result_peg_160 = rule_s(stream, result_peg_160.getPosition());
            if (result_peg_160.error()){
                goto out_peg_162;
            }
        
        
        
        Result result_peg_167(result_peg_160.getPosition());
            result_peg_167 = rule_line_end(stream, result_peg_167.getPosition());
            if (result_peg_167.error()){
                goto out_peg_162;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_160.setValue(value);
            }
        
        
    }
    
    if (column_peg_145.chunk2 == 0){
        column_peg_145.chunk2 = new Chunk2();
    }
    column_peg_145.chunk2->chunk_assignment = result_peg_160;
    stream.update(result_peg_160.getPosition());
    
    
    return result_peg_160;
    out_peg_162:
    Result result_peg_168(myposition);
    
    {
    
        result_peg_168 = rule_identifier(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
            name = result_peg_168.getValues();
        
        
        
        result_peg_168 = rule_s(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_168.getPosition()))){
                    result_peg_168.nextPosition();
                } else {
                    goto out_peg_170;
                }
            }
            result_peg_168.setValue((void*) "(");
        
        
        
        result_peg_168 = rule_s(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
        
        
        
        result_peg_168 = rule_integer(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
            index = result_peg_168.getValues();
        
        
        
        result_peg_168 = rule_s(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_168.getPosition()))){
                    result_peg_168.nextPosition();
                } else {
                    goto out_peg_170;
                }
            }
            result_peg_168.setValue((void*) ")");
        
        
        
        result_peg_168 = rule_s(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_168.getPosition()))){
                    result_peg_168.nextPosition();
                } else {
                    goto out_peg_170;
                }
            }
            result_peg_168.setValue((void*) "=");
        
        
        
        result_peg_168 = rule_s(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
        
        
        
        result_peg_168 = rule_expr(stream, result_peg_168.getPosition());
            if (result_peg_168.error()){
                goto out_peg_170;
            }
            exp = result_peg_168.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_168.setValue(value);
            }
        
        
    }
    
    if (column_peg_145.chunk2 == 0){
        column_peg_145.chunk2 = new Chunk2();
    }
    column_peg_145.chunk2->chunk_assignment = result_peg_168;
    stream.update(result_peg_168.getPosition());
    
    
    return result_peg_168;
    out_peg_170:
    
    if (column_peg_145.chunk2 == 0){
        column_peg_145.chunk2 = new Chunk2();
    }
    column_peg_145.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_182 = stream.getColumn(position);
    if (column_peg_182.chunk2 != 0 && column_peg_182.chunk2->chunk_identifier.calculated()){
        return column_peg_182.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_190(stream, "identifier");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_183(myposition);
    
    {
    
        result_peg_183 = rule_name(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
            first = result_peg_183.getValues();
        
        
        
        result_peg_183.reset();
            do{
                Result result_peg_188(result_peg_183.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_188.getPosition()))){
                                result_peg_188.nextPosition();
                            } else {
                                goto loop_peg_187;
                            }
                        }
                        result_peg_188.setValue((void*) ".");
                    
                    
                    
                    result_peg_188 = rule_name(stream, result_peg_188.getPosition());
                        if (result_peg_188.error()){
                            goto loop_peg_187;
                        }
                    
                    
                }
                result_peg_183.addResult(result_peg_188);
            } while (true);
            loop_peg_187:
            ;
            rest = result_peg_183.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_183.setValue(value);
            }
        
        
    }
    
    if (column_peg_182.chunk2 == 0){
        column_peg_182.chunk2 = new Chunk2();
    }
    column_peg_182.chunk2->chunk_identifier = result_peg_183;
    stream.update(result_peg_183.getPosition());
    
    
    return result_peg_183;
    out_peg_185:
    
    if (column_peg_182.chunk2 == 0){
        column_peg_182.chunk2 = new Chunk2();
    }
    column_peg_182.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_191 = stream.getColumn(position);
    if (column_peg_191.chunk2 != 0 && column_peg_191.chunk2->chunk_integer.calculated()){
        return column_peg_191.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_209(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_192(myposition);
    
    {
    
        int save_peg_194 = result_peg_192.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_192.getPosition()))){
                        result_peg_192.nextPosition();
                    } else {
                        goto out_peg_197;
                    }
                }
                result_peg_192.setValue((void*) "-");
                    
            }
            goto success_peg_195;
            out_peg_197:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_192.getPosition()))){
                        result_peg_192.nextPosition();
                    } else {
                        goto out_peg_199;
                    }
                }
                result_peg_192.setValue((void*) "+");
                    
            }
            goto success_peg_195;
            out_peg_199:
            
            result_peg_192 = Result(save_peg_194);
            result_peg_192.setValue((void*) 0);
            
            success_peg_195:
            ;
        
        Result result_peg_193 = result_peg_192;
        
        result_peg_192.reset();
            do{
                Result result_peg_202(result_peg_192.getPosition());
                {
                    
                    char letter_peg_207 = stream.get(result_peg_202.getPosition());
                    if (letter_peg_207 != '\0' && strchr("0123456789", letter_peg_207) != NULL){
                        result_peg_202.nextPosition();
                        result_peg_202.setValue((void*) (long) letter_peg_207);
                    } else {
                        goto out_peg_206;
                    }
                    
                }
                goto success_peg_203;
                out_peg_206:
                goto loop_peg_201;
                success_peg_203:
                ;
                result_peg_192.addResult(result_peg_202);
            } while (true);
            loop_peg_201:
            if (result_peg_192.matches() == 0){
                goto out_peg_208;
            }
        
        Result result_peg_200 = result_peg_192;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_193.getValues(), result_peg_200.getValues());
                result_peg_192.setValue(value);
            }
        
        
    }
    
    if (column_peg_191.chunk2 == 0){
        column_peg_191.chunk2 = new Chunk2();
    }
    column_peg_191.chunk2->chunk_integer = result_peg_192;
    stream.update(result_peg_192.getPosition());
    
    
    return result_peg_192;
    out_peg_208:
    
    if (column_peg_191.chunk2 == 0){
        column_peg_191.chunk2 = new Chunk2();
    }
    column_peg_191.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_210 = stream.getColumn(position);
    if (column_peg_210.chunk2 != 0 && column_peg_210.chunk2->chunk_float.calculated()){
        return column_peg_210.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_255(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
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
            ;
            left = result_peg_211.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_211.getPosition()))){
                    result_peg_211.nextPosition();
                } else {
                    goto out_peg_228;
                }
            }
            result_peg_211.setValue((void*) ".");
        
        
        
        result_peg_211.reset();
            do{
                Result result_peg_231(result_peg_211.getPosition());
                {
                    
                    char letter_peg_236 = stream.get(result_peg_231.getPosition());
                    if (letter_peg_236 != '\0' && strchr("0123456789", letter_peg_236) != NULL){
                        result_peg_231.nextPosition();
                        result_peg_231.setValue((void*) (long) letter_peg_236);
                    } else {
                        goto out_peg_235;
                    }
                    
                }
                goto success_peg_232;
                out_peg_235:
                goto loop_peg_230;
                success_peg_232:
                ;
                result_peg_211.addResult(result_peg_231);
            } while (true);
            loop_peg_230:
            if (result_peg_211.matches() == 0){
                goto out_peg_228;
            }
            right = result_peg_211.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_212.getValues(), parseDouble(left,right));
                result_peg_211.setValue(value);
            }
        
        
    }
    
    if (column_peg_210.chunk2 == 0){
        column_peg_210.chunk2 = new Chunk2();
    }
    column_peg_210.chunk2->chunk_float = result_peg_211;
    stream.update(result_peg_211.getPosition());
    
    
    return result_peg_211;
    out_peg_228:
    Result result_peg_237(myposition);
    
    {
    
        int save_peg_239 = result_peg_237.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_237.getPosition()))){
                        result_peg_237.nextPosition();
                    } else {
                        goto out_peg_242;
                    }
                }
                result_peg_237.setValue((void*) "-");
                    
            }
            goto success_peg_240;
            out_peg_242:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_237.getPosition()))){
                        result_peg_237.nextPosition();
                    } else {
                        goto out_peg_244;
                    }
                }
                result_peg_237.setValue((void*) "+");
                    
            }
            goto success_peg_240;
            out_peg_244:
            
            result_peg_237 = Result(save_peg_239);
            result_peg_237.setValue((void*) 0);
            
            success_peg_240:
            ;
        
        Result result_peg_238 = result_peg_237;
        
        result_peg_237.reset();
            do{
                Result result_peg_247(result_peg_237.getPosition());
                {
                    
                    char letter_peg_252 = stream.get(result_peg_247.getPosition());
                    if (letter_peg_252 != '\0' && strchr("0123456789", letter_peg_252) != NULL){
                        result_peg_247.nextPosition();
                        result_peg_247.setValue((void*) (long) letter_peg_252);
                    } else {
                        goto out_peg_251;
                    }
                    
                }
                goto success_peg_248;
                out_peg_251:
                goto loop_peg_246;
                success_peg_248:
                ;
                result_peg_237.addResult(result_peg_247);
            } while (true);
            loop_peg_246:
            if (result_peg_237.matches() == 0){
                goto out_peg_253;
            }
            left = result_peg_237.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_237.getPosition()))){
                    result_peg_237.nextPosition();
                } else {
                    goto out_peg_253;
                }
            }
            result_peg_237.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_238.getValues(), parseDouble(left));
                result_peg_237.setValue(value);
            }
        
        
    }
    
    if (column_peg_210.chunk2 == 0){
        column_peg_210.chunk2 = new Chunk2();
    }
    column_peg_210.chunk2->chunk_float = result_peg_237;
    stream.update(result_peg_237.getPosition());
    
    
    return result_peg_237;
    out_peg_253:
    
    if (column_peg_210.chunk2 == 0){
        column_peg_210.chunk2 = new Chunk2();
    }
    column_peg_210.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_256 = stream.getColumn(position);
    if (column_peg_256.chunk2 != 0 && column_peg_256.chunk2->chunk_string.calculated()){
        return column_peg_256.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_271(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_257(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_257.getPosition()))){
                    result_peg_257.nextPosition();
                } else {
                    goto out_peg_259;
                }
            }
            result_peg_257.setValue((void*) "\"");
        
        
        
        result_peg_257.reset();
            do{
                Result result_peg_262(result_peg_257.getPosition());
                {
                
                    Result result_peg_265(result_peg_262);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_265.getPosition()))){
                                result_peg_265.nextPosition();
                            } else {
                                goto not_peg_264;
                            }
                        }
                        result_peg_265.setValue((void*) "\"");
                        goto loop_peg_261;
                        not_peg_264:
                        result_peg_262.setValue((void*)0);
                    
                    
                    
                    Result result_peg_268(result_peg_262);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_268.getPosition()))){
                                result_peg_268.nextPosition();
                            } else {
                                goto not_peg_267;
                            }
                        }
                        result_peg_268.setValue((void*) "\n");
                        goto loop_peg_261;
                        not_peg_267:
                        result_peg_262.setValue((void*)0);
                    
                    
                    
                    char temp_peg_269 = stream.get(result_peg_262.getPosition());
                        if (temp_peg_269 != '\0'){
                            result_peg_262.setValue((void*) (long) temp_peg_269);
                            result_peg_262.nextPosition();
                        } else {
                            goto loop_peg_261;
                        }
                    
                    
                }
                result_peg_257.addResult(result_peg_262);
            } while (true);
            loop_peg_261:
            ;
            contents = result_peg_257.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_257.getPosition()))){
                    result_peg_257.nextPosition();
                } else {
                    goto out_peg_259;
                }
            }
            result_peg_257.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_257.setValue(value);
            }
        
        
    }
    
    if (column_peg_256.chunk2 == 0){
        column_peg_256.chunk2 = new Chunk2();
    }
    column_peg_256.chunk2->chunk_string = result_peg_257;
    stream.update(result_peg_257.getPosition());
    
    
    return result_peg_257;
    out_peg_259:
    
    if (column_peg_256.chunk2 == 0){
        column_peg_256.chunk2 = new Chunk2();
    }
    column_peg_256.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_272 = stream.getColumn(position);
    if (column_peg_272.chunk3 != 0 && column_peg_272.chunk3->chunk_range.calculated()){
        return column_peg_272.chunk3->chunk_range;
    }
    
    RuleTrace trace_peg_317(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_273(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_273.getPosition()))){
                    result_peg_273.nextPosition();
                } else {
                    goto out_peg_275;
                }
            }
            result_peg_273.setValue((void*) "[");
        
        
        
        result_peg_273 = rule_s(stream, result_peg_273.getPosition());
            if (result_peg_273.error()){
                goto out_peg_275;
            }
        
        
        
        result_peg_273 = rule_expr_c(stream, result_peg_273.getPosition());
            if (result_peg_273.error()){
                goto out_peg_275;
            }
            low = result_peg_273.getValues();
        
        
        
        result_peg_273 = rule_s(stream, result_peg_273.getPosition());
            if (result_peg_273.error()){
                goto out_peg_275;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_273.getPosition()))){
                    result_peg_273.nextPosition();
                } else {
                    goto out_peg_275;
                }
            }
            result_peg_273.setValue((void*) ",");
        
        
        
        result_peg_273 = rule_s(stream, result_peg_273.getPosition());
            if (result_peg_273.error()){
                goto out_peg_275;
            }
        
        
        
        result_peg_273 = rule_expr_c(stream, result_peg_273.getPosition());
            if (result_peg_273.error()){
                goto out_peg_275;
            }
            high = result_peg_273.getValues();
        
        
        
        result_peg_273 = rule_s(stream, result_peg_273.getPosition());
            if (result_peg_273.error()){
                goto out_peg_275;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_273.getPosition()))){
                    result_peg_273.nextPosition();
                } else {
                    goto out_peg_275;
                }
            }
            result_peg_273.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_273.setValue(value);
            }
        
        
    }
    
    if (column_peg_272.chunk3 == 0){
        column_peg_272.chunk3 = new Chunk3();
    }
    column_peg_272.chunk3->chunk_range = result_peg_273;
    stream.update(result_peg_273.getPosition());
    
    
    return result_peg_273;
    out_peg_275:
    Result result_peg_284(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_284.getPosition()))){
                    result_peg_284.nextPosition();
                } else {
                    goto out_peg_286;
                }
            }
            result_peg_284.setValue((void*) "(");
        
        
        
        result_peg_284 = rule_s(stream, result_peg_284.getPosition());
            if (result_peg_284.error()){
                goto out_peg_286;
            }
        
        
        
        result_peg_284 = rule_expr_c(stream, result_peg_284.getPosition());
            if (result_peg_284.error()){
                goto out_peg_286;
            }
            low = result_peg_284.getValues();
        
        
        
        result_peg_284 = rule_s(stream, result_peg_284.getPosition());
            if (result_peg_284.error()){
                goto out_peg_286;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_284.getPosition()))){
                    result_peg_284.nextPosition();
                } else {
                    goto out_peg_286;
                }
            }
            result_peg_284.setValue((void*) ",");
        
        
        
        result_peg_284 = rule_s(stream, result_peg_284.getPosition());
            if (result_peg_284.error()){
                goto out_peg_286;
            }
        
        
        
        result_peg_284 = rule_expr_c(stream, result_peg_284.getPosition());
            if (result_peg_284.error()){
                goto out_peg_286;
            }
            high = result_peg_284.getValues();
        
        
        
        result_peg_284 = rule_s(stream, result_peg_284.getPosition());
            if (result_peg_284.error()){
                goto out_peg_286;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_284.getPosition()))){
                    result_peg_284.nextPosition();
                } else {
                    goto out_peg_286;
                }
            }
            result_peg_284.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_284.setValue(value);
            }
        
        
    }
    
    if (column_peg_272.chunk3 == 0){
        column_peg_272.chunk3 = new Chunk3();
    }
    column_peg_272.chunk3->chunk_range = result_peg_284;
    stream.update(result_peg_284.getPosition());
    
    
    return result_peg_284;
    out_peg_286:
    Result result_peg_295(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_295.getPosition()))){
                    result_peg_295.nextPosition();
                } else {
                    goto out_peg_297;
                }
            }
            result_peg_295.setValue((void*) "(");
        
        
        
        result_peg_295 = rule_s(stream, result_peg_295.getPosition());
            if (result_peg_295.error()){
                goto out_peg_297;
            }
        
        
        
        result_peg_295 = rule_expr_c(stream, result_peg_295.getPosition());
            if (result_peg_295.error()){
                goto out_peg_297;
            }
            low = result_peg_295.getValues();
        
        
        
        result_peg_295 = rule_s(stream, result_peg_295.getPosition());
            if (result_peg_295.error()){
                goto out_peg_297;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_295.getPosition()))){
                    result_peg_295.nextPosition();
                } else {
                    goto out_peg_297;
                }
            }
            result_peg_295.setValue((void*) ",");
        
        
        
        result_peg_295 = rule_s(stream, result_peg_295.getPosition());
            if (result_peg_295.error()){
                goto out_peg_297;
            }
        
        
        
        result_peg_295 = rule_expr_c(stream, result_peg_295.getPosition());
            if (result_peg_295.error()){
                goto out_peg_297;
            }
            high = result_peg_295.getValues();
        
        
        
        result_peg_295 = rule_s(stream, result_peg_295.getPosition());
            if (result_peg_295.error()){
                goto out_peg_297;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_295.getPosition()))){
                    result_peg_295.nextPosition();
                } else {
                    goto out_peg_297;
                }
            }
            result_peg_295.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_295.setValue(value);
            }
        
        
    }
    
    if (column_peg_272.chunk3 == 0){
        column_peg_272.chunk3 = new Chunk3();
    }
    column_peg_272.chunk3->chunk_range = result_peg_295;
    stream.update(result_peg_295.getPosition());
    
    
    return result_peg_295;
    out_peg_297:
    Result result_peg_306(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_306.getPosition()))){
                    result_peg_306.nextPosition();
                } else {
                    goto out_peg_308;
                }
            }
            result_peg_306.setValue((void*) "[");
        
        
        
        result_peg_306 = rule_s(stream, result_peg_306.getPosition());
            if (result_peg_306.error()){
                goto out_peg_308;
            }
        
        
        
        result_peg_306 = rule_expr_c(stream, result_peg_306.getPosition());
            if (result_peg_306.error()){
                goto out_peg_308;
            }
            low = result_peg_306.getValues();
        
        
        
        result_peg_306 = rule_s(stream, result_peg_306.getPosition());
            if (result_peg_306.error()){
                goto out_peg_308;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_306.getPosition()))){
                    result_peg_306.nextPosition();
                } else {
                    goto out_peg_308;
                }
            }
            result_peg_306.setValue((void*) ",");
        
        
        
        result_peg_306 = rule_s(stream, result_peg_306.getPosition());
            if (result_peg_306.error()){
                goto out_peg_308;
            }
        
        
        
        result_peg_306 = rule_expr_c(stream, result_peg_306.getPosition());
            if (result_peg_306.error()){
                goto out_peg_308;
            }
            high = result_peg_306.getValues();
        
        
        
        result_peg_306 = rule_s(stream, result_peg_306.getPosition());
            if (result_peg_306.error()){
                goto out_peg_308;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_306.getPosition()))){
                    result_peg_306.nextPosition();
                } else {
                    goto out_peg_308;
                }
            }
            result_peg_306.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_306.setValue(value);
            }
        
        
    }
    
    if (column_peg_272.chunk3 == 0){
        column_peg_272.chunk3 = new Chunk3();
    }
    column_peg_272.chunk3->chunk_range = result_peg_306;
    stream.update(result_peg_306.getPosition());
    
    
    return result_peg_306;
    out_peg_308:
    
    if (column_peg_272.chunk3 == 0){
        column_peg_272.chunk3 = new Chunk3();
    }
    column_peg_272.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_318 = stream.getColumn(position);
    if (column_peg_318.chunk3 != 0 && column_peg_318.chunk3->chunk_name.calculated()){
        return column_peg_318.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_330(stream, "name");
    int myposition = position;
    
    
    Result result_peg_319(myposition);
    
    {
    
        {
                
                char letter_peg_325 = stream.get(result_peg_319.getPosition());
                if (letter_peg_325 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_325) != NULL){
                    result_peg_319.nextPosition();
                    result_peg_319.setValue((void*) (long) letter_peg_325);
                } else {
                    goto out_peg_324;
                }
                
            }
            goto success_peg_321;
            out_peg_324:
            goto out_peg_326;
            success_peg_321:
            ;
        
        Result result_peg_320 = result_peg_319;
        
        result_peg_319.reset();
            do{
                Result result_peg_329(result_peg_319.getPosition());
                result_peg_329 = rule_alpha_digit(stream, result_peg_329.getPosition());
                if (result_peg_329.error()){
                    goto loop_peg_328;
                }
                result_peg_319.addResult(result_peg_329);
            } while (true);
            loop_peg_328:
            ;
        
        Result result_peg_327 = result_peg_319;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_320.getValues().getValue(),result_peg_327.getValues());
                result_peg_319.setValue(value);
            }
        
        
    }
    
    if (column_peg_318.chunk3 == 0){
        column_peg_318.chunk3 = new Chunk3();
    }
    column_peg_318.chunk3->chunk_name = result_peg_319;
    stream.update(result_peg_319.getPosition());
    
    
    return result_peg_319;
    out_peg_326:
    
    if (column_peg_318.chunk3 == 0){
        column_peg_318.chunk3 = new Chunk3();
    }
    column_peg_318.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_331 = stream.getColumn(position);
    if (column_peg_331.chunk3 != 0 && column_peg_331.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_331.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_346(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_332(myposition);
    
    {
        
        char letter_peg_337 = stream.get(result_peg_332.getPosition());
        if (letter_peg_337 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_337) != NULL){
            result_peg_332.nextPosition();
            result_peg_332.setValue((void*) (long) letter_peg_337);
        } else {
            goto out_peg_336;
        }
        
    }
    goto success_peg_333;
    out_peg_336:
    goto out_peg_338;
    success_peg_333:
    ;
    
    if (column_peg_331.chunk3 == 0){
        column_peg_331.chunk3 = new Chunk3();
    }
    column_peg_331.chunk3->chunk_alpha_digit = result_peg_332;
    stream.update(result_peg_332.getPosition());
    
    
    return result_peg_332;
    out_peg_338:
    Result result_peg_339(myposition);
    
    {
        
        char letter_peg_344 = stream.get(result_peg_339.getPosition());
        if (letter_peg_344 != '\0' && strchr("0123456789", letter_peg_344) != NULL){
            result_peg_339.nextPosition();
            result_peg_339.setValue((void*) (long) letter_peg_344);
        } else {
            goto out_peg_343;
        }
        
    }
    goto success_peg_340;
    out_peg_343:
    goto out_peg_345;
    success_peg_340:
    ;
    
    if (column_peg_331.chunk3 == 0){
        column_peg_331.chunk3 = new Chunk3();
    }
    column_peg_331.chunk3->chunk_alpha_digit = result_peg_339;
    stream.update(result_peg_339.getPosition());
    
    
    return result_peg_339;
    out_peg_345:
    
    if (column_peg_331.chunk3 == 0){
        column_peg_331.chunk3 = new Chunk3();
    }
    column_peg_331.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_347 = stream.getColumn(position);
    if (column_peg_347.chunk3 != 0 && column_peg_347.chunk3->chunk_valuelist.calculated()){
        return column_peg_347.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_369(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_348(myposition);
    
    {
    
        result_peg_348 = rule_expr_c(stream, result_peg_348.getPosition());
            if (result_peg_348.error()){
                goto out_peg_350;
            }
            first = result_peg_348.getValues();
        
        
        
        result_peg_348.reset();
            do{
                Result result_peg_353(result_peg_348.getPosition());
                {
                
                    result_peg_353 = rule_s(stream, result_peg_353.getPosition());
                        if (result_peg_353.error()){
                            goto loop_peg_352;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_353.getPosition()))){
                                result_peg_353.nextPosition();
                            } else {
                                goto loop_peg_352;
                            }
                        }
                        result_peg_353.setValue((void*) ",");
                    
                    
                    
                    result_peg_353 = rule_s(stream, result_peg_353.getPosition());
                        if (result_peg_353.error()){
                            goto loop_peg_352;
                        }
                    
                    
                    
                    result_peg_353 = rule_expr_c(stream, result_peg_353.getPosition());
                        if (result_peg_353.error()){
                            goto loop_peg_352;
                        }
                    
                    
                }
                result_peg_348.addResult(result_peg_353);
            } while (true);
            loop_peg_352:
            if (result_peg_348.matches() == 0){
                goto out_peg_350;
            }
            rest = result_peg_348.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_348.setValue(value);
            }
        
        
    }
    
    if (column_peg_347.chunk3 == 0){
        column_peg_347.chunk3 = new Chunk3();
    }
    column_peg_347.chunk3->chunk_valuelist = result_peg_348;
    stream.update(result_peg_348.getPosition());
    
    
    return result_peg_348;
    out_peg_350:
    Result result_peg_357(myposition);
    
    {
    
        result_peg_357 = rule_s(stream, result_peg_357.getPosition());
            if (result_peg_357.error()){
                goto out_peg_359;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_357.getPosition()))){
                    result_peg_357.nextPosition();
                } else {
                    goto out_peg_359;
                }
            }
            result_peg_357.setValue((void*) ",");
        
        
        
        result_peg_357 = rule_s(stream, result_peg_357.getPosition());
            if (result_peg_357.error()){
                goto out_peg_359;
            }
        
        
        
        result_peg_357 = rule_expr_c(stream, result_peg_357.getPosition());
            if (result_peg_357.error()){
                goto out_peg_359;
            }
            first = result_peg_357.getValues();
        
        
        
        result_peg_357.reset();
            do{
                Result result_peg_365(result_peg_357.getPosition());
                {
                
                    result_peg_365 = rule_s(stream, result_peg_365.getPosition());
                        if (result_peg_365.error()){
                            goto loop_peg_364;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_365.getPosition()))){
                                result_peg_365.nextPosition();
                            } else {
                                goto loop_peg_364;
                            }
                        }
                        result_peg_365.setValue((void*) ",");
                    
                    
                    
                    result_peg_365 = rule_s(stream, result_peg_365.getPosition());
                        if (result_peg_365.error()){
                            goto loop_peg_364;
                        }
                    
                    
                    
                    result_peg_365 = rule_expr_c(stream, result_peg_365.getPosition());
                        if (result_peg_365.error()){
                            goto loop_peg_364;
                        }
                    
                    
                }
                result_peg_357.addResult(result_peg_365);
            } while (true);
            loop_peg_364:
            ;
            rest = result_peg_357.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_357.setValue(value);
            }
        
        
    }
    
    if (column_peg_347.chunk3 == 0){
        column_peg_347.chunk3 = new Chunk3();
    }
    column_peg_347.chunk3->chunk_valuelist = result_peg_357;
    stream.update(result_peg_357.getPosition());
    
    
    return result_peg_357;
    out_peg_359:
    
    if (column_peg_347.chunk3 == 0){
        column_peg_347.chunk3 = new Chunk3();
    }
    column_peg_347.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_370 = stream.getColumn(position);
    if (column_peg_370.chunk3 != 0 && column_peg_370.chunk3->chunk_expr.calculated()){
        return column_peg_370.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_381(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_371(myposition);
    
    {
    
        result_peg_371 = rule_expr_c(stream, result_peg_371.getPosition());
            if (result_peg_371.error()){
                goto out_peg_373;
            }
        
        Result result_peg_372 = result_peg_371;
        
        Result result_peg_376(result_peg_371);
            {
            
                result_peg_376 = rule_s(stream, result_peg_376.getPosition());
                    if (result_peg_376.error()){
                        goto not_peg_375;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_376.getPosition()))){
                            result_peg_376.nextPosition();
                        } else {
                            goto not_peg_375;
                        }
                    }
                    result_peg_376.setValue((void*) ",");
                
                
            }
            goto out_peg_373;
            not_peg_375:
            result_peg_371.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_372.getValues();
                result_peg_371.setValue(value);
            }
        
        
    }
    
    if (column_peg_370.chunk3 == 0){
        column_peg_370.chunk3 = new Chunk3();
    }
    column_peg_370.chunk3->chunk_expr = result_peg_371;
    stream.update(result_peg_371.getPosition());
    
    
    return result_peg_371;
    out_peg_373:
    Result result_peg_378(myposition);
    
    {
    
        result_peg_378 = rule_valuelist(stream, result_peg_378.getPosition());
            if (result_peg_378.error()){
                goto out_peg_380;
            }
        
        Result result_peg_379 = result_peg_378;
        
        result_peg_378 = rule_expr2_rest(stream, result_peg_378.getPosition(), result_peg_379.getValues());
            if (result_peg_378.error()){
                goto out_peg_380;
            }
        
        
    }
    
    if (column_peg_370.chunk3 == 0){
        column_peg_370.chunk3 = new Chunk3();
    }
    column_peg_370.chunk3->chunk_expr = result_peg_378;
    stream.update(result_peg_378.getPosition());
    
    
    return result_peg_378;
    out_peg_380:
    
    if (column_peg_370.chunk3 == 0){
        column_peg_370.chunk3 = new Chunk3();
    }
    column_peg_370.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_382 = stream.getColumn(position);
    if (column_peg_382.chunk4 != 0 && column_peg_382.chunk4->chunk_expr_c.calculated()){
        return column_peg_382.chunk4->chunk_expr_c;
    }
    
    RuleTrace trace_peg_386(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_383(myposition);
    
    {
    
        result_peg_383 = rule_expr2(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_385;
            }
            left = result_peg_383.getValues();
        
        
        
        result_peg_383 = rule_expr_rest(stream, result_peg_383.getPosition(), left);
            if (result_peg_383.error()){
                goto out_peg_385;
            }
        
        
    }
    
    if (column_peg_382.chunk4 == 0){
        column_peg_382.chunk4 = new Chunk4();
    }
    column_peg_382.chunk4->chunk_expr_c = result_peg_383;
    stream.update(result_peg_383.getPosition());
    
    
    return result_peg_383;
    out_peg_385:
    
    if (column_peg_382.chunk4 == 0){
        column_peg_382.chunk4 = new Chunk4();
    }
    column_peg_382.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_387 = stream.getColumn(position);
    if (column_peg_387.chunk4 != 0 && column_peg_387.chunk4->chunk_expr2.calculated()){
        return column_peg_387.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_391(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_388(myposition);
    
    {
    
        result_peg_388 = rule_expr3(stream, result_peg_388.getPosition());
            if (result_peg_388.error()){
                goto out_peg_390;
            }
            left = result_peg_388.getValues();
        
        
        
        result_peg_388 = rule_expr2_rest(stream, result_peg_388.getPosition(), left);
            if (result_peg_388.error()){
                goto out_peg_390;
            }
        
        
    }
    
    if (column_peg_387.chunk4 == 0){
        column_peg_387.chunk4 = new Chunk4();
    }
    column_peg_387.chunk4->chunk_expr2 = result_peg_388;
    stream.update(result_peg_388.getPosition());
    
    
    return result_peg_388;
    out_peg_390:
    
    if (column_peg_387.chunk4 == 0){
        column_peg_387.chunk4 = new Chunk4();
    }
    column_peg_387.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_392 = stream.getColumn(position);
    if (column_peg_392.chunk4 != 0 && column_peg_392.chunk4->chunk_expr3.calculated()){
        return column_peg_392.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_396(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_393(myposition);
    
    {
    
        result_peg_393 = rule_expr4(stream, result_peg_393.getPosition());
            if (result_peg_393.error()){
                goto out_peg_395;
            }
            left = result_peg_393.getValues();
        
        
        
        result_peg_393 = rule_expr3_rest(stream, result_peg_393.getPosition(), left);
            if (result_peg_393.error()){
                goto out_peg_395;
            }
        
        
    }
    
    if (column_peg_392.chunk4 == 0){
        column_peg_392.chunk4 = new Chunk4();
    }
    column_peg_392.chunk4->chunk_expr3 = result_peg_393;
    stream.update(result_peg_393.getPosition());
    
    
    return result_peg_393;
    out_peg_395:
    
    if (column_peg_392.chunk4 == 0){
        column_peg_392.chunk4 = new Chunk4();
    }
    column_peg_392.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_397 = stream.getColumn(position);
    if (column_peg_397.chunk4 != 0 && column_peg_397.chunk4->chunk_expr4.calculated()){
        return column_peg_397.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_401(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_398(myposition);
    
    {
    
        result_peg_398 = rule_expr5(stream, result_peg_398.getPosition());
            if (result_peg_398.error()){
                goto out_peg_400;
            }
            left = result_peg_398.getValues();
        
        
        
        result_peg_398 = rule_expr4_rest(stream, result_peg_398.getPosition(), left);
            if (result_peg_398.error()){
                goto out_peg_400;
            }
        
        
    }
    
    if (column_peg_397.chunk4 == 0){
        column_peg_397.chunk4 = new Chunk4();
    }
    column_peg_397.chunk4->chunk_expr4 = result_peg_398;
    stream.update(result_peg_398.getPosition());
    
    
    return result_peg_398;
    out_peg_400:
    
    if (column_peg_397.chunk4 == 0){
        column_peg_397.chunk4 = new Chunk4();
    }
    column_peg_397.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_402 = stream.getColumn(position);
    if (column_peg_402.chunk4 != 0 && column_peg_402.chunk4->chunk_expr5.calculated()){
        return column_peg_402.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_406(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_403(myposition);
    
    {
    
        result_peg_403 = rule_expr6(stream, result_peg_403.getPosition());
            if (result_peg_403.error()){
                goto out_peg_405;
            }
            left = result_peg_403.getValues();
        
        
        
        result_peg_403 = rule_expr5_rest(stream, result_peg_403.getPosition(), left);
            if (result_peg_403.error()){
                goto out_peg_405;
            }
        
        
    }
    
    if (column_peg_402.chunk4 == 0){
        column_peg_402.chunk4 = new Chunk4();
    }
    column_peg_402.chunk4->chunk_expr5 = result_peg_403;
    stream.update(result_peg_403.getPosition());
    
    
    return result_peg_403;
    out_peg_405:
    
    if (column_peg_402.chunk4 == 0){
        column_peg_402.chunk4 = new Chunk4();
    }
    column_peg_402.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_407 = stream.getColumn(position);
    if (column_peg_407.chunk5 != 0 && column_peg_407.chunk5->chunk_expr6.calculated()){
        return column_peg_407.chunk5->chunk_expr6;
    }
    
    RuleTrace trace_peg_411(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_408(myposition);
    
    {
    
        result_peg_408 = rule_expr7(stream, result_peg_408.getPosition());
            if (result_peg_408.error()){
                goto out_peg_410;
            }
            left = result_peg_408.getValues();
        
        
        
        result_peg_408 = rule_expr6_rest(stream, result_peg_408.getPosition(), left);
            if (result_peg_408.error()){
                goto out_peg_410;
            }
        
        
    }
    
    if (column_peg_407.chunk5 == 0){
        column_peg_407.chunk5 = new Chunk5();
    }
    column_peg_407.chunk5->chunk_expr6 = result_peg_408;
    stream.update(result_peg_408.getPosition());
    
    
    return result_peg_408;
    out_peg_410:
    
    if (column_peg_407.chunk5 == 0){
        column_peg_407.chunk5 = new Chunk5();
    }
    column_peg_407.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_412 = stream.getColumn(position);
    if (column_peg_412.chunk5 != 0 && column_peg_412.chunk5->chunk_expr7.calculated()){
        return column_peg_412.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_416(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_413(myposition);
    
    {
    
        result_peg_413 = rule_expr8(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_415;
            }
            left = result_peg_413.getValues();
        
        
        
        result_peg_413 = rule_expr7_rest(stream, result_peg_413.getPosition(), left);
            if (result_peg_413.error()){
                goto out_peg_415;
            }
        
        
    }
    
    if (column_peg_412.chunk5 == 0){
        column_peg_412.chunk5 = new Chunk5();
    }
    column_peg_412.chunk5->chunk_expr7 = result_peg_413;
    stream.update(result_peg_413.getPosition());
    
    
    return result_peg_413;
    out_peg_415:
    
    if (column_peg_412.chunk5 == 0){
        column_peg_412.chunk5 = new Chunk5();
    }
    column_peg_412.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_417 = stream.getColumn(position);
    if (column_peg_417.chunk5 != 0 && column_peg_417.chunk5->chunk_expr8.calculated()){
        return column_peg_417.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_421(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_418(myposition);
    
    {
    
        result_peg_418 = rule_expr9(stream, result_peg_418.getPosition());
            if (result_peg_418.error()){
                goto out_peg_420;
            }
            left = result_peg_418.getValues();
        
        
        
        result_peg_418 = rule_expr8_rest(stream, result_peg_418.getPosition(), left);
            if (result_peg_418.error()){
                goto out_peg_420;
            }
        
        
    }
    
    if (column_peg_417.chunk5 == 0){
        column_peg_417.chunk5 = new Chunk5();
    }
    column_peg_417.chunk5->chunk_expr8 = result_peg_418;
    stream.update(result_peg_418.getPosition());
    
    
    return result_peg_418;
    out_peg_420:
    
    if (column_peg_417.chunk5 == 0){
        column_peg_417.chunk5 = new Chunk5();
    }
    column_peg_417.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_422 = stream.getColumn(position);
    if (column_peg_422.chunk5 != 0 && column_peg_422.chunk5->chunk_expr9.calculated()){
        return column_peg_422.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_426(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_423(myposition);
    
    {
    
        result_peg_423 = rule_expr10(stream, result_peg_423.getPosition());
            if (result_peg_423.error()){
                goto out_peg_425;
            }
            left = result_peg_423.getValues();
        
        
        
        result_peg_423 = rule_expr9_rest(stream, result_peg_423.getPosition(), left);
            if (result_peg_423.error()){
                goto out_peg_425;
            }
        
        
    }
    
    if (column_peg_422.chunk5 == 0){
        column_peg_422.chunk5 = new Chunk5();
    }
    column_peg_422.chunk5->chunk_expr9 = result_peg_423;
    stream.update(result_peg_423.getPosition());
    
    
    return result_peg_423;
    out_peg_425:
    
    if (column_peg_422.chunk5 == 0){
        column_peg_422.chunk5 = new Chunk5();
    }
    column_peg_422.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_427 = stream.getColumn(position);
    if (column_peg_427.chunk5 != 0 && column_peg_427.chunk5->chunk_expr10.calculated()){
        return column_peg_427.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_431(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_428(myposition);
    
    {
    
        result_peg_428 = rule_expr11(stream, result_peg_428.getPosition());
            if (result_peg_428.error()){
                goto out_peg_430;
            }
            left = result_peg_428.getValues();
        
        
        
        result_peg_428 = rule_expr10_rest(stream, result_peg_428.getPosition(), left);
            if (result_peg_428.error()){
                goto out_peg_430;
            }
        
        
    }
    
    if (column_peg_427.chunk5 == 0){
        column_peg_427.chunk5 = new Chunk5();
    }
    column_peg_427.chunk5->chunk_expr10 = result_peg_428;
    stream.update(result_peg_428.getPosition());
    
    
    return result_peg_428;
    out_peg_430:
    
    if (column_peg_427.chunk5 == 0){
        column_peg_427.chunk5 = new Chunk5();
    }
    column_peg_427.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_432 = stream.getColumn(position);
    if (column_peg_432.chunk6 != 0 && column_peg_432.chunk6->chunk_expr11.calculated()){
        return column_peg_432.chunk6->chunk_expr11;
    }
    
    RuleTrace trace_peg_436(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_433(myposition);
    
    {
    
        result_peg_433 = rule_expr12(stream, result_peg_433.getPosition());
            if (result_peg_433.error()){
                goto out_peg_435;
            }
            left = result_peg_433.getValues();
        
        
        
        result_peg_433 = rule_expr11_rest(stream, result_peg_433.getPosition(), left);
            if (result_peg_433.error()){
                goto out_peg_435;
            }
        
        
    }
    
    if (column_peg_432.chunk6 == 0){
        column_peg_432.chunk6 = new Chunk6();
    }
    column_peg_432.chunk6->chunk_expr11 = result_peg_433;
    stream.update(result_peg_433.getPosition());
    
    
    return result_peg_433;
    out_peg_435:
    
    if (column_peg_432.chunk6 == 0){
        column_peg_432.chunk6 = new Chunk6();
    }
    column_peg_432.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_437 = stream.getColumn(position);
    if (column_peg_437.chunk6 != 0 && column_peg_437.chunk6->chunk_expr12.calculated()){
        return column_peg_437.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_441(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_438(myposition);
    
    {
    
        result_peg_438 = rule_expr13(stream, result_peg_438.getPosition());
            if (result_peg_438.error()){
                goto out_peg_440;
            }
            left = result_peg_438.getValues();
        
        
        
        result_peg_438 = rule_expr12_rest(stream, result_peg_438.getPosition(), left);
            if (result_peg_438.error()){
                goto out_peg_440;
            }
        
        
    }
    
    if (column_peg_437.chunk6 == 0){
        column_peg_437.chunk6 = new Chunk6();
    }
    column_peg_437.chunk6->chunk_expr12 = result_peg_438;
    stream.update(result_peg_438.getPosition());
    
    
    return result_peg_438;
    out_peg_440:
    
    if (column_peg_437.chunk6 == 0){
        column_peg_437.chunk6 = new Chunk6();
    }
    column_peg_437.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_442 = stream.getColumn(position);
    if (column_peg_442.chunk6 != 0 && column_peg_442.chunk6->chunk_expr13.calculated()){
        return column_peg_442.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_449(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_443(myposition);
    
    {
    
        result_peg_443.reset();
            do{
                Result result_peg_446(result_peg_443.getPosition());
                result_peg_446 = rule_unary(stream, result_peg_446.getPosition());
                if (result_peg_446.error()){
                    goto loop_peg_445;
                }
                result_peg_443.addResult(result_peg_446);
            } while (true);
            loop_peg_445:
            ;
        
        Result result_peg_444 = result_peg_443;
        
        result_peg_443 = rule_expr13_real(stream, result_peg_443.getPosition());
            if (result_peg_443.error()){
                goto out_peg_448;
            }
            exp = result_peg_443.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_444.getValues(), exp);
                result_peg_443.setValue(value);
            }
        
        
    }
    
    if (column_peg_442.chunk6 == 0){
        column_peg_442.chunk6 = new Chunk6();
    }
    column_peg_442.chunk6->chunk_expr13 = result_peg_443;
    stream.update(result_peg_443.getPosition());
    
    
    return result_peg_443;
    out_peg_448:
    
    if (column_peg_442.chunk6 == 0){
        column_peg_442.chunk6 = new Chunk6();
    }
    column_peg_442.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_461(stream, "expr_rest");
    int myposition = position;
    tail_peg_452:
    Value right;
    Value new_left;
    Result result_peg_451(myposition);
    {
    
        result_peg_451 = rule_s(stream, result_peg_451.getPosition());
            if (result_peg_451.error()){
                goto out_peg_454;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_451.getPosition()))){
                    result_peg_451.nextPosition();
                } else {
                    goto out_peg_454;
                }
            }
            result_peg_451.setValue((void*) "||");
        
        
        
        result_peg_451 = rule_s(stream, result_peg_451.getPosition());
            if (result_peg_451.error()){
                goto out_peg_454;
            }
        
        
        
        result_peg_451 = rule_expr2(stream, result_peg_451.getPosition());
            if (result_peg_451.error()){
                goto out_peg_454;
            }
            right = result_peg_451.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_451.setValue(value);
            }
            new_left = result_peg_451.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_451.getPosition();
    goto tail_peg_452;
    out_peg_454:
    Result result_peg_459(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_459.setValue(value);
            }
        
        
    }
    stream.update(result_peg_459.getPosition());
    
    return result_peg_459;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_473(stream, "expr2_rest");
    int myposition = position;
    tail_peg_464:
    Value right;
    Value new_left;
    Result result_peg_463(myposition);
    {
    
        result_peg_463 = rule_s(stream, result_peg_463.getPosition());
            if (result_peg_463.error()){
                goto out_peg_466;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_463.getPosition()))){
                    result_peg_463.nextPosition();
                } else {
                    goto out_peg_466;
                }
            }
            result_peg_463.setValue((void*) "^^");
        
        
        
        result_peg_463 = rule_s(stream, result_peg_463.getPosition());
            if (result_peg_463.error()){
                goto out_peg_466;
            }
        
        
        
        result_peg_463 = rule_expr3(stream, result_peg_463.getPosition());
            if (result_peg_463.error()){
                goto out_peg_466;
            }
            right = result_peg_463.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_463.setValue(value);
            }
            new_left = result_peg_463.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_463.getPosition();
    goto tail_peg_464;
    out_peg_466:
    Result result_peg_471(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_471.setValue(value);
            }
        
        
    }
    stream.update(result_peg_471.getPosition());
    
    return result_peg_471;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_485(stream, "expr3_rest");
    int myposition = position;
    tail_peg_476:
    Value right;
    Value new_left;
    Result result_peg_475(myposition);
    {
    
        result_peg_475 = rule_s(stream, result_peg_475.getPosition());
            if (result_peg_475.error()){
                goto out_peg_478;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_475.getPosition()))){
                    result_peg_475.nextPosition();
                } else {
                    goto out_peg_478;
                }
            }
            result_peg_475.setValue((void*) "&&");
        
        
        
        result_peg_475 = rule_s(stream, result_peg_475.getPosition());
            if (result_peg_475.error()){
                goto out_peg_478;
            }
        
        
        
        result_peg_475 = rule_expr4(stream, result_peg_475.getPosition());
            if (result_peg_475.error()){
                goto out_peg_478;
            }
            right = result_peg_475.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_475.setValue(value);
            }
            new_left = result_peg_475.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_475.getPosition();
    goto tail_peg_476;
    out_peg_478:
    Result result_peg_483(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_483.setValue(value);
            }
        
        
    }
    stream.update(result_peg_483.getPosition());
    
    return result_peg_483;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_497(stream, "expr4_rest");
    int myposition = position;
    tail_peg_488:
    Value right;
    Value new_left;
    Result result_peg_487(myposition);
    {
    
        result_peg_487 = rule_s(stream, result_peg_487.getPosition());
            if (result_peg_487.error()){
                goto out_peg_490;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_487.getPosition()))){
                    result_peg_487.nextPosition();
                } else {
                    goto out_peg_490;
                }
            }
            result_peg_487.setValue((void*) "|");
        
        
        
        result_peg_487 = rule_s(stream, result_peg_487.getPosition());
            if (result_peg_487.error()){
                goto out_peg_490;
            }
        
        
        
        result_peg_487 = rule_expr5(stream, result_peg_487.getPosition());
            if (result_peg_487.error()){
                goto out_peg_490;
            }
            right = result_peg_487.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_487.setValue(value);
            }
            new_left = result_peg_487.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_487.getPosition();
    goto tail_peg_488;
    out_peg_490:
    Result result_peg_495(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_495.setValue(value);
            }
        
        
    }
    stream.update(result_peg_495.getPosition());
    
    return result_peg_495;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_509(stream, "expr5_rest");
    int myposition = position;
    tail_peg_500:
    Value right;
    Value new_left;
    Result result_peg_499(myposition);
    {
    
        result_peg_499 = rule_s(stream, result_peg_499.getPosition());
            if (result_peg_499.error()){
                goto out_peg_502;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_499.getPosition()))){
                    result_peg_499.nextPosition();
                } else {
                    goto out_peg_502;
                }
            }
            result_peg_499.setValue((void*) "^");
        
        
        
        result_peg_499 = rule_s(stream, result_peg_499.getPosition());
            if (result_peg_499.error()){
                goto out_peg_502;
            }
        
        
        
        result_peg_499 = rule_expr6(stream, result_peg_499.getPosition());
            if (result_peg_499.error()){
                goto out_peg_502;
            }
            right = result_peg_499.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_499.setValue(value);
            }
            new_left = result_peg_499.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_499.getPosition();
    goto tail_peg_500;
    out_peg_502:
    Result result_peg_507(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_507.setValue(value);
            }
        
        
    }
    stream.update(result_peg_507.getPosition());
    
    return result_peg_507;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_521(stream, "expr6_rest");
    int myposition = position;
    tail_peg_512:
    Value right;
    Value new_left;
    Result result_peg_511(myposition);
    {
    
        result_peg_511 = rule_s(stream, result_peg_511.getPosition());
            if (result_peg_511.error()){
                goto out_peg_514;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_511.getPosition()))){
                    result_peg_511.nextPosition();
                } else {
                    goto out_peg_514;
                }
            }
            result_peg_511.setValue((void*) "&");
        
        
        
        result_peg_511 = rule_s(stream, result_peg_511.getPosition());
            if (result_peg_511.error()){
                goto out_peg_514;
            }
        
        
        
        result_peg_511 = rule_expr7(stream, result_peg_511.getPosition());
            if (result_peg_511.error()){
                goto out_peg_514;
            }
            right = result_peg_511.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_511.setValue(value);
            }
            new_left = result_peg_511.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_511.getPosition();
    goto tail_peg_512;
    out_peg_514:
    Result result_peg_519(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_519.setValue(value);
            }
        
        
    }
    stream.update(result_peg_519.getPosition());
    
    return result_peg_519;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_533(stream, "expr7_rest");
    int myposition = position;
    tail_peg_524:
    Value right;
    Value new_left;
    Result result_peg_523(myposition);
    {
    
        result_peg_523 = rule_s(stream, result_peg_523.getPosition());
            if (result_peg_523.error()){
                goto out_peg_526;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_523.getPosition()))){
                    result_peg_523.nextPosition();
                } else {
                    goto out_peg_526;
                }
            }
            result_peg_523.setValue((void*) ":=");
        
        
        
        result_peg_523 = rule_s(stream, result_peg_523.getPosition());
            if (result_peg_523.error()){
                goto out_peg_526;
            }
        
        
        
        result_peg_523 = rule_expr8(stream, result_peg_523.getPosition());
            if (result_peg_523.error()){
                goto out_peg_526;
            }
            right = result_peg_523.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_523.setValue(value);
            }
            new_left = result_peg_523.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_523.getPosition();
    goto tail_peg_524;
    out_peg_526:
    Result result_peg_531(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_531.setValue(value);
            }
        
        
    }
    stream.update(result_peg_531.getPosition());
    
    return result_peg_531;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_552(stream, "expr8_rest");
    int myposition = position;
    tail_peg_536:
    Value right;
    Value new_left;
    Result result_peg_535(myposition);
    {
    
        result_peg_535 = rule_s(stream, result_peg_535.getPosition());
            if (result_peg_535.error()){
                goto out_peg_538;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_535.getPosition()))){
                    result_peg_535.nextPosition();
                } else {
                    goto out_peg_538;
                }
            }
            result_peg_535.setValue((void*) "=");
        
        
        
        result_peg_535 = rule_s(stream, result_peg_535.getPosition());
            if (result_peg_535.error()){
                goto out_peg_538;
            }
        
        
        
        result_peg_535 = rule_expr9(stream, result_peg_535.getPosition());
            if (result_peg_535.error()){
                goto out_peg_538;
            }
            right = result_peg_535.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_535.setValue(value);
            }
            new_left = result_peg_535.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_535.getPosition();
    goto tail_peg_536;
    out_peg_538:
    Result result_peg_543(myposition);
    {
    
        result_peg_543 = rule_s(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_545;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_543.getPosition()))){
                    result_peg_543.nextPosition();
                } else {
                    goto out_peg_545;
                }
            }
            result_peg_543.setValue((void*) "!=");
        
        
        
        result_peg_543 = rule_s(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_545;
            }
        
        
        
        result_peg_543 = rule_expr9(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_545;
            }
            right = result_peg_543.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_543.setValue(value);
            }
            new_left = result_peg_543.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_543.getPosition();
    goto tail_peg_536;
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
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_564(stream, "expr9_rest");
    int myposition = position;
    tail_peg_555:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_554(myposition);
    {
    
        result_peg_554 = rule_s(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
        
        
        
        result_peg_554 = rule_compare(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
            maker = result_peg_554.getValues();
        
        
        
        result_peg_554 = rule_s(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
        
        
        
        result_peg_554 = rule_expr10(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_557;
            }
            right = result_peg_554.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
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
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_562.setValue(value);
            }
        
        
    }
    stream.update(result_peg_562.getPosition());
    
    return result_peg_562;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_565 = stream.getColumn(position);
    if (column_peg_565.chunk8 != 0 && column_peg_565.chunk8->chunk_compare.calculated()){
        return column_peg_565.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_578(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_566(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_566.getPosition()))){
                    result_peg_566.nextPosition();
                } else {
                    goto out_peg_568;
                }
            }
            result_peg_566.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_566.setValue(value);
            }
        
        
    }
    
    if (column_peg_565.chunk8 == 0){
        column_peg_565.chunk8 = new Chunk8();
    }
    column_peg_565.chunk8->chunk_compare = result_peg_566;
    stream.update(result_peg_566.getPosition());
    
    
    return result_peg_566;
    out_peg_568:
    Result result_peg_569(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_569.getPosition()))){
                    result_peg_569.nextPosition();
                } else {
                    goto out_peg_571;
                }
            }
            result_peg_569.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_569.setValue(value);
            }
        
        
    }
    
    if (column_peg_565.chunk8 == 0){
        column_peg_565.chunk8 = new Chunk8();
    }
    column_peg_565.chunk8->chunk_compare = result_peg_569;
    stream.update(result_peg_569.getPosition());
    
    
    return result_peg_569;
    out_peg_571:
    Result result_peg_572(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_572.getPosition()))){
                    result_peg_572.nextPosition();
                } else {
                    goto out_peg_574;
                }
            }
            result_peg_572.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_572.setValue(value);
            }
        
        
    }
    
    if (column_peg_565.chunk8 == 0){
        column_peg_565.chunk8 = new Chunk8();
    }
    column_peg_565.chunk8->chunk_compare = result_peg_572;
    stream.update(result_peg_572.getPosition());
    
    
    return result_peg_572;
    out_peg_574:
    Result result_peg_575(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_575.getPosition()))){
                    result_peg_575.nextPosition();
                } else {
                    goto out_peg_577;
                }
            }
            result_peg_575.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_575.setValue(value);
            }
        
        
    }
    
    if (column_peg_565.chunk8 == 0){
        column_peg_565.chunk8 = new Chunk8();
    }
    column_peg_565.chunk8->chunk_compare = result_peg_575;
    stream.update(result_peg_575.getPosition());
    
    
    return result_peg_575;
    out_peg_577:
    
    if (column_peg_565.chunk8 == 0){
        column_peg_565.chunk8 = new Chunk8();
    }
    column_peg_565.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_579 = stream.getColumn(position);
    if (column_peg_579.chunk8 != 0 && column_peg_579.chunk8->chunk_all_compare.calculated()){
        return column_peg_579.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_586(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_580(myposition);
    
    result_peg_580 = rule_compare(stream, result_peg_580.getPosition());
    if (result_peg_580.error()){
        goto out_peg_581;
    }
    
    if (column_peg_579.chunk8 == 0){
        column_peg_579.chunk8 = new Chunk8();
    }
    column_peg_579.chunk8->chunk_all_compare = result_peg_580;
    stream.update(result_peg_580.getPosition());
    
    
    return result_peg_580;
    out_peg_581:
    Result result_peg_582(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_582.getPosition()))){
            result_peg_582.nextPosition();
        } else {
            goto out_peg_583;
        }
    }
    result_peg_582.setValue((void*) "=");
    
    if (column_peg_579.chunk8 == 0){
        column_peg_579.chunk8 = new Chunk8();
    }
    column_peg_579.chunk8->chunk_all_compare = result_peg_582;
    stream.update(result_peg_582.getPosition());
    
    
    return result_peg_582;
    out_peg_583:
    Result result_peg_584(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_584.getPosition()))){
            result_peg_584.nextPosition();
        } else {
            goto out_peg_585;
        }
    }
    result_peg_584.setValue((void*) "!=");
    
    if (column_peg_579.chunk8 == 0){
        column_peg_579.chunk8 = new Chunk8();
    }
    column_peg_579.chunk8->chunk_all_compare = result_peg_584;
    stream.update(result_peg_584.getPosition());
    
    
    return result_peg_584;
    out_peg_585:
    
    if (column_peg_579.chunk8 == 0){
        column_peg_579.chunk8 = new Chunk8();
    }
    column_peg_579.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_605(stream, "expr10_rest");
    int myposition = position;
    tail_peg_589:
    Value right;
    Value new_left;
    Result result_peg_588(myposition);
    {
    
        result_peg_588 = rule_s(stream, result_peg_588.getPosition());
            if (result_peg_588.error()){
                goto out_peg_591;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_588.getPosition()))){
                    result_peg_588.nextPosition();
                } else {
                    goto out_peg_591;
                }
            }
            result_peg_588.setValue((void*) "+");
        
        
        
        result_peg_588 = rule_s(stream, result_peg_588.getPosition());
            if (result_peg_588.error()){
                goto out_peg_591;
            }
        
        
        
        result_peg_588 = rule_expr11(stream, result_peg_588.getPosition());
            if (result_peg_588.error()){
                goto out_peg_591;
            }
            right = result_peg_588.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_588.setValue(value);
            }
            new_left = result_peg_588.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_588.getPosition();
    goto tail_peg_589;
    out_peg_591:
    Result result_peg_596(myposition);
    {
    
        result_peg_596 = rule_s(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto out_peg_598;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_596.getPosition()))){
                    result_peg_596.nextPosition();
                } else {
                    goto out_peg_598;
                }
            }
            result_peg_596.setValue((void*) "-");
        
        
        
        result_peg_596 = rule_s(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto out_peg_598;
            }
        
        
        
        result_peg_596 = rule_expr11(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto out_peg_598;
            }
            right = result_peg_596.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_596.setValue(value);
            }
            new_left = result_peg_596.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_596.getPosition();
    goto tail_peg_589;
    out_peg_598:
    Result result_peg_603(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_603.setValue(value);
            }
        
        
    }
    stream.update(result_peg_603.getPosition());
    
    return result_peg_603;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_631(stream, "expr11_rest");
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
                if (compareChar("*"[i], stream.get(result_peg_607.getPosition()))){
                    result_peg_607.nextPosition();
                } else {
                    goto out_peg_610;
                }
            }
            result_peg_607.setValue((void*) "*");
        
        
        
        result_peg_607 = rule_s(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_610;
            }
        
        
        
        result_peg_607 = rule_expr12(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_610;
            }
            right = result_peg_607.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
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
                if (compareChar("/"[i], stream.get(result_peg_615.getPosition()))){
                    result_peg_615.nextPosition();
                } else {
                    goto out_peg_617;
                }
            }
            result_peg_615.setValue((void*) "/");
        
        
        
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
        
        
        
        result_peg_615 = rule_expr12(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
            right = result_peg_615.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
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
    
        result_peg_622 = rule_s(stream, result_peg_622.getPosition());
            if (result_peg_622.error()){
                goto out_peg_624;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_622.getPosition()))){
                    result_peg_622.nextPosition();
                } else {
                    goto out_peg_624;
                }
            }
            result_peg_622.setValue((void*) "%");
        
        
        
        result_peg_622 = rule_s(stream, result_peg_622.getPosition());
            if (result_peg_622.error()){
                goto out_peg_624;
            }
        
        
        
        result_peg_622 = rule_expr12(stream, result_peg_622.getPosition());
            if (result_peg_622.error()){
                goto out_peg_624;
            }
            right = result_peg_622.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_622.setValue(value);
            }
            new_left = result_peg_622.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_622.getPosition();
    goto tail_peg_608;
    out_peg_624:
    Result result_peg_629(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_629.setValue(value);
            }
        
        
    }
    stream.update(result_peg_629.getPosition());
    
    return result_peg_629;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_643(stream, "expr12_rest");
    int myposition = position;
    tail_peg_634:
    Value right;
    Value new_left;
    Result result_peg_633(myposition);
    {
    
        result_peg_633 = rule_s(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_636;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_633.getPosition()))){
                    result_peg_633.nextPosition();
                } else {
                    goto out_peg_636;
                }
            }
            result_peg_633.setValue((void*) "**");
        
        
        
        result_peg_633 = rule_s(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_636;
            }
        
        
        
        result_peg_633 = rule_expr13(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_636;
            }
            right = result_peg_633.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_633.setValue(value);
            }
            new_left = result_peg_633.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_633.getPosition();
    goto tail_peg_634;
    out_peg_636:
    Result result_peg_641(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_641.setValue(value);
            }
        
        
    }
    stream.update(result_peg_641.getPosition());
    
    return result_peg_641;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_644 = stream.getColumn(position);
    if (column_peg_644.chunk9 != 0 && column_peg_644.chunk9->chunk_unary.calculated()){
        return column_peg_644.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_673(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_645(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_645.getPosition()))){
                    result_peg_645.nextPosition();
                } else {
                    goto out_peg_647;
                }
            }
            result_peg_645.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_645.setValue(value);
            }
        
        
    }
    
    if (column_peg_644.chunk9 == 0){
        column_peg_644.chunk9 = new Chunk9();
    }
    column_peg_644.chunk9->chunk_unary = result_peg_645;
    stream.update(result_peg_645.getPosition());
    
    
    return result_peg_645;
    out_peg_647:
    Result result_peg_648(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_648.getPosition()))){
                    result_peg_648.nextPosition();
                } else {
                    goto out_peg_650;
                }
            }
            result_peg_648.setValue((void*) "-");
        
        
        
        Result result_peg_653(result_peg_648);
            {
                
                char letter_peg_658 = stream.get(result_peg_653.getPosition());
                if (letter_peg_658 != '\0' && strchr("0123456789", letter_peg_658) != NULL){
                    result_peg_653.nextPosition();
                    result_peg_653.setValue((void*) (long) letter_peg_658);
                } else {
                    goto out_peg_657;
                }
                
            }
            goto success_peg_654;
            out_peg_657:
            goto not_peg_652;
            success_peg_654:
            ;
            goto out_peg_650;
            not_peg_652:
            result_peg_648.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_648.setValue(value);
            }
        
        
    }
    
    if (column_peg_644.chunk9 == 0){
        column_peg_644.chunk9 = new Chunk9();
    }
    column_peg_644.chunk9->chunk_unary = result_peg_648;
    stream.update(result_peg_648.getPosition());
    
    
    return result_peg_648;
    out_peg_650:
    Result result_peg_659(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_659.getPosition()))){
                    result_peg_659.nextPosition();
                } else {
                    goto out_peg_661;
                }
            }
            result_peg_659.setValue((void*) "~");
        
        
        
        Result result_peg_664(result_peg_659);
            result_peg_664 = rule_key(stream, result_peg_664.getPosition());
            if (result_peg_664.error()){
                goto not_peg_663;
            }
            goto out_peg_661;
            not_peg_663:
            result_peg_659.setValue((void*)0);
        
        
        
        Result result_peg_667(result_peg_659);
            {
                
                char letter_peg_672 = stream.get(result_peg_667.getPosition());
                if (letter_peg_672 != '\0' && strchr("0123456789", letter_peg_672) != NULL){
                    result_peg_667.nextPosition();
                    result_peg_667.setValue((void*) (long) letter_peg_672);
                } else {
                    goto out_peg_671;
                }
                
            }
            goto success_peg_668;
            out_peg_671:
            goto not_peg_666;
            success_peg_668:
            ;
            goto out_peg_661;
            not_peg_666:
            result_peg_659.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_659.setValue(value);
            }
        
        
    }
    
    if (column_peg_644.chunk9 == 0){
        column_peg_644.chunk9 = new Chunk9();
    }
    column_peg_644.chunk9->chunk_unary = result_peg_659;
    stream.update(result_peg_659.getPosition());
    
    
    return result_peg_659;
    out_peg_661:
    
    if (column_peg_644.chunk9 == 0){
        column_peg_644.chunk9 = new Chunk9();
    }
    column_peg_644.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_674 = stream.getColumn(position);
    if (column_peg_674.chunk9 != 0 && column_peg_674.chunk9->chunk_expr13_real.calculated()){
        return column_peg_674.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_686(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_675(myposition);
    
    result_peg_675 = rule_function(stream, result_peg_675.getPosition());
    if (result_peg_675.error()){
        goto out_peg_676;
    }
    
    if (column_peg_674.chunk9 == 0){
        column_peg_674.chunk9 = new Chunk9();
    }
    column_peg_674.chunk9->chunk_expr13_real = result_peg_675;
    stream.update(result_peg_675.getPosition());
    
    
    return result_peg_675;
    out_peg_676:
    Result result_peg_677(myposition);
    
    result_peg_677 = rule_value(stream, result_peg_677.getPosition());
    if (result_peg_677.error()){
        goto out_peg_678;
    }
    
    if (column_peg_674.chunk9 == 0){
        column_peg_674.chunk9 = new Chunk9();
    }
    column_peg_674.chunk9->chunk_expr13_real = result_peg_677;
    stream.update(result_peg_677.getPosition());
    
    
    return result_peg_677;
    out_peg_678:
    Result result_peg_679(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_679.getPosition()))){
                    result_peg_679.nextPosition();
                } else {
                    goto out_peg_681;
                }
            }
            result_peg_679.setValue((void*) "(");
        
        
        
        result_peg_679 = rule_s(stream, result_peg_679.getPosition());
            if (result_peg_679.error()){
                goto out_peg_681;
            }
        
        
        
        result_peg_679 = rule_expr(stream, result_peg_679.getPosition());
            if (result_peg_679.error()){
                goto out_peg_681;
            }
            e = result_peg_679.getValues();
        
        
        
        result_peg_679 = rule_s(stream, result_peg_679.getPosition());
            if (result_peg_679.error()){
                goto out_peg_681;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_679.getPosition()))){
                    result_peg_679.nextPosition();
                } else {
                    goto out_peg_681;
                }
            }
            result_peg_679.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_679.setValue(value);
            }
        
        
    }
    
    if (column_peg_674.chunk9 == 0){
        column_peg_674.chunk9 = new Chunk9();
    }
    column_peg_674.chunk9->chunk_expr13_real = result_peg_679;
    stream.update(result_peg_679.getPosition());
    
    
    return result_peg_679;
    out_peg_681:
    
    if (column_peg_674.chunk9 == 0){
        column_peg_674.chunk9 = new Chunk9();
    }
    column_peg_674.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_687 = stream.getColumn(position);
    if (column_peg_687.chunk9 != 0 && column_peg_687.chunk9->chunk_function.calculated()){
        return column_peg_687.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_952(stream, "function");
    int myposition = position;
    
    Value name;
    Value arg1;
    Value arg2;
    Value arg3;
    Result result_peg_688(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_688.getPosition()))){
                    result_peg_688.nextPosition();
                } else {
                    goto out_peg_690;
                }
            }
            result_peg_688.setValue((void*) "abs");
            name = result_peg_688.getValues();
        
        
        
        result_peg_688 = rule_s(stream, result_peg_688.getPosition());
            if (result_peg_688.error()){
                goto out_peg_690;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_688.getPosition()))){
                    result_peg_688.nextPosition();
                } else {
                    goto out_peg_690;
                }
            }
            result_peg_688.setValue((void*) "(");
        
        
        
        result_peg_688 = rule_s(stream, result_peg_688.getPosition());
            if (result_peg_688.error()){
                goto out_peg_690;
            }
        
        
        
        result_peg_688 = rule_expr(stream, result_peg_688.getPosition());
            if (result_peg_688.error()){
                goto out_peg_690;
            }
            arg1 = result_peg_688.getValues();
        
        
        
        result_peg_688 = rule_s(stream, result_peg_688.getPosition());
            if (result_peg_688.error()){
                goto out_peg_690;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_688.getPosition()))){
                    result_peg_688.nextPosition();
                } else {
                    goto out_peg_690;
                }
            }
            result_peg_688.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_688.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_688;
    stream.update(result_peg_688.getPosition());
    
    
    return result_peg_688;
    out_peg_690:
    Result result_peg_697(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_697.getPosition()))){
                    result_peg_697.nextPosition();
                } else {
                    goto out_peg_699;
                }
            }
            result_peg_697.setValue((void*) "const");
            name = result_peg_697.getValues();
        
        
        
        result_peg_697 = rule_s(stream, result_peg_697.getPosition());
            if (result_peg_697.error()){
                goto out_peg_699;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_697.getPosition()))){
                    result_peg_697.nextPosition();
                } else {
                    goto out_peg_699;
                }
            }
            result_peg_697.setValue((void*) "(");
        
        
        
        result_peg_697 = rule_s(stream, result_peg_697.getPosition());
            if (result_peg_697.error()){
                goto out_peg_699;
            }
        
        
        
        result_peg_697 = rule_expr(stream, result_peg_697.getPosition());
            if (result_peg_697.error()){
                goto out_peg_699;
            }
            arg1 = result_peg_697.getValues();
        
        
        
        result_peg_697 = rule_s(stream, result_peg_697.getPosition());
            if (result_peg_697.error()){
                goto out_peg_699;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_697.getPosition()))){
                    result_peg_697.nextPosition();
                } else {
                    goto out_peg_699;
                }
            }
            result_peg_697.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_697.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_697;
    stream.update(result_peg_697.getPosition());
    
    
    return result_peg_697;
    out_peg_699:
    Result result_peg_706(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_706.getPosition()))){
                    result_peg_706.nextPosition();
                } else {
                    goto out_peg_708;
                }
            }
            result_peg_706.setValue((void*) "selfanimexist");
            name = result_peg_706.getValues();
        
        
        
        result_peg_706 = rule_s(stream, result_peg_706.getPosition());
            if (result_peg_706.error()){
                goto out_peg_708;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_706.getPosition()))){
                    result_peg_706.nextPosition();
                } else {
                    goto out_peg_708;
                }
            }
            result_peg_706.setValue((void*) "(");
        
        
        
        result_peg_706 = rule_s(stream, result_peg_706.getPosition());
            if (result_peg_706.error()){
                goto out_peg_708;
            }
        
        
        
        result_peg_706 = rule_expr(stream, result_peg_706.getPosition());
            if (result_peg_706.error()){
                goto out_peg_708;
            }
            arg1 = result_peg_706.getValues();
        
        
        
        result_peg_706 = rule_s(stream, result_peg_706.getPosition());
            if (result_peg_706.error()){
                goto out_peg_708;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_706.getPosition()))){
                    result_peg_706.nextPosition();
                } else {
                    goto out_peg_708;
                }
            }
            result_peg_706.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_706.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_706;
    stream.update(result_peg_706.getPosition());
    
    
    return result_peg_706;
    out_peg_708:
    Result result_peg_715(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_715.getPosition()))){
                    result_peg_715.nextPosition();
                } else {
                    goto out_peg_717;
                }
            }
            result_peg_715.setValue((void*) "ifelse");
            name = result_peg_715.getValues();
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_715.getPosition()))){
                    result_peg_715.nextPosition();
                } else {
                    goto out_peg_717;
                }
            }
            result_peg_715.setValue((void*) "(");
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        result_peg_715 = rule_expr_c(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
            arg1 = result_peg_715.getValues();
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_715.getPosition()))){
                    result_peg_715.nextPosition();
                } else {
                    goto out_peg_717;
                }
            }
            result_peg_715.setValue((void*) ",");
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        result_peg_715 = rule_expr_c(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
            arg2 = result_peg_715.getValues();
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_715.getPosition()))){
                    result_peg_715.nextPosition();
                } else {
                    goto out_peg_717;
                }
            }
            result_peg_715.setValue((void*) ",");
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        result_peg_715 = rule_expr_c(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
            arg3 = result_peg_715.getValues();
        
        
        
        result_peg_715 = rule_s(stream, result_peg_715.getPosition());
            if (result_peg_715.error()){
                goto out_peg_717;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_715.getPosition()))){
                    result_peg_715.nextPosition();
                } else {
                    goto out_peg_717;
                }
            }
            result_peg_715.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction("ifelse", arg1, arg2, arg3);
                result_peg_715.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_715;
    stream.update(result_peg_715.getPosition());
    
    
    return result_peg_715;
    out_peg_717:
    Result result_peg_732(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_732.getPosition()))){
                    result_peg_732.nextPosition();
                } else {
                    goto out_peg_734;
                }
            }
            result_peg_732.setValue((void*) "gethitvar");
            name = result_peg_732.getValues();
        
        
        
        result_peg_732 = rule_s(stream, result_peg_732.getPosition());
            if (result_peg_732.error()){
                goto out_peg_734;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_732.getPosition()))){
                    result_peg_732.nextPosition();
                } else {
                    goto out_peg_734;
                }
            }
            result_peg_732.setValue((void*) "(");
        
        
        
        result_peg_732 = rule_s(stream, result_peg_732.getPosition());
            if (result_peg_732.error()){
                goto out_peg_734;
            }
        
        
        
        result_peg_732 = rule_expr_c(stream, result_peg_732.getPosition());
            if (result_peg_732.error()){
                goto out_peg_734;
            }
            arg1 = result_peg_732.getValues();
        
        
        
        result_peg_732 = rule_s(stream, result_peg_732.getPosition());
            if (result_peg_732.error()){
                goto out_peg_734;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_732.getPosition()))){
                    result_peg_732.nextPosition();
                } else {
                    goto out_peg_734;
                }
            }
            result_peg_732.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_732.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_732;
    stream.update(result_peg_732.getPosition());
    
    
    return result_peg_732;
    out_peg_734:
    Result result_peg_741(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_741.getPosition()))){
                    result_peg_741.nextPosition();
                } else {
                    goto out_peg_743;
                }
            }
            result_peg_741.setValue((void*) "floor");
            name = result_peg_741.getValues();
        
        
        
        result_peg_741 = rule_s(stream, result_peg_741.getPosition());
            if (result_peg_741.error()){
                goto out_peg_743;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_741.getPosition()))){
                    result_peg_741.nextPosition();
                } else {
                    goto out_peg_743;
                }
            }
            result_peg_741.setValue((void*) "(");
        
        
        
        result_peg_741 = rule_s(stream, result_peg_741.getPosition());
            if (result_peg_741.error()){
                goto out_peg_743;
            }
        
        
        
        result_peg_741 = rule_expr_c(stream, result_peg_741.getPosition());
            if (result_peg_741.error()){
                goto out_peg_743;
            }
            arg1 = result_peg_741.getValues();
        
        
        
        result_peg_741 = rule_s(stream, result_peg_741.getPosition());
            if (result_peg_741.error()){
                goto out_peg_743;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_741.getPosition()))){
                    result_peg_741.nextPosition();
                } else {
                    goto out_peg_743;
                }
            }
            result_peg_741.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_741.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_741;
    stream.update(result_peg_741.getPosition());
    
    
    return result_peg_741;
    out_peg_743:
    Result result_peg_750(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_750.getPosition()))){
                    result_peg_750.nextPosition();
                } else {
                    goto out_peg_752;
                }
            }
            result_peg_750.setValue((void*) "ceil");
            name = result_peg_750.getValues();
        
        
        
        result_peg_750 = rule_s(stream, result_peg_750.getPosition());
            if (result_peg_750.error()){
                goto out_peg_752;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_750.getPosition()))){
                    result_peg_750.nextPosition();
                } else {
                    goto out_peg_752;
                }
            }
            result_peg_750.setValue((void*) "(");
        
        
        
        result_peg_750 = rule_s(stream, result_peg_750.getPosition());
            if (result_peg_750.error()){
                goto out_peg_752;
            }
        
        
        
        result_peg_750 = rule_expr_c(stream, result_peg_750.getPosition());
            if (result_peg_750.error()){
                goto out_peg_752;
            }
            arg1 = result_peg_750.getValues();
        
        
        
        result_peg_750 = rule_s(stream, result_peg_750.getPosition());
            if (result_peg_750.error()){
                goto out_peg_752;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_750.getPosition()))){
                    result_peg_750.nextPosition();
                } else {
                    goto out_peg_752;
                }
            }
            result_peg_750.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_750.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_750;
    stream.update(result_peg_750.getPosition());
    
    
    return result_peg_750;
    out_peg_752:
    Result result_peg_759(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("sin"[i], stream.get(result_peg_759.getPosition()))){
                    result_peg_759.nextPosition();
                } else {
                    goto out_peg_761;
                }
            }
            result_peg_759.setValue((void*) "sin");
            name = result_peg_759.getValues();
        
        
        
        result_peg_759 = rule_s(stream, result_peg_759.getPosition());
            if (result_peg_759.error()){
                goto out_peg_761;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_759.getPosition()))){
                    result_peg_759.nextPosition();
                } else {
                    goto out_peg_761;
                }
            }
            result_peg_759.setValue((void*) "(");
        
        
        
        result_peg_759 = rule_s(stream, result_peg_759.getPosition());
            if (result_peg_759.error()){
                goto out_peg_761;
            }
        
        
        
        result_peg_759 = rule_expr_c(stream, result_peg_759.getPosition());
            if (result_peg_759.error()){
                goto out_peg_761;
            }
            arg1 = result_peg_759.getValues();
        
        
        
        result_peg_759 = rule_s(stream, result_peg_759.getPosition());
            if (result_peg_759.error()){
                goto out_peg_761;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_759.getPosition()))){
                    result_peg_759.nextPosition();
                } else {
                    goto out_peg_761;
                }
            }
            result_peg_759.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_759.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_759;
    stream.update(result_peg_759.getPosition());
    
    
    return result_peg_759;
    out_peg_761:
    Result result_peg_768(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_768.getPosition()))){
                    result_peg_768.nextPosition();
                } else {
                    goto out_peg_770;
                }
            }
            result_peg_768.setValue((void*) "sysvar");
            name = result_peg_768.getValues();
        
        
        
        result_peg_768 = rule_s(stream, result_peg_768.getPosition());
            if (result_peg_768.error()){
                goto out_peg_770;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_768.getPosition()))){
                    result_peg_768.nextPosition();
                } else {
                    goto out_peg_770;
                }
            }
            result_peg_768.setValue((void*) "(");
        
        
        
        result_peg_768 = rule_s(stream, result_peg_768.getPosition());
            if (result_peg_768.error()){
                goto out_peg_770;
            }
        
        
        
        result_peg_768 = rule_integer(stream, result_peg_768.getPosition());
            if (result_peg_768.error()){
                goto out_peg_770;
            }
            arg1 = result_peg_768.getValues();
        
        
        
        result_peg_768 = rule_s(stream, result_peg_768.getPosition());
            if (result_peg_768.error()){
                goto out_peg_770;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_768.getPosition()))){
                    result_peg_768.nextPosition();
                } else {
                    goto out_peg_770;
                }
            }
            result_peg_768.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_768.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_768;
    stream.update(result_peg_768.getPosition());
    
    
    return result_peg_768;
    out_peg_770:
    Result result_peg_777(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_777.getPosition()))){
                    result_peg_777.nextPosition();
                } else {
                    goto out_peg_779;
                }
            }
            result_peg_777.setValue((void*) "var");
            name = result_peg_777.getValues();
        
        
        
        result_peg_777 = rule_s(stream, result_peg_777.getPosition());
            if (result_peg_777.error()){
                goto out_peg_779;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_777.getPosition()))){
                    result_peg_777.nextPosition();
                } else {
                    goto out_peg_779;
                }
            }
            result_peg_777.setValue((void*) "(");
        
        
        
        result_peg_777 = rule_s(stream, result_peg_777.getPosition());
            if (result_peg_777.error()){
                goto out_peg_779;
            }
        
        
        
        result_peg_777 = rule_integer(stream, result_peg_777.getPosition());
            if (result_peg_777.error()){
                goto out_peg_779;
            }
            arg1 = result_peg_777.getValues();
        
        
        
        result_peg_777 = rule_s(stream, result_peg_777.getPosition());
            if (result_peg_777.error()){
                goto out_peg_779;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_777.getPosition()))){
                    result_peg_777.nextPosition();
                } else {
                    goto out_peg_779;
                }
            }
            result_peg_777.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_777.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_777;
    stream.update(result_peg_777.getPosition());
    
    
    return result_peg_777;
    out_peg_779:
    Result result_peg_786(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_786.getPosition()))){
                    result_peg_786.nextPosition();
                } else {
                    goto out_peg_788;
                }
            }
            result_peg_786.setValue((void*) "numexplod");
            name = result_peg_786.getValues();
        
        
        
        result_peg_786 = rule_s(stream, result_peg_786.getPosition());
            if (result_peg_786.error()){
                goto out_peg_788;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_786.getPosition()))){
                    result_peg_786.nextPosition();
                } else {
                    goto out_peg_788;
                }
            }
            result_peg_786.setValue((void*) "(");
        
        
        
        result_peg_786 = rule_s(stream, result_peg_786.getPosition());
            if (result_peg_786.error()){
                goto out_peg_788;
            }
        
        
        
        result_peg_786 = rule_integer(stream, result_peg_786.getPosition());
            if (result_peg_786.error()){
                goto out_peg_788;
            }
            arg1 = result_peg_786.getValues();
        
        
        
        result_peg_786 = rule_s(stream, result_peg_786.getPosition());
            if (result_peg_786.error()){
                goto out_peg_788;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_786.getPosition()))){
                    result_peg_786.nextPosition();
                } else {
                    goto out_peg_788;
                }
            }
            result_peg_786.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_786.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_786;
    stream.update(result_peg_786.getPosition());
    
    
    return result_peg_786;
    out_peg_788:
    Result result_peg_795(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_795.getPosition()))){
                    result_peg_795.nextPosition();
                } else {
                    goto out_peg_797;
                }
            }
            result_peg_795.setValue((void*) "numhelper");
            name = result_peg_795.getValues();
        
        
        
        result_peg_795 = rule_s(stream, result_peg_795.getPosition());
            if (result_peg_795.error()){
                goto out_peg_797;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_795.getPosition()))){
                    result_peg_795.nextPosition();
                } else {
                    goto out_peg_797;
                }
            }
            result_peg_795.setValue((void*) "(");
        
        
        
        result_peg_795 = rule_s(stream, result_peg_795.getPosition());
            if (result_peg_795.error()){
                goto out_peg_797;
            }
        
        
        
        result_peg_795 = rule_integer(stream, result_peg_795.getPosition());
            if (result_peg_795.error()){
                goto out_peg_797;
            }
            arg1 = result_peg_795.getValues();
        
        
        
        result_peg_795 = rule_s(stream, result_peg_795.getPosition());
            if (result_peg_795.error()){
                goto out_peg_797;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_795.getPosition()))){
                    result_peg_795.nextPosition();
                } else {
                    goto out_peg_797;
                }
            }
            result_peg_795.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_795.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_795;
    stream.update(result_peg_795.getPosition());
    
    
    return result_peg_795;
    out_peg_797:
    Result result_peg_804(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_804.getPosition()))){
                    result_peg_804.nextPosition();
                } else {
                    goto out_peg_806;
                }
            }
            result_peg_804.setValue((void*) "numprojid");
            name = result_peg_804.getValues();
        
        
        
        result_peg_804 = rule_s(stream, result_peg_804.getPosition());
            if (result_peg_804.error()){
                goto out_peg_806;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_804.getPosition()))){
                    result_peg_804.nextPosition();
                } else {
                    goto out_peg_806;
                }
            }
            result_peg_804.setValue((void*) "(");
        
        
        
        result_peg_804 = rule_s(stream, result_peg_804.getPosition());
            if (result_peg_804.error()){
                goto out_peg_806;
            }
        
        
        
        result_peg_804 = rule_integer(stream, result_peg_804.getPosition());
            if (result_peg_804.error()){
                goto out_peg_806;
            }
            arg1 = result_peg_804.getValues();
        
        
        
        result_peg_804 = rule_s(stream, result_peg_804.getPosition());
            if (result_peg_804.error()){
                goto out_peg_806;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_804.getPosition()))){
                    result_peg_804.nextPosition();
                } else {
                    goto out_peg_806;
                }
            }
            result_peg_804.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_804.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_804;
    stream.update(result_peg_804.getPosition());
    
    
    return result_peg_804;
    out_peg_806:
    Result result_peg_813(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_813.getPosition()))){
                    result_peg_813.nextPosition();
                } else {
                    goto out_peg_815;
                }
            }
            result_peg_813.setValue((void*) "helper");
            name = result_peg_813.getValues();
        
        
        
        result_peg_813 = rule_s(stream, result_peg_813.getPosition());
            if (result_peg_813.error()){
                goto out_peg_815;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_813.getPosition()))){
                    result_peg_813.nextPosition();
                } else {
                    goto out_peg_815;
                }
            }
            result_peg_813.setValue((void*) "(");
        
        
        
        result_peg_813 = rule_s(stream, result_peg_813.getPosition());
            if (result_peg_813.error()){
                goto out_peg_815;
            }
        
        
        
        result_peg_813 = rule_integer(stream, result_peg_813.getPosition());
            if (result_peg_813.error()){
                goto out_peg_815;
            }
            arg1 = result_peg_813.getValues();
        
        
        
        result_peg_813 = rule_s(stream, result_peg_813.getPosition());
            if (result_peg_813.error()){
                goto out_peg_815;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_813.getPosition()))){
                    result_peg_813.nextPosition();
                } else {
                    goto out_peg_815;
                }
            }
            result_peg_813.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_813.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_813;
    stream.update(result_peg_813.getPosition());
    
    
    return result_peg_813;
    out_peg_815:
    Result result_peg_822(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_822.getPosition()))){
                    result_peg_822.nextPosition();
                } else {
                    goto out_peg_824;
                }
            }
            result_peg_822.setValue((void*) "fvar");
            name = result_peg_822.getValues();
        
        
        
        result_peg_822 = rule_s(stream, result_peg_822.getPosition());
            if (result_peg_822.error()){
                goto out_peg_824;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_822.getPosition()))){
                    result_peg_822.nextPosition();
                } else {
                    goto out_peg_824;
                }
            }
            result_peg_822.setValue((void*) "(");
        
        
        
        result_peg_822 = rule_s(stream, result_peg_822.getPosition());
            if (result_peg_822.error()){
                goto out_peg_824;
            }
        
        
        
        result_peg_822 = rule_integer(stream, result_peg_822.getPosition());
            if (result_peg_822.error()){
                goto out_peg_824;
            }
            arg1 = result_peg_822.getValues();
        
        
        
        result_peg_822 = rule_s(stream, result_peg_822.getPosition());
            if (result_peg_822.error()){
                goto out_peg_824;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_822.getPosition()))){
                    result_peg_822.nextPosition();
                } else {
                    goto out_peg_824;
                }
            }
            result_peg_822.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_822.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_822;
    stream.update(result_peg_822.getPosition());
    
    
    return result_peg_822;
    out_peg_824:
    Result result_peg_831(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("enemynear"[i], stream.get(result_peg_831.getPosition()))){
                    result_peg_831.nextPosition();
                } else {
                    goto out_peg_833;
                }
            }
            result_peg_831.setValue((void*) "enemynear");
            name = result_peg_831.getValues();
        
        
        
        result_peg_831 = rule_s(stream, result_peg_831.getPosition());
            if (result_peg_831.error()){
                goto out_peg_833;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_831.getPosition()))){
                    result_peg_831.nextPosition();
                } else {
                    goto out_peg_833;
                }
            }
            result_peg_831.setValue((void*) "(");
        
        
        
        result_peg_831 = rule_s(stream, result_peg_831.getPosition());
            if (result_peg_831.error()){
                goto out_peg_833;
            }
        
        
        
        result_peg_831 = rule_integer(stream, result_peg_831.getPosition());
            if (result_peg_831.error()){
                goto out_peg_833;
            }
            arg1 = result_peg_831.getValues();
        
        
        
        result_peg_831 = rule_s(stream, result_peg_831.getPosition());
            if (result_peg_831.error()){
                goto out_peg_833;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_831.getPosition()))){
                    result_peg_831.nextPosition();
                } else {
                    goto out_peg_833;
                }
            }
            result_peg_831.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_831.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_831;
    stream.update(result_peg_831.getPosition());
    
    
    return result_peg_831;
    out_peg_833:
    Result result_peg_840(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("target"[i], stream.get(result_peg_840.getPosition()))){
                    result_peg_840.nextPosition();
                } else {
                    goto out_peg_842;
                }
            }
            result_peg_840.setValue((void*) "target");
            name = result_peg_840.getValues();
        
        
        
        result_peg_840 = rule_s(stream, result_peg_840.getPosition());
            if (result_peg_840.error()){
                goto out_peg_842;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_840.getPosition()))){
                    result_peg_840.nextPosition();
                } else {
                    goto out_peg_842;
                }
            }
            result_peg_840.setValue((void*) "(");
        
        
        
        result_peg_840 = rule_s(stream, result_peg_840.getPosition());
            if (result_peg_840.error()){
                goto out_peg_842;
            }
        
        
        
        result_peg_840 = rule_integer(stream, result_peg_840.getPosition());
            if (result_peg_840.error()){
                goto out_peg_842;
            }
            arg1 = result_peg_840.getValues();
        
        
        
        result_peg_840 = rule_s(stream, result_peg_840.getPosition());
            if (result_peg_840.error()){
                goto out_peg_842;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_840.getPosition()))){
                    result_peg_840.nextPosition();
                } else {
                    goto out_peg_842;
                }
            }
            result_peg_840.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_840.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_840;
    stream.update(result_peg_840.getPosition());
    
    
    return result_peg_840;
    out_peg_842:
    Result result_peg_849(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_849.getPosition()))){
                    result_peg_849.nextPosition();
                } else {
                    goto out_peg_851;
                }
            }
            result_peg_849.setValue((void*) "ishelper");
            name = result_peg_849.getValues();
        
        
        
        result_peg_849 = rule_s(stream, result_peg_849.getPosition());
            if (result_peg_849.error()){
                goto out_peg_851;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_849.getPosition()))){
                    result_peg_849.nextPosition();
                } else {
                    goto out_peg_851;
                }
            }
            result_peg_849.setValue((void*) "(");
        
        
        
        result_peg_849 = rule_s(stream, result_peg_849.getPosition());
            if (result_peg_849.error()){
                goto out_peg_851;
            }
        
        
        
        result_peg_849 = rule_integer(stream, result_peg_849.getPosition());
            if (result_peg_849.error()){
                goto out_peg_851;
            }
            arg1 = result_peg_849.getValues();
        
        
        
        result_peg_849 = rule_s(stream, result_peg_849.getPosition());
            if (result_peg_849.error()){
                goto out_peg_851;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_849.getPosition()))){
                    result_peg_849.nextPosition();
                } else {
                    goto out_peg_851;
                }
            }
            result_peg_849.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_849.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_849;
    stream.update(result_peg_849.getPosition());
    
    
    return result_peg_849;
    out_peg_851:
    Result result_peg_858(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numtarget"[i], stream.get(result_peg_858.getPosition()))){
                    result_peg_858.nextPosition();
                } else {
                    goto out_peg_860;
                }
            }
            result_peg_858.setValue((void*) "numtarget");
            name = result_peg_858.getValues();
        
        
        
        result_peg_858 = rule_s(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_858.getPosition()))){
                    result_peg_858.nextPosition();
                } else {
                    goto out_peg_860;
                }
            }
            result_peg_858.setValue((void*) "(");
        
        
        
        result_peg_858 = rule_s(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        result_peg_858 = rule_expr_c(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
            arg1 = result_peg_858.getValues();
        
        
        
        result_peg_858 = rule_s(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_858.getPosition()))){
                    result_peg_858.nextPosition();
                } else {
                    goto out_peg_860;
                }
            }
            result_peg_858.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_858.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_858;
    stream.update(result_peg_858.getPosition());
    
    
    return result_peg_858;
    out_peg_860:
    Result result_peg_867(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_867.getPosition()))){
                    result_peg_867.nextPosition();
                } else {
                    goto out_peg_869;
                }
            }
            result_peg_867.setValue((void*) "animelemtime");
            name = result_peg_867.getValues();
        
        
        
        result_peg_867 = rule_s(stream, result_peg_867.getPosition());
            if (result_peg_867.error()){
                goto out_peg_869;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_867.getPosition()))){
                    result_peg_867.nextPosition();
                } else {
                    goto out_peg_869;
                }
            }
            result_peg_867.setValue((void*) "(");
        
        
        
        result_peg_867 = rule_s(stream, result_peg_867.getPosition());
            if (result_peg_867.error()){
                goto out_peg_869;
            }
        
        
        
        result_peg_867 = rule_expr_c(stream, result_peg_867.getPosition());
            if (result_peg_867.error()){
                goto out_peg_869;
            }
            arg1 = result_peg_867.getValues();
        
        
        
        result_peg_867 = rule_s(stream, result_peg_867.getPosition());
            if (result_peg_867.error()){
                goto out_peg_869;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_867.getPosition()))){
                    result_peg_867.nextPosition();
                } else {
                    goto out_peg_869;
                }
            }
            result_peg_867.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_867.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_867;
    stream.update(result_peg_867.getPosition());
    
    
    return result_peg_867;
    out_peg_869:
    Result result_peg_876(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_876.getPosition()))){
                    result_peg_876.nextPosition();
                } else {
                    goto out_peg_878;
                }
            }
            result_peg_876.setValue((void*) "animelemno");
            name = result_peg_876.getValues();
        
        
        
        result_peg_876 = rule_s(stream, result_peg_876.getPosition());
            if (result_peg_876.error()){
                goto out_peg_878;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_876.getPosition()))){
                    result_peg_876.nextPosition();
                } else {
                    goto out_peg_878;
                }
            }
            result_peg_876.setValue((void*) "(");
        
        
        
        result_peg_876 = rule_s(stream, result_peg_876.getPosition());
            if (result_peg_876.error()){
                goto out_peg_878;
            }
        
        
        
        result_peg_876 = rule_expr_c(stream, result_peg_876.getPosition());
            if (result_peg_876.error()){
                goto out_peg_878;
            }
            arg1 = result_peg_876.getValues();
        
        
        
        result_peg_876 = rule_s(stream, result_peg_876.getPosition());
            if (result_peg_876.error()){
                goto out_peg_878;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_876.getPosition()))){
                    result_peg_876.nextPosition();
                } else {
                    goto out_peg_878;
                }
            }
            result_peg_876.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_876.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_876;
    stream.update(result_peg_876.getPosition());
    
    
    return result_peg_876;
    out_peg_878:
    Result result_peg_885(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_885.getPosition()))){
                    result_peg_885.nextPosition();
                } else {
                    goto out_peg_887;
                }
            }
            result_peg_885.setValue((void*) "animexist");
            name = result_peg_885.getValues();
        
        
        
        result_peg_885 = rule_s(stream, result_peg_885.getPosition());
            if (result_peg_885.error()){
                goto out_peg_887;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_885.getPosition()))){
                    result_peg_885.nextPosition();
                } else {
                    goto out_peg_887;
                }
            }
            result_peg_885.setValue((void*) "(");
        
        
        
        result_peg_885 = rule_s(stream, result_peg_885.getPosition());
            if (result_peg_885.error()){
                goto out_peg_887;
            }
        
        
        
        result_peg_885 = rule_expr_c(stream, result_peg_885.getPosition());
            if (result_peg_885.error()){
                goto out_peg_887;
            }
            arg1 = result_peg_885.getValues();
        
        
        
        result_peg_885 = rule_s(stream, result_peg_885.getPosition());
            if (result_peg_885.error()){
                goto out_peg_887;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_885.getPosition()))){
                    result_peg_885.nextPosition();
                } else {
                    goto out_peg_887;
                }
            }
            result_peg_885.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_885.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_885;
    stream.update(result_peg_885.getPosition());
    
    
    return result_peg_885;
    out_peg_887:
    Result result_peg_894(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_894.getPosition()))){
                    result_peg_894.nextPosition();
                } else {
                    goto out_peg_896;
                }
            }
            result_peg_894.setValue((void*) "projguarded");
            name = result_peg_894.getValues();
        
        
        
        result_peg_894 = rule_s(stream, result_peg_894.getPosition());
            if (result_peg_894.error()){
                goto out_peg_896;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_894.getPosition()))){
                    result_peg_894.nextPosition();
                } else {
                    goto out_peg_896;
                }
            }
            result_peg_894.setValue((void*) "(");
        
        
        
        result_peg_894 = rule_s(stream, result_peg_894.getPosition());
            if (result_peg_894.error()){
                goto out_peg_896;
            }
        
        
        
        result_peg_894 = rule_expr_c(stream, result_peg_894.getPosition());
            if (result_peg_894.error()){
                goto out_peg_896;
            }
            arg1 = result_peg_894.getValues();
        
        
        
        result_peg_894 = rule_s(stream, result_peg_894.getPosition());
            if (result_peg_894.error()){
                goto out_peg_896;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_894.getPosition()))){
                    result_peg_894.nextPosition();
                } else {
                    goto out_peg_896;
                }
            }
            result_peg_894.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_894.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_894;
    stream.update(result_peg_894.getPosition());
    
    
    return result_peg_894;
    out_peg_896:
    Result result_peg_903(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projhittime"[i], stream.get(result_peg_903.getPosition()))){
                    result_peg_903.nextPosition();
                } else {
                    goto out_peg_905;
                }
            }
            result_peg_903.setValue((void*) "projhittime");
            name = result_peg_903.getValues();
        
        
        
        result_peg_903 = rule_s(stream, result_peg_903.getPosition());
            if (result_peg_903.error()){
                goto out_peg_905;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_903.getPosition()))){
                    result_peg_903.nextPosition();
                } else {
                    goto out_peg_905;
                }
            }
            result_peg_903.setValue((void*) "(");
        
        
        
        result_peg_903 = rule_s(stream, result_peg_903.getPosition());
            if (result_peg_903.error()){
                goto out_peg_905;
            }
        
        
        
        result_peg_903 = rule_expr_c(stream, result_peg_903.getPosition());
            if (result_peg_903.error()){
                goto out_peg_905;
            }
            arg1 = result_peg_903.getValues();
        
        
        
        result_peg_903 = rule_s(stream, result_peg_903.getPosition());
            if (result_peg_903.error()){
                goto out_peg_905;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_903.getPosition()))){
                    result_peg_903.nextPosition();
                } else {
                    goto out_peg_905;
                }
            }
            result_peg_903.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_903.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_903;
    stream.update(result_peg_903.getPosition());
    
    
    return result_peg_903;
    out_peg_905:
    Result result_peg_912(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_912.getPosition()))){
                    result_peg_912.nextPosition();
                } else {
                    goto out_peg_914;
                }
            }
            result_peg_912.setValue((void*) "projcontact");
            name = result_peg_912.getValues();
        
        
        
        result_peg_912 = rule_s(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_912.getPosition()))){
                    result_peg_912.nextPosition();
                } else {
                    goto out_peg_914;
                }
            }
            result_peg_912.setValue((void*) "(");
        
        
        
        result_peg_912 = rule_s(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        result_peg_912 = rule_expr_c(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
            arg1 = result_peg_912.getValues();
        
        
        
        result_peg_912 = rule_s(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_912.getPosition()))){
                    result_peg_912.nextPosition();
                } else {
                    goto out_peg_914;
                }
            }
            result_peg_912.setValue((void*) ")");
        
        
        
        result_peg_912 = rule_s(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_912.getPosition()))){
                    result_peg_912.nextPosition();
                } else {
                    goto out_peg_914;
                }
            }
            result_peg_912.setValue((void*) "=");
        
        
        
        result_peg_912 = rule_s(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        result_peg_912 = rule_integer(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        result_peg_912 = rule_function_rest(stream, result_peg_912.getPosition());
            if (result_peg_912.error()){
                goto out_peg_914;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_912.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_912;
    stream.update(result_peg_912.getPosition());
    
    
    return result_peg_912;
    out_peg_914:
    Result result_peg_926(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_926.getPosition()))){
                    result_peg_926.nextPosition();
                } else {
                    goto out_peg_928;
                }
            }
            result_peg_926.setValue((void*) "animelem");
            name = result_peg_926.getValues();
        
        
        
        result_peg_926 = rule_s(stream, result_peg_926.getPosition());
            if (result_peg_926.error()){
                goto out_peg_928;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_926.getPosition()))){
                    result_peg_926.nextPosition();
                } else {
                    goto out_peg_928;
                }
            }
            result_peg_926.setValue((void*) "=");
        
        
        
        result_peg_926 = rule_s(stream, result_peg_926.getPosition());
            if (result_peg_926.error()){
                goto out_peg_928;
            }
        
        
        
        result_peg_926 = rule_integer(stream, result_peg_926.getPosition());
            if (result_peg_926.error()){
                goto out_peg_928;
            }
            arg1 = result_peg_926.getValues();
        
        
        
        result_peg_926 = rule_function_rest(stream, result_peg_926.getPosition());
            if (result_peg_926.error()){
                goto out_peg_928;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction(name, arg1);
                result_peg_926.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_926;
    stream.update(result_peg_926.getPosition());
    
    
    return result_peg_926;
    out_peg_928:
    Result result_peg_934(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_934.getPosition()))){
                    result_peg_934.nextPosition();
                } else {
                    goto out_peg_936;
                }
            }
            result_peg_934.setValue((void*) "numhelper");
            name = result_peg_934.getValues();
        
        
        
        result_peg_934 = rule_s(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_934.getPosition()))){
                    result_peg_934.nextPosition();
                } else {
                    goto out_peg_936;
                }
            }
            result_peg_934.setValue((void*) "(");
        
        
        
        result_peg_934 = rule_s(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
        
        
        
        result_peg_934 = rule_expr_c(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
            arg1 = result_peg_934.getValues();
        
        
        
        result_peg_934 = rule_s(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_934.getPosition()))){
                    result_peg_934.nextPosition();
                } else {
                    goto out_peg_936;
                }
            }
            result_peg_934.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_934.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_934;
    stream.update(result_peg_934.getPosition());
    
    
    return result_peg_934;
    out_peg_936:
    Result result_peg_943(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_943.getPosition()))){
                    result_peg_943.nextPosition();
                } else {
                    goto out_peg_945;
                }
            }
            result_peg_943.setValue((void*) "helper");
            name = result_peg_943.getValues();
        
        
        
        result_peg_943 = rule_s(stream, result_peg_943.getPosition());
            if (result_peg_943.error()){
                goto out_peg_945;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_943.getPosition()))){
                    result_peg_943.nextPosition();
                } else {
                    goto out_peg_945;
                }
            }
            result_peg_943.setValue((void*) "(");
        
        
        
        result_peg_943 = rule_s(stream, result_peg_943.getPosition());
            if (result_peg_943.error()){
                goto out_peg_945;
            }
        
        
        
        result_peg_943 = rule_expr_c(stream, result_peg_943.getPosition());
            if (result_peg_943.error()){
                goto out_peg_945;
            }
            arg1 = result_peg_943.getValues();
        
        
        
        result_peg_943 = rule_s(stream, result_peg_943.getPosition());
            if (result_peg_943.error()){
                goto out_peg_945;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_943.getPosition()))){
                    result_peg_943.nextPosition();
                } else {
                    goto out_peg_945;
                }
            }
            result_peg_943.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_943.setValue(value);
            }
        
        
    }
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = result_peg_943;
    stream.update(result_peg_943.getPosition());
    
    
    return result_peg_943;
    out_peg_945:
    
    if (column_peg_687.chunk9 == 0){
        column_peg_687.chunk9 = new Chunk9();
    }
    column_peg_687.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_953 = stream.getColumn(position);
    if (column_peg_953.chunk10 != 0 && column_peg_953.chunk10->chunk_function_rest.calculated()){
        return column_peg_953.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_963(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_954(myposition);
    
    {
    
        result_peg_954 = rule_s(stream, result_peg_954.getPosition());
            if (result_peg_954.error()){
                goto out_peg_956;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_954.getPosition()))){
                    result_peg_954.nextPosition();
                } else {
                    goto out_peg_956;
                }
            }
            result_peg_954.setValue((void*) ",");
        
        
        
        result_peg_954 = rule_s(stream, result_peg_954.getPosition());
            if (result_peg_954.error()){
                goto out_peg_956;
            }
        
        
        
        int save_peg_960 = result_peg_954.getPosition();
            
            result_peg_954 = rule_all_compare(stream, result_peg_954.getPosition());
            if (result_peg_954.error()){
                
                result_peg_954 = Result(save_peg_960);
                result_peg_954.setValue((void*) 0);
                
            }
        
        
        
        result_peg_954 = rule_s(stream, result_peg_954.getPosition());
            if (result_peg_954.error()){
                goto out_peg_956;
            }
        
        
        
        result_peg_954 = rule_value(stream, result_peg_954.getPosition());
            if (result_peg_954.error()){
                goto out_peg_956;
            }
        
        
    }
    
    if (column_peg_953.chunk10 == 0){
        column_peg_953.chunk10 = new Chunk10();
    }
    column_peg_953.chunk10->chunk_function_rest = result_peg_954;
    stream.update(result_peg_954.getPosition());
    
    
    return result_peg_954;
    out_peg_956:
    Result result_peg_962(myposition);
    
    
    
    if (column_peg_953.chunk10 == 0){
        column_peg_953.chunk10 = new Chunk10();
    }
    column_peg_953.chunk10->chunk_function_rest = result_peg_962;
    stream.update(result_peg_962.getPosition());
    
    
    return result_peg_962;
    
    if (column_peg_953.chunk10 == 0){
        column_peg_953.chunk10 = new Chunk10();
    }
    column_peg_953.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_964 = stream.getColumn(position);
    if (column_peg_964.chunk10 != 0 && column_peg_964.chunk10->chunk_keys.calculated()){
        return column_peg_964.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_967(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_965(myposition);
    
    result_peg_965 = rule_key_value_list(stream, result_peg_965.getPosition());
    if (result_peg_965.error()){
        goto out_peg_966;
    }
    
    if (column_peg_964.chunk10 == 0){
        column_peg_964.chunk10 = new Chunk10();
    }
    column_peg_964.chunk10->chunk_keys = result_peg_965;
    stream.update(result_peg_965.getPosition());
    
    
    return result_peg_965;
    out_peg_966:
    
    if (column_peg_964.chunk10 == 0){
        column_peg_964.chunk10 = new Chunk10();
    }
    column_peg_964.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_968 = stream.getColumn(position);
    if (column_peg_968.chunk10 != 0 && column_peg_968.chunk10->chunk_key_value_list.calculated()){
        return column_peg_968.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_978(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_969(myposition);
    
    {
    
        result_peg_969 = rule_key(stream, result_peg_969.getPosition());
            if (result_peg_969.error()){
                goto out_peg_971;
            }
            first = result_peg_969.getValues();
        
        
        
        result_peg_969.reset();
            do{
                Result result_peg_974(result_peg_969.getPosition());
                {
                
                    result_peg_974 = rule_s(stream, result_peg_974.getPosition());
                        if (result_peg_974.error()){
                            goto loop_peg_973;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_974.getPosition()))){
                                result_peg_974.nextPosition();
                            } else {
                                goto loop_peg_973;
                            }
                        }
                        result_peg_974.setValue((void*) ",");
                    
                    
                    
                    result_peg_974 = rule_s(stream, result_peg_974.getPosition());
                        if (result_peg_974.error()){
                            goto loop_peg_973;
                        }
                    
                    
                    
                    result_peg_974 = rule_key(stream, result_peg_974.getPosition());
                        if (result_peg_974.error()){
                            goto loop_peg_973;
                        }
                    
                    
                }
                result_peg_969.addResult(result_peg_974);
            } while (true);
            loop_peg_973:
            ;
            rest = result_peg_969.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_969.setValue(value);
            }
        
        
    }
    
    if (column_peg_968.chunk10 == 0){
        column_peg_968.chunk10 = new Chunk10();
    }
    column_peg_968.chunk10->chunk_key_value_list = result_peg_969;
    stream.update(result_peg_969.getPosition());
    
    
    return result_peg_969;
    out_peg_971:
    
    if (column_peg_968.chunk10 == 0){
        column_peg_968.chunk10 = new Chunk10();
    }
    column_peg_968.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_979 = stream.getColumn(position);
    if (column_peg_979.chunk10 != 0 && column_peg_979.chunk10->chunk_key.calculated()){
        return column_peg_979.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_987(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_980(myposition);
    
    {
    
        result_peg_980 = rule_key_real(stream, result_peg_980.getPosition());
            if (result_peg_980.error()){
                goto out_peg_982;
            }
        
        Result result_peg_981 = result_peg_980;
        
        result_peg_980 = rule_key_rest(stream, result_peg_980.getPosition(), result_peg_981.getValues());
            if (result_peg_980.error()){
                goto out_peg_982;
            }
            ok = result_peg_980.getValues();
        
        
        
        Result result_peg_986(result_peg_980);
            result_peg_986 = rule_identifier(stream, result_peg_986.getPosition());
            if (result_peg_986.error()){
                goto not_peg_985;
            }
            goto out_peg_982;
            not_peg_985:
            result_peg_980.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_980.setValue(value);
            }
        
        
    }
    
    if (column_peg_979.chunk10 == 0){
        column_peg_979.chunk10 = new Chunk10();
    }
    column_peg_979.chunk10->chunk_key = result_peg_980;
    stream.update(result_peg_980.getPosition());
    
    
    return result_peg_980;
    out_peg_982:
    
    if (column_peg_979.chunk10 == 0){
        column_peg_979.chunk10 = new Chunk10();
    }
    column_peg_979.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_998(stream, "key_rest");
    int myposition = position;
    tail_peg_990:
    Value another;
    Value new_left;
    Result result_peg_989(myposition);
    {
    
        result_peg_989 = rule_s(stream, result_peg_989.getPosition());
            if (result_peg_989.error()){
                goto out_peg_992;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_989.getPosition()))){
                    result_peg_989.nextPosition();
                } else {
                    goto out_peg_992;
                }
            }
            result_peg_989.setValue((void*) "+");
        
        
        
        result_peg_989 = rule_key_real(stream, result_peg_989.getPosition());
            if (result_peg_989.error()){
                goto out_peg_992;
            }
            another = result_peg_989.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_989.setValue(value);
            }
            new_left = result_peg_989.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_989.getPosition();
    goto tail_peg_990;
    out_peg_992:
    Result result_peg_996(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_996.setValue(value);
            }
        
        
    }
    stream.update(result_peg_996.getPosition());
    
    return result_peg_996;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_999 = stream.getColumn(position);
    if (column_peg_999.chunk11 != 0 && column_peg_999.chunk11->chunk_key_real.calculated()){
        return column_peg_999.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_1006(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_1000(myposition);
    
    {
    
        result_peg_1000.reset();
            do{
                Result result_peg_1003(result_peg_1000.getPosition());
                result_peg_1003 = rule_key_modifier(stream, result_peg_1003.getPosition());
                if (result_peg_1003.error()){
                    goto loop_peg_1002;
                }
                result_peg_1000.addResult(result_peg_1003);
            } while (true);
            loop_peg_1002:
            ;
            mods = result_peg_1000.getValues();
        
        
        
        result_peg_1000 = rule_key_name(stream, result_peg_1000.getPosition());
            if (result_peg_1000.error()){
                goto out_peg_1005;
            }
            name = result_peg_1000.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_1000.setValue(value);
            }
        
        
    }
    
    if (column_peg_999.chunk11 == 0){
        column_peg_999.chunk11 = new Chunk11();
    }
    column_peg_999.chunk11->chunk_key_real = result_peg_1000;
    stream.update(result_peg_1000.getPosition());
    
    
    return result_peg_1000;
    out_peg_1005:
    
    if (column_peg_999.chunk11 == 0){
        column_peg_999.chunk11 = new Chunk11();
    }
    column_peg_999.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_1007 = stream.getColumn(position);
    if (column_peg_1007.chunk11 != 0 && column_peg_1007.chunk11->chunk_key_modifier.calculated()){
        return column_peg_1007.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_1028(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_1008(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1008.getPosition()))){
                    result_peg_1008.nextPosition();
                } else {
                    goto out_peg_1010;
                }
            }
            result_peg_1008.setValue((void*) "~");
        
        
        
        result_peg_1008.reset();
            do{
                Result result_peg_1013(result_peg_1008.getPosition());
                {
                    
                    char letter_peg_1018 = stream.get(result_peg_1013.getPosition());
                    if (letter_peg_1018 != '\0' && strchr("0123456789", letter_peg_1018) != NULL){
                        result_peg_1013.nextPosition();
                        result_peg_1013.setValue((void*) (long) letter_peg_1018);
                    } else {
                        goto out_peg_1017;
                    }
                    
                }
                goto success_peg_1014;
                out_peg_1017:
                goto loop_peg_1012;
                success_peg_1014:
                ;
                result_peg_1008.addResult(result_peg_1013);
            } while (true);
            loop_peg_1012:
            ;
            num = result_peg_1008.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_1008.setValue(value);
            }
        
        
    }
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_modifier = result_peg_1008;
    stream.update(result_peg_1008.getPosition());
    
    
    return result_peg_1008;
    out_peg_1010:
    Result result_peg_1019(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_1019.getPosition()))){
                    result_peg_1019.nextPosition();
                } else {
                    goto out_peg_1021;
                }
            }
            result_peg_1019.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_1019.setValue(value);
            }
        
        
    }
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_modifier = result_peg_1019;
    stream.update(result_peg_1019.getPosition());
    
    
    return result_peg_1019;
    out_peg_1021:
    Result result_peg_1022(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1022.getPosition()))){
                    result_peg_1022.nextPosition();
                } else {
                    goto out_peg_1024;
                }
            }
            result_peg_1022.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_1022.setValue(value);
            }
        
        
    }
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_modifier = result_peg_1022;
    stream.update(result_peg_1022.getPosition());
    
    
    return result_peg_1022;
    out_peg_1024:
    Result result_peg_1025(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1025.getPosition()))){
                    result_peg_1025.nextPosition();
                } else {
                    goto out_peg_1027;
                }
            }
            result_peg_1025.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_1025.setValue(value);
            }
        
        
    }
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_modifier = result_peg_1025;
    stream.update(result_peg_1025.getPosition());
    
    
    return result_peg_1025;
    out_peg_1027:
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_1029 = stream.getColumn(position);
    if (column_peg_1029.chunk11 != 0 && column_peg_1029.chunk11->chunk_key_name.calculated()){
        return column_peg_1029.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_1060(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_1030(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_1030.getPosition()))){
            result_peg_1030.nextPosition();
        } else {
            goto out_peg_1031;
        }
    }
    result_peg_1030.setValue((void*) "DB");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1030;
    stream.update(result_peg_1030.getPosition());
    
    
    return result_peg_1030;
    out_peg_1031:
    Result result_peg_1032(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_1032.getPosition()))){
            result_peg_1032.nextPosition();
        } else {
            goto out_peg_1033;
        }
    }
    result_peg_1032.setValue((void*) "B");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1032;
    stream.update(result_peg_1032.getPosition());
    
    
    return result_peg_1032;
    out_peg_1033:
    Result result_peg_1034(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_1034.getPosition()))){
            result_peg_1034.nextPosition();
        } else {
            goto out_peg_1035;
        }
    }
    result_peg_1034.setValue((void*) "DF");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1034;
    stream.update(result_peg_1034.getPosition());
    
    
    return result_peg_1034;
    out_peg_1035:
    Result result_peg_1036(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_1036.getPosition()))){
            result_peg_1036.nextPosition();
        } else {
            goto out_peg_1037;
        }
    }
    result_peg_1036.setValue((void*) "D");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1036;
    stream.update(result_peg_1036.getPosition());
    
    
    return result_peg_1036;
    out_peg_1037:
    Result result_peg_1038(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_1038.getPosition()))){
            result_peg_1038.nextPosition();
        } else {
            goto out_peg_1039;
        }
    }
    result_peg_1038.setValue((void*) "F");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1038;
    stream.update(result_peg_1038.getPosition());
    
    
    return result_peg_1038;
    out_peg_1039:
    Result result_peg_1040(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_1040.getPosition()))){
            result_peg_1040.nextPosition();
        } else {
            goto out_peg_1041;
        }
    }
    result_peg_1040.setValue((void*) "UF");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1040;
    stream.update(result_peg_1040.getPosition());
    
    
    return result_peg_1040;
    out_peg_1041:
    Result result_peg_1042(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_1042.getPosition()))){
            result_peg_1042.nextPosition();
        } else {
            goto out_peg_1043;
        }
    }
    result_peg_1042.setValue((void*) "UB");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1042;
    stream.update(result_peg_1042.getPosition());
    
    
    return result_peg_1042;
    out_peg_1043:
    Result result_peg_1044(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_1044.getPosition()))){
            result_peg_1044.nextPosition();
        } else {
            goto out_peg_1045;
        }
    }
    result_peg_1044.setValue((void*) "U");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1044;
    stream.update(result_peg_1044.getPosition());
    
    
    return result_peg_1044;
    out_peg_1045:
    Result result_peg_1046(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_1046.getPosition()))){
            result_peg_1046.nextPosition();
        } else {
            goto out_peg_1047;
        }
    }
    result_peg_1046.setValue((void*) "a");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1046;
    stream.update(result_peg_1046.getPosition());
    
    
    return result_peg_1046;
    out_peg_1047:
    Result result_peg_1048(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_1048.getPosition()))){
            result_peg_1048.nextPosition();
        } else {
            goto out_peg_1049;
        }
    }
    result_peg_1048.setValue((void*) "b");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1048;
    stream.update(result_peg_1048.getPosition());
    
    
    return result_peg_1048;
    out_peg_1049:
    Result result_peg_1050(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_1050.getPosition()))){
            result_peg_1050.nextPosition();
        } else {
            goto out_peg_1051;
        }
    }
    result_peg_1050.setValue((void*) "c");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1050;
    stream.update(result_peg_1050.getPosition());
    
    
    return result_peg_1050;
    out_peg_1051:
    Result result_peg_1052(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_1052.getPosition()))){
            result_peg_1052.nextPosition();
        } else {
            goto out_peg_1053;
        }
    }
    result_peg_1052.setValue((void*) "x");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1052;
    stream.update(result_peg_1052.getPosition());
    
    
    return result_peg_1052;
    out_peg_1053:
    Result result_peg_1054(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_1054.getPosition()))){
            result_peg_1054.nextPosition();
        } else {
            goto out_peg_1055;
        }
    }
    result_peg_1054.setValue((void*) "y");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1054;
    stream.update(result_peg_1054.getPosition());
    
    
    return result_peg_1054;
    out_peg_1055:
    Result result_peg_1056(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_1056.getPosition()))){
            result_peg_1056.nextPosition();
        } else {
            goto out_peg_1057;
        }
    }
    result_peg_1056.setValue((void*) "z");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1056;
    stream.update(result_peg_1056.getPosition());
    
    
    return result_peg_1056;
    out_peg_1057:
    Result result_peg_1058(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_1058.getPosition()))){
            result_peg_1058.nextPosition();
        } else {
            goto out_peg_1059;
        }
    }
    result_peg_1058.setValue((void*) "s");
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = result_peg_1058;
    stream.update(result_peg_1058.getPosition());
    
    
    return result_peg_1058;
    out_peg_1059:
    
    if (column_peg_1029.chunk11 == 0){
        column_peg_1029.chunk11 = new Chunk11();
    }
    column_peg_1029.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1061 = stream.getColumn(position);
    if (column_peg_1061.chunk11 != 0 && column_peg_1061.chunk11->chunk_value.calculated()){
        return column_peg_1061.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_1079(stream, "value");
    int myposition = position;
    
    
    Result result_peg_1062(myposition);
    
    result_peg_1062 = rule_float(stream, result_peg_1062.getPosition());
    if (result_peg_1062.error()){
        goto out_peg_1063;
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1062;
    stream.update(result_peg_1062.getPosition());
    
    
    return result_peg_1062;
    out_peg_1063:
    Result result_peg_1064(myposition);
    
    result_peg_1064 = rule_integer(stream, result_peg_1064.getPosition());
    if (result_peg_1064.error()){
        goto out_peg_1065;
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1064;
    stream.update(result_peg_1064.getPosition());
    
    
    return result_peg_1064;
    out_peg_1065:
    Result result_peg_1066(myposition);
    
    result_peg_1066 = rule_keyword(stream, result_peg_1066.getPosition());
    if (result_peg_1066.error()){
        goto out_peg_1067;
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1066;
    stream.update(result_peg_1066.getPosition());
    
    
    return result_peg_1066;
    out_peg_1067:
    Result result_peg_1068(myposition);
    
    {
    
        Result result_peg_1071(result_peg_1068);
            result_peg_1071 = rule_keyword(stream, result_peg_1071.getPosition());
            if (result_peg_1071.error()){
                goto not_peg_1070;
            }
            goto out_peg_1072;
            not_peg_1070:
            result_peg_1068.setValue((void*)0);
        
        
        
        result_peg_1068 = rule_identifier(stream, result_peg_1068.getPosition());
            if (result_peg_1068.error()){
                goto out_peg_1072;
            }
        
        
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1068;
    stream.update(result_peg_1068.getPosition());
    
    
    return result_peg_1068;
    out_peg_1072:
    Result result_peg_1073(myposition);
    
    result_peg_1073 = rule_range(stream, result_peg_1073.getPosition());
    if (result_peg_1073.error()){
        goto out_peg_1074;
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1073;
    stream.update(result_peg_1073.getPosition());
    
    
    return result_peg_1073;
    out_peg_1074:
    Result result_peg_1075(myposition);
    
    result_peg_1075 = rule_string(stream, result_peg_1075.getPosition());
    if (result_peg_1075.error()){
        goto out_peg_1076;
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1075;
    stream.update(result_peg_1075.getPosition());
    
    
    return result_peg_1075;
    out_peg_1076:
    Result result_peg_1077(myposition);
    
    result_peg_1077 = rule_hitflag(stream, result_peg_1077.getPosition());
    if (result_peg_1077.error()){
        goto out_peg_1078;
    }
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = result_peg_1077;
    stream.update(result_peg_1077.getPosition());
    
    
    return result_peg_1077;
    out_peg_1078:
    
    if (column_peg_1061.chunk11 == 0){
        column_peg_1061.chunk11 = new Chunk11();
    }
    column_peg_1061.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_1080 = stream.getColumn(position);
    if (column_peg_1080.chunk11 != 0 && column_peg_1080.chunk11->chunk_hitflag.calculated()){
        return column_peg_1080.chunk11->chunk_hitflag;
    }
    
    RuleTrace trace_peg_1087(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_1081(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("M-"[i], stream.get(result_peg_1081.getPosition()))){
                    result_peg_1081.nextPosition();
                } else {
                    goto out_peg_1083;
                }
            }
            result_peg_1081.setValue((void*) "M-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("M-");
                result_peg_1081.setValue(value);
            }
        
        
    }
    
    if (column_peg_1080.chunk11 == 0){
        column_peg_1080.chunk11 = new Chunk11();
    }
    column_peg_1080.chunk11->chunk_hitflag = result_peg_1081;
    stream.update(result_peg_1081.getPosition());
    
    
    return result_peg_1081;
    out_peg_1083:
    Result result_peg_1084(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("A-"[i], stream.get(result_peg_1084.getPosition()))){
                    result_peg_1084.nextPosition();
                } else {
                    goto out_peg_1086;
                }
            }
            result_peg_1084.setValue((void*) "A-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("A-");
                result_peg_1084.setValue(value);
            }
        
        
    }
    
    if (column_peg_1080.chunk11 == 0){
        column_peg_1080.chunk11 = new Chunk11();
    }
    column_peg_1080.chunk11->chunk_hitflag = result_peg_1084;
    stream.update(result_peg_1084.getPosition());
    
    
    return result_peg_1084;
    out_peg_1086:
    
    if (column_peg_1080.chunk11 == 0){
        column_peg_1080.chunk11 = new Chunk11();
    }
    column_peg_1080.chunk11->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_1088 = stream.getColumn(position);
    if (column_peg_1088.chunk12 != 0 && column_peg_1088.chunk12->chunk_keyword.calculated()){
        return column_peg_1088.chunk12->chunk_keyword;
    }
    
    RuleTrace trace_peg_1095(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_1089(myposition);
    
    {
    
        result_peg_1089 = rule_keyword_real(stream, result_peg_1089.getPosition());
            if (result_peg_1089.error()){
                goto out_peg_1091;
            }
        
        Result result_peg_1090 = result_peg_1089;
        
        Result result_peg_1094(result_peg_1089);
            result_peg_1094 = rule_alpha_digit(stream, result_peg_1094.getPosition());
            if (result_peg_1094.error()){
                goto not_peg_1093;
            }
            goto out_peg_1091;
            not_peg_1093:
            result_peg_1089.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1090.getValues();
                result_peg_1089.setValue(value);
            }
        
        
    }
    
    if (column_peg_1088.chunk12 == 0){
        column_peg_1088.chunk12 = new Chunk12();
    }
    column_peg_1088.chunk12->chunk_keyword = result_peg_1089;
    stream.update(result_peg_1089.getPosition());
    
    
    return result_peg_1089;
    out_peg_1091:
    
    if (column_peg_1088.chunk12 == 0){
        column_peg_1088.chunk12 = new Chunk12();
    }
    column_peg_1088.chunk12->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_1096 = stream.getColumn(position);
    if (column_peg_1096.chunk12 != 0 && column_peg_1096.chunk12->chunk_keyword_real.calculated()){
        return column_peg_1096.chunk12->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_1174(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_1097(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1097.getPosition()))){
                    result_peg_1097.nextPosition();
                } else {
                    goto out_peg_1099;
                }
            }
            result_peg_1097.setValue((void*) "vel");
        
        
        
        result_peg_1097 = rule_s(stream, result_peg_1097.getPosition());
            if (result_peg_1097.error()){
                goto out_peg_1099;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1097.getPosition()))){
                    result_peg_1097.nextPosition();
                } else {
                    goto out_peg_1099;
                }
            }
            result_peg_1097.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_1097.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1097;
    stream.update(result_peg_1097.getPosition());
    
    
    return result_peg_1097;
    out_peg_1099:
    Result result_peg_1102(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1102.getPosition()))){
                    result_peg_1102.nextPosition();
                } else {
                    goto out_peg_1104;
                }
            }
            result_peg_1102.setValue((void*) "vel");
        
        
        
        result_peg_1102 = rule_s(stream, result_peg_1102.getPosition());
            if (result_peg_1102.error()){
                goto out_peg_1104;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1102.getPosition()))){
                    result_peg_1102.nextPosition();
                } else {
                    goto out_peg_1104;
                }
            }
            result_peg_1102.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_1102.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1102;
    stream.update(result_peg_1102.getPosition());
    
    
    return result_peg_1102;
    out_peg_1104:
    Result result_peg_1107(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1107.getPosition()))){
                    result_peg_1107.nextPosition();
                } else {
                    goto out_peg_1109;
                }
            }
            result_peg_1107.setValue((void*) "pos");
        
        
        
        result_peg_1107 = rule_s(stream, result_peg_1107.getPosition());
            if (result_peg_1107.error()){
                goto out_peg_1109;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1107.getPosition()))){
                    result_peg_1107.nextPosition();
                } else {
                    goto out_peg_1109;
                }
            }
            result_peg_1107.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_1107.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1107;
    stream.update(result_peg_1107.getPosition());
    
    
    return result_peg_1107;
    out_peg_1109:
    Result result_peg_1112(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1112.getPosition()))){
                    result_peg_1112.nextPosition();
                } else {
                    goto out_peg_1114;
                }
            }
            result_peg_1112.setValue((void*) "pos");
        
        
        
        result_peg_1112 = rule_s(stream, result_peg_1112.getPosition());
            if (result_peg_1112.error()){
                goto out_peg_1114;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1112.getPosition()))){
                    result_peg_1112.nextPosition();
                } else {
                    goto out_peg_1114;
                }
            }
            result_peg_1112.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_1112.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1112;
    stream.update(result_peg_1112.getPosition());
    
    
    return result_peg_1112;
    out_peg_1114:
    Result result_peg_1117(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1117.getPosition()))){
                    result_peg_1117.nextPosition();
                } else {
                    goto out_peg_1119;
                }
            }
            result_peg_1117.setValue((void*) "p2dist");
        
        
        
        result_peg_1117 = rule_s(stream, result_peg_1117.getPosition());
            if (result_peg_1117.error()){
                goto out_peg_1119;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1117.getPosition()))){
                    result_peg_1117.nextPosition();
                } else {
                    goto out_peg_1119;
                }
            }
            result_peg_1117.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_1117.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1117;
    stream.update(result_peg_1117.getPosition());
    
    
    return result_peg_1117;
    out_peg_1119:
    Result result_peg_1122(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1122.getPosition()))){
                    result_peg_1122.nextPosition();
                } else {
                    goto out_peg_1124;
                }
            }
            result_peg_1122.setValue((void*) "p2dist");
        
        
        
        result_peg_1122 = rule_s(stream, result_peg_1122.getPosition());
            if (result_peg_1122.error()){
                goto out_peg_1124;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1122.getPosition()))){
                    result_peg_1122.nextPosition();
                } else {
                    goto out_peg_1124;
                }
            }
            result_peg_1122.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_1122.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1122;
    stream.update(result_peg_1122.getPosition());
    
    
    return result_peg_1122;
    out_peg_1124:
    Result result_peg_1127(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1127.getPosition()))){
                    result_peg_1127.nextPosition();
                } else {
                    goto out_peg_1129;
                }
            }
            result_peg_1127.setValue((void*) "p1dist");
        
        
        
        result_peg_1127 = rule_s(stream, result_peg_1127.getPosition());
            if (result_peg_1127.error()){
                goto out_peg_1129;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1127.getPosition()))){
                    result_peg_1127.nextPosition();
                } else {
                    goto out_peg_1129;
                }
            }
            result_peg_1127.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_1127.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1127;
    stream.update(result_peg_1127.getPosition());
    
    
    return result_peg_1127;
    out_peg_1129:
    Result result_peg_1132(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1132.getPosition()))){
                    result_peg_1132.nextPosition();
                } else {
                    goto out_peg_1134;
                }
            }
            result_peg_1132.setValue((void*) "p1dist");
        
        
        
        result_peg_1132 = rule_s(stream, result_peg_1132.getPosition());
            if (result_peg_1132.error()){
                goto out_peg_1134;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1132.getPosition()))){
                    result_peg_1132.nextPosition();
                } else {
                    goto out_peg_1134;
                }
            }
            result_peg_1132.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_1132.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1132;
    stream.update(result_peg_1132.getPosition());
    
    
    return result_peg_1132;
    out_peg_1134:
    Result result_peg_1137(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1137.getPosition()))){
                    result_peg_1137.nextPosition();
                } else {
                    goto out_peg_1139;
                }
            }
            result_peg_1137.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1137 = rule_s(stream, result_peg_1137.getPosition());
            if (result_peg_1137.error()){
                goto out_peg_1139;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1137.getPosition()))){
                    result_peg_1137.nextPosition();
                } else {
                    goto out_peg_1139;
                }
            }
            result_peg_1137.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1137.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1137;
    stream.update(result_peg_1137.getPosition());
    
    
    return result_peg_1137;
    out_peg_1139:
    Result result_peg_1142(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1142.getPosition()))){
                    result_peg_1142.nextPosition();
                } else {
                    goto out_peg_1144;
                }
            }
            result_peg_1142.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1142 = rule_s(stream, result_peg_1142.getPosition());
            if (result_peg_1142.error()){
                goto out_peg_1144;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1142.getPosition()))){
                    result_peg_1142.nextPosition();
                } else {
                    goto out_peg_1144;
                }
            }
            result_peg_1142.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1142.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1142;
    stream.update(result_peg_1142.getPosition());
    
    
    return result_peg_1142;
    out_peg_1144:
    Result result_peg_1147(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1147.getPosition()))){
                    result_peg_1147.nextPosition();
                } else {
                    goto out_peg_1149;
                }
            }
            result_peg_1147.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1147 = rule_s(stream, result_peg_1147.getPosition());
            if (result_peg_1147.error()){
                goto out_peg_1149;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1147.getPosition()))){
                    result_peg_1147.nextPosition();
                } else {
                    goto out_peg_1149;
                }
            }
            result_peg_1147.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1147.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1147;
    stream.update(result_peg_1147.getPosition());
    
    
    return result_peg_1147;
    out_peg_1149:
    Result result_peg_1152(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1152.getPosition()))){
                    result_peg_1152.nextPosition();
                } else {
                    goto out_peg_1154;
                }
            }
            result_peg_1152.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1152 = rule_s(stream, result_peg_1152.getPosition());
            if (result_peg_1152.error()){
                goto out_peg_1154;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1152.getPosition()))){
                    result_peg_1152.nextPosition();
                } else {
                    goto out_peg_1154;
                }
            }
            result_peg_1152.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1152.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1152;
    stream.update(result_peg_1152.getPosition());
    
    
    return result_peg_1152;
    out_peg_1154:
    Result result_peg_1157(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1157.getPosition()))){
                    result_peg_1157.nextPosition();
                } else {
                    goto out_peg_1159;
                }
            }
            result_peg_1157.setValue((void*) "parentdist");
        
        
        
        result_peg_1157 = rule_s(stream, result_peg_1157.getPosition());
            if (result_peg_1157.error()){
                goto out_peg_1159;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1157.getPosition()))){
                    result_peg_1157.nextPosition();
                } else {
                    goto out_peg_1159;
                }
            }
            result_peg_1157.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1157.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1157;
    stream.update(result_peg_1157.getPosition());
    
    
    return result_peg_1157;
    out_peg_1159:
    Result result_peg_1162(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1162.getPosition()))){
                    result_peg_1162.nextPosition();
                } else {
                    goto out_peg_1164;
                }
            }
            result_peg_1162.setValue((void*) "screenpos");
        
        
        
        result_peg_1162 = rule_s(stream, result_peg_1162.getPosition());
            if (result_peg_1162.error()){
                goto out_peg_1164;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1162.getPosition()))){
                    result_peg_1162.nextPosition();
                } else {
                    goto out_peg_1164;
                }
            }
            result_peg_1162.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_1162.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1162;
    stream.update(result_peg_1162.getPosition());
    
    
    return result_peg_1162;
    out_peg_1164:
    Result result_peg_1167(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1167.getPosition()))){
                    result_peg_1167.nextPosition();
                } else {
                    goto out_peg_1169;
                }
            }
            result_peg_1167.setValue((void*) "screenpos");
        
        
        
        result_peg_1167 = rule_s(stream, result_peg_1167.getPosition());
            if (result_peg_1167.error()){
                goto out_peg_1169;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1167.getPosition()))){
                    result_peg_1167.nextPosition();
                } else {
                    goto out_peg_1169;
                }
            }
            result_peg_1167.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1167.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1167;
    stream.update(result_peg_1167.getPosition());
    
    
    return result_peg_1167;
    out_peg_1169:
    Result result_peg_1172(myposition);
    
    result_peg_1172 = rule_hitflag(stream, result_peg_1172.getPosition());
    if (result_peg_1172.error()){
        goto out_peg_1173;
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = result_peg_1172;
    stream.update(result_peg_1172.getPosition());
    
    
    return result_peg_1172;
    out_peg_1173:
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

const void * main(const std::string & filename, bool stats = false){
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

const void * main(const char * in, bool stats = false){
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



    
    } /* Cmd */
    
} /* Mugen */

        
