

#include "mugen/ast/all.h"
#include <map>
#include "gc.h"
typedef std::list<Ast::Section*> SectionList;


#include <list>
#include <string>
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
        int context = 10;
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
        }
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
    // std::map<const int, Column> memo;
    Column ** memo;
    int memo_size;
    // std::vector<Column> memo;
    int max;
    int farthest;
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
};

class ReleaseKeyModifier: public KeyModifier {
public:
    ReleaseKeyModifier(int ticks):
    ticks(ticks){
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
};

class HeldDownKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::MustBeHeldDown);
    }
};

class OnlyKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Only);
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
    
    int myposition = position;
    
    
    Result result_peg_22(myposition);
    
    {
    
        result_peg_22 = rule_s(stream, result_peg_22.getPosition());
            if (result_peg_22.error()){
                goto out_peg_24;
            }
        
        
        
        result_peg_22 = rule_comment(stream, result_peg_22.getPosition());
            if (result_peg_22.error()){
                goto out_peg_24;
            }
        
        
    }
    
    
    return result_peg_22;
    out_peg_24:
    Result result_peg_25(myposition);
    
    {
    
        result_peg_25 = rule_s(stream, result_peg_25.getPosition());
            if (result_peg_25.error()){
                goto out_peg_27;
            }
        
        
        
        result_peg_25 = rule_section(stream, result_peg_25.getPosition());
            if (result_peg_25.error()){
                goto out_peg_27;
            }
        
        Result result_peg_28 = result_peg_25;
        
        {
                Value value((void*) 0);
                addSection(current, result_peg_28.getValues());
                result_peg_25.setValue(value);
            }
        
        
    }
    
    
    return result_peg_25;
    out_peg_27:
    
    
    return errorResult;
}
        

Result rule_line_end(Stream & stream, const int position){
    
    Column & column_peg_29 = stream.getColumn(position);
    if (column_peg_29.chunk0 != 0 && column_peg_29.chunk0->chunk_line_end.calculated()){
        return column_peg_29.chunk0->chunk_line_end;
    }
    
    int myposition = position;
    
    
    Result result_peg_30(myposition);
    
    result_peg_30.reset();
    do{
        Result result_peg_32(result_peg_30.getPosition());
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\n"[i], stream.get(result_peg_32.getPosition()))){
                    result_peg_32.nextPosition();
                } else {
                    goto out_peg_35;
                }
            }
            result_peg_32.setValue((void*) "\n");
                
        }
        goto success_peg_33;
        out_peg_35:
        {
            
            for (int i = 0; i < 1; i++){
                if (compareChar("\r"[i], stream.get(result_peg_32.getPosition()))){
                    result_peg_32.nextPosition();
                } else {
                    goto out_peg_37;
                }
            }
            result_peg_32.setValue((void*) "\r");
                
        }
        goto success_peg_33;
        out_peg_37:
        goto loop_peg_31;
        success_peg_33:
        ;
        result_peg_30.addResult(result_peg_32);
    } while (true);
    loop_peg_31:
    if (result_peg_30.matches() == 0){
        goto out_peg_38;
    }
    
    if (column_peg_29.chunk0 == 0){
        column_peg_29.chunk0 = new Chunk0();
    }
    column_peg_29.chunk0->chunk_line_end = result_peg_30;
    stream.update(result_peg_30.getPosition());
    
    
    return result_peg_30;
    out_peg_38:
    Result result_peg_39(myposition);
    
    {
    
        Result result_peg_41(result_peg_39.getPosition());
            if ('\0' == stream.get(result_peg_41.getPosition())){
                result_peg_41.nextPosition();
                result_peg_41.setValue((void *) '\0');
            } else {
                goto out_peg_42;
            }
        
        
        
        
        
        
    }
    
    if (column_peg_29.chunk0 == 0){
        column_peg_29.chunk0 = new Chunk0();
    }
    column_peg_29.chunk0->chunk_line_end = result_peg_39;
    stream.update(result_peg_39.getPosition());
    
    
    return result_peg_39;
    out_peg_42:
    
    if (column_peg_29.chunk0 == 0){
        column_peg_29.chunk0 = new Chunk0();
    }
    column_peg_29.chunk0->chunk_line_end = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_whitespace(Stream & stream, const int position){
    
    Column & column_peg_43 = stream.getColumn(position);
    if (column_peg_43.chunk0 != 0 && column_peg_43.chunk0->chunk_whitespace.calculated()){
        return column_peg_43.chunk0->chunk_whitespace;
    }
    
    int myposition = position;
    
    
    Result result_peg_44(myposition);
    
    result_peg_44.reset();
    do{
        Result result_peg_46(result_peg_44.getPosition());
        result_peg_46 = rule_sw(stream, result_peg_46.getPosition());
        if (result_peg_46.error()){
            goto loop_peg_45;
        }
        result_peg_44.addResult(result_peg_46);
    } while (true);
    loop_peg_45:
    ;
    
    if (column_peg_43.chunk0 == 0){
        column_peg_43.chunk0 = new Chunk0();
    }
    column_peg_43.chunk0->chunk_whitespace = result_peg_44;
    stream.update(result_peg_44.getPosition());
    
    
    return result_peg_44;
    
    if (column_peg_43.chunk0 == 0){
        column_peg_43.chunk0 = new Chunk0();
    }
    column_peg_43.chunk0->chunk_whitespace = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_sw(Stream & stream, const int position){
    
    Column & column_peg_47 = stream.getColumn(position);
    if (column_peg_47.chunk0 != 0 && column_peg_47.chunk0->chunk_sw.calculated()){
        return column_peg_47.chunk0->chunk_sw;
    }
    
    int myposition = position;
    
    
    Result result_peg_48(myposition);
    
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar(" "[i], stream.get(result_peg_48.getPosition()))){
                result_peg_48.nextPosition();
            } else {
                goto out_peg_51;
            }
        }
        result_peg_48.setValue((void*) " ");
            
    }
    goto success_peg_49;
    out_peg_51:
    {
        
        for (int i = 0; i < 1; i++){
            if (compareChar("\t"[i], stream.get(result_peg_48.getPosition()))){
                result_peg_48.nextPosition();
            } else {
                goto out_peg_53;
            }
        }
        result_peg_48.setValue((void*) "\t");
            
    }
    goto success_peg_49;
    out_peg_53:
    goto out_peg_54;
    success_peg_49:
    ;
    
    if (column_peg_47.chunk0 == 0){
        column_peg_47.chunk0 = new Chunk0();
    }
    column_peg_47.chunk0->chunk_sw = result_peg_48;
    stream.update(result_peg_48.getPosition());
    
    
    return result_peg_48;
    out_peg_54:
    Result result_peg_55(myposition);
    
    result_peg_55 = rule_comment(stream, result_peg_55.getPosition());
    if (result_peg_55.error()){
        goto out_peg_56;
    }
    
    if (column_peg_47.chunk0 == 0){
        column_peg_47.chunk0 = new Chunk0();
    }
    column_peg_47.chunk0->chunk_sw = result_peg_55;
    stream.update(result_peg_55.getPosition());
    
    
    return result_peg_55;
    out_peg_56:
    
    if (column_peg_47.chunk0 == 0){
        column_peg_47.chunk0 = new Chunk0();
    }
    column_peg_47.chunk0->chunk_sw = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_s(Stream & stream, const int position){
    
    Column & column_peg_57 = stream.getColumn(position);
    if (column_peg_57.chunk1 != 0 && column_peg_57.chunk1->chunk_s.calculated()){
        return column_peg_57.chunk1->chunk_s;
    }
    
    int myposition = position;
    
    
    Result result_peg_58(myposition);
    
    result_peg_58.reset();
    do{
        Result result_peg_60(result_peg_58.getPosition());
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
        goto loop_peg_59;
        success_peg_61:
        ;
        result_peg_58.addResult(result_peg_60);
    } while (true);
    loop_peg_59:
    ;
    
    if (column_peg_57.chunk1 == 0){
        column_peg_57.chunk1 = new Chunk1();
    }
    column_peg_57.chunk1->chunk_s = result_peg_58;
    stream.update(result_peg_58.getPosition());
    
    
    return result_peg_58;
    
    if (column_peg_57.chunk1 == 0){
        column_peg_57.chunk1 = new Chunk1();
    }
    column_peg_57.chunk1->chunk_s = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_comment(Stream & stream, const int position){
    
    Column & column_peg_66 = stream.getColumn(position);
    if (column_peg_66.chunk1 != 0 && column_peg_66.chunk1->chunk_comment.calculated()){
        return column_peg_66.chunk1->chunk_comment;
    }
    
    int myposition = position;
    
    
    Result result_peg_67(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(";"[i], stream.get(result_peg_67.getPosition()))){
                    result_peg_67.nextPosition();
                } else {
                    goto out_peg_69;
                }
            }
            result_peg_67.setValue((void*) ";");
        
        
        
        result_peg_67.reset();
            do{
                Result result_peg_71(result_peg_67.getPosition());
                {
                
                    Result result_peg_74(result_peg_71);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_74.getPosition()))){
                                result_peg_74.nextPosition();
                            } else {
                                goto not_peg_73;
                            }
                        }
                        result_peg_74.setValue((void*) "\n");
                        goto loop_peg_70;
                        not_peg_73:
                        result_peg_71.setValue((void*)0);
                    
                    
                    
                    char temp_peg_75 = stream.get(result_peg_71.getPosition());
                        if (temp_peg_75 != '\0'){
                            result_peg_71.setValue((void*) temp_peg_75);
                            result_peg_71.nextPosition();
                        } else {
                            goto loop_peg_70;
                        }
                    
                    
                }
                result_peg_67.addResult(result_peg_71);
            } while (true);
            loop_peg_70:
            ;
        
        
    }
    
    if (column_peg_66.chunk1 == 0){
        column_peg_66.chunk1 = new Chunk1();
    }
    column_peg_66.chunk1->chunk_comment = result_peg_67;
    stream.update(result_peg_67.getPosition());
    
    
    return result_peg_67;
    out_peg_69:
    Result result_peg_76(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_76.getPosition()))){
                    result_peg_76.nextPosition();
                } else {
                    goto out_peg_78;
                }
            }
            result_peg_76.setValue((void*) "=");
        
        
        
        result_peg_76.reset();
            do{
                Result result_peg_80(result_peg_76.getPosition());
                {
                
                    Result result_peg_83(result_peg_80);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_83.getPosition()))){
                                result_peg_83.nextPosition();
                            } else {
                                goto not_peg_82;
                            }
                        }
                        result_peg_83.setValue((void*) "\n");
                        goto loop_peg_79;
                        not_peg_82:
                        result_peg_80.setValue((void*)0);
                    
                    
                    
                    char temp_peg_84 = stream.get(result_peg_80.getPosition());
                        if (temp_peg_84 != '\0'){
                            result_peg_80.setValue((void*) temp_peg_84);
                            result_peg_80.nextPosition();
                        } else {
                            goto loop_peg_79;
                        }
                    
                    
                }
                result_peg_76.addResult(result_peg_80);
            } while (true);
            loop_peg_79:
            ;
        
        
    }
    
    if (column_peg_66.chunk1 == 0){
        column_peg_66.chunk1 = new Chunk1();
    }
    column_peg_66.chunk1->chunk_comment = result_peg_76;
    stream.update(result_peg_76.getPosition());
    
    
    return result_peg_76;
    out_peg_78:
    Result result_peg_85(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_85.getPosition()))){
                    result_peg_85.nextPosition();
                } else {
                    goto out_peg_87;
                }
            }
            result_peg_85.setValue((void*) "-");
        
        
        
        result_peg_85.reset();
            do{
                Result result_peg_89(result_peg_85.getPosition());
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_89.getPosition()))){
                        result_peg_89.nextPosition();
                    } else {
                        goto loop_peg_88;
                    }
                }
                result_peg_89.setValue((void*) "-");
                result_peg_85.addResult(result_peg_89);
            } while (true);
            loop_peg_88:
            if (result_peg_85.matches() == 0){
                goto out_peg_87;
            }
        
        
    }
    
    if (column_peg_66.chunk1 == 0){
        column_peg_66.chunk1 = new Chunk1();
    }
    column_peg_66.chunk1->chunk_comment = result_peg_85;
    stream.update(result_peg_85.getPosition());
    
    
    return result_peg_85;
    out_peg_87:
    
    if (column_peg_66.chunk1 == 0){
        column_peg_66.chunk1 = new Chunk1();
    }
    column_peg_66.chunk1->chunk_comment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section(Stream & stream, const int position){
    
    Column & column_peg_90 = stream.getColumn(position);
    if (column_peg_90.chunk1 != 0 && column_peg_90.chunk1->chunk_section.calculated()){
        return column_peg_90.chunk1->chunk_section;
    }
    
    int myposition = position;
    
    Value ast;
    Result result_peg_91(myposition);
    
    {
    
        result_peg_91 = rule_section_title(stream, result_peg_91.getPosition());
            if (result_peg_91.error()){
                goto out_peg_93;
            }
        
        Result result_peg_92 = result_peg_91;
        
        {
                Value value((void*) 0);
                value = makeSection(result_peg_92.getValues());
                result_peg_91.setValue(value);
            }
            ast = result_peg_91.getValues();
        
        
        
        result_peg_91 = rule_whitespace(stream, result_peg_91.getPosition());
            if (result_peg_91.error()){
                goto out_peg_93;
            }
        
        
        
        result_peg_91.reset();
            do{
                Result result_peg_98(result_peg_91.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\n"[i], stream.get(result_peg_98.getPosition()))){
                            result_peg_98.nextPosition();
                        } else {
                            goto out_peg_101;
                        }
                    }
                    result_peg_98.setValue((void*) "\n");
                        
                }
                goto success_peg_99;
                out_peg_101:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("\r"[i], stream.get(result_peg_98.getPosition()))){
                            result_peg_98.nextPosition();
                        } else {
                            goto out_peg_103;
                        }
                    }
                    result_peg_98.setValue((void*) "\r");
                        
                }
                goto success_peg_99;
                out_peg_103:
                goto loop_peg_97;
                success_peg_99:
                ;
                result_peg_91.addResult(result_peg_98);
            } while (true);
            loop_peg_97:
            if (result_peg_91.matches() == 0){
                goto out_peg_93;
            }
        
        
        
        result_peg_91.reset();
            do{
                Result result_peg_106(result_peg_91.getPosition());
                {
                
                    result_peg_106 = rule_section_item(stream, result_peg_106.getPosition(), ast);
                        if (result_peg_106.error()){
                            goto loop_peg_105;
                        }
                    
                    
                    
                    result_peg_106 = rule_whitespace(stream, result_peg_106.getPosition());
                        if (result_peg_106.error()){
                            goto loop_peg_105;
                        }
                    
                    
                    
                    result_peg_106 = rule_line_end(stream, result_peg_106.getPosition());
                        if (result_peg_106.error()){
                            goto loop_peg_105;
                        }
                    
                    
                }
                result_peg_91.addResult(result_peg_106);
            } while (true);
            loop_peg_105:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = ast;
                result_peg_91.setValue(value);
            }
        
        
    }
    
    if (column_peg_90.chunk1 == 0){
        column_peg_90.chunk1 = new Chunk1();
    }
    column_peg_90.chunk1->chunk_section = result_peg_91;
    stream.update(result_peg_91.getPosition());
    
    
    return result_peg_91;
    out_peg_93:
    
    if (column_peg_90.chunk1 == 0){
        column_peg_90.chunk1 = new Chunk1();
    }
    column_peg_90.chunk1->chunk_section = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_section_item(Stream & stream, const int position, Value ast){
    
    int myposition = position;
    
    Value data;
    Result result_peg_110(myposition);
    
    {
    
        result_peg_110 = rule_s(stream, result_peg_110.getPosition());
            if (result_peg_110.error()){
                goto out_peg_112;
            }
        
        
        
        result_peg_110 = rule_comment(stream, result_peg_110.getPosition());
            if (result_peg_110.error()){
                goto out_peg_112;
            }
        
        
    }
    
    
    return result_peg_110;
    out_peg_112:
    Result result_peg_113(myposition);
    
    {
    
        result_peg_113 = rule_s(stream, result_peg_113.getPosition());
            if (result_peg_113.error()){
                goto out_peg_115;
            }
        
        
        
        result_peg_113 = rule_assignment(stream, result_peg_113.getPosition());
            if (result_peg_113.error()){
                goto out_peg_115;
            }
            data = result_peg_113.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_113.setValue(value);
            }
        
        
    }
    
    
    return result_peg_113;
    out_peg_115:
    
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_117 = stream.getColumn(position);
    if (column_peg_117.chunk1 != 0 && column_peg_117.chunk1->chunk_section_title.calculated()){
        return column_peg_117.chunk1->chunk_section_title;
    }
    
    int myposition = position;
    
    Value name;
    Result result_peg_118(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "[");
        
        
        
        result_peg_118 = rule_s(stream, result_peg_118.getPosition());
            if (result_peg_118.error()){
                goto out_peg_120;
            }
        
        
        
        result_peg_118.reset();
            do{
                Result result_peg_124(result_peg_118.getPosition());
                {
                
                    Result result_peg_127(result_peg_124);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_127.getPosition()))){
                                result_peg_127.nextPosition();
                            } else {
                                goto not_peg_126;
                            }
                        }
                        result_peg_127.setValue((void*) "]");
                        goto loop_peg_123;
                        not_peg_126:
                        result_peg_124.setValue((void*)0);
                    
                    
                    
                    char temp_peg_128 = stream.get(result_peg_124.getPosition());
                        if (temp_peg_128 != '\0'){
                            result_peg_124.setValue((void*) temp_peg_128);
                            result_peg_124.nextPosition();
                        } else {
                            goto loop_peg_123;
                        }
                    
                    
                }
                result_peg_118.addResult(result_peg_124);
            } while (true);
            loop_peg_123:
            if (result_peg_118.matches() == 0){
                goto out_peg_120;
            }
            name = result_peg_118.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_118.getPosition()))){
                    result_peg_118.nextPosition();
                } else {
                    goto out_peg_120;
                }
            }
            result_peg_118.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(name);
                result_peg_118.setValue(value);
            }
        
        
    }
    
    if (column_peg_117.chunk1 == 0){
        column_peg_117.chunk1 = new Chunk1();
    }
    column_peg_117.chunk1->chunk_section_title = result_peg_118;
    stream.update(result_peg_118.getPosition());
    
    
    return result_peg_118;
    out_peg_120:
    
    if (column_peg_117.chunk1 == 0){
        column_peg_117.chunk1 = new Chunk1();
    }
    column_peg_117.chunk1->chunk_section_title = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_130 = stream.getColumn(position);
    if (column_peg_130.chunk2 != 0 && column_peg_130.chunk2->chunk_assignment.calculated()){
        return column_peg_130.chunk2->chunk_assignment;
    }
    
    int myposition = position;
    
    Value all;
    Value name;
    Value exp;
    Value index;
    Result result_peg_131(myposition);
    
    {
    
        for (int i = 0; i < 7; i++){
                if (compareCharCase("command"[i], stream.get(result_peg_131.getPosition()))){
                    result_peg_131.nextPosition();
                } else {
                    goto out_peg_133;
                }
            }
            result_peg_131.setValue((void*) "command");
        
        
        
        result_peg_131 = rule_s(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_131.getPosition()))){
                    result_peg_131.nextPosition();
                } else {
                    goto out_peg_133;
                }
            }
            result_peg_131.setValue((void*) "=");
        
        
        
        result_peg_131 = rule_s(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
        
        
        
        result_peg_131 = rule_keys(stream, result_peg_131.getPosition());
            if (result_peg_131.error()){
                goto out_peg_133;
            }
            all = result_peg_131.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("command", all);
                result_peg_131.setValue(value);
            }
        
        
    }
    
    if (column_peg_130.chunk2 == 0){
        column_peg_130.chunk2 = new Chunk2();
    }
    column_peg_130.chunk2->chunk_assignment = result_peg_131;
    stream.update(result_peg_131.getPosition());
    
    
    return result_peg_131;
    out_peg_133:
    Result result_peg_138(myposition);
    
    {
    
        result_peg_138 = rule_identifier(stream, result_peg_138.getPosition());
            if (result_peg_138.error()){
                goto out_peg_140;
            }
            name = result_peg_138.getValues();
        
        
        
        result_peg_138 = rule_s(stream, result_peg_138.getPosition());
            if (result_peg_138.error()){
                goto out_peg_140;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_138.getPosition()))){
                    result_peg_138.nextPosition();
                } else {
                    goto out_peg_140;
                }
            }
            result_peg_138.setValue((void*) "=");
        
        
        
        result_peg_138 = rule_s(stream, result_peg_138.getPosition());
            if (result_peg_138.error()){
                goto out_peg_140;
            }
        
        
        
        result_peg_138 = rule_expr(stream, result_peg_138.getPosition());
            if (result_peg_138.error()){
                goto out_peg_140;
            }
            exp = result_peg_138.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
                result_peg_138.setValue(value);
            }
        
        
    }
    
    if (column_peg_130.chunk2 == 0){
        column_peg_130.chunk2 = new Chunk2();
    }
    column_peg_130.chunk2->chunk_assignment = result_peg_138;
    stream.update(result_peg_138.getPosition());
    
    
    return result_peg_138;
    out_peg_140:
    Result result_peg_145(myposition);
    
    {
    
        result_peg_145 = rule_identifier(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
            name = result_peg_145.getValues();
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "=");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        Result result_peg_152(result_peg_145.getPosition());
            result_peg_152 = rule_line_end(stream, result_peg_152.getPosition());
            if (result_peg_152.error()){
                goto out_peg_147;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_145.setValue(value);
            }
        
        
    }
    
    if (column_peg_130.chunk2 == 0){
        column_peg_130.chunk2 = new Chunk2();
    }
    column_peg_130.chunk2->chunk_assignment = result_peg_145;
    stream.update(result_peg_145.getPosition());
    
    
    return result_peg_145;
    out_peg_147:
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
                if (compareChar("("[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) "(");
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        result_peg_153 = rule_integer(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
            index = result_peg_153.getValues();
        
        
        
        result_peg_153 = rule_s(stream, result_peg_153.getPosition());
            if (result_peg_153.error()){
                goto out_peg_155;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_153.getPosition()))){
                    result_peg_153.nextPosition();
                } else {
                    goto out_peg_155;
                }
            }
            result_peg_153.setValue((void*) ")");
        
        
        
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
                value = makeIndexedAttribute(name, index, exp);
                result_peg_153.setValue(value);
            }
        
        
    }
    
    if (column_peg_130.chunk2 == 0){
        column_peg_130.chunk2 = new Chunk2();
    }
    column_peg_130.chunk2->chunk_assignment = result_peg_153;
    stream.update(result_peg_153.getPosition());
    
    
    return result_peg_153;
    out_peg_155:
    
    if (column_peg_130.chunk2 == 0){
        column_peg_130.chunk2 = new Chunk2();
    }
    column_peg_130.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_166 = stream.getColumn(position);
    if (column_peg_166.chunk2 != 0 && column_peg_166.chunk2->chunk_identifier.calculated()){
        return column_peg_166.chunk2->chunk_identifier;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_167(myposition);
    
    {
    
        result_peg_167 = rule_name(stream, result_peg_167.getPosition());
            if (result_peg_167.error()){
                goto out_peg_169;
            }
            first = result_peg_167.getValues();
        
        
        
        result_peg_167.reset();
            do{
                Result result_peg_172(result_peg_167.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_172.getPosition()))){
                                result_peg_172.nextPosition();
                            } else {
                                goto loop_peg_171;
                            }
                        }
                        result_peg_172.setValue((void*) ".");
                    
                    
                    
                    result_peg_172 = rule_name(stream, result_peg_172.getPosition());
                        if (result_peg_172.error()){
                            goto loop_peg_171;
                        }
                    
                    
                }
                result_peg_167.addResult(result_peg_172);
            } while (true);
            loop_peg_171:
            ;
            rest = result_peg_167.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_167.setValue(value);
            }
        
        
    }
    
    if (column_peg_166.chunk2 == 0){
        column_peg_166.chunk2 = new Chunk2();
    }
    column_peg_166.chunk2->chunk_identifier = result_peg_167;
    stream.update(result_peg_167.getPosition());
    
    
    return result_peg_167;
    out_peg_169:
    
    if (column_peg_166.chunk2 == 0){
        column_peg_166.chunk2 = new Chunk2();
    }
    column_peg_166.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_174 = stream.getColumn(position);
    if (column_peg_174.chunk2 != 0 && column_peg_174.chunk2->chunk_integer.calculated()){
        return column_peg_174.chunk2->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_175(myposition);
    
    {
    
        int save_peg_177 = result_peg_175.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_175.getPosition()))){
                        result_peg_175.nextPosition();
                    } else {
                        goto out_peg_180;
                    }
                }
                result_peg_175.setValue((void*) "-");
                    
            }
            goto success_peg_178;
            out_peg_180:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_175.getPosition()))){
                        result_peg_175.nextPosition();
                    } else {
                        goto out_peg_182;
                    }
                }
                result_peg_175.setValue((void*) "+");
                    
            }
            goto success_peg_178;
            out_peg_182:
            
            result_peg_175 = Result(save_peg_177);
            result_peg_175.setValue((void*) 0);
            
            success_peg_178:
            ;
        
        Result result_peg_176 = result_peg_175;
        
        result_peg_175.reset();
            do{
                Result result_peg_185(result_peg_175.getPosition());
                {
                    
                    char letter_peg_190 = stream.get(result_peg_185.getPosition());
                    if (letter_peg_190 != '\0' && strchr("0123456789", letter_peg_190) != NULL){
                        result_peg_185.nextPosition();
                        result_peg_185.setValue((void*) letter_peg_190);
                    } else {
                        goto out_peg_189;
                    }
                    
                }
                goto success_peg_186;
                out_peg_189:
                goto loop_peg_184;
                success_peg_186:
                ;
                result_peg_175.addResult(result_peg_185);
            } while (true);
            loop_peg_184:
            if (result_peg_175.matches() == 0){
                goto out_peg_191;
            }
        
        Result result_peg_183 = result_peg_175;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_176.getValues(), result_peg_183.getValues());
                result_peg_175.setValue(value);
            }
        
        
    }
    
    if (column_peg_174.chunk2 == 0){
        column_peg_174.chunk2 = new Chunk2();
    }
    column_peg_174.chunk2->chunk_integer = result_peg_175;
    stream.update(result_peg_175.getPosition());
    
    
    return result_peg_175;
    out_peg_191:
    
    if (column_peg_174.chunk2 == 0){
        column_peg_174.chunk2 = new Chunk2();
    }
    column_peg_174.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_192 = stream.getColumn(position);
    if (column_peg_192.chunk2 != 0 && column_peg_192.chunk2->chunk_float.calculated()){
        return column_peg_192.chunk2->chunk_float;
    }
    
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_193(myposition);
    
    {
    
        int save_peg_195 = result_peg_193.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_193.getPosition()))){
                        result_peg_193.nextPosition();
                    } else {
                        goto out_peg_198;
                    }
                }
                result_peg_193.setValue((void*) "-");
                    
            }
            goto success_peg_196;
            out_peg_198:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_193.getPosition()))){
                        result_peg_193.nextPosition();
                    } else {
                        goto out_peg_200;
                    }
                }
                result_peg_193.setValue((void*) "+");
                    
            }
            goto success_peg_196;
            out_peg_200:
            
            result_peg_193 = Result(save_peg_195);
            result_peg_193.setValue((void*) 0);
            
            success_peg_196:
            ;
        
        Result result_peg_194 = result_peg_193;
        
        result_peg_193.reset();
            do{
                Result result_peg_203(result_peg_193.getPosition());
                {
                    
                    char letter_peg_208 = stream.get(result_peg_203.getPosition());
                    if (letter_peg_208 != '\0' && strchr("0123456789", letter_peg_208) != NULL){
                        result_peg_203.nextPosition();
                        result_peg_203.setValue((void*) letter_peg_208);
                    } else {
                        goto out_peg_207;
                    }
                    
                }
                goto success_peg_204;
                out_peg_207:
                goto loop_peg_202;
                success_peg_204:
                ;
                result_peg_193.addResult(result_peg_203);
            } while (true);
            loop_peg_202:
            ;
            left = result_peg_193.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_193.getPosition()))){
                    result_peg_193.nextPosition();
                } else {
                    goto out_peg_210;
                }
            }
            result_peg_193.setValue((void*) ".");
        
        
        
        result_peg_193.reset();
            do{
                Result result_peg_213(result_peg_193.getPosition());
                {
                    
                    char letter_peg_218 = stream.get(result_peg_213.getPosition());
                    if (letter_peg_218 != '\0' && strchr("0123456789", letter_peg_218) != NULL){
                        result_peg_213.nextPosition();
                        result_peg_213.setValue((void*) letter_peg_218);
                    } else {
                        goto out_peg_217;
                    }
                    
                }
                goto success_peg_214;
                out_peg_217:
                goto loop_peg_212;
                success_peg_214:
                ;
                result_peg_193.addResult(result_peg_213);
            } while (true);
            loop_peg_212:
            if (result_peg_193.matches() == 0){
                goto out_peg_210;
            }
            right = result_peg_193.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_194.getValues(), parseDouble(left,right));
                result_peg_193.setValue(value);
            }
        
        
    }
    
    if (column_peg_192.chunk2 == 0){
        column_peg_192.chunk2 = new Chunk2();
    }
    column_peg_192.chunk2->chunk_float = result_peg_193;
    stream.update(result_peg_193.getPosition());
    
    
    return result_peg_193;
    out_peg_210:
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
                        result_peg_229.setValue((void*) letter_peg_234);
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
            if (result_peg_219.matches() == 0){
                goto out_peg_235;
            }
            left = result_peg_219.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_219.getPosition()))){
                    result_peg_219.nextPosition();
                } else {
                    goto out_peg_235;
                }
            }
            result_peg_219.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_220.getValues(), parseDouble(left));
                result_peg_219.setValue(value);
            }
        
        
    }
    
    if (column_peg_192.chunk2 == 0){
        column_peg_192.chunk2 = new Chunk2();
    }
    column_peg_192.chunk2->chunk_float = result_peg_219;
    stream.update(result_peg_219.getPosition());
    
    
    return result_peg_219;
    out_peg_235:
    
    if (column_peg_192.chunk2 == 0){
        column_peg_192.chunk2 = new Chunk2();
    }
    column_peg_192.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_237 = stream.getColumn(position);
    if (column_peg_237.chunk2 != 0 && column_peg_237.chunk2->chunk_string.calculated()){
        return column_peg_237.chunk2->chunk_string;
    }
    
    int myposition = position;
    
    Value contents;
    Result result_peg_238(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_238.getPosition()))){
                    result_peg_238.nextPosition();
                } else {
                    goto out_peg_240;
                }
            }
            result_peg_238.setValue((void*) "\"");
        
        
        
        result_peg_238.reset();
            do{
                Result result_peg_243(result_peg_238.getPosition());
                {
                
                    Result result_peg_246(result_peg_243);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_246.getPosition()))){
                                result_peg_246.nextPosition();
                            } else {
                                goto not_peg_245;
                            }
                        }
                        result_peg_246.setValue((void*) "\"");
                        goto loop_peg_242;
                        not_peg_245:
                        result_peg_243.setValue((void*)0);
                    
                    
                    
                    Result result_peg_249(result_peg_243);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_249.getPosition()))){
                                result_peg_249.nextPosition();
                            } else {
                                goto not_peg_248;
                            }
                        }
                        result_peg_249.setValue((void*) "\n");
                        goto loop_peg_242;
                        not_peg_248:
                        result_peg_243.setValue((void*)0);
                    
                    
                    
                    char temp_peg_250 = stream.get(result_peg_243.getPosition());
                        if (temp_peg_250 != '\0'){
                            result_peg_243.setValue((void*) temp_peg_250);
                            result_peg_243.nextPosition();
                        } else {
                            goto loop_peg_242;
                        }
                    
                    
                }
                result_peg_238.addResult(result_peg_243);
            } while (true);
            loop_peg_242:
            ;
            contents = result_peg_238.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_238.getPosition()))){
                    result_peg_238.nextPosition();
                } else {
                    goto out_peg_240;
                }
            }
            result_peg_238.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_238.setValue(value);
            }
        
        
    }
    
    if (column_peg_237.chunk2 == 0){
        column_peg_237.chunk2 = new Chunk2();
    }
    column_peg_237.chunk2->chunk_string = result_peg_238;
    stream.update(result_peg_238.getPosition());
    
    
    return result_peg_238;
    out_peg_240:
    
    if (column_peg_237.chunk2 == 0){
        column_peg_237.chunk2 = new Chunk2();
    }
    column_peg_237.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_252 = stream.getColumn(position);
    if (column_peg_252.chunk3 != 0 && column_peg_252.chunk3->chunk_range.calculated()){
        return column_peg_252.chunk3->chunk_range;
    }
    
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_253(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) "[");
        
        
        
        result_peg_253 = rule_s(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
        
        
        
        result_peg_253 = rule_value(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
            low = result_peg_253.getValues();
        
        
        
        result_peg_253 = rule_s(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) ",");
        
        
        
        result_peg_253 = rule_s(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
        
        
        
        result_peg_253 = rule_value(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
            high = result_peg_253.getValues();
        
        
        
        result_peg_253 = rule_s(stream, result_peg_253.getPosition());
            if (result_peg_253.error()){
                goto out_peg_255;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_253.setValue(value);
            }
        
        
    }
    
    if (column_peg_252.chunk3 == 0){
        column_peg_252.chunk3 = new Chunk3();
    }
    column_peg_252.chunk3->chunk_range = result_peg_253;
    stream.update(result_peg_253.getPosition());
    
    
    return result_peg_253;
    out_peg_255:
    Result result_peg_264(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_264.getPosition()))){
                    result_peg_264.nextPosition();
                } else {
                    goto out_peg_266;
                }
            }
            result_peg_264.setValue((void*) "(");
        
        
        
        result_peg_264 = rule_s(stream, result_peg_264.getPosition());
            if (result_peg_264.error()){
                goto out_peg_266;
            }
        
        
        
        result_peg_264 = rule_value(stream, result_peg_264.getPosition());
            if (result_peg_264.error()){
                goto out_peg_266;
            }
            low = result_peg_264.getValues();
        
        
        
        result_peg_264 = rule_s(stream, result_peg_264.getPosition());
            if (result_peg_264.error()){
                goto out_peg_266;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_264.getPosition()))){
                    result_peg_264.nextPosition();
                } else {
                    goto out_peg_266;
                }
            }
            result_peg_264.setValue((void*) ",");
        
        
        
        result_peg_264 = rule_s(stream, result_peg_264.getPosition());
            if (result_peg_264.error()){
                goto out_peg_266;
            }
        
        
        
        result_peg_264 = rule_value(stream, result_peg_264.getPosition());
            if (result_peg_264.error()){
                goto out_peg_266;
            }
            high = result_peg_264.getValues();
        
        
        
        result_peg_264 = rule_s(stream, result_peg_264.getPosition());
            if (result_peg_264.error()){
                goto out_peg_266;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_264.getPosition()))){
                    result_peg_264.nextPosition();
                } else {
                    goto out_peg_266;
                }
            }
            result_peg_264.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_264.setValue(value);
            }
        
        
    }
    
    if (column_peg_252.chunk3 == 0){
        column_peg_252.chunk3 = new Chunk3();
    }
    column_peg_252.chunk3->chunk_range = result_peg_264;
    stream.update(result_peg_264.getPosition());
    
    
    return result_peg_264;
    out_peg_266:
    Result result_peg_275(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_275.getPosition()))){
                    result_peg_275.nextPosition();
                } else {
                    goto out_peg_277;
                }
            }
            result_peg_275.setValue((void*) "(");
        
        
        
        result_peg_275 = rule_s(stream, result_peg_275.getPosition());
            if (result_peg_275.error()){
                goto out_peg_277;
            }
        
        
        
        result_peg_275 = rule_value(stream, result_peg_275.getPosition());
            if (result_peg_275.error()){
                goto out_peg_277;
            }
            low = result_peg_275.getValues();
        
        
        
        result_peg_275 = rule_s(stream, result_peg_275.getPosition());
            if (result_peg_275.error()){
                goto out_peg_277;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_275.getPosition()))){
                    result_peg_275.nextPosition();
                } else {
                    goto out_peg_277;
                }
            }
            result_peg_275.setValue((void*) ",");
        
        
        
        result_peg_275 = rule_s(stream, result_peg_275.getPosition());
            if (result_peg_275.error()){
                goto out_peg_277;
            }
        
        
        
        result_peg_275 = rule_value(stream, result_peg_275.getPosition());
            if (result_peg_275.error()){
                goto out_peg_277;
            }
            high = result_peg_275.getValues();
        
        
        
        result_peg_275 = rule_s(stream, result_peg_275.getPosition());
            if (result_peg_275.error()){
                goto out_peg_277;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_275.getPosition()))){
                    result_peg_275.nextPosition();
                } else {
                    goto out_peg_277;
                }
            }
            result_peg_275.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_275.setValue(value);
            }
        
        
    }
    
    if (column_peg_252.chunk3 == 0){
        column_peg_252.chunk3 = new Chunk3();
    }
    column_peg_252.chunk3->chunk_range = result_peg_275;
    stream.update(result_peg_275.getPosition());
    
    
    return result_peg_275;
    out_peg_277:
    Result result_peg_286(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_286.getPosition()))){
                    result_peg_286.nextPosition();
                } else {
                    goto out_peg_288;
                }
            }
            result_peg_286.setValue((void*) "[");
        
        
        
        result_peg_286 = rule_s(stream, result_peg_286.getPosition());
            if (result_peg_286.error()){
                goto out_peg_288;
            }
        
        
        
        result_peg_286 = rule_value(stream, result_peg_286.getPosition());
            if (result_peg_286.error()){
                goto out_peg_288;
            }
            low = result_peg_286.getValues();
        
        
        
        result_peg_286 = rule_s(stream, result_peg_286.getPosition());
            if (result_peg_286.error()){
                goto out_peg_288;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_286.getPosition()))){
                    result_peg_286.nextPosition();
                } else {
                    goto out_peg_288;
                }
            }
            result_peg_286.setValue((void*) ",");
        
        
        
        result_peg_286 = rule_s(stream, result_peg_286.getPosition());
            if (result_peg_286.error()){
                goto out_peg_288;
            }
        
        
        
        result_peg_286 = rule_value(stream, result_peg_286.getPosition());
            if (result_peg_286.error()){
                goto out_peg_288;
            }
            high = result_peg_286.getValues();
        
        
        
        result_peg_286 = rule_s(stream, result_peg_286.getPosition());
            if (result_peg_286.error()){
                goto out_peg_288;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_286.getPosition()))){
                    result_peg_286.nextPosition();
                } else {
                    goto out_peg_288;
                }
            }
            result_peg_286.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_286.setValue(value);
            }
        
        
    }
    
    if (column_peg_252.chunk3 == 0){
        column_peg_252.chunk3 = new Chunk3();
    }
    column_peg_252.chunk3->chunk_range = result_peg_286;
    stream.update(result_peg_286.getPosition());
    
    
    return result_peg_286;
    out_peg_288:
    
    if (column_peg_252.chunk3 == 0){
        column_peg_252.chunk3 = new Chunk3();
    }
    column_peg_252.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_297 = stream.getColumn(position);
    if (column_peg_297.chunk3 != 0 && column_peg_297.chunk3->chunk_name.calculated()){
        return column_peg_297.chunk3->chunk_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_298(myposition);
    
    {
    
        {
                
                char letter_peg_304 = stream.get(result_peg_298.getPosition());
                if (letter_peg_304 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_304) != NULL){
                    result_peg_298.nextPosition();
                    result_peg_298.setValue((void*) letter_peg_304);
                } else {
                    goto out_peg_303;
                }
                
            }
            goto success_peg_300;
            out_peg_303:
            goto out_peg_305;
            success_peg_300:
            ;
        
        Result result_peg_299 = result_peg_298;
        
        result_peg_298.reset();
            do{
                Result result_peg_308(result_peg_298.getPosition());
                result_peg_308 = rule_alpha_digit(stream, result_peg_308.getPosition());
                if (result_peg_308.error()){
                    goto loop_peg_307;
                }
                result_peg_298.addResult(result_peg_308);
            } while (true);
            loop_peg_307:
            ;
        
        Result result_peg_306 = result_peg_298;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_299.getValues().getValue(),result_peg_306.getValues());
                result_peg_298.setValue(value);
            }
        
        
    }
    
    if (column_peg_297.chunk3 == 0){
        column_peg_297.chunk3 = new Chunk3();
    }
    column_peg_297.chunk3->chunk_name = result_peg_298;
    stream.update(result_peg_298.getPosition());
    
    
    return result_peg_298;
    out_peg_305:
    
    if (column_peg_297.chunk3 == 0){
        column_peg_297.chunk3 = new Chunk3();
    }
    column_peg_297.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_309 = stream.getColumn(position);
    if (column_peg_309.chunk3 != 0 && column_peg_309.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_309.chunk3->chunk_alpha_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_310(myposition);
    
    {
        
        char letter_peg_315 = stream.get(result_peg_310.getPosition());
        if (letter_peg_315 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_315) != NULL){
            result_peg_310.nextPosition();
            result_peg_310.setValue((void*) letter_peg_315);
        } else {
            goto out_peg_314;
        }
        
    }
    goto success_peg_311;
    out_peg_314:
    goto out_peg_316;
    success_peg_311:
    ;
    
    if (column_peg_309.chunk3 == 0){
        column_peg_309.chunk3 = new Chunk3();
    }
    column_peg_309.chunk3->chunk_alpha_digit = result_peg_310;
    stream.update(result_peg_310.getPosition());
    
    
    return result_peg_310;
    out_peg_316:
    Result result_peg_317(myposition);
    
    {
        
        char letter_peg_322 = stream.get(result_peg_317.getPosition());
        if (letter_peg_322 != '\0' && strchr("0123456789", letter_peg_322) != NULL){
            result_peg_317.nextPosition();
            result_peg_317.setValue((void*) letter_peg_322);
        } else {
            goto out_peg_321;
        }
        
    }
    goto success_peg_318;
    out_peg_321:
    goto out_peg_323;
    success_peg_318:
    ;
    
    if (column_peg_309.chunk3 == 0){
        column_peg_309.chunk3 = new Chunk3();
    }
    column_peg_309.chunk3->chunk_alpha_digit = result_peg_317;
    stream.update(result_peg_317.getPosition());
    
    
    return result_peg_317;
    out_peg_323:
    
    if (column_peg_309.chunk3 == 0){
        column_peg_309.chunk3 = new Chunk3();
    }
    column_peg_309.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_324 = stream.getColumn(position);
    if (column_peg_324.chunk3 != 0 && column_peg_324.chunk3->chunk_valuelist.calculated()){
        return column_peg_324.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_325(myposition);
    
    {
    
        result_peg_325 = rule_expr_c(stream, result_peg_325.getPosition());
            if (result_peg_325.error()){
                goto out_peg_327;
            }
            first = result_peg_325.getValues();
        
        
        
        result_peg_325.reset();
            do{
                Result result_peg_330(result_peg_325.getPosition());
                {
                
                    result_peg_330 = rule_s(stream, result_peg_330.getPosition());
                        if (result_peg_330.error()){
                            goto loop_peg_329;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_330.getPosition()))){
                                result_peg_330.nextPosition();
                            } else {
                                goto loop_peg_329;
                            }
                        }
                        result_peg_330.setValue((void*) ",");
                    
                    
                    
                    result_peg_330 = rule_s(stream, result_peg_330.getPosition());
                        if (result_peg_330.error()){
                            goto loop_peg_329;
                        }
                    
                    
                    
                    result_peg_330 = rule_expr_c(stream, result_peg_330.getPosition());
                        if (result_peg_330.error()){
                            goto loop_peg_329;
                        }
                    
                    
                }
                result_peg_325.addResult(result_peg_330);
            } while (true);
            loop_peg_329:
            if (result_peg_325.matches() == 0){
                goto out_peg_327;
            }
            rest = result_peg_325.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_325.setValue(value);
            }
        
        
    }
    
    if (column_peg_324.chunk3 == 0){
        column_peg_324.chunk3 = new Chunk3();
    }
    column_peg_324.chunk3->chunk_valuelist = result_peg_325;
    stream.update(result_peg_325.getPosition());
    
    
    return result_peg_325;
    out_peg_327:
    Result result_peg_334(myposition);
    
    {
    
        result_peg_334 = rule_s(stream, result_peg_334.getPosition());
            if (result_peg_334.error()){
                goto out_peg_336;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_334.getPosition()))){
                    result_peg_334.nextPosition();
                } else {
                    goto out_peg_336;
                }
            }
            result_peg_334.setValue((void*) ",");
        
        
        
        result_peg_334 = rule_s(stream, result_peg_334.getPosition());
            if (result_peg_334.error()){
                goto out_peg_336;
            }
        
        
        
        result_peg_334 = rule_expr_c(stream, result_peg_334.getPosition());
            if (result_peg_334.error()){
                goto out_peg_336;
            }
            first = result_peg_334.getValues();
        
        
        
        result_peg_334.reset();
            do{
                Result result_peg_342(result_peg_334.getPosition());
                {
                
                    result_peg_342 = rule_s(stream, result_peg_342.getPosition());
                        if (result_peg_342.error()){
                            goto loop_peg_341;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_342.getPosition()))){
                                result_peg_342.nextPosition();
                            } else {
                                goto loop_peg_341;
                            }
                        }
                        result_peg_342.setValue((void*) ",");
                    
                    
                    
                    result_peg_342 = rule_s(stream, result_peg_342.getPosition());
                        if (result_peg_342.error()){
                            goto loop_peg_341;
                        }
                    
                    
                    
                    result_peg_342 = rule_expr_c(stream, result_peg_342.getPosition());
                        if (result_peg_342.error()){
                            goto loop_peg_341;
                        }
                    
                    
                }
                result_peg_334.addResult(result_peg_342);
            } while (true);
            loop_peg_341:
            ;
            rest = result_peg_334.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_334.setValue(value);
            }
        
        
    }
    
    if (column_peg_324.chunk3 == 0){
        column_peg_324.chunk3 = new Chunk3();
    }
    column_peg_324.chunk3->chunk_valuelist = result_peg_334;
    stream.update(result_peg_334.getPosition());
    
    
    return result_peg_334;
    out_peg_336:
    
    if (column_peg_324.chunk3 == 0){
        column_peg_324.chunk3 = new Chunk3();
    }
    column_peg_324.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_346 = stream.getColumn(position);
    if (column_peg_346.chunk3 != 0 && column_peg_346.chunk3->chunk_expr.calculated()){
        return column_peg_346.chunk3->chunk_expr;
    }
    
    int myposition = position;
    
    
    Result result_peg_347(myposition);
    
    {
    
        result_peg_347 = rule_expr_c(stream, result_peg_347.getPosition());
            if (result_peg_347.error()){
                goto out_peg_349;
            }
        
        Result result_peg_348 = result_peg_347;
        
        Result result_peg_352(result_peg_347);
            {
            
                result_peg_352 = rule_s(stream, result_peg_352.getPosition());
                    if (result_peg_352.error()){
                        goto not_peg_351;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_352.getPosition()))){
                            result_peg_352.nextPosition();
                        } else {
                            goto not_peg_351;
                        }
                    }
                    result_peg_352.setValue((void*) ",");
                
                
            }
            goto out_peg_349;
            not_peg_351:
            result_peg_347.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_348.getValues();
                result_peg_347.setValue(value);
            }
        
        
    }
    
    if (column_peg_346.chunk3 == 0){
        column_peg_346.chunk3 = new Chunk3();
    }
    column_peg_346.chunk3->chunk_expr = result_peg_347;
    stream.update(result_peg_347.getPosition());
    
    
    return result_peg_347;
    out_peg_349:
    Result result_peg_354(myposition);
    
    {
    
        result_peg_354 = rule_valuelist(stream, result_peg_354.getPosition());
            if (result_peg_354.error()){
                goto out_peg_356;
            }
        
        Result result_peg_355 = result_peg_354;
        
        result_peg_354 = rule_expr2_rest(stream, result_peg_354.getPosition(), result_peg_355.getValues());
            if (result_peg_354.error()){
                goto out_peg_356;
            }
        
        
    }
    
    if (column_peg_346.chunk3 == 0){
        column_peg_346.chunk3 = new Chunk3();
    }
    column_peg_346.chunk3->chunk_expr = result_peg_354;
    stream.update(result_peg_354.getPosition());
    
    
    return result_peg_354;
    out_peg_356:
    
    if (column_peg_346.chunk3 == 0){
        column_peg_346.chunk3 = new Chunk3();
    }
    column_peg_346.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_357 = stream.getColumn(position);
    if (column_peg_357.chunk4 != 0 && column_peg_357.chunk4->chunk_expr_c.calculated()){
        return column_peg_357.chunk4->chunk_expr_c;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_358(myposition);
    
    {
    
        result_peg_358 = rule_expr2(stream, result_peg_358.getPosition());
            if (result_peg_358.error()){
                goto out_peg_360;
            }
            left = result_peg_358.getValues();
        
        
        
        result_peg_358 = rule_expr_rest(stream, result_peg_358.getPosition(), left);
            if (result_peg_358.error()){
                goto out_peg_360;
            }
        
        
    }
    
    if (column_peg_357.chunk4 == 0){
        column_peg_357.chunk4 = new Chunk4();
    }
    column_peg_357.chunk4->chunk_expr_c = result_peg_358;
    stream.update(result_peg_358.getPosition());
    
    
    return result_peg_358;
    out_peg_360:
    
    if (column_peg_357.chunk4 == 0){
        column_peg_357.chunk4 = new Chunk4();
    }
    column_peg_357.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_361 = stream.getColumn(position);
    if (column_peg_361.chunk4 != 0 && column_peg_361.chunk4->chunk_expr2.calculated()){
        return column_peg_361.chunk4->chunk_expr2;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_362(myposition);
    
    {
    
        result_peg_362 = rule_expr3(stream, result_peg_362.getPosition());
            if (result_peg_362.error()){
                goto out_peg_364;
            }
            left = result_peg_362.getValues();
        
        
        
        result_peg_362 = rule_expr2_rest(stream, result_peg_362.getPosition(), left);
            if (result_peg_362.error()){
                goto out_peg_364;
            }
        
        
    }
    
    if (column_peg_361.chunk4 == 0){
        column_peg_361.chunk4 = new Chunk4();
    }
    column_peg_361.chunk4->chunk_expr2 = result_peg_362;
    stream.update(result_peg_362.getPosition());
    
    
    return result_peg_362;
    out_peg_364:
    
    if (column_peg_361.chunk4 == 0){
        column_peg_361.chunk4 = new Chunk4();
    }
    column_peg_361.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_365 = stream.getColumn(position);
    if (column_peg_365.chunk4 != 0 && column_peg_365.chunk4->chunk_expr3.calculated()){
        return column_peg_365.chunk4->chunk_expr3;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_366(myposition);
    
    {
    
        result_peg_366 = rule_expr4(stream, result_peg_366.getPosition());
            if (result_peg_366.error()){
                goto out_peg_368;
            }
            left = result_peg_366.getValues();
        
        
        
        result_peg_366 = rule_expr3_rest(stream, result_peg_366.getPosition(), left);
            if (result_peg_366.error()){
                goto out_peg_368;
            }
        
        
    }
    
    if (column_peg_365.chunk4 == 0){
        column_peg_365.chunk4 = new Chunk4();
    }
    column_peg_365.chunk4->chunk_expr3 = result_peg_366;
    stream.update(result_peg_366.getPosition());
    
    
    return result_peg_366;
    out_peg_368:
    
    if (column_peg_365.chunk4 == 0){
        column_peg_365.chunk4 = new Chunk4();
    }
    column_peg_365.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_369 = stream.getColumn(position);
    if (column_peg_369.chunk4 != 0 && column_peg_369.chunk4->chunk_expr4.calculated()){
        return column_peg_369.chunk4->chunk_expr4;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_370(myposition);
    
    {
    
        result_peg_370 = rule_expr5(stream, result_peg_370.getPosition());
            if (result_peg_370.error()){
                goto out_peg_372;
            }
            left = result_peg_370.getValues();
        
        
        
        result_peg_370 = rule_expr4_rest(stream, result_peg_370.getPosition(), left);
            if (result_peg_370.error()){
                goto out_peg_372;
            }
        
        
    }
    
    if (column_peg_369.chunk4 == 0){
        column_peg_369.chunk4 = new Chunk4();
    }
    column_peg_369.chunk4->chunk_expr4 = result_peg_370;
    stream.update(result_peg_370.getPosition());
    
    
    return result_peg_370;
    out_peg_372:
    
    if (column_peg_369.chunk4 == 0){
        column_peg_369.chunk4 = new Chunk4();
    }
    column_peg_369.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_373 = stream.getColumn(position);
    if (column_peg_373.chunk4 != 0 && column_peg_373.chunk4->chunk_expr5.calculated()){
        return column_peg_373.chunk4->chunk_expr5;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_374(myposition);
    
    {
    
        result_peg_374 = rule_expr6(stream, result_peg_374.getPosition());
            if (result_peg_374.error()){
                goto out_peg_376;
            }
            left = result_peg_374.getValues();
        
        
        
        result_peg_374 = rule_expr5_rest(stream, result_peg_374.getPosition(), left);
            if (result_peg_374.error()){
                goto out_peg_376;
            }
        
        
    }
    
    if (column_peg_373.chunk4 == 0){
        column_peg_373.chunk4 = new Chunk4();
    }
    column_peg_373.chunk4->chunk_expr5 = result_peg_374;
    stream.update(result_peg_374.getPosition());
    
    
    return result_peg_374;
    out_peg_376:
    
    if (column_peg_373.chunk4 == 0){
        column_peg_373.chunk4 = new Chunk4();
    }
    column_peg_373.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_377 = stream.getColumn(position);
    if (column_peg_377.chunk5 != 0 && column_peg_377.chunk5->chunk_expr6.calculated()){
        return column_peg_377.chunk5->chunk_expr6;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_378(myposition);
    
    {
    
        result_peg_378 = rule_expr7(stream, result_peg_378.getPosition());
            if (result_peg_378.error()){
                goto out_peg_380;
            }
            left = result_peg_378.getValues();
        
        
        
        result_peg_378 = rule_expr6_rest(stream, result_peg_378.getPosition(), left);
            if (result_peg_378.error()){
                goto out_peg_380;
            }
        
        
    }
    
    if (column_peg_377.chunk5 == 0){
        column_peg_377.chunk5 = new Chunk5();
    }
    column_peg_377.chunk5->chunk_expr6 = result_peg_378;
    stream.update(result_peg_378.getPosition());
    
    
    return result_peg_378;
    out_peg_380:
    
    if (column_peg_377.chunk5 == 0){
        column_peg_377.chunk5 = new Chunk5();
    }
    column_peg_377.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_381 = stream.getColumn(position);
    if (column_peg_381.chunk5 != 0 && column_peg_381.chunk5->chunk_expr7.calculated()){
        return column_peg_381.chunk5->chunk_expr7;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_382(myposition);
    
    {
    
        result_peg_382 = rule_expr8(stream, result_peg_382.getPosition());
            if (result_peg_382.error()){
                goto out_peg_384;
            }
            left = result_peg_382.getValues();
        
        
        
        result_peg_382 = rule_expr7_rest(stream, result_peg_382.getPosition(), left);
            if (result_peg_382.error()){
                goto out_peg_384;
            }
        
        
    }
    
    if (column_peg_381.chunk5 == 0){
        column_peg_381.chunk5 = new Chunk5();
    }
    column_peg_381.chunk5->chunk_expr7 = result_peg_382;
    stream.update(result_peg_382.getPosition());
    
    
    return result_peg_382;
    out_peg_384:
    
    if (column_peg_381.chunk5 == 0){
        column_peg_381.chunk5 = new Chunk5();
    }
    column_peg_381.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_385 = stream.getColumn(position);
    if (column_peg_385.chunk5 != 0 && column_peg_385.chunk5->chunk_expr8.calculated()){
        return column_peg_385.chunk5->chunk_expr8;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_386(myposition);
    
    {
    
        result_peg_386 = rule_expr9(stream, result_peg_386.getPosition());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
            left = result_peg_386.getValues();
        
        
        
        result_peg_386 = rule_expr8_rest(stream, result_peg_386.getPosition(), left);
            if (result_peg_386.error()){
                goto out_peg_388;
            }
        
        
    }
    
    if (column_peg_385.chunk5 == 0){
        column_peg_385.chunk5 = new Chunk5();
    }
    column_peg_385.chunk5->chunk_expr8 = result_peg_386;
    stream.update(result_peg_386.getPosition());
    
    
    return result_peg_386;
    out_peg_388:
    
    if (column_peg_385.chunk5 == 0){
        column_peg_385.chunk5 = new Chunk5();
    }
    column_peg_385.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_389 = stream.getColumn(position);
    if (column_peg_389.chunk5 != 0 && column_peg_389.chunk5->chunk_expr9.calculated()){
        return column_peg_389.chunk5->chunk_expr9;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_390(myposition);
    
    {
    
        result_peg_390 = rule_expr10(stream, result_peg_390.getPosition());
            if (result_peg_390.error()){
                goto out_peg_392;
            }
            left = result_peg_390.getValues();
        
        
        
        result_peg_390 = rule_expr9_rest(stream, result_peg_390.getPosition(), left);
            if (result_peg_390.error()){
                goto out_peg_392;
            }
        
        
    }
    
    if (column_peg_389.chunk5 == 0){
        column_peg_389.chunk5 = new Chunk5();
    }
    column_peg_389.chunk5->chunk_expr9 = result_peg_390;
    stream.update(result_peg_390.getPosition());
    
    
    return result_peg_390;
    out_peg_392:
    
    if (column_peg_389.chunk5 == 0){
        column_peg_389.chunk5 = new Chunk5();
    }
    column_peg_389.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_393 = stream.getColumn(position);
    if (column_peg_393.chunk5 != 0 && column_peg_393.chunk5->chunk_expr10.calculated()){
        return column_peg_393.chunk5->chunk_expr10;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_394(myposition);
    
    {
    
        result_peg_394 = rule_expr11(stream, result_peg_394.getPosition());
            if (result_peg_394.error()){
                goto out_peg_396;
            }
            left = result_peg_394.getValues();
        
        
        
        result_peg_394 = rule_expr10_rest(stream, result_peg_394.getPosition(), left);
            if (result_peg_394.error()){
                goto out_peg_396;
            }
        
        
    }
    
    if (column_peg_393.chunk5 == 0){
        column_peg_393.chunk5 = new Chunk5();
    }
    column_peg_393.chunk5->chunk_expr10 = result_peg_394;
    stream.update(result_peg_394.getPosition());
    
    
    return result_peg_394;
    out_peg_396:
    
    if (column_peg_393.chunk5 == 0){
        column_peg_393.chunk5 = new Chunk5();
    }
    column_peg_393.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_397 = stream.getColumn(position);
    if (column_peg_397.chunk6 != 0 && column_peg_397.chunk6->chunk_expr11.calculated()){
        return column_peg_397.chunk6->chunk_expr11;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_398(myposition);
    
    {
    
        result_peg_398 = rule_expr12(stream, result_peg_398.getPosition());
            if (result_peg_398.error()){
                goto out_peg_400;
            }
            left = result_peg_398.getValues();
        
        
        
        result_peg_398 = rule_expr11_rest(stream, result_peg_398.getPosition(), left);
            if (result_peg_398.error()){
                goto out_peg_400;
            }
        
        
    }
    
    if (column_peg_397.chunk6 == 0){
        column_peg_397.chunk6 = new Chunk6();
    }
    column_peg_397.chunk6->chunk_expr11 = result_peg_398;
    stream.update(result_peg_398.getPosition());
    
    
    return result_peg_398;
    out_peg_400:
    
    if (column_peg_397.chunk6 == 0){
        column_peg_397.chunk6 = new Chunk6();
    }
    column_peg_397.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_401 = stream.getColumn(position);
    if (column_peg_401.chunk6 != 0 && column_peg_401.chunk6->chunk_expr12.calculated()){
        return column_peg_401.chunk6->chunk_expr12;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_402(myposition);
    
    {
    
        result_peg_402 = rule_expr13(stream, result_peg_402.getPosition());
            if (result_peg_402.error()){
                goto out_peg_404;
            }
            left = result_peg_402.getValues();
        
        
        
        result_peg_402 = rule_expr12_rest(stream, result_peg_402.getPosition(), left);
            if (result_peg_402.error()){
                goto out_peg_404;
            }
        
        
    }
    
    if (column_peg_401.chunk6 == 0){
        column_peg_401.chunk6 = new Chunk6();
    }
    column_peg_401.chunk6->chunk_expr12 = result_peg_402;
    stream.update(result_peg_402.getPosition());
    
    
    return result_peg_402;
    out_peg_404:
    
    if (column_peg_401.chunk6 == 0){
        column_peg_401.chunk6 = new Chunk6();
    }
    column_peg_401.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_405 = stream.getColumn(position);
    if (column_peg_405.chunk6 != 0 && column_peg_405.chunk6->chunk_expr13.calculated()){
        return column_peg_405.chunk6->chunk_expr13;
    }
    
    int myposition = position;
    
    Value exp;
    Result result_peg_406(myposition);
    
    {
    
        result_peg_406.reset();
            do{
                Result result_peg_409(result_peg_406.getPosition());
                result_peg_409 = rule_unary(stream, result_peg_409.getPosition());
                if (result_peg_409.error()){
                    goto loop_peg_408;
                }
                result_peg_406.addResult(result_peg_409);
            } while (true);
            loop_peg_408:
            ;
        
        Result result_peg_407 = result_peg_406;
        
        result_peg_406 = rule_expr13_real(stream, result_peg_406.getPosition());
            if (result_peg_406.error()){
                goto out_peg_411;
            }
            exp = result_peg_406.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_407.getValues(), exp);
                result_peg_406.setValue(value);
            }
        
        
    }
    
    if (column_peg_405.chunk6 == 0){
        column_peg_405.chunk6 = new Chunk6();
    }
    column_peg_405.chunk6->chunk_expr13 = result_peg_406;
    stream.update(result_peg_406.getPosition());
    
    
    return result_peg_406;
    out_peg_411:
    
    if (column_peg_405.chunk6 == 0){
        column_peg_405.chunk6 = new Chunk6();
    }
    column_peg_405.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_414:
    Value right;
    Value new_left;
    Result result_peg_413(myposition);
    {
    
        result_peg_413 = rule_s(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_416;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_413.getPosition()))){
                    result_peg_413.nextPosition();
                } else {
                    goto out_peg_416;
                }
            }
            result_peg_413.setValue((void*) "||");
        
        
        
        result_peg_413 = rule_s(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_416;
            }
        
        
        
        result_peg_413 = rule_expr2(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_416;
            }
            right = result_peg_413.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_413.setValue(value);
            }
            new_left = result_peg_413.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_413.getPosition();
    goto tail_peg_414;
    out_peg_416:
    Result result_peg_421(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_421.setValue(value);
            }
        
        
    }
    
    
    return result_peg_421;
    
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_425:
    Value right;
    Value new_left;
    Result result_peg_424(myposition);
    {
    
        result_peg_424 = rule_s(stream, result_peg_424.getPosition());
            if (result_peg_424.error()){
                goto out_peg_427;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_424.getPosition()))){
                    result_peg_424.nextPosition();
                } else {
                    goto out_peg_427;
                }
            }
            result_peg_424.setValue((void*) "^^");
        
        
        
        result_peg_424 = rule_s(stream, result_peg_424.getPosition());
            if (result_peg_424.error()){
                goto out_peg_427;
            }
        
        
        
        result_peg_424 = rule_expr3(stream, result_peg_424.getPosition());
            if (result_peg_424.error()){
                goto out_peg_427;
            }
            right = result_peg_424.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_424.setValue(value);
            }
            new_left = result_peg_424.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_424.getPosition();
    goto tail_peg_425;
    out_peg_427:
    Result result_peg_432(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_432.setValue(value);
            }
        
        
    }
    
    
    return result_peg_432;
    
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_436:
    Value right;
    Value new_left;
    Result result_peg_435(myposition);
    {
    
        result_peg_435 = rule_s(stream, result_peg_435.getPosition());
            if (result_peg_435.error()){
                goto out_peg_438;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_435.getPosition()))){
                    result_peg_435.nextPosition();
                } else {
                    goto out_peg_438;
                }
            }
            result_peg_435.setValue((void*) "&&");
        
        
        
        result_peg_435 = rule_s(stream, result_peg_435.getPosition());
            if (result_peg_435.error()){
                goto out_peg_438;
            }
        
        
        
        result_peg_435 = rule_expr4(stream, result_peg_435.getPosition());
            if (result_peg_435.error()){
                goto out_peg_438;
            }
            right = result_peg_435.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_435.setValue(value);
            }
            new_left = result_peg_435.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_435.getPosition();
    goto tail_peg_436;
    out_peg_438:
    Result result_peg_443(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_443.setValue(value);
            }
        
        
    }
    
    
    return result_peg_443;
    
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
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
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_446.getPosition()))){
                    result_peg_446.nextPosition();
                } else {
                    goto out_peg_449;
                }
            }
            result_peg_446.setValue((void*) "|");
        
        
        
        result_peg_446 = rule_s(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
        
        
        
        result_peg_446 = rule_expr5(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
            right = result_peg_446.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
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
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_458:
    Value right;
    Value new_left;
    Result result_peg_457(myposition);
    {
    
        result_peg_457 = rule_s(stream, result_peg_457.getPosition());
            if (result_peg_457.error()){
                goto out_peg_460;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_457.getPosition()))){
                    result_peg_457.nextPosition();
                } else {
                    goto out_peg_460;
                }
            }
            result_peg_457.setValue((void*) "^");
        
        
        
        result_peg_457 = rule_s(stream, result_peg_457.getPosition());
            if (result_peg_457.error()){
                goto out_peg_460;
            }
        
        
        
        result_peg_457 = rule_expr6(stream, result_peg_457.getPosition());
            if (result_peg_457.error()){
                goto out_peg_460;
            }
            right = result_peg_457.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_457.setValue(value);
            }
            new_left = result_peg_457.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_457.getPosition();
    goto tail_peg_458;
    out_peg_460:
    Result result_peg_465(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_465.setValue(value);
            }
        
        
    }
    
    
    return result_peg_465;
    
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_469:
    Value right;
    Value new_left;
    Result result_peg_468(myposition);
    {
    
        result_peg_468 = rule_s(stream, result_peg_468.getPosition());
            if (result_peg_468.error()){
                goto out_peg_471;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_468.getPosition()))){
                    result_peg_468.nextPosition();
                } else {
                    goto out_peg_471;
                }
            }
            result_peg_468.setValue((void*) "&");
        
        
        
        result_peg_468 = rule_s(stream, result_peg_468.getPosition());
            if (result_peg_468.error()){
                goto out_peg_471;
            }
        
        
        
        result_peg_468 = rule_expr7(stream, result_peg_468.getPosition());
            if (result_peg_468.error()){
                goto out_peg_471;
            }
            right = result_peg_468.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_468.setValue(value);
            }
            new_left = result_peg_468.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_468.getPosition();
    goto tail_peg_469;
    out_peg_471:
    Result result_peg_476(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_476.setValue(value);
            }
        
        
    }
    
    
    return result_peg_476;
    
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_480:
    Value right;
    Value new_left;
    Result result_peg_479(myposition);
    {
    
        result_peg_479 = rule_s(stream, result_peg_479.getPosition());
            if (result_peg_479.error()){
                goto out_peg_482;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_479.getPosition()))){
                    result_peg_479.nextPosition();
                } else {
                    goto out_peg_482;
                }
            }
            result_peg_479.setValue((void*) ":=");
        
        
        
        result_peg_479 = rule_s(stream, result_peg_479.getPosition());
            if (result_peg_479.error()){
                goto out_peg_482;
            }
        
        
        
        result_peg_479 = rule_expr8(stream, result_peg_479.getPosition());
            if (result_peg_479.error()){
                goto out_peg_482;
            }
            right = result_peg_479.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_479.setValue(value);
            }
            new_left = result_peg_479.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_479.getPosition();
    goto tail_peg_480;
    out_peg_482:
    Result result_peg_487(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_487.setValue(value);
            }
        
        
    }
    
    
    return result_peg_487;
    
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_491:
    Value right;
    Value new_left;
    Result result_peg_490(myposition);
    {
    
        result_peg_490 = rule_s(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_493;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_490.getPosition()))){
                    result_peg_490.nextPosition();
                } else {
                    goto out_peg_493;
                }
            }
            result_peg_490.setValue((void*) "=");
        
        
        
        result_peg_490 = rule_s(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_493;
            }
        
        
        
        result_peg_490 = rule_expr9(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_493;
            }
            right = result_peg_490.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_490.setValue(value);
            }
            new_left = result_peg_490.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_490.getPosition();
    goto tail_peg_491;
    out_peg_493:
    Result result_peg_498(myposition);
    {
    
        result_peg_498 = rule_s(stream, result_peg_498.getPosition());
            if (result_peg_498.error()){
                goto out_peg_500;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_498.getPosition()))){
                    result_peg_498.nextPosition();
                } else {
                    goto out_peg_500;
                }
            }
            result_peg_498.setValue((void*) "!=");
        
        
        
        result_peg_498 = rule_s(stream, result_peg_498.getPosition());
            if (result_peg_498.error()){
                goto out_peg_500;
            }
        
        
        
        result_peg_498 = rule_expr9(stream, result_peg_498.getPosition());
            if (result_peg_498.error()){
                goto out_peg_500;
            }
            right = result_peg_498.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_498.setValue(value);
            }
            new_left = result_peg_498.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_498.getPosition();
    goto tail_peg_491;
    out_peg_500:
    Result result_peg_505(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_505.setValue(value);
            }
        
        
    }
    
    
    return result_peg_505;
    
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_509:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_508(myposition);
    {
    
        result_peg_508 = rule_s(stream, result_peg_508.getPosition());
            if (result_peg_508.error()){
                goto out_peg_511;
            }
        
        
        
        result_peg_508 = rule_compare(stream, result_peg_508.getPosition());
            if (result_peg_508.error()){
                goto out_peg_511;
            }
            maker = result_peg_508.getValues();
        
        
        
        result_peg_508 = rule_s(stream, result_peg_508.getPosition());
            if (result_peg_508.error()){
                goto out_peg_511;
            }
        
        
        
        result_peg_508 = rule_expr10(stream, result_peg_508.getPosition());
            if (result_peg_508.error()){
                goto out_peg_511;
            }
            right = result_peg_508.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_508.setValue(value);
            }
            new_left = result_peg_508.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_508.getPosition();
    goto tail_peg_509;
    out_peg_511:
    Result result_peg_516(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_516.setValue(value);
            }
        
        
    }
    
    
    return result_peg_516;
    
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_518 = stream.getColumn(position);
    if (column_peg_518.chunk8 != 0 && column_peg_518.chunk8->chunk_compare.calculated()){
        return column_peg_518.chunk8->chunk_compare;
    }
    
    int myposition = position;
    
    
    Result result_peg_519(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_519.getPosition()))){
                    result_peg_519.nextPosition();
                } else {
                    goto out_peg_521;
                }
            }
            result_peg_519.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_519.setValue(value);
            }
        
        
    }
    
    if (column_peg_518.chunk8 == 0){
        column_peg_518.chunk8 = new Chunk8();
    }
    column_peg_518.chunk8->chunk_compare = result_peg_519;
    stream.update(result_peg_519.getPosition());
    
    
    return result_peg_519;
    out_peg_521:
    Result result_peg_522(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_522.getPosition()))){
                    result_peg_522.nextPosition();
                } else {
                    goto out_peg_524;
                }
            }
            result_peg_522.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_522.setValue(value);
            }
        
        
    }
    
    if (column_peg_518.chunk8 == 0){
        column_peg_518.chunk8 = new Chunk8();
    }
    column_peg_518.chunk8->chunk_compare = result_peg_522;
    stream.update(result_peg_522.getPosition());
    
    
    return result_peg_522;
    out_peg_524:
    Result result_peg_525(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_525.getPosition()))){
                    result_peg_525.nextPosition();
                } else {
                    goto out_peg_527;
                }
            }
            result_peg_525.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_525.setValue(value);
            }
        
        
    }
    
    if (column_peg_518.chunk8 == 0){
        column_peg_518.chunk8 = new Chunk8();
    }
    column_peg_518.chunk8->chunk_compare = result_peg_525;
    stream.update(result_peg_525.getPosition());
    
    
    return result_peg_525;
    out_peg_527:
    Result result_peg_528(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_528.getPosition()))){
                    result_peg_528.nextPosition();
                } else {
                    goto out_peg_530;
                }
            }
            result_peg_528.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_528.setValue(value);
            }
        
        
    }
    
    if (column_peg_518.chunk8 == 0){
        column_peg_518.chunk8 = new Chunk8();
    }
    column_peg_518.chunk8->chunk_compare = result_peg_528;
    stream.update(result_peg_528.getPosition());
    
    
    return result_peg_528;
    out_peg_530:
    
    if (column_peg_518.chunk8 == 0){
        column_peg_518.chunk8 = new Chunk8();
    }
    column_peg_518.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_531 = stream.getColumn(position);
    if (column_peg_531.chunk8 != 0 && column_peg_531.chunk8->chunk_all_compare.calculated()){
        return column_peg_531.chunk8->chunk_all_compare;
    }
    
    int myposition = position;
    
    
    Result result_peg_532(myposition);
    
    result_peg_532 = rule_compare(stream, result_peg_532.getPosition());
    if (result_peg_532.error()){
        goto out_peg_533;
    }
    
    if (column_peg_531.chunk8 == 0){
        column_peg_531.chunk8 = new Chunk8();
    }
    column_peg_531.chunk8->chunk_all_compare = result_peg_532;
    stream.update(result_peg_532.getPosition());
    
    
    return result_peg_532;
    out_peg_533:
    Result result_peg_534(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_534.getPosition()))){
            result_peg_534.nextPosition();
        } else {
            goto out_peg_535;
        }
    }
    result_peg_534.setValue((void*) "=");
    
    if (column_peg_531.chunk8 == 0){
        column_peg_531.chunk8 = new Chunk8();
    }
    column_peg_531.chunk8->chunk_all_compare = result_peg_534;
    stream.update(result_peg_534.getPosition());
    
    
    return result_peg_534;
    out_peg_535:
    Result result_peg_536(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_536.getPosition()))){
            result_peg_536.nextPosition();
        } else {
            goto out_peg_537;
        }
    }
    result_peg_536.setValue((void*) "!=");
    
    if (column_peg_531.chunk8 == 0){
        column_peg_531.chunk8 = new Chunk8();
    }
    column_peg_531.chunk8->chunk_all_compare = result_peg_536;
    stream.update(result_peg_536.getPosition());
    
    
    return result_peg_536;
    out_peg_537:
    
    if (column_peg_531.chunk8 == 0){
        column_peg_531.chunk8 = new Chunk8();
    }
    column_peg_531.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_540:
    Value right;
    Value new_left;
    Result result_peg_539(myposition);
    {
    
        result_peg_539 = rule_s(stream, result_peg_539.getPosition());
            if (result_peg_539.error()){
                goto out_peg_542;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_539.getPosition()))){
                    result_peg_539.nextPosition();
                } else {
                    goto out_peg_542;
                }
            }
            result_peg_539.setValue((void*) "+");
        
        
        
        result_peg_539 = rule_s(stream, result_peg_539.getPosition());
            if (result_peg_539.error()){
                goto out_peg_542;
            }
        
        
        
        result_peg_539 = rule_expr11(stream, result_peg_539.getPosition());
            if (result_peg_539.error()){
                goto out_peg_542;
            }
            right = result_peg_539.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_539.setValue(value);
            }
            new_left = result_peg_539.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_539.getPosition();
    goto tail_peg_540;
    out_peg_542:
    Result result_peg_547(myposition);
    {
    
        result_peg_547 = rule_s(stream, result_peg_547.getPosition());
            if (result_peg_547.error()){
                goto out_peg_549;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_547.getPosition()))){
                    result_peg_547.nextPosition();
                } else {
                    goto out_peg_549;
                }
            }
            result_peg_547.setValue((void*) "-");
        
        
        
        result_peg_547 = rule_s(stream, result_peg_547.getPosition());
            if (result_peg_547.error()){
                goto out_peg_549;
            }
        
        
        
        result_peg_547 = rule_expr11(stream, result_peg_547.getPosition());
            if (result_peg_547.error()){
                goto out_peg_549;
            }
            right = result_peg_547.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_547.setValue(value);
            }
            new_left = result_peg_547.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_547.getPosition();
    goto tail_peg_540;
    out_peg_549:
    Result result_peg_554(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_554.setValue(value);
            }
        
        
    }
    
    
    return result_peg_554;
    
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_558:
    Value right;
    Value new_left;
    Result result_peg_557(myposition);
    {
    
        result_peg_557 = rule_s(stream, result_peg_557.getPosition());
            if (result_peg_557.error()){
                goto out_peg_560;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_557.getPosition()))){
                    result_peg_557.nextPosition();
                } else {
                    goto out_peg_560;
                }
            }
            result_peg_557.setValue((void*) "*");
        
        
        
        result_peg_557 = rule_s(stream, result_peg_557.getPosition());
            if (result_peg_557.error()){
                goto out_peg_560;
            }
        
        
        
        result_peg_557 = rule_expr12(stream, result_peg_557.getPosition());
            if (result_peg_557.error()){
                goto out_peg_560;
            }
            right = result_peg_557.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_557.setValue(value);
            }
            new_left = result_peg_557.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_557.getPosition();
    goto tail_peg_558;
    out_peg_560:
    Result result_peg_565(myposition);
    {
    
        result_peg_565 = rule_s(stream, result_peg_565.getPosition());
            if (result_peg_565.error()){
                goto out_peg_567;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_565.getPosition()))){
                    result_peg_565.nextPosition();
                } else {
                    goto out_peg_567;
                }
            }
            result_peg_565.setValue((void*) "/");
        
        
        
        result_peg_565 = rule_s(stream, result_peg_565.getPosition());
            if (result_peg_565.error()){
                goto out_peg_567;
            }
        
        
        
        result_peg_565 = rule_expr12(stream, result_peg_565.getPosition());
            if (result_peg_565.error()){
                goto out_peg_567;
            }
            right = result_peg_565.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_565.setValue(value);
            }
            new_left = result_peg_565.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_565.getPosition();
    goto tail_peg_558;
    out_peg_567:
    Result result_peg_572(myposition);
    {
    
        result_peg_572 = rule_s(stream, result_peg_572.getPosition());
            if (result_peg_572.error()){
                goto out_peg_574;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_572.getPosition()))){
                    result_peg_572.nextPosition();
                } else {
                    goto out_peg_574;
                }
            }
            result_peg_572.setValue((void*) "%");
        
        
        
        result_peg_572 = rule_s(stream, result_peg_572.getPosition());
            if (result_peg_572.error()){
                goto out_peg_574;
            }
        
        
        
        result_peg_572 = rule_expr12(stream, result_peg_572.getPosition());
            if (result_peg_572.error()){
                goto out_peg_574;
            }
            right = result_peg_572.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_572.setValue(value);
            }
            new_left = result_peg_572.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_572.getPosition();
    goto tail_peg_558;
    out_peg_574:
    Result result_peg_579(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_579.setValue(value);
            }
        
        
    }
    
    
    return result_peg_579;
    
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_583:
    Value right;
    Value new_left;
    Result result_peg_582(myposition);
    {
    
        result_peg_582 = rule_s(stream, result_peg_582.getPosition());
            if (result_peg_582.error()){
                goto out_peg_585;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_582.getPosition()))){
                    result_peg_582.nextPosition();
                } else {
                    goto out_peg_585;
                }
            }
            result_peg_582.setValue((void*) "**");
        
        
        
        result_peg_582 = rule_s(stream, result_peg_582.getPosition());
            if (result_peg_582.error()){
                goto out_peg_585;
            }
        
        
        
        result_peg_582 = rule_expr13(stream, result_peg_582.getPosition());
            if (result_peg_582.error()){
                goto out_peg_585;
            }
            right = result_peg_582.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_582.setValue(value);
            }
            new_left = result_peg_582.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_582.getPosition();
    goto tail_peg_583;
    out_peg_585:
    Result result_peg_590(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_590.setValue(value);
            }
        
        
    }
    
    
    return result_peg_590;
    
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_592 = stream.getColumn(position);
    if (column_peg_592.chunk9 != 0 && column_peg_592.chunk9->chunk_unary.calculated()){
        return column_peg_592.chunk9->chunk_unary;
    }
    
    int myposition = position;
    
    
    Result result_peg_593(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_593.getPosition()))){
                    result_peg_593.nextPosition();
                } else {
                    goto out_peg_595;
                }
            }
            result_peg_593.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_593.setValue(value);
            }
        
        
    }
    
    if (column_peg_592.chunk9 == 0){
        column_peg_592.chunk9 = new Chunk9();
    }
    column_peg_592.chunk9->chunk_unary = result_peg_593;
    stream.update(result_peg_593.getPosition());
    
    
    return result_peg_593;
    out_peg_595:
    Result result_peg_596(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_596.getPosition()))){
                    result_peg_596.nextPosition();
                } else {
                    goto out_peg_598;
                }
            }
            result_peg_596.setValue((void*) "-");
        
        
        
        Result result_peg_601(result_peg_596);
            {
                
                char letter_peg_606 = stream.get(result_peg_601.getPosition());
                if (letter_peg_606 != '\0' && strchr("0123456789", letter_peg_606) != NULL){
                    result_peg_601.nextPosition();
                    result_peg_601.setValue((void*) letter_peg_606);
                } else {
                    goto out_peg_605;
                }
                
            }
            goto success_peg_602;
            out_peg_605:
            goto not_peg_600;
            success_peg_602:
            ;
            goto out_peg_598;
            not_peg_600:
            result_peg_596.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_596.setValue(value);
            }
        
        
    }
    
    if (column_peg_592.chunk9 == 0){
        column_peg_592.chunk9 = new Chunk9();
    }
    column_peg_592.chunk9->chunk_unary = result_peg_596;
    stream.update(result_peg_596.getPosition());
    
    
    return result_peg_596;
    out_peg_598:
    Result result_peg_607(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_607.getPosition()))){
                    result_peg_607.nextPosition();
                } else {
                    goto out_peg_609;
                }
            }
            result_peg_607.setValue((void*) "~");
        
        
        
        Result result_peg_612(result_peg_607);
            result_peg_612 = rule_key(stream, result_peg_612.getPosition());
            if (result_peg_612.error()){
                goto not_peg_611;
            }
            goto out_peg_609;
            not_peg_611:
            result_peg_607.setValue((void*)0);
        
        
        
        Result result_peg_615(result_peg_607);
            {
                
                char letter_peg_620 = stream.get(result_peg_615.getPosition());
                if (letter_peg_620 != '\0' && strchr("0123456789", letter_peg_620) != NULL){
                    result_peg_615.nextPosition();
                    result_peg_615.setValue((void*) letter_peg_620);
                } else {
                    goto out_peg_619;
                }
                
            }
            goto success_peg_616;
            out_peg_619:
            goto not_peg_614;
            success_peg_616:
            ;
            goto out_peg_609;
            not_peg_614:
            result_peg_607.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_607.setValue(value);
            }
        
        
    }
    
    if (column_peg_592.chunk9 == 0){
        column_peg_592.chunk9 = new Chunk9();
    }
    column_peg_592.chunk9->chunk_unary = result_peg_607;
    stream.update(result_peg_607.getPosition());
    
    
    return result_peg_607;
    out_peg_609:
    
    if (column_peg_592.chunk9 == 0){
        column_peg_592.chunk9 = new Chunk9();
    }
    column_peg_592.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_621 = stream.getColumn(position);
    if (column_peg_621.chunk9 != 0 && column_peg_621.chunk9->chunk_expr13_real.calculated()){
        return column_peg_621.chunk9->chunk_expr13_real;
    }
    
    int myposition = position;
    
    Value e;
    Result result_peg_622(myposition);
    
    result_peg_622 = rule_function(stream, result_peg_622.getPosition());
    if (result_peg_622.error()){
        goto out_peg_623;
    }
    
    if (column_peg_621.chunk9 == 0){
        column_peg_621.chunk9 = new Chunk9();
    }
    column_peg_621.chunk9->chunk_expr13_real = result_peg_622;
    stream.update(result_peg_622.getPosition());
    
    
    return result_peg_622;
    out_peg_623:
    Result result_peg_624(myposition);
    
    result_peg_624 = rule_value(stream, result_peg_624.getPosition());
    if (result_peg_624.error()){
        goto out_peg_625;
    }
    
    if (column_peg_621.chunk9 == 0){
        column_peg_621.chunk9 = new Chunk9();
    }
    column_peg_621.chunk9->chunk_expr13_real = result_peg_624;
    stream.update(result_peg_624.getPosition());
    
    
    return result_peg_624;
    out_peg_625:
    Result result_peg_626(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_626.getPosition()))){
                    result_peg_626.nextPosition();
                } else {
                    goto out_peg_628;
                }
            }
            result_peg_626.setValue((void*) "(");
        
        
        
        result_peg_626 = rule_s(stream, result_peg_626.getPosition());
            if (result_peg_626.error()){
                goto out_peg_628;
            }
        
        
        
        result_peg_626 = rule_expr(stream, result_peg_626.getPosition());
            if (result_peg_626.error()){
                goto out_peg_628;
            }
            e = result_peg_626.getValues();
        
        
        
        result_peg_626 = rule_s(stream, result_peg_626.getPosition());
            if (result_peg_626.error()){
                goto out_peg_628;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_626.getPosition()))){
                    result_peg_626.nextPosition();
                } else {
                    goto out_peg_628;
                }
            }
            result_peg_626.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_626.setValue(value);
            }
        
        
    }
    
    if (column_peg_621.chunk9 == 0){
        column_peg_621.chunk9 = new Chunk9();
    }
    column_peg_621.chunk9->chunk_expr13_real = result_peg_626;
    stream.update(result_peg_626.getPosition());
    
    
    return result_peg_626;
    out_peg_628:
    
    if (column_peg_621.chunk9 == 0){
        column_peg_621.chunk9 = new Chunk9();
    }
    column_peg_621.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_633 = stream.getColumn(position);
    if (column_peg_633.chunk9 != 0 && column_peg_633.chunk9->chunk_function.calculated()){
        return column_peg_633.chunk9->chunk_function;
    }
    
    int myposition = position;
    
    Value name;
    Value arg1;
    Value arg2;
    Value arg3;
    Result result_peg_634(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_634.getPosition()))){
                    result_peg_634.nextPosition();
                } else {
                    goto out_peg_636;
                }
            }
            result_peg_634.setValue((void*) "abs");
            name = result_peg_634.getValues();
        
        
        
        result_peg_634 = rule_s(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_634.getPosition()))){
                    result_peg_634.nextPosition();
                } else {
                    goto out_peg_636;
                }
            }
            result_peg_634.setValue((void*) "(");
        
        
        
        result_peg_634 = rule_s(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
        
        
        
        result_peg_634 = rule_expr(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
            arg1 = result_peg_634.getValues();
        
        
        
        result_peg_634 = rule_s(stream, result_peg_634.getPosition());
            if (result_peg_634.error()){
                goto out_peg_636;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_634.getPosition()))){
                    result_peg_634.nextPosition();
                } else {
                    goto out_peg_636;
                }
            }
            result_peg_634.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_634.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_634;
    stream.update(result_peg_634.getPosition());
    
    
    return result_peg_634;
    out_peg_636:
    Result result_peg_643(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_643.getPosition()))){
                    result_peg_643.nextPosition();
                } else {
                    goto out_peg_645;
                }
            }
            result_peg_643.setValue((void*) "const");
            name = result_peg_643.getValues();
        
        
        
        result_peg_643 = rule_s(stream, result_peg_643.getPosition());
            if (result_peg_643.error()){
                goto out_peg_645;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_643.getPosition()))){
                    result_peg_643.nextPosition();
                } else {
                    goto out_peg_645;
                }
            }
            result_peg_643.setValue((void*) "(");
        
        
        
        result_peg_643 = rule_s(stream, result_peg_643.getPosition());
            if (result_peg_643.error()){
                goto out_peg_645;
            }
        
        
        
        result_peg_643 = rule_expr(stream, result_peg_643.getPosition());
            if (result_peg_643.error()){
                goto out_peg_645;
            }
            arg1 = result_peg_643.getValues();
        
        
        
        result_peg_643 = rule_s(stream, result_peg_643.getPosition());
            if (result_peg_643.error()){
                goto out_peg_645;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_643.getPosition()))){
                    result_peg_643.nextPosition();
                } else {
                    goto out_peg_645;
                }
            }
            result_peg_643.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_643.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_643;
    stream.update(result_peg_643.getPosition());
    
    
    return result_peg_643;
    out_peg_645:
    Result result_peg_652(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_652.getPosition()))){
                    result_peg_652.nextPosition();
                } else {
                    goto out_peg_654;
                }
            }
            result_peg_652.setValue((void*) "selfanimexist");
            name = result_peg_652.getValues();
        
        
        
        result_peg_652 = rule_s(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_654;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_652.getPosition()))){
                    result_peg_652.nextPosition();
                } else {
                    goto out_peg_654;
                }
            }
            result_peg_652.setValue((void*) "(");
        
        
        
        result_peg_652 = rule_s(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_654;
            }
        
        
        
        result_peg_652 = rule_expr(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_654;
            }
            arg1 = result_peg_652.getValues();
        
        
        
        result_peg_652 = rule_s(stream, result_peg_652.getPosition());
            if (result_peg_652.error()){
                goto out_peg_654;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_652.getPosition()))){
                    result_peg_652.nextPosition();
                } else {
                    goto out_peg_654;
                }
            }
            result_peg_652.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_652.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_652;
    stream.update(result_peg_652.getPosition());
    
    
    return result_peg_652;
    out_peg_654:
    Result result_peg_661(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_661.getPosition()))){
                    result_peg_661.nextPosition();
                } else {
                    goto out_peg_663;
                }
            }
            result_peg_661.setValue((void*) "ifelse");
            name = result_peg_661.getValues();
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_661.getPosition()))){
                    result_peg_661.nextPosition();
                } else {
                    goto out_peg_663;
                }
            }
            result_peg_661.setValue((void*) "(");
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        result_peg_661 = rule_expr_c(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
            arg1 = result_peg_661.getValues();
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_661.getPosition()))){
                    result_peg_661.nextPosition();
                } else {
                    goto out_peg_663;
                }
            }
            result_peg_661.setValue((void*) ",");
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        result_peg_661 = rule_expr_c(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
            arg2 = result_peg_661.getValues();
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_661.getPosition()))){
                    result_peg_661.nextPosition();
                } else {
                    goto out_peg_663;
                }
            }
            result_peg_661.setValue((void*) ",");
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        result_peg_661 = rule_expr_c(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
            arg3 = result_peg_661.getValues();
        
        
        
        result_peg_661 = rule_s(stream, result_peg_661.getPosition());
            if (result_peg_661.error()){
                goto out_peg_663;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_661.getPosition()))){
                    result_peg_661.nextPosition();
                } else {
                    goto out_peg_663;
                }
            }
            result_peg_661.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction("ifelse", arg1, arg2, arg3);
                result_peg_661.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_661;
    stream.update(result_peg_661.getPosition());
    
    
    return result_peg_661;
    out_peg_663:
    Result result_peg_678(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_678.getPosition()))){
                    result_peg_678.nextPosition();
                } else {
                    goto out_peg_680;
                }
            }
            result_peg_678.setValue((void*) "gethitvar");
            name = result_peg_678.getValues();
        
        
        
        result_peg_678 = rule_s(stream, result_peg_678.getPosition());
            if (result_peg_678.error()){
                goto out_peg_680;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_678.getPosition()))){
                    result_peg_678.nextPosition();
                } else {
                    goto out_peg_680;
                }
            }
            result_peg_678.setValue((void*) "(");
        
        
        
        result_peg_678 = rule_s(stream, result_peg_678.getPosition());
            if (result_peg_678.error()){
                goto out_peg_680;
            }
        
        
        
        result_peg_678 = rule_expr_c(stream, result_peg_678.getPosition());
            if (result_peg_678.error()){
                goto out_peg_680;
            }
            arg1 = result_peg_678.getValues();
        
        
        
        result_peg_678 = rule_s(stream, result_peg_678.getPosition());
            if (result_peg_678.error()){
                goto out_peg_680;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_678.getPosition()))){
                    result_peg_678.nextPosition();
                } else {
                    goto out_peg_680;
                }
            }
            result_peg_678.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_678.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_678;
    stream.update(result_peg_678.getPosition());
    
    
    return result_peg_678;
    out_peg_680:
    Result result_peg_687(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_687.getPosition()))){
                    result_peg_687.nextPosition();
                } else {
                    goto out_peg_689;
                }
            }
            result_peg_687.setValue((void*) "floor");
            name = result_peg_687.getValues();
        
        
        
        result_peg_687 = rule_s(stream, result_peg_687.getPosition());
            if (result_peg_687.error()){
                goto out_peg_689;
            }
        
        
        
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
        
        
        
        result_peg_687 = rule_expr_c(stream, result_peg_687.getPosition());
            if (result_peg_687.error()){
                goto out_peg_689;
            }
            arg1 = result_peg_687.getValues();
        
        
        
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
                value = makeFunction(name, arg1);
                result_peg_687.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_687;
    stream.update(result_peg_687.getPosition());
    
    
    return result_peg_687;
    out_peg_689:
    Result result_peg_696(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_696.getPosition()))){
                    result_peg_696.nextPosition();
                } else {
                    goto out_peg_698;
                }
            }
            result_peg_696.setValue((void*) "ceil");
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
        
        
        
        result_peg_696 = rule_expr_c(stream, result_peg_696.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_696;
    stream.update(result_peg_696.getPosition());
    
    
    return result_peg_696;
    out_peg_698:
    Result result_peg_705(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("sin"[i], stream.get(result_peg_705.getPosition()))){
                    result_peg_705.nextPosition();
                } else {
                    goto out_peg_707;
                }
            }
            result_peg_705.setValue((void*) "sin");
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
        
        
        
        result_peg_705 = rule_expr_c(stream, result_peg_705.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_705;
    stream.update(result_peg_705.getPosition());
    
    
    return result_peg_705;
    out_peg_707:
    Result result_peg_714(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_714.getPosition()))){
                    result_peg_714.nextPosition();
                } else {
                    goto out_peg_716;
                }
            }
            result_peg_714.setValue((void*) "sysvar");
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
        
        
        
        result_peg_714 = rule_integer(stream, result_peg_714.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_714;
    stream.update(result_peg_714.getPosition());
    
    
    return result_peg_714;
    out_peg_716:
    Result result_peg_723(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_723.getPosition()))){
                    result_peg_723.nextPosition();
                } else {
                    goto out_peg_725;
                }
            }
            result_peg_723.setValue((void*) "var");
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
        
        
        
        result_peg_723 = rule_integer(stream, result_peg_723.getPosition());
            if (result_peg_723.error()){
                goto out_peg_725;
            }
            arg1 = result_peg_723.getValues();
        
        
        
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
                value = makeFunction(name, arg1);
                result_peg_723.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_723;
    stream.update(result_peg_723.getPosition());
    
    
    return result_peg_723;
    out_peg_725:
    Result result_peg_732(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_732.getPosition()))){
                    result_peg_732.nextPosition();
                } else {
                    goto out_peg_734;
                }
            }
            result_peg_732.setValue((void*) "numexplod");
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
        
        
        
        result_peg_732 = rule_integer(stream, result_peg_732.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_732;
    stream.update(result_peg_732.getPosition());
    
    
    return result_peg_732;
    out_peg_734:
    Result result_peg_741(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_741.getPosition()))){
                    result_peg_741.nextPosition();
                } else {
                    goto out_peg_743;
                }
            }
            result_peg_741.setValue((void*) "numhelper");
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
        
        
        
        result_peg_741 = rule_integer(stream, result_peg_741.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_741;
    stream.update(result_peg_741.getPosition());
    
    
    return result_peg_741;
    out_peg_743:
    Result result_peg_750(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_750.getPosition()))){
                    result_peg_750.nextPosition();
                } else {
                    goto out_peg_752;
                }
            }
            result_peg_750.setValue((void*) "numprojid");
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
        
        
        
        result_peg_750 = rule_integer(stream, result_peg_750.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_750;
    stream.update(result_peg_750.getPosition());
    
    
    return result_peg_750;
    out_peg_752:
    Result result_peg_759(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_759.getPosition()))){
                    result_peg_759.nextPosition();
                } else {
                    goto out_peg_761;
                }
            }
            result_peg_759.setValue((void*) "helper");
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
        
        
        
        result_peg_759 = rule_integer(stream, result_peg_759.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_759;
    stream.update(result_peg_759.getPosition());
    
    
    return result_peg_759;
    out_peg_761:
    Result result_peg_768(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_768.getPosition()))){
                    result_peg_768.nextPosition();
                } else {
                    goto out_peg_770;
                }
            }
            result_peg_768.setValue((void*) "fvar");
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_768;
    stream.update(result_peg_768.getPosition());
    
    
    return result_peg_768;
    out_peg_770:
    Result result_peg_777(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("enemynear"[i], stream.get(result_peg_777.getPosition()))){
                    result_peg_777.nextPosition();
                } else {
                    goto out_peg_779;
                }
            }
            result_peg_777.setValue((void*) "enemynear");
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_777;
    stream.update(result_peg_777.getPosition());
    
    
    return result_peg_777;
    out_peg_779:
    Result result_peg_786(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("target"[i], stream.get(result_peg_786.getPosition()))){
                    result_peg_786.nextPosition();
                } else {
                    goto out_peg_788;
                }
            }
            result_peg_786.setValue((void*) "target");
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_786;
    stream.update(result_peg_786.getPosition());
    
    
    return result_peg_786;
    out_peg_788:
    Result result_peg_795(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_795.getPosition()))){
                    result_peg_795.nextPosition();
                } else {
                    goto out_peg_797;
                }
            }
            result_peg_795.setValue((void*) "ishelper");
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_795;
    stream.update(result_peg_795.getPosition());
    
    
    return result_peg_795;
    out_peg_797:
    Result result_peg_804(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numtarget"[i], stream.get(result_peg_804.getPosition()))){
                    result_peg_804.nextPosition();
                } else {
                    goto out_peg_806;
                }
            }
            result_peg_804.setValue((void*) "numtarget");
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
        
        
        
        result_peg_804 = rule_expr_c(stream, result_peg_804.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_804;
    stream.update(result_peg_804.getPosition());
    
    
    return result_peg_804;
    out_peg_806:
    Result result_peg_813(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_813.getPosition()))){
                    result_peg_813.nextPosition();
                } else {
                    goto out_peg_815;
                }
            }
            result_peg_813.setValue((void*) "animelemtime");
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
        
        
        
        result_peg_813 = rule_expr_c(stream, result_peg_813.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_813;
    stream.update(result_peg_813.getPosition());
    
    
    return result_peg_813;
    out_peg_815:
    Result result_peg_822(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_822.getPosition()))){
                    result_peg_822.nextPosition();
                } else {
                    goto out_peg_824;
                }
            }
            result_peg_822.setValue((void*) "animelemno");
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
        
        
        
        result_peg_822 = rule_expr_c(stream, result_peg_822.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_822;
    stream.update(result_peg_822.getPosition());
    
    
    return result_peg_822;
    out_peg_824:
    Result result_peg_831(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_831.getPosition()))){
                    result_peg_831.nextPosition();
                } else {
                    goto out_peg_833;
                }
            }
            result_peg_831.setValue((void*) "animexist");
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
        
        
        
        result_peg_831 = rule_expr_c(stream, result_peg_831.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_831;
    stream.update(result_peg_831.getPosition());
    
    
    return result_peg_831;
    out_peg_833:
    Result result_peg_840(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_840.getPosition()))){
                    result_peg_840.nextPosition();
                } else {
                    goto out_peg_842;
                }
            }
            result_peg_840.setValue((void*) "projguarded");
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
        
        
        
        result_peg_840 = rule_expr_c(stream, result_peg_840.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_840;
    stream.update(result_peg_840.getPosition());
    
    
    return result_peg_840;
    out_peg_842:
    Result result_peg_849(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projhittime"[i], stream.get(result_peg_849.getPosition()))){
                    result_peg_849.nextPosition();
                } else {
                    goto out_peg_851;
                }
            }
            result_peg_849.setValue((void*) "projhittime");
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
        
        
        
        result_peg_849 = rule_expr_c(stream, result_peg_849.getPosition());
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_849;
    stream.update(result_peg_849.getPosition());
    
    
    return result_peg_849;
    out_peg_851:
    Result result_peg_858(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_858.getPosition()))){
                    result_peg_858.nextPosition();
                } else {
                    goto out_peg_860;
                }
            }
            result_peg_858.setValue((void*) "projcontact");
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
        
        
        
        result_peg_858 = rule_s(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_858.getPosition()))){
                    result_peg_858.nextPosition();
                } else {
                    goto out_peg_860;
                }
            }
            result_peg_858.setValue((void*) "=");
        
        
        
        result_peg_858 = rule_s(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        result_peg_858 = rule_integer(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        result_peg_858 = rule_function_rest(stream, result_peg_858.getPosition());
            if (result_peg_858.error()){
                goto out_peg_860;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_858.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_858;
    stream.update(result_peg_858.getPosition());
    
    
    return result_peg_858;
    out_peg_860:
    Result result_peg_872(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_872.getPosition()))){
                    result_peg_872.nextPosition();
                } else {
                    goto out_peg_874;
                }
            }
            result_peg_872.setValue((void*) "animelem");
            name = result_peg_872.getValues();
        
        
        
        result_peg_872 = rule_s(stream, result_peg_872.getPosition());
            if (result_peg_872.error()){
                goto out_peg_874;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_872.getPosition()))){
                    result_peg_872.nextPosition();
                } else {
                    goto out_peg_874;
                }
            }
            result_peg_872.setValue((void*) "=");
        
        
        
        result_peg_872 = rule_s(stream, result_peg_872.getPosition());
            if (result_peg_872.error()){
                goto out_peg_874;
            }
        
        
        
        result_peg_872 = rule_integer(stream, result_peg_872.getPosition());
            if (result_peg_872.error()){
                goto out_peg_874;
            }
            arg1 = result_peg_872.getValues();
        
        
        
        result_peg_872 = rule_function_rest(stream, result_peg_872.getPosition());
            if (result_peg_872.error()){
                goto out_peg_874;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction(name, arg1);
                result_peg_872.setValue(value);
            }
        
        
    }
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_872;
    stream.update(result_peg_872.getPosition());
    
    
    return result_peg_872;
    out_peg_874:
    Result result_peg_880(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_880.getPosition()))){
                    result_peg_880.nextPosition();
                } else {
                    goto out_peg_882;
                }
            }
            result_peg_880.setValue((void*) "numhelper");
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_880;
    stream.update(result_peg_880.getPosition());
    
    
    return result_peg_880;
    out_peg_882:
    Result result_peg_889(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_889.getPosition()))){
                    result_peg_889.nextPosition();
                } else {
                    goto out_peg_891;
                }
            }
            result_peg_889.setValue((void*) "helper");
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
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = result_peg_889;
    stream.update(result_peg_889.getPosition());
    
    
    return result_peg_889;
    out_peg_891:
    
    if (column_peg_633.chunk9 == 0){
        column_peg_633.chunk9 = new Chunk9();
    }
    column_peg_633.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_898 = stream.getColumn(position);
    if (column_peg_898.chunk10 != 0 && column_peg_898.chunk10->chunk_function_rest.calculated()){
        return column_peg_898.chunk10->chunk_function_rest;
    }
    
    int myposition = position;
    
    
    Result result_peg_899(myposition);
    
    {
    
        result_peg_899 = rule_s(stream, result_peg_899.getPosition());
            if (result_peg_899.error()){
                goto out_peg_901;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_899.getPosition()))){
                    result_peg_899.nextPosition();
                } else {
                    goto out_peg_901;
                }
            }
            result_peg_899.setValue((void*) ",");
        
        
        
        result_peg_899 = rule_s(stream, result_peg_899.getPosition());
            if (result_peg_899.error()){
                goto out_peg_901;
            }
        
        
        
        result_peg_899 = rule_all_compare(stream, result_peg_899.getPosition());
            if (result_peg_899.error()){
                goto out_peg_901;
            }
        
        
        
        result_peg_899 = rule_s(stream, result_peg_899.getPosition());
            if (result_peg_899.error()){
                goto out_peg_901;
            }
        
        
        
        result_peg_899 = rule_value(stream, result_peg_899.getPosition());
            if (result_peg_899.error()){
                goto out_peg_901;
            }
        
        
    }
    
    if (column_peg_898.chunk10 == 0){
        column_peg_898.chunk10 = new Chunk10();
    }
    column_peg_898.chunk10->chunk_function_rest = result_peg_899;
    stream.update(result_peg_899.getPosition());
    
    
    return result_peg_899;
    out_peg_901:
    Result result_peg_906(myposition);
    
    
    
    if (column_peg_898.chunk10 == 0){
        column_peg_898.chunk10 = new Chunk10();
    }
    column_peg_898.chunk10->chunk_function_rest = result_peg_906;
    stream.update(result_peg_906.getPosition());
    
    
    return result_peg_906;
    
    if (column_peg_898.chunk10 == 0){
        column_peg_898.chunk10 = new Chunk10();
    }
    column_peg_898.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_907 = stream.getColumn(position);
    if (column_peg_907.chunk10 != 0 && column_peg_907.chunk10->chunk_keys.calculated()){
        return column_peg_907.chunk10->chunk_keys;
    }
    
    int myposition = position;
    
    
    Result result_peg_908(myposition);
    
    result_peg_908 = rule_key_value_list(stream, result_peg_908.getPosition());
    if (result_peg_908.error()){
        goto out_peg_909;
    }
    
    if (column_peg_907.chunk10 == 0){
        column_peg_907.chunk10 = new Chunk10();
    }
    column_peg_907.chunk10->chunk_keys = result_peg_908;
    stream.update(result_peg_908.getPosition());
    
    
    return result_peg_908;
    out_peg_909:
    
    if (column_peg_907.chunk10 == 0){
        column_peg_907.chunk10 = new Chunk10();
    }
    column_peg_907.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_910 = stream.getColumn(position);
    if (column_peg_910.chunk10 != 0 && column_peg_910.chunk10->chunk_key_value_list.calculated()){
        return column_peg_910.chunk10->chunk_key_value_list;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_911(myposition);
    
    {
    
        result_peg_911 = rule_key(stream, result_peg_911.getPosition());
            if (result_peg_911.error()){
                goto out_peg_913;
            }
            first = result_peg_911.getValues();
        
        
        
        result_peg_911.reset();
            do{
                Result result_peg_916(result_peg_911.getPosition());
                {
                
                    result_peg_916 = rule_s(stream, result_peg_916.getPosition());
                        if (result_peg_916.error()){
                            goto loop_peg_915;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_916.getPosition()))){
                                result_peg_916.nextPosition();
                            } else {
                                goto loop_peg_915;
                            }
                        }
                        result_peg_916.setValue((void*) ",");
                    
                    
                    
                    result_peg_916 = rule_s(stream, result_peg_916.getPosition());
                        if (result_peg_916.error()){
                            goto loop_peg_915;
                        }
                    
                    
                    
                    result_peg_916 = rule_key(stream, result_peg_916.getPosition());
                        if (result_peg_916.error()){
                            goto loop_peg_915;
                        }
                    
                    
                }
                result_peg_911.addResult(result_peg_916);
            } while (true);
            loop_peg_915:
            ;
            rest = result_peg_911.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_911.setValue(value);
            }
        
        
    }
    
    if (column_peg_910.chunk10 == 0){
        column_peg_910.chunk10 = new Chunk10();
    }
    column_peg_910.chunk10->chunk_key_value_list = result_peg_911;
    stream.update(result_peg_911.getPosition());
    
    
    return result_peg_911;
    out_peg_913:
    
    if (column_peg_910.chunk10 == 0){
        column_peg_910.chunk10 = new Chunk10();
    }
    column_peg_910.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_920 = stream.getColumn(position);
    if (column_peg_920.chunk10 != 0 && column_peg_920.chunk10->chunk_key.calculated()){
        return column_peg_920.chunk10->chunk_key;
    }
    
    int myposition = position;
    
    Value ok;
    Result result_peg_921(myposition);
    
    {
    
        result_peg_921 = rule_key_real(stream, result_peg_921.getPosition());
            if (result_peg_921.error()){
                goto out_peg_923;
            }
        
        Result result_peg_922 = result_peg_921;
        
        result_peg_921 = rule_key_rest(stream, result_peg_921.getPosition(), result_peg_922.getValues());
            if (result_peg_921.error()){
                goto out_peg_923;
            }
            ok = result_peg_921.getValues();
        
        
        
        Result result_peg_927(result_peg_921);
            result_peg_927 = rule_identifier(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto not_peg_926;
            }
            goto out_peg_923;
            not_peg_926:
            result_peg_921.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_921.setValue(value);
            }
        
        
    }
    
    if (column_peg_920.chunk10 == 0){
        column_peg_920.chunk10 = new Chunk10();
    }
    column_peg_920.chunk10->chunk_key = result_peg_921;
    stream.update(result_peg_921.getPosition());
    
    
    return result_peg_921;
    out_peg_923:
    
    if (column_peg_920.chunk10 == 0){
        column_peg_920.chunk10 = new Chunk10();
    }
    column_peg_920.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_930:
    Value another;
    Value new_left;
    Result result_peg_929(myposition);
    {
    
        result_peg_929 = rule_s(stream, result_peg_929.getPosition());
            if (result_peg_929.error()){
                goto out_peg_932;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_929.getPosition()))){
                    result_peg_929.nextPosition();
                } else {
                    goto out_peg_932;
                }
            }
            result_peg_929.setValue((void*) "+");
        
        
        
        result_peg_929 = rule_key_real(stream, result_peg_929.getPosition());
            if (result_peg_929.error()){
                goto out_peg_932;
            }
            another = result_peg_929.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_929.setValue(value);
            }
            new_left = result_peg_929.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_929.getPosition();
    goto tail_peg_930;
    out_peg_932:
    Result result_peg_936(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_936.setValue(value);
            }
        
        
    }
    
    
    return result_peg_936;
    
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_938 = stream.getColumn(position);
    if (column_peg_938.chunk11 != 0 && column_peg_938.chunk11->chunk_key_real.calculated()){
        return column_peg_938.chunk11->chunk_key_real;
    }
    
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_939(myposition);
    
    {
    
        result_peg_939.reset();
            do{
                Result result_peg_942(result_peg_939.getPosition());
                result_peg_942 = rule_key_modifier(stream, result_peg_942.getPosition());
                if (result_peg_942.error()){
                    goto loop_peg_941;
                }
                result_peg_939.addResult(result_peg_942);
            } while (true);
            loop_peg_941:
            ;
            mods = result_peg_939.getValues();
        
        
        
        result_peg_939 = rule_key_name(stream, result_peg_939.getPosition());
            if (result_peg_939.error()){
                goto out_peg_944;
            }
            name = result_peg_939.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_939.setValue(value);
            }
        
        
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_key_real = result_peg_939;
    stream.update(result_peg_939.getPosition());
    
    
    return result_peg_939;
    out_peg_944:
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_945 = stream.getColumn(position);
    if (column_peg_945.chunk11 != 0 && column_peg_945.chunk11->chunk_key_modifier.calculated()){
        return column_peg_945.chunk11->chunk_key_modifier;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_946(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_946.getPosition()))){
                    result_peg_946.nextPosition();
                } else {
                    goto out_peg_948;
                }
            }
            result_peg_946.setValue((void*) "~");
        
        
        
        result_peg_946.reset();
            do{
                Result result_peg_951(result_peg_946.getPosition());
                {
                    
                    char letter_peg_956 = stream.get(result_peg_951.getPosition());
                    if (letter_peg_956 != '\0' && strchr("0123456789", letter_peg_956) != NULL){
                        result_peg_951.nextPosition();
                        result_peg_951.setValue((void*) letter_peg_956);
                    } else {
                        goto out_peg_955;
                    }
                    
                }
                goto success_peg_952;
                out_peg_955:
                goto loop_peg_950;
                success_peg_952:
                ;
                result_peg_946.addResult(result_peg_951);
            } while (true);
            loop_peg_950:
            ;
            num = result_peg_946.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier(*parseDouble(num));
                result_peg_946.setValue(value);
            }
        
        
    }
    
    if (column_peg_945.chunk11 == 0){
        column_peg_945.chunk11 = new Chunk11();
    }
    column_peg_945.chunk11->chunk_key_modifier = result_peg_946;
    stream.update(result_peg_946.getPosition());
    
    
    return result_peg_946;
    out_peg_948:
    Result result_peg_957(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_957.getPosition()))){
                    result_peg_957.nextPosition();
                } else {
                    goto out_peg_959;
                }
            }
            result_peg_957.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_957.setValue(value);
            }
        
        
    }
    
    if (column_peg_945.chunk11 == 0){
        column_peg_945.chunk11 = new Chunk11();
    }
    column_peg_945.chunk11->chunk_key_modifier = result_peg_957;
    stream.update(result_peg_957.getPosition());
    
    
    return result_peg_957;
    out_peg_959:
    Result result_peg_960(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_960.getPosition()))){
                    result_peg_960.nextPosition();
                } else {
                    goto out_peg_962;
                }
            }
            result_peg_960.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_960.setValue(value);
            }
        
        
    }
    
    if (column_peg_945.chunk11 == 0){
        column_peg_945.chunk11 = new Chunk11();
    }
    column_peg_945.chunk11->chunk_key_modifier = result_peg_960;
    stream.update(result_peg_960.getPosition());
    
    
    return result_peg_960;
    out_peg_962:
    Result result_peg_963(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_963.getPosition()))){
                    result_peg_963.nextPosition();
                } else {
                    goto out_peg_965;
                }
            }
            result_peg_963.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_963.setValue(value);
            }
        
        
    }
    
    if (column_peg_945.chunk11 == 0){
        column_peg_945.chunk11 = new Chunk11();
    }
    column_peg_945.chunk11->chunk_key_modifier = result_peg_963;
    stream.update(result_peg_963.getPosition());
    
    
    return result_peg_963;
    out_peg_965:
    
    if (column_peg_945.chunk11 == 0){
        column_peg_945.chunk11 = new Chunk11();
    }
    column_peg_945.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_966 = stream.getColumn(position);
    if (column_peg_966.chunk11 != 0 && column_peg_966.chunk11->chunk_key_name.calculated()){
        return column_peg_966.chunk11->chunk_key_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_967(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_967.getPosition()))){
            result_peg_967.nextPosition();
        } else {
            goto out_peg_968;
        }
    }
    result_peg_967.setValue((void*) "DB");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_967;
    stream.update(result_peg_967.getPosition());
    
    
    return result_peg_967;
    out_peg_968:
    Result result_peg_969(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_969.getPosition()))){
            result_peg_969.nextPosition();
        } else {
            goto out_peg_970;
        }
    }
    result_peg_969.setValue((void*) "B");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_969;
    stream.update(result_peg_969.getPosition());
    
    
    return result_peg_969;
    out_peg_970:
    Result result_peg_971(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_971.getPosition()))){
            result_peg_971.nextPosition();
        } else {
            goto out_peg_972;
        }
    }
    result_peg_971.setValue((void*) "DF");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_971;
    stream.update(result_peg_971.getPosition());
    
    
    return result_peg_971;
    out_peg_972:
    Result result_peg_973(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_973.getPosition()))){
            result_peg_973.nextPosition();
        } else {
            goto out_peg_974;
        }
    }
    result_peg_973.setValue((void*) "D");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_973;
    stream.update(result_peg_973.getPosition());
    
    
    return result_peg_973;
    out_peg_974:
    Result result_peg_975(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_975.getPosition()))){
            result_peg_975.nextPosition();
        } else {
            goto out_peg_976;
        }
    }
    result_peg_975.setValue((void*) "F");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_975;
    stream.update(result_peg_975.getPosition());
    
    
    return result_peg_975;
    out_peg_976:
    Result result_peg_977(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_977.getPosition()))){
            result_peg_977.nextPosition();
        } else {
            goto out_peg_978;
        }
    }
    result_peg_977.setValue((void*) "UF");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_977;
    stream.update(result_peg_977.getPosition());
    
    
    return result_peg_977;
    out_peg_978:
    Result result_peg_979(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_979.getPosition()))){
            result_peg_979.nextPosition();
        } else {
            goto out_peg_980;
        }
    }
    result_peg_979.setValue((void*) "UB");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_979;
    stream.update(result_peg_979.getPosition());
    
    
    return result_peg_979;
    out_peg_980:
    Result result_peg_981(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_981.getPosition()))){
            result_peg_981.nextPosition();
        } else {
            goto out_peg_982;
        }
    }
    result_peg_981.setValue((void*) "U");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_981;
    stream.update(result_peg_981.getPosition());
    
    
    return result_peg_981;
    out_peg_982:
    Result result_peg_983(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_983.getPosition()))){
            result_peg_983.nextPosition();
        } else {
            goto out_peg_984;
        }
    }
    result_peg_983.setValue((void*) "a");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_983;
    stream.update(result_peg_983.getPosition());
    
    
    return result_peg_983;
    out_peg_984:
    Result result_peg_985(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_985.getPosition()))){
            result_peg_985.nextPosition();
        } else {
            goto out_peg_986;
        }
    }
    result_peg_985.setValue((void*) "b");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_985;
    stream.update(result_peg_985.getPosition());
    
    
    return result_peg_985;
    out_peg_986:
    Result result_peg_987(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_987.getPosition()))){
            result_peg_987.nextPosition();
        } else {
            goto out_peg_988;
        }
    }
    result_peg_987.setValue((void*) "c");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_987;
    stream.update(result_peg_987.getPosition());
    
    
    return result_peg_987;
    out_peg_988:
    Result result_peg_989(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_989.getPosition()))){
            result_peg_989.nextPosition();
        } else {
            goto out_peg_990;
        }
    }
    result_peg_989.setValue((void*) "x");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_989;
    stream.update(result_peg_989.getPosition());
    
    
    return result_peg_989;
    out_peg_990:
    Result result_peg_991(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_991.getPosition()))){
            result_peg_991.nextPosition();
        } else {
            goto out_peg_992;
        }
    }
    result_peg_991.setValue((void*) "y");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_991;
    stream.update(result_peg_991.getPosition());
    
    
    return result_peg_991;
    out_peg_992:
    Result result_peg_993(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_993.getPosition()))){
            result_peg_993.nextPosition();
        } else {
            goto out_peg_994;
        }
    }
    result_peg_993.setValue((void*) "z");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_993;
    stream.update(result_peg_993.getPosition());
    
    
    return result_peg_993;
    out_peg_994:
    Result result_peg_995(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_995.getPosition()))){
            result_peg_995.nextPosition();
        } else {
            goto out_peg_996;
        }
    }
    result_peg_995.setValue((void*) "s");
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = result_peg_995;
    stream.update(result_peg_995.getPosition());
    
    
    return result_peg_995;
    out_peg_996:
    
    if (column_peg_966.chunk11 == 0){
        column_peg_966.chunk11 = new Chunk11();
    }
    column_peg_966.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_997 = stream.getColumn(position);
    if (column_peg_997.chunk11 != 0 && column_peg_997.chunk11->chunk_value.calculated()){
        return column_peg_997.chunk11->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_998(myposition);
    
    result_peg_998 = rule_float(stream, result_peg_998.getPosition());
    if (result_peg_998.error()){
        goto out_peg_999;
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_998;
    stream.update(result_peg_998.getPosition());
    
    
    return result_peg_998;
    out_peg_999:
    Result result_peg_1000(myposition);
    
    result_peg_1000 = rule_integer(stream, result_peg_1000.getPosition());
    if (result_peg_1000.error()){
        goto out_peg_1001;
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_1000;
    stream.update(result_peg_1000.getPosition());
    
    
    return result_peg_1000;
    out_peg_1001:
    Result result_peg_1002(myposition);
    
    result_peg_1002 = rule_keyword(stream, result_peg_1002.getPosition());
    if (result_peg_1002.error()){
        goto out_peg_1003;
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_1002;
    stream.update(result_peg_1002.getPosition());
    
    
    return result_peg_1002;
    out_peg_1003:
    Result result_peg_1004(myposition);
    
    {
    
        Result result_peg_1007(result_peg_1004);
            result_peg_1007 = rule_keyword(stream, result_peg_1007.getPosition());
            if (result_peg_1007.error()){
                goto not_peg_1006;
            }
            goto out_peg_1008;
            not_peg_1006:
            result_peg_1004.setValue((void*)0);
        
        
        
        result_peg_1004 = rule_identifier(stream, result_peg_1004.getPosition());
            if (result_peg_1004.error()){
                goto out_peg_1008;
            }
        
        
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_1004;
    stream.update(result_peg_1004.getPosition());
    
    
    return result_peg_1004;
    out_peg_1008:
    Result result_peg_1009(myposition);
    
    result_peg_1009 = rule_range(stream, result_peg_1009.getPosition());
    if (result_peg_1009.error()){
        goto out_peg_1010;
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_1009;
    stream.update(result_peg_1009.getPosition());
    
    
    return result_peg_1009;
    out_peg_1010:
    Result result_peg_1011(myposition);
    
    result_peg_1011 = rule_string(stream, result_peg_1011.getPosition());
    if (result_peg_1011.error()){
        goto out_peg_1012;
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_1011;
    stream.update(result_peg_1011.getPosition());
    
    
    return result_peg_1011;
    out_peg_1012:
    Result result_peg_1013(myposition);
    
    result_peg_1013 = rule_hitflag(stream, result_peg_1013.getPosition());
    if (result_peg_1013.error()){
        goto out_peg_1014;
    }
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = result_peg_1013;
    stream.update(result_peg_1013.getPosition());
    
    
    return result_peg_1013;
    out_peg_1014:
    
    if (column_peg_997.chunk11 == 0){
        column_peg_997.chunk11 = new Chunk11();
    }
    column_peg_997.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_1015 = stream.getColumn(position);
    if (column_peg_1015.chunk11 != 0 && column_peg_1015.chunk11->chunk_hitflag.calculated()){
        return column_peg_1015.chunk11->chunk_hitflag;
    }
    
    int myposition = position;
    
    
    Result result_peg_1016(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("M-"[i], stream.get(result_peg_1016.getPosition()))){
                    result_peg_1016.nextPosition();
                } else {
                    goto out_peg_1018;
                }
            }
            result_peg_1016.setValue((void*) "M-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("M-");
                result_peg_1016.setValue(value);
            }
        
        
    }
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_hitflag = result_peg_1016;
    stream.update(result_peg_1016.getPosition());
    
    
    return result_peg_1016;
    out_peg_1018:
    Result result_peg_1019(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("A-"[i], stream.get(result_peg_1019.getPosition()))){
                    result_peg_1019.nextPosition();
                } else {
                    goto out_peg_1021;
                }
            }
            result_peg_1019.setValue((void*) "A-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("A-");
                result_peg_1019.setValue(value);
            }
        
        
    }
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_hitflag = result_peg_1019;
    stream.update(result_peg_1019.getPosition());
    
    
    return result_peg_1019;
    out_peg_1021:
    
    if (column_peg_1015.chunk11 == 0){
        column_peg_1015.chunk11 = new Chunk11();
    }
    column_peg_1015.chunk11->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_1022 = stream.getColumn(position);
    if (column_peg_1022.chunk12 != 0 && column_peg_1022.chunk12->chunk_keyword.calculated()){
        return column_peg_1022.chunk12->chunk_keyword;
    }
    
    int myposition = position;
    
    
    Result result_peg_1023(myposition);
    
    {
    
        result_peg_1023 = rule_keyword_real(stream, result_peg_1023.getPosition());
            if (result_peg_1023.error()){
                goto out_peg_1025;
            }
        
        Result result_peg_1024 = result_peg_1023;
        
        Result result_peg_1028(result_peg_1023);
            result_peg_1028 = rule_alpha_digit(stream, result_peg_1028.getPosition());
            if (result_peg_1028.error()){
                goto not_peg_1027;
            }
            goto out_peg_1025;
            not_peg_1027:
            result_peg_1023.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_1024.getValues();
                result_peg_1023.setValue(value);
            }
        
        
    }
    
    if (column_peg_1022.chunk12 == 0){
        column_peg_1022.chunk12 = new Chunk12();
    }
    column_peg_1022.chunk12->chunk_keyword = result_peg_1023;
    stream.update(result_peg_1023.getPosition());
    
    
    return result_peg_1023;
    out_peg_1025:
    
    if (column_peg_1022.chunk12 == 0){
        column_peg_1022.chunk12 = new Chunk12();
    }
    column_peg_1022.chunk12->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_1029 = stream.getColumn(position);
    if (column_peg_1029.chunk12 != 0 && column_peg_1029.chunk12->chunk_keyword_real.calculated()){
        return column_peg_1029.chunk12->chunk_keyword_real;
    }
    
    int myposition = position;
    
    
    Result result_peg_1030(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1030.getPosition()))){
                    result_peg_1030.nextPosition();
                } else {
                    goto out_peg_1032;
                }
            }
            result_peg_1030.setValue((void*) "vel");
        
        
        
        result_peg_1030 = rule_s(stream, result_peg_1030.getPosition());
            if (result_peg_1030.error()){
                goto out_peg_1032;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1030.getPosition()))){
                    result_peg_1030.nextPosition();
                } else {
                    goto out_peg_1032;
                }
            }
            result_peg_1030.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_1030.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1030;
    stream.update(result_peg_1030.getPosition());
    
    
    return result_peg_1030;
    out_peg_1032:
    Result result_peg_1035(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_1035.getPosition()))){
                    result_peg_1035.nextPosition();
                } else {
                    goto out_peg_1037;
                }
            }
            result_peg_1035.setValue((void*) "vel");
        
        
        
        result_peg_1035 = rule_s(stream, result_peg_1035.getPosition());
            if (result_peg_1035.error()){
                goto out_peg_1037;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1035.getPosition()))){
                    result_peg_1035.nextPosition();
                } else {
                    goto out_peg_1037;
                }
            }
            result_peg_1035.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_1035.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1035;
    stream.update(result_peg_1035.getPosition());
    
    
    return result_peg_1035;
    out_peg_1037:
    Result result_peg_1040(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1040.getPosition()))){
                    result_peg_1040.nextPosition();
                } else {
                    goto out_peg_1042;
                }
            }
            result_peg_1040.setValue((void*) "pos");
        
        
        
        result_peg_1040 = rule_s(stream, result_peg_1040.getPosition());
            if (result_peg_1040.error()){
                goto out_peg_1042;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1040.getPosition()))){
                    result_peg_1040.nextPosition();
                } else {
                    goto out_peg_1042;
                }
            }
            result_peg_1040.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_1040.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1040;
    stream.update(result_peg_1040.getPosition());
    
    
    return result_peg_1040;
    out_peg_1042:
    Result result_peg_1045(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_1045.getPosition()))){
                    result_peg_1045.nextPosition();
                } else {
                    goto out_peg_1047;
                }
            }
            result_peg_1045.setValue((void*) "pos");
        
        
        
        result_peg_1045 = rule_s(stream, result_peg_1045.getPosition());
            if (result_peg_1045.error()){
                goto out_peg_1047;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1045.getPosition()))){
                    result_peg_1045.nextPosition();
                } else {
                    goto out_peg_1047;
                }
            }
            result_peg_1045.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_1045.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1045;
    stream.update(result_peg_1045.getPosition());
    
    
    return result_peg_1045;
    out_peg_1047:
    Result result_peg_1050(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1050.getPosition()))){
                    result_peg_1050.nextPosition();
                } else {
                    goto out_peg_1052;
                }
            }
            result_peg_1050.setValue((void*) "p2dist");
        
        
        
        result_peg_1050 = rule_s(stream, result_peg_1050.getPosition());
            if (result_peg_1050.error()){
                goto out_peg_1052;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1050.getPosition()))){
                    result_peg_1050.nextPosition();
                } else {
                    goto out_peg_1052;
                }
            }
            result_peg_1050.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_1050.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1050;
    stream.update(result_peg_1050.getPosition());
    
    
    return result_peg_1050;
    out_peg_1052:
    Result result_peg_1055(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1055.getPosition()))){
                    result_peg_1055.nextPosition();
                } else {
                    goto out_peg_1057;
                }
            }
            result_peg_1055.setValue((void*) "p2dist");
        
        
        
        result_peg_1055 = rule_s(stream, result_peg_1055.getPosition());
            if (result_peg_1055.error()){
                goto out_peg_1057;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1055.getPosition()))){
                    result_peg_1055.nextPosition();
                } else {
                    goto out_peg_1057;
                }
            }
            result_peg_1055.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_1055.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1055;
    stream.update(result_peg_1055.getPosition());
    
    
    return result_peg_1055;
    out_peg_1057:
    Result result_peg_1060(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1060.getPosition()))){
                    result_peg_1060.nextPosition();
                } else {
                    goto out_peg_1062;
                }
            }
            result_peg_1060.setValue((void*) "p1dist");
        
        
        
        result_peg_1060 = rule_s(stream, result_peg_1060.getPosition());
            if (result_peg_1060.error()){
                goto out_peg_1062;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1060.getPosition()))){
                    result_peg_1060.nextPosition();
                } else {
                    goto out_peg_1062;
                }
            }
            result_peg_1060.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_1060.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1060;
    stream.update(result_peg_1060.getPosition());
    
    
    return result_peg_1060;
    out_peg_1062:
    Result result_peg_1065(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1065.getPosition()))){
                    result_peg_1065.nextPosition();
                } else {
                    goto out_peg_1067;
                }
            }
            result_peg_1065.setValue((void*) "p1dist");
        
        
        
        result_peg_1065 = rule_s(stream, result_peg_1065.getPosition());
            if (result_peg_1065.error()){
                goto out_peg_1067;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1065.getPosition()))){
                    result_peg_1065.nextPosition();
                } else {
                    goto out_peg_1067;
                }
            }
            result_peg_1065.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_1065.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1065;
    stream.update(result_peg_1065.getPosition());
    
    
    return result_peg_1065;
    out_peg_1067:
    Result result_peg_1070(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1070.getPosition()))){
                    result_peg_1070.nextPosition();
                } else {
                    goto out_peg_1072;
                }
            }
            result_peg_1070.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1070 = rule_s(stream, result_peg_1070.getPosition());
            if (result_peg_1070.error()){
                goto out_peg_1072;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1070.getPosition()))){
                    result_peg_1070.nextPosition();
                } else {
                    goto out_peg_1072;
                }
            }
            result_peg_1070.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1070.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1070;
    stream.update(result_peg_1070.getPosition());
    
    
    return result_peg_1070;
    out_peg_1072:
    Result result_peg_1075(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1075.getPosition()))){
                    result_peg_1075.nextPosition();
                } else {
                    goto out_peg_1077;
                }
            }
            result_peg_1075.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1075 = rule_s(stream, result_peg_1075.getPosition());
            if (result_peg_1075.error()){
                goto out_peg_1077;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1075.getPosition()))){
                    result_peg_1075.nextPosition();
                } else {
                    goto out_peg_1077;
                }
            }
            result_peg_1075.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1075.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1075;
    stream.update(result_peg_1075.getPosition());
    
    
    return result_peg_1075;
    out_peg_1077:
    Result result_peg_1080(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1080.getPosition()))){
                    result_peg_1080.nextPosition();
                } else {
                    goto out_peg_1082;
                }
            }
            result_peg_1080.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1080 = rule_s(stream, result_peg_1080.getPosition());
            if (result_peg_1080.error()){
                goto out_peg_1082;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1080.getPosition()))){
                    result_peg_1080.nextPosition();
                } else {
                    goto out_peg_1082;
                }
            }
            result_peg_1080.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1080.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1080;
    stream.update(result_peg_1080.getPosition());
    
    
    return result_peg_1080;
    out_peg_1082:
    Result result_peg_1085(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1085.getPosition()))){
                    result_peg_1085.nextPosition();
                } else {
                    goto out_peg_1087;
                }
            }
            result_peg_1085.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1085 = rule_s(stream, result_peg_1085.getPosition());
            if (result_peg_1085.error()){
                goto out_peg_1087;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1085.getPosition()))){
                    result_peg_1085.nextPosition();
                } else {
                    goto out_peg_1087;
                }
            }
            result_peg_1085.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1085.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1085;
    stream.update(result_peg_1085.getPosition());
    
    
    return result_peg_1085;
    out_peg_1087:
    Result result_peg_1090(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1090.getPosition()))){
                    result_peg_1090.nextPosition();
                } else {
                    goto out_peg_1092;
                }
            }
            result_peg_1090.setValue((void*) "parentdist");
        
        
        
        result_peg_1090 = rule_s(stream, result_peg_1090.getPosition());
            if (result_peg_1090.error()){
                goto out_peg_1092;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1090.getPosition()))){
                    result_peg_1090.nextPosition();
                } else {
                    goto out_peg_1092;
                }
            }
            result_peg_1090.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1090.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1090;
    stream.update(result_peg_1090.getPosition());
    
    
    return result_peg_1090;
    out_peg_1092:
    Result result_peg_1095(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1095.getPosition()))){
                    result_peg_1095.nextPosition();
                } else {
                    goto out_peg_1097;
                }
            }
            result_peg_1095.setValue((void*) "screenpos");
        
        
        
        result_peg_1095 = rule_s(stream, result_peg_1095.getPosition());
            if (result_peg_1095.error()){
                goto out_peg_1097;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1095.getPosition()))){
                    result_peg_1095.nextPosition();
                } else {
                    goto out_peg_1097;
                }
            }
            result_peg_1095.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_1095.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1095;
    stream.update(result_peg_1095.getPosition());
    
    
    return result_peg_1095;
    out_peg_1097:
    Result result_peg_1100(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1100.getPosition()))){
                    result_peg_1100.nextPosition();
                } else {
                    goto out_peg_1102;
                }
            }
            result_peg_1100.setValue((void*) "screenpos");
        
        
        
        result_peg_1100 = rule_s(stream, result_peg_1100.getPosition());
            if (result_peg_1100.error()){
                goto out_peg_1102;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1100.getPosition()))){
                    result_peg_1100.nextPosition();
                } else {
                    goto out_peg_1102;
                }
            }
            result_peg_1100.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1100.setValue(value);
            }
        
        
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1100;
    stream.update(result_peg_1100.getPosition());
    
    
    return result_peg_1100;
    out_peg_1102:
    Result result_peg_1105(myposition);
    
    result_peg_1105 = rule_hitflag(stream, result_peg_1105.getPosition());
    if (result_peg_1105.error()){
        goto out_peg_1106;
    }
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = result_peg_1105;
    stream.update(result_peg_1105.getPosition());
    
    
    return result_peg_1105;
    out_peg_1106:
    
    if (column_peg_1029.chunk12 == 0){
        column_peg_1029.chunk12 = new Chunk12();
    }
    column_peg_1029.chunk12->chunk_keyword_real = errorResult;
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

        
