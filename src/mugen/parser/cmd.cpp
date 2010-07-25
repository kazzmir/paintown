

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
(chunk12 != NULL ? ((chunk12->chunk_value.calculated() ? 1 : 0)
+ (chunk12->chunk_helper.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__name.calculated() ? 1 : 0)
+ (chunk12->chunk_helper__identifier.calculated() ? 1 : 0)
+ (chunk12->chunk_hitflag.calculated() ? 1 : 0)) : 0)
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
    
    RuleTrace trace_peg_196(stream, "assignment");
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
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_168.getPosition()))){
                    result_peg_168.nextPosition();
                } else {
                    goto out_peg_170;
                }
            }
            result_peg_168.setValue((void*) "!=");
        
        
        
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
                value = makeAttribute(name, negateExpression(exp));
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
    Result result_peg_175(myposition);
    
    {
    
        result_peg_175 = rule_identifier(stream, result_peg_175.getPosition());
            if (result_peg_175.error()){
                goto out_peg_177;
            }
            name = result_peg_175.getValues();
        
        
        
        result_peg_175 = rule_s(stream, result_peg_175.getPosition());
            if (result_peg_175.error()){
                goto out_peg_177;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_175.getPosition()))){
                    result_peg_175.nextPosition();
                } else {
                    goto out_peg_177;
                }
            }
            result_peg_175.setValue((void*) "=");
        
        
        
        result_peg_175 = rule_s(stream, result_peg_175.getPosition());
            if (result_peg_175.error()){
                goto out_peg_177;
            }
        
        
        
        Result result_peg_182(result_peg_175.getPosition());
            result_peg_182 = rule_line_end(stream, result_peg_182.getPosition());
            if (result_peg_182.error()){
                goto out_peg_177;
            }
        
        
        
        {
                Value value((void*) 0);
                value = makeAttribute(name);
                result_peg_175.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_175;
    stream.update(result_peg_175.getPosition());
    
    
    return result_peg_175;
    out_peg_177:
    Result result_peg_183(myposition);
    
    {
    
        result_peg_183 = rule_identifier(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
            name = result_peg_183.getValues();
        
        
        
        result_peg_183 = rule_s(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_183.getPosition()))){
                    result_peg_183.nextPosition();
                } else {
                    goto out_peg_185;
                }
            }
            result_peg_183.setValue((void*) "(");
        
        
        
        result_peg_183 = rule_s(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
        
        
        
        result_peg_183 = rule_integer(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
            index = result_peg_183.getValues();
        
        
        
        result_peg_183 = rule_s(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_183.getPosition()))){
                    result_peg_183.nextPosition();
                } else {
                    goto out_peg_185;
                }
            }
            result_peg_183.setValue((void*) ")");
        
        
        
        result_peg_183 = rule_s(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_183.getPosition()))){
                    result_peg_183.nextPosition();
                } else {
                    goto out_peg_185;
                }
            }
            result_peg_183.setValue((void*) "=");
        
        
        
        result_peg_183 = rule_s(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
        
        
        
        result_peg_183 = rule_expr(stream, result_peg_183.getPosition());
            if (result_peg_183.error()){
                goto out_peg_185;
            }
            exp = result_peg_183.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIndexedAttribute(name, index, exp);
                result_peg_183.setValue(value);
            }
        
        
    }
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = result_peg_183;
    stream.update(result_peg_183.getPosition());
    
    
    return result_peg_183;
    out_peg_185:
    
    if (column_peg_153.chunk2 == 0){
        column_peg_153.chunk2 = new Chunk2();
    }
    column_peg_153.chunk2->chunk_assignment = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_identifier(Stream & stream, const int position){
    
    Column & column_peg_197 = stream.getColumn(position);
    if (column_peg_197.chunk2 != 0 && column_peg_197.chunk2->chunk_identifier.calculated()){
        return column_peg_197.chunk2->chunk_identifier;
    }
    
    RuleTrace trace_peg_205(stream, "identifier");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_198(myposition);
    
    {
    
        result_peg_198 = rule_name(stream, result_peg_198.getPosition());
            if (result_peg_198.error()){
                goto out_peg_200;
            }
            first = result_peg_198.getValues();
        
        
        
        result_peg_198.reset();
            do{
                Result result_peg_203(result_peg_198.getPosition());
                {
                
                    for (int i = 0; i < 1; i++){
                            if (compareChar("."[i], stream.get(result_peg_203.getPosition()))){
                                result_peg_203.nextPosition();
                            } else {
                                goto loop_peg_202;
                            }
                        }
                        result_peg_203.setValue((void*) ".");
                    
                    
                    
                    result_peg_203 = rule_name(stream, result_peg_203.getPosition());
                        if (result_peg_203.error()){
                            goto loop_peg_202;
                        }
                    
                    
                }
                result_peg_198.addResult(result_peg_203);
            } while (true);
            loop_peg_202:
            ;
            rest = result_peg_198.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeIdentifier(first, rest);
                result_peg_198.setValue(value);
            }
        
        
    }
    
    if (column_peg_197.chunk2 == 0){
        column_peg_197.chunk2 = new Chunk2();
    }
    column_peg_197.chunk2->chunk_identifier = result_peg_198;
    stream.update(result_peg_198.getPosition());
    
    
    return result_peg_198;
    out_peg_200:
    
    if (column_peg_197.chunk2 == 0){
        column_peg_197.chunk2 = new Chunk2();
    }
    column_peg_197.chunk2->chunk_identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_integer(Stream & stream, const int position){
    
    Column & column_peg_206 = stream.getColumn(position);
    if (column_peg_206.chunk2 != 0 && column_peg_206.chunk2->chunk_integer.calculated()){
        return column_peg_206.chunk2->chunk_integer;
    }
    
    RuleTrace trace_peg_224(stream, "integer");
    int myposition = position;
    
    
    Result result_peg_207(myposition);
    
    {
    
        int save_peg_209 = result_peg_207.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_207.getPosition()))){
                        result_peg_207.nextPosition();
                    } else {
                        goto out_peg_212;
                    }
                }
                result_peg_207.setValue((void*) "-");
                    
            }
            goto success_peg_210;
            out_peg_212:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_207.getPosition()))){
                        result_peg_207.nextPosition();
                    } else {
                        goto out_peg_214;
                    }
                }
                result_peg_207.setValue((void*) "+");
                    
            }
            goto success_peg_210;
            out_peg_214:
            
            result_peg_207 = Result(save_peg_209);
            result_peg_207.setValue((void*) 0);
            
            success_peg_210:
            ;
        
        Result result_peg_208 = result_peg_207;
        
        result_peg_207.reset();
            do{
                Result result_peg_217(result_peg_207.getPosition());
                {
                    
                    char letter_peg_222 = stream.get(result_peg_217.getPosition());
                    if (letter_peg_222 != '\0' && strchr("0123456789", letter_peg_222) != NULL){
                        result_peg_217.nextPosition();
                        result_peg_217.setValue((void*) (long) letter_peg_222);
                    } else {
                        goto out_peg_221;
                    }
                    
                }
                goto success_peg_218;
                out_peg_221:
                goto loop_peg_216;
                success_peg_218:
                ;
                result_peg_207.addResult(result_peg_217);
            } while (true);
            loop_peg_216:
            if (result_peg_207.matches() == 0){
                goto out_peg_223;
            }
        
        Result result_peg_215 = result_peg_207;
        
        {
                Value value((void*) 0);
                value = makeInteger(result_peg_208.getValues(), result_peg_215.getValues());
                result_peg_207.setValue(value);
            }
        
        
    }
    
    if (column_peg_206.chunk2 == 0){
        column_peg_206.chunk2 = new Chunk2();
    }
    column_peg_206.chunk2->chunk_integer = result_peg_207;
    stream.update(result_peg_207.getPosition());
    
    
    return result_peg_207;
    out_peg_223:
    
    if (column_peg_206.chunk2 == 0){
        column_peg_206.chunk2 = new Chunk2();
    }
    column_peg_206.chunk2->chunk_integer = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_float(Stream & stream, const int position){
    
    Column & column_peg_225 = stream.getColumn(position);
    if (column_peg_225.chunk2 != 0 && column_peg_225.chunk2->chunk_float.calculated()){
        return column_peg_225.chunk2->chunk_float;
    }
    
    RuleTrace trace_peg_270(stream, "float");
    int myposition = position;
    
    Value left;
    Value right;
    Result result_peg_226(myposition);
    
    {
    
        int save_peg_228 = result_peg_226.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_226.getPosition()))){
                        result_peg_226.nextPosition();
                    } else {
                        goto out_peg_231;
                    }
                }
                result_peg_226.setValue((void*) "-");
                    
            }
            goto success_peg_229;
            out_peg_231:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_226.getPosition()))){
                        result_peg_226.nextPosition();
                    } else {
                        goto out_peg_233;
                    }
                }
                result_peg_226.setValue((void*) "+");
                    
            }
            goto success_peg_229;
            out_peg_233:
            
            result_peg_226 = Result(save_peg_228);
            result_peg_226.setValue((void*) 0);
            
            success_peg_229:
            ;
        
        Result result_peg_227 = result_peg_226;
        
        result_peg_226.reset();
            do{
                Result result_peg_236(result_peg_226.getPosition());
                {
                    
                    char letter_peg_241 = stream.get(result_peg_236.getPosition());
                    if (letter_peg_241 != '\0' && strchr("0123456789", letter_peg_241) != NULL){
                        result_peg_236.nextPosition();
                        result_peg_236.setValue((void*) (long) letter_peg_241);
                    } else {
                        goto out_peg_240;
                    }
                    
                }
                goto success_peg_237;
                out_peg_240:
                goto loop_peg_235;
                success_peg_237:
                ;
                result_peg_226.addResult(result_peg_236);
            } while (true);
            loop_peg_235:
            ;
            left = result_peg_226.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_226.getPosition()))){
                    result_peg_226.nextPosition();
                } else {
                    goto out_peg_243;
                }
            }
            result_peg_226.setValue((void*) ".");
        
        
        
        result_peg_226.reset();
            do{
                Result result_peg_246(result_peg_226.getPosition());
                {
                    
                    char letter_peg_251 = stream.get(result_peg_246.getPosition());
                    if (letter_peg_251 != '\0' && strchr("0123456789", letter_peg_251) != NULL){
                        result_peg_246.nextPosition();
                        result_peg_246.setValue((void*) (long) letter_peg_251);
                    } else {
                        goto out_peg_250;
                    }
                    
                }
                goto success_peg_247;
                out_peg_250:
                goto loop_peg_245;
                success_peg_247:
                ;
                result_peg_226.addResult(result_peg_246);
            } while (true);
            loop_peg_245:
            if (result_peg_226.matches() == 0){
                goto out_peg_243;
            }
            right = result_peg_226.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_227.getValues(), parseDouble(left,right));
                result_peg_226.setValue(value);
            }
        
        
    }
    
    if (column_peg_225.chunk2 == 0){
        column_peg_225.chunk2 = new Chunk2();
    }
    column_peg_225.chunk2->chunk_float = result_peg_226;
    stream.update(result_peg_226.getPosition());
    
    
    return result_peg_226;
    out_peg_243:
    Result result_peg_252(myposition);
    
    {
    
        int save_peg_254 = result_peg_252.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_252.getPosition()))){
                        result_peg_252.nextPosition();
                    } else {
                        goto out_peg_257;
                    }
                }
                result_peg_252.setValue((void*) "-");
                    
            }
            goto success_peg_255;
            out_peg_257:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_252.getPosition()))){
                        result_peg_252.nextPosition();
                    } else {
                        goto out_peg_259;
                    }
                }
                result_peg_252.setValue((void*) "+");
                    
            }
            goto success_peg_255;
            out_peg_259:
            
            result_peg_252 = Result(save_peg_254);
            result_peg_252.setValue((void*) 0);
            
            success_peg_255:
            ;
        
        Result result_peg_253 = result_peg_252;
        
        result_peg_252.reset();
            do{
                Result result_peg_262(result_peg_252.getPosition());
                {
                    
                    char letter_peg_267 = stream.get(result_peg_262.getPosition());
                    if (letter_peg_267 != '\0' && strchr("0123456789", letter_peg_267) != NULL){
                        result_peg_262.nextPosition();
                        result_peg_262.setValue((void*) (long) letter_peg_267);
                    } else {
                        goto out_peg_266;
                    }
                    
                }
                goto success_peg_263;
                out_peg_266:
                goto loop_peg_261;
                success_peg_263:
                ;
                result_peg_252.addResult(result_peg_262);
            } while (true);
            loop_peg_261:
            if (result_peg_252.matches() == 0){
                goto out_peg_268;
            }
            left = result_peg_252.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("."[i], stream.get(result_peg_252.getPosition()))){
                    result_peg_252.nextPosition();
                } else {
                    goto out_peg_268;
                }
            }
            result_peg_252.setValue((void*) ".");
        
        
        
        {
                Value value((void*) 0);
                value = makeNumber(result_peg_253.getValues(), parseDouble(left));
                result_peg_252.setValue(value);
            }
        
        
    }
    
    if (column_peg_225.chunk2 == 0){
        column_peg_225.chunk2 = new Chunk2();
    }
    column_peg_225.chunk2->chunk_float = result_peg_252;
    stream.update(result_peg_252.getPosition());
    
    
    return result_peg_252;
    out_peg_268:
    
    if (column_peg_225.chunk2 == 0){
        column_peg_225.chunk2 = new Chunk2();
    }
    column_peg_225.chunk2->chunk_float = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_string(Stream & stream, const int position){
    
    Column & column_peg_271 = stream.getColumn(position);
    if (column_peg_271.chunk2 != 0 && column_peg_271.chunk2->chunk_string.calculated()){
        return column_peg_271.chunk2->chunk_string;
    }
    
    RuleTrace trace_peg_286(stream, "string");
    int myposition = position;
    
    Value contents;
    Result result_peg_272(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_272.getPosition()))){
                    result_peg_272.nextPosition();
                } else {
                    goto out_peg_274;
                }
            }
            result_peg_272.setValue((void*) "\"");
        
        
        
        result_peg_272.reset();
            do{
                Result result_peg_277(result_peg_272.getPosition());
                {
                
                    Result result_peg_280(result_peg_277);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\""[i], stream.get(result_peg_280.getPosition()))){
                                result_peg_280.nextPosition();
                            } else {
                                goto not_peg_279;
                            }
                        }
                        result_peg_280.setValue((void*) "\"");
                        goto loop_peg_276;
                        not_peg_279:
                        result_peg_277.setValue((void*)0);
                    
                    
                    
                    Result result_peg_283(result_peg_277);
                        for (int i = 0; i < 1; i++){
                            if (compareChar("\n"[i], stream.get(result_peg_283.getPosition()))){
                                result_peg_283.nextPosition();
                            } else {
                                goto not_peg_282;
                            }
                        }
                        result_peg_283.setValue((void*) "\n");
                        goto loop_peg_276;
                        not_peg_282:
                        result_peg_277.setValue((void*)0);
                    
                    
                    
                    char temp_peg_284 = stream.get(result_peg_277.getPosition());
                        if (temp_peg_284 != '\0'){
                            result_peg_277.setValue((void*) (long) temp_peg_284);
                            result_peg_277.nextPosition();
                        } else {
                            goto loop_peg_276;
                        }
                    
                    
                }
                result_peg_272.addResult(result_peg_277);
            } while (true);
            loop_peg_276:
            ;
            contents = result_peg_272.getValues();
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("\""[i], stream.get(result_peg_272.getPosition()))){
                    result_peg_272.nextPosition();
                } else {
                    goto out_peg_274;
                }
            }
            result_peg_272.setValue((void*) "\"");
        
        
        
        {
                Value value((void*) 0);
                value = makeString(contents);
                result_peg_272.setValue(value);
            }
        
        
    }
    
    if (column_peg_271.chunk2 == 0){
        column_peg_271.chunk2 = new Chunk2();
    }
    column_peg_271.chunk2->chunk_string = result_peg_272;
    stream.update(result_peg_272.getPosition());
    
    
    return result_peg_272;
    out_peg_274:
    
    if (column_peg_271.chunk2 == 0){
        column_peg_271.chunk2 = new Chunk2();
    }
    column_peg_271.chunk2->chunk_string = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_range(Stream & stream, const int position){
    
    Column & column_peg_287 = stream.getColumn(position);
    if (column_peg_287.chunk3 != 0 && column_peg_287.chunk3->chunk_range.calculated()){
        return column_peg_287.chunk3->chunk_range;
    }
    
    RuleTrace trace_peg_332(stream, "range");
    int myposition = position;
    
    Value low;
    Value high;
    Result result_peg_288(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_288.getPosition()))){
                    result_peg_288.nextPosition();
                } else {
                    goto out_peg_290;
                }
            }
            result_peg_288.setValue((void*) "[");
        
        
        
        result_peg_288 = rule_s(stream, result_peg_288.getPosition());
            if (result_peg_288.error()){
                goto out_peg_290;
            }
        
        
        
        result_peg_288 = rule_expr_c(stream, result_peg_288.getPosition());
            if (result_peg_288.error()){
                goto out_peg_290;
            }
            low = result_peg_288.getValues();
        
        
        
        result_peg_288 = rule_s(stream, result_peg_288.getPosition());
            if (result_peg_288.error()){
                goto out_peg_290;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_288.getPosition()))){
                    result_peg_288.nextPosition();
                } else {
                    goto out_peg_290;
                }
            }
            result_peg_288.setValue((void*) ",");
        
        
        
        result_peg_288 = rule_s(stream, result_peg_288.getPosition());
            if (result_peg_288.error()){
                goto out_peg_290;
            }
        
        
        
        result_peg_288 = rule_expr_c(stream, result_peg_288.getPosition());
            if (result_peg_288.error()){
                goto out_peg_290;
            }
            high = result_peg_288.getValues();
        
        
        
        result_peg_288 = rule_s(stream, result_peg_288.getPosition());
            if (result_peg_288.error()){
                goto out_peg_290;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_288.getPosition()))){
                    result_peg_288.nextPosition();
                } else {
                    goto out_peg_290;
                }
            }
            result_peg_288.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllInclusive, low, high);
                result_peg_288.setValue(value);
            }
        
        
    }
    
    if (column_peg_287.chunk3 == 0){
        column_peg_287.chunk3 = new Chunk3();
    }
    column_peg_287.chunk3->chunk_range = result_peg_288;
    stream.update(result_peg_288.getPosition());
    
    
    return result_peg_288;
    out_peg_290:
    Result result_peg_299(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_299.getPosition()))){
                    result_peg_299.nextPosition();
                } else {
                    goto out_peg_301;
                }
            }
            result_peg_299.setValue((void*) "(");
        
        
        
        result_peg_299 = rule_s(stream, result_peg_299.getPosition());
            if (result_peg_299.error()){
                goto out_peg_301;
            }
        
        
        
        result_peg_299 = rule_expr_c(stream, result_peg_299.getPosition());
            if (result_peg_299.error()){
                goto out_peg_301;
            }
            low = result_peg_299.getValues();
        
        
        
        result_peg_299 = rule_s(stream, result_peg_299.getPosition());
            if (result_peg_299.error()){
                goto out_peg_301;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_299.getPosition()))){
                    result_peg_299.nextPosition();
                } else {
                    goto out_peg_301;
                }
            }
            result_peg_299.setValue((void*) ",");
        
        
        
        result_peg_299 = rule_s(stream, result_peg_299.getPosition());
            if (result_peg_299.error()){
                goto out_peg_301;
            }
        
        
        
        result_peg_299 = rule_expr_c(stream, result_peg_299.getPosition());
            if (result_peg_299.error()){
                goto out_peg_301;
            }
            high = result_peg_299.getValues();
        
        
        
        result_peg_299 = rule_s(stream, result_peg_299.getPosition());
            if (result_peg_299.error()){
                goto out_peg_301;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_299.getPosition()))){
                    result_peg_299.nextPosition();
                } else {
                    goto out_peg_301;
                }
            }
            result_peg_299.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::AllExclusive, low, high);
                result_peg_299.setValue(value);
            }
        
        
    }
    
    if (column_peg_287.chunk3 == 0){
        column_peg_287.chunk3 = new Chunk3();
    }
    column_peg_287.chunk3->chunk_range = result_peg_299;
    stream.update(result_peg_299.getPosition());
    
    
    return result_peg_299;
    out_peg_301:
    Result result_peg_310(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_310.getPosition()))){
                    result_peg_310.nextPosition();
                } else {
                    goto out_peg_312;
                }
            }
            result_peg_310.setValue((void*) "(");
        
        
        
        result_peg_310 = rule_s(stream, result_peg_310.getPosition());
            if (result_peg_310.error()){
                goto out_peg_312;
            }
        
        
        
        result_peg_310 = rule_expr_c(stream, result_peg_310.getPosition());
            if (result_peg_310.error()){
                goto out_peg_312;
            }
            low = result_peg_310.getValues();
        
        
        
        result_peg_310 = rule_s(stream, result_peg_310.getPosition());
            if (result_peg_310.error()){
                goto out_peg_312;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_310.getPosition()))){
                    result_peg_310.nextPosition();
                } else {
                    goto out_peg_312;
                }
            }
            result_peg_310.setValue((void*) ",");
        
        
        
        result_peg_310 = rule_s(stream, result_peg_310.getPosition());
            if (result_peg_310.error()){
                goto out_peg_312;
            }
        
        
        
        result_peg_310 = rule_expr_c(stream, result_peg_310.getPosition());
            if (result_peg_310.error()){
                goto out_peg_312;
            }
            high = result_peg_310.getValues();
        
        
        
        result_peg_310 = rule_s(stream, result_peg_310.getPosition());
            if (result_peg_310.error()){
                goto out_peg_312;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("]"[i], stream.get(result_peg_310.getPosition()))){
                    result_peg_310.nextPosition();
                } else {
                    goto out_peg_312;
                }
            }
            result_peg_310.setValue((void*) "]");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftExclusiveRightInclusive, low, high);
                result_peg_310.setValue(value);
            }
        
        
    }
    
    if (column_peg_287.chunk3 == 0){
        column_peg_287.chunk3 = new Chunk3();
    }
    column_peg_287.chunk3->chunk_range = result_peg_310;
    stream.update(result_peg_310.getPosition());
    
    
    return result_peg_310;
    out_peg_312:
    Result result_peg_321(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("["[i], stream.get(result_peg_321.getPosition()))){
                    result_peg_321.nextPosition();
                } else {
                    goto out_peg_323;
                }
            }
            result_peg_321.setValue((void*) "[");
        
        
        
        result_peg_321 = rule_s(stream, result_peg_321.getPosition());
            if (result_peg_321.error()){
                goto out_peg_323;
            }
        
        
        
        result_peg_321 = rule_expr_c(stream, result_peg_321.getPosition());
            if (result_peg_321.error()){
                goto out_peg_323;
            }
            low = result_peg_321.getValues();
        
        
        
        result_peg_321 = rule_s(stream, result_peg_321.getPosition());
            if (result_peg_321.error()){
                goto out_peg_323;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_321.getPosition()))){
                    result_peg_321.nextPosition();
                } else {
                    goto out_peg_323;
                }
            }
            result_peg_321.setValue((void*) ",");
        
        
        
        result_peg_321 = rule_s(stream, result_peg_321.getPosition());
            if (result_peg_321.error()){
                goto out_peg_323;
            }
        
        
        
        result_peg_321 = rule_expr_c(stream, result_peg_321.getPosition());
            if (result_peg_321.error()){
                goto out_peg_323;
            }
            high = result_peg_321.getValues();
        
        
        
        result_peg_321 = rule_s(stream, result_peg_321.getPosition());
            if (result_peg_321.error()){
                goto out_peg_323;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_321.getPosition()))){
                    result_peg_321.nextPosition();
                } else {
                    goto out_peg_323;
                }
            }
            result_peg_321.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeRange(Ast::Range::LeftInclusiveRightExclusive, low, high);
                result_peg_321.setValue(value);
            }
        
        
    }
    
    if (column_peg_287.chunk3 == 0){
        column_peg_287.chunk3 = new Chunk3();
    }
    column_peg_287.chunk3->chunk_range = result_peg_321;
    stream.update(result_peg_321.getPosition());
    
    
    return result_peg_321;
    out_peg_323:
    
    if (column_peg_287.chunk3 == 0){
        column_peg_287.chunk3 = new Chunk3();
    }
    column_peg_287.chunk3->chunk_range = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_name(Stream & stream, const int position){
    
    Column & column_peg_333 = stream.getColumn(position);
    if (column_peg_333.chunk3 != 0 && column_peg_333.chunk3->chunk_name.calculated()){
        return column_peg_333.chunk3->chunk_name;
    }
    
    RuleTrace trace_peg_345(stream, "name");
    int myposition = position;
    
    
    Result result_peg_334(myposition);
    
    {
    
        {
                
                char letter_peg_340 = stream.get(result_peg_334.getPosition());
                if (letter_peg_340 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_340) != NULL){
                    result_peg_334.nextPosition();
                    result_peg_334.setValue((void*) (long) letter_peg_340);
                } else {
                    goto out_peg_339;
                }
                
            }
            goto success_peg_336;
            out_peg_339:
            goto out_peg_341;
            success_peg_336:
            ;
        
        Result result_peg_335 = result_peg_334;
        
        result_peg_334.reset();
            do{
                Result result_peg_344(result_peg_334.getPosition());
                result_peg_344 = rule_alpha_digit(stream, result_peg_344.getPosition());
                if (result_peg_344.error()){
                    goto loop_peg_343;
                }
                result_peg_334.addResult(result_peg_344);
            } while (true);
            loop_peg_343:
            ;
        
        Result result_peg_342 = result_peg_334;
        
        {
                Value value((void*) 0);
                value = toString((char)(long)result_peg_335.getValues().getValue(),result_peg_342.getValues());
                result_peg_334.setValue(value);
            }
        
        
    }
    
    if (column_peg_333.chunk3 == 0){
        column_peg_333.chunk3 = new Chunk3();
    }
    column_peg_333.chunk3->chunk_name = result_peg_334;
    stream.update(result_peg_334.getPosition());
    
    
    return result_peg_334;
    out_peg_341:
    
    if (column_peg_333.chunk3 == 0){
        column_peg_333.chunk3 = new Chunk3();
    }
    column_peg_333.chunk3->chunk_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_alpha_digit(Stream & stream, const int position){
    
    Column & column_peg_346 = stream.getColumn(position);
    if (column_peg_346.chunk3 != 0 && column_peg_346.chunk3->chunk_alpha_digit.calculated()){
        return column_peg_346.chunk3->chunk_alpha_digit;
    }
    
    RuleTrace trace_peg_361(stream, "alpha_digit");
    int myposition = position;
    
    
    Result result_peg_347(myposition);
    
    {
        
        char letter_peg_352 = stream.get(result_peg_347.getPosition());
        if (letter_peg_352 != '\0' && strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_", letter_peg_352) != NULL){
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
    
    if (column_peg_346.chunk3 == 0){
        column_peg_346.chunk3 = new Chunk3();
    }
    column_peg_346.chunk3->chunk_alpha_digit = result_peg_347;
    stream.update(result_peg_347.getPosition());
    
    
    return result_peg_347;
    out_peg_353:
    Result result_peg_354(myposition);
    
    {
        
        char letter_peg_359 = stream.get(result_peg_354.getPosition());
        if (letter_peg_359 != '\0' && strchr("0123456789", letter_peg_359) != NULL){
            result_peg_354.nextPosition();
            result_peg_354.setValue((void*) (long) letter_peg_359);
        } else {
            goto out_peg_358;
        }
        
    }
    goto success_peg_355;
    out_peg_358:
    goto out_peg_360;
    success_peg_355:
    ;
    
    if (column_peg_346.chunk3 == 0){
        column_peg_346.chunk3 = new Chunk3();
    }
    column_peg_346.chunk3->chunk_alpha_digit = result_peg_354;
    stream.update(result_peg_354.getPosition());
    
    
    return result_peg_354;
    out_peg_360:
    
    if (column_peg_346.chunk3 == 0){
        column_peg_346.chunk3 = new Chunk3();
    }
    column_peg_346.chunk3->chunk_alpha_digit = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_valuelist(Stream & stream, const int position){
    
    Column & column_peg_362 = stream.getColumn(position);
    if (column_peg_362.chunk3 != 0 && column_peg_362.chunk3->chunk_valuelist.calculated()){
        return column_peg_362.chunk3->chunk_valuelist;
    }
    
    RuleTrace trace_peg_384(stream, "valuelist");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_363(myposition);
    
    {
    
        result_peg_363 = rule_expr_c(stream, result_peg_363.getPosition());
            if (result_peg_363.error()){
                goto out_peg_365;
            }
            first = result_peg_363.getValues();
        
        
        
        result_peg_363.reset();
            do{
                Result result_peg_368(result_peg_363.getPosition());
                {
                
                    result_peg_368 = rule_s(stream, result_peg_368.getPosition());
                        if (result_peg_368.error()){
                            goto loop_peg_367;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_368.getPosition()))){
                                result_peg_368.nextPosition();
                            } else {
                                goto loop_peg_367;
                            }
                        }
                        result_peg_368.setValue((void*) ",");
                    
                    
                    
                    result_peg_368 = rule_s(stream, result_peg_368.getPosition());
                        if (result_peg_368.error()){
                            goto loop_peg_367;
                        }
                    
                    
                    
                    result_peg_368 = rule_expr_c(stream, result_peg_368.getPosition());
                        if (result_peg_368.error()){
                            goto loop_peg_367;
                        }
                    
                    
                }
                result_peg_363.addResult(result_peg_368);
            } while (true);
            loop_peg_367:
            if (result_peg_363.matches() == 0){
                goto out_peg_365;
            }
            rest = result_peg_363.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_363.setValue(value);
            }
        
        
    }
    
    if (column_peg_362.chunk3 == 0){
        column_peg_362.chunk3 = new Chunk3();
    }
    column_peg_362.chunk3->chunk_valuelist = result_peg_363;
    stream.update(result_peg_363.getPosition());
    
    
    return result_peg_363;
    out_peg_365:
    Result result_peg_372(myposition);
    
    {
    
        result_peg_372 = rule_s(stream, result_peg_372.getPosition());
            if (result_peg_372.error()){
                goto out_peg_374;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_372.getPosition()))){
                    result_peg_372.nextPosition();
                } else {
                    goto out_peg_374;
                }
            }
            result_peg_372.setValue((void*) ",");
        
        
        
        result_peg_372 = rule_s(stream, result_peg_372.getPosition());
            if (result_peg_372.error()){
                goto out_peg_374;
            }
        
        
        
        result_peg_372 = rule_expr_c(stream, result_peg_372.getPosition());
            if (result_peg_372.error()){
                goto out_peg_374;
            }
            first = result_peg_372.getValues();
        
        
        
        result_peg_372.reset();
            do{
                Result result_peg_380(result_peg_372.getPosition());
                {
                
                    result_peg_380 = rule_s(stream, result_peg_380.getPosition());
                        if (result_peg_380.error()){
                            goto loop_peg_379;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_380.getPosition()))){
                                result_peg_380.nextPosition();
                            } else {
                                goto loop_peg_379;
                            }
                        }
                        result_peg_380.setValue((void*) ",");
                    
                    
                    
                    result_peg_380 = rule_s(stream, result_peg_380.getPosition());
                        if (result_peg_380.error()){
                            goto loop_peg_379;
                        }
                    
                    
                    
                    result_peg_380 = rule_expr_c(stream, result_peg_380.getPosition());
                        if (result_peg_380.error()){
                            goto loop_peg_379;
                        }
                    
                    
                }
                result_peg_372.addResult(result_peg_380);
            } while (true);
            loop_peg_379:
            ;
            rest = result_peg_372.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(first, rest);
                result_peg_372.setValue(value);
            }
        
        
    }
    
    if (column_peg_362.chunk3 == 0){
        column_peg_362.chunk3 = new Chunk3();
    }
    column_peg_362.chunk3->chunk_valuelist = result_peg_372;
    stream.update(result_peg_372.getPosition());
    
    
    return result_peg_372;
    out_peg_374:
    
    if (column_peg_362.chunk3 == 0){
        column_peg_362.chunk3 = new Chunk3();
    }
    column_peg_362.chunk3->chunk_valuelist = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr(Stream & stream, const int position){
    
    Column & column_peg_385 = stream.getColumn(position);
    if (column_peg_385.chunk3 != 0 && column_peg_385.chunk3->chunk_expr.calculated()){
        return column_peg_385.chunk3->chunk_expr;
    }
    
    RuleTrace trace_peg_396(stream, "expr");
    int myposition = position;
    
    
    Result result_peg_386(myposition);
    
    {
    
        result_peg_386 = rule_expr_c(stream, result_peg_386.getPosition());
            if (result_peg_386.error()){
                goto out_peg_388;
            }
        
        Result result_peg_387 = result_peg_386;
        
        Result result_peg_391(result_peg_386);
            {
            
                result_peg_391 = rule_s(stream, result_peg_391.getPosition());
                    if (result_peg_391.error()){
                        goto not_peg_390;
                    }
                
                
                
                for (int i = 0; i < 1; i++){
                        if (compareChar(","[i], stream.get(result_peg_391.getPosition()))){
                            result_peg_391.nextPosition();
                        } else {
                            goto not_peg_390;
                        }
                    }
                    result_peg_391.setValue((void*) ",");
                
                
            }
            goto out_peg_388;
            not_peg_390:
            result_peg_386.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_387.getValues();
                result_peg_386.setValue(value);
            }
        
        
    }
    
    if (column_peg_385.chunk3 == 0){
        column_peg_385.chunk3 = new Chunk3();
    }
    column_peg_385.chunk3->chunk_expr = result_peg_386;
    stream.update(result_peg_386.getPosition());
    
    
    return result_peg_386;
    out_peg_388:
    Result result_peg_393(myposition);
    
    {
    
        result_peg_393 = rule_valuelist(stream, result_peg_393.getPosition());
            if (result_peg_393.error()){
                goto out_peg_395;
            }
        
        Result result_peg_394 = result_peg_393;
        
        result_peg_393 = rule_expr2_rest(stream, result_peg_393.getPosition(), result_peg_394.getValues());
            if (result_peg_393.error()){
                goto out_peg_395;
            }
        
        
    }
    
    if (column_peg_385.chunk3 == 0){
        column_peg_385.chunk3 = new Chunk3();
    }
    column_peg_385.chunk3->chunk_expr = result_peg_393;
    stream.update(result_peg_393.getPosition());
    
    
    return result_peg_393;
    out_peg_395:
    
    if (column_peg_385.chunk3 == 0){
        column_peg_385.chunk3 = new Chunk3();
    }
    column_peg_385.chunk3->chunk_expr = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_c(Stream & stream, const int position){
    
    Column & column_peg_397 = stream.getColumn(position);
    if (column_peg_397.chunk4 != 0 && column_peg_397.chunk4->chunk_expr_c.calculated()){
        return column_peg_397.chunk4->chunk_expr_c;
    }
    
    RuleTrace trace_peg_401(stream, "expr_c");
    int myposition = position;
    
    Value left;
    Result result_peg_398(myposition);
    
    {
    
        result_peg_398 = rule_expr2(stream, result_peg_398.getPosition());
            if (result_peg_398.error()){
                goto out_peg_400;
            }
            left = result_peg_398.getValues();
        
        
        
        result_peg_398 = rule_expr_rest(stream, result_peg_398.getPosition(), left);
            if (result_peg_398.error()){
                goto out_peg_400;
            }
        
        
    }
    
    if (column_peg_397.chunk4 == 0){
        column_peg_397.chunk4 = new Chunk4();
    }
    column_peg_397.chunk4->chunk_expr_c = result_peg_398;
    stream.update(result_peg_398.getPosition());
    
    
    return result_peg_398;
    out_peg_400:
    
    if (column_peg_397.chunk4 == 0){
        column_peg_397.chunk4 = new Chunk4();
    }
    column_peg_397.chunk4->chunk_expr_c = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr2(Stream & stream, const int position){
    
    Column & column_peg_402 = stream.getColumn(position);
    if (column_peg_402.chunk4 != 0 && column_peg_402.chunk4->chunk_expr2.calculated()){
        return column_peg_402.chunk4->chunk_expr2;
    }
    
    RuleTrace trace_peg_406(stream, "expr2");
    int myposition = position;
    
    Value left;
    Result result_peg_403(myposition);
    
    {
    
        result_peg_403 = rule_expr3(stream, result_peg_403.getPosition());
            if (result_peg_403.error()){
                goto out_peg_405;
            }
            left = result_peg_403.getValues();
        
        
        
        result_peg_403 = rule_expr2_rest(stream, result_peg_403.getPosition(), left);
            if (result_peg_403.error()){
                goto out_peg_405;
            }
        
        
    }
    
    if (column_peg_402.chunk4 == 0){
        column_peg_402.chunk4 = new Chunk4();
    }
    column_peg_402.chunk4->chunk_expr2 = result_peg_403;
    stream.update(result_peg_403.getPosition());
    
    
    return result_peg_403;
    out_peg_405:
    
    if (column_peg_402.chunk4 == 0){
        column_peg_402.chunk4 = new Chunk4();
    }
    column_peg_402.chunk4->chunk_expr2 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr3(Stream & stream, const int position){
    
    Column & column_peg_407 = stream.getColumn(position);
    if (column_peg_407.chunk4 != 0 && column_peg_407.chunk4->chunk_expr3.calculated()){
        return column_peg_407.chunk4->chunk_expr3;
    }
    
    RuleTrace trace_peg_411(stream, "expr3");
    int myposition = position;
    
    Value left;
    Result result_peg_408(myposition);
    
    {
    
        result_peg_408 = rule_expr4(stream, result_peg_408.getPosition());
            if (result_peg_408.error()){
                goto out_peg_410;
            }
            left = result_peg_408.getValues();
        
        
        
        result_peg_408 = rule_expr3_rest(stream, result_peg_408.getPosition(), left);
            if (result_peg_408.error()){
                goto out_peg_410;
            }
        
        
    }
    
    if (column_peg_407.chunk4 == 0){
        column_peg_407.chunk4 = new Chunk4();
    }
    column_peg_407.chunk4->chunk_expr3 = result_peg_408;
    stream.update(result_peg_408.getPosition());
    
    
    return result_peg_408;
    out_peg_410:
    
    if (column_peg_407.chunk4 == 0){
        column_peg_407.chunk4 = new Chunk4();
    }
    column_peg_407.chunk4->chunk_expr3 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr4(Stream & stream, const int position){
    
    Column & column_peg_412 = stream.getColumn(position);
    if (column_peg_412.chunk4 != 0 && column_peg_412.chunk4->chunk_expr4.calculated()){
        return column_peg_412.chunk4->chunk_expr4;
    }
    
    RuleTrace trace_peg_416(stream, "expr4");
    int myposition = position;
    
    Value left;
    Result result_peg_413(myposition);
    
    {
    
        result_peg_413 = rule_expr5(stream, result_peg_413.getPosition());
            if (result_peg_413.error()){
                goto out_peg_415;
            }
            left = result_peg_413.getValues();
        
        
        
        result_peg_413 = rule_expr4_rest(stream, result_peg_413.getPosition(), left);
            if (result_peg_413.error()){
                goto out_peg_415;
            }
        
        
    }
    
    if (column_peg_412.chunk4 == 0){
        column_peg_412.chunk4 = new Chunk4();
    }
    column_peg_412.chunk4->chunk_expr4 = result_peg_413;
    stream.update(result_peg_413.getPosition());
    
    
    return result_peg_413;
    out_peg_415:
    
    if (column_peg_412.chunk4 == 0){
        column_peg_412.chunk4 = new Chunk4();
    }
    column_peg_412.chunk4->chunk_expr4 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr5(Stream & stream, const int position){
    
    Column & column_peg_417 = stream.getColumn(position);
    if (column_peg_417.chunk4 != 0 && column_peg_417.chunk4->chunk_expr5.calculated()){
        return column_peg_417.chunk4->chunk_expr5;
    }
    
    RuleTrace trace_peg_421(stream, "expr5");
    int myposition = position;
    
    Value left;
    Result result_peg_418(myposition);
    
    {
    
        result_peg_418 = rule_expr6(stream, result_peg_418.getPosition());
            if (result_peg_418.error()){
                goto out_peg_420;
            }
            left = result_peg_418.getValues();
        
        
        
        result_peg_418 = rule_expr5_rest(stream, result_peg_418.getPosition(), left);
            if (result_peg_418.error()){
                goto out_peg_420;
            }
        
        
    }
    
    if (column_peg_417.chunk4 == 0){
        column_peg_417.chunk4 = new Chunk4();
    }
    column_peg_417.chunk4->chunk_expr5 = result_peg_418;
    stream.update(result_peg_418.getPosition());
    
    
    return result_peg_418;
    out_peg_420:
    
    if (column_peg_417.chunk4 == 0){
        column_peg_417.chunk4 = new Chunk4();
    }
    column_peg_417.chunk4->chunk_expr5 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr6(Stream & stream, const int position){
    
    Column & column_peg_422 = stream.getColumn(position);
    if (column_peg_422.chunk5 != 0 && column_peg_422.chunk5->chunk_expr6.calculated()){
        return column_peg_422.chunk5->chunk_expr6;
    }
    
    RuleTrace trace_peg_426(stream, "expr6");
    int myposition = position;
    
    Value left;
    Result result_peg_423(myposition);
    
    {
    
        result_peg_423 = rule_expr7(stream, result_peg_423.getPosition());
            if (result_peg_423.error()){
                goto out_peg_425;
            }
            left = result_peg_423.getValues();
        
        
        
        result_peg_423 = rule_expr6_rest(stream, result_peg_423.getPosition(), left);
            if (result_peg_423.error()){
                goto out_peg_425;
            }
        
        
    }
    
    if (column_peg_422.chunk5 == 0){
        column_peg_422.chunk5 = new Chunk5();
    }
    column_peg_422.chunk5->chunk_expr6 = result_peg_423;
    stream.update(result_peg_423.getPosition());
    
    
    return result_peg_423;
    out_peg_425:
    
    if (column_peg_422.chunk5 == 0){
        column_peg_422.chunk5 = new Chunk5();
    }
    column_peg_422.chunk5->chunk_expr6 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr7(Stream & stream, const int position){
    
    Column & column_peg_427 = stream.getColumn(position);
    if (column_peg_427.chunk5 != 0 && column_peg_427.chunk5->chunk_expr7.calculated()){
        return column_peg_427.chunk5->chunk_expr7;
    }
    
    RuleTrace trace_peg_431(stream, "expr7");
    int myposition = position;
    
    Value left;
    Result result_peg_428(myposition);
    
    {
    
        result_peg_428 = rule_expr8(stream, result_peg_428.getPosition());
            if (result_peg_428.error()){
                goto out_peg_430;
            }
            left = result_peg_428.getValues();
        
        
        
        result_peg_428 = rule_expr7_rest(stream, result_peg_428.getPosition(), left);
            if (result_peg_428.error()){
                goto out_peg_430;
            }
        
        
    }
    
    if (column_peg_427.chunk5 == 0){
        column_peg_427.chunk5 = new Chunk5();
    }
    column_peg_427.chunk5->chunk_expr7 = result_peg_428;
    stream.update(result_peg_428.getPosition());
    
    
    return result_peg_428;
    out_peg_430:
    
    if (column_peg_427.chunk5 == 0){
        column_peg_427.chunk5 = new Chunk5();
    }
    column_peg_427.chunk5->chunk_expr7 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr8(Stream & stream, const int position){
    
    Column & column_peg_432 = stream.getColumn(position);
    if (column_peg_432.chunk5 != 0 && column_peg_432.chunk5->chunk_expr8.calculated()){
        return column_peg_432.chunk5->chunk_expr8;
    }
    
    RuleTrace trace_peg_436(stream, "expr8");
    int myposition = position;
    
    Value left;
    Result result_peg_433(myposition);
    
    {
    
        result_peg_433 = rule_expr9(stream, result_peg_433.getPosition());
            if (result_peg_433.error()){
                goto out_peg_435;
            }
            left = result_peg_433.getValues();
        
        
        
        result_peg_433 = rule_expr8_rest(stream, result_peg_433.getPosition(), left);
            if (result_peg_433.error()){
                goto out_peg_435;
            }
        
        
    }
    
    if (column_peg_432.chunk5 == 0){
        column_peg_432.chunk5 = new Chunk5();
    }
    column_peg_432.chunk5->chunk_expr8 = result_peg_433;
    stream.update(result_peg_433.getPosition());
    
    
    return result_peg_433;
    out_peg_435:
    
    if (column_peg_432.chunk5 == 0){
        column_peg_432.chunk5 = new Chunk5();
    }
    column_peg_432.chunk5->chunk_expr8 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr9(Stream & stream, const int position){
    
    Column & column_peg_437 = stream.getColumn(position);
    if (column_peg_437.chunk5 != 0 && column_peg_437.chunk5->chunk_expr9.calculated()){
        return column_peg_437.chunk5->chunk_expr9;
    }
    
    RuleTrace trace_peg_441(stream, "expr9");
    int myposition = position;
    
    Value left;
    Result result_peg_438(myposition);
    
    {
    
        result_peg_438 = rule_expr10(stream, result_peg_438.getPosition());
            if (result_peg_438.error()){
                goto out_peg_440;
            }
            left = result_peg_438.getValues();
        
        
        
        result_peg_438 = rule_expr9_rest(stream, result_peg_438.getPosition(), left);
            if (result_peg_438.error()){
                goto out_peg_440;
            }
        
        
    }
    
    if (column_peg_437.chunk5 == 0){
        column_peg_437.chunk5 = new Chunk5();
    }
    column_peg_437.chunk5->chunk_expr9 = result_peg_438;
    stream.update(result_peg_438.getPosition());
    
    
    return result_peg_438;
    out_peg_440:
    
    if (column_peg_437.chunk5 == 0){
        column_peg_437.chunk5 = new Chunk5();
    }
    column_peg_437.chunk5->chunk_expr9 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10(Stream & stream, const int position){
    
    Column & column_peg_442 = stream.getColumn(position);
    if (column_peg_442.chunk5 != 0 && column_peg_442.chunk5->chunk_expr10.calculated()){
        return column_peg_442.chunk5->chunk_expr10;
    }
    
    RuleTrace trace_peg_446(stream, "expr10");
    int myposition = position;
    
    Value left;
    Result result_peg_443(myposition);
    
    {
    
        result_peg_443 = rule_expr11(stream, result_peg_443.getPosition());
            if (result_peg_443.error()){
                goto out_peg_445;
            }
            left = result_peg_443.getValues();
        
        
        
        result_peg_443 = rule_expr10_rest(stream, result_peg_443.getPosition(), left);
            if (result_peg_443.error()){
                goto out_peg_445;
            }
        
        
    }
    
    if (column_peg_442.chunk5 == 0){
        column_peg_442.chunk5 = new Chunk5();
    }
    column_peg_442.chunk5->chunk_expr10 = result_peg_443;
    stream.update(result_peg_443.getPosition());
    
    
    return result_peg_443;
    out_peg_445:
    
    if (column_peg_442.chunk5 == 0){
        column_peg_442.chunk5 = new Chunk5();
    }
    column_peg_442.chunk5->chunk_expr10 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr11(Stream & stream, const int position){
    
    Column & column_peg_447 = stream.getColumn(position);
    if (column_peg_447.chunk6 != 0 && column_peg_447.chunk6->chunk_expr11.calculated()){
        return column_peg_447.chunk6->chunk_expr11;
    }
    
    RuleTrace trace_peg_451(stream, "expr11");
    int myposition = position;
    
    Value left;
    Result result_peg_448(myposition);
    
    {
    
        result_peg_448 = rule_expr12(stream, result_peg_448.getPosition());
            if (result_peg_448.error()){
                goto out_peg_450;
            }
            left = result_peg_448.getValues();
        
        
        
        result_peg_448 = rule_expr11_rest(stream, result_peg_448.getPosition(), left);
            if (result_peg_448.error()){
                goto out_peg_450;
            }
        
        
    }
    
    if (column_peg_447.chunk6 == 0){
        column_peg_447.chunk6 = new Chunk6();
    }
    column_peg_447.chunk6->chunk_expr11 = result_peg_448;
    stream.update(result_peg_448.getPosition());
    
    
    return result_peg_448;
    out_peg_450:
    
    if (column_peg_447.chunk6 == 0){
        column_peg_447.chunk6 = new Chunk6();
    }
    column_peg_447.chunk6->chunk_expr11 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr12(Stream & stream, const int position){
    
    Column & column_peg_452 = stream.getColumn(position);
    if (column_peg_452.chunk6 != 0 && column_peg_452.chunk6->chunk_expr12.calculated()){
        return column_peg_452.chunk6->chunk_expr12;
    }
    
    RuleTrace trace_peg_456(stream, "expr12");
    int myposition = position;
    
    Value left;
    Result result_peg_453(myposition);
    
    {
    
        result_peg_453 = rule_expr13(stream, result_peg_453.getPosition());
            if (result_peg_453.error()){
                goto out_peg_455;
            }
            left = result_peg_453.getValues();
        
        
        
        result_peg_453 = rule_expr12_rest(stream, result_peg_453.getPosition(), left);
            if (result_peg_453.error()){
                goto out_peg_455;
            }
        
        
    }
    
    if (column_peg_452.chunk6 == 0){
        column_peg_452.chunk6 = new Chunk6();
    }
    column_peg_452.chunk6->chunk_expr12 = result_peg_453;
    stream.update(result_peg_453.getPosition());
    
    
    return result_peg_453;
    out_peg_455:
    
    if (column_peg_452.chunk6 == 0){
        column_peg_452.chunk6 = new Chunk6();
    }
    column_peg_452.chunk6->chunk_expr12 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13(Stream & stream, const int position){
    
    Column & column_peg_457 = stream.getColumn(position);
    if (column_peg_457.chunk6 != 0 && column_peg_457.chunk6->chunk_expr13.calculated()){
        return column_peg_457.chunk6->chunk_expr13;
    }
    
    RuleTrace trace_peg_464(stream, "expr13");
    int myposition = position;
    
    Value exp;
    Result result_peg_458(myposition);
    
    {
    
        result_peg_458.reset();
            do{
                Result result_peg_461(result_peg_458.getPosition());
                result_peg_461 = rule_unary(stream, result_peg_461.getPosition());
                if (result_peg_461.error()){
                    goto loop_peg_460;
                }
                result_peg_458.addResult(result_peg_461);
            } while (true);
            loop_peg_460:
            ;
        
        Result result_peg_459 = result_peg_458;
        
        result_peg_458 = rule_expr13_real(stream, result_peg_458.getPosition());
            if (result_peg_458.error()){
                goto out_peg_463;
            }
            exp = result_peg_458.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeUnaryExpression(result_peg_459.getValues(), exp);
                result_peg_458.setValue(value);
            }
        
        
    }
    
    if (column_peg_457.chunk6 == 0){
        column_peg_457.chunk6 = new Chunk6();
    }
    column_peg_457.chunk6->chunk_expr13 = result_peg_458;
    stream.update(result_peg_458.getPosition());
    
    
    return result_peg_458;
    out_peg_463:
    
    if (column_peg_457.chunk6 == 0){
        column_peg_457.chunk6 = new Chunk6();
    }
    column_peg_457.chunk6->chunk_expr13 = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_476(stream, "expr_rest");
    int myposition = position;
    tail_peg_467:
    Value right;
    Value new_left;
    Result result_peg_466(myposition);
    {
    
        result_peg_466 = rule_s(stream, result_peg_466.getPosition());
            if (result_peg_466.error()){
                goto out_peg_469;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("||"[i], stream.get(result_peg_466.getPosition()))){
                    result_peg_466.nextPosition();
                } else {
                    goto out_peg_469;
                }
            }
            result_peg_466.setValue((void*) "||");
        
        
        
        result_peg_466 = rule_s(stream, result_peg_466.getPosition());
            if (result_peg_466.error()){
                goto out_peg_469;
            }
        
        
        
        result_peg_466 = rule_expr2(stream, result_peg_466.getPosition());
            if (result_peg_466.error()){
                goto out_peg_469;
            }
            right = result_peg_466.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionOr(left, right);
                result_peg_466.setValue(value);
            }
            new_left = result_peg_466.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_466.getPosition();
    goto tail_peg_467;
    out_peg_469:
    Result result_peg_474(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_474.setValue(value);
            }
        
        
    }
    stream.update(result_peg_474.getPosition());
    
    return result_peg_474;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr2_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_488(stream, "expr2_rest");
    int myposition = position;
    tail_peg_479:
    Value right;
    Value new_left;
    Result result_peg_478(myposition);
    {
    
        result_peg_478 = rule_s(stream, result_peg_478.getPosition());
            if (result_peg_478.error()){
                goto out_peg_481;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("^^"[i], stream.get(result_peg_478.getPosition()))){
                    result_peg_478.nextPosition();
                } else {
                    goto out_peg_481;
                }
            }
            result_peg_478.setValue((void*) "^^");
        
        
        
        result_peg_478 = rule_s(stream, result_peg_478.getPosition());
            if (result_peg_478.error()){
                goto out_peg_481;
            }
        
        
        
        result_peg_478 = rule_expr3(stream, result_peg_478.getPosition());
            if (result_peg_478.error()){
                goto out_peg_481;
            }
            right = result_peg_478.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionXOr(left, right);
                result_peg_478.setValue(value);
            }
            new_left = result_peg_478.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_478.getPosition();
    goto tail_peg_479;
    out_peg_481:
    Result result_peg_486(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_486.setValue(value);
            }
        
        
    }
    stream.update(result_peg_486.getPosition());
    
    return result_peg_486;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr3_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_500(stream, "expr3_rest");
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
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("&&"[i], stream.get(result_peg_490.getPosition()))){
                    result_peg_490.nextPosition();
                } else {
                    goto out_peg_493;
                }
            }
            result_peg_490.setValue((void*) "&&");
        
        
        
        result_peg_490 = rule_s(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_493;
            }
        
        
        
        result_peg_490 = rule_expr4(stream, result_peg_490.getPosition());
            if (result_peg_490.error()){
                goto out_peg_493;
            }
            right = result_peg_490.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAnd(left, right);
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
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_498.setValue(value);
            }
        
        
    }
    stream.update(result_peg_498.getPosition());
    
    return result_peg_498;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr4_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_512(stream, "expr4_rest");
    int myposition = position;
    tail_peg_503:
    Value right;
    Value new_left;
    Result result_peg_502(myposition);
    {
    
        result_peg_502 = rule_s(stream, result_peg_502.getPosition());
            if (result_peg_502.error()){
                goto out_peg_505;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("|"[i], stream.get(result_peg_502.getPosition()))){
                    result_peg_502.nextPosition();
                } else {
                    goto out_peg_505;
                }
            }
            result_peg_502.setValue((void*) "|");
        
        
        
        result_peg_502 = rule_s(stream, result_peg_502.getPosition());
            if (result_peg_502.error()){
                goto out_peg_505;
            }
        
        
        
        result_peg_502 = rule_expr5(stream, result_peg_502.getPosition());
            if (result_peg_502.error()){
                goto out_peg_505;
            }
            right = result_peg_502.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseOr(left, right);
                result_peg_502.setValue(value);
            }
            new_left = result_peg_502.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_502.getPosition();
    goto tail_peg_503;
    out_peg_505:
    Result result_peg_510(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_510.setValue(value);
            }
        
        
    }
    stream.update(result_peg_510.getPosition());
    
    return result_peg_510;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr5_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_524(stream, "expr5_rest");
    int myposition = position;
    tail_peg_515:
    Value right;
    Value new_left;
    Result result_peg_514(myposition);
    {
    
        result_peg_514 = rule_s(stream, result_peg_514.getPosition());
            if (result_peg_514.error()){
                goto out_peg_517;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("^"[i], stream.get(result_peg_514.getPosition()))){
                    result_peg_514.nextPosition();
                } else {
                    goto out_peg_517;
                }
            }
            result_peg_514.setValue((void*) "^");
        
        
        
        result_peg_514 = rule_s(stream, result_peg_514.getPosition());
            if (result_peg_514.error()){
                goto out_peg_517;
            }
        
        
        
        result_peg_514 = rule_expr6(stream, result_peg_514.getPosition());
            if (result_peg_514.error()){
                goto out_peg_517;
            }
            right = result_peg_514.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseXOr(left, right);
                result_peg_514.setValue(value);
            }
            new_left = result_peg_514.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_514.getPosition();
    goto tail_peg_515;
    out_peg_517:
    Result result_peg_522(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_522.setValue(value);
            }
        
        
    }
    stream.update(result_peg_522.getPosition());
    
    return result_peg_522;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr6_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_536(stream, "expr6_rest");
    int myposition = position;
    tail_peg_527:
    Value right;
    Value new_left;
    Result result_peg_526(myposition);
    {
    
        result_peg_526 = rule_s(stream, result_peg_526.getPosition());
            if (result_peg_526.error()){
                goto out_peg_529;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("&"[i], stream.get(result_peg_526.getPosition()))){
                    result_peg_526.nextPosition();
                } else {
                    goto out_peg_529;
                }
            }
            result_peg_526.setValue((void*) "&");
        
        
        
        result_peg_526 = rule_s(stream, result_peg_526.getPosition());
            if (result_peg_526.error()){
                goto out_peg_529;
            }
        
        
        
        result_peg_526 = rule_expr7(stream, result_peg_526.getPosition());
            if (result_peg_526.error()){
                goto out_peg_529;
            }
            right = result_peg_526.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionBitwiseAnd(left, right);
                result_peg_526.setValue(value);
            }
            new_left = result_peg_526.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_526.getPosition();
    goto tail_peg_527;
    out_peg_529:
    Result result_peg_534(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_534.setValue(value);
            }
        
        
    }
    stream.update(result_peg_534.getPosition());
    
    return result_peg_534;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr7_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_548(stream, "expr7_rest");
    int myposition = position;
    tail_peg_539:
    Value right;
    Value new_left;
    Result result_peg_538(myposition);
    {
    
        result_peg_538 = rule_s(stream, result_peg_538.getPosition());
            if (result_peg_538.error()){
                goto out_peg_541;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar(":="[i], stream.get(result_peg_538.getPosition()))){
                    result_peg_538.nextPosition();
                } else {
                    goto out_peg_541;
                }
            }
            result_peg_538.setValue((void*) ":=");
        
        
        
        result_peg_538 = rule_s(stream, result_peg_538.getPosition());
            if (result_peg_538.error()){
                goto out_peg_541;
            }
        
        
        
        result_peg_538 = rule_expr8(stream, result_peg_538.getPosition());
            if (result_peg_538.error()){
                goto out_peg_541;
            }
            right = result_peg_538.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAssignment(left, right);
                result_peg_538.setValue(value);
            }
            new_left = result_peg_538.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_538.getPosition();
    goto tail_peg_539;
    out_peg_541:
    Result result_peg_546(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_546.setValue(value);
            }
        
        
    }
    stream.update(result_peg_546.getPosition());
    
    return result_peg_546;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr8_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_567(stream, "expr8_rest");
    int myposition = position;
    tail_peg_551:
    Value right;
    Value new_left;
    Result result_peg_550(myposition);
    {
    
        result_peg_550 = rule_s(stream, result_peg_550.getPosition());
            if (result_peg_550.error()){
                goto out_peg_553;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_550.getPosition()))){
                    result_peg_550.nextPosition();
                } else {
                    goto out_peg_553;
                }
            }
            result_peg_550.setValue((void*) "=");
        
        
        
        result_peg_550 = rule_s(stream, result_peg_550.getPosition());
            if (result_peg_550.error()){
                goto out_peg_553;
            }
        
        
        
        result_peg_550 = rule_expr9(stream, result_peg_550.getPosition());
            if (result_peg_550.error()){
                goto out_peg_553;
            }
            right = result_peg_550.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionEquals(left, right);
                result_peg_550.setValue(value);
            }
            new_left = result_peg_550.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_550.getPosition();
    goto tail_peg_551;
    out_peg_553:
    Result result_peg_558(myposition);
    {
    
        result_peg_558 = rule_s(stream, result_peg_558.getPosition());
            if (result_peg_558.error()){
                goto out_peg_560;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("!="[i], stream.get(result_peg_558.getPosition()))){
                    result_peg_558.nextPosition();
                } else {
                    goto out_peg_560;
                }
            }
            result_peg_558.setValue((void*) "!=");
        
        
        
        result_peg_558 = rule_s(stream, result_peg_558.getPosition());
            if (result_peg_558.error()){
                goto out_peg_560;
            }
        
        
        
        result_peg_558 = rule_expr9(stream, result_peg_558.getPosition());
            if (result_peg_558.error()){
                goto out_peg_560;
            }
            right = result_peg_558.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionUnequals(left, right);
                result_peg_558.setValue(value);
            }
            new_left = result_peg_558.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_558.getPosition();
    goto tail_peg_551;
    out_peg_560:
    Result result_peg_565(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_565.setValue(value);
            }
        
        
    }
    stream.update(result_peg_565.getPosition());
    
    return result_peg_565;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr9_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_579(stream, "expr9_rest");
    int myposition = position;
    tail_peg_570:
    Value maker;
    Value right;
    Value new_left;
    Result result_peg_569(myposition);
    {
    
        result_peg_569 = rule_s(stream, result_peg_569.getPosition());
            if (result_peg_569.error()){
                goto out_peg_572;
            }
        
        
        
        result_peg_569 = rule_compare(stream, result_peg_569.getPosition());
            if (result_peg_569.error()){
                goto out_peg_572;
            }
            maker = result_peg_569.getValues();
        
        
        
        result_peg_569 = rule_s(stream, result_peg_569.getPosition());
            if (result_peg_569.error()){
                goto out_peg_572;
            }
        
        
        
        result_peg_569 = rule_expr10(stream, result_peg_569.getPosition());
            if (result_peg_569.error()){
                goto out_peg_572;
            }
            right = result_peg_569.getValues();
        
        
        
        {
                Value value((void*) 0);
                {
                        typedef Ast::Value * (*compare_func)(const Value &, const Value &);
                        value = as<compare_func>(maker)(left, right);
                        }
                result_peg_569.setValue(value);
            }
            new_left = result_peg_569.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_569.getPosition();
    goto tail_peg_570;
    out_peg_572:
    Result result_peg_577(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_577.setValue(value);
            }
        
        
    }
    stream.update(result_peg_577.getPosition());
    
    return result_peg_577;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_compare(Stream & stream, const int position){
    
    Column & column_peg_580 = stream.getColumn(position);
    if (column_peg_580.chunk8 != 0 && column_peg_580.chunk8->chunk_compare.calculated()){
        return column_peg_580.chunk8->chunk_compare;
    }
    
    RuleTrace trace_peg_593(stream, "compare");
    int myposition = position;
    
    
    Result result_peg_581(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_581.getPosition()))){
                    result_peg_581.nextPosition();
                } else {
                    goto out_peg_583;
                }
            }
            result_peg_581.setValue((void*) "<=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThanEquals;
                result_peg_581.setValue(value);
            }
        
        
    }
    
    if (column_peg_580.chunk8 == 0){
        column_peg_580.chunk8 = new Chunk8();
    }
    column_peg_580.chunk8->chunk_compare = result_peg_581;
    stream.update(result_peg_581.getPosition());
    
    
    return result_peg_581;
    out_peg_583:
    Result result_peg_584(myposition);
    
    {
    
        for (int i = 0; i < 2; i++){
                if (compareChar(">="[i], stream.get(result_peg_584.getPosition()))){
                    result_peg_584.nextPosition();
                } else {
                    goto out_peg_586;
                }
            }
            result_peg_584.setValue((void*) ">=");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThanEquals;
                result_peg_584.setValue(value);
            }
        
        
    }
    
    if (column_peg_580.chunk8 == 0){
        column_peg_580.chunk8 = new Chunk8();
    }
    column_peg_580.chunk8->chunk_compare = result_peg_584;
    stream.update(result_peg_584.getPosition());
    
    
    return result_peg_584;
    out_peg_586:
    Result result_peg_587(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("<"[i], stream.get(result_peg_587.getPosition()))){
                    result_peg_587.nextPosition();
                } else {
                    goto out_peg_589;
                }
            }
            result_peg_587.setValue((void*) "<");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionLessThan;
                result_peg_587.setValue(value);
            }
        
        
    }
    
    if (column_peg_580.chunk8 == 0){
        column_peg_580.chunk8 = new Chunk8();
    }
    column_peg_580.chunk8->chunk_compare = result_peg_587;
    stream.update(result_peg_587.getPosition());
    
    
    return result_peg_587;
    out_peg_589:
    Result result_peg_590(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_590.getPosition()))){
                    result_peg_590.nextPosition();
                } else {
                    goto out_peg_592;
                }
            }
            result_peg_590.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = (void *) makeExpressionGreaterThan;
                result_peg_590.setValue(value);
            }
        
        
    }
    
    if (column_peg_580.chunk8 == 0){
        column_peg_580.chunk8 = new Chunk8();
    }
    column_peg_580.chunk8->chunk_compare = result_peg_590;
    stream.update(result_peg_590.getPosition());
    
    
    return result_peg_590;
    out_peg_592:
    
    if (column_peg_580.chunk8 == 0){
        column_peg_580.chunk8 = new Chunk8();
    }
    column_peg_580.chunk8->chunk_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_all_compare(Stream & stream, const int position){
    
    Column & column_peg_594 = stream.getColumn(position);
    if (column_peg_594.chunk8 != 0 && column_peg_594.chunk8->chunk_all_compare.calculated()){
        return column_peg_594.chunk8->chunk_all_compare;
    }
    
    RuleTrace trace_peg_601(stream, "all_compare");
    int myposition = position;
    
    
    Result result_peg_595(myposition);
    
    result_peg_595 = rule_compare(stream, result_peg_595.getPosition());
    if (result_peg_595.error()){
        goto out_peg_596;
    }
    
    if (column_peg_594.chunk8 == 0){
        column_peg_594.chunk8 = new Chunk8();
    }
    column_peg_594.chunk8->chunk_all_compare = result_peg_595;
    stream.update(result_peg_595.getPosition());
    
    
    return result_peg_595;
    out_peg_596:
    Result result_peg_597(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("="[i], stream.get(result_peg_597.getPosition()))){
            result_peg_597.nextPosition();
        } else {
            goto out_peg_598;
        }
    }
    result_peg_597.setValue((void*) "=");
    
    if (column_peg_594.chunk8 == 0){
        column_peg_594.chunk8 = new Chunk8();
    }
    column_peg_594.chunk8->chunk_all_compare = result_peg_597;
    stream.update(result_peg_597.getPosition());
    
    
    return result_peg_597;
    out_peg_598:
    Result result_peg_599(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("!="[i], stream.get(result_peg_599.getPosition()))){
            result_peg_599.nextPosition();
        } else {
            goto out_peg_600;
        }
    }
    result_peg_599.setValue((void*) "!=");
    
    if (column_peg_594.chunk8 == 0){
        column_peg_594.chunk8 = new Chunk8();
    }
    column_peg_594.chunk8->chunk_all_compare = result_peg_599;
    stream.update(result_peg_599.getPosition());
    
    
    return result_peg_599;
    out_peg_600:
    
    if (column_peg_594.chunk8 == 0){
        column_peg_594.chunk8 = new Chunk8();
    }
    column_peg_594.chunk8->chunk_all_compare = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr10_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_620(stream, "expr10_rest");
    int myposition = position;
    tail_peg_604:
    Value right;
    Value new_left;
    Result result_peg_603(myposition);
    {
    
        result_peg_603 = rule_s(stream, result_peg_603.getPosition());
            if (result_peg_603.error()){
                goto out_peg_606;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_603.getPosition()))){
                    result_peg_603.nextPosition();
                } else {
                    goto out_peg_606;
                }
            }
            result_peg_603.setValue((void*) "+");
        
        
        
        result_peg_603 = rule_s(stream, result_peg_603.getPosition());
            if (result_peg_603.error()){
                goto out_peg_606;
            }
        
        
        
        result_peg_603 = rule_expr11(stream, result_peg_603.getPosition());
            if (result_peg_603.error()){
                goto out_peg_606;
            }
            right = result_peg_603.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionAdd(left, right);
                result_peg_603.setValue(value);
            }
            new_left = result_peg_603.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_603.getPosition();
    goto tail_peg_604;
    out_peg_606:
    Result result_peg_611(myposition);
    {
    
        result_peg_611 = rule_s(stream, result_peg_611.getPosition());
            if (result_peg_611.error()){
                goto out_peg_613;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_611.getPosition()))){
                    result_peg_611.nextPosition();
                } else {
                    goto out_peg_613;
                }
            }
            result_peg_611.setValue((void*) "-");
        
        
        
        result_peg_611 = rule_s(stream, result_peg_611.getPosition());
            if (result_peg_611.error()){
                goto out_peg_613;
            }
        
        
        
        result_peg_611 = rule_expr11(stream, result_peg_611.getPosition());
            if (result_peg_611.error()){
                goto out_peg_613;
            }
            right = result_peg_611.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionSubtract(left, right);
                result_peg_611.setValue(value);
            }
            new_left = result_peg_611.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_611.getPosition();
    goto tail_peg_604;
    out_peg_613:
    Result result_peg_618(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_618.setValue(value);
            }
        
        
    }
    stream.update(result_peg_618.getPosition());
    
    return result_peg_618;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr11_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_646(stream, "expr11_rest");
    int myposition = position;
    tail_peg_623:
    Value right;
    Value new_left;
    Result result_peg_622(myposition);
    {
    
        result_peg_622 = rule_s(stream, result_peg_622.getPosition());
            if (result_peg_622.error()){
                goto out_peg_625;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("*"[i], stream.get(result_peg_622.getPosition()))){
                    result_peg_622.nextPosition();
                } else {
                    goto out_peg_625;
                }
            }
            result_peg_622.setValue((void*) "*");
        
        
        
        result_peg_622 = rule_s(stream, result_peg_622.getPosition());
            if (result_peg_622.error()){
                goto out_peg_625;
            }
        
        
        
        result_peg_622 = rule_expr12(stream, result_peg_622.getPosition());
            if (result_peg_622.error()){
                goto out_peg_625;
            }
            right = result_peg_622.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionMultiply(left, right);
                result_peg_622.setValue(value);
            }
            new_left = result_peg_622.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_622.getPosition();
    goto tail_peg_623;
    out_peg_625:
    Result result_peg_630(myposition);
    {
    
        result_peg_630 = rule_s(stream, result_peg_630.getPosition());
            if (result_peg_630.error()){
                goto out_peg_632;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_630.getPosition()))){
                    result_peg_630.nextPosition();
                } else {
                    goto out_peg_632;
                }
            }
            result_peg_630.setValue((void*) "/");
        
        
        
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
                value = makeExpressionDivide(left, right);
                result_peg_630.setValue(value);
            }
            new_left = result_peg_630.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_630.getPosition();
    goto tail_peg_623;
    out_peg_632:
    Result result_peg_637(myposition);
    {
    
        result_peg_637 = rule_s(stream, result_peg_637.getPosition());
            if (result_peg_637.error()){
                goto out_peg_639;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("%"[i], stream.get(result_peg_637.getPosition()))){
                    result_peg_637.nextPosition();
                } else {
                    goto out_peg_639;
                }
            }
            result_peg_637.setValue((void*) "%");
        
        
        
        result_peg_637 = rule_s(stream, result_peg_637.getPosition());
            if (result_peg_637.error()){
                goto out_peg_639;
            }
        
        
        
        result_peg_637 = rule_expr12(stream, result_peg_637.getPosition());
            if (result_peg_637.error()){
                goto out_peg_639;
            }
            right = result_peg_637.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionModulo(left, right);
                result_peg_637.setValue(value);
            }
            new_left = result_peg_637.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_637.getPosition();
    goto tail_peg_623;
    out_peg_639:
    Result result_peg_644(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_644.setValue(value);
            }
        
        
    }
    stream.update(result_peg_644.getPosition());
    
    return result_peg_644;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_expr12_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_658(stream, "expr12_rest");
    int myposition = position;
    tail_peg_649:
    Value right;
    Value new_left;
    Result result_peg_648(myposition);
    {
    
        result_peg_648 = rule_s(stream, result_peg_648.getPosition());
            if (result_peg_648.error()){
                goto out_peg_651;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("**"[i], stream.get(result_peg_648.getPosition()))){
                    result_peg_648.nextPosition();
                } else {
                    goto out_peg_651;
                }
            }
            result_peg_648.setValue((void*) "**");
        
        
        
        result_peg_648 = rule_s(stream, result_peg_648.getPosition());
            if (result_peg_648.error()){
                goto out_peg_651;
            }
        
        
        
        result_peg_648 = rule_expr13(stream, result_peg_648.getPosition());
            if (result_peg_648.error()){
                goto out_peg_651;
            }
            right = result_peg_648.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeExpressionPower(left, right);
                result_peg_648.setValue(value);
            }
            new_left = result_peg_648.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_648.getPosition();
    goto tail_peg_649;
    out_peg_651:
    Result result_peg_656(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_656.setValue(value);
            }
        
        
    }
    stream.update(result_peg_656.getPosition());
    
    return result_peg_656;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_unary(Stream & stream, const int position){
    
    Column & column_peg_659 = stream.getColumn(position);
    if (column_peg_659.chunk9 != 0 && column_peg_659.chunk9->chunk_unary.calculated()){
        return column_peg_659.chunk9->chunk_unary;
    }
    
    RuleTrace trace_peg_688(stream, "unary");
    int myposition = position;
    
    
    Result result_peg_660(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("!"[i], stream.get(result_peg_660.getPosition()))){
                    result_peg_660.nextPosition();
                } else {
                    goto out_peg_662;
                }
            }
            result_peg_660.setValue((void*) "!");
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Not;
                result_peg_660.setValue(value);
            }
        
        
    }
    
    if (column_peg_659.chunk9 == 0){
        column_peg_659.chunk9 = new Chunk9();
    }
    column_peg_659.chunk9->chunk_unary = result_peg_660;
    stream.update(result_peg_660.getPosition());
    
    
    return result_peg_660;
    out_peg_662:
    Result result_peg_663(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("-"[i], stream.get(result_peg_663.getPosition()))){
                    result_peg_663.nextPosition();
                } else {
                    goto out_peg_665;
                }
            }
            result_peg_663.setValue((void*) "-");
        
        
        
        Result result_peg_668(result_peg_663);
            {
                
                char letter_peg_673 = stream.get(result_peg_668.getPosition());
                if (letter_peg_673 != '\0' && strchr("0123456789", letter_peg_673) != NULL){
                    result_peg_668.nextPosition();
                    result_peg_668.setValue((void*) (long) letter_peg_673);
                } else {
                    goto out_peg_672;
                }
                
            }
            goto success_peg_669;
            out_peg_672:
            goto not_peg_667;
            success_peg_669:
            ;
            goto out_peg_665;
            not_peg_667:
            result_peg_663.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Minus;
                result_peg_663.setValue(value);
            }
        
        
    }
    
    if (column_peg_659.chunk9 == 0){
        column_peg_659.chunk9 = new Chunk9();
    }
    column_peg_659.chunk9->chunk_unary = result_peg_663;
    stream.update(result_peg_663.getPosition());
    
    
    return result_peg_663;
    out_peg_665:
    Result result_peg_674(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_674.getPosition()))){
                    result_peg_674.nextPosition();
                } else {
                    goto out_peg_676;
                }
            }
            result_peg_674.setValue((void*) "~");
        
        
        
        Result result_peg_679(result_peg_674);
            result_peg_679 = rule_key(stream, result_peg_679.getPosition());
            if (result_peg_679.error()){
                goto not_peg_678;
            }
            goto out_peg_676;
            not_peg_678:
            result_peg_674.setValue((void*)0);
        
        
        
        Result result_peg_682(result_peg_674);
            {
                
                char letter_peg_687 = stream.get(result_peg_682.getPosition());
                if (letter_peg_687 != '\0' && strchr("0123456789", letter_peg_687) != NULL){
                    result_peg_682.nextPosition();
                    result_peg_682.setValue((void*) (long) letter_peg_687);
                } else {
                    goto out_peg_686;
                }
                
            }
            goto success_peg_683;
            out_peg_686:
            goto not_peg_681;
            success_peg_683:
            ;
            goto out_peg_676;
            not_peg_681:
            result_peg_674.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = (void*) Ast::ExpressionUnary::Negation;
                result_peg_674.setValue(value);
            }
        
        
    }
    
    if (column_peg_659.chunk9 == 0){
        column_peg_659.chunk9 = new Chunk9();
    }
    column_peg_659.chunk9->chunk_unary = result_peg_674;
    stream.update(result_peg_674.getPosition());
    
    
    return result_peg_674;
    out_peg_676:
    
    if (column_peg_659.chunk9 == 0){
        column_peg_659.chunk9 = new Chunk9();
    }
    column_peg_659.chunk9->chunk_unary = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_expr13_real(Stream & stream, const int position){
    
    Column & column_peg_689 = stream.getColumn(position);
    if (column_peg_689.chunk9 != 0 && column_peg_689.chunk9->chunk_expr13_real.calculated()){
        return column_peg_689.chunk9->chunk_expr13_real;
    }
    
    RuleTrace trace_peg_701(stream, "expr13_real");
    int myposition = position;
    
    Value e;
    Result result_peg_690(myposition);
    
    result_peg_690 = rule_function(stream, result_peg_690.getPosition());
    if (result_peg_690.error()){
        goto out_peg_691;
    }
    
    if (column_peg_689.chunk9 == 0){
        column_peg_689.chunk9 = new Chunk9();
    }
    column_peg_689.chunk9->chunk_expr13_real = result_peg_690;
    stream.update(result_peg_690.getPosition());
    
    
    return result_peg_690;
    out_peg_691:
    Result result_peg_692(myposition);
    
    result_peg_692 = rule_value(stream, result_peg_692.getPosition());
    if (result_peg_692.error()){
        goto out_peg_693;
    }
    
    if (column_peg_689.chunk9 == 0){
        column_peg_689.chunk9 = new Chunk9();
    }
    column_peg_689.chunk9->chunk_expr13_real = result_peg_692;
    stream.update(result_peg_692.getPosition());
    
    
    return result_peg_692;
    out_peg_693:
    Result result_peg_694(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_694.getPosition()))){
                    result_peg_694.nextPosition();
                } else {
                    goto out_peg_696;
                }
            }
            result_peg_694.setValue((void*) "(");
        
        
        
        result_peg_694 = rule_s(stream, result_peg_694.getPosition());
            if (result_peg_694.error()){
                goto out_peg_696;
            }
        
        
        
        result_peg_694 = rule_expr(stream, result_peg_694.getPosition());
            if (result_peg_694.error()){
                goto out_peg_696;
            }
            e = result_peg_694.getValues();
        
        
        
        result_peg_694 = rule_s(stream, result_peg_694.getPosition());
            if (result_peg_694.error()){
                goto out_peg_696;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_694.getPosition()))){
                    result_peg_694.nextPosition();
                } else {
                    goto out_peg_696;
                }
            }
            result_peg_694.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = e;
                result_peg_694.setValue(value);
            }
        
        
    }
    
    if (column_peg_689.chunk9 == 0){
        column_peg_689.chunk9 = new Chunk9();
    }
    column_peg_689.chunk9->chunk_expr13_real = result_peg_694;
    stream.update(result_peg_694.getPosition());
    
    
    return result_peg_694;
    out_peg_696:
    
    if (column_peg_689.chunk9 == 0){
        column_peg_689.chunk9 = new Chunk9();
    }
    column_peg_689.chunk9->chunk_expr13_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function(Stream & stream, const int position){
    
    Column & column_peg_702 = stream.getColumn(position);
    if (column_peg_702.chunk9 != 0 && column_peg_702.chunk9->chunk_function.calculated()){
        return column_peg_702.chunk9->chunk_function;
    }
    
    RuleTrace trace_peg_728(stream, "function");
    int myposition = position;
    
    Value name;
    Value args;
    Value arg1;
    Result result_peg_703(myposition);
    
    {
    
        result_peg_703 = rule_function_name(stream, result_peg_703.getPosition());
            if (result_peg_703.error()){
                goto out_peg_705;
            }
            name = result_peg_703.getValues();
        
        
        
        result_peg_703 = rule_s(stream, result_peg_703.getPosition());
            if (result_peg_703.error()){
                goto out_peg_705;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("("[i], stream.get(result_peg_703.getPosition()))){
                    result_peg_703.nextPosition();
                } else {
                    goto out_peg_705;
                }
            }
            result_peg_703.setValue((void*) "(");
        
        
        
        result_peg_703 = rule_s(stream, result_peg_703.getPosition());
            if (result_peg_703.error()){
                goto out_peg_705;
            }
        
        
        
        result_peg_703 = rule_args(stream, result_peg_703.getPosition());
            if (result_peg_703.error()){
                goto out_peg_705;
            }
            args = result_peg_703.getValues();
        
        
        
        result_peg_703 = rule_s(stream, result_peg_703.getPosition());
            if (result_peg_703.error()){
                goto out_peg_705;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(")"[i], stream.get(result_peg_703.getPosition()))){
                    result_peg_703.nextPosition();
                } else {
                    goto out_peg_705;
                }
            }
            result_peg_703.setValue((void*) ")");
        
        
        
        {
                Value value((void*) 0);
                value = makeFunction(name, args);
                result_peg_703.setValue(value);
            }
        
        
    }
    
    if (column_peg_702.chunk9 == 0){
        column_peg_702.chunk9 = new Chunk9();
    }
    column_peg_702.chunk9->chunk_function = result_peg_703;
    stream.update(result_peg_703.getPosition());
    
    
    return result_peg_703;
    out_peg_705:
    Result result_peg_712(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_712.getPosition()))){
                    result_peg_712.nextPosition();
                } else {
                    goto out_peg_714;
                }
            }
            result_peg_712.setValue((void*) "animelem");
            name = result_peg_712.getValues();
        
        
        
        result_peg_712 = rule_s(stream, result_peg_712.getPosition());
            if (result_peg_712.error()){
                goto out_peg_714;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("="[i], stream.get(result_peg_712.getPosition()))){
                    result_peg_712.nextPosition();
                } else {
                    goto out_peg_714;
                }
            }
            result_peg_712.setValue((void*) "=");
        
        
        
        result_peg_712 = rule_s(stream, result_peg_712.getPosition());
            if (result_peg_712.error()){
                goto out_peg_714;
            }
        
        
        
        result_peg_712 = rule_integer(stream, result_peg_712.getPosition());
            if (result_peg_712.error()){
                goto out_peg_714;
            }
            arg1 = result_peg_712.getValues();
        
        
        
        result_peg_712 = rule_function_rest(stream, result_peg_712.getPosition());
            if (result_peg_712.error()){
                goto out_peg_714;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1(name, arg1);
                result_peg_712.setValue(value);
            }
        
        
    }
    
    if (column_peg_702.chunk9 == 0){
        column_peg_702.chunk9 = new Chunk9();
    }
    column_peg_702.chunk9->chunk_function = result_peg_712;
    stream.update(result_peg_712.getPosition());
    
    
    return result_peg_712;
    out_peg_714:
    Result result_peg_720(myposition);
    
    {
    
        for (int i = 0; i < 8; i++){
                if (compareCharCase("animelem"[i], stream.get(result_peg_720.getPosition()))){
                    result_peg_720.nextPosition();
                } else {
                    goto out_peg_722;
                }
            }
            result_peg_720.setValue((void*) "animelem");
            name = result_peg_720.getValues();
        
        
        
        result_peg_720 = rule_s(stream, result_peg_720.getPosition());
            if (result_peg_720.error()){
                goto out_peg_722;
            }
        
        
        
        for (int i = 0; i < 2; i++){
                if (compareChar("<="[i], stream.get(result_peg_720.getPosition()))){
                    result_peg_720.nextPosition();
                } else {
                    goto out_peg_722;
                }
            }
            result_peg_720.setValue((void*) "<=");
        
        
        
        result_peg_720 = rule_s(stream, result_peg_720.getPosition());
            if (result_peg_720.error()){
                goto out_peg_722;
            }
        
        
        
        result_peg_720 = rule_integer(stream, result_peg_720.getPosition());
            if (result_peg_720.error()){
                goto out_peg_722;
            }
            arg1 = result_peg_720.getValues();
        
        
        
        result_peg_720 = rule_function_rest(stream, result_peg_720.getPosition());
            if (result_peg_720.error()){
                goto out_peg_722;
            }
        
        
        
        {
                Value value((void*) 0);
                /* FIXME: handle function rest */
                                value = makeFunction1("animelem<=", arg1);
                result_peg_720.setValue(value);
            }
        
        
    }
    
    if (column_peg_702.chunk9 == 0){
        column_peg_702.chunk9 = new Chunk9();
    }
    column_peg_702.chunk9->chunk_function = result_peg_720;
    stream.update(result_peg_720.getPosition());
    
    
    return result_peg_720;
    out_peg_722:
    
    if (column_peg_702.chunk9 == 0){
        column_peg_702.chunk9 = new Chunk9();
    }
    column_peg_702.chunk9->chunk_function = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_args(Stream & stream, const int position){
    
    Column & column_peg_729 = stream.getColumn(position);
    if (column_peg_729.chunk10 != 0 && column_peg_729.chunk10->chunk_args.calculated()){
        return column_peg_729.chunk10->chunk_args;
    }
    
    RuleTrace trace_peg_739(stream, "args");
    int myposition = position;
    
    Value expr1;
    Value expr_rest;
    Result result_peg_730(myposition);
    
    {
    
        result_peg_730 = rule_expr_c(stream, result_peg_730.getPosition());
            if (result_peg_730.error()){
                goto out_peg_732;
            }
            expr1 = result_peg_730.getValues();
        
        
        
        result_peg_730.reset();
            do{
                Result result_peg_735(result_peg_730.getPosition());
                {
                
                    result_peg_735 = rule_s(stream, result_peg_735.getPosition());
                        if (result_peg_735.error()){
                            goto loop_peg_734;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_735.getPosition()))){
                                result_peg_735.nextPosition();
                            } else {
                                goto loop_peg_734;
                            }
                        }
                        result_peg_735.setValue((void*) ",");
                    
                    
                    
                    result_peg_735 = rule_s(stream, result_peg_735.getPosition());
                        if (result_peg_735.error()){
                            goto loop_peg_734;
                        }
                    
                    
                    
                    result_peg_735 = rule_expr_c(stream, result_peg_735.getPosition());
                        if (result_peg_735.error()){
                            goto loop_peg_734;
                        }
                    
                    
                }
                result_peg_730.addResult(result_peg_735);
            } while (true);
            loop_peg_734:
            ;
            expr_rest = result_peg_730.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeValueList(expr1, expr_rest);
                result_peg_730.setValue(value);
            }
        
        
    }
    
    if (column_peg_729.chunk10 == 0){
        column_peg_729.chunk10 = new Chunk10();
    }
    column_peg_729.chunk10->chunk_args = result_peg_730;
    stream.update(result_peg_730.getPosition());
    
    
    return result_peg_730;
    out_peg_732:
    
    if (column_peg_729.chunk10 == 0){
        column_peg_729.chunk10 = new Chunk10();
    }
    column_peg_729.chunk10->chunk_args = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_name(Stream & stream, const int position){
    
    Column & column_peg_740 = stream.getColumn(position);
    if (column_peg_740.chunk10 != 0 && column_peg_740.chunk10->chunk_function_name.calculated()){
        return column_peg_740.chunk10->chunk_function_name;
    }
    
    RuleTrace trace_peg_796(stream, "function_name");
    int myposition = position;
    
    
    Result result_peg_741(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("abs"[i], stream.get(result_peg_741.getPosition()))){
            result_peg_741.nextPosition();
        } else {
            goto out_peg_742;
        }
    }
    result_peg_741.setValue((void*) "abs");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_741;
    stream.update(result_peg_741.getPosition());
    
    
    return result_peg_741;
    out_peg_742:
    Result result_peg_743(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("const"[i], stream.get(result_peg_743.getPosition()))){
            result_peg_743.nextPosition();
        } else {
            goto out_peg_744;
        }
    }
    result_peg_743.setValue((void*) "const");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_743;
    stream.update(result_peg_743.getPosition());
    
    
    return result_peg_743;
    out_peg_744:
    Result result_peg_745(myposition);
    
    for (int i = 0; i < 13; i++){
        if (compareCharCase("selfanimexist"[i], stream.get(result_peg_745.getPosition()))){
            result_peg_745.nextPosition();
        } else {
            goto out_peg_746;
        }
    }
    result_peg_745.setValue((void*) "selfanimexist");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_745;
    stream.update(result_peg_745.getPosition());
    
    
    return result_peg_745;
    out_peg_746:
    Result result_peg_747(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("ifelse"[i], stream.get(result_peg_747.getPosition()))){
            result_peg_747.nextPosition();
        } else {
            goto out_peg_748;
        }
    }
    result_peg_747.setValue((void*) "ifelse");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_747;
    stream.update(result_peg_747.getPosition());
    
    
    return result_peg_747;
    out_peg_748:
    Result result_peg_749(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("gethitvar"[i], stream.get(result_peg_749.getPosition()))){
            result_peg_749.nextPosition();
        } else {
            goto out_peg_750;
        }
    }
    result_peg_749.setValue((void*) "gethitvar");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_749;
    stream.update(result_peg_749.getPosition());
    
    
    return result_peg_749;
    out_peg_750:
    Result result_peg_751(myposition);
    
    for (int i = 0; i < 5; i++){
        if (compareCharCase("floor"[i], stream.get(result_peg_751.getPosition()))){
            result_peg_751.nextPosition();
        } else {
            goto out_peg_752;
        }
    }
    result_peg_751.setValue((void*) "floor");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_751;
    stream.update(result_peg_751.getPosition());
    
    
    return result_peg_751;
    out_peg_752:
    Result result_peg_753(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("ceil"[i], stream.get(result_peg_753.getPosition()))){
            result_peg_753.nextPosition();
        } else {
            goto out_peg_754;
        }
    }
    result_peg_753.setValue((void*) "ceil");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_753;
    stream.update(result_peg_753.getPosition());
    
    
    return result_peg_753;
    out_peg_754:
    Result result_peg_755(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("sin"[i], stream.get(result_peg_755.getPosition()))){
            result_peg_755.nextPosition();
        } else {
            goto out_peg_756;
        }
    }
    result_peg_755.setValue((void*) "sin");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_755;
    stream.update(result_peg_755.getPosition());
    
    
    return result_peg_755;
    out_peg_756:
    Result result_peg_757(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("sysvar"[i], stream.get(result_peg_757.getPosition()))){
            result_peg_757.nextPosition();
        } else {
            goto out_peg_758;
        }
    }
    result_peg_757.setValue((void*) "sysvar");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_757;
    stream.update(result_peg_757.getPosition());
    
    
    return result_peg_757;
    out_peg_758:
    Result result_peg_759(myposition);
    
    for (int i = 0; i < 3; i++){
        if (compareCharCase("var"[i], stream.get(result_peg_759.getPosition()))){
            result_peg_759.nextPosition();
        } else {
            goto out_peg_760;
        }
    }
    result_peg_759.setValue((void*) "var");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_759;
    stream.update(result_peg_759.getPosition());
    
    
    return result_peg_759;
    out_peg_760:
    Result result_peg_761(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numexplod"[i], stream.get(result_peg_761.getPosition()))){
            result_peg_761.nextPosition();
        } else {
            goto out_peg_762;
        }
    }
    result_peg_761.setValue((void*) "numexplod");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_761;
    stream.update(result_peg_761.getPosition());
    
    
    return result_peg_761;
    out_peg_762:
    Result result_peg_763(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_763.getPosition()))){
            result_peg_763.nextPosition();
        } else {
            goto out_peg_764;
        }
    }
    result_peg_763.setValue((void*) "numhelper");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_763;
    stream.update(result_peg_763.getPosition());
    
    
    return result_peg_763;
    out_peg_764:
    Result result_peg_765(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numprojid"[i], stream.get(result_peg_765.getPosition()))){
            result_peg_765.nextPosition();
        } else {
            goto out_peg_766;
        }
    }
    result_peg_765.setValue((void*) "numprojid");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_765;
    stream.update(result_peg_765.getPosition());
    
    
    return result_peg_765;
    out_peg_766:
    Result result_peg_767(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("helper"[i], stream.get(result_peg_767.getPosition()))){
            result_peg_767.nextPosition();
        } else {
            goto out_peg_768;
        }
    }
    result_peg_767.setValue((void*) "helper");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_767;
    stream.update(result_peg_767.getPosition());
    
    
    return result_peg_767;
    out_peg_768:
    Result result_peg_769(myposition);
    
    for (int i = 0; i < 4; i++){
        if (compareCharCase("fvar"[i], stream.get(result_peg_769.getPosition()))){
            result_peg_769.nextPosition();
        } else {
            goto out_peg_770;
        }
    }
    result_peg_769.setValue((void*) "fvar");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_769;
    stream.update(result_peg_769.getPosition());
    
    
    return result_peg_769;
    out_peg_770:
    Result result_peg_771(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemynear"[i], stream.get(result_peg_771.getPosition()))){
            result_peg_771.nextPosition();
        } else {
            goto out_peg_772;
        }
    }
    result_peg_771.setValue((void*) "enemynear");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_771;
    stream.update(result_peg_771.getPosition());
    
    
    return result_peg_771;
    out_peg_772:
    Result result_peg_773(myposition);
    
    for (int i = 0; i < 6; i++){
        if (compareCharCase("target"[i], stream.get(result_peg_773.getPosition()))){
            result_peg_773.nextPosition();
        } else {
            goto out_peg_774;
        }
    }
    result_peg_773.setValue((void*) "target");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_773;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_775;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_777;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_779;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_781;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_783;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_785;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_787;
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
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_789;
    stream.update(result_peg_789.getPosition());
    
    
    return result_peg_789;
    out_peg_790:
    Result result_peg_791(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("numhelper"[i], stream.get(result_peg_791.getPosition()))){
            result_peg_791.nextPosition();
        } else {
            goto out_peg_792;
        }
    }
    result_peg_791.setValue((void*) "numhelper");
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_791;
    stream.update(result_peg_791.getPosition());
    
    
    return result_peg_791;
    out_peg_792:
    Result result_peg_793(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("helper"[i], stream.get(result_peg_793.getPosition()))){
                    result_peg_793.nextPosition();
                } else {
                    goto out_peg_795;
                }
            }
            result_peg_793.setValue((void*) "helper");
        
        
        
        result_peg_793 = rule_s(stream, result_peg_793.getPosition());
            if (result_peg_793.error()){
                goto out_peg_795;
            }
        
        
    }
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = result_peg_793;
    stream.update(result_peg_793.getPosition());
    
    
    return result_peg_793;
    out_peg_795:
    
    if (column_peg_740.chunk10 == 0){
        column_peg_740.chunk10 = new Chunk10();
    }
    column_peg_740.chunk10->chunk_function_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_function_rest(Stream & stream, const int position){
    
    Column & column_peg_797 = stream.getColumn(position);
    if (column_peg_797.chunk10 != 0 && column_peg_797.chunk10->chunk_function_rest.calculated()){
        return column_peg_797.chunk10->chunk_function_rest;
    }
    
    RuleTrace trace_peg_807(stream, "function_rest");
    int myposition = position;
    
    
    Result result_peg_798(myposition);
    
    {
    
        result_peg_798 = rule_s(stream, result_peg_798.getPosition());
            if (result_peg_798.error()){
                goto out_peg_800;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_798.getPosition()))){
                    result_peg_798.nextPosition();
                } else {
                    goto out_peg_800;
                }
            }
            result_peg_798.setValue((void*) ",");
        
        
        
        result_peg_798 = rule_s(stream, result_peg_798.getPosition());
            if (result_peg_798.error()){
                goto out_peg_800;
            }
        
        
        
        int save_peg_804 = result_peg_798.getPosition();
            
            result_peg_798 = rule_all_compare(stream, result_peg_798.getPosition());
            if (result_peg_798.error()){
                
                result_peg_798 = Result(save_peg_804);
                result_peg_798.setValue((void*) 0);
                
            }
        
        
        
        result_peg_798 = rule_s(stream, result_peg_798.getPosition());
            if (result_peg_798.error()){
                goto out_peg_800;
            }
        
        
        
        result_peg_798 = rule_value(stream, result_peg_798.getPosition());
            if (result_peg_798.error()){
                goto out_peg_800;
            }
        
        
    }
    
    if (column_peg_797.chunk10 == 0){
        column_peg_797.chunk10 = new Chunk10();
    }
    column_peg_797.chunk10->chunk_function_rest = result_peg_798;
    stream.update(result_peg_798.getPosition());
    
    
    return result_peg_798;
    out_peg_800:
    Result result_peg_806(myposition);
    
    
    
    if (column_peg_797.chunk10 == 0){
        column_peg_797.chunk10 = new Chunk10();
    }
    column_peg_797.chunk10->chunk_function_rest = result_peg_806;
    stream.update(result_peg_806.getPosition());
    
    
    return result_peg_806;
    
    if (column_peg_797.chunk10 == 0){
        column_peg_797.chunk10 = new Chunk10();
    }
    column_peg_797.chunk10->chunk_function_rest = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keys(Stream & stream, const int position){
    
    Column & column_peg_808 = stream.getColumn(position);
    if (column_peg_808.chunk10 != 0 && column_peg_808.chunk10->chunk_keys.calculated()){
        return column_peg_808.chunk10->chunk_keys;
    }
    
    RuleTrace trace_peg_811(stream, "keys");
    int myposition = position;
    
    
    Result result_peg_809(myposition);
    
    result_peg_809 = rule_key_value_list(stream, result_peg_809.getPosition());
    if (result_peg_809.error()){
        goto out_peg_810;
    }
    
    if (column_peg_808.chunk10 == 0){
        column_peg_808.chunk10 = new Chunk10();
    }
    column_peg_808.chunk10->chunk_keys = result_peg_809;
    stream.update(result_peg_809.getPosition());
    
    
    return result_peg_809;
    out_peg_810:
    
    if (column_peg_808.chunk10 == 0){
        column_peg_808.chunk10 = new Chunk10();
    }
    column_peg_808.chunk10->chunk_keys = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_value_list(Stream & stream, const int position){
    
    Column & column_peg_812 = stream.getColumn(position);
    if (column_peg_812.chunk10 != 0 && column_peg_812.chunk10->chunk_key_value_list.calculated()){
        return column_peg_812.chunk10->chunk_key_value_list;
    }
    
    RuleTrace trace_peg_822(stream, "key_value_list");
    int myposition = position;
    
    Value first;
    Value rest;
    Result result_peg_813(myposition);
    
    {
    
        result_peg_813 = rule_key(stream, result_peg_813.getPosition());
            if (result_peg_813.error()){
                goto out_peg_815;
            }
            first = result_peg_813.getValues();
        
        
        
        result_peg_813.reset();
            do{
                Result result_peg_818(result_peg_813.getPosition());
                {
                
                    result_peg_818 = rule_s(stream, result_peg_818.getPosition());
                        if (result_peg_818.error()){
                            goto loop_peg_817;
                        }
                    
                    
                    
                    for (int i = 0; i < 1; i++){
                            if (compareChar(","[i], stream.get(result_peg_818.getPosition()))){
                                result_peg_818.nextPosition();
                            } else {
                                goto loop_peg_817;
                            }
                        }
                        result_peg_818.setValue((void*) ",");
                    
                    
                    
                    result_peg_818 = rule_s(stream, result_peg_818.getPosition());
                        if (result_peg_818.error()){
                            goto loop_peg_817;
                        }
                    
                    
                    
                    result_peg_818 = rule_key(stream, result_peg_818.getPosition());
                        if (result_peg_818.error()){
                            goto loop_peg_817;
                        }
                    
                    
                }
                result_peg_813.addResult(result_peg_818);
            } while (true);
            loop_peg_817:
            ;
            rest = result_peg_813.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyList(first, rest);
                result_peg_813.setValue(value);
            }
        
        
    }
    
    if (column_peg_812.chunk10 == 0){
        column_peg_812.chunk10 = new Chunk10();
    }
    column_peg_812.chunk10->chunk_key_value_list = result_peg_813;
    stream.update(result_peg_813.getPosition());
    
    
    return result_peg_813;
    out_peg_815:
    
    if (column_peg_812.chunk10 == 0){
        column_peg_812.chunk10 = new Chunk10();
    }
    column_peg_812.chunk10->chunk_key_value_list = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key(Stream & stream, const int position){
    
    Column & column_peg_823 = stream.getColumn(position);
    if (column_peg_823.chunk11 != 0 && column_peg_823.chunk11->chunk_key.calculated()){
        return column_peg_823.chunk11->chunk_key;
    }
    
    RuleTrace trace_peg_831(stream, "key");
    int myposition = position;
    
    Value ok;
    Result result_peg_824(myposition);
    
    {
    
        result_peg_824 = rule_key_real(stream, result_peg_824.getPosition());
            if (result_peg_824.error()){
                goto out_peg_826;
            }
        
        Result result_peg_825 = result_peg_824;
        
        result_peg_824 = rule_key_rest(stream, result_peg_824.getPosition(), result_peg_825.getValues());
            if (result_peg_824.error()){
                goto out_peg_826;
            }
            ok = result_peg_824.getValues();
        
        
        
        Result result_peg_830(result_peg_824);
            result_peg_830 = rule_identifier(stream, result_peg_830.getPosition());
            if (result_peg_830.error()){
                goto not_peg_829;
            }
            goto out_peg_826;
            not_peg_829:
            result_peg_824.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = ok;
                result_peg_824.setValue(value);
            }
        
        
    }
    
    if (column_peg_823.chunk11 == 0){
        column_peg_823.chunk11 = new Chunk11();
    }
    column_peg_823.chunk11->chunk_key = result_peg_824;
    stream.update(result_peg_824.getPosition());
    
    
    return result_peg_824;
    out_peg_826:
    
    if (column_peg_823.chunk11 == 0){
        column_peg_823.chunk11 = new Chunk11();
    }
    column_peg_823.chunk11->chunk_key = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_rest(Stream & stream, const int position, Value left){
    
    RuleTrace trace_peg_842(stream, "key_rest");
    int myposition = position;
    tail_peg_834:
    Value another;
    Value new_left;
    Result result_peg_833(myposition);
    {
    
        result_peg_833 = rule_s(stream, result_peg_833.getPosition());
            if (result_peg_833.error()){
                goto out_peg_836;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar("+"[i], stream.get(result_peg_833.getPosition()))){
                    result_peg_833.nextPosition();
                } else {
                    goto out_peg_836;
                }
            }
            result_peg_833.setValue((void*) "+");
        
        
        
        result_peg_833 = rule_key_real(stream, result_peg_833.getPosition());
            if (result_peg_833.error()){
                goto out_peg_836;
            }
            another = result_peg_833.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyCombined(left, another);
                result_peg_833.setValue(value);
            }
            new_left = result_peg_833.getValues();
        
        
        
        left = new_left;
        
        
    }
    myposition = result_peg_833.getPosition();
    goto tail_peg_834;
    out_peg_836:
    Result result_peg_840(myposition);
    
    {
    
        
        
        
        
        {
                Value value((void*) 0);
                value = left;
                result_peg_840.setValue(value);
            }
        
        
    }
    stream.update(result_peg_840.getPosition());
    
    return result_peg_840;
    stream.update(errorResult.getPosition());
    
    return errorResult;
}
        

Result rule_key_real(Stream & stream, const int position){
    
    Column & column_peg_843 = stream.getColumn(position);
    if (column_peg_843.chunk11 != 0 && column_peg_843.chunk11->chunk_key_real.calculated()){
        return column_peg_843.chunk11->chunk_key_real;
    }
    
    RuleTrace trace_peg_850(stream, "key_real");
    int myposition = position;
    
    Value mods;
    Value name;
    Result result_peg_844(myposition);
    
    {
    
        result_peg_844.reset();
            do{
                Result result_peg_847(result_peg_844.getPosition());
                result_peg_847 = rule_key_modifier(stream, result_peg_847.getPosition());
                if (result_peg_847.error()){
                    goto loop_peg_846;
                }
                result_peg_844.addResult(result_peg_847);
            } while (true);
            loop_peg_846:
            ;
            mods = result_peg_844.getValues();
        
        
        
        result_peg_844 = rule_key_name(stream, result_peg_844.getPosition());
            if (result_peg_844.error()){
                goto out_peg_849;
            }
            name = result_peg_844.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = applyKeyModifiers(mods, makeKey(name));
                result_peg_844.setValue(value);
            }
        
        
    }
    
    if (column_peg_843.chunk11 == 0){
        column_peg_843.chunk11 = new Chunk11();
    }
    column_peg_843.chunk11->chunk_key_real = result_peg_844;
    stream.update(result_peg_844.getPosition());
    
    
    return result_peg_844;
    out_peg_849:
    
    if (column_peg_843.chunk11 == 0){
        column_peg_843.chunk11 = new Chunk11();
    }
    column_peg_843.chunk11->chunk_key_real = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_modifier(Stream & stream, const int position){
    
    Column & column_peg_851 = stream.getColumn(position);
    if (column_peg_851.chunk11 != 0 && column_peg_851.chunk11->chunk_key_modifier.calculated()){
        return column_peg_851.chunk11->chunk_key_modifier;
    }
    
    RuleTrace trace_peg_872(stream, "key_modifier");
    int myposition = position;
    
    Value num;
    Result result_peg_852(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("~"[i], stream.get(result_peg_852.getPosition()))){
                    result_peg_852.nextPosition();
                } else {
                    goto out_peg_854;
                }
            }
            result_peg_852.setValue((void*) "~");
        
        
        
        result_peg_852.reset();
            do{
                Result result_peg_857(result_peg_852.getPosition());
                {
                    
                    char letter_peg_862 = stream.get(result_peg_857.getPosition());
                    if (letter_peg_862 != '\0' && strchr("0123456789", letter_peg_862) != NULL){
                        result_peg_857.nextPosition();
                        result_peg_857.setValue((void*) (long) letter_peg_862);
                    } else {
                        goto out_peg_861;
                    }
                    
                }
                goto success_peg_858;
                out_peg_861:
                goto loop_peg_856;
                success_peg_858:
                ;
                result_peg_852.addResult(result_peg_857);
            } while (true);
            loop_peg_856:
            ;
            num = result_peg_852.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = new ReleaseKeyModifier((int) *parseDouble(num));
                result_peg_852.setValue(value);
            }
        
        
    }
    
    if (column_peg_851.chunk11 == 0){
        column_peg_851.chunk11 = new Chunk11();
    }
    column_peg_851.chunk11->chunk_key_modifier = result_peg_852;
    stream.update(result_peg_852.getPosition());
    
    
    return result_peg_852;
    out_peg_854:
    Result result_peg_863(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("$"[i], stream.get(result_peg_863.getPosition()))){
                    result_peg_863.nextPosition();
                } else {
                    goto out_peg_865;
                }
            }
            result_peg_863.setValue((void*) "$");
        
        
        
        {
                Value value((void*) 0);
                value = new DirectionKeyModifier();
                result_peg_863.setValue(value);
            }
        
        
    }
    
    if (column_peg_851.chunk11 == 0){
        column_peg_851.chunk11 = new Chunk11();
    }
    column_peg_851.chunk11->chunk_key_modifier = result_peg_863;
    stream.update(result_peg_863.getPosition());
    
    
    return result_peg_863;
    out_peg_865:
    Result result_peg_866(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar("/"[i], stream.get(result_peg_866.getPosition()))){
                    result_peg_866.nextPosition();
                } else {
                    goto out_peg_868;
                }
            }
            result_peg_866.setValue((void*) "/");
        
        
        
        {
                Value value((void*) 0);
                value = new HeldDownKeyModifier();
                result_peg_866.setValue(value);
            }
        
        
    }
    
    if (column_peg_851.chunk11 == 0){
        column_peg_851.chunk11 = new Chunk11();
    }
    column_peg_851.chunk11->chunk_key_modifier = result_peg_866;
    stream.update(result_peg_866.getPosition());
    
    
    return result_peg_866;
    out_peg_868:
    Result result_peg_869(myposition);
    
    {
    
        for (int i = 0; i < 1; i++){
                if (compareChar(">"[i], stream.get(result_peg_869.getPosition()))){
                    result_peg_869.nextPosition();
                } else {
                    goto out_peg_871;
                }
            }
            result_peg_869.setValue((void*) ">");
        
        
        
        {
                Value value((void*) 0);
                value = new OnlyKeyModifier();
                result_peg_869.setValue(value);
            }
        
        
    }
    
    if (column_peg_851.chunk11 == 0){
        column_peg_851.chunk11 = new Chunk11();
    }
    column_peg_851.chunk11->chunk_key_modifier = result_peg_869;
    stream.update(result_peg_869.getPosition());
    
    
    return result_peg_869;
    out_peg_871:
    
    if (column_peg_851.chunk11 == 0){
        column_peg_851.chunk11 = new Chunk11();
    }
    column_peg_851.chunk11->chunk_key_modifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_key_name(Stream & stream, const int position){
    
    Column & column_peg_873 = stream.getColumn(position);
    if (column_peg_873.chunk11 != 0 && column_peg_873.chunk11->chunk_key_name.calculated()){
        return column_peg_873.chunk11->chunk_key_name;
    }
    
    RuleTrace trace_peg_904(stream, "key_name");
    int myposition = position;
    
    
    Result result_peg_874(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DB"[i], stream.get(result_peg_874.getPosition()))){
            result_peg_874.nextPosition();
        } else {
            goto out_peg_875;
        }
    }
    result_peg_874.setValue((void*) "DB");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_874;
    stream.update(result_peg_874.getPosition());
    
    
    return result_peg_874;
    out_peg_875:
    Result result_peg_876(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("B"[i], stream.get(result_peg_876.getPosition()))){
            result_peg_876.nextPosition();
        } else {
            goto out_peg_877;
        }
    }
    result_peg_876.setValue((void*) "B");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_876;
    stream.update(result_peg_876.getPosition());
    
    
    return result_peg_876;
    out_peg_877:
    Result result_peg_878(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("DF"[i], stream.get(result_peg_878.getPosition()))){
            result_peg_878.nextPosition();
        } else {
            goto out_peg_879;
        }
    }
    result_peg_878.setValue((void*) "DF");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_878;
    stream.update(result_peg_878.getPosition());
    
    
    return result_peg_878;
    out_peg_879:
    Result result_peg_880(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("D"[i], stream.get(result_peg_880.getPosition()))){
            result_peg_880.nextPosition();
        } else {
            goto out_peg_881;
        }
    }
    result_peg_880.setValue((void*) "D");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_880;
    stream.update(result_peg_880.getPosition());
    
    
    return result_peg_880;
    out_peg_881:
    Result result_peg_882(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("F"[i], stream.get(result_peg_882.getPosition()))){
            result_peg_882.nextPosition();
        } else {
            goto out_peg_883;
        }
    }
    result_peg_882.setValue((void*) "F");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_882;
    stream.update(result_peg_882.getPosition());
    
    
    return result_peg_882;
    out_peg_883:
    Result result_peg_884(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UF"[i], stream.get(result_peg_884.getPosition()))){
            result_peg_884.nextPosition();
        } else {
            goto out_peg_885;
        }
    }
    result_peg_884.setValue((void*) "UF");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_884;
    stream.update(result_peg_884.getPosition());
    
    
    return result_peg_884;
    out_peg_885:
    Result result_peg_886(myposition);
    
    for (int i = 0; i < 2; i++){
        if (compareChar("UB"[i], stream.get(result_peg_886.getPosition()))){
            result_peg_886.nextPosition();
        } else {
            goto out_peg_887;
        }
    }
    result_peg_886.setValue((void*) "UB");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_886;
    stream.update(result_peg_886.getPosition());
    
    
    return result_peg_886;
    out_peg_887:
    Result result_peg_888(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("U"[i], stream.get(result_peg_888.getPosition()))){
            result_peg_888.nextPosition();
        } else {
            goto out_peg_889;
        }
    }
    result_peg_888.setValue((void*) "U");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_888;
    stream.update(result_peg_888.getPosition());
    
    
    return result_peg_888;
    out_peg_889:
    Result result_peg_890(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("a"[i], stream.get(result_peg_890.getPosition()))){
            result_peg_890.nextPosition();
        } else {
            goto out_peg_891;
        }
    }
    result_peg_890.setValue((void*) "a");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_890;
    stream.update(result_peg_890.getPosition());
    
    
    return result_peg_890;
    out_peg_891:
    Result result_peg_892(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("b"[i], stream.get(result_peg_892.getPosition()))){
            result_peg_892.nextPosition();
        } else {
            goto out_peg_893;
        }
    }
    result_peg_892.setValue((void*) "b");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_892;
    stream.update(result_peg_892.getPosition());
    
    
    return result_peg_892;
    out_peg_893:
    Result result_peg_894(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("c"[i], stream.get(result_peg_894.getPosition()))){
            result_peg_894.nextPosition();
        } else {
            goto out_peg_895;
        }
    }
    result_peg_894.setValue((void*) "c");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_894;
    stream.update(result_peg_894.getPosition());
    
    
    return result_peg_894;
    out_peg_895:
    Result result_peg_896(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("x"[i], stream.get(result_peg_896.getPosition()))){
            result_peg_896.nextPosition();
        } else {
            goto out_peg_897;
        }
    }
    result_peg_896.setValue((void*) "x");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_896;
    stream.update(result_peg_896.getPosition());
    
    
    return result_peg_896;
    out_peg_897:
    Result result_peg_898(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("y"[i], stream.get(result_peg_898.getPosition()))){
            result_peg_898.nextPosition();
        } else {
            goto out_peg_899;
        }
    }
    result_peg_898.setValue((void*) "y");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_898;
    stream.update(result_peg_898.getPosition());
    
    
    return result_peg_898;
    out_peg_899:
    Result result_peg_900(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("z"[i], stream.get(result_peg_900.getPosition()))){
            result_peg_900.nextPosition();
        } else {
            goto out_peg_901;
        }
    }
    result_peg_900.setValue((void*) "z");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_900;
    stream.update(result_peg_900.getPosition());
    
    
    return result_peg_900;
    out_peg_901:
    Result result_peg_902(myposition);
    
    for (int i = 0; i < 1; i++){
        if (compareChar("s"[i], stream.get(result_peg_902.getPosition()))){
            result_peg_902.nextPosition();
        } else {
            goto out_peg_903;
        }
    }
    result_peg_902.setValue((void*) "s");
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = result_peg_902;
    stream.update(result_peg_902.getPosition());
    
    
    return result_peg_902;
    out_peg_903:
    
    if (column_peg_873.chunk11 == 0){
        column_peg_873.chunk11 = new Chunk11();
    }
    column_peg_873.chunk11->chunk_key_name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_value(Stream & stream, const int position){
    
    Column & column_peg_905 = stream.getColumn(position);
    if (column_peg_905.chunk12 != 0 && column_peg_905.chunk12->chunk_value.calculated()){
        return column_peg_905.chunk12->chunk_value;
    }
    
    RuleTrace trace_peg_925(stream, "value");
    int myposition = position;
    
    
    Result result_peg_906(myposition);
    
    result_peg_906 = rule_float(stream, result_peg_906.getPosition());
    if (result_peg_906.error()){
        goto out_peg_907;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_906;
    stream.update(result_peg_906.getPosition());
    
    
    return result_peg_906;
    out_peg_907:
    Result result_peg_908(myposition);
    
    result_peg_908 = rule_integer(stream, result_peg_908.getPosition());
    if (result_peg_908.error()){
        goto out_peg_909;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_908;
    stream.update(result_peg_908.getPosition());
    
    
    return result_peg_908;
    out_peg_909:
    Result result_peg_910(myposition);
    
    result_peg_910 = rule_helper(stream, result_peg_910.getPosition());
    if (result_peg_910.error()){
        goto out_peg_911;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_910;
    stream.update(result_peg_910.getPosition());
    
    
    return result_peg_910;
    out_peg_911:
    Result result_peg_912(myposition);
    
    result_peg_912 = rule_keyword(stream, result_peg_912.getPosition());
    if (result_peg_912.error()){
        goto out_peg_913;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_912;
    stream.update(result_peg_912.getPosition());
    
    
    return result_peg_912;
    out_peg_913:
    Result result_peg_914(myposition);
    
    {
    
        Result result_peg_917(result_peg_914);
            result_peg_917 = rule_keyword(stream, result_peg_917.getPosition());
            if (result_peg_917.error()){
                goto not_peg_916;
            }
            goto out_peg_918;
            not_peg_916:
            result_peg_914.setValue((void*)0);
        
        
        
        result_peg_914 = rule_identifier(stream, result_peg_914.getPosition());
            if (result_peg_914.error()){
                goto out_peg_918;
            }
        
        
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_914;
    stream.update(result_peg_914.getPosition());
    
    
    return result_peg_914;
    out_peg_918:
    Result result_peg_919(myposition);
    
    result_peg_919 = rule_range(stream, result_peg_919.getPosition());
    if (result_peg_919.error()){
        goto out_peg_920;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_919;
    stream.update(result_peg_919.getPosition());
    
    
    return result_peg_919;
    out_peg_920:
    Result result_peg_921(myposition);
    
    result_peg_921 = rule_string(stream, result_peg_921.getPosition());
    if (result_peg_921.error()){
        goto out_peg_922;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_921;
    stream.update(result_peg_921.getPosition());
    
    
    return result_peg_921;
    out_peg_922:
    Result result_peg_923(myposition);
    
    result_peg_923 = rule_hitflag(stream, result_peg_923.getPosition());
    if (result_peg_923.error()){
        goto out_peg_924;
    }
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = result_peg_923;
    stream.update(result_peg_923.getPosition());
    
    
    return result_peg_923;
    out_peg_924:
    
    if (column_peg_905.chunk12 == 0){
        column_peg_905.chunk12 = new Chunk12();
    }
    column_peg_905.chunk12->chunk_value = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper(Stream & stream, const int position){
    
    Column & column_peg_926 = stream.getColumn(position);
    if (column_peg_926.chunk12 != 0 && column_peg_926.chunk12->chunk_helper.calculated()){
        return column_peg_926.chunk12->chunk_helper;
    }
    
    RuleTrace trace_peg_934(stream, "helper");
    int myposition = position;
    
    Value name;
    Value id;
    Result result_peg_927(myposition);
    
    {
    
        result_peg_927 = rule_helper__name(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto out_peg_929;
            }
            name = result_peg_927.getValues();
        
        
        
        result_peg_927 = rule_s(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto out_peg_929;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareChar(","[i], stream.get(result_peg_927.getPosition()))){
                    result_peg_927.nextPosition();
                } else {
                    goto out_peg_929;
                }
            }
            result_peg_927.setValue((void*) ",");
        
        
        
        result_peg_927 = rule_s(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto out_peg_929;
            }
        
        
        
        result_peg_927 = rule_helper__identifier(stream, result_peg_927.getPosition());
            if (result_peg_927.error()){
                goto out_peg_929;
            }
            id = result_peg_927.getValues();
        
        
        
        {
                Value value((void*) 0);
                value = makeHelper(name, id);
                result_peg_927.setValue(value);
            }
        
        
    }
    
    if (column_peg_926.chunk12 == 0){
        column_peg_926.chunk12 = new Chunk12();
    }
    column_peg_926.chunk12->chunk_helper = result_peg_927;
    stream.update(result_peg_927.getPosition());
    
    
    return result_peg_927;
    out_peg_929:
    
    if (column_peg_926.chunk12 == 0){
        column_peg_926.chunk12 = new Chunk12();
    }
    column_peg_926.chunk12->chunk_helper = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__name(Stream & stream, const int position){
    
    Column & column_peg_935 = stream.getColumn(position);
    if (column_peg_935.chunk12 != 0 && column_peg_935.chunk12->chunk_helper__name.calculated()){
        return column_peg_935.chunk12->chunk_helper__name;
    }
    
    RuleTrace trace_peg_938(stream, "helper__name");
    int myposition = position;
    
    
    Result result_peg_936(myposition);
    
    for (int i = 0; i < 9; i++){
        if (compareCharCase("enemynear"[i], stream.get(result_peg_936.getPosition()))){
            result_peg_936.nextPosition();
        } else {
            goto out_peg_937;
        }
    }
    result_peg_936.setValue((void*) "enemynear");
    
    if (column_peg_935.chunk12 == 0){
        column_peg_935.chunk12 = new Chunk12();
    }
    column_peg_935.chunk12->chunk_helper__name = result_peg_936;
    stream.update(result_peg_936.getPosition());
    
    
    return result_peg_936;
    out_peg_937:
    
    if (column_peg_935.chunk12 == 0){
        column_peg_935.chunk12 = new Chunk12();
    }
    column_peg_935.chunk12->chunk_helper__name = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_helper__identifier(Stream & stream, const int position){
    
    Column & column_peg_939 = stream.getColumn(position);
    if (column_peg_939.chunk12 != 0 && column_peg_939.chunk12->chunk_helper__identifier.calculated()){
        return column_peg_939.chunk12->chunk_helper__identifier;
    }
    
    RuleTrace trace_peg_947(stream, "helper__identifier");
    int myposition = position;
    
    
    Result result_peg_940(myposition);
    
    result_peg_940 = rule_keyword(stream, result_peg_940.getPosition());
    if (result_peg_940.error()){
        goto out_peg_941;
    }
    
    if (column_peg_939.chunk12 == 0){
        column_peg_939.chunk12 = new Chunk12();
    }
    column_peg_939.chunk12->chunk_helper__identifier = result_peg_940;
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
    
    if (column_peg_939.chunk12 == 0){
        column_peg_939.chunk12 = new Chunk12();
    }
    column_peg_939.chunk12->chunk_helper__identifier = result_peg_942;
    stream.update(result_peg_942.getPosition());
    
    
    return result_peg_942;
    out_peg_946:
    
    if (column_peg_939.chunk12 == 0){
        column_peg_939.chunk12 = new Chunk12();
    }
    column_peg_939.chunk12->chunk_helper__identifier = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_hitflag(Stream & stream, const int position){
    
    Column & column_peg_948 = stream.getColumn(position);
    if (column_peg_948.chunk12 != 0 && column_peg_948.chunk12->chunk_hitflag.calculated()){
        return column_peg_948.chunk12->chunk_hitflag;
    }
    
    RuleTrace trace_peg_968(stream, "hitflag");
    int myposition = position;
    
    
    Result result_peg_949(myposition);
    
    {
    
        result_peg_949.reset();
            do{
                Result result_peg_952(result_peg_949.getPosition());
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("A"[i], stream.get(result_peg_952.getPosition()))){
                            result_peg_952.nextPosition();
                        } else {
                            goto out_peg_955;
                        }
                    }
                    result_peg_952.setValue((void*) "A");
                        
                }
                goto success_peg_953;
                out_peg_955:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("M"[i], stream.get(result_peg_952.getPosition()))){
                            result_peg_952.nextPosition();
                        } else {
                            goto out_peg_957;
                        }
                    }
                    result_peg_952.setValue((void*) "M");
                        
                }
                goto success_peg_953;
                out_peg_957:
                {
                    
                    for (int i = 0; i < 1; i++){
                        if (compareChar("F"[i], stream.get(result_peg_952.getPosition()))){
                            result_peg_952.nextPosition();
                        } else {
                            goto out_peg_959;
                        }
                    }
                    result_peg_952.setValue((void*) "F");
                        
                }
                goto success_peg_953;
                out_peg_959:
                goto loop_peg_951;
                success_peg_953:
                ;
                result_peg_949.addResult(result_peg_952);
            } while (true);
            loop_peg_951:
            if (result_peg_949.matches() == 0){
                goto out_peg_960;
            }
        
        
        
        int save_peg_962 = result_peg_949.getPosition();
            
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("+"[i], stream.get(result_peg_949.getPosition()))){
                        result_peg_949.nextPosition();
                    } else {
                        goto out_peg_965;
                    }
                }
                result_peg_949.setValue((void*) "+");
                    
            }
            goto success_peg_963;
            out_peg_965:
            {
                
                for (int i = 0; i < 1; i++){
                    if (compareChar("-"[i], stream.get(result_peg_949.getPosition()))){
                        result_peg_949.nextPosition();
                    } else {
                        goto out_peg_967;
                    }
                }
                result_peg_949.setValue((void*) "-");
                    
            }
            goto success_peg_963;
            out_peg_967:
            
            result_peg_949 = Result(save_peg_962);
            result_peg_949.setValue((void*) 0);
            
            success_peg_963:
            ;
        
        
        
        {
                Value value((void*) 0);
                value = makeSimpleIdentifier("A");
                result_peg_949.setValue(value);
            }
        
        
    }
    
    if (column_peg_948.chunk12 == 0){
        column_peg_948.chunk12 = new Chunk12();
    }
    column_peg_948.chunk12->chunk_hitflag = result_peg_949;
    stream.update(result_peg_949.getPosition());
    
    
    return result_peg_949;
    out_peg_960:
    
    if (column_peg_948.chunk12 == 0){
        column_peg_948.chunk12 = new Chunk12();
    }
    column_peg_948.chunk12->chunk_hitflag = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword(Stream & stream, const int position){
    
    Column & column_peg_969 = stream.getColumn(position);
    if (column_peg_969.chunk13 != 0 && column_peg_969.chunk13->chunk_keyword.calculated()){
        return column_peg_969.chunk13->chunk_keyword;
    }
    
    RuleTrace trace_peg_976(stream, "keyword");
    int myposition = position;
    
    
    Result result_peg_970(myposition);
    
    {
    
        result_peg_970 = rule_keyword_real(stream, result_peg_970.getPosition());
            if (result_peg_970.error()){
                goto out_peg_972;
            }
        
        Result result_peg_971 = result_peg_970;
        
        Result result_peg_975(result_peg_970);
            result_peg_975 = rule_alpha_digit(stream, result_peg_975.getPosition());
            if (result_peg_975.error()){
                goto not_peg_974;
            }
            goto out_peg_972;
            not_peg_974:
            result_peg_970.setValue((void*)0);
        
        
        
        {
                Value value((void*) 0);
                value = result_peg_971.getValues();
                result_peg_970.setValue(value);
            }
        
        
    }
    
    if (column_peg_969.chunk13 == 0){
        column_peg_969.chunk13 = new Chunk13();
    }
    column_peg_969.chunk13->chunk_keyword = result_peg_970;
    stream.update(result_peg_970.getPosition());
    
    
    return result_peg_970;
    out_peg_972:
    
    if (column_peg_969.chunk13 == 0){
        column_peg_969.chunk13 = new Chunk13();
    }
    column_peg_969.chunk13->chunk_keyword = errorResult;
    stream.update(errorResult.getPosition());
    
    
    return errorResult;
}
        

Result rule_keyword_real(Stream & stream, const int position){
    
    Column & column_peg_977 = stream.getColumn(position);
    if (column_peg_977.chunk13 != 0 && column_peg_977.chunk13->chunk_keyword_real.calculated()){
        return column_peg_977.chunk13->chunk_keyword_real;
    }
    
    RuleTrace trace_peg_1065(stream, "keyword_real");
    int myposition = position;
    
    
    Result result_peg_978(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_978.getPosition()))){
                    result_peg_978.nextPosition();
                } else {
                    goto out_peg_980;
                }
            }
            result_peg_978.setValue((void*) "vel");
        
        
        
        result_peg_978 = rule_s(stream, result_peg_978.getPosition());
            if (result_peg_978.error()){
                goto out_peg_980;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_978.getPosition()))){
                    result_peg_978.nextPosition();
                } else {
                    goto out_peg_980;
                }
            }
            result_peg_978.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel y");
                result_peg_978.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_978;
    stream.update(result_peg_978.getPosition());
    
    
    return result_peg_978;
    out_peg_980:
    Result result_peg_983(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("vel"[i], stream.get(result_peg_983.getPosition()))){
                    result_peg_983.nextPosition();
                } else {
                    goto out_peg_985;
                }
            }
            result_peg_983.setValue((void*) "vel");
        
        
        
        result_peg_983 = rule_s(stream, result_peg_983.getPosition());
            if (result_peg_983.error()){
                goto out_peg_985;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_983.getPosition()))){
                    result_peg_983.nextPosition();
                } else {
                    goto out_peg_985;
                }
            }
            result_peg_983.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("vel x");
                result_peg_983.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_983;
    stream.update(result_peg_983.getPosition());
    
    
    return result_peg_983;
    out_peg_985:
    Result result_peg_988(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_988.getPosition()))){
                    result_peg_988.nextPosition();
                } else {
                    goto out_peg_990;
                }
            }
            result_peg_988.setValue((void*) "pos");
        
        
        
        result_peg_988 = rule_s(stream, result_peg_988.getPosition());
            if (result_peg_988.error()){
                goto out_peg_990;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_988.getPosition()))){
                    result_peg_988.nextPosition();
                } else {
                    goto out_peg_990;
                }
            }
            result_peg_988.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos y");
                result_peg_988.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_988;
    stream.update(result_peg_988.getPosition());
    
    
    return result_peg_988;
    out_peg_990:
    Result result_peg_993(myposition);
    
    {
    
        for (int i = 0; i < 3; i++){
                if (compareCharCase("pos"[i], stream.get(result_peg_993.getPosition()))){
                    result_peg_993.nextPosition();
                } else {
                    goto out_peg_995;
                }
            }
            result_peg_993.setValue((void*) "pos");
        
        
        
        result_peg_993 = rule_s(stream, result_peg_993.getPosition());
            if (result_peg_993.error()){
                goto out_peg_995;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_993.getPosition()))){
                    result_peg_993.nextPosition();
                } else {
                    goto out_peg_995;
                }
            }
            result_peg_993.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("pos x");
                result_peg_993.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_993;
    stream.update(result_peg_993.getPosition());
    
    
    return result_peg_993;
    out_peg_995:
    Result result_peg_998(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_998.getPosition()))){
                    result_peg_998.nextPosition();
                } else {
                    goto out_peg_1000;
                }
            }
            result_peg_998.setValue((void*) "p2dist");
        
        
        
        result_peg_998 = rule_s(stream, result_peg_998.getPosition());
            if (result_peg_998.error()){
                goto out_peg_1000;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_998.getPosition()))){
                    result_peg_998.nextPosition();
                } else {
                    goto out_peg_1000;
                }
            }
            result_peg_998.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist x");
                result_peg_998.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_998;
    stream.update(result_peg_998.getPosition());
    
    
    return result_peg_998;
    out_peg_1000:
    Result result_peg_1003(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p2dist"[i], stream.get(result_peg_1003.getPosition()))){
                    result_peg_1003.nextPosition();
                } else {
                    goto out_peg_1005;
                }
            }
            result_peg_1003.setValue((void*) "p2dist");
        
        
        
        result_peg_1003 = rule_s(stream, result_peg_1003.getPosition());
            if (result_peg_1003.error()){
                goto out_peg_1005;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1003.getPosition()))){
                    result_peg_1003.nextPosition();
                } else {
                    goto out_peg_1005;
                }
            }
            result_peg_1003.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2dist y");
                result_peg_1003.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1003;
    stream.update(result_peg_1003.getPosition());
    
    
    return result_peg_1003;
    out_peg_1005:
    Result result_peg_1008(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1008.getPosition()))){
                    result_peg_1008.nextPosition();
                } else {
                    goto out_peg_1010;
                }
            }
            result_peg_1008.setValue((void*) "p1dist");
        
        
        
        result_peg_1008 = rule_s(stream, result_peg_1008.getPosition());
            if (result_peg_1008.error()){
                goto out_peg_1010;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1008.getPosition()))){
                    result_peg_1008.nextPosition();
                } else {
                    goto out_peg_1010;
                }
            }
            result_peg_1008.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist x");
                result_peg_1008.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1008;
    stream.update(result_peg_1008.getPosition());
    
    
    return result_peg_1008;
    out_peg_1010:
    Result result_peg_1013(myposition);
    
    {
    
        for (int i = 0; i < 6; i++){
                if (compareCharCase("p1dist"[i], stream.get(result_peg_1013.getPosition()))){
                    result_peg_1013.nextPosition();
                } else {
                    goto out_peg_1015;
                }
            }
            result_peg_1013.setValue((void*) "p1dist");
        
        
        
        result_peg_1013 = rule_s(stream, result_peg_1013.getPosition());
            if (result_peg_1013.error()){
                goto out_peg_1015;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1013.getPosition()))){
                    result_peg_1013.nextPosition();
                } else {
                    goto out_peg_1015;
                }
            }
            result_peg_1013.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1dist y");
                result_peg_1013.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1013;
    stream.update(result_peg_1013.getPosition());
    
    
    return result_peg_1013;
    out_peg_1015:
    Result result_peg_1018(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1018.getPosition()))){
                    result_peg_1018.nextPosition();
                } else {
                    goto out_peg_1020;
                }
            }
            result_peg_1018.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1018 = rule_s(stream, result_peg_1018.getPosition());
            if (result_peg_1018.error()){
                goto out_peg_1020;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1018.getPosition()))){
                    result_peg_1018.nextPosition();
                } else {
                    goto out_peg_1020;
                }
            }
            result_peg_1018.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist x");
                result_peg_1018.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1018;
    stream.update(result_peg_1018.getPosition());
    
    
    return result_peg_1018;
    out_peg_1020:
    Result result_peg_1023(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p2bodydist"[i], stream.get(result_peg_1023.getPosition()))){
                    result_peg_1023.nextPosition();
                } else {
                    goto out_peg_1025;
                }
            }
            result_peg_1023.setValue((void*) "p2bodydist");
        
        
        
        result_peg_1023 = rule_s(stream, result_peg_1023.getPosition());
            if (result_peg_1023.error()){
                goto out_peg_1025;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1023.getPosition()))){
                    result_peg_1023.nextPosition();
                } else {
                    goto out_peg_1025;
                }
            }
            result_peg_1023.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p2bodydist y");
                result_peg_1023.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1023;
    stream.update(result_peg_1023.getPosition());
    
    
    return result_peg_1023;
    out_peg_1025:
    Result result_peg_1028(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1028.getPosition()))){
                    result_peg_1028.nextPosition();
                } else {
                    goto out_peg_1030;
                }
            }
            result_peg_1028.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1028 = rule_s(stream, result_peg_1028.getPosition());
            if (result_peg_1028.error()){
                goto out_peg_1030;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1028.getPosition()))){
                    result_peg_1028.nextPosition();
                } else {
                    goto out_peg_1030;
                }
            }
            result_peg_1028.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist x");
                result_peg_1028.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1028;
    stream.update(result_peg_1028.getPosition());
    
    
    return result_peg_1028;
    out_peg_1030:
    Result result_peg_1033(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("p1bodydist"[i], stream.get(result_peg_1033.getPosition()))){
                    result_peg_1033.nextPosition();
                } else {
                    goto out_peg_1035;
                }
            }
            result_peg_1033.setValue((void*) "p1bodydist");
        
        
        
        result_peg_1033 = rule_s(stream, result_peg_1033.getPosition());
            if (result_peg_1033.error()){
                goto out_peg_1035;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1033.getPosition()))){
                    result_peg_1033.nextPosition();
                } else {
                    goto out_peg_1035;
                }
            }
            result_peg_1033.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("p1bodydist y");
                result_peg_1033.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1033;
    stream.update(result_peg_1033.getPosition());
    
    
    return result_peg_1033;
    out_peg_1035:
    Result result_peg_1038(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1038.getPosition()))){
                    result_peg_1038.nextPosition();
                } else {
                    goto out_peg_1040;
                }
            }
            result_peg_1038.setValue((void*) "parentdist");
        
        
        
        result_peg_1038 = rule_s(stream, result_peg_1038.getPosition());
            if (result_peg_1038.error()){
                goto out_peg_1040;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1038.getPosition()))){
                    result_peg_1038.nextPosition();
                } else {
                    goto out_peg_1040;
                }
            }
            result_peg_1038.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1038.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1038;
    stream.update(result_peg_1038.getPosition());
    
    
    return result_peg_1038;
    out_peg_1040:
    Result result_peg_1043(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1043.getPosition()))){
                    result_peg_1043.nextPosition();
                } else {
                    goto out_peg_1045;
                }
            }
            result_peg_1043.setValue((void*) "screenpos");
        
        
        
        result_peg_1043 = rule_s(stream, result_peg_1043.getPosition());
            if (result_peg_1043.error()){
                goto out_peg_1045;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1043.getPosition()))){
                    result_peg_1043.nextPosition();
                } else {
                    goto out_peg_1045;
                }
            }
            result_peg_1043.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos x");
                result_peg_1043.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1043;
    stream.update(result_peg_1043.getPosition());
    
    
    return result_peg_1043;
    out_peg_1045:
    Result result_peg_1048(myposition);
    
    {
    
        for (int i = 0; i < 9; i++){
                if (compareCharCase("screenpos"[i], stream.get(result_peg_1048.getPosition()))){
                    result_peg_1048.nextPosition();
                } else {
                    goto out_peg_1050;
                }
            }
            result_peg_1048.setValue((void*) "screenpos");
        
        
        
        result_peg_1048 = rule_s(stream, result_peg_1048.getPosition());
            if (result_peg_1048.error()){
                goto out_peg_1050;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1048.getPosition()))){
                    result_peg_1048.nextPosition();
                } else {
                    goto out_peg_1050;
                }
            }
            result_peg_1048.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("screenpos y");
                result_peg_1048.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1048;
    stream.update(result_peg_1048.getPosition());
    
    
    return result_peg_1048;
    out_peg_1050:
    Result result_peg_1053(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1053.getPosition()))){
                    result_peg_1053.nextPosition();
                } else {
                    goto out_peg_1055;
                }
            }
            result_peg_1053.setValue((void*) "parentdist");
        
        
        
        result_peg_1053 = rule_s(stream, result_peg_1053.getPosition());
            if (result_peg_1053.error()){
                goto out_peg_1055;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("x"[i], stream.get(result_peg_1053.getPosition()))){
                    result_peg_1053.nextPosition();
                } else {
                    goto out_peg_1055;
                }
            }
            result_peg_1053.setValue((void*) "x");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist x");
                result_peg_1053.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1053;
    stream.update(result_peg_1053.getPosition());
    
    
    return result_peg_1053;
    out_peg_1055:
    Result result_peg_1058(myposition);
    
    {
    
        for (int i = 0; i < 10; i++){
                if (compareCharCase("parentdist"[i], stream.get(result_peg_1058.getPosition()))){
                    result_peg_1058.nextPosition();
                } else {
                    goto out_peg_1060;
                }
            }
            result_peg_1058.setValue((void*) "parentdist");
        
        
        
        result_peg_1058 = rule_s(stream, result_peg_1058.getPosition());
            if (result_peg_1058.error()){
                goto out_peg_1060;
            }
        
        
        
        for (int i = 0; i < 1; i++){
                if (compareCharCase("y"[i], stream.get(result_peg_1058.getPosition()))){
                    result_peg_1058.nextPosition();
                } else {
                    goto out_peg_1060;
                }
            }
            result_peg_1058.setValue((void*) "y");
        
        
        
        {
                Value value((void*) 0);
                value = makeKeyword("parentdist y");
                result_peg_1058.setValue(value);
            }
        
        
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1058;
    stream.update(result_peg_1058.getPosition());
    
    
    return result_peg_1058;
    out_peg_1060:
    Result result_peg_1063(myposition);
    
    result_peg_1063 = rule_hitflag(stream, result_peg_1063.getPosition());
    if (result_peg_1063.error()){
        goto out_peg_1064;
    }
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = result_peg_1063;
    stream.update(result_peg_1063.getPosition());
    
    
    return result_peg_1063;
    out_peg_1064:
    
    if (column_peg_977.chunk13 == 0){
        column_peg_977.chunk13 = new Chunk13();
    }
    column_peg_977.chunk13->chunk_keyword_real = errorResult;
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

        
