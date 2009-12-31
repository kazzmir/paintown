

#include "../ast/all.h"
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
    Result chunk_keyword;
};

struct Chunk12{
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
(chunk12 != NULL ? ((chunk12->chunk_keyword_real.calculated() ? 1 : 0)) : 0)
+
(chunk11 != NULL ? ((chunk11->chunk_key_real.calculated() ? 1 : 0)
+ (chunk11->chunk_key_modifier.calculated() ? 1 : 0)
+ (chunk11->chunk_key_name.calculated() ? 1 : 0)
+ (chunk11->chunk_value.calculated() ? 1 : 0)
+ (chunk11->chunk_keyword.calculated() ? 1 : 0)) : 0)
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
        return 61;
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


class Stream{
public:
    Stream(const std::string & filename):
    temp(0),
    buffer(0),
    farthest(0){
        std::ifstream stream;
        stream.open(filename.c_str());
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

Ast::Value * makeRange(Ast::Range::RangeType type, const Value & low, const Value & high){
    Ast::Value * range = new Ast::Range(type, as<Ast::Value*>(low), as<Ast::Value*>(high));
    GC::save(range);
    return range;
}

Ast::Identifier * makeIdentifier(const Value & front, const Value & rest){
    std::list<std::string*> ids;
    ids.push_back(as<std::string*>(front));
    for (Value::iterator it = rest.getValues().begin(); it != rest.getValues().end(); it++){
        ids.push_back(as<std::string*>((*it).getValue()));
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
    if (sign.getValue() != 0){
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
        
        
        
        result_peg_110 = rule_assignment(stream, result_peg_110.getPosition());
            if (result_peg_110.error()){
                goto out_peg_112;
            }
            data = result_peg_110.getValues();
        
        
        
        {
                Value value((void*) 0);
                as<Ast::Section*>(ast)->addAttribute(as<Ast::Attribute*>(data));
                result_peg_110.setValue(value);
            }
        
        
    }
    
    
    return result_peg_110;
    out_peg_112:
    
    
    return errorResult;
}
        

Result rule_section_title(Stream & stream, const int position){
    
    Column & column_peg_114 = stream.getColumn(position);
    if (column_peg_114.chunk1 != 0 && column_peg_114.chunk1->chunk_section_title.calculated()){
        return column_peg_114.chunk1->chunk_section_title;
    }
    
    int myposition = position;
    
    Value name;
    Result result_peg_115(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_115.getPosition()))){
                    result_peg_115.nextPosition();
                } else {
                    goto out_peg_117;
                }
            }
            result_peg_115.setValue((void*) "[");
        
        
        
        result_peg_115 = rule_s(stream, result_peg_115.getPosition());
            if (result_peg_115.error()){
                goto out_peg_117;
            }
        
        
        
        result_peg_115.reset();
            do{
                Result result_peg_121(result_peg_115.getPosition());
                {
                
                    Result result_peg_124(result_peg_121);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("]"[i], stream.get(result_peg_124.getPosition()))){
                                result_peg_124.nextPosition();
                            } else {
                                goto not_peg_123;
                            }
                        }
                        result_peg_124.setValue((void*) "]");
                        goto loop_peg_120;
                        not_peg_123:
                        result_peg_121.setValue((void*)0);
                    
                    
                    
                    char temp_peg_125 = stream.get(result_peg_121.getPosition());
                        if (temp_peg_125 != '\0'){
                            result_peg_121.setValue((void*) temp_peg_125);
                            result_peg_121.nextPosition();
                        } else {
                            goto loop_peg_120;
                        }
                    
                    
                }
                result_peg_115.addResult(result_peg_121);
            } while (true);
            loop_peg_120:
            if (result_peg_115.matches() == 0){
                goto out_peg_117;
            }
            name = result_peg_115.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_115.getPosition()))){
                    result_peg_115.nextPosition();
                } else {
                    goto out_peg_117;
                }
            }
            result_peg_115.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = toString(name);
                result_peg_115.setValue(value);
            }
        
        
    }
    
    if (column_peg_114.chunk1 == 0){
        column_peg_114.chunk1 = new Chunk1();
    }
    column_peg_114.chunk1->chunk_section_title = result_peg_115;
    stream.update(result_peg_115.getPosition());
    
    
    return result_peg_115;
    out_peg_117:
    
    if (column_peg_114.chunk1 == 0){
        column_peg_114.chunk1 = new Chunk1();
    }
    column_peg_114.chunk1->chunk_section_title = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_assignment(Stream & stream, const int position){
    
    Column & column_peg_127 = stream.getColumn(position);
    if (column_peg_127.chunk2 != 0 && column_peg_127.chunk2->chunk_assignment.calculated()){
        return column_peg_127.chunk2->chunk_assignment;
    }
    
    int myposition = position;
    
    Value all;
    Value name;
    Value exp;
    Value index;
    Result result_peg_128(myposition);
    
    {
    
        for (int i = 0; i < 7; i++){
                if (compareChar("command"[i], stream.get(result_peg_128.getPosition()))){
                    result_peg_128.nextPosition();
                } else {
                    goto out_peg_130;
                }
            }
            result_peg_128.setValue((void*) "command");
        
        
        
        result_peg_128 = rule_s(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_128.getPosition()))){
                    result_peg_128.nextPosition();
                } else {
                    goto out_peg_130;
                }
            }
            result_peg_128.setValue((void*) "=");
        
        
        
        result_peg_128 = rule_s(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
        
        
        
        result_peg_128 = rule_keys(stream, result_peg_128.getPosition());
            if (result_peg_128.error()){
                goto out_peg_130;
            }
            all = result_peg_128.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute("command", all);
                result_peg_128.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_assignment = result_peg_128;
    stream.update(result_peg_128.getPosition());
    
    
    return result_peg_128;
    out_peg_130:
    Result result_peg_135(myposition);
    
    {
    
        result_peg_135 = rule_identifier(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
            name = result_peg_135.getValues();
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_135.getPosition()))){
                    result_peg_135.nextPosition();
                } else {
                    goto out_peg_137;
                }
            }
            result_peg_135.setValue((void*) "=");
        
        
        
        result_peg_135 = rule_s(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
        
        
        
        result_peg_135 = rule_expr(stream, result_peg_135.getPosition());
            if (result_peg_135.error()){
                goto out_peg_137;
            }
            exp = result_peg_135.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
                result_peg_135.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_assignment = result_peg_135;
    stream.update(result_peg_135.getPosition());
    
    
    return result_peg_135;
    out_peg_137:
    Result result_peg_142(myposition);
    
    {
    
        result_peg_142 = rule_identifier(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
            name = result_peg_142.getValues();
        
        
        
        result_peg_142 = rule_s(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_142.getPosition()))){
                    result_peg_142.nextPosition();
                } else {
                    goto out_peg_144;
                }
            }
            result_peg_142.setValue((void*) "(");
        
        
        
        result_peg_142 = rule_s(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
        
        
        
        result_peg_142 = rule_integer(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
            index = result_peg_142.getValues();
        
        
        
        result_peg_142 = rule_s(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_142.getPosition()))){
                    result_peg_142.nextPosition();
                } else {
                    goto out_peg_144;
                }
            }
            result_peg_142.setValue((void*) ")");
        
        
        
        result_peg_142 = rule_s(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_142.getPosition()))){
                    result_peg_142.nextPosition();
                } else {
                    goto out_peg_144;
                }
            }
            result_peg_142.setValue((void*) "=");
        
        
        
        result_peg_142 = rule_s(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
        
        
        
        result_peg_142 = rule_expr(stream, result_peg_142.getPosition());
            if (result_peg_142.error()){
                goto out_peg_144;
            }
            exp = result_peg_142.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
                result_peg_142.setValue(value);
            }
        
        
    }
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_assignment = result_peg_142;
    stream.update(result_peg_142.getPosition());
    
    
    return result_peg_142;
    out_peg_144:
    
    if (column_peg_127.chunk2 == 0){
        column_peg_127.chunk2 = new Chunk2();
    }
    column_peg_127.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_155 = stream.getColumn(position);
    if (column_peg_155.chunk2 != 0 && column_peg_155.chunk2->chunk_identifier.calculated()){
        return column_peg_155.chunk2->chunk_identifier;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_156(myposition);
    
    {
    
        result_peg_156 = rule_name(stream, result_peg_156.getPosition());
            if (result_peg_156.error()){
                goto out_peg_158;
            }
            first = result_peg_156.getValues();
        
        
        
        result_peg_156.reset();
            do{
                Result result_peg_161(result_peg_156.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_161.getPosition()))){
                                result_peg_161.nextPosition();
                            } else {
                                goto loop_peg_160;
                            }
                        }
                        result_peg_161.setValue((void*) ".");
                    
                    
                    
                    result_peg_161 = rule_name(stream, result_peg_161.getPosition());
                        if (result_peg_161.error()){
                            goto loop_peg_160;
                        }
                    
                    
                }
                result_peg_156.addResult(result_peg_161);
            } while (true);
            loop_peg_160:
            ;
            rest = result_peg_156.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_156.setValue(value);
            }
        
        
    }
    
    if (column_peg_155.chunk2 == 0){
        column_peg_155.chunk2 = new Chunk2();
    }
    column_peg_155.chunk2->chunk_identifier = result_peg_156;
    stream.update(result_peg_156.getPosition());
    
    
    return result_peg_156;
    out_peg_158:
    
    if (column_peg_155.chunk2 == 0){
        column_peg_155.chunk2 = new Chunk2();
    }
    column_peg_155.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_163 = stream.getColumn(position);
    if (column_peg_163.chunk2 != 0 && column_peg_163.chunk2->chunk_integer.calculated()){
        return column_peg_163.chunk2->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_164(myposition);
    
    {
    
        int save_peg_166 = result_peg_164.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_164.getPosition()))){
                        result_peg_164.nextPosition();
                    } else {
                        goto out_peg_169;
                    }
                }
                result_peg_164.setValue((void*) "-");
                    
            }
            goto success_peg_167;
            out_peg_169:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_164.getPosition()))){
                        result_peg_164.nextPosition();
                    } else {
                        goto out_peg_171;
                    }
                }
                result_peg_164.setValue((void*) "+");
                    
            }
            goto success_peg_167;
            out_peg_171:
            
            result_peg_164 = Result(save_peg_166);
            result_peg_164.setValue((void*) 0);
            
            success_peg_167:
            ;
        
        Result result_peg_165 = result_peg_164;
        
        result_peg_164.reset();
            do{
                Result result_peg_174(result_peg_164.getPosition());
                {
                    
                    char letter_peg_179 = stream.get(result_peg_174.getPosition());
                    if (letter_peg_179 != '\0' && strchr("0123456789", letter_peg_179) != NULL){
                        result_peg_174.nextPosition();
                        result_peg_174.setValue((void*) letter_peg_179);
                    } else {
                        goto out_peg_178;
                    }
                    
                }
                goto success_peg_175;
                out_peg_178:
                goto loop_peg_173;
                success_peg_175:
                ;
                result_peg_164.addResult(result_peg_174);
            } while (true);
            loop_peg_173:
            if (result_peg_164.matches() == 0){
                goto out_peg_180;
            }
        
        Result result_peg_172 = result_peg_164;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_165.getValues(), result_peg_172.getValues());
                result_peg_164.setValue(value);
            }
        
        
    }
    
    if (column_peg_163.chunk2 == 0){
        column_peg_163.chunk2 = new Chunk2();
    }
    column_peg_163.chunk2->chunk_integer = result_peg_164;
    stream.update(result_peg_164.getPosition());
    
    
    return result_peg_164;
    out_peg_180:
    
    if (column_peg_163.chunk2 == 0){
        column_peg_163.chunk2 = new Chunk2();
    }
    column_peg_163.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_181 = stream.getColumn(position);
    if (column_peg_181.chunk2 != 0 && column_peg_181.chunk2->chunk_float.calculated()){
        return column_peg_181.chunk2->chunk_float;
    }
    
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_182(myposition);
    
    {
    
        int save_peg_184 = result_peg_182.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_182.getPosition()))){
                        result_peg_182.nextPosition();
                    } else {
                        goto out_peg_187;
                    }
                }
                result_peg_182.setValue((void*) "-");
                    
            }
            goto success_peg_185;
            out_peg_187:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_182.getPosition()))){
                        result_peg_182.nextPosition();
                    } else {
                        goto out_peg_189;
                    }
                }
                result_peg_182.setValue((void*) "+");
                    
            }
            goto success_peg_185;
            out_peg_189:
            
            result_peg_182 = Result(save_peg_184);
            result_peg_182.setValue((void*) 0);
            
            success_peg_185:
            ;
        
        Result result_peg_183 = result_peg_182;
        
        result_peg_182.reset();
            do{
                Result result_peg_192(result_peg_182.getPosition());
                {
                    
                    char letter_peg_197 = stream.get(result_peg_192.getPosition());
                    if (letter_peg_197 != '\0' && strchr("0123456789", letter_peg_197) != NULL){
                        result_peg_192.nextPosition();
                        result_peg_192.setValue((void*) letter_peg_197);
                    } else {
                        goto out_peg_196;
                    }
                    
                }
                goto success_peg_193;
                out_peg_196:
                goto loop_peg_191;
                success_peg_193:
                ;
                result_peg_182.addResult(result_peg_192);
            } while (true);
            loop_peg_191:
            ;
            left = result_peg_182.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_182.getPosition()))){
                    result_peg_182.nextPosition();
                } else {
                    goto out_peg_199;
                }
            }
            result_peg_182.setValue((void*) ".");
        
        
        
        result_peg_182.reset();
            do{
                Result result_peg_202(result_peg_182.getPosition());
                {
                    
                    char letter_peg_207 = stream.get(result_peg_202.getPosition());
                    if (letter_peg_207 != '\0' && strchr("0123456789", letter_peg_207) != NULL){
                        result_peg_202.nextPosition();
                        result_peg_202.setValue((void*) letter_peg_207);
                    } else {
                        goto out_peg_206;
                    }
                    
                }
                goto success_peg_203;
                out_peg_206:
                goto loop_peg_201;
                success_peg_203:
                ;
                result_peg_182.addResult(result_peg_202);
            } while (true);
            loop_peg_201:
            if (result_peg_182.matches() == 0){
                goto out_peg_199;
            }
            right = result_peg_182.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_183.getValues(), parseDouble(left,right));
                result_peg_182.setValue(value);
            }
        
        
    }
    
    if (column_peg_181.chunk2 == 0){
        column_peg_181.chunk2 = new Chunk2();
    }
    column_peg_181.chunk2->chunk_float = result_peg_182;
    stream.update(result_peg_182.getPosition());
    
    
    return result_peg_182;
    out_peg_199:
    Result result_peg_208(myposition);
    
    {
    
        int save_peg_210 = result_peg_208.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_208.getPosition()))){
                        result_peg_208.nextPosition();
                    } else {
                        goto out_peg_213;
                    }
                }
                result_peg_208.setValue((void*) "-");
                    
            }
            goto success_peg_211;
            out_peg_213:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_208.getPosition()))){
                        result_peg_208.nextPosition();
                    } else {
                        goto out_peg_215;
                    }
                }
                result_peg_208.setValue((void*) "+");
                    
            }
            goto success_peg_211;
            out_peg_215:
            
            result_peg_208 = Result(save_peg_210);
            result_peg_208.setValue((void*) 0);
            
            success_peg_211:
            ;
        
        Result result_peg_209 = result_peg_208;
        
        result_peg_208.reset();
            do{
                Result result_peg_218(result_peg_208.getPosition());
                {
                    
                    char letter_peg_223 = stream.get(result_peg_218.getPosition());
                    if (letter_peg_223 != '\0' && strchr("0123456789", letter_peg_223) != NULL){
                        result_peg_218.nextPosition();
                        result_peg_218.setValue((void*) letter_peg_223);
                    } else {
                        goto out_peg_222;
                    }
                    
                }
                goto success_peg_219;
                out_peg_222:
                goto loop_peg_217;
                success_peg_219:
                ;
                result_peg_208.addResult(result_peg_218);
            } while (true);
            loop_peg_217:
            if (result_peg_208.matches() == 0){
                goto out_peg_224;
            }
            left = result_peg_208.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_208.getPosition()))){
                    result_peg_208.nextPosition();
                } else {
                    goto out_peg_224;
                }
            }
            result_peg_208.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_209.getValues(), parseDouble(left));
                result_peg_208.setValue(value);
            }
        
        
    }
    
    if (column_peg_181.chunk2 == 0){
        column_peg_181.chunk2 = new Chunk2();
    }
    column_peg_181.chunk2->chunk_float = result_peg_208;
    stream.update(result_peg_208.getPosition());
    
    
    return result_peg_208;
    out_peg_224:
    
    if (column_peg_181.chunk2 == 0){
        column_peg_181.chunk2 = new Chunk2();
    }
    column_peg_181.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_226 = stream.getColumn(position);
    if (column_peg_226.chunk2 != 0 && column_peg_226.chunk2->chunk_string.calculated()){
        return column_peg_226.chunk2->chunk_string;
    }
    
    int myposition = position;
    
    Value contents;
    Result result_peg_227(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_227.getPosition()))){
                    result_peg_227.nextPosition();
                } else {
                    goto out_peg_229;
                }
            }
            result_peg_227.setValue((void*) "\"");
        
        
        
        result_peg_227.reset();
            do{
                Result result_peg_232(result_peg_227.getPosition());
                {
                
                    Result result_peg_235(result_peg_232);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_235.getPosition()))){
                                result_peg_235.nextPosition();
                            } else {
                                goto not_peg_234;
                            }
                        }
                        result_peg_235.setValue((void*) "\"");
                        goto loop_peg_231;
                        not_peg_234:
                        result_peg_232.setValue((void*)0);
                    
                    
                    
                    Result result_peg_238(result_peg_232);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_238.getPosition()))){
                                result_peg_238.nextPosition();
                            } else {
                                goto not_peg_237;
                            }
                        }
                        result_peg_238.setValue((void*) "\n");
                        goto loop_peg_231;
                        not_peg_237:
                        result_peg_232.setValue((void*)0);
                    
                    
                    
                    char temp_peg_239 = stream.get(result_peg_232.getPosition());
                        if (temp_peg_239 != '\0'){
                            result_peg_232.setValue((void*) temp_peg_239);
                            result_peg_232.nextPosition();
                        } else {
                            goto loop_peg_231;
                        }
                    
                    
                }
                result_peg_227.addResult(result_peg_232);
            } while (true);
            loop_peg_231:
            ;
            contents = result_peg_227.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_227.getPosition()))){
                    result_peg_227.nextPosition();
                } else {
                    goto out_peg_229;
                }
            }
            result_peg_227.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_227.setValue(value);
            }
        
        
    }
    
    if (column_peg_226.chunk2 == 0){
        column_peg_226.chunk2 = new Chunk2();
    }
    column_peg_226.chunk2->chunk_string = result_peg_227;
    stream.update(result_peg_227.getPosition());
    
    
    return result_peg_227;
    out_peg_229:
    
    if (column_peg_226.chunk2 == 0){
        column_peg_226.chunk2 = new Chunk2();
    }
    column_peg_226.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_241 = stream.getColumn(position);
    if (column_peg_241.chunk3 != 0 && column_peg_241.chunk3->chunk_range.calculated()){
        return column_peg_241.chunk3->chunk_range;
    }
    
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_242(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_242.getPosition()))){
                    result_peg_242.nextPosition();
                } else {
                    goto out_peg_244;
                }
            }
            result_peg_242.setValue((void*) "[");
        
        
        
        result_peg_242 = rule_s(stream, result_peg_242.getPosition());
            if (result_peg_242.error()){
                goto out_peg_244;
            }
        
        
        
        result_peg_242 = rule_value(stream, result_peg_242.getPosition());
            if (result_peg_242.error()){
                goto out_peg_244;
            }
            low = result_peg_242.getValues();
        
        
        
        result_peg_242 = rule_s(stream, result_peg_242.getPosition());
            if (result_peg_242.error()){
                goto out_peg_244;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_242.getPosition()))){
                    result_peg_242.nextPosition();
                } else {
                    goto out_peg_244;
                }
            }
            result_peg_242.setValue((void*) ",");
        
        
        
        result_peg_242 = rule_s(stream, result_peg_242.getPosition());
            if (result_peg_242.error()){
                goto out_peg_244;
            }
        
        
        
        result_peg_242 = rule_value(stream, result_peg_242.getPosition());
            if (result_peg_242.error()){
                goto out_peg_244;
            }
            high = result_peg_242.getValues();
        
        
        
        result_peg_242 = rule_s(stream, result_peg_242.getPosition());
            if (result_peg_242.error()){
                goto out_peg_244;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_242.getPosition()))){
                    result_peg_242.nextPosition();
                } else {
                    goto out_peg_244;
                }
            }
            result_peg_242.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_242.setValue(value);
            }
        
        
    }
    
    if (column_peg_241.chunk3 == 0){
        column_peg_241.chunk3 = new Chunk3();
    }
    column_peg_241.chunk3->chunk_range = result_peg_242;
    stream.update(result_peg_242.getPosition());
    
    
    return result_peg_242;
    out_peg_244:
    Result result_peg_253(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) "(");
        
        
        
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
                if (compareChar(")"[i], stream.get(result_peg_253.getPosition()))){
                    result_peg_253.nextPosition();
                } else {
                    goto out_peg_255;
                }
            }
            result_peg_253.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_253.setValue(value);
            }
        
        
    }
    
    if (column_peg_241.chunk3 == 0){
        column_peg_241.chunk3 = new Chunk3();
    }
    column_peg_241.chunk3->chunk_range = result_peg_253;
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
                if (compareChar("]"[i], stream.get(result_peg_264.getPosition()))){
                    result_peg_264.nextPosition();
                } else {
                    goto out_peg_266;
                }
            }
            result_peg_264.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_264.setValue(value);
            }
        
        
    }
    
    if (column_peg_241.chunk3 == 0){
        column_peg_241.chunk3 = new Chunk3();
    }
    column_peg_241.chunk3->chunk_range = result_peg_264;
    stream.update(result_peg_264.getPosition());
    
    
    return result_peg_264;
    out_peg_266:
    Result result_peg_275(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_275.getPosition()))){
                    result_peg_275.nextPosition();
                } else {
                    goto out_peg_277;
                }
            }
            result_peg_275.setValue((void*) "[");
        
        
        
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
                if (compareChar(")"[i], stream.get(result_peg_275.getPosition()))){
                    result_peg_275.nextPosition();
                } else {
                    goto out_peg_277;
                }
            }
            result_peg_275.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_275.setValue(value);
            }
        
        
    }
    
    if (column_peg_241.chunk3 == 0){
        column_peg_241.chunk3 = new Chunk3();
    }
    column_peg_241.chunk3->chunk_range = result_peg_275;
    stream.update(result_peg_275.getPosition());
    
    
    return result_peg_275;
    out_peg_277:
    
    if (column_peg_241.chunk3 == 0){
        column_peg_241.chunk3 = new Chunk3();
    }
    column_peg_241.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_286 = stream.getColumn(position);
    if (column_peg_286.chunk3 != 0 && column_peg_286.chunk3->chunk_name.calculated()){
        return column_peg_286.chunk3->chunk_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_287(myposition);
    
    {
    
        {
                
                char letter_peg_293 = stream.get(result_peg_287.getPosition());
                if (letter_peg_293 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_293) != NULL){
                    result_peg_287.nextPosition();
                    result_peg_287.setValue((void*) letter_peg_293);
                } else {
                    goto out_peg_292;
                }
                
            }
            goto success_peg_289;
            out_peg_292:
            goto out_peg_294;
            success_peg_289:
            ;
        
        Result result_peg_288 = result_peg_287;
        
        result_peg_287.reset();
            do{
                Result result_peg_297(result_peg_287.getPosition());
                result_peg_297 = rule_alpha_digit(stream, result_peg_297.getPosition());
                if (result_peg_297.error()){
                    goto loop_peg_296;
                }
                result_peg_287.addResult(result_peg_297);
            } while (true);
            loop_peg_296:
            ;
        
        Result result_peg_295 = result_peg_287;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_288.getValues().getValue(),result_peg_295.getValues());
                result_peg_287.setValue(value);
            }
        
        
    }
    
    if (column_peg_286.chunk3 == 0){
        column_peg_286.chunk3 = new Chunk3();
    }
    column_peg_286.chunk3->chunk_name = result_peg_287;
    stream.update(result_peg_287.getPosition());
    
    
    return result_peg_287;
    out_peg_294:
    
    if (column_peg_286.chunk3 == 0){
        column_peg_286.chunk3 = new Chunk3();
    }
    column_peg_286.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_298 = stream.getColumn(position);
    if (column_peg_298.chunk3 != 0 && column_peg_298.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_298.chunk3->chunk_alpha_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_299(myposition);
    
    {
        
        char letter_peg_304 = stream.get(result_peg_299.getPosition());
        if (letter_peg_304 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_304) != NULL){
            result_peg_299.nextPosition();
            result_peg_299.setValue((void*) letter_peg_304);
        } else {
            goto out_peg_303;
        }
        
    }
    goto success_peg_300;
    out_peg_303:
    goto out_peg_305;
    success_peg_300:
    ;
    
    if (column_peg_298.chunk3 == 0){
        column_peg_298.chunk3 = new Chunk3();
    }
    column_peg_298.chunk3->chunk_alpha_digit = result_peg_299;
    stream.update(result_peg_299.getPosition());
    
    
    return result_peg_299;
    out_peg_305:
    Result result_peg_306(myposition);
    
    {
        
        char letter_peg_311 = stream.get(result_peg_306.getPosition());
        if (letter_peg_311 != '\0' && strchr("0123456789", letter_peg_311) != NULL){
            result_peg_306.nextPosition();
            result_peg_306.setValue((void*) letter_peg_311);
        } else {
            goto out_peg_310;
        }
        
    }
    goto success_peg_307;
    out_peg_310:
    goto out_peg_312;
    success_peg_307:
    ;
    
    if (column_peg_298.chunk3 == 0){
        column_peg_298.chunk3 = new Chunk3();
    }
    column_peg_298.chunk3->chunk_alpha_digit = result_peg_306;
    stream.update(result_peg_306.getPosition());
    
    
    return result_peg_306;
    out_peg_312:
    
    if (column_peg_298.chunk3 == 0){
        column_peg_298.chunk3 = new Chunk3();
    }
    column_peg_298.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_313 = stream.getColumn(position);
    if (column_peg_313.chunk3 != 0 && column_peg_313.chunk3->chunk_valuelist.calculated()){
        return column_peg_313.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_314(myposition);
    
    {
    
        result_peg_314 = rule_expr_c(stream, result_peg_314.getPosition());
            if (result_peg_314.error()){
                goto out_peg_316;
            }
            first = result_peg_314.getValues();
        
        
        
        result_peg_314.reset();
            do{
                Result result_peg_319(result_peg_314.getPosition());
                {
                
                    result_peg_319 = rule_s(stream, result_peg_319.getPosition());
                        if (result_peg_319.error()){
                            goto loop_peg_318;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_319.getPosition()))){
                                result_peg_319.nextPosition();
                            } else {
                                goto loop_peg_318;
                            }
                        }
                        result_peg_319.setValue((void*) ",");
                    
                    
                    
                    result_peg_319 = rule_s(stream, result_peg_319.getPosition());
                        if (result_peg_319.error()){
                            goto loop_peg_318;
                        }
                    
                    
                    
                    result_peg_319 = rule_expr_c(stream, result_peg_319.getPosition());
                        if (result_peg_319.error()){
                            goto loop_peg_318;
                        }
                    
                    
                }
                result_peg_314.addResult(result_peg_319);
            } while (true);
            loop_peg_318:
            if (result_peg_314.matches() == 0){
                goto out_peg_316;
            }
            rest = result_peg_314.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_314.setValue(value);
            }
        
        
    }
    
    if (column_peg_313.chunk3 == 0){
        column_peg_313.chunk3 = new Chunk3();
    }
    column_peg_313.chunk3->chunk_valuelist = result_peg_314;
    stream.update(result_peg_314.getPosition());
    
    
    return result_peg_314;
    out_peg_316:
    Result result_peg_323(myposition);
    
    {
    
        result_peg_323 = rule_s(stream, result_peg_323.getPosition());
            if (result_peg_323.error()){
                goto out_peg_325;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_323.getPosition()))){
                    result_peg_323.nextPosition();
                } else {
                    goto out_peg_325;
                }
            }
            result_peg_323.setValue((void*) ",");
        
        
        
        result_peg_323 = rule_s(stream, result_peg_323.getPosition());
            if (result_peg_323.error()){
                goto out_peg_325;
            }
        
        
        
        result_peg_323 = rule_expr_c(stream, result_peg_323.getPosition());
            if (result_peg_323.error()){
                goto out_peg_325;
            }
            first = result_peg_323.getValues();
        
        
        
        result_peg_323.reset();
            do{
                Result result_peg_331(result_peg_323.getPosition());
                {
                
                    result_peg_331 = rule_s(stream, result_peg_331.getPosition());
                        if (result_peg_331.error()){
                            goto loop_peg_330;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_331.getPosition()))){
                                result_peg_331.nextPosition();
                            } else {
                                goto loop_peg_330;
                            }
                        }
                        result_peg_331.setValue((void*) ",");
                    
                    
                    
                    result_peg_331 = rule_s(stream, result_peg_331.getPosition());
                        if (result_peg_331.error()){
                            goto loop_peg_330;
                        }
                    
                    
                    
                    result_peg_331 = rule_expr_c(stream, result_peg_331.getPosition());
                        if (result_peg_331.error()){
                            goto loop_peg_330;
                        }
                    
                    
                }
                result_peg_323.addResult(result_peg_331);
            } while (true);
            loop_peg_330:
            ;
            rest = result_peg_323.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_323.setValue(value);
            }
        
        
    }
    
    if (column_peg_313.chunk3 == 0){
        column_peg_313.chunk3 = new Chunk3();
    }
    column_peg_313.chunk3->chunk_valuelist = result_peg_323;
    stream.update(result_peg_323.getPosition());
    
    
    return result_peg_323;
    out_peg_325:
    
    if (column_peg_313.chunk3 == 0){
        column_peg_313.chunk3 = new Chunk3();
    }
    column_peg_313.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_335 = stream.getColumn(position);
    if (column_peg_335.chunk3 != 0 && column_peg_335.chunk3->chunk_expr.calculated()){
        return column_peg_335.chunk3->chunk_expr;
    }
    
    int myposition = position;
    
    
    Result result_peg_336(myposition);
    
    {
    
        result_peg_336 = rule_expr_c(stream, result_peg_336.getPosition());
            if (result_peg_336.error()){
                goto out_peg_338;
            }
        
        Result result_peg_337 = result_peg_336;
        
        Result result_peg_341(result_peg_336);
            {
            
                result_peg_341 = rule_s(stream, result_peg_341.getPosition());
                    if (result_peg_341.error()){
                        goto not_peg_340;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_341.getPosition()))){
                            result_peg_341.nextPosition();
                        } else {
                            goto not_peg_340;
                        }
                    }
                    result_peg_341.setValue((void*) ",");
                
                
            }
            goto out_peg_338;
            not_peg_340:
            result_peg_336.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_337.getValues();
                result_peg_336.setValue(value);
            }
        
        
    }
    
    if (column_peg_335.chunk3 == 0){
        column_peg_335.chunk3 = new Chunk3();
    }
    column_peg_335.chunk3->chunk_expr = result_peg_336;
    stream.update(result_peg_336.getPosition());
    
    
    return result_peg_336;
    out_peg_338:
    Result result_peg_343(myposition);
    
    {
    
        result_peg_343 = rule_valuelist(stream, result_peg_343.getPosition());
            if (result_peg_343.error()){
                goto out_peg_345;
            }
        
        Result result_peg_344 = result_peg_343;
        
        result_peg_343 = rule_expr2_rest(stream, result_peg_343.getPosition(), result_peg_344.getValues());
            if (result_peg_343.error()){
                goto out_peg_345;
            }
        
        
    }
    
    if (column_peg_335.chunk3 == 0){
        column_peg_335.chunk3 = new Chunk3();
    }
    column_peg_335.chunk3->chunk_expr = result_peg_343;
    stream.update(result_peg_343.getPosition());
    
    
    return result_peg_343;
    out_peg_345:
    
    if (column_peg_335.chunk3 == 0){
        column_peg_335.chunk3 = new Chunk3();
    }
    column_peg_335.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_346 = stream.getColumn(position);
    if (column_peg_346.chunk4 != 0 && column_peg_346.chunk4->chunk_expr_c.calculated()){
        return column_peg_346.chunk4->chunk_expr_c;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_347(myposition);
    
    {
    
        result_peg_347 = rule_expr2(stream, result_peg_347.getPosition());
            if (result_peg_347.error()){
                goto out_peg_349;
            }
            left = result_peg_347.getValues();
        
        
        
        result_peg_347 = rule_expr_rest(stream, result_peg_347.getPosition(), left);
            if (result_peg_347.error()){
                goto out_peg_349;
            }
        
        
    }
    
    if (column_peg_346.chunk4 == 0){
        column_peg_346.chunk4 = new Chunk4();
    }
    column_peg_346.chunk4->chunk_expr_c = result_peg_347;
    stream.update(result_peg_347.getPosition());
    
    
    return result_peg_347;
    out_peg_349:
    
    if (column_peg_346.chunk4 == 0){
        column_peg_346.chunk4 = new Chunk4();
    }
    column_peg_346.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_350 = stream.getColumn(position);
    if (column_peg_350.chunk4 != 0 && column_peg_350.chunk4->chunk_expr2.calculated()){
        return column_peg_350.chunk4->chunk_expr2;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_351(myposition);
    
    {
    
        result_peg_351 = rule_expr3(stream, result_peg_351.getPosition());
            if (result_peg_351.error()){
                goto out_peg_353;
            }
            left = result_peg_351.getValues();
        
        
        
        result_peg_351 = rule_expr2_rest(stream, result_peg_351.getPosition(), left);
            if (result_peg_351.error()){
                goto out_peg_353;
            }
        
        
    }
    
    if (column_peg_350.chunk4 == 0){
        column_peg_350.chunk4 = new Chunk4();
    }
    column_peg_350.chunk4->chunk_expr2 = result_peg_351;
    stream.update(result_peg_351.getPosition());
    
    
    return result_peg_351;
    out_peg_353:
    
    if (column_peg_350.chunk4 == 0){
        column_peg_350.chunk4 = new Chunk4();
    }
    column_peg_350.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_354 = stream.getColumn(position);
    if (column_peg_354.chunk4 != 0 && column_peg_354.chunk4->chunk_expr3.calculated()){
        return column_peg_354.chunk4->chunk_expr3;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_355(myposition);
    
    {
    
        result_peg_355 = rule_expr4(stream, result_peg_355.getPosition());
            if (result_peg_355.error()){
                goto out_peg_357;
            }
            left = result_peg_355.getValues();
        
        
        
        result_peg_355 = rule_expr3_rest(stream, result_peg_355.getPosition(), left);
            if (result_peg_355.error()){
                goto out_peg_357;
            }
        
        
    }
    
    if (column_peg_354.chunk4 == 0){
        column_peg_354.chunk4 = new Chunk4();
    }
    column_peg_354.chunk4->chunk_expr3 = result_peg_355;
    stream.update(result_peg_355.getPosition());
    
    
    return result_peg_355;
    out_peg_357:
    
    if (column_peg_354.chunk4 == 0){
        column_peg_354.chunk4 = new Chunk4();
    }
    column_peg_354.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_358 = stream.getColumn(position);
    if (column_peg_358.chunk4 != 0 && column_peg_358.chunk4->chunk_expr4.calculated()){
        return column_peg_358.chunk4->chunk_expr4;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_359(myposition);
    
    {
    
        result_peg_359 = rule_expr5(stream, result_peg_359.getPosition());
            if (result_peg_359.error()){
                goto out_peg_361;
            }
            left = result_peg_359.getValues();
        
        
        
        result_peg_359 = rule_expr4_rest(stream, result_peg_359.getPosition(), left);
            if (result_peg_359.error()){
                goto out_peg_361;
            }
        
        
    }
    
    if (column_peg_358.chunk4 == 0){
        column_peg_358.chunk4 = new Chunk4();
    }
    column_peg_358.chunk4->chunk_expr4 = result_peg_359;
    stream.update(result_peg_359.getPosition());
    
    
    return result_peg_359;
    out_peg_361:
    
    if (column_peg_358.chunk4 == 0){
        column_peg_358.chunk4 = new Chunk4();
    }
    column_peg_358.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_362 = stream.getColumn(position);
    if (column_peg_362.chunk4 != 0 && column_peg_362.chunk4->chunk_expr5.calculated()){
        return column_peg_362.chunk4->chunk_expr5;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_363(myposition);
    
    {
    
        result_peg_363 = rule_expr6(stream, result_peg_363.getPosition());
            if (result_peg_363.error()){
                goto out_peg_365;
            }
            left = result_peg_363.getValues();
        
        
        
        result_peg_363 = rule_expr5_rest(stream, result_peg_363.getPosition(), left);
            if (result_peg_363.error()){
                goto out_peg_365;
            }
        
        
    }
    
    if (column_peg_362.chunk4 == 0){
        column_peg_362.chunk4 = new Chunk4();
    }
    column_peg_362.chunk4->chunk_expr5 = result_peg_363;
    stream.update(result_peg_363.getPosition());
    
    
    return result_peg_363;
    out_peg_365:
    
    if (column_peg_362.chunk4 == 0){
        column_peg_362.chunk4 = new Chunk4();
    }
    column_peg_362.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_366 = stream.getColumn(position);
    if (column_peg_366.chunk5 != 0 && column_peg_366.chunk5->chunk_expr6.calculated()){
        return column_peg_366.chunk5->chunk_expr6;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_367(myposition);
    
    {
    
        result_peg_367 = rule_expr7(stream, result_peg_367.getPosition());
            if (result_peg_367.error()){
                goto out_peg_369;
            }
            left = result_peg_367.getValues();
        
        
        
        result_peg_367 = rule_expr6_rest(stream, result_peg_367.getPosition(), left);
            if (result_peg_367.error()){
                goto out_peg_369;
            }
        
        
    }
    
    if (column_peg_366.chunk5 == 0){
        column_peg_366.chunk5 = new Chunk5();
    }
    column_peg_366.chunk5->chunk_expr6 = result_peg_367;
    stream.update(result_peg_367.getPosition());
    
    
    return result_peg_367;
    out_peg_369:
    
    if (column_peg_366.chunk5 == 0){
        column_peg_366.chunk5 = new Chunk5();
    }
    column_peg_366.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_370 = stream.getColumn(position);
    if (column_peg_370.chunk5 != 0 && column_peg_370.chunk5->chunk_expr7.calculated()){
        return column_peg_370.chunk5->chunk_expr7;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_371(myposition);
    
    {
    
        result_peg_371 = rule_expr8(stream, result_peg_371.getPosition());
            if (result_peg_371.error()){
                goto out_peg_373;
            }
            left = result_peg_371.getValues();
        
        
        
        result_peg_371 = rule_expr7_rest(stream, result_peg_371.getPosition(), left);
            if (result_peg_371.error()){
                goto out_peg_373;
            }
        
        
    }
    
    if (column_peg_370.chunk5 == 0){
        column_peg_370.chunk5 = new Chunk5();
    }
    column_peg_370.chunk5->chunk_expr7 = result_peg_371;
    stream.update(result_peg_371.getPosition());
    
    
    return result_peg_371;
    out_peg_373:
    
    if (column_peg_370.chunk5 == 0){
        column_peg_370.chunk5 = new Chunk5();
    }
    column_peg_370.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_374 = stream.getColumn(position);
    if (column_peg_374.chunk5 != 0 && column_peg_374.chunk5->chunk_expr8.calculated()){
        return column_peg_374.chunk5->chunk_expr8;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_375(myposition);
    
    {
    
        result_peg_375 = rule_expr9(stream, result_peg_375.getPosition());
            if (result_peg_375.error()){
                goto out_peg_377;
            }
            left = result_peg_375.getValues();
        
        
        
        result_peg_375 = rule_expr8_rest(stream, result_peg_375.getPosition(), left);
            if (result_peg_375.error()){
                goto out_peg_377;
            }
        
        
    }
    
    if (column_peg_374.chunk5 == 0){
        column_peg_374.chunk5 = new Chunk5();
    }
    column_peg_374.chunk5->chunk_expr8 = result_peg_375;
    stream.update(result_peg_375.getPosition());
    
    
    return result_peg_375;
    out_peg_377:
    
    if (column_peg_374.chunk5 == 0){
        column_peg_374.chunk5 = new Chunk5();
    }
    column_peg_374.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_378 = stream.getColumn(position);
    if (column_peg_378.chunk5 != 0 && column_peg_378.chunk5->chunk_expr9.calculated()){
        return column_peg_378.chunk5->chunk_expr9;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_379(myposition);
    
    {
    
        result_peg_379 = rule_expr10(stream, result_peg_379.getPosition());
            if (result_peg_379.error()){
                goto out_peg_381;
            }
            left = result_peg_379.getValues();
        
        
        
        result_peg_379 = rule_expr9_rest(stream, result_peg_379.getPosition(), left);
            if (result_peg_379.error()){
                goto out_peg_381;
            }
        
        
    }
    
    if (column_peg_378.chunk5 == 0){
        column_peg_378.chunk5 = new Chunk5();
    }
    column_peg_378.chunk5->chunk_expr9 = result_peg_379;
    stream.update(result_peg_379.getPosition());
    
    
    return result_peg_379;
    out_peg_381:
    
    if (column_peg_378.chunk5 == 0){
        column_peg_378.chunk5 = new Chunk5();
    }
    column_peg_378.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_382 = stream.getColumn(position);
    if (column_peg_382.chunk5 != 0 && column_peg_382.chunk5->chunk_expr10.calculated()){
        return column_peg_382.chunk5->chunk_expr10;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_383(myposition);
    
    {
    
        result_peg_383 = rule_expr11(stream, result_peg_383.getPosition());
            if (result_peg_383.error()){
                goto out_peg_385;
            }
            left = result_peg_383.getValues();
        
        
        
        result_peg_383 = rule_expr10_rest(stream, result_peg_383.getPosition(), left);
            if (result_peg_383.error()){
                goto out_peg_385;
            }
        
        
    }
    
    if (column_peg_382.chunk5 == 0){
        column_peg_382.chunk5 = new Chunk5();
    }
    column_peg_382.chunk5->chunk_expr10 = result_peg_383;
    stream.update(result_peg_383.getPosition());
    
    
    return result_peg_383;
    out_peg_385:
    
    if (column_peg_382.chunk5 == 0){
        column_peg_382.chunk5 = new Chunk5();
    }
    column_peg_382.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_386 = stream.getColumn(position);
    if (column_peg_386.chunk6 != 0 && column_peg_386.chunk6->chunk_expr11.calculated()){
        return column_peg_386.chunk6->chunk_expr11;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_387(myposition);
    
    {
    
        result_peg_387 = rule_expr12(stream, result_peg_387.getPosition());
            if (result_peg_387.error()){
                goto out_peg_389;
            }
            left = result_peg_387.getValues();
        
        
        
        result_peg_387 = rule_expr11_rest(stream, result_peg_387.getPosition(), left);
            if (result_peg_387.error()){
                goto out_peg_389;
            }
        
        
    }
    
    if (column_peg_386.chunk6 == 0){
        column_peg_386.chunk6 = new Chunk6();
    }
    column_peg_386.chunk6->chunk_expr11 = result_peg_387;
    stream.update(result_peg_387.getPosition());
    
    
    return result_peg_387;
    out_peg_389:
    
    if (column_peg_386.chunk6 == 0){
        column_peg_386.chunk6 = new Chunk6();
    }
    column_peg_386.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_390 = stream.getColumn(position);
    if (column_peg_390.chunk6 != 0 && column_peg_390.chunk6->chunk_expr12.calculated()){
        return column_peg_390.chunk6->chunk_expr12;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_391(myposition);
    
    {
    
        result_peg_391 = rule_expr13(stream, result_peg_391.getPosition());
            if (result_peg_391.error()){
                goto out_peg_393;
            }
            left = result_peg_391.getValues();
        
        
        
        result_peg_391 = rule_expr12_rest(stream, result_peg_391.getPosition(), left);
            if (result_peg_391.error()){
                goto out_peg_393;
            }
        
        
    }
    
    if (column_peg_390.chunk6 == 0){
        column_peg_390.chunk6 = new Chunk6();
    }
    column_peg_390.chunk6->chunk_expr12 = result_peg_391;
    stream.update(result_peg_391.getPosition());
    
    
    return result_peg_391;
    out_peg_393:
    
    if (column_peg_390.chunk6 == 0){
        column_peg_390.chunk6 = new Chunk6();
    }
    column_peg_390.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_394 = stream.getColumn(position);
    if (column_peg_394.chunk6 != 0 && column_peg_394.chunk6->chunk_expr13.calculated()){
        return column_peg_394.chunk6->chunk_expr13;
    }
    
    int myposition = position;
    
    Value exp;
    Result result_peg_395(myposition);
    
    {
    
        result_peg_395.reset();
            do{
                Result result_peg_398(result_peg_395.getPosition());
                result_peg_398 = rule_unary(stream, result_peg_398.getPosition());
                if (result_peg_398.error()){
                    goto loop_peg_397;
                }
                result_peg_395.addResult(result_peg_398);
            } while (true);
            loop_peg_397:
            ;
        
        Result result_peg_396 = result_peg_395;
        
        result_peg_395 = rule_expr13_real(stream, result_peg_395.getPosition());
            if (result_peg_395.error()){
                goto out_peg_400;
            }
            exp = result_peg_395.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_396.getValues(), exp);
                result_peg_395.setValue(value);
            }
        
        
    }
    
    if (column_peg_394.chunk6 == 0){
        column_peg_394.chunk6 = new Chunk6();
    }
    column_peg_394.chunk6->chunk_expr13 = result_peg_395;
    stream.update(result_peg_395.getPosition());
    
    
    return result_peg_395;
    out_peg_400:
    
    if (column_peg_394.chunk6 == 0){
        column_peg_394.chunk6 = new Chunk6();
    }
    column_peg_394.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_403:
    Value right;
    Value new_left;
    Result result_peg_402(myposition);
    {
    
        result_peg_402 = rule_s(stream, result_peg_402.getPosition());
            if (result_peg_402.error()){
                goto out_peg_405;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_402.getPosition()))){
                    result_peg_402.nextPosition();
                } else {
                    goto out_peg_405;
                }
            }
            result_peg_402.setValue((void*) "||");
        
        
        
        result_peg_402 = rule_s(stream, result_peg_402.getPosition());
            if (result_peg_402.error()){
                goto out_peg_405;
            }
        
        
        
        result_peg_402 = rule_expr2(stream, result_peg_402.getPosition());
            if (result_peg_402.error()){
                goto out_peg_405;
            }
            right = result_peg_402.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_402.setValue(value);
            }
            new_left = result_peg_402.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_402.getPosition();
    goto tail_peg_403;
    out_peg_405:
    Result result_peg_410(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_410.setValue(value);
            }
        
        
    }
    
    
    return result_peg_410;
    
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
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
                if (compareChar("^^"[i], stream.get(result_peg_413.getPosition()))){
                    result_peg_413.nextPosition();
                } else {
                    goto out_peg_416;
                }
            }
            result_peg_413.setValue((void*) "^^");
        
        
        
        result_peg_413 = rule_s(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_416;
            }
        
        
        
        result_peg_413 = rule_expr3(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_416;
            }
            right = result_peg_413.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
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
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
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
                if (compareChar("&&"[i], stream.get(result_peg_424.getPosition()))){
                    result_peg_424.nextPosition();
                } else {
                    goto out_peg_427;
                }
            }
            result_peg_424.setValue((void*) "&&");
        
        
        
        result_peg_424 = rule_s(stream, result_peg_424.getPosition());
            if (result_peg_424.error()){
                goto out_peg_427;
            }
        
        
        
        result_peg_424 = rule_expr4(stream, result_peg_424.getPosition());
            if (result_peg_424.error()){
                goto out_peg_427;
            }
            right = result_peg_424.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
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
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
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
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_435.getPosition()))){
                    result_peg_435.nextPosition();
                } else {
                    goto out_peg_438;
                }
            }
            result_peg_435.setValue((void*) "|");
        
        
        
        result_peg_435 = rule_s(stream, result_peg_435.getPosition());
            if (result_peg_435.error()){
                goto out_peg_438;
            }
        
        
        
        result_peg_435 = rule_expr5(stream, result_peg_435.getPosition());
            if (result_peg_435.error()){
                goto out_peg_438;
            }
            right = result_peg_435.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
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
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
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
                if (compareChar("^"[i], stream.get(result_peg_446.getPosition()))){
                    result_peg_446.nextPosition();
                } else {
                    goto out_peg_449;
                }
            }
            result_peg_446.setValue((void*) "^");
        
        
        
        result_peg_446 = rule_s(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
        
        
        
        result_peg_446 = rule_expr6(stream, result_peg_446.getPosition());
            if (result_peg_446.error()){
                goto out_peg_449;
            }
            right = result_peg_446.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
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
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
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
                if (compareChar("&"[i], stream.get(result_peg_457.getPosition()))){
                    result_peg_457.nextPosition();
                } else {
                    goto out_peg_460;
                }
            }
            result_peg_457.setValue((void*) "&");
        
        
        
        result_peg_457 = rule_s(stream, result_peg_457.getPosition());
            if (result_peg_457.error()){
                goto out_peg_460;
            }
        
        
        
        result_peg_457 = rule_expr7(stream, result_peg_457.getPosition());
            if (result_peg_457.error()){
                goto out_peg_460;
            }
            right = result_peg_457.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
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
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
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
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_468.getPosition()))){
                    result_peg_468.nextPosition();
                } else {
                    goto out_peg_471;
                }
            }
            result_peg_468.setValue((void*) ":=");
        
        
        
        result_peg_468 = rule_s(stream, result_peg_468.getPosition());
            if (result_peg_468.error()){
                goto out_peg_471;
            }
        
        
        
        result_peg_468 = rule_expr8(stream, result_peg_468.getPosition());
            if (result_peg_468.error()){
                goto out_peg_471;
            }
            right = result_peg_468.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
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
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
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
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_479.getPosition()))){
                    result_peg_479.nextPosition();
                } else {
                    goto out_peg_482;
                }
            }
            result_peg_479.setValue((void*) "=");
        
        
        
        result_peg_479 = rule_s(stream, result_peg_479.getPosition());
            if (result_peg_479.error()){
                goto out_peg_482;
            }
        
        
        
        result_peg_479 = rule_expr9(stream, result_peg_479.getPosition());
            if (result_peg_479.error()){
                goto out_peg_482;
            }
            right = result_peg_479.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
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
    
        result_peg_487 = rule_s(stream, result_peg_487.getPosition());
            if (result_peg_487.error()){
                goto out_peg_489;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_487.getPosition()))){
                    result_peg_487.nextPosition();
                } else {
                    goto out_peg_489;
                }
            }
            result_peg_487.setValue((void*) "!=");
        
        
        
        result_peg_487 = rule_s(stream, result_peg_487.getPosition());
            if (result_peg_487.error()){
                goto out_peg_489;
            }
        
        
        
        result_peg_487 = rule_expr9(stream, result_peg_487.getPosition());
            if (result_peg_487.error()){
                goto out_peg_489;
            }
            right = result_peg_487.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_487.setValue(value);
            }
            new_left = result_peg_487.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_487.getPosition();
    goto tail_peg_480;
    out_peg_489:
    Result result_peg_494(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_494.setValue(value);
            }
        
        
    }
    
    
    return result_peg_494;
    
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_498:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_497(myposition);
    {
    
        result_peg_497 = rule_s(stream, result_peg_497.getPosition());
            if (result_peg_497.error()){
                goto out_peg_500;
            }
        
        
        
        result_peg_497 = rule_compare(stream, result_peg_497.getPosition());
            if (result_peg_497.error()){
                goto out_peg_500;
            }
            maker = result_peg_497.getValues();
        
        
        
        result_peg_497 = rule_s(stream, result_peg_497.getPosition());
            if (result_peg_497.error()){
                goto out_peg_500;
            }
        
        
        
        result_peg_497 = rule_expr10(stream, result_peg_497.getPosition());
            if (result_peg_497.error()){
                goto out_peg_500;
            }
            right = result_peg_497.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_497.setValue(value);
            }
            new_left = result_peg_497.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_497.getPosition();
    goto tail_peg_498;
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
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_507 = stream.getColumn(position);
    if (column_peg_507.chunk8 != 0 && column_peg_507.chunk8->chunk_compare.calculated()){
        return column_peg_507.chunk8->chunk_compare;
    }
    
    int myposition = position;
    
    
    Result result_peg_508(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_508.getPosition()))){
                    result_peg_508.nextPosition();
                } else {
                    goto out_peg_510;
                }
            }
            result_peg_508.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_508.setValue(value);
            }
        
        
    }
    
    if (column_peg_507.chunk8 == 0){
        column_peg_507.chunk8 = new Chunk8();
    }
    column_peg_507.chunk8->chunk_compare = result_peg_508;
    stream.update(result_peg_508.getPosition());
    
    
    return result_peg_508;
    out_peg_510:
    Result result_peg_511(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_511.getPosition()))){
                    result_peg_511.nextPosition();
                } else {
                    goto out_peg_513;
                }
            }
            result_peg_511.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_511.setValue(value);
            }
        
        
    }
    
    if (column_peg_507.chunk8 == 0){
        column_peg_507.chunk8 = new Chunk8();
    }
    column_peg_507.chunk8->chunk_compare = result_peg_511;
    stream.update(result_peg_511.getPosition());
    
    
    return result_peg_511;
    out_peg_513:
    Result result_peg_514(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_514.getPosition()))){
                    result_peg_514.nextPosition();
                } else {
                    goto out_peg_516;
                }
            }
            result_peg_514.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_514.setValue(value);
            }
        
        
    }
    
    if (column_peg_507.chunk8 == 0){
        column_peg_507.chunk8 = new Chunk8();
    }
    column_peg_507.chunk8->chunk_compare = result_peg_514;
    stream.update(result_peg_514.getPosition());
    
    
    return result_peg_514;
    out_peg_516:
    Result result_peg_517(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_517.getPosition()))){
                    result_peg_517.nextPosition();
                } else {
                    goto out_peg_519;
                }
            }
            result_peg_517.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_517.setValue(value);
            }
        
        
    }
    
    if (column_peg_507.chunk8 == 0){
        column_peg_507.chunk8 = new Chunk8();
    }
    column_peg_507.chunk8->chunk_compare = result_peg_517;
    stream.update(result_peg_517.getPosition());
    
    
    return result_peg_517;
    out_peg_519:
    
    if (column_peg_507.chunk8 == 0){
        column_peg_507.chunk8 = new Chunk8();
    }
    column_peg_507.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_520 = stream.getColumn(position);
    if (column_peg_520.chunk8 != 0 && column_peg_520.chunk8->chunk_all_compare.calculated()){
        return column_peg_520.chunk8->chunk_all_compare;
    }
    
    int myposition = position;
    
    
    Result result_peg_521(myposition);
    
    result_peg_521 = rule_compare(stream, result_peg_521.getPosition());
    if (result_peg_521.error()){
        goto out_peg_522;
    }
    
    if (column_peg_520.chunk8 == 0){
        column_peg_520.chunk8 = new Chunk8();
    }
    column_peg_520.chunk8->chunk_all_compare = result_peg_521;
    stream.update(result_peg_521.getPosition());
    
    
    return result_peg_521;
    out_peg_522:
    Result result_peg_523(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_523.getPosition()))){
            result_peg_523.nextPosition();
        } else {
            goto out_peg_524;
        }
    }
    result_peg_523.setValue((void*) "=");
    
    if (column_peg_520.chunk8 == 0){
        column_peg_520.chunk8 = new Chunk8();
    }
    column_peg_520.chunk8->chunk_all_compare = result_peg_523;
    stream.update(result_peg_523.getPosition());
    
    
    return result_peg_523;
    out_peg_524:
    Result result_peg_525(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_525.getPosition()))){
            result_peg_525.nextPosition();
        } else {
            goto out_peg_526;
        }
    }
    result_peg_525.setValue((void*) "!=");
    
    if (column_peg_520.chunk8 == 0){
        column_peg_520.chunk8 = new Chunk8();
    }
    column_peg_520.chunk8->chunk_all_compare = result_peg_525;
    stream.update(result_peg_525.getPosition());
    
    
    return result_peg_525;
    out_peg_526:
    
    if (column_peg_520.chunk8 == 0){
        column_peg_520.chunk8 = new Chunk8();
    }
    column_peg_520.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_529:
    Value right;
    Value new_left;
    Result result_peg_528(myposition);
    {
    
        result_peg_528 = rule_s(stream, result_peg_528.getPosition());
            if (result_peg_528.error()){
                goto out_peg_531;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_528.getPosition()))){
                    result_peg_528.nextPosition();
                } else {
                    goto out_peg_531;
                }
            }
            result_peg_528.setValue((void*) "+");
        
        
        
        result_peg_528 = rule_s(stream, result_peg_528.getPosition());
            if (result_peg_528.error()){
                goto out_peg_531;
            }
        
        
        
        result_peg_528 = rule_expr11(stream, result_peg_528.getPosition());
            if (result_peg_528.error()){
                goto out_peg_531;
            }
            right = result_peg_528.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_528.setValue(value);
            }
            new_left = result_peg_528.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_528.getPosition();
    goto tail_peg_529;
    out_peg_531:
    Result result_peg_536(myposition);
    {
    
        result_peg_536 = rule_s(stream, result_peg_536.getPosition());
            if (result_peg_536.error()){
                goto out_peg_538;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_536.getPosition()))){
                    result_peg_536.nextPosition();
                } else {
                    goto out_peg_538;
                }
            }
            result_peg_536.setValue((void*) "-");
        
        
        
        result_peg_536 = rule_s(stream, result_peg_536.getPosition());
            if (result_peg_536.error()){
                goto out_peg_538;
            }
        
        
        
        result_peg_536 = rule_expr11(stream, result_peg_536.getPosition());
            if (result_peg_536.error()){
                goto out_peg_538;
            }
            right = result_peg_536.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_536.setValue(value);
            }
            new_left = result_peg_536.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_536.getPosition();
    goto tail_peg_529;
    out_peg_538:
    Result result_peg_543(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_543.setValue(value);
            }
        
        
    }
    
    
    return result_peg_543;
    
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_547:
    Value right;
    Value new_left;
    Result result_peg_546(myposition);
    {
    
        result_peg_546 = rule_s(stream, result_peg_546.getPosition());
            if (result_peg_546.error()){
                goto out_peg_549;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_546.getPosition()))){
                    result_peg_546.nextPosition();
                } else {
                    goto out_peg_549;
                }
            }
            result_peg_546.setValue((void*) "*");
        
        
        
        result_peg_546 = rule_s(stream, result_peg_546.getPosition());
            if (result_peg_546.error()){
                goto out_peg_549;
            }
        
        
        
        result_peg_546 = rule_expr12(stream, result_peg_546.getPosition());
            if (result_peg_546.error()){
                goto out_peg_549;
            }
            right = result_peg_546.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_546.setValue(value);
            }
            new_left = result_peg_546.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_546.getPosition();
    goto tail_peg_547;
    out_peg_549:
    Result result_peg_554(myposition);
    {
    
        result_peg_554 = rule_s(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_556;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_554.getPosition()))){
                    result_peg_554.nextPosition();
                } else {
                    goto out_peg_556;
                }
            }
            result_peg_554.setValue((void*) "/");
        
        
        
        result_peg_554 = rule_s(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_556;
            }
        
        
        
        result_peg_554 = rule_expr12(stream, result_peg_554.getPosition());
            if (result_peg_554.error()){
                goto out_peg_556;
            }
            right = result_peg_554.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_554.setValue(value);
            }
            new_left = result_peg_554.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_554.getPosition();
    goto tail_peg_547;
    out_peg_556:
    Result result_peg_561(myposition);
    {
    
        result_peg_561 = rule_s(stream, result_peg_561.getPosition());
            if (result_peg_561.error()){
                goto out_peg_563;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_561.getPosition()))){
                    result_peg_561.nextPosition();
                } else {
                    goto out_peg_563;
                }
            }
            result_peg_561.setValue((void*) "%");
        
        
        
        result_peg_561 = rule_s(stream, result_peg_561.getPosition());
            if (result_peg_561.error()){
                goto out_peg_563;
            }
        
        
        
        result_peg_561 = rule_expr12(stream, result_peg_561.getPosition());
            if (result_peg_561.error()){
                goto out_peg_563;
            }
            right = result_peg_561.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_561.setValue(value);
            }
            new_left = result_peg_561.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_561.getPosition();
    goto tail_peg_547;
    out_peg_563:
    Result result_peg_568(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_568.setValue(value);
            }
        
        
    }
    
    
    return result_peg_568;
    
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_572:
    Value right;
    Value new_left;
    Result result_peg_571(myposition);
    {
    
        result_peg_571 = rule_s(stream, result_peg_571.getPosition());
            if (result_peg_571.error()){
                goto out_peg_574;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_571.getPosition()))){
                    result_peg_571.nextPosition();
                } else {
                    goto out_peg_574;
                }
            }
            result_peg_571.setValue((void*) "**");
        
        
        
        result_peg_571 = rule_s(stream, result_peg_571.getPosition());
            if (result_peg_571.error()){
                goto out_peg_574;
            }
        
        
        
        result_peg_571 = rule_expr13(stream, result_peg_571.getPosition());
            if (result_peg_571.error()){
                goto out_peg_574;
            }
            right = result_peg_571.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_571.setValue(value);
            }
            new_left = result_peg_571.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_571.getPosition();
    goto tail_peg_572;
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
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_581 = stream.getColumn(position);
    if (column_peg_581.chunk9 != 0 && column_peg_581.chunk9->chunk_unary.calculated()){
        return column_peg_581.chunk9->chunk_unary;
    }
    
    int myposition = position;
    
    
    Result result_peg_582(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_582.getPosition()))){
                    result_peg_582.nextPosition();
                } else {
                    goto out_peg_584;
                }
            }
            result_peg_582.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_582.setValue(value);
            }
        
        
    }
    
    if (column_peg_581.chunk9 == 0){
        column_peg_581.chunk9 = new Chunk9();
    }
    column_peg_581.chunk9->chunk_unary = result_peg_582;
    stream.update(result_peg_582.getPosition());
    
    
    return result_peg_582;
    out_peg_584:
    Result result_peg_585(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_585.getPosition()))){
                    result_peg_585.nextPosition();
                } else {
                    goto out_peg_587;
                }
            }
            result_peg_585.setValue((void*) "-");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_585.setValue(value);
            }
        
        
    }
    
    if (column_peg_581.chunk9 == 0){
        column_peg_581.chunk9 = new Chunk9();
    }
    column_peg_581.chunk9->chunk_unary = result_peg_585;
    stream.update(result_peg_585.getPosition());
    
    
    return result_peg_585;
    out_peg_587:
    Result result_peg_588(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_588.getPosition()))){
                    result_peg_588.nextPosition();
                } else {
                    goto out_peg_590;
                }
            }
            result_peg_588.setValue((void*) "~");
        
        
        
        Result result_peg_593(result_peg_588);
            result_peg_593 = rule_key(stream, result_peg_593.getPosition());
            if (result_peg_593.error()){
                goto not_peg_592;
            }
            goto out_peg_590;
            not_peg_592:
            result_peg_588.setValue((void*)0);
        
        
        
        Result result_peg_596(result_peg_588);
            {
                
                char letter_peg_601 = stream.get(result_peg_596.getPosition());
                if (letter_peg_601 != '\0' && strchr("0123456789", letter_peg_601) != NULL){
                    result_peg_596.nextPosition();
                    result_peg_596.setValue((void*) letter_peg_601);
                } else {
                    goto out_peg_600;
                }
                
            }
            goto success_peg_597;
            out_peg_600:
            goto not_peg_595;
            success_peg_597:
            ;
            goto out_peg_590;
            not_peg_595:
            result_peg_588.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_588.setValue(value);
            }
        
        
    }
    
    if (column_peg_581.chunk9 == 0){
        column_peg_581.chunk9 = new Chunk9();
    }
    column_peg_581.chunk9->chunk_unary = result_peg_588;
    stream.update(result_peg_588.getPosition());
    
    
    return result_peg_588;
    out_peg_590:
    
    if (column_peg_581.chunk9 == 0){
        column_peg_581.chunk9 = new Chunk9();
    }
    column_peg_581.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_602 = stream.getColumn(position);
    if (column_peg_602.chunk9 != 0 && column_peg_602.chunk9->chunk_expr13_real.calculated()){
        return column_peg_602.chunk9->chunk_expr13_real;
    }
    
    int myposition = position;
    
    Value e;
    Result result_peg_603(myposition);
    
    result_peg_603 = rule_function(stream, result_peg_603.getPosition());
    if (result_peg_603.error()){
        goto out_peg_604;
    }
    
    if (column_peg_602.chunk9 == 0){
        column_peg_602.chunk9 = new Chunk9();
    }
    column_peg_602.chunk9->chunk_expr13_real = result_peg_603;
    stream.update(result_peg_603.getPosition());
    
    
    return result_peg_603;
    out_peg_604:
    Result result_peg_605(myposition);
    
    result_peg_605 = rule_value(stream, result_peg_605.getPosition());
    if (result_peg_605.error()){
        goto out_peg_606;
    }
    
    if (column_peg_602.chunk9 == 0){
        column_peg_602.chunk9 = new Chunk9();
    }
    column_peg_602.chunk9->chunk_expr13_real = result_peg_605;
    stream.update(result_peg_605.getPosition());
    
    
    return result_peg_605;
    out_peg_606:
    Result result_peg_607(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_607.getPosition()))){
                    result_peg_607.nextPosition();
                } else {
                    goto out_peg_609;
                }
            }
            result_peg_607.setValue((void*) "(");
        
        
        
        result_peg_607 = rule_s(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_609;
            }
        
        
        
        result_peg_607 = rule_expr(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_609;
            }
            e = result_peg_607.getValues();
        
        
        
        result_peg_607 = rule_s(stream, result_peg_607.getPosition());
            if (result_peg_607.error()){
                goto out_peg_609;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_607.getPosition()))){
                    result_peg_607.nextPosition();
                } else {
                    goto out_peg_609;
                }
            }
            result_peg_607.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_607.setValue(value);
            }
        
        
    }
    
    if (column_peg_602.chunk9 == 0){
        column_peg_602.chunk9 = new Chunk9();
    }
    column_peg_602.chunk9->chunk_expr13_real = result_peg_607;
    stream.update(result_peg_607.getPosition());
    
    
    return result_peg_607;
    out_peg_609:
    
    if (column_peg_602.chunk9 == 0){
        column_peg_602.chunk9 = new Chunk9();
    }
    column_peg_602.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_614 = stream.getColumn(position);
    if (column_peg_614.chunk9 != 0 && column_peg_614.chunk9->chunk_function.calculated()){
        return column_peg_614.chunk9->chunk_function;
    }
    
    int myposition = position;
    
    Value name;
    Value arg1;
    Value arg2;
    Value arg3;
    Result result_peg_615(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_615.getPosition()))){
                    result_peg_615.nextPosition();
                } else {
                    goto out_peg_617;
                }
            }
            result_peg_615.setValue((void*) "abs");
            name = result_peg_615.getValues();
        
        
        
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_615.getPosition()))){
                    result_peg_615.nextPosition();
                } else {
                    goto out_peg_617;
                }
            }
            result_peg_615.setValue((void*) "(");
        
        
        
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
        
        
        
        result_peg_615 = rule_expr(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
            arg1 = result_peg_615.getValues();
        
        
        
        result_peg_615 = rule_s(stream, result_peg_615.getPosition());
            if (result_peg_615.error()){
                goto out_peg_617;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_615.getPosition()))){
                    result_peg_615.nextPosition();
                } else {
                    goto out_peg_617;
                }
            }
            result_peg_615.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_615.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_615;
    stream.update(result_peg_615.getPosition());
    
    
    return result_peg_615;
    out_peg_617:
    Result result_peg_624(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_624.getPosition()))){
                    result_peg_624.nextPosition();
                } else {
                    goto out_peg_626;
                }
            }
            result_peg_624.setValue((void*) "const");
            name = result_peg_624.getValues();
        
        
        
        result_peg_624 = rule_s(stream, result_peg_624.getPosition());
            if (result_peg_624.error()){
                goto out_peg_626;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_624.getPosition()))){
                    result_peg_624.nextPosition();
                } else {
                    goto out_peg_626;
                }
            }
            result_peg_624.setValue((void*) "(");
        
        
        
        result_peg_624 = rule_s(stream, result_peg_624.getPosition());
            if (result_peg_624.error()){
                goto out_peg_626;
            }
        
        
        
        result_peg_624 = rule_expr(stream, result_peg_624.getPosition());
            if (result_peg_624.error()){
                goto out_peg_626;
            }
            arg1 = result_peg_624.getValues();
        
        
        
        result_peg_624 = rule_s(stream, result_peg_624.getPosition());
            if (result_peg_624.error()){
                goto out_peg_626;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_624.getPosition()))){
                    result_peg_624.nextPosition();
                } else {
                    goto out_peg_626;
                }
            }
            result_peg_624.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_624.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_624;
    stream.update(result_peg_624.getPosition());
    
    
    return result_peg_624;
    out_peg_626:
    Result result_peg_633(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_633.getPosition()))){
                    result_peg_633.nextPosition();
                } else {
                    goto out_peg_635;
                }
            }
            result_peg_633.setValue((void*) "selfanimexist");
            name = result_peg_633.getValues();
        
        
        
        result_peg_633 = rule_s(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_635;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_633.getPosition()))){
                    result_peg_633.nextPosition();
                } else {
                    goto out_peg_635;
                }
            }
            result_peg_633.setValue((void*) "(");
        
        
        
        result_peg_633 = rule_s(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_635;
            }
        
        
        
        result_peg_633 = rule_expr(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_635;
            }
            arg1 = result_peg_633.getValues();
        
        
        
        result_peg_633 = rule_s(stream, result_peg_633.getPosition());
            if (result_peg_633.error()){
                goto out_peg_635;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_633.getPosition()))){
                    result_peg_633.nextPosition();
                } else {
                    goto out_peg_635;
                }
            }
            result_peg_633.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_633.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_633;
    stream.update(result_peg_633.getPosition());
    
    
    return result_peg_633;
    out_peg_635:
    Result result_peg_642(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_642.getPosition()))){
                    result_peg_642.nextPosition();
                } else {
                    goto out_peg_644;
                }
            }
            result_peg_642.setValue((void*) "ifelse");
            name = result_peg_642.getValues();
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_642.getPosition()))){
                    result_peg_642.nextPosition();
                } else {
                    goto out_peg_644;
                }
            }
            result_peg_642.setValue((void*) "(");
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        result_peg_642 = rule_expr_c(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
            arg1 = result_peg_642.getValues();
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_642.getPosition()))){
                    result_peg_642.nextPosition();
                } else {
                    goto out_peg_644;
                }
            }
            result_peg_642.setValue((void*) ",");
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        result_peg_642 = rule_expr_c(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
            arg2 = result_peg_642.getValues();
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_642.getPosition()))){
                    result_peg_642.nextPosition();
                } else {
                    goto out_peg_644;
                }
            }
            result_peg_642.setValue((void*) ",");
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        result_peg_642 = rule_expr_c(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
            arg3 = result_peg_642.getValues();
        
        
        
        result_peg_642 = rule_s(stream, result_peg_642.getPosition());
            if (result_peg_642.error()){
                goto out_peg_644;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_642.getPosition()))){
                    result_peg_642.nextPosition();
                } else {
                    goto out_peg_644;
                }
            }
            result_peg_642.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_642.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_642;
    stream.update(result_peg_642.getPosition());
    
    
    return result_peg_642;
    out_peg_644:
    Result result_peg_659(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_659.getPosition()))){
                    result_peg_659.nextPosition();
                } else {
                    goto out_peg_661;
                }
            }
            result_peg_659.setValue((void*) "gethitvar");
            name = result_peg_659.getValues();
        
        
        
        result_peg_659 = rule_s(stream, result_peg_659.getPosition());
            if (result_peg_659.error()){
                goto out_peg_661;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_659.getPosition()))){
                    result_peg_659.nextPosition();
                } else {
                    goto out_peg_661;
                }
            }
            result_peg_659.setValue((void*) "(");
        
        
        
        result_peg_659 = rule_s(stream, result_peg_659.getPosition());
            if (result_peg_659.error()){
                goto out_peg_661;
            }
        
        
        
        result_peg_659 = rule_expr_c(stream, result_peg_659.getPosition());
            if (result_peg_659.error()){
                goto out_peg_661;
            }
            arg1 = result_peg_659.getValues();
        
        
        
        result_peg_659 = rule_s(stream, result_peg_659.getPosition());
            if (result_peg_659.error()){
                goto out_peg_661;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_659.getPosition()))){
                    result_peg_659.nextPosition();
                } else {
                    goto out_peg_661;
                }
            }
            result_peg_659.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_659.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_659;
    stream.update(result_peg_659.getPosition());
    
    
    return result_peg_659;
    out_peg_661:
    Result result_peg_668(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_668.getPosition()))){
                    result_peg_668.nextPosition();
                } else {
                    goto out_peg_670;
                }
            }
            result_peg_668.setValue((void*) "floor");
            name = result_peg_668.getValues();
        
        
        
        result_peg_668 = rule_s(stream, result_peg_668.getPosition());
            if (result_peg_668.error()){
                goto out_peg_670;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_668.getPosition()))){
                    result_peg_668.nextPosition();
                } else {
                    goto out_peg_670;
                }
            }
            result_peg_668.setValue((void*) "(");
        
        
        
        result_peg_668 = rule_s(stream, result_peg_668.getPosition());
            if (result_peg_668.error()){
                goto out_peg_670;
            }
        
        
        
        result_peg_668 = rule_expr_c(stream, result_peg_668.getPosition());
            if (result_peg_668.error()){
                goto out_peg_670;
            }
            arg1 = result_peg_668.getValues();
        
        
        
        result_peg_668 = rule_s(stream, result_peg_668.getPosition());
            if (result_peg_668.error()){
                goto out_peg_670;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_668.getPosition()))){
                    result_peg_668.nextPosition();
                } else {
                    goto out_peg_670;
                }
            }
            result_peg_668.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_668.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_668;
    stream.update(result_peg_668.getPosition());
    
    
    return result_peg_668;
    out_peg_670:
    Result result_peg_677(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_677.getPosition()))){
                    result_peg_677.nextPosition();
                } else {
                    goto out_peg_679;
                }
            }
            result_peg_677.setValue((void*) "ceil");
            name = result_peg_677.getValues();
        
        
        
        result_peg_677 = rule_s(stream, result_peg_677.getPosition());
            if (result_peg_677.error()){
                goto out_peg_679;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_677.getPosition()))){
                    result_peg_677.nextPosition();
                } else {
                    goto out_peg_679;
                }
            }
            result_peg_677.setValue((void*) "(");
        
        
        
        result_peg_677 = rule_s(stream, result_peg_677.getPosition());
            if (result_peg_677.error()){
                goto out_peg_679;
            }
        
        
        
        result_peg_677 = rule_expr_c(stream, result_peg_677.getPosition());
            if (result_peg_677.error()){
                goto out_peg_679;
            }
            arg1 = result_peg_677.getValues();
        
        
        
        result_peg_677 = rule_s(stream, result_peg_677.getPosition());
            if (result_peg_677.error()){
                goto out_peg_679;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_677.getPosition()))){
                    result_peg_677.nextPosition();
                } else {
                    goto out_peg_679;
                }
            }
            result_peg_677.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_677.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_677;
    stream.update(result_peg_677.getPosition());
    
    
    return result_peg_677;
    out_peg_679:
    Result result_peg_686(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_686.getPosition()))){
                    result_peg_686.nextPosition();
                } else {
                    goto out_peg_688;
                }
            }
            result_peg_686.setValue((void*) "sysvar");
            name = result_peg_686.getValues();
        
        
        
        result_peg_686 = rule_s(stream, result_peg_686.getPosition());
            if (result_peg_686.error()){
                goto out_peg_688;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_686.getPosition()))){
                    result_peg_686.nextPosition();
                } else {
                    goto out_peg_688;
                }
            }
            result_peg_686.setValue((void*) "(");
        
        
        
        result_peg_686 = rule_s(stream, result_peg_686.getPosition());
            if (result_peg_686.error()){
                goto out_peg_688;
            }
        
        
        
        result_peg_686 = rule_integer(stream, result_peg_686.getPosition());
            if (result_peg_686.error()){
                goto out_peg_688;
            }
            arg1 = result_peg_686.getValues();
        
        
        
        result_peg_686 = rule_s(stream, result_peg_686.getPosition());
            if (result_peg_686.error()){
                goto out_peg_688;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_686.getPosition()))){
                    result_peg_686.nextPosition();
                } else {
                    goto out_peg_688;
                }
            }
            result_peg_686.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_686.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_686;
    stream.update(result_peg_686.getPosition());
    
    
    return result_peg_686;
    out_peg_688:
    Result result_peg_695(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_695.getPosition()))){
                    result_peg_695.nextPosition();
                } else {
                    goto out_peg_697;
                }
            }
            result_peg_695.setValue((void*) "var");
            name = result_peg_695.getValues();
        
        
        
        result_peg_695 = rule_s(stream, result_peg_695.getPosition());
            if (result_peg_695.error()){
                goto out_peg_697;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_695.getPosition()))){
                    result_peg_695.nextPosition();
                } else {
                    goto out_peg_697;
                }
            }
            result_peg_695.setValue((void*) "(");
        
        
        
        result_peg_695 = rule_s(stream, result_peg_695.getPosition());
            if (result_peg_695.error()){
                goto out_peg_697;
            }
        
        
        
        result_peg_695 = rule_integer(stream, result_peg_695.getPosition());
            if (result_peg_695.error()){
                goto out_peg_697;
            }
            arg1 = result_peg_695.getValues();
        
        
        
        result_peg_695 = rule_s(stream, result_peg_695.getPosition());
            if (result_peg_695.error()){
                goto out_peg_697;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_695.getPosition()))){
                    result_peg_695.nextPosition();
                } else {
                    goto out_peg_697;
                }
            }
            result_peg_695.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_695.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_695;
    stream.update(result_peg_695.getPosition());
    
    
    return result_peg_695;
    out_peg_697:
    Result result_peg_704(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_704.getPosition()))){
                    result_peg_704.nextPosition();
                } else {
                    goto out_peg_706;
                }
            }
            result_peg_704.setValue((void*) "numexplod");
            name = result_peg_704.getValues();
        
        
        
        result_peg_704 = rule_s(stream, result_peg_704.getPosition());
            if (result_peg_704.error()){
                goto out_peg_706;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_704.getPosition()))){
                    result_peg_704.nextPosition();
                } else {
                    goto out_peg_706;
                }
            }
            result_peg_704.setValue((void*) "(");
        
        
        
        result_peg_704 = rule_s(stream, result_peg_704.getPosition());
            if (result_peg_704.error()){
                goto out_peg_706;
            }
        
        
        
        result_peg_704 = rule_integer(stream, result_peg_704.getPosition());
            if (result_peg_704.error()){
                goto out_peg_706;
            }
            arg1 = result_peg_704.getValues();
        
        
        
        result_peg_704 = rule_s(stream, result_peg_704.getPosition());
            if (result_peg_704.error()){
                goto out_peg_706;
            }
        
        
        
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
                value = makeExpression();
                result_peg_704.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_704;
    stream.update(result_peg_704.getPosition());
    
    
    return result_peg_704;
    out_peg_706:
    Result result_peg_713(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_713.getPosition()))){
                    result_peg_713.nextPosition();
                } else {
                    goto out_peg_715;
                }
            }
            result_peg_713.setValue((void*) "numhelper");
            name = result_peg_713.getValues();
        
        
        
        result_peg_713 = rule_s(stream, result_peg_713.getPosition());
            if (result_peg_713.error()){
                goto out_peg_715;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_713.getPosition()))){
                    result_peg_713.nextPosition();
                } else {
                    goto out_peg_715;
                }
            }
            result_peg_713.setValue((void*) "(");
        
        
        
        result_peg_713 = rule_s(stream, result_peg_713.getPosition());
            if (result_peg_713.error()){
                goto out_peg_715;
            }
        
        
        
        result_peg_713 = rule_integer(stream, result_peg_713.getPosition());
            if (result_peg_713.error()){
                goto out_peg_715;
            }
            arg1 = result_peg_713.getValues();
        
        
        
        result_peg_713 = rule_s(stream, result_peg_713.getPosition());
            if (result_peg_713.error()){
                goto out_peg_715;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_713.getPosition()))){
                    result_peg_713.nextPosition();
                } else {
                    goto out_peg_715;
                }
            }
            result_peg_713.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_713.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_713;
    stream.update(result_peg_713.getPosition());
    
    
    return result_peg_713;
    out_peg_715:
    Result result_peg_722(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_722.getPosition()))){
                    result_peg_722.nextPosition();
                } else {
                    goto out_peg_724;
                }
            }
            result_peg_722.setValue((void*) "numprojid");
            name = result_peg_722.getValues();
        
        
        
        result_peg_722 = rule_s(stream, result_peg_722.getPosition());
            if (result_peg_722.error()){
                goto out_peg_724;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_722.getPosition()))){
                    result_peg_722.nextPosition();
                } else {
                    goto out_peg_724;
                }
            }
            result_peg_722.setValue((void*) "(");
        
        
        
        result_peg_722 = rule_s(stream, result_peg_722.getPosition());
            if (result_peg_722.error()){
                goto out_peg_724;
            }
        
        
        
        result_peg_722 = rule_integer(stream, result_peg_722.getPosition());
            if (result_peg_722.error()){
                goto out_peg_724;
            }
            arg1 = result_peg_722.getValues();
        
        
        
        result_peg_722 = rule_s(stream, result_peg_722.getPosition());
            if (result_peg_722.error()){
                goto out_peg_724;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_722.getPosition()))){
                    result_peg_722.nextPosition();
                } else {
                    goto out_peg_724;
                }
            }
            result_peg_722.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_722.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_722;
    stream.update(result_peg_722.getPosition());
    
    
    return result_peg_722;
    out_peg_724:
    Result result_peg_731(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_731.getPosition()))){
                    result_peg_731.nextPosition();
                } else {
                    goto out_peg_733;
                }
            }
            result_peg_731.setValue((void*) "helper");
            name = result_peg_731.getValues();
        
        
        
        result_peg_731 = rule_s(stream, result_peg_731.getPosition());
            if (result_peg_731.error()){
                goto out_peg_733;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_731.getPosition()))){
                    result_peg_731.nextPosition();
                } else {
                    goto out_peg_733;
                }
            }
            result_peg_731.setValue((void*) "(");
        
        
        
        result_peg_731 = rule_s(stream, result_peg_731.getPosition());
            if (result_peg_731.error()){
                goto out_peg_733;
            }
        
        
        
        result_peg_731 = rule_integer(stream, result_peg_731.getPosition());
            if (result_peg_731.error()){
                goto out_peg_733;
            }
            arg1 = result_peg_731.getValues();
        
        
        
        result_peg_731 = rule_s(stream, result_peg_731.getPosition());
            if (result_peg_731.error()){
                goto out_peg_733;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_731.getPosition()))){
                    result_peg_731.nextPosition();
                } else {
                    goto out_peg_733;
                }
            }
            result_peg_731.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_731.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_731;
    stream.update(result_peg_731.getPosition());
    
    
    return result_peg_731;
    out_peg_733:
    Result result_peg_740(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_740.getPosition()))){
                    result_peg_740.nextPosition();
                } else {
                    goto out_peg_742;
                }
            }
            result_peg_740.setValue((void*) "fvar");
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
        
        
        
        result_peg_740 = rule_integer(stream, result_peg_740.getPosition());
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
                value = makeExpression();
                result_peg_740.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_740;
    stream.update(result_peg_740.getPosition());
    
    
    return result_peg_740;
    out_peg_742:
    Result result_peg_749(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("target"[i], stream.get(result_peg_749.getPosition()))){
                    result_peg_749.nextPosition();
                } else {
                    goto out_peg_751;
                }
            }
            result_peg_749.setValue((void*) "target");
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
        
        
        
        result_peg_749 = rule_integer(stream, result_peg_749.getPosition());
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
                value = makeExpression();
                result_peg_749.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_749;
    stream.update(result_peg_749.getPosition());
    
    
    return result_peg_749;
    out_peg_751:
    Result result_peg_758(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_758.getPosition()))){
                    result_peg_758.nextPosition();
                } else {
                    goto out_peg_760;
                }
            }
            result_peg_758.setValue((void*) "ishelper");
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
        
        
        
        result_peg_758 = rule_integer(stream, result_peg_758.getPosition());
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
                value = makeExpression();
                result_peg_758.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_758;
    stream.update(result_peg_758.getPosition());
    
    
    return result_peg_758;
    out_peg_760:
    Result result_peg_767(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_767.getPosition()))){
                    result_peg_767.nextPosition();
                } else {
                    goto out_peg_769;
                }
            }
            result_peg_767.setValue((void*) "animelemtime");
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
        
        
        
        result_peg_767 = rule_integer(stream, result_peg_767.getPosition());
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
                value = makeExpression();
                result_peg_767.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_767;
    stream.update(result_peg_767.getPosition());
    
    
    return result_peg_767;
    out_peg_769:
    Result result_peg_776(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_776.getPosition()))){
                    result_peg_776.nextPosition();
                } else {
                    goto out_peg_778;
                }
            }
            result_peg_776.setValue((void*) "animelemno");
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
        
        
        
        result_peg_776 = rule_expr_c(stream, result_peg_776.getPosition());
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
                value = makeExpression();
                result_peg_776.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_776;
    stream.update(result_peg_776.getPosition());
    
    
    return result_peg_776;
    out_peg_778:
    Result result_peg_785(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_785.getPosition()))){
                    result_peg_785.nextPosition();
                } else {
                    goto out_peg_787;
                }
            }
            result_peg_785.setValue((void*) "animexist");
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
        
        
        
        result_peg_785 = rule_expr_c(stream, result_peg_785.getPosition());
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
                value = makeExpression();
                result_peg_785.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_785;
    stream.update(result_peg_785.getPosition());
    
    
    return result_peg_785;
    out_peg_787:
    Result result_peg_794(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_794.getPosition()))){
                    result_peg_794.nextPosition();
                } else {
                    goto out_peg_796;
                }
            }
            result_peg_794.setValue((void*) "projguarded");
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
        
        
        
        result_peg_794 = rule_expr_c(stream, result_peg_794.getPosition());
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
                value = makeExpression();
                result_peg_794.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_794;
    stream.update(result_peg_794.getPosition());
    
    
    return result_peg_794;
    out_peg_796:
    Result result_peg_803(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_803.getPosition()))){
                    result_peg_803.nextPosition();
                } else {
                    goto out_peg_805;
                }
            }
            result_peg_803.setValue((void*) "projcontact");
            name = result_peg_803.getValues();
        
        
        
        result_peg_803 = rule_s(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_803.getPosition()))){
                    result_peg_803.nextPosition();
                } else {
                    goto out_peg_805;
                }
            }
            result_peg_803.setValue((void*) "=");
        
        
        
        result_peg_803 = rule_s(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        result_peg_803 = rule_integer(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        result_peg_803 = rule_function_rest(stream, result_peg_803.getPosition());
            if (result_peg_803.error()){
                goto out_peg_805;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_803.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_803;
    stream.update(result_peg_803.getPosition());
    
    
    return result_peg_803;
    out_peg_805:
    Result result_peg_811(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_811.getPosition()))){
                    result_peg_811.nextPosition();
                } else {
                    goto out_peg_813;
                }
            }
            result_peg_811.setValue((void*) "animelem");
            name = result_peg_811.getValues();
        
        
        
        result_peg_811 = rule_s(stream, result_peg_811.getPosition());
            if (result_peg_811.error()){
                goto out_peg_813;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_811.getPosition()))){
                    result_peg_811.nextPosition();
                } else {
                    goto out_peg_813;
                }
            }
            result_peg_811.setValue((void*) "=");
        
        
        
        result_peg_811 = rule_s(stream, result_peg_811.getPosition());
            if (result_peg_811.error()){
                goto out_peg_813;
            }
        
        
        
        result_peg_811 = rule_integer(stream, result_peg_811.getPosition());
            if (result_peg_811.error()){
                goto out_peg_813;
            }
        
        
        
        result_peg_811 = rule_function_rest(stream, result_peg_811.getPosition());
            if (result_peg_811.error()){
                goto out_peg_813;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_811.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_811;
    stream.update(result_peg_811.getPosition());
    
    
    return result_peg_811;
    out_peg_813:
    Result result_peg_819(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_819.getPosition()))){
                    result_peg_819.nextPosition();
                } else {
                    goto out_peg_821;
                }
            }
            result_peg_819.setValue((void*) "numhelper");
            name = result_peg_819.getValues();
        
        
        
        result_peg_819 = rule_s(stream, result_peg_819.getPosition());
            if (result_peg_819.error()){
                goto out_peg_821;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_819.getPosition()))){
                    result_peg_819.nextPosition();
                } else {
                    goto out_peg_821;
                }
            }
            result_peg_819.setValue((void*) "(");
        
        
        
        result_peg_819 = rule_s(stream, result_peg_819.getPosition());
            if (result_peg_819.error()){
                goto out_peg_821;
            }
        
        
        
        result_peg_819 = rule_expr_c(stream, result_peg_819.getPosition());
            if (result_peg_819.error()){
                goto out_peg_821;
            }
            arg1 = result_peg_819.getValues();
        
        
        
        result_peg_819 = rule_s(stream, result_peg_819.getPosition());
            if (result_peg_819.error()){
                goto out_peg_821;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_819.getPosition()))){
                    result_peg_819.nextPosition();
                } else {
                    goto out_peg_821;
                }
            }
            result_peg_819.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_819.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_819;
    stream.update(result_peg_819.getPosition());
    
    
    return result_peg_819;
    out_peg_821:
    Result result_peg_828(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_828.getPosition()))){
                    result_peg_828.nextPosition();
                } else {
                    goto out_peg_830;
                }
            }
            result_peg_828.setValue((void*) "helper");
            name = result_peg_828.getValues();
        
        
        
        result_peg_828 = rule_s(stream, result_peg_828.getPosition());
            if (result_peg_828.error()){
                goto out_peg_830;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_828.getPosition()))){
                    result_peg_828.nextPosition();
                } else {
                    goto out_peg_830;
                }
            }
            result_peg_828.setValue((void*) "(");
        
        
        
        result_peg_828 = rule_s(stream, result_peg_828.getPosition());
            if (result_peg_828.error()){
                goto out_peg_830;
            }
        
        
        
        result_peg_828 = rule_expr_c(stream, result_peg_828.getPosition());
            if (result_peg_828.error()){
                goto out_peg_830;
            }
            arg1 = result_peg_828.getValues();
        
        
        
        result_peg_828 = rule_s(stream, result_peg_828.getPosition());
            if (result_peg_828.error()){
                goto out_peg_830;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_828.getPosition()))){
                    result_peg_828.nextPosition();
                } else {
                    goto out_peg_830;
                }
            }
            result_peg_828.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_828.setValue(value);
            }
        
        
    }
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = result_peg_828;
    stream.update(result_peg_828.getPosition());
    
    
    return result_peg_828;
    out_peg_830:
    
    if (column_peg_614.chunk9 == 0){
        column_peg_614.chunk9 = new Chunk9();
    }
    column_peg_614.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_837 = stream.getColumn(position);
    if (column_peg_837.chunk10 != 0 && column_peg_837.chunk10->chunk_function_rest.calculated()){
        return column_peg_837.chunk10->chunk_function_rest;
    }
    
    int myposition = position;
    
    
    Result result_peg_838(myposition);
    
    {
    
        result_peg_838 = rule_s(stream, result_peg_838.getPosition());
            if (result_peg_838.error()){
                goto out_peg_840;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_838.getPosition()))){
                    result_peg_838.nextPosition();
                } else {
                    goto out_peg_840;
                }
            }
            result_peg_838.setValue((void*) ",");
        
        
        
        result_peg_838 = rule_s(stream, result_peg_838.getPosition());
            if (result_peg_838.error()){
                goto out_peg_840;
            }
        
        
        
        result_peg_838 = rule_all_compare(stream, result_peg_838.getPosition());
            if (result_peg_838.error()){
                goto out_peg_840;
            }
        
        
        
        result_peg_838 = rule_s(stream, result_peg_838.getPosition());
            if (result_peg_838.error()){
                goto out_peg_840;
            }
        
        
        
        result_peg_838 = rule_integer(stream, result_peg_838.getPosition());
            if (result_peg_838.error()){
                goto out_peg_840;
            }
        
        
    }
    
    if (column_peg_837.chunk10 == 0){
        column_peg_837.chunk10 = new Chunk10();
    }
    column_peg_837.chunk10->chunk_function_rest = result_peg_838;
    stream.update(result_peg_838.getPosition());
    
    
    return result_peg_838;
    out_peg_840:
    
    if (column_peg_837.chunk10 == 0){
        column_peg_837.chunk10 = new Chunk10();
    }
    column_peg_837.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_845 = stream.getColumn(position);
    if (column_peg_845.chunk10 != 0 && column_peg_845.chunk10->chunk_keys.calculated()){
        return column_peg_845.chunk10->chunk_keys;
    }
    
    int myposition = position;
    
    
    Result result_peg_846(myposition);
    
    result_peg_846 = rule_key_value_list(stream, result_peg_846.getPosition());
    if (result_peg_846.error()){
        goto out_peg_847;
    }
    
    if (column_peg_845.chunk10 == 0){
        column_peg_845.chunk10 = new Chunk10();
    }
    column_peg_845.chunk10->chunk_keys = result_peg_846;
    stream.update(result_peg_846.getPosition());
    
    
    return result_peg_846;
    out_peg_847:
    
    if (column_peg_845.chunk10 == 0){
        column_peg_845.chunk10 = new Chunk10();
    }
    column_peg_845.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_848 = stream.getColumn(position);
    if (column_peg_848.chunk10 != 0 && column_peg_848.chunk10->chunk_key_value_list.calculated()){
        return column_peg_848.chunk10->chunk_key_value_list;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_849(myposition);
    
    {
    
        result_peg_849 = rule_key(stream, result_peg_849.getPosition());
            if (result_peg_849.error()){
                goto out_peg_851;
            }
            first = result_peg_849.getValues();
        
        
        
        result_peg_849.reset();
            do{
                Result result_peg_854(result_peg_849.getPosition());
                {
                
                    result_peg_854 = rule_s(stream, result_peg_854.getPosition());
                        if (result_peg_854.error()){
                            goto loop_peg_853;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_854.getPosition()))){
                                result_peg_854.nextPosition();
                            } else {
                                goto loop_peg_853;
                            }
                        }
                        result_peg_854.setValue((void*) ",");
                    
                    
                    
                    result_peg_854 = rule_s(stream, result_peg_854.getPosition());
                        if (result_peg_854.error()){
                            goto loop_peg_853;
                        }
                    
                    
                    
                    result_peg_854 = rule_key(stream, result_peg_854.getPosition());
                        if (result_peg_854.error()){
                            goto loop_peg_853;
                        }
                    
                    
                }
                result_peg_849.addResult(result_peg_854);
            } while (true);
            loop_peg_853:
            ;
            rest = result_peg_849.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_849.setValue(value);
            }
        
        
    }
    
    if (column_peg_848.chunk10 == 0){
        column_peg_848.chunk10 = new Chunk10();
    }
    column_peg_848.chunk10->chunk_key_value_list = result_peg_849;
    stream.update(result_peg_849.getPosition());
    
    
    return result_peg_849;
    out_peg_851:
    
    if (column_peg_848.chunk10 == 0){
        column_peg_848.chunk10 = new Chunk10();
    }
    column_peg_848.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_858 = stream.getColumn(position);
    if (column_peg_858.chunk10 != 0 && column_peg_858.chunk10->chunk_key.calculated()){
        return column_peg_858.chunk10->chunk_key;
    }
    
    int myposition = position;
    
    Value ok;
    Result result_peg_859(myposition);
    
    {
    
        result_peg_859 = rule_key_real(stream, result_peg_859.getPosition());
            if (result_peg_859.error()){
                goto out_peg_861;
            }
        
        Result result_peg_860 = result_peg_859;
        
        result_peg_859 = rule_key_rest(stream, result_peg_859.getPosition(), result_peg_860.getValues());
            if (result_peg_859.error()){
                goto out_peg_861;
            }
            ok = result_peg_859.getValues();
        
        
        
        Result result_peg_865(result_peg_859);
            result_peg_865 = rule_identifier(stream, result_peg_865.getPosition());
            if (result_peg_865.error()){
                goto not_peg_864;
            }
            goto out_peg_861;
            not_peg_864:
            result_peg_859.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_859.setValue(value);
            }
        
        
    }
    
    if (column_peg_858.chunk10 == 0){
        column_peg_858.chunk10 = new Chunk10();
    }
    column_peg_858.chunk10->chunk_key = result_peg_859;
    stream.update(result_peg_859.getPosition());
    
    
    return result_peg_859;
    out_peg_861:
    
    if (column_peg_858.chunk10 == 0){
        column_peg_858.chunk10 = new Chunk10();
    }
    column_peg_858.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_868:
    Value another;
    Value new_left;
    Result result_peg_867(myposition);
    {
    
        result_peg_867 = rule_s(stream, result_peg_867.getPosition());
            if (result_peg_867.error()){
                goto out_peg_870;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_867.getPosition()))){
                    result_peg_867.nextPosition();
                } else {
                    goto out_peg_870;
                }
            }
            result_peg_867.setValue((void*) "+");
        
        
        
        result_peg_867 = rule_key_real(stream, result_peg_867.getPosition());
            if (result_peg_867.error()){
                goto out_peg_870;
            }
            another = result_peg_867.getValues();
        
        
        
        {
                Value value((void*) 0);
                std::ostringstream out;
                        out << as<Ast::Identifier*>(left)->toString();
                        out << "+";
                        out << as<Ast::Identifier*>(another)->toString();
                        value = makeSimpleIdentifier(out.str());
                result_peg_867.setValue(value);
            }
            new_left = result_peg_867.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_867.getPosition();
    goto tail_peg_868;
    out_peg_870:
    Result result_peg_874(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_874.setValue(value);
            }
        
        
    }
    
    
    return result_peg_874;
    
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_876 = stream.getColumn(position);
    if (column_peg_876.chunk11 != 0 && column_peg_876.chunk11->chunk_key_real.calculated()){
        return column_peg_876.chunk11->chunk_key_real;
    }
    
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_877(myposition);
    
    {
    
        result_peg_877.reset();
            do{
                Result result_peg_880(result_peg_877.getPosition());
                result_peg_880 = rule_key_modifier(stream, result_peg_880.getPosition());
                if (result_peg_880.error()){
                    goto loop_peg_879;
                }
                result_peg_877.addResult(result_peg_880);
            } while (true);
            loop_peg_879:
            ;
            mods = result_peg_877.getValues();
        
        
        
        result_peg_877 = rule_key_name(stream, result_peg_877.getPosition());
            if (result_peg_877.error()){
                goto out_peg_882;
            }
            name = result_peg_877.getValues();
        
        
        
        {
                Value value((void*) 0);
                std::ostringstream out;
                        out << combineStrings(mods);
                        out << as<const char *>(name);
                        value = makeSimpleIdentifier(out.str());
                result_peg_877.setValue(value);
            }
        
        
    }
    
    if (column_peg_876.chunk11 == 0){
        column_peg_876.chunk11 = new Chunk11();
    }
    column_peg_876.chunk11->chunk_key_real = result_peg_877;
    stream.update(result_peg_877.getPosition());
    
    
    return result_peg_877;
    out_peg_882:
    
    if (column_peg_876.chunk11 == 0){
        column_peg_876.chunk11 = new Chunk11();
    }
    column_peg_876.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_883 = stream.getColumn(position);
    if (column_peg_883.chunk11 != 0 && column_peg_883.chunk11->chunk_key_modifier.calculated()){
        return column_peg_883.chunk11->chunk_key_modifier;
    }
    
    int myposition = position;
    
    
    Result result_peg_884(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_884.getPosition()))){
                    result_peg_884.nextPosition();
                } else {
                    goto out_peg_886;
                }
            }
            result_peg_884.setValue((void*) "~");
        
        
        
        result_peg_884.reset();
            do{
                Result result_peg_889(result_peg_884.getPosition());
                {
                    
                    char letter_peg_894 = stream.get(result_peg_889.getPosition());
                    if (letter_peg_894 != '\0' && strchr("0123456789", letter_peg_894) != NULL){
                        result_peg_889.nextPosition();
                        result_peg_889.setValue((void*) letter_peg_894);
                    } else {
                        goto out_peg_893;
                    }
                    
                }
                goto success_peg_890;
                out_peg_893:
                goto loop_peg_888;
                success_peg_890:
                ;
                result_peg_884.addResult(result_peg_889);
            } while (true);
            loop_peg_888:
            ;
        
        Result result_peg_887 = result_peg_884;
        
        {
                Value value((void*) 0);
                value = toString('~', result_peg_887.getValues());
                result_peg_884.setValue(value);
            }
        
        
    }
    
    if (column_peg_883.chunk11 == 0){
        column_peg_883.chunk11 = new Chunk11();
    }
    column_peg_883.chunk11->chunk_key_modifier = result_peg_884;
    stream.update(result_peg_884.getPosition());
    
    
    return result_peg_884;
    out_peg_886:
    Result result_peg_895(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_895.getPosition()))){
                    result_peg_895.nextPosition();
                } else {
                    goto out_peg_897;
                }
            }
            result_peg_895.setValue((void*) "$");
        
        Result result_peg_896 = result_peg_895;
        
        {
                Value value((void*) 0);
                value = toString(as<const char *>(result_peg_896.getValues()));
                result_peg_895.setValue(value);
            }
        
        
    }
    
    if (column_peg_883.chunk11 == 0){
        column_peg_883.chunk11 = new Chunk11();
    }
    column_peg_883.chunk11->chunk_key_modifier = result_peg_895;
    stream.update(result_peg_895.getPosition());
    
    
    return result_peg_895;
    out_peg_897:
    Result result_peg_898(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_898.getPosition()))){
                    result_peg_898.nextPosition();
                } else {
                    goto out_peg_900;
                }
            }
            result_peg_898.setValue((void*) "/");
        
        Result result_peg_899 = result_peg_898;
        
        {
                Value value((void*) 0);
                value = toString(as<const char *>(result_peg_899.getValues()));
                result_peg_898.setValue(value);
            }
        
        
    }
    
    if (column_peg_883.chunk11 == 0){
        column_peg_883.chunk11 = new Chunk11();
    }
    column_peg_883.chunk11->chunk_key_modifier = result_peg_898;
    stream.update(result_peg_898.getPosition());
    
    
    return result_peg_898;
    out_peg_900:
    Result result_peg_901(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_901.getPosition()))){
                    result_peg_901.nextPosition();
                } else {
                    goto out_peg_903;
                }
            }
            result_peg_901.setValue((void*) ">");
        
        Result result_peg_902 = result_peg_901;
        
        {
                Value value((void*) 0);
                value = toString(as<const char *>(result_peg_902.getValues()));
                result_peg_901.setValue(value);
            }
        
        
    }
    
    if (column_peg_883.chunk11 == 0){
        column_peg_883.chunk11 = new Chunk11();
    }
    column_peg_883.chunk11->chunk_key_modifier = result_peg_901;
    stream.update(result_peg_901.getPosition());
    
    
    return result_peg_901;
    out_peg_903:
    
    if (column_peg_883.chunk11 == 0){
        column_peg_883.chunk11 = new Chunk11();
    }
    column_peg_883.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_904 = stream.getColumn(position);
    if (column_peg_904.chunk11 != 0 && column_peg_904.chunk11->chunk_key_name.calculated()){
        return column_peg_904.chunk11->chunk_key_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_905(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_905.getPosition()))){
            result_peg_905.nextPosition();
        } else {
            goto out_peg_906;
        }
    }
    result_peg_905.setValue((void*) "DB");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_905;
    stream.update(result_peg_905.getPosition());
    
    
    return result_peg_905;
    out_peg_906:
    Result result_peg_907(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_907.getPosition()))){
            result_peg_907.nextPosition();
        } else {
            goto out_peg_908;
        }
    }
    result_peg_907.setValue((void*) "B");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_907;
    stream.update(result_peg_907.getPosition());
    
    
    return result_peg_907;
    out_peg_908:
    Result result_peg_909(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_909.getPosition()))){
            result_peg_909.nextPosition();
        } else {
            goto out_peg_910;
        }
    }
    result_peg_909.setValue((void*) "DF");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_909;
    stream.update(result_peg_909.getPosition());
    
    
    return result_peg_909;
    out_peg_910:
    Result result_peg_911(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_911.getPosition()))){
            result_peg_911.nextPosition();
        } else {
            goto out_peg_912;
        }
    }
    result_peg_911.setValue((void*) "D");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_911;
    stream.update(result_peg_911.getPosition());
    
    
    return result_peg_911;
    out_peg_912:
    Result result_peg_913(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_913.getPosition()))){
            result_peg_913.nextPosition();
        } else {
            goto out_peg_914;
        }
    }
    result_peg_913.setValue((void*) "F");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_913;
    stream.update(result_peg_913.getPosition());
    
    
    return result_peg_913;
    out_peg_914:
    Result result_peg_915(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_915.getPosition()))){
            result_peg_915.nextPosition();
        } else {
            goto out_peg_916;
        }
    }
    result_peg_915.setValue((void*) "UF");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_915;
    stream.update(result_peg_915.getPosition());
    
    
    return result_peg_915;
    out_peg_916:
    Result result_peg_917(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_917.getPosition()))){
            result_peg_917.nextPosition();
        } else {
            goto out_peg_918;
        }
    }
    result_peg_917.setValue((void*) "UB");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_917;
    stream.update(result_peg_917.getPosition());
    
    
    return result_peg_917;
    out_peg_918:
    Result result_peg_919(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_919.getPosition()))){
            result_peg_919.nextPosition();
        } else {
            goto out_peg_920;
        }
    }
    result_peg_919.setValue((void*) "U");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_919;
    stream.update(result_peg_919.getPosition());
    
    
    return result_peg_919;
    out_peg_920:
    Result result_peg_921(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_921.getPosition()))){
            result_peg_921.nextPosition();
        } else {
            goto out_peg_922;
        }
    }
    result_peg_921.setValue((void*) "a");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_921;
    stream.update(result_peg_921.getPosition());
    
    
    return result_peg_921;
    out_peg_922:
    Result result_peg_923(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_923.getPosition()))){
            result_peg_923.nextPosition();
        } else {
            goto out_peg_924;
        }
    }
    result_peg_923.setValue((void*) "b");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_923;
    stream.update(result_peg_923.getPosition());
    
    
    return result_peg_923;
    out_peg_924:
    Result result_peg_925(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_925.getPosition()))){
            result_peg_925.nextPosition();
        } else {
            goto out_peg_926;
        }
    }
    result_peg_925.setValue((void*) "c");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_925;
    stream.update(result_peg_925.getPosition());
    
    
    return result_peg_925;
    out_peg_926:
    Result result_peg_927(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_927.getPosition()))){
            result_peg_927.nextPosition();
        } else {
            goto out_peg_928;
        }
    }
    result_peg_927.setValue((void*) "x");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_927;
    stream.update(result_peg_927.getPosition());
    
    
    return result_peg_927;
    out_peg_928:
    Result result_peg_929(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_929.getPosition()))){
            result_peg_929.nextPosition();
        } else {
            goto out_peg_930;
        }
    }
    result_peg_929.setValue((void*) "y");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_929;
    stream.update(result_peg_929.getPosition());
    
    
    return result_peg_929;
    out_peg_930:
    Result result_peg_931(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_931.getPosition()))){
            result_peg_931.nextPosition();
        } else {
            goto out_peg_932;
        }
    }
    result_peg_931.setValue((void*) "z");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_931;
    stream.update(result_peg_931.getPosition());
    
    
    return result_peg_931;
    out_peg_932:
    Result result_peg_933(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_933.getPosition()))){
            result_peg_933.nextPosition();
        } else {
            goto out_peg_934;
        }
    }
    result_peg_933.setValue((void*) "s");
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = result_peg_933;
    stream.update(result_peg_933.getPosition());
    
    
    return result_peg_933;
    out_peg_934:
    
    if (column_peg_904.chunk11 == 0){
        column_peg_904.chunk11 = new Chunk11();
    }
    column_peg_904.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_935 = stream.getColumn(position);
    if (column_peg_935.chunk11 != 0 && column_peg_935.chunk11->chunk_value.calculated()){
        return column_peg_935.chunk11->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_936(myposition);
    
    result_peg_936 = rule_float(stream, result_peg_936.getPosition());
    if (result_peg_936.error()){
        goto out_peg_937;
    }
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = result_peg_936;
    stream.update(result_peg_936.getPosition());
    
    
    return result_peg_936;
    out_peg_937:
    Result result_peg_938(myposition);
    
    result_peg_938 = rule_integer(stream, result_peg_938.getPosition());
    if (result_peg_938.error()){
        goto out_peg_939;
    }
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = result_peg_938;
    stream.update(result_peg_938.getPosition());
    
    
    return result_peg_938;
    out_peg_939:
    Result result_peg_940(myposition);
    
    result_peg_940 = rule_keyword(stream, result_peg_940.getPosition());
    if (result_peg_940.error()){
        goto out_peg_941;
    }
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = result_peg_940;
    stream.update(result_peg_940.getPosition());
    
    
    return result_peg_940;
    out_peg_941:
    Result result_peg_942(myposition);
    
    {
    
        Result result_peg_945(result_peg_942);
            result_peg_945 = rule_keyword(stream, result_peg_945.getPosition());
            if (result_peg_945.error()){
                goto not_peg_944;
            }
            goto out_peg_946;
            not_peg_944:
            result_peg_942.setValue((void*)0);
        
        
        
        result_peg_942 = rule_identifier(stream, result_peg_942.getPosition());
            if (result_peg_942.error()){
                goto out_peg_946;
            }
        
        
    }
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = result_peg_942;
    stream.update(result_peg_942.getPosition());
    
    
    return result_peg_942;
    out_peg_946:
    Result result_peg_947(myposition);
    
    result_peg_947 = rule_range(stream, result_peg_947.getPosition());
    if (result_peg_947.error()){
        goto out_peg_948;
    }
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = result_peg_947;
    stream.update(result_peg_947.getPosition());
    
    
    return result_peg_947;
    out_peg_948:
    Result result_peg_949(myposition);
    
    result_peg_949 = rule_string(stream, result_peg_949.getPosition());
    if (result_peg_949.error()){
        goto out_peg_950;
    }
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = result_peg_949;
    stream.update(result_peg_949.getPosition());
    
    
    return result_peg_949;
    out_peg_950:
    
    if (column_peg_935.chunk11 == 0){
        column_peg_935.chunk11 = new Chunk11();
    }
    column_peg_935.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_951 = stream.getColumn(position);
    if (column_peg_951.chunk11 != 0 && column_peg_951.chunk11->chunk_keyword.calculated()){
        return column_peg_951.chunk11->chunk_keyword;
    }
    
    int myposition = position;
    
    
    Result result_peg_952(myposition);
    
    {
    
        result_peg_952 = rule_keyword_real(stream, result_peg_952.getPosition());
            if (result_peg_952.error()){
                goto out_peg_954;
            }
        
        Result result_peg_953 = result_peg_952;
        
        Result result_peg_957(result_peg_952);
            result_peg_957 = rule_alpha_digit(stream, result_peg_957.getPosition());
            if (result_peg_957.error()){
                goto not_peg_956;
            }
            goto out_peg_954;
            not_peg_956:
            result_peg_952.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_953.getValues();
                result_peg_952.setValue(value);
            }
        
        
    }
    
    if (column_peg_951.chunk11 == 0){
        column_peg_951.chunk11 = new Chunk11();
    }
    column_peg_951.chunk11->chunk_keyword = result_peg_952;
    stream.update(result_peg_952.getPosition());
    
    
    return result_peg_952;
    out_peg_954:
    
    if (column_peg_951.chunk11 == 0){
        column_peg_951.chunk11 = new Chunk11();
    }
    column_peg_951.chunk11->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_958 = stream.getColumn(position);
    if (column_peg_958.chunk12 != 0 && column_peg_958.chunk12->chunk_keyword_real.calculated()){
        return column_peg_958.chunk12->chunk_keyword_real;
    }
    
    int myposition = position;
    
    
    Result result_peg_959(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_959.getPosition()))){
                    result_peg_959.nextPosition();
                } else {
                    goto out_peg_961;
                }
            }
            result_peg_959.setValue((void*) "vel");
        
        
        
        result_peg_959 = rule_s(stream, result_peg_959.getPosition());
            if (result_peg_959.error()){
                goto out_peg_961;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_959.getPosition()))){
                    result_peg_959.nextPosition();
                } else {
                    goto out_peg_961;
                }
            }
            result_peg_959.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_959.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_959;
    stream.update(result_peg_959.getPosition());
    
    
    return result_peg_959;
    out_peg_961:
    Result result_peg_964(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_964.getPosition()))){
                    result_peg_964.nextPosition();
                } else {
                    goto out_peg_966;
                }
            }
            result_peg_964.setValue((void*) "vel");
        
        
        
        result_peg_964 = rule_s(stream, result_peg_964.getPosition());
            if (result_peg_964.error()){
                goto out_peg_966;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_964.getPosition()))){
                    result_peg_964.nextPosition();
                } else {
                    goto out_peg_966;
                }
            }
            result_peg_964.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_964.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_964;
    stream.update(result_peg_964.getPosition());
    
    
    return result_peg_964;
    out_peg_966:
    Result result_peg_969(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_969.getPosition()))){
                    result_peg_969.nextPosition();
                } else {
                    goto out_peg_971;
                }
            }
            result_peg_969.setValue((void*) "pos");
        
        
        
        result_peg_969 = rule_s(stream, result_peg_969.getPosition());
            if (result_peg_969.error()){
                goto out_peg_971;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_969.getPosition()))){
                    result_peg_969.nextPosition();
                } else {
                    goto out_peg_971;
                }
            }
            result_peg_969.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_969.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_969;
    stream.update(result_peg_969.getPosition());
    
    
    return result_peg_969;
    out_peg_971:
    Result result_peg_974(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_974.getPosition()))){
                    result_peg_974.nextPosition();
                } else {
                    goto out_peg_976;
                }
            }
            result_peg_974.setValue((void*) "pos");
        
        
        
        result_peg_974 = rule_s(stream, result_peg_974.getPosition());
            if (result_peg_974.error()){
                goto out_peg_976;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_974.getPosition()))){
                    result_peg_974.nextPosition();
                } else {
                    goto out_peg_976;
                }
            }
            result_peg_974.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_974.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_974;
    stream.update(result_peg_974.getPosition());
    
    
    return result_peg_974;
    out_peg_976:
    Result result_peg_979(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_979.getPosition()))){
                    result_peg_979.nextPosition();
                } else {
                    goto out_peg_981;
                }
            }
            result_peg_979.setValue((void*) "p2dist");
        
        
        
        result_peg_979 = rule_s(stream, result_peg_979.getPosition());
            if (result_peg_979.error()){
                goto out_peg_981;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_979.getPosition()))){
                    result_peg_979.nextPosition();
                } else {
                    goto out_peg_981;
                }
            }
            result_peg_979.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_979.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_979;
    stream.update(result_peg_979.getPosition());
    
    
    return result_peg_979;
    out_peg_981:
    Result result_peg_984(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_984.getPosition()))){
                    result_peg_984.nextPosition();
                } else {
                    goto out_peg_986;
                }
            }
            result_peg_984.setValue((void*) "p2dist");
        
        
        
        result_peg_984 = rule_s(stream, result_peg_984.getPosition());
            if (result_peg_984.error()){
                goto out_peg_986;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_984.getPosition()))){
                    result_peg_984.nextPosition();
                } else {
                    goto out_peg_986;
                }
            }
            result_peg_984.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_984.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_984;
    stream.update(result_peg_984.getPosition());
    
    
    return result_peg_984;
    out_peg_986:
    Result result_peg_989(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_989.getPosition()))){
                    result_peg_989.nextPosition();
                } else {
                    goto out_peg_991;
                }
            }
            result_peg_989.setValue((void*) "p1dist");
        
        
        
        result_peg_989 = rule_s(stream, result_peg_989.getPosition());
            if (result_peg_989.error()){
                goto out_peg_991;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_989.getPosition()))){
                    result_peg_989.nextPosition();
                } else {
                    goto out_peg_991;
                }
            }
            result_peg_989.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_989.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_989;
    stream.update(result_peg_989.getPosition());
    
    
    return result_peg_989;
    out_peg_991:
    Result result_peg_994(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_994.getPosition()))){
                    result_peg_994.nextPosition();
                } else {
                    goto out_peg_996;
                }
            }
            result_peg_994.setValue((void*) "p1dist");
        
        
        
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
                value = makeKeyword("p1dist y");
                result_peg_994.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_994;
    stream.update(result_peg_994.getPosition());
    
    
    return result_peg_994;
    out_peg_996:
    Result result_peg_999(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_999.getPosition()))){
                    result_peg_999.nextPosition();
                } else {
                    goto out_peg_1001;
                }
            }
            result_peg_999.setValue((void*) "p2bodydist");
        
        
        
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
                value = makeKeyword("p2bodydist x");
                result_peg_999.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_999;
    stream.update(result_peg_999.getPosition());
    
    
    return result_peg_999;
    out_peg_1001:
    Result result_peg_1004(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1004.getPosition()))){
                    result_peg_1004.nextPosition();
                } else {
                    goto out_peg_1006;
                }
            }
            result_peg_1004.setValue((void*) "p2bodydist");
        
        
        
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
                value = makeKeyword("p2bodydist y");
                result_peg_1004.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_1004;
    stream.update(result_peg_1004.getPosition());
    
    
    return result_peg_1004;
    out_peg_1006:
    Result result_peg_1009(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1009.getPosition()))){
                    result_peg_1009.nextPosition();
                } else {
                    goto out_peg_1011;
                }
            }
            result_peg_1009.setValue((void*) "p1bodydist");
        
        
        
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
                value = makeKeyword("p1bodydist x");
                result_peg_1009.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_1009;
    stream.update(result_peg_1009.getPosition());
    
    
    return result_peg_1009;
    out_peg_1011:
    Result result_peg_1014(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1014 = rule_s(stream, result_peg_1014.getPosition());
            if (result_peg_1014.error()){
                goto out_peg_1016;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1014.getPosition()))){
                    result_peg_1014.nextPosition();
                } else {
                    goto out_peg_1016;
                }
            }
            result_peg_1014.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1014.setValue(value);
            }
        
        
    }
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = result_peg_1014;
    stream.update(result_peg_1014.getPosition());
    
    
    return result_peg_1014;
    out_peg_1016:
    
    if (column_peg_958.chunk12 == 0){
        column_peg_958.chunk12 = new Chunk12();
    }
    column_peg_958.chunk12->chunk_keyword_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

const void * main(const std::string & filename, bool stats = false){
    Stream stream(filename);
    errorResult.setError();
    Result done = rule_start(stream, 0);
    if (done.error()){
        std::cout << "Could not parse" << std::endl;
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
        std::cout << "Could not parse" << std::endl;
        throw ParseException(stream.reportError());
    }
    if (stats){
        stream.printStats();
    }
    return done.getValues().getValue();
}



    
    } /* Cmd */
    
} /* Mugen */

        
