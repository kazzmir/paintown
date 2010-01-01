

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
        stream.open(filename.c_str());
        if (stream.fail()){
            std::ostringstream out;
            out << "Cannot open '" << filename << "'";
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
        /* this works becuase as() will coerce a void* into Value(void*) */
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
                if (compareChar("command"[i], stream.get(result_peg_131.getPosition()))){
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
                if (compareChar("("[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) "(");
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        result_peg_145 = rule_integer(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
            index = result_peg_145.getValues();
        
        
        
        result_peg_145 = rule_s(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_145.getPosition()))){
                    result_peg_145.nextPosition();
                } else {
                    goto out_peg_147;
                }
            }
            result_peg_145.setValue((void*) ")");
        
        
        
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
        
        
        
        result_peg_145 = rule_expr(stream, result_peg_145.getPosition());
            if (result_peg_145.error()){
                goto out_peg_147;
            }
            exp = result_peg_145.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name, exp);
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
    
    if (column_peg_130.chunk2 == 0){
        column_peg_130.chunk2 = new Chunk2();
    }
    column_peg_130.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_158 = stream.getColumn(position);
    if (column_peg_158.chunk2 != 0 && column_peg_158.chunk2->chunk_identifier.calculated()){
        return column_peg_158.chunk2->chunk_identifier;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_159(myposition);
    
    {
    
        result_peg_159 = rule_name(stream, result_peg_159.getPosition());
            if (result_peg_159.error()){
                goto out_peg_161;
            }
            first = result_peg_159.getValues();
        
        
        
        result_peg_159.reset();
            do{
                Result result_peg_164(result_peg_159.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_164.getPosition()))){
                                result_peg_164.nextPosition();
                            } else {
                                goto loop_peg_163;
                            }
                        }
                        result_peg_164.setValue((void*) ".");
                    
                    
                    
                    result_peg_164 = rule_name(stream, result_peg_164.getPosition());
                        if (result_peg_164.error()){
                            goto loop_peg_163;
                        }
                    
                    
                }
                result_peg_159.addResult(result_peg_164);
            } while (true);
            loop_peg_163:
            ;
            rest = result_peg_159.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_159.setValue(value);
            }
        
        
    }
    
    if (column_peg_158.chunk2 == 0){
        column_peg_158.chunk2 = new Chunk2();
    }
    column_peg_158.chunk2->chunk_identifier = result_peg_159;
    stream.update(result_peg_159.getPosition());
    
    
    return result_peg_159;
    out_peg_161:
    
    if (column_peg_158.chunk2 == 0){
        column_peg_158.chunk2 = new Chunk2();
    }
    column_peg_158.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_166 = stream.getColumn(position);
    if (column_peg_166.chunk2 != 0 && column_peg_166.chunk2->chunk_integer.calculated()){
        return column_peg_166.chunk2->chunk_integer;
    }
    
    int myposition = position;
    
    
    Result result_peg_167(myposition);
    
    {
    
        int save_peg_169 = result_peg_167.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_167.getPosition()))){
                        result_peg_167.nextPosition();
                    } else {
                        goto out_peg_172;
                    }
                }
                result_peg_167.setValue((void*) "-");
                    
            }
            goto success_peg_170;
            out_peg_172:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_167.getPosition()))){
                        result_peg_167.nextPosition();
                    } else {
                        goto out_peg_174;
                    }
                }
                result_peg_167.setValue((void*) "+");
                    
            }
            goto success_peg_170;
            out_peg_174:
            
            result_peg_167 = Result(save_peg_169);
            result_peg_167.setValue((void*) 0);
            
            success_peg_170:
            ;
        
        Result result_peg_168 = result_peg_167;
        
        result_peg_167.reset();
            do{
                Result result_peg_177(result_peg_167.getPosition());
                {
                    
                    char letter_peg_182 = stream.get(result_peg_177.getPosition());
                    if (letter_peg_182 != '\0' && strchr("0123456789", letter_peg_182) != NULL){
                        result_peg_177.nextPosition();
                        result_peg_177.setValue((void*) letter_peg_182);
                    } else {
                        goto out_peg_181;
                    }
                    
                }
                goto success_peg_178;
                out_peg_181:
                goto loop_peg_176;
                success_peg_178:
                ;
                result_peg_167.addResult(result_peg_177);
            } while (true);
            loop_peg_176:
            if (result_peg_167.matches() == 0){
                goto out_peg_183;
            }
        
        Result result_peg_175 = result_peg_167;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_168.getValues(), result_peg_175.getValues());
                result_peg_167.setValue(value);
            }
        
        
    }
    
    if (column_peg_166.chunk2 == 0){
        column_peg_166.chunk2 = new Chunk2();
    }
    column_peg_166.chunk2->chunk_integer = result_peg_167;
    stream.update(result_peg_167.getPosition());
    
    
    return result_peg_167;
    out_peg_183:
    
    if (column_peg_166.chunk2 == 0){
        column_peg_166.chunk2 = new Chunk2();
    }
    column_peg_166.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_184 = stream.getColumn(position);
    if (column_peg_184.chunk2 != 0 && column_peg_184.chunk2->chunk_float.calculated()){
        return column_peg_184.chunk2->chunk_float;
    }
    
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_185(myposition);
    
    {
    
        int save_peg_187 = result_peg_185.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_185.getPosition()))){
                        result_peg_185.nextPosition();
                    } else {
                        goto out_peg_190;
                    }
                }
                result_peg_185.setValue((void*) "-");
                    
            }
            goto success_peg_188;
            out_peg_190:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_185.getPosition()))){
                        result_peg_185.nextPosition();
                    } else {
                        goto out_peg_192;
                    }
                }
                result_peg_185.setValue((void*) "+");
                    
            }
            goto success_peg_188;
            out_peg_192:
            
            result_peg_185 = Result(save_peg_187);
            result_peg_185.setValue((void*) 0);
            
            success_peg_188:
            ;
        
        Result result_peg_186 = result_peg_185;
        
        result_peg_185.reset();
            do{
                Result result_peg_195(result_peg_185.getPosition());
                {
                    
                    char letter_peg_200 = stream.get(result_peg_195.getPosition());
                    if (letter_peg_200 != '\0' && strchr("0123456789", letter_peg_200) != NULL){
                        result_peg_195.nextPosition();
                        result_peg_195.setValue((void*) letter_peg_200);
                    } else {
                        goto out_peg_199;
                    }
                    
                }
                goto success_peg_196;
                out_peg_199:
                goto loop_peg_194;
                success_peg_196:
                ;
                result_peg_185.addResult(result_peg_195);
            } while (true);
            loop_peg_194:
            ;
            left = result_peg_185.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_185.getPosition()))){
                    result_peg_185.nextPosition();
                } else {
                    goto out_peg_202;
                }
            }
            result_peg_185.setValue((void*) ".");
        
        
        
        result_peg_185.reset();
            do{
                Result result_peg_205(result_peg_185.getPosition());
                {
                    
                    char letter_peg_210 = stream.get(result_peg_205.getPosition());
                    if (letter_peg_210 != '\0' && strchr("0123456789", letter_peg_210) != NULL){
                        result_peg_205.nextPosition();
                        result_peg_205.setValue((void*) letter_peg_210);
                    } else {
                        goto out_peg_209;
                    }
                    
                }
                goto success_peg_206;
                out_peg_209:
                goto loop_peg_204;
                success_peg_206:
                ;
                result_peg_185.addResult(result_peg_205);
            } while (true);
            loop_peg_204:
            if (result_peg_185.matches() == 0){
                goto out_peg_202;
            }
            right = result_peg_185.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_186.getValues(), parseDouble(left,right));
                result_peg_185.setValue(value);
            }
        
        
    }
    
    if (column_peg_184.chunk2 == 0){
        column_peg_184.chunk2 = new Chunk2();
    }
    column_peg_184.chunk2->chunk_float = result_peg_185;
    stream.update(result_peg_185.getPosition());
    
    
    return result_peg_185;
    out_peg_202:
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
                        result_peg_221.setValue((void*) letter_peg_226);
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
            left = result_peg_211.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_211.getPosition()))){
                    result_peg_211.nextPosition();
                } else {
                    goto out_peg_227;
                }
            }
            result_peg_211.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_212.getValues(), parseDouble(left));
                result_peg_211.setValue(value);
            }
        
        
    }
    
    if (column_peg_184.chunk2 == 0){
        column_peg_184.chunk2 = new Chunk2();
    }
    column_peg_184.chunk2->chunk_float = result_peg_211;
    stream.update(result_peg_211.getPosition());
    
    
    return result_peg_211;
    out_peg_227:
    
    if (column_peg_184.chunk2 == 0){
        column_peg_184.chunk2 = new Chunk2();
    }
    column_peg_184.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_229 = stream.getColumn(position);
    if (column_peg_229.chunk2 != 0 && column_peg_229.chunk2->chunk_string.calculated()){
        return column_peg_229.chunk2->chunk_string;
    }
    
    int myposition = position;
    
    Value contents;
    Result result_peg_230(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_230.getPosition()))){
                    result_peg_230.nextPosition();
                } else {
                    goto out_peg_232;
                }
            }
            result_peg_230.setValue((void*) "\"");
        
        
        
        result_peg_230.reset();
            do{
                Result result_peg_235(result_peg_230.getPosition());
                {
                
                    Result result_peg_238(result_peg_235);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_238.getPosition()))){
                                result_peg_238.nextPosition();
                            } else {
                                goto not_peg_237;
                            }
                        }
                        result_peg_238.setValue((void*) "\"");
                        goto loop_peg_234;
                        not_peg_237:
                        result_peg_235.setValue((void*)0);
                    
                    
                    
                    Result result_peg_241(result_peg_235);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_241.getPosition()))){
                                result_peg_241.nextPosition();
                            } else {
                                goto not_peg_240;
                            }
                        }
                        result_peg_241.setValue((void*) "\n");
                        goto loop_peg_234;
                        not_peg_240:
                        result_peg_235.setValue((void*)0);
                    
                    
                    
                    char temp_peg_242 = stream.get(result_peg_235.getPosition());
                        if (temp_peg_242 != '\0'){
                            result_peg_235.setValue((void*) temp_peg_242);
                            result_peg_235.nextPosition();
                        } else {
                            goto loop_peg_234;
                        }
                    
                    
                }
                result_peg_230.addResult(result_peg_235);
            } while (true);
            loop_peg_234:
            ;
            contents = result_peg_230.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_230.getPosition()))){
                    result_peg_230.nextPosition();
                } else {
                    goto out_peg_232;
                }
            }
            result_peg_230.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_230.setValue(value);
            }
        
        
    }
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_string = result_peg_230;
    stream.update(result_peg_230.getPosition());
    
    
    return result_peg_230;
    out_peg_232:
    
    if (column_peg_229.chunk2 == 0){
        column_peg_229.chunk2 = new Chunk2();
    }
    column_peg_229.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_244 = stream.getColumn(position);
    if (column_peg_244.chunk3 != 0 && column_peg_244.chunk3->chunk_range.calculated()){
        return column_peg_244.chunk3->chunk_range;
    }
    
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_245(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_245.getPosition()))){
                    result_peg_245.nextPosition();
                } else {
                    goto out_peg_247;
                }
            }
            result_peg_245.setValue((void*) "[");
        
        
        
        result_peg_245 = rule_s(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_247;
            }
        
        
        
        result_peg_245 = rule_value(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_247;
            }
            low = result_peg_245.getValues();
        
        
        
        result_peg_245 = rule_s(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_247;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_245.getPosition()))){
                    result_peg_245.nextPosition();
                } else {
                    goto out_peg_247;
                }
            }
            result_peg_245.setValue((void*) ",");
        
        
        
        result_peg_245 = rule_s(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_247;
            }
        
        
        
        result_peg_245 = rule_value(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_247;
            }
            high = result_peg_245.getValues();
        
        
        
        result_peg_245 = rule_s(stream, result_peg_245.getPosition());
            if (result_peg_245.error()){
                goto out_peg_247;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_245.getPosition()))){
                    result_peg_245.nextPosition();
                } else {
                    goto out_peg_247;
                }
            }
            result_peg_245.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_245.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_range = result_peg_245;
    stream.update(result_peg_245.getPosition());
    
    
    return result_peg_245;
    out_peg_247:
    Result result_peg_256(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_256.getPosition()))){
                    result_peg_256.nextPosition();
                } else {
                    goto out_peg_258;
                }
            }
            result_peg_256.setValue((void*) "(");
        
        
        
        result_peg_256 = rule_s(stream, result_peg_256.getPosition());
            if (result_peg_256.error()){
                goto out_peg_258;
            }
        
        
        
        result_peg_256 = rule_value(stream, result_peg_256.getPosition());
            if (result_peg_256.error()){
                goto out_peg_258;
            }
            low = result_peg_256.getValues();
        
        
        
        result_peg_256 = rule_s(stream, result_peg_256.getPosition());
            if (result_peg_256.error()){
                goto out_peg_258;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_256.getPosition()))){
                    result_peg_256.nextPosition();
                } else {
                    goto out_peg_258;
                }
            }
            result_peg_256.setValue((void*) ",");
        
        
        
        result_peg_256 = rule_s(stream, result_peg_256.getPosition());
            if (result_peg_256.error()){
                goto out_peg_258;
            }
        
        
        
        result_peg_256 = rule_value(stream, result_peg_256.getPosition());
            if (result_peg_256.error()){
                goto out_peg_258;
            }
            high = result_peg_256.getValues();
        
        
        
        result_peg_256 = rule_s(stream, result_peg_256.getPosition());
            if (result_peg_256.error()){
                goto out_peg_258;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_256.getPosition()))){
                    result_peg_256.nextPosition();
                } else {
                    goto out_peg_258;
                }
            }
            result_peg_256.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_256.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_range = result_peg_256;
    stream.update(result_peg_256.getPosition());
    
    
    return result_peg_256;
    out_peg_258:
    Result result_peg_267(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_267.getPosition()))){
                    result_peg_267.nextPosition();
                } else {
                    goto out_peg_269;
                }
            }
            result_peg_267.setValue((void*) "(");
        
        
        
        result_peg_267 = rule_s(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
        
        
        
        result_peg_267 = rule_value(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
            low = result_peg_267.getValues();
        
        
        
        result_peg_267 = rule_s(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_267.getPosition()))){
                    result_peg_267.nextPosition();
                } else {
                    goto out_peg_269;
                }
            }
            result_peg_267.setValue((void*) ",");
        
        
        
        result_peg_267 = rule_s(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
        
        
        
        result_peg_267 = rule_value(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
            high = result_peg_267.getValues();
        
        
        
        result_peg_267 = rule_s(stream, result_peg_267.getPosition());
            if (result_peg_267.error()){
                goto out_peg_269;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_267.getPosition()))){
                    result_peg_267.nextPosition();
                } else {
                    goto out_peg_269;
                }
            }
            result_peg_267.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_267.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_range = result_peg_267;
    stream.update(result_peg_267.getPosition());
    
    
    return result_peg_267;
    out_peg_269:
    Result result_peg_278(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_278.getPosition()))){
                    result_peg_278.nextPosition();
                } else {
                    goto out_peg_280;
                }
            }
            result_peg_278.setValue((void*) "[");
        
        
        
        result_peg_278 = rule_s(stream, result_peg_278.getPosition());
            if (result_peg_278.error()){
                goto out_peg_280;
            }
        
        
        
        result_peg_278 = rule_value(stream, result_peg_278.getPosition());
            if (result_peg_278.error()){
                goto out_peg_280;
            }
            low = result_peg_278.getValues();
        
        
        
        result_peg_278 = rule_s(stream, result_peg_278.getPosition());
            if (result_peg_278.error()){
                goto out_peg_280;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_278.getPosition()))){
                    result_peg_278.nextPosition();
                } else {
                    goto out_peg_280;
                }
            }
            result_peg_278.setValue((void*) ",");
        
        
        
        result_peg_278 = rule_s(stream, result_peg_278.getPosition());
            if (result_peg_278.error()){
                goto out_peg_280;
            }
        
        
        
        result_peg_278 = rule_value(stream, result_peg_278.getPosition());
            if (result_peg_278.error()){
                goto out_peg_280;
            }
            high = result_peg_278.getValues();
        
        
        
        result_peg_278 = rule_s(stream, result_peg_278.getPosition());
            if (result_peg_278.error()){
                goto out_peg_280;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_278.getPosition()))){
                    result_peg_278.nextPosition();
                } else {
                    goto out_peg_280;
                }
            }
            result_peg_278.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_278.setValue(value);
            }
        
        
    }
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_range = result_peg_278;
    stream.update(result_peg_278.getPosition());
    
    
    return result_peg_278;
    out_peg_280:
    
    if (column_peg_244.chunk3 == 0){
        column_peg_244.chunk3 = new Chunk3();
    }
    column_peg_244.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_289 = stream.getColumn(position);
    if (column_peg_289.chunk3 != 0 && column_peg_289.chunk3->chunk_name.calculated()){
        return column_peg_289.chunk3->chunk_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_290(myposition);
    
    {
    
        {
                
                char letter_peg_296 = stream.get(result_peg_290.getPosition());
                if (letter_peg_296 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_296) != NULL){
                    result_peg_290.nextPosition();
                    result_peg_290.setValue((void*) letter_peg_296);
                } else {
                    goto out_peg_295;
                }
                
            }
            goto success_peg_292;
            out_peg_295:
            goto out_peg_297;
            success_peg_292:
            ;
        
        Result result_peg_291 = result_peg_290;
        
        result_peg_290.reset();
            do{
                Result result_peg_300(result_peg_290.getPosition());
                result_peg_300 = rule_alpha_digit(stream, result_peg_300.getPosition());
                if (result_peg_300.error()){
                    goto loop_peg_299;
                }
                result_peg_290.addResult(result_peg_300);
            } while (true);
            loop_peg_299:
            ;
        
        Result result_peg_298 = result_peg_290;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_291.getValues().getValue(),result_peg_298.getValues());
                result_peg_290.setValue(value);
            }
        
        
    }
    
    if (column_peg_289.chunk3 == 0){
        column_peg_289.chunk3 = new Chunk3();
    }
    column_peg_289.chunk3->chunk_name = result_peg_290;
    stream.update(result_peg_290.getPosition());
    
    
    return result_peg_290;
    out_peg_297:
    
    if (column_peg_289.chunk3 == 0){
        column_peg_289.chunk3 = new Chunk3();
    }
    column_peg_289.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_301 = stream.getColumn(position);
    if (column_peg_301.chunk3 != 0 && column_peg_301.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_301.chunk3->chunk_alpha_digit;
    }
    
    int myposition = position;
    
    
    Result result_peg_302(myposition);
    
    {
        
        char letter_peg_307 = stream.get(result_peg_302.getPosition());
        if (letter_peg_307 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_307) != NULL){
            result_peg_302.nextPosition();
            result_peg_302.setValue((void*) letter_peg_307);
        } else {
            goto out_peg_306;
        }
        
    }
    goto success_peg_303;
    out_peg_306:
    goto out_peg_308;
    success_peg_303:
    ;
    
    if (column_peg_301.chunk3 == 0){
        column_peg_301.chunk3 = new Chunk3();
    }
    column_peg_301.chunk3->chunk_alpha_digit = result_peg_302;
    stream.update(result_peg_302.getPosition());
    
    
    return result_peg_302;
    out_peg_308:
    Result result_peg_309(myposition);
    
    {
        
        char letter_peg_314 = stream.get(result_peg_309.getPosition());
        if (letter_peg_314 != '\0' && strchr("0123456789", letter_peg_314) != NULL){
            result_peg_309.nextPosition();
            result_peg_309.setValue((void*) letter_peg_314);
        } else {
            goto out_peg_313;
        }
        
    }
    goto success_peg_310;
    out_peg_313:
    goto out_peg_315;
    success_peg_310:
    ;
    
    if (column_peg_301.chunk3 == 0){
        column_peg_301.chunk3 = new Chunk3();
    }
    column_peg_301.chunk3->chunk_alpha_digit = result_peg_309;
    stream.update(result_peg_309.getPosition());
    
    
    return result_peg_309;
    out_peg_315:
    
    if (column_peg_301.chunk3 == 0){
        column_peg_301.chunk3 = new Chunk3();
    }
    column_peg_301.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_316 = stream.getColumn(position);
    if (column_peg_316.chunk3 != 0 && column_peg_316.chunk3->chunk_valuelist.calculated()){
        return column_peg_316.chunk3->chunk_valuelist;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_317(myposition);
    
    {
    
        result_peg_317 = rule_expr_c(stream, result_peg_317.getPosition());
            if (result_peg_317.error()){
                goto out_peg_319;
            }
            first = result_peg_317.getValues();
        
        
        
        result_peg_317.reset();
            do{
                Result result_peg_322(result_peg_317.getPosition());
                {
                
                    result_peg_322 = rule_s(stream, result_peg_322.getPosition());
                        if (result_peg_322.error()){
                            goto loop_peg_321;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_322.getPosition()))){
                                result_peg_322.nextPosition();
                            } else {
                                goto loop_peg_321;
                            }
                        }
                        result_peg_322.setValue((void*) ",");
                    
                    
                    
                    result_peg_322 = rule_s(stream, result_peg_322.getPosition());
                        if (result_peg_322.error()){
                            goto loop_peg_321;
                        }
                    
                    
                    
                    result_peg_322 = rule_expr_c(stream, result_peg_322.getPosition());
                        if (result_peg_322.error()){
                            goto loop_peg_321;
                        }
                    
                    
                }
                result_peg_317.addResult(result_peg_322);
            } while (true);
            loop_peg_321:
            if (result_peg_317.matches() == 0){
                goto out_peg_319;
            }
            rest = result_peg_317.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_317.setValue(value);
            }
        
        
    }
    
    if (column_peg_316.chunk3 == 0){
        column_peg_316.chunk3 = new Chunk3();
    }
    column_peg_316.chunk3->chunk_valuelist = result_peg_317;
    stream.update(result_peg_317.getPosition());
    
    
    return result_peg_317;
    out_peg_319:
    Result result_peg_326(myposition);
    
    {
    
        result_peg_326 = rule_s(stream, result_peg_326.getPosition());
            if (result_peg_326.error()){
                goto out_peg_328;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_326.getPosition()))){
                    result_peg_326.nextPosition();
                } else {
                    goto out_peg_328;
                }
            }
            result_peg_326.setValue((void*) ",");
        
        
        
        result_peg_326 = rule_s(stream, result_peg_326.getPosition());
            if (result_peg_326.error()){
                goto out_peg_328;
            }
        
        
        
        result_peg_326 = rule_expr_c(stream, result_peg_326.getPosition());
            if (result_peg_326.error()){
                goto out_peg_328;
            }
            first = result_peg_326.getValues();
        
        
        
        result_peg_326.reset();
            do{
                Result result_peg_334(result_peg_326.getPosition());
                {
                
                    result_peg_334 = rule_s(stream, result_peg_334.getPosition());
                        if (result_peg_334.error()){
                            goto loop_peg_333;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_334.getPosition()))){
                                result_peg_334.nextPosition();
                            } else {
                                goto loop_peg_333;
                            }
                        }
                        result_peg_334.setValue((void*) ",");
                    
                    
                    
                    result_peg_334 = rule_s(stream, result_peg_334.getPosition());
                        if (result_peg_334.error()){
                            goto loop_peg_333;
                        }
                    
                    
                    
                    result_peg_334 = rule_expr_c(stream, result_peg_334.getPosition());
                        if (result_peg_334.error()){
                            goto loop_peg_333;
                        }
                    
                    
                }
                result_peg_326.addResult(result_peg_334);
            } while (true);
            loop_peg_333:
            ;
            rest = result_peg_326.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_326.setValue(value);
            }
        
        
    }
    
    if (column_peg_316.chunk3 == 0){
        column_peg_316.chunk3 = new Chunk3();
    }
    column_peg_316.chunk3->chunk_valuelist = result_peg_326;
    stream.update(result_peg_326.getPosition());
    
    
    return result_peg_326;
    out_peg_328:
    
    if (column_peg_316.chunk3 == 0){
        column_peg_316.chunk3 = new Chunk3();
    }
    column_peg_316.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_338 = stream.getColumn(position);
    if (column_peg_338.chunk3 != 0 && column_peg_338.chunk3->chunk_expr.calculated()){
        return column_peg_338.chunk3->chunk_expr;
    }
    
    int myposition = position;
    
    
    Result result_peg_339(myposition);
    
    {
    
        result_peg_339 = rule_expr_c(stream, result_peg_339.getPosition());
            if (result_peg_339.error()){
                goto out_peg_341;
            }
        
        Result result_peg_340 = result_peg_339;
        
        Result result_peg_344(result_peg_339);
            {
            
                result_peg_344 = rule_s(stream, result_peg_344.getPosition());
                    if (result_peg_344.error()){
                        goto not_peg_343;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_344.getPosition()))){
                            result_peg_344.nextPosition();
                        } else {
                            goto not_peg_343;
                        }
                    }
                    result_peg_344.setValue((void*) ",");
                
                
            }
            goto out_peg_341;
            not_peg_343:
            result_peg_339.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_340.getValues();
                result_peg_339.setValue(value);
            }
        
        
    }
    
    if (column_peg_338.chunk3 == 0){
        column_peg_338.chunk3 = new Chunk3();
    }
    column_peg_338.chunk3->chunk_expr = result_peg_339;
    stream.update(result_peg_339.getPosition());
    
    
    return result_peg_339;
    out_peg_341:
    Result result_peg_346(myposition);
    
    {
    
        result_peg_346 = rule_valuelist(stream, result_peg_346.getPosition());
            if (result_peg_346.error()){
                goto out_peg_348;
            }
        
        Result result_peg_347 = result_peg_346;
        
        result_peg_346 = rule_expr2_rest(stream, result_peg_346.getPosition(), result_peg_347.getValues());
            if (result_peg_346.error()){
                goto out_peg_348;
            }
        
        
    }
    
    if (column_peg_338.chunk3 == 0){
        column_peg_338.chunk3 = new Chunk3();
    }
    column_peg_338.chunk3->chunk_expr = result_peg_346;
    stream.update(result_peg_346.getPosition());
    
    
    return result_peg_346;
    out_peg_348:
    
    if (column_peg_338.chunk3 == 0){
        column_peg_338.chunk3 = new Chunk3();
    }
    column_peg_338.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_349 = stream.getColumn(position);
    if (column_peg_349.chunk4 != 0 && column_peg_349.chunk4->chunk_expr_c.calculated()){
        return column_peg_349.chunk4->chunk_expr_c;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_350(myposition);
    
    {
    
        result_peg_350 = rule_expr2(stream, result_peg_350.getPosition());
            if (result_peg_350.error()){
                goto out_peg_352;
            }
            left = result_peg_350.getValues();
        
        
        
        result_peg_350 = rule_expr_rest(stream, result_peg_350.getPosition(), left);
            if (result_peg_350.error()){
                goto out_peg_352;
            }
        
        
    }
    
    if (column_peg_349.chunk4 == 0){
        column_peg_349.chunk4 = new Chunk4();
    }
    column_peg_349.chunk4->chunk_expr_c = result_peg_350;
    stream.update(result_peg_350.getPosition());
    
    
    return result_peg_350;
    out_peg_352:
    
    if (column_peg_349.chunk4 == 0){
        column_peg_349.chunk4 = new Chunk4();
    }
    column_peg_349.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_353 = stream.getColumn(position);
    if (column_peg_353.chunk4 != 0 && column_peg_353.chunk4->chunk_expr2.calculated()){
        return column_peg_353.chunk4->chunk_expr2;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_354(myposition);
    
    {
    
        result_peg_354 = rule_expr3(stream, result_peg_354.getPosition());
            if (result_peg_354.error()){
                goto out_peg_356;
            }
            left = result_peg_354.getValues();
        
        
        
        result_peg_354 = rule_expr2_rest(stream, result_peg_354.getPosition(), left);
            if (result_peg_354.error()){
                goto out_peg_356;
            }
        
        
    }
    
    if (column_peg_353.chunk4 == 0){
        column_peg_353.chunk4 = new Chunk4();
    }
    column_peg_353.chunk4->chunk_expr2 = result_peg_354;
    stream.update(result_peg_354.getPosition());
    
    
    return result_peg_354;
    out_peg_356:
    
    if (column_peg_353.chunk4 == 0){
        column_peg_353.chunk4 = new Chunk4();
    }
    column_peg_353.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_357 = stream.getColumn(position);
    if (column_peg_357.chunk4 != 0 && column_peg_357.chunk4->chunk_expr3.calculated()){
        return column_peg_357.chunk4->chunk_expr3;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_358(myposition);
    
    {
    
        result_peg_358 = rule_expr4(stream, result_peg_358.getPosition());
            if (result_peg_358.error()){
                goto out_peg_360;
            }
            left = result_peg_358.getValues();
        
        
        
        result_peg_358 = rule_expr3_rest(stream, result_peg_358.getPosition(), left);
            if (result_peg_358.error()){
                goto out_peg_360;
            }
        
        
    }
    
    if (column_peg_357.chunk4 == 0){
        column_peg_357.chunk4 = new Chunk4();
    }
    column_peg_357.chunk4->chunk_expr3 = result_peg_358;
    stream.update(result_peg_358.getPosition());
    
    
    return result_peg_358;
    out_peg_360:
    
    if (column_peg_357.chunk4 == 0){
        column_peg_357.chunk4 = new Chunk4();
    }
    column_peg_357.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_361 = stream.getColumn(position);
    if (column_peg_361.chunk4 != 0 && column_peg_361.chunk4->chunk_expr4.calculated()){
        return column_peg_361.chunk4->chunk_expr4;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_362(myposition);
    
    {
    
        result_peg_362 = rule_expr5(stream, result_peg_362.getPosition());
            if (result_peg_362.error()){
                goto out_peg_364;
            }
            left = result_peg_362.getValues();
        
        
        
        result_peg_362 = rule_expr4_rest(stream, result_peg_362.getPosition(), left);
            if (result_peg_362.error()){
                goto out_peg_364;
            }
        
        
    }
    
    if (column_peg_361.chunk4 == 0){
        column_peg_361.chunk4 = new Chunk4();
    }
    column_peg_361.chunk4->chunk_expr4 = result_peg_362;
    stream.update(result_peg_362.getPosition());
    
    
    return result_peg_362;
    out_peg_364:
    
    if (column_peg_361.chunk4 == 0){
        column_peg_361.chunk4 = new Chunk4();
    }
    column_peg_361.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_365 = stream.getColumn(position);
    if (column_peg_365.chunk4 != 0 && column_peg_365.chunk4->chunk_expr5.calculated()){
        return column_peg_365.chunk4->chunk_expr5;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_366(myposition);
    
    {
    
        result_peg_366 = rule_expr6(stream, result_peg_366.getPosition());
            if (result_peg_366.error()){
                goto out_peg_368;
            }
            left = result_peg_366.getValues();
        
        
        
        result_peg_366 = rule_expr5_rest(stream, result_peg_366.getPosition(), left);
            if (result_peg_366.error()){
                goto out_peg_368;
            }
        
        
    }
    
    if (column_peg_365.chunk4 == 0){
        column_peg_365.chunk4 = new Chunk4();
    }
    column_peg_365.chunk4->chunk_expr5 = result_peg_366;
    stream.update(result_peg_366.getPosition());
    
    
    return result_peg_366;
    out_peg_368:
    
    if (column_peg_365.chunk4 == 0){
        column_peg_365.chunk4 = new Chunk4();
    }
    column_peg_365.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_369 = stream.getColumn(position);
    if (column_peg_369.chunk5 != 0 && column_peg_369.chunk5->chunk_expr6.calculated()){
        return column_peg_369.chunk5->chunk_expr6;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_370(myposition);
    
    {
    
        result_peg_370 = rule_expr7(stream, result_peg_370.getPosition());
            if (result_peg_370.error()){
                goto out_peg_372;
            }
            left = result_peg_370.getValues();
        
        
        
        result_peg_370 = rule_expr6_rest(stream, result_peg_370.getPosition(), left);
            if (result_peg_370.error()){
                goto out_peg_372;
            }
        
        
    }
    
    if (column_peg_369.chunk5 == 0){
        column_peg_369.chunk5 = new Chunk5();
    }
    column_peg_369.chunk5->chunk_expr6 = result_peg_370;
    stream.update(result_peg_370.getPosition());
    
    
    return result_peg_370;
    out_peg_372:
    
    if (column_peg_369.chunk5 == 0){
        column_peg_369.chunk5 = new Chunk5();
    }
    column_peg_369.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_373 = stream.getColumn(position);
    if (column_peg_373.chunk5 != 0 && column_peg_373.chunk5->chunk_expr7.calculated()){
        return column_peg_373.chunk5->chunk_expr7;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_374(myposition);
    
    {
    
        result_peg_374 = rule_expr8(stream, result_peg_374.getPosition());
            if (result_peg_374.error()){
                goto out_peg_376;
            }
            left = result_peg_374.getValues();
        
        
        
        result_peg_374 = rule_expr7_rest(stream, result_peg_374.getPosition(), left);
            if (result_peg_374.error()){
                goto out_peg_376;
            }
        
        
    }
    
    if (column_peg_373.chunk5 == 0){
        column_peg_373.chunk5 = new Chunk5();
    }
    column_peg_373.chunk5->chunk_expr7 = result_peg_374;
    stream.update(result_peg_374.getPosition());
    
    
    return result_peg_374;
    out_peg_376:
    
    if (column_peg_373.chunk5 == 0){
        column_peg_373.chunk5 = new Chunk5();
    }
    column_peg_373.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_377 = stream.getColumn(position);
    if (column_peg_377.chunk5 != 0 && column_peg_377.chunk5->chunk_expr8.calculated()){
        return column_peg_377.chunk5->chunk_expr8;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_378(myposition);
    
    {
    
        result_peg_378 = rule_expr9(stream, result_peg_378.getPosition());
            if (result_peg_378.error()){
                goto out_peg_380;
            }
            left = result_peg_378.getValues();
        
        
        
        result_peg_378 = rule_expr8_rest(stream, result_peg_378.getPosition(), left);
            if (result_peg_378.error()){
                goto out_peg_380;
            }
        
        
    }
    
    if (column_peg_377.chunk5 == 0){
        column_peg_377.chunk5 = new Chunk5();
    }
    column_peg_377.chunk5->chunk_expr8 = result_peg_378;
    stream.update(result_peg_378.getPosition());
    
    
    return result_peg_378;
    out_peg_380:
    
    if (column_peg_377.chunk5 == 0){
        column_peg_377.chunk5 = new Chunk5();
    }
    column_peg_377.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_381 = stream.getColumn(position);
    if (column_peg_381.chunk5 != 0 && column_peg_381.chunk5->chunk_expr9.calculated()){
        return column_peg_381.chunk5->chunk_expr9;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_382(myposition);
    
    {
    
        result_peg_382 = rule_expr10(stream, result_peg_382.getPosition());
            if (result_peg_382.error()){
                goto out_peg_384;
            }
            left = result_peg_382.getValues();
        
        
        
        result_peg_382 = rule_expr9_rest(stream, result_peg_382.getPosition(), left);
            if (result_peg_382.error()){
                goto out_peg_384;
            }
        
        
    }
    
    if (column_peg_381.chunk5 == 0){
        column_peg_381.chunk5 = new Chunk5();
    }
    column_peg_381.chunk5->chunk_expr9 = result_peg_382;
    stream.update(result_peg_382.getPosition());
    
    
    return result_peg_382;
    out_peg_384:
    
    if (column_peg_381.chunk5 == 0){
        column_peg_381.chunk5 = new Chunk5();
    }
    column_peg_381.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_385 = stream.getColumn(position);
    if (column_peg_385.chunk5 != 0 && column_peg_385.chunk5->chunk_expr10.calculated()){
        return column_peg_385.chunk5->chunk_expr10;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_386(myposition);
    
    {
    
        result_peg_386 = rule_expr11(stream, result_peg_386.getPosition());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
            left = result_peg_386.getValues();
        
        
        
        result_peg_386 = rule_expr10_rest(stream, result_peg_386.getPosition(), left);
            if (result_peg_386.error()){
                goto out_peg_388;
            }
        
        
    }
    
    if (column_peg_385.chunk5 == 0){
        column_peg_385.chunk5 = new Chunk5();
    }
    column_peg_385.chunk5->chunk_expr10 = result_peg_386;
    stream.update(result_peg_386.getPosition());
    
    
    return result_peg_386;
    out_peg_388:
    
    if (column_peg_385.chunk5 == 0){
        column_peg_385.chunk5 = new Chunk5();
    }
    column_peg_385.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_389 = stream.getColumn(position);
    if (column_peg_389.chunk6 != 0 && column_peg_389.chunk6->chunk_expr11.calculated()){
        return column_peg_389.chunk6->chunk_expr11;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_390(myposition);
    
    {
    
        result_peg_390 = rule_expr12(stream, result_peg_390.getPosition());
            if (result_peg_390.error()){
                goto out_peg_392;
            }
            left = result_peg_390.getValues();
        
        
        
        result_peg_390 = rule_expr11_rest(stream, result_peg_390.getPosition(), left);
            if (result_peg_390.error()){
                goto out_peg_392;
            }
        
        
    }
    
    if (column_peg_389.chunk6 == 0){
        column_peg_389.chunk6 = new Chunk6();
    }
    column_peg_389.chunk6->chunk_expr11 = result_peg_390;
    stream.update(result_peg_390.getPosition());
    
    
    return result_peg_390;
    out_peg_392:
    
    if (column_peg_389.chunk6 == 0){
        column_peg_389.chunk6 = new Chunk6();
    }
    column_peg_389.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_393 = stream.getColumn(position);
    if (column_peg_393.chunk6 != 0 && column_peg_393.chunk6->chunk_expr12.calculated()){
        return column_peg_393.chunk6->chunk_expr12;
    }
    
    int myposition = position;
    
    Value left;
    Result result_peg_394(myposition);
    
    {
    
        result_peg_394 = rule_expr13(stream, result_peg_394.getPosition());
            if (result_peg_394.error()){
                goto out_peg_396;
            }
            left = result_peg_394.getValues();
        
        
        
        result_peg_394 = rule_expr12_rest(stream, result_peg_394.getPosition(), left);
            if (result_peg_394.error()){
                goto out_peg_396;
            }
        
        
    }
    
    if (column_peg_393.chunk6 == 0){
        column_peg_393.chunk6 = new Chunk6();
    }
    column_peg_393.chunk6->chunk_expr12 = result_peg_394;
    stream.update(result_peg_394.getPosition());
    
    
    return result_peg_394;
    out_peg_396:
    
    if (column_peg_393.chunk6 == 0){
        column_peg_393.chunk6 = new Chunk6();
    }
    column_peg_393.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_397 = stream.getColumn(position);
    if (column_peg_397.chunk6 != 0 && column_peg_397.chunk6->chunk_expr13.calculated()){
        return column_peg_397.chunk6->chunk_expr13;
    }
    
    int myposition = position;
    
    Value exp;
    Result result_peg_398(myposition);
    
    {
    
        result_peg_398.reset();
            do{
                Result result_peg_401(result_peg_398.getPosition());
                result_peg_401 = rule_unary(stream, result_peg_401.getPosition());
                if (result_peg_401.error()){
                    goto loop_peg_400;
                }
                result_peg_398.addResult(result_peg_401);
            } while (true);
            loop_peg_400:
            ;
        
        Result result_peg_399 = result_peg_398;
        
        result_peg_398 = rule_expr13_real(stream, result_peg_398.getPosition());
            if (result_peg_398.error()){
                goto out_peg_403;
            }
            exp = result_peg_398.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_399.getValues(), exp);
                result_peg_398.setValue(value);
            }
        
        
    }
    
    if (column_peg_397.chunk6 == 0){
        column_peg_397.chunk6 = new Chunk6();
    }
    column_peg_397.chunk6->chunk_expr13 = result_peg_398;
    stream.update(result_peg_398.getPosition());
    
    
    return result_peg_398;
    out_peg_403:
    
    if (column_peg_397.chunk6 == 0){
        column_peg_397.chunk6 = new Chunk6();
    }
    column_peg_397.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_406:
    Value right;
    Value new_left;
    Result result_peg_405(myposition);
    {
    
        result_peg_405 = rule_s(stream, result_peg_405.getPosition());
            if (result_peg_405.error()){
                goto out_peg_408;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_405.getPosition()))){
                    result_peg_405.nextPosition();
                } else {
                    goto out_peg_408;
                }
            }
            result_peg_405.setValue((void*) "||");
        
        
        
        result_peg_405 = rule_s(stream, result_peg_405.getPosition());
            if (result_peg_405.error()){
                goto out_peg_408;
            }
        
        
        
        result_peg_405 = rule_expr2(stream, result_peg_405.getPosition());
            if (result_peg_405.error()){
                goto out_peg_408;
            }
            right = result_peg_405.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_405.setValue(value);
            }
            new_left = result_peg_405.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_405.getPosition();
    goto tail_peg_406;
    out_peg_408:
    Result result_peg_413(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_413.setValue(value);
            }
        
        
    }
    
    
    return result_peg_413;
    
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_417:
    Value right;
    Value new_left;
    Result result_peg_416(myposition);
    {
    
        result_peg_416 = rule_s(stream, result_peg_416.getPosition());
            if (result_peg_416.error()){
                goto out_peg_419;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_416.getPosition()))){
                    result_peg_416.nextPosition();
                } else {
                    goto out_peg_419;
                }
            }
            result_peg_416.setValue((void*) "^^");
        
        
        
        result_peg_416 = rule_s(stream, result_peg_416.getPosition());
            if (result_peg_416.error()){
                goto out_peg_419;
            }
        
        
        
        result_peg_416 = rule_expr3(stream, result_peg_416.getPosition());
            if (result_peg_416.error()){
                goto out_peg_419;
            }
            right = result_peg_416.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_416.setValue(value);
            }
            new_left = result_peg_416.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_416.getPosition();
    goto tail_peg_417;
    out_peg_419:
    Result result_peg_424(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_424.setValue(value);
            }
        
        
    }
    
    
    return result_peg_424;
    
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_428:
    Value right;
    Value new_left;
    Result result_peg_427(myposition);
    {
    
        result_peg_427 = rule_s(stream, result_peg_427.getPosition());
            if (result_peg_427.error()){
                goto out_peg_430;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_427.getPosition()))){
                    result_peg_427.nextPosition();
                } else {
                    goto out_peg_430;
                }
            }
            result_peg_427.setValue((void*) "&&");
        
        
        
        result_peg_427 = rule_s(stream, result_peg_427.getPosition());
            if (result_peg_427.error()){
                goto out_peg_430;
            }
        
        
        
        result_peg_427 = rule_expr4(stream, result_peg_427.getPosition());
            if (result_peg_427.error()){
                goto out_peg_430;
            }
            right = result_peg_427.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
                result_peg_427.setValue(value);
            }
            new_left = result_peg_427.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_427.getPosition();
    goto tail_peg_428;
    out_peg_430:
    Result result_peg_435(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_435.setValue(value);
            }
        
        
    }
    
    
    return result_peg_435;
    
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_439:
    Value right;
    Value new_left;
    Result result_peg_438(myposition);
    {
    
        result_peg_438 = rule_s(stream, result_peg_438.getPosition());
            if (result_peg_438.error()){
                goto out_peg_441;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_438.getPosition()))){
                    result_peg_438.nextPosition();
                } else {
                    goto out_peg_441;
                }
            }
            result_peg_438.setValue((void*) "|");
        
        
        
        result_peg_438 = rule_s(stream, result_peg_438.getPosition());
            if (result_peg_438.error()){
                goto out_peg_441;
            }
        
        
        
        result_peg_438 = rule_expr5(stream, result_peg_438.getPosition());
            if (result_peg_438.error()){
                goto out_peg_441;
            }
            right = result_peg_438.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_438.setValue(value);
            }
            new_left = result_peg_438.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_438.getPosition();
    goto tail_peg_439;
    out_peg_441:
    Result result_peg_446(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_446.setValue(value);
            }
        
        
    }
    
    
    return result_peg_446;
    
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_450:
    Value right;
    Value new_left;
    Result result_peg_449(myposition);
    {
    
        result_peg_449 = rule_s(stream, result_peg_449.getPosition());
            if (result_peg_449.error()){
                goto out_peg_452;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_449.getPosition()))){
                    result_peg_449.nextPosition();
                } else {
                    goto out_peg_452;
                }
            }
            result_peg_449.setValue((void*) "^");
        
        
        
        result_peg_449 = rule_s(stream, result_peg_449.getPosition());
            if (result_peg_449.error()){
                goto out_peg_452;
            }
        
        
        
        result_peg_449 = rule_expr6(stream, result_peg_449.getPosition());
            if (result_peg_449.error()){
                goto out_peg_452;
            }
            right = result_peg_449.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_449.setValue(value);
            }
            new_left = result_peg_449.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_449.getPosition();
    goto tail_peg_450;
    out_peg_452:
    Result result_peg_457(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_457.setValue(value);
            }
        
        
    }
    
    
    return result_peg_457;
    
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_461:
    Value right;
    Value new_left;
    Result result_peg_460(myposition);
    {
    
        result_peg_460 = rule_s(stream, result_peg_460.getPosition());
            if (result_peg_460.error()){
                goto out_peg_463;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_460.getPosition()))){
                    result_peg_460.nextPosition();
                } else {
                    goto out_peg_463;
                }
            }
            result_peg_460.setValue((void*) "&");
        
        
        
        result_peg_460 = rule_s(stream, result_peg_460.getPosition());
            if (result_peg_460.error()){
                goto out_peg_463;
            }
        
        
        
        result_peg_460 = rule_expr7(stream, result_peg_460.getPosition());
            if (result_peg_460.error()){
                goto out_peg_463;
            }
            right = result_peg_460.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_460.setValue(value);
            }
            new_left = result_peg_460.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_460.getPosition();
    goto tail_peg_461;
    out_peg_463:
    Result result_peg_468(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_468.setValue(value);
            }
        
        
    }
    
    
    return result_peg_468;
    
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
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
                if (compareChar(":="[i], stream.get(result_peg_471.getPosition()))){
                    result_peg_471.nextPosition();
                } else {
                    goto out_peg_474;
                }
            }
            result_peg_471.setValue((void*) ":=");
        
        
        
        result_peg_471 = rule_s(stream, result_peg_471.getPosition());
            if (result_peg_471.error()){
                goto out_peg_474;
            }
        
        
        
        result_peg_471 = rule_expr8(stream, result_peg_471.getPosition());
            if (result_peg_471.error()){
                goto out_peg_474;
            }
            right = result_peg_471.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
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
    
    
    return result_peg_479;
    
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
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
                if (compareChar("="[i], stream.get(result_peg_482.getPosition()))){
                    result_peg_482.nextPosition();
                } else {
                    goto out_peg_485;
                }
            }
            result_peg_482.setValue((void*) "=");
        
        
        
        result_peg_482 = rule_s(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
        
        
        
        result_peg_482 = rule_expr9(stream, result_peg_482.getPosition());
            if (result_peg_482.error()){
                goto out_peg_485;
            }
            right = result_peg_482.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
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
    
        result_peg_490 = rule_s(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_492;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_490.getPosition()))){
                    result_peg_490.nextPosition();
                } else {
                    goto out_peg_492;
                }
            }
            result_peg_490.setValue((void*) "!=");
        
        
        
        result_peg_490 = rule_s(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_492;
            }
        
        
        
        result_peg_490 = rule_expr9(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_492;
            }
            right = result_peg_490.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_490.setValue(value);
            }
            new_left = result_peg_490.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_490.getPosition();
    goto tail_peg_483;
    out_peg_492:
    Result result_peg_497(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_497.setValue(value);
            }
        
        
    }
    
    
    return result_peg_497;
    
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_501:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_500(myposition);
    {
    
        result_peg_500 = rule_s(stream, result_peg_500.getPosition());
            if (result_peg_500.error()){
                goto out_peg_503;
            }
        
        
        
        result_peg_500 = rule_compare(stream, result_peg_500.getPosition());
            if (result_peg_500.error()){
                goto out_peg_503;
            }
            maker = result_peg_500.getValues();
        
        
        
        result_peg_500 = rule_s(stream, result_peg_500.getPosition());
            if (result_peg_500.error()){
                goto out_peg_503;
            }
        
        
        
        result_peg_500 = rule_expr10(stream, result_peg_500.getPosition());
            if (result_peg_500.error()){
                goto out_peg_503;
            }
            right = result_peg_500.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_500.setValue(value);
            }
            new_left = result_peg_500.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_500.getPosition();
    goto tail_peg_501;
    out_peg_503:
    Result result_peg_508(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_508.setValue(value);
            }
        
        
    }
    
    
    return result_peg_508;
    
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_510 = stream.getColumn(position);
    if (column_peg_510.chunk8 != 0 && column_peg_510.chunk8->chunk_compare.calculated()){
        return column_peg_510.chunk8->chunk_compare;
    }
    
    int myposition = position;
    
    
    Result result_peg_511(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_511.getPosition()))){
                    result_peg_511.nextPosition();
                } else {
                    goto out_peg_513;
                }
            }
            result_peg_511.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_511.setValue(value);
            }
        
        
    }
    
    if (column_peg_510.chunk8 == 0){
        column_peg_510.chunk8 = new Chunk8();
    }
    column_peg_510.chunk8->chunk_compare = result_peg_511;
    stream.update(result_peg_511.getPosition());
    
    
    return result_peg_511;
    out_peg_513:
    Result result_peg_514(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_514.getPosition()))){
                    result_peg_514.nextPosition();
                } else {
                    goto out_peg_516;
                }
            }
            result_peg_514.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_514.setValue(value);
            }
        
        
    }
    
    if (column_peg_510.chunk8 == 0){
        column_peg_510.chunk8 = new Chunk8();
    }
    column_peg_510.chunk8->chunk_compare = result_peg_514;
    stream.update(result_peg_514.getPosition());
    
    
    return result_peg_514;
    out_peg_516:
    Result result_peg_517(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_517.getPosition()))){
                    result_peg_517.nextPosition();
                } else {
                    goto out_peg_519;
                }
            }
            result_peg_517.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_517.setValue(value);
            }
        
        
    }
    
    if (column_peg_510.chunk8 == 0){
        column_peg_510.chunk8 = new Chunk8();
    }
    column_peg_510.chunk8->chunk_compare = result_peg_517;
    stream.update(result_peg_517.getPosition());
    
    
    return result_peg_517;
    out_peg_519:
    Result result_peg_520(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_520.getPosition()))){
                    result_peg_520.nextPosition();
                } else {
                    goto out_peg_522;
                }
            }
            result_peg_520.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_520.setValue(value);
            }
        
        
    }
    
    if (column_peg_510.chunk8 == 0){
        column_peg_510.chunk8 = new Chunk8();
    }
    column_peg_510.chunk8->chunk_compare = result_peg_520;
    stream.update(result_peg_520.getPosition());
    
    
    return result_peg_520;
    out_peg_522:
    
    if (column_peg_510.chunk8 == 0){
        column_peg_510.chunk8 = new Chunk8();
    }
    column_peg_510.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_523 = stream.getColumn(position);
    if (column_peg_523.chunk8 != 0 && column_peg_523.chunk8->chunk_all_compare.calculated()){
        return column_peg_523.chunk8->chunk_all_compare;
    }
    
    int myposition = position;
    
    
    Result result_peg_524(myposition);
    
    result_peg_524 = rule_compare(stream, result_peg_524.getPosition());
    if (result_peg_524.error()){
        goto out_peg_525;
    }
    
    if (column_peg_523.chunk8 == 0){
        column_peg_523.chunk8 = new Chunk8();
    }
    column_peg_523.chunk8->chunk_all_compare = result_peg_524;
    stream.update(result_peg_524.getPosition());
    
    
    return result_peg_524;
    out_peg_525:
    Result result_peg_526(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_526.getPosition()))){
            result_peg_526.nextPosition();
        } else {
            goto out_peg_527;
        }
    }
    result_peg_526.setValue((void*) "=");
    
    if (column_peg_523.chunk8 == 0){
        column_peg_523.chunk8 = new Chunk8();
    }
    column_peg_523.chunk8->chunk_all_compare = result_peg_526;
    stream.update(result_peg_526.getPosition());
    
    
    return result_peg_526;
    out_peg_527:
    Result result_peg_528(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_528.getPosition()))){
            result_peg_528.nextPosition();
        } else {
            goto out_peg_529;
        }
    }
    result_peg_528.setValue((void*) "!=");
    
    if (column_peg_523.chunk8 == 0){
        column_peg_523.chunk8 = new Chunk8();
    }
    column_peg_523.chunk8->chunk_all_compare = result_peg_528;
    stream.update(result_peg_528.getPosition());
    
    
    return result_peg_528;
    out_peg_529:
    
    if (column_peg_523.chunk8 == 0){
        column_peg_523.chunk8 = new Chunk8();
    }
    column_peg_523.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
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
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_531.getPosition()))){
                    result_peg_531.nextPosition();
                } else {
                    goto out_peg_534;
                }
            }
            result_peg_531.setValue((void*) "+");
        
        
        
        result_peg_531 = rule_s(stream, result_peg_531.getPosition());
            if (result_peg_531.error()){
                goto out_peg_534;
            }
        
        
        
        result_peg_531 = rule_expr11(stream, result_peg_531.getPosition());
            if (result_peg_531.error()){
                goto out_peg_534;
            }
            right = result_peg_531.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
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
    
        result_peg_539 = rule_s(stream, result_peg_539.getPosition());
            if (result_peg_539.error()){
                goto out_peg_541;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_539.getPosition()))){
                    result_peg_539.nextPosition();
                } else {
                    goto out_peg_541;
                }
            }
            result_peg_539.setValue((void*) "-");
        
        
        
        result_peg_539 = rule_s(stream, result_peg_539.getPosition());
            if (result_peg_539.error()){
                goto out_peg_541;
            }
        
        
        
        result_peg_539 = rule_expr11(stream, result_peg_539.getPosition());
            if (result_peg_539.error()){
                goto out_peg_541;
            }
            right = result_peg_539.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_539.setValue(value);
            }
            new_left = result_peg_539.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_539.getPosition();
    goto tail_peg_532;
    out_peg_541:
    Result result_peg_546(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_546.setValue(value);
            }
        
        
    }
    
    
    return result_peg_546;
    
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_550:
    Value right;
    Value new_left;
    Result result_peg_549(myposition);
    {
    
        result_peg_549 = rule_s(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_549.getPosition()))){
                    result_peg_549.nextPosition();
                } else {
                    goto out_peg_552;
                }
            }
            result_peg_549.setValue((void*) "*");
        
        
        
        result_peg_549 = rule_s(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
        
        
        
        result_peg_549 = rule_expr12(stream, result_peg_549.getPosition());
            if (result_peg_549.error()){
                goto out_peg_552;
            }
            right = result_peg_549.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
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
    
        result_peg_557 = rule_s(stream, result_peg_557.getPosition());
            if (result_peg_557.error()){
                goto out_peg_559;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_557.getPosition()))){
                    result_peg_557.nextPosition();
                } else {
                    goto out_peg_559;
                }
            }
            result_peg_557.setValue((void*) "/");
        
        
        
        result_peg_557 = rule_s(stream, result_peg_557.getPosition());
            if (result_peg_557.error()){
                goto out_peg_559;
            }
        
        
        
        result_peg_557 = rule_expr12(stream, result_peg_557.getPosition());
            if (result_peg_557.error()){
                goto out_peg_559;
            }
            right = result_peg_557.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionDivide(left, right);
                result_peg_557.setValue(value);
            }
            new_left = result_peg_557.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_557.getPosition();
    goto tail_peg_550;
    out_peg_559:
    Result result_peg_564(myposition);
    {
    
        result_peg_564 = rule_s(stream, result_peg_564.getPosition());
            if (result_peg_564.error()){
                goto out_peg_566;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_564.getPosition()))){
                    result_peg_564.nextPosition();
                } else {
                    goto out_peg_566;
                }
            }
            result_peg_564.setValue((void*) "%");
        
        
        
        result_peg_564 = rule_s(stream, result_peg_564.getPosition());
            if (result_peg_564.error()){
                goto out_peg_566;
            }
        
        
        
        result_peg_564 = rule_expr12(stream, result_peg_564.getPosition());
            if (result_peg_564.error()){
                goto out_peg_566;
            }
            right = result_peg_564.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_564.setValue(value);
            }
            new_left = result_peg_564.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_564.getPosition();
    goto tail_peg_550;
    out_peg_566:
    Result result_peg_571(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_571.setValue(value);
            }
        
        
    }
    
    
    return result_peg_571;
    
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_575:
    Value right;
    Value new_left;
    Result result_peg_574(myposition);
    {
    
        result_peg_574 = rule_s(stream, result_peg_574.getPosition());
            if (result_peg_574.error()){
                goto out_peg_577;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_574.getPosition()))){
                    result_peg_574.nextPosition();
                } else {
                    goto out_peg_577;
                }
            }
            result_peg_574.setValue((void*) "**");
        
        
        
        result_peg_574 = rule_s(stream, result_peg_574.getPosition());
            if (result_peg_574.error()){
                goto out_peg_577;
            }
        
        
        
        result_peg_574 = rule_expr13(stream, result_peg_574.getPosition());
            if (result_peg_574.error()){
                goto out_peg_577;
            }
            right = result_peg_574.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_574.setValue(value);
            }
            new_left = result_peg_574.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_574.getPosition();
    goto tail_peg_575;
    out_peg_577:
    Result result_peg_582(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_582.setValue(value);
            }
        
        
    }
    
    
    return result_peg_582;
    
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_584 = stream.getColumn(position);
    if (column_peg_584.chunk9 != 0 && column_peg_584.chunk9->chunk_unary.calculated()){
        return column_peg_584.chunk9->chunk_unary;
    }
    
    int myposition = position;
    
    
    Result result_peg_585(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_585.getPosition()))){
                    result_peg_585.nextPosition();
                } else {
                    goto out_peg_587;
                }
            }
            result_peg_585.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_585.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk9 == 0){
        column_peg_584.chunk9 = new Chunk9();
    }
    column_peg_584.chunk9->chunk_unary = result_peg_585;
    stream.update(result_peg_585.getPosition());
    
    
    return result_peg_585;
    out_peg_587:
    Result result_peg_588(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_588.getPosition()))){
                    result_peg_588.nextPosition();
                } else {
                    goto out_peg_590;
                }
            }
            result_peg_588.setValue((void*) "-");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_588.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk9 == 0){
        column_peg_584.chunk9 = new Chunk9();
    }
    column_peg_584.chunk9->chunk_unary = result_peg_588;
    stream.update(result_peg_588.getPosition());
    
    
    return result_peg_588;
    out_peg_590:
    Result result_peg_591(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_591.getPosition()))){
                    result_peg_591.nextPosition();
                } else {
                    goto out_peg_593;
                }
            }
            result_peg_591.setValue((void*) "~");
        
        
        
        Result result_peg_596(result_peg_591);
            result_peg_596 = rule_key(stream, result_peg_596.getPosition());
            if (result_peg_596.error()){
                goto not_peg_595;
            }
            goto out_peg_593;
            not_peg_595:
            result_peg_591.setValue((void*)0);
        
        
        
        Result result_peg_599(result_peg_591);
            {
                
                char letter_peg_604 = stream.get(result_peg_599.getPosition());
                if (letter_peg_604 != '\0' && strchr("0123456789", letter_peg_604) != NULL){
                    result_peg_599.nextPosition();
                    result_peg_599.setValue((void*) letter_peg_604);
                } else {
                    goto out_peg_603;
                }
                
            }
            goto success_peg_600;
            out_peg_603:
            goto not_peg_598;
            success_peg_600:
            ;
            goto out_peg_593;
            not_peg_598:
            result_peg_591.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_591.setValue(value);
            }
        
        
    }
    
    if (column_peg_584.chunk9 == 0){
        column_peg_584.chunk9 = new Chunk9();
    }
    column_peg_584.chunk9->chunk_unary = result_peg_591;
    stream.update(result_peg_591.getPosition());
    
    
    return result_peg_591;
    out_peg_593:
    
    if (column_peg_584.chunk9 == 0){
        column_peg_584.chunk9 = new Chunk9();
    }
    column_peg_584.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_605 = stream.getColumn(position);
    if (column_peg_605.chunk9 != 0 && column_peg_605.chunk9->chunk_expr13_real.calculated()){
        return column_peg_605.chunk9->chunk_expr13_real;
    }
    
    int myposition = position;
    
    Value e;
    Result result_peg_606(myposition);
    
    result_peg_606 = rule_function(stream, result_peg_606.getPosition());
    if (result_peg_606.error()){
        goto out_peg_607;
    }
    
    if (column_peg_605.chunk9 == 0){
        column_peg_605.chunk9 = new Chunk9();
    }
    column_peg_605.chunk9->chunk_expr13_real = result_peg_606;
    stream.update(result_peg_606.getPosition());
    
    
    return result_peg_606;
    out_peg_607:
    Result result_peg_608(myposition);
    
    result_peg_608 = rule_value(stream, result_peg_608.getPosition());
    if (result_peg_608.error()){
        goto out_peg_609;
    }
    
    if (column_peg_605.chunk9 == 0){
        column_peg_605.chunk9 = new Chunk9();
    }
    column_peg_605.chunk9->chunk_expr13_real = result_peg_608;
    stream.update(result_peg_608.getPosition());
    
    
    return result_peg_608;
    out_peg_609:
    Result result_peg_610(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_610.getPosition()))){
                    result_peg_610.nextPosition();
                } else {
                    goto out_peg_612;
                }
            }
            result_peg_610.setValue((void*) "(");
        
        
        
        result_peg_610 = rule_s(stream, result_peg_610.getPosition());
            if (result_peg_610.error()){
                goto out_peg_612;
            }
        
        
        
        result_peg_610 = rule_expr(stream, result_peg_610.getPosition());
            if (result_peg_610.error()){
                goto out_peg_612;
            }
            e = result_peg_610.getValues();
        
        
        
        result_peg_610 = rule_s(stream, result_peg_610.getPosition());
            if (result_peg_610.error()){
                goto out_peg_612;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_610.getPosition()))){
                    result_peg_610.nextPosition();
                } else {
                    goto out_peg_612;
                }
            }
            result_peg_610.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_610.setValue(value);
            }
        
        
    }
    
    if (column_peg_605.chunk9 == 0){
        column_peg_605.chunk9 = new Chunk9();
    }
    column_peg_605.chunk9->chunk_expr13_real = result_peg_610;
    stream.update(result_peg_610.getPosition());
    
    
    return result_peg_610;
    out_peg_612:
    
    if (column_peg_605.chunk9 == 0){
        column_peg_605.chunk9 = new Chunk9();
    }
    column_peg_605.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_617 = stream.getColumn(position);
    if (column_peg_617.chunk9 != 0 && column_peg_617.chunk9->chunk_function.calculated()){
        return column_peg_617.chunk9->chunk_function;
    }
    
    int myposition = position;
    
    Value name;
    Value arg1;
    Value arg2;
    Value arg3;
    Result result_peg_618(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("abs"[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) "abs");
            name = result_peg_618.getValues();
        
        
        
        result_peg_618 = rule_s(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) "(");
        
        
        
        result_peg_618 = rule_s(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
        
        
        
        result_peg_618 = rule_expr(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
            arg1 = result_peg_618.getValues();
        
        
        
        result_peg_618 = rule_s(stream, result_peg_618.getPosition());
            if (result_peg_618.error()){
                goto out_peg_620;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_618.getPosition()))){
                    result_peg_618.nextPosition();
                } else {
                    goto out_peg_620;
                }
            }
            result_peg_618.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_618.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_618;
    stream.update(result_peg_618.getPosition());
    
    
    return result_peg_618;
    out_peg_620:
    Result result_peg_627(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("const"[i], stream.get(result_peg_627.getPosition()))){
                    result_peg_627.nextPosition();
                } else {
                    goto out_peg_629;
                }
            }
            result_peg_627.setValue((void*) "const");
            name = result_peg_627.getValues();
        
        
        
        result_peg_627 = rule_s(stream, result_peg_627.getPosition());
            if (result_peg_627.error()){
                goto out_peg_629;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_627.getPosition()))){
                    result_peg_627.nextPosition();
                } else {
                    goto out_peg_629;
                }
            }
            result_peg_627.setValue((void*) "(");
        
        
        
        result_peg_627 = rule_s(stream, result_peg_627.getPosition());
            if (result_peg_627.error()){
                goto out_peg_629;
            }
        
        
        
        result_peg_627 = rule_expr(stream, result_peg_627.getPosition());
            if (result_peg_627.error()){
                goto out_peg_629;
            }
            arg1 = result_peg_627.getValues();
        
        
        
        result_peg_627 = rule_s(stream, result_peg_627.getPosition());
            if (result_peg_627.error()){
                goto out_peg_629;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_627.getPosition()))){
                    result_peg_627.nextPosition();
                } else {
                    goto out_peg_629;
                }
            }
            result_peg_627.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_627.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_627;
    stream.update(result_peg_627.getPosition());
    
    
    return result_peg_627;
    out_peg_629:
    Result result_peg_636(myposition);
    
    {
    
        for (int i = 0; i < 13; i++){
                if (compareCharCase("selfanimexist"[i], stream.get(result_peg_636.getPosition()))){
                    result_peg_636.nextPosition();
                } else {
                    goto out_peg_638;
                }
            }
            result_peg_636.setValue((void*) "selfanimexist");
            name = result_peg_636.getValues();
        
        
        
        result_peg_636 = rule_s(stream, result_peg_636.getPosition());
            if (result_peg_636.error()){
                goto out_peg_638;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_636.getPosition()))){
                    result_peg_636.nextPosition();
                } else {
                    goto out_peg_638;
                }
            }
            result_peg_636.setValue((void*) "(");
        
        
        
        result_peg_636 = rule_s(stream, result_peg_636.getPosition());
            if (result_peg_636.error()){
                goto out_peg_638;
            }
        
        
        
        result_peg_636 = rule_expr(stream, result_peg_636.getPosition());
            if (result_peg_636.error()){
                goto out_peg_638;
            }
            arg1 = result_peg_636.getValues();
        
        
        
        result_peg_636 = rule_s(stream, result_peg_636.getPosition());
            if (result_peg_636.error()){
                goto out_peg_638;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_636.getPosition()))){
                    result_peg_636.nextPosition();
                } else {
                    goto out_peg_638;
                }
            }
            result_peg_636.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_636.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_636;
    stream.update(result_peg_636.getPosition());
    
    
    return result_peg_636;
    out_peg_638:
    Result result_peg_645(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("ifelse"[i], stream.get(result_peg_645.getPosition()))){
                    result_peg_645.nextPosition();
                } else {
                    goto out_peg_647;
                }
            }
            result_peg_645.setValue((void*) "ifelse");
            name = result_peg_645.getValues();
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_645.getPosition()))){
                    result_peg_645.nextPosition();
                } else {
                    goto out_peg_647;
                }
            }
            result_peg_645.setValue((void*) "(");
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        result_peg_645 = rule_expr_c(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
            arg1 = result_peg_645.getValues();
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_645.getPosition()))){
                    result_peg_645.nextPosition();
                } else {
                    goto out_peg_647;
                }
            }
            result_peg_645.setValue((void*) ",");
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        result_peg_645 = rule_expr_c(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
            arg2 = result_peg_645.getValues();
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_645.getPosition()))){
                    result_peg_645.nextPosition();
                } else {
                    goto out_peg_647;
                }
            }
            result_peg_645.setValue((void*) ",");
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        result_peg_645 = rule_expr_c(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
            arg3 = result_peg_645.getValues();
        
        
        
        result_peg_645 = rule_s(stream, result_peg_645.getPosition());
            if (result_peg_645.error()){
                goto out_peg_647;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_645.getPosition()))){
                    result_peg_645.nextPosition();
                } else {
                    goto out_peg_647;
                }
            }
            result_peg_645.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_645.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_645;
    stream.update(result_peg_645.getPosition());
    
    
    return result_peg_645;
    out_peg_647:
    Result result_peg_662(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("gethitvar"[i], stream.get(result_peg_662.getPosition()))){
                    result_peg_662.nextPosition();
                } else {
                    goto out_peg_664;
                }
            }
            result_peg_662.setValue((void*) "gethitvar");
            name = result_peg_662.getValues();
        
        
        
        result_peg_662 = rule_s(stream, result_peg_662.getPosition());
            if (result_peg_662.error()){
                goto out_peg_664;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_662.getPosition()))){
                    result_peg_662.nextPosition();
                } else {
                    goto out_peg_664;
                }
            }
            result_peg_662.setValue((void*) "(");
        
        
        
        result_peg_662 = rule_s(stream, result_peg_662.getPosition());
            if (result_peg_662.error()){
                goto out_peg_664;
            }
        
        
        
        result_peg_662 = rule_expr_c(stream, result_peg_662.getPosition());
            if (result_peg_662.error()){
                goto out_peg_664;
            }
            arg1 = result_peg_662.getValues();
        
        
        
        result_peg_662 = rule_s(stream, result_peg_662.getPosition());
            if (result_peg_662.error()){
                goto out_peg_664;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_662.getPosition()))){
                    result_peg_662.nextPosition();
                } else {
                    goto out_peg_664;
                }
            }
            result_peg_662.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_662.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_662;
    stream.update(result_peg_662.getPosition());
    
    
    return result_peg_662;
    out_peg_664:
    Result result_peg_671(myposition);
    
    {
    
        for (int i = 0; i < 5; i++){
                if (compareCharCase("floor"[i], stream.get(result_peg_671.getPosition()))){
                    result_peg_671.nextPosition();
                } else {
                    goto out_peg_673;
                }
            }
            result_peg_671.setValue((void*) "floor");
            name = result_peg_671.getValues();
        
        
        
        result_peg_671 = rule_s(stream, result_peg_671.getPosition());
            if (result_peg_671.error()){
                goto out_peg_673;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_671.getPosition()))){
                    result_peg_671.nextPosition();
                } else {
                    goto out_peg_673;
                }
            }
            result_peg_671.setValue((void*) "(");
        
        
        
        result_peg_671 = rule_s(stream, result_peg_671.getPosition());
            if (result_peg_671.error()){
                goto out_peg_673;
            }
        
        
        
        result_peg_671 = rule_expr_c(stream, result_peg_671.getPosition());
            if (result_peg_671.error()){
                goto out_peg_673;
            }
            arg1 = result_peg_671.getValues();
        
        
        
        result_peg_671 = rule_s(stream, result_peg_671.getPosition());
            if (result_peg_671.error()){
                goto out_peg_673;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_671.getPosition()))){
                    result_peg_671.nextPosition();
                } else {
                    goto out_peg_673;
                }
            }
            result_peg_671.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_671.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_671;
    stream.update(result_peg_671.getPosition());
    
    
    return result_peg_671;
    out_peg_673:
    Result result_peg_680(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("ceil"[i], stream.get(result_peg_680.getPosition()))){
                    result_peg_680.nextPosition();
                } else {
                    goto out_peg_682;
                }
            }
            result_peg_680.setValue((void*) "ceil");
            name = result_peg_680.getValues();
        
        
        
        result_peg_680 = rule_s(stream, result_peg_680.getPosition());
            if (result_peg_680.error()){
                goto out_peg_682;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_680.getPosition()))){
                    result_peg_680.nextPosition();
                } else {
                    goto out_peg_682;
                }
            }
            result_peg_680.setValue((void*) "(");
        
        
        
        result_peg_680 = rule_s(stream, result_peg_680.getPosition());
            if (result_peg_680.error()){
                goto out_peg_682;
            }
        
        
        
        result_peg_680 = rule_expr_c(stream, result_peg_680.getPosition());
            if (result_peg_680.error()){
                goto out_peg_682;
            }
            arg1 = result_peg_680.getValues();
        
        
        
        result_peg_680 = rule_s(stream, result_peg_680.getPosition());
            if (result_peg_680.error()){
                goto out_peg_682;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_680.getPosition()))){
                    result_peg_680.nextPosition();
                } else {
                    goto out_peg_682;
                }
            }
            result_peg_680.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_680.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_680;
    stream.update(result_peg_680.getPosition());
    
    
    return result_peg_680;
    out_peg_682:
    Result result_peg_689(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("sysvar"[i], stream.get(result_peg_689.getPosition()))){
                    result_peg_689.nextPosition();
                } else {
                    goto out_peg_691;
                }
            }
            result_peg_689.setValue((void*) "sysvar");
            name = result_peg_689.getValues();
        
        
        
        result_peg_689 = rule_s(stream, result_peg_689.getPosition());
            if (result_peg_689.error()){
                goto out_peg_691;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_689.getPosition()))){
                    result_peg_689.nextPosition();
                } else {
                    goto out_peg_691;
                }
            }
            result_peg_689.setValue((void*) "(");
        
        
        
        result_peg_689 = rule_s(stream, result_peg_689.getPosition());
            if (result_peg_689.error()){
                goto out_peg_691;
            }
        
        
        
        result_peg_689 = rule_integer(stream, result_peg_689.getPosition());
            if (result_peg_689.error()){
                goto out_peg_691;
            }
            arg1 = result_peg_689.getValues();
        
        
        
        result_peg_689 = rule_s(stream, result_peg_689.getPosition());
            if (result_peg_689.error()){
                goto out_peg_691;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_689.getPosition()))){
                    result_peg_689.nextPosition();
                } else {
                    goto out_peg_691;
                }
            }
            result_peg_689.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_689.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_689;
    stream.update(result_peg_689.getPosition());
    
    
    return result_peg_689;
    out_peg_691:
    Result result_peg_698(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("var"[i], stream.get(result_peg_698.getPosition()))){
                    result_peg_698.nextPosition();
                } else {
                    goto out_peg_700;
                }
            }
            result_peg_698.setValue((void*) "var");
            name = result_peg_698.getValues();
        
        
        
        result_peg_698 = rule_s(stream, result_peg_698.getPosition());
            if (result_peg_698.error()){
                goto out_peg_700;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_698.getPosition()))){
                    result_peg_698.nextPosition();
                } else {
                    goto out_peg_700;
                }
            }
            result_peg_698.setValue((void*) "(");
        
        
        
        result_peg_698 = rule_s(stream, result_peg_698.getPosition());
            if (result_peg_698.error()){
                goto out_peg_700;
            }
        
        
        
        result_peg_698 = rule_integer(stream, result_peg_698.getPosition());
            if (result_peg_698.error()){
                goto out_peg_700;
            }
            arg1 = result_peg_698.getValues();
        
        
        
        result_peg_698 = rule_s(stream, result_peg_698.getPosition());
            if (result_peg_698.error()){
                goto out_peg_700;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_698.getPosition()))){
                    result_peg_698.nextPosition();
                } else {
                    goto out_peg_700;
                }
            }
            result_peg_698.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_698.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_698;
    stream.update(result_peg_698.getPosition());
    
    
    return result_peg_698;
    out_peg_700:
    Result result_peg_707(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numexplod"[i], stream.get(result_peg_707.getPosition()))){
                    result_peg_707.nextPosition();
                } else {
                    goto out_peg_709;
                }
            }
            result_peg_707.setValue((void*) "numexplod");
            name = result_peg_707.getValues();
        
        
        
        result_peg_707 = rule_s(stream, result_peg_707.getPosition());
            if (result_peg_707.error()){
                goto out_peg_709;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_707.getPosition()))){
                    result_peg_707.nextPosition();
                } else {
                    goto out_peg_709;
                }
            }
            result_peg_707.setValue((void*) "(");
        
        
        
        result_peg_707 = rule_s(stream, result_peg_707.getPosition());
            if (result_peg_707.error()){
                goto out_peg_709;
            }
        
        
        
        result_peg_707 = rule_integer(stream, result_peg_707.getPosition());
            if (result_peg_707.error()){
                goto out_peg_709;
            }
            arg1 = result_peg_707.getValues();
        
        
        
        result_peg_707 = rule_s(stream, result_peg_707.getPosition());
            if (result_peg_707.error()){
                goto out_peg_709;
            }
        
        
        
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
                value = makeFunction(name, arg1);
                result_peg_707.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_707;
    stream.update(result_peg_707.getPosition());
    
    
    return result_peg_707;
    out_peg_709:
    Result result_peg_716(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_716.getPosition()))){
                    result_peg_716.nextPosition();
                } else {
                    goto out_peg_718;
                }
            }
            result_peg_716.setValue((void*) "numhelper");
            name = result_peg_716.getValues();
        
        
        
        result_peg_716 = rule_s(stream, result_peg_716.getPosition());
            if (result_peg_716.error()){
                goto out_peg_718;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_716.getPosition()))){
                    result_peg_716.nextPosition();
                } else {
                    goto out_peg_718;
                }
            }
            result_peg_716.setValue((void*) "(");
        
        
        
        result_peg_716 = rule_s(stream, result_peg_716.getPosition());
            if (result_peg_716.error()){
                goto out_peg_718;
            }
        
        
        
        result_peg_716 = rule_integer(stream, result_peg_716.getPosition());
            if (result_peg_716.error()){
                goto out_peg_718;
            }
            arg1 = result_peg_716.getValues();
        
        
        
        result_peg_716 = rule_s(stream, result_peg_716.getPosition());
            if (result_peg_716.error()){
                goto out_peg_718;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_716.getPosition()))){
                    result_peg_716.nextPosition();
                } else {
                    goto out_peg_718;
                }
            }
            result_peg_716.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_716.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_716;
    stream.update(result_peg_716.getPosition());
    
    
    return result_peg_716;
    out_peg_718:
    Result result_peg_725(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numprojid"[i], stream.get(result_peg_725.getPosition()))){
                    result_peg_725.nextPosition();
                } else {
                    goto out_peg_727;
                }
            }
            result_peg_725.setValue((void*) "numprojid");
            name = result_peg_725.getValues();
        
        
        
        result_peg_725 = rule_s(stream, result_peg_725.getPosition());
            if (result_peg_725.error()){
                goto out_peg_727;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_725.getPosition()))){
                    result_peg_725.nextPosition();
                } else {
                    goto out_peg_727;
                }
            }
            result_peg_725.setValue((void*) "(");
        
        
        
        result_peg_725 = rule_s(stream, result_peg_725.getPosition());
            if (result_peg_725.error()){
                goto out_peg_727;
            }
        
        
        
        result_peg_725 = rule_integer(stream, result_peg_725.getPosition());
            if (result_peg_725.error()){
                goto out_peg_727;
            }
            arg1 = result_peg_725.getValues();
        
        
        
        result_peg_725 = rule_s(stream, result_peg_725.getPosition());
            if (result_peg_725.error()){
                goto out_peg_727;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_725.getPosition()))){
                    result_peg_725.nextPosition();
                } else {
                    goto out_peg_727;
                }
            }
            result_peg_725.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_725.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_725;
    stream.update(result_peg_725.getPosition());
    
    
    return result_peg_725;
    out_peg_727:
    Result result_peg_734(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_734.getPosition()))){
                    result_peg_734.nextPosition();
                } else {
                    goto out_peg_736;
                }
            }
            result_peg_734.setValue((void*) "helper");
            name = result_peg_734.getValues();
        
        
        
        result_peg_734 = rule_s(stream, result_peg_734.getPosition());
            if (result_peg_734.error()){
                goto out_peg_736;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_734.getPosition()))){
                    result_peg_734.nextPosition();
                } else {
                    goto out_peg_736;
                }
            }
            result_peg_734.setValue((void*) "(");
        
        
        
        result_peg_734 = rule_s(stream, result_peg_734.getPosition());
            if (result_peg_734.error()){
                goto out_peg_736;
            }
        
        
        
        result_peg_734 = rule_integer(stream, result_peg_734.getPosition());
            if (result_peg_734.error()){
                goto out_peg_736;
            }
            arg1 = result_peg_734.getValues();
        
        
        
        result_peg_734 = rule_s(stream, result_peg_734.getPosition());
            if (result_peg_734.error()){
                goto out_peg_736;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_734.getPosition()))){
                    result_peg_734.nextPosition();
                } else {
                    goto out_peg_736;
                }
            }
            result_peg_734.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, arg1);
                result_peg_734.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_734;
    stream.update(result_peg_734.getPosition());
    
    
    return result_peg_734;
    out_peg_736:
    Result result_peg_743(myposition);
    
    {
    
        for (int i = 0; i < 4; i++){
                if (compareCharCase("fvar"[i], stream.get(result_peg_743.getPosition()))){
                    result_peg_743.nextPosition();
                } else {
                    goto out_peg_745;
                }
            }
            result_peg_743.setValue((void*) "fvar");
            name = result_peg_743.getValues();
        
        
        
        result_peg_743 = rule_s(stream, result_peg_743.getPosition());
            if (result_peg_743.error()){
                goto out_peg_745;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_743.getPosition()))){
                    result_peg_743.nextPosition();
                } else {
                    goto out_peg_745;
                }
            }
            result_peg_743.setValue((void*) "(");
        
        
        
        result_peg_743 = rule_s(stream, result_peg_743.getPosition());
            if (result_peg_743.error()){
                goto out_peg_745;
            }
        
        
        
        result_peg_743 = rule_integer(stream, result_peg_743.getPosition());
            if (result_peg_743.error()){
                goto out_peg_745;
            }
            arg1 = result_peg_743.getValues();
        
        
        
        result_peg_743 = rule_s(stream, result_peg_743.getPosition());
            if (result_peg_743.error()){
                goto out_peg_745;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_743.getPosition()))){
                    result_peg_743.nextPosition();
                } else {
                    goto out_peg_745;
                }
            }
            result_peg_743.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_743.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_743;
    stream.update(result_peg_743.getPosition());
    
    
    return result_peg_743;
    out_peg_745:
    Result result_peg_752(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("target"[i], stream.get(result_peg_752.getPosition()))){
                    result_peg_752.nextPosition();
                } else {
                    goto out_peg_754;
                }
            }
            result_peg_752.setValue((void*) "target");
            name = result_peg_752.getValues();
        
        
        
        result_peg_752 = rule_s(stream, result_peg_752.getPosition());
            if (result_peg_752.error()){
                goto out_peg_754;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_752.getPosition()))){
                    result_peg_752.nextPosition();
                } else {
                    goto out_peg_754;
                }
            }
            result_peg_752.setValue((void*) "(");
        
        
        
        result_peg_752 = rule_s(stream, result_peg_752.getPosition());
            if (result_peg_752.error()){
                goto out_peg_754;
            }
        
        
        
        result_peg_752 = rule_integer(stream, result_peg_752.getPosition());
            if (result_peg_752.error()){
                goto out_peg_754;
            }
            arg1 = result_peg_752.getValues();
        
        
        
        result_peg_752 = rule_s(stream, result_peg_752.getPosition());
            if (result_peg_752.error()){
                goto out_peg_754;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_752.getPosition()))){
                    result_peg_752.nextPosition();
                } else {
                    goto out_peg_754;
                }
            }
            result_peg_752.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_752.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_752;
    stream.update(result_peg_752.getPosition());
    
    
    return result_peg_752;
    out_peg_754:
    Result result_peg_761(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("ishelper"[i], stream.get(result_peg_761.getPosition()))){
                    result_peg_761.nextPosition();
                } else {
                    goto out_peg_763;
                }
            }
            result_peg_761.setValue((void*) "ishelper");
            name = result_peg_761.getValues();
        
        
        
        result_peg_761 = rule_s(stream, result_peg_761.getPosition());
            if (result_peg_761.error()){
                goto out_peg_763;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_761.getPosition()))){
                    result_peg_761.nextPosition();
                } else {
                    goto out_peg_763;
                }
            }
            result_peg_761.setValue((void*) "(");
        
        
        
        result_peg_761 = rule_s(stream, result_peg_761.getPosition());
            if (result_peg_761.error()){
                goto out_peg_763;
            }
        
        
        
        result_peg_761 = rule_integer(stream, result_peg_761.getPosition());
            if (result_peg_761.error()){
                goto out_peg_763;
            }
            arg1 = result_peg_761.getValues();
        
        
        
        result_peg_761 = rule_s(stream, result_peg_761.getPosition());
            if (result_peg_761.error()){
                goto out_peg_763;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_761.getPosition()))){
                    result_peg_761.nextPosition();
                } else {
                    goto out_peg_763;
                }
            }
            result_peg_761.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_761.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_761;
    stream.update(result_peg_761.getPosition());
    
    
    return result_peg_761;
    out_peg_763:
    Result result_peg_770(myposition);
    
    {
    
        for (int i = 0; i < 12; i++){
                if (compareCharCase("animelemtime"[i], stream.get(result_peg_770.getPosition()))){
                    result_peg_770.nextPosition();
                } else {
                    goto out_peg_772;
                }
            }
            result_peg_770.setValue((void*) "animelemtime");
            name = result_peg_770.getValues();
        
        
        
        result_peg_770 = rule_s(stream, result_peg_770.getPosition());
            if (result_peg_770.error()){
                goto out_peg_772;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_770.getPosition()))){
                    result_peg_770.nextPosition();
                } else {
                    goto out_peg_772;
                }
            }
            result_peg_770.setValue((void*) "(");
        
        
        
        result_peg_770 = rule_s(stream, result_peg_770.getPosition());
            if (result_peg_770.error()){
                goto out_peg_772;
            }
        
        
        
        result_peg_770 = rule_integer(stream, result_peg_770.getPosition());
            if (result_peg_770.error()){
                goto out_peg_772;
            }
            arg1 = result_peg_770.getValues();
        
        
        
        result_peg_770 = rule_s(stream, result_peg_770.getPosition());
            if (result_peg_770.error()){
                goto out_peg_772;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_770.getPosition()))){
                    result_peg_770.nextPosition();
                } else {
                    goto out_peg_772;
                }
            }
            result_peg_770.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_770.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_770;
    stream.update(result_peg_770.getPosition());
    
    
    return result_peg_770;
    out_peg_772:
    Result result_peg_779(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("animelemno"[i], stream.get(result_peg_779.getPosition()))){
                    result_peg_779.nextPosition();
                } else {
                    goto out_peg_781;
                }
            }
            result_peg_779.setValue((void*) "animelemno");
            name = result_peg_779.getValues();
        
        
        
        result_peg_779 = rule_s(stream, result_peg_779.getPosition());
            if (result_peg_779.error()){
                goto out_peg_781;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_779.getPosition()))){
                    result_peg_779.nextPosition();
                } else {
                    goto out_peg_781;
                }
            }
            result_peg_779.setValue((void*) "(");
        
        
        
        result_peg_779 = rule_s(stream, result_peg_779.getPosition());
            if (result_peg_779.error()){
                goto out_peg_781;
            }
        
        
        
        result_peg_779 = rule_expr_c(stream, result_peg_779.getPosition());
            if (result_peg_779.error()){
                goto out_peg_781;
            }
            arg1 = result_peg_779.getValues();
        
        
        
        result_peg_779 = rule_s(stream, result_peg_779.getPosition());
            if (result_peg_779.error()){
                goto out_peg_781;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_779.getPosition()))){
                    result_peg_779.nextPosition();
                } else {
                    goto out_peg_781;
                }
            }
            result_peg_779.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_779.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_779;
    stream.update(result_peg_779.getPosition());
    
    
    return result_peg_779;
    out_peg_781:
    Result result_peg_788(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("animexist"[i], stream.get(result_peg_788.getPosition()))){
                    result_peg_788.nextPosition();
                } else {
                    goto out_peg_790;
                }
            }
            result_peg_788.setValue((void*) "animexist");
            name = result_peg_788.getValues();
        
        
        
        result_peg_788 = rule_s(stream, result_peg_788.getPosition());
            if (result_peg_788.error()){
                goto out_peg_790;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_788.getPosition()))){
                    result_peg_788.nextPosition();
                } else {
                    goto out_peg_790;
                }
            }
            result_peg_788.setValue((void*) "(");
        
        
        
        result_peg_788 = rule_s(stream, result_peg_788.getPosition());
            if (result_peg_788.error()){
                goto out_peg_790;
            }
        
        
        
        result_peg_788 = rule_expr_c(stream, result_peg_788.getPosition());
            if (result_peg_788.error()){
                goto out_peg_790;
            }
            arg1 = result_peg_788.getValues();
        
        
        
        result_peg_788 = rule_s(stream, result_peg_788.getPosition());
            if (result_peg_788.error()){
                goto out_peg_790;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_788.getPosition()))){
                    result_peg_788.nextPosition();
                } else {
                    goto out_peg_790;
                }
            }
            result_peg_788.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_788.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_788;
    stream.update(result_peg_788.getPosition());
    
    
    return result_peg_788;
    out_peg_790:
    Result result_peg_797(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projguarded"[i], stream.get(result_peg_797.getPosition()))){
                    result_peg_797.nextPosition();
                } else {
                    goto out_peg_799;
                }
            }
            result_peg_797.setValue((void*) "projguarded");
            name = result_peg_797.getValues();
        
        
        
        result_peg_797 = rule_s(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_797.getPosition()))){
                    result_peg_797.nextPosition();
                } else {
                    goto out_peg_799;
                }
            }
            result_peg_797.setValue((void*) "(");
        
        
        
        result_peg_797 = rule_s(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
        
        result_peg_797 = rule_expr_c(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
            arg1 = result_peg_797.getValues();
        
        
        
        result_peg_797 = rule_s(stream, result_peg_797.getPosition());
            if (result_peg_797.error()){
                goto out_peg_799;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_797.getPosition()))){
                    result_peg_797.nextPosition();
                } else {
                    goto out_peg_799;
                }
            }
            result_peg_797.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_797.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_797;
    stream.update(result_peg_797.getPosition());
    
    
    return result_peg_797;
    out_peg_799:
    Result result_peg_806(myposition);
    
    {
    
        for (int i = 0; i < 11; i++){
                if (compareCharCase("projcontact"[i], stream.get(result_peg_806.getPosition()))){
                    result_peg_806.nextPosition();
                } else {
                    goto out_peg_808;
                }
            }
            result_peg_806.setValue((void*) "projcontact");
            name = result_peg_806.getValues();
        
        
        
        result_peg_806 = rule_s(stream, result_peg_806.getPosition());
            if (result_peg_806.error()){
                goto out_peg_808;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_806.getPosition()))){
                    result_peg_806.nextPosition();
                } else {
                    goto out_peg_808;
                }
            }
            result_peg_806.setValue((void*) "=");
        
        
        
        result_peg_806 = rule_s(stream, result_peg_806.getPosition());
            if (result_peg_806.error()){
                goto out_peg_808;
            }
        
        
        
        result_peg_806 = rule_integer(stream, result_peg_806.getPosition());
            if (result_peg_806.error()){
                goto out_peg_808;
            }
        
        
        
        result_peg_806 = rule_function_rest(stream, result_peg_806.getPosition());
            if (result_peg_806.error()){
                goto out_peg_808;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_806.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_806;
    stream.update(result_peg_806.getPosition());
    
    
    return result_peg_806;
    out_peg_808:
    Result result_peg_814(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_814.getPosition()))){
                    result_peg_814.nextPosition();
                } else {
                    goto out_peg_816;
                }
            }
            result_peg_814.setValue((void*) "animelem");
            name = result_peg_814.getValues();
        
        
        
        result_peg_814 = rule_s(stream, result_peg_814.getPosition());
            if (result_peg_814.error()){
                goto out_peg_816;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_814.getPosition()))){
                    result_peg_814.nextPosition();
                } else {
                    goto out_peg_816;
                }
            }
            result_peg_814.setValue((void*) "=");
        
        
        
        result_peg_814 = rule_s(stream, result_peg_814.getPosition());
            if (result_peg_814.error()){
                goto out_peg_816;
            }
        
        
        
        result_peg_814 = rule_integer(stream, result_peg_814.getPosition());
            if (result_peg_814.error()){
                goto out_peg_816;
            }
        
        
        
        result_peg_814 = rule_function_rest(stream, result_peg_814.getPosition());
            if (result_peg_814.error()){
                goto out_peg_816;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeExpression();
                result_peg_814.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_814;
    stream.update(result_peg_814.getPosition());
    
    
    return result_peg_814;
    out_peg_816:
    Result result_peg_822(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("numhelper"[i], stream.get(result_peg_822.getPosition()))){
                    result_peg_822.nextPosition();
                } else {
                    goto out_peg_824;
                }
            }
            result_peg_822.setValue((void*) "numhelper");
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
                value = makeExpression();
                result_peg_822.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_822;
    stream.update(result_peg_822.getPosition());
    
    
    return result_peg_822;
    out_peg_824:
    Result result_peg_831(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_831.getPosition()))){
                    result_peg_831.nextPosition();
                } else {
                    goto out_peg_833;
                }
            }
            result_peg_831.setValue((void*) "helper");
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
                value = makeExpression();
                result_peg_831.setValue(value);
            }
        
        
    }
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = result_peg_831;
    stream.update(result_peg_831.getPosition());
    
    
    return result_peg_831;
    out_peg_833:
    
    if (column_peg_617.chunk9 == 0){
        column_peg_617.chunk9 = new Chunk9();
    }
    column_peg_617.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_840 = stream.getColumn(position);
    if (column_peg_840.chunk10 != 0 && column_peg_840.chunk10->chunk_function_rest.calculated()){
        return column_peg_840.chunk10->chunk_function_rest;
    }
    
    int myposition = position;
    
    
    Result result_peg_841(myposition);
    
    {
    
        result_peg_841 = rule_s(stream, result_peg_841.getPosition());
            if (result_peg_841.error()){
                goto out_peg_843;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_841.getPosition()))){
                    result_peg_841.nextPosition();
                } else {
                    goto out_peg_843;
                }
            }
            result_peg_841.setValue((void*) ",");
        
        
        
        result_peg_841 = rule_s(stream, result_peg_841.getPosition());
            if (result_peg_841.error()){
                goto out_peg_843;
            }
        
        
        
        result_peg_841 = rule_all_compare(stream, result_peg_841.getPosition());
            if (result_peg_841.error()){
                goto out_peg_843;
            }
        
        
        
        result_peg_841 = rule_s(stream, result_peg_841.getPosition());
            if (result_peg_841.error()){
                goto out_peg_843;
            }
        
        
        
        result_peg_841 = rule_integer(stream, result_peg_841.getPosition());
            if (result_peg_841.error()){
                goto out_peg_843;
            }
        
        
    }
    
    if (column_peg_840.chunk10 == 0){
        column_peg_840.chunk10 = new Chunk10();
    }
    column_peg_840.chunk10->chunk_function_rest = result_peg_841;
    stream.update(result_peg_841.getPosition());
    
    
    return result_peg_841;
    out_peg_843:
    
    if (column_peg_840.chunk10 == 0){
        column_peg_840.chunk10 = new Chunk10();
    }
    column_peg_840.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_848 = stream.getColumn(position);
    if (column_peg_848.chunk10 != 0 && column_peg_848.chunk10->chunk_keys.calculated()){
        return column_peg_848.chunk10->chunk_keys;
    }
    
    int myposition = position;
    
    
    Result result_peg_849(myposition);
    
    result_peg_849 = rule_key_value_list(stream, result_peg_849.getPosition());
    if (result_peg_849.error()){
        goto out_peg_850;
    }
    
    if (column_peg_848.chunk10 == 0){
        column_peg_848.chunk10 = new Chunk10();
    }
    column_peg_848.chunk10->chunk_keys = result_peg_849;
    stream.update(result_peg_849.getPosition());
    
    
    return result_peg_849;
    out_peg_850:
    
    if (column_peg_848.chunk10 == 0){
        column_peg_848.chunk10 = new Chunk10();
    }
    column_peg_848.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_851 = stream.getColumn(position);
    if (column_peg_851.chunk10 != 0 && column_peg_851.chunk10->chunk_key_value_list.calculated()){
        return column_peg_851.chunk10->chunk_key_value_list;
    }
    
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_852(myposition);
    
    {
    
        result_peg_852 = rule_key(stream, result_peg_852.getPosition());
            if (result_peg_852.error()){
                goto out_peg_854;
            }
            first = result_peg_852.getValues();
        
        
        
        result_peg_852.reset();
            do{
                Result result_peg_857(result_peg_852.getPosition());
                {
                
                    result_peg_857 = rule_s(stream, result_peg_857.getPosition());
                        if (result_peg_857.error()){
                            goto loop_peg_856;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_857.getPosition()))){
                                result_peg_857.nextPosition();
                            } else {
                                goto loop_peg_856;
                            }
                        }
                        result_peg_857.setValue((void*) ",");
                    
                    
                    
                    result_peg_857 = rule_s(stream, result_peg_857.getPosition());
                        if (result_peg_857.error()){
                            goto loop_peg_856;
                        }
                    
                    
                    
                    result_peg_857 = rule_key(stream, result_peg_857.getPosition());
                        if (result_peg_857.error()){
                            goto loop_peg_856;
                        }
                    
                    
                }
                result_peg_852.addResult(result_peg_857);
            } while (true);
            loop_peg_856:
            ;
            rest = result_peg_852.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_852.setValue(value);
            }
        
        
    }
    
    if (column_peg_851.chunk10 == 0){
        column_peg_851.chunk10 = new Chunk10();
    }
    column_peg_851.chunk10->chunk_key_value_list = result_peg_852;
    stream.update(result_peg_852.getPosition());
    
    
    return result_peg_852;
    out_peg_854:
    
    if (column_peg_851.chunk10 == 0){
        column_peg_851.chunk10 = new Chunk10();
    }
    column_peg_851.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_861 = stream.getColumn(position);
    if (column_peg_861.chunk10 != 0 && column_peg_861.chunk10->chunk_key.calculated()){
        return column_peg_861.chunk10->chunk_key;
    }
    
    int myposition = position;
    
    Value ok;
    Result result_peg_862(myposition);
    
    {
    
        result_peg_862 = rule_key_real(stream, result_peg_862.getPosition());
            if (result_peg_862.error()){
                goto out_peg_864;
            }
        
        Result result_peg_863 = result_peg_862;
        
        result_peg_862 = rule_key_rest(stream, result_peg_862.getPosition(), result_peg_863.getValues());
            if (result_peg_862.error()){
                goto out_peg_864;
            }
            ok = result_peg_862.getValues();
        
        
        
        Result result_peg_868(result_peg_862);
            result_peg_868 = rule_identifier(stream, result_peg_868.getPosition());
            if (result_peg_868.error()){
                goto not_peg_867;
            }
            goto out_peg_864;
            not_peg_867:
            result_peg_862.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_862.setValue(value);
            }
        
        
    }
    
    if (column_peg_861.chunk10 == 0){
        column_peg_861.chunk10 = new Chunk10();
    }
    column_peg_861.chunk10->chunk_key = result_peg_862;
    stream.update(result_peg_862.getPosition());
    
    
    return result_peg_862;
    out_peg_864:
    
    if (column_peg_861.chunk10 == 0){
        column_peg_861.chunk10 = new Chunk10();
    }
    column_peg_861.chunk10->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    int myposition = position;
    tail_peg_871:
    Value another;
    Value new_left;
    Result result_peg_870(myposition);
    {
    
        result_peg_870 = rule_s(stream, result_peg_870.getPosition());
            if (result_peg_870.error()){
                goto out_peg_873;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_870.getPosition()))){
                    result_peg_870.nextPosition();
                } else {
                    goto out_peg_873;
                }
            }
            result_peg_870.setValue((void*) "+");
        
        
        
        result_peg_870 = rule_key_real(stream, result_peg_870.getPosition());
            if (result_peg_870.error()){
                goto out_peg_873;
            }
            another = result_peg_870.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_870.setValue(value);
            }
            new_left = result_peg_870.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_870.getPosition();
    goto tail_peg_871;
    out_peg_873:
    Result result_peg_877(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_877.setValue(value);
            }
        
        
    }
    
    
    return result_peg_877;
    
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_879 = stream.getColumn(position);
    if (column_peg_879.chunk11 != 0 && column_peg_879.chunk11->chunk_key_real.calculated()){
        return column_peg_879.chunk11->chunk_key_real;
    }
    
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_880(myposition);
    
    {
    
        result_peg_880.reset();
            do{
                Result result_peg_883(result_peg_880.getPosition());
                result_peg_883 = rule_key_modifier(stream, result_peg_883.getPosition());
                if (result_peg_883.error()){
                    goto loop_peg_882;
                }
                result_peg_880.addResult(result_peg_883);
            } while (true);
            loop_peg_882:
            ;
            mods = result_peg_880.getValues();
        
        
        
        result_peg_880 = rule_key_name(stream, result_peg_880.getPosition());
            if (result_peg_880.error()){
                goto out_peg_885;
            }
            name = result_peg_880.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_880.setValue(value);
            }
        
        
    }
    
    if (column_peg_879.chunk11 == 0){
        column_peg_879.chunk11 = new Chunk11();
    }
    column_peg_879.chunk11->chunk_key_real = result_peg_880;
    stream.update(result_peg_880.getPosition());
    
    
    return result_peg_880;
    out_peg_885:
    
    if (column_peg_879.chunk11 == 0){
        column_peg_879.chunk11 = new Chunk11();
    }
    column_peg_879.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_886 = stream.getColumn(position);
    if (column_peg_886.chunk11 != 0 && column_peg_886.chunk11->chunk_key_modifier.calculated()){
        return column_peg_886.chunk11->chunk_key_modifier;
    }
    
    int myposition = position;
    
    Value num;
    Result result_peg_887(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_887.getPosition()))){
                    result_peg_887.nextPosition();
                } else {
                    goto out_peg_889;
                }
            }
            result_peg_887.setValue((void*) "~");
        
        
        
        result_peg_887.reset();
            do{
                Result result_peg_892(result_peg_887.getPosition());
                {
                    
                    char letter_peg_897 = stream.get(result_peg_892.getPosition());
                    if (letter_peg_897 != '\0' && strchr("0123456789", letter_peg_897) != NULL){
                        result_peg_892.nextPosition();
                        result_peg_892.setValue((void*) letter_peg_897);
                    } else {
                        goto out_peg_896;
                    }
                    
                }
                goto success_peg_893;
                out_peg_896:
                goto loop_peg_891;
                success_peg_893:
                ;
                result_peg_887.addResult(result_peg_892);
            } while (true);
            loop_peg_891:
            ;
            num = result_peg_887.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier(*parseDouble(num));
                result_peg_887.setValue(value);
            }
        
        
    }
    
    if (column_peg_886.chunk11 == 0){
        column_peg_886.chunk11 = new Chunk11();
    }
    column_peg_886.chunk11->chunk_key_modifier = result_peg_887;
    stream.update(result_peg_887.getPosition());
    
    
    return result_peg_887;
    out_peg_889:
    Result result_peg_898(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_898.getPosition()))){
                    result_peg_898.nextPosition();
                } else {
                    goto out_peg_900;
                }
            }
            result_peg_898.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_898.setValue(value);
            }
        
        
    }
    
    if (column_peg_886.chunk11 == 0){
        column_peg_886.chunk11 = new Chunk11();
    }
    column_peg_886.chunk11->chunk_key_modifier = result_peg_898;
    stream.update(result_peg_898.getPosition());
    
    
    return result_peg_898;
    out_peg_900:
    Result result_peg_901(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_901.getPosition()))){
                    result_peg_901.nextPosition();
                } else {
                    goto out_peg_903;
                }
            }
            result_peg_901.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_901.setValue(value);
            }
        
        
    }
    
    if (column_peg_886.chunk11 == 0){
        column_peg_886.chunk11 = new Chunk11();
    }
    column_peg_886.chunk11->chunk_key_modifier = result_peg_901;
    stream.update(result_peg_901.getPosition());
    
    
    return result_peg_901;
    out_peg_903:
    Result result_peg_904(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_904.getPosition()))){
                    result_peg_904.nextPosition();
                } else {
                    goto out_peg_906;
                }
            }
            result_peg_904.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_904.setValue(value);
            }
        
        
    }
    
    if (column_peg_886.chunk11 == 0){
        column_peg_886.chunk11 = new Chunk11();
    }
    column_peg_886.chunk11->chunk_key_modifier = result_peg_904;
    stream.update(result_peg_904.getPosition());
    
    
    return result_peg_904;
    out_peg_906:
    
    if (column_peg_886.chunk11 == 0){
        column_peg_886.chunk11 = new Chunk11();
    }
    column_peg_886.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_907 = stream.getColumn(position);
    if (column_peg_907.chunk11 != 0 && column_peg_907.chunk11->chunk_key_name.calculated()){
        return column_peg_907.chunk11->chunk_key_name;
    }
    
    int myposition = position;
    
    
    Result result_peg_908(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_908.getPosition()))){
            result_peg_908.nextPosition();
        } else {
            goto out_peg_909;
        }
    }
    result_peg_908.setValue((void*) "DB");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_908;
    stream.update(result_peg_908.getPosition());
    
    
    return result_peg_908;
    out_peg_909:
    Result result_peg_910(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_910.getPosition()))){
            result_peg_910.nextPosition();
        } else {
            goto out_peg_911;
        }
    }
    result_peg_910.setValue((void*) "B");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_910;
    stream.update(result_peg_910.getPosition());
    
    
    return result_peg_910;
    out_peg_911:
    Result result_peg_912(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_912.getPosition()))){
            result_peg_912.nextPosition();
        } else {
            goto out_peg_913;
        }
    }
    result_peg_912.setValue((void*) "DF");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_912;
    stream.update(result_peg_912.getPosition());
    
    
    return result_peg_912;
    out_peg_913:
    Result result_peg_914(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_914.getPosition()))){
            result_peg_914.nextPosition();
        } else {
            goto out_peg_915;
        }
    }
    result_peg_914.setValue((void*) "D");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_914;
    stream.update(result_peg_914.getPosition());
    
    
    return result_peg_914;
    out_peg_915:
    Result result_peg_916(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_916.getPosition()))){
            result_peg_916.nextPosition();
        } else {
            goto out_peg_917;
        }
    }
    result_peg_916.setValue((void*) "F");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_916;
    stream.update(result_peg_916.getPosition());
    
    
    return result_peg_916;
    out_peg_917:
    Result result_peg_918(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_918.getPosition()))){
            result_peg_918.nextPosition();
        } else {
            goto out_peg_919;
        }
    }
    result_peg_918.setValue((void*) "UF");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_918;
    stream.update(result_peg_918.getPosition());
    
    
    return result_peg_918;
    out_peg_919:
    Result result_peg_920(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_920.getPosition()))){
            result_peg_920.nextPosition();
        } else {
            goto out_peg_921;
        }
    }
    result_peg_920.setValue((void*) "UB");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_920;
    stream.update(result_peg_920.getPosition());
    
    
    return result_peg_920;
    out_peg_921:
    Result result_peg_922(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_922.getPosition()))){
            result_peg_922.nextPosition();
        } else {
            goto out_peg_923;
        }
    }
    result_peg_922.setValue((void*) "U");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_922;
    stream.update(result_peg_922.getPosition());
    
    
    return result_peg_922;
    out_peg_923:
    Result result_peg_924(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_924.getPosition()))){
            result_peg_924.nextPosition();
        } else {
            goto out_peg_925;
        }
    }
    result_peg_924.setValue((void*) "a");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_924;
    stream.update(result_peg_924.getPosition());
    
    
    return result_peg_924;
    out_peg_925:
    Result result_peg_926(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_926.getPosition()))){
            result_peg_926.nextPosition();
        } else {
            goto out_peg_927;
        }
    }
    result_peg_926.setValue((void*) "b");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_926;
    stream.update(result_peg_926.getPosition());
    
    
    return result_peg_926;
    out_peg_927:
    Result result_peg_928(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_928.getPosition()))){
            result_peg_928.nextPosition();
        } else {
            goto out_peg_929;
        }
    }
    result_peg_928.setValue((void*) "c");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_928;
    stream.update(result_peg_928.getPosition());
    
    
    return result_peg_928;
    out_peg_929:
    Result result_peg_930(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_930.getPosition()))){
            result_peg_930.nextPosition();
        } else {
            goto out_peg_931;
        }
    }
    result_peg_930.setValue((void*) "x");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_930;
    stream.update(result_peg_930.getPosition());
    
    
    return result_peg_930;
    out_peg_931:
    Result result_peg_932(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_932.getPosition()))){
            result_peg_932.nextPosition();
        } else {
            goto out_peg_933;
        }
    }
    result_peg_932.setValue((void*) "y");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_932;
    stream.update(result_peg_932.getPosition());
    
    
    return result_peg_932;
    out_peg_933:
    Result result_peg_934(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_934.getPosition()))){
            result_peg_934.nextPosition();
        } else {
            goto out_peg_935;
        }
    }
    result_peg_934.setValue((void*) "z");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_934;
    stream.update(result_peg_934.getPosition());
    
    
    return result_peg_934;
    out_peg_935:
    Result result_peg_936(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_936.getPosition()))){
            result_peg_936.nextPosition();
        } else {
            goto out_peg_937;
        }
    }
    result_peg_936.setValue((void*) "s");
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = result_peg_936;
    stream.update(result_peg_936.getPosition());
    
    
    return result_peg_936;
    out_peg_937:
    
    if (column_peg_907.chunk11 == 0){
        column_peg_907.chunk11 = new Chunk11();
    }
    column_peg_907.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_938 = stream.getColumn(position);
    if (column_peg_938.chunk11 != 0 && column_peg_938.chunk11->chunk_value.calculated()){
        return column_peg_938.chunk11->chunk_value;
    }
    
    int myposition = position;
    
    
    Result result_peg_939(myposition);
    
    result_peg_939 = rule_float(stream, result_peg_939.getPosition());
    if (result_peg_939.error()){
        goto out_peg_940;
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = result_peg_939;
    stream.update(result_peg_939.getPosition());
    
    
    return result_peg_939;
    out_peg_940:
    Result result_peg_941(myposition);
    
    result_peg_941 = rule_integer(stream, result_peg_941.getPosition());
    if (result_peg_941.error()){
        goto out_peg_942;
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = result_peg_941;
    stream.update(result_peg_941.getPosition());
    
    
    return result_peg_941;
    out_peg_942:
    Result result_peg_943(myposition);
    
    result_peg_943 = rule_keyword(stream, result_peg_943.getPosition());
    if (result_peg_943.error()){
        goto out_peg_944;
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = result_peg_943;
    stream.update(result_peg_943.getPosition());
    
    
    return result_peg_943;
    out_peg_944:
    Result result_peg_945(myposition);
    
    {
    
        Result result_peg_948(result_peg_945);
            result_peg_948 = rule_keyword(stream, result_peg_948.getPosition());
            if (result_peg_948.error()){
                goto not_peg_947;
            }
            goto out_peg_949;
            not_peg_947:
            result_peg_945.setValue((void*)0);
        
        
        
        result_peg_945 = rule_identifier(stream, result_peg_945.getPosition());
            if (result_peg_945.error()){
                goto out_peg_949;
            }
        
        
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = result_peg_945;
    stream.update(result_peg_945.getPosition());
    
    
    return result_peg_945;
    out_peg_949:
    Result result_peg_950(myposition);
    
    result_peg_950 = rule_range(stream, result_peg_950.getPosition());
    if (result_peg_950.error()){
        goto out_peg_951;
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = result_peg_950;
    stream.update(result_peg_950.getPosition());
    
    
    return result_peg_950;
    out_peg_951:
    Result result_peg_952(myposition);
    
    result_peg_952 = rule_string(stream, result_peg_952.getPosition());
    if (result_peg_952.error()){
        goto out_peg_953;
    }
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = result_peg_952;
    stream.update(result_peg_952.getPosition());
    
    
    return result_peg_952;
    out_peg_953:
    
    if (column_peg_938.chunk11 == 0){
        column_peg_938.chunk11 = new Chunk11();
    }
    column_peg_938.chunk11->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_954 = stream.getColumn(position);
    if (column_peg_954.chunk11 != 0 && column_peg_954.chunk11->chunk_keyword.calculated()){
        return column_peg_954.chunk11->chunk_keyword;
    }
    
    int myposition = position;
    
    
    Result result_peg_955(myposition);
    
    {
    
        result_peg_955 = rule_keyword_real(stream, result_peg_955.getPosition());
            if (result_peg_955.error()){
                goto out_peg_957;
            }
        
        Result result_peg_956 = result_peg_955;
        
        Result result_peg_960(result_peg_955);
            result_peg_960 = rule_alpha_digit(stream, result_peg_960.getPosition());
            if (result_peg_960.error()){
                goto not_peg_959;
            }
            goto out_peg_957;
            not_peg_959:
            result_peg_955.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_956.getValues();
                result_peg_955.setValue(value);
            }
        
        
    }
    
    if (column_peg_954.chunk11 == 0){
        column_peg_954.chunk11 = new Chunk11();
    }
    column_peg_954.chunk11->chunk_keyword = result_peg_955;
    stream.update(result_peg_955.getPosition());
    
    
    return result_peg_955;
    out_peg_957:
    
    if (column_peg_954.chunk11 == 0){
        column_peg_954.chunk11 = new Chunk11();
    }
    column_peg_954.chunk11->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_961 = stream.getColumn(position);
    if (column_peg_961.chunk12 != 0 && column_peg_961.chunk12->chunk_keyword_real.calculated()){
        return column_peg_961.chunk12->chunk_keyword_real;
    }
    
    int myposition = position;
    
    
    Result result_peg_962(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_962.getPosition()))){
                    result_peg_962.nextPosition();
                } else {
                    goto out_peg_964;
                }
            }
            result_peg_962.setValue((void*) "vel");
        
        
        
        result_peg_962 = rule_s(stream, result_peg_962.getPosition());
            if (result_peg_962.error()){
                goto out_peg_964;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_962.getPosition()))){
                    result_peg_962.nextPosition();
                } else {
                    goto out_peg_964;
                }
            }
            result_peg_962.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_962.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_962;
    stream.update(result_peg_962.getPosition());
    
    
    return result_peg_962;
    out_peg_964:
    Result result_peg_967(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_967.getPosition()))){
                    result_peg_967.nextPosition();
                } else {
                    goto out_peg_969;
                }
            }
            result_peg_967.setValue((void*) "vel");
        
        
        
        result_peg_967 = rule_s(stream, result_peg_967.getPosition());
            if (result_peg_967.error()){
                goto out_peg_969;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_967.getPosition()))){
                    result_peg_967.nextPosition();
                } else {
                    goto out_peg_969;
                }
            }
            result_peg_967.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_967.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_967;
    stream.update(result_peg_967.getPosition());
    
    
    return result_peg_967;
    out_peg_969:
    Result result_peg_972(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_972.getPosition()))){
                    result_peg_972.nextPosition();
                } else {
                    goto out_peg_974;
                }
            }
            result_peg_972.setValue((void*) "pos");
        
        
        
        result_peg_972 = rule_s(stream, result_peg_972.getPosition());
            if (result_peg_972.error()){
                goto out_peg_974;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_972.getPosition()))){
                    result_peg_972.nextPosition();
                } else {
                    goto out_peg_974;
                }
            }
            result_peg_972.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_972.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_972;
    stream.update(result_peg_972.getPosition());
    
    
    return result_peg_972;
    out_peg_974:
    Result result_peg_977(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_977.getPosition()))){
                    result_peg_977.nextPosition();
                } else {
                    goto out_peg_979;
                }
            }
            result_peg_977.setValue((void*) "pos");
        
        
        
        result_peg_977 = rule_s(stream, result_peg_977.getPosition());
            if (result_peg_977.error()){
                goto out_peg_979;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_977.getPosition()))){
                    result_peg_977.nextPosition();
                } else {
                    goto out_peg_979;
                }
            }
            result_peg_977.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_977.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_977;
    stream.update(result_peg_977.getPosition());
    
    
    return result_peg_977;
    out_peg_979:
    Result result_peg_982(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_982.getPosition()))){
                    result_peg_982.nextPosition();
                } else {
                    goto out_peg_984;
                }
            }
            result_peg_982.setValue((void*) "p2dist");
        
        
        
        result_peg_982 = rule_s(stream, result_peg_982.getPosition());
            if (result_peg_982.error()){
                goto out_peg_984;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_982.getPosition()))){
                    result_peg_982.nextPosition();
                } else {
                    goto out_peg_984;
                }
            }
            result_peg_982.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_982.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_982;
    stream.update(result_peg_982.getPosition());
    
    
    return result_peg_982;
    out_peg_984:
    Result result_peg_987(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_987.getPosition()))){
                    result_peg_987.nextPosition();
                } else {
                    goto out_peg_989;
                }
            }
            result_peg_987.setValue((void*) "p2dist");
        
        
        
        result_peg_987 = rule_s(stream, result_peg_987.getPosition());
            if (result_peg_987.error()){
                goto out_peg_989;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_987.getPosition()))){
                    result_peg_987.nextPosition();
                } else {
                    goto out_peg_989;
                }
            }
            result_peg_987.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_987.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_987;
    stream.update(result_peg_987.getPosition());
    
    
    return result_peg_987;
    out_peg_989:
    Result result_peg_992(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_992.getPosition()))){
                    result_peg_992.nextPosition();
                } else {
                    goto out_peg_994;
                }
            }
            result_peg_992.setValue((void*) "p1dist");
        
        
        
        result_peg_992 = rule_s(stream, result_peg_992.getPosition());
            if (result_peg_992.error()){
                goto out_peg_994;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_992.getPosition()))){
                    result_peg_992.nextPosition();
                } else {
                    goto out_peg_994;
                }
            }
            result_peg_992.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_992.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_992;
    stream.update(result_peg_992.getPosition());
    
    
    return result_peg_992;
    out_peg_994:
    Result result_peg_997(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_997.getPosition()))){
                    result_peg_997.nextPosition();
                } else {
                    goto out_peg_999;
                }
            }
            result_peg_997.setValue((void*) "p1dist");
        
        
        
        result_peg_997 = rule_s(stream, result_peg_997.getPosition());
            if (result_peg_997.error()){
                goto out_peg_999;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_997.getPosition()))){
                    result_peg_997.nextPosition();
                } else {
                    goto out_peg_999;
                }
            }
            result_peg_997.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_997.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_997;
    stream.update(result_peg_997.getPosition());
    
    
    return result_peg_997;
    out_peg_999:
    Result result_peg_1002(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1002.getPosition()))){
                    result_peg_1002.nextPosition();
                } else {
                    goto out_peg_1004;
                }
            }
            result_peg_1002.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1002 = rule_s(stream, result_peg_1002.getPosition());
            if (result_peg_1002.error()){
                goto out_peg_1004;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1002.getPosition()))){
                    result_peg_1002.nextPosition();
                } else {
                    goto out_peg_1004;
                }
            }
            result_peg_1002.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1002.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1002;
    stream.update(result_peg_1002.getPosition());
    
    
    return result_peg_1002;
    out_peg_1004:
    Result result_peg_1007(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1007.getPosition()))){
                    result_peg_1007.nextPosition();
                } else {
                    goto out_peg_1009;
                }
            }
            result_peg_1007.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1007 = rule_s(stream, result_peg_1007.getPosition());
            if (result_peg_1007.error()){
                goto out_peg_1009;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1007.getPosition()))){
                    result_peg_1007.nextPosition();
                } else {
                    goto out_peg_1009;
                }
            }
            result_peg_1007.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1007.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1007;
    stream.update(result_peg_1007.getPosition());
    
    
    return result_peg_1007;
    out_peg_1009:
    Result result_peg_1012(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1012.getPosition()))){
                    result_peg_1012.nextPosition();
                } else {
                    goto out_peg_1014;
                }
            }
            result_peg_1012.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1012 = rule_s(stream, result_peg_1012.getPosition());
            if (result_peg_1012.error()){
                goto out_peg_1014;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1012.getPosition()))){
                    result_peg_1012.nextPosition();
                } else {
                    goto out_peg_1014;
                }
            }
            result_peg_1012.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1012.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1012;
    stream.update(result_peg_1012.getPosition());
    
    
    return result_peg_1012;
    out_peg_1014:
    Result result_peg_1017(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1017.getPosition()))){
                    result_peg_1017.nextPosition();
                } else {
                    goto out_peg_1019;
                }
            }
            result_peg_1017.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1017 = rule_s(stream, result_peg_1017.getPosition());
            if (result_peg_1017.error()){
                goto out_peg_1019;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1017.getPosition()))){
                    result_peg_1017.nextPosition();
                } else {
                    goto out_peg_1019;
                }
            }
            result_peg_1017.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1017.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1017;
    stream.update(result_peg_1017.getPosition());
    
    
    return result_peg_1017;
    out_peg_1019:
    Result result_peg_1022(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1022.getPosition()))){
                    result_peg_1022.nextPosition();
                } else {
                    goto out_peg_1024;
                }
            }
            result_peg_1022.setValue((void*) "screenpos");
        
        
        
        result_peg_1022 = rule_s(stream, result_peg_1022.getPosition());
            if (result_peg_1022.error()){
                goto out_peg_1024;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1022.getPosition()))){
                    result_peg_1022.nextPosition();
                } else {
                    goto out_peg_1024;
                }
            }
            result_peg_1022.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_1022.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1022;
    stream.update(result_peg_1022.getPosition());
    
    
    return result_peg_1022;
    out_peg_1024:
    Result result_peg_1027(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1027.getPosition()))){
                    result_peg_1027.nextPosition();
                } else {
                    goto out_peg_1029;
                }
            }
            result_peg_1027.setValue((void*) "screenpos");
        
        
        
        result_peg_1027 = rule_s(stream, result_peg_1027.getPosition());
            if (result_peg_1027.error()){
                goto out_peg_1029;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1027.getPosition()))){
                    result_peg_1027.nextPosition();
                } else {
                    goto out_peg_1029;
                }
            }
            result_peg_1027.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1027.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1027;
    stream.update(result_peg_1027.getPosition());
    
    
    return result_peg_1027;
    out_peg_1029:
    Result result_peg_1032(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("M-"[i], stream.get(result_peg_1032.getPosition()))){
                    result_peg_1032.nextPosition();
                } else {
                    goto out_peg_1034;
                }
            }
            result_peg_1032.setValue((void*) "M-");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("M-");
                result_peg_1032.setValue(value);
            }
        
        
    }
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = result_peg_1032;
    stream.update(result_peg_1032.getPosition());
    
    
    return result_peg_1032;
    out_peg_1034:
    
    if (column_peg_961.chunk12 == 0){
        column_peg_961.chunk12 = new Chunk12();
    }
    column_peg_961.chunk12->chunk_keyword_real = errorResult;
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

        
