

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
    
    
    return result_peg_26;
    out_peg_28:
    
    
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
    
    RuleTrace trace_peg_96(stream, "comment");
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
    
    if (column_peg_72.chunk1 == 0){
        column_peg_72.chunk1 = new Chunk1();
    }
    column_peg_72.chunk1->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_97 = stream.getColumn(position);
    if (column_peg_97.chunk1 != 0 && column_peg_97.chunk1->chunk_section.calculated()){
        return column_peg_97.chunk1->chunk_section;
    }
    
    RuleTrace trace_peg_116(stream, "section");
    int myposition = position;
    
    Value ast;
    Result result_peg_98(myposition);
    
    {
    
        result_peg_98 = rule_section_title(stream, result_peg_98.getPosition());
            if (result_peg_98.error()){
                goto out_peg_100;
            }
        
        Result result_peg_99 = result_peg_98;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_99.getValues());
                result_peg_98.setValue(value);
            }
            ast = result_peg_98.getValues();
        
        
        
        result_peg_98 = rule_whitespace(stream, result_peg_98.getPosition());
            if (result_peg_98.error()){
                goto out_peg_100;
            }
        
        
        
        result_peg_98.reset();
            do{
                Result result_peg_105(result_peg_98.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_105.getPosition()))){
                            result_peg_105.nextPosition();
                        } else {
                            goto out_peg_108;
                        }
                    }
                    result_peg_105.setValue((void*) "\n");
                        
                }
                goto success_peg_106;
                out_peg_108:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_105.getPosition()))){
                            result_peg_105.nextPosition();
                        } else {
                            goto out_peg_110;
                        }
                    }
                    result_peg_105.setValue((void*) "\r");
                        
                }
                goto success_peg_106;
                out_peg_110:
                goto loop_peg_104;
                success_peg_106:
                ;
                result_peg_98.addResult(result_peg_105);
            } while (true);
            loop_peg_104:
            if (result_peg_98.matches() == 0){
                goto out_peg_100;
            }
        
        
        
        result_peg_98.reset();
            do{
                Result result_peg_113(result_peg_98.getPosition());
                {
                
                    result_peg_113 = rule_section_item(stream, result_peg_113.getPosition(), ast);
                        if (result_peg_113.error()){
                            goto loop_peg_112;
                        }
                    
                    
                    
                    result_peg_113 = rule_whitespace(stream, result_peg_113.getPosition());
                        if (result_peg_113.error()){
                            goto loop_peg_112;
                        }
                    
                    
                    
                    result_peg_113 = rule_line_end(stream, result_peg_113.getPosition());
                        if (result_peg_113.error()){
                            goto loop_peg_112;
                        }
                    
                    
                }
                result_peg_98.addResult(result_peg_113);
            } while (true);
            loop_peg_112:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_98.setValue(value);
            }
        
        
    }
    
    if (column_peg_97.chunk1 == 0){
        column_peg_97.chunk1 = new Chunk1();
    }
    column_peg_97.chunk1->chunk_section = result_peg_98;
    stream.update(result_peg_98.getPosition());
    
    
    return result_peg_98;
    out_peg_100:
    
    if (column_peg_97.chunk1 == 0){
        column_peg_97.chunk1 = new Chunk1();
    }
    column_peg_97.chunk1->chunk_section = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    RuleTrace trace_peg_125(stream, "section_item");
    int myposition = position;
    
    Value data;
    Result result_peg_118(myposition);
    
    {
    
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118 = rule_comment(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
    }
    
    
    return result_peg_118;
    out_peg_120:
    Result result_peg_121(myposition);
    
    {
    
        result_peg_121 = rule_s(stream, result_peg_121.getPosition());
            if (result_peg_121.error()){
                goto out_peg_123;
            }
        
        
        
        result_peg_121 = rule_assignment(stream, result_peg_121.getPosition());
            if (result_peg_121.error()){
                goto out_peg_123;
            }
            data = result_peg_121.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_121.setValue(value);
            }
        
        
    }
    
    
    return result_peg_121;
    out_peg_123:
    
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_126 = stream.getColumn(position);
    if (column_peg_126.chunk1 != 0 && column_peg_126.chunk1->chunk_section_title.calculated()){
        return column_peg_126.chunk1->chunk_section_title;
    }
    
    RuleTrace trace_peg_139(stream, "section_title");
    int myposition = position;
    
    Value name;
    Result result_peg_127(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_127.getPosition()))){
                    result_peg_127.nextPosition();
                } else {
                    goto out_peg_129;
                }
            }
            result_peg_127.setValue((void*) "[");
        
        
        
        result_peg_127 = rule_s(stream, result_peg_127.getPosition());
            if (result_peg_127.error()){
                goto out_peg_129;
            }
        
        
        
        result_peg_127.reset();
            do{
                Result result_peg_133(result_peg_127.getPosition());
                {
                
                    Result result_peg_136(result_peg_133);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_136.getPosition()))){
                                result_peg_136.nextPosition();
                            } else {
                                goto not_peg_135;
                            }
                        }
                        result_peg_136.setValue((void*) "]");
                        goto loop_peg_132;
                        not_peg_135:
                        result_peg_133.setValue((void*)0);
                    
                    
                    
                    char temp_peg_137 = stream.get(result_peg_133.getPosition());
                        if (temp_peg_137 != '\0'){
                            result_peg_133.setValue((void*) (long) temp_peg_137);
                            result_peg_133.nextPosition();
                        } else {
                            goto loop_peg_132;
                        }
                    
                    
                }
                result_peg_127.addResult(result_peg_133);
            } while (true);
            loop_peg_132:
            if (result_peg_127.matches() == 0){
                goto out_peg_129;
            }
            name = result_peg_127.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_127.getPosition()))){
                    result_peg_127.nextPosition();
                } else {
                    goto out_peg_129;
                }
            }
            result_peg_127.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(name);
                result_peg_127.setValue(value);
            }
        
        
    }
    
    if (column_peg_126.chunk1 == 0){
        column_peg_126.chunk1 = new Chunk1();
    }
    column_peg_126.chunk1->chunk_section_title = result_peg_127;
    stream.update(result_peg_127.getPosition());
    
    
    return result_peg_127;
    out_peg_129:
    
    if (column_peg_126.chunk1 == 0){
        column_peg_126.chunk1 = new Chunk1();
    }
    column_peg_126.chunk1->chunk_section_title = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_140 = stream.getColumn(position);
    if (column_peg_140.chunk2 != 0 && column_peg_140.chunk2->chunk_assignment.calculated()){
        return column_peg_140.chunk2->chunk_assignment;
    }
    
    RuleTrace trace_peg_176(stream, "assignment");
    int myposition = position;
    
    Value all;
    Value name;
    Value exp;
    Value index;
    Result result_peg_141(myposition);
    
    {
    
        for (int i = 0; i < 7; i++){
                if (compareCharCase("command"[i], stream.get(result_peg_141.getPosition()))){
                    result_peg_141.nextPosition();
                } else {
                    goto out_peg_143;
                }
            }
            result_peg_141.setValue((void*) "command");
        
        
        
        result_peg_141 = rule_s(stream, result_peg_141.getPosition());
            if (result_peg_141.error()){
                goto out_peg_143;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_141.getPosition()))){
                    result_peg_141.nextPosition();
                } else {
                    goto out_peg_143;
                }
            }
            result_peg_141.setValue((void*) "=");
        
        
        
        result_peg_141 = rule_s(stream, result_peg_141.getPosition());
            if (result_peg_141.error()){
                goto out_peg_143;
            }
        
        
        
        result_peg_141 = rule_keys(stream, result_peg_141.getPosition());
            if (result_peg_141.error()){
                goto out_peg_143;
            }
            all = result_peg_141.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("command", all);
                result_peg_141.setValue(value);
            }
        
        
    }
    
    if (column_peg_140.chunk2 == 0){
        column_peg_140.chunk2 = new Chunk2();
    }
    column_peg_140.chunk2->chunk_assignment = result_peg_141;
    stream.update(result_peg_141.getPosition());
    
    
    return result_peg_141;
    out_peg_143:
    Result result_peg_148(myposition);
    
    {
    
        result_peg_148 = rule_identifier(stream, result_peg_148.getPosition());
            if (result_peg_148.error()){
                goto out_peg_150;
            }
            name = result_peg_148.getValues();
        
        
        
        result_peg_148 = rule_s(stream, result_peg_148.getPosition());
            if (result_peg_148.error()){
                goto out_peg_150;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_148.getPosition()))){
                    result_peg_148.nextPosition();
                } else {
                    goto out_peg_150;
                }
            }
            result_peg_148.setValue((void*) "=");
        
        
        
        result_peg_148 = rule_s(stream, result_peg_148.getPosition());
            if (result_peg_148.error()){
                goto out_peg_150;
            }
        
        
        
        result_peg_148 = rule_expr(stream, result_peg_148.getPosition());
            if (result_peg_148.error()){
                goto out_peg_150;
            }
            exp = result_peg_148.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
                result_peg_148.setValue(value);
            }
        
        
    }
    
    if (column_peg_140.chunk2 == 0){
        column_peg_140.chunk2 = new Chunk2();
    }
    column_peg_140.chunk2->chunk_assignment = result_peg_148;
    stream.update(result_peg_148.getPosition());
    
    
    return result_peg_148;
    out_peg_150:
    Result result_peg_155(myposition);
    
    {
    
        result_peg_155 = rule_identifier(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
            name = result_peg_155.getValues();
        
        
        
        result_peg_155 = rule_s(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_155.getPosition()))){
                    result_peg_155.nextPosition();
                } else {
                    goto out_peg_157;
                }
            }
            result_peg_155.setValue((void*) "=");
        
        
        
        result_peg_155 = rule_s(stream, result_peg_155.getPosition());
            if (result_peg_155.error()){
                goto out_peg_157;
            }
        
        
        
        Result result_peg_162(result_peg_155.getPosition());
            result_peg_162 = rule_line_end(stream, result_peg_162.getPosition());
            if (result_peg_162.error()){
                goto out_peg_157;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_155.setValue(value);
            }
        
        
    }
    
    if (column_peg_140.chunk2 == 0){
        column_peg_140.chunk2 = new Chunk2();
    }
    column_peg_140.chunk2->chunk_assignment = result_peg_155;
    stream.update(result_peg_155.getPosition());
    
    
    return result_peg_155;
    out_peg_157:
    Result result_peg_163(myposition);
    
    {
    
        result_peg_163 = rule_identifier(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
            name = result_peg_163.getValues();
        
        
        
        result_peg_163 = rule_s(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_163.getPosition()))){
                    result_peg_163.nextPosition();
                } else {
                    goto out_peg_165;
                }
            }
            result_peg_163.setValue((void*) "(");
        
        
        
        result_peg_163 = rule_s(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
        
        
        
        result_peg_163 = rule_integer(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
            index = result_peg_163.getValues();
        
        
        
        result_peg_163 = rule_s(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_163.getPosition()))){
                    result_peg_163.nextPosition();
                } else {
                    goto out_peg_165;
                }
            }
            result_peg_163.setValue((void*) ")");
        
        
        
        result_peg_163 = rule_s(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_163.getPosition()))){
                    result_peg_163.nextPosition();
                } else {
                    goto out_peg_165;
                }
            }
            result_peg_163.setValue((void*) "=");
        
        
        
        result_peg_163 = rule_s(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
        
        
        
        result_peg_163 = rule_expr(stream, result_peg_163.getPosition());
            if (result_peg_163.error()){
                goto out_peg_165;
            }
            exp = result_peg_163.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_163.setValue(value);
            }
        
        
    }
    
    if (column_peg_140.chunk2 == 0){
        column_peg_140.chunk2 = new Chunk2();
    }
    column_peg_140.chunk2->chunk_assignment = result_peg_163;
    stream.update(result_peg_163.getPosition());
    
    
    return result_peg_163;
    out_peg_165:
    
    if (column_peg_140.chunk2 == 0){
        column_peg_140.chunk2 = new Chunk2();
    }
    column_peg_140.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_177 = stream.getColumn(position);
    if (column_peg_177.chunk2 != 0 && column_peg_177.chunk2->chunk_identifier.calculated()){
        return column_peg_177.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_185(stream, "identifier");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_178(myposition);
    
    {
    
        result_peg_178 = rule_name(stream, result_peg_178.getPosition());
            if (result_peg_178.error()){
                goto out_peg_180;
            }
            first = result_peg_178.getValues();
        
        
        
        result_peg_178.reset();
            do{
                Result result_peg_183(result_peg_178.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_183.getPosition()))){
                                result_peg_183.nextPosition();
                            } else {
                                goto loop_peg_182;
                            }
                        }
                        result_peg_183.setValue((void*) ".");
                    
                    
                    
                    result_peg_183 = rule_name(stream, result_peg_183.getPosition());
                        if (result_peg_183.error()){
                            goto loop_peg_182;
                        }
                    
                    
                }
                result_peg_178.addResult(result_peg_183);
            } while (true);
            loop_peg_182:
            ;
            rest = result_peg_178.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_178.setValue(value);
            }
        
        
    }
    
    if (column_peg_177.chunk2 == 0){
        column_peg_177.chunk2 = new Chunk2();
    }
    column_peg_177.chunk2->chunk_identifier = result_peg_178;
    stream.update(result_peg_178.getPosition());
    
    
    return result_peg_178;
    out_peg_180:
    
    if (column_peg_177.chunk2 == 0){
        column_peg_177.chunk2 = new Chunk2();
    }
    column_peg_177.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_186 = stream.getColumn(position);
    if (column_peg_186.chunk2 != 0 && column_peg_186.chunk2->chunk_integer.calculated()){
        return column_peg_186.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_204(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_187(myposition);
    
    {
    
        int save_peg_189 = result_peg_187.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_187.getPosition()))){
                        result_peg_187.nextPosition();
                    } else {
                        goto out_peg_192;
                    }
                }
                result_peg_187.setValue((void*) "-");
                    
            }
            goto success_peg_190;
            out_peg_192:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_187.getPosition()))){
                        result_peg_187.nextPosition();
                    } else {
                        goto out_peg_194;
                    }
                }
                result_peg_187.setValue((void*) "+");
                    
            }
            goto success_peg_190;
            out_peg_194:
            
            result_peg_187 = Result(save_peg_189);
            result_peg_187.setValue((void*) 0);
            
            success_peg_190:
            ;
        
        Result result_peg_188 = result_peg_187;
        
        result_peg_187.reset();
            do{
                Result result_peg_197(result_peg_187.getPosition());
                {
                    
                    char letter_peg_202 = stream.get(result_peg_197.getPosition());
                    if (letter_peg_202 != '\0' && strchr("0123456789", letter_peg_202) != NULL){
                        result_peg_197.nextPosition();
                        result_peg_197.setValue((void*) (long) letter_peg_202);
                    } else {
                        goto out_peg_201;
                    }
                    
                }
                goto success_peg_198;
                out_peg_201:
                goto loop_peg_196;
                success_peg_198:
                ;
                result_peg_187.addResult(result_peg_197);
            } while (true);
            loop_peg_196:
            if (result_peg_187.matches() == 0){
                goto out_peg_203;
            }
        
        Result result_peg_195 = result_peg_187;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_188.getValues(), result_peg_195.getValues());
                result_peg_187.setValue(value);
            }
        
        
    }
    
    if (column_peg_186.chunk2 == 0){
        column_peg_186.chunk2 = new Chunk2();
    }
    column_peg_186.chunk2->chunk_integer = result_peg_187;
    stream.update(result_peg_187.getPosition());
    
    
    return result_peg_187;
    out_peg_203:
    
    if (column_peg_186.chunk2 == 0){
        column_peg_186.chunk2 = new Chunk2();
    }
    column_peg_186.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_205 = stream.getColumn(position);
    if (column_peg_205.chunk2 != 0 && column_peg_205.chunk2->chunk_float.calculated()){
        return column_peg_205.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_250(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_206(myposition);
    
    {
    
        int save_peg_208 = result_peg_206.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_206.getPosition()))){
                        result_peg_206.nextPosition();
                    } else {
                        goto out_peg_211;
                    }
                }
                result_peg_206.setValue((void*) "-");
                    
            }
            goto success_peg_209;
            out_peg_211:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_206.getPosition()))){
                        result_peg_206.nextPosition();
                    } else {
                        goto out_peg_213;
                    }
                }
                result_peg_206.setValue((void*) "+");
                    
            }
            goto success_peg_209;
            out_peg_213:
            
            result_peg_206 = Result(save_peg_208);
            result_peg_206.setValue((void*) 0);
            
            success_peg_209:
            ;
        
        Result result_peg_207 = result_peg_206;
        
        result_peg_206.reset();
            do{
                Result result_peg_216(result_peg_206.getPosition());
                {
                    
                    char letter_peg_221 = stream.get(result_peg_216.getPosition());
                    if (letter_peg_221 != '\0' && strchr("0123456789", letter_peg_221) != NULL){
                        result_peg_216.nextPosition();
                        result_peg_216.setValue((void*) (long) letter_peg_221);
                    } else {
                        goto out_peg_220;
                    }
                    
                }
                goto success_peg_217;
                out_peg_220:
                goto loop_peg_215;
                success_peg_217:
                ;
                result_peg_206.addResult(result_peg_216);
            } while (true);
            loop_peg_215:
            ;
            left = result_peg_206.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_206.getPosition()))){
                    result_peg_206.nextPosition();
                } else {
                    goto out_peg_223;
                }
            }
            result_peg_206.setValue((void*) ".");
        
        
        
        result_peg_206.reset();
            do{
                Result result_peg_226(result_peg_206.getPosition());
                {
                    
                    char letter_peg_231 = stream.get(result_peg_226.getPosition());
                    if (letter_peg_231 != '\0' && strchr("0123456789", letter_peg_231) != NULL){
                        result_peg_226.nextPosition();
                        result_peg_226.setValue((void*) (long) letter_peg_231);
                    } else {
                        goto out_peg_230;
                    }
                    
                }
                goto success_peg_227;
                out_peg_230:
                goto loop_peg_225;
                success_peg_227:
                ;
                result_peg_206.addResult(result_peg_226);
            } while (true);
            loop_peg_225:
            if (result_peg_206.matches() == 0){
                goto out_peg_223;
            }
            right = result_peg_206.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_207.getValues(), parseDouble(left,right));
                result_peg_206.setValue(value);
            }
        
        
    }
    
    if (column_peg_205.chunk2 == 0){
        column_peg_205.chunk2 = new Chunk2();
    }
    column_peg_205.chunk2->chunk_float = result_peg_206;
    stream.update(result_peg_206.getPosition());
    
    
    return result_peg_206;
    out_peg_223:
    Result result_peg_232(myposition);
    
    {
    
        int save_peg_234 = result_peg_232.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_232.getPosition()))){
                        result_peg_232.nextPosition();
                    } else {
                        goto out_peg_237;
                    }
                }
                result_peg_232.setValue((void*) "-");
                    
            }
            goto success_peg_235;
            out_peg_237:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_232.getPosition()))){
                        result_peg_232.nextPosition();
                    } else {
                        goto out_peg_239;
                    }
                }
                result_peg_232.setValue((void*) "+");
                    
            }
            goto success_peg_235;
            out_peg_239:
            
            result_peg_232 = Result(save_peg_234);
            result_peg_232.setValue((void*) 0);
            
            success_peg_235:
            ;
        
        Result result_peg_233 = result_peg_232;
        
        result_peg_232.reset();
            do{
                Result result_peg_242(result_peg_232.getPosition());
                {
                    
                    char letter_peg_247 = stream.get(result_peg_242.getPosition());
                    if (letter_peg_247 != '\0' && strchr("0123456789", letter_peg_247) != NULL){
                        result_peg_242.nextPosition();
                        result_peg_242.setValue((void*) (long) letter_peg_247);
                    } else {
                        goto out_peg_246;
                    }
                    
                }
                goto success_peg_243;
                out_peg_246:
                goto loop_peg_241;
                success_peg_243:
                ;
                result_peg_232.addResult(result_peg_242);
            } while (true);
            loop_peg_241:
            if (result_peg_232.matches() == 0){
                goto out_peg_248;
            }
            left = result_peg_232.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_232.getPosition()))){
                    result_peg_232.nextPosition();
                } else {
                    goto out_peg_248;
                }
            }
            result_peg_232.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_233.getValues(), parseDouble(left));
                result_peg_232.setValue(value);
            }
        
        
    }
    
    if (column_peg_205.chunk2 == 0){
        column_peg_205.chunk2 = new Chunk2();
    }
    column_peg_205.chunk2->chunk_float = result_peg_232;
    stream.update(result_peg_232.getPosition());
    
    
    return result_peg_232;
    out_peg_248:
    
    if (column_peg_205.chunk2 == 0){
        column_peg_205.chunk2 = new Chunk2();
    }
    column_peg_205.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_251 = stream.getColumn(position);
    if (column_peg_251.chunk2 != 0 && column_peg_251.chunk2->chunk_string.calculated()){
        return column_peg_251.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_266(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_252(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_252.getPosition()))){
                    result_peg_252.nextPosition();
                } else {
                    goto out_peg_254;
                }
            }
            result_peg_252.setValue((void*) "\"");
        
        
        
        result_peg_252.reset();
            do{
                Result result_peg_257(result_peg_252.getPosition());
                {
                
                    Result result_peg_260(result_peg_257);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_260.getPosition()))){
                                result_peg_260.nextPosition();
                            } else {
                                goto not_peg_259;
                            }
                        }
                        result_peg_260.setValue((void*) "\"");
                        goto loop_peg_256;
                        not_peg_259:
                        result_peg_257.setValue((void*)0);
                    
                    
                    
                    Result result_peg_263(result_peg_257);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_263.getPosition()))){
                                result_peg_263.nextPosition();
                            } else {
                                goto not_peg_262;
                            }
                        }
                        result_peg_263.setValue((void*) "\n");
                        goto loop_peg_256;
                        not_peg_262:
                        result_peg_257.setValue((void*)0);
                    
                    
                    
                    char temp_peg_264 = stream.get(result_peg_257.getPosition());
                        if (temp_peg_264 != '\0'){
                            result_peg_257.setValue((void*) (long) temp_peg_264);
                            result_peg_257.nextPosition();
                        } else {
                            goto loop_peg_256;
                        }
                    
                    
                }
                result_peg_252.addResult(result_peg_257);
            } while (true);
            loop_peg_256:
            ;
            contents = result_peg_252.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_252.getPosition()))){
                    result_peg_252.nextPosition();
                } else {
                    goto out_peg_254;
                }
            }
            result_peg_252.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_252.setValue(value);
            }
        
        
    }
    
    if (column_peg_251.chunk2 == 0){
        column_peg_251.chunk2 = new Chunk2();
    }
    column_peg_251.chunk2->chunk_string = result_peg_252;
    stream.update(result_peg_252.getPosition());
    
    
    return result_peg_252;
    out_peg_254:
    
    if (column_peg_251.chunk2 == 0){
        column_peg_251.chunk2 = new Chunk2();
    }
    column_peg_251.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_267 = stream.getColumn(position);
    if (column_peg_267.chunk3 != 0 && column_peg_267.chunk3->chunk_range.calculated()){
        return column_peg_267.chunk3->chunk_range;
    }
    
    RuleTrace trace_peg_312(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_268(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_268.getPosition()))){
                    result_peg_268.nextPosition();
                } else {
                    goto out_peg_270;
                }
            }
            result_peg_268.setValue((void*) "[");
        
        
        
        result_peg_268 = rule_s(stream, result_peg_268.getPosition());
            if (result_peg_268.error()){
                goto out_peg_270;
            }
        
        
        
        result_peg_268 = rule_value(stream, result_peg_268.getPosition());
            if (result_peg_268.error()){
                goto out_peg_270;
            }
            low = result_peg_268.getValues();
        
        
        
        result_peg_268 = rule_s(stream, result_peg_268.getPosition());
            if (result_peg_268.error()){
                goto out_peg_270;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_268.getPosition()))){
                    result_peg_268.nextPosition();
                } else {
                    goto out_peg_270;
                }
            }
            result_peg_268.setValue((void*) ",");
        
        
        
        result_peg_268 = rule_s(stream, result_peg_268.getPosition());
            if (result_peg_268.error()){
                goto out_peg_270;
            }
        
        
        
        result_peg_268 = rule_value(stream, result_peg_268.getPosition());
            if (result_peg_268.error()){
                goto out_peg_270;
            }
            high = result_peg_268.getValues();
        
        
        
        result_peg_268 = rule_s(stream, result_peg_268.getPosition());
            if (result_peg_268.error()){
                goto out_peg_270;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_268.getPosition()))){
                    result_peg_268.nextPosition();
                } else {
                    goto out_peg_270;
                }
            }
            result_peg_268.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_268.setValue(value);
            }
        
        
    }
    
    if (column_peg_267.chunk3 == 0){
        column_peg_267.chunk3 = new Chunk3();
    }
    column_peg_267.chunk3->chunk_range = result_peg_268;
    stream.update(result_peg_268.getPosition());
    
    
    return result_peg_268;
    out_peg_270:
    Result result_peg_279(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_281;
                }
            }
            result_peg_279.setValue((void*) "(");
        
        
        
        result_peg_279 = rule_s(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_281;
            }
        
        
        
        result_peg_279 = rule_value(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_281;
            }
            low = result_peg_279.getValues();
        
        
        
        result_peg_279 = rule_s(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_281;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_281;
                }
            }
            result_peg_279.setValue((void*) ",");
        
        
        
        result_peg_279 = rule_s(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_281;
            }
        
        
        
        result_peg_279 = rule_value(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_281;
            }
            high = result_peg_279.getValues();
        
        
        
        result_peg_279 = rule_s(stream, result_peg_279.getPosition());
            if (result_peg_279.error()){
                goto out_peg_281;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_279.getPosition()))){
                    result_peg_279.nextPosition();
                } else {
                    goto out_peg_281;
                }
            }
            result_peg_279.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_279.setValue(value);
            }
        
        
    }
    
    if (column_peg_267.chunk3 == 0){
        column_peg_267.chunk3 = new Chunk3();
    }
    column_peg_267.chunk3->chunk_range = result_peg_279;
    stream.update(result_peg_279.getPosition());
    
    
    return result_peg_279;
    out_peg_281:
    Result result_peg_290(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_290.getPosition()))){
                    result_peg_290.nextPosition();
                } else {
                    goto out_peg_292;
                }
            }
            result_peg_290.setValue((void*) "(");
        
        
        
        result_peg_290 = rule_s(stream, result_peg_290.getPosition());
            if (result_peg_290.error()){
                goto out_peg_292;
            }
        
        
        
        result_peg_290 = rule_value(stream, result_peg_290.getPosition());
            if (result_peg_290.error()){
                goto out_peg_292;
            }
            low = result_peg_290.getValues();
        
        
        
        result_peg_290 = rule_s(stream, result_peg_290.getPosition());
            if (result_peg_290.error()){
                goto out_peg_292;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_290.getPosition()))){
                    result_peg_290.nextPosition();
                } else {
                    goto out_peg_292;
                }
            }
            result_peg_290.setValue((void*) ",");
        
        
        
        result_peg_290 = rule_s(stream, result_peg_290.getPosition());
            if (result_peg_290.error()){
                goto out_peg_292;
            }
        
        
        
        result_peg_290 = rule_value(stream, result_peg_290.getPosition());
            if (result_peg_290.error()){
                goto out_peg_292;
            }
            high = result_peg_290.getValues();
        
        
        
        result_peg_290 = rule_s(stream, result_peg_290.getPosition());
            if (result_peg_290.error()){
                goto out_peg_292;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_290.getPosition()))){
                    result_peg_290.nextPosition();
                } else {
                    goto out_peg_292;
                }
            }
            result_peg_290.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_290.setValue(value);
            }
        
        
    }
    
    if (column_peg_267.chunk3 == 0){
        column_peg_267.chunk3 = new Chunk3();
    }
    column_peg_267.chunk3->chunk_range = result_peg_290;
    stream.update(result_peg_290.getPosition());
    
    
    return result_peg_290;
    out_peg_292:
    Result result_peg_301(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_301.getPosition()))){
                    result_peg_301.nextPosition();
                } else {
                    goto out_peg_303;
                }
            }
            result_peg_301.setValue((void*) "[");
        
        
        
        result_peg_301 = rule_s(stream, result_peg_301.getPosition());
            if (result_peg_301.error()){
                goto out_peg_303;
            }
        
        
        
        result_peg_301 = rule_value(stream, result_peg_301.getPosition());
            if (result_peg_301.error()){
                goto out_peg_303;
            }
            low = result_peg_301.getValues();
        
        
        
        result_peg_301 = rule_s(stream, result_peg_301.getPosition());
            if (result_peg_301.error()){
                goto out_peg_303;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_301.getPosition()))){
                    result_peg_301.nextPosition();
                } else {
                    goto out_peg_303;
                }
            }
            result_peg_301.setValue((void*) ",");
        
        
        
        result_peg_301 = rule_s(stream, result_peg_301.getPosition());
            if (result_peg_301.error()){
                goto out_peg_303;
            }
        
        
        
        result_peg_301 = rule_value(stream, result_peg_301.getPosition());
            if (result_peg_301.error()){
                goto out_peg_303;
            }
            high = result_peg_301.getValues();
        
        
        
        result_peg_301 = rule_s(stream, result_peg_301.getPosition());
            if (result_peg_301.error()){
                goto out_peg_303;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_301.getPosition()))){
                    result_peg_301.nextPosition();
                } else {
                    goto out_peg_303;
                }
            }
            result_peg_301.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_301.setValue(value);
            }
        
        
    }
    
    if (column_peg_267.chunk3 == 0){
        column_peg_267.chunk3 = new Chunk3();
    }
    column_peg_267.chunk3->chunk_range = result_peg_301;
    stream.update(result_peg_301.getPosition());
    
    
    return result_peg_301;
    out_peg_303:
    
    if (column_peg_267.chunk3 == 0){
        column_peg_267.chunk3 = new Chunk3();
    }
    column_peg_267.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_313 = stream.getColumn(position);
    if (column_peg_313.chunk3 != 0 && column_peg_313.chunk3->chunk_name.calculated()){
        return column_peg_313.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_325(stream, "name");
    int myposition = position;
    
    
    Result result_peg_314(myposition);
    
    {
    
        {
                
                char letter_peg_320 = stream.get(result_peg_314.getPosition());
                if (letter_peg_320 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_320) != NULL){
                    result_peg_314.nextPosition();
                    result_peg_314.setValue((void*) (long) letter_peg_320);
                } else {
                    goto out_peg_319;
                }
                
            }
            goto success_peg_316;
            out_peg_319:
            goto out_peg_321;
            success_peg_316:
            ;
        
        Result result_peg_315 = result_peg_314;
        
        result_peg_314.reset();
            do{
                Result result_peg_324(result_peg_314.getPosition());
                result_peg_324 = rule_alpha_digit(stream, result_peg_324.getPosition());
                if (result_peg_324.error()){
                    goto loop_peg_323;
                }
                result_peg_314.addResult(result_peg_324);
            } while (true);
            loop_peg_323:
            ;
        
        Result result_peg_322 = result_peg_314;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_315.getValues().getValue(),result_peg_322.getValues());
                result_peg_314.setValue(value);
            }
        
        
    }
    
    if (column_peg_313.chunk3 == 0){
        column_peg_313.chunk3 = new Chunk3();
    }
    column_peg_313.chunk3->chunk_name = result_peg_314;
    stream.update(result_peg_314.getPosition());
    
    
    return result_peg_314;
    out_peg_321:
    
    if (column_peg_313.chunk3 == 0){
        column_peg_313.chunk3 = new Chunk3();
    }
    column_peg_313.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_326 = stream.getColumn(position);
    if (column_peg_326.chunk3 != 0 && column_peg_326.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_326.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_341(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_327(myposition);
    
    {
        
        char letter_peg_332 = stream.get(result_peg_327.getPosition());
        if (letter_peg_332 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_332) != NULL){
            result_peg_327.nextPosition();
            result_peg_327.setValue((void*) (long) letter_peg_332);
        } else {
            goto out_peg_331;
        }
        
    }
    goto success_peg_328;
    out_peg_331:
    goto out_peg_333;
    success_peg_328:
    ;
    
    if (column_peg_326.chunk3 == 0){
        column_peg_326.chunk3 = new Chunk3();
    }
    column_peg_326.chunk3->chunk_alpha_digit = result_peg_327;
    stream.update(result_peg_327.getPosition());
    
    
    return result_peg_327;
    out_peg_333:
    Result result_peg_334(myposition);
    
    {
        
        char letter_peg_339 = stream.get(result_peg_334.getPosition());
        if (letter_peg_339 != '\0' && strchr("0123456789", letter_peg_339) != NULL){
            result_peg_334.nextPosition();
            result_peg_334.setValue((void*) (long) letter_peg_339);
        } else {
            goto out_peg_338;
        }
        
    }
    goto success_peg_335;
    out_peg_338:
    goto out_peg_340;
    success_peg_335:
    ;
    
    if (column_peg_326.chunk3 == 0){
        column_peg_326.chunk3 = new Chunk3();
    }
    column_peg_326.chunk3->chunk_alpha_digit = result_peg_334;
    stream.update(result_peg_334.getPosition());
    
    
    return result_peg_334;
    out_peg_340:
    
    if (column_peg_326.chunk3 == 0){
        column_peg_326.chunk3 = new Chunk3();
    }
    column_peg_326.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_342 = stream.getColumn(position);
    if (column_peg_342.chunk3 != 0 && column_peg_342.chunk3->chunk_valuelist.calculated()){
        return column_peg_342.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_364(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_343(myposition);
    
    {
    
        result_peg_343 = rule_expr_c(stream, result_peg_343.getPosition());
            if (result_peg_343.error()){
                goto out_peg_345;
            }
            first = result_peg_343.getValues();
        
        
        
        result_peg_343.reset();
            do{
                Result result_peg_348(result_peg_343.getPosition());
                {
                
                    result_peg_348 = rule_s(stream, result_peg_348.getPosition());
                        if (result_peg_348.error()){
                            goto loop_peg_347;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_348.getPosition()))){
                                result_peg_348.nextPosition();
                            } else {
                                goto loop_peg_347;
                            }
                        }
                        result_peg_348.setValue((void*) ",");
                    
                    
                    
                    result_peg_348 = rule_s(stream, result_peg_348.getPosition());
                        if (result_peg_348.error()){
                            goto loop_peg_347;
                        }
                    
                    
                    
                    result_peg_348 = rule_expr_c(stream, result_peg_348.getPosition());
                        if (result_peg_348.error()){
                            goto loop_peg_347;
                        }
                    
                    
                }
                result_peg_343.addResult(result_peg_348);
            } while (true);
            loop_peg_347:
            if (result_peg_343.matches() == 0){
                goto out_peg_345;
            }
            rest = result_peg_343.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_343.setValue(value);
            }
        
        
    }
    
    if (column_peg_342.chunk3 == 0){
        column_peg_342.chunk3 = new Chunk3();
    }
    column_peg_342.chunk3->chunk_valuelist = result_peg_343;
    stream.update(result_peg_343.getPosition());
    
    
    return result_peg_343;
    out_peg_345:
    Result result_peg_352(myposition);
    
    {
    
        result_peg_352 = rule_s(stream, result_peg_352.getPosition());
            if (result_peg_352.error()){
                goto out_peg_354;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_352.getPosition()))){
                    result_peg_352.nextPosition();
                } else {
                    goto out_peg_354;
                }
            }
            result_peg_352.setValue((void*) ",");
        
        
        
        result_peg_352 = rule_s(stream, result_peg_352.getPosition());
            if (result_peg_352.error()){
                goto out_peg_354;
            }
        
        
        
        result_peg_352 = rule_expr_c(stream, result_peg_352.getPosition());
            if (result_peg_352.error()){
                goto out_peg_354;
            }
            first = result_peg_352.getValues();
        
        
        
        result_peg_352.reset();
            do{
                Result result_peg_360(result_peg_352.getPosition());
                {
                
                    result_peg_360 = rule_s(stream, result_peg_360.getPosition());
                        if (result_peg_360.error()){
                            goto loop_peg_359;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_360.getPosition()))){
                                result_peg_360.nextPosition();
                            } else {
                                goto loop_peg_359;
                            }
                        }
                        result_peg_360.setValue((void*) ",");
                    
                    
                    
                    result_peg_360 = rule_s(stream, result_peg_360.getPosition());
                        if (result_peg_360.error()){
                            goto loop_peg_359;
                        }
                    
                    
                    
                    result_peg_360 = rule_expr_c(stream, result_peg_360.getPosition());
                        if (result_peg_360.error()){
                            goto loop_peg_359;
                        }
                    
                    
                }
                result_peg_352.addResult(result_peg_360);
            } while (true);
            loop_peg_359:
            ;
            rest = result_peg_352.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_352.setValue(value);
            }
        
        
    }
    
    if (column_peg_342.chunk3 == 0){
        column_peg_342.chunk3 = new Chunk3();
    }
    column_peg_342.chunk3->chunk_valuelist = result_peg_352;
    stream.update(result_peg_352.getPosition());
    
    
    return result_peg_352;
    out_peg_354:
    
    if (column_peg_342.chunk3 == 0){
        column_peg_342.chunk3 = new Chunk3();
    }
    column_peg_342.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_365 = stream.getColumn(position);
    if (column_peg_365.chunk3 != 0 && column_peg_365.chunk3->chunk_expr.calculated()){
        return column_peg_365.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_376(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_366(myposition);
    
    {
    
        result_peg_366 = rule_expr_c(stream, result_peg_366.getPosition());
            if (result_peg_366.error()){
                goto out_peg_368;
            }
        
        Result result_peg_367 = result_peg_366;
        
        Result result_peg_371(result_peg_366);
            {
            
                result_peg_371 = rule_s(stream, result_peg_371.getPosition());
                    if (result_peg_371.error()){
                        goto not_peg_370;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_371.getPosition()))){
                            result_peg_371.nextPosition();
                        } else {
                            goto not_peg_370;
                        }
                    }
                    result_peg_371.setValue((void*) ",");
                
                
            }
            goto out_peg_368;
            not_peg_370:
            result_peg_366.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_367.getValues();
                result_peg_366.setValue(value);
            }
        
        
    }
    
    if (column_peg_365.chunk3 == 0){
        column_peg_365.chunk3 = new Chunk3();
    }
    column_peg_365.chunk3->chunk_expr = result_peg_366;
    stream.update(result_peg_366.getPosition());
    
    
    return result_peg_366;
    out_peg_368:
    Result result_peg_373(myposition);
    
    {
    
        result_peg_373 = rule_valuelist(stream, result_peg_373.getPosition());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
        
        Result result_peg_374 = result_peg_373;
        
        result_peg_373 = rule_expr2_rest(stream, result_peg_373.getPosition(), result_peg_374.getValues());
            if (result_peg_373.error()){
                goto out_peg_375;
            }
        
        
    }
    
    if (column_peg_365.chunk3 == 0){
        column_peg_365.chunk3 = new Chunk3();
    }
    column_peg_365.chunk3->chunk_expr = result_peg_373;
    stream.update(result_peg_373.getPosition());
    
    
    return result_peg_373;
    out_peg_375:
    
    if (column_peg_365.chunk3 == 0){
        column_peg_365.chunk3 = new Chunk3();
    }
    column_peg_365.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_377 = stream.getColumn(position);
    if (column_peg_377.chunk4 != 0 && column_peg_377.chunk4->chunk_expr_c.calculated()){
        return column_peg_377.chunk4->chunk_expr_c;
    }
    
    RuleTrace trace_peg_381(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_378(myposition);
    
    {
    
        result_peg_378 = rule_expr2(stream, result_peg_378.getPosition());
            if (result_peg_378.error()){
                goto out_peg_380;
            }
            left = result_peg_378.getValues();
        
        
        
        result_peg_378 = rule_expr_rest(stream, result_peg_378.getPosition(), left);
            if (result_peg_378.error()){
                goto out_peg_380;
            }
        
        
    }
    
    if (column_peg_377.chunk4 == 0){
        column_peg_377.chunk4 = new Chunk4();
    }
    column_peg_377.chunk4->chunk_expr_c = result_peg_378;
    stream.update(result_peg_378.getPosition());
    
    
    return result_peg_378;
    out_peg_380:
    
    if (column_peg_377.chunk4 == 0){
        column_peg_377.chunk4 = new Chunk4();
    }
    column_peg_377.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_382 = stream.getColumn(position);
    if (column_peg_382.chunk4 != 0 && column_peg_382.chunk4->chunk_expr2.calculated()){
        return column_peg_382.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_386(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_383(myposition);
    
    {
    
        result_peg_383 = rule_expr3(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_385;
            }
            left = result_peg_383.getValues();
        
        
        
        result_peg_383 = rule_expr2_rest(stream, result_peg_383.getPosition(), left);
            if (result_peg_383.error()){
                goto out_peg_385;
            }
        
        
    }
    
    if (column_peg_382.chunk4 == 0){
        column_peg_382.chunk4 = new Chunk4();
    }
    column_peg_382.chunk4->chunk_expr2 = result_peg_383;
    stream.update(result_peg_383.getPosition());
    
    
    return result_peg_383;
    out_peg_385:
    
    if (column_peg_382.chunk4 == 0){
        column_peg_382.chunk4 = new Chunk4();
    }
    column_peg_382.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_387 = stream.getColumn(position);
    if (column_peg_387.chunk4 != 0 && column_peg_387.chunk4->chunk_expr3.calculated()){
        return column_peg_387.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_391(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_388(myposition);
    
    {
    
        result_peg_388 = rule_expr4(stream, result_peg_388.getPosition());
            if (result_peg_388.error()){
                goto out_peg_390;
            }
            left = result_peg_388.getValues();
        
        
        
        result_peg_388 = rule_expr3_rest(stream, result_peg_388.getPosition(), left);
            if (result_peg_388.error()){
                goto out_peg_390;
            }
        
        
    }
    
    if (column_peg_387.chunk4 == 0){
        column_peg_387.chunk4 = new Chunk4();
    }
    column_peg_387.chunk4->chunk_expr3 = result_peg_388;
    stream.update(result_peg_388.getPosition());
    
    
    return result_peg_388;
    out_peg_390:
    
    if (column_peg_387.chunk4 == 0){
        column_peg_387.chunk4 = new Chunk4();
    }
    column_peg_387.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_392 = stream.getColumn(position);
    if (column_peg_392.chunk4 != 0 && column_peg_392.chunk4->chunk_expr4.calculated()){
        return column_peg_392.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_396(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_393(myposition);
    
    {
    
        result_peg_393 = rule_expr5(stream, result_peg_393.getPosition());
            if (result_peg_393.error()){
                goto out_peg_395;
            }
            left = result_peg_393.getValues();
        
        
        
        result_peg_393 = rule_expr4_rest(stream, result_peg_393.getPosition(), left);
            if (result_peg_393.error()){
                goto out_peg_395;
            }
        
        
    }
    
    if (column_peg_392.chunk4 == 0){
        column_peg_392.chunk4 = new Chunk4();
    }
    column_peg_392.chunk4->chunk_expr4 = result_peg_393;
    stream.update(result_peg_393.getPosition());
    
    
    return result_peg_393;
    out_peg_395:
    
    if (column_peg_392.chunk4 == 0){
        column_peg_392.chunk4 = new Chunk4();
    }
    column_peg_392.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_397 = stream.getColumn(position);
    if (column_peg_397.chunk4 != 0 && column_peg_397.chunk4->chunk_expr5.calculated()){
        return column_peg_397.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_401(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_398(myposition);
    
    {
    
        result_peg_398 = rule_expr6(stream, result_peg_398.getPosition());
            if (result_peg_398.error()){
                goto out_peg_400;
            }
            left = result_peg_398.getValues();
        
        
        
        result_peg_398 = rule_expr5_rest(stream, result_peg_398.getPosition(), left);
            if (result_peg_398.error()){
                goto out_peg_400;
            }
        
        
    }
    
    if (column_peg_397.chunk4 == 0){
        column_peg_397.chunk4 = new Chunk4();
    }
    column_peg_397.chunk4->chunk_expr5 = result_peg_398;
    stream.update(result_peg_398.getPosition());
    
    
    return result_peg_398;
    out_peg_400:
    
    if (column_peg_397.chunk4 == 0){
        column_peg_397.chunk4 = new Chunk4();
    }
    column_peg_397.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_402 = stream.getColumn(position);
    if (column_peg_402.chunk5 != 0 && column_peg_402.chunk5->chunk_expr6.calculated()){
        return column_peg_402.chunk5->chunk_expr6;
    }
    
    RuleTrace trace_peg_406(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_403(myposition);
    
    {
    
        result_peg_403 = rule_expr7(stream, result_peg_403.getPosition());
            if (result_peg_403.error()){
                goto out_peg_405;
            }
            left = result_peg_403.getValues();
        
        
        
        result_peg_403 = rule_expr6_rest(stream, result_peg_403.getPosition(), left);
            if (result_peg_403.error()){
                goto out_peg_405;
            }
        
        
    }
    
    if (column_peg_402.chunk5 == 0){
        column_peg_402.chunk5 = new Chunk5();
    }
    column_peg_402.chunk5->chunk_expr6 = result_peg_403;
    stream.update(result_peg_403.getPosition());
    
    
    return result_peg_403;
    out_peg_405:
    
    if (column_peg_402.chunk5 == 0){
        column_peg_402.chunk5 = new Chunk5();
    }
    column_peg_402.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_407 = stream.getColumn(position);
    if (column_peg_407.chunk5 != 0 && column_peg_407.chunk5->chunk_expr7.calculated()){
        return column_peg_407.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_411(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_408(myposition);
    
    {
    
        result_peg_408 = rule_expr8(stream, result_peg_408.getPosition());
            if (result_peg_408.error()){
                goto out_peg_410;
            }
            left = result_peg_408.getValues();
        
        
        
        result_peg_408 = rule_expr7_rest(stream, result_peg_408.getPosition(), left);
            if (result_peg_408.error()){
                goto out_peg_410;
            }
        
        
    }
    
    if (column_peg_407.chunk5 == 0){
        column_peg_407.chunk5 = new Chunk5();
    }
    column_peg_407.chunk5->chunk_expr7 = result_peg_408;
    stream.update(result_peg_408.getPosition());
    
    
    return result_peg_408;
    out_peg_410:
    
    if (column_peg_407.chunk5 == 0){
        column_peg_407.chunk5 = new Chunk5();
    }
    column_peg_407.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_412 = stream.getColumn(position);
    if (column_peg_412.chunk5 != 0 && column_peg_412.chunk5->chunk_expr8.calculated()){
        return column_peg_412.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_416(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_413(myposition);
    
    {
    
        result_peg_413 = rule_expr9(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_415;
            }
            left = result_peg_413.getValues();
        
        
        
        result_peg_413 = rule_expr8_rest(stream, result_peg_413.getPosition(), left);
            if (result_peg_413.error()){
                goto out_peg_415;
            }
        
        
    }
    
    if (column_peg_412.chunk5 == 0){
        column_peg_412.chunk5 = new Chunk5();
    }
    column_peg_412.chunk5->chunk_expr8 = result_peg_413;
    stream.update(result_peg_413.getPosition());
    
    
    return result_peg_413;
    out_peg_415:
    
    if (column_peg_412.chunk5 == 0){
        column_peg_412.chunk5 = new Chunk5();
    }
    column_peg_412.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_417 = stream.getColumn(position);
    if (column_peg_417.chunk5 != 0 && column_peg_417.chunk5->chunk_expr9.calculated()){
        return column_peg_417.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_421(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_418(myposition);
    
    {
    
        result_peg_418 = rule_expr10(stream, result_peg_418.getPosition());
            if (result_peg_418.error()){
                goto out_peg_420;
            }
            left = result_peg_418.getValues();
        
        
        
        result_peg_418 = rule_expr9_rest(stream, result_peg_418.getPosition(), left);
            if (result_peg_418.error()){
                goto out_peg_420;
            }
        
        
    }
    
    if (column_peg_417.chunk5 == 0){
        column_peg_417.chunk5 = new Chunk5();
    }
    column_peg_417.chunk5->chunk_expr9 = result_peg_418;
    stream.update(result_peg_418.getPosition());
    
    
    return result_peg_418;
    out_peg_420:
    
    if (column_peg_417.chunk5 == 0){
        column_peg_417.chunk5 = new Chunk5();
    }
    column_peg_417.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_422 = stream.getColumn(position);
    if (column_peg_422.chunk5 != 0 && column_peg_422.chunk5->chunk_expr10.calculated()){
        return column_peg_422.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_426(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_423(myposition);
    
    {
    
        result_peg_423 = rule_expr11(stream, result_peg_423.getPosition());
            if (result_peg_423.error()){
                goto out_peg_425;
            }
            left = result_peg_423.getValues();
        
        
        
        result_peg_423 = rule_expr10_rest(stream, result_peg_423.getPosition(), left);
            if (result_peg_423.error()){
                goto out_peg_425;
            }
        
        
    }
    
    if (column_peg_422.chunk5 == 0){
        column_peg_422.chunk5 = new Chunk5();
    }
    column_peg_422.chunk5->chunk_expr10 = result_peg_423;
    stream.update(result_peg_423.getPosition());
    
    
    return result_peg_423;
    out_peg_425:
    
    if (column_peg_422.chunk5 == 0){
        column_peg_422.chunk5 = new Chunk5();
    }
    column_peg_422.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_427 = stream.getColumn(position);
    if (column_peg_427.chunk6 != 0 && column_peg_427.chunk6->chunk_expr11.calculated()){
        return column_peg_427.chunk6->chunk_expr11;
    }
    
    RuleTrace trace_peg_431(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_428(myposition);
    
    {
    
        result_peg_428 = rule_expr12(stream, result_peg_428.getPosition());
            if (result_peg_428.error()){
                goto out_peg_430;
            }
            left = result_peg_428.getValues();
        
        
        
        result_peg_428 = rule_expr11_rest(stream, result_peg_428.getPosition(), left);
            if (result_peg_428.error()){
                goto out_peg_430;
            }
        
        
    }
    
    if (column_peg_427.chunk6 == 0){
        column_peg_427.chunk6 = new Chunk6();
    }
    column_peg_427.chunk6->chunk_expr11 = result_peg_428;
    stream.update(result_peg_428.getPosition());
    
    
    return result_peg_428;
    out_peg_430:
    
    if (column_peg_427.chunk6 == 0){
        column_peg_427.chunk6 = new Chunk6();
    }
    column_peg_427.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_432 = stream.getColumn(position);
    if (column_peg_432.chunk6 != 0 && column_peg_432.chunk6->chunk_expr12.calculated()){
        return column_peg_432.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_436(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_433(myposition);
    
    {
    
        result_peg_433 = rule_expr13(stream, result_peg_433.getPosition());
            if (result_peg_433.error()){
                goto out_peg_435;
            }
            left = result_peg_433.getValues();
        
        
        
        result_peg_433 = rule_expr12_rest(stream, result_peg_433.getPosition(), left);
            if (result_peg_433.error()){
                goto out_peg_435;
            }
        
        
    }
    
    if (column_peg_432.chunk6 == 0){
        column_peg_432.chunk6 = new Chunk6();
    }
    column_peg_432.chunk6->chunk_expr12 = result_peg_433;
    stream.update(result_peg_433.getPosition());
    
    
    return result_peg_433;
    out_peg_435:
    
    if (column_peg_432.chunk6 == 0){
        column_peg_432.chunk6 = new Chunk6();
    }
    column_peg_432.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_437 = stream.getColumn(position);
    if (column_peg_437.chunk6 != 0 && column_peg_437.chunk6->chunk_expr13.calculated()){
        return column_peg_437.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_444(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_438(myposition);
    
    {
    
        result_peg_438.reset();
            do{
                Result result_peg_441(result_peg_438.getPosition());
                result_peg_441 = rule_unary(stream, result_peg_441.getPosition());
                if (result_peg_441.error()){
                    goto loop_peg_440;
                }
                result_peg_438.addResult(result_peg_441);
            } while (true);
            loop_peg_440:
            ;
        
        Result result_peg_439 = result_peg_438;
        
        result_peg_438 = rule_expr13_real(stream, result_peg_438.getPosition());
            if (result_peg_438.error()){
                goto out_peg_443;
            }
            exp = result_peg_438.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_439.getValues(), exp);
                result_peg_438.setValue(value);
            }
        
        
    }
    
    if (column_peg_437.chunk6 == 0){
        column_peg_437.chunk6 = new Chunk6();
    }
    column_peg_437.chunk6->chunk_expr13 = result_peg_438;
    stream.update(result_peg_438.getPosition());
    
    
    return result_peg_438;
    out_peg_443:
    
    if (column_peg_437.chunk6 == 0){
        column_peg_437.chunk6 = new Chunk6();
    }
    column_peg_437.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_456(stream, "expr_rest");
    int myposition = position;
    tail_peg_447:
    Value right;
    Value new_left;
    Result result_peg_446(myposition);
    {
    
        result_peg_446 = rule_s(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_446.getPosition()))){
                    result_peg_446.nextPosition();
                } else {
                    goto out_peg_449;
                }
            }
            result_peg_446.setValue((void*) "||");
        
        
        
        result_peg_446 = rule_s(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
        
        
        
        result_peg_446 = rule_expr2(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
            right = result_peg_446.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_446.setValue(value);
            }
            new_left = result_peg_446.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_446.getPosition();
    goto tail_peg_447;
    out_peg_449:
    Result result_peg_454(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_454.setValue(value);
            }
        
        
    }
    
    
    return result_peg_454;
    
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_468(stream, "expr2_rest");
    int myposition = position;
    tail_peg_459:
    Value right;
    Value new_left;
    Result result_peg_458(myposition);
    {
    
        result_peg_458 = rule_s(stream, result_peg_458.getPosition());
            if (result_peg_458.error()){
                goto out_peg_461;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_458.getPosition()))){
                    result_peg_458.nextPosition();
                } else {
                    goto out_peg_461;
                }
            }
            result_peg_458.setValue((void*) "^^");
        
        
        
        result_peg_458 = rule_s(stream, result_peg_458.getPosition());
            if (result_peg_458.error()){
                goto out_peg_461;
            }
        
        
        
        result_peg_458 = rule_expr3(stream, result_peg_458.getPosition());
            if (result_peg_458.error()){
                goto out_peg_461;
            }
            right = result_peg_458.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_458.setValue(value);
            }
            new_left = result_peg_458.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_458.getPosition();
    goto tail_peg_459;
    out_peg_461:
    Result result_peg_466(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_466.setValue(value);
            }
        
        
    }
    
    
    return result_peg_466;
    
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_480(stream, "expr3_rest");
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
                if (compareChar("&&"[i], stream.get(result_peg_470.getPosition()))){
                    result_peg_470.nextPosition();
                } else {
                    goto out_peg_473;
                }
            }
            result_peg_470.setValue((void*) "&&");
        
        
        
        result_peg_470 = rule_s(stream, result_peg_470.getPosition());
            if (result_peg_470.error()){
                goto out_peg_473;
            }
        
        
        
        result_peg_470 = rule_expr4(stream, result_peg_470.getPosition());
            if (result_peg_470.error()){
                goto out_peg_473;
            }
            right = result_peg_470.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
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
    
    
    return result_peg_478;
    
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_492(stream, "expr4_rest");
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
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_482.getPosition()))){
                    result_peg_482.nextPosition();
                } else {
                    goto out_peg_485;
                }
            }
            result_peg_482.setValue((void*) "|");
        
        
        
        result_peg_482 = rule_s(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
        
        
        
        result_peg_482 = rule_expr5(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
            right = result_peg_482.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
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
    
    
    return result_peg_490;
    
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_504(stream, "expr5_rest");
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
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_494.getPosition()))){
                    result_peg_494.nextPosition();
                } else {
                    goto out_peg_497;
                }
            }
            result_peg_494.setValue((void*) "^");
        
        
        
        result_peg_494 = rule_s(stream, result_peg_494.getPosition());
            if (result_peg_494.error()){
                goto out_peg_497;
            }
        
        
        
        result_peg_494 = rule_expr6(stream, result_peg_494.getPosition());
            if (result_peg_494.error()){
                goto out_peg_497;
            }
            right = result_peg_494.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
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
    
    
    return result_peg_502;
    
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_516(stream, "expr6_rest");
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
                if (compareChar("&"[i], stream.get(result_peg_506.getPosition()))){
                    result_peg_506.nextPosition();
                } else {
                    goto out_peg_509;
                }
            }
            result_peg_506.setValue((void*) "&");
        
        
        
        result_peg_506 = rule_s(stream, result_peg_506.getPosition());
            if (result_peg_506.error()){
                goto out_peg_509;
            }
        
        
        
        result_peg_506 = rule_expr7(stream, result_peg_506.getPosition());
            if (result_peg_506.error()){
                goto out_peg_509;
            }
            right = result_peg_506.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
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
    
    
    return result_peg_514;
    
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_528(stream, "expr7_rest");
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
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_518.getPosition()))){
                    result_peg_518.nextPosition();
                } else {
                    goto out_peg_521;
                }
            }
            result_peg_518.setValue((void*) ":=");
        
        
        
        result_peg_518 = rule_s(stream, result_peg_518.getPosition());
            if (result_peg_518.error()){
                goto out_peg_521;
            }
        
        
        
        result_peg_518 = rule_expr8(stream, result_peg_518.getPosition());
            if (result_peg_518.error()){
                goto out_peg_521;
            }
            right = result_peg_518.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
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
    
    
    return result_peg_526;
    
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_547(stream, "expr8_rest");
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
                if (compareChar("="[i], stream.get(result_peg_530.getPosition()))){
                    result_peg_530.nextPosition();
                } else {
                    goto out_peg_533;
                }
            }
            result_peg_530.setValue((void*) "=");
        
        
        
        result_peg_530 = rule_s(stream, result_peg_530.getPosition());
            if (result_peg_530.error()){
                goto out_peg_533;
            }
        
        
        
        result_peg_530 = rule_expr9(stream, result_peg_530.getPosition());
            if (result_peg_530.error()){
                goto out_peg_533;
            }
            right = result_peg_530.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
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
    
        result_peg_538 = rule_s(stream, result_peg_538.getPosition());
            if (result_peg_538.error()){
                goto out_peg_540;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_538.getPosition()))){
                    result_peg_538.nextPosition();
                } else {
                    goto out_peg_540;
                }
            }
            result_peg_538.setValue((void*) "!=");
        
        
        
        result_peg_538 = rule_s(stream, result_peg_538.getPosition());
            if (result_peg_538.error()){
                goto out_peg_540;
            }
        
        
        
        result_peg_538 = rule_expr9(stream, result_peg_538.getPosition());
            if (result_peg_538.error()){
                goto out_peg_540;
            }
            right = result_peg_538.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_538.setValue(value);
            }
            new_left = result_peg_538.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_538.getPosition();
    goto tail_peg_531;
    out_peg_540:
    Result result_peg_545(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_545.setValue(value);
            }
        
        
    }
    
    
    return result_peg_545;
    
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_559(stream, "expr9_rest");
    int myposition = position;
    tail_peg_550:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_549(myposition);
    {
    
        result_peg_549 = rule_s(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
        
        
        
        result_peg_549 = rule_compare(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
            maker = result_peg_549.getValues();
        
        
        
        result_peg_549 = rule_s(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
        
        
        
        result_peg_549 = rule_expr10(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
            right = result_peg_549.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_549.setValue(value);
            }
            new_left = result_peg_549.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_549.getPosition();
    goto tail_peg_550;
    out_peg_552:
    Result result_peg_557(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_557.setValue(value);
            }
        
        
    }
    
    
    return result_peg_557;
    
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_560 = stream.getColumn(position);
    if (column_peg_560.chunk8 != 0 && column_peg_560.chunk8->chunk_compare.calculated()){
        return column_peg_560.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_573(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_561(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_561.getPosition()))){
                    result_peg_561.nextPosition();
                } else {
                    goto out_peg_563;
                }
            }
            result_peg_561.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_561.setValue(value);
            }
        
        
    }
    
    if (column_peg_560.chunk8 == 0){
        column_peg_560.chunk8 = new Chunk8();
    }
    column_peg_560.chunk8->chunk_compare = result_peg_561;
    stream.update(result_peg_561.getPosition());
    
    
    return result_peg_561;
    out_peg_563:
    Result result_peg_564(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_564.getPosition()))){
                    result_peg_564.nextPosition();
                } else {
                    goto out_peg_566;
                }
            }
            result_peg_564.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_564.setValue(value);
            }
        
        
    }
    
    if (column_peg_560.chunk8 == 0){
        column_peg_560.chunk8 = new Chunk8();
    }
    column_peg_560.chunk8->chunk_compare = result_peg_564;
    stream.update(result_peg_564.getPosition());
    
    
    return result_peg_564;
    out_peg_566:
    Result result_peg_567(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_567.getPosition()))){
                    result_peg_567.nextPosition();
                } else {
                    goto out_peg_569;
                }
            }
            result_peg_567.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_567.setValue(value);
            }
        
        
    }
    
    if (column_peg_560.chunk8 == 0){
        column_peg_560.chunk8 = new Chunk8();
    }
    column_peg_560.chunk8->chunk_compare = result_peg_567;
    stream.update(result_peg_567.getPosition());
    
    
    return result_peg_567;
    out_peg_569:
    Result result_peg_570(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_570.getPosition()))){
                    result_peg_570.nextPosition();
                } else {
                    goto out_peg_572;
                }
            }
            result_peg_570.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_570.setValue(value);
            }
        
        
    }
    
    if (column_peg_560.chunk8 == 0){
        column_peg_560.chunk8 = new Chunk8();
    }
    column_peg_560.chunk8->chunk_compare = result_peg_570;
    stream.update(result_peg_570.getPosition());
    
    
    return result_peg_570;
    out_peg_572:
    
    if (column_peg_560.chunk8 == 0){
        column_peg_560.chunk8 = new Chunk8();
    }
    column_peg_560.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_574 = stream.getColumn(position);
    if (column_peg_574.chunk8 != 0 && column_peg_574.chunk8->chunk_all_compare.calculated()){
        return column_peg_574.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_581(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_575(myposition);
    
    result_peg_575 = rule_compare(stream, result_peg_575.getPosition());
    if (result_peg_575.error()){
        goto out_peg_576;
    }
    
    if (column_peg_574.chunk8 == 0){
        column_peg_574.chunk8 = new Chunk8();
    }
    column_peg_574.chunk8->chunk_all_compare = result_peg_575;
    stream.update(result_peg_575.getPosition());
    
    
    return result_peg_575;
    out_peg_576:
    Result result_peg_577(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_577.getPosition()))){
            result_peg_577.nextPosition();
        } else {
            goto out_peg_578;
        }
    }
    result_peg_577.setValue((void*) "=");
    
    if (column_peg_574.chunk8 == 0){
        column_peg_574.chunk8 = new Chunk8();
    }
    column_peg_574.chunk8->chunk_all_compare = result_peg_577;
    stream.update(result_peg_577.getPosition());
    
    
    return result_peg_577;
    out_peg_578:
    Result result_peg_579(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_579.getPosition()))){
            result_peg_579.nextPosition();
        } else {
            goto out_peg_580;
        }
    }
    result_peg_579.setValue((void*) "!=");
    
    if (column_peg_574.chunk8 == 0){
        column_peg_574.chunk8 = new Chunk8();
    }
    column_peg_574.chunk8->chunk_all_compare = result_peg_579;
    stream.update(result_peg_579.getPosition());
    
    
    return result_peg_579;
    out_peg_580:
    
    if (column_peg_574.chunk8 == 0){
        column_peg_574.chunk8 = new Chunk8();
    }
    column_peg_574.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_600(stream, "expr10_rest");
    int myposition = position;
    tail_peg_584:
    Value right;
    Value new_left;
    Result result_peg_583(myposition);
    {
    
        result_peg_583 = rule_s(stream, result_peg_583.getPosition());
            if (result_peg_583.error()){
                goto out_peg_586;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_583.getPosition()))){
                    result_peg_583.nextPosition();
                } else {
                    goto out_peg_586;
                }
            }
            result_peg_583.setValue((void*) "+");
        
        
        
        result_peg_583 = rule_s(stream, result_peg_583.getPosition());
            if (result_peg_583.error()){
                goto out_peg_586;
            }
        
        
        
        result_peg_583 = rule_expr11(stream, result_peg_583.getPosition());
            if (result_peg_583.error()){
                goto out_peg_586;
            }
            right = result_peg_583.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_583.setValue(value);
            }
            new_left = result_peg_583.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_583.getPosition();
    goto tail_peg_584;
    out_peg_586:
    Result result_peg_591(myposition);
    {
    
        result_peg_591 = rule_s(stream, result_peg_591.getPosition());
            if (result_peg_591.error()){
                goto out_peg_593;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_591.getPosition()))){
                    result_peg_591.nextPosition();
                } else {
                    goto out_peg_593;
                }
            }
            result_peg_591.setValue((void*) "-");
        
        
        
        result_peg_591 = rule_s(stream, result_peg_591.getPosition());
            if (result_peg_591.error()){
                goto out_peg_593;
            }
        
        
        
        result_peg_591 = rule_expr11(stream, result_peg_591.getPosition());
            if (result_peg_591.error()){
                goto out_peg_593;
            }
            right = result_peg_591.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_591.setValue(value);
            }
            new_left = result_peg_591.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_591.getPosition();
    goto tail_peg_584;
    out_peg_593:
    Result result_peg_598(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_598.setValue(value);
            }
        
        
    }
    
    
    return result_peg_598;
    
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_626(stream, "expr11_rest");
    int myposition = position;
    tail_peg_603:
    Value right;
    Value new_left;
    Result result_peg_602(myposition);
    {
    
        result_peg_602 = rule_s(stream, result_peg_602.getPosition());
            if (result_peg_602.error()){
                goto out_peg_605;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_602.getPosition()))){
                    result_peg_602.nextPosition();
                } else {
                    goto out_peg_605;
                }
            }
            result_peg_602.setValue((void*) "*");
        
        
        
        result_peg_602 = rule_s(stream, result_peg_602.getPosition());
            if (result_peg_602.error()){
                goto out_peg_605;
            }
        
        
        
        result_peg_602 = rule_expr12(stream, result_peg_602.getPosition());
            if (result_peg_602.error()){
                goto out_peg_605;
            }
            right = result_peg_602.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_602.setValue(value);
            }
            new_left = result_peg_602.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_602.getPosition();
    goto tail_peg_603;
    out_peg_605:
    Result result_peg_610(myposition);
    {
    
        result_peg_610 = rule_s(stream, result_peg_610.getPosition());
            if (result_peg_610.error()){
                goto out_peg_612;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_610.getPosition()))){
                    result_peg_610.nextPosition();
                } else {
                    goto out_peg_612;
                }
            }
            result_peg_610.setValue((void*) "/");
        
        
        
        result_peg_610 = rule_s(stream, result_peg_610.getPosition());
            if (result_peg_610.error()){
                goto out_peg_612;
            }
        
        
        
        result_peg_610 = rule_expr12(stream, result_peg_610.getPosition());
            if (result_peg_610.error()){
                goto out_peg_612;
            }
            right = result_peg_610.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_610.setValue(value);
            }
            new_left = result_peg_610.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_610.getPosition();
    goto tail_peg_603;
    out_peg_612:
    Result result_peg_617(myposition);
    {
    
        result_peg_617 = rule_s(stream, result_peg_617.getPosition());
            if (result_peg_617.error()){
                goto out_peg_619;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_617.getPosition()))){
                    result_peg_617.nextPosition();
                } else {
                    goto out_peg_619;
                }
            }
            result_peg_617.setValue((void*) "%");
        
        
        
        result_peg_617 = rule_s(stream, result_peg_617.getPosition());
            if (result_peg_617.error()){
                goto out_peg_619;
            }
        
        
        
        result_peg_617 = rule_expr12(stream, result_peg_617.getPosition());
            if (result_peg_617.error()){
                goto out_peg_619;
            }
            right = result_peg_617.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_617.setValue(value);
            }
            new_left = result_peg_617.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_617.getPosition();
    goto tail_peg_603;
    out_peg_619:
    Result result_peg_624(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_624.setValue(value);
            }
        
        
    }
    
    
    return result_peg_624;
    
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_638(stream, "expr12_rest");
    int myposition = position;
    tail_peg_629:
    Value right;
    Value new_left;
    Result result_peg_628(myposition);
    {
    
        result_peg_628 = rule_s(stream, result_peg_628.getPosition());
            if (result_peg_628.error()){
                goto out_peg_631;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_628.getPosition()))){
                    result_peg_628.nextPosition();
                } else {
                    goto out_peg_631;
                }
            }
            result_peg_628.setValue((void*) "**");
        
        
        
        result_peg_628 = rule_s(stream, result_peg_628.getPosition());
            if (result_peg_628.error()){
                goto out_peg_631;
            }
        
        
        
        result_peg_628 = rule_expr13(stream, result_peg_628.getPosition());
            if (result_peg_628.error()){
                goto out_peg_631;
            }
            right = result_peg_628.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_628.setValue(value);
            }
            new_left = result_peg_628.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_628.getPosition();
    goto tail_peg_629;
    out_peg_631:
    Result result_peg_636(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_636.setValue(value);
            }
        
        
    }
    
    
    return result_peg_636;
    
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_639 = stream.getColumn(position);
    if (column_peg_639.chunk9 != 0 && column_peg_639.chunk9->chunk_unary.calculated()){
        return column_peg_639.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_668(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_640(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_640.getPosition()))){
                    result_peg_640.nextPosition();
                } else {
                    goto out_peg_642;
                }
            }
            result_peg_640.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_640.setValue(value);
            }
        
        
    }
    
    if (column_peg_639.chunk9 == 0){
        column_peg_639.chunk9 = new Chunk9();
    }
    column_peg_639.chunk9->chunk_unary = result_peg_640;
    stream.update(result_peg_640.getPosition());
    
    
    return result_peg_640;
    out_peg_642:
    Result result_peg_643(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_643.getPosition()))){
                    result_peg_643.nextPosition();
                } else {
                    goto out_peg_645;
                }
            }
            result_peg_643.setValue((void*) "-");
        
        
        
        Result result_peg_648(result_peg_643);
            {
                
                char letter_peg_653 = stream.get(result_peg_648.getPosition());
                if (letter_peg_653 != '\0' && strchr("0123456789", letter_peg_653) != NULL){
                    result_peg_648.nextPosition();
                    result_peg_648.setValue((void*) (long) letter_peg_653);
                } else {
                    goto out_peg_652;
                }
                
            }
            goto success_peg_649;
            out_peg_652:
            goto not_peg_647;
            success_peg_649:
            ;
            goto out_peg_645;
            not_peg_647:
            result_peg_643.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_643.setValue(value);
            }
        
        
    }
    
    if (column_peg_639.chunk9 == 0){
        column_peg_639.chunk9 = new Chunk9();
    }
    column_peg_639.chunk9->chunk_unary = result_peg_643;
    stream.update(result_peg_643.getPosition());
    
    
    return result_peg_643;
    out_peg_645:
    Result result_peg_654(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_654.getPosition()))){
                    result_peg_654.nextPosition();
                } else {
                    goto out_peg_656;
                }
            }
            result_peg_654.setValue((void*) "~");
        
        
        
        Result result_peg_659(result_peg_654);
            result_peg_659 = rule_key(stream, result_peg_659.getPosition());
            if (result_peg_659.error()){
                goto not_peg_658;
            }
            goto out_peg_656;
            not_peg_658:
            result_peg_654.setValue((void*)0);
        
        
        
        Result result_peg_662(result_peg_654);
            {
                
                char letter_peg_667 = stream.get(result_peg_662.getPosition());
                if (letter_peg_667 != '\0' && strchr("0123456789", letter_peg_667) != NULL){
                    result_peg_662.nextPosition();
                    result_peg_662.setValue((void*) (long) letter_peg_667);
                } else {
                    goto out_peg_666;
                }
                
            }
            goto success_peg_663;
            out_peg_666:
            goto not_peg_661;
            success_peg_663:
            ;
            goto out_peg_656;
            not_peg_661:
            result_peg_654.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_654.setValue(value);
            }
        
        
    }
    
    if (column_peg_639.chunk9 == 0){
        column_peg_639.chunk9 = new Chunk9();
    }
    column_peg_639.chunk9->chunk_unary = result_peg_654;
    stream.update(result_peg_654.getPosition());
    
    
    return result_peg_654;
    out_peg_656:
    
    if (column_peg_639.chunk9 == 0){
        column_peg_639.chunk9 = new Chunk9();
    }
    column_peg_639.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_669 = stream.getColumn(position);
    if (column_peg_669.chunk9 != 0 && column_peg_669.chunk9->chunk_expr13_real.calculated()){
        return column_peg_669.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_681(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_670(myposition);
    
    result_peg_670 = rule_function(stream, result_peg_670.getPosition());
    if (result_peg_670.error()){
        goto out_peg_671;
    }
    
    if (column_peg_669.chunk9 == 0){
        column_peg_669.chunk9 = new Chunk9();
    }
    column_peg_669.chunk9->chunk_expr13_real = result_peg_670;
    stream.update(result_peg_670.getPosition());
    
    
    return result_peg_670;
    out_peg_671:
    Result result_peg_672(myposition);
    
    result_peg_672 = rule_value(stream, result_peg_672.getPosition());
    if (result_peg_672.error()){
        goto out_peg_673;
    }
    
    if (column_peg_669.chunk9 == 0){
        column_peg_669.chunk9 = new Chunk9();
    }
    column_peg_669.chunk9->chunk_expr13_real = result_peg_672;
    stream.update(result_peg_672.getPosition());
    
    
    return result_peg_672;
    out_peg_673:
    Result result_peg_674(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_674.getPosition()))){
                    result_peg_674.nextPosition();
                } else {
                    goto out_peg_676;
                }
            }
            result_peg_674.setValue((void*) "(");
        
        
        
        result_peg_674 = rule_s(stream, result_peg_674.getPosition());
            if (result_peg_674.error()){
                goto out_peg_676;
            }
        
        
        
        result_peg_674 = rule_expr(stream, result_peg_674.getPosition());
            if (result_peg_674.error()){
                goto out_peg_676;
            }
            e = result_peg_674.getValues();
        
        
        
        result_peg_674 = rule_s(stream, result_peg_674.getPosition());
            if (result_peg_674.error()){
                goto out_peg_676;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_674.getPosition()))){
                    result_peg_674.nextPosition();
                } else {
                    goto out_peg_676;
                }
            }
            result_peg_674.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_674.setValue(value);
            }
        
        
    }
    
    if (column_peg_669.chunk9 == 0){
        column_peg_669.chunk9 = new Chunk9();
    }
    column_peg_669.chunk9->chunk_expr13_real = result_peg_674;
    stream.update(result_peg_674.getPosition());
    
    
    return result_peg_674;
    out_peg_676:
    
    if (column_peg_669.chunk9 == 0){
        column_peg_669.chunk9 = new Chunk9();
    }
    column_peg_669.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_682 = stream.getColumn(position);
    if (column_peg_682.chunk9 != 0 && column_peg_682.chunk9->chunk_function.calculated()){
        return column_peg_682.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_947(stream, "function");
    int myposition = position;
    
    Value name;
    Value arg1;
    Value arg2;
    Value arg3;
    Result result_peg_683(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_683.getPosition()))){
                    result_peg_683.nextPosition();
                } else {
                    goto out_peg_685;
                }
            }
            result_peg_683.setValue((void*) "abs");
            name = result_peg_683.getValues();
        
        
        
        result_peg_683 = rule_s(stream, result_peg_683.getPosition());
            if (result_peg_683.error()){
                goto out_peg_685;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_683.getPosition()))){
                    result_peg_683.nextPosition();
                } else {
                    goto out_peg_685;
                }
            }
            result_peg_683.setValue((void*) "(");
        
        
        
        result_peg_683 = rule_s(stream, result_peg_683.getPosition());
            if (result_peg_683.error()){
                goto out_peg_685;
            }
        
        
        
        result_peg_683 = rule_expr(stream, result_peg_683.getPosition());
            if (result_peg_683.error()){
                goto out_peg_685;
            }
            arg1 = result_peg_683.getValues();
        
        
        
        result_peg_683 = rule_s(stream, result_peg_683.getPosition());
            if (result_peg_683.error()){
                goto out_peg_685;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_683.getPosition()))){
                    result_peg_683.nextPosition();
                } else {
                    goto out_peg_685;
                }
            }
            result_peg_683.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_683.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_683;
    stream.update(result_peg_683.getPosition());
    
    
    return result_peg_683;
    out_peg_685:
    Result result_peg_692(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_692.getPosition()))){
                    result_peg_692.nextPosition();
                } else {
                    goto out_peg_694;
                }
            }
            result_peg_692.setValue((void*) "const");
            name = result_peg_692.getValues();
        
        
        
        result_peg_692 = rule_s(stream, result_peg_692.getPosition());
            if (result_peg_692.error()){
                goto out_peg_694;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_692.getPosition()))){
                    result_peg_692.nextPosition();
                } else {
                    goto out_peg_694;
                }
            }
            result_peg_692.setValue((void*) "(");
        
        
        
        result_peg_692 = rule_s(stream, result_peg_692.getPosition());
            if (result_peg_692.error()){
                goto out_peg_694;
            }
        
        
        
        result_peg_692 = rule_expr(stream, result_peg_692.getPosition());
            if (result_peg_692.error()){
                goto out_peg_694;
            }
            arg1 = result_peg_692.getValues();
        
        
        
        result_peg_692 = rule_s(stream, result_peg_692.getPosition());
            if (result_peg_692.error()){
                goto out_peg_694;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_692.getPosition()))){
                    result_peg_692.nextPosition();
                } else {
                    goto out_peg_694;
                }
            }
            result_peg_692.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_692.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_692;
    stream.update(result_peg_692.getPosition());
    
    
    return result_peg_692;
    out_peg_694:
    Result result_peg_701(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_701.getPosition()))){
                    result_peg_701.nextPosition();
                } else {
                    goto out_peg_703;
                }
            }
            result_peg_701.setValue((void*) "selfanimexist");
            name = result_peg_701.getValues();
        
        
        
        result_peg_701 = rule_s(stream, result_peg_701.getPosition());
            if (result_peg_701.error()){
                goto out_peg_703;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_701.getPosition()))){
                    result_peg_701.nextPosition();
                } else {
                    goto out_peg_703;
                }
            }
            result_peg_701.setValue((void*) "(");
        
        
        
        result_peg_701 = rule_s(stream, result_peg_701.getPosition());
            if (result_peg_701.error()){
                goto out_peg_703;
            }
        
        
        
        result_peg_701 = rule_expr(stream, result_peg_701.getPosition());
            if (result_peg_701.error()){
                goto out_peg_703;
            }
            arg1 = result_peg_701.getValues();
        
        
        
        result_peg_701 = rule_s(stream, result_peg_701.getPosition());
            if (result_peg_701.error()){
                goto out_peg_703;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_701.getPosition()))){
                    result_peg_701.nextPosition();
                } else {
                    goto out_peg_703;
                }
            }
            result_peg_701.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_701.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_701;
    stream.update(result_peg_701.getPosition());
    
    
    return result_peg_701;
    out_peg_703:
    Result result_peg_710(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_710.getPosition()))){
                    result_peg_710.nextPosition();
                } else {
                    goto out_peg_712;
                }
            }
            result_peg_710.setValue((void*) "ifelse");
            name = result_peg_710.getValues();
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_710.getPosition()))){
                    result_peg_710.nextPosition();
                } else {
                    goto out_peg_712;
                }
            }
            result_peg_710.setValue((void*) "(");
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        result_peg_710 = rule_expr_c(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
            arg1 = result_peg_710.getValues();
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_710.getPosition()))){
                    result_peg_710.nextPosition();
                } else {
                    goto out_peg_712;
                }
            }
            result_peg_710.setValue((void*) ",");
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        result_peg_710 = rule_expr_c(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
            arg2 = result_peg_710.getValues();
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_710.getPosition()))){
                    result_peg_710.nextPosition();
                } else {
                    goto out_peg_712;
                }
            }
            result_peg_710.setValue((void*) ",");
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        result_peg_710 = rule_expr_c(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
            arg3 = result_peg_710.getValues();
        
        
        
        result_peg_710 = rule_s(stream, result_peg_710.getPosition());
            if (result_peg_710.error()){
                goto out_peg_712;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_710.getPosition()))){
                    result_peg_710.nextPosition();
                } else {
                    goto out_peg_712;
                }
            }
            result_peg_710.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction("ifelse", arg1, arg2, arg3);
                result_peg_710.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_710;
    stream.update(result_peg_710.getPosition());
    
    
    return result_peg_710;
    out_peg_712:
    Result result_peg_727(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_727.getPosition()))){
                    result_peg_727.nextPosition();
                } else {
                    goto out_peg_729;
                }
            }
            result_peg_727.setValue((void*) "gethitvar");
            name = result_peg_727.getValues();
        
        
        
        result_peg_727 = rule_s(stream, result_peg_727.getPosition());
            if (result_peg_727.error()){
                goto out_peg_729;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_727.getPosition()))){
                    result_peg_727.nextPosition();
                } else {
                    goto out_peg_729;
                }
            }
            result_peg_727.setValue((void*) "(");
        
        
        
        result_peg_727 = rule_s(stream, result_peg_727.getPosition());
            if (result_peg_727.error()){
                goto out_peg_729;
            }
        
        
        
        result_peg_727 = rule_expr_c(stream, result_peg_727.getPosition());
            if (result_peg_727.error()){
                goto out_peg_729;
            }
            arg1 = result_peg_727.getValues();
        
        
        
        result_peg_727 = rule_s(stream, result_peg_727.getPosition());
            if (result_peg_727.error()){
                goto out_peg_729;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_727.getPosition()))){
                    result_peg_727.nextPosition();
                } else {
                    goto out_peg_729;
                }
            }
            result_peg_727.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_727.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_727;
    stream.update(result_peg_727.getPosition());
    
    
    return result_peg_727;
    out_peg_729:
    Result result_peg_736(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_736.getPosition()))){
                    result_peg_736.nextPosition();
                } else {
                    goto out_peg_738;
                }
            }
            result_peg_736.setValue((void*) "floor");
            name = result_peg_736.getValues();
        
        
        
        result_peg_736 = rule_s(stream, result_peg_736.getPosition());
            if (result_peg_736.error()){
                goto out_peg_738;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_736.getPosition()))){
                    result_peg_736.nextPosition();
                } else {
                    goto out_peg_738;
                }
            }
            result_peg_736.setValue((void*) "(");
        
        
        
        result_peg_736 = rule_s(stream, result_peg_736.getPosition());
            if (result_peg_736.error()){
                goto out_peg_738;
            }
        
        
        
        result_peg_736 = rule_expr_c(stream, result_peg_736.getPosition());
            if (result_peg_736.error()){
                goto out_peg_738;
            }
            arg1 = result_peg_736.getValues();
        
        
        
        result_peg_736 = rule_s(stream, result_peg_736.getPosition());
            if (result_peg_736.error()){
                goto out_peg_738;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_736.getPosition()))){
                    result_peg_736.nextPosition();
                } else {
                    goto out_peg_738;
                }
            }
            result_peg_736.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_736.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_736;
    stream.update(result_peg_736.getPosition());
    
    
    return result_peg_736;
    out_peg_738:
    Result result_peg_745(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_745.getPosition()))){
                    result_peg_745.nextPosition();
                } else {
                    goto out_peg_747;
                }
            }
            result_peg_745.setValue((void*) "ceil");
            name = result_peg_745.getValues();
        
        
        
        result_peg_745 = rule_s(stream, result_peg_745.getPosition());
            if (result_peg_745.error()){
                goto out_peg_747;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_745.getPosition()))){
                    result_peg_745.nextPosition();
                } else {
                    goto out_peg_747;
                }
            }
            result_peg_745.setValue((void*) "(");
        
        
        
        result_peg_745 = rule_s(stream, result_peg_745.getPosition());
            if (result_peg_745.error()){
                goto out_peg_747;
            }
        
        
        
        result_peg_745 = rule_expr_c(stream, result_peg_745.getPosition());
            if (result_peg_745.error()){
                goto out_peg_747;
            }
            arg1 = result_peg_745.getValues();
        
        
        
        result_peg_745 = rule_s(stream, result_peg_745.getPosition());
            if (result_peg_745.error()){
                goto out_peg_747;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_745.getPosition()))){
                    result_peg_745.nextPosition();
                } else {
                    goto out_peg_747;
                }
            }
            result_peg_745.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_745.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_745;
    stream.update(result_peg_745.getPosition());
    
    
    return result_peg_745;
    out_peg_747:
    Result result_peg_754(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("sin"[i], stream.get(result_peg_754.getPosition()))){
                    result_peg_754.nextPosition();
                } else {
                    goto out_peg_756;
                }
            }
            result_peg_754.setValue((void*) "sin");
            name = result_peg_754.getValues();
        
        
        
        result_peg_754 = rule_s(stream, result_peg_754.getPosition());
            if (result_peg_754.error()){
                goto out_peg_756;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_754.getPosition()))){
                    result_peg_754.nextPosition();
                } else {
                    goto out_peg_756;
                }
            }
            result_peg_754.setValue((void*) "(");
        
        
        
        result_peg_754 = rule_s(stream, result_peg_754.getPosition());
            if (result_peg_754.error()){
                goto out_peg_756;
            }
        
        
        
        result_peg_754 = rule_expr_c(stream, result_peg_754.getPosition());
            if (result_peg_754.error()){
                goto out_peg_756;
            }
            arg1 = result_peg_754.getValues();
        
        
        
        result_peg_754 = rule_s(stream, result_peg_754.getPosition());
            if (result_peg_754.error()){
                goto out_peg_756;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_754.getPosition()))){
                    result_peg_754.nextPosition();
                } else {
                    goto out_peg_756;
                }
            }
            result_peg_754.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_754.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_754;
    stream.update(result_peg_754.getPosition());
    
    
    return result_peg_754;
    out_peg_756:
    Result result_peg_763(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_763.getPosition()))){
                    result_peg_763.nextPosition();
                } else {
                    goto out_peg_765;
                }
            }
            result_peg_763.setValue((void*) "sysvar");
            name = result_peg_763.getValues();
        
        
        
        result_peg_763 = rule_s(stream, result_peg_763.getPosition());
            if (result_peg_763.error()){
                goto out_peg_765;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_763.getPosition()))){
                    result_peg_763.nextPosition();
                } else {
                    goto out_peg_765;
                }
            }
            result_peg_763.setValue((void*) "(");
        
        
        
        result_peg_763 = rule_s(stream, result_peg_763.getPosition());
            if (result_peg_763.error()){
                goto out_peg_765;
            }
        
        
        
        result_peg_763 = rule_integer(stream, result_peg_763.getPosition());
            if (result_peg_763.error()){
                goto out_peg_765;
            }
            arg1 = result_peg_763.getValues();
        
        
        
        result_peg_763 = rule_s(stream, result_peg_763.getPosition());
            if (result_peg_763.error()){
                goto out_peg_765;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_763.getPosition()))){
                    result_peg_763.nextPosition();
                } else {
                    goto out_peg_765;
                }
            }
            result_peg_763.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_763.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_763;
    stream.update(result_peg_763.getPosition());
    
    
    return result_peg_763;
    out_peg_765:
    Result result_peg_772(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_772.getPosition()))){
                    result_peg_772.nextPosition();
                } else {
                    goto out_peg_774;
                }
            }
            result_peg_772.setValue((void*) "var");
            name = result_peg_772.getValues();
        
        
        
        result_peg_772 = rule_s(stream, result_peg_772.getPosition());
            if (result_peg_772.error()){
                goto out_peg_774;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_772.getPosition()))){
                    result_peg_772.nextPosition();
                } else {
                    goto out_peg_774;
                }
            }
            result_peg_772.setValue((void*) "(");
        
        
        
        result_peg_772 = rule_s(stream, result_peg_772.getPosition());
            if (result_peg_772.error()){
                goto out_peg_774;
            }
        
        
        
        result_peg_772 = rule_integer(stream, result_peg_772.getPosition());
            if (result_peg_772.error()){
                goto out_peg_774;
            }
            arg1 = result_peg_772.getValues();
        
        
        
        result_peg_772 = rule_s(stream, result_peg_772.getPosition());
            if (result_peg_772.error()){
                goto out_peg_774;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_772.getPosition()))){
                    result_peg_772.nextPosition();
                } else {
                    goto out_peg_774;
                }
            }
            result_peg_772.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_772.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_772;
    stream.update(result_peg_772.getPosition());
    
    
    return result_peg_772;
    out_peg_774:
    Result result_peg_781(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_781.getPosition()))){
                    result_peg_781.nextPosition();
                } else {
                    goto out_peg_783;
                }
            }
            result_peg_781.setValue((void*) "numexplod");
            name = result_peg_781.getValues();
        
        
        
        result_peg_781 = rule_s(stream, result_peg_781.getPosition());
            if (result_peg_781.error()){
                goto out_peg_783;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_781.getPosition()))){
                    result_peg_781.nextPosition();
                } else {
                    goto out_peg_783;
                }
            }
            result_peg_781.setValue((void*) "(");
        
        
        
        result_peg_781 = rule_s(stream, result_peg_781.getPosition());
            if (result_peg_781.error()){
                goto out_peg_783;
            }
        
        
        
        result_peg_781 = rule_integer(stream, result_peg_781.getPosition());
            if (result_peg_781.error()){
                goto out_peg_783;
            }
            arg1 = result_peg_781.getValues();
        
        
        
        result_peg_781 = rule_s(stream, result_peg_781.getPosition());
            if (result_peg_781.error()){
                goto out_peg_783;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_781.getPosition()))){
                    result_peg_781.nextPosition();
                } else {
                    goto out_peg_783;
                }
            }
            result_peg_781.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_781.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_781;
    stream.update(result_peg_781.getPosition());
    
    
    return result_peg_781;
    out_peg_783:
    Result result_peg_790(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_790.getPosition()))){
                    result_peg_790.nextPosition();
                } else {
                    goto out_peg_792;
                }
            }
            result_peg_790.setValue((void*) "numhelper");
            name = result_peg_790.getValues();
        
        
        
        result_peg_790 = rule_s(stream, result_peg_790.getPosition());
            if (result_peg_790.error()){
                goto out_peg_792;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_790.getPosition()))){
                    result_peg_790.nextPosition();
                } else {
                    goto out_peg_792;
                }
            }
            result_peg_790.setValue((void*) "(");
        
        
        
        result_peg_790 = rule_s(stream, result_peg_790.getPosition());
            if (result_peg_790.error()){
                goto out_peg_792;
            }
        
        
        
        result_peg_790 = rule_integer(stream, result_peg_790.getPosition());
            if (result_peg_790.error()){
                goto out_peg_792;
            }
            arg1 = result_peg_790.getValues();
        
        
        
        result_peg_790 = rule_s(stream, result_peg_790.getPosition());
            if (result_peg_790.error()){
                goto out_peg_792;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_790.getPosition()))){
                    result_peg_790.nextPosition();
                } else {
                    goto out_peg_792;
                }
            }
            result_peg_790.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_790.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_790;
    stream.update(result_peg_790.getPosition());
    
    
    return result_peg_790;
    out_peg_792:
    Result result_peg_799(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_799.getPosition()))){
                    result_peg_799.nextPosition();
                } else {
                    goto out_peg_801;
                }
            }
            result_peg_799.setValue((void*) "numprojid");
            name = result_peg_799.getValues();
        
        
        
        result_peg_799 = rule_s(stream, result_peg_799.getPosition());
            if (result_peg_799.error()){
                goto out_peg_801;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_799.getPosition()))){
                    result_peg_799.nextPosition();
                } else {
                    goto out_peg_801;
                }
            }
            result_peg_799.setValue((void*) "(");
        
        
        
        result_peg_799 = rule_s(stream, result_peg_799.getPosition());
            if (result_peg_799.error()){
                goto out_peg_801;
            }
        
        
        
        result_peg_799 = rule_integer(stream, result_peg_799.getPosition());
            if (result_peg_799.error()){
                goto out_peg_801;
            }
            arg1 = result_peg_799.getValues();
        
        
        
        result_peg_799 = rule_s(stream, result_peg_799.getPosition());
            if (result_peg_799.error()){
                goto out_peg_801;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_799.getPosition()))){
                    result_peg_799.nextPosition();
                } else {
                    goto out_peg_801;
                }
            }
            result_peg_799.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_799.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_799;
    stream.update(result_peg_799.getPosition());
    
    
    return result_peg_799;
    out_peg_801:
    Result result_peg_808(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_808.getPosition()))){
                    result_peg_808.nextPosition();
                } else {
                    goto out_peg_810;
                }
            }
            result_peg_808.setValue((void*) "helper");
            name = result_peg_808.getValues();
        
        
        
        result_peg_808 = rule_s(stream, result_peg_808.getPosition());
            if (result_peg_808.error()){
                goto out_peg_810;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_808.getPosition()))){
                    result_peg_808.nextPosition();
                } else {
                    goto out_peg_810;
                }
            }
            result_peg_808.setValue((void*) "(");
        
        
        
        result_peg_808 = rule_s(stream, result_peg_808.getPosition());
            if (result_peg_808.error()){
                goto out_peg_810;
            }
        
        
        
        result_peg_808 = rule_integer(stream, result_peg_808.getPosition());
            if (result_peg_808.error()){
                goto out_peg_810;
            }
            arg1 = result_peg_808.getValues();
        
        
        
        result_peg_808 = rule_s(stream, result_peg_808.getPosition());
            if (result_peg_808.error()){
                goto out_peg_810;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_808.getPosition()))){
                    result_peg_808.nextPosition();
                } else {
                    goto out_peg_810;
                }
            }
            result_peg_808.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_808.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_808;
    stream.update(result_peg_808.getPosition());
    
    
    return result_peg_808;
    out_peg_810:
    Result result_peg_817(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_817.getPosition()))){
                    result_peg_817.nextPosition();
                } else {
                    goto out_peg_819;
                }
            }
            result_peg_817.setValue((void*) "fvar");
            name = result_peg_817.getValues();
        
        
        
        result_peg_817 = rule_s(stream, result_peg_817.getPosition());
            if (result_peg_817.error()){
                goto out_peg_819;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_817.getPosition()))){
                    result_peg_817.nextPosition();
                } else {
                    goto out_peg_819;
                }
            }
            result_peg_817.setValue((void*) "(");
        
        
        
        result_peg_817 = rule_s(stream, result_peg_817.getPosition());
            if (result_peg_817.error()){
                goto out_peg_819;
            }
        
        
        
        result_peg_817 = rule_integer(stream, result_peg_817.getPosition());
            if (result_peg_817.error()){
                goto out_peg_819;
            }
            arg1 = result_peg_817.getValues();
        
        
        
        result_peg_817 = rule_s(stream, result_peg_817.getPosition());
            if (result_peg_817.error()){
                goto out_peg_819;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_817.getPosition()))){
                    result_peg_817.nextPosition();
                } else {
                    goto out_peg_819;
                }
            }
            result_peg_817.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_817.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_817;
    stream.update(result_peg_817.getPosition());
    
    
    return result_peg_817;
    out_peg_819:
    Result result_peg_826(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("enemynear"[i], stream.get(result_peg_826.getPosition()))){
                    result_peg_826.nextPosition();
                } else {
                    goto out_peg_828;
                }
            }
            result_peg_826.setValue((void*) "enemynear");
            name = result_peg_826.getValues();
        
        
        
        result_peg_826 = rule_s(stream, result_peg_826.getPosition());
            if (result_peg_826.error()){
                goto out_peg_828;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_826.getPosition()))){
                    result_peg_826.nextPosition();
                } else {
                    goto out_peg_828;
                }
            }
            result_peg_826.setValue((void*) "(");
        
        
        
        result_peg_826 = rule_s(stream, result_peg_826.getPosition());
            if (result_peg_826.error()){
                goto out_peg_828;
            }
        
        
        
        result_peg_826 = rule_integer(stream, result_peg_826.getPosition());
            if (result_peg_826.error()){
                goto out_peg_828;
            }
            arg1 = result_peg_826.getValues();
        
        
        
        result_peg_826 = rule_s(stream, result_peg_826.getPosition());
            if (result_peg_826.error()){
                goto out_peg_828;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_826.getPosition()))){
                    result_peg_826.nextPosition();
                } else {
                    goto out_peg_828;
                }
            }
            result_peg_826.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_826.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_826;
    stream.update(result_peg_826.getPosition());
    
    
    return result_peg_826;
    out_peg_828:
    Result result_peg_835(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("target"[i], stream.get(result_peg_835.getPosition()))){
                    result_peg_835.nextPosition();
                } else {
                    goto out_peg_837;
                }
            }
            result_peg_835.setValue((void*) "target");
            name = result_peg_835.getValues();
        
        
        
        result_peg_835 = rule_s(stream, result_peg_835.getPosition());
            if (result_peg_835.error()){
                goto out_peg_837;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_835.getPosition()))){
                    result_peg_835.nextPosition();
                } else {
                    goto out_peg_837;
                }
            }
            result_peg_835.setValue((void*) "(");
        
        
        
        result_peg_835 = rule_s(stream, result_peg_835.getPosition());
            if (result_peg_835.error()){
                goto out_peg_837;
            }
        
        
        
        result_peg_835 = rule_integer(stream, result_peg_835.getPosition());
            if (result_peg_835.error()){
                goto out_peg_837;
            }
            arg1 = result_peg_835.getValues();
        
        
        
        result_peg_835 = rule_s(stream, result_peg_835.getPosition());
            if (result_peg_835.error()){
                goto out_peg_837;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_835.getPosition()))){
                    result_peg_835.nextPosition();
                } else {
                    goto out_peg_837;
                }
            }
            result_peg_835.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_835.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_835;
    stream.update(result_peg_835.getPosition());
    
    
    return result_peg_835;
    out_peg_837:
    Result result_peg_844(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_844.getPosition()))){
                    result_peg_844.nextPosition();
                } else {
                    goto out_peg_846;
                }
            }
            result_peg_844.setValue((void*) "ishelper");
            name = result_peg_844.getValues();
        
        
        
        result_peg_844 = rule_s(stream, result_peg_844.getPosition());
            if (result_peg_844.error()){
                goto out_peg_846;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_844.getPosition()))){
                    result_peg_844.nextPosition();
                } else {
                    goto out_peg_846;
                }
            }
            result_peg_844.setValue((void*) "(");
        
        
        
        result_peg_844 = rule_s(stream, result_peg_844.getPosition());
            if (result_peg_844.error()){
                goto out_peg_846;
            }
        
        
        
        result_peg_844 = rule_integer(stream, result_peg_844.getPosition());
            if (result_peg_844.error()){
                goto out_peg_846;
            }
            arg1 = result_peg_844.getValues();
        
        
        
        result_peg_844 = rule_s(stream, result_peg_844.getPosition());
            if (result_peg_844.error()){
                goto out_peg_846;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_844.getPosition()))){
                    result_peg_844.nextPosition();
                } else {
                    goto out_peg_846;
                }
            }
            result_peg_844.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_844.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_844;
    stream.update(result_peg_844.getPosition());
    
    
    return result_peg_844;
    out_peg_846:
    Result result_peg_853(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numtarget"[i], stream.get(result_peg_853.getPosition()))){
                    result_peg_853.nextPosition();
                } else {
                    goto out_peg_855;
                }
            }
            result_peg_853.setValue((void*) "numtarget");
            name = result_peg_853.getValues();
        
        
        
        result_peg_853 = rule_s(stream, result_peg_853.getPosition());
            if (result_peg_853.error()){
                goto out_peg_855;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_853.getPosition()))){
                    result_peg_853.nextPosition();
                } else {
                    goto out_peg_855;
                }
            }
            result_peg_853.setValue((void*) "(");
        
        
        
        result_peg_853 = rule_s(stream, result_peg_853.getPosition());
            if (result_peg_853.error()){
                goto out_peg_855;
            }
        
        
        
        result_peg_853 = rule_expr_c(stream, result_peg_853.getPosition());
            if (result_peg_853.error()){
                goto out_peg_855;
            }
            arg1 = result_peg_853.getValues();
        
        
        
        result_peg_853 = rule_s(stream, result_peg_853.getPosition());
            if (result_peg_853.error()){
                goto out_peg_855;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_853.getPosition()))){
                    result_peg_853.nextPosition();
                } else {
                    goto out_peg_855;
                }
            }
            result_peg_853.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_853.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_853;
    stream.update(result_peg_853.getPosition());
    
    
    return result_peg_853;
    out_peg_855:
    Result result_peg_862(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_862.getPosition()))){
                    result_peg_862.nextPosition();
                } else {
                    goto out_peg_864;
                }
            }
            result_peg_862.setValue((void*) "animelemtime");
            name = result_peg_862.getValues();
        
        
        
        result_peg_862 = rule_s(stream, result_peg_862.getPosition());
            if (result_peg_862.error()){
                goto out_peg_864;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_862.getPosition()))){
                    result_peg_862.nextPosition();
                } else {
                    goto out_peg_864;
                }
            }
            result_peg_862.setValue((void*) "(");
        
        
        
        result_peg_862 = rule_s(stream, result_peg_862.getPosition());
            if (result_peg_862.error()){
                goto out_peg_864;
            }
        
        
        
        result_peg_862 = rule_expr_c(stream, result_peg_862.getPosition());
            if (result_peg_862.error()){
                goto out_peg_864;
            }
            arg1 = result_peg_862.getValues();
        
        
        
        result_peg_862 = rule_s(stream, result_peg_862.getPosition());
            if (result_peg_862.error()){
                goto out_peg_864;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_862.getPosition()))){
                    result_peg_862.nextPosition();
                } else {
                    goto out_peg_864;
                }
            }
            result_peg_862.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_862.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_862;
    stream.update(result_peg_862.getPosition());
    
    
    return result_peg_862;
    out_peg_864:
    Result result_peg_871(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_871.getPosition()))){
                    result_peg_871.nextPosition();
                } else {
                    goto out_peg_873;
                }
            }
            result_peg_871.setValue((void*) "animelemno");
            name = result_peg_871.getValues();
        
        
        
        result_peg_871 = rule_s(stream, result_peg_871.getPosition());
            if (result_peg_871.error()){
                goto out_peg_873;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_871.getPosition()))){
                    result_peg_871.nextPosition();
                } else {
                    goto out_peg_873;
                }
            }
            result_peg_871.setValue((void*) "(");
        
        
        
        result_peg_871 = rule_s(stream, result_peg_871.getPosition());
            if (result_peg_871.error()){
                goto out_peg_873;
            }
        
        
        
        result_peg_871 = rule_expr_c(stream, result_peg_871.getPosition());
            if (result_peg_871.error()){
                goto out_peg_873;
            }
            arg1 = result_peg_871.getValues();
        
        
        
        result_peg_871 = rule_s(stream, result_peg_871.getPosition());
            if (result_peg_871.error()){
                goto out_peg_873;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_871.getPosition()))){
                    result_peg_871.nextPosition();
                } else {
                    goto out_peg_873;
                }
            }
            result_peg_871.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_871.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_871;
    stream.update(result_peg_871.getPosition());
    
    
    return result_peg_871;
    out_peg_873:
    Result result_peg_880(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_880.getPosition()))){
                    result_peg_880.nextPosition();
                } else {
                    goto out_peg_882;
                }
            }
            result_peg_880.setValue((void*) "animexist");
            name = result_peg_880.getValues();
        
        
        
        result_peg_880 = rule_s(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_882;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_880.getPosition()))){
                    result_peg_880.nextPosition();
                } else {
                    goto out_peg_882;
                }
            }
            result_peg_880.setValue((void*) "(");
        
        
        
        result_peg_880 = rule_s(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_882;
            }
        
        
        
        result_peg_880 = rule_expr_c(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_882;
            }
            arg1 = result_peg_880.getValues();
        
        
        
        result_peg_880 = rule_s(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_882;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_880.getPosition()))){
                    result_peg_880.nextPosition();
                } else {
                    goto out_peg_882;
                }
            }
            result_peg_880.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_880.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_880;
    stream.update(result_peg_880.getPosition());
    
    
    return result_peg_880;
    out_peg_882:
    Result result_peg_889(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_889.getPosition()))){
                    result_peg_889.nextPosition();
                } else {
                    goto out_peg_891;
                }
            }
            result_peg_889.setValue((void*) "projguarded");
            name = result_peg_889.getValues();
        
        
        
        result_peg_889 = rule_s(stream, result_peg_889.getPosition());
            if (result_peg_889.error()){
                goto out_peg_891;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_889.getPosition()))){
                    result_peg_889.nextPosition();
                } else {
                    goto out_peg_891;
                }
            }
            result_peg_889.setValue((void*) "(");
        
        
        
        result_peg_889 = rule_s(stream, result_peg_889.getPosition());
            if (result_peg_889.error()){
                goto out_peg_891;
            }
        
        
        
        result_peg_889 = rule_expr_c(stream, result_peg_889.getPosition());
            if (result_peg_889.error()){
                goto out_peg_891;
            }
            arg1 = result_peg_889.getValues();
        
        
        
        result_peg_889 = rule_s(stream, result_peg_889.getPosition());
            if (result_peg_889.error()){
                goto out_peg_891;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_889.getPosition()))){
                    result_peg_889.nextPosition();
                } else {
                    goto out_peg_891;
                }
            }
            result_peg_889.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_889.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_889;
    stream.update(result_peg_889.getPosition());
    
    
    return result_peg_889;
    out_peg_891:
    Result result_peg_898(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projhittime"[i], stream.get(result_peg_898.getPosition()))){
                    result_peg_898.nextPosition();
                } else {
                    goto out_peg_900;
                }
            }
            result_peg_898.setValue((void*) "projhittime");
            name = result_peg_898.getValues();
        
        
        
        result_peg_898 = rule_s(stream, result_peg_898.getPosition());
            if (result_peg_898.error()){
                goto out_peg_900;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_898.getPosition()))){
                    result_peg_898.nextPosition();
                } else {
                    goto out_peg_900;
                }
            }
            result_peg_898.setValue((void*) "(");
        
        
        
        result_peg_898 = rule_s(stream, result_peg_898.getPosition());
            if (result_peg_898.error()){
                goto out_peg_900;
            }
        
        
        
        result_peg_898 = rule_expr_c(stream, result_peg_898.getPosition());
            if (result_peg_898.error()){
                goto out_peg_900;
            }
            arg1 = result_peg_898.getValues();
        
        
        
        result_peg_898 = rule_s(stream, result_peg_898.getPosition());
            if (result_peg_898.error()){
                goto out_peg_900;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_898.getPosition()))){
                    result_peg_898.nextPosition();
                } else {
                    goto out_peg_900;
                }
            }
            result_peg_898.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_898.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_898;
    stream.update(result_peg_898.getPosition());
    
    
    return result_peg_898;
    out_peg_900:
    Result result_peg_907(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_907.getPosition()))){
                    result_peg_907.nextPosition();
                } else {
                    goto out_peg_909;
                }
            }
            result_peg_907.setValue((void*) "projcontact");
            name = result_peg_907.getValues();
        
        
        
        result_peg_907 = rule_s(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_907.getPosition()))){
                    result_peg_907.nextPosition();
                } else {
                    goto out_peg_909;
                }
            }
            result_peg_907.setValue((void*) "(");
        
        
        
        result_peg_907 = rule_s(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        result_peg_907 = rule_expr_c(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
            arg1 = result_peg_907.getValues();
        
        
        
        result_peg_907 = rule_s(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_907.getPosition()))){
                    result_peg_907.nextPosition();
                } else {
                    goto out_peg_909;
                }
            }
            result_peg_907.setValue((void*) ")");
        
        
        
        result_peg_907 = rule_s(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_907.getPosition()))){
                    result_peg_907.nextPosition();
                } else {
                    goto out_peg_909;
                }
            }
            result_peg_907.setValue((void*) "=");
        
        
        
        result_peg_907 = rule_s(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        result_peg_907 = rule_integer(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        result_peg_907 = rule_function_rest(stream, result_peg_907.getPosition());
            if (result_peg_907.error()){
                goto out_peg_909;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_907.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_907;
    stream.update(result_peg_907.getPosition());
    
    
    return result_peg_907;
    out_peg_909:
    Result result_peg_921(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_921.getPosition()))){
                    result_peg_921.nextPosition();
                } else {
                    goto out_peg_923;
                }
            }
            result_peg_921.setValue((void*) "animelem");
            name = result_peg_921.getValues();
        
        
        
        result_peg_921 = rule_s(stream, result_peg_921.getPosition());
            if (result_peg_921.error()){
                goto out_peg_923;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_921.getPosition()))){
                    result_peg_921.nextPosition();
                } else {
                    goto out_peg_923;
                }
            }
            result_peg_921.setValue((void*) "=");
        
        
        
        result_peg_921 = rule_s(stream, result_peg_921.getPosition());
            if (result_peg_921.error()){
                goto out_peg_923;
            }
        
        
        
        result_peg_921 = rule_integer(stream, result_peg_921.getPosition());
            if (result_peg_921.error()){
                goto out_peg_923;
            }
            arg1 = result_peg_921.getValues();
        
        
        
        result_peg_921 = rule_function_rest(stream, result_peg_921.getPosition());
            if (result_peg_921.error()){
                goto out_peg_923;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction(name, arg1);
                result_peg_921.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_921;
    stream.update(result_peg_921.getPosition());
    
    
    return result_peg_921;
    out_peg_923:
    Result result_peg_929(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_929.getPosition()))){
                    result_peg_929.nextPosition();
                } else {
                    goto out_peg_931;
                }
            }
            result_peg_929.setValue((void*) "numhelper");
            name = result_peg_929.getValues();
        
        
        
        result_peg_929 = rule_s(stream, result_peg_929.getPosition());
            if (result_peg_929.error()){
                goto out_peg_931;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_929.getPosition()))){
                    result_peg_929.nextPosition();
                } else {
                    goto out_peg_931;
                }
            }
            result_peg_929.setValue((void*) "(");
        
        
        
        result_peg_929 = rule_s(stream, result_peg_929.getPosition());
            if (result_peg_929.error()){
                goto out_peg_931;
            }
        
        
        
        result_peg_929 = rule_expr_c(stream, result_peg_929.getPosition());
            if (result_peg_929.error()){
                goto out_peg_931;
            }
            arg1 = result_peg_929.getValues();
        
        
        
        result_peg_929 = rule_s(stream, result_peg_929.getPosition());
            if (result_peg_929.error()){
                goto out_peg_931;
            }
        
        
        
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
                value = makeFunction(name, arg1);
                result_peg_929.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_929;
    stream.update(result_peg_929.getPosition());
    
    
    return result_peg_929;
    out_peg_931:
    Result result_peg_938(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_938.getPosition()))){
                    result_peg_938.nextPosition();
                } else {
                    goto out_peg_940;
                }
            }
            result_peg_938.setValue((void*) "helper");
            name = result_peg_938.getValues();
        
        
        
        result_peg_938 = rule_s(stream, result_peg_938.getPosition());
            if (result_peg_938.error()){
                goto out_peg_940;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_938.getPosition()))){
                    result_peg_938.nextPosition();
                } else {
                    goto out_peg_940;
                }
            }
            result_peg_938.setValue((void*) "(");
        
        
        
        result_peg_938 = rule_s(stream, result_peg_938.getPosition());
            if (result_peg_938.error()){
                goto out_peg_940;
            }
        
        
        
        result_peg_938 = rule_expr_c(stream, result_peg_938.getPosition());
            if (result_peg_938.error()){
                goto out_peg_940;
            }
            arg1 = result_peg_938.getValues();
        
        
        
        result_peg_938 = rule_s(stream, result_peg_938.getPosition());
            if (result_peg_938.error()){
                goto out_peg_940;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_938.getPosition()))){
                    result_peg_938.nextPosition();
                } else {
                    goto out_peg_940;
                }
            }
            result_peg_938.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_938.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = result_peg_938;
    stream.update(result_peg_938.getPosition());
    
    
    return result_peg_938;
    out_peg_940:
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_948 = stream.getColumn(position);
    if (column_peg_948.chunk10 != 0 && column_peg_948.chunk10->chunk_function_rest.calculated()){
        return column_peg_948.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_958(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_949(myposition);
    
    {
    
        result_peg_949 = rule_s(stream, result_peg_949.getPosition());
            if (result_peg_949.error()){
                goto out_peg_951;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_949.getPosition()))){
                    result_peg_949.nextPosition();
                } else {
                    goto out_peg_951;
                }
            }
            result_peg_949.setValue((void*) ",");
        
        
        
        result_peg_949 = rule_s(stream, result_peg_949.getPosition());
            if (result_peg_949.error()){
                goto out_peg_951;
            }
        
        
        
        int save_peg_955 = result_peg_949.getPosition();
            
            result_peg_949 = rule_all_compare(stream, result_peg_949.getPosition());
            if (result_peg_949.error()){
                
                result_peg_949 = Result(save_peg_955);
                result_peg_949.setValue((void*) 0);
                
            }
        
        
        
        result_peg_949 = rule_s(stream, result_peg_949.getPosition());
            if (result_peg_949.error()){
                goto out_peg_951;
            }
        
        
        
        result_peg_949 = rule_value(stream, result_peg_949.getPosition());
            if (result_peg_949.error()){
                goto out_peg_951;
            }
        
        
    }
    
    if (column_peg_948.chunk10 == 0){
        column_peg_948.chunk10 = new Chunk10();
    }
    column_peg_948.chunk10->chunk_function_rest = result_peg_949;
    stream.update(result_peg_949.getPosition());
    
    
    return result_peg_949;
    out_peg_951:
    Result result_peg_957(myposition);
    
    
    
    if (column_peg_948.chunk10 == 0){
        column_peg_948.chunk10 = new Chunk10();
    }
    column_peg_948.chunk10->chunk_function_rest = result_peg_957;
    stream.update(result_peg_957.getPosition());
    
    
    return result_peg_957;
    
    if (column_peg_948.chunk10 == 0){
        column_peg_948.chunk10 = new Chunk10();
    }
    column_peg_948.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_959 = stream.getColumn(position);
    if (column_peg_959.chunk10 != 0 && column_peg_959.chunk10->chunk_keys.calculated()){
        return column_peg_959.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_962(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_960(myposition);
    
    result_peg_960 = rule_key_value_list(stream, result_peg_960.getPosition());
    if (result_peg_960.error()){
        goto out_peg_961;
    }
    
    if (column_peg_959.chunk10 == 0){
        column_peg_959.chunk10 = new Chunk10();
    }
    column_peg_959.chunk10->chunk_keys = result_peg_960;
    stream.update(result_peg_960.getPosition());
    
    
    return result_peg_960;
    out_peg_961:
    
    if (column_peg_959.chunk10 == 0){
        column_peg_959.chunk10 = new Chunk10();
    }
    column_peg_959.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_963 = stream.getColumn(position);
    if (column_peg_963.chunk10 != 0 && column_peg_963.chunk10->chunk_key_value_list.calculated()){
        return column_peg_963.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_973(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_964(myposition);
    
    {
    
        result_peg_964 = rule_key(stream, result_peg_964.getPosition());
            if (result_peg_964.error()){
                goto out_peg_966;
            }
            first = result_peg_964.getValues();
        
        
        
        result_peg_964.reset();
            do{
                Result result_peg_969(result_peg_964.getPosition());
                {
                
                    result_peg_969 = rule_s(stream, result_peg_969.getPosition());
                        if (result_peg_969.error()){
                            goto loop_peg_968;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_969.getPosition()))){
                                result_peg_969.nextPosition();
                            } else {
                                goto loop_peg_968;
                            }
                        }
                        result_peg_969.setValue((void*) ",");
                    
                    
                    
                    result_peg_969 = rule_s(stream, result_peg_969.getPosition());
                        if (result_peg_969.error()){
                            goto loop_peg_968;
                        }
                    
                    
                    
                    result_peg_969 = rule_key(stream, result_peg_969.getPosition());
                        if (result_peg_969.error()){
                            goto loop_peg_968;
                        }
                    
                    
                }
                result_peg_964.addResult(result_peg_969);
            } while (true);
            loop_peg_968:
            ;
            rest = result_peg_964.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_964.setValue(value);
            }
        
        
    }
    
    if (column_peg_963.chunk10 == 0){
        column_peg_963.chunk10 = new Chunk10();
    }
    column_peg_963.chunk10->chunk_key_value_list = result_peg_964;
    stream.update(result_peg_964.getPosition());
    
    
    return result_peg_964;
    out_peg_966:
    
    if (column_peg_963.chunk10 == 0){
        column_peg_963.chunk10 = new Chunk10();
    }
    column_peg_963.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_974 = stream.getColumn(position);
    if (column_peg_974.chunk10 != 0 && column_peg_974.chunk10->chunk_key.calculated()){
        return column_peg_974.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_982(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_975(myposition);
    
    {
    
        result_peg_975 = rule_key_real(stream, result_peg_975.getPosition());
            if (result_peg_975.error()){
                goto out_peg_977;
            }
        
        Result result_peg_976 = result_peg_975;
        
        result_peg_975 = rule_key_rest(stream, result_peg_975.getPosition(), result_peg_976.getValues());
            if (result_peg_975.error()){
                goto out_peg_977;
            }
            ok = result_peg_975.getValues();
        
        
        
        Result result_peg_981(result_peg_975);
            result_peg_981 = rule_identifier(stream, result_peg_981.getPosition());
            if (result_peg_981.error()){
                goto not_peg_980;
            }
            goto out_peg_977;
            not_peg_980:
            result_peg_975.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_975.setValue(value);
            }
        
        
    }
    
    if (column_peg_974.chunk10 == 0){
        column_peg_974.chunk10 = new Chunk10();
    }
    column_peg_974.chunk10->chunk_key = result_peg_975;
    stream.update(result_peg_975.getPosition());
    
    
    return result_peg_975;
    out_peg_977:
    
    if (column_peg_974.chunk10 == 0){
        column_peg_974.chunk10 = new Chunk10();
    }
    column_peg_974.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_993(stream, "key_rest");
    int myposition = position;
    tail_peg_985:
    Value another;
    Value new_left;
    Result result_peg_984(myposition);
    {
    
        result_peg_984 = rule_s(stream, result_peg_984.getPosition());
            if (result_peg_984.error()){
                goto out_peg_987;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_984.getPosition()))){
                    result_peg_984.nextPosition();
                } else {
                    goto out_peg_987;
                }
            }
            result_peg_984.setValue((void*) "+");
        
        
        
        result_peg_984 = rule_key_real(stream, result_peg_984.getPosition());
            if (result_peg_984.error()){
                goto out_peg_987;
            }
            another = result_peg_984.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_984.setValue(value);
            }
            new_left = result_peg_984.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_984.getPosition();
    goto tail_peg_985;
    out_peg_987:
    Result result_peg_991(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_991.setValue(value);
            }
        
        
    }
    
    
    return result_peg_991;
    
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_994 = stream.getColumn(position);
    if (column_peg_994.chunk11 != 0 && column_peg_994.chunk11->chunk_key_real.calculated()){
        return column_peg_994.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_1001(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_995(myposition);
    
    {
    
        result_peg_995.reset();
            do{
                Result result_peg_998(result_peg_995.getPosition());
                result_peg_998 = rule_key_modifier(stream, result_peg_998.getPosition());
                if (result_peg_998.error()){
                    goto loop_peg_997;
                }
                result_peg_995.addResult(result_peg_998);
            } while (true);
            loop_peg_997:
            ;
            mods = result_peg_995.getValues();
        
        
        
        result_peg_995 = rule_key_name(stream, result_peg_995.getPosition());
            if (result_peg_995.error()){
                goto out_peg_1000;
            }
            name = result_peg_995.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_995.setValue(value);
            }
        
        
    }
    
    if (column_peg_994.chunk11 == 0){
        column_peg_994.chunk11 = new Chunk11();
    }
    column_peg_994.chunk11->chunk_key_real = result_peg_995;
    stream.update(result_peg_995.getPosition());
    
    
    return result_peg_995;
    out_peg_1000:
    
    if (column_peg_994.chunk11 == 0){
        column_peg_994.chunk11 = new Chunk11();
    }
    column_peg_994.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_1002 = stream.getColumn(position);
    if (column_peg_1002.chunk11 != 0 && column_peg_1002.chunk11->chunk_key_modifier.calculated()){
        return column_peg_1002.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_1023(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_1003(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1003.getPosition()))){
                    result_peg_1003.nextPosition();
                } else {
                    goto out_peg_1005;
                }
            }
            result_peg_1003.setValue((void*) "~");
        
        
        
        result_peg_1003.reset();
            do{
                Result result_peg_1008(result_peg_1003.getPosition());
                {
                    
                    char letter_peg_1013 = stream.get(result_peg_1008.getPosition());
                    if (letter_peg_1013 != '\0' && strchr("0123456789", letter_peg_1013) != NULL){
                        result_peg_1008.nextPosition();
                        result_peg_1008.setValue((void*) (long) letter_peg_1013);
                    } else {
                        goto out_peg_1012;
                    }
                    
                }
                goto success_peg_1009;
                out_peg_1012:
                goto loop_peg_1007;
                success_peg_1009:
                ;
                result_peg_1003.addResult(result_peg_1008);
            } while (true);
            loop_peg_1007:
            ;
            num = result_peg_1003.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_1003.setValue(value);
            }
        
        
    }
    
    if (column_peg_1002.chunk11 == 0){
        column_peg_1002.chunk11 = new Chunk11();
    }
    column_peg_1002.chunk11->chunk_key_modifier = result_peg_1003;
    stream.update(result_peg_1003.getPosition());
    
    
    return result_peg_1003;
    out_peg_1005:
    Result result_peg_1014(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_1014.setValue(value);
            }
        
        
    }
    
    if (column_peg_1002.chunk11 == 0){
        column_peg_1002.chunk11 = new Chunk11();
    }
    column_peg_1002.chunk11->chunk_key_modifier = result_peg_1014;
    stream.update(result_peg_1014.getPosition());
    
    
    return result_peg_1014;
    out_peg_1016:
    Result result_peg_1017(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1017.getPosition()))){
                    result_peg_1017.nextPosition();
                } else {
                    goto out_peg_1019;
                }
            }
            result_peg_1017.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_1017.setValue(value);
            }
        
        
    }
    
    if (column_peg_1002.chunk11 == 0){
        column_peg_1002.chunk11 = new Chunk11();
    }
    column_peg_1002.chunk11->chunk_key_modifier = result_peg_1017;
    stream.update(result_peg_1017.getPosition());
    
    
    return result_peg_1017;
    out_peg_1019:
    Result result_peg_1020(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1020.getPosition()))){
                    result_peg_1020.nextPosition();
                } else {
                    goto out_peg_1022;
                }
            }
            result_peg_1020.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_1020.setValue(value);
            }
        
        
    }
    
    if (column_peg_1002.chunk11 == 0){
        column_peg_1002.chunk11 = new Chunk11();
    }
    column_peg_1002.chunk11->chunk_key_modifier = result_peg_1020;
    stream.update(result_peg_1020.getPosition());
    
    
    return result_peg_1020;
    out_peg_1022:
    
    if (column_peg_1002.chunk11 == 0){
        column_peg_1002.chunk11 = new Chunk11();
    }
    column_peg_1002.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_1024 = stream.getColumn(position);
    if (column_peg_1024.chunk11 != 0 && column_peg_1024.chunk11->chunk_key_name.calculated()){
        return column_peg_1024.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_1055(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_1025(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_1025.getPosition()))){
            result_peg_1025.nextPosition();
        } else {
            goto out_peg_1026;
        }
    }
    result_peg_1025.setValue((void*) "DB");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1025;
    stream.update(result_peg_1025.getPosition());
    
    
    return result_peg_1025;
    out_peg_1026:
    Result result_peg_1027(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_1027.getPosition()))){
            result_peg_1027.nextPosition();
        } else {
            goto out_peg_1028;
        }
    }
    result_peg_1027.setValue((void*) "B");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1027;
    stream.update(result_peg_1027.getPosition());
    
    
    return result_peg_1027;
    out_peg_1028:
    Result result_peg_1029(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_1029.getPosition()))){
            result_peg_1029.nextPosition();
        } else {
            goto out_peg_1030;
        }
    }
    result_peg_1029.setValue((void*) "DF");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1029;
    stream.update(result_peg_1029.getPosition());
    
    
    return result_peg_1029;
    out_peg_1030:
    Result result_peg_1031(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_1031.getPosition()))){
            result_peg_1031.nextPosition();
        } else {
            goto out_peg_1032;
        }
    }
    result_peg_1031.setValue((void*) "D");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1031;
    stream.update(result_peg_1031.getPosition());
    
    
    return result_peg_1031;
    out_peg_1032:
    Result result_peg_1033(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_1033.getPosition()))){
            result_peg_1033.nextPosition();
        } else {
            goto out_peg_1034;
        }
    }
    result_peg_1033.setValue((void*) "F");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1033;
    stream.update(result_peg_1033.getPosition());
    
    
    return result_peg_1033;
    out_peg_1034:
    Result result_peg_1035(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_1035.getPosition()))){
            result_peg_1035.nextPosition();
        } else {
            goto out_peg_1036;
        }
    }
    result_peg_1035.setValue((void*) "UF");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1035;
    stream.update(result_peg_1035.getPosition());
    
    
    return result_peg_1035;
    out_peg_1036:
    Result result_peg_1037(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_1037.getPosition()))){
            result_peg_1037.nextPosition();
        } else {
            goto out_peg_1038;
        }
    }
    result_peg_1037.setValue((void*) "UB");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1037;
    stream.update(result_peg_1037.getPosition());
    
    
    return result_peg_1037;
    out_peg_1038:
    Result result_peg_1039(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_1039.getPosition()))){
            result_peg_1039.nextPosition();
        } else {
            goto out_peg_1040;
        }
    }
    result_peg_1039.setValue((void*) "U");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1039;
    stream.update(result_peg_1039.getPosition());
    
    
    return result_peg_1039;
    out_peg_1040:
    Result result_peg_1041(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_1041.getPosition()))){
            result_peg_1041.nextPosition();
        } else {
            goto out_peg_1042;
        }
    }
    result_peg_1041.setValue((void*) "a");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1041;
    stream.update(result_peg_1041.getPosition());
    
    
    return result_peg_1041;
    out_peg_1042:
    Result result_peg_1043(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_1043.getPosition()))){
            result_peg_1043.nextPosition();
        } else {
            goto out_peg_1044;
        }
    }
    result_peg_1043.setValue((void*) "b");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1043;
    stream.update(result_peg_1043.getPosition());
    
    
    return result_peg_1043;
    out_peg_1044:
    Result result_peg_1045(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_1045.getPosition()))){
            result_peg_1045.nextPosition();
        } else {
            goto out_peg_1046;
        }
    }
    result_peg_1045.setValue((void*) "c");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1045;
    stream.update(result_peg_1045.getPosition());
    
    
    return result_peg_1045;
    out_peg_1046:
    Result result_peg_1047(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_1047.getPosition()))){
            result_peg_1047.nextPosition();
        } else {
            goto out_peg_1048;
        }
    }
    result_peg_1047.setValue((void*) "x");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1047;
    stream.update(result_peg_1047.getPosition());
    
    
    return result_peg_1047;
    out_peg_1048:
    Result result_peg_1049(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_1049.getPosition()))){
            result_peg_1049.nextPosition();
        } else {
            goto out_peg_1050;
        }
    }
    result_peg_1049.setValue((void*) "y");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1049;
    stream.update(result_peg_1049.getPosition());
    
    
    return result_peg_1049;
    out_peg_1050:
    Result result_peg_1051(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_1051.getPosition()))){
            result_peg_1051.nextPosition();
        } else {
            goto out_peg_1052;
        }
    }
    result_peg_1051.setValue((void*) "z");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1051;
    stream.update(result_peg_1051.getPosition());
    
    
    return result_peg_1051;
    out_peg_1052:
    Result result_peg_1053(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_1053.getPosition()))){
            result_peg_1053.nextPosition();
        } else {
            goto out_peg_1054;
        }
    }
    result_peg_1053.setValue((void*) "s");
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = result_peg_1053;
    stream.update(result_peg_1053.getPosition());
    
    
    return result_peg_1053;
    out_peg_1054:
    
    if (column_peg_1024.chunk11 == 0){
        column_peg_1024.chunk11 = new Chunk11();
    }
    column_peg_1024.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1056 = stream.getColumn(position);
    if (column_peg_1056.chunk11 != 0 && column_peg_1056.chunk11->chunk_value.calculated()){
        return column_peg_1056.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_1074(stream, "value");
    int myposition = position;
    
    
    Result result_peg_1057(myposition);
    
    result_peg_1057 = rule_float(stream, result_peg_1057.getPosition());
    if (result_peg_1057.error()){
        goto out_peg_1058;
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1057;
    stream.update(result_peg_1057.getPosition());
    
    
    return result_peg_1057;
    out_peg_1058:
    Result result_peg_1059(myposition);
    
    result_peg_1059 = rule_integer(stream, result_peg_1059.getPosition());
    if (result_peg_1059.error()){
        goto out_peg_1060;
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1059;
    stream.update(result_peg_1059.getPosition());
    
    
    return result_peg_1059;
    out_peg_1060:
    Result result_peg_1061(myposition);
    
    result_peg_1061 = rule_keyword(stream, result_peg_1061.getPosition());
    if (result_peg_1061.error()){
        goto out_peg_1062;
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1061;
    stream.update(result_peg_1061.getPosition());
    
    
    return result_peg_1061;
    out_peg_1062:
    Result result_peg_1063(myposition);
    
    {
    
        Result result_peg_1066(result_peg_1063);
            result_peg_1066 = rule_keyword(stream, result_peg_1066.getPosition());
            if (result_peg_1066.error()){
                goto not_peg_1065;
            }
            goto out_peg_1067;
            not_peg_1065:
            result_peg_1063.setValue((void*)0);
        
        
        
        result_peg_1063 = rule_identifier(stream, result_peg_1063.getPosition());
            if (result_peg_1063.error()){
                goto out_peg_1067;
            }
        
        
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1063;
    stream.update(result_peg_1063.getPosition());
    
    
    return result_peg_1063;
    out_peg_1067:
    Result result_peg_1068(myposition);
    
    result_peg_1068 = rule_range(stream, result_peg_1068.getPosition());
    if (result_peg_1068.error()){
        goto out_peg_1069;
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1068;
    stream.update(result_peg_1068.getPosition());
    
    
    return result_peg_1068;
    out_peg_1069:
    Result result_peg_1070(myposition);
    
    result_peg_1070 = rule_string(stream, result_peg_1070.getPosition());
    if (result_peg_1070.error()){
        goto out_peg_1071;
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1070;
    stream.update(result_peg_1070.getPosition());
    
    
    return result_peg_1070;
    out_peg_1071:
    Result result_peg_1072(myposition);
    
    result_peg_1072 = rule_hitflag(stream, result_peg_1072.getPosition());
    if (result_peg_1072.error()){
        goto out_peg_1073;
    }
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = result_peg_1072;
    stream.update(result_peg_1072.getPosition());
    
    
    return result_peg_1072;
    out_peg_1073:
    
    if (column_peg_1056.chunk11 == 0){
        column_peg_1056.chunk11 = new Chunk11();
    }
    column_peg_1056.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_1075 = stream.getColumn(position);
    if (column_peg_1075.chunk11 != 0 && column_peg_1075.chunk11->chunk_hitflag.calculated()){
        return column_peg_1075.chunk11->chunk_hitflag;
    }
    
    RuleTrace trace_peg_1082(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_1076(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("M-"[i], stream.get(result_peg_1076.getPosition()))){
                    result_peg_1076.nextPosition();
                } else {
                    goto out_peg_1078;
                }
            }
            result_peg_1076.setValue((void*) "M-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("M-");
                result_peg_1076.setValue(value);
            }
        
        
    }
    
    if (column_peg_1075.chunk11 == 0){
        column_peg_1075.chunk11 = new Chunk11();
    }
    column_peg_1075.chunk11->chunk_hitflag = result_peg_1076;
    stream.update(result_peg_1076.getPosition());
    
    
    return result_peg_1076;
    out_peg_1078:
    Result result_peg_1079(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("A-"[i], stream.get(result_peg_1079.getPosition()))){
                    result_peg_1079.nextPosition();
                } else {
                    goto out_peg_1081;
                }
            }
            result_peg_1079.setValue((void*) "A-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("A-");
                result_peg_1079.setValue(value);
            }
        
        
    }
    
    if (column_peg_1075.chunk11 == 0){
        column_peg_1075.chunk11 = new Chunk11();
    }
    column_peg_1075.chunk11->chunk_hitflag = result_peg_1079;
    stream.update(result_peg_1079.getPosition());
    
    
    return result_peg_1079;
    out_peg_1081:
    
    if (column_peg_1075.chunk11 == 0){
        column_peg_1075.chunk11 = new Chunk11();
    }
    column_peg_1075.chunk11->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_1083 = stream.getColumn(position);
    if (column_peg_1083.chunk12 != 0 && column_peg_1083.chunk12->chunk_keyword.calculated()){
        return column_peg_1083.chunk12->chunk_keyword;
    }
    
    RuleTrace trace_peg_1090(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_1084(myposition);
    
    {
    
        result_peg_1084 = rule_keyword_real(stream, result_peg_1084.getPosition());
            if (result_peg_1084.error()){
                goto out_peg_1086;
            }
        
        Result result_peg_1085 = result_peg_1084;
        
        Result result_peg_1089(result_peg_1084);
            result_peg_1089 = rule_alpha_digit(stream, result_peg_1089.getPosition());
            if (result_peg_1089.error()){
                goto not_peg_1088;
            }
            goto out_peg_1086;
            not_peg_1088:
            result_peg_1084.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1085.getValues();
                result_peg_1084.setValue(value);
            }
        
        
    }
    
    if (column_peg_1083.chunk12 == 0){
        column_peg_1083.chunk12 = new Chunk12();
    }
    column_peg_1083.chunk12->chunk_keyword = result_peg_1084;
    stream.update(result_peg_1084.getPosition());
    
    
    return result_peg_1084;
    out_peg_1086:
    
    if (column_peg_1083.chunk12 == 0){
        column_peg_1083.chunk12 = new Chunk12();
    }
    column_peg_1083.chunk12->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_1091 = stream.getColumn(position);
    if (column_peg_1091.chunk12 != 0 && column_peg_1091.chunk12->chunk_keyword_real.calculated()){
        return column_peg_1091.chunk12->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_1169(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_1092(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1092.getPosition()))){
                    result_peg_1092.nextPosition();
                } else {
                    goto out_peg_1094;
                }
            }
            result_peg_1092.setValue((void*) "vel");
        
        
        
        result_peg_1092 = rule_s(stream, result_peg_1092.getPosition());
            if (result_peg_1092.error()){
                goto out_peg_1094;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1092.getPosition()))){
                    result_peg_1092.nextPosition();
                } else {
                    goto out_peg_1094;
                }
            }
            result_peg_1092.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_1092.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1092;
    stream.update(result_peg_1092.getPosition());
    
    
    return result_peg_1092;
    out_peg_1094:
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
                if (compareCharCase("x"[i], stream.get(result_peg_1097.getPosition()))){
                    result_peg_1097.nextPosition();
                } else {
                    goto out_peg_1099;
                }
            }
            result_peg_1097.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_1097.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1097;
    stream.update(result_peg_1097.getPosition());
    
    
    return result_peg_1097;
    out_peg_1099:
    Result result_peg_1102(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1102.getPosition()))){
                    result_peg_1102.nextPosition();
                } else {
                    goto out_peg_1104;
                }
            }
            result_peg_1102.setValue((void*) "pos");
        
        
        
        result_peg_1102 = rule_s(stream, result_peg_1102.getPosition());
            if (result_peg_1102.error()){
                goto out_peg_1104;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1102.getPosition()))){
                    result_peg_1102.nextPosition();
                } else {
                    goto out_peg_1104;
                }
            }
            result_peg_1102.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_1102.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1102;
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
                if (compareCharCase("x"[i], stream.get(result_peg_1107.getPosition()))){
                    result_peg_1107.nextPosition();
                } else {
                    goto out_peg_1109;
                }
            }
            result_peg_1107.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_1107.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1107;
    stream.update(result_peg_1107.getPosition());
    
    
    return result_peg_1107;
    out_peg_1109:
    Result result_peg_1112(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1112.getPosition()))){
                    result_peg_1112.nextPosition();
                } else {
                    goto out_peg_1114;
                }
            }
            result_peg_1112.setValue((void*) "p2dist");
        
        
        
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
                value = makeKeyword("p2dist x");
                result_peg_1112.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1112;
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
                if (compareCharCase("y"[i], stream.get(result_peg_1117.getPosition()))){
                    result_peg_1117.nextPosition();
                } else {
                    goto out_peg_1119;
                }
            }
            result_peg_1117.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_1117.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1117;
    stream.update(result_peg_1117.getPosition());
    
    
    return result_peg_1117;
    out_peg_1119:
    Result result_peg_1122(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1122.getPosition()))){
                    result_peg_1122.nextPosition();
                } else {
                    goto out_peg_1124;
                }
            }
            result_peg_1122.setValue((void*) "p1dist");
        
        
        
        result_peg_1122 = rule_s(stream, result_peg_1122.getPosition());
            if (result_peg_1122.error()){
                goto out_peg_1124;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1122.getPosition()))){
                    result_peg_1122.nextPosition();
                } else {
                    goto out_peg_1124;
                }
            }
            result_peg_1122.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_1122.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1122;
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
                if (compareCharCase("y"[i], stream.get(result_peg_1127.getPosition()))){
                    result_peg_1127.nextPosition();
                } else {
                    goto out_peg_1129;
                }
            }
            result_peg_1127.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_1127.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1127;
    stream.update(result_peg_1127.getPosition());
    
    
    return result_peg_1127;
    out_peg_1129:
    Result result_peg_1132(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1132.getPosition()))){
                    result_peg_1132.nextPosition();
                } else {
                    goto out_peg_1134;
                }
            }
            result_peg_1132.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1132 = rule_s(stream, result_peg_1132.getPosition());
            if (result_peg_1132.error()){
                goto out_peg_1134;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1132.getPosition()))){
                    result_peg_1132.nextPosition();
                } else {
                    goto out_peg_1134;
                }
            }
            result_peg_1132.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1132.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1132;
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
                if (compareCharCase("y"[i], stream.get(result_peg_1137.getPosition()))){
                    result_peg_1137.nextPosition();
                } else {
                    goto out_peg_1139;
                }
            }
            result_peg_1137.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1137.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1137;
    stream.update(result_peg_1137.getPosition());
    
    
    return result_peg_1137;
    out_peg_1139:
    Result result_peg_1142(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1142.getPosition()))){
                    result_peg_1142.nextPosition();
                } else {
                    goto out_peg_1144;
                }
            }
            result_peg_1142.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1142 = rule_s(stream, result_peg_1142.getPosition());
            if (result_peg_1142.error()){
                goto out_peg_1144;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1142.getPosition()))){
                    result_peg_1142.nextPosition();
                } else {
                    goto out_peg_1144;
                }
            }
            result_peg_1142.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1142.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1142;
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
                if (compareCharCase("y"[i], stream.get(result_peg_1147.getPosition()))){
                    result_peg_1147.nextPosition();
                } else {
                    goto out_peg_1149;
                }
            }
            result_peg_1147.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1147.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1147;
    stream.update(result_peg_1147.getPosition());
    
    
    return result_peg_1147;
    out_peg_1149:
    Result result_peg_1152(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1152.getPosition()))){
                    result_peg_1152.nextPosition();
                } else {
                    goto out_peg_1154;
                }
            }
            result_peg_1152.setValue((void*) "parentdist");
        
        
        
        result_peg_1152 = rule_s(stream, result_peg_1152.getPosition());
            if (result_peg_1152.error()){
                goto out_peg_1154;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1152.getPosition()))){
                    result_peg_1152.nextPosition();
                } else {
                    goto out_peg_1154;
                }
            }
            result_peg_1152.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1152.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1152;
    stream.update(result_peg_1152.getPosition());
    
    
    return result_peg_1152;
    out_peg_1154:
    Result result_peg_1157(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1157.getPosition()))){
                    result_peg_1157.nextPosition();
                } else {
                    goto out_peg_1159;
                }
            }
            result_peg_1157.setValue((void*) "screenpos");
        
        
        
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
                value = makeKeyword("screenpos x");
                result_peg_1157.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1157;
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
                if (compareCharCase("y"[i], stream.get(result_peg_1162.getPosition()))){
                    result_peg_1162.nextPosition();
                } else {
                    goto out_peg_1164;
                }
            }
            result_peg_1162.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1162.setValue(value);
            }
        
        
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1162;
    stream.update(result_peg_1162.getPosition());
    
    
    return result_peg_1162;
    out_peg_1164:
    Result result_peg_1167(myposition);
    
    result_peg_1167 = rule_hitflag(stream, result_peg_1167.getPosition());
    if (result_peg_1167.error()){
        goto out_peg_1168;
    }
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = result_peg_1167;
    stream.update(result_peg_1167.getPosition());
    
    
    return result_peg_1167;
    out_peg_1168:
    
    if (column_peg_1091.chunk12 == 0){
        column_peg_1091.chunk12 = new Chunk12();
    }
    column_peg_1091.chunk12->chunk_keyword_real = errorResult;
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

        
