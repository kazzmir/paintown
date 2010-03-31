

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
    
    RuleTrace trace_peg_189(stream, "assignment");
    int myposition = position;
    
    Value all;
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
    
        result_peg_161 = rule_identifier(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
            name = result_peg_161.getValues();
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_161.getPosition()))){
                    result_peg_161.nextPosition();
                } else {
                    goto out_peg_163;
                }
            }
            result_peg_161.setValue((void*) "=");
        
        
        
        result_peg_161 = rule_s(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
        
        
        
        result_peg_161 = rule_expr(stream, result_peg_161.getPosition());
            if (result_peg_161.error()){
                goto out_peg_163;
            }
            exp = result_peg_161.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
                result_peg_161.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_161;
    stream.update(result_peg_161.getPosition());
    
    
    return result_peg_161;
    out_peg_163:
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
        
        
        
        Result result_peg_175(result_peg_168.getPosition());
            result_peg_175 = rule_line_end(stream, result_peg_175.getPosition());
            if (result_peg_175.error()){
                goto out_peg_170;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_168.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_168;
    stream.update(result_peg_168.getPosition());
    
    
    return result_peg_168;
    out_peg_170:
    Result result_peg_176(myposition);
    
    {
    
        result_peg_176 = rule_identifier(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
            name = result_peg_176.getValues();
        
        
        
        result_peg_176 = rule_s(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_176.getPosition()))){
                    result_peg_176.nextPosition();
                } else {
                    goto out_peg_178;
                }
            }
            result_peg_176.setValue((void*) "(");
        
        
        
        result_peg_176 = rule_s(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
        
        
        
        result_peg_176 = rule_integer(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
            index = result_peg_176.getValues();
        
        
        
        result_peg_176 = rule_s(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_176.getPosition()))){
                    result_peg_176.nextPosition();
                } else {
                    goto out_peg_178;
                }
            }
            result_peg_176.setValue((void*) ")");
        
        
        
        result_peg_176 = rule_s(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_176.getPosition()))){
                    result_peg_176.nextPosition();
                } else {
                    goto out_peg_178;
                }
            }
            result_peg_176.setValue((void*) "=");
        
        
        
        result_peg_176 = rule_s(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
        
        
        
        result_peg_176 = rule_expr(stream, result_peg_176.getPosition());
            if (result_peg_176.error()){
                goto out_peg_178;
            }
            exp = result_peg_176.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_176.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_176;
    stream.update(result_peg_176.getPosition());
    
    
    return result_peg_176;
    out_peg_178:
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_190 = stream.getColumn(position);
    if (column_peg_190.chunk2 != 0 && column_peg_190.chunk2->chunk_identifier.calculated()){
        return column_peg_190.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_198(stream, "identifier");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_191(myposition);
    
    {
    
        result_peg_191 = rule_name(stream, result_peg_191.getPosition());
            if (result_peg_191.error()){
                goto out_peg_193;
            }
            first = result_peg_191.getValues();
        
        
        
        result_peg_191.reset();
            do{
                Result result_peg_196(result_peg_191.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_196.getPosition()))){
                                result_peg_196.nextPosition();
                            } else {
                                goto loop_peg_195;
                            }
                        }
                        result_peg_196.setValue((void*) ".");
                    
                    
                    
                    result_peg_196 = rule_name(stream, result_peg_196.getPosition());
                        if (result_peg_196.error()){
                            goto loop_peg_195;
                        }
                    
                    
                }
                result_peg_191.addResult(result_peg_196);
            } while (true);
            loop_peg_195:
            ;
            rest = result_peg_191.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_191.setValue(value);
            }
        
        
    }
    
    if (column_peg_190.chunk2 == 0){
        column_peg_190.chunk2 = new Chunk2();
    }
    column_peg_190.chunk2->chunk_identifier = result_peg_191;
    stream.update(result_peg_191.getPosition());
    
    
    return result_peg_191;
    out_peg_193:
    
    if (column_peg_190.chunk2 == 0){
        column_peg_190.chunk2 = new Chunk2();
    }
    column_peg_190.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_199 = stream.getColumn(position);
    if (column_peg_199.chunk2 != 0 && column_peg_199.chunk2->chunk_integer.calculated()){
        return column_peg_199.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_217(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_200(myposition);
    
    {
    
        int save_peg_202 = result_peg_200.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_200.getPosition()))){
                        result_peg_200.nextPosition();
                    } else {
                        goto out_peg_205;
                    }
                }
                result_peg_200.setValue((void*) "-");
                    
            }
            goto success_peg_203;
            out_peg_205:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_200.getPosition()))){
                        result_peg_200.nextPosition();
                    } else {
                        goto out_peg_207;
                    }
                }
                result_peg_200.setValue((void*) "+");
                    
            }
            goto success_peg_203;
            out_peg_207:
            
            result_peg_200 = Result(save_peg_202);
            result_peg_200.setValue((void*) 0);
            
            success_peg_203:
            ;
        
        Result result_peg_201 = result_peg_200;
        
        result_peg_200.reset();
            do{
                Result result_peg_210(result_peg_200.getPosition());
                {
                    
                    char letter_peg_215 = stream.get(result_peg_210.getPosition());
                    if (letter_peg_215 != '\0' && strchr("0123456789", letter_peg_215) != NULL){
                        result_peg_210.nextPosition();
                        result_peg_210.setValue((void*) (long) letter_peg_215);
                    } else {
                        goto out_peg_214;
                    }
                    
                }
                goto success_peg_211;
                out_peg_214:
                goto loop_peg_209;
                success_peg_211:
                ;
                result_peg_200.addResult(result_peg_210);
            } while (true);
            loop_peg_209:
            if (result_peg_200.matches() == 0){
                goto out_peg_216;
            }
        
        Result result_peg_208 = result_peg_200;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_201.getValues(), result_peg_208.getValues());
                result_peg_200.setValue(value);
            }
        
        
    }
    
    if (column_peg_199.chunk2 == 0){
        column_peg_199.chunk2 = new Chunk2();
    }
    column_peg_199.chunk2->chunk_integer = result_peg_200;
    stream.update(result_peg_200.getPosition());
    
    
    return result_peg_200;
    out_peg_216:
    
    if (column_peg_199.chunk2 == 0){
        column_peg_199.chunk2 = new Chunk2();
    }
    column_peg_199.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_218 = stream.getColumn(position);
    if (column_peg_218.chunk2 != 0 && column_peg_218.chunk2->chunk_float.calculated()){
        return column_peg_218.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_263(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_219(myposition);
    
    {
    
        int save_peg_221 = result_peg_219.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_219.getPosition()))){
                        result_peg_219.nextPosition();
                    } else {
                        goto out_peg_224;
                    }
                }
                result_peg_219.setValue((void*) "-");
                    
            }
            goto success_peg_222;
            out_peg_224:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_219.getPosition()))){
                        result_peg_219.nextPosition();
                    } else {
                        goto out_peg_226;
                    }
                }
                result_peg_219.setValue((void*) "+");
                    
            }
            goto success_peg_222;
            out_peg_226:
            
            result_peg_219 = Result(save_peg_221);
            result_peg_219.setValue((void*) 0);
            
            success_peg_222:
            ;
        
        Result result_peg_220 = result_peg_219;
        
        result_peg_219.reset();
            do{
                Result result_peg_229(result_peg_219.getPosition());
                {
                    
                    char letter_peg_234 = stream.get(result_peg_229.getPosition());
                    if (letter_peg_234 != '\0' && strchr("0123456789", letter_peg_234) != NULL){
                        result_peg_229.nextPosition();
                        result_peg_229.setValue((void*) (long) letter_peg_234);
                    } else {
                        goto out_peg_233;
                    }
                    
                }
                goto success_peg_230;
                out_peg_233:
                goto loop_peg_228;
                success_peg_230:
                ;
                result_peg_219.addResult(result_peg_229);
            } while (true);
            loop_peg_228:
            ;
            left = result_peg_219.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_219.getPosition()))){
                    result_peg_219.nextPosition();
                } else {
                    goto out_peg_236;
                }
            }
            result_peg_219.setValue((void*) ".");
        
        
        
        result_peg_219.reset();
            do{
                Result result_peg_239(result_peg_219.getPosition());
                {
                    
                    char letter_peg_244 = stream.get(result_peg_239.getPosition());
                    if (letter_peg_244 != '\0' && strchr("0123456789", letter_peg_244) != NULL){
                        result_peg_239.nextPosition();
                        result_peg_239.setValue((void*) (long) letter_peg_244);
                    } else {
                        goto out_peg_243;
                    }
                    
                }
                goto success_peg_240;
                out_peg_243:
                goto loop_peg_238;
                success_peg_240:
                ;
                result_peg_219.addResult(result_peg_239);
            } while (true);
            loop_peg_238:
            if (result_peg_219.matches() == 0){
                goto out_peg_236;
            }
            right = result_peg_219.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_220.getValues(), parseDouble(left,right));
                result_peg_219.setValue(value);
            }
        
        
    }
    
    if (column_peg_218.chunk2 == 0){
        column_peg_218.chunk2 = new Chunk2();
    }
    column_peg_218.chunk2->chunk_float = result_peg_219;
    stream.update(result_peg_219.getPosition());
    
    
    return result_peg_219;
    out_peg_236:
    Result result_peg_245(myposition);
    
    {
    
        int save_peg_247 = result_peg_245.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_245.getPosition()))){
                        result_peg_245.nextPosition();
                    } else {
                        goto out_peg_250;
                    }
                }
                result_peg_245.setValue((void*) "-");
                    
            }
            goto success_peg_248;
            out_peg_250:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_245.getPosition()))){
                        result_peg_245.nextPosition();
                    } else {
                        goto out_peg_252;
                    }
                }
                result_peg_245.setValue((void*) "+");
                    
            }
            goto success_peg_248;
            out_peg_252:
            
            result_peg_245 = Result(save_peg_247);
            result_peg_245.setValue((void*) 0);
            
            success_peg_248:
            ;
        
        Result result_peg_246 = result_peg_245;
        
        result_peg_245.reset();
            do{
                Result result_peg_255(result_peg_245.getPosition());
                {
                    
                    char letter_peg_260 = stream.get(result_peg_255.getPosition());
                    if (letter_peg_260 != '\0' && strchr("0123456789", letter_peg_260) != NULL){
                        result_peg_255.nextPosition();
                        result_peg_255.setValue((void*) (long) letter_peg_260);
                    } else {
                        goto out_peg_259;
                    }
                    
                }
                goto success_peg_256;
                out_peg_259:
                goto loop_peg_254;
                success_peg_256:
                ;
                result_peg_245.addResult(result_peg_255);
            } while (true);
            loop_peg_254:
            if (result_peg_245.matches() == 0){
                goto out_peg_261;
            }
            left = result_peg_245.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_245.getPosition()))){
                    result_peg_245.nextPosition();
                } else {
                    goto out_peg_261;
                }
            }
            result_peg_245.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_246.getValues(), parseDouble(left));
                result_peg_245.setValue(value);
            }
        
        
    }
    
    if (column_peg_218.chunk2 == 0){
        column_peg_218.chunk2 = new Chunk2();
    }
    column_peg_218.chunk2->chunk_float = result_peg_245;
    stream.update(result_peg_245.getPosition());
    
    
    return result_peg_245;
    out_peg_261:
    
    if (column_peg_218.chunk2 == 0){
        column_peg_218.chunk2 = new Chunk2();
    }
    column_peg_218.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_264 = stream.getColumn(position);
    if (column_peg_264.chunk2 != 0 && column_peg_264.chunk2->chunk_string.calculated()){
        return column_peg_264.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_279(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_265(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_265.getPosition()))){
                    result_peg_265.nextPosition();
                } else {
                    goto out_peg_267;
                }
            }
            result_peg_265.setValue((void*) "\"");
        
        
        
        result_peg_265.reset();
            do{
                Result result_peg_270(result_peg_265.getPosition());
                {
                
                    Result result_peg_273(result_peg_270);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_273.getPosition()))){
                                result_peg_273.nextPosition();
                            } else {
                                goto not_peg_272;
                            }
                        }
                        result_peg_273.setValue((void*) "\"");
                        goto loop_peg_269;
                        not_peg_272:
                        result_peg_270.setValue((void*)0);
                    
                    
                    
                    Result result_peg_276(result_peg_270);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_276.getPosition()))){
                                result_peg_276.nextPosition();
                            } else {
                                goto not_peg_275;
                            }
                        }
                        result_peg_276.setValue((void*) "\n");
                        goto loop_peg_269;
                        not_peg_275:
                        result_peg_270.setValue((void*)0);
                    
                    
                    
                    char temp_peg_277 = stream.get(result_peg_270.getPosition());
                        if (temp_peg_277 != '\0'){
                            result_peg_270.setValue((void*) (long) temp_peg_277);
                            result_peg_270.nextPosition();
                        } else {
                            goto loop_peg_269;
                        }
                    
                    
                }
                result_peg_265.addResult(result_peg_270);
            } while (true);
            loop_peg_269:
            ;
            contents = result_peg_265.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_265.getPosition()))){
                    result_peg_265.nextPosition();
                } else {
                    goto out_peg_267;
                }
            }
            result_peg_265.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_265.setValue(value);
            }
        
        
    }
    
    if (column_peg_264.chunk2 == 0){
        column_peg_264.chunk2 = new Chunk2();
    }
    column_peg_264.chunk2->chunk_string = result_peg_265;
    stream.update(result_peg_265.getPosition());
    
    
    return result_peg_265;
    out_peg_267:
    
    if (column_peg_264.chunk2 == 0){
        column_peg_264.chunk2 = new Chunk2();
    }
    column_peg_264.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_280 = stream.getColumn(position);
    if (column_peg_280.chunk3 != 0 && column_peg_280.chunk3->chunk_range.calculated()){
        return column_peg_280.chunk3->chunk_range;
    }
    
    RuleTrace trace_peg_325(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_281(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_281.getPosition()))){
                    result_peg_281.nextPosition();
                } else {
                    goto out_peg_283;
                }
            }
            result_peg_281.setValue((void*) "[");
        
        
        
        result_peg_281 = rule_s(stream, result_peg_281.getPosition());
            if (result_peg_281.error()){
                goto out_peg_283;
            }
        
        
        
        result_peg_281 = rule_expr_c(stream, result_peg_281.getPosition());
            if (result_peg_281.error()){
                goto out_peg_283;
            }
            low = result_peg_281.getValues();
        
        
        
        result_peg_281 = rule_s(stream, result_peg_281.getPosition());
            if (result_peg_281.error()){
                goto out_peg_283;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_281.getPosition()))){
                    result_peg_281.nextPosition();
                } else {
                    goto out_peg_283;
                }
            }
            result_peg_281.setValue((void*) ",");
        
        
        
        result_peg_281 = rule_s(stream, result_peg_281.getPosition());
            if (result_peg_281.error()){
                goto out_peg_283;
            }
        
        
        
        result_peg_281 = rule_expr_c(stream, result_peg_281.getPosition());
            if (result_peg_281.error()){
                goto out_peg_283;
            }
            high = result_peg_281.getValues();
        
        
        
        result_peg_281 = rule_s(stream, result_peg_281.getPosition());
            if (result_peg_281.error()){
                goto out_peg_283;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_281.getPosition()))){
                    result_peg_281.nextPosition();
                } else {
                    goto out_peg_283;
                }
            }
            result_peg_281.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_281.setValue(value);
            }
        
        
    }
    
    if (column_peg_280.chunk3 == 0){
        column_peg_280.chunk3 = new Chunk3();
    }
    column_peg_280.chunk3->chunk_range = result_peg_281;
    stream.update(result_peg_281.getPosition());
    
    
    return result_peg_281;
    out_peg_283:
    Result result_peg_292(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_292.getPosition()))){
                    result_peg_292.nextPosition();
                } else {
                    goto out_peg_294;
                }
            }
            result_peg_292.setValue((void*) "(");
        
        
        
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
                if (compareChar(")"[i], stream.get(result_peg_292.getPosition()))){
                    result_peg_292.nextPosition();
                } else {
                    goto out_peg_294;
                }
            }
            result_peg_292.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_292.setValue(value);
            }
        
        
    }
    
    if (column_peg_280.chunk3 == 0){
        column_peg_280.chunk3 = new Chunk3();
    }
    column_peg_280.chunk3->chunk_range = result_peg_292;
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
                if (compareChar("]"[i], stream.get(result_peg_303.getPosition()))){
                    result_peg_303.nextPosition();
                } else {
                    goto out_peg_305;
                }
            }
            result_peg_303.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_303.setValue(value);
            }
        
        
    }
    
    if (column_peg_280.chunk3 == 0){
        column_peg_280.chunk3 = new Chunk3();
    }
    column_peg_280.chunk3->chunk_range = result_peg_303;
    stream.update(result_peg_303.getPosition());
    
    
    return result_peg_303;
    out_peg_305:
    Result result_peg_314(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_314.getPosition()))){
                    result_peg_314.nextPosition();
                } else {
                    goto out_peg_316;
                }
            }
            result_peg_314.setValue((void*) "[");
        
        
        
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
                if (compareChar(")"[i], stream.get(result_peg_314.getPosition()))){
                    result_peg_314.nextPosition();
                } else {
                    goto out_peg_316;
                }
            }
            result_peg_314.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_314.setValue(value);
            }
        
        
    }
    
    if (column_peg_280.chunk3 == 0){
        column_peg_280.chunk3 = new Chunk3();
    }
    column_peg_280.chunk3->chunk_range = result_peg_314;
    stream.update(result_peg_314.getPosition());
    
    
    return result_peg_314;
    out_peg_316:
    
    if (column_peg_280.chunk3 == 0){
        column_peg_280.chunk3 = new Chunk3();
    }
    column_peg_280.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_326 = stream.getColumn(position);
    if (column_peg_326.chunk3 != 0 && column_peg_326.chunk3->chunk_name.calculated()){
        return column_peg_326.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_338(stream, "name");
    int myposition = position;
    
    
    Result result_peg_327(myposition);
    
    {
    
        {
                
                char letter_peg_333 = stream.get(result_peg_327.getPosition());
                if (letter_peg_333 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_333) != NULL){
                    result_peg_327.nextPosition();
                    result_peg_327.setValue((void*) (long) letter_peg_333);
                } else {
                    goto out_peg_332;
                }
                
            }
            goto success_peg_329;
            out_peg_332:
            goto out_peg_334;
            success_peg_329:
            ;
        
        Result result_peg_328 = result_peg_327;
        
        result_peg_327.reset();
            do{
                Result result_peg_337(result_peg_327.getPosition());
                result_peg_337 = rule_alpha_digit(stream, result_peg_337.getPosition());
                if (result_peg_337.error()){
                    goto loop_peg_336;
                }
                result_peg_327.addResult(result_peg_337);
            } while (true);
            loop_peg_336:
            ;
        
        Result result_peg_335 = result_peg_327;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_328.getValues().getValue(),result_peg_335.getValues());
                result_peg_327.setValue(value);
            }
        
        
    }
    
    if (column_peg_326.chunk3 == 0){
        column_peg_326.chunk3 = new Chunk3();
    }
    column_peg_326.chunk3->chunk_name = result_peg_327;
    stream.update(result_peg_327.getPosition());
    
    
    return result_peg_327;
    out_peg_334:
    
    if (column_peg_326.chunk3 == 0){
        column_peg_326.chunk3 = new Chunk3();
    }
    column_peg_326.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_339 = stream.getColumn(position);
    if (column_peg_339.chunk3 != 0 && column_peg_339.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_339.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_354(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_340(myposition);
    
    {
        
        char letter_peg_345 = stream.get(result_peg_340.getPosition());
        if (letter_peg_345 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_345) != NULL){
            result_peg_340.nextPosition();
            result_peg_340.setValue((void*) (long) letter_peg_345);
        } else {
            goto out_peg_344;
        }
        
    }
    goto success_peg_341;
    out_peg_344:
    goto out_peg_346;
    success_peg_341:
    ;
    
    if (column_peg_339.chunk3 == 0){
        column_peg_339.chunk3 = new Chunk3();
    }
    column_peg_339.chunk3->chunk_alpha_digit = result_peg_340;
    stream.update(result_peg_340.getPosition());
    
    
    return result_peg_340;
    out_peg_346:
    Result result_peg_347(myposition);
    
    {
        
        char letter_peg_352 = stream.get(result_peg_347.getPosition());
        if (letter_peg_352 != '\0' && strchr("0123456789", letter_peg_352) != NULL){
            result_peg_347.nextPosition();
            result_peg_347.setValue((void*) (long) letter_peg_352);
        } else {
            goto out_peg_351;
        }
        
    }
    goto success_peg_348;
    out_peg_351:
    goto out_peg_353;
    success_peg_348:
    ;
    
    if (column_peg_339.chunk3 == 0){
        column_peg_339.chunk3 = new Chunk3();
    }
    column_peg_339.chunk3->chunk_alpha_digit = result_peg_347;
    stream.update(result_peg_347.getPosition());
    
    
    return result_peg_347;
    out_peg_353:
    
    if (column_peg_339.chunk3 == 0){
        column_peg_339.chunk3 = new Chunk3();
    }
    column_peg_339.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_355 = stream.getColumn(position);
    if (column_peg_355.chunk3 != 0 && column_peg_355.chunk3->chunk_valuelist.calculated()){
        return column_peg_355.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_377(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_356(myposition);
    
    {
    
        result_peg_356 = rule_expr_c(stream, result_peg_356.getPosition());
            if (result_peg_356.error()){
                goto out_peg_358;
            }
            first = result_peg_356.getValues();
        
        
        
        result_peg_356.reset();
            do{
                Result result_peg_361(result_peg_356.getPosition());
                {
                
                    result_peg_361 = rule_s(stream, result_peg_361.getPosition());
                        if (result_peg_361.error()){
                            goto loop_peg_360;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_361.getPosition()))){
                                result_peg_361.nextPosition();
                            } else {
                                goto loop_peg_360;
                            }
                        }
                        result_peg_361.setValue((void*) ",");
                    
                    
                    
                    result_peg_361 = rule_s(stream, result_peg_361.getPosition());
                        if (result_peg_361.error()){
                            goto loop_peg_360;
                        }
                    
                    
                    
                    result_peg_361 = rule_expr_c(stream, result_peg_361.getPosition());
                        if (result_peg_361.error()){
                            goto loop_peg_360;
                        }
                    
                    
                }
                result_peg_356.addResult(result_peg_361);
            } while (true);
            loop_peg_360:
            if (result_peg_356.matches() == 0){
                goto out_peg_358;
            }
            rest = result_peg_356.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_356.setValue(value);
            }
        
        
    }
    
    if (column_peg_355.chunk3 == 0){
        column_peg_355.chunk3 = new Chunk3();
    }
    column_peg_355.chunk3->chunk_valuelist = result_peg_356;
    stream.update(result_peg_356.getPosition());
    
    
    return result_peg_356;
    out_peg_358:
    Result result_peg_365(myposition);
    
    {
    
        result_peg_365 = rule_s(stream, result_peg_365.getPosition());
            if (result_peg_365.error()){
                goto out_peg_367;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_365.getPosition()))){
                    result_peg_365.nextPosition();
                } else {
                    goto out_peg_367;
                }
            }
            result_peg_365.setValue((void*) ",");
        
        
        
        result_peg_365 = rule_s(stream, result_peg_365.getPosition());
            if (result_peg_365.error()){
                goto out_peg_367;
            }
        
        
        
        result_peg_365 = rule_expr_c(stream, result_peg_365.getPosition());
            if (result_peg_365.error()){
                goto out_peg_367;
            }
            first = result_peg_365.getValues();
        
        
        
        result_peg_365.reset();
            do{
                Result result_peg_373(result_peg_365.getPosition());
                {
                
                    result_peg_373 = rule_s(stream, result_peg_373.getPosition());
                        if (result_peg_373.error()){
                            goto loop_peg_372;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_373.getPosition()))){
                                result_peg_373.nextPosition();
                            } else {
                                goto loop_peg_372;
                            }
                        }
                        result_peg_373.setValue((void*) ",");
                    
                    
                    
                    result_peg_373 = rule_s(stream, result_peg_373.getPosition());
                        if (result_peg_373.error()){
                            goto loop_peg_372;
                        }
                    
                    
                    
                    result_peg_373 = rule_expr_c(stream, result_peg_373.getPosition());
                        if (result_peg_373.error()){
                            goto loop_peg_372;
                        }
                    
                    
                }
                result_peg_365.addResult(result_peg_373);
            } while (true);
            loop_peg_372:
            ;
            rest = result_peg_365.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_365.setValue(value);
            }
        
        
    }
    
    if (column_peg_355.chunk3 == 0){
        column_peg_355.chunk3 = new Chunk3();
    }
    column_peg_355.chunk3->chunk_valuelist = result_peg_365;
    stream.update(result_peg_365.getPosition());
    
    
    return result_peg_365;
    out_peg_367:
    
    if (column_peg_355.chunk3 == 0){
        column_peg_355.chunk3 = new Chunk3();
    }
    column_peg_355.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_378 = stream.getColumn(position);
    if (column_peg_378.chunk3 != 0 && column_peg_378.chunk3->chunk_expr.calculated()){
        return column_peg_378.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_389(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_379(myposition);
    
    {
    
        result_peg_379 = rule_expr_c(stream, result_peg_379.getPosition());
            if (result_peg_379.error()){
                goto out_peg_381;
            }
        
        Result result_peg_380 = result_peg_379;
        
        Result result_peg_384(result_peg_379);
            {
            
                result_peg_384 = rule_s(stream, result_peg_384.getPosition());
                    if (result_peg_384.error()){
                        goto not_peg_383;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_384.getPosition()))){
                            result_peg_384.nextPosition();
                        } else {
                            goto not_peg_383;
                        }
                    }
                    result_peg_384.setValue((void*) ",");
                
                
            }
            goto out_peg_381;
            not_peg_383:
            result_peg_379.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_380.getValues();
                result_peg_379.setValue(value);
            }
        
        
    }
    
    if (column_peg_378.chunk3 == 0){
        column_peg_378.chunk3 = new Chunk3();
    }
    column_peg_378.chunk3->chunk_expr = result_peg_379;
    stream.update(result_peg_379.getPosition());
    
    
    return result_peg_379;
    out_peg_381:
    Result result_peg_386(myposition);
    
    {
    
        result_peg_386 = rule_valuelist(stream, result_peg_386.getPosition());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
        
        Result result_peg_387 = result_peg_386;
        
        result_peg_386 = rule_expr2_rest(stream, result_peg_386.getPosition(), result_peg_387.getValues());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
        
        
    }
    
    if (column_peg_378.chunk3 == 0){
        column_peg_378.chunk3 = new Chunk3();
    }
    column_peg_378.chunk3->chunk_expr = result_peg_386;
    stream.update(result_peg_386.getPosition());
    
    
    return result_peg_386;
    out_peg_388:
    
    if (column_peg_378.chunk3 == 0){
        column_peg_378.chunk3 = new Chunk3();
    }
    column_peg_378.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_390 = stream.getColumn(position);
    if (column_peg_390.chunk4 != 0 && column_peg_390.chunk4->chunk_expr_c.calculated()){
        return column_peg_390.chunk4->chunk_expr_c;
    }
    
    RuleTrace trace_peg_394(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_391(myposition);
    
    {
    
        result_peg_391 = rule_expr2(stream, result_peg_391.getPosition());
            if (result_peg_391.error()){
                goto out_peg_393;
            }
            left = result_peg_391.getValues();
        
        
        
        result_peg_391 = rule_expr_rest(stream, result_peg_391.getPosition(), left);
            if (result_peg_391.error()){
                goto out_peg_393;
            }
        
        
    }
    
    if (column_peg_390.chunk4 == 0){
        column_peg_390.chunk4 = new Chunk4();
    }
    column_peg_390.chunk4->chunk_expr_c = result_peg_391;
    stream.update(result_peg_391.getPosition());
    
    
    return result_peg_391;
    out_peg_393:
    
    if (column_peg_390.chunk4 == 0){
        column_peg_390.chunk4 = new Chunk4();
    }
    column_peg_390.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_395 = stream.getColumn(position);
    if (column_peg_395.chunk4 != 0 && column_peg_395.chunk4->chunk_expr2.calculated()){
        return column_peg_395.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_399(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_396(myposition);
    
    {
    
        result_peg_396 = rule_expr3(stream, result_peg_396.getPosition());
            if (result_peg_396.error()){
                goto out_peg_398;
            }
            left = result_peg_396.getValues();
        
        
        
        result_peg_396 = rule_expr2_rest(stream, result_peg_396.getPosition(), left);
            if (result_peg_396.error()){
                goto out_peg_398;
            }
        
        
    }
    
    if (column_peg_395.chunk4 == 0){
        column_peg_395.chunk4 = new Chunk4();
    }
    column_peg_395.chunk4->chunk_expr2 = result_peg_396;
    stream.update(result_peg_396.getPosition());
    
    
    return result_peg_396;
    out_peg_398:
    
    if (column_peg_395.chunk4 == 0){
        column_peg_395.chunk4 = new Chunk4();
    }
    column_peg_395.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_400 = stream.getColumn(position);
    if (column_peg_400.chunk4 != 0 && column_peg_400.chunk4->chunk_expr3.calculated()){
        return column_peg_400.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_404(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_401(myposition);
    
    {
    
        result_peg_401 = rule_expr4(stream, result_peg_401.getPosition());
            if (result_peg_401.error()){
                goto out_peg_403;
            }
            left = result_peg_401.getValues();
        
        
        
        result_peg_401 = rule_expr3_rest(stream, result_peg_401.getPosition(), left);
            if (result_peg_401.error()){
                goto out_peg_403;
            }
        
        
    }
    
    if (column_peg_400.chunk4 == 0){
        column_peg_400.chunk4 = new Chunk4();
    }
    column_peg_400.chunk4->chunk_expr3 = result_peg_401;
    stream.update(result_peg_401.getPosition());
    
    
    return result_peg_401;
    out_peg_403:
    
    if (column_peg_400.chunk4 == 0){
        column_peg_400.chunk4 = new Chunk4();
    }
    column_peg_400.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_405 = stream.getColumn(position);
    if (column_peg_405.chunk4 != 0 && column_peg_405.chunk4->chunk_expr4.calculated()){
        return column_peg_405.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_409(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_406(myposition);
    
    {
    
        result_peg_406 = rule_expr5(stream, result_peg_406.getPosition());
            if (result_peg_406.error()){
                goto out_peg_408;
            }
            left = result_peg_406.getValues();
        
        
        
        result_peg_406 = rule_expr4_rest(stream, result_peg_406.getPosition(), left);
            if (result_peg_406.error()){
                goto out_peg_408;
            }
        
        
    }
    
    if (column_peg_405.chunk4 == 0){
        column_peg_405.chunk4 = new Chunk4();
    }
    column_peg_405.chunk4->chunk_expr4 = result_peg_406;
    stream.update(result_peg_406.getPosition());
    
    
    return result_peg_406;
    out_peg_408:
    
    if (column_peg_405.chunk4 == 0){
        column_peg_405.chunk4 = new Chunk4();
    }
    column_peg_405.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_410 = stream.getColumn(position);
    if (column_peg_410.chunk4 != 0 && column_peg_410.chunk4->chunk_expr5.calculated()){
        return column_peg_410.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_414(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_411(myposition);
    
    {
    
        result_peg_411 = rule_expr6(stream, result_peg_411.getPosition());
            if (result_peg_411.error()){
                goto out_peg_413;
            }
            left = result_peg_411.getValues();
        
        
        
        result_peg_411 = rule_expr5_rest(stream, result_peg_411.getPosition(), left);
            if (result_peg_411.error()){
                goto out_peg_413;
            }
        
        
    }
    
    if (column_peg_410.chunk4 == 0){
        column_peg_410.chunk4 = new Chunk4();
    }
    column_peg_410.chunk4->chunk_expr5 = result_peg_411;
    stream.update(result_peg_411.getPosition());
    
    
    return result_peg_411;
    out_peg_413:
    
    if (column_peg_410.chunk4 == 0){
        column_peg_410.chunk4 = new Chunk4();
    }
    column_peg_410.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_415 = stream.getColumn(position);
    if (column_peg_415.chunk5 != 0 && column_peg_415.chunk5->chunk_expr6.calculated()){
        return column_peg_415.chunk5->chunk_expr6;
    }
    
    RuleTrace trace_peg_419(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_416(myposition);
    
    {
    
        result_peg_416 = rule_expr7(stream, result_peg_416.getPosition());
            if (result_peg_416.error()){
                goto out_peg_418;
            }
            left = result_peg_416.getValues();
        
        
        
        result_peg_416 = rule_expr6_rest(stream, result_peg_416.getPosition(), left);
            if (result_peg_416.error()){
                goto out_peg_418;
            }
        
        
    }
    
    if (column_peg_415.chunk5 == 0){
        column_peg_415.chunk5 = new Chunk5();
    }
    column_peg_415.chunk5->chunk_expr6 = result_peg_416;
    stream.update(result_peg_416.getPosition());
    
    
    return result_peg_416;
    out_peg_418:
    
    if (column_peg_415.chunk5 == 0){
        column_peg_415.chunk5 = new Chunk5();
    }
    column_peg_415.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_420 = stream.getColumn(position);
    if (column_peg_420.chunk5 != 0 && column_peg_420.chunk5->chunk_expr7.calculated()){
        return column_peg_420.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_424(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_421(myposition);
    
    {
    
        result_peg_421 = rule_expr8(stream, result_peg_421.getPosition());
            if (result_peg_421.error()){
                goto out_peg_423;
            }
            left = result_peg_421.getValues();
        
        
        
        result_peg_421 = rule_expr7_rest(stream, result_peg_421.getPosition(), left);
            if (result_peg_421.error()){
                goto out_peg_423;
            }
        
        
    }
    
    if (column_peg_420.chunk5 == 0){
        column_peg_420.chunk5 = new Chunk5();
    }
    column_peg_420.chunk5->chunk_expr7 = result_peg_421;
    stream.update(result_peg_421.getPosition());
    
    
    return result_peg_421;
    out_peg_423:
    
    if (column_peg_420.chunk5 == 0){
        column_peg_420.chunk5 = new Chunk5();
    }
    column_peg_420.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_425 = stream.getColumn(position);
    if (column_peg_425.chunk5 != 0 && column_peg_425.chunk5->chunk_expr8.calculated()){
        return column_peg_425.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_429(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_426(myposition);
    
    {
    
        result_peg_426 = rule_expr9(stream, result_peg_426.getPosition());
            if (result_peg_426.error()){
                goto out_peg_428;
            }
            left = result_peg_426.getValues();
        
        
        
        result_peg_426 = rule_expr8_rest(stream, result_peg_426.getPosition(), left);
            if (result_peg_426.error()){
                goto out_peg_428;
            }
        
        
    }
    
    if (column_peg_425.chunk5 == 0){
        column_peg_425.chunk5 = new Chunk5();
    }
    column_peg_425.chunk5->chunk_expr8 = result_peg_426;
    stream.update(result_peg_426.getPosition());
    
    
    return result_peg_426;
    out_peg_428:
    
    if (column_peg_425.chunk5 == 0){
        column_peg_425.chunk5 = new Chunk5();
    }
    column_peg_425.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_430 = stream.getColumn(position);
    if (column_peg_430.chunk5 != 0 && column_peg_430.chunk5->chunk_expr9.calculated()){
        return column_peg_430.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_434(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_431(myposition);
    
    {
    
        result_peg_431 = rule_expr10(stream, result_peg_431.getPosition());
            if (result_peg_431.error()){
                goto out_peg_433;
            }
            left = result_peg_431.getValues();
        
        
        
        result_peg_431 = rule_expr9_rest(stream, result_peg_431.getPosition(), left);
            if (result_peg_431.error()){
                goto out_peg_433;
            }
        
        
    }
    
    if (column_peg_430.chunk5 == 0){
        column_peg_430.chunk5 = new Chunk5();
    }
    column_peg_430.chunk5->chunk_expr9 = result_peg_431;
    stream.update(result_peg_431.getPosition());
    
    
    return result_peg_431;
    out_peg_433:
    
    if (column_peg_430.chunk5 == 0){
        column_peg_430.chunk5 = new Chunk5();
    }
    column_peg_430.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_435 = stream.getColumn(position);
    if (column_peg_435.chunk5 != 0 && column_peg_435.chunk5->chunk_expr10.calculated()){
        return column_peg_435.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_439(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_436(myposition);
    
    {
    
        result_peg_436 = rule_expr11(stream, result_peg_436.getPosition());
            if (result_peg_436.error()){
                goto out_peg_438;
            }
            left = result_peg_436.getValues();
        
        
        
        result_peg_436 = rule_expr10_rest(stream, result_peg_436.getPosition(), left);
            if (result_peg_436.error()){
                goto out_peg_438;
            }
        
        
    }
    
    if (column_peg_435.chunk5 == 0){
        column_peg_435.chunk5 = new Chunk5();
    }
    column_peg_435.chunk5->chunk_expr10 = result_peg_436;
    stream.update(result_peg_436.getPosition());
    
    
    return result_peg_436;
    out_peg_438:
    
    if (column_peg_435.chunk5 == 0){
        column_peg_435.chunk5 = new Chunk5();
    }
    column_peg_435.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_440 = stream.getColumn(position);
    if (column_peg_440.chunk6 != 0 && column_peg_440.chunk6->chunk_expr11.calculated()){
        return column_peg_440.chunk6->chunk_expr11;
    }
    
    RuleTrace trace_peg_444(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_441(myposition);
    
    {
    
        result_peg_441 = rule_expr12(stream, result_peg_441.getPosition());
            if (result_peg_441.error()){
                goto out_peg_443;
            }
            left = result_peg_441.getValues();
        
        
        
        result_peg_441 = rule_expr11_rest(stream, result_peg_441.getPosition(), left);
            if (result_peg_441.error()){
                goto out_peg_443;
            }
        
        
    }
    
    if (column_peg_440.chunk6 == 0){
        column_peg_440.chunk6 = new Chunk6();
    }
    column_peg_440.chunk6->chunk_expr11 = result_peg_441;
    stream.update(result_peg_441.getPosition());
    
    
    return result_peg_441;
    out_peg_443:
    
    if (column_peg_440.chunk6 == 0){
        column_peg_440.chunk6 = new Chunk6();
    }
    column_peg_440.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_445 = stream.getColumn(position);
    if (column_peg_445.chunk6 != 0 && column_peg_445.chunk6->chunk_expr12.calculated()){
        return column_peg_445.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_449(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_446(myposition);
    
    {
    
        result_peg_446 = rule_expr13(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_448;
            }
            left = result_peg_446.getValues();
        
        
        
        result_peg_446 = rule_expr12_rest(stream, result_peg_446.getPosition(), left);
            if (result_peg_446.error()){
                goto out_peg_448;
            }
        
        
    }
    
    if (column_peg_445.chunk6 == 0){
        column_peg_445.chunk6 = new Chunk6();
    }
    column_peg_445.chunk6->chunk_expr12 = result_peg_446;
    stream.update(result_peg_446.getPosition());
    
    
    return result_peg_446;
    out_peg_448:
    
    if (column_peg_445.chunk6 == 0){
        column_peg_445.chunk6 = new Chunk6();
    }
    column_peg_445.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_450 = stream.getColumn(position);
    if (column_peg_450.chunk6 != 0 && column_peg_450.chunk6->chunk_expr13.calculated()){
        return column_peg_450.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_457(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_451(myposition);
    
    {
    
        result_peg_451.reset();
            do{
                Result result_peg_454(result_peg_451.getPosition());
                result_peg_454 = rule_unary(stream, result_peg_454.getPosition());
                if (result_peg_454.error()){
                    goto loop_peg_453;
                }
                result_peg_451.addResult(result_peg_454);
            } while (true);
            loop_peg_453:
            ;
        
        Result result_peg_452 = result_peg_451;
        
        result_peg_451 = rule_expr13_real(stream, result_peg_451.getPosition());
            if (result_peg_451.error()){
                goto out_peg_456;
            }
            exp = result_peg_451.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_452.getValues(), exp);
                result_peg_451.setValue(value);
            }
        
        
    }
    
    if (column_peg_450.chunk6 == 0){
        column_peg_450.chunk6 = new Chunk6();
    }
    column_peg_450.chunk6->chunk_expr13 = result_peg_451;
    stream.update(result_peg_451.getPosition());
    
    
    return result_peg_451;
    out_peg_456:
    
    if (column_peg_450.chunk6 == 0){
        column_peg_450.chunk6 = new Chunk6();
    }
    column_peg_450.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_469(stream, "expr_rest");
    int myposition = position;
    tail_peg_460:
    Value right;
    Value new_left;
    Result result_peg_459(myposition);
    {
    
        result_peg_459 = rule_s(stream, result_peg_459.getPosition());
            if (result_peg_459.error()){
                goto out_peg_462;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_459.getPosition()))){
                    result_peg_459.nextPosition();
                } else {
                    goto out_peg_462;
                }
            }
            result_peg_459.setValue((void*) "||");
        
        
        
        result_peg_459 = rule_s(stream, result_peg_459.getPosition());
            if (result_peg_459.error()){
                goto out_peg_462;
            }
        
        
        
        result_peg_459 = rule_expr2(stream, result_peg_459.getPosition());
            if (result_peg_459.error()){
                goto out_peg_462;
            }
            right = result_peg_459.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_459.setValue(value);
            }
            new_left = result_peg_459.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_459.getPosition();
    goto tail_peg_460;
    out_peg_462:
    Result result_peg_467(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_467.setValue(value);
            }
        
        
    }
    stream.update(result_peg_467.getPosition());
    
    return result_peg_467;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_481(stream, "expr2_rest");
    int myposition = position;
    tail_peg_472:
    Value right;
    Value new_left;
    Result result_peg_471(myposition);
    {
    
        result_peg_471 = rule_s(stream, result_peg_471.getPosition());
            if (result_peg_471.error()){
                goto out_peg_474;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_471.getPosition()))){
                    result_peg_471.nextPosition();
                } else {
                    goto out_peg_474;
                }
            }
            result_peg_471.setValue((void*) "^^");
        
        
        
        result_peg_471 = rule_s(stream, result_peg_471.getPosition());
            if (result_peg_471.error()){
                goto out_peg_474;
            }
        
        
        
        result_peg_471 = rule_expr3(stream, result_peg_471.getPosition());
            if (result_peg_471.error()){
                goto out_peg_474;
            }
            right = result_peg_471.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_471.setValue(value);
            }
            new_left = result_peg_471.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_471.getPosition();
    goto tail_peg_472;
    out_peg_474:
    Result result_peg_479(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_479.setValue(value);
            }
        
        
    }
    stream.update(result_peg_479.getPosition());
    
    return result_peg_479;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_493(stream, "expr3_rest");
    int myposition = position;
    tail_peg_484:
    Value right;
    Value new_left;
    Result result_peg_483(myposition);
    {
    
        result_peg_483 = rule_s(stream, result_peg_483.getPosition());
            if (result_peg_483.error()){
                goto out_peg_486;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_483.getPosition()))){
                    result_peg_483.nextPosition();
                } else {
                    goto out_peg_486;
                }
            }
            result_peg_483.setValue((void*) "&&");
        
        
        
        result_peg_483 = rule_s(stream, result_peg_483.getPosition());
            if (result_peg_483.error()){
                goto out_peg_486;
            }
        
        
        
        result_peg_483 = rule_expr4(stream, result_peg_483.getPosition());
            if (result_peg_483.error()){
                goto out_peg_486;
            }
            right = result_peg_483.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_483.setValue(value);
            }
            new_left = result_peg_483.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_483.getPosition();
    goto tail_peg_484;
    out_peg_486:
    Result result_peg_491(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_491.setValue(value);
            }
        
        
    }
    stream.update(result_peg_491.getPosition());
    
    return result_peg_491;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_505(stream, "expr4_rest");
    int myposition = position;
    tail_peg_496:
    Value right;
    Value new_left;
    Result result_peg_495(myposition);
    {
    
        result_peg_495 = rule_s(stream, result_peg_495.getPosition());
            if (result_peg_495.error()){
                goto out_peg_498;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_495.getPosition()))){
                    result_peg_495.nextPosition();
                } else {
                    goto out_peg_498;
                }
            }
            result_peg_495.setValue((void*) "|");
        
        
        
        result_peg_495 = rule_s(stream, result_peg_495.getPosition());
            if (result_peg_495.error()){
                goto out_peg_498;
            }
        
        
        
        result_peg_495 = rule_expr5(stream, result_peg_495.getPosition());
            if (result_peg_495.error()){
                goto out_peg_498;
            }
            right = result_peg_495.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_495.setValue(value);
            }
            new_left = result_peg_495.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_495.getPosition();
    goto tail_peg_496;
    out_peg_498:
    Result result_peg_503(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_503.setValue(value);
            }
        
        
    }
    stream.update(result_peg_503.getPosition());
    
    return result_peg_503;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_517(stream, "expr5_rest");
    int myposition = position;
    tail_peg_508:
    Value right;
    Value new_left;
    Result result_peg_507(myposition);
    {
    
        result_peg_507 = rule_s(stream, result_peg_507.getPosition());
            if (result_peg_507.error()){
                goto out_peg_510;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_507.getPosition()))){
                    result_peg_507.nextPosition();
                } else {
                    goto out_peg_510;
                }
            }
            result_peg_507.setValue((void*) "^");
        
        
        
        result_peg_507 = rule_s(stream, result_peg_507.getPosition());
            if (result_peg_507.error()){
                goto out_peg_510;
            }
        
        
        
        result_peg_507 = rule_expr6(stream, result_peg_507.getPosition());
            if (result_peg_507.error()){
                goto out_peg_510;
            }
            right = result_peg_507.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_507.setValue(value);
            }
            new_left = result_peg_507.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_507.getPosition();
    goto tail_peg_508;
    out_peg_510:
    Result result_peg_515(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_515.setValue(value);
            }
        
        
    }
    stream.update(result_peg_515.getPosition());
    
    return result_peg_515;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_529(stream, "expr6_rest");
    int myposition = position;
    tail_peg_520:
    Value right;
    Value new_left;
    Result result_peg_519(myposition);
    {
    
        result_peg_519 = rule_s(stream, result_peg_519.getPosition());
            if (result_peg_519.error()){
                goto out_peg_522;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_519.getPosition()))){
                    result_peg_519.nextPosition();
                } else {
                    goto out_peg_522;
                }
            }
            result_peg_519.setValue((void*) "&");
        
        
        
        result_peg_519 = rule_s(stream, result_peg_519.getPosition());
            if (result_peg_519.error()){
                goto out_peg_522;
            }
        
        
        
        result_peg_519 = rule_expr7(stream, result_peg_519.getPosition());
            if (result_peg_519.error()){
                goto out_peg_522;
            }
            right = result_peg_519.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_519.setValue(value);
            }
            new_left = result_peg_519.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_519.getPosition();
    goto tail_peg_520;
    out_peg_522:
    Result result_peg_527(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_527.setValue(value);
            }
        
        
    }
    stream.update(result_peg_527.getPosition());
    
    return result_peg_527;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_541(stream, "expr7_rest");
    int myposition = position;
    tail_peg_532:
    Value right;
    Value new_left;
    Result result_peg_531(myposition);
    {
    
        result_peg_531 = rule_s(stream, result_peg_531.getPosition());
            if (result_peg_531.error()){
                goto out_peg_534;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_531.getPosition()))){
                    result_peg_531.nextPosition();
                } else {
                    goto out_peg_534;
                }
            }
            result_peg_531.setValue((void*) ":=");
        
        
        
        result_peg_531 = rule_s(stream, result_peg_531.getPosition());
            if (result_peg_531.error()){
                goto out_peg_534;
            }
        
        
        
        result_peg_531 = rule_expr8(stream, result_peg_531.getPosition());
            if (result_peg_531.error()){
                goto out_peg_534;
            }
            right = result_peg_531.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_531.setValue(value);
            }
            new_left = result_peg_531.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_531.getPosition();
    goto tail_peg_532;
    out_peg_534:
    Result result_peg_539(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_539.setValue(value);
            }
        
        
    }
    stream.update(result_peg_539.getPosition());
    
    return result_peg_539;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_560(stream, "expr8_rest");
    int myposition = position;
    tail_peg_544:
    Value right;
    Value new_left;
    Result result_peg_543(myposition);
    {
    
        result_peg_543 = rule_s(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_546;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_543.getPosition()))){
                    result_peg_543.nextPosition();
                } else {
                    goto out_peg_546;
                }
            }
            result_peg_543.setValue((void*) "=");
        
        
        
        result_peg_543 = rule_s(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_546;
            }
        
        
        
        result_peg_543 = rule_expr9(stream, result_peg_543.getPosition());
            if (result_peg_543.error()){
                goto out_peg_546;
            }
            right = result_peg_543.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_543.setValue(value);
            }
            new_left = result_peg_543.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_543.getPosition();
    goto tail_peg_544;
    out_peg_546:
    Result result_peg_551(myposition);
    {
    
        result_peg_551 = rule_s(stream, result_peg_551.getPosition());
            if (result_peg_551.error()){
                goto out_peg_553;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_551.getPosition()))){
                    result_peg_551.nextPosition();
                } else {
                    goto out_peg_553;
                }
            }
            result_peg_551.setValue((void*) "!=");
        
        
        
        result_peg_551 = rule_s(stream, result_peg_551.getPosition());
            if (result_peg_551.error()){
                goto out_peg_553;
            }
        
        
        
        result_peg_551 = rule_expr9(stream, result_peg_551.getPosition());
            if (result_peg_551.error()){
                goto out_peg_553;
            }
            right = result_peg_551.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_551.setValue(value);
            }
            new_left = result_peg_551.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_551.getPosition();
    goto tail_peg_544;
    out_peg_553:
    Result result_peg_558(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_558.setValue(value);
            }
        
        
    }
    stream.update(result_peg_558.getPosition());
    
    return result_peg_558;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_572(stream, "expr9_rest");
    int myposition = position;
    tail_peg_563:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_562(myposition);
    {
    
        result_peg_562 = rule_s(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_565;
            }
        
        
        
        result_peg_562 = rule_compare(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_565;
            }
            maker = result_peg_562.getValues();
        
        
        
        result_peg_562 = rule_s(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_565;
            }
        
        
        
        result_peg_562 = rule_expr10(stream, result_peg_562.getPosition());
            if (result_peg_562.error()){
                goto out_peg_565;
            }
            right = result_peg_562.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_562.setValue(value);
            }
            new_left = result_peg_562.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_562.getPosition();
    goto tail_peg_563;
    out_peg_565:
    Result result_peg_570(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_570.setValue(value);
            }
        
        
    }
    stream.update(result_peg_570.getPosition());
    
    return result_peg_570;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_573 = stream.getColumn(position);
    if (column_peg_573.chunk8 != 0 && column_peg_573.chunk8->chunk_compare.calculated()){
        return column_peg_573.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_586(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_574(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_574.getPosition()))){
                    result_peg_574.nextPosition();
                } else {
                    goto out_peg_576;
                }
            }
            result_peg_574.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_574.setValue(value);
            }
        
        
    }
    
    if (column_peg_573.chunk8 == 0){
        column_peg_573.chunk8 = new Chunk8();
    }
    column_peg_573.chunk8->chunk_compare = result_peg_574;
    stream.update(result_peg_574.getPosition());
    
    
    return result_peg_574;
    out_peg_576:
    Result result_peg_577(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_577.getPosition()))){
                    result_peg_577.nextPosition();
                } else {
                    goto out_peg_579;
                }
            }
            result_peg_577.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_577.setValue(value);
            }
        
        
    }
    
    if (column_peg_573.chunk8 == 0){
        column_peg_573.chunk8 = new Chunk8();
    }
    column_peg_573.chunk8->chunk_compare = result_peg_577;
    stream.update(result_peg_577.getPosition());
    
    
    return result_peg_577;
    out_peg_579:
    Result result_peg_580(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_580.getPosition()))){
                    result_peg_580.nextPosition();
                } else {
                    goto out_peg_582;
                }
            }
            result_peg_580.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_580.setValue(value);
            }
        
        
    }
    
    if (column_peg_573.chunk8 == 0){
        column_peg_573.chunk8 = new Chunk8();
    }
    column_peg_573.chunk8->chunk_compare = result_peg_580;
    stream.update(result_peg_580.getPosition());
    
    
    return result_peg_580;
    out_peg_582:
    Result result_peg_583(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_583.getPosition()))){
                    result_peg_583.nextPosition();
                } else {
                    goto out_peg_585;
                }
            }
            result_peg_583.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_583.setValue(value);
            }
        
        
    }
    
    if (column_peg_573.chunk8 == 0){
        column_peg_573.chunk8 = new Chunk8();
    }
    column_peg_573.chunk8->chunk_compare = result_peg_583;
    stream.update(result_peg_583.getPosition());
    
    
    return result_peg_583;
    out_peg_585:
    
    if (column_peg_573.chunk8 == 0){
        column_peg_573.chunk8 = new Chunk8();
    }
    column_peg_573.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_587 = stream.getColumn(position);
    if (column_peg_587.chunk8 != 0 && column_peg_587.chunk8->chunk_all_compare.calculated()){
        return column_peg_587.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_594(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_588(myposition);
    
    result_peg_588 = rule_compare(stream, result_peg_588.getPosition());
    if (result_peg_588.error()){
        goto out_peg_589;
    }
    
    if (column_peg_587.chunk8 == 0){
        column_peg_587.chunk8 = new Chunk8();
    }
    column_peg_587.chunk8->chunk_all_compare = result_peg_588;
    stream.update(result_peg_588.getPosition());
    
    
    return result_peg_588;
    out_peg_589:
    Result result_peg_590(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_590.getPosition()))){
            result_peg_590.nextPosition();
        } else {
            goto out_peg_591;
        }
    }
    result_peg_590.setValue((void*) "=");
    
    if (column_peg_587.chunk8 == 0){
        column_peg_587.chunk8 = new Chunk8();
    }
    column_peg_587.chunk8->chunk_all_compare = result_peg_590;
    stream.update(result_peg_590.getPosition());
    
    
    return result_peg_590;
    out_peg_591:
    Result result_peg_592(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_592.getPosition()))){
            result_peg_592.nextPosition();
        } else {
            goto out_peg_593;
        }
    }
    result_peg_592.setValue((void*) "!=");
    
    if (column_peg_587.chunk8 == 0){
        column_peg_587.chunk8 = new Chunk8();
    }
    column_peg_587.chunk8->chunk_all_compare = result_peg_592;
    stream.update(result_peg_592.getPosition());
    
    
    return result_peg_592;
    out_peg_593:
    
    if (column_peg_587.chunk8 == 0){
        column_peg_587.chunk8 = new Chunk8();
    }
    column_peg_587.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_613(stream, "expr10_rest");
    int myposition = position;
    tail_peg_597:
    Value right;
    Value new_left;
    Result result_peg_596(myposition);
    {
    
        result_peg_596 = rule_s(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto out_peg_599;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_596.getPosition()))){
                    result_peg_596.nextPosition();
                } else {
                    goto out_peg_599;
                }
            }
            result_peg_596.setValue((void*) "+");
        
        
        
        result_peg_596 = rule_s(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto out_peg_599;
            }
        
        
        
        result_peg_596 = rule_expr11(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto out_peg_599;
            }
            right = result_peg_596.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_596.setValue(value);
            }
            new_left = result_peg_596.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_596.getPosition();
    goto tail_peg_597;
    out_peg_599:
    Result result_peg_604(myposition);
    {
    
        result_peg_604 = rule_s(stream, result_peg_604.getPosition());
            if (result_peg_604.error()){
                goto out_peg_606;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_604.getPosition()))){
                    result_peg_604.nextPosition();
                } else {
                    goto out_peg_606;
                }
            }
            result_peg_604.setValue((void*) "-");
        
        
        
        result_peg_604 = rule_s(stream, result_peg_604.getPosition());
            if (result_peg_604.error()){
                goto out_peg_606;
            }
        
        
        
        result_peg_604 = rule_expr11(stream, result_peg_604.getPosition());
            if (result_peg_604.error()){
                goto out_peg_606;
            }
            right = result_peg_604.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_604.setValue(value);
            }
            new_left = result_peg_604.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_604.getPosition();
    goto tail_peg_597;
    out_peg_606:
    Result result_peg_611(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_611.setValue(value);
            }
        
        
    }
    stream.update(result_peg_611.getPosition());
    
    return result_peg_611;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_639(stream, "expr11_rest");
    int myposition = position;
    tail_peg_616:
    Value right;
    Value new_left;
    Result result_peg_615(myposition);
    {
    
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_618;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_615.getPosition()))){
                    result_peg_615.nextPosition();
                } else {
                    goto out_peg_618;
                }
            }
            result_peg_615.setValue((void*) "*");
        
        
        
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_618;
            }
        
        
        
        result_peg_615 = rule_expr12(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_618;
            }
            right = result_peg_615.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_615.setValue(value);
            }
            new_left = result_peg_615.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_615.getPosition();
    goto tail_peg_616;
    out_peg_618:
    Result result_peg_623(myposition);
    {
    
        result_peg_623 = rule_s(stream, result_peg_623.getPosition());
            if (result_peg_623.error()){
                goto out_peg_625;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_623.getPosition()))){
                    result_peg_623.nextPosition();
                } else {
                    goto out_peg_625;
                }
            }
            result_peg_623.setValue((void*) "/");
        
        
        
        result_peg_623 = rule_s(stream, result_peg_623.getPosition());
            if (result_peg_623.error()){
                goto out_peg_625;
            }
        
        
        
        result_peg_623 = rule_expr12(stream, result_peg_623.getPosition());
            if (result_peg_623.error()){
                goto out_peg_625;
            }
            right = result_peg_623.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_623.setValue(value);
            }
            new_left = result_peg_623.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_623.getPosition();
    goto tail_peg_616;
    out_peg_625:
    Result result_peg_630(myposition);
    {
    
        result_peg_630 = rule_s(stream, result_peg_630.getPosition());
            if (result_peg_630.error()){
                goto out_peg_632;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_630.getPosition()))){
                    result_peg_630.nextPosition();
                } else {
                    goto out_peg_632;
                }
            }
            result_peg_630.setValue((void*) "%");
        
        
        
        result_peg_630 = rule_s(stream, result_peg_630.getPosition());
            if (result_peg_630.error()){
                goto out_peg_632;
            }
        
        
        
        result_peg_630 = rule_expr12(stream, result_peg_630.getPosition());
            if (result_peg_630.error()){
                goto out_peg_632;
            }
            right = result_peg_630.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_630.setValue(value);
            }
            new_left = result_peg_630.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_630.getPosition();
    goto tail_peg_616;
    out_peg_632:
    Result result_peg_637(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_637.setValue(value);
            }
        
        
    }
    stream.update(result_peg_637.getPosition());
    
    return result_peg_637;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_651(stream, "expr12_rest");
    int myposition = position;
    tail_peg_642:
    Value right;
    Value new_left;
    Result result_peg_641(myposition);
    {
    
        result_peg_641 = rule_s(stream, result_peg_641.getPosition());
            if (result_peg_641.error()){
                goto out_peg_644;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_641.getPosition()))){
                    result_peg_641.nextPosition();
                } else {
                    goto out_peg_644;
                }
            }
            result_peg_641.setValue((void*) "**");
        
        
        
        result_peg_641 = rule_s(stream, result_peg_641.getPosition());
            if (result_peg_641.error()){
                goto out_peg_644;
            }
        
        
        
        result_peg_641 = rule_expr13(stream, result_peg_641.getPosition());
            if (result_peg_641.error()){
                goto out_peg_644;
            }
            right = result_peg_641.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_641.setValue(value);
            }
            new_left = result_peg_641.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_641.getPosition();
    goto tail_peg_642;
    out_peg_644:
    Result result_peg_649(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_649.setValue(value);
            }
        
        
    }
    stream.update(result_peg_649.getPosition());
    
    return result_peg_649;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_652 = stream.getColumn(position);
    if (column_peg_652.chunk9 != 0 && column_peg_652.chunk9->chunk_unary.calculated()){
        return column_peg_652.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_681(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_653(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_653.getPosition()))){
                    result_peg_653.nextPosition();
                } else {
                    goto out_peg_655;
                }
            }
            result_peg_653.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_653.setValue(value);
            }
        
        
    }
    
    if (column_peg_652.chunk9 == 0){
        column_peg_652.chunk9 = new Chunk9();
    }
    column_peg_652.chunk9->chunk_unary = result_peg_653;
    stream.update(result_peg_653.getPosition());
    
    
    return result_peg_653;
    out_peg_655:
    Result result_peg_656(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_656.getPosition()))){
                    result_peg_656.nextPosition();
                } else {
                    goto out_peg_658;
                }
            }
            result_peg_656.setValue((void*) "-");
        
        
        
        Result result_peg_661(result_peg_656);
            {
                
                char letter_peg_666 = stream.get(result_peg_661.getPosition());
                if (letter_peg_666 != '\0' && strchr("0123456789", letter_peg_666) != NULL){
                    result_peg_661.nextPosition();
                    result_peg_661.setValue((void*) (long) letter_peg_666);
                } else {
                    goto out_peg_665;
                }
                
            }
            goto success_peg_662;
            out_peg_665:
            goto not_peg_660;
            success_peg_662:
            ;
            goto out_peg_658;
            not_peg_660:
            result_peg_656.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_656.setValue(value);
            }
        
        
    }
    
    if (column_peg_652.chunk9 == 0){
        column_peg_652.chunk9 = new Chunk9();
    }
    column_peg_652.chunk9->chunk_unary = result_peg_656;
    stream.update(result_peg_656.getPosition());
    
    
    return result_peg_656;
    out_peg_658:
    Result result_peg_667(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_667.getPosition()))){
                    result_peg_667.nextPosition();
                } else {
                    goto out_peg_669;
                }
            }
            result_peg_667.setValue((void*) "~");
        
        
        
        Result result_peg_672(result_peg_667);
            result_peg_672 = rule_key(stream, result_peg_672.getPosition());
            if (result_peg_672.error()){
                goto not_peg_671;
            }
            goto out_peg_669;
            not_peg_671:
            result_peg_667.setValue((void*)0);
        
        
        
        Result result_peg_675(result_peg_667);
            {
                
                char letter_peg_680 = stream.get(result_peg_675.getPosition());
                if (letter_peg_680 != '\0' && strchr("0123456789", letter_peg_680) != NULL){
                    result_peg_675.nextPosition();
                    result_peg_675.setValue((void*) (long) letter_peg_680);
                } else {
                    goto out_peg_679;
                }
                
            }
            goto success_peg_676;
            out_peg_679:
            goto not_peg_674;
            success_peg_676:
            ;
            goto out_peg_669;
            not_peg_674:
            result_peg_667.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_667.setValue(value);
            }
        
        
    }
    
    if (column_peg_652.chunk9 == 0){
        column_peg_652.chunk9 = new Chunk9();
    }
    column_peg_652.chunk9->chunk_unary = result_peg_667;
    stream.update(result_peg_667.getPosition());
    
    
    return result_peg_667;
    out_peg_669:
    
    if (column_peg_652.chunk9 == 0){
        column_peg_652.chunk9 = new Chunk9();
    }
    column_peg_652.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_682 = stream.getColumn(position);
    if (column_peg_682.chunk9 != 0 && column_peg_682.chunk9->chunk_expr13_real.calculated()){
        return column_peg_682.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_694(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_683(myposition);
    
    result_peg_683 = rule_function(stream, result_peg_683.getPosition());
    if (result_peg_683.error()){
        goto out_peg_684;
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_expr13_real = result_peg_683;
    stream.update(result_peg_683.getPosition());
    
    
    return result_peg_683;
    out_peg_684:
    Result result_peg_685(myposition);
    
    result_peg_685 = rule_value(stream, result_peg_685.getPosition());
    if (result_peg_685.error()){
        goto out_peg_686;
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_expr13_real = result_peg_685;
    stream.update(result_peg_685.getPosition());
    
    
    return result_peg_685;
    out_peg_686:
    Result result_peg_687(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_687.getPosition()))){
                    result_peg_687.nextPosition();
                } else {
                    goto out_peg_689;
                }
            }
            result_peg_687.setValue((void*) "(");
        
        
        
        result_peg_687 = rule_s(stream, result_peg_687.getPosition());
            if (result_peg_687.error()){
                goto out_peg_689;
            }
        
        
        
        result_peg_687 = rule_expr(stream, result_peg_687.getPosition());
            if (result_peg_687.error()){
                goto out_peg_689;
            }
            e = result_peg_687.getValues();
        
        
        
        result_peg_687 = rule_s(stream, result_peg_687.getPosition());
            if (result_peg_687.error()){
                goto out_peg_689;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_687.getPosition()))){
                    result_peg_687.nextPosition();
                } else {
                    goto out_peg_689;
                }
            }
            result_peg_687.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_687.setValue(value);
            }
        
        
    }
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_expr13_real = result_peg_687;
    stream.update(result_peg_687.getPosition());
    
    
    return result_peg_687;
    out_peg_689:
    
    if (column_peg_682.chunk9 == 0){
        column_peg_682.chunk9 = new Chunk9();
    }
    column_peg_682.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_695 = stream.getColumn(position);
    if (column_peg_695.chunk9 != 0 && column_peg_695.chunk9->chunk_function.calculated()){
        return column_peg_695.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_960(stream, "function");
    int myposition = position;
    
    Value name;
    Value arg1;
    Value arg2;
    Value arg3;
    Result result_peg_696(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_696.getPosition()))){
                    result_peg_696.nextPosition();
                } else {
                    goto out_peg_698;
                }
            }
            result_peg_696.setValue((void*) "abs");
            name = result_peg_696.getValues();
        
        
        
        result_peg_696 = rule_s(stream, result_peg_696.getPosition());
            if (result_peg_696.error()){
                goto out_peg_698;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_696.getPosition()))){
                    result_peg_696.nextPosition();
                } else {
                    goto out_peg_698;
                }
            }
            result_peg_696.setValue((void*) "(");
        
        
        
        result_peg_696 = rule_s(stream, result_peg_696.getPosition());
            if (result_peg_696.error()){
                goto out_peg_698;
            }
        
        
        
        result_peg_696 = rule_expr(stream, result_peg_696.getPosition());
            if (result_peg_696.error()){
                goto out_peg_698;
            }
            arg1 = result_peg_696.getValues();
        
        
        
        result_peg_696 = rule_s(stream, result_peg_696.getPosition());
            if (result_peg_696.error()){
                goto out_peg_698;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_696.getPosition()))){
                    result_peg_696.nextPosition();
                } else {
                    goto out_peg_698;
                }
            }
            result_peg_696.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_696.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_696;
    stream.update(result_peg_696.getPosition());
    
    
    return result_peg_696;
    out_peg_698:
    Result result_peg_705(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_705.getPosition()))){
                    result_peg_705.nextPosition();
                } else {
                    goto out_peg_707;
                }
            }
            result_peg_705.setValue((void*) "const");
            name = result_peg_705.getValues();
        
        
        
        result_peg_705 = rule_s(stream, result_peg_705.getPosition());
            if (result_peg_705.error()){
                goto out_peg_707;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_705.getPosition()))){
                    result_peg_705.nextPosition();
                } else {
                    goto out_peg_707;
                }
            }
            result_peg_705.setValue((void*) "(");
        
        
        
        result_peg_705 = rule_s(stream, result_peg_705.getPosition());
            if (result_peg_705.error()){
                goto out_peg_707;
            }
        
        
        
        result_peg_705 = rule_expr(stream, result_peg_705.getPosition());
            if (result_peg_705.error()){
                goto out_peg_707;
            }
            arg1 = result_peg_705.getValues();
        
        
        
        result_peg_705 = rule_s(stream, result_peg_705.getPosition());
            if (result_peg_705.error()){
                goto out_peg_707;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_705.getPosition()))){
                    result_peg_705.nextPosition();
                } else {
                    goto out_peg_707;
                }
            }
            result_peg_705.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_705.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_705;
    stream.update(result_peg_705.getPosition());
    
    
    return result_peg_705;
    out_peg_707:
    Result result_peg_714(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_714.getPosition()))){
                    result_peg_714.nextPosition();
                } else {
                    goto out_peg_716;
                }
            }
            result_peg_714.setValue((void*) "selfanimexist");
            name = result_peg_714.getValues();
        
        
        
        result_peg_714 = rule_s(stream, result_peg_714.getPosition());
            if (result_peg_714.error()){
                goto out_peg_716;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_714.getPosition()))){
                    result_peg_714.nextPosition();
                } else {
                    goto out_peg_716;
                }
            }
            result_peg_714.setValue((void*) "(");
        
        
        
        result_peg_714 = rule_s(stream, result_peg_714.getPosition());
            if (result_peg_714.error()){
                goto out_peg_716;
            }
        
        
        
        result_peg_714 = rule_expr(stream, result_peg_714.getPosition());
            if (result_peg_714.error()){
                goto out_peg_716;
            }
            arg1 = result_peg_714.getValues();
        
        
        
        result_peg_714 = rule_s(stream, result_peg_714.getPosition());
            if (result_peg_714.error()){
                goto out_peg_716;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_714.getPosition()))){
                    result_peg_714.nextPosition();
                } else {
                    goto out_peg_716;
                }
            }
            result_peg_714.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_714.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_714;
    stream.update(result_peg_714.getPosition());
    
    
    return result_peg_714;
    out_peg_716:
    Result result_peg_723(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_723.getPosition()))){
                    result_peg_723.nextPosition();
                } else {
                    goto out_peg_725;
                }
            }
            result_peg_723.setValue((void*) "ifelse");
            name = result_peg_723.getValues();
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_723.getPosition()))){
                    result_peg_723.nextPosition();
                } else {
                    goto out_peg_725;
                }
            }
            result_peg_723.setValue((void*) "(");
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        result_peg_723 = rule_expr_c(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
            arg1 = result_peg_723.getValues();
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_723.getPosition()))){
                    result_peg_723.nextPosition();
                } else {
                    goto out_peg_725;
                }
            }
            result_peg_723.setValue((void*) ",");
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        result_peg_723 = rule_expr_c(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
            arg2 = result_peg_723.getValues();
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_723.getPosition()))){
                    result_peg_723.nextPosition();
                } else {
                    goto out_peg_725;
                }
            }
            result_peg_723.setValue((void*) ",");
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        result_peg_723 = rule_expr_c(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
            arg3 = result_peg_723.getValues();
        
        
        
        result_peg_723 = rule_s(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_723.getPosition()))){
                    result_peg_723.nextPosition();
                } else {
                    goto out_peg_725;
                }
            }
            result_peg_723.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction("ifelse", arg1, arg2, arg3);
                result_peg_723.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_723;
    stream.update(result_peg_723.getPosition());
    
    
    return result_peg_723;
    out_peg_725:
    Result result_peg_740(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_740.getPosition()))){
                    result_peg_740.nextPosition();
                } else {
                    goto out_peg_742;
                }
            }
            result_peg_740.setValue((void*) "gethitvar");
            name = result_peg_740.getValues();
        
        
        
        result_peg_740 = rule_s(stream, result_peg_740.getPosition());
            if (result_peg_740.error()){
                goto out_peg_742;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_740.getPosition()))){
                    result_peg_740.nextPosition();
                } else {
                    goto out_peg_742;
                }
            }
            result_peg_740.setValue((void*) "(");
        
        
        
        result_peg_740 = rule_s(stream, result_peg_740.getPosition());
            if (result_peg_740.error()){
                goto out_peg_742;
            }
        
        
        
        result_peg_740 = rule_expr_c(stream, result_peg_740.getPosition());
            if (result_peg_740.error()){
                goto out_peg_742;
            }
            arg1 = result_peg_740.getValues();
        
        
        
        result_peg_740 = rule_s(stream, result_peg_740.getPosition());
            if (result_peg_740.error()){
                goto out_peg_742;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_740.getPosition()))){
                    result_peg_740.nextPosition();
                } else {
                    goto out_peg_742;
                }
            }
            result_peg_740.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_740.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_740;
    stream.update(result_peg_740.getPosition());
    
    
    return result_peg_740;
    out_peg_742:
    Result result_peg_749(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_749.getPosition()))){
                    result_peg_749.nextPosition();
                } else {
                    goto out_peg_751;
                }
            }
            result_peg_749.setValue((void*) "floor");
            name = result_peg_749.getValues();
        
        
        
        result_peg_749 = rule_s(stream, result_peg_749.getPosition());
            if (result_peg_749.error()){
                goto out_peg_751;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_749.getPosition()))){
                    result_peg_749.nextPosition();
                } else {
                    goto out_peg_751;
                }
            }
            result_peg_749.setValue((void*) "(");
        
        
        
        result_peg_749 = rule_s(stream, result_peg_749.getPosition());
            if (result_peg_749.error()){
                goto out_peg_751;
            }
        
        
        
        result_peg_749 = rule_expr_c(stream, result_peg_749.getPosition());
            if (result_peg_749.error()){
                goto out_peg_751;
            }
            arg1 = result_peg_749.getValues();
        
        
        
        result_peg_749 = rule_s(stream, result_peg_749.getPosition());
            if (result_peg_749.error()){
                goto out_peg_751;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_749.getPosition()))){
                    result_peg_749.nextPosition();
                } else {
                    goto out_peg_751;
                }
            }
            result_peg_749.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_749.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_749;
    stream.update(result_peg_749.getPosition());
    
    
    return result_peg_749;
    out_peg_751:
    Result result_peg_758(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_758.getPosition()))){
                    result_peg_758.nextPosition();
                } else {
                    goto out_peg_760;
                }
            }
            result_peg_758.setValue((void*) "ceil");
            name = result_peg_758.getValues();
        
        
        
        result_peg_758 = rule_s(stream, result_peg_758.getPosition());
            if (result_peg_758.error()){
                goto out_peg_760;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_758.getPosition()))){
                    result_peg_758.nextPosition();
                } else {
                    goto out_peg_760;
                }
            }
            result_peg_758.setValue((void*) "(");
        
        
        
        result_peg_758 = rule_s(stream, result_peg_758.getPosition());
            if (result_peg_758.error()){
                goto out_peg_760;
            }
        
        
        
        result_peg_758 = rule_expr_c(stream, result_peg_758.getPosition());
            if (result_peg_758.error()){
                goto out_peg_760;
            }
            arg1 = result_peg_758.getValues();
        
        
        
        result_peg_758 = rule_s(stream, result_peg_758.getPosition());
            if (result_peg_758.error()){
                goto out_peg_760;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_758.getPosition()))){
                    result_peg_758.nextPosition();
                } else {
                    goto out_peg_760;
                }
            }
            result_peg_758.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_758.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_758;
    stream.update(result_peg_758.getPosition());
    
    
    return result_peg_758;
    out_peg_760:
    Result result_peg_767(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("sin"[i], stream.get(result_peg_767.getPosition()))){
                    result_peg_767.nextPosition();
                } else {
                    goto out_peg_769;
                }
            }
            result_peg_767.setValue((void*) "sin");
            name = result_peg_767.getValues();
        
        
        
        result_peg_767 = rule_s(stream, result_peg_767.getPosition());
            if (result_peg_767.error()){
                goto out_peg_769;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_767.getPosition()))){
                    result_peg_767.nextPosition();
                } else {
                    goto out_peg_769;
                }
            }
            result_peg_767.setValue((void*) "(");
        
        
        
        result_peg_767 = rule_s(stream, result_peg_767.getPosition());
            if (result_peg_767.error()){
                goto out_peg_769;
            }
        
        
        
        result_peg_767 = rule_expr_c(stream, result_peg_767.getPosition());
            if (result_peg_767.error()){
                goto out_peg_769;
            }
            arg1 = result_peg_767.getValues();
        
        
        
        result_peg_767 = rule_s(stream, result_peg_767.getPosition());
            if (result_peg_767.error()){
                goto out_peg_769;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_767.getPosition()))){
                    result_peg_767.nextPosition();
                } else {
                    goto out_peg_769;
                }
            }
            result_peg_767.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_767.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_767;
    stream.update(result_peg_767.getPosition());
    
    
    return result_peg_767;
    out_peg_769:
    Result result_peg_776(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_776.getPosition()))){
                    result_peg_776.nextPosition();
                } else {
                    goto out_peg_778;
                }
            }
            result_peg_776.setValue((void*) "sysvar");
            name = result_peg_776.getValues();
        
        
        
        result_peg_776 = rule_s(stream, result_peg_776.getPosition());
            if (result_peg_776.error()){
                goto out_peg_778;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_776.getPosition()))){
                    result_peg_776.nextPosition();
                } else {
                    goto out_peg_778;
                }
            }
            result_peg_776.setValue((void*) "(");
        
        
        
        result_peg_776 = rule_s(stream, result_peg_776.getPosition());
            if (result_peg_776.error()){
                goto out_peg_778;
            }
        
        
        
        result_peg_776 = rule_integer(stream, result_peg_776.getPosition());
            if (result_peg_776.error()){
                goto out_peg_778;
            }
            arg1 = result_peg_776.getValues();
        
        
        
        result_peg_776 = rule_s(stream, result_peg_776.getPosition());
            if (result_peg_776.error()){
                goto out_peg_778;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_776.getPosition()))){
                    result_peg_776.nextPosition();
                } else {
                    goto out_peg_778;
                }
            }
            result_peg_776.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_776.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_776;
    stream.update(result_peg_776.getPosition());
    
    
    return result_peg_776;
    out_peg_778:
    Result result_peg_785(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_785.getPosition()))){
                    result_peg_785.nextPosition();
                } else {
                    goto out_peg_787;
                }
            }
            result_peg_785.setValue((void*) "var");
            name = result_peg_785.getValues();
        
        
        
        result_peg_785 = rule_s(stream, result_peg_785.getPosition());
            if (result_peg_785.error()){
                goto out_peg_787;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_785.getPosition()))){
                    result_peg_785.nextPosition();
                } else {
                    goto out_peg_787;
                }
            }
            result_peg_785.setValue((void*) "(");
        
        
        
        result_peg_785 = rule_s(stream, result_peg_785.getPosition());
            if (result_peg_785.error()){
                goto out_peg_787;
            }
        
        
        
        result_peg_785 = rule_integer(stream, result_peg_785.getPosition());
            if (result_peg_785.error()){
                goto out_peg_787;
            }
            arg1 = result_peg_785.getValues();
        
        
        
        result_peg_785 = rule_s(stream, result_peg_785.getPosition());
            if (result_peg_785.error()){
                goto out_peg_787;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_785.getPosition()))){
                    result_peg_785.nextPosition();
                } else {
                    goto out_peg_787;
                }
            }
            result_peg_785.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_785.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_785;
    stream.update(result_peg_785.getPosition());
    
    
    return result_peg_785;
    out_peg_787:
    Result result_peg_794(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_794.getPosition()))){
                    result_peg_794.nextPosition();
                } else {
                    goto out_peg_796;
                }
            }
            result_peg_794.setValue((void*) "numexplod");
            name = result_peg_794.getValues();
        
        
        
        result_peg_794 = rule_s(stream, result_peg_794.getPosition());
            if (result_peg_794.error()){
                goto out_peg_796;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_794.getPosition()))){
                    result_peg_794.nextPosition();
                } else {
                    goto out_peg_796;
                }
            }
            result_peg_794.setValue((void*) "(");
        
        
        
        result_peg_794 = rule_s(stream, result_peg_794.getPosition());
            if (result_peg_794.error()){
                goto out_peg_796;
            }
        
        
        
        result_peg_794 = rule_integer(stream, result_peg_794.getPosition());
            if (result_peg_794.error()){
                goto out_peg_796;
            }
            arg1 = result_peg_794.getValues();
        
        
        
        result_peg_794 = rule_s(stream, result_peg_794.getPosition());
            if (result_peg_794.error()){
                goto out_peg_796;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_794.getPosition()))){
                    result_peg_794.nextPosition();
                } else {
                    goto out_peg_796;
                }
            }
            result_peg_794.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_794.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_794;
    stream.update(result_peg_794.getPosition());
    
    
    return result_peg_794;
    out_peg_796:
    Result result_peg_803(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_803.getPosition()))){
                    result_peg_803.nextPosition();
                } else {
                    goto out_peg_805;
                }
            }
            result_peg_803.setValue((void*) "numhelper");
            name = result_peg_803.getValues();
        
        
        
        result_peg_803 = rule_s(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_803.getPosition()))){
                    result_peg_803.nextPosition();
                } else {
                    goto out_peg_805;
                }
            }
            result_peg_803.setValue((void*) "(");
        
        
        
        result_peg_803 = rule_s(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        result_peg_803 = rule_integer(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
            arg1 = result_peg_803.getValues();
        
        
        
        result_peg_803 = rule_s(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_803.getPosition()))){
                    result_peg_803.nextPosition();
                } else {
                    goto out_peg_805;
                }
            }
            result_peg_803.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_803.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_803;
    stream.update(result_peg_803.getPosition());
    
    
    return result_peg_803;
    out_peg_805:
    Result result_peg_812(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_812.getPosition()))){
                    result_peg_812.nextPosition();
                } else {
                    goto out_peg_814;
                }
            }
            result_peg_812.setValue((void*) "numprojid");
            name = result_peg_812.getValues();
        
        
        
        result_peg_812 = rule_s(stream, result_peg_812.getPosition());
            if (result_peg_812.error()){
                goto out_peg_814;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_812.getPosition()))){
                    result_peg_812.nextPosition();
                } else {
                    goto out_peg_814;
                }
            }
            result_peg_812.setValue((void*) "(");
        
        
        
        result_peg_812 = rule_s(stream, result_peg_812.getPosition());
            if (result_peg_812.error()){
                goto out_peg_814;
            }
        
        
        
        result_peg_812 = rule_integer(stream, result_peg_812.getPosition());
            if (result_peg_812.error()){
                goto out_peg_814;
            }
            arg1 = result_peg_812.getValues();
        
        
        
        result_peg_812 = rule_s(stream, result_peg_812.getPosition());
            if (result_peg_812.error()){
                goto out_peg_814;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_812.getPosition()))){
                    result_peg_812.nextPosition();
                } else {
                    goto out_peg_814;
                }
            }
            result_peg_812.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_812.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_812;
    stream.update(result_peg_812.getPosition());
    
    
    return result_peg_812;
    out_peg_814:
    Result result_peg_821(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_821.getPosition()))){
                    result_peg_821.nextPosition();
                } else {
                    goto out_peg_823;
                }
            }
            result_peg_821.setValue((void*) "helper");
            name = result_peg_821.getValues();
        
        
        
        result_peg_821 = rule_s(stream, result_peg_821.getPosition());
            if (result_peg_821.error()){
                goto out_peg_823;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_821.getPosition()))){
                    result_peg_821.nextPosition();
                } else {
                    goto out_peg_823;
                }
            }
            result_peg_821.setValue((void*) "(");
        
        
        
        result_peg_821 = rule_s(stream, result_peg_821.getPosition());
            if (result_peg_821.error()){
                goto out_peg_823;
            }
        
        
        
        result_peg_821 = rule_integer(stream, result_peg_821.getPosition());
            if (result_peg_821.error()){
                goto out_peg_823;
            }
            arg1 = result_peg_821.getValues();
        
        
        
        result_peg_821 = rule_s(stream, result_peg_821.getPosition());
            if (result_peg_821.error()){
                goto out_peg_823;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_821.getPosition()))){
                    result_peg_821.nextPosition();
                } else {
                    goto out_peg_823;
                }
            }
            result_peg_821.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_821.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_821;
    stream.update(result_peg_821.getPosition());
    
    
    return result_peg_821;
    out_peg_823:
    Result result_peg_830(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_830.getPosition()))){
                    result_peg_830.nextPosition();
                } else {
                    goto out_peg_832;
                }
            }
            result_peg_830.setValue((void*) "fvar");
            name = result_peg_830.getValues();
        
        
        
        result_peg_830 = rule_s(stream, result_peg_830.getPosition());
            if (result_peg_830.error()){
                goto out_peg_832;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_830.getPosition()))){
                    result_peg_830.nextPosition();
                } else {
                    goto out_peg_832;
                }
            }
            result_peg_830.setValue((void*) "(");
        
        
        
        result_peg_830 = rule_s(stream, result_peg_830.getPosition());
            if (result_peg_830.error()){
                goto out_peg_832;
            }
        
        
        
        result_peg_830 = rule_integer(stream, result_peg_830.getPosition());
            if (result_peg_830.error()){
                goto out_peg_832;
            }
            arg1 = result_peg_830.getValues();
        
        
        
        result_peg_830 = rule_s(stream, result_peg_830.getPosition());
            if (result_peg_830.error()){
                goto out_peg_832;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_830.getPosition()))){
                    result_peg_830.nextPosition();
                } else {
                    goto out_peg_832;
                }
            }
            result_peg_830.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_830.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_830;
    stream.update(result_peg_830.getPosition());
    
    
    return result_peg_830;
    out_peg_832:
    Result result_peg_839(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("enemynear"[i], stream.get(result_peg_839.getPosition()))){
                    result_peg_839.nextPosition();
                } else {
                    goto out_peg_841;
                }
            }
            result_peg_839.setValue((void*) "enemynear");
            name = result_peg_839.getValues();
        
        
        
        result_peg_839 = rule_s(stream, result_peg_839.getPosition());
            if (result_peg_839.error()){
                goto out_peg_841;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_839.getPosition()))){
                    result_peg_839.nextPosition();
                } else {
                    goto out_peg_841;
                }
            }
            result_peg_839.setValue((void*) "(");
        
        
        
        result_peg_839 = rule_s(stream, result_peg_839.getPosition());
            if (result_peg_839.error()){
                goto out_peg_841;
            }
        
        
        
        result_peg_839 = rule_integer(stream, result_peg_839.getPosition());
            if (result_peg_839.error()){
                goto out_peg_841;
            }
            arg1 = result_peg_839.getValues();
        
        
        
        result_peg_839 = rule_s(stream, result_peg_839.getPosition());
            if (result_peg_839.error()){
                goto out_peg_841;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_839.getPosition()))){
                    result_peg_839.nextPosition();
                } else {
                    goto out_peg_841;
                }
            }
            result_peg_839.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_839.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_839;
    stream.update(result_peg_839.getPosition());
    
    
    return result_peg_839;
    out_peg_841:
    Result result_peg_848(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("target"[i], stream.get(result_peg_848.getPosition()))){
                    result_peg_848.nextPosition();
                } else {
                    goto out_peg_850;
                }
            }
            result_peg_848.setValue((void*) "target");
            name = result_peg_848.getValues();
        
        
        
        result_peg_848 = rule_s(stream, result_peg_848.getPosition());
            if (result_peg_848.error()){
                goto out_peg_850;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_848.getPosition()))){
                    result_peg_848.nextPosition();
                } else {
                    goto out_peg_850;
                }
            }
            result_peg_848.setValue((void*) "(");
        
        
        
        result_peg_848 = rule_s(stream, result_peg_848.getPosition());
            if (result_peg_848.error()){
                goto out_peg_850;
            }
        
        
        
        result_peg_848 = rule_integer(stream, result_peg_848.getPosition());
            if (result_peg_848.error()){
                goto out_peg_850;
            }
            arg1 = result_peg_848.getValues();
        
        
        
        result_peg_848 = rule_s(stream, result_peg_848.getPosition());
            if (result_peg_848.error()){
                goto out_peg_850;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_848.getPosition()))){
                    result_peg_848.nextPosition();
                } else {
                    goto out_peg_850;
                }
            }
            result_peg_848.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_848.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_848;
    stream.update(result_peg_848.getPosition());
    
    
    return result_peg_848;
    out_peg_850:
    Result result_peg_857(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_857.getPosition()))){
                    result_peg_857.nextPosition();
                } else {
                    goto out_peg_859;
                }
            }
            result_peg_857.setValue((void*) "ishelper");
            name = result_peg_857.getValues();
        
        
        
        result_peg_857 = rule_s(stream, result_peg_857.getPosition());
            if (result_peg_857.error()){
                goto out_peg_859;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_857.getPosition()))){
                    result_peg_857.nextPosition();
                } else {
                    goto out_peg_859;
                }
            }
            result_peg_857.setValue((void*) "(");
        
        
        
        result_peg_857 = rule_s(stream, result_peg_857.getPosition());
            if (result_peg_857.error()){
                goto out_peg_859;
            }
        
        
        
        result_peg_857 = rule_integer(stream, result_peg_857.getPosition());
            if (result_peg_857.error()){
                goto out_peg_859;
            }
            arg1 = result_peg_857.getValues();
        
        
        
        result_peg_857 = rule_s(stream, result_peg_857.getPosition());
            if (result_peg_857.error()){
                goto out_peg_859;
            }
        
        
        
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
                value = makeFunction(name, arg1);
                result_peg_857.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_857;
    stream.update(result_peg_857.getPosition());
    
    
    return result_peg_857;
    out_peg_859:
    Result result_peg_866(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numtarget"[i], stream.get(result_peg_866.getPosition()))){
                    result_peg_866.nextPosition();
                } else {
                    goto out_peg_868;
                }
            }
            result_peg_866.setValue((void*) "numtarget");
            name = result_peg_866.getValues();
        
        
        
        result_peg_866 = rule_s(stream, result_peg_866.getPosition());
            if (result_peg_866.error()){
                goto out_peg_868;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_866.getPosition()))){
                    result_peg_866.nextPosition();
                } else {
                    goto out_peg_868;
                }
            }
            result_peg_866.setValue((void*) "(");
        
        
        
        result_peg_866 = rule_s(stream, result_peg_866.getPosition());
            if (result_peg_866.error()){
                goto out_peg_868;
            }
        
        
        
        result_peg_866 = rule_expr_c(stream, result_peg_866.getPosition());
            if (result_peg_866.error()){
                goto out_peg_868;
            }
            arg1 = result_peg_866.getValues();
        
        
        
        result_peg_866 = rule_s(stream, result_peg_866.getPosition());
            if (result_peg_866.error()){
                goto out_peg_868;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_866.getPosition()))){
                    result_peg_866.nextPosition();
                } else {
                    goto out_peg_868;
                }
            }
            result_peg_866.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_866.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_866;
    stream.update(result_peg_866.getPosition());
    
    
    return result_peg_866;
    out_peg_868:
    Result result_peg_875(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_875.getPosition()))){
                    result_peg_875.nextPosition();
                } else {
                    goto out_peg_877;
                }
            }
            result_peg_875.setValue((void*) "animelemtime");
            name = result_peg_875.getValues();
        
        
        
        result_peg_875 = rule_s(stream, result_peg_875.getPosition());
            if (result_peg_875.error()){
                goto out_peg_877;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_875.getPosition()))){
                    result_peg_875.nextPosition();
                } else {
                    goto out_peg_877;
                }
            }
            result_peg_875.setValue((void*) "(");
        
        
        
        result_peg_875 = rule_s(stream, result_peg_875.getPosition());
            if (result_peg_875.error()){
                goto out_peg_877;
            }
        
        
        
        result_peg_875 = rule_expr_c(stream, result_peg_875.getPosition());
            if (result_peg_875.error()){
                goto out_peg_877;
            }
            arg1 = result_peg_875.getValues();
        
        
        
        result_peg_875 = rule_s(stream, result_peg_875.getPosition());
            if (result_peg_875.error()){
                goto out_peg_877;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_875.getPosition()))){
                    result_peg_875.nextPosition();
                } else {
                    goto out_peg_877;
                }
            }
            result_peg_875.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_875.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_875;
    stream.update(result_peg_875.getPosition());
    
    
    return result_peg_875;
    out_peg_877:
    Result result_peg_884(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_884.getPosition()))){
                    result_peg_884.nextPosition();
                } else {
                    goto out_peg_886;
                }
            }
            result_peg_884.setValue((void*) "animelemno");
            name = result_peg_884.getValues();
        
        
        
        result_peg_884 = rule_s(stream, result_peg_884.getPosition());
            if (result_peg_884.error()){
                goto out_peg_886;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_884.getPosition()))){
                    result_peg_884.nextPosition();
                } else {
                    goto out_peg_886;
                }
            }
            result_peg_884.setValue((void*) "(");
        
        
        
        result_peg_884 = rule_s(stream, result_peg_884.getPosition());
            if (result_peg_884.error()){
                goto out_peg_886;
            }
        
        
        
        result_peg_884 = rule_expr_c(stream, result_peg_884.getPosition());
            if (result_peg_884.error()){
                goto out_peg_886;
            }
            arg1 = result_peg_884.getValues();
        
        
        
        result_peg_884 = rule_s(stream, result_peg_884.getPosition());
            if (result_peg_884.error()){
                goto out_peg_886;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_884.getPosition()))){
                    result_peg_884.nextPosition();
                } else {
                    goto out_peg_886;
                }
            }
            result_peg_884.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_884.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_884;
    stream.update(result_peg_884.getPosition());
    
    
    return result_peg_884;
    out_peg_886:
    Result result_peg_893(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_893.getPosition()))){
                    result_peg_893.nextPosition();
                } else {
                    goto out_peg_895;
                }
            }
            result_peg_893.setValue((void*) "animexist");
            name = result_peg_893.getValues();
        
        
        
        result_peg_893 = rule_s(stream, result_peg_893.getPosition());
            if (result_peg_893.error()){
                goto out_peg_895;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_893.getPosition()))){
                    result_peg_893.nextPosition();
                } else {
                    goto out_peg_895;
                }
            }
            result_peg_893.setValue((void*) "(");
        
        
        
        result_peg_893 = rule_s(stream, result_peg_893.getPosition());
            if (result_peg_893.error()){
                goto out_peg_895;
            }
        
        
        
        result_peg_893 = rule_expr_c(stream, result_peg_893.getPosition());
            if (result_peg_893.error()){
                goto out_peg_895;
            }
            arg1 = result_peg_893.getValues();
        
        
        
        result_peg_893 = rule_s(stream, result_peg_893.getPosition());
            if (result_peg_893.error()){
                goto out_peg_895;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_893.getPosition()))){
                    result_peg_893.nextPosition();
                } else {
                    goto out_peg_895;
                }
            }
            result_peg_893.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_893.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_893;
    stream.update(result_peg_893.getPosition());
    
    
    return result_peg_893;
    out_peg_895:
    Result result_peg_902(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_902.getPosition()))){
                    result_peg_902.nextPosition();
                } else {
                    goto out_peg_904;
                }
            }
            result_peg_902.setValue((void*) "projguarded");
            name = result_peg_902.getValues();
        
        
        
        result_peg_902 = rule_s(stream, result_peg_902.getPosition());
            if (result_peg_902.error()){
                goto out_peg_904;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_902.getPosition()))){
                    result_peg_902.nextPosition();
                } else {
                    goto out_peg_904;
                }
            }
            result_peg_902.setValue((void*) "(");
        
        
        
        result_peg_902 = rule_s(stream, result_peg_902.getPosition());
            if (result_peg_902.error()){
                goto out_peg_904;
            }
        
        
        
        result_peg_902 = rule_expr_c(stream, result_peg_902.getPosition());
            if (result_peg_902.error()){
                goto out_peg_904;
            }
            arg1 = result_peg_902.getValues();
        
        
        
        result_peg_902 = rule_s(stream, result_peg_902.getPosition());
            if (result_peg_902.error()){
                goto out_peg_904;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_902.getPosition()))){
                    result_peg_902.nextPosition();
                } else {
                    goto out_peg_904;
                }
            }
            result_peg_902.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_902.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_902;
    stream.update(result_peg_902.getPosition());
    
    
    return result_peg_902;
    out_peg_904:
    Result result_peg_911(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projhittime"[i], stream.get(result_peg_911.getPosition()))){
                    result_peg_911.nextPosition();
                } else {
                    goto out_peg_913;
                }
            }
            result_peg_911.setValue((void*) "projhittime");
            name = result_peg_911.getValues();
        
        
        
        result_peg_911 = rule_s(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_913;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_911.getPosition()))){
                    result_peg_911.nextPosition();
                } else {
                    goto out_peg_913;
                }
            }
            result_peg_911.setValue((void*) "(");
        
        
        
        result_peg_911 = rule_s(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_913;
            }
        
        
        
        result_peg_911 = rule_expr_c(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_913;
            }
            arg1 = result_peg_911.getValues();
        
        
        
        result_peg_911 = rule_s(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_913;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_911.getPosition()))){
                    result_peg_911.nextPosition();
                } else {
                    goto out_peg_913;
                }
            }
            result_peg_911.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_911.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_911;
    stream.update(result_peg_911.getPosition());
    
    
    return result_peg_911;
    out_peg_913:
    Result result_peg_920(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_920.getPosition()))){
                    result_peg_920.nextPosition();
                } else {
                    goto out_peg_922;
                }
            }
            result_peg_920.setValue((void*) "projcontact");
            name = result_peg_920.getValues();
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_920.getPosition()))){
                    result_peg_920.nextPosition();
                } else {
                    goto out_peg_922;
                }
            }
            result_peg_920.setValue((void*) "(");
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        result_peg_920 = rule_expr_c(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
            arg1 = result_peg_920.getValues();
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_920.getPosition()))){
                    result_peg_920.nextPosition();
                } else {
                    goto out_peg_922;
                }
            }
            result_peg_920.setValue((void*) ")");
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_920.getPosition()))){
                    result_peg_920.nextPosition();
                } else {
                    goto out_peg_922;
                }
            }
            result_peg_920.setValue((void*) "=");
        
        
        
        result_peg_920 = rule_s(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        result_peg_920 = rule_integer(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        result_peg_920 = rule_function_rest(stream, result_peg_920.getPosition());
            if (result_peg_920.error()){
                goto out_peg_922;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_920.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_920;
    stream.update(result_peg_920.getPosition());
    
    
    return result_peg_920;
    out_peg_922:
    Result result_peg_934(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_934.getPosition()))){
                    result_peg_934.nextPosition();
                } else {
                    goto out_peg_936;
                }
            }
            result_peg_934.setValue((void*) "animelem");
            name = result_peg_934.getValues();
        
        
        
        result_peg_934 = rule_s(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_934.getPosition()))){
                    result_peg_934.nextPosition();
                } else {
                    goto out_peg_936;
                }
            }
            result_peg_934.setValue((void*) "=");
        
        
        
        result_peg_934 = rule_s(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
        
        
        
        result_peg_934 = rule_integer(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
            arg1 = result_peg_934.getValues();
        
        
        
        result_peg_934 = rule_function_rest(stream, result_peg_934.getPosition());
            if (result_peg_934.error()){
                goto out_peg_936;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction(name, arg1);
                result_peg_934.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_934;
    stream.update(result_peg_934.getPosition());
    
    
    return result_peg_934;
    out_peg_936:
    Result result_peg_942(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_942.getPosition()))){
                    result_peg_942.nextPosition();
                } else {
                    goto out_peg_944;
                }
            }
            result_peg_942.setValue((void*) "numhelper");
            name = result_peg_942.getValues();
        
        
        
        result_peg_942 = rule_s(stream, result_peg_942.getPosition());
            if (result_peg_942.error()){
                goto out_peg_944;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_942.getPosition()))){
                    result_peg_942.nextPosition();
                } else {
                    goto out_peg_944;
                }
            }
            result_peg_942.setValue((void*) "(");
        
        
        
        result_peg_942 = rule_s(stream, result_peg_942.getPosition());
            if (result_peg_942.error()){
                goto out_peg_944;
            }
        
        
        
        result_peg_942 = rule_expr_c(stream, result_peg_942.getPosition());
            if (result_peg_942.error()){
                goto out_peg_944;
            }
            arg1 = result_peg_942.getValues();
        
        
        
        result_peg_942 = rule_s(stream, result_peg_942.getPosition());
            if (result_peg_942.error()){
                goto out_peg_944;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_942.getPosition()))){
                    result_peg_942.nextPosition();
                } else {
                    goto out_peg_944;
                }
            }
            result_peg_942.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_942.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_942;
    stream.update(result_peg_942.getPosition());
    
    
    return result_peg_942;
    out_peg_944:
    Result result_peg_951(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_951.getPosition()))){
                    result_peg_951.nextPosition();
                } else {
                    goto out_peg_953;
                }
            }
            result_peg_951.setValue((void*) "helper");
            name = result_peg_951.getValues();
        
        
        
        result_peg_951 = rule_s(stream, result_peg_951.getPosition());
            if (result_peg_951.error()){
                goto out_peg_953;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_951.getPosition()))){
                    result_peg_951.nextPosition();
                } else {
                    goto out_peg_953;
                }
            }
            result_peg_951.setValue((void*) "(");
        
        
        
        result_peg_951 = rule_s(stream, result_peg_951.getPosition());
            if (result_peg_951.error()){
                goto out_peg_953;
            }
        
        
        
        result_peg_951 = rule_expr_c(stream, result_peg_951.getPosition());
            if (result_peg_951.error()){
                goto out_peg_953;
            }
            arg1 = result_peg_951.getValues();
        
        
        
        result_peg_951 = rule_s(stream, result_peg_951.getPosition());
            if (result_peg_951.error()){
                goto out_peg_953;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_951.getPosition()))){
                    result_peg_951.nextPosition();
                } else {
                    goto out_peg_953;
                }
            }
            result_peg_951.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_951.setValue(value);
            }
        
        
    }
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = result_peg_951;
    stream.update(result_peg_951.getPosition());
    
    
    return result_peg_951;
    out_peg_953:
    
    if (column_peg_695.chunk9 == 0){
        column_peg_695.chunk9 = new Chunk9();
    }
    column_peg_695.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_961 = stream.getColumn(position);
    if (column_peg_961.chunk10 != 0 && column_peg_961.chunk10->chunk_function_rest.calculated()){
        return column_peg_961.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_971(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_962(myposition);
    
    {
    
        result_peg_962 = rule_s(stream, result_peg_962.getPosition());
            if (result_peg_962.error()){
                goto out_peg_964;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_962.getPosition()))){
                    result_peg_962.nextPosition();
                } else {
                    goto out_peg_964;
                }
            }
            result_peg_962.setValue((void*) ",");
        
        
        
        result_peg_962 = rule_s(stream, result_peg_962.getPosition());
            if (result_peg_962.error()){
                goto out_peg_964;
            }
        
        
        
        int save_peg_968 = result_peg_962.getPosition();
            
            result_peg_962 = rule_all_compare(stream, result_peg_962.getPosition());
            if (result_peg_962.error()){
                
                result_peg_962 = Result(save_peg_968);
                result_peg_962.setValue((void*) 0);
                
            }
        
        
        
        result_peg_962 = rule_s(stream, result_peg_962.getPosition());
            if (result_peg_962.error()){
                goto out_peg_964;
            }
        
        
        
        result_peg_962 = rule_value(stream, result_peg_962.getPosition());
            if (result_peg_962.error()){
                goto out_peg_964;
            }
        
        
    }
    
    if (column_peg_961.chunk10 == 0){
        column_peg_961.chunk10 = new Chunk10();
    }
    column_peg_961.chunk10->chunk_function_rest = result_peg_962;
    stream.update(result_peg_962.getPosition());
    
    
    return result_peg_962;
    out_peg_964:
    Result result_peg_970(myposition);
    
    
    
    if (column_peg_961.chunk10 == 0){
        column_peg_961.chunk10 = new Chunk10();
    }
    column_peg_961.chunk10->chunk_function_rest = result_peg_970;
    stream.update(result_peg_970.getPosition());
    
    
    return result_peg_970;
    
    if (column_peg_961.chunk10 == 0){
        column_peg_961.chunk10 = new Chunk10();
    }
    column_peg_961.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_972 = stream.getColumn(position);
    if (column_peg_972.chunk10 != 0 && column_peg_972.chunk10->chunk_keys.calculated()){
        return column_peg_972.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_975(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_973(myposition);
    
    result_peg_973 = rule_key_value_list(stream, result_peg_973.getPosition());
    if (result_peg_973.error()){
        goto out_peg_974;
    }
    
    if (column_peg_972.chunk10 == 0){
        column_peg_972.chunk10 = new Chunk10();
    }
    column_peg_972.chunk10->chunk_keys = result_peg_973;
    stream.update(result_peg_973.getPosition());
    
    
    return result_peg_973;
    out_peg_974:
    
    if (column_peg_972.chunk10 == 0){
        column_peg_972.chunk10 = new Chunk10();
    }
    column_peg_972.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_976 = stream.getColumn(position);
    if (column_peg_976.chunk10 != 0 && column_peg_976.chunk10->chunk_key_value_list.calculated()){
        return column_peg_976.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_986(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_977(myposition);
    
    {
    
        result_peg_977 = rule_key(stream, result_peg_977.getPosition());
            if (result_peg_977.error()){
                goto out_peg_979;
            }
            first = result_peg_977.getValues();
        
        
        
        result_peg_977.reset();
            do{
                Result result_peg_982(result_peg_977.getPosition());
                {
                
                    result_peg_982 = rule_s(stream, result_peg_982.getPosition());
                        if (result_peg_982.error()){
                            goto loop_peg_981;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_982.getPosition()))){
                                result_peg_982.nextPosition();
                            } else {
                                goto loop_peg_981;
                            }
                        }
                        result_peg_982.setValue((void*) ",");
                    
                    
                    
                    result_peg_982 = rule_s(stream, result_peg_982.getPosition());
                        if (result_peg_982.error()){
                            goto loop_peg_981;
                        }
                    
                    
                    
                    result_peg_982 = rule_key(stream, result_peg_982.getPosition());
                        if (result_peg_982.error()){
                            goto loop_peg_981;
                        }
                    
                    
                }
                result_peg_977.addResult(result_peg_982);
            } while (true);
            loop_peg_981:
            ;
            rest = result_peg_977.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_977.setValue(value);
            }
        
        
    }
    
    if (column_peg_976.chunk10 == 0){
        column_peg_976.chunk10 = new Chunk10();
    }
    column_peg_976.chunk10->chunk_key_value_list = result_peg_977;
    stream.update(result_peg_977.getPosition());
    
    
    return result_peg_977;
    out_peg_979:
    
    if (column_peg_976.chunk10 == 0){
        column_peg_976.chunk10 = new Chunk10();
    }
    column_peg_976.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_987 = stream.getColumn(position);
    if (column_peg_987.chunk10 != 0 && column_peg_987.chunk10->chunk_key.calculated()){
        return column_peg_987.chunk10->chunk_key;
    }
    
    RuleTrace trace_peg_995(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_988(myposition);
    
    {
    
        result_peg_988 = rule_key_real(stream, result_peg_988.getPosition());
            if (result_peg_988.error()){
                goto out_peg_990;
            }
        
        Result result_peg_989 = result_peg_988;
        
        result_peg_988 = rule_key_rest(stream, result_peg_988.getPosition(), result_peg_989.getValues());
            if (result_peg_988.error()){
                goto out_peg_990;
            }
            ok = result_peg_988.getValues();
        
        
        
        Result result_peg_994(result_peg_988);
            result_peg_994 = rule_identifier(stream, result_peg_994.getPosition());
            if (result_peg_994.error()){
                goto not_peg_993;
            }
            goto out_peg_990;
            not_peg_993:
            result_peg_988.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_988.setValue(value);
            }
        
        
    }
    
    if (column_peg_987.chunk10 == 0){
        column_peg_987.chunk10 = new Chunk10();
    }
    column_peg_987.chunk10->chunk_key = result_peg_988;
    stream.update(result_peg_988.getPosition());
    
    
    return result_peg_988;
    out_peg_990:
    
    if (column_peg_987.chunk10 == 0){
        column_peg_987.chunk10 = new Chunk10();
    }
    column_peg_987.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_1006(stream, "key_rest");
    int myposition = position;
    tail_peg_998:
    Value another;
    Value new_left;
    Result result_peg_997(myposition);
    {
    
        result_peg_997 = rule_s(stream, result_peg_997.getPosition());
            if (result_peg_997.error()){
                goto out_peg_1000;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_997.getPosition()))){
                    result_peg_997.nextPosition();
                } else {
                    goto out_peg_1000;
                }
            }
            result_peg_997.setValue((void*) "+");
        
        
        
        result_peg_997 = rule_key_real(stream, result_peg_997.getPosition());
            if (result_peg_997.error()){
                goto out_peg_1000;
            }
            another = result_peg_997.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_997.setValue(value);
            }
            new_left = result_peg_997.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_997.getPosition();
    goto tail_peg_998;
    out_peg_1000:
    Result result_peg_1004(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_1004.setValue(value);
            }
        
        
    }
    stream.update(result_peg_1004.getPosition());
    
    return result_peg_1004;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_1007 = stream.getColumn(position);
    if (column_peg_1007.chunk11 != 0 && column_peg_1007.chunk11->chunk_key_real.calculated()){
        return column_peg_1007.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_1014(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_1008(myposition);
    
    {
    
        result_peg_1008.reset();
            do{
                Result result_peg_1011(result_peg_1008.getPosition());
                result_peg_1011 = rule_key_modifier(stream, result_peg_1011.getPosition());
                if (result_peg_1011.error()){
                    goto loop_peg_1010;
                }
                result_peg_1008.addResult(result_peg_1011);
            } while (true);
            loop_peg_1010:
            ;
            mods = result_peg_1008.getValues();
        
        
        
        result_peg_1008 = rule_key_name(stream, result_peg_1008.getPosition());
            if (result_peg_1008.error()){
                goto out_peg_1013;
            }
            name = result_peg_1008.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_1008.setValue(value);
            }
        
        
    }
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_real = result_peg_1008;
    stream.update(result_peg_1008.getPosition());
    
    
    return result_peg_1008;
    out_peg_1013:
    
    if (column_peg_1007.chunk11 == 0){
        column_peg_1007.chunk11 = new Chunk11();
    }
    column_peg_1007.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_1015 = stream.getColumn(position);
    if (column_peg_1015.chunk11 != 0 && column_peg_1015.chunk11->chunk_key_modifier.calculated()){
        return column_peg_1015.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_1036(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_1016(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_1016.getPosition()))){
                    result_peg_1016.nextPosition();
                } else {
                    goto out_peg_1018;
                }
            }
            result_peg_1016.setValue((void*) "~");
        
        
        
        result_peg_1016.reset();
            do{
                Result result_peg_1021(result_peg_1016.getPosition());
                {
                    
                    char letter_peg_1026 = stream.get(result_peg_1021.getPosition());
                    if (letter_peg_1026 != '\0' && strchr("0123456789", letter_peg_1026) != NULL){
                        result_peg_1021.nextPosition();
                        result_peg_1021.setValue((void*) (long) letter_peg_1026);
                    } else {
                        goto out_peg_1025;
                    }
                    
                }
                goto success_peg_1022;
                out_peg_1025:
                goto loop_peg_1020;
                success_peg_1022:
                ;
                result_peg_1016.addResult(result_peg_1021);
            } while (true);
            loop_peg_1020:
            ;
            num = result_peg_1016.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_1016.setValue(value);
            }
        
        
    }
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_key_modifier = result_peg_1016;
    stream.update(result_peg_1016.getPosition());
    
    
    return result_peg_1016;
    out_peg_1018:
    Result result_peg_1027(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_1027.getPosition()))){
                    result_peg_1027.nextPosition();
                } else {
                    goto out_peg_1029;
                }
            }
            result_peg_1027.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_1027.setValue(value);
            }
        
        
    }
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_key_modifier = result_peg_1027;
    stream.update(result_peg_1027.getPosition());
    
    
    return result_peg_1027;
    out_peg_1029:
    Result result_peg_1030(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_1030.getPosition()))){
                    result_peg_1030.nextPosition();
                } else {
                    goto out_peg_1032;
                }
            }
            result_peg_1030.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_1030.setValue(value);
            }
        
        
    }
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_key_modifier = result_peg_1030;
    stream.update(result_peg_1030.getPosition());
    
    
    return result_peg_1030;
    out_peg_1032:
    Result result_peg_1033(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_1033.getPosition()))){
                    result_peg_1033.nextPosition();
                } else {
                    goto out_peg_1035;
                }
            }
            result_peg_1033.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_1033.setValue(value);
            }
        
        
    }
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_key_modifier = result_peg_1033;
    stream.update(result_peg_1033.getPosition());
    
    
    return result_peg_1033;
    out_peg_1035:
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_1037 = stream.getColumn(position);
    if (column_peg_1037.chunk11 != 0 && column_peg_1037.chunk11->chunk_key_name.calculated()){
        return column_peg_1037.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_1068(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_1038(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_1038.getPosition()))){
            result_peg_1038.nextPosition();
        } else {
            goto out_peg_1039;
        }
    }
    result_peg_1038.setValue((void*) "DB");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1038;
    stream.update(result_peg_1038.getPosition());
    
    
    return result_peg_1038;
    out_peg_1039:
    Result result_peg_1040(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_1040.getPosition()))){
            result_peg_1040.nextPosition();
        } else {
            goto out_peg_1041;
        }
    }
    result_peg_1040.setValue((void*) "B");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1040;
    stream.update(result_peg_1040.getPosition());
    
    
    return result_peg_1040;
    out_peg_1041:
    Result result_peg_1042(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_1042.getPosition()))){
            result_peg_1042.nextPosition();
        } else {
            goto out_peg_1043;
        }
    }
    result_peg_1042.setValue((void*) "DF");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1042;
    stream.update(result_peg_1042.getPosition());
    
    
    return result_peg_1042;
    out_peg_1043:
    Result result_peg_1044(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_1044.getPosition()))){
            result_peg_1044.nextPosition();
        } else {
            goto out_peg_1045;
        }
    }
    result_peg_1044.setValue((void*) "D");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1044;
    stream.update(result_peg_1044.getPosition());
    
    
    return result_peg_1044;
    out_peg_1045:
    Result result_peg_1046(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_1046.getPosition()))){
            result_peg_1046.nextPosition();
        } else {
            goto out_peg_1047;
        }
    }
    result_peg_1046.setValue((void*) "F");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1046;
    stream.update(result_peg_1046.getPosition());
    
    
    return result_peg_1046;
    out_peg_1047:
    Result result_peg_1048(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_1048.getPosition()))){
            result_peg_1048.nextPosition();
        } else {
            goto out_peg_1049;
        }
    }
    result_peg_1048.setValue((void*) "UF");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1048;
    stream.update(result_peg_1048.getPosition());
    
    
    return result_peg_1048;
    out_peg_1049:
    Result result_peg_1050(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_1050.getPosition()))){
            result_peg_1050.nextPosition();
        } else {
            goto out_peg_1051;
        }
    }
    result_peg_1050.setValue((void*) "UB");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1050;
    stream.update(result_peg_1050.getPosition());
    
    
    return result_peg_1050;
    out_peg_1051:
    Result result_peg_1052(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_1052.getPosition()))){
            result_peg_1052.nextPosition();
        } else {
            goto out_peg_1053;
        }
    }
    result_peg_1052.setValue((void*) "U");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1052;
    stream.update(result_peg_1052.getPosition());
    
    
    return result_peg_1052;
    out_peg_1053:
    Result result_peg_1054(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_1054.getPosition()))){
            result_peg_1054.nextPosition();
        } else {
            goto out_peg_1055;
        }
    }
    result_peg_1054.setValue((void*) "a");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1054;
    stream.update(result_peg_1054.getPosition());
    
    
    return result_peg_1054;
    out_peg_1055:
    Result result_peg_1056(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_1056.getPosition()))){
            result_peg_1056.nextPosition();
        } else {
            goto out_peg_1057;
        }
    }
    result_peg_1056.setValue((void*) "b");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1056;
    stream.update(result_peg_1056.getPosition());
    
    
    return result_peg_1056;
    out_peg_1057:
    Result result_peg_1058(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_1058.getPosition()))){
            result_peg_1058.nextPosition();
        } else {
            goto out_peg_1059;
        }
    }
    result_peg_1058.setValue((void*) "c");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1058;
    stream.update(result_peg_1058.getPosition());
    
    
    return result_peg_1058;
    out_peg_1059:
    Result result_peg_1060(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_1060.getPosition()))){
            result_peg_1060.nextPosition();
        } else {
            goto out_peg_1061;
        }
    }
    result_peg_1060.setValue((void*) "x");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1060;
    stream.update(result_peg_1060.getPosition());
    
    
    return result_peg_1060;
    out_peg_1061:
    Result result_peg_1062(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_1062.getPosition()))){
            result_peg_1062.nextPosition();
        } else {
            goto out_peg_1063;
        }
    }
    result_peg_1062.setValue((void*) "y");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1062;
    stream.update(result_peg_1062.getPosition());
    
    
    return result_peg_1062;
    out_peg_1063:
    Result result_peg_1064(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_1064.getPosition()))){
            result_peg_1064.nextPosition();
        } else {
            goto out_peg_1065;
        }
    }
    result_peg_1064.setValue((void*) "z");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1064;
    stream.update(result_peg_1064.getPosition());
    
    
    return result_peg_1064;
    out_peg_1065:
    Result result_peg_1066(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_1066.getPosition()))){
            result_peg_1066.nextPosition();
        } else {
            goto out_peg_1067;
        }
    }
    result_peg_1066.setValue((void*) "s");
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = result_peg_1066;
    stream.update(result_peg_1066.getPosition());
    
    
    return result_peg_1066;
    out_peg_1067:
    
    if (column_peg_1037.chunk11 == 0){
        column_peg_1037.chunk11 = new Chunk11();
    }
    column_peg_1037.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_1069 = stream.getColumn(position);
    if (column_peg_1069.chunk11 != 0 && column_peg_1069.chunk11->chunk_value.calculated()){
        return column_peg_1069.chunk11->chunk_value;
    }
    
    RuleTrace trace_peg_1087(stream, "value");
    int myposition = position;
    
    
    Result result_peg_1070(myposition);
    
    result_peg_1070 = rule_float(stream, result_peg_1070.getPosition());
    if (result_peg_1070.error()){
        goto out_peg_1071;
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1070;
    stream.update(result_peg_1070.getPosition());
    
    
    return result_peg_1070;
    out_peg_1071:
    Result result_peg_1072(myposition);
    
    result_peg_1072 = rule_integer(stream, result_peg_1072.getPosition());
    if (result_peg_1072.error()){
        goto out_peg_1073;
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1072;
    stream.update(result_peg_1072.getPosition());
    
    
    return result_peg_1072;
    out_peg_1073:
    Result result_peg_1074(myposition);
    
    result_peg_1074 = rule_keyword(stream, result_peg_1074.getPosition());
    if (result_peg_1074.error()){
        goto out_peg_1075;
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1074;
    stream.update(result_peg_1074.getPosition());
    
    
    return result_peg_1074;
    out_peg_1075:
    Result result_peg_1076(myposition);
    
    {
    
        Result result_peg_1079(result_peg_1076);
            result_peg_1079 = rule_keyword(stream, result_peg_1079.getPosition());
            if (result_peg_1079.error()){
                goto not_peg_1078;
            }
            goto out_peg_1080;
            not_peg_1078:
            result_peg_1076.setValue((void*)0);
        
        
        
        result_peg_1076 = rule_identifier(stream, result_peg_1076.getPosition());
            if (result_peg_1076.error()){
                goto out_peg_1080;
            }
        
        
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1076;
    stream.update(result_peg_1076.getPosition());
    
    
    return result_peg_1076;
    out_peg_1080:
    Result result_peg_1081(myposition);
    
    result_peg_1081 = rule_range(stream, result_peg_1081.getPosition());
    if (result_peg_1081.error()){
        goto out_peg_1082;
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1081;
    stream.update(result_peg_1081.getPosition());
    
    
    return result_peg_1081;
    out_peg_1082:
    Result result_peg_1083(myposition);
    
    result_peg_1083 = rule_string(stream, result_peg_1083.getPosition());
    if (result_peg_1083.error()){
        goto out_peg_1084;
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1083;
    stream.update(result_peg_1083.getPosition());
    
    
    return result_peg_1083;
    out_peg_1084:
    Result result_peg_1085(myposition);
    
    result_peg_1085 = rule_hitflag(stream, result_peg_1085.getPosition());
    if (result_peg_1085.error()){
        goto out_peg_1086;
    }
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = result_peg_1085;
    stream.update(result_peg_1085.getPosition());
    
    
    return result_peg_1085;
    out_peg_1086:
    
    if (column_peg_1069.chunk11 == 0){
        column_peg_1069.chunk11 = new Chunk11();
    }
    column_peg_1069.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_1088 = stream.getColumn(position);
    if (column_peg_1088.chunk11 != 0 && column_peg_1088.chunk11->chunk_hitflag.calculated()){
        return column_peg_1088.chunk11->chunk_hitflag;
    }
    
    RuleTrace trace_peg_1095(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_1089(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("M-"[i], stream.get(result_peg_1089.getPosition()))){
                    result_peg_1089.nextPosition();
                } else {
                    goto out_peg_1091;
                }
            }
            result_peg_1089.setValue((void*) "M-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("M-");
                result_peg_1089.setValue(value);
            }
        
        
    }
    
    if (column_peg_1088.chunk11 == 0){
        column_peg_1088.chunk11 = new Chunk11();
    }
    column_peg_1088.chunk11->chunk_hitflag = result_peg_1089;
    stream.update(result_peg_1089.getPosition());
    
    
    return result_peg_1089;
    out_peg_1091:
    Result result_peg_1092(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("A-"[i], stream.get(result_peg_1092.getPosition()))){
                    result_peg_1092.nextPosition();
                } else {
                    goto out_peg_1094;
                }
            }
            result_peg_1092.setValue((void*) "A-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("A-");
                result_peg_1092.setValue(value);
            }
        
        
    }
    
    if (column_peg_1088.chunk11 == 0){
        column_peg_1088.chunk11 = new Chunk11();
    }
    column_peg_1088.chunk11->chunk_hitflag = result_peg_1092;
    stream.update(result_peg_1092.getPosition());
    
    
    return result_peg_1092;
    out_peg_1094:
    
    if (column_peg_1088.chunk11 == 0){
        column_peg_1088.chunk11 = new Chunk11();
    }
    column_peg_1088.chunk11->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_1096 = stream.getColumn(position);
    if (column_peg_1096.chunk12 != 0 && column_peg_1096.chunk12->chunk_keyword.calculated()){
        return column_peg_1096.chunk12->chunk_keyword;
    }
    
    RuleTrace trace_peg_1103(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_1097(myposition);
    
    {
    
        result_peg_1097 = rule_keyword_real(stream, result_peg_1097.getPosition());
            if (result_peg_1097.error()){
                goto out_peg_1099;
            }
        
        Result result_peg_1098 = result_peg_1097;
        
        Result result_peg_1102(result_peg_1097);
            result_peg_1102 = rule_alpha_digit(stream, result_peg_1102.getPosition());
            if (result_peg_1102.error()){
                goto not_peg_1101;
            }
            goto out_peg_1099;
            not_peg_1101:
            result_peg_1097.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1098.getValues();
                result_peg_1097.setValue(value);
            }
        
        
    }
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword = result_peg_1097;
    stream.update(result_peg_1097.getPosition());
    
    
    return result_peg_1097;
    out_peg_1099:
    
    if (column_peg_1096.chunk12 == 0){
        column_peg_1096.chunk12 = new Chunk12();
    }
    column_peg_1096.chunk12->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_1104 = stream.getColumn(position);
    if (column_peg_1104.chunk12 != 0 && column_peg_1104.chunk12->chunk_keyword_real.calculated()){
        return column_peg_1104.chunk12->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_1182(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_1105(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1105.getPosition()))){
                    result_peg_1105.nextPosition();
                } else {
                    goto out_peg_1107;
                }
            }
            result_peg_1105.setValue((void*) "vel");
        
        
        
        result_peg_1105 = rule_s(stream, result_peg_1105.getPosition());
            if (result_peg_1105.error()){
                goto out_peg_1107;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1105.getPosition()))){
                    result_peg_1105.nextPosition();
                } else {
                    goto out_peg_1107;
                }
            }
            result_peg_1105.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_1105.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1105;
    stream.update(result_peg_1105.getPosition());
    
    
    return result_peg_1105;
    out_peg_1107:
    Result result_peg_1110(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1110.getPosition()))){
                    result_peg_1110.nextPosition();
                } else {
                    goto out_peg_1112;
                }
            }
            result_peg_1110.setValue((void*) "vel");
        
        
        
        result_peg_1110 = rule_s(stream, result_peg_1110.getPosition());
            if (result_peg_1110.error()){
                goto out_peg_1112;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1110.getPosition()))){
                    result_peg_1110.nextPosition();
                } else {
                    goto out_peg_1112;
                }
            }
            result_peg_1110.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_1110.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1110;
    stream.update(result_peg_1110.getPosition());
    
    
    return result_peg_1110;
    out_peg_1112:
    Result result_peg_1115(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1115.getPosition()))){
                    result_peg_1115.nextPosition();
                } else {
                    goto out_peg_1117;
                }
            }
            result_peg_1115.setValue((void*) "pos");
        
        
        
        result_peg_1115 = rule_s(stream, result_peg_1115.getPosition());
            if (result_peg_1115.error()){
                goto out_peg_1117;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1115.getPosition()))){
                    result_peg_1115.nextPosition();
                } else {
                    goto out_peg_1117;
                }
            }
            result_peg_1115.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_1115.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1115;
    stream.update(result_peg_1115.getPosition());
    
    
    return result_peg_1115;
    out_peg_1117:
    Result result_peg_1120(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1120.getPosition()))){
                    result_peg_1120.nextPosition();
                } else {
                    goto out_peg_1122;
                }
            }
            result_peg_1120.setValue((void*) "pos");
        
        
        
        result_peg_1120 = rule_s(stream, result_peg_1120.getPosition());
            if (result_peg_1120.error()){
                goto out_peg_1122;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1120.getPosition()))){
                    result_peg_1120.nextPosition();
                } else {
                    goto out_peg_1122;
                }
            }
            result_peg_1120.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_1120.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1120;
    stream.update(result_peg_1120.getPosition());
    
    
    return result_peg_1120;
    out_peg_1122:
    Result result_peg_1125(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1125.getPosition()))){
                    result_peg_1125.nextPosition();
                } else {
                    goto out_peg_1127;
                }
            }
            result_peg_1125.setValue((void*) "p2dist");
        
        
        
        result_peg_1125 = rule_s(stream, result_peg_1125.getPosition());
            if (result_peg_1125.error()){
                goto out_peg_1127;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1125.getPosition()))){
                    result_peg_1125.nextPosition();
                } else {
                    goto out_peg_1127;
                }
            }
            result_peg_1125.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_1125.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1125;
    stream.update(result_peg_1125.getPosition());
    
    
    return result_peg_1125;
    out_peg_1127:
    Result result_peg_1130(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1130.getPosition()))){
                    result_peg_1130.nextPosition();
                } else {
                    goto out_peg_1132;
                }
            }
            result_peg_1130.setValue((void*) "p2dist");
        
        
        
        result_peg_1130 = rule_s(stream, result_peg_1130.getPosition());
            if (result_peg_1130.error()){
                goto out_peg_1132;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1130.getPosition()))){
                    result_peg_1130.nextPosition();
                } else {
                    goto out_peg_1132;
                }
            }
            result_peg_1130.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_1130.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1130;
    stream.update(result_peg_1130.getPosition());
    
    
    return result_peg_1130;
    out_peg_1132:
    Result result_peg_1135(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1135.getPosition()))){
                    result_peg_1135.nextPosition();
                } else {
                    goto out_peg_1137;
                }
            }
            result_peg_1135.setValue((void*) "p1dist");
        
        
        
        result_peg_1135 = rule_s(stream, result_peg_1135.getPosition());
            if (result_peg_1135.error()){
                goto out_peg_1137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1135.getPosition()))){
                    result_peg_1135.nextPosition();
                } else {
                    goto out_peg_1137;
                }
            }
            result_peg_1135.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_1135.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1135;
    stream.update(result_peg_1135.getPosition());
    
    
    return result_peg_1135;
    out_peg_1137:
    Result result_peg_1140(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1140.getPosition()))){
                    result_peg_1140.nextPosition();
                } else {
                    goto out_peg_1142;
                }
            }
            result_peg_1140.setValue((void*) "p1dist");
        
        
        
        result_peg_1140 = rule_s(stream, result_peg_1140.getPosition());
            if (result_peg_1140.error()){
                goto out_peg_1142;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1140.getPosition()))){
                    result_peg_1140.nextPosition();
                } else {
                    goto out_peg_1142;
                }
            }
            result_peg_1140.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_1140.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1140;
    stream.update(result_peg_1140.getPosition());
    
    
    return result_peg_1140;
    out_peg_1142:
    Result result_peg_1145(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1145.getPosition()))){
                    result_peg_1145.nextPosition();
                } else {
                    goto out_peg_1147;
                }
            }
            result_peg_1145.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1145 = rule_s(stream, result_peg_1145.getPosition());
            if (result_peg_1145.error()){
                goto out_peg_1147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1145.getPosition()))){
                    result_peg_1145.nextPosition();
                } else {
                    goto out_peg_1147;
                }
            }
            result_peg_1145.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1145.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1145;
    stream.update(result_peg_1145.getPosition());
    
    
    return result_peg_1145;
    out_peg_1147:
    Result result_peg_1150(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1150.getPosition()))){
                    result_peg_1150.nextPosition();
                } else {
                    goto out_peg_1152;
                }
            }
            result_peg_1150.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1150 = rule_s(stream, result_peg_1150.getPosition());
            if (result_peg_1150.error()){
                goto out_peg_1152;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1150.getPosition()))){
                    result_peg_1150.nextPosition();
                } else {
                    goto out_peg_1152;
                }
            }
            result_peg_1150.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1150.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1150;
    stream.update(result_peg_1150.getPosition());
    
    
    return result_peg_1150;
    out_peg_1152:
    Result result_peg_1155(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1155.getPosition()))){
                    result_peg_1155.nextPosition();
                } else {
                    goto out_peg_1157;
                }
            }
            result_peg_1155.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1155 = rule_s(stream, result_peg_1155.getPosition());
            if (result_peg_1155.error()){
                goto out_peg_1157;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1155.getPosition()))){
                    result_peg_1155.nextPosition();
                } else {
                    goto out_peg_1157;
                }
            }
            result_peg_1155.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1155.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1155;
    stream.update(result_peg_1155.getPosition());
    
    
    return result_peg_1155;
    out_peg_1157:
    Result result_peg_1160(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1160.getPosition()))){
                    result_peg_1160.nextPosition();
                } else {
                    goto out_peg_1162;
                }
            }
            result_peg_1160.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1160 = rule_s(stream, result_peg_1160.getPosition());
            if (result_peg_1160.error()){
                goto out_peg_1162;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1160.getPosition()))){
                    result_peg_1160.nextPosition();
                } else {
                    goto out_peg_1162;
                }
            }
            result_peg_1160.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1160.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1160;
    stream.update(result_peg_1160.getPosition());
    
    
    return result_peg_1160;
    out_peg_1162:
    Result result_peg_1165(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1165.getPosition()))){
                    result_peg_1165.nextPosition();
                } else {
                    goto out_peg_1167;
                }
            }
            result_peg_1165.setValue((void*) "parentdist");
        
        
        
        result_peg_1165 = rule_s(stream, result_peg_1165.getPosition());
            if (result_peg_1165.error()){
                goto out_peg_1167;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1165.getPosition()))){
                    result_peg_1165.nextPosition();
                } else {
                    goto out_peg_1167;
                }
            }
            result_peg_1165.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1165.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1165;
    stream.update(result_peg_1165.getPosition());
    
    
    return result_peg_1165;
    out_peg_1167:
    Result result_peg_1170(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1170.getPosition()))){
                    result_peg_1170.nextPosition();
                } else {
                    goto out_peg_1172;
                }
            }
            result_peg_1170.setValue((void*) "screenpos");
        
        
        
        result_peg_1170 = rule_s(stream, result_peg_1170.getPosition());
            if (result_peg_1170.error()){
                goto out_peg_1172;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1170.getPosition()))){
                    result_peg_1170.nextPosition();
                } else {
                    goto out_peg_1172;
                }
            }
            result_peg_1170.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_1170.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1170;
    stream.update(result_peg_1170.getPosition());
    
    
    return result_peg_1170;
    out_peg_1172:
    Result result_peg_1175(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1175.getPosition()))){
                    result_peg_1175.nextPosition();
                } else {
                    goto out_peg_1177;
                }
            }
            result_peg_1175.setValue((void*) "screenpos");
        
        
        
        result_peg_1175 = rule_s(stream, result_peg_1175.getPosition());
            if (result_peg_1175.error()){
                goto out_peg_1177;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1175.getPosition()))){
                    result_peg_1175.nextPosition();
                } else {
                    goto out_peg_1177;
                }
            }
            result_peg_1175.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1175.setValue(value);
            }
        
        
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1175;
    stream.update(result_peg_1175.getPosition());
    
    
    return result_peg_1175;
    out_peg_1177:
    Result result_peg_1180(myposition);
    
    result_peg_1180 = rule_hitflag(stream, result_peg_1180.getPosition());
    if (result_peg_1180.error()){
        goto out_peg_1181;
    }
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = result_peg_1180;
    stream.update(result_peg_1180.getPosition());
    
    
    return result_peg_1180;
    out_peg_1181:
    
    if (column_peg_1104.chunk12 == 0){
        column_peg_1104.chunk12 = new Chunk12();
    }
    column_peg_1104.chunk12->chunk_keyword_real = errorResult;
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

        
